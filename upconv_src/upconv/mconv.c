//---------------------------------------------------------------------------
/****************************************************************************/
/* mconv (C) 2010-2012 By 59414d41											*/
/*																			*/
/*																			*/
/****************************************************************************/

/*--- Log ------------------------------------------------------------------
 * Ver 0.10 <10/12/05> - 作成
 * Ver 0.20 <11/06/12> - 高速化対応
 * Ver 0.30 <12/06/30> - 高速化対応、fio対応,upconv の GUI から呼び出すようにした。
 * Ver 1.20 <19/11/03> - upconv.c から呼び出すように修正
 * Ver 1.21 <21/04/23> - upconv 0.8.4 用に修正
 */

#define STR_COPYRIGHT	"mconv.exe (c) 2019 Ver 1.20 By 59414d41\n\n"
#define STR_USAGE		"mconv.exe paramfile\n"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "./upconv.h"
#include "./fileio.h"
#include "./fftw3.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#ifndef M_PI
#define M_PI		(3.14159265358979323846)
#endif

#ifndef DWORD
#define DWORD		unsigned long
#endif

#ifndef TRUE
#define TRUE		(1)
#endif

#ifndef FALSE
#define FALSE		(0)
#endif

#define CLIP_NX(v,nx)	(v) == 0 ? ((SSIZE)0) : \
							((v) > 0 ?	\
								(((SSIZE)(v) * nx) >= (SSIZE)(0x007FFFFFFFFFFFFF) ?		(SSIZE)(0x7FFFFFFFFFFFFF)		: ((SSIZE)((v) * nx))) : \
								(((SSIZE)(v) * nx) <= ((SSIZE)0x007FFFFFFFFFFFFF * -1) ? ((SSIZE)0x7FFFFFFFFFFFFF * -1) : ((SSIZE)((v) * nx))))

#if 0
#define	PRINT_LOG(s)	do {																	\
							FILE *log;															\
							log = fopen("d:\\mconv.log","a");									\
							if (log) {															\
								fprintf(log,"%s [%d] %s\n",__FUNCTION__,__LINE__,s);			\
								fclose(log);													\
							}																	\
						} while (0)
#else
#define	PRINT_LOG(s)	//
#endif

// サンプルを処理するデータ型
#define SSIZE	signed long long

typedef struct {
	double p1;
	SSIZE  p2;
	double p3;
	double p4;
} PARAM_NX;

typedef struct {
	long mc_inSampleR;
	long mc_outSampleR;
//	long chC;
//	long chS;
//	long chLFE;
//	long ch;
	long adjust;
	long echo;
//	long downmix;
	fftw_complex *fftw_out[3];
	PARAM_NX *param_nx;
	UPCONV_PARAM *p;
	long leftSave;
	int  err;
	int  error_line;
//	int  thread;
//	int  fio;
	int dis_downsample;
	char *workpath;
	char abort_filename[_MAX_PATH];
	char pr_str[40];
	int abort_percent;
} PARAM_INFO;

typedef struct {
	int	m;
	double n;
} DELAY_PARAM;

NORM_INFO NormInfo;
SSIZE *diskBuffer1;
SSIZE *diskBuffer2;
extern ABORT_INFO abortInfo;
// 
DELAY_PARAM DelayParam1[]={16,0.38};
DELAY_PARAM DelayParam2[]={
{15,0.29},{26,0},{29,0},{33,0},{42,0},{42,0},{53,0},{54,0},{59,0},{65,0}
,{20,0},{24,0},{28,0},{34,0},{40,0},{44,0},{48,0},{54,0},{59,0},{63,0}
,{54,0},{69,0},{86,0},{103,0},{119,0},{134,0},{152,0},{166,0},{172,0},{177,0}
,{135,0},{189,0},{216,0},{264,0},{312,0},{332,0},{344,0},{354,0},{270,0},{378,0},{432,0},{528,0},{624,0},{0,0}};
DELAY_PARAM DelayParam3[]={
{31,0.15},{48,0},{66,0},{82,0},{98,0},{114,0},{130,0},{146,0},{162,0},{179,0}};

DELAY_PARAM DelayParam4[]={{160,0.25},{321,0.11},{328,0.08},{368,0.07},{381,0.07},{477,0.06},{489,0.06},{517,0.07},{618,0.05},{631,0.06},{648,0.04},{671,0.04},{699,0.06},{713,0.04},{739,0.05},{758,0.07},{783,0.04},{822,0.06},{855,0.03},{877,0.02},{899,0},{1003,0},{1203,0},{0,0}};

DELAY_PARAM Adjust[]={{0,0},{2,0},{4,0},{8,0},{27,3}};

/*--- Function Prototype ---------------------------------------------------*/
void MultiConvert(char *paramFile,PARAM_INFO *param);
static void fftFilter(int type,SSIZE inSample1,SSIZE inSample2,FIO *fio1,FIO *fio2,FIO *fio3,PARAM_INFO *param);
static void fftFilterSub(int type,SSIZE *pIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,PARAM_INFO *param,int id);
static void merageTempFile(char type,int normFlag,FIO *fp_r,FIO *fp_r2,FIO *fp_w,SSIZE inSample,PARAM_INFO *param);
void convolutionFile(SSIZE inSample,FIO *fio_in1,FIO *fio_in2,FIO *fio_out,PARAM_INFO *param);
extern void calcNormInfo(FIO *fp_r,SSIZE inSample,PARAM_INFO *param,NORM_INFO *normInfo);
extern void copyToFFTW(fftw_complex *fftw,SSIZE *buf,long size);
extern void windowFFTW(fftw_complex *fftw,long size);
extern void cutFFTW(fftw_complex *fftw,long index,long size);
extern void *al_malloc(long size);
extern void *al_free(void *ptr);
static void volumeAdjustFile(double nx,FIO *in,FIO *out,SSIZE inSample,PARAM_INFO *param,NORM_INFO *normInfo);
extern int chkAbort(int percent,int percent_diff);

