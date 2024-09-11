#include "msr_common.h"

#ifdef READER_MAIN
bool terminal_only = false;
#endif

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
    while (keep_running) {
        // std::cout << "Reading card data..." << std::endl;
        try {
            read_card_once();
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

        #ifdef READER_MAIN
        if (!terminal_only) {
            write_card_data_to_file(card_data);
        }
        #endif
        if (MSR_Raw_Read() != '0') {
            throw std::runtime_error("Failed to start raw read mode");
        }
    } else if (status != 0) {
        std::cerr << "Error reading card" << std::endl;
    } else {
        // std::cerr << "No card detected" << std::endl;
    }
}

void init_reader_device() {
    init_device();
    if (MSR_Raw_Read() != '0') {
        throw std::runtime_error("Failed to start raw read mode");
    }
}

void read_card_once() {
    unsigned char TK1Dat[TRACK_DATA_SIZE] = {0}, TK2Dat[TRACK_DATA_SIZE] = {0}, TK3Dat[TRACK_DATA_SIZE] = {0};
    read_card_data(TK1Dat, TK2Dat, TK3Dat);
}

void reader_main() {
    try {
        init_reader_device();
        read_card_loop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

#ifdef READER_MAIN
int main(int argc, char* argv[]) {
    setup_signal_handlers();
    terminal_only = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) {
            terminal_only = true;
            std::cout << "Terminal-only mode activated. Data will not be written to file." << std::endl;
            break;
        }
    }

    reader_main();
    reset_and_exit_device();
    return 0;
}
#endif