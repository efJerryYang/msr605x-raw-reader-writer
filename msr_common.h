#ifndef MSR_COMMON_H
#define MSR_COMMON_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <string>
#include <csignal>
#include <windows.h>
#include <vector>
#include "MSR_API.h"

#define TRACK_DATA_SIZE 512

extern volatile sig_atomic_t keep_running;

void setup_signal_handlers();
std::string get_iso_timestamp();
void init_device();
void reset_and_exit_device();
void print_track_data(const unsigned char* data, const char* track_name);
std::string bytes_to_hex_string(const unsigned char* data, size_t length);

struct CardData {
    std::string timestamp;
    std::string track1;
    std::string track2;
    std::string track3;
};

#endif // MSR_COMMON_H