//---------------------------------------------------------------------------
// Function   : main
// Description: 引数を処理し変換関数を呼び出す
//
//
int mc_main(int argc, char *argv[],PARAM_NX *param_nx,UPCONV_PARAM *p)
{
	FILE *fp;
	FILE *fp_files;
	FILE *fp_err;
	char tmppath[_MAX_PATH];
	char workpath[_MAX_PATH];
	char filepath1[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char workdrive[_MAX_DRIVE];
	char workdir[_MAX_DIR];
	char workfname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char pparam[4096];
	char sss[100];
	double rx[6];
	SSIZE  rax[6];
	long paramFlag;
	char *p1,*p2;
	SSIZE i;
	SSIZE is;
	SSIZE rd;
	static PARAM_INFO param;
	long temp,temp2,temp3,temp4,temp5;
	char *tmp;
	int retCode;
	SSIZE max;
	int c;
	long errLine;
	long paramDelay,paramPower;

	do {
		diskBuffer1 = (SSIZE *)al_malloc(sizeof (SSIZE) * 4 * 1024 * 1024);
		diskBuffer2 = (SSIZE *)al_malloc(sizeof (SSIZE) * 4 * 1024 * 1024);
		if (diskBuffer1 == NULL || diskBuffer2 == NULL) {
			param.err = STATUS_MEM_ALLOC_ERR;
			param.error_line = __LINE__;
			break;
		}

		memset(&param,0,sizeof (PARAM_INFO));
		pparam[0] = '\0';

//		param.chC = 0;		// Center
//		param.chS = 0;		// Sourround
//		param.chLFE = 0;	// LFE
		param.dis_downsample = 0;
		param.param_nx = param_nx;
		param.p = p;
		param.err = STATUS_SUCCESS;
//		param.thread = 8;
//		param.fio = 5;
		param.abort_percent = 0;
		paramFlag = 0;
		paramDelay = 2;
		paramPower = 2;
		

		retCode = 0;
PRINT_LOG("mc_main");
		if (argc == 5) {
			sprintf(sss,"argv[1]:%s",argv[1]);
			PRINT_LOG(sss);
			sprintf(sss,"argv[2]:%s",argv[2]);
			PRINT_LOG(sss);
			sprintf(sss,"argv[3]:%s",argv[3]);
			PRINT_LOG(sss);
			sprintf(sss,"argv[4]:%s",argv[4]);
			PRINT_LOG(sss);

			fp = fopen(argv[3],"r");
			if (fp == NULL) {
				retCode = STATUS_FILE_READ_ERR;	errLine = __LINE__;
				break;
			}
			// パラメーター作成
			if (fgets(pparam,4000,fp) == NULL) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			p1 = strrchr(pparam,'\n');if (p1 != NULL) *p1 = '\0';
			strcat(pparam," ");
			strcat(pparam,argv[4]);
			if (fgets(workpath,4000,fp) == NULL) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			p1 = strrchr(workpath,'\n');if (p1 != NULL) *p1 = '\0';
			if (strlen(workpath) >= 2 && workpath[strlen(workpath) - 1] != '\\') strcat(workpath,"\\");

			fclose(fp);

			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"abort");
			strcpy(param.abort_filename,tmppath);

			p1 = pparam;
			p2 = strchr(p1,(int)' ');

#if 0
			for (;p1 != NULL;) {
				if (p2 != NULL) {
					*p2 = '\0';
				}

				if (sscanf(p1,"-ms:%ld",&temp) == 1) {
					switch (temp) {
						case 32000:
						case 44100:
						case 48000:
						case 88200:
						case 96000:
						case 176400:
						case 192000:
						case 352800:
						case 384000:
						case 705600:
						case 768000:
							param.inSampleR = temp;
							paramFlag |= 0x0001;
							break;
					}
				}

				if (sscanf(p1,"-s:%ld",&temp) == 1) {
					switch (temp) {
						case 32000:
						case 44100:
						case 48000:
						case 88200:
						case 96000:
						case 176400:
						case 192000:
						case 352800:
						case 384000:
						case 705600:
						case 768000:
							param.outSampleR = temp;
							paramFlag |= 0x0002;
							break;
					}
				}
				if (sscanf(p1,"-ch:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 6) {
						param.ch = temp;
					}
				}
				if (strcmp(p1,"-C") == 0) {
					paramFlag |= 0x0100;
					param.chC = 1;
				}
				if (strcmp(p1,"-SLR") == 0) {
					paramFlag |= 0x0100;
					param.chS = 1;
				}
				if (strcmp(p1,"-LFE") == 0) {
					paramFlag |= 0x0100;
					param.chLFE = 1;
				}

				if (sscanf(p1,"-MC_Option:%d,%d,%d,%d",&temp,&temp2,&temp3,&temp4) == 4) {
					if (temp2) param.downmix = 1;
					if (temp3) param.echo = 1;
					if (temp4 >= 0 && temp4 <= 4) param.adjust = temp4;
				}

				if (sscanf(p1,"-thread:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 24) {
						param.thread = (int)temp;
					}
				}

				if (sscanf(p1,"-fio:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 1000) {
						param.fio = temp;
					}
				}

				if (p2 == NULL) {
					break;
				}
				p1 = p2 + 1;
				p2 = strchr(p1,(int)' ');
			}
#endif

#ifdef _OPENMP
	omp_set_num_threads(p->thread);
#endif
PRINT_LOG("before:paramFlag");

			paramFlag = 0x0103;
			if (paramFlag == 0x0103) {
				param.mc_inSampleR  = p->outSampleR_ms;
				param.mc_outSampleR = p->outSampleR;

				param.workpath = workpath;

				_splitpath(argv[2],drive,dir,fname,ext);
				_splitpath(workpath,workdrive,workdir,workfname,NULL);
				_makepath(tmppath,workdrive,workdir,fname,"tmp");
				if (strlen(workdrive) == 2 && strlen(workdir) >= 1) {
					strcpy(workfname,fname);
				} else {
					strcpy(workdrive,drive);
					strcpy(workdir,dir);
					strcpy(workfname,fname);
				}

				if (p->chC == 1 && p->LRC_process == 0) {
					param.param_nx[2].p1 = (param.param_nx[0].p1 + param.param_nx[1].p1) / 2;
				}
				if (p->chS == 1) {
					param.param_nx[3].p1 = ((param.param_nx[0].p1 * 0.75) + (param.param_nx[1].p1 * 0.25)) * 0.6;
					param.param_nx[4].p1 = ((param.param_nx[0].p1 * 0.25) + (param.param_nx[1].p1 * 0.75)) * 0.6;
				}
				if (p->chLFE == 1) {
					param.param_nx[5].p1 = ((param.param_nx[0].p1 + param.param_nx[1].p1 + param.param_nx[2].p1 + param.param_nx[3].p1 + param.param_nx[4].p1) / 5.0) * 0.7;
				}
				if (p->chC == 0) {
					param.param_nx[2].p1 = 0;
				}
				if (p->chS == 0) {
					param.param_nx[3].p1 = 0;
					param.param_nx[4].p1 = 0;
				}
				if (p->chLFE == 0) {
					param.param_nx[5].p1 = 0;
				}

PRINT_LOG("before:MultiConvert");
				MultiConvert(argv[2],&param);
				if (param.err) {
					break;
				}
			}
		}
	} while (0);

	_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r3.tmp");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r4.tmp");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r5.tmp");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r6.tmp");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r3.tmp2");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r4.tmp2");
	unlink(tmppath);

	_makepath(tmppath,workdrive,workdir,workfname,"r5.tmp2");
	unlink(tmppath);

	if (param.err) {
		_makepath(tmppath,workdrive,workdir,workfname,"r3");
		unlink(tmppath);

		_makepath(tmppath,workdrive,workdir,workfname,"r4");
		unlink(tmppath);

		_makepath(tmppath,workdrive,workdir,workfname,"r5");
		unlink(tmppath);

		_makepath(tmppath,workdrive,workdir,workfname,"r6");
		unlink(tmppath);

		_splitpath(argv[2],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"err");
		fp_err = fopen(tmppath,"a");
		if (fp_err) {
			if (param.err == STATUS_FILE_READ_ERR) {
				fprintf(fp_err,"mconv.exe - [%04d] Read Error.",param.error_line);
			}
			if (param.err == STATUS_FILE_WRITE_ERR) {
				fprintf(fp_err,"mconv.exe - [%04d] Write Error.",param.error_line);
			}
			if (param.err == STATUS_MEM_ALLOC_ERR) {
				fprintf(fp_err,"mconv.exe - [%04d] Memory Allocation Error.",param.error_line);
			}
			fclose(fp_err);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
// Function   : MultiConvert
// Description: ステレオファイルからマルチチャンネルファイルを作成する
// 
// 基準の音から何サンプル遅れているかを計算し、基準のLRの音声に遅れたサンプルをたしこんでいく。
// Lの音にはLの音が遅延したものと、Rの音が遅延したものの両方が含まれる
// Rの音にはRの音が遅延したものと、Lの音が遅延したものの両方が含まれる
// Cの音はLとRの共通な音が強く含まれる、非共通な音は弱い
// LFEはそれぞれの音の低音のみ含まれる
//
// 1.各音を生成
// 2.downmixならミックスする
// 3.サンプリングレートを1/2する
//
// ---
//	paramFile	: paramファイル名
//	param	: 変換パラメータ構造体
//
void MultiConvert(char *paramFile,PARAM_INFO *param)
/*
 *	マルチチャンネルファイル作成
 */
{
	char outFile[_MAX_PATH];
	char tmpFile[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char workdrive[_MAX_DRIVE];
	char workdir[_MAX_DIR];
	char workfname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char sss[100];
	FIO fp_l,fp_r,fp_w,fp_tmp;
	FILE *fp;
	SSIZE inSample1,inSample2;
	SSIZE i;
	SSIZE rd,wr;
	SSIZE size;
	SSIZE max;
	double dd;
	double persent;
	double param_nx;
	SSIZE avg;
	double persent2;
	double vla_nx;
	SSIZE r1_size;
	SSIZE r2_size;
	SSIZE r3_size;
	SSIZE r4_size;
	SSIZE r5_size;
	SSIZE r6_size;
	SSIZE max_size;
	FIO_COMMON_PARAM fio_common;

	memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));

	if (param->p->fio_sec_size > 0) {
		// 入力のn秒分
		fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
		fio_common.sector_n    = param->p->fio_sec_size;
		fio_common.upconv_n    = 3;
	} else {
		// n MB 分
		fio_common.sector_size = (fio_size)1 * 1000 * 1000;
		fio_common.sector_n    = param->p->fio_mb_size;
		fio_common.upconv_n    = 3;
	}

	do {
		memset(&fp_l,0,sizeof (FIO));
		memset(&fp_r,0,sizeof (FIO));
		memset(&fp_w,0,sizeof (FIO));
		memset(&fp_tmp,0,sizeof (FIO));

		sprintf(sss,"MultiConvert:inSampleR:%ld,OutSampleR:%ld",param->mc_inSampleR,param->mc_outSampleR);
		PRINT_LOG(sss);
		fprintf(stdout,"[mconv]0%\n");
		fflush(stdout);
		_splitpath(paramFile,drive,dir,fname,ext);
		_splitpath(param->workpath,workdrive,workdir,workfname,NULL);
		_makepath(tmpFile,workdrive,workdir,workfname,"r1");
		if (strlen(workdrive) == 2 && strlen(workdir) >= 1) {
			strcpy(workfname,fname);
		} else {
			strcpy(workdrive,drive);
			strcpy(workdir,dir);
			strcpy(workfname,fname);
		}

		// Front Left
		_makepath(outFile,workdrive,workdir,workfname,"r1");
PRINT_LOG(outFile);
		inSample1 = 0;
		//fio_open(&fp_l,outFile,FIO_MODE_R);
		fio_open_with_set_param(&fp_l,outFile,FIO_MODE_R,&fio_common);
		if (fp_l.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		//fio_set_memory_limit(&fp_l,20,param->p->fio);
		fio_set_memory_limit(&fp_l,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);

		// ファイルサイズ取得
		fio_get_filesize(&fp_l,&size);
		if (fp_l.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		sprintf(sss,"first r1 size:%lld",size);
		PRINT_LOG(sss);
		inSample1 = size / sizeof (SSIZE);
		if (inSample1 == 0) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}

		// Front Right
		_makepath(outFile,workdrive,workdir,workfname,"r2");
		//fio_open(&fp_r,outFile,FIO_MODE_R);
		fio_open_with_set_param(&fp_r,outFile,FIO_MODE_R,&fio_common);
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		//fio_set_memory_limit(&fp_r,20,param->p->fio);
		//fio_set_memory_limit(&fp_r,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);

		inSample2 = 0;
		// ファイルサイズ取得
		fio_get_filesize(&fp_r,&size);
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		sprintf(sss,"first r2 size:%lld",size);
		PRINT_LOG(sss);
		inSample2 = size / sizeof (SSIZE);
		if (inSample2 == 0) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}

		if (param->p->chC == 1 && param->p->LRC_process == 0) {
			// Create Center
			strcpy(param->pr_str,"[GenC]");
			fprintf(stdout,"%s0%%\n",param->pr_str);
			fflush(stdout);

			memset(&NormInfo,0,sizeof (NORM_INFO));
			_makepath(outFile,workdrive,workdir,workfname,"r3");
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			fio_set_maxsize(&fp_w,size);

			// Center を抽出(周波数軸を元に共通の音を抜き出し)
			param->dis_downsample = 1;
			fftFilter(3,inSample1,inSample2,&fp_l,&fp_r,&fp_w,param);
			if (param->err) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			fio_close(&fp_w);
		}

		if (param->p->chS == 1) {
			// Create surround Left
			strcpy(param->pr_str,"[GenSLR]");
			fprintf(stdout,"%s0%%\n",param->pr_str);
			fflush(stdout);

			_makepath(outFile,workdrive,workdir,workfname,"r4");
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			fio_set_maxsize(&fp_w,size);

			convolutionFile(inSample1,&fp_l,&fp_r,&fp_w,param);
			if (param->err) {
				break;
			}
			fio_close(&fp_w);

			_makepath(outFile,workdrive,workdir,workfname,"r5");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			fio_set_maxsize(&fp_w,size);

			convolutionFile(inSample1,&fp_r,&fp_l,&fp_w,param);
			if (param->err) {
				break;
			}
			fio_close(&fp_w);
		}

		if (param->p->chLFE == 1) {
			// Create LFE
			strcpy(param->pr_str,"[GenLFE]");
			fprintf(stdout,"%s0%%\n",param->pr_str);
			fflush(stdout);

			_makepath(outFile,workdrive,workdir,workfname,"r6");
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			fio_set_maxsize(&fp_w,size);
			param->dis_downsample = 1;
			fftFilter(6,inSample1,inSample2,&fp_l,&fp_r,&fp_w,param);
			if (param->err) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			fio_close(&fp_w);
		}

		if (param->p->mc_downmix) {
			if (param->p->chC == 1) {
				strcpy(param->pr_str,"[Post MC(DM)]");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);

				_makepath(outFile,workdrive,workdir,workfname,"r3");
				//fio_open(&fp_tmp,outFile,FIO_MODE_R);
				fio_common.upconv_n    = 3;
				fio_open_with_set_param(&fp_tmp,outFile,FIO_MODE_R,&fio_common);
				if (fp_tmp.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				_makepath(outFile,workdrive,workdir,workfname,"r1.tmp2");
				fio_common.upconv_n    = 3;
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				// ステレオにダウンミックス
				merageTempFile('+',0,&fp_l,&fp_tmp,&fp_w,inSample1,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_l);
				_makepath(outFile,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w,&fp_l,outFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
					break;
				}
				if (fp_l.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}

				_makepath(outFile,workdrive,workdir,workfname,"r2.tmp2");
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				// ステレオにダウンミックス
				merageTempFile('+',0,&fp_r,&fp_tmp,&fp_w,inSample2,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				_makepath(outFile,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w,&fp_r,outFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				fio_close(&fp_tmp);
				_makepath(outFile,workdrive,workdir,workfname,"r3");
				unlink(outFile);
				param->p->chC = 0;
			}
			if (param->p->chS == 1) {
				_makepath(outFile,workdrive,workdir,workfname,"r4");
				fio_common.upconv_n    = 3;
				//fio_open(&fp_tmp,outFile,FIO_MODE_R);
				fio_open_with_set_param(&fp_tmp,outFile,FIO_MODE_R,&fio_common);
				if (fp_tmp.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				_makepath(outFile,workdrive,workdir,workfname,"r1.tmp2");
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				// ステレオにダウンミックス
				merageTempFile('+',0,&fp_l,&fp_tmp,&fp_w,inSample1,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_l);
				_makepath(outFile,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w,&fp_l,outFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
					break;
				}
				if (fp_l.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				fio_close(&fp_tmp);
				_makepath(outFile,workdrive,workdir,workfname,"r5");
				//fio_open(&fp_tmp,outFile,FIO_MODE_R);
				fio_open_with_set_param(&fp_tmp,outFile,FIO_MODE_R,&fio_common);
				if (fp_tmp.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				_makepath(outFile,workdrive,workdir,workfname,"r2.tmp2");
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				// ステレオにダウンミックス
				merageTempFile('+',0,&fp_r,&fp_tmp,&fp_w,inSample2,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				_makepath(outFile,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w,&fp_r,outFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
					break;
				}
				fio_close(&fp_tmp);
				_makepath(outFile,workdrive,workdir,workfname,"r4");
				unlink(outFile);
				_makepath(outFile,workdrive,workdir,workfname,"r5");
				unlink(outFile);
				param->p->chS = 0;
			}
			param->p->chLFE = 0;
		}
		PRINT_LOG("");
		strcpy(param->pr_str,"[Post MC]");
		fprintf(stdout,"%s0%%\n",param->pr_str);
		fflush(stdout);

		// Left を本来の出力サンプリングレートにする。
		memset(&NormInfo,0,sizeof (NORM_INFO));
		_makepath(outFile,workdrive,workdir,workfname,"r1.tmp");
		//fio_open(&fp_w,outFile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		//fio_set_memory_limit(&fp_w,20,param->p->fio);
		//fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);
		// サンプリングレートを1/2にして出力
		fftFilter(1,inSample1,inSample2,&fp_l,NULL,&fp_w,param);
		if (param->err) {
			break;
		}
		sprintf(sss,"fp_w.data_size:%lld",fp_w.data_size);
		PRINT_LOG(sss);
		sprintf(sss,"fp_w.data_offset:%lld",fp_w.data_offset);
		PRINT_LOG(sss);
		sprintf(sss,"fp_w.data_maxsize:%lld",fp_w.data_maxsize);
		PRINT_LOG(sss);
		fio_close(&fp_l);
		_makepath(outFile,workdrive,workdir,workfname,"r1");
		fio_setmode_r(&fp_w,&fp_l,outFile);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		if (fp_l.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		merageTempFile(' ',1,&fp_l,NULL,NULL,inSample1 / 2,param);
		sprintf(sss,"merageTempFile:%d,line:%d",param->err,param->error_line);
		PRINT_LOG(sss);
		if (param->err) {
			break;
		}
		if (NormInfo.max < 0) {
			NormInfo.max *= -1;
		}
		if (NormInfo.min < 0) {
			NormInfo.min *= -1;
		}
		max = NormInfo.max;
		if (max < NormInfo.min) {
			max = NormInfo.min;
		}
		persent = (double)max / (double)0x7FFFFFFFFFFFFF;
		param_nx = (param->param_nx[0].p1 / persent);

		_makepath(outFile,workdrive,workdir,workfname,"r1.tmp");
		//fio_open(&fp_w,outFile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		//fio_set_memory_limit(&fp_w,20,param->p->fio);
		//fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);
		volumeAdjustFile(param_nx,&fp_l,&fp_w,inSample1 / 2,param,NULL);
		fio_close(&fp_l);
		_makepath(outFile,workdrive,workdir,workfname,"r1");
		fio_setmode_r(&fp_w,&fp_l,outFile);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		if (fp_l.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		fio_close(&fp_l);

		PRINT_LOG("");

		// Right を本来の出力サンプリングレートにする。
		memset(&NormInfo,0,sizeof (NORM_INFO));
		_makepath(outFile,workdrive,workdir,workfname,"r2.tmp");
		//fio_open(&fp_w,outFile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		//fio_set_memory_limit(&fp_w,20,param->p->fio);
		//fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);
		// サンプリングレートを1/2にして出力
		fftFilter(2,inSample1,inSample2,NULL,&fp_r,&fp_w,param);
		if (param->err) {
			break;
		}
		fio_close(&fp_r);
		_makepath(outFile,workdrive,workdir,workfname,"r2");
		fio_setmode_r(&fp_w,&fp_r,outFile);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		merageTempFile(' ',1,&fp_r,NULL,NULL,inSample2 / 2,param);
		if (param->err) {
			break;
		}
		if (NormInfo.max < 0) {
			NormInfo.max *= -1;
		}
		if (NormInfo.min < 0) {
			NormInfo.min *= -1;
		}
		max = NormInfo.max;
		if (max < NormInfo.min) {
			max = NormInfo.min;
		}
		persent = (double)max / (double)0x7FFFFFFFFFFFFF;
		param_nx = (param->param_nx[1].p1 / persent);
		_makepath(outFile,workdrive,workdir,workfname,"r2.tmp");
		//fio_open(&fp_w,outFile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		//fio_set_memory_limit(&fp_w,20,param->p->fio);
		//fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);

		// 音量調整
		volumeAdjustFile(param_nx,&fp_r,&fp_w,inSample2 / 2,param,NULL);
		fio_close(&fp_r);
		_makepath(outFile,workdrive,workdir,workfname,"r2");
		fio_setmode_r(&fp_w,&fp_r,outFile);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		if (fp_l.error) {
			param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
			break;
		}
		fio_close(&fp_r);

		PRINT_LOG("");
		if (param->p->chC == 1) {
			// Center を本来の出力サンプリングレートにする。
			memset(&NormInfo,0,sizeof (NORM_INFO));
			_makepath(outFile,workdrive,workdir,workfname,"r3");
			//fio_open(&fp_tmp,outFile,FIO_MODE_R);
			fio_open_with_set_param(&fp_tmp,outFile,FIO_MODE_R,&fio_common);
			if (fp_tmp.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			_makepath(outFile,workdrive,workdir,workfname,"r3.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);
			// サンプリングレートを1/2にして出力
			fftFilter(1,inSample1,inSample2,&fp_tmp,NULL,&fp_w,param);
			if (param->err) {
				break;
			}
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r3");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			merageTempFile(' ',1,&fp_tmp,NULL,NULL,inSample1 / 2,param);
			if (param->err) {
				break;
			}
			if (NormInfo.max < 0) {
				NormInfo.max *= -1;
			}
			if (NormInfo.min < 0) {
				NormInfo.min *= -1;
			}
			max = NormInfo.max;
			if (max < NormInfo.min) {
				max = NormInfo.min;
			}
			persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			param_nx = (param->param_nx[2].p1 / persent);
			_makepath(outFile,workdrive,workdir,workfname,"r3.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);
			// 音量調整
			volumeAdjustFile(param_nx,&fp_tmp,&fp_w,inSample1 / 2,param,NULL);
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r3");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			fio_close(&fp_tmp);
		}
		PRINT_LOG("");
		if (param->p->chS == 1) {
			// Surround Left/Right を本来の出力サンプリングレートにする。
			memset(&NormInfo,0,sizeof (NORM_INFO));
			_makepath(outFile,workdrive,workdir,workfname,"r4");
			//fio_open(&fp_tmp,outFile,FIO_MODE_R);
			fio_open_with_set_param(&fp_tmp,outFile,FIO_MODE_R,&fio_common);
			if (fp_tmp.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			_makepath(outFile,workdrive,workdir,workfname,"r4.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);
			// サンプリングレートを1/2にして出力
			fftFilter(1,inSample1,inSample2,&fp_tmp,NULL,&fp_w,param);
			if (param->err) {
				break;
			}
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r4");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			merageTempFile(' ',1,&fp_tmp,NULL,NULL,inSample1 / 2,param);
			if (param->err) {
				break;
			}
			if (NormInfo.max < 0) {
				NormInfo.max *= -1;
			}
			if (NormInfo.min < 0) {
				NormInfo.min *= -1;
			}
			max = NormInfo.max;
			if (max < NormInfo.min) {
				max = NormInfo.min;
			}
			persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			param_nx = (param->param_nx[3].p1 / persent);
			_makepath(outFile,workdrive,workdir,workfname,"r4.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_memory_limit(&fp_w,param->mc_inSampleR * sizeof (SSIZE),param->p->fio);
			// 音量調整
			volumeAdjustFile(param_nx,&fp_tmp,&fp_w,inSample1 / 2,param,NULL);
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r4");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			fio_close(&fp_tmp);

			// Surround Left/Right を本来の出力サンプリングレートにする。
			memset(&NormInfo,0,sizeof (NORM_INFO));
			_makepath(outFile,workdrive,workdir,workfname,"r5");
			//fio_open(&fp_tmp,outFile,FIO_MODE_R);
			fio_open_with_set_param(&fp_tmp,outFile,FIO_MODE_R,&fio_common);
			if (fp_tmp.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			_makepath(outFile,workdrive,workdir,workfname,"r5.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,(param->mc_inSampleR * sizeof (SSIZE)) / 2,param->p->fio);
			// サンプリングレートを1/2にして出力
			fftFilter(2,inSample1,inSample2,NULL,&fp_tmp,&fp_w,param);
			if (param->err) {
				break;
			}
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r5");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			merageTempFile(' ',1,&fp_tmp,NULL,NULL,inSample2 / 2,param);
			if (param->err) {
				break;
			}
			if (NormInfo.max < 0) {
				NormInfo.max *= -1;
			}
			if (NormInfo.min < 0) {
				NormInfo.min *= -1;
			}
			max = NormInfo.max;
			if (max < NormInfo.min) {
				max = NormInfo.min;
			}
			persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			param_nx = (param->param_nx[4].p1 / persent);
			_makepath(outFile,workdrive,workdir,workfname,"r5.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,(param->mc_inSampleR * sizeof (SSIZE)) / 2,param->p->fio);
			// 音量調整
			volumeAdjustFile(param_nx,&fp_tmp,&fp_w,inSample1 / 2,param,NULL);
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r5");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			fio_close(&fp_tmp);
		}
		PRINT_LOG("");
		if (param->p->chLFE == 1) {
			// LFE を本来の出力サンプリングレートにする。
			memset(&NormInfo,0,sizeof (NORM_INFO));
			_makepath(outFile,workdrive,workdir,workfname,"r6");
			//fio_open(&fp_tmp,outFile,FIO_MODE_R);
			fio_open_with_set_param(&fp_tmp,outFile,FIO_MODE_R,&fio_common);
			if (fp_tmp.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			_makepath(outFile,workdrive,workdir,workfname,"r6.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,(param->mc_inSampleR * sizeof (SSIZE)) / 2,param->p->fio);
			// サンプリングレートを1/2にして出力
			fftFilter(1,inSample1,inSample2,&fp_tmp,NULL,&fp_w,param);
			if (param->err) {
				break;
			}
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r6");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			merageTempFile(' ',1,&fp_tmp,NULL,NULL,inSample1 / 2,param);
			if (param->err) {
				break;
			}
			if (NormInfo.max < 0) {
				NormInfo.max *= -1;
			}
			if (NormInfo.min < 0) {
				NormInfo.min *= -1;
			}
			max = NormInfo.max;
			if (max < NormInfo.min) {
				max = NormInfo.min;
			}
			persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			param_nx = (param->param_nx[5].p1 / persent);
			_makepath(outFile,workdrive,workdir,workfname,"r6.tmp");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,(param->mc_inSampleR * sizeof (SSIZE)) / 2,param->p->fio);
			PRINT_LOG("before volumeAdjustFile");
			// 音量調整
			volumeAdjustFile(param_nx,&fp_tmp,&fp_w,inSample1 / 2,param,NULL);
			PRINT_LOG("after volumeAdjustFile");
			fio_close(&fp_tmp);
			_makepath(outFile,workdrive,workdir,workfname,"r6");
			fio_setmode_r(&fp_w,&fp_tmp,outFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
				break;
			}
			if (fp_tmp.error) {
				param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
				break;
			}
			fio_close(&fp_tmp);
		}
	} while (0);
}
//---------------------------------------------------------------------------
// Function   : convolutionFile
// Description: 残響音生成(Surround Left/Right)
// ---
//	inSample :サンプル数
//	fio_in1  :入力ファイル1
//	fio_in2	 :入力ファイル2
//	fio_out  :出力ファイル
//	param	 :パラメーター
//
void convolutionFile(SSIZE inSample,FIO *fio_in1,FIO *fio_in2,FIO *fio_out,PARAM_INFO *param)
{
	SSIZE *pIn1,*pOut,*pIn;
	SSIZE startInSample,rdStart,rdEnd,rdSample;
	fio_size nSample,wr;
	int delay;
	SSIZE max_delay;
	int i,j,n;
	double per,dis,pw;
	double percent;
	char s[100];
	fio_rewind(fio_in1);
	fio_rewind(fio_in2);

	// 3 秒分のサンプルを保存するためのバッファを用意する。
	pIn1 = (SSIZE *)al_malloc(param->mc_inSampleR * sizeof (SSIZE) * 3);
	if (pIn1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->error_line = __LINE__;
		return;
	}
	// 1 秒分のサンプルを保存するためのバッファを用意する。
	pOut = (SSIZE *)al_malloc(param->mc_inSampleR * sizeof (SSIZE));
	if (pOut == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->error_line = __LINE__;
		return;
	}

	per = -1;
	for (startInSample = 0;startInSample < inSample;startInSample += param->mc_inSampleR) {
		rdStart = rdEnd = -1;
		// ゼロクリア
		memset(pOut,0,param->mc_inSampleR * sizeof (SSIZE));

		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			if (percent != per) {
//				Sleep(1);
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) break;

				fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
				fflush(stdout);
			}
			per = percent;
		}

		// L + Lの残響、またはR + Rの残響
		for (n = 0;;n++) {
			// ゼロクリア
			memset(pIn1,0,param->mc_inSampleR * sizeof (SSIZE));

			// 残響特性のパラメーター計算。
			// delay には何サンプル遅れて音が到達するか。
			// per は遅れて到達した音の強さ。
			if (DelayParam2[n].m == 0) {
				break;
			}
			delay = (long)(((double)1000 / 330) * DelayParam2[n].m);	// 1メートル進むのにx ms。x ms × ディレイ(メートル) = 残響音が x ms 遅れて音が到達
			delay = (param->mc_outSampleR / 1000) * delay;					// 残響音の最初のサンプルが、何サンプル遅れて到達するか。
			dis   = (double)DelayParam2[n].m / DelayParam2[0].m;
			if (DelayParam2[n].n > 0) {
				pw	 = DelayParam2[n].n;									// 残響音の強さ
			} else {
				if (param->adjust == 0) {
					pw	 = DelayParam2[0].n / (dis * dis);						// 残響音の強さ
				} else if (param->adjust == 1) {
					pw	 = DelayParam2[0].n / ((dis * dis) / 3);
				} else if (param->adjust == 2) {
					pw	 = DelayParam2[0].n / (dis * 2);
				} else if (param->adjust == 3) {
					pw	 = DelayParam2[0].n / (dis);
				} else if (param->adjust == 4) {
					pw	 = DelayParam2[0].n / (dis / 8);
				}
			}
			rdSample = startInSample - delay;
			if (rdSample >= 0 && rdSample + (param->mc_inSampleR) < inSample && startInSample > param->mc_inSampleR * 3) {
				nSample = param->mc_inSampleR;
				if (rdStart != -1 && rdStart <= rdSample && (rdSample + param->mc_inSampleR) < rdEnd) {
					// バッファへすでに読み込んでいるので読まない
					sprintf(s,"[%d] CACHE",n);
					PRINT_LOG(s);
				} else {
					rdStart = rdSample - (param->mc_inSampleR * 2);
					rdEnd	= rdStart + (param->mc_inSampleR * 3);
					if (rdStart >= 0 && rdEnd < inSample) {
						sprintf(s,"[%d] 1:buffer3",n);
						PRINT_LOG(s);
						fio_seek(fio_in1,rdStart * sizeof (SSIZE),SEEK_SET);
						fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in1);
					} else {
						rdStart = rdSample - (param->mc_inSampleR * 1);
						rdEnd	= rdStart + (param->mc_inSampleR * 2);
						if (rdStart >= 0 && rdEnd < inSample) {
							sprintf(s,"[%d] 1:buffer2",n);
							PRINT_LOG(s);
							fio_seek(fio_in1,rdStart * sizeof (SSIZE),SEEK_SET);
							fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in1);
						} else {
							rdStart = rdSample;
							rdEnd	= param->mc_inSampleR;
							if (rdStart >= 0 && rdEnd < inSample) {
								sprintf(s,"[%d] 1:buffer1",n);
								PRINT_LOG(s);
								fio_seek(fio_in1,rdStart * sizeof (SSIZE),SEEK_SET);
								fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in1);
							} else {
								param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
								return;
							}
						}
					}
				}
			} else {
				PRINT_LOG("not cache");
				pIn = pIn1;
				nSample = param->mc_inSampleR;
				if (rdSample >= 0) {
					rdStart = rdSample;
					rdEnd	= rdStart + nSample;
					fio_seek(fio_in1,rdSample * sizeof (SSIZE),SEEK_SET);
				} else {
					rdStart = rdSample;
					fio_seek(fio_in1,0,SEEK_SET);
					pIn += (rdSample * -1);
					if (nSample > rdSample * -1) {
						nSample -= rdSample * -1;
					} else {
						nSample = 0;
					}
					rdEnd = rdStart + nSample;
				}

				if (rdSample >= inSample) {
					nSample = 0;
				} else {
					if (nSample != 0) {
						if (nSample > inSample - rdSample) {
							nSample = inSample - rdSample;
						}
					}
				}
				if (nSample > 0) {
					fio_read(pIn,sizeof (SSIZE),nSample,fio_in1);
				}
				nSample = param->mc_inSampleR;
			}

			for (i = 0;i < nSample;i++) {
				pOut[i] += (SSIZE)((double)pIn1[(rdSample - rdStart) + i] * pw);
			}
		}

		// L + Rの残響、またはR + Lの残響
		rdStart = rdEnd = -1;
		for (n = 0;;n++) {
			// ゼロクリア
			memset(pIn1,0,param->mc_inSampleR * sizeof (SSIZE));

			// 残響特性のパラメーター計算。
			// delay には何サンプル遅れて音が到達するか。
			// per は遅れて到達した音の強さ。
			if (DelayParam3[n].m == 0) {
				break;
			}
			delay = (long)(((double)1000 / 330) * DelayParam3[n].m);	// 1メートル進むのにx ms。x ms × ディレイ(メートル) = 残響音が x ms 遅れて音が到達
			delay = (param->mc_outSampleR / 1000) * delay;					// 残響音の最初のサンプルが、何サンプル遅れて到達するか。
			dis   = (double)DelayParam3[n].m / DelayParam3[0].m;
			if (DelayParam3[n].n > 0) {
				pw = DelayParam3[n].n;
			} else {
				if (param->adjust == 0) {
					pw	 = DelayParam3[0].n / (dis * dis);						// 残響音の強さ
				} else if (param->adjust == 1) {
					pw	 = DelayParam3[0].n / ((dis * dis) * 3);
				} else if (param->adjust == 2) {
					pw	 = DelayParam3[0].n / (dis * 2);
				} else if (param->adjust == 3) {
					pw	 = DelayParam3[0].n / (dis);
				} else if (param->adjust == 4) {
					pw	 = DelayParam3[0].n / (dis / 8);
				}
			}

			rdSample = startInSample - delay;
			if (rdSample >= 0 && rdSample + (param->mc_inSampleR) < inSample && startInSample > param->mc_inSampleR * 3) {
				nSample = param->mc_inSampleR;
				if (rdStart != -1 && rdStart <= rdSample && (rdSample + param->mc_inSampleR) < rdEnd) {
					sprintf(s,"[%d] CACHE",n);
					PRINT_LOG(s);
					// バッファへすでに読み込んでいるので読まない
				} else {
					rdStart = rdSample - (param->mc_inSampleR * 2);
					rdEnd	= rdStart + (param->mc_inSampleR * 3);
					if (rdStart >= 0 && rdEnd < inSample) {
						sprintf(s,"[%d] 1:buffer3",n);
						PRINT_LOG(s);
						fio_seek(fio_in2,rdStart * sizeof (SSIZE),SEEK_SET);
						fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in2);
					} else {
						rdStart = rdSample - (param->mc_inSampleR * 1);
						rdEnd	= rdStart + (param->mc_inSampleR * 2);
						if (rdStart >= 0 && rdEnd < inSample) {
							sprintf(s,"[%d] 1:buffer2",n);
							PRINT_LOG(s);
							fio_seek(fio_in2,rdStart * sizeof (SSIZE),SEEK_SET);
							fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in2);
						} else {
							rdStart = rdSample;
							rdEnd	= param->mc_inSampleR;
							if (rdStart >= 0 && rdEnd < inSample) {
								sprintf(s,"[%d] 1:buffer1",n);
								PRINT_LOG(s);
								fio_seek(fio_in2,rdStart * sizeof (SSIZE),SEEK_SET);
								fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in2);
							} else {
								param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
								return;
							}
						}
					}
				}
			} else {
				PRINT_LOG("2:not cache");
				pIn = pIn1;
				nSample = param->mc_inSampleR;
				if (rdSample >= 0) {
					rdStart = rdSample;
					rdEnd	= rdStart + nSample;
					fio_seek(fio_in2,rdSample * sizeof (SSIZE),SEEK_SET);
				} else {
					rdStart = rdSample;
					fio_seek(fio_in2,0,SEEK_SET);
					pIn += (rdSample * -1);
					if (nSample > rdSample * -1) {
						nSample -= rdSample * -1;
					} else {
						nSample = 0;
					}
					rdEnd = rdStart + nSample;
				}

				if (rdSample >= inSample) {
					nSample = 0;
				} else {
					if (nSample != 0) {
						if (nSample > inSample - rdSample) {
							nSample = inSample - rdSample;
						}
					}
				}
				if (nSample > 0) {
					fio_read(pIn,sizeof (SSIZE),nSample,fio_in2);
				}
				nSample = param->mc_inSampleR;
			}

			for (i = 0;i < nSample;i++) {
				pOut[i] += (SSIZE)((double)pIn1[(rdSample - rdStart) + i] * pw);
			}
		}
		if (param->echo == 1) {
			rdStart = rdEnd = -1;
			for (n = 0;;n++) {
				// ゼロクリア
				memset(pIn1,0,param->mc_inSampleR * sizeof (SSIZE));

				// 残響特性のパラメーター計算。
				// delay には何サンプル遅れて音が到達するか。
				// per は遅れて到達した音の強さ。
				if (DelayParam4[n].m == 0) {
					break;
				}
				delay = (long)(((double)1000 / 330) * DelayParam4[n].m);	// 1メートル進むのにx ms。x ms × ディレイ(メートル) = 残響音が x ms 遅れて音が到達
				delay = (param->mc_outSampleR / 1000) * delay;					// 残響音の最初のサンプルが、何サンプル遅れて到達するか。
				dis   = (double)DelayParam4[n].m / DelayParam4[0].m;
				if (DelayParam4[n].n > 0) {
					pw = DelayParam4[n].n;
				} else {
					pw = DelayParam4[0].n / (dis * dis);						// 残響音の強さ
				}
				rdSample = startInSample - delay;
				if (rdSample >= 0 && rdSample + (param->mc_inSampleR) < inSample && startInSample > param->mc_inSampleR * 3) {
					nSample = param->mc_inSampleR;
					if (rdStart != -1 && rdStart <= rdSample && (rdSample + param->mc_inSampleR) < rdEnd) {
						sprintf(s,"[%d] CACHE",n);
						PRINT_LOG(s);
						// バッファへすでに読み込んでいるので読まない
					} else {
						rdStart = rdSample - (param->mc_inSampleR * 2);
						rdEnd	= rdStart + (param->mc_inSampleR * 3);
						if (rdStart >= 0 && rdEnd < inSample) {
							sprintf(s,"[%d] 1:buffer3",n);
							PRINT_LOG(s);
							fio_seek(fio_in2,rdStart * sizeof (SSIZE),SEEK_SET);
							fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in2);
						} else {
							rdStart = rdSample - (param->mc_inSampleR * 1);
							rdEnd	= rdStart + (param->mc_inSampleR * 2);
							if (rdStart >= 0 && rdEnd < inSample) {
								sprintf(s,"[%d] 1:buffer2",n);
								PRINT_LOG(s);
								fio_seek(fio_in2,rdStart * sizeof (SSIZE),SEEK_SET);
								fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in2);
							} else {
								rdStart = rdSample;
								rdEnd	= param->mc_inSampleR;
								if (rdStart >= 0 && rdEnd < inSample) {
									sprintf(s,"[%d] 1:buffer1",n);
									PRINT_LOG(s);
									fio_seek(fio_in2,rdStart * sizeof (SSIZE),SEEK_SET);
									fio_read(pIn1,sizeof (SSIZE),rdEnd - rdStart,fio_in2);
								} else {
									param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
									return;
								}
							}
						}
					}
				} else {
					PRINT_LOG("2:not cache");
					pIn = pIn1;
					nSample = param->mc_inSampleR;
					if (rdSample >= 0) {
						rdStart = rdSample;
						rdEnd	= rdStart + nSample;
						fio_seek(fio_in2,rdSample * sizeof (SSIZE),SEEK_SET);
					} else {
						rdStart = rdSample;
						fio_seek(fio_in2,0,SEEK_SET);
						pIn += (rdSample * -1);
						if (nSample > rdSample * -1) {
							nSample -= rdSample * -1;
						} else {
							nSample = 0;
						}
						rdEnd = rdStart + nSample;
					}

					if (rdSample >= inSample) {
						nSample = 0;
					} else {
						if (nSample != 0) {
							if (nSample > inSample - rdSample) {
								nSample = inSample - rdSample;
							}
						}
					}
					if (nSample > 0) {
						fio_read(pIn,sizeof (SSIZE),nSample,fio_in2);
					}
					nSample = param->mc_inSampleR;
				}

				for (i = 0;i < nSample;i++) {
					pOut[i] += (SSIZE)((double)pIn1[(rdSample - rdStart) + i] * pw);
				}
			}
		}
		wr = fio_write(pOut,sizeof (SSIZE),nSample,fio_out);
		if (wr != nSample) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
	}

	al_free(pIn1);
	al_free(pOut);

}
#if 0
//---------------------------------------------------------------------------
// Function   : merageTempFile
// Description: 出力結果のファイルをマージする
// ---
//	type	 :マージの種類
//	normFlag :ノーマライズ用変数更新フラグ
//	in1 	 :入力ファイル1
//	in2		 :入力ファイル2
//	out		 :出力ファイル
//	inSample :サンプル数
//	type	 :残響タイプ
//	count	 :回数
//
void merageTempFile(char type,int normFlag,FILE *in1,FILE *in2,DWORD inSample,int dtype,int count,PARAM_INFO *param)
{
	SSIZE v1,v2,v3;
	SSIZE min,max;
	DWORD remainSample;
	long i,remain1,remain2;
	SSIZE pos,pos2,pos3;
	int ret;
	int delay;
	int persent;
	int n;
	double per;
	double dis;
	rewind(in1);
	rewind(in2);
	
	if (count == 1) {
		count = 0;
	}

	ret = 0;
	min = max = 0;
	remainSample = inSample;
	if (dtype == 1) {
		delay = (1000 / 330) * DelayParam1[0].m;
		delay = (param->mc_outSampleR / 1000) * delay;
		per   = DelayParam1[0].n;
		for (i = 0;i < delay;i++) {
			diskBuffer1[i] = 0;
		}
		if (fwrite(diskBuffer1,sizeof (SSIZE),delay,in1) != delay) {
			param->error_line = __LINE__;
			param->err =	STATUS_FILE_WRITE_ERR;
			return;
		}
		remainSample -= delay;
	} else if (dtype == 2) {
		delay = (1000 / 330) * DelayParam2[0].m;
		delay = (param->mc_outSampleR / 1000) * delay;
		per   = DelayParam1[0].n;
	} else if (dtype == 3) {
		delay = (1000 / 330) * DelayParam3[0].m;
		delay = (param->mc_outSampleR / 1000) * delay;
		per   = DelayParam1[0].n;
	} else if (dtype == 4) {
		delay = (1000 / 330) * DelayParam4[0].m;
		delay = (param->mc_outSampleR / 1000) * delay;
		per   = DelayParam1[0].n;
		for (i = 0;i < delay;i++) {
			diskBuffer1[i] = 0;
		}
		if (fwrite(diskBuffer1,sizeof (SSIZE),delay,in1) != delay) {
			param->error_line = __LINE__;
			param->err =	STATUS_FILE_WRITE_ERR;
			return;
		}
		remainSample -= delay;
	}

	do {
		pos = ftello64(in1);
		fseeko64(in1,0,SEEK_CUR);

		if (type == '+') {
			//Sleep(1);
			remain1 = fread(diskBuffer1,sizeof (SSIZE),4 * 1024 * 1024,in1);
			remain2 = fread(diskBuffer2,sizeof (SSIZE),4 * 1024 * 1024,in2);
			if (remain1 == 0) {
				break;
			}
			for (i = 0;i < remain1;i++) {
				if (diskBuffer1[i] != 0) {
					if (dtype != 0) {
						diskBuffer1[i] *= 0.85;
						diskBuffer2[i] *= 0.85;
					}
					diskBuffer1[i] += (diskBuffer2[i] * 0.5);
					if (diskBuffer1[i] < min) {
						min = diskBuffer1[i];
					}
					if (diskBuffer1[i] > max) {
						max = diskBuffer1[i];
					}
				}
			}
			fseeko64(in1,pos,SEEK_SET);
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fwrite(diskBuffer1,sizeof (SSIZE),remain1,in1) != remain1) {
				param->error_line = __LINE__;
				param->err =	STATUS_FILE_WRITE_ERR;
				return;
			}
			remainSample -= remain1;
		} else if (type == '-') {
			//Sleep(1);
			remain1 = fread(diskBuffer1,sizeof (SSIZE),4 * 1024 * 1024,in1);
			remain2 = fread(diskBuffer2,sizeof (SSIZE),4 * 1024 * 1024,in2);
			if (remain1 == 0) {
				break;
			}
			for (i = 0;i < remain1;i++) {
				if (diskBuffer1[i] != 0) {
					diskBuffer1[i] -= diskBuffer2[i];
					if (diskBuffer1[i] < min) {
						min = diskBuffer1[i];
					}
					if (diskBuffer1[i] > max) {
						max = diskBuffer1[i];
					}
				}
			}
			fseeko64(in1,pos,SEEK_SET);
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fwrite(diskBuffer1,sizeof (SSIZE),remain1,in1) != remain1) {
				param->error_line = __LINE__;
				param->err =	STATUS_FILE_WRITE_ERR;
				return;
			}
			remainSample -= remain1;
		} else if (type == ' ') {
			//Sleep(1);
			remain2 = fread(diskBuffer2,sizeof (SSIZE),4 * 1024 * 1024,in2);
			if (remain2 == 0) {
				break;
			}
			for (i = 0;i < remain2;i++) {
				if (diskBuffer2[i] < min) {
					min = diskBuffer2[i];
				}
				if (diskBuffer2[i] > max) {
					max = diskBuffer2[i];
				}
			}
			fseeko64(in1,pos,SEEK_SET);
			remain2 =  remain2 < remainSample ? remain2 : remainSample;
			if (fwrite(diskBuffer2,sizeof (SSIZE),remain2,in1) != remain2) {
				param->error_line = __LINE__;
				param->err =	STATUS_FILE_WRITE_ERR;
				return;
			}
			remainSample -= remain2;
			remain1 = remain2;
		}
	} while (remain1 == 4 * 1024 * 1024 && remainSample > 0);

	fflush(in1);

	if (count) {
		rewind(in1);

		for (n = 1;n < count;n++) {
			pos = 0;
			if (dtype == 2) {
				delay = (1000 / 330) * DelayParam2[n].m;
				delay = (param->mc_outSampleR / 1000) * delay;
				dis   = DelayParam2[n].m / DelayParam2[0].m;
				per   = DelayParam2[0].n / (dis * dis);
			} else if (dtype == 3) {
				delay = (1000 / 330) * DelayParam3[n].m;
				delay = (param->mc_outSampleR / 1000) * delay;
				dis   = DelayParam3[n].m / DelayParam2[0].m;
				per   = DelayParam3[0].n / (dis * dis);
			}
			remainSample = inSample;
			do {
				fseeko64(in1,pos,SEEK_SET);
				remain1 = fread(diskBuffer1,sizeof (SSIZE),4 * 1024 * 1024,in1);
				if (remain1 == 0) {
					break;
				}
				pos2 = ftello64(in1);
				fseeko64(in1,pos + (delay * sizeof (SSIZE)),SEEK_SET);
				pos3 = ftello64(in1);
				remain2 = fread(diskBuffer2,sizeof (SSIZE),4 * 1024 * 1024,in1);
				for (i = 0;i < remain2;i++) {
					diskBuffer1[i] *= 0.94;
					diskBuffer2[i] *= 0.94;
					diskBuffer2[i] += (diskBuffer1[i] * per);
					if (diskBuffer2[i] < min) {
						min = diskBuffer2[i];
					}
					if (diskBuffer2[i] > max) {
						max = diskBuffer2[i];
					}
				}
				fseeko64(in1,pos3,SEEK_SET);
				if (fwrite(diskBuffer2,sizeof (SSIZE),remain2,in1) != remain2) {
					param->error_line = __LINE__;
					param->err =	STATUS_FILE_WRITE_ERR;
					return;
				}
				remainSample -= remain1;
				pos = pos2;
			} while (remain1 == 4 * 1024 * 1024 && remainSample > 0);
		}
	}

	if (normFlag == 1) {
		if (max > NormInfo.max) {
			NormInfo.max = max;
		}
		if (min < NormInfo.min) {
			NormInfo.min = min;
		}
	}

}
#endif
//---------------------------------------------------------------------------
// Function   : fftFilter
// Description: FFT によるフィルタ処理
// ---
//	type		:変換タイプ
//	inSample1	:入力データのサンプル数
//	inSample2	:入力データのサンプル数
//	fio1		:入力ファイル用構造体
//	fio2		:入力ファイル用構造体
//	fio3		:出力ファイル用構造体
//	param		:変換パラメータ
//
static void fftFilter(int type,SSIZE inSample1,SSIZE inSample2,FIO *fio1,FIO *fio2,FIO *fio3,PARAM_INFO *param)
{
	FIO *fio;
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4;
	SSIZE *c_mem0,*c_mem1,*c_mem2,*c_mem3,*c_mem4;
	char *inPtr;
	long wkMemSize;
	long inSampleR;
	long outSampleR;
	SSIZE fftSizeIn,fftSizeOut,i,j,n;
	long cutOff;
	long hfc;
	SSIZE wr;
	long pwCnt;
	long hz;
	long margin;
	double percent,per;
	double nx;
	double *pwBase,basePw;
	SSIZE *pIn[3],*pOut[3];
	SSIZE *c_pIn[3],*c_pOut[3];
	SSIZE startInSample,inSampLen,outSampLen,nSample;
	SSIZE inSample;
	SSIZE outRemain;
	SSIZE min,max;
	fftw_complex *fftw_in[3],*fftw_out[3];
	fftw_plan fftw_p[3],fftw_ip[3];

	if (fio1 != NULL) fio_rewind(fio1);
	if (fio2 != NULL) fio_rewind(fio2);

	inSampleR  = param->mc_inSampleR;
	outSampleR = param->mc_outSampleR;
	if (param->dis_downsample) {
		outSampleR = inSampleR;
	}

	param->leftSave = 0;

	fftSizeIn  = inSampleR * 2;
	fftSizeOut = outSampleR * 2;

	wkMemSize = fftSizeIn;
	if (wkMemSize < fftSizeOut) {
		wkMemSize = fftSizeOut;
	}
	wkMemSize *= 2;

	// 入力用
	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 出力用
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// ワーク用(別スレッド用)
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	if (type == 3 || type == 6) {
		// center /LFE 生成用
		// 入力用
		c_mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
		if (c_mem1 == NULL) {
			param->error_line = __LINE__;
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}

		// 出力用
		c_mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
		if (c_mem2 == NULL) {
			param->error_line = __LINE__;
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}

		// ワーク用(別スレッド用)
		c_mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
		if (c_mem3 == NULL) {
			param->error_line = __LINE__;
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
		c_mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
		if (c_mem4 == NULL) {
			param->error_line = __LINE__;
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
	}

	// 1
	fftw_in[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in[0] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_out[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_out[0] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_p[0] = fftw_plan_dft_1d(fftSizeIn,fftw_in[0],fftw_out[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[0] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[0] = fftw_plan_dft_1d(fftSizeOut,fftw_out[0],fftw_in[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[0] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2
	fftw_in[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in[1] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_out[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_out[1] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_p[1] = fftw_plan_dft_1d(fftSizeIn,fftw_in[1],fftw_out[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[1] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[1] = fftw_plan_dft_1d(fftSizeOut,fftw_out[1],fftw_in[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[1] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 3
	fftw_in[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in[2] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_out[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_out[2] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_p[2] = fftw_plan_dft_1d(fftSizeIn,fftw_in[2],fftw_out[2],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[2] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[2] = fftw_plan_dft_1d(fftSizeOut,fftw_out[2],fftw_in[2],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[2] == NULL) {
		param->error_line = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	if (type == 3 || type == 6) {
		// center
		param->fftw_out[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
		if (param->fftw_out[0] == NULL) {
			param->error_line = __LINE__;
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
		param->fftw_out[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
		if (param->fftw_out[1] == NULL) {
			param->error_line = __LINE__;
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
		param->fftw_out[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
		if (param->fftw_out[2] == NULL) {
			param->error_line = __LINE__;
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
	}

	pIn[0]	= &mem1[((fftSizeIn / 2) * 0)];
	pOut[0] = &mem2[((fftSizeOut / 2) * 0)];
	pIn[1]	= &mem1[((fftSizeIn / 2) * 1)];
	pOut[1] = &mem3[((fftSizeOut / 2) * 1)];
	pIn[2]	= &mem1[((fftSizeIn / 2) * 2)];
	pOut[2] = &mem4[((fftSizeOut / 2) * 2)];
	
	per = -1;
	if (type == 1 || type == 4) {
		inSample = inSample1;
		fio = fio1;
	} else if (type == 2) {
		inSample = inSample2;
		fio = fio2;
	} else if (type == 3 || type == 6) {
		c_pIn[0]	= &c_mem1[((fftSizeIn / 2) * 0)];
		c_pOut[0]	= &c_mem2[((fftSizeOut / 2) * 0)];
		c_pIn[1]	= &c_mem1[((fftSizeIn / 2) * 1)];
		c_pOut[1]	= &c_mem3[((fftSizeOut / 2) * 1)];
		c_pIn[2]	= &c_mem1[((fftSizeIn / 2) * 2)];
		c_pOut[2]	= &c_mem4[((fftSizeOut / 2) * 2)];
		inSample = inSample1;
		if (inSample > inSample2) {
			inSample = inSample2;
		}
		fio = fio1;
	}
	outRemain = inSample * ((double)outSampleR / inSampleR);

	for (startInSample = ((fftSizeIn + (fftSizeIn / 2)) * -1);startInSample < inSample + (fftSizeIn * 3);startInSample += fftSizeIn) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			if (percent != per) {
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) break;
				fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
				fflush(stdout);
			}
			per = percent;
			//Sleep(1);
		}

		memset(mem1,0,wkMemSize * sizeof (SSIZE));
		if (type == 3 || type == 6) {
			memset(c_mem1,0,wkMemSize * sizeof (SSIZE));
		}
		if (startInSample >= 0 && startInSample + (fftSizeIn * 2) < inSample) {
			fio_seek(fio,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSizeIn * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fio);
			if (type == 3 || type == 6) {
				fio_seek(fio2,startInSample * sizeof (SSIZE),SEEK_SET);
				fio_read(c_mem1,sizeof (SSIZE),nSample,fio2);
			}
		} else {
			mem0 = mem1;
			if (type == 3 || type == 6) {
				c_mem0 = c_mem1;
			}
			nSample = fftSizeIn * 2;
			if (startInSample >= 0) {
				fio_seek(fio,startInSample * sizeof (SSIZE),SEEK_SET);
				if (type == 3 || type == 6) {
					fio_seek(fio2,startInSample * sizeof (SSIZE),SEEK_SET);
				}
			} else {
				fio_seek(fio,0,SEEK_SET);
				if (type == 3 || type == 6) {
					fio_seek(fio2,0,SEEK_SET);
				}
				mem0 += (startInSample * -1);
				if (type == 3 || type == 6) {
					c_mem0 += (startInSample * -1);
				}
				if (nSample > startInSample * -1) {
					nSample -= startInSample * -1;
				} else {
					nSample = 0;
				}
			}

			if (startInSample >= inSample) {
				nSample = 0;
			} else {
				if (nSample != 0) {
					if (nSample > inSample - startInSample) {
						nSample = inSample - startInSample;
					}
				}
			}
			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fio);
				if (type == 3 || type == 6) {
					fio_read(c_mem0,sizeof (SSIZE),nSample,fio2);
				}
			}
			nSample = fftSizeIn * 2;
		}

		memset(mem2,0,wkMemSize * sizeof (SSIZE));
		memset(mem3,0,wkMemSize * sizeof (SSIZE));
		memset(mem4,0,wkMemSize * sizeof (SSIZE));
		
		if (type == 3 || type == 6) {
			memset(c_mem2,0,wkMemSize * sizeof (SSIZE));
			memset(c_mem3,0,wkMemSize * sizeof (SSIZE));
			memset(c_mem4,0,wkMemSize * sizeof (SSIZE));
			param->leftSave = 1;
		}

		#pragma omp parallel
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					// 1
					fftFilterSub(type,pIn[0],pOut[0],fftw_in[0],fftw_out[0],fftw_p[0],fftw_ip[0],param,0);
				}
				#pragma omp section
				{
					// 2
					fftFilterSub(type,pIn[1],pOut[1],fftw_in[1],fftw_out[1],fftw_p[1],fftw_ip[1],param,1);
				}
				#pragma omp section
				{
					// 3
					fftFilterSub(type,pIn[2],pOut[2],fftw_in[2],fftw_out[2],fftw_p[2],fftw_ip[2],param,2);
				}
			}
			#pragma omp for
			for (i = 0;i < wkMemSize;i++) {
				mem2[i] += mem3[i] + mem4[i];
			}
		}
		if (type == 3 || type == 6) {
			param->leftSave = 0;

			// 1
			fftFilterSub(type,c_pIn[0],c_pOut[0],fftw_in[0],fftw_out[0],fftw_p[0],fftw_ip[0],param,0);

			// 2
			fftFilterSub(type,c_pIn[1],c_pOut[1],fftw_in[1],fftw_out[1],fftw_p[1],fftw_ip[1],param,1);

			// 3
			fftFilterSub(type,c_pIn[2],c_pOut[2],fftw_in[2],fftw_out[2],fftw_p[2],fftw_ip[2],param,2);

			for (i = 0;i < wkMemSize;i++) {
				c_mem2[i] += c_mem3[i] + c_mem4[i];
			}
		}
		if (startInSample + fftSizeIn / 2 >= 0) {
			if (type != 3) {
				if (outRemain >= fftSizeOut) {
					wr = fio_write(mem2 + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut,fio3);
					if (wr != fftSizeOut) {
						param->error_line = __LINE__;
						param->err =	STATUS_FILE_WRITE_ERR;
						return;
					}
				} else {
					wr = fio_write(mem2 + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fio3);
					if (wr != outRemain) {
						param->error_line = __LINE__;
						param->err =	STATUS_FILE_WRITE_ERR;
						return;
					}
				}
			} else {
				if (outRemain >= fftSizeOut) {
					wr = fio_write(c_mem2 + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut,fio3);
					if (wr != fftSizeOut) {
						param->error_line = __LINE__;
						param->err =	STATUS_FILE_WRITE_ERR;
						return;
					}
				} else {
					wr = fio_write(c_mem2 + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fio3);
					if (wr != outRemain) {
						param->error_line = __LINE__;
						param->err =	STATUS_FILE_WRITE_ERR;
						return;
					}
				}
			}
			if (outRemain >= fftSizeOut) {
				outRemain -= fftSizeOut;
			} else {
				break;
			}
		}
	}

	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);
	
	if (type == 3 || type == 6) {
		al_free(c_mem1);
		al_free(c_mem2);
		al_free(c_mem3);
		al_free(c_mem4);
	}
	// 1
	fftw_destroy_plan(fftw_p[0]);
	fftw_destroy_plan(fftw_ip[0]);
	fftw_free(fftw_in[0]);
	fftw_free(fftw_out[0]);

	// 2
	fftw_destroy_plan(fftw_p[1]);
	fftw_destroy_plan(fftw_ip[1]);
	fftw_free(fftw_in[1]);
	fftw_free(fftw_out[1]);

	// 3
	fftw_destroy_plan(fftw_p[2]);
	fftw_destroy_plan(fftw_ip[2]);
	fftw_free(fftw_in[2]);
	fftw_free(fftw_out[2]);
	// center
	if (type == 3 || type == 6) {
		fftw_free(param->fftw_out[0]);
		fftw_free(param->fftw_out[1]);
		fftw_free(param->fftw_out[2]);
	}
	param->dis_downsample = 0;

}
//---------------------------------------------------------------------------
// Function   : fftFilterSub
// Description: FFT によるフィルタ処理(サブ関数)
// ---
//	param		:変換パラメータ
//
static void fftFilterSub(int type,SSIZE *pIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,PARAM_INFO *param,int id)
{
	long inSampleR,outSampleR;
	long wkSampleR;
	long fftSizeIn,fftSizeOut,i,j,n;
	long cutOff;
	long lowIndex;
	long hfc;
	long f_s,f_e,s_s,s_e;
	double step,div;
	double nx;
	double p;
	long validIndex;
	long h;

	inSampleR  = param->mc_inSampleR;
	outSampleR = param->mc_outSampleR;
	if (param->dis_downsample) {
		outSampleR = inSampleR;
	}
	fftSizeIn = inSampleR * 2;
	fftSizeOut = outSampleR * 2;

	// FFT 初期設定
	copyToFFTW(fftw_in,pIn,fftSizeIn);

	windowFFTW(fftw_in,fftSizeIn);

	for (i = 0;i < fftSizeOut;i++) {
		fftw_out[i][0] = 0;
		fftw_out[i][1] = 0;
	}
	
	// FFT
	fftw_execute(fftw_p);

	// 高域削除
	wkSampleR = inSampleR;
	if (wkSampleR > outSampleR) {
		wkSampleR = outSampleR;
	}

	hfc = wkSampleR / 2;
	cutOff = ((double)fftSizeOut / outSampleR) * hfc;

	if (type == 1 || type == 2) {
#if 0
		lowIndex = ((double)fftSizeOut / outSampleR) * 200;
		// 第一強調周波数
		f_s = ((double)fftSizeOut / outSampleR) * 800;
		f_e = ((double)fftSizeOut / outSampleR) * 1600;
		// 第二強調周波数
		s_s = ((double)fftSizeOut / outSampleR) * 10000;
		s_e = ((double)fftSizeOut / outSampleR) * 12000;

		for (i = 0;i < fftSizeOut / 2;i++) {
			p = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
			if (p > 0) {
				p = sqrt(p);
			}
			fftw_out[i][0] /= 2;
			fftw_out[i][1] /= 2;
			if (i < lowIndex) {
				fftw_out[i][0] /= 10;
				fftw_out[i][1] /= 10;
			}
			if (i >= f_s && i <= f_e) {
				fftw_out[i][0] *= 1.30;
				fftw_out[i][1] *= 1.30;
			}
			if (i >= s_s && i <= s_e) {
				fftw_out[i][0] *= 1.21;
				fftw_out[i][1] *= 1.21;
			}
		}
#endif
	} else if (type == 3 || type == 6) {
		if (param->leftSave == 0) {
			for (i = 0;i < fftSizeOut / 2;i++) {
				fftw_out[i][0] = (param->fftw_out[id][i][0] + fftw_out[i][0]) / 2;
				fftw_out[i][1] = (param->fftw_out[id][i][1] + fftw_out[i][1]) / 2;
			}
		}
		lowIndex = ((double)fftSizeOut / outSampleR) * 50;
		if (type == 3) {
			// 第一強調周波数
			f_s = ((double)fftSizeOut / outSampleR) * 300;
			f_e = ((double)fftSizeOut / outSampleR) * 500;
			// 第二強調周波数
			s_s = ((double)fftSizeOut / outSampleR) * 3000;
			s_e = ((double)fftSizeOut / outSampleR) * 5000;
			for (i = 0;i < fftSizeOut / 2;i++) {
				p = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
				if (p > 0) {
					p = sqrt(p);
				}
				if (i >= f_s && i <= f_e) {
					fftw_out[i][0] *= 1.30;
					fftw_out[i][1] *= 1.30;
				}
				if (i >= s_s && i <= s_e) {
					fftw_out[i][0] *= 1.21;
					fftw_out[i][1] *= 1.21;
				}
				fftw_out[i][0] *= 0.925;
				fftw_out[i][1] *= 0.925;
			}
		} else {
			// 第一強調周波数
			f_s = ((double)fftSizeOut / outSampleR) * 30;
			// 第一強調周波数end
			f_e = ((double)fftSizeOut / outSampleR) * 150;
			// カットオフ周波数
			s_s = ((double)fftSizeOut / outSampleR) * 4000;
			for (i = 0;i < fftSizeOut / 2;i++) {
				p = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
				if (p > 0) {
					p = sqrt(p);
				}
				if (i >= f_s && i <= f_e) {
					fftw_out[i][0] *= 1.50;
					fftw_out[i][1] *= 1.50;
				} else if (i >= s_s) {
					fftw_out[i][0] = 0;
					fftw_out[i][1] = 0;
				}
			}
		}
		if (param->leftSave) {
			for (i = 0;i < fftSizeOut;i++) {
				param->fftw_out[id][i][0] = fftw_out[i][0];
				param->fftw_out[id][i][1] = fftw_out[i][1];
			}
		}
	} else if (type == 4) {
		// 第一強調周波数
		f_s = ((double)fftSizeOut / outSampleR) * 800;
		f_e = ((double)fftSizeOut / outSampleR) * 1600;
		// 第二強調周波数
		s_s = ((double)fftSizeOut / outSampleR) * 10000;
		s_e = ((double)fftSizeOut / outSampleR) * 12000;
		for (i = 0;i < fftSizeOut / 2;i++) {
			p = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
			if (p > 0) {
				p = sqrt(p);
			}
			if (i >= f_s && i <= f_e) {
				fftw_out[i][0] *= 1.30;
				fftw_out[i][1] *= 1.30;
			}
			if (i >= s_s && i <= s_e) {
				fftw_out[i][0] *= 1.21;
				fftw_out[i][1] *= 1.21;
			}
		}
		lowIndex = ((double)fftSizeOut / outSampleR) * 300;
		for (i = 1;i < fftSizeOut / 2;i++) {
			if (i < lowIndex) {
				fftw_out[i][0] *= 0.05;
				fftw_out[i][1] *= 0.05;
			}
		}
	}


	// カットオフ
	cutFFTW(fftw_out,cutOff,fftSizeOut);

	// 半分のデータを復元
	for (i = 1;i < fftSizeOut / 2;i++) {
		fftw_out[fftSizeOut - i][0] = fftw_out[i][0];
		fftw_out[fftSizeOut - i][1] = fftw_out[i][1] * -1;
	}

	// invert FFT
	fftw_execute(fftw_ip);

	// 出力
	for (i = 0;i < fftSizeOut;i++) {
		pOut[i] = (SSIZE)(fftw_in[i][0] / fftSizeOut);
	}
}
#if 0
//---------------------------------------------------------------------------
// Function   : copyToFFTW
// Description: fftw用配列に値をコピーする
// ---
//	
//
void copyToFFTW(fftw_complex *fftw,SSIZE *buf,long size)
{
	long i;
	
	for (i = 0;i + 64 < size;i+=64) {
		fftw[i + 0][0] = buf[0];
		fftw[i + 0][1] = 0;
		fftw[i + 1][0] = buf[1];
		fftw[i + 1][1] = 0;
		fftw[i + 2][0] = buf[2];
		fftw[i + 2][1] = 0;
		fftw[i + 3][0] = buf[3];
		fftw[i + 3][1] = 0;
		fftw[i + 4][0] = buf[4];
		fftw[i + 4][1] = 0;
		fftw[i + 5][0] = buf[5];
		fftw[i + 5][1] = 0;
		fftw[i + 6][0] = buf[6];
		fftw[i + 6][1] = 0;
		fftw[i + 7][0] = buf[7];
		fftw[i + 7][1] = 0;
		fftw[i + 8][0] = buf[8];
		fftw[i + 8][1] = 0;
		fftw[i + 9][0] = buf[9];
		fftw[i + 9][1] = 0;
		fftw[i + 10][0] = buf[10];
		fftw[i + 10][1] = 0;
		fftw[i + 11][0] = buf[11];
		fftw[i + 11][1] = 0;
		fftw[i + 12][0] = buf[12];
		fftw[i + 12][1] = 0;
		fftw[i + 13][0] = buf[13];
		fftw[i + 13][1] = 0;
		fftw[i + 14][0] = buf[14];
		fftw[i + 14][1] = 0;
		fftw[i + 15][0] = buf[15];
		fftw[i + 15][1] = 0;
		fftw[i + 16][0] = buf[16];
		fftw[i + 16][1] = 0;
		fftw[i + 17][0] = buf[17];
		fftw[i + 17][1] = 0;
		fftw[i + 18][0] = buf[18];
		fftw[i + 18][1] = 0;
		fftw[i + 19][0] = buf[19];
		fftw[i + 19][1] = 0;
		fftw[i + 20][0] = buf[20];
		fftw[i + 20][1] = 0;
		fftw[i + 21][0] = buf[21];
		fftw[i + 21][1] = 0;
		fftw[i + 22][0] = buf[22];
		fftw[i + 22][1] = 0;
		fftw[i + 23][0] = buf[23];
		fftw[i + 23][1] = 0;
		fftw[i + 24][0] = buf[24];
		fftw[i + 24][1] = 0;
		fftw[i + 25][0] = buf[25];
		fftw[i + 25][1] = 0;
		fftw[i + 26][0] = buf[26];
		fftw[i + 26][1] = 0;
		fftw[i + 27][0] = buf[27];
		fftw[i + 27][1] = 0;
		fftw[i + 28][0] = buf[28];
		fftw[i + 28][1] = 0;
		fftw[i + 29][0] = buf[29];
		fftw[i + 29][1] = 0;
		fftw[i + 30][0] = buf[30];
		fftw[i + 30][1] = 0;
		fftw[i + 31][0] = buf[31];
		fftw[i + 31][1] = 0;
		fftw[i + 32][0] = buf[32];
		fftw[i + 32][1] = 0;
		fftw[i + 33][0] = buf[33];
		fftw[i + 33][1] = 0;
		fftw[i + 34][0] = buf[34];
		fftw[i + 34][1] = 0;
		fftw[i + 35][0] = buf[35];
		fftw[i + 35][1] = 0;
		fftw[i + 36][0] = buf[36];
		fftw[i + 36][1] = 0;
		fftw[i + 37][0] = buf[37];
		fftw[i + 37][1] = 0;
		fftw[i + 38][0] = buf[38];
		fftw[i + 38][1] = 0;
		fftw[i + 39][0] = buf[39];
		fftw[i + 39][1] = 0;
		fftw[i + 40][0] = buf[40];
		fftw[i + 40][1] = 0;
		fftw[i + 41][0] = buf[41];
		fftw[i + 41][1] = 0;
		fftw[i + 42][0] = buf[42];
		fftw[i + 42][1] = 0;
		fftw[i + 43][0] = buf[43];
		fftw[i + 43][1] = 0;
		fftw[i + 44][0] = buf[44];
		fftw[i + 44][1] = 0;
		fftw[i + 45][0] = buf[45];
		fftw[i + 45][1] = 0;
		fftw[i + 46][0] = buf[46];
		fftw[i + 46][1] = 0;
		fftw[i + 47][0] = buf[47];
		fftw[i + 47][1] = 0;
		fftw[i + 48][0] = buf[48];
		fftw[i + 48][1] = 0;
		fftw[i + 49][0] = buf[49];
		fftw[i + 49][1] = 0;
		fftw[i + 50][0] = buf[50];
		fftw[i + 50][1] = 0;
		fftw[i + 51][0] = buf[51];
		fftw[i + 51][1] = 0;
		fftw[i + 52][0] = buf[52];
		fftw[i + 52][1] = 0;
		fftw[i + 53][0] = buf[53];
		fftw[i + 53][1] = 0;
		fftw[i + 54][0] = buf[54];
		fftw[i + 54][1] = 0;
		fftw[i + 55][0] = buf[55];
		fftw[i + 55][1] = 0;
		fftw[i + 56][0] = buf[56];
		fftw[i + 56][1] = 0;
		fftw[i + 57][0] = buf[57];
		fftw[i + 57][1] = 0;
		fftw[i + 58][0] = buf[58];
		fftw[i + 58][1] = 0;
		fftw[i + 59][0] = buf[59];
		fftw[i + 59][1] = 0;
		fftw[i + 60][0] = buf[60];
		fftw[i + 60][1] = 0;
		fftw[i + 61][0] = buf[61];
		fftw[i + 61][1] = 0;
		fftw[i + 62][0] = buf[62];
		fftw[i + 62][1] = 0;
		fftw[i + 63][0] = buf[63];
		fftw[i + 63][1] = 0;
		buf += 64;
	}
	for (;i < size;i++) {
		fftw[i][0] = *buf++;
		fftw[i][1] = 0;
	}
}
//---------------------------------------------------------------------------
// Function   : windowFFTW
// Description: FFTW用Window関数
// ---
//	
//
void windowFFTW(fftw_complex *fftw,long size)
{
	long i,j;

	// ウインドウサイズ毎に定数化する
	switch (size) {
		case (4096 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 1));
			}
			#pragma omp parallel for
			for (i = ((4096 * 1) - 1) / 2;i < (4096 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 1));
			}
			break;
		case (4096 * 2):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 2) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 2));
			}
			#pragma omp parallel for
			for (i = ((4096 * 2) - 1) / 2;i < (4096 * 2);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 2));
			}
			break;
		case (4096 * 4):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 4) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 4));
			}
			#pragma omp parallel for
			for (i = ((4096 * 4) - 1) / 2;i < (4096 * 4);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 4));
			}
			break;
		case (4096 * 8):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 8) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 8));
			}
			#pragma omp parallel for
			for (i = ((4096 * 8) - 1) / 2;i < (4096 * 8);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 8));
			}
			break;
		case (32000 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((32000 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(32000 * 1));
			}
			#pragma omp parallel for
			for (i = ((32000 * 1) - 1) / 2;i < (32000 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(32000 * 1));
			}
			break;
		case (44100 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 1));
			}
			#pragma omp parallel for
			for (i = ((44100 * 1) - 1) / 2;i < (44100 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 1));
			}
			break;
		case (48000 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 1));
			}
			#pragma omp parallel for
			for (i = ((48000 * 1) - 1) / 2;i < (48000 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 1));
			}
			break;
		case (44100 * 2):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 2) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 2));
			}
			#pragma omp parallel for
			for (i = ((44100 * 2) - 1) / 2;i < (44100 * 2);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 2));
			}
			break;
		case (48000 * 2):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 2) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 2));
			}
			#pragma omp parallel for
			for (i = ((48000 * 2) - 1) / 2;i < (48000 * 2);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 2));
			}
			break;
		case (44100 * 4):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 4) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 4));
			}
			#pragma omp parallel for
			for (i = ((44100 * 4) - 1) / 2;i < (44100 * 4);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 4));
			}
			break;
		case (48000 * 4):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 4) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 4));
			}
			#pragma omp parallel for
			for (i = ((48000 * 4) - 1) / 2;i < (48000 * 4);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 4));
			}
			break;
		case (44100 * 8):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 8) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 8));
			}
			#pragma omp parallel for
			for (i = ((44100 * 8) - 1) / 2;i < (44100 * 8);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 8));
			}
			break;
		case (48000 * 8):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 8) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 8));
			}
			#pragma omp parallel for
			for (i = ((48000 * 8) - 1) / 2;i < (48000 * 8);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 8));
			}
			break;
		case (44100 * 16):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 16) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 16));
			}
			#pragma omp parallel for
			for (i = ((44100 * 16) - 1) / 2;i < (44100 * 16);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 16));
			}
			break;
		case (48000 * 16):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 16) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 16));
			}
			#pragma omp parallel for
			for (i = ((48000 * 16) - 1) / 2;i < (48000 * 16);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 16));
			}
			break;
		case (44100 * 32):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 32) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 32));
			}
			#pragma omp parallel for
			for (i = ((44100 * 32) - 1) / 2;i < (44100 * 32);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 32));
			}
			break;
		case (48000 * 32):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 32) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 32));
			}
			#pragma omp parallel for
			for (i = ((48000 * 32) - 1) / 2;i < (48000 * 32);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 32));
			}
			break;
		default:
			break;
	}
}
//---------------------------------------------------------------------------
// Function   : cutFFTW
// Description: FFTW用カットオフ関数
// ---
//	
//
void cutFFTW(fftw_complex *fftw,long index,long size)
{
	long i;

	// 64 個ずつ
	for (i = index;i + 64 < size;i+= 64) {
		fftw[i + 0][0] = 0;
		fftw[i + 0][1] = 0;
		fftw[i + 1][0] = 0;
		fftw[i + 1][1] = 0;
		fftw[i + 2][0] = 0;
		fftw[i + 2][1] = 0;
		fftw[i + 3][0] = 0;
		fftw[i + 3][1] = 0;
		fftw[i + 4][0] = 0;
		fftw[i + 4][1] = 0;
		fftw[i + 5][0] = 0;
		fftw[i + 5][1] = 0;
		fftw[i + 6][0] = 0;
		fftw[i + 6][1] = 0;
		fftw[i + 7][0] = 0;
		fftw[i + 7][1] = 0;
		fftw[i + 8][0] = 0;
		fftw[i + 8][1] = 0;
		fftw[i + 9][0] = 0;
		fftw[i + 9][1] = 0;
		fftw[i + 10][0] = 0;
		fftw[i + 10][1] = 0;
		fftw[i + 11][0] = 0;
		fftw[i + 11][1] = 0;
		fftw[i + 12][0] = 0;
		fftw[i + 12][1] = 0;
		fftw[i + 13][0] = 0;
		fftw[i + 13][1] = 0;
		fftw[i + 14][0] = 0;
		fftw[i + 14][1] = 0;
		fftw[i + 15][0] = 0;
		fftw[i + 15][1] = 0;
		fftw[i + 16][0] = 0;
		fftw[i + 16][1] = 0;
		fftw[i + 17][0] = 0;
		fftw[i + 17][1] = 0;
		fftw[i + 18][0] = 0;
		fftw[i + 18][1] = 0;
		fftw[i + 19][0] = 0;
		fftw[i + 19][1] = 0;
		fftw[i + 20][0] = 0;
		fftw[i + 20][1] = 0;
		fftw[i + 21][0] = 0;
		fftw[i + 21][1] = 0;
		fftw[i + 22][0] = 0;
		fftw[i + 22][1] = 0;
		fftw[i + 23][0] = 0;
		fftw[i + 23][1] = 0;
		fftw[i + 24][0] = 0;
		fftw[i + 24][1] = 0;
		fftw[i + 25][0] = 0;
		fftw[i + 25][1] = 0;
		fftw[i + 26][0] = 0;
		fftw[i + 26][1] = 0;
		fftw[i + 27][0] = 0;
		fftw[i + 27][1] = 0;
		fftw[i + 28][0] = 0;
		fftw[i + 28][1] = 0;
		fftw[i + 29][0] = 0;
		fftw[i + 29][1] = 0;
		fftw[i + 30][0] = 0;
		fftw[i + 30][1] = 0;
		fftw[i + 31][0] = 0;
		fftw[i + 31][1] = 0;
		fftw[i + 32][0] = 0;
		fftw[i + 32][1] = 0;
		fftw[i + 33][0] = 0;
		fftw[i + 33][1] = 0;
		fftw[i + 34][0] = 0;
		fftw[i + 34][1] = 0;
		fftw[i + 35][0] = 0;
		fftw[i + 35][1] = 0;
		fftw[i + 36][0] = 0;
		fftw[i + 36][1] = 0;
		fftw[i + 37][0] = 0;
		fftw[i + 37][1] = 0;
		fftw[i + 38][0] = 0;
		fftw[i + 38][1] = 0;
		fftw[i + 39][0] = 0;
		fftw[i + 39][1] = 0;
		fftw[i + 40][0] = 0;
		fftw[i + 40][1] = 0;
		fftw[i + 41][0] = 0;
		fftw[i + 41][1] = 0;
		fftw[i + 42][0] = 0;
		fftw[i + 42][1] = 0;
		fftw[i + 43][0] = 0;
		fftw[i + 43][1] = 0;
		fftw[i + 44][0] = 0;
		fftw[i + 44][1] = 0;
		fftw[i + 45][0] = 0;
		fftw[i + 45][1] = 0;
		fftw[i + 46][0] = 0;
		fftw[i + 46][1] = 0;
		fftw[i + 47][0] = 0;
		fftw[i + 47][1] = 0;
		fftw[i + 48][0] = 0;
		fftw[i + 48][1] = 0;
		fftw[i + 49][0] = 0;
		fftw[i + 49][1] = 0;
		fftw[i + 50][0] = 0;
		fftw[i + 50][1] = 0;
		fftw[i + 51][0] = 0;
		fftw[i + 51][1] = 0;
		fftw[i + 52][0] = 0;
		fftw[i + 52][1] = 0;
		fftw[i + 53][0] = 0;
		fftw[i + 53][1] = 0;
		fftw[i + 54][0] = 0;
		fftw[i + 54][1] = 0;
		fftw[i + 55][0] = 0;
		fftw[i + 55][1] = 0;
		fftw[i + 56][0] = 0;
		fftw[i + 56][1] = 0;
		fftw[i + 57][0] = 0;
		fftw[i + 57][1] = 0;
		fftw[i + 58][0] = 0;
		fftw[i + 58][1] = 0;
		fftw[i + 59][0] = 0;
		fftw[i + 59][1] = 0;
		fftw[i + 60][0] = 0;
		fftw[i + 60][1] = 0;
		fftw[i + 61][0] = 0;
		fftw[i + 61][1] = 0;
		fftw[i + 62][0] = 0;
		fftw[i + 62][1] = 0;
		fftw[i + 63][0] = 0;
		fftw[i + 63][1] = 0;
	}
	// 残り
	for (;i < size;i++) {
		fftw[i + 0][0] = 0;
		fftw[i + 0][1] = 0;
	}
}
#endif
//---------------------------------------------------------------------------
// Function   : merageTempFile
// Description: 出力結果のファイルをマージする
// ---
//	type	 :マージの種類
//	normFlag :ノーマライズ用変数更新フラグ
//	fp_r 	 :入力ファイル1
//	fp_r2	 :入力ファイル2
//	fp_w	 :出力ファイル
//	inSample :サンプル数
//	param	 :パラメーター
//
void merageTempFile(char type,int normFlag,FIO *fp_r,FIO *fp_r2,FIO *fp_w,SSIZE inSample,PARAM_INFO *param)
{
	SSIZE v1,v2,v3;
	SSIZE min,max;
	SSIZE maxLv,maxLv2;
	DWORD ns;
	SSIZE remainSample;
	char sss[300];
	long i;
	fio_size remain1,remain2;
	fio_size wr_n;
	fio_size pos;
	int percent;

	if (fp_r != NULL) {
		fio_rewind(fp_r);
	}

	if (fp_r2 != NULL) {
		fio_rewind(fp_r2);
	}

	if (fp_w != NULL) {
		fio_rewind(fp_w);
	}

	ns	= 0;
	maxLv = 0;
	maxLv2 = 0;
	min = max = 0;
	remainSample = inSample;

	do {
		if (type == '+') {
//			Sleep(1);
			remain1 = fio_read(diskBuffer1,sizeof (SSIZE),1 * 1024 * 1024,fp_r);
			if (fp_r2 != NULL) {
				remain2 = fio_read(diskBuffer2,sizeof (SSIZE),1 * 1024 * 1024,fp_r2);
			}
			if (remain1 == 0 || remain2 == 0) {
				break;
			}
			for (i = 0;i < remain1;i++) {
				if (diskBuffer1[i] != 0) {
					diskBuffer1[i] += diskBuffer2[i];
					if (diskBuffer1[i] < min) {
						min = diskBuffer1[i];
					}
					if (diskBuffer1[i] > max) {
						max = diskBuffer1[i];
					}
					if (diskBuffer1[i] >> 40 > 0) {
						maxLv2 += diskBuffer1[i] >> 40;
						ns++;
						if (maxLv2 >= 0x1000000000000) {
							maxLv2 /= ns;
							if (maxLv > 0) {
								maxLv = (maxLv + maxLv2) / 2;
							} else {
								maxLv = maxLv2;
							}
							maxLv2 = 0;
							ns = 0;
						}
					}
				}
			}
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fp_w != NULL) {
//				fio_seek(fp_w,pos,SEEK_SET);
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
					break;
				}
			}
			remainSample -= remain1;
		} else if (type == '-') {
//			Sleep(1);
			remain1 = fio_read(diskBuffer1,sizeof (SSIZE),1 * 1024 * 1024,fp_r);
			if (fp_r2 != NULL) {
				remain2 = fio_read(diskBuffer2,sizeof (SSIZE),1 * 1024 * 1024,fp_r2);
			}
			if (remain1 == 0) {
				break;
			}
			for (i = 0;i < remain1;i++) {
				if (diskBuffer1[i] != 0) {
					diskBuffer1[i] -= diskBuffer2[i];
					if (diskBuffer1[i] < min) {
						min = diskBuffer1[i];
					}
					if (diskBuffer1[i] > max) {
						max = diskBuffer1[i];
					}
					if (diskBuffer1[i] >> 40 > 0) {
						maxLv2 += diskBuffer1[i] >> 40;
						ns++;
						if (maxLv2 >= 0x1000000000000) {
							maxLv2 /= ns;
							if (maxLv > 0) {
								maxLv = (maxLv + maxLv2) / 2;
							} else {
								maxLv = maxLv2;
							}
							maxLv2 = 0;
							ns = 0;
						}
					}
				}
			}
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fp_w != NULL) {
//				fio_seek(fp_w,pos,SEEK_SET);
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
					break;
				}
			}
			remainSample -= remain1;
		} else if (type == ' ') {
			//Sleep(1);
			sprintf(sss,"merageTempFile:%lld",remainSample);
			PRINT_LOG(sss);
			remain1 = fio_read(diskBuffer1,sizeof (SSIZE),1 * 1024 * 1024,fp_r);
			sprintf(sss,"remain1:%lld",remain1);
			PRINT_LOG(sss);
			if (remain1 == 0) {
				break;
			}
			sprintf(sss,"merageTempFile:%lld,remain1:%lld",remainSample,remain1);
			PRINT_LOG(sss);

			for (i = 0;i < remain1;i++) {
				if (diskBuffer1[i] < min) {
					min = diskBuffer1[i];
				}
				if (diskBuffer1[i] > max) {
					max = diskBuffer1[i];
				}
				if (diskBuffer1[i] >> 40 > 0) {
					maxLv2 += diskBuffer1[i] >> 40;
					ns++;
					if (maxLv2 >= 0x1000000000000) {
						maxLv2 /= ns;
						if (maxLv > 0) {
							maxLv = (maxLv + maxLv2) / 2;
						} else {
							maxLv = maxLv2;
						}
						maxLv2 = 0;
						ns = 0;
					}
				}
			}
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fp_w != NULL) {
//				fio_seek(fp_w,pos,SEEK_SET);
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
					break;
				}
			}
			remainSample -= remain1;
		}
	} while (remain1 == 1 * 1024 * 1024L && remainSample > 0);

	if (remainSample > 0 && param->err == STATUS_SUCCESS) {
		param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
	}

	if (param->err) {
		// エラー終了
		return;
	}
	
	if (fp_w != NULL) {
		fio_flush(fp_w);
	}
	if (normFlag == 1) {
		if (max > NormInfo.max) {
			NormInfo.max = max;
		}
		if (min < NormInfo.min) {
			NormInfo.min = min;
		}
		if (ns > 0) {
			maxLv2 /= ns;
		}
		if (maxLv > 0) {
			maxLv = (maxLv + maxLv2) / 2;
		} else {
			maxLv = maxLv2;
		}
		NormInfo.avg = maxLv;
	}
}
//---------------------------------------------------------------------------
// Function   : volumeAdjustFile
// Description: 音量調整後のファイルを出力する。
// ---
//  nx       :調整値
//	in 	     :入力ファイル
//  out      :出力ファイル
//	inSample :サンプル数
//	param	 :パラメーター
//  normInfo :ノーマライズ情報
//
static void volumeAdjustFile(double nx,FIO *fp_r,FIO *fp_w,SSIZE inSample,PARAM_INFO *param,NORM_INFO *normInfo)
{
	SSIZE min,max;
	SSIZE maxLv,maxLv2;
	SSIZE remainSample;
	SSIZE ns;
	long i;
	fio_size remain;
	fio_size wr_n;

	fio_rewind(fp_r);
	fio_rewind(fp_w);

	min = max = 0;
	ns	= 0;
	maxLv = 0;
	maxLv2 = 0;
	remainSample = inSample;

	do {
		remain = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
		if (remain == 0) {
			break;
		}
		for (i = 0;i < remain;i++) {
			diskBuffer1[i] = CLIP_NX(diskBuffer1[i],nx);
			if (diskBuffer1[i] < min) {
				min = diskBuffer1[i];
			}
			if (diskBuffer1[i] > max) {
				max = diskBuffer1[i];
			}
			if (diskBuffer1[i] >> 40 > 0) {
				maxLv2 += diskBuffer1[i] >> 40;
				ns++;
				if (maxLv2 >= 0x1000000000000) {
					maxLv2 /= ns;
					if (maxLv > 0) {
						maxLv = (maxLv + maxLv2) / 2;
					} else {
						maxLv = maxLv2;
					}
					ns = 0;
				}
			}
		}
		remain =  remain < remainSample ? remain : remainSample;
		wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain,fp_w);
		if (wr_n != remain) {
			param->err = STATUS_FILE_WRITE_ERR;param->error_line = __LINE__;
			break;
		}
		remainSample -= remain;
	} while (remain == 4 * 1024 * 1024L && remainSample > 0);

	if (remainSample > 0 && param->err == STATUS_SUCCESS) {
		param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
	}

	if (param->err) {
		param->err = STATUS_FILE_READ_ERR;param->error_line = __LINE__;
		// エラー終了
		return;
	}
	
	fio_flush(fp_w);
	if (normInfo != NULL) {
		if (max > normInfo->max) {
			normInfo->max = max;
		}
		if (min < normInfo->min) {
			normInfo->min = min;
		}
	}
	if (ns > 0) {
		maxLv2 /= ns;
	}
	if (maxLv > 0) {
		maxLv = (maxLv + maxLv2) / 2;
	} else {
		maxLv = maxLv2;
	}
	if (normInfo != NULL) {
		normInfo->avg = maxLv;
	}
}
#if 0
//---------------------------------------------------------------------------
// Function   : al_malloc
// Description: 16バイト境界対応malloc関数
// ---
// 返すポインタの16バイト前にmallocで確保したメモリ領域のアドレスを入れる
//
void *al_malloc(long size)
{
	void *ptr;
	void *ret;
	int align;

	ptr = malloc(size + 32);
	if (ptr) {
		ret = ptr;
		align = (int)ptr % 16;
		if (align != 0) {
			align = 16 - align;
			ret = (char *)ret + align;
		}
		*((SSIZE *)ret) = (SSIZE)ptr;

		ret = (char *)ret + 16;
	} else {
		ret = NULL;
	}
	return ret;
}
//---------------------------------------------------------------------------
// Function   : al_free
// Description: 16バイト境界対応free関数
// ---
// 
//
void *al_free(void *ptr)
{
	void *p;
	
	if (ptr) {
		p = (char *)ptr - 16;
		p = (void *)(*((SSIZE *)p));
		free(p);
	}
}
static int chkAbort(PARAM_INFO *param,int percent,int percent_diff)
{
	FILE *fp;
	int diff = param->abort_percent - percent;
	
	if (diff == 0 || diff > percent_diff || diff < (percent_diff * -1) || percent == 0 || percent == 100) {
		param->abort_percent = percent;
		if (param->abort_filename[0] != '\0') {
			fp = fopen(param->abort_filename,"r");
			if (fp) {
				fclose(fp);
				param->err = 1;
				return 1;
			}
		}
	}
	return 0;
}
#endif

