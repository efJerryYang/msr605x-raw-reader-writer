#include "msr_common.h"

volatile sig_atomic_t keep_running = 1;

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

std::string bytes_to_hex_string(const unsigned char* data, size_t length, size_t& hex_length) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    hex_length = 0;
    for (size_t i = 0; i < length; ++i) {
        if (data[i] == 0 && data[i+1] == 0) {
            break;
        }
        ss << std::setw(2) << static_cast<int>(data[i]);
        hex_length += 2;
    }
    return ss.str();
}

void write_card_data_to_file(const CardData& card_data) {
    std::ofstream outfile("card-content.txt", std::ios::app);
    if (outfile.is_open()) {
        outfile << card_data.timestamp << std::endl;
        outfile << "track1: " << card_data.track1 << std::endl;
        outfile << "track2: " << card_data.track2 << std::endl;
        outfile << "track3: " << card_data.track3 << std::endl;
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

void read_card_data() {
    unsigned char TK1Dat[TRACK_DATA_SIZE] = {0}, TK2Dat[TRACK_DATA_SIZE] = {0}, TK3Dat[TRACK_DATA_SIZE] = {0};

    if (MSR_Raw_Read() != '0') {
        throw std::runtime_error("Failed to start raw read mode");
    }

    while (keep_running) {
        clear_track_data(TK1Dat, TK2Dat, TK3Dat);

        int status = MSR_Get_RawRead_Data(TK1Dat, TK2Dat, TK3Dat);
        if (status == '0') {
            std::cout << "Card read successfully" << std::endl;

            CardData card_data;
            card_data.timestamp = get_iso_timestamp();
            
            size_t hex_length1, hex_length2, hex_length3;
            card_data.track1 = bytes_to_hex_string(TK1Dat, TRACK_DATA_SIZE, hex_length1);
            card_data.track2 = bytes_to_hex_string(TK2Dat, TRACK_DATA_SIZE, hex_length2);
            card_data.track3 = bytes_to_hex_string(TK3Dat, TRACK_DATA_SIZE, hex_length3);

            std::cout << "Timestamp: " << card_data.timestamp << std::endl;
            std::cout << "track1[" << hex_length1 << "]: " << card_data.track1 << std::endl;
            std::cout << "track2[" << hex_length2 << "]: " << card_data.track2 << std::endl;
            std::cout << "track3[" << hex_length3 << "]: " << card_data.track3 << std::endl;
            std::cout << std::endl;

            write_card_data_to_file(card_data);

            // Reset for next read
            if (MSR_Raw_Read() != '0') {
                throw std::runtime_error("Failed to restart raw read mode");
            }
        } else if (status != 0) {
            std::cerr << "Error reading card" << std::endl;
        }
        Sleep(100); // Wait for 100ms before checking again
    }
}

int main() {
    setup_signal_handlers();

    try {
        init_device();
        read_card_data();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    reset_and_exit_device();
    return 0;
}