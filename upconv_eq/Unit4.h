//---------------------------------------------------------------------------

#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class Exec_c : public TThread
{
private:
	HANDLE hChildProcess;
	HANDLE hChildThread;
	void __fastcall HandleProcess(void);
	BOOL __fastcall RunChildProcess(HANDLE hStdout,HANDLE hStdErr);
	void __fastcall UpdateProgress(void);
	void __fastcall EndProcess(void);
	void __fastcall UpdateLog(void);
protected:
	UnicodeString uLog;
	void __fastcall Execute();
public:
	AnsiString cmdInfo;
	int index;
	int percent;
	int flag_progress;
	int flag_finish;
	int cpu_pri;
	DWORD exitCode;
	AnsiString cmd;
	AnsiString toFile;
	AnsiString errFile;
	AnsiString workFile;
	__fastcall Exec_c(bool CreateSuspended);
	void __fastcall AbortExec(void);
};
//---------------------------------------------------------------------------
#endif
