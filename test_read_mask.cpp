#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cctype>
#include <iomanip>
#include <stdexcept>

std::vector<uint64_t> read_masks_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    std::vector<uint64_t> masks;
    std::string line;
    int line_number = 0;
    while (std::getline(file, line) && masks.size() < 3) {
        line_number++;
        try {
            uint64_t mask = 0;
            // Remove possible leading/trailing whitespace
            std::istringstream iss(line);
            iss >> line;
            if (line.substr(0, 2) == "0b") {
                // binary
                for (size_t i = line.length() - 1; i >= 2 && i < line.length(); --i) {
                    mask = (mask << 1) | (line[i] == '1' ? 1 : 0);
                    if (i == 2) break;
                }
            } else if (line.substr(0, 2) == "0x") {
                // hexadecimal
                for (size_t i = line.length() - 1; i >= 2 && i < line.length(); --i) {
                    char ch = line[i];
                    uint64_t digit = 0;
                    if (std::isdigit(ch)) {
                        digit = ch - '0';
                    } else if (std::isxdigit(ch)) {
                        digit = std::tolower(ch) - 'a' + 10;
                    }
                    mask = (mask << 4) | digit;
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

void print_mask_bytes(uint64_t mask) {
    // Print as 8 bytes in big-endian order.
    std::cout << "Complete mask: 0x" << std::hex << mask << std::dec << std::endl;
    for (int i = 7; i >= 0; --i) {
        uint8_t byte = (mask >> (i * 8)) & 0xFF;
        std::cout << "Byte " << (7 - i) << ": 0x" 
                  << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte)
                  << std::dec << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::string filename = "masks.txt";
    std::vector<uint64_t> masks;
    try {
        masks = read_masks_from_file(filename);
    } catch (const std::exception& e) {
        std::cerr << "Failed to read masks: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nPrinting each mask byte-by-byte:\n" << std::endl;
    for (size_t i = 0; i < masks.size(); ++i) {
        std::cout << "Mask " << i + 1 << ":" << std::endl;
        print_mask_bytes(masks[i]);
    }
    return 0;
}
