# Msr609 Dynamic Library Function Usage Instructions

## 1. Device Operations

### 1.1 Close Device Function

```cpp
void _stdcall MSR_ExitComm();
```

### 1.2 Initialize Device Function

```cpp
BOOL _stdcall MSR_InitComm(char *portname, unsigned long baud);
```

- **Parameters:**
  - `portname`: Port name, set to `"HID"`.
  - `baud`: Communication baud rate, set to `9600`.
- **Returns:**
  - `BOOL` value, `0` for failure, `1` for success.

### 1.3 Other Related Functions

- **Read Firmware Version:**

  ```cpp
  int _stdcall MSR_Get_Firmware_Version(unsigned char *version);
  ```

- **Read Device Model:**

  ```cpp
  int _stdcall MSR_Get_Device_Model(unsigned char *model);
  ```

- **Set Write Density:**

  ```cpp
  int _stdcall MSR_Select_BPI(unsigned char TK2Density);
  ```

  - Default `TK2Density = 0x4b`

- **Set Leading Zero Length:**

  ```cpp
  int _stdcall MSR_Set_Leading_Zero(int lead210, int lead75);
  ```

  - Defaults: `lead210 = 61`, `lead75 = 22`

- **Set Bit Length for Encoding:**

  ```cpp
  int _stdcall MSR_Set_BPC(int TK1Bit, int TK2Bit, int TK3Bit);
  ```

  - Defaults: `TK1Bit = 8`, `TK2Bit = 8`, `TK3Bit = 8`

- **Set to High Coercivity:**

  ```cpp
  int _stdcall MSR_Set_HiCo();
  ```

- **Set to Low Coercivity:**

  ```cpp
  int _stdcall MSR_Set_LoCo();
  ```

### 1.4 Example

```cpp
unsigned int tick;
char stu;
String str;
char temp[32];

CHAR szPortName[25];
LONG Status;
DWORD Type;

memcpy(szPortName, "HID", strlen("HID")+1);
str = ""; str += szPortName;
EditPort->Text = str;

MSR_ExitComm();

if (MSR_InitComm(szPortName, 9600))
{
    MSR_Reset();
    tick = GetTickCount();
    while ((unsigned int)GetTickCount() - tick < 5) {};  // delay 5ms

    memset(temp, 0, sizeof(temp));
    stu = MSR_Get_Firmware_Version(temp);
    if (stu == '0')
    {
        str = ""; str += temp;
        EditVersion->Text = str;

        memset(temp, 0, sizeof(temp));
        stu = MSR_Get_Device_Model(temp);
        if (stu == '0')
        {
            str = ""; str += temp;
            EditModel->Text = str;

            MSR_Select_BPI(0x4b);
            MSR_Set_Leading_Zero(61, 22);
            MSR_Set_BPC(8, 8, 8);
            if (CheckBoxHiCo->Checked == true)
                MSR_Set_HiCo();
            else
                MSR_Set_LoCo();
        }
    }
}

if (stu != '0')
{
    // Failure
}
else
{
    // Success
}
```

## 2. Card Reading Operations

### 2.1 Read Card Command Function

```cpp
int _stdcall MSR_Raw_Read();
```

- **Returns:**
  - `int` value, `0x30` for success, other values for failure.
- **Description:** This function puts the reader into card reading mode.

### 2.2 Get Read Card Data Function

```cpp
int _stdcall MSR_Get_Read_Data(unsigned char *TK1Dat, unsigned char *TK2Dat, unsigned char *TK3Dat);
```

- **Parameters:**
  - `TK1Dat`, `TK2Dat`, `TK3Dat` are buffers for track 1, 2, and 3 data respectively.
- **Returns:**
  - `int` value, `0` for no data (card not swiped), `0x30` for success, other values for failure.
- **Description:** After sending the read command, use a timer to periodically check the card swipe result.

### 2.3 Decode Function

```cpp
int _stdcall MSR_DecodeTrack(char *AscBuff, unsigned char *BinBuff, unsigned char bpc, unsigned char parity, unsigned char ss, unsigned char es);
```

- **Parameters:**
  - `AscBuff`: Buffer for decoded character data.
  - `BinBuff`: Buffer for magnetic stripe data from the previous function.
  - `bpc`, `parity`, `ss`, `es`: Decoding parameters. Standard ISO magnetic card decoding parameters are:
    - Track 1: `bpc = 7`, `parity = 1`, `ss = 0x05`, `es = 0x1F`
    - Track 2: `bpc = 5`, `parity = 1`, `ss = 0x0B`, `es = 0x0F`
    - Track 3: `bpc = 5`, `parity = 1`, `ss = 0x0B`, `es = 0x0F`

### 2.4 Example

