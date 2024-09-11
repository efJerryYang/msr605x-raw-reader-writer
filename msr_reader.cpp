#include "msr_common.h"

volatile sig_atomic_t keep_running = 1;
bool terminal_only = false;

void reset_and_exit_device() {
    static bool already_called = false;
    if (already_called) return;
    already_called = true;

    std::cout << "Resetting and exiting device..." << std::endl;
    if (MSR_Reset() != '0') {
        std::cerr << "Warning: Failed to reset device" << std::endl;
    }
    Sleep(100);
    MSR_ExitComm();
    std::cout << "Device exited" << std::endl;
}

void signal_handler(int signal) {
    keep_running = 0;
    std::cout << "\nReceived interrupt signal. Exiting program..." << std::endl;
}

void setup_signal_handlers() {
    signal(SIGINT, signal_handler);
}

std::string get_iso_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

void init_device() {
    char portname[] = "HID";
    if (!MSR_InitComm(portname, 9600)) {
        throw std::runtime_error("Device initialization failed");
    }
    std::cout << "Device initialized successfully" << std::endl;
}

std::string pad_hex_string(const std::string& hex, size_t min_length) {
    if (hex.length() >= min_length) {
        return hex;
    }
    return hex + std::string(min_length - hex.length(), '0');
}

std::string bytes_to_hex_string(const unsigned char* data, size_t length, size_t& hex_length) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    hex_length = 0;
    for (size_t i = 0; i < length; i++) {
        ss << std::setw(2) << static_cast<int>(data[i]);
        hex_length += 2;
    }
    std::string result = ss.str();
    result = pad_hex_string(result, TRACK_DATA_HEX_NUM_LIMIT);
    hex_length = result.length();
    return result;
}

void write_card_data_to_file(const CardData& card_data) {
    std::ofstream outfile("card-content.txt", std::ios::app);
    if (outfile.is_open()) {
        outfile << card_data.timestamp << std::endl;
        outfile << "track1: " << card_data.getHexString(card_data.track1) << std::endl;
        outfile << "track2: " << card_data.getHexString(card_data.track2) << std::endl;
        outfile << "track3: " << card_data.getHexString(card_data.track3) << std::endl;
        outfile << std::endl;
        outfile.close();
    } else {
        std::cerr << "Unable to open file for writing" << std::endl;
    }
}

void clear_track_data(unsigned char* TK1Dat, unsigned char* TK2Dat, unsigned char* TK3Dat) {
    memset(TK1Dat, 0, TRACK_DATA_SIZE);
    memset(TK2Dat, 0, TRACK_DATA_SIZE);
    memset(TK3Dat, 0, TRACK_DATA_SIZE);
}

void read_card_loop() {
    if (MSR_Raw_Read() != '0') {
        throw std::runtime_error("Failed to start raw read mode");
    }
    unsigned char TK1Dat[TRACK_DATA_SIZE] = {0}, TK2Dat[TRACK_DATA_SIZE] = {0}, TK3Dat[TRACK_DATA_SIZE] = {0};
    while (keep_running) {
        // std::cout << "Reading card data..." << std::endl;
        try {
            read_card_data(TK1Dat, TK2Dat, TK3Dat);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        Sleep(100);
    }
}

void read_card_data(unsigned char TK1Dat[TRACK_DATA_SIZE], unsigned char TK2Dat[TRACK_DATA_SIZE], unsigned char TK3Dat[TRACK_DATA_SIZE]) {
    clear_track_data(TK1Dat, TK2Dat, TK3Dat);
    int status = MSR_Get_RawRead_Data(TK1Dat, TK2Dat, TK3Dat);
    CardData card_data;
    if (status == '0') {
        std::cout << "Card read successfully" << std::endl;

        card_data.timestamp = get_iso_timestamp();
        card_data.track1 = std::vector<unsigned char>(TK1Dat, TK1Dat + TRACK_DATA_SIZE);
        card_data.track2 = std::vector<unsigned char>(TK2Dat, TK2Dat + TRACK_DATA_SIZE);
        card_data.track3 = std::vector<unsigned char>(TK3Dat, TK3Dat + TRACK_DATA_SIZE);

        std::cout << "Timestamp: " << card_data.timestamp << std::endl;
        std::cout << "track1[" << card_data.track1.size() << "]: " << card_data.getHexString(card_data.track1) << std::endl;
        std::cout << "track2[" << card_data.track2.size() << "]: " << card_data.getHexString(card_data.track2) << std::endl;
        std::cout << "track3[" << card_data.track3.size() << "]: " << card_data.getHexString(card_data.track3) << std::endl;
        std::cout << std::endl;

        if (!terminal_only) {
            write_card_data_to_file(card_data);
        }

        if (MSR_Raw_Read() != '0') {
            throw std::runtime_error("Failed to start raw read mode");
        }
    } else if (status != 0) {
        std::cerr << "Error reading card" << std::endl;
    } else {
        // std::cerr << "No card detected" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    setup_signal_handlers();

    // cli args
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) {
            terminal_only = true;
            std::cout << "Terminal-only mode activated. Data will not be written to file." << std::endl;
            break;
        }
    }

    try {
        init_device();
        read_card_loop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    reset_and_exit_device();
    return 0;
}