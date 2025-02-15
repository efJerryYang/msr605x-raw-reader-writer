#include "msr_common.h"
#include <fstream>
#include <cctype>

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

std::vector<uint64_t> read_masks_from_file(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<uint64_t> masks;
    std::string line;
    while (std::getline(file, line) && masks.size() < 3) {
        try {
            uint64_t mask = std::stoull(line, nullptr, 0);
            masks.push_back(mask);
            std::cout << "Successfully read mask: 0x" << std::hex << mask << std::dec << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing mask: " << line << " (" << e.what() << ")" << std::endl;
        }
    }
    return masks;
}


void write_card_data() {
    while (keep_running) {
        try {
            CardData latest_data = read_latest_card_data();

            std::vector<uint64_t> masks = read_masks_from_file("masks.txt");
            if (masks.size() != 3) {
                throw std::runtime_error("Invalid number of masks in file");
            }
            CardData::reverseByteBits(latest_data.track1, masks[0]);
            CardData::reverseByteBits(latest_data.track2, masks[1]);
            CardData::reverseByteBits(latest_data.track3, masks[2]);

            std::fill(latest_data.track1.begin(), latest_data.track1.end(), 0x00);
            std::cout << "Try to write card data:" << std::endl;
            std::cout << "track1[" << latest_data.track1.size() << "]: " << latest_data.getHexString(latest_data.track1) << std::endl;
            std::cout << "track2[" << latest_data.track2.size() << "]: " << latest_data.getHexString(latest_data.track2) << std::endl;
            std::cout << "track3[" << latest_data.track3.size() << "]: " << latest_data.getHexString(latest_data.track3) << std::endl;

            // set track 1 to zero
            int status = MSR_Raw_Write(latest_data.track1.data(), 
                                       latest_data.track2.data(), 
                                       latest_data.track3.data());
            if (status != '0') {
                throw std::runtime_error("Failed to write data to card: " + std::to_string(status));
            }

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
        } catch (const std::exception &e) {
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
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    reset_and_exit_device();
    return 0;
}
#endif
