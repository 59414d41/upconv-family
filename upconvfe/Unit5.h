//---------------------------------------------------------------------------

#ifndef Unit5H
#define Unit5H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm5 : public TForm
{
__published:	// IDE �ŊǗ������R���|�[�l���g
	TScrollBox *ScrollBox1;
	TValueListEditor *tvDesc;
	TPanel *Panel1;
	TTreeView *tvTree;
	TImage *Image1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall tvTreeClick(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
	AnsiString filename;
	__fastcall TForm5(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm5 *Form5;
//---------------------------------------------------------------------------
#endif
