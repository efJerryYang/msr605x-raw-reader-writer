#ifndef MSR_COMMON_H
#define MSR_COMMON_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cstring>
#include <string>
#include <csignal>
#include <windows.h>
#include <vector>
#include "MSR_API.h"

#define TRACK_DATA_SIZE 256

extern volatile sig_atomic_t keep_running;

void setup_signal_handlers();
std::string get_iso_timestamp();
void init_device();
void reset_and_exit_device();
void print_track_data(const unsigned char* data, const char* track_name);
std::string bytes_to_hex_string(const unsigned char* data, size_t length);

class CardData {
public:
    std::string timestamp;
    std::vector<unsigned char> track1;
    std::vector<unsigned char> track2;
    std::vector<unsigned char> track3;

    std::string getHexString(const std::vector<unsigned char>& data) const {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');
        for (unsigned char byte : data) {
            ss << std::setw(2) << static_cast<int>(byte);
        }
        return ss.str();
    }

    void setFromHexString(std::vector<unsigned char>& track, const std::string& hex) {
        track.clear();
        for (size_t i = 0; i < hex.length(); i += 2) {
            unsigned char byte = static_cast<unsigned char>(std::stoi(hex.substr(i, 2), nullptr, 16));
            track.push_back(byte);
        }
    }
};

#endif // MSR_COMMON_H