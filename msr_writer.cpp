#include "msr_common.h"

volatile sig_atomic_t keep_running = 1;

// CardData read_latest_card_data() {
//     std::ifstream infile("card-content.txt");
//     if (!infile.is_open()) {
//         throw std::runtime_error("Unable to open file for reading");
//     }

//     std::vector<CardData> records;
//     CardData current_record;
//     std::string line;

//     while (std::getline(infile, line)) {
//         if (line.empty()) {
//             if (!current_record.timestamp.empty()) {
//                 records.push_back(current_record);
//                 current_record = CardData();
//             }
//         } else if (current_record.timestamp.empty()) {
//             current_record.timestamp = line;
//         } else if (line.find("track1: ") == 0) {
//             current_record.setFromHexString(current_record.track1, line.substr(8));
//         } else if (line.find("track2: ") == 0) {
//             current_record.setFromHexString(current_record.track2, line.substr(8));
//         } else if (line.find("track3: ") == 0) {
//             current_record.setFromHexString(current_record.track3, line.substr(8));
//         }
//     }

//     if (!current_record.timestamp.empty()) {
//         records.push_back(current_record);
//     }

//     infile.close();

//     if (records.empty()) {
//         throw std::runtime_error("No valid card data found in file");
//     }

//     return records.back();
// }

// void write_card_loop() {
//     while (keep_running) {
//         try {
//             write_card_once();
//             while (keep_running && MSR_Get_Write_Erase_Status() == 0) {
//                 Sleep(100);
//             }
//             int write_status = MSR_Get_Write_Erase_Status();
//             if (write_status == '0') {
//                 std::cout << "Card write successful" << std::endl;
//             } else {
//                 std::cerr << "Card write failed with status: " << write_status << std::endl;
//             }
//         } catch (const std::exception& e) {
//             std::cerr << "Error: " << e.what() << std::endl;
//         }
//         Sleep(1000);
//     }
// }

// void write_card_data(CardData& latest_data, std::vector<uint64_t>& masks) {
//     try {
//         // Define endian masks for each track
//         uint64_t track1_mask = masks[0];
//         uint64_t track2_mask = masks[1];
//         uint64_t track3_mask = masks[2];

//         // Apply reverse endian
//         CardData::reverseEndian(latest_data.track1, track1_mask);
//         CardData::reverseEndian(latest_data.track2, track2_mask);
//         CardData::reverseEndian(latest_data.track3, track3_mask);

//         std::cout << "Try to write card data:" << std::endl;
//         std::cout << "track1[" << latest_data.track1.size() << "]: " << latest_data.getHexString(latest_data.track1) << std::endl;
//         std::cout << "track2[" << latest_data.track2.size() << "]: " << latest_data.getHexString(latest_data.track2) << std::endl;
//         std::cout << "track3[" << latest_data.track3.size() << "]: " << latest_data.getHexString(latest_data.track3) << std::endl;

//         int status = MSR_Raw_Write(latest_data.track1.data(), 
//             latest_data.track2.data(), 
//             latest_data.track3.data());
//         if (status != '0') {
//             throw std::runtime_error("Failed to write data to card" + std::to_string(status));
//         }
//         while (keep_running && MSR_Get_Write_Erase_Status() == 0) {
//             Sleep(100);
//         }
//         int write_status = MSR_Get_Write_Erase_Status();
//         if (write_status == '0') {
//             std::cout << "Card write successful" << std::endl;
//         } else {
//             std::cerr << "Card write failed with status: " << write_status << std::endl;
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
// }

// void init_writer_device() {
//     init_device();
//     if (MSR_Set_HiCo() != '0') {
//         throw std::runtime_error("Failed to set HiCo mode");
//     }
// }

// void write_card_once() {
//     CardData latest_data = read_latest_card_data();
//     std::vector<uint64_t> masks = read_masks_from_file("masks.txt");

//     std::cout << "masks: " << masks.size() << std::endl;
//     for (auto mask : masks) {
//         std::cout << "mask: " << std::hex << mask << std::endl;
//     }

