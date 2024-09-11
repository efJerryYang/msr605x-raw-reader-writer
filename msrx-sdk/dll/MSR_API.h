#ifndef MSR_API_H
#define MSR_API_H

#include <windows.h>

// Õ®”√API
extern "C" __declspec(dllexport) void _stdcall MSR_GetDLL_Ver(char *rVER);
extern "C" __declspec(dllexport) BOOL _stdcall MSR_InitComm(char *portname, unsigned long baud);
extern "C" __declspec(dllexport) void _stdcall MSR_ExitComm();

extern "C" __declspec(dllexport) int _stdcall MSR_CtrlBUZZ(unsigned char *sound, unsigned char len);

extern "C" __declspec(dllexport) int _stdcall MSR_Reset();
extern "C" __declspec(dllexport) int _stdcall MSR_Read();
extern "C" __declspec(dllexport) int _stdcall MSR_Write(unsigned char *TK1Dat, unsigned char *TK2Dat, unsigned char *TK3Dat);
extern "C" __declspec(dllexport) int _stdcall MSR_CommTest();
extern "C" __declspec(dllexport) int _stdcall MSR_All_LED_Off();
extern "C" __declspec(dllexport) int _stdcall MSR_All_LED_On();
extern "C" __declspec(dllexport) int _stdcall MSR_Green_LED_On();
extern "C" __declspec(dllexport) int _stdcall MSR_Yellow_LED_On();
extern "C" __declspec(dllexport) int _stdcall MSR_Red_LED_On();
extern "C" __declspec(dllexport) int _stdcall MSR_Sensor_Test();
extern "C" __declspec(dllexport) int _stdcall MSR_Ram_Test();
extern "C" __declspec(dllexport) int _stdcall MSR_Set_Leading_Zero(int lead210, int lead75);
extern "C" __declspec(dllexport) int _stdcall MSR_Check_Leading_Zero(int *lead210, int *lead75);
extern "C" __declspec(dllexport) int _stdcall MSR_Erase(int SelectTrack);
extern "C" __declspec(dllexport) int _stdcall MSR_Select_BPI(unsigned char TK2Density);
extern "C" __declspec(dllexport) int _stdcall MSR_Raw_Read();
extern "C" __declspec(dllexport) int _stdcall MSR_Raw_Write(unsigned char *TK1Dat, unsigned char *TK2Dat, unsigned char *TK3Dat);
extern "C" __declspec(dllexport) int _stdcall MSR_Get_Device_Model(unsigned char *model);
extern "C" __declspec(dllexport) int _stdcall MSR_Get_Firmware_Version(unsigned char *version);
extern "C" __declspec(dllexport) int _stdcall MSR_Set_BPC(int TK1Bit, int TK2Bit, int TK3Bit);
extern "C" __declspec(dllexport) int _stdcall MSR_Set_HiCo();
extern "C" __declspec(dllexport) int _stdcall MSR_Set_LoCo();
extern "C" __declspec(dllexport) int _stdcall MSR_Get_CoStatus(unsigned char *status);
extern "C" __declspec(dllexport) int _stdcall MSR_Get_Write_Erase_Status();
extern "C" __declspec(dllexport) int _stdcall MSR_Get_Read_Data(unsigned char *TK1Dat, unsigned char *TK2Dat, unsigned char *TK3Dat);
extern "C" __declspec(dllexport) int _stdcall MSR_Get_RawRead_Data(unsigned char *TK1Dat, unsigned char *TK2Dat, unsigned char *TK3Dat);

extern "C" __declspec(dllexport) int _stdcall MSR_DecodeTrack(char *AscBuff, unsigned char *BinBuff, unsigned char bpc, unsigned char parity, unsigned char ss, unsigned char es);
extern "C" __declspec(dllexport) unsigned char _stdcall get_trackbyte(unsigned char bpc, unsigned char parity);
extern "C" __declspec(dllexport) int _stdcall MSR_EncodeTrack(char *AscBuff, unsigned char *BinBuff, unsigned char bpc, unsigned char parity, unsigned char ss, unsigned char es);

#endif // MSR_API_H
