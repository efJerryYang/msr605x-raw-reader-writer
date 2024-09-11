#include <iostream>
#include <windows.h>
#include <cstring>
#include <iomanip>
#include "MSR_API.h"

// We have to use dynamic loading of the DLL because the MSR_API.lib is not compatible with MinGW
typedef BOOL (__stdcall *MSR_InitComm_t)(char*, unsigned long);
typedef void (__stdcall *MSR_ExitComm_t)();
typedef int (__stdcall *MSR_Raw_Read_t)();
typedef int (__stdcall *MSR_Get_RawRead_Data_t)(unsigned char*, unsigned char*, unsigned char*);

void printTrackData(const unsigned char* data, const char* trackName) {
    std::cout << trackName << " raw data:" << std::endl;
    for (int i = 0; i < 512; i++) {
        if (data[i] == 0 && data[i+1] == 0) break; // Stop at two consecutive zeros
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Starting program..." << std::endl;

    HMODULE hModule = LoadLibraryW(L"MSR_API.dll");
    if (!hModule) {
        std::cerr << "Failed to load MSR_API.dll, error code: " << GetLastError() << std::endl;
        return 1;
    }
    std::cout << "Successfully loaded MSR_API.dll" << std::endl;

    MSR_InitComm_t MSR_InitComm = (MSR_InitComm_t)GetProcAddress(hModule, "MSR_InitComm");
    MSR_ExitComm_t MSR_ExitComm = (MSR_ExitComm_t)GetProcAddress(hModule, "MSR_ExitComm");
    MSR_Raw_Read_t MSR_Raw_Read = (MSR_Raw_Read_t)GetProcAddress(hModule, "MSR_Raw_Read");
    MSR_Get_RawRead_Data_t MSR_Get_RawRead_Data = (MSR_Get_RawRead_Data_t)GetProcAddress(hModule, "MSR_Get_RawRead_Data");

    if (!MSR_InitComm || !MSR_ExitComm || !MSR_Raw_Read || !MSR_Get_RawRead_Data) {
        std::cerr << "Failed to get function addresses" << std::endl;
        FreeLibrary(hModule);
        return 1;
    }

    // 1. Initialize device
    char portname[] = "HID";
    std::cout << "Initializing device..." << std::endl;
    if (!MSR_InitComm(portname, 9600)) {
        std::cerr << "Device initialization failed" << std::endl;
        FreeLibrary(hModule);
        return 1;
    }
    std::cout << "Device initialized successfully" << std::endl;

    // 2. Read strip card in raw mode
    std::cout << "Please swipe the card..." << std::endl;
    if (MSR_Raw_Read() != '0') {
        std::cerr << "Failed to start raw read mode" << std::endl;
        MSR_ExitComm();
        FreeLibrary(hModule);
        return 1;
    }

    unsigned char TK1Dat[512] = {0}, TK2Dat[512] = {0}, TK3Dat[512] = {0};

    while (true) {
        int status = MSR_Get_RawRead_Data(TK1Dat, TK2Dat, TK3Dat);
        if (status == '0') {
            std::cout << "Card read successfully" << std::endl;

            printTrackData(TK1Dat, "Track 1");
            printTrackData(TK2Dat, "Track 2");
            printTrackData(TK3Dat, "Track 3");

            break;
        } else if (status != 0) {
            std::cerr << "Error reading card" << std::endl;
            break;
        }
        Sleep(100); // Wait for 100ms before checking again
    }

    // 3. Exit device
    std::cout << "Exiting device..." << std::endl;
    MSR_ExitComm();
    FreeLibrary(hModule);

    return 0;
}