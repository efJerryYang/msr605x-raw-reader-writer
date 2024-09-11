//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormWriteCard : public TForm
{
__published:	// IDE-managed Components
    TEdit *EditTK1;
    TEdit *EditTK2;
    TEdit *EditTK3;
    TGroupBox *GroupBox1;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TButton *ButtonAutoScan;
    TEdit *EditPort;
    TEdit *EditModel;
    TEdit *EditVersion;
    TButton *ButtonRead;
    TButton *ButtonErase;
    TButton *ButtonWrite;
    TButton *ButtonExit;
    TCheckBox *CheckBoxHiCo;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TTimer *Timer1;
    TStatusBar *StatusBar1;
    TLabel *LabelTK1LEN;
    TLabel *LabelTK2LEN;
    TLabel *LabelTK3LEN;
    void __fastcall ButtonAutoScanClick(TObject *Sender);
    void __fastcall ButtonReadClick(TObject *Sender);
    void __fastcall ButtonWriteClick(TObject *Sender);
    void __fastcall ButtonEraseClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall CheckBoxHiCoClick(TObject *Sender);
    void __fastcall ButtonExitClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall EditTK1Change(TObject *Sender);
    void __fastcall EditTK2Change(TObject *Sender);
    void __fastcall EditTK3Change(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormWriteCard(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormWriteCard *FormWriteCard;
//---------------------------------------------------------------------------
#endif
