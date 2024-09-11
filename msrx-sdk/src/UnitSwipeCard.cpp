//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "UnitSwipeCard.h"
#include "MSR_API.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSwipeCard *FormSwipeCard;
extern int CmdType;

//---------------------------------------------------------------------------
__fastcall TFormSwipeCard::TFormSwipeCard(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormSwipeCard::BitBtnCancelClick(TObject *Sender)
{
    MSR_Reset();
    FormWriteCard->Timer1->Enabled = false;
    FormWriteCard->Enabled = 1;
    FormSwipeCard->Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormSwipeCard::FormActivate(TObject *Sender)
{
    static int PositionReady = 0;
    if (PositionReady == 0)
    {
        PositionReady = 1;
        FormSwipeCard->Left = FormWriteCard->Left + (FormWriteCard->Width - FormSwipeCard->Width)/2-60;
        FormSwipeCard->Top  = FormWriteCard->Top  + (FormWriteCard->Height - FormSwipeCard->Height)/2 + 50;
    }

    switch (CmdType)
    {
    case 1:
        FormSwipeCard->Caption = "MSR609 READ CARD";
        FormSwipeCard->Color = clTeal;
        break;
    case 2:
        FormSwipeCard->Caption = "MSR609 WRITE CARD";
        FormSwipeCard->Color = clRed;
        break;
    case 3:
        FormSwipeCard->Caption = "MSR609 ERASE CARD";
        FormSwipeCard->Color = clRed;
        break;
    }
    FormWriteCard->Timer1->Enabled = true;
    FormWriteCard->Enabled = 0;
}
//---------------------------------------------------------------------------
