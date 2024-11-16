//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Unit4.h"

//---------------------------------------------------------------------------
class Exec : public TThread
{
private:
	AnsiString cmdInfo_c1;
	AnsiString cmdInfo_c2;
	AnsiString cmdInfo_c3;
	AnsiString cmdInfo_c4;
	AnsiString cmdInfo_c5;
	AnsiString cmdInfo_c6;
	AnsiString cmdInfo_c7;
	AnsiString cmdInfo_c8;
	Exec_c* threadExec_c1;
	Exec_c* threadExec_c2;
	Exec_c* threadExec_c3;
	Exec_c* threadExec_c4;
	Exec_c* threadExec_c5;
	Exec_c* threadExec_c6;
	Exec_c* threadExec_c7;
	Exec_c* threadExec_c8;
	int persent_c1;
	int persent_c2;
	int persent_c3;
	int persent_c4;
	int persent_c5;
	int persent_c6;
	int persent_c7;
	int persent_c8;
	void __fastcall UpdateProgress_c1(void);
	void __fastcall UpdateProgress_c2(void);
	void __fastcall UpdateProgress_c3(void);
	void __fastcall UpdateProgress_c4(void);
	void __fastcall UpdateProgress_c5(void);
	void __fastcall UpdateProgress_c6(void);
	void __fastcall UpdateProgress_c7(void);
	void __fastcall UpdateProgress_c8(void);
protected:
	void __fastcall Execute();
public:
	__fastcall Exec(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
