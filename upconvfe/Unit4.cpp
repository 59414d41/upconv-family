//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include "Unit1.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   重要: VCL におけるオブジェクトのメソッドとプロパティは、Synchronize を使って
//   呼び出されるメソッドでのみ使用できます。たとえば、次のようになります。
//
//      Synchronize(&UpdateCaption);
//
//   ここで、UpdateCaption は 次のようなコードになります。
//
//      void __fastcall Exec_c::UpdateCaption()
//      {
//        Form1->Caption = "スレッドで更新されました";
//      }
//---------------------------------------------------------------------------

__fastcall Exec_c::Exec_c(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall Exec_c::Execute()
{
	if (Terminated == true) {
		return;
	}
	cmdInfo = "";
	percent = 0;
	HandleProcess();

}
//---------------------------------------------------------------------------
void __fastcall Exec_c::HandleProcess()
{
	HANDLE hStdOutChild,hStdOutReadTmp,hStdOutRead;
	HANDLE hStdErrChild;
	SECURITY_ATTRIBUTES sa;
	CHAR lpBuffer[256];
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char ext[_MAX_EXT];

	char buf[2048];
	char pr_str[2048];
	char *p;
	char *p2;
	FILE *fp;
	DWORD nBytesRead;
	DWORD nCharsWritten;
	BOOL ret;
	AnsiString s,ss;
	int per,d1,d2;

	hChildProcess = NULL;
	ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES));
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	// Read,Write用のパイプを作成する
	CreatePipe(&hStdOutReadTmp,&hStdOutChild,&sa,0);
	// 標準エラー出力を複製する
	DuplicateHandle(GetCurrentProcess(),hStdOutChild,
					GetCurrentProcess(),&hStdErrChild,0,
					TRUE,DUPLICATE_SAME_ACCESS);

	// GUIアプリが使う標準出力から読み込むためのハンドルを作成する
	DuplicateHandle(GetCurrentProcess(),hStdOutReadTmp,
					GetCurrentProcess(),
					&hStdOutRead, // Address of new handle.
					0,FALSE, // Make it uninheritable.
					DUPLICATE_SAME_ACCESS);

	CloseHandle(hStdOutReadTmp);

	ret = RunChildProcess(hStdOutChild,hStdErrChild);
	CloseHandle(hStdOutChild);
	CloseHandle(hStdErrChild);
	if (ret == TRUE) {
		for (;Terminated != true;) {
			if (!ReadFile(hStdOutRead,lpBuffer,sizeof(lpBuffer),&nBytesRead,NULL)) {
				break; // pipe done
			}
			if (nBytesRead == 0) {
				continue;
			}
			if (Terminated == true) {
				TerminateProcess(hChildProcess,0);
				break;
			}
			percent = 0;
			s = "";
			lpBuffer[nBytesRead] = '\0';
			if (sscanf(lpBuffer,"%*c%[^]]%*c%d%*c/%*c%d",pr_str,&d1,&d2) == 3) {
				addp_n      = d1;
				addp_count  = d2;
				Synchronize(UpdateADDP);
			} else if (sscanf(lpBuffer,"%*c%[^]]%*c%d%*c",pr_str,&per) == 2) {
				if (per >= 0 && per <= 100) {
					percent = per;
				}
				s = (AnsiString)pr_str;
				if (s == "wav2raw") {
					cmdInfo = "wav2raw";
				} else if (s == "dsf2raw") {
					cmdInfo = "dsf2raw";
				} else if (s == "raw2wav") {
					cmdInfo = "raw2wav";
				} else if (s == "ABE") {
					cmdInfo = "ABE";
				} else if (s == "Post ABE") {
					cmdInfo = "Post ABE";
				} else if (s == "SRC") {
					cmdInfo = "Resampling";
				} else if (s == "HFC Auto") {
					cmdInfo = "HFC Auto";
				} else if (s == "LPF") {
					cmdInfo = "LPF";
				} else if (s == "NR") {
					cmdInfo = "NR";
				} else if (s == "HFA1") {
					cmdInfo = "HFA1";
				} else if (s == "HFA2") {
					cmdInfo = "HFA2";
				} else if (s == "HFA3") {
					cmdInfo = "HFA3";
				} else if (s == "HFA4") {
					cmdInfo = "HFA4";
				} else if (s == "SRC(DS)") {
					cmdInfo = "Down Sampling";
				} else if (s == "mconv") {
					cmdInfo = "mconv";
				} else if (s == "GenC") {
					cmdInfo = "Gen(Center)";
				} else if (s == "GenSLR") {
					cmdInfo = "Gen(Surround)";
				} else if (s == "GenLFE") {
					cmdInfo = "Gen(LFE)";
				} else if (s == "Post MC") {
					cmdInfo = "mconv(Post Process)";
				} else if (s == "POST MC(DM)") {
					cmdInfo = "mconv(Downmix)";
				} else if (s == "DeClip") {
					cmdInfo = "Abe DeClip";
				} else if (s == "DeClick") {
					cmdInfo = "Abe CNR";
				} else if (s == "PreEQ") {
					cmdInfo = "Pre EQ";
				} else if (s == "Post EQ") {
					cmdInfo = "Post EQ";
				} else if (s == "MS1") {
					cmdInfo = "MS Process(1)";
				} else if (s == "MS2") {
					cmdInfo = "MS Process(2)";
				} else if (s == "MS3") {
					cmdInfo = "MS Process(3)";
				} else if (s == "MS4") {
					cmdInfo = "MS Process(4)";
				} else if (s == "LRC1") {
					cmdInfo = "LRC Process";
				} else if (s == "LRC2") {
					cmdInfo = "LRC Downmix";
				} else if (s == "SFA") {
					cmdInfo = "SFA";
				} else if (s == "LA") {
					cmdInfo = "Level Adj";
				} else if (s == "Volume_Level") {
					volume_level = per;
				}
				if (s == "End") {
					uLog.sprintf(L"End:%d",index);
					Synchronize(UpdateLog);
					break;
				}
				Synchronize(UpdateProgress);
			}
		}
		if (Terminated != true) {
			uLog.sprintf(L"Before:WaitSingleObject[%d]",index);
			Synchronize(UpdateLog);
			WaitForSingleObject(hChildProcess,INFINITE);
			uLog.sprintf(L"WaitSingleObject[%d]",index);
			Synchronize(UpdateLog);
			GetExitCodeProcess(hChildProcess,&exitCode);
		}
		CloseHandle(hChildProcess);
		CloseHandle(hChildThread);
		if (Terminated == true) {
			Sleep(3000);
			// 後始末
			_splitpath(toFile.c_str(),drive,dir,file,ext);
			_makepath(fullpath,drive,dir,file,"files");
			fp = fopen(fullpath,"r");
			if (fp != NULL) {
				while (fgets(buf,_MAX_PATH,fp) != NULL) {
					p = strrchr(buf,'\n');
					if (p) {
						*p = '\0';
					}
					unlink(buf);
				}
				fclose(fp);
				unlink(fullpath);
			}
			if (Terminated || FileExists(errFile)) {
				unlink(toFile.c_str());
			}
			_splitpath(toFile.c_str(),drive,dir,file,ext);
			_makepath(fullpath,drive,dir,file,"abort");
			unlink(fullpath);
		}
	}
	hChildProcess = NULL;
	CloseHandle(hStdOutRead);
	Synchronize(EndProcess);
}
//---------------------------------------------------------------------------
BOOL __fastcall Exec_c::RunChildProcess(HANDLE hStdOut,HANDLE hStdErr)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL ret;
	UnicodeString ucmd;
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = hStdOut;
	si.hStdError  = hStdErr;

	ret = TRUE;
	ucmd = (UnicodeString)cmd;
	if (!CreateProcess(NULL,ucmd.c_str(),NULL,NULL,TRUE,CREATE_NEW_CONSOLE | IDLE_PRIORITY_CLASS | 0x00000200,NULL,NULL,&si,&pi)) {
		ret = FALSE;
	} else {
		hChildProcess = pi.hProcess;
		hChildThread  = pi.hThread;
		if (cpu_pri == 0) {
			SetPriorityClass(hChildProcess,IDLE_PRIORITY_CLASS);
		} else if (cpu_pri == 1) {
			SetPriorityClass(hChildProcess,NORMAL_PRIORITY_CLASS);
		} else {
			SetPriorityClass(hChildProcess,HIGH_PRIORITY_CLASS);
		}
	}
	return ret;
}
//---------------------------------------------------------------------------
void __fastcall Exec_c::AbortExec(void)
{
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char ext[_MAX_EXT];
	FILE *fp;
	char work[_MAX_PATH];
	char *p;

	if (Finished == false && Terminated == false) {
		//TerminateProcess(hChildProcess,0);
		_splitpath(toFile.c_str(),drive,dir,file,ext);
		_makepath(fullpath,drive,dir,file,"abort");
		fp = fopen(fullpath,"w");
		if (fp != NULL) {
			fprintf(fp,"abort");
			fclose(fp);
		}
		//this->Terminate();
	}
}
//---------------------------------------------------------------------------
void __fastcall Exec_c::UpdateProgress(void)
{
	Form1->UpdateProgress(index,cmdInfo,percent);
}
//---------------------------------------------------------------------------
void __fastcall Exec_c::UpdateADDP(void)
{
	Form1->UpdateADDP(addp_n,addp_count);
}
//---------------------------------------------------------------------------
void __fastcall Exec_c::UpdateLog(void)
{
	Form1->UpdateLog(uLog);
}
//---------------------------------------------------------------------------
void __fastcall Exec_c::EndProcess(void)
{
	Form1->EndExec(index,exitCode);
}
//---------------------------------------------------------------------------
int __fastcall Exec_c::IsEndProcess(void)
{
	DWORD dwExitCode;
	dwExitCode = GetExitCodeProcess(this->hChildProcess,&dwExitCode);
	if (dwExitCode == STILL_ACTIVE) {
		return false;
	} else {
		return true;
	}
}