```cpp
// Send read command
if (MSR_Raw_Read() == '0')
{
    // Read command sent successfully, reader in read state
}
else
{
    // Failure
}

// Periodically check card read result
stu = MSR_Get_Read_Data(TK1Dat, TK2Dat, TK3Dat);
if (stu == 0) return;    // No card swiped
if (stu == 0) // Data read
{
    // Decoding program
    Tk1Stu = MSR_DecodeTrack(TK1AscBuff, TK1Dat, BPC1, Parity1, SS1, ES1);
    Tk2Stu = MSR_DecodeTrack(TK2AscBuff, TK2Dat, BPC2, Parity2, SS2, ES2);
    Tk3Stu = MSR_DecodeTrack(TK3AscBuff, TK3Dat, BPC3, Parity3, SS3, ES3);
    str = "";
    if (Tk1Stu > 2)
    {
        memcpy(TK1AscBuff, TK1AscBuff + 1, Tk1Stu - 2);
        TK1AscBuff[Tk1Stu - 2] = 0;
        Tk1Stu -= 2;
        str += TK1AscBuff;
    }
    EditTK1->Text = str;
    str = "";
    if (Tk2Stu > 2)
    {
        memcpy(TK2AscBuff, TK2AscBuff + 1, Tk2Stu - 2);
        TK2AscBuff[Tk2Stu - 2] = 0;
        Tk2Stu -= 2;
        str += TK2AscBuff;
    }
    EditTK2->Text = str;
    str = "";
    if (Tk3Stu > 2)
    {
        memcpy(TK3AscBuff, TK3AscBuff + 1, Tk3Stu - 2);
        TK3AscBuff[Tk3Stu - 2] = 0;
        Tk3Stu -= 2;
        str += TK3AscBuff;
    }
    EditTK3->Text = str;
    StatusBar1->Panels->Items[0]->Text = "Read Card OK!";
}
```

## 3. Card Writing Operations

### 3.1 Encoding Command Function

```cpp
int _stdcall MSR_EncodeTrack(char *AscBuff, unsigned char *BinBuff, unsigned char bpc, unsigned char parity, unsigned char ss, unsigned char es);
```

- **Parameters:**
  - `AscBuff`: Buffer for data to be encoded.
  - `BinBuff`: Buffer for encoded magnetic stripe data.
  - `bpc`, `parity`, `ss`, `es`: Encoding parameters. Standard ISO magnetic card encoding parameters are:
    - Track 1: `bpc = 7`, `parity = 1`, `ss = 0x05`, `es = 0x1F`
    - Track 2: `bpc = 5`, `parity = 1`, `ss = 0x0B`, `es = 0x0F`
    - Track 3: `bpc = 5`, `parity = 1`, `ss = 0x0B`, `es = 0x0F`
- **Description:** Encodes track data into magnetic stripe data.

### 3.2 Write Card Command Function

```cpp
int _stdcall MSR_Raw_Write(unsigned char *TK1Dat, unsigned char *TK2Dat, unsigned char *TK3Dat);
```

- **Parameters:**
  - `TK1Dat`, `TK2Dat`, `TK3Dat` are buffers for track 1, 2, and 3 magnetic stripe data.
- **Returns:**
  - `int` value, `0x30` for success, other values for failure.
- **Description:** Sends the write command and puts the reader into write card mode.

### 3.3 Query Write Status Function

```cpp
int _stdcall MSR_Get_Write_Erase_Status();
```

- **Returns:**
  - `int` value, `0` for no card swiped, `0x30` for success, other values for failure.

### 3.4 Example

```cpp
// First, encode the data
MSR_EncodeTrack(EditTK1->Text.c_str(), TK1Dat, BPC1, Parity1, SS1, ES1);
MSR_EncodeTrack(EditTK2->Text.c_str(), TK2Dat, BPC2, Parity2, SS2, ES2);
MSR_EncodeTrack(EditTK3->Text.c_str(), TK3Dat, BPC3, Parity3, SS3, ES3);
if ((TK1Dat[0] == 0) && (TK2Dat[0] == 0) && (TK3Dat[0] == 0)) return;    // No data

// Send write command
if (MSR_Raw_Write(TK1Dat, TK2Dat, TK3Dat) == '0')
{
    // Success, reader enters write card mode
}

// Periodically check write status
stu = MSR_Get_Write_Erase_Status();
if (stu == 0) return;    // No card swiped
if (stu == '0')
{
    // Write successful
}
else
{
    // Write failed
}
```

## 4. Cancel Read/Write Operations

### Reset Function

```cpp
int _stdcall MSR_Reset();
```

- **Description:** This function resets the reader, allowing it to exit from card reading, writing, or erasing states.
