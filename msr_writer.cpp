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

void init_device() {
    char portname[] = "HID";
    if (!MSR_InitComm(portname, 9600)) {
        throw std::runtime_error("Device initialization failed");
    }
    std::cout << "Device initialized successfully" << std::endl;
}

std::string hex_string_to_bytes(const std::string& hex) {
    std::string bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = (char) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

CardData read_latest_card_data() {
    std::ifstream infile("card-content.txt");
    if (!infile.is_open()) {
        throw std::runtime_error("Unable to open file for reading");
    }

    std::vector<CardData> records;
    CardData current_record;
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty()) {
            if (!current_record.timestamp.empty()) {
                records.push_back(current_record);
                current_record = CardData();
            }
        } else if (current_record.timestamp.empty()) {
            current_record.timestamp = line;
        } else if (line.find("track1: ") == 0) {
            current_record.track1 = line.substr(8);
        } else if (line.find("track2: ") == 0) {
            current_record.track2 = line.substr(8);
        } else if (line.find("track3: ") == 0) {
            current_record.track3 = line.substr(8);
        }
    }

    if (!current_record.timestamp.empty()) {
        records.push_back(current_record);
    }

    infile.close();

    if (records.empty()) {
        throw std::runtime_error("No valid card data found in file");
    }

    return records.back();  // 返回最后一条记录
}

void write_card_data() {
    while (keep_running) {
        try {
            CardData latest_data = read_latest_card_data();

            std::string track1_data = hex_string_to_bytes(latest_data.track1);
            std::string track2_data = hex_string_to_bytes(latest_data.track2);
            std::string track3_data = hex_string_to_bytes(latest_data.track3);

            std::cout << "Attempting to write card data:" << std::endl;
            std::cout << "track1[" << track1_data.length() << "]: " << latest_data.track1 << std::endl;
            std::cout << "track2[" << track2_data.length() << "]: " << latest_data.track2 << std::endl;
            std::cout << "track3[" << track3_data.length() << "]: " << latest_data.track3 << std::endl;

            int status = MSR_Raw_Write((unsigned char*)track1_data.c_str(), 
                (unsigned char*)track2_data.c_str(), 
                (unsigned char*)track3_data.c_str());
            if (status != '0') {
                throw std::runtime_error("Failed to write data to card" + std::to_string(status));
            }

            // Wait for card swipe
            while (keep_running && MSR_Get_Write_Erase_Status() == 0) {
                Sleep(100);
            }

            int write_status = MSR_Get_Write_Erase_Status();
            if (write_status == '0') {
                std::cout << "Card write successful" << std::endl;
            } else {
                std::cerr << "Card write failed with status: " << write_status << std::endl;
            }

            Sleep(1000);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            Sleep(1000);
        }
    }
}

int main() {
    setup_signal_handlers();

    try {
        init_device();
        write_card_data();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    reset_and_exit_device();
    return 0;
}