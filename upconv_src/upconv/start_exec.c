#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define _WIN_CP	(1)
#endif

#include "upconv.h"
#include "start_exec.h"


#if 0
#define	PRINT_LOG(s)	do {																	\
							FILE *log;															\
							log = fopen("d:\\start_exec.log","a");								\
							if (log) {															\
								fprintf(log,"%s [%d] %s\n",__FUNCTION__,__LINE__,s);			\
								fclose(log);													\
							}																	\
						} while (0)

#else
#define	PRINT_LOG(s)				//
#endif


#ifdef _WIN_CP

//---------------------------------------------------------------------------
// Function   : start_exec
// Description: プロセス起動処理
// ---
//	argc	: コマンドライン引数の数
//	argv	: コマンドライン引数の配列
//  sp_info	: プロセス情報
//
int start_exec(int argc,char *argv[],int cpu_pri,STARTEXEC_PROCESS_INFO *sp_info)
{
	HANDLE hStdOutReadTmp,hStdOutRead,hStdOutWrite;
	HANDLE hStdErrorWrite;
	SECURITY_ATTRIBUTES sa;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	int i;
	char cmd[4096];
	char work[2048];

	PRINT_LOG("start_exec():start");
	
	memset(&sa,0,sizeof(SECURITY_ATTRIBUTES));
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	sp_info->process_id  = INVALID_HANDLE_VALUE;
	sp_info->thread_id   = INVALID_HANDLE_VALUE;
	sp_info->hStdOutRead = INVALID_HANDLE_VALUE;
	
	// Read,Write用のパイプを作成する
	if (!CreatePipe(&hStdOutReadTmp,&hStdOutWrite,&sa,0)) {
		PRINT_LOG("error:CreatePipe");
		return -1;
	}
	// 標準エラー出力を複製する
	if (!DuplicateHandle(GetCurrentProcess(),hStdOutWrite,GetCurrentProcess(),&hStdErrorWrite,0,TRUE,DUPLICATE_SAME_ACCESS)) {
		PRINT_LOG("error:DuplicateHandle");
		return -1;
	}
	if (!DuplicateHandle(GetCurrentProcess(),hStdOutReadTmp,GetCurrentProcess(),&hStdOutRead,0,FALSE,DUPLICATE_SAME_ACCESS)) {
		PRINT_LOG("error:DuplicateHandle");
		return -1;
	}
	CloseHandle(hStdOutReadTmp);

	memset(&si,0,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW | 0x00000020;
	si.wShowWindow = SW_HIDE;
	si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
	if (sp_info->flag_stdout) {
		si.hStdOutput = hStdOutWrite;
		si.hStdError  = hStdErrorWrite;
	} else {
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError  = GetStdHandle(STD_ERROR_HANDLE);
	}

	// コマンドの組み立て
	cmd[0] = '\0';
	for (i = 0;i < argc;i++) {
		if (argv[i][0] != '|') {
			sprintf(work,"\"%s\" ",argv[i]);
		} else {
			sprintf(work,"%s ",&argv[i][1]);
		}
		strcat(cmd,work);
	}

	PRINT_LOG("start_exec:cmd");
	PRINT_LOG(cmd);

	if (!CreateProcess(NULL,cmd,NULL,NULL,TRUE,IDLE_PRIORITY_CLASS,NULL,NULL,&si,&pi)) {
		PRINT_LOG("error:CreateProcess");
		return -1;
	}
	if (sp_info->flag_stdout) {
		sp_info->hStdOutRead = hStdOutRead;
	}
	sp_info->process_id = pi.hProcess;
	sp_info->thread_id  = pi.hThread;

	if (cpu_pri == 0) {
		SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
	} else if (cpu_pri == 1) {
		SetPriorityClass(pi.hProcess,NORMAL_PRIORITY_CLASS);
	} else {
		SetPriorityClass(pi.hProcess,HIGH_PRIORITY_CLASS);
	}

	CloseHandle(hStdOutWrite);
	CloseHandle(hStdErrorWrite);

	PRINT_LOG("start_exec():end");
	return 0;
}

//---------------------------------------------------------------------------
// Function   : wait_exec
// Description: プロセス終了待ち処理
// ---
//	argc	: コマンドライン引数の数
//	argv	: コマンドライン引数の配列
//  sp_info	: プロセス情報
//
void wait_exec(STARTEXEC_PROCESS_INFO *sp_info,int n)
{
	int i;
	unsigned long ret;
	PRINT_LOG("wait_exec:start");
	
	for (i = 0;i < n;i++) {
		HANDLE ep = sp_info[i].process_id;
		//printf("handle:%p\n",ep);
		ret = WaitForSingleObject(ep,0xFFFFFFFF);
		//printf("ret:%lx\n",ret);
	}

	PRINT_LOG("wait_exec:end");
}

//---------------------------------------------------------------------------
// Function   : finish_exec
// Description: プロセス終了処理
// ---
//  sp_info	: プロセス情報
//
int finish_exec(STARTEXEC_PROCESS_INFO *sp_info)
{
	PRINT_LOG("finish_exec:start");

	if (sp_info->process_id  != INVALID_HANDLE_VALUE) CloseHandle(sp_info->process_id);
	if (sp_info->thread_id   != INVALID_HANDLE_VALUE) CloseHandle(sp_info->thread_id);
	if (sp_info->hStdOutRead != INVALID_HANDLE_VALUE) CloseHandle(sp_info->hStdOutRead);
	
	sp_info->process_id  = INVALID_HANDLE_VALUE;
	sp_info->thread_id   = INVALID_HANDLE_VALUE;
	sp_info->hStdOutRead = INVALID_HANDLE_VALUE;

	PRINT_LOG("finish_exec:end");

	return 0;
}

//---------------------------------------------------------------------------
// Function   : read_stdout
// Description: 標準出力読み込み処理
// ---
//  sp_info	: プロセス情報
//
int read_stdout(char *buffer,int len,STARTEXEC_PROCESS_INFO *sp_info)
{
	long read_size;

	read_size = 0;

	if (buffer != NULL && len > 0) {
		buffer[0] = '\0';

		if (sp_info->flag_stdout) {
			if (!ReadFile(sp_info->hStdOutRead,buffer,len,&read_size,NULL)) {
				PRINT_LOG("error:read_stdout");
				if (GetLastError() == ERROR_BROKEN_PIPE) {
					PRINT_LOG("error:read_stdout:broken_pipe");
					read_size = -1;
				} else {
					PRINT_LOG("error:read_stdout:other error");
					// Error
					read_size = -1;
				}
			}
		}
	}

	return read_size;
}

#else

//---------------------------------------------------------------------------
// Function   : start_exec
// Description: プロセス起動処理
// ---
//	argc	: コマンドライン引数の数
//	argv	: コマンドライン引数の配列
//  sp_info	: プロセス情報
//
int start_exec(int argc,char *argv[],int cpu_pri,STARTEXEC_PROCESS_INFO *sp_info)
{
	char cmd[4096];
	char work[4096];
	char *p;
	int i;
	int ret;

	PRINT_LOG("start_exec():start");
	
	sp_info->fp = NULL;

	ret = -1;

	// コマンドの組み立て
	cmd[0] = '\0';
	for (i = 0;i < argc;i++) {
		if (argv[i][0] != '|') {
			sprintf(work,"\"%s\" ",argv[i]);
		} else {
			sprintf(work,"%s ",&argv[i][1]);
		}
		strcat(cmd,work);
	}

	sp_info->fp = popen(cmd,"r");
	if (sp_info->fp != NULL) {
		ret = 0;
	}

	PRINT_LOG("start_exec():end");
	
	return ret;
}

//---------------------------------------------------------------------------
// Function   : wait_exec
// Description: プロセス終了待ち処理
// ---
//	argc	: コマンドライン引数の数
//	argv	: コマンドライン引数の配列
//  sp_info	: プロセス情報
//
void wait_exec(STARTEXEC_PROCESS_INFO *sp_info,int n)
{
	char work[4096];
	char *p;

	if (sp_info[n].fp != NULL) {
		while (1) {
			fgets(work,4000,sp_info[n].fp);
			if (feof(sp_info[n].fp)) {
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
// Function   : finish_exec
// Description: プロセス終了処理
// ---
//  sp_info	: プロセス情報
//
int finish_exec(STARTEXEC_PROCESS_INFO *sp_info)
{
	if (sp_info->fp != NULL) {
		wait_exec(&sp_info,0);
	}
	pclose(sp_info->fp);

	sp_info->fp = NULL;

	return 0;
}

//---------------------------------------------------------------------------
// Function   : read_stdout
// Description: 標準出力読み込み処理
// ---
//  sp_info	: プロセス情報
//
int read_stdout(char *buffer,int len,STARTEXEC_PROCESS_INFO *sp_info)
{
	char work[4096];
	char *p;

	buffer[0] = '\0';
	work[0] = '\0';
	if (sp_info->fp != NULL) {
		fgets(work,4000,sp_info->fp);
		if (strlen(work) > 0) {
			strcpy(buffer,work);
		}
	}
	
	return strlen(buffer);
}

#endif

