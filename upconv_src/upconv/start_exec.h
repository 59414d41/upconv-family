/****************************************************************************/
/* start_exec (C) 2022 By 59414d41											*/
/* ÉvÉçÉZÉXä«óù																*/
/*																			*/
/****************************************************************************/

#ifndef START_EXEC_H
#define START_EXEC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN_CP
typedef struct {
	HANDLE	process_id;
	HANDLE	thread_id;
	HANDLE	hStdOutRead;
	int		flag_stdin;
	int		flag_stdout;
} STARTEXEC_PROCESS_INFO;
#else
typedef struct {
	FILE *fp;
} STARTEXEC_PROCESS_INFO;
#endif

int start_exec(int argc,char *argv[],int cpu_pri,STARTEXEC_PROCESS_INFO *sp_info);
void wait_exec(STARTEXEC_PROCESS_INFO *sp_info,int n);
int finish_exec(STARTEXEC_PROCESS_INFO *sp_info);
int read_stdout(char *buffer,int len,STARTEXEC_PROCESS_INFO *sp_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

