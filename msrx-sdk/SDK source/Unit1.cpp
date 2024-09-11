//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "MSR_API.h"
#include "UnitSwipeCard.h"

unsigned char CardType = 1;	// 0->RawData, 1->ISO, 2->AAMVA, 3->CaDMV, 4->UserType
unsigned char BPC1 = 7, Parity1 = 1, SS1 = 0x05, ES1 = 0x1F;
unsigned char BPC2 = 5, Parity2 = 1, SS2 = 0x0B, ES2 = 0x0F;
unsigned char BPC3 = 5, Parity3 = 1, SS3 = 0x0B, ES3 = 0x0F;

int CmdType = 0;


//*/
int BPI = 0;
int CoLoHi = 1;
int EraseSelTK = 0;
int SegSelTK  = 1;
INT64 SegNum  = 0;
int SegByte = 0;
//
int SegStart = 0;
INT64 SegTotal = 1;

int Tk1Stu, Tk2Stu, Tk3Stu;
char TK1AscBuff[1024], TK2AscBuff[1024], TK3AscBuff[1024];
unsigned char TK1Dat[1024];
unsigned char TK2Dat[1024];
unsigned char TK3Dat[1024];
int FlagNextRecord;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormWriteCard *FormWriteCard;
//---------------------------------------------------------------------------
__fastcall TFormWriteCard::TFormWriteCard(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TFormWriteCard::ButtonAutoScanClick(TObject *Sender)
{

	unsigned int tick;
	char stu;
	String str;
	char temp[32];
	
	CHAR szPortName[25];
	LONG Status;
	DWORD Type;
	
	StatusBar1->Panels->Items[0]->Text = "Wait for Auto Scan ......";
	ButtonRead->Enabled = false;
	ButtonErase->Enabled = false;
	ButtonWrite->Enabled = false;
	
	memcpy(szPortName, "HID", strlen("HID")+1);
	str = ""; str += szPortName;
	EditPort->Text = str;
	
	MSR_ExitComm();
	
    if(MSR_InitComm(szPortName,9600))
    {
   	    MSR_Reset();
    	tick = GetTickCount();
    	while ((unsigned int)GetTickCount()-tick <5){};

        memset(temp, 0, sizeof(temp));
		stu = MSR_Get_Firmware_Version(temp);
		if (stu=='0')
        {
            str = ""; str+= temp;
            EditVersion->Text = str;

            memset(temp, 0, sizeof(temp));
			stu = MSR_Get_Device_Model(temp);
            if (stu=='0')
            {
	            str = ""; str+= temp;
	            EditModel->Text = str;

	            MSR_Select_BPI(0xa1);
	            MSR_Select_BPI(0xc1);
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
        EditPort->Text = "";
        EditModel->Text = "";
        EditVersion->Text = "";
        StatusBar1->Panels->Items[0]->Text = "No Found Device";
    }
    else
    {
        StatusBar1->Panels->Items[0]->Text = "";
        ButtonRead->Enabled = true;
        ButtonErase->Enabled = true;
        ButtonWrite->Enabled = true;
        ButtonRead->SetFocus();
    }

}
//---------------------------------------------------------------------------
void __fastcall TFormWriteCard::ButtonReadClick(TObject *Sender)
{
    StatusBar1->Panels->Items[0]->Text = "";
    EditTK1->Text = "";
    EditTK2->Text = "";
    EditTK3->Text = "";
    if (MSR_Raw_Read() != '0')
    {
		MessageDlg("Comunication Error, Please Check the Deivce is Connected!", mtInformation, TMsgDlgButtons()<<mbOK, 0);
        return;
    }
    CmdType = 1;
    FormSwipeCard->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormWriteCard::ButtonWriteClick(TObject *Sender)
{
	MSR_EncodeTrack(EditTK1->Text.c_str(), TK1Dat, BPC1, Parity1, SS1, ES1);
	MSR_EncodeTrack(EditTK2->Text.c_str(), TK2Dat, BPC2, Parity2, SS2, ES2);
	MSR_EncodeTrack(EditTK3->Text.c_str(), TK3Dat, BPC3, Parity3, SS3, ES3);
    if ((TK1Dat[0]==0)&&(TK2Dat[0]==0)&&(TK3Dat[0]==0)) return;

//    TK1Dat[0]+=2;
//    TK2Dat[0]+=2;
//    TK3Dat[0]+=2;

unsigned char i, j, k;
    i = TK1Dat[0];
    j = TK2Dat[0];
    k = TK3Dat[0];

    StatusBar1->Panels->Items[0]->Text = "";
    if (MSR_Raw_Write(TK1Dat, TK2Dat, TK3Dat) != '0')
    {
		MessageDlg("Comunication Error, Please Check the Deivce is Connected!", mtInformation, TMsgDlgButtons()<<mbOK, 0);
        return;
    }
    CmdType = 2;
    FormSwipeCard->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormWriteCard::ButtonEraseClick(TObject *Sender)
{
    StatusBar1->Panels->Items[0]->Text = "";
    EditTK1->Text = "";
    EditTK2->Text = "";
    EditTK3->Text = "";
    if (MSR_Erase(0x07) != '0')
    {
		MessageDlg("Comunication Error, Please Check the Deivce is Connected!", mtInformation, TMsgDlgButtons()<<mbOK, 0);
        return;
    }
    CmdType = 3;
    FormSwipeCard->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormWriteCard::Timer1Timer(TObject *Sender)
{
    int stu;
    unsigned char ch;
    unsigned int i;

    char temp[256];
    String str;

    if (CmdType == 1)
    {
        stu = MSR_Get_RawRead_Data(TK1Dat, TK2Dat, TK3Dat);
    }
    else if ((CmdType == 2) || ((CmdType == 3)))
    {
		stu = MSR_Get_Write_Erase_Status();
    }
	if (stu == 0) return;
    if (stu < 0)
    {
        Timer1->Enabled = false;
        FormWriteCard->Enabled = 1;
        FormSwipeCard->Close();
        return;
    }

    if (stu == '0')
    {
        if (CmdType == 1)
	    {
	    	Tk1Stu = MSR_DecodeTrack(TK1AscBuff, TK1Dat, BPC1, Parity1, SS1, ES1);
	    	Tk2Stu = MSR_DecodeTrack(TK2AscBuff, TK2Dat, BPC2, Parity2, SS2, ES2);
	    	Tk3Stu = MSR_DecodeTrack(TK3AscBuff, TK3Dat, BPC3, Parity3, SS3, ES3);
			str = "";
			if (Tk1Stu > 2)
			{
				memcpy(TK1AscBuff, TK1AscBuff+1, Tk1Stu-2);
				TK1AscBuff[Tk1Stu-2] = 0;
				Tk1Stu -= 2;
				str += TK1AscBuff;
			}
			EditTK1->Text = str;
			str = "";
			if (Tk2Stu > 2)
			{
				memcpy(TK2AscBuff, TK2AscBuff+1, Tk2Stu-2);
				TK2AscBuff[Tk2Stu-2] = 0;
				Tk2Stu -= 2;
				str += TK2AscBuff;
			}
			EditTK2->Text = str;
			str = "";
			if (Tk3Stu > 2)
			{
				memcpy(TK3AscBuff, TK3AscBuff+1, Tk3Stu-2);
				TK3AscBuff[Tk3Stu-2] = 0;
				Tk3Stu -= 2;
				str += TK3AscBuff;
			}
			EditTK3->Text = str;
            StatusBar1->Panels->Items[0]->Text = "Read Card OK!";
        }
        else if (CmdType == 2)
            StatusBar1->Panels->Items[0]->Text = "Write Card OK!";
        else if (CmdType == 3)
            StatusBar1->Panels->Items[0]->Text = "Erase Card OK!";
    }
    else
    {
        if (CmdType == 1)
	    {
	        MSR_Red_LED_On();
            StatusBar1->Panels->Items[0]->Text = "Read Card Error!";
	    }
        else if (CmdType == 2)
            StatusBar1->Panels->Items[0]->Text = "Write Card Error!";
        else if (CmdType == 3)
            StatusBar1->Panels->Items[0]->Text = "Erase Card Error!";
    }

    FormSwipeCard->BitBtnCancelClick(this);

    CmdType = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormWriteCard::FormActivate(TObject *Sender)
{
    static int PositionReady = 0;
    if (PositionReady == 0)
    {
        PositionReady = 1;
        ButtonAutoScanClick(this);
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormWriteCard::CheckBoxHiCoClick(TObject *Sender)
{
    if (ButtonWrite->Enabled == false) return;
    
    if (CheckBoxHiCo->Checked == true)
    	MSR_Set_HiCo();
    else
    	MSR_Set_LoCo();
}
//---------------------------------------------------------------------------

void __fastcall TFormWriteCard::ButtonExitClick(TObject *Sender)
{
    StatusBar1->Panels->Items[0]->Text = "";
    CmdType  = 0;
    FormWriteCard->Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormWriteCard::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    MSR_ExitComm();
}
//---------------------------------------------------------------------------

void __fastcall TFormWriteCard::EditTK1Change(TObject *Sender)
{
    LabelTK1LEN->Caption = strlen(EditTK1->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFormWriteCard::EditTK2Change(TObject *Sender)
{
    LabelTK2LEN->Caption = strlen(EditTK2->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFormWriteCard::EditTK3Change(TObject *Sender)
{
    LabelTK3LEN->Caption = strlen(EditTK3->Text.c_str());
}
//---------------------------------------------------------------------------