//     if (masks.size() != 3) {
//         throw std::runtime_error("Invalid number of masks in file");
//     } 
//     write_card_data(latest_data, masks);
// }

std::vector<uint64_t> read_masks_from_file(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<uint64_t> masks;
    std::string line;
    int line_number = 0;
    while (std::getline(file, line) && masks.size() < 3) {
        line_number++;
        try {
            uint64_t mask = 0;
            if (line.substr(0, 2) == "0b") {
                // binary
                for (size_t i = line.length() - 1; i >= 2 && i < line.length(); --i) {
                    mask = (mask << 1) | (line[i] == '1' ? 1 : 0);
                    if (i == 2) break;
                }
            } else if (line.substr(0, 2) == "0x") {
                // hexadecimal
                for (size_t i = line.length() - 1; i >= 2 && i < line.length(); --i) {
                    mask = (mask << 4) | (std::isdigit(line[i]) ? line[i] - '0' : 
                           (std::tolower(line[i]) - 'a' + 10));
                    if (i == 2) break;
                }
            } else {
                // decimal
                mask = std::stoull(line);
            }
            masks.push_back(mask);
            std::cout << "Successfully read mask: 0x" << std::hex << mask << std::dec << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line " << line_number << ": " << line << std::endl;
            std::cerr << "Error details: " << e.what() << std::endl;
        }
    }
    if (masks.size() != 3) {
        std::cerr << "Warning: Expected 3 masks, but read " << masks.size() << std::endl;
    }
    return masks;
}

// void writer_main() {
//     try {
//         init_writer_device();
//         write_card_loop();
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
// }

// #ifdef WRITER_MAIN
// int main() {
//     setup_signal_handlers();
//     writer_main();
//     reset_and_exit_device();
//     return 0;
// }
// #endif


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
    
    if (MSR_Set_HiCo() != '0') {
        throw std::runtime_error("Failed to set HiCo mode");
    }
    
    std::cout << "Device initialized successfully in HiCo mode" << std::endl;
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

std::string pad_hex_string(const std::string& hex, size_t min_length) {
    if (hex.length() >= min_length) {
        return hex;
    }
    return hex + std::string(min_length - hex.length(), '0');
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
            current_record.setFromHexString(current_record.track1, line.substr(8));
        } else if (line.find("track2: ") == 0) {
            current_record.setFromHexString(current_record.track2, line.substr(8));
        } else if (line.find("track3: ") == 0) {
            current_record.setFromHexString(current_record.track3, line.substr(8));
        }
    }

    if (!current_record.timestamp.empty()) {
        records.push_back(current_record);
    }

    infile.close();

    if (records.empty()) {
        throw std::runtime_error("No valid card data found in file");
    }

    return records.back();
}

void write_card_data() {
    while (keep_running) {
        try {
            CardData latest_data = read_latest_card_data();

            // Define endian masks for each track
            // std::vector<uint64_t> masks = read_masks_from_file("masks.txt");
            // uint64_t track1_mask = masks[0];
            // uint64_t track2_mask = masks[1];
            // uint64_t track3_mask = masks[2];
            uint64_t track1_mask = 0x0000000000000000;
            uint64_t track2_mask = 0x0000000000000000;
            uint64_t track3_mask = 0x0000000000000000;

            // Apply reverse endian
            CardData::reverseByteBits(latest_data.track1, track1_mask);
            CardData::reverseByteBits(latest_data.track2, track2_mask);
            CardData::reverseByteBits(latest_data.track3, track3_mask);

            std::cout << "Try to write card data:" << std::endl;
            std::cout << "track1[" << latest_data.track1.size() << "]: " << latest_data.getHexString(latest_data.track1) << std::endl;
            std::cout << "track2[" << latest_data.track2.size() << "]: " << latest_data.getHexString(latest_data.track2) << std::endl;
            std::cout << "track3[" << latest_data.track3.size() << "]: " << latest_data.getHexString(latest_data.track3) << std::endl;

            int status = MSR_Raw_Write(latest_data.track1.data(), 
                latest_data.track2.data(), 
                latest_data.track3.data());
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

#ifdef WRITER_MAIN
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
#endif