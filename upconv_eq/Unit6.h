//---------------------------------------------------------------------------

#ifndef Unit6H
#define Unit6H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TForm6 : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TLabel *Label1;
	TEdit *edt_DP_version;
	TLabel *Label2;
	TEdit *edt_dp_mtime;
	TMemo *edt_default_parameter;
	TBitBtn *btnClose;
	TBitBtn *btnSave;
	TStaticText *StaticText1;
	TEdit *edt_dpn;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TForm6(TComponent* Owner);
	char dp_version[64];
	char dp_mtime[64];
	char dp_param[4096];
};
//---------------------------------------------------------------------------
extern PACKAGE TForm6 *Form6;
//---------------------------------------------------------------------------
#endif
