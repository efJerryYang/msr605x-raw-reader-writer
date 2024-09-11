//---------------------------------------------------------------------------

#ifndef UnitSwipeCardH
#define UnitSwipeCardH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormSwipeCard : public TForm
{
__published:	// IDE-managed Components
    TBitBtn *BitBtnCancel;
    TLabel *Label1;
    void __fastcall BitBtnCancelClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormSwipeCard(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSwipeCard *FormSwipeCard;
//---------------------------------------------------------------------------
#endif
