/****************************************************************************/
/* wav2raw (C) 2011-2024 By 59414d41										*/
/* wavファイルを64bitのrawファイルへ変換する								*/
/* rawファイルをwavファイルへ変換する										*/
/* dsfファイルを64bitのrawファイルへ変換する								*/
/* rawファイルをdsfファイルにする											*/
/*																			*/
/****************************************************************************/

/*--- Log ------------------------------------------------------------------
 * Ver 0.10 <09/07/15>  - upconvから分離
 * Ver 0.21 <09/10/26>  - ちょっと修正
 * Ver 0.30 <09/11/01>  - パラメータファイルの採用
 * Ver 0.31 <09/11/16>  - エラー情報をファイルへ出力するようにした
 * Ver 0.50 <10/11/02>  - 処理修正
 * Ver 0.70 <11/07/24>  - コンパイラをmingwに変更
 *						 大きなファイルに対応
 *						 split処理をやめたことによる修正
 * Ver 0.80 <12/02/11>  - fileioを使用するように修正
 *						 マルチチャンネルに対応
 * ver 0.99 <18/10/25>  - raw2wav,dsf2raw とマージ
 * ver 1.20 <19/10/12>  - upconv.c から呼び出すように修正
 * ver 1.21 <21/03/19>  - 32bit(Int) に対応
 *                      - ADDP 用の修正
 * ver 1.22 <21/12/20>  - FileIO Auto 用の処理追加
 * ver 1.0.5 <24/05/09> - FileIO の処理変更
 * Ver 1.13 <24/11/09>  - ソースコードの整理。バージョンをupconvfeに合わせた
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#define _WIN_CP	(1)
#endif
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "upconv.h"
#include "start_exec.h"
#include "fftw3.h"
#include "./fileio.h"
#include "./../PLG_AUDIO_IO/PLG_AudioIO.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#define STR_VER "Upconv Frontend 1.0.4 [2024/0x/xx]"


// サンプルを処理するデータ型
#define SSIZE	signed long long int
#define UI64	unsigned long long int

typedef struct {
	SSIZE	minR1;
	SSIZE	maxR1;
	SSIZE	minR2;
	SSIZE	maxR2;
	SSIZE	minR3;
	SSIZE	maxR3;
	SSIZE	minR4;
	SSIZE	maxR4;
	SSIZE	minR5;
	SSIZE	maxR5;
	SSIZE	minR6;
	SSIZE	maxR6;
	SSIZE	maxLv1;
	SSIZE	maxLv2;
	SSIZE	maxLv3;
	SSIZE	maxLv4;
	SSIZE	maxLv5;
	SSIZE	maxLv6;
	SSIZE	tmpLv1;
	SSIZE	tmpLv2;
	SSIZE	tmpLv3;
	SSIZE	tmpLv4;
	SSIZE	tmpLv5;
	SSIZE	tmpLv6;
	SSIZE	cntLv1;
	SSIZE	cntLv2;
	SSIZE	cntLv3;
	SSIZE	cntLv4;
	SSIZE	cntLv5;
	SSIZE	cntLv6;
	int		clipR1;
	int		clipR2;
	int		clipR3;
	int		clipR4;
	int		clipR5;
	int		clipR6;
} NORM_INFO_WAV2;

typedef struct {
	double p1;
	SSIZE  p2;
	double p3;
	double p4;
} PARAM_NX;

typedef struct {
	long r2w_outSampleR;
	long r2w_inSampleR;
	int r2w_ch;
	int err;
	int		errLine;
	double	nx1;
	double	nx2;
	double	nx3;
	double	nx4;
	double	nx5;
	double	nx6;
	SSIZE	sum1;
	SSIZE	sum2;
	SSIZE	sum3;
	SSIZE	sum4;
	SSIZE	sum5;
	SSIZE	sum6;
	SSIZE	sum1_2nd;
	SSIZE	sum2_2nd;
	SSIZE	sum3_2nd;
	SSIZE	sum4_2nd;
	SSIZE	sum5_2nd;
	SSIZE	sum6_2nd;
	SSIZE	sum1_3rd;
	SSIZE	sum2_3rd;
	SSIZE	sum3_3rd;
	SSIZE	sum4_3rd;
	SSIZE	sum5_3rd;
	SSIZE	sum6_3rd;
	SSIZE	old_s1;
	SSIZE	old_s2;
	SSIZE	old_s3;
	SSIZE	old_s4;
	SSIZE	old_s5;
	SSIZE	old_s6;
	SSIZE	s_b1,s_a1;
	SSIZE	s_b2,s_a2;
	SSIZE	s_b3,s_a3;
	SSIZE	s_b4,s_a4;
	SSIZE	s_b5,s_a5;
	SSIZE	s_b6,s_a6;
	double  adjPer1;
	double  adjPer2;
	double  adjPer3;
	double  adjPer4;
	double  adjPer5;
	double  adjPer6;
	PARAM_NX param_nx[5];
	char fromfile[_MAX_PATH];
	char tofile[_MAX_PATH];
	char opt_flac[512];
	char opt_wavpack[512];
	char opt_mp3[512];
	char opt_opus[512];
	char opt_m4a[512];
	char opt_alt_lame_option[512];
	char opt_alt_ffmpeg_option[512];
	char opt_alt_ext[512];
	char pr_str[40];
	FIO *fp_w[6];
	FIO *fp_temp[2];
	char	debug_upconv[100];
	char *arg_dp;
	double *analyze_eq_pw;
} PARAM_INFO_TO_WAV;

typedef struct {
	UI64	w2r_data_offset;
	UI64	n_sample;
	int		w2r_channel;
	int		err;
	int		errLine;
	int		w2r_decode;
	int		dsf_decode;
	int 	addp_overrap;
	long	w2r_inSampleR;
	long	w2r_outSampleR;
	long	enable_hfc;
	long	lfc;				// DSF 用
	long	hfc;				// DSF 用
	long	src_flag;
	long	w2r_dsf_mode;
	SSIZE	l_min,l_max;
	SSIZE	r_min,r_max;
	SSIZE	l_maxLv,r_maxLv;
	SSIZE	l_tmpLv,r_tmpLv;
	SSIZE	l_cntLv,r_cntLv;
	char	*workpath;
	char	*argv4;
	char	*file_ms4;
	char	debug_upconv[100];
	char pr_str[40];
} PARAM_INFO_TO_RAW;


// Mid/Side 用 Delay 構造体
typedef struct {
	int 	delay;
	double	atr;
} MS_DELAY;

// BWF の link チャンク対応
char link_start[]="<LINK>\r\n";
char link_file[]=							\
	"\t<FILE type=\"%s\">\r\n"				\
	"\t\t<FILENUMBER>%d</FILENUMBER>\r\n"	\
	"\t\t<FILENAME>%s</FILENAME>\r\n"		\
	"\t</FILE>\r\n";
char link_end[]="</LINK>";

//
// DSF ファイルフォーマット仕様書を参照
#pragma pack(push, 1)
typedef struct {
	char	id[4];
	UI64	chunk_size;
	UI64	file_size;
	UI64	ptr_meta;
} DSF;

typedef struct {
	char	id[4];
	UI64	chunk_size;
	DWORD	fmt_version;
	DWORD	fmt_id;
	DWORD	channel_type;
	DWORD	channel_count;
	DWORD	sampling;
	DWORD	sample_bit_count;
	UI64	sample_count;
	DWORD	block_size;
	DWORD	reserved;
} DSF_FMT;

typedef struct {
	char	id[4];
	UI64	chunk_size;
} DSF_DATA;
#pragma pack(pop)

extern LEVEL_INFO Wav2RawLevelInfo;
extern LEVEL_INFO Raw2WavLevelInfo;
extern LEVEL_INFO Raw2WavLevelInfo2;

NORM_INFO_WAV2 NormInfoWav2;
int errLine;
char w2r_log_filename[_MAX_PATH];
extern ABORT_INFO abortInfo;

#ifdef UPCONV_LOG
#define	PRINT_LOG(s)			do {																	\
									FILE *log;															\
									log = fopen(w2r_log_filename,"a");									\
									if (log) {															\
										fprintf(log,"%s [%d] %s\n",__FUNCTION__,__LINE__,s);			\
										fclose(log);													\
									}																	\
								} while (0)

#define	PRINT_FN_LOG(pn,s)		do {																	\
									FILE *log;															\
									log = fopen(w2r_log_filename,"a");									\
									if (log) {															\
										fprintf(log,"[%05d]%s:%s\n",__LINE__,pn,s);						\
										fclose(log);													\
									}																	\
								} while (0)
#else
#define	PRINT_LOG(s)			//
#define	PRINT_FN_LOG(pn,s)		//
#endif


/*--- Function Prototype ---------------------------------------------------*/
int to_raw_main(int argc, char *argv[],UPCONV_PARAM *p);
int dsf_main(int argc, char *argv[],UPCONV_PARAM *p);
int to_wav_main(int argc, char *argv[],UPCONV_PARAM *p);
extern double normalNoise(void);
extern int mc_main(int argc, char *argv[],PARAM_NX *param_nx,UPCONV_PARAM *p);
extern int chkAbort(int percent,int percent_diff);

extern int log10_lf(double n);

/*--- Wav Function Prototype ---------------------------------------------------*/
int updateLevelInfo(SSIZE inSample,int ch_num,FIO *fp_r1,FIO *fp_r2,FIO *fp_r3,FIO *fp_r4,FIO *fp_r5,FIO *fp_r6,LEVEL_INFO *levelInfo_from,LEVEL_INFO *levelInfo_to,UPCONV_PARAM *p);
int updateLevel(SSIZE inSample,int ch_num,FIO *fp_r,FIO *fp_w,LEVEL_INFO *levelInfo_from,LEVEL_INFO *levelInfo_to,UPCONV_PARAM *p);
void JoinFiles(char *workfname,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
void OutputMidSide(FIO *fp_l,FIO *fp_r,FIO *fp_m,FIO *fp_s,SSIZE nSample,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
SSIZE CopyData(FIO *fp_r,FIO *fp_w,SSIZE nSample,SSIZE *buffer,double per,double clip_nx,int norm_flag,int *error);
SSIZE CopyDataAdj(FIO *fp_r,FIO *fp_w,SSIZE nSample,SSIZE *buffer,int *error,PARAM_INFO_TO_WAV *param);
static SSIZE copyFile(FIO *fp_r,FIO *fp_w,void *in,SSIZE buffer_size);
int Normalize(int *nCount,SOUNDFMT *inFmt,SOUNDFMT *outFmt,FIO *fp_r1,FIO *fp_r2,FIO *fp_r3,FIO *fp_r4,FIO *fp_r5,FIO *fp_r6,SSIZE *start,SSIZE nSample,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
int Normalize_Mx(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
int Normalize_M0(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
int Normalize_M1(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
int Normalize_M2(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
int Normalize_M3(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p);
int UpdateBext(BROADCAST_EXT *bext,SOUNDFMT *inFmt,SOUNDFMT *outFmt,PARAM_INFO_TO_WAV *param,long bwf_size,UPCONV_PARAM *p);
int nBitTo64S(int nCh,int ch,int bit,int intFlag,void *in,FIO *fio,SSIZE nSample);
void ms_process_1(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
void ms_process_2(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
void ms_process_3(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
static void outTempFile_(FIO *fp_w,void *in,SSIZE size,PARAM_INFO_TO_RAW *param);
void setup_addp_files(int nCh,FIO *fp_r1,FIO *fp_r2,FIO *fp_r3,FIO *fp_r4,FIO *fp_r5,FIO *fp_r6);

/*--- DSF Function Prototype ---------------------------------------------------*/
int Normalize_DSD(SOUNDFMT *inFmt,SOUNDFMT *outFmt,FIO *fp_r1,FIO *fp_r2,SSIZE nSample,PARAM_INFO_TO_WAV *param);
void dsf_encode(char *in_file,char *out_file,PARAM_INFO_TO_RAW *param);
void dsf_decode(char *in_file,char *out_file,char *argv4,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
static void fftFilter(int lr,SSIZE inSample,SSIZE outSample,FIO *fp_r,FIO *fp_w,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
static void fftFilterSub(SSIZE *lpIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,PARAM_INFO_TO_RAW *param,int id,UPCONV_PARAM *p);
static void LRC_fftFilter(SSIZE inSample1,SSIZE inSample2,FIO *fio1,FIO *fio2,FIO *fio_l,FIO *fio_r,FIO *fio_c,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
static void LRC_fftFilterSub(int type,SSIZE *l_pIn,SSIZE *l_pOut,fftw_complex *l_fftw_in,fftw_complex *l_fftw_out,fftw_plan l_fftw_p,fftw_plan l_fftw_ip,SSIZE *r_pIn,SSIZE *r_pOut,fftw_complex *r_fftw_in,fftw_complex *r_fftw_out,fftw_plan r_fftw_p,fftw_plan r_fftw_ip,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p,int id);
static void LRC_merageTempFile(char type,int normFlag,FIO *fp_r,FIO *fp_r2,FIO *fp_w,SSIZE inSample,int *p_errLine);

void onebit2nbit(SSIZE offset,SSIZE n,SSIZE *buffer,FIO *fp_r,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
void deinterleave(UI64 inByte,FIO *fp_r,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param);
void nbit2onebit(SSIZE *i_buf,BYTE *o_buf,int size);
void ana_abe(SSIZE start,UI64 nSample,SSIZE *i_buffer,SSIZE *o_buffer,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p);
void copyToFFTW(fftw_complex *fftw,SSIZE *buf,long size);
void windowFFTW(fftw_complex *fftw,long size);
void cutFFTW(fftw_complex *fftw,long index,long size);
void *al_malloc(long size);
void *al_free(void *ptr);

double normalNoise(void);

#if 1
#define CLIP_NX(v,nx)	(v) == 0 ? ((SSIZE)0) : \
							((v) > 0 ?	\
								(((SSIZE)(v) * nx) >= (SSIZE)(0x007FFFFFFFFFFFFF) ?		(SSIZE)(0x7FFFFFFFFFFFFF)		: ((SSIZE)((v) * nx))) : \
								(((SSIZE)(v) * nx) <= ((SSIZE)0x007FFFFFFFFFFFFF * -1) ? ((SSIZE)0x7FFFFFFFFFFFFF * -1) : ((SSIZE)((v) * nx))))
#else
#define CLIP_NX(v,nx)	((SSIZE)((v) * (nx)) << 6)
#endif

#define IS_CLIPPED(v)	(v) == 0 ? 0 : ((v) > 0) ? ((v) >> 40) >= (SSIZE)(32439) ? 1 : 0 : ((v) >> 40) <= ((SSIZE)-32439) ? 1 : 0

#define ROUND_NBIT(b)	((SSIZE)1 << (b) - 1) 

#define CLIP_ADD(v,a)	(SSIZE)(v) + (a)

#define CLIP_MAX(v)	(v) == 0 ? ((SSIZE)0) : \
							((v) > 0 ?	\
								((SSIZE)(v) >= (SSIZE)(0x007FFFFFFFFFFFFF) ?		(SSIZE)(0x7FFFFFFFFFFFFFFF)		: ((SSIZE)((v) << 8))) : \
								(((SSIZE)(v)) <= ((SSIZE)0x007FFFFFFFFFFFFF * -1) ? ((SSIZE)0x7FFFFFFFFFFFFFFF * -1) : ((SSIZE)((v) << 8))))

#define CLIP_MAX_N(v,b)	(v) == 0 ? ((SSIZE)0) : \
							((v) > 0 ?	\
								((SSIZE)(v) >= ((((SSIZE)1 << (b)) - 1) >> 1) ?		((((SSIZE)1 << (b)) - 1) >> 1)		: (SSIZE)(v)) : \
								(((SSIZE)(v)) <= (((((SSIZE)1 << (b)) - 1) * -1) >> 1) ? (((((SSIZE)1 << (b)) - 1) * -1) >> 1) : ((SSIZE)(v))))

#define IS_CLIP_MAX(v,nx)	((v) == 0 ? 0 : \
							((v) > 0 ?	\
								((SSIZE)((v) * (double)(nx)) >= (SSIZE)(0x007FFFFFFFFFFFFF) ?	1 : 0) : \
								((SSIZE)((v) * (double)(nx)) <= ((SSIZE)0x007FFFFFFFFFFFFF * -1) ? 1 : 0 )))


//---------------------------------------------------------------------------
// Function   : to_raw_main
// Description: upconv 処理をするため、raw ファイルへ変換する
//
// argv[1] Input  WAV File
// argv[2] Output WAV File
// argv[3] default parameter
// argv[4] parameter
// p       parameter
//

int to_raw_main(int argc, char *argv[],UPCONV_PARAM *p)
{
	static char workpath[_MAX_PATH];
	static char tmppath[_MAX_PATH];
	static char drive[_MAX_DRIVE];
	static char dir[_MAX_DIR];
	static char workdrive[_MAX_DRIVE];
	static char workdir[_MAX_DIR];
	static char workfname[_MAX_FNAME];
	static char workext[_MAX_EXT];
	char fname[_MAX_FNAME];
	char fname2[_MAX_FNAME];
	char ext[_MAX_EXT];
	char work[3072];
	char pparam[3072];
	char sss[100];
	char *p1,*p2;
	unsigned char *disk_buffer;

	long temp;
	int  r_nnn;
	int  r_nnn_remain;
	int LRC_errLine;
	FIO  fp_r;
	FIO  fp_r1,fp_r2,fp_r3,fp_r4,fp_r5,fp_r6;
	FIO  fp_w1,fp_w2,fp_w3,fp_w4,fp_w5,fp_w6;
	FILE *ofp;
	FILE *fp_param;
	FILE *fp_files;
	SOUNDFMT inFmt;

	SSIZE i,inSample,nSample;
	SSIZE out_i;
	fio_size rs,rd;
	fio_size rs1,wr1,rs2,wr2,rs3,wr3,rs4,wr4,rs5,wr5,rs6,wr6;
	fio_size wr;
	fio_size max_size;
	fio_size sec_data_size;
	fio_size file_size;
	int retCode;
	FILEINFO fileInfo;
	STARTEXEC_PROCESS_INFO sp_info[1];

	fio_size seekPtr;
	fio_size rd_byte;
	SSIZE max,avg;
	double percent,per;
	int err1,err2,err3,err4,err5,err6;
//	int thread = 1;
	int fio;
	PARAM_INFO_TO_RAW param_info_to_raw;
	ABORT_INFO abortInfo;

	FIO_COMMON_PARAM fio_common;

	memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));
	fio_common.sector_size = (fio_size)48000 * sizeof (SSIZE);
	fio_common.sector_n    = 10;
	fio_common.upconv_n    = 1;

	memset(&fp_r1,0,sizeof (FIO));
	memset(&fp_r2,0,sizeof (FIO));
	memset(&fp_r3,0,sizeof (FIO));
	memset(&fp_r4,0,sizeof (FIO));
	memset(&fp_r5,0,sizeof (FIO));
	memset(&fp_r6,0,sizeof (FIO));

	memset(&fp_w1,0,sizeof (FIO));
	memset(&fp_w2,0,sizeof (FIO));
	memset(&fp_w3,0,sizeof (FIO));
	memset(&fp_w4,0,sizeof (FIO));
	memset(&fp_w5,0,sizeof (FIO));
	memset(&fp_w6,0,sizeof (FIO));

	disk_buffer = NULL;
	fio = 5;
	errLine = 0;
	sec_data_size = 0;
	strcpy(w2r_log_filename,"C:\\ProgramData\\upconv\\wav2raw.log");
	pparam[0] = '\0';
	if (argc == 4 || argc == 5 || argc == 6) {
		do {
			memset(&NormInfoWav2,0,sizeof (NORM_INFO));
			memset(&fileInfo,0,sizeof (FILEINFO));
			memset(&param_info_to_raw,0,sizeof (PARAM_INFO_TO_RAW));
			param_info_to_raw.l_min = 0;
			param_info_to_raw.l_max = 0;
			param_info_to_raw.r_min = 0;
			param_info_to_raw.r_max = 0;
//			param_info_to_raw.thread = 1;
			param_info_to_raw.n_sample = 0;
			param_info_to_raw.w2r_dsf_mode = 0;
//			param_info_to_raw.fio = -1;
//			param_info_to_raw.fio_auto = 0;
			param_info_to_raw.hfc = -1;
			param_info_to_raw.enable_hfc = 0;
//			param_info_to_raw.addp = 0;
			param_info_to_raw.addp_overrap = 1;
//			param_info_to_raw.w2r_ms_process = 0;
//			param_info_to_raw.w2r_skip_start_offset = 0;
//			param_info_to_raw.w2r_bitwidth = -1;
//			param_info_to_raw.no_rate_conv = -1;

			abortInfo.abort_percent = 0;
			abortInfo.abort_check_interval = 5;
			strcpy(param_info_to_raw.debug_upconv,"to_raw_main");

			if (argc != 4) {
				// default parameter ファイル
				fp_param = fopen(argv[3],"r");
				if (fp_param == NULL) {
					retCode = STATUS_PARAMETER_ERR;errLine = __LINE__;
					break;
				}
				
				// パラメータの読みこみ
				if (fgets(work,2047,fp_param) == NULL) {
					retCode = STATUS_PARAMETER_ERR;errLine = __LINE__;
					break;
				}
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				strcat(pparam,work);
				strcat(pparam," ");
				if (strlen(argv[4]) >= 1) strcat(pparam,argv[4]);

				// tmpファイル用の作業ディレクトリ
				if (fgets(workpath,_MAX_PATH,fp_param) == NULL) {
					retCode = STATUS_PARAMETER_ERR;errLine = __LINE__;
					break;
				}
				p1 = strrchr(workpath,'\n');if (p1 != NULL) *p1 = '\0';
				if (strlen(workpath) >= 2 && workpath[strlen(workpath) - 1] != '\\') strcat(workpath,"\\");

				param_info_to_raw.workpath = strdup(workpath);
				fclose(fp_param);
				fp_param = NULL;
			} else {
				param_info_to_raw.workpath = strdup("");
			}

			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"err");
			unlink(tmppath);

			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"files");
			fp_files = fopen(tmppath,"w");
			if (fp_files == NULL) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}

			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"param");
			fp_param = fopen(tmppath,"w");
			if (fp_param == NULL) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			fprintf(fp_files,"%s\n",tmppath);

			p1 = pparam;
			p2 = strchr(p1,(int)' ');
			if (argc == 6 && strlen(argv[5]) > 0) {
				strcpy(w2r_log_filename,argv[5]);
			}
			PRINT_FN_LOG(param_info_to_raw.debug_upconv,"main:start");


			param_info_to_raw.hfc = p->hfc;
			param_info_to_raw.lfc = p->lfc;

			// 音声ファイル情報を取得する
			retCode = PLG_InfoAudioData(argv[1],&inFmt,&inSample,&fileInfo);
			if (retCode != STATUS_SUCCESS) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}

			PRINT_LOG("PLG_InfoAudioData");
			PRINT_LOG(inFmt.fmt);
			PRINT_LOG(inFmt.extra);

			
			if (inFmt.channel < 1 || inFmt.channel > 6) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			
			if (strcmp(inFmt.fmt,"wav") != 0 && strcmp(inFmt.fmt,"dsf") != 0 && strcmp(inFmt.fmt,"rf64") != 0) {
				char *arg[4];
				char arg0[4096];
				char arg1[4096];
				char arg2[4096];
				char arg3[4096];
				char dec_wav[4096];
				_splitpath(argv[0],drive,dir,fname,ext);
				sprintf(arg0,"%s%s%s",drive,dir,"ffmpeg.exe");
				sprintf(arg1,"|-y -i");
				sprintf(arg2,"%s",argv[1]);
				sprintf(arg3,"%s_decode.wav",argv[2]);
				if ((strcmp(inFmt.fmt,"flac") == 0) || (strcmp(inFmt.fmt,"wavp") == 0)) {
					p->ms_process = 0;
				}
				strcpy(dec_wav,argv[2]);strcat(dec_wav,"_decode.wav");
				arg[0] = arg0;
				arg[1] = arg1;
				arg[2] = arg2;
				arg[3] = arg3;
				sprintf(work,"argv:[%s,%s,%s,%s]",arg0,arg1,arg2,arg3);
				PRINT_LOG(work);
				memset(&sp_info[0],0,sizeof (STARTEXEC_PROCESS_INFO));
				if (start_exec(4,arg,p->cpu_pri,&sp_info[0]) == -1) {
					retCode = STATUS_EXEC_FAIL;errLine = __LINE__;
					break;
				}
				wait_exec(&sp_info[0],1);
				finish_exec(&sp_info[0]);

				retCode = PLG_InfoAudioData(dec_wav,&inFmt,&inSample,&fileInfo);
				if (retCode != STATUS_SUCCESS) {
					PRINT_LOG("Decode Error");
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (strcmp(inFmt.fmt,"wav") != 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (inFmt.sample == 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (p->skip_start_offset > 0 && p->skip_start_offset + 1 >= (inSample / inFmt.sample)) {
					retCode = STATUS_PARAMETER_ERR;errLine = __LINE__;
					break;
				}
				if (p->skip_start_offset > 0 && p->skip_start_offset + ((p->conv_test + 1) * inFmt.sample) >= inSample) {
					retCode = STATUS_PARAMETER_ERR;errLine = __LINE__;
					break;
				}

				// 1秒間のデータサイズ
				switch (inFmt.bitsPerSample) {
					case 16:
						sec_data_size = (fio_size)inFmt.sample * inFmt.channel * sizeof (short);
						break;
					case 20:
					case 24:
						sec_data_size = (fio_size)inFmt.sample * inFmt.channel * 3;
						break;
					case 32:
						if (inFmt.bitsPerSampleInt == 1) {
							sec_data_size = (fio_size)inFmt.sample * inFmt.channel * 4;
						} else {
							sec_data_size = (fio_size)inFmt.sample * sizeof (float);
						}
						break;
					case 64:
						sec_data_size = (fio_size)inFmt.sample * inFmt.channel * sizeof (double);
						break;
				}
				if (sec_data_size == 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				// 元の入力ファイル名を保存(後でメタ情報の復元時に使用する)
				fprintf(fp_param,"%s\n",argv[1]);
				strcpy(argv[1],dec_wav);
				
				fprintf(fp_files,"%s\n",dec_wav);
				_splitpath(argv[2],drive,dir,fname,ext);
				_makepath(tmppath,drive,dir,fname,"wav.param");
				fprintf(fp_files,"%s\n",tmppath);
			} else {
				fprintf(fp_param,"%s\n",argv[1]);
				if (strcmp(inFmt.fmt,"dsf") == 0) {
					p->ms_process = 0;
				}
				if (strcmp(inFmt.fmt,"dsf") == 0) {
					if (p->skip_start_offset > 0 && p->skip_start_offset + 1 >= inSample) {
						retCode = STATUS_PARAMETER_ERR;errLine = __LINE__;
						break;
					}
					// 1秒間のデータサイズ(DSD) はあとで計算する
				} else {
					if (p->skip_start_offset > 0 && (p->skip_start_offset + 1) * (fio_size)inFmt.sample >= inSample) {
						retCode = STATUS_PARAMETER_ERR;errLine = __LINE__;
						break;
					}
					// 1秒間のデータサイズ
					switch (inFmt.bitsPerSample) {
						case 16:
							sec_data_size = (fio_size)inFmt.sample * inFmt.channel * sizeof (short);
							break;
						case 20:
						case 24:
							sec_data_size = (fio_size)inFmt.sample * inFmt.channel * 3;
							break;
						case 32:
							if (inFmt.bitsPerSampleInt == 1) {
								sec_data_size = (fio_size)inFmt.sample * inFmt.channel * 4;
							} else {
								sec_data_size = (fio_size)inFmt.sample * sizeof (float);
							}
							break;
						case 64:
							sec_data_size = (fio_size)inFmt.sample * inFmt.channel * sizeof (double);
							break;
					}
					if (sec_data_size == 0) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
				}
			}
			_makepath(tmppath,drive,dir,fname,"level");
			fprintf(fp_files,"%s\n",tmppath);

			_makepath(tmppath,drive,dir,fname,"ms4");
			fprintf(fp_files,"%s\n",tmppath);

			_makepath(tmppath,drive,dir,fname,"param");
			fprintf(fp_files,"%s\n",tmppath);

			// r1,r2,r3,r4,r5,r6 ファイルの出力先パスファイル生成
			_splitpath(argv[2],drive,dir,fname,ext);
			_splitpath(workpath,workdrive,workdir,workfname,workext);
			if (strlen(workdrive) == 2 && strlen(workdir) >= 1) {
				strcpy(workfname,fname);
			} else {
				strcpy(workdrive,drive);
				strcpy(workdir,dir);
				strcpy(workfname,fname);
			}
			sprintf(work,"workpath:%s,%s,%s,%s",workdrive,workdir,workfname,workext);
			PRINT_LOG(work);

#ifdef _OPENMP
	omp_set_num_threads(p->thread);
#endif
			if (strcmp(inFmt.fmt,"dsf") == 0) {
				/* DSF */
				strcpy(param_info_to_raw.pr_str,"[dsf2raw]");
				fprintf(stdout,"%s0%%\n",param_info_to_raw.pr_str);
				fflush(stdout);
				fclose(fp_param);fp_param = NULL;
				fclose(fp_files);fp_files = NULL;
				param_info_to_raw.w2r_inSampleR = inFmt.sample;
				param_info_to_raw.w2r_decode = 1;
				PRINT_LOG("*");
				if (argc != 4) {
					dsf_decode(argv[1],argv[2],argv[4],&param_info_to_raw,p);
				} else {
					dsf_decode(argv[1],argv[2],argv[3],&param_info_to_raw,p);
				}
				PRINT_LOG("*");
				if (param_info_to_raw.err != STATUS_SUCCESS) {
					retCode = param_info_to_raw.err;errLine = __LINE__;
					break;
				}
				PRINT_LOG("*");
				_splitpath(argv[2],drive,dir,fname,ext);
				_makepath(tmppath,drive,dir,fname,"files");
				fp_files = fopen(tmppath,"a");
				if (fp_files == NULL) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (inFmt.sample == 2822400) {
					inFmt.sample = 192000;
				} else if (inFmt.sample == 2822400*2) {
					inFmt.sample = 192000 * 2;
				} else if (inFmt.sample == 2822400*4) {
					inFmt.sample = 192000 * 4;
				}
				// 1秒間のデータサイズ
				sec_data_size = (fio_size)inFmt.sample * inFmt.channel * 8;
				if (sec_data_size == 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				max_size = inSample * inFmt.sample;	// この inSample は秒数
				max_size *= sizeof (SSIZE);
				if (p->conv_test > 0 && inSample > p->conv_test) {
					inSample = p->conv_test;
					max_size = inSample * inFmt.sample;
					max_size *= sizeof (SSIZE);
				}
				inSample = inSample * inFmt.sample;

				disk_buffer = (unsigned char *)malloc(8 * inFmt.sample * 10);
				if (disk_buffer == NULL) {
					retCode = STATUS_MEM_ALLOC_ERR;errLine = __LINE__;
					break;
				}
			} else {
				/* WAV */
				if (p->simple_mode == 0) {
					strcpy(param_info_to_raw.pr_str,"[wav2raw]");
					fprintf(stdout,"%s0%%\n",param_info_to_raw.pr_str);
					fflush(stdout);
				}
				// 入力ファイルオープン
				//fio_open(&fp_r,argv[1],FIO_MODE_R);
				if (p->fio_sec_size > 0) {
					// 入力のn秒分
					fio_common.sector_size = (fio_size)inFmt.sample * sizeof (SSIZE);
					fio_common.sector_n    = p->fio_sec_size;
					fio_common.upconv_n    = 1;
				} else {
					// n MB 分
					fio_common.sector_size = (fio_size)1 * 1000 * 1000;
					fio_common.sector_n    = p->fio_mb_size;
					fio_common.upconv_n    = 1;
				}
				fio_open_with_set_param(&fp_r,argv[1],FIO_MODE_R,&fio_common);
				if (fp_r.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_get_filesize(&fp_r,&file_size);
				if (1) {
					char sss[300];
					sprintf(sss,"wav2raw:wav filesize:%lld",file_size);
					PRINT_LOG(sss);
				}

#ifdef UPCONV_LOG
				fio_set_logfn(&fp_r,"wav2raw");
				fio_set_log_remain(&fp_r,50);
#endif
				seekPtr = (fio_size)fileInfo.dataOffset;
				fio_seek(&fp_r,seekPtr,SEEK_SET);
				if (fp_r.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				// パラメーターの追加
				sprintf(work," -is:%d -iw:%d -ch:%d",inFmt.sample,inFmt.bitsPerSample,inFmt.channel);
				param_info_to_raw.w2r_inSampleR = inFmt.sample;
				param_info_to_raw.w2r_outSampleR = inFmt.sample;
				if (argc != 4) {
					strcat(argv[4],work);
				} else {
					strcat(argv[3],work);
				}
				p->inSampleR = inFmt.sample;
				p->iw = inFmt.bitsPerSample;
				p->ch = inFmt.channel;
				p->realCh = inFmt.channel;
				// EQ 用のパラメータ追加
				if (p->no_rate_conv == 0) {
					p->outSampleR = inFmt.sample;
				} else if (p->no_rate_conv == 1) {
					p->outSampleR = 44100;
				} else if (p->no_rate_conv == 2) {
					p->outSampleR = 48000;
				} else if (p->no_rate_conv == 3) {
					p->outSampleR = 96000;
				}
				if (p->no_rate_conv >= 0 && p->w == -1) {
					p->w = inFmt.bitsPerSample;
					p->bitwidth_int = 0;
					if (inFmt.bitsPerSample == 32 && inFmt.bitsPerSampleInt == 1) {
						p->bitwidth_int = 1;
					}
				}

				max_size = inSample;
				max_size *= sizeof (SSIZE);
				if (p->conv_test > 0 && inSample >= inFmt.sample * p->conv_test) {
					inSample = inFmt.sample * p->conv_test;
					max_size = inSample;
					max_size *= sizeof (SSIZE);
				}

				
				// 読み込みバッファサイズ計算
				if (inFmt.bitsPerSample != 20) {
					rs = (inFmt.bitsPerSample / 8) * inFmt.channel;
				} else {
					rs = (24 / 8) * inFmt.channel;
				}

				disk_buffer = (unsigned char *)malloc((8 * inFmt.channel) * inFmt.sample * 10);
				if (disk_buffer == NULL) {
					retCode = STATUS_MEM_ALLOC_ERR;errLine = __LINE__;
					break;
				}
				// 変換と書き出し
				// 出力ファイル名の作成(Left)
				//_makepath(tmppath,workdrive,workdir,workfname,"r1");
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r1");
				PRINT_LOG(tmppath);
				fio_common.upconv_n = inFmt.channel;
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fprintf(fp_files,"%s\n",tmppath);
				fprintf(fp_files,"%s.tmp\n",tmppath);
				fprintf(fp_files,"%s.tmp2\n",tmppath);
				fio_set_maxsize(&fp_w1,max_size);

#ifdef UPCONV_LOG
				fio_set_logfn(&fp_w1,"wav2raw");
				fio_set_log_remain(&fp_w1,50);
#endif
				if (inFmt.channel >= 2) {
					// 出力ファイル名の作成(Right)
					//_makepath(tmppath,workdrive,workdir,workfname,"r2");
					sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r2");
					PRINT_LOG(tmppath);
					//fio_open(&fp_w2,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w2.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
					fprintf(fp_files,"%s\n",tmppath);
					fprintf(fp_files,"%s.tmp\n",tmppath);
					fprintf(fp_files,"%s.tmp2\n",tmppath);
					fio_set_maxsize(&fp_w2,max_size);
					if (p->ms_process == 4) {
						sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"ms4");
						param_info_to_raw.file_ms4 = strdup(tmppath);
//						fprintf(fp_files,"%s\n",tmppath);
					}
				}
				if (inFmt.channel >= 3) {
					// 出力ファイル名の作成(3)
					//_makepath(tmppath,workdrive,workdir,workfname,"r3");
					sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r3");
					PRINT_LOG(tmppath);
					//fio_open(&fp_w3,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w3,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w3.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
					fprintf(fp_files,"%s\n",tmppath);
					fprintf(fp_files,"%s.tmp\n",tmppath);
					fprintf(fp_files,"%s.tmp2\n",tmppath);
					fio_set_maxsize(&fp_w3,max_size);
				}
				if (inFmt.channel >= 4) {
					// 出力ファイル名の作成(4)
					//_makepath(tmppath,workdrive,workdir,workfname,"r4");
					sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r4");
					PRINT_LOG(tmppath);
					//fio_open(&fp_w4,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w4,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w4.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
					fprintf(fp_files,"%s\n",tmppath);
					fprintf(fp_files,"%s.tmp\n",tmppath);
					fprintf(fp_files,"%s.tmp2\n",tmppath);
					fio_set_maxsize(&fp_w4,max_size);
				}
				if (inFmt.channel >= 5) {
					// 出力ファイル名の作成(5)
					//_makepath(tmppath,workdrive,workdir,workfname,"r5");
					sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r5");
					PRINT_LOG(tmppath);
					//fio_open(&fp_w5,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w5,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w5.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
					fprintf(fp_files,"%s\n",tmppath);
					fprintf(fp_files,"%s.tmp\n",tmppath);
					fprintf(fp_files,"%s.tmp2\n",tmppath);
					fio_set_maxsize(&fp_w5,max_size);
				}
				if (inFmt.channel >= 6) {
					// 出力ファイル名の作成(6)
					//_makepath(tmppath,workdrive,workdir,workfname,"r6");
					sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r6");
					PRINT_LOG(tmppath);
					// 出力ファイルオープン
					//fio_open(&fp_w6,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w6,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w6.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
					fprintf(fp_files,"%s\n",tmppath);
					fprintf(fp_files,"%s.tmp\n",tmppath);
					fprintf(fp_files,"%s.tmp2\n",tmppath);
					fio_set_maxsize(&fp_w6,max_size);
				}
				if (p->LRC_process == 1) {
					sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r3");
					PRINT_LOG(tmppath);
					fprintf(fp_files,"%s\n",tmppath);
					fprintf(fp_files,"%s.tmp\n",tmppath);
					fprintf(fp_files,"%s.tmp2\n",tmppath);
				}
				err1 = err2 = err3 = err4 = err5 = err6 = 0;
				per = -1;

				i = 0;
				out_i = 0;
				if (p->skip_start_offset > 0) {
					fio_seek(&fp_r,sec_data_size * p->skip_start_offset,SEEK_CUR);
					i = p->skip_start_offset;
				}
				
				for (;i < inSample;i += inFmt.sample * 10,out_i += inFmt.sample * 10) {
					percent = ((double)i / inSample);
					percent *= 100;
					if (percent != per) {
						if (p->simple_mode == 0) {
							fprintf(stdout,"%s%d%%\n",param_info_to_raw.pr_str,(int)percent);
							fflush(stdout);
						} else {
							// step 1 / 5
							if (p->simple_mode_done_percent_div != 0) {
								fprintf(stdout,"%d%%\n",(int)(percent / p->simple_mode_done_percent_div) + p->simple_mode_done_percent);
								fflush(stdout);
							}
						}
						if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);

						per = percent;
					}

					if (p->conv_test > 0 && out_i >= inFmt.sample * p->conv_test) {
						break;
					}

					if (err1 || err2 || err3 || err4 || err5 || err6) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
					nSample = inFmt.sample * 10;
					if (i + nSample > inSample) {
						nSample = inSample - i;
					}
					rd = fio_read(disk_buffer,rs,nSample,&fp_r);
					if (fp_r.error) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd == 0) {
						break;
					}
					#pragma omp parallel
					{
						#pragma omp sections
						{
							#pragma omp section
							{
								if (nBitTo64S(inFmt.channel,0,inFmt.bitsPerSample,inFmt.bitsPerSampleInt,disk_buffer,&fp_w1,rd)) {
									err1 = 1;
								}
							}
							#pragma omp section
							{
								if (inFmt.channel >= 2) {
									if (nBitTo64S(inFmt.channel,1,inFmt.bitsPerSample,inFmt.bitsPerSampleInt,disk_buffer,&fp_w2,rd)) {
										err2 = 1;
									}
								}
							}
							#pragma omp section
							{
								if (inFmt.channel >= 3) {
									if (nBitTo64S(inFmt.channel,2,inFmt.bitsPerSample,inFmt.bitsPerSampleInt,disk_buffer,&fp_w3,rd)) {
										err3 = 1;
									}
								}
							}
							#pragma omp section
							{
								if (inFmt.channel >= 4) {
									if (nBitTo64S(inFmt.channel,3,inFmt.bitsPerSample,inFmt.bitsPerSampleInt,disk_buffer,&fp_w4,rd)) {
										err4 = 1;
									}
								}
							}
							#pragma omp section
							{
								if (inFmt.channel >= 5) {
									if (nBitTo64S(inFmt.channel,4,inFmt.bitsPerSample,inFmt.bitsPerSampleInt,disk_buffer,&fp_w5,rd)) {
										err5 = 1;
									}
								}
							}
							#pragma omp section
							{
								if (inFmt.channel >= 6) {
									if (nBitTo64S(inFmt.channel,5,inFmt.bitsPerSample,inFmt.bitsPerSampleInt,disk_buffer,&fp_w6,rd)) {
										err6 = 1;
									}
								}
							}
						}
					}
				}
				fio_close(&fp_r);
				if (fp_r.error) {
					PRINT_LOG("fp_r.error");
				}
				fio_close(&fp_w1);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (inFmt.channel >= 2) {
					fio_close(&fp_w2);
					if (fp_w2.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
				}
				if (inFmt.channel >= 3) {
					fio_close(&fp_w3);
					if (fp_w3.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
				}
				if (inFmt.channel >= 4) {
					fio_close(&fp_w4);
					if (fp_w4.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
				}
				if (inFmt.channel >= 5) {
					fio_close(&fp_w5);
					if (fp_w5.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
				}
				if (inFmt.channel >= 6) {
					fio_close(&fp_w6);
					if (fp_w6.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
				}

				if (retCode != STATUS_SUCCESS) {
					break;
				}

				p->simple_mode_done_percent_div = 0;
				if (NormInfoWav2.clipR1 || NormInfoWav2.clipR2 || NormInfoWav2.clipR3 || NormInfoWav2.clipR4 || NormInfoWav2.clipR5 || NormInfoWav2.clipR6) {
					p->clipped = 1;
				}

				// 音量記録用ファイル名の作成
				if (1) {
					FIO *p_r1,*p_r2,*p_r3,*p_r4,*p_r5,*p_r6;
					FILE *fp_level;
					p_r1 = p_r2 = p_r3 = p_r4 = p_r5 = p_r6 = NULL;
					_makepath(tmppath,workdrive,workdir,workfname,"r1");
					//fio_open(&fp_r1,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r1.error) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					p_r1 = &fp_r1;
					if (inFmt.channel >= 2) {
						_makepath(tmppath,workdrive,workdir,workfname,"r2");
						//fio_open(&fp_r2,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r2.error) {
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r2 = &fp_r2;
					}
					if (inFmt.channel >= 3) {
						_makepath(tmppath,workdrive,workdir,workfname,"r3");
						//fio_open(&fp_r3,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r3,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r3.error) {
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r3 = &fp_r3;
					}
					if (inFmt.channel >= 4) {
						_makepath(tmppath,workdrive,workdir,workfname,"r4");
						//fio_open(&fp_r4,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r4,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r4.error) {
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r4 = &fp_r4;
					}
					if (inFmt.channel >= 5) {
						_makepath(tmppath,workdrive,workdir,workfname,"r5");
						//fio_open(&fp_r5,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r5,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r5.error) {
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r5 = &fp_r5;
					}
					if (inFmt.channel >= 6) {
						_makepath(tmppath,workdrive,workdir,workfname,"r6");
						//fio_open(&fp_r6,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r6,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r6.error) {
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r6 = &fp_r6;
					}

					sprintf(sss,"updateLevelInfo1:inSample:%lld",inSample);
					PRINT_LOG(sss);
					if (updateLevelInfo(inSample,inFmt.channel,p_r1,p_r2,p_r3,p_r4,p_r5,p_r6,&Wav2RawLevelInfo,NULL,p) < 0) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}

					_makepath(tmppath,drive,dir,fname,"level");
					fp_level = fopen(tmppath,"w");
					if (fp_level) {
						fprintf(fp_level,"volume_level:%lf\n",Wav2RawLevelInfo.level_per);
						fprintf(fp_level,"r1:%lld,%ld,%lld,%ld\n",Wav2RawLevelInfo.r1_avg,Wav2RawLevelInfo.r1_peak,Wav2RawLevelInfo.r1_m_avg,Wav2RawLevelInfo.r1_m_peak);
						fprintf(fp_level,"r2:%lld,%ld,%lld,%ld\n",Wav2RawLevelInfo.r2_avg,Wav2RawLevelInfo.r2_peak,Wav2RawLevelInfo.r2_m_avg,Wav2RawLevelInfo.r2_m_peak);
						fprintf(fp_level,"r3:%lld,%ld,%lld,%ld\n",Wav2RawLevelInfo.r3_avg,Wav2RawLevelInfo.r3_peak,Wav2RawLevelInfo.r3_m_avg,Wav2RawLevelInfo.r3_m_peak);
						fprintf(fp_level,"r4:%lld,%ld,%lld,%ld\n",Wav2RawLevelInfo.r4_avg,Wav2RawLevelInfo.r4_peak,Wav2RawLevelInfo.r4_m_avg,Wav2RawLevelInfo.r4_m_peak);
						fprintf(fp_level,"r5:%lld,%ld,%lld,%ld\n",Wav2RawLevelInfo.r5_avg,Wav2RawLevelInfo.r5_peak,Wav2RawLevelInfo.r5_m_avg,Wav2RawLevelInfo.r5_m_peak);
						fprintf(fp_level,"r6:%lld,%ld,%lld,%ld\n",Wav2RawLevelInfo.r6_avg,Wav2RawLevelInfo.r6_peak,Wav2RawLevelInfo.r6_m_avg,Wav2RawLevelInfo.r6_m_peak);
						fclose(fp_level);
					}
					if (p_r1 != NULL) fio_close(&fp_r1);
					if (p_r2 != NULL) fio_close(&fp_r2);
					if (p_r3 != NULL) fio_close(&fp_r3);
					if (p_r4 != NULL) fio_close(&fp_r4);
					if (p_r5 != NULL) fio_close(&fp_r5);
					if (p_r6 != NULL) fio_close(&fp_r6);
				}
				if (p->level_scan_only == 1) {
					break;
				}

				if (p->conv_test > 0 && inSample >= inFmt.sample * p->conv_test) {
					inSample = inFmt.sample * p->conv_test;
				}
			}

			// MS 処理(MS -> LRC の場合も含む)
			if (inFmt.channel == 2 && p->ms_process != 0 && p->MS_LRC_mode == 1) {
				strcpy(param_info_to_raw.pr_str,"[MS1]");
				// 入力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_common.upconv_n    = 2;
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 入力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				//fio_open(&fp_r2,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				// 出力ファイル名の作成(Mid)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				// x2 でオーバーサンプリングしてからMS処理を実施する
				fio_set_maxsize(&fp_w1,max_size * 2);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				// 出力ファイル名の作成(Side)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size * 2);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				ms_process_1(inSample,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&param_info_to_raw,p);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				fio_close(&fp_r2);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w1,&fp_r1,tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w2,&fp_r2,tmppath);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Mid)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w1,max_size * 2);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				// 出力ファイル名の作成(Side)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size * 2);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				strcpy(param_info_to_raw.pr_str,"[MS2]");
				ms_process_2(inSample * 2,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&param_info_to_raw,p);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r2);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w1,&fp_r1,tmppath);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w2,&fp_r2,tmppath);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				// 出力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w1,max_size);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				// 出力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}

				fio_set_maxsize(&fp_w2,max_size);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				strcpy(param_info_to_raw.pr_str,"[MS3]");
				ms_process_3(inSample * 2,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&param_info_to_raw,p);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				fio_close(&fp_r2);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w1,&fp_r1,tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w2,&fp_r2,tmppath);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				fio_close(&fp_r2);
			}

			// LRC -> MS の場合
			if (inFmt.channel == 2 && p->ms_process != 0 && p->LRC_process == 1 && p->MS_LRC_mode == 2) {
				SSIZE wSample;
				// 入力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_common.upconv_n    = 1;
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.backup");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				for (i = 0;i < inSample;i += inFmt.sample * 10) {
					nSample = inFmt.sample * 10;
					if (i + nSample > inSample) {
						nSample = inSample - i;
					}
					wSample = copyFile(&fp_r1,&fp_w1,disk_buffer,nSample);
					if (wSample != nSample) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
				}
				if (retCode != STATUS_SUCCESS) break;
				fio_close(&fp_r1);
				fio_close(&fp_w1);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				// 入力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.backup");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				for (i = 0;i < inSample;i += inFmt.sample * 10) {
					nSample = inFmt.sample * 10;
					if (i + nSample > inSample) {
						nSample = inSample - i;
					}
					wSample = copyFile(&fp_r1,&fp_w1,disk_buffer,nSample);
					if (wSample != nSample) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
				}
				if (retCode != STATUS_SUCCESS) break;
				fio_close(&fp_r1);
				fio_close(&fp_w1);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
			}

			// Left / Right / Center 処理 
			if (p->LRC_process == 1 && inFmt.channel == 2) {
				SSIZE r1_size,r2_size,r3_size;
				strcpy(param_info_to_raw.pr_str,"[LRC1]");

				// 入力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_common.upconv_n    = 2;
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 入力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				//fio_open(&fp_r2,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				fio_common.upconv_n    = 3;
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				fio_set_maxsize(&fp_w1,max_size);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Center)
				_makepath(tmppath,workdrive,workdir,workfname,"r3");
				//fio_open(&fp_w3,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w3,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w3.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w3,max_size);
				//fio_set_memory_limit(&fp_w3,20,p->fio);
				//fio_set_memory_limit(&fp_w3,inFmt.sample * sizeof (SSIZE),p->fio);
				LRC_fftFilter(inSample,inSample,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&fp_w3,&param_info_to_raw,p);
				PRINT_LOG("LRC_fftFilter");
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w3.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				if (fp_r1.error) {
					PRINT_LOG("fp_r1.error");
				}
				fio_close(&fp_r2);
				if (fp_r2.error) {
					PRINT_LOG("fp_r2.error");
				}
				fio_setmode_r(&fp_w1,&fp_r1,NULL);
				fio_setmode_r(&fp_w2,&fp_r2,NULL);
				fio_setmode_r(&fp_w3,&fp_r3,NULL);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r3.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_common.upconv_n    = 2;
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				fio_set_maxsize(&fp_w1,max_size);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * sizeof (SSIZE),p->fio);

				// Left から Center を引く
				LRC_errLine = 0;
				LRC_merageTempFile('-',0,&fp_r1,&fp_r3,&fp_w1,inSample,&LRC_errLine);
				if (LRC_errLine != 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = LRC_errLine;
					break;
				}
				// Right から Center を引く
				LRC_errLine = 0;
				LRC_merageTempFile('-',0,&fp_r2,&fp_r3,&fp_w2,inSample,&LRC_errLine);
				if (LRC_errLine != 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = LRC_errLine;
					break;
				}
				fio_get_filesize(&fp_w1,&r1_size);
				fio_get_filesize(&fp_w2,&r2_size);
				fio_get_filesize(&fp_r3,&r3_size);
				if (!(r1_size == r2_size && r2_size == r3_size && r3_size == max_size)) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}

				fio_close(&fp_r1);
				if (fp_r1.error) {
					PRINT_LOG("fp_r1.error");
				}
				fio_close(&fp_r2);
				if (fp_r2.error) {
					PRINT_LOG("fp_r2.error");
				}
				fio_close(&fp_r3);
				if (fp_r3.error) {
					PRINT_LOG("fp_r3.error");
				}
				fio_close(&fp_w1);
				if (fp_w1.error) {
					PRINT_LOG("fp_w1.error");
				}
				fio_close(&fp_w2);
				if (fp_w2.error) {
					PRINT_LOG("fp_w2.error");
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				unlink(tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				unlink(tmppath);
			}

			// Center バックアップ
			if (inFmt.channel == 2 && p->ms_process != 0 && p->LRC_process == 1 && p->MS_LRC_mode == 2) {
				SSIZE wSample;
				char *tmp_name;
				// 入力ファイル名の作成(Center)
				_makepath(tmppath,workdrive,workdir,workfname,"r3");
				fio_common.upconv_n    = 1;
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Center)
				_makepath(tmppath,workdrive,workdir,workfname,"r3.backup");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				for (i = 0;i < inSample;i += inFmt.sample * 10) {
					nSample = inFmt.sample * 10;
					if (i + nSample > inSample) {
						nSample = inSample - i;
					}
					wSample = copyFile(&fp_r1,&fp_w1,disk_buffer,nSample);
					if (wSample != nSample) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
				}
				if (retCode != STATUS_SUCCESS) break;
				fio_close(&fp_r1);
				fio_close(&fp_w1);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}

				// Left 戻し
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				unlink(tmppath);
				tmp_name = strdup(tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r1.backup");
				if(rename(tmppath,tmp_name) != 0) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (tmp_name != NULL) free(tmp_name);

				// Right 戻し
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				unlink(tmppath);
				tmp_name = strdup(tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r2.backup");
				if(rename(tmppath,tmp_name) != 0) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (tmp_name != NULL) free(tmp_name);
			}

			// MS 処理
			if (inFmt.channel == 2 && p->ms_process != 0 && p->MS_LRC_mode == 2) {
				strcpy(param_info_to_raw.pr_str,"[MS1]");
				// 入力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_common.upconv_n    = 2;
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 入力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				//fio_open(&fp_r2,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Mid)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				// x2 でオーバーサンプリングしてからMS処理を実施する
				fio_set_maxsize(&fp_w1,max_size * 2);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * 2 * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Side)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size * 2);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				ms_process_1(inSample,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&param_info_to_raw,p);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				fio_close(&fp_r2);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w1,&fp_r1,tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w2,&fp_r2,tmppath);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Mid)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w1,max_size * 2);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * 2 * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Side)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size * 2);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				strcpy(param_info_to_raw.pr_str,"[MS2]");
				ms_process_2(inSample * 2,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&param_info_to_raw,p);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r2);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w1,&fp_r1,tmppath);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w2,&fp_r2,tmppath);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w1,max_size);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * 2 * sizeof (SSIZE),p->fio);
				// 出力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * 2 * sizeof (SSIZE),p->fio);

				strcpy(param_info_to_raw.pr_str,"[MS3]");
				ms_process_3(inSample * 2,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&param_info_to_raw,p);
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				fio_close(&fp_r2);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_setmode_r(&fp_w1,&fp_r1,tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				fio_setmode_r(&fp_w2,&fp_r2,tmppath);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				fio_close(&fp_r2);
			}

			// Left / Right / Center 処理 
			if (p->LRC_process == 1 && inFmt.channel == 2 && p->MS_LRC_mode == 2) {
				SSIZE r1_size,r2_size,r3_size;
				strcpy(param_info_to_raw.pr_str,"[LRC1]");

				// 入力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				fio_common.upconv_n    = 2;
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 入力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				//fio_open(&fp_r2,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				fio_common.upconv_n    = 3;
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				fio_set_maxsize(&fp_w1,max_size);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Center)
				_makepath(tmppath,workdrive,workdir,workfname,"r3");
				//fio_open(&fp_w3,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w3,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w3.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w3,max_size);
				//fio_set_memory_limit(&fp_w3,20,p->fio);
				//fio_set_memory_limit(&fp_w3,inFmt.sample * sizeof (SSIZE),p->fio);
				LRC_fftFilter(inSample,inSample,&fp_r1,&fp_r2,&fp_w1,&fp_w2,&fp_w3,&param_info_to_raw,p);
				PRINT_LOG("LRC_fftFilter");
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w1.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (fp_w3.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fio_close(&fp_r1);
				if (fp_r1.error) {
					PRINT_LOG("fp_r1.error");
				}
				fio_close(&fp_r2);
				if (fp_r2.error) {
					PRINT_LOG("fp_r2.error");
				}
				fio_setmode_r(&fp_w1,&fp_r1,NULL);
				fio_setmode_r(&fp_w2,&fp_r2,NULL);
				fio_setmode_r(&fp_w3,&fp_r3,NULL);
				if (fp_r1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (fp_r3.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				// 出力ファイル名の作成(Left)
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				//fio_open(&fp_w1,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w1.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}

				fio_set_maxsize(&fp_w1,max_size);
				//fio_set_memory_limit(&fp_w1,20,p->fio);
				//fio_set_memory_limit(&fp_w1,inFmt.sample * sizeof (SSIZE),p->fio);

				// 出力ファイル名の作成(Right)
				_makepath(tmppath,workdrive,workdir,workfname,"r2");
				//fio_open(&fp_w2,tmppath,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
				if (fp_w2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				fio_set_maxsize(&fp_w2,max_size);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,inFmt.sample * sizeof (SSIZE),p->fio);

				// Left から Center を引く
				LRC_errLine = 0;
				LRC_merageTempFile('-',0,&fp_r1,&fp_r3,&fp_w1,inSample,&LRC_errLine);
				if (LRC_errLine != 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = LRC_errLine;
					break;
				}
				// Right から Center を引く
				LRC_errLine = 0;
				LRC_merageTempFile('-',0,&fp_r2,&fp_r3,&fp_w2,inSample,&LRC_errLine);
				if (LRC_errLine != 0) {
					retCode = STATUS_FILE_READ_ERR;errLine = LRC_errLine;
					break;
				}
				fio_get_filesize(&fp_w1,&r1_size);
				fio_get_filesize(&fp_w2,&r2_size);
				fio_get_filesize(&fp_r3,&r3_size);
				if (!(r1_size == r2_size && r2_size == r3_size && r3_size == max_size)) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}

				fio_close(&fp_r1);
				if (fp_r1.error) {
					PRINT_LOG("fp_r1.error");
				}
				fio_close(&fp_r2);
				if (fp_r2.error) {
					PRINT_LOG("fp_r2.error");
				}
				fio_close(&fp_r3);
				if (fp_r3.error) {
					PRINT_LOG("fp_r3.error");
				}
				fio_close(&fp_w1);
				if (fp_w1.error) {
					PRINT_LOG("fp_w1.error");
				}
				fio_close(&fp_w2);
				if (fp_w2.error) {
					PRINT_LOG("fp_w2.error");
				}
				_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
				unlink(tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
				unlink(tmppath);
			}
			// Center 戻し
			if (p->LRC_process == 1 && inFmt.channel == 2 && p->MS_LRC_mode == 2) {
				char *tmp_name;
				// Center 戻し
				_makepath(tmppath,workdrive,workdir,workfname,"r3");
				unlink(tmppath);
				tmp_name = strdup(tmppath);
				_makepath(tmppath,workdrive,workdir,workfname,"r3.backup");
				if(rename(tmppath,tmp_name) != 0) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				if (tmp_name != NULL) free(tmp_name);
			}

			// ADDP 用の処理(既存の処理と共通化する)
			// 音量の調査は upconv.c で実施するように変更する。

			// 入力ファイル名の作成(Left)
			//_makepath(tmppath,workdrive,workdir,workfname,"r1");
			sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r1");
			fio_common.upconv_n = inFmt.channel;
			//fio_open(&fp_r1,tmppath,FIO_MODE_R);
			fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
			if (fp_r1.error) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_r1,20,p->fio);
			//fio_set_memory_limit(&fp_r1,inFmt.sample * sizeof (SSIZE),p->fio);
			if (inFmt.channel >= 2) {
				// 入力ファイル名の作成(Right)
				//_makepath(tmppath,workdrive,workdir,workfname,"r2");
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r2");
				//fio_open(&fp_r2,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r2.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				//fio_set_memory_limit(&fp_r2,20,p->fio);
				//fio_set_memory_limit(&fp_r2,inFmt.sample * sizeof (SSIZE),p->fio);
			}
			if (inFmt.channel >= 3 || p->LRC_process == 1) {
				// 入力ファイル名の作成(3)
				//_makepath(tmppath,workdrive,workdir,workfname,"r3");
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r3");
				//fio_open(&fp_r3,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r3,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r3.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				//fio_set_memory_limit(&fp_r3,20,p->fio);
				//fio_set_memory_limit(&fp_r3,inFmt.sample * sizeof (SSIZE),p->fio);
			}
			if (inFmt.channel >= 4) {
				// 入力ファイル名の作成(4)
				//_makepath(tmppath,workdrive,workdir,workfname,"r4");
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r4");
				//fio_open(&fp_r4,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r4,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r4.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				//fio_set_memory_limit(&fp_r4,20,p->fio);
				//fio_set_memory_limit(&fp_r4,inFmt.sample * sizeof (SSIZE),p->fio);
			}
			if (inFmt.channel >= 5) {
				// 入力ファイル名の作成(5)
				//_makepath(tmppath,workdrive,workdir,workfname,"r5");
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r5");
				//fio_open(&fp_r5,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r5,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r5.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				//fio_set_memory_limit(&fp_r5,20,p->fio);
				//fio_set_memory_limit(&fp_r5,inFmt.sample * sizeof (SSIZE),p->fio);
			}
			if (inFmt.channel >= 6) {
				// 入力ファイル名の作成(6)
				//_makepath(tmppath,workdrive,workdir,workfname,"r6");
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r6");
				//fio_open(&fp_r6,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r6,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r6.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				//fio_set_memory_limit(&fp_r6,20,p->fio);
				//fio_set_memory_limit(&fp_r6,inFmt.sample * sizeof (SSIZE),p->fio);
			}
			r_nnn = 1;
			r_nnn_remain = 0;

			if (1) {
				SSIZE size;
				char sss[100];
				fio_get_filesize(&fp_r1,&size);
				sprintf(sss,"r1:%lld",size);PRINT_LOG(sss);
			}

			if (p->addp == 0) {
				param_info_to_raw.addp_overrap = 0;
			}
			for (i = 0;i < inSample;i += inFmt.sample * 10) {
				char sss[100];
				int  addp_overrap_first,addp_overrap_end,addp_overrap_last;
				sprintf(sss,"loop1");
				PRINT_LOG(sss);
				addp_overrap_first = 0;	// 最初の2秒を以前の後ろから2秒のデータとするフラグ
				addp_overrap_end   = 0;	// 最後の2秒を次データのはじめから2秒のデータとするフラグ
				addp_overrap_last  = 0;	// 最後のデータであり、短い場合は分割しないようにする
				if (param_info_to_raw.addp_overrap == 1) {
					if (i > 0 && r_nnn_remain == 0) {
						addp_overrap_first = 1;
					}
				}

				if (r_nnn == 1 || (p->addp > 0 && r_nnn_remain == 0)) {
					// 出力ファイル名の作成(Left)
					//_makepath(tmppath,workdrive,workdir,workfname,"r1");
					sprintf(tmppath,"%s%s%s.%s.%03d",workdrive,workdir,workfname,"r1",r_nnn);
					PRINT_LOG(tmppath);
					fio_common.upconv_n = inFmt.channel;
					if (r_nnn > 1) fio_close(&fp_w1);
					//fio_open(&fp_w1,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w1.error) {
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
					fprintf(fp_files,"%s\n",tmppath);
					fprintf(fp_files,"%s.param\n",tmppath);
					fprintf(fp_files,"%s.tmp\n",tmppath);
					fprintf(fp_files,"%s.tmp2\n",tmppath);
					fio_set_maxsize(&fp_w1,max_size);
					//fio_set_memory_limit(&fp_w1,20,p->fio);
					//fio_set_memory_limit(&fp_w1,inFmt.sample * sizeof (SSIZE),p->fio);

					if (inFmt.channel >= 2) {
						// 出力ファイル名の作成(Right)
						//_makepath(tmppath,workdrive,workdir,workfname,"r2");
						sprintf(tmppath,"%s%s%s.%s.%03d",workdrive,workdir,workfname,"r2",r_nnn);
						PRINT_LOG(tmppath);
						if (r_nnn > 1) fio_close(&fp_w2);
						//fio_open(&fp_w2,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w2.error) {
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
						fprintf(fp_files,"%s\n",tmppath);
						fprintf(fp_files,"%s.param\n",tmppath);
						fprintf(fp_files,"%s.tmp\n",tmppath);
						fprintf(fp_files,"%s.tmp2\n",tmppath);
						fio_set_maxsize(&fp_w2,max_size);
						//fio_set_memory_limit(&fp_w2,20,p->fio);
						//fio_set_memory_limit(&fp_w2,inFmt.sample * sizeof (SSIZE),p->fio);
					}
					if (inFmt.channel >= 3 || p->LRC_process == 1) {
						// 出力ファイル名の作成(3)
						//_makepath(tmppath,workdrive,workdir,workfname,"r3");
						sprintf(tmppath,"%s%s%s.%s.%03d",workdrive,workdir,workfname,"r3",r_nnn);
						PRINT_LOG(tmppath);
						if (r_nnn > 1) fio_close(&fp_w3);
						//fio_open(&fp_w3,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w3,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w3.error) {
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
						fprintf(fp_files,"%s\n",tmppath);
						fprintf(fp_files,"%s.param\n",tmppath);
						fprintf(fp_files,"%s.tmp\n",tmppath);
						fprintf(fp_files,"%s.tmp2\n",tmppath);
						fio_set_maxsize(&fp_w3,max_size);
						//fio_set_memory_limit(&fp_w3,20,p->fio);
						//fio_set_memory_limit(&fp_w3,inFmt.sample * sizeof (SSIZE),p->fio);
					}
					if (inFmt.channel >= 4) {
						// 出力ファイル名の作成(4)
						//_makepath(tmppath,workdrive,workdir,workfname,"r4");
						sprintf(tmppath,"%s%s%s.%s.%03d",workdrive,workdir,workfname,"r4",r_nnn);
						PRINT_LOG(tmppath);
						if (r_nnn > 1) fio_close(&fp_w4);
						//fio_open(&fp_w4,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w4,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w4.error) {
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
						fprintf(fp_files,"%s\n",tmppath);
						fprintf(fp_files,"%s.param\n",tmppath);
						fprintf(fp_files,"%s.tmp\n",tmppath);
						fprintf(fp_files,"%s.tmp2\n",tmppath);
						fio_set_maxsize(&fp_w4,max_size);
						//fio_set_memory_limit(&fp_w4,20,p->fio);
						//fio_set_memory_limit(&fp_w4,inFmt.sample * sizeof (SSIZE),p->fio);
					}
					if (inFmt.channel >= 5) {
						// 出力ファイル名の作成(5)
						//_makepath(tmppath,workdrive,workdir,workfname,"r5");
						sprintf(tmppath,"%s%s%s.%s.%03d",workdrive,workdir,workfname,"r5",r_nnn);
						PRINT_LOG(tmppath);
						if (r_nnn > 1) fio_close(&fp_w5);
						//fio_open(&fp_w5,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w5,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w5.error) {
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
						fprintf(fp_files,"%s\n",tmppath);
						fprintf(fp_files,"%s.param\n",tmppath);
						fprintf(fp_files,"%s.tmp\n",tmppath);
						fprintf(fp_files,"%s.tmp2\n",tmppath);
						fio_set_maxsize(&fp_w5,max_size);
						//fio_set_memory_limit(&fp_w5,20,p->fio);
						//fio_set_memory_limit(&fp_w5,inFmt.sample * sizeof (SSIZE),p->fio);
					}
					if (inFmt.channel >= 6) {
						// 出力ファイル名の作成(6)
						//_makepath(tmppath,workdrive,workdir,workfname,"r6");
						sprintf(tmppath,"%s%s%s.%s.%03d",workdrive,workdir,workfname,"r6",r_nnn);
						PRINT_LOG(tmppath);
						// 出力ファイルオープン
						if (r_nnn > 1) fio_close(&fp_w6);
						//fio_open(&fp_w6,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w6,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w6.error) {
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
						fprintf(fp_files,"%s\n",tmppath);
						fprintf(fp_files,"%s.param\n",tmppath);
						fprintf(fp_files,"%s.tmp\n",tmppath);
						fprintf(fp_files,"%s.tmp2\n",tmppath);
						fio_set_maxsize(&fp_w6,max_size);
						//fio_set_memory_limit(&fp_w6,20,p->fio);
						//fio_set_memory_limit(&fp_w6,inFmt.sample * sizeof (SSIZE),p->fio);
					}

					r_nnn++;
					r_nnn_remain = 3;
				}
				r_nnn_remain--;

				rs = (64 / 8);
				if (1) {
					char sss[100];
					sprintf(sss,"r1:count %lld",nSample);
					PRINT_LOG(sss);
				}

				if (addp_overrap_first == 1) {
					fio_size save_fio_pos;

					nSample = inFmt.sample * 2;
					save_fio_pos = fio_tell(&fp_r1);
					fio_seek(&fp_r1,nSample * -1,SEEK_CUR);
					if (fp_r1.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					rd = fio_read(disk_buffer,rs,nSample,&fp_r1);
					if (fp_r1.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd > 0) {
						wr1 = fio_write(disk_buffer,rs,nSample,&fp_w1);
						if (fp_w1.error || wr1 != nSample) {
							PRINT_LOG("");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
					}
					fio_seek(&fp_r1,save_fio_pos,SEEK_SET);
					if (fp_r1.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (inFmt.channel >= 2) {
						if (1) {
							char sss[100];
							sprintf(sss,"r2:count %lld",nSample);
							PRINT_LOG(sss);
						}
						save_fio_pos = fio_tell(&fp_r2);
						fio_seek(&fp_r2,nSample * -1,SEEK_CUR);
						if (fp_r2.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						rd = fio_read(disk_buffer,rs,nSample,&fp_r2);
						if (fp_r2.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w2);
							if (fp_w2.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r2,save_fio_pos,SEEK_SET);
						if (fp_r2.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 3 || p->LRC_process == 1) {
						save_fio_pos = fio_tell(&fp_r3);
						fio_seek(&fp_r3,nSample * -1,SEEK_CUR);
						if (fp_r3.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						rd = fio_read(disk_buffer,rs,nSample,&fp_r3);
						if (fp_r3.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w3);
							if (fp_w3.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r3,save_fio_pos,SEEK_SET);
						if (fp_r3.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 4) {
						save_fio_pos = fio_tell(&fp_r4);
						fio_seek(&fp_r4,nSample * -1,SEEK_CUR);
						if (fp_r4.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						rd = fio_read(disk_buffer,rs,nSample,&fp_r4);
						if (fp_r4.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w4);
							if (fp_w4.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r4,save_fio_pos,SEEK_SET);
						if (fp_r4.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 5) {
						save_fio_pos = fio_tell(&fp_r5);
						fio_seek(&fp_r5,nSample * -1,SEEK_CUR);
						if (fp_r5.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						rd = fio_read(disk_buffer,rs,nSample,&fp_r5);
						if (fp_r5.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w5);
							if (fp_w5.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r5,save_fio_pos,SEEK_SET);
						if (fp_r5.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 6) {
						save_fio_pos = fio_tell(&fp_r6);
						fio_seek(&fp_r6,nSample * -1,SEEK_CUR);
						if (fp_r6.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						rd = fio_read(disk_buffer,rs,nSample,&fp_r6);
						if (fp_r6.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w6);
							if (fp_w6.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r6,save_fio_pos,SEEK_SET);
						if (fp_r6.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
				}
				nSample = inFmt.sample * 10;
				if (i + nSample > inSample) {
					nSample = inSample - i;
				}
				rd = fio_read(disk_buffer,rs,nSample,&fp_r1);
				if (fp_r1.error) {
					PRINT_LOG("");
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (rd > 0) {
					wr1 = fio_write(disk_buffer,rs,nSample,&fp_w1);
					if (fp_w1.error || wr1 != nSample) {
						PRINT_LOG("");
						retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
						break;
					}
				}
				if (inFmt.channel >= 2) {
					if (1) {
						char sss[100];
						sprintf(sss,"r2:count %lld",nSample);
						PRINT_LOG(sss);
					}
					rd = fio_read(disk_buffer,rs,nSample,&fp_r2);
					if (fp_r2.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd > 0) {
						wr1 = fio_write(disk_buffer,rs,nSample,&fp_w2);
						if (fp_w2.error || wr1 != nSample) {
							PRINT_LOG("");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
					}
				}
				if (inFmt.channel >= 3 || p->LRC_process == 1) {
					rd = fio_read(disk_buffer,rs,nSample,&fp_r3);
					if (fp_r3.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd > 0) {
						wr1 = fio_write(disk_buffer,rs,nSample,&fp_w3);
						if (fp_w3.error || wr1 != nSample) {
							PRINT_LOG("");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
					}
				}
				if (inFmt.channel >= 4) {
					rd = fio_read(disk_buffer,rs,nSample,&fp_r4);
					if (fp_r4.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd > 0) {
						wr1 = fio_write(disk_buffer,rs,nSample,&fp_w4);
						if (fp_w4.error || wr1 != nSample) {
							PRINT_LOG("");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
					}
				}
				if (inFmt.channel >= 5) {
					rd = fio_read(disk_buffer,rs,nSample,&fp_r5);
					if (fp_r5.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd > 0) {
						wr1 = fio_write(disk_buffer,rs,nSample,&fp_w5);
						if (fp_w5.error || wr1 != nSample) {
							PRINT_LOG("");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
					}
				}
				if (inFmt.channel >= 6) {
					rd = fio_read(disk_buffer,rs,nSample,&fp_r6);
					if (fp_r6.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd > 0) {
						wr1 = fio_write(disk_buffer,rs,nSample,&fp_w6);
						if (fp_w6.error || wr1 != nSample) {
							PRINT_LOG("");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
					}
				}
				if (param_info_to_raw.addp_overrap == 1) {
					if (r_nnn_remain == 0 && i + nSample < inSample) {
						addp_overrap_end = 1;
					}
				}
				if (addp_overrap_end == 1) {
					fio_size save_fio_pos;

					nSample = inFmt.sample * 2;
					save_fio_pos = fio_tell(&fp_r1);

					rd = fio_read(disk_buffer,rs,nSample,&fp_r1);
					if (fp_r1.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (rd > 0) {
						wr1 = fio_write(disk_buffer,rs,nSample,&fp_w1);
						if (fp_w1.error || wr1 != nSample) {
							PRINT_LOG("");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
					}
					fio_seek(&fp_r1,save_fio_pos,SEEK_SET);
					if (fp_r1.error) {
						PRINT_LOG("");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (inFmt.channel >= 2) {
						if (1) {
							char sss[100];
							sprintf(sss,"r2:count %lld",nSample);
							PRINT_LOG(sss);
						}
						save_fio_pos = fio_tell(&fp_r2);
						rd = fio_read(disk_buffer,rs,nSample,&fp_r2);
						if (fp_r2.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w2);
							if (fp_w2.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r2,save_fio_pos,SEEK_SET);
						if (fp_r2.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 3 || p->LRC_process == 1) {
						save_fio_pos = fio_tell(&fp_r3);
						rd = fio_read(disk_buffer,rs,nSample,&fp_r3);
						if (fp_r3.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w3);
							if (fp_w3.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r3,save_fio_pos,SEEK_SET);
						if (fp_r3.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 4) {
						save_fio_pos = fio_tell(&fp_r4);
						rd = fio_read(disk_buffer,rs,nSample,&fp_r4);
						if (fp_r4.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w4);
							if (fp_w4.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r4,save_fio_pos,SEEK_SET);
						if (fp_r4.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 5) {
						save_fio_pos = fio_tell(&fp_r5);
						rd = fio_read(disk_buffer,rs,nSample,&fp_r5);
						if (fp_r5.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w5);
							if (fp_w5.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r5,save_fio_pos,SEEK_SET);
						if (fp_r5.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
					if (inFmt.channel >= 6) {
						save_fio_pos = fio_tell(&fp_r6);
						rd = fio_read(disk_buffer,rs,nSample,&fp_r6);
						if (fp_r6.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						if (rd > 0) {
							wr1 = fio_write(disk_buffer,rs,nSample,&fp_w6);
							if (fp_w6.error || wr1 != nSample) {
								PRINT_LOG("");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
						}
						fio_seek(&fp_r6,save_fio_pos,SEEK_SET);
						if (fp_r6.error) {
							PRINT_LOG("");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
					}
				}
			}
			fio_close(&fp_r1);
			fio_close(&fp_w1);
			sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r1");
			PRINT_LOG("delete:");
			PRINT_LOG(tmppath);
			unlink(tmppath);
			if (fp_w1.error) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			if (inFmt.channel >= 2) {
				fio_close(&fp_r2);
				fio_close(&fp_w2);
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r2");
				PRINT_LOG("delete:");
				PRINT_LOG(tmppath);
				unlink(tmppath);
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 3 || p->LRC_process == 1) {
				fio_close(&fp_r3);
				fio_close(&fp_w3);
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r3");
				PRINT_LOG("delete:");
				PRINT_LOG(tmppath);
				unlink(tmppath);
				if (fp_w3.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 4) {
				fio_close(&fp_r4);
				fio_close(&fp_w4);
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r4");
				unlink(tmppath);
				if (fp_w4.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 5) {
				fio_close(&fp_r5);
				fio_close(&fp_w5);
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r5");
				unlink(tmppath);
				if (fp_w5.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 6) {
				fio_close(&fp_r6);
				fio_close(&fp_w6);
				sprintf(tmppath,"%s%s%s.%s",workdrive,workdir,workfname,"r6");
				unlink(tmppath);
				if (fp_w6.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			r_nnn--;
			sprintf(work," -r_nnn_count:%d",r_nnn);
			PRINT_LOG(work);
			p->r_nnn_count = r_nnn;
			fclose(fp_param);
			fclose(fp_files);
			PRINT_LOG("wav2raw success");
			retCode = STATUS_SUCCESS;
		} while (0);
	}
	PRINT_LOG("wav2raw before retCode");
	if (retCode != STATUS_SUCCESS) {
		PRINT_LOG("wav2raw before retCode != STATUS_SUCCESS");
		_splitpath(argv[2],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"err");
		PRINT_LOG(tmppath);
		ofp = fopen(tmppath,"a");
		if (ofp) {
			switch (retCode) {
				case STATUS_PARAMETER_ERR:
					fprintf(ofp,"to_raw_main:[%04d] Parameter error.\n",errLine);
					break;
				case STATUS_FILE_READ_ERR:
					fprintf(ofp,"to_raw_main:[%04d] File read error.\n",errLine);
					break;
				case STATUS_FILE_WRITE_ERR:
					fprintf(ofp,"to_raw_main:[%04d] File write error.\n",errLine);
					break;
				case STATUS_MEM_ALLOC_ERR:
					fprintf(ofp,"to_raw_main:[%04d] Memory Allocation error.\n",errLine);
					break;
				default:
					fprintf(ofp,"to_raw_main:[%04d] Other error(%d).\n",errLine,retCode);
			}
			fclose(ofp);
		}
	} else {
		PRINT_LOG("wav2raw before retCode=STATUS_SUCCESS");
	}
	if (disk_buffer != NULL) {
		free(disk_buffer);
	}
	PRINT_FN_LOG(param_info_to_raw.debug_upconv,"main:end");

	return retCode;
}
//---------------------------------------------------------------------------
// Function   : nBitTo64S
// Description: nBit のデータを64Bit 符号付データに変換する(内部表現は56Bit)
// ---
//	nCh		:チャンネル数
//	ch		:音声データのチャネル
//	bit 	:入力データのビット数
//  bitIntFlag : 入力データのビット数がIntの場合(32bitのときのみ参照)
//	in		:入力データのアドレス
//	fio 	:出力FIOのアドレス
//	nSample :サンプル数
//
int nBitTo64S(int nCh,int ch,int bit,int bitIntFlag,void *in,FIO *fio,SSIZE nSample)
{
	SSIZE i;
	SSIZE ns;
	short *p16;
	unsigned char *p24;
	long   *p32int;
	float  *p32;
	double *p64;
	SSIZE out;
	SSIZE max,min;
	SSIZE maxLv,maxLv2;
	SSIZE clip_count;
	int next;
	fio_size ws;

	ns = 0;
	maxLv2 = 0;
	clip_count = 0;

	if (bit == 16) {
		/* 16Bit */
		next = 1 * nCh;
		p16 = (short *)in;
		if (ch >= 1) {
			p16++;
		}
		if (ch >= 2) {
			p16++;
		}
		if (ch >= 3) {
			p16++;
		}
		if (ch >= 4) {
			p16++;
		}
		if (ch >= 5) {
			p16++;
		}
	}
	if (bit == 24 || bit == 20) {
		/* 24Bit */
		next = 3 * nCh;
		p24 = (char *)in;
		if (ch >= 1) {
			p24+=3;
		}
		if (ch >= 2) {
			p24+=3;
		}
		if (ch >= 3) {
			p24+=3;
		}
		if (ch >= 4) {
			p24+=3;
		}
		if (ch >= 5) {
			p24+=3;
		}
	}
	if (bit == 32 && bitIntFlag == 1) {
		/* 32Bit */
		next = 1 * nCh;
		p32int = (long *)in;
		if (ch >= 1) {
			p32int++;
		}
		if (ch >= 2) {
			p32int++;
		}
		if (ch >= 3) {
			p32int++;
		}
		if (ch >= 4) {
			p32int++;
		}
		if (ch >= 5) {
			p32int++;
		}
	}
	if (bit == 32 && bitIntFlag == 0) {
		/* 32Bit */
		p32 = (float *)in;
		next = 1 * nCh;
		if (ch >= 1) {
			p32++;
		}
		if (ch >= 2) {
			p32++;
		}
		if (ch >= 3) {
			p32++;
		}
		if (ch >= 4) {
			p32++;
		}
		if (ch >= 5) {
			p32++;
		}
	}
	if (bit == 64) {
		/* 64Bit */
		p64 = (double *)in;
		next = 1 * nCh;
		if (ch >= 1) {
			p64++;
		}
		if (ch >= 2) {
			p64++;
		}
		if (ch >= 3) {
			p64++;
		}
		if (ch >= 4) {
			p64++;
		}
		if (ch >= 5) {
			p64++;
		}
	}

	if (ch == 0) {
		max = NormInfoWav2.maxR1;
		min = NormInfoWav2.minR1;
		maxLv = NormInfoWav2.maxLv1;
		maxLv2 = NormInfoWav2.tmpLv1;
		ns	   = NormInfoWav2.cntLv1;
	} else if (ch == 1) {
		max = NormInfoWav2.maxR2;
		min = NormInfoWav2.minR2;
		maxLv = NormInfoWav2.maxLv2;
		maxLv2 = NormInfoWav2.tmpLv2;
		ns	   = NormInfoWav2.cntLv2;
	} else if (ch == 2) {
		max = NormInfoWav2.maxR3;
		min = NormInfoWav2.minR3;
		maxLv = NormInfoWav2.maxLv3;
		maxLv2 = NormInfoWav2.tmpLv3;
		ns	   = NormInfoWav2.cntLv3;
	} else if (ch == 3) {
		max = NormInfoWav2.maxR4;
		min = NormInfoWav2.minR4;
		maxLv = NormInfoWav2.maxLv4;
		maxLv2 = NormInfoWav2.tmpLv4;
		ns	   = NormInfoWav2.cntLv4;
	} else if (ch == 4) {
		max = NormInfoWav2.maxR5;
		min = NormInfoWav2.minR5;
		maxLv = NormInfoWav2.maxLv5;
		maxLv2 = NormInfoWav2.tmpLv5;
		ns	   = NormInfoWav2.cntLv5;
	} else if (ch == 5) {
		max = NormInfoWav2.maxR6;
		min = NormInfoWav2.minR6;
		maxLv = NormInfoWav2.maxLv6;
		maxLv2 = NormInfoWav2.tmpLv6;
		ns	   = NormInfoWav2.cntLv6;
	}

	switch (bit) {
		case 16:
			for (i = 0;i < nSample;i++) {
				out = (SSIZE)*p16;
				out <<= (64 - 16);
				out >>= 8;
				
				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}

				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
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
				p16 += next;
				if (IS_CLIPPED(out)) clip_count++;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		case 20:
			for (i = 0;i < nSample;i++) {
				out  = (SSIZE)p24[2];
				out  = out << 8;
				out |= (SSIZE)p24[1];
				out  = out << 8;
				out |= (SSIZE)p24[0];
				out  = out << (64 - 20);
				out  = out >> 8;

				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}

				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
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

				p24 += next;
				if (IS_CLIPPED(out)) clip_count++;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		case 24:
			for (i = 0;i < nSample;i++) {
				out  = (SSIZE)p24[2];
				out  = out << 8;
				out |= (SSIZE)p24[1];
				out  = out << 8;
				out |= (SSIZE)p24[0];
				out  = out << (64 - 24);
				out  = out >> 8;

				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}

				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
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

				p24 += next;
				if (IS_CLIPPED(out)) clip_count++;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		case 32:
			if (bitIntFlag == 0) {
				for (i = 0;i < nSample;i++) {
					out = (SSIZE)(*p32 * 0x7FFFFFFFFFFFFF);
					
					if (max < out) {
						max = out;
					} else if (min > out) {
						min = out;
					}

					if ((out >> 40) > 0) {
						maxLv2 += (out >> 40);
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
					
					p32 += next;
					if (IS_CLIPPED(out)) clip_count++;
					ws = fio_write(&out,sizeof (SSIZE),1,fio);
					if (fio->error || ws != 1) {
						break;
					}
				}
			} else {
				for (i = 0;i < nSample;i++) {
					out = (SSIZE)*p32int;
					out <<= (64 - 32);
					out >>= 8;
					
					if (max < out) {
						max = out;
					} else if (min > out) {
						min = out;
					}

					if ((out >> 40) > 0) {
						maxLv2 += (out >> 40);
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
					p32int += next;
					if (IS_CLIPPED(out)) clip_count++;
					ws = fio_write(&out,sizeof (SSIZE),1,fio);
					if (fio->error || ws != 1) {
						break;
					}
				}
			}
			break;
		case 64:
			for (i = 0;i < nSample;i++) {
				out = (SSIZE)(*p64 * 0x7FFFFFFFFFFFFF);
				
				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}

				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
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

				p64 += next;
				if (IS_CLIPPED(out)) clip_count++;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		default:
			break;
	}
	
	if (ch == 0) {
		NormInfoWav2.maxR1 = max;
		NormInfoWav2.minR1 = min;
		NormInfoWav2.maxLv1 = maxLv;
		NormInfoWav2.tmpLv1 = maxLv2;
		NormInfoWav2.cntLv1 = ns;
		if ((nSample / 4000) < clip_count) NormInfoWav2.clipR1 = 1;
	} else if (ch == 1) {
		NormInfoWav2.maxR2 = max;
		NormInfoWav2.minR2 = min;
		NormInfoWav2.maxLv2 = maxLv;
		NormInfoWav2.tmpLv2 = maxLv2;
		NormInfoWav2.cntLv2 = ns;
		if ((nSample / 4000) < clip_count) NormInfoWav2.clipR2 = 1;
	} else if (ch == 2) {
		NormInfoWav2.maxR3 = max;
		NormInfoWav2.minR3 = min;
		NormInfoWav2.maxLv3 = maxLv;
		NormInfoWav2.tmpLv3 = maxLv2;
		NormInfoWav2.cntLv3 = ns;
		if ((nSample / 4000) < clip_count) NormInfoWav2.clipR3 = 1;
	} else if (ch == 3) {
		NormInfoWav2.maxR4 = max;
		NormInfoWav2.minR4 = min;
		NormInfoWav2.maxLv4 = maxLv;
		NormInfoWav2.tmpLv4 = maxLv2;
		NormInfoWav2.cntLv4 = ns;
		if ((nSample / 4000) < clip_count) NormInfoWav2.clipR4 = 1;
	} else if (ch == 4) {
		NormInfoWav2.maxR5 = max;
		NormInfoWav2.minR5 = min;
		NormInfoWav2.maxLv5 = maxLv;
		NormInfoWav2.tmpLv5 = maxLv2;
		NormInfoWav2.cntLv5 = ns;
		if ((nSample / 4000) < clip_count) NormInfoWav2.clipR5 = 1;
	} else if (ch == 5) {
		NormInfoWav2.maxR6 = max;
		NormInfoWav2.minR6 = min;
		NormInfoWav2.maxLv6 = maxLv;
		NormInfoWav2.tmpLv6 = maxLv2;
		NormInfoWav2.cntLv6 = ns;
		if ((nSample / 4000) < clip_count) NormInfoWav2.clipR6 = 1;
	}

	if (fio->error || ws != 1) {
		return -1;
	}
	
	return 0;
}

//---------------------------------------------------------------------------
// Function   : to_wav_main
// Description: wav ファイル化メイン
//
//
int to_wav_main(int argc, char *argv[],UPCONV_PARAM *p)
{
	PARAM_INFO_TO_WAV paramInfo;
	STARTEXEC_PROCESS_INFO sp_info[1];
	char workpath[_MAX_PATH];
	char tmppath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char tmpfn[_MAX_PATH];
	char ext[_MAX_EXT];
	static char workdrive[_MAX_DRIVE];
	static char workdir[_MAX_DIR];
	static char workfname[_MAX_FNAME];
	static char workext[_MAX_EXT];
	static char pdrive[_MAX_DRIVE];
	static char pdir[_MAX_DIR];
	static char pfname[_MAX_FNAME];
	static char pext[_MAX_EXT];
	char *arg_encorder[6];
	char *mp4_encorder[16];
	SSIZE *disk_buffer;
	char work[2048];
	char sss[512];
	char param[3072];
	char fn[6][10]={"_1","_2","_3","_4","_5","_6"};
	char s[300];
	int paramFlag;
	int ccc;
	int r_nnn;
	int LRC_errLine;
	FILE *fp,*fp_files;
	FIO fp_r1,fp_r2,fp_r3,fp_r4,fp_r5,fp_r6,fp_w,fp_w1,fp_w2;
	FIO fp_ws[6];
	FIO *p_fp1,*p_fp2,*p_fp3,*p_fp4,*p_fp5,*p_fp6;
	SOUNDFMT inFmt;
	SOUNDFMT outFmt;
	SOUNDFMT flacFmt;
	char *p1,*p2;
	SSIZE i,inSample,outSample,flacSample,startSample;
	SSIZE size;
	double nx;
	long rd;
	int count;
	int retCode;
//	int thread;
//	int fio;
	int genCh;
	double *p_nx[6];
	SSIZE  *p_max[6];
	SSIZE  *p_avg[6];
	FILEINFO fileInfo;
	long temp,temp2,temp3,temp4;
	double perR1,perR2,perR3,perR4,perR5,perR6;
	SSIZE maxR1,maxR2,maxR3,maxR4,maxR5,maxR6;
	SSIZE avgR1,avgR2,avgR3,avgR4,avgR5,avgR6;
	double adjPer1,adjPer2,adjPer3,adjPer4,adjPer5,adjPer6;
	double per;
	double temp_f;
	SSIZE max,avg;
	SSIZE max_l,max_r;
	FIO_COMMON_PARAM fio_common;
	int fp_r1_flag,fp_r2_flag,fp_r3_flag,fp_r4_flag,fp_r5_flag,fp_r6_flag;

//	thread = 1;
	genCh = 0;
	fp_files = NULL;

	memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));

	do {
		param[0] = '\0';
		memset(&paramInfo,0,sizeof (PARAM_INFO_TO_WAV));
		memset(&fileInfo,0,sizeof (FILEINFO));
		memset(&fp_ws,0,sizeof (FIO) * 6);
		paramFlag = 0;
		paramInfo.err = STATUS_SUCCESS;

		if (paramInfo.err != STATUS_SUCCESS) {
			PRINT_LOG("error");
			break;
		}

		paramInfo.r2w_ch = 0;
//		paramInfo.r2w_chC = 0;
//		paramInfo.r2w_chS = 0;
//		paramInfo.r2w_chLFE = 0;
//		paramInfo.fio = 5;
//		paramInfo.fio_auto = 0;
//		paramInfo.r2w_dsd_fmt = -1;
//		paramInfo.r2w_mode = 0;
//		paramInfo.r2w_norm = 0;
//		paramInfo.r2w_norm_option = 1;
//		paramInfo.r2w_ditherLv = 0;
//		paramInfo.r2w_encorder = 0;
//		paramInfo.r2w_vla_auto = 0;
//		paramInfo.r2w_vla = 1.0;
//		paramInfo.r2w_split_size = 0;
//		paramInfo.r2w_ms_process = 0;
//		paramInfo.r2w_bitwidth_int = 0;
//		paramInfo.r_nnn_count = 0;
//		paramInfo.mc_flag = 0;
//		paramInfo.addp_overrap = 1;
//		paramInfo.no_rate_conv = -1;
//		paramInfo.eq_analyze = 0;
//		paramInfo.index_pre_eq = -1;

		// ノーマライズするときにだけ使用する
		paramInfo.adjPer1 = (double)0;
		paramInfo.adjPer2 = (double)0;
		paramInfo.adjPer3 = (double)0;
		paramInfo.adjPer4 = (double)0;
		paramInfo.adjPer5 = (double)0;
		paramInfo.adjPer6 = (double)0;

		// overwrite
		if (p->enable_ovwrite_sw == 1) {
			p->outSampleR = p->ovwrite_s;
			p->validOutSampleR = p->ovwrite_s;
			p->outSampleR_final = p->ovwrite_s;
			p->w = p->ovwrite_w;
			p->bitwidth_int = p->ovwrite_w_int;
		}

		if (p->fio_sec_size > 0) {
			// 入力のn秒分
			fio_common.sector_size = p->outSampleR * sizeof (SSIZE);
			fio_common.sector_n    = p->fio_sec_size;
			fio_common.upconv_n    = 1;
		} else {
			// n MB 分
			fio_common.sector_size = (fio_size)1 * 1000 * 1000;
			fio_common.sector_n    = p->fio_mb_size;
			fio_common.upconv_n    = 1;
		}

		if (paramInfo.err != STATUS_SUCCESS) {
			PRINT_LOG("error");
			break;
		}

		strcpy(paramInfo.debug_upconv,"to_wav_main");
		strcpy(w2r_log_filename,"C:\\ProgramData\\upconv\\wav2raw.log");

		paramInfo.analyze_eq_pw = (double *)malloc(384001 * sizeof (double));
		if (paramInfo.analyze_eq_pw == NULL) {
			retCode = STATUS_MEM_ALLOC_ERR;paramInfo.errLine = __LINE__;
			break;
		}
		for (i = 0;i < 384000;i++) {
			paramInfo.analyze_eq_pw[i] = 0;
		}

		if (paramInfo.err != STATUS_SUCCESS) {
			PRINT_LOG("error");
			break;
		}

		strcpy(paramInfo.pr_str,"[raw2wav]");
		arg_encorder[0] = NULL;
		arg_encorder[1] = NULL;
		arg_encorder[2] = NULL;
		arg_encorder[3] = NULL;
		arg_encorder[4] = NULL;
		arg_encorder[5] = NULL;
		// Wave ファイルのは違う順番
		p_nx[0]  = &paramInfo.nx1;	// Left
		p_nx[1]  = &paramInfo.nx2;	// Right
		p_nx[2]  = &paramInfo.nx3;	// Center
		p_nx[3]  = &paramInfo.nx4;	// Surround Left
		p_nx[4]  = &paramInfo.nx5;	// Surround Right
		p_nx[5]  = &paramInfo.nx6;	// LFE

		p_fp1 = p_fp2 = p_fp3 = p_fp4 = p_fp5 = p_fp6 = NULL;
		
		retCode = STATUS_SUCCESS;
		paramInfo.errLine = 0;
		if (argc != 4) {
			paramInfo.arg_dp = argv[3];
		}
		sprintf(sss,"to_wav argc:%d",argc);
		PRINT_LOG(sss);
		PRINT_FN_LOG(paramInfo.debug_upconv,sss);

		if (paramInfo.err != STATUS_SUCCESS) {
			PRINT_LOG("error");
			break;
		}

		p->simple_mode_done_percent = 80;
		p->simple_mode_done_percent_div = 5;

		if (argc == 4 || argc == 5 || argc == 6) {
			if (paramInfo.err != STATUS_SUCCESS) {
				PRINT_LOG("error");
				break;
			}

			if (p->simple_mode == 0) {
				// default parameter
				fp = fopen(argv[3],"r");
				if (fp == NULL) {
					retCode = STATUS_PARAMETER_ERR;paramInfo.errLine = __LINE__;
					break;
				}

				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}
				if (fgets(work,2047,fp) == NULL) {
					retCode = STATUS_PARAMETER_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				strcat(param,work);
				strcat(param," ");
				if (strlen(argv[4]) >= 1) strcat(param,argv[4]);

				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}

				// テンポラリファイルの出力先
				if (fgets(workpath,_MAX_PATH - 1,fp) == NULL) {
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				p1 = strrchr(workpath,'\n');if (p1 != NULL) *p1 = '\0';
				if (strlen(workpath) >= 2 && workpath[strlen(workpath) - 1] != '\\') strcat(workpath,"\\");

				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}

				// FLAC(Encorder Option)
				if (fgets(work,500,fp) == NULL) {
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				strcpy(paramInfo.opt_flac,"|");
				strcat(paramInfo.opt_flac,work);
				
				// WavPack(Encorder Option)
				if (fgets(work,500,fp) == NULL) {
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				strcpy(paramInfo.opt_wavpack,"|");
				strcat(paramInfo.opt_wavpack,work);

				// MP3(Encorder Option)
				if (fgets(work,500,fp) == NULL) {
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				strcpy(paramInfo.opt_mp3,"|");
				strcat(paramInfo.opt_mp3,work);

				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}

				// Title
				fgets(work,500,fp);

				// OPUS(Encorder Option)
				work[0] = '\0';
				fgets(work,500,fp);
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				if (strlen(work) > 0) {
					strcpy(paramInfo.opt_opus,"|");
					strcat(paramInfo.opt_opus,work);
				} else {
					strcpy(paramInfo.opt_opus,"|");
					strcat(paramInfo.opt_opus,"--vbr");
				}

				// M4A(Encorder Option)
				work[0] = '\0';
				fgets(work,500,fp);
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				if (strlen(work) > 0) {
					strcpy(paramInfo.opt_m4a,"|");
					strcat(paramInfo.opt_m4a,work);
				} else {
					strcpy(paramInfo.opt_m4a,"|");
					strcat(paramInfo.opt_m4a,"-b:a 256k");
				}
				fclose(fp);

				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}

				// Encorder 追加設定(lame か ffmpeg を使用するもの)
				// C:\ProgramData\upconv_encorder.txt を読む
				sprintf(sss,"p->encorder:%d",p->encorder);
				PRINT_LOG(sss);
				if (p->encorder > 5) {
					FILE *fp_encorder;
					int upconv_encorder_found = 0;
					char *e_p1,*e_p2,*e_p3,*e_p4;

					e_p1 = e_p2 = e_p3 = e_p4 = NULL;
					// ffmpeg(mp3)  96k|ffmpeg.exe|mp3|-c:a libmp3lame -b:a 96k
					// lame(mp3)    96k|lame.exe|mp3|-V 0 -b 96

					_splitpath(argv[3],drive,dir,fname,ext);
					_makepath(tmppath,drive,dir,"upconv_encoder",".txt");
					// ファイルオープン
					PRINT_LOG(tmppath);
					fp_encorder = fopen(tmppath,"r");
					PRINT_LOG("upconv_encorder.txt");
					if (fp_encorder != NULL) {
						for (i = 6;i <= p->encorder;i++) {
							work[0] = '\0';
							fgets(work,500,fp);
							PRINT_LOG("*");
						}
						e_p1 = work;
						e_p2 = strchr(e_p1,'|');
						if (e_p2 != NULL) {
							*e_p2 = '\0';e_p2++;
						}
						if (e_p2 != NULL) {
							e_p3 = strchr(e_p2,'|');
							if (e_p3 != NULL) {
								*e_p3 = '\0';e_p3++;
							}
						}
						if (e_p3 != NULL) {
							e_p4 = strchr(e_p3,'|');
							if (e_p4 != NULL) {
								*e_p4 = '\0';e_p4++;
							}
						}
						fclose(fp_encorder);fp_encorder = NULL;
						PRINT_FN_LOG(paramInfo.debug_upconv,e_p1);
						PRINT_FN_LOG(paramInfo.debug_upconv,e_p2);
						PRINT_FN_LOG(paramInfo.debug_upconv,e_p3);
						PRINT_FN_LOG(paramInfo.debug_upconv,e_p4);
					}
					if (e_p1 != NULL && e_p2 != NULL && e_p3 != NULL && e_p4 != NULL) {
						if (strlen(e_p2) > 0 && strlen(e_p3) > 0 && strlen(e_p4) > 0) {
							if (strcmp(e_p2,"ffmpeg.exe") == 0) {
								PRINT_LOG("ffmpeg:encorder=5");
								strcpy(paramInfo.opt_alt_ffmpeg_option,"|");
								strcat(paramInfo.opt_alt_ffmpeg_option,e_p4);
								strcpy(paramInfo.opt_alt_ext,e_p3);
								// encoder 5 番で処理する
								p->encorder = 5;
								upconv_encorder_found = 1;
								PRINT_FN_LOG(paramInfo.debug_upconv,"encorder=5");
							} else if (strcmp(e_p2,"lame.exe") == 0) {
								PRINT_LOG("ffmpeg:encorder=3");
								strcpy(paramInfo.opt_alt_lame_option,"|");
								strcat(paramInfo.opt_alt_lame_option,e_p4);
								strcpy(paramInfo.opt_alt_ext,e_p3);
								// encoder 3 番で処理する
								p->encorder = 3;
								PRINT_FN_LOG(paramInfo.debug_upconv,"encorder=3");
								if (p->rf64 == 1) {
									// RF64 はサポートしていないため ffmpeg で変換をする。
									char opt_work[512];
									char *o_p1;
									int  o_num;

									opt_work[0] = '\0';
									strcpy(paramInfo.opt_alt_lame_option,"");
									strcpy(paramInfo.opt_alt_ffmpeg_option,"");
									o_p1 = strstr(e_p4,"--cbr ");
									if (o_p1 != NULL) {
										// 固定レート
										strcpy(opt_work,"-b:a 128k ");
									}
									o_p1 = strstr(e_p4,"-b ");
									if (o_p1 != NULL) {
										if (sscanf(o_p1,"-b %d",&o_num) == 1) {
											// 固定レート
											sprintf(opt_work,"-b:a %d%s ",o_num,"k");
										}
									}
									o_p1 = strstr(e_p4,"--abr ");
									if (o_p1 != NULL) {
										strcpy(opt_work,"-q:a 5 ");
										if (sscanf(o_p1,"--abr %d",&o_num) == 1) {
											// ABR
											if (o_num > 300) {
												strcpy(opt_work,"-q:a 0 ");
											} else if (o_num > 250) {
												strcpy(opt_work,"-q:a 1 ");
											} else if (o_num > 190) {
												strcpy(opt_work,"-q:a 2 ");
											} else if (o_num > 180) {
												strcpy(opt_work,"-q:a 3 ");
											} else if (o_num > 170) {
												strcpy(opt_work,"-q:a 4 ");
											} else if (o_num > 160) {
												strcpy(opt_work,"-q:a 5 ");
											} else if (o_num > 140) {
												strcpy(opt_work,"-q:a 6 ");
											} else if (o_num > 130) {
												strcpy(opt_work,"-q:a 7 ");
											} else if (o_num > 120) {
												strcpy(opt_work,"-q:a 8 ");
											} else if (o_num > 100) {
												strcpy(opt_work,"-q:a 9 ");
											}
										}
									}
									o_p1 = strstr(e_p4,"-v ");
									if (o_p1 != NULL) {
										if (sscanf(o_p1,"--abr %d",&o_num) == 1) {
											sprintf(opt_work,"-q:a %d ",o_num);
										}
									}
									if (strlen(opt_work) > 0) {
										strcpy(paramInfo.opt_alt_ffmpeg_option,"|");
										strcat(paramInfo.opt_alt_ffmpeg_option,opt_work);
										strcat(paramInfo.opt_alt_ffmpeg_option," -c:a libmp3lame");
									}
								}
								upconv_encorder_found = 1;
							}
						}
					}
					if (upconv_encorder_found == 0) {
						p->encorder = 0;
					}
				}
			}

			// param ファイル
			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"param");
			// ファイルオープン
			fp = fopen(tmppath,"r");
			if (fp == NULL) {
				retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
				break;
			}
			
			if (fgets(work,_MAX_PATH,fp) == NULL) {
				retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
				break;
			}
			p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
			if (paramInfo.err != STATUS_SUCCESS) {
				PRINT_LOG("error");
				break;
			}

			retCode = PLG_InfoAudioData(work,&inFmt,&inSample,&fileInfo);
			if (retCode == STATUS_SUCCESS) {
				if (strcmp(inFmt.fmt,"dsf") == 0) {
					p->norm = 1;
					p->norm_option = 1;
				}
			}
			strcpy(paramInfo.fromfile,work);

			if (paramInfo.err != STATUS_SUCCESS) {
				PRINT_LOG("error");
				break;
			}

			fclose(fp);
			p1 = param;
			p2 = strchr(p1,(int)' ');

			if (argc == 6 && strlen(argv[5]) > 0) {
				strcpy(w2r_log_filename,argv[5]);
			}
			PRINT_FN_LOG(paramInfo.debug_upconv,"to_wav_main:start");

			if (paramInfo.err != STATUS_SUCCESS) {
				PRINT_LOG("error");
				break;
			}

			paramFlag = 0x0F;
			if (paramFlag == 0x0F) {
#ifdef _OPENMP
	omp_set_num_threads(p->thread);
#endif
				paramInfo.r2w_inSampleR  = p->inSampleR;
				paramInfo.r2w_outSampleR = p->outSampleR;
				paramInfo.r2w_ch         = p->ch;
				if (p->split_size != 0) {
					paramInfo.r2w_outSampleR = paramInfo.r2w_inSampleR;
				}

				if (p->dsd_fmt != -1) {
					p->mc_downmix = 1;
					p->mc_split   = 0;
				}

				if (p->mc_downmix) {
					p->genCh = 0;
				}
//				if (p->LRC_process == 1 && p->chC == 1) {
//					p->chC = 0;
//					p->genCh--;
//				}
				p->genCh = 0;
				paramInfo.r2w_ch += p->genCh;

				if ((p->output_left_right_center == 1 || p->chC == 1 || p->chS == 1 || p->chLFE == 1) && p->LRC_process == 1) {
					paramInfo.r2w_ch++;
					p->realCh++;
				}

				if (p->w64 == 1) {
					p->encorder = 0;
					_splitpath(argv[0],pdrive,pdir,pfname,pext);
					_makepath(tmppath,pdrive,pdir,"sndfile-convert","exe");
					arg_encorder[0] = strdup(tmppath);
					arg_encorder[1] = malloc(4096);
					arg_encorder[2] = malloc(4096);
					arg_encorder[3] = malloc(4096);
					if (arg_encorder[0] == NULL || arg_encorder[1] == NULL || arg_encorder[2] == NULL || arg_encorder[3] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					strcpy(arg_encorder[1],argv[2]);
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(arg_encorder[2],drive,dir,fname,"w64");
					PRINT_LOG(arg_encorder[0]);
					PRINT_LOG(arg_encorder[1]);
					PRINT_LOG(arg_encorder[2]);
				}

				if (p->encorder == 1) {
					_splitpath(argv[0],pdrive,pdir,pfname,pext);
					_makepath(tmppath,pdrive,pdir,"flac","exe");
					arg_encorder[0] = strdup(tmppath);
					arg_encorder[1] = strdup(paramInfo.opt_flac);
					arg_encorder[2] = malloc(4096);
					arg_encorder[3] = malloc(4096);
					if (arg_encorder[0] == NULL || arg_encorder[1] == NULL || arg_encorder[2] == NULL || arg_encorder[3] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(&arg_encorder[2][16],drive,dir,fname,"flac");
					arg_encorder[2][0] = '|';
					arg_encorder[2][1] = '"';
					arg_encorder[2][2] = '-';
					arg_encorder[2][3] = '-';
					arg_encorder[2][4] = 'o';
					arg_encorder[2][5] = 'u';
					arg_encorder[2][6] = 't';
					arg_encorder[2][7] = 'p';
					arg_encorder[2][8] = 'u';
					arg_encorder[2][9] = 't';
					arg_encorder[2][10] = '-';
					arg_encorder[2][11] = 'n';
					arg_encorder[2][12] = 'a';
					arg_encorder[2][13] = 'm';
					arg_encorder[2][14] = 'e';
					arg_encorder[2][15] = '=';
					strcat(arg_encorder[2],"\"");
					strcpy(arg_encorder[3],argv[2]);
					PRINT_LOG(arg_encorder[0]);
					PRINT_LOG(arg_encorder[1]);
					PRINT_LOG(arg_encorder[2]);
					PRINT_LOG(arg_encorder[3]);
				} else if (p->encorder == 2) {
					_splitpath(argv[0],pdrive,pdir,pfname,pext);
					_makepath(tmppath,pdrive,pdir,"wavpack","exe");
					arg_encorder[0] = strdup(tmppath);
					arg_encorder[1] = strdup(paramInfo.opt_wavpack);
					arg_encorder[2] = malloc(4096);
					arg_encorder[3] = malloc(4096);
					if (arg_encorder[0] == NULL || arg_encorder[1] == NULL || arg_encorder[2] == NULL || arg_encorder[3] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					strcpy(arg_encorder[2],argv[2]);
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(arg_encorder[3],drive,dir,fname,"wv");
					PRINT_LOG(arg_encorder[0]);
					PRINT_LOG(arg_encorder[1]);
					PRINT_LOG(arg_encorder[2]);
					PRINT_LOG(arg_encorder[3]);

				} else if (p->encorder == 3) {
					_splitpath(argv[0],pdrive,pdir,pfname,pext);
					_makepath(tmppath,pdrive,pdir,"lame","exe");
					arg_encorder[0] = strdup(tmppath);
					if (strlen(paramInfo.opt_alt_lame_option) > 0) {
						arg_encorder[1] = strdup(paramInfo.opt_alt_lame_option);
					} else {
						arg_encorder[1] = strdup(paramInfo.opt_mp3);
					}
					arg_encorder[2] = malloc(4096);
					arg_encorder[3] = malloc(4096);
					if (arg_encorder[0] == NULL || arg_encorder[1] == NULL || arg_encorder[2] == NULL || arg_encorder[3] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					strcpy(arg_encorder[2],argv[2]);
					_splitpath(argv[2],drive,dir,fname,ext);
					if (strlen(paramInfo.opt_alt_ext) > 0) {
						_makepath(arg_encorder[3],drive,dir,fname,paramInfo.opt_alt_ext);
					} else {
						_makepath(arg_encorder[3],drive,dir,fname,"mp3");
					}
					if (p->rf64 == 1) {
						_splitpath(argv[0],pdrive,pdir,pfname,pext);
						_makepath(tmppath,pdrive,pdir,"ffmpeg","exe");
						free(arg_encorder[0]);
						arg_encorder[0] = strdup(tmppath);
						free(arg_encorder[1]);
						arg_encorder[1] = strdup("-i");
						strcpy(arg_encorder[2],argv[2]);
						strcpy(arg_encorder[3],"|-acodec libmp3lame");

						arg_encorder[4] = malloc(4096);
						_splitpath(argv[2],drive,dir,fname,ext);
						_makepath(arg_encorder[4],drive,dir,fname,"mp3");
					}
					PRINT_LOG(arg_encorder[0]);
					PRINT_LOG(arg_encorder[1]);
					PRINT_LOG(arg_encorder[2]);
					PRINT_LOG(arg_encorder[3]);
					PRINT_LOG(arg_encorder[4]);

				} else if (p->encorder == 4) {
					_splitpath(argv[0],pdrive,pdir,pfname,pext);
					_makepath(tmppath,pdrive,pdir,"opusenc","exe");
					arg_encorder[0] = strdup(tmppath);
					arg_encorder[1] = strdup(paramInfo.opt_opus);
					arg_encorder[2] = malloc(4096);
					arg_encorder[3] = malloc(4096);
					if (arg_encorder[0] == NULL || arg_encorder[1] == NULL || arg_encorder[2] == NULL || arg_encorder[3] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					strcpy(arg_encorder[2],argv[2]);
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(arg_encorder[3],drive,dir,fname,"opus");
					if (p->rf64 == 1) {
						_splitpath(argv[0],pdrive,pdir,pfname,pext);
						_makepath(tmppath,pdrive,pdir,"ffmpeg","exe");
						free(arg_encorder[0]);
						arg_encorder[0] = strdup(tmppath);
						free(arg_encorder[1]);
						arg_encorder[1] = strdup("-i");
						strcpy(arg_encorder[2],argv[2]);
						strcpy(arg_encorder[3],"|-acodec libopus");
						arg_encorder[4] = malloc(4096);
						_splitpath(argv[2],drive,dir,fname,ext);
						_makepath(arg_encorder[4],drive,dir,fname,"opus");
					}
					PRINT_LOG(arg_encorder[0]);
					PRINT_LOG(arg_encorder[1]);
					PRINT_LOG(arg_encorder[2]);
					PRINT_LOG(arg_encorder[3]);

				} else if (p->encorder == 5) {
					_splitpath(argv[0],pdrive,pdir,pfname,pext);
					_makepath(tmppath,pdrive,pdir,"ffmpeg","exe");
					arg_encorder[0] = strdup(tmppath);
					arg_encorder[1] = strdup("-i");
					arg_encorder[2] = malloc(4096);
					arg_encorder[3] = malloc(4096);
					arg_encorder[4] = malloc(4096);
					if (arg_encorder[0] == NULL || arg_encorder[1] == NULL || arg_encorder[2] == NULL || arg_encorder[3] == NULL || arg_encorder[4] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					strcpy(arg_encorder[2],argv[2]);
					if (strlen(paramInfo.opt_alt_ffmpeg_option) > 0) {
						strcpy(arg_encorder[3],paramInfo.opt_alt_ffmpeg_option);
					} else {
						strcpy(arg_encorder[3],paramInfo.opt_m4a);
					}
					_splitpath(argv[2],drive,dir,fname,ext);
					if (strlen(paramInfo.opt_alt_ext) > 0) {
						_makepath(arg_encorder[4],drive,dir,fname,paramInfo.opt_alt_ext);
					} else {
						_makepath(arg_encorder[4],drive,dir,fname,"m4a");
					}
					PRINT_LOG(arg_encorder[0]);
					PRINT_LOG(arg_encorder[1]);
					PRINT_LOG(arg_encorder[2]);
					PRINT_LOG(arg_encorder[3]);
					PRINT_LOG(arg_encorder[4]);
				}

				retCode = PLG_InfoAudioData(argv[1],&inFmt,&inSample,&fileInfo);
				if (retCode != STATUS_SUCCESS) {
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}

				outFmt.sample  = paramInfo.r2w_outSampleR;
				if (p->mc_split == 0) {
					outFmt.channel = paramInfo.r2w_ch;
					if (p->mc_downmix == 0) {
						if (p->chC && p->LRC_process == 0) {
							outFmt.channel++;
						}
						if (p->chS) {
							outFmt.channel+=2;
						}
						if (p->chLFE) {
							outFmt.channel++;
						}
					}
				} else {
					outFmt.channel = 1;
				}
				outFmt.bitsPerSample = (unsigned char)p->w;
				outFmt.bitsPerSampleInt = 0;
				if (p->bitwidth_int == 1) {
					outFmt.bitsPerSampleInt = 1;
				}
				sprintf(sss,"outFmt.ch:%d,bit:%d",outFmt.channel,outFmt.bitsPerSample);
				PRINT_LOG(sss);
				// r1 r2 r3 r4 r5 r6ファイルのパス生成
				_splitpath(argv[2],drive,dir,fname,ext);
				_splitpath(workpath,workdrive,workdir,workfname,workext);
				if (strlen(workdrive) == 2 && strlen(workdir) >= 1) {
					strcpy(workfname,fname);
					strcpy(workext,ext);
				} else {
					strcpy(workdrive,drive);
					strcpy(workdir,dir);
					strcpy(workfname,fname);
				}
				_makepath(tmppath,workdrive,workdir,workfname,"");

				// 分割ファイルの結合
				sprintf(s,"to_wav JoinFiles:%s",tmppath);
				PRINT_LOG(s);
				sprintf(s,"paramInfo.err:%d",paramInfo.err);
				PRINT_LOG(s);
				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}
				JoinFiles(tmppath,&paramInfo,p);
				if (paramInfo.err != STATUS_SUCCESS) {
					PRINT_LOG("error");
					break;
				}

				outSample = 0;
				_makepath(tmppath,workdrive,workdir,workfname,"r1");

				fio_open(&fp_r1,tmppath,FIO_MODE_R);
				if (fp_r1.error) {
					PRINT_LOG("fp_r1:open error");
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				fio_get_filesize(&fp_r1,&size);
				if (fp_r1.error || size < sizeof (SSIZE)) {
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				outSample = size / sizeof (SSIZE);
				fio_close(&fp_r1);

				// LRC Process
				if (p->realCh == 2 && (p->LRC_process == 1 && !(p->chC == 1 || p->chS == 1 || p->chLFE == 1))) {
					PRINT_LOG("LRC2");
					p->LRC_process = 0;
					strcpy(paramInfo.pr_str,"[LRC2]");

					_makepath(tmppath,workdrive,workdir,workfname,"r1");

					fio_common.upconv_n = 3;
					//fio_open(&fp_r1,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r1.error) {
						PRINT_LOG("fp_r1:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}

					_makepath(tmppath,workdrive,workdir,workfname,"r2");
					//fio_open(&fp_r2,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r2.error) {
						PRINT_LOG("fp_r2:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}

					_makepath(tmppath,workdrive,workdir,workfname,"r3");
					//fio_open(&fp_r3,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r3,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r3.error) {
						PRINT_LOG("fp_r3:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
					fio_common.upconv_n = 2;
					//fio_open(&fp_w1,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w1.error) {
						PRINT_LOG("fp_w1:open error");
						retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
						break;
					}

					_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
					//fio_open(&fp_w2,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w2.error) {
						PRINT_LOG("fp_w2:open error");
						retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					LRC_errLine = 0;
					LRC_merageTempFile('+',0,&fp_r1,&fp_r3,&fp_w1,outSample,&LRC_errLine);
					if (LRC_errLine != 0) {
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = LRC_errLine;
						break;
					}
					LRC_errLine = 0;
					LRC_merageTempFile('+',0,&fp_r2,&fp_r3,&fp_w2,outSample,&LRC_errLine);
					if (LRC_errLine != 0) {
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = LRC_errLine;
						break;
					}
					fio_close(&fp_r1);
					fio_close(&fp_r2);
					fio_close(&fp_r3);
					_makepath(tmppath,workdrive,workdir,workfname,"r1");
					fio_setmode_r(&fp_w1,&fp_r1,tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r2");
					fio_setmode_r(&fp_w2,&fp_r2,tmppath);
					if (fp_r1.error) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (fp_r2.error) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					fio_close(&fp_r1);
					fio_close(&fp_r2);
					paramInfo.r2w_ch = 2;
				}
				PRINT_LOG("LRC2:end");
				
				// Output Mid / Side
				if (p->sfa != 0) {
					if (p->output_mid_side == 1) {
						p->output_mid_side = 0;
					} else {
						p->output_mid_side = 1;
					}
				}
				
				if (p->realCh == 2 && p->output_mid_side == 1) {
					_makepath(tmppath,workdrive,workdir,workfname,"r1");
					fio_common.upconv_n = 2;
					//fio_open(&fp_r1,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r1.error) {
						PRINT_LOG("fp_r1:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}

					_makepath(tmppath,workdrive,workdir,workfname,"r2");
					//fio_open(&fp_r2,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r2.error) {
						PRINT_LOG("fp_r2:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
					//fio_open(&fp_w1,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w1.error) {
						PRINT_LOG("fp_w1:open error");
						retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
					//fio_open(&fp_w2,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w2,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w2.error) {
						PRINT_LOG("fp_w2:open error");
						retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					OutputMidSide(&fp_r1,&fp_r2,&fp_w1,&fp_w2,outSample,&paramInfo,p);
					if (paramInfo.err != STATUS_SUCCESS) {
						PRINT_LOG("error");
						break;
					}
					fio_close(&fp_r1);
					fio_close(&fp_r2);
					_makepath(tmppath,workdrive,workdir,workfname,"r1");
					fio_setmode_r(&fp_w1,&fp_r1,tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r2");
					fio_setmode_r(&fp_w2,&fp_r2,tmppath);
					if (fp_r1.error) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					if (fp_r2.error) {
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					fio_close(&fp_r1);
					fio_close(&fp_r2);
				}

				// 音量調整用調査&調整
				if (1) {
					FIO *p_r1,*p_r2,*p_r3,*p_r4,*p_r5,*p_r6;
					FIO *p_w1;
					FIO vfp_w1;
					FILE *fp_level,*fp_ana;
					double level_per,m_level_per;

					p_r1 = p_r2 = p_r3 = p_r4 = p_r5 = p_r6 = p_w1 = NULL;
					strcpy(paramInfo.pr_str,"[LA]");
					_makepath(tmppath,workdrive,workdir,workfname,"r1");
					fio_common.upconv_n = p->realCh;
					//fio_open(&fp_r1,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r1.error) {
						PRINT_LOG("fp_r1:open error");
						retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
						break;
					}
					p_r1 = &fp_r1;
					if (p->realCh >= 2) {
						_makepath(tmppath,workdrive,workdir,workfname,"r2");
						//fio_open(&fp_r2,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r2.error) {
							PRINT_LOG("fp_r2:open error");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r2 = &fp_r2;
					}
					if (p->realCh >= 3 || p->LRC_process == 1) {
						_makepath(tmppath,workdrive,workdir,workfname,"r3");
						//fio_open(&fp_r3,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r3,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r3.error) {
							PRINT_LOG("fp_r3:open error");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r3 = &fp_r3;
					}
					if (p->realCh >= 4) {
						_makepath(tmppath,workdrive,workdir,workfname,"r4");
						//fio_open(&fp_r4,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r4,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r4.error) {
							PRINT_LOG("fp_r4:open error");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r4 = &fp_r4;
					}
					if (p->realCh >= 5) {
						_makepath(tmppath,workdrive,workdir,workfname,"r5");
						//fio_open(&fp_r5,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r5,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r5.error) {
							PRINT_LOG("fp_r5:open error");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r5 = &fp_r5;
					}
					if (p->realCh >= 6) {
						_makepath(tmppath,workdrive,workdir,workfname,"r6");
						//fio_open(&fp_r6,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r6,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r6.error) {
							PRINT_LOG("fp_r6:open error");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r6 = &fp_r6;
					}
					sprintf(sss,"updateLevelInfo2:inSample:%lld",outSample);
					PRINT_LOG(sss);
					if (updateLevelInfo(outSample,paramInfo.r2w_ch,p_r1,p_r2,p_r3,p_r4,p_r5,p_r6,&Wav2RawLevelInfo,&Raw2WavLevelInfo,p) < 0) {
						retCode = STATUS_FILE_READ_ERR;
						break;
					}
					PRINT_LOG("updateLevelInfo2:End");

//					Raw2WavLevelInfo.nx = 1.0;

					if (Raw2WavLevelInfo.nx != 1.0) {
						_makepath(tmppath,workdrive,workdir,workfname,"r1.tmp");
						fio_common.upconv_n = 1;
						//fio_open(&fp_w1,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w1.error) {
							PRINT_LOG("fp_w1:open error");
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
						if (updateLevel(outSample,1,p_r1,&fp_w1,&Wav2RawLevelInfo,&Raw2WavLevelInfo,p) < 0) {
							retCode = STATUS_FILE_WRITE_ERR;
							break;
						}
						fio_close(&fp_r1);
						_makepath(tmppath,workdrive,workdir,workfname,"r1");
						fio_setmode_r(&fp_w1,&fp_r1,tmppath);
						if (fp_r1.error) {
							retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
							break;
						}
						fio_close(&fp_r1);

						if (p->realCh >= 2) {
							_makepath(tmppath,workdrive,workdir,workfname,"r2.tmp");
							//fio_open(&fp_w1,tmppath,FIO_MODE_W);
							fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
							if (fp_w1.error) {
								PRINT_LOG("fp_w1:open error");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							if (updateLevel(outSample,2,p_r2,&fp_w1,&Wav2RawLevelInfo,&Raw2WavLevelInfo,p) < 0) {
								retCode = STATUS_FILE_WRITE_ERR;
								break;
							}
							fio_close(&fp_r2);
							_makepath(tmppath,workdrive,workdir,workfname,"r2");
							fio_setmode_r(&fp_w1,&fp_r2,tmppath);
							if (fp_r2.error) {
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							fio_close(&fp_r2);
						}

						if (p->realCh >= 3 || p->LRC_process == 1) {
							_makepath(tmppath,workdrive,workdir,workfname,"r3.tmp");
							//fio_open(&fp_w1,tmppath,FIO_MODE_W);
							fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
							if (fp_w1.error) {
								PRINT_LOG("fp_w1:open error");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							if (updateLevel(outSample,3,p_r3,&fp_w1,&Wav2RawLevelInfo,&Raw2WavLevelInfo,p) < 0) {
								retCode = STATUS_FILE_WRITE_ERR;
								break;
							}
							fio_close(&fp_r3);
							_makepath(tmppath,workdrive,workdir,workfname,"r3");
							fio_setmode_r(&fp_w1,&fp_r3,tmppath);
							if (fp_r3.error) {
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							fio_close(&fp_r3);
						}
						if (p->realCh >= 4) {
							_makepath(tmppath,workdrive,workdir,workfname,"r4.tmp");
							//fio_open(&fp_w1,tmppath,FIO_MODE_W);
							fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
							if (fp_w1.error) {
								PRINT_LOG("fp_w1:open error");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							if (updateLevel(outSample,4,p_r4,&fp_w1,&Wav2RawLevelInfo,&Raw2WavLevelInfo,p) < 0) {
								retCode = STATUS_FILE_WRITE_ERR;
								break;
							}
							fio_close(&fp_r4);
							_makepath(tmppath,workdrive,workdir,workfname,"r4");
							fio_setmode_r(&fp_w1,&fp_r4,tmppath);
							if (fp_r4.error) {
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							fio_close(&fp_r4);
						}
						if (p->realCh >= 5) {
							_makepath(tmppath,workdrive,workdir,workfname,"r5.tmp");
							//fio_open(&fp_w1,tmppath,FIO_MODE_W);
							fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
							if (fp_w1.error) {
								PRINT_LOG("fp_w1:open error");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							if (updateLevel(outSample,5,p_r5,&fp_w1,&Wav2RawLevelInfo,&Raw2WavLevelInfo,p) < 0) {
								retCode = STATUS_FILE_WRITE_ERR;
								break;
							}
							fio_close(&fp_r5);
							_makepath(tmppath,workdrive,workdir,workfname,"r5");
							fio_setmode_r(&fp_w1,&fp_r5,tmppath);
							if (fp_r5.error) {
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							fio_close(&fp_r5);
						}
						if (p->realCh >= 6) {
							_makepath(tmppath,workdrive,workdir,workfname,"r6.tmp");
							//fio_open(&fp_w1,tmppath,FIO_MODE_W);
							fio_open_with_set_param(&fp_w1,tmppath,FIO_MODE_W,&fio_common);
							if (fp_w1.error) {
								PRINT_LOG("fp_w1:open error");
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							if (updateLevel(outSample,6,p_r6,&fp_w1,&Wav2RawLevelInfo,&Raw2WavLevelInfo,p) < 0) {
								retCode = STATUS_FILE_WRITE_ERR;
								break;
							}
							fio_close(&fp_r6);
							_makepath(tmppath,workdrive,workdir,workfname,"r6");
							fio_setmode_r(&fp_w1,&fp_r6,tmppath);
							if (fp_r6.error) {
								retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
								break;
							}
							fio_close(&fp_r6);
						}
						_makepath(tmppath,workdrive,workdir,workfname,"r1");

						fio_common.upconv_n = paramInfo.r2w_ch;
						//fio_open(&fp_r1,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r1.error) {
							PRINT_LOG("fp_r1:open error");
							retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
							break;
						}
						p_r1 = &fp_r1;
						if (p->realCh >= 2) {
							_makepath(tmppath,workdrive,workdir,workfname,"r2");
							//fio_open(&fp_r2,tmppath,FIO_MODE_R);
							fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
							if (fp_r2.error) {
								PRINT_LOG("fp_r2:open error");
								retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
								break;
							}
							p_r2 = &fp_r2;
						}
						if (p->realCh >= 3 || p->LRC_process == 1) {
							_makepath(tmppath,workdrive,workdir,workfname,"r3");
							//fio_open(&fp_r3,tmppath,FIO_MODE_R);
							fio_open_with_set_param(&fp_r3,tmppath,FIO_MODE_R,&fio_common);
							if (fp_r3.error) {
								PRINT_LOG("fp_r3:open error");
								retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
								break;
							}
							p_r3 = &fp_r3;
						}
						if (p->realCh >= 4) {
							_makepath(tmppath,workdrive,workdir,workfname,"r4");
							//fio_open(&fp_r4,tmppath,FIO_MODE_R);
							fio_open_with_set_param(&fp_r4,tmppath,FIO_MODE_R,&fio_common);
							if (fp_r4.error) {
								PRINT_LOG("fp_r4:open error");
								retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
								break;
							}
							p_r4 = &fp_r4;
						}
						if (p->realCh >= 5) {
							_makepath(tmppath,workdrive,workdir,workfname,"r5");
							//fio_open(&fp_r5,tmppath,FIO_MODE_R);
							fio_open_with_set_param(&fp_r5,tmppath,FIO_MODE_R,&fio_common);
							if (fp_r5.error) {
								PRINT_LOG("fp_r5:open error");
								retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
								break;
							}
							p_r5 = &fp_r5;
						}
						if (p->realCh >= 6) {
							_makepath(tmppath,workdrive,workdir,workfname,"r6");
							//fio_open(&fp_r6,tmppath,FIO_MODE_R);
							fio_open_with_set_param(&fp_r6,tmppath,FIO_MODE_R,&fio_common);
							if (fp_r6.error) {
								PRINT_LOG("fp_r6:open error");
								retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
								break;
							}
							p_r6 = &fp_r6;
						}
						if (updateLevelInfo(outSample,paramInfo.r2w_ch,p_r1,p_r2,p_r3,p_r4,p_r5,p_r6,NULL,&Raw2WavLevelInfo2,p) < 0) {
							retCode = STATUS_FILE_READ_ERR;
							break;
						}
					}

					_makepath(tmppath,workdrive,workdir,workfname,"level");
					fp_level = fopen(tmppath,"a");
					if (fp_level) {
						level_per = (Raw2WavLevelInfo.r1_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo.r1_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r1:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo.r1_avg,level_per,Raw2WavLevelInfo.r1_peak,Raw2WavLevelInfo.r1_m_avg,m_level_per,Raw2WavLevelInfo.r1_m_peak);
						level_per = (Raw2WavLevelInfo.r2_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo.r2_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r2:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo.r2_avg,level_per,Raw2WavLevelInfo.r2_peak,Raw2WavLevelInfo.r2_m_avg,m_level_per,Raw2WavLevelInfo.r2_m_peak);
						level_per = (Raw2WavLevelInfo.r3_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo.r3_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r3:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo.r3_avg,level_per,Raw2WavLevelInfo.r3_peak,Raw2WavLevelInfo.r3_m_avg,m_level_per,Raw2WavLevelInfo.r3_m_peak);
						level_per = (Raw2WavLevelInfo.r4_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo.r4_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r4:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo.r4_avg,level_per,Raw2WavLevelInfo.r4_peak,Raw2WavLevelInfo.r4_m_avg,m_level_per,Raw2WavLevelInfo.r4_m_peak);
						level_per = (Raw2WavLevelInfo.r5_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo.r5_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r5:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo.r5_avg,level_per,Raw2WavLevelInfo.r5_peak,Raw2WavLevelInfo.r5_m_avg,m_level_per,Raw2WavLevelInfo.r5_m_peak);
						level_per = (Raw2WavLevelInfo.r6_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo.r6_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r6:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo.r6_avg,level_per,Raw2WavLevelInfo.r6_peak,Raw2WavLevelInfo.r6_m_avg,m_level_per,Raw2WavLevelInfo.r6_m_peak);

						level_per = (Raw2WavLevelInfo2.r1_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo2.r1_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r1:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo2.r1_avg,level_per,Raw2WavLevelInfo2.r1_peak,Raw2WavLevelInfo2.r1_m_avg,m_level_per,Raw2WavLevelInfo2.r1_m_peak);
						level_per = (Raw2WavLevelInfo2.r2_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo2.r2_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r2:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo2.r2_avg,level_per,Raw2WavLevelInfo.r2_peak,Raw2WavLevelInfo2.r2_m_avg,m_level_per,Raw2WavLevelInfo2.r2_m_peak);
						level_per = (Raw2WavLevelInfo2.r3_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo2.r3_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r3:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo2.r3_avg,level_per,Raw2WavLevelInfo.r3_peak,Raw2WavLevelInfo2.r3_m_avg,m_level_per,Raw2WavLevelInfo2.r3_m_peak);
						level_per = (Raw2WavLevelInfo2.r4_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo2.r4_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r4:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo2.r4_avg,level_per,Raw2WavLevelInfo.r4_peak,Raw2WavLevelInfo2.r4_m_avg,m_level_per,Raw2WavLevelInfo2.r4_m_peak);
						level_per = (Raw2WavLevelInfo2.r5_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo2.r5_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r5:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo2.r5_avg,level_per,Raw2WavLevelInfo.r5_peak,Raw2WavLevelInfo2.r5_m_avg,m_level_per,Raw2WavLevelInfo2.r5_m_peak);
						level_per = (Raw2WavLevelInfo2.r6_avg / 32767.0) * 100;
						m_level_per = (Raw2WavLevelInfo2.r6_m_avg / -32768.0) * 100;
						fprintf(fp_level,"r6:%lld,%lf,%ld,%lld,%lf,%ld\n",Raw2WavLevelInfo2.r6_avg,level_per,Raw2WavLevelInfo.r6_peak,Raw2WavLevelInfo2.r6_m_avg,m_level_per,Raw2WavLevelInfo2.r6_m_peak);

						fclose(fp_level);
					}

					if (p_r1 != NULL) fio_close(&fp_r1);
					if (p_r2 != NULL) fio_close(&fp_r2);
					if (p_r3 != NULL) fio_close(&fp_r3);
					if (p_r4 != NULL) fio_close(&fp_r4);
					if (p_r5 != NULL) fio_close(&fp_r5);
					if (p_r6 != NULL) fio_close(&fp_r6);
				}

				PRINT_LOG("LevelInfo");
				sprintf(sss,"paramInfo.nx1:%lf",paramInfo.nx1);PRINT_LOG(sss);
				sprintf(sss,"paramInfo.nx2:%lf",paramInfo.nx2);PRINT_LOG(sss);
				sprintf(sss,"paramInfo.nx3:%lf",paramInfo.nx3);PRINT_LOG(sss);
				sprintf(sss,"paramInfo.nx4:%lf",paramInfo.nx4);PRINT_LOG(sss);
				sprintf(sss,"paramInfo.nx5:%lf",paramInfo.nx5);PRINT_LOG(sss);
				sprintf(sss,"paramInfo.nx6:%lf",paramInfo.nx6);PRINT_LOG(sss);

				if (p->nr != -1 || p->hfa == 2 || p->hfa == 3 || p->hfa == 4) {
					PRINT_LOG("nr hfa nx");
					if (paramInfo.nx1 != 0) paramInfo.nx1 *= 1.3;
					if (paramInfo.nx2 != 0) paramInfo.nx2 *= 1.3;
					if (paramInfo.nx3 != 0) paramInfo.nx3 *= 1.3;
					if (paramInfo.nx4 != 0) paramInfo.nx4 *= 1.3;
					if (paramInfo.nx5 != 0) paramInfo.nx5 *= 1.3;
					if (paramInfo.nx6 != 0) paramInfo.nx6 *= 1.3;
				}

				strcpy(paramInfo.pr_str,"[raw2wav]");
				if (p->eq_analyze > 0 && p->index_pre_eq > 0) {
					if (p->eq_analyze == 1) {
						sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze1.r1",p->index_pre_eq);
					} else if (p->eq_analyze == 2) {
						sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze2.r1",p->index_pre_eq);
					}
					fp = fopen(tmppath,"rt");
					if (fp != NULL) {
						double pp;
						for (i = 0;i < 384000;i++) {
							if (fscanf(fp,"%lf\n",&pp) != 1) break;
							paramInfo.analyze_eq_pw[i] = pp;
						}
						fclose(fp);
						unlink(tmppath);
						if (i == 384000) {
							if (p->eq_analyze == 1) {
								sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze1.r2",p->index_pre_eq);
							} else if (p->eq_analyze == 2) {
								sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze2.r2",p->index_pre_eq);
							}
							fp = fopen(tmppath,"rt");
							if (fp != NULL) {
								double pp;
								for (i = 0;i < 384000;i++) {
									if (fscanf(fp,"%lf\n",&pp) != 1) break;
									if (paramInfo.analyze_eq_pw[i] > 0 || pp > 0) {
										paramInfo.analyze_eq_pw[i] = (paramInfo.analyze_eq_pw[i] + pp) / 2;
									} else {
										paramInfo.analyze_eq_pw[i] = 0;
									}
								}
								fclose(fp);
								unlink(tmppath);
							}
						}
						if (i == 384000) {
							if (p->eq_analyze == 1) {
								sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze1",p->index_pre_eq);
							} else if (p->eq_analyze == 2) {
								sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze2",p->index_pre_eq);
							}
							fp = fopen(tmppath,"wt");
							if (fp != NULL) {
								double avg;
								avg = 0;
								for (i = 0;i < 384000;i++) {
									fprintf(fp,"%lf\n",paramInfo.analyze_eq_pw[i]);
									avg += paramInfo.analyze_eq_pw[i];
								}
								avg /= 384000;
								fprintf(fp,"%lf,%lld,%lf,%lf,%lf\n",(paramInfo.param_nx[0].p1 + paramInfo.param_nx[1].p1) / 2,(paramInfo.param_nx[0].p2 + paramInfo.param_nx[1].p2) / 2,(paramInfo.param_nx[0].p3 + paramInfo.param_nx[1].p3) / 2,(paramInfo.param_nx[0].p4 + paramInfo.param_nx[1].p4) / 2,avg);
								fprintf(fp,"volume_level:%lf\n",Raw2WavLevelInfo.level_per);
								fclose(fp);
							}
						}
					}
				}
				PRINT_LOG("before mc_flag");
				if (p->mc_flag == 1) {
				PRINT_LOG("mc_flag");
					int retStatus;
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(tmppath,drive,dir,fname,"files");
					fp_files = fopen(tmppath,"a");
					if (fp_files == NULL) {
						paramInfo.err = STATUS_FILE_WRITE_ERR;
						break;
					}
					_makepath(tmppath,workdrive,workdir,workfname,"r3");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r4");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r5");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r6");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r3.tmp");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r4.tmp");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r5.tmp");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r6.tmp2");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r3.tmp2");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r4.tmp2");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r5.tmp2");
					fprintf(fp_files,"%s\n",tmppath);
					_makepath(tmppath,workdrive,workdir,workfname,"r6.tmp2");
					fprintf(fp_files,"%s\n",tmppath);
					fclose(fp_files);
					PRINT_LOG("before mc_main");
					retStatus = mc_main(5,argv,paramInfo.param_nx,p);
					sprintf(s,"mc_main retStatus:%d",retStatus);
					PRINT_LOG(s);
					if (retStatus != STATUS_SUCCESS) {
						paramInfo.err = retStatus;
						break;
					}
					outSample = 0;
					_makepath(tmppath,workdrive,workdir,workfname,"r1");
					fio_open(&fp_r1,tmppath,FIO_MODE_R);
					if (fp_r1.error) {
						PRINT_LOG("fp_r1:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					fio_get_filesize(&fp_r1,&size);
					if (fp_r1.error || size < sizeof (SSIZE)) {
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					outSample = size / sizeof (SSIZE);
					fio_close(&fp_r1);
				}

				sprintf(sss,"sampling:%lld",paramInfo.r2w_outSampleR);
				PRINT_LOG(sss);

				sprintf(sss,"paramInfo.r2w_ch:%d",paramInfo.r2w_ch);
				PRINT_LOG(sss);

				if (p->chC == 1 && p->LRC_process == 0) {
					paramInfo.r2w_ch++;
				}
				if (p->chS == 1) {
					paramInfo.r2w_ch+=2;
				}
				if (p->chLFE == 1) {
					paramInfo.r2w_ch++;
				}

				sprintf(sss,"paramInfo.r2w_ch:%d",paramInfo.r2w_ch);
				PRINT_LOG(sss);
				sprintf(sss,"realCh:%d",p->realCh);
				PRINT_LOG(sss);
				sprintf(sss,"chC:%d,chS:%d,chLFE:%d",p->chC,p->chS,p->chLFE);
				PRINT_LOG(sss);
				if (paramInfo.r2w_ch == 3) {
					if (p->chC == 1 || p->LRC_process == 1 || p->realCh == 3) {
						PRINT_LOG("r2w_ch:3");
						p_nx[2]  = &paramInfo.nx3;
						p_max[2] = &maxR3;
						p_avg[2] = &avgR3;
						strcpy(fn[2],"_C");
					} else if (p->chLFE == 1) {
						p_nx[2]  = &paramInfo.nx3;
						p_max[2] = &maxR6;
						p_avg[2] = &avgR6;
						strcpy(fn[2],"_LFE");
					}
					fp_r3_flag = 1;
				}
				if (paramInfo.r2w_ch == 4) {
					if (p->realCh == 4) {
						PRINT_LOG("r2w_ch:4");
						p_nx[2]  = &paramInfo.nx3;
						p_nx[3]  = &paramInfo.nx4;
						p_max[2] = &maxR4;
						p_max[3] = &maxR5;
						p_avg[2] = &avgR4;
						p_avg[3] = &avgR5;
						strcpy(fn[2],"_SL");
						strcpy(fn[3],"_SR");
					}
					if (p->chS == 1) {
						PRINT_LOG("r2w_ch:4");
						p_nx[2]  = &paramInfo.nx4;
						p_nx[3]  = &paramInfo.nx5;
						p_max[2] = &maxR4;
						p_max[3] = &maxR5;
						p_avg[2] = &avgR4;
						p_avg[3] = &avgR5;
						strcpy(fn[2],"_SL");
						strcpy(fn[3],"_SR");
					}
					if (p->chC == 1 && p->chLFE == 1) {
						p_nx[2]  = &paramInfo.nx3;
						p_nx[3]  = &paramInfo.nx4;
						p_max[2] = &maxR3;
						p_max[3] = &maxR6;
						p_avg[2] = &avgR3;
						p_avg[3] = &avgR6;
						strcpy(fn[2],"_C");
						strcpy(fn[3],"_LFE");
					}
					fp_r3_flag = 1;
					fp_r4_flag = 1;
				}
				if (paramInfo.r2w_ch == 5) {
					if ((p->chC == 1 && p->chS == 1) || p->realCh == 5) {
						PRINT_LOG("r2w_ch:5");
						p_nx[2]  = &paramInfo.nx3;
						p_nx[3]  = &paramInfo.nx4;
						p_nx[4]  = &paramInfo.nx5;
						p_max[2] = &maxR3;
						p_max[3] = &maxR4;
						p_max[4] = &maxR5;
						p_avg[2] = &avgR3;
						p_avg[3] = &avgR4;
						p_avg[4] = &avgR5;
						strcpy(fn[2],"_C");
						strcpy(fn[3],"_SL");
						strcpy(fn[4],"_SR");
					}
					if (p->chLFE == 1 && p->chS == 1) {
						p_nx[2]  = &paramInfo.nx3;
						p_nx[3]  = &paramInfo.nx4;
						p_nx[4]  = &paramInfo.nx5;
						p_max[2] = &maxR6;
						p_max[3] = &maxR4;
						p_max[4] = &maxR5;
						p_avg[2] = &avgR6;
						p_avg[3] = &avgR4;
						p_avg[4] = &avgR5;
						strcpy(fn[2],"_LFE");
						strcpy(fn[3],"_SL");
						strcpy(fn[4],"_SR");
					}
					fp_r3_flag = 1;
					fp_r4_flag = 1;
					fp_r5_flag = 1;
				}
				if (paramInfo.r2w_ch == 6) {
					PRINT_LOG("r2w_ch:6");
					p_nx[2]  = &paramInfo.nx3;
					p_nx[5]  = &paramInfo.nx6;
					p_nx[3]  = &paramInfo.nx4;
					p_nx[4]  = &paramInfo.nx5;
					strcpy(fn[2],"_C");
					strcpy(fn[3],"_LFE");
					strcpy(fn[4],"_SL");
					strcpy(fn[5],"_SR");

					fp_r3_flag = 1;
					fp_r4_flag = 1;
					fp_r5_flag = 1;
					fp_r6_flag = 1;
				}

				*p_nx[0] = 1;
				*p_nx[1] = 1;
				*p_nx[2] = 1;
				*p_nx[3] = 1;
				*p_nx[4] = 1;
				*p_nx[5] = 1;
				_makepath(tmppath,workdrive,workdir,workfname,"r1");
				// ファイルオープン
				fio_common.upconv_n = paramInfo.r2w_ch;
				//fio_open(&fp_r1,tmppath,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,tmppath,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					PRINT_LOG("fp_r1:open error");
					retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
					break;
				}
				p_fp1 = &fp_r1;
				fp_r1_flag = 1;
				if (paramInfo.r2w_ch >= 2) {
					// R2 ファイル
					_makepath(tmppath,workdrive,workdir,workfname,"r2");

					// ファイルオープン
					//fio_open(&fp_r2,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r2,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r2.error) {
						PRINT_LOG("fp_r2:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					p_fp2 = &fp_r2;
					fp_r2_flag = 1;
				}
				PRINT_LOG("");

				if (paramInfo.r2w_ch >= 3) {
					if (!(paramInfo.r2w_ch == 4 && p->chS == 1)) {
						// R3 ファイル
						_makepath(tmppath,workdrive,workdir,workfname,"r3");

						// ファイルオープン
						//fio_open(&fp_r3,tmppath,FIO_MODE_R);
						fio_open_with_set_param(&fp_r3,tmppath,FIO_MODE_R,&fio_common);
						if (fp_r3.error) {
							PRINT_LOG("fp_r3:open error");
							retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
							break;
						}
						p_fp3 = &fp_r3;
						fp_r3_flag = 1;
					}
				}
				PRINT_LOG("");

				if (paramInfo.r2w_ch >= 4) {
					// R4 ファイル
					_makepath(tmppath,workdrive,workdir,workfname,"r4");

					// ファイルオープン
					//fio_open(&fp_r4,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r4,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r4.error) {
						PRINT_LOG("fp_r4:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					p_fp4 = &fp_r4;
				}
				if (paramInfo.r2w_ch >= 5 || p->chS == 1) {
					// R5 ファイル
					_makepath(tmppath,workdrive,workdir,workfname,"r5");

					// ファイルオープン
					//fio_open(&fp_r5,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r5,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r5.error) {
						PRINT_LOG("fp_r5:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					p_fp5 = &fp_r5;
				}

				if (paramInfo.r2w_ch >= 6) {
					// R6 ファイル
					_makepath(tmppath,workdrive,workdir,workfname,"r6");

					// ファイルオープン
					//fio_open(&fp_r6,tmppath,FIO_MODE_R);
					fio_open_with_set_param(&fp_r6,tmppath,FIO_MODE_R,&fio_common);
					if (fp_r6.error) {
						PRINT_LOG("fp_r6:open error");
						retCode = STATUS_FILE_READ_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					p_fp6 = &fp_r6;
				}
				// wav_auto
				// 2g 以下から WAVファイルにする。2g 以上ならRF64にする
				// 1ch あたりのサイズを求める
				PRINT_LOG("before wav_auto == 1");
				if (p->wav_auto == 1) {
					SSIZE wav_auto_filesize;
					int sample_per_byte;
					PRINT_LOG("after mc_split");
					if (p->w == 16) {
						sample_per_byte = 2;
					} else if (p->w == 24) {
						sample_per_byte = 3;
					} else if (p->w == 32) {
						sample_per_byte = 4;
					} else if (p->w == 64) {
						sample_per_byte = 8;
					} else {
						sample_per_byte = 0;
					}
					p->rf64 = 1;
					if (sample_per_byte != 0) {
						PRINT_LOG("sample_per_byte != 0");
						wav_auto_filesize = outSample * sample_per_byte;
						if (p->mc_split == 0 && paramInfo.r2w_ch > 1) {
							wav_auto_filesize *= paramInfo.r2w_ch;
						}
						if (wav_auto_filesize < ((SSIZE)2 * 1024 * 1024 * 1024) - ((SSIZE)1 * 1024 * 1024)) {
							PRINT_LOG("before mc_split");
							p->rf64 = 0;
							p->wav2g = 1;
							PRINT_LOG("wav_auto -> wav2g");
						}
					}
				}

				PRINT_LOG("before mc_split");
				if (p->simple_mode == 0) {
					fprintf(stdout,"[raw2wav]0%%\n");
					fflush(stdout);
				}
				if (p->mc_split == 0) {
					PRINT_LOG("mc_split=0");
					// 出力ファイル
					_splitpath(argv[2],drive,dir,fname,ext);
					if (p->dsd_fmt == -1) {
						if (p->raw) {
							_makepath(tmppath,drive,dir,fname,"raw");
						} else {
							_makepath(tmppath,drive,dir,fname,"wav");
						}
						strcpy(paramInfo.tofile,fname);
						strcat(paramInfo.tofile,".wav");
					} else {
						if (p->dsd_fmt == 64) {
							outFmt.sample = 2822400;
						} else if (p->dsd_fmt == 128) {
							outFmt.sample = 2822400 * 2;
						} else if (p->dsd_fmt == 256) {
							outFmt.sample = 2822400 * 4;
						}
						_makepath(tmppath,drive,dir,fname,"dsf");
						strcpy(paramInfo.tofile,fname);
						strcat(paramInfo.tofile,".dsf");
					}
					// ファイルオープン

					fio_common.upconv_n = 1;
					//fio_open(&fp_w,tmppath,FIO_MODE_W);
					fio_open_with_set_param(&fp_w,tmppath,FIO_MODE_W,&fio_common);
					if (fp_w.error) {
						retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
						break;
					}
					paramInfo.fp_w[0] = &fp_w;
					PRINT_LOG("before Normalize");
					for (count = 1,startSample = 0;count > 0;) {
						PRINT_LOG("loop Normalize");
						if (p->dsd_fmt == -1) {
							retCode = Normalize(&count,&inFmt,&outFmt,p_fp1,p_fp2,p_fp3,p_fp4,p_fp5,p_fp6,&startSample,outSample,&paramInfo,p);
							PRINT_LOG("end Normalize");
							if (retCode != STATUS_SUCCESS) {
								paramInfo.errLine = __LINE__;
								break;
							}
						} else {
							retCode = Normalize_DSD(&inFmt,&outFmt,p_fp1,p_fp2,outSample,&paramInfo);
							count = 0;
						}
						PRINT_LOG("before fio_close");
						fio_close(paramInfo.fp_w[0]);
						if (paramInfo.fp_w[0]->error) {
							retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
							break;
						}
						if (p->encorder != 0) {
							int arg_argc;
							PRINT_LOG("encorder start");
							if (arg_encorder[4] == NULL) {
								arg_argc = 4;
							} else {
								arg_argc = 5;
							}
							if (start_exec(arg_argc,arg_encorder,p->cpu_pri,&sp_info[0]) == 0) {
								wait_exec(&sp_info[0],1);
								finish_exec(&sp_info[0]);
							}
							PRINT_LOG("encorder end");
						}
						if (p->w64 != 0) {
							PRINT_LOG("w64 start");
							if (start_exec(3,arg_encorder,p->cpu_pri,&sp_info[0]) == 0) {
								wait_exec(&sp_info[0],1);
								finish_exec(&sp_info[0]);
							}
						}

						// mp4
						if (p->out_mp4 != 0) {
							int iii;
							char mp4_str[4096];
							PRINT_LOG("Start:out mp4");

							_splitpath(argv[0],pdrive,pdir,pfname,pext);
							_makepath(tmppath,pdrive,pdir,"ffmpeg","exe");
							mp4_str[0] = '\0';
							mp4_encorder[0] = strdup(tmppath);
							PRINT_LOG(mp4_encorder[0]);
							mp4_encorder[1] = strdup("|-y");
							PRINT_LOG(mp4_encorder[1]);
							mp4_encorder[2] = strdup("|-i");
							PRINT_LOG(mp4_encorder[2]);
							mp4_encorder[3] = strdup(paramInfo.fromfile);
							PRINT_LOG(mp4_encorder[3]);
							mp4_encorder[4] = strdup("|-i");
							PRINT_LOG(mp4_encorder[4]);
							mp4_encorder[5] = strdup(argv[2]);
							PRINT_LOG(mp4_encorder[5]);
							mp4_encorder[6] = strdup("|-c:v copy -c:a libmp3lame -b:a 256k -map 0:v:0 -map 1:a:0");
							PRINT_LOG(mp4_encorder[6]);
							mp4_encorder[7] = malloc(4096);
							_splitpath(argv[2],pdrive,pdir,pfname,pext);
							_makepath(tmppath,pdrive,pdir,pfname,".mp4");
							strcpy(mp4_encorder[7],tmppath);
							PRINT_LOG(mp4_encorder[7]);

							sp_info[0].flag_stdout = 1;
							sleep(6);
							if (start_exec(8,mp4_encorder,p->cpu_pri,&sp_info[0]) == 0) {
								int read_n;
								while (1) {
									read_n = read_stdout(work,500,&sp_info[0]);
									if (read_n == 0) continue;
									if (read_n < 0) break;
									PRINT_LOG(work);
								}
								wait_exec(&sp_info[0],1);
								finish_exec(&sp_info[0]);
							}
							PRINT_LOG("End:out mp4");
						}

						if (count == 0) {
							break;
						}
						count++;
						// 出力ファイル
						_splitpath(argv[2],drive,dir,fname,ext);
						sprintf(fname,"%s_%d.wav",fname,count);
						_makepath(tmppath,drive,dir,fname,NULL);
						// ファイルオープン
						memset(&fp_w,0,sizeof (FIO));
						//fio_open(&fp_w,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w.error) {
							retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
							break;
						}
						strcpy(paramInfo.tofile,fname);
						paramInfo.fp_w[0] = &fp_w;
					}
				} else {
					// 出力ファイル
					fio_common.upconv_n = 1;
					for (ccc = 0;ccc < paramInfo.r2w_ch;ccc++) {
						_splitpath(argv[2],drive,dir,fname,ext);
						strcat(fname,fn[ccc]);
						if (p->raw) {
							_makepath(tmppath,drive,dir,fname,"raw");
						} else {
							_makepath(tmppath,drive,dir,fname,"wav");
						}
						if (ccc == 0) {
							strcpy(paramInfo.tofile,fname);
							strcat(paramInfo.tofile,"wav");
						}
						// ファイルオープン

						//fio_open(&fp_ws[ccc],tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_ws[ccc],tmppath,FIO_MODE_W,&fio_common);
						if (fp_ws[ccc].error) {
							retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
							break;
						}
						paramInfo.fp_w[ccc] = &fp_ws[ccc];
					}
					for (count = 1,startSample = 0;count > 0;) {
						retCode = Normalize(&count,&inFmt,&outFmt,p_fp1,p_fp2,p_fp3,p_fp4,p_fp5,p_fp6,&startSample,outSample,&paramInfo,p);
						if (retCode != STATUS_SUCCESS) {
							break;
						}
						for (ccc = 0;ccc < paramInfo.r2w_ch;ccc++) {
							fio_close(paramInfo.fp_w[ccc]);
							if (paramInfo.fp_w[ccc]->error) {
								retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
								count = 0;
								break;
							}
						}
						if (count == 0) {
							break;
						}
						count++;
						for (ccc = 0;ccc < paramInfo.r2w_ch;ccc++) {
							// 出力ファイル
							_splitpath(argv[2],drive,dir,fname,ext);
							sprintf(fname,"%s_%d%s",fname,count,fn[ccc]);
							_makepath(tmppath,drive,dir,fname,"wav");
							// ファイルオープン
							//fio_open(&fp_w,tmppath,FIO_MODE_W);
							fio_open_with_set_param(&fp_w,tmppath,FIO_MODE_W,&fio_common);
							if (fp_w.error) {
								retCode = STATUS_FILE_WRITE_ERR;paramInfo.errLine = __LINE__;
								break;
							}
							strcpy(paramInfo.tofile,fname);
							paramInfo.fp_w[ccc] = &fp_w;
							memset(&fp_w,0,sizeof (FIO));
						}
					}
				}
				fio_close(&fp_r1);
				if (p_fp2 != NULL) {
					fio_close(&fp_r2);
				}
				if (p_fp3 != NULL) {
					fio_close(&fp_r3);
				}
				if (p_fp4 != NULL) {
					fio_close(&fp_r4);
				}
				if (p_fp5 != NULL) {
					fio_close(&fp_r5);
				}
				if (p_fp6 != NULL) {
					fio_close(&fp_r6);
				}
			}
		}
		if (!(argc == 5) || paramFlag != 0x07) {
//			exit(0);
		}
	} while (0);

	if (arg_encorder[0] != NULL) free(arg_encorder[0]);
	if (arg_encorder[1] != NULL) free(arg_encorder[1]);
	if (arg_encorder[2] != NULL) free(arg_encorder[2]);

	if (retCode != STATUS_SUCCESS) {
		_splitpath(argv[2],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"err");
		fp = fopen(tmppath,"a");
		if (fp) {
			switch (retCode) {
				case STATUS_PARAMETER_ERR:
					fprintf(fp,"to_wav_main:[%04d] File read error.\n",paramInfo.errLine);
					break;
				case STATUS_FILE_READ_ERR:
					fprintf(fp,"to_wav_main:[%04d] File read error.\n",paramInfo.errLine);
					break;
				case STATUS_FILE_WRITE_ERR:
					fprintf(fp,"to_wav_main:[%04d] File write error.\n",paramInfo.errLine);
					break;
				case STATUS_MEM_ALLOC_ERR:
					fprintf(fp,"to_wav_main:[%04d] Memory Allocation error.\n",paramInfo.errLine);
					break;
				default:
					fprintf(fp,"to_wav_main:[%04d] Other error(%d).\n",paramInfo.errLine,retCode);
			}
			fclose(fp);
		}
	}
	PRINT_FN_LOG(paramInfo.debug_upconv,"to_wav_main:end");

	return retCode;
}
//---------------------------------------------------------------------------
// Function   : JoinFiles
// Description: ファイルの結合処理(r1_001、r1_002など)
// ---
//	pcount	:ファイル番号のアドレス
//	inFmt	:入力ファイル音声形式情報
//	outFmt	:出力ファイル音声形式情報
//	fp_r1	:音声データのFIO構造体
//	fp_r2	:音声データのFIO構造体
//	fp_r3	:音声データのFIO構造体
//	fp_r4	:音声データのFIO構造体
//	fp_r5	:音声データのFIO構造体
//	fp_r6	:音声データのFIO構造体
//	startSample : 開始サンプル
//	nSample :処理をするサンプル数のアドレス
//	param	:パラメーター構造体
//  p       :UPCONV_PARAM
//
void JoinFiles(char *workfname,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	char sss[300];
	static char temp_path[_MAX_PATH];
	double param_per,param_nx1,param_nx2,param_nx2_min;
	double *ptr_param_per_max,per_nx;
	SSIZE param_max;
	SSIZE *disk_buffer1;
	SSIZE *disk_buffer2;
	SSIZE *disk_buffer3;
	SSIZE *disk_buffer4;
	SSIZE *disk_buffer5;
	SSIZE *disk_buffer6;
	SSIZE val;
	SSIZE diff_min_index;
	SSIZE data_size_adjust;
	fio_size temp_offset;
	fio_size raw_files[6];
	fio_size raw_files_end;
	fio_size r_filesize[6];
	FIO fp_r;
	FIO fp_r1,fp_r2,fp_r3,fp_r4,fp_r5,fp_r6;
	FIO fp_w1,fp_w2,fp_w3,fp_w4,fp_w5,fp_w6;
	FILE *fp;
	SSIZE rd_n,wr_n,i;
	SSIZE size;
	int ii,n,nn;
	int  addp_overrap_first,addp_overrap_end;
	int  *declip_flag[6];
	int fio_ch;
	FIO_COMMON_PARAM fio_common;

	memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));

	if (p->fio_sec_size > 0) {
		// 入力のn秒分
		fio_common.sector_size = p->outSampleR * sizeof (SSIZE);
		fio_common.sector_n    = p->fio_sec_size;
		fio_common.upconv_n    = 1;
	} else {
		// n MB 分
		fio_common.sector_size = (fio_size)1 * 1000 * 1000;
		fio_common.sector_n    = p->fio_mb_size;
		fio_common.upconv_n    = 1;
	}

	PRINT_FN_LOG(param->debug_upconv,"JoinFiles:Start");

	raw_files[0]  = raw_files[1]  = raw_files[2]  = raw_files[3]  = raw_files[4]  = raw_files[5]  = 0;
	r_filesize[0] = r_filesize[1] = r_filesize[2] = r_filesize[3] = r_filesize[4] = r_filesize[5] = 0;
	raw_files_end = 0;
	fp = NULL;

	declip_flag[0] = declip_flag[1] = declip_flag[2] = declip_flag[3] = declip_flag[4] = declip_flag[5] = NULL;

	data_size_adjust = (param->r2w_outSampleR * 2) * sizeof (SSIZE);

	sprintf(sss,"p->r_nnn_count:%d,p->realCh:%d",p->r_nnn_count,p->realCh);
	PRINT_LOG(sss);

	sprintf(sss,"disk_buffer2 malloc:%ld",param->r2w_outSampleR);
	PRINT_LOG(sss);

	disk_buffer1 = disk_buffer2 = disk_buffer3 = disk_buffer4 = disk_buffer5 = disk_buffer6 = NULL;

	declip_flag[0] = (int *)malloc((p->r_nnn_count + 1) * sizeof (int));
	if (declip_flag[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	memset(declip_flag[0],0,(p->r_nnn_count + 1) * sizeof (int));

	declip_flag[1] = (int *)malloc((p->r_nnn_count + 1) * sizeof (int));
	if (declip_flag[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	memset(declip_flag[1],0,(p->r_nnn_count + 1) * sizeof (int));

	declip_flag[2] = (int *)malloc((p->r_nnn_count + 1) * sizeof (int));
	if (declip_flag[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	memset(declip_flag[2],0,(p->r_nnn_count + 1) * sizeof (int));

	declip_flag[3] = (int *)malloc((p->r_nnn_count + 1) * sizeof (int));
	if (declip_flag[3] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	memset(declip_flag[3],0,(p->r_nnn_count + 1) * sizeof (int));

	declip_flag[4] = (int *)malloc((p->r_nnn_count + 1) * sizeof (int));
	if (declip_flag[4] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	memset(declip_flag[4],0,(p->r_nnn_count + 1) * sizeof (int));

	declip_flag[5] = (int *)malloc((p->r_nnn_count + 1) * sizeof (int));
	if (declip_flag[5] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	memset(declip_flag[5],0,(p->r_nnn_count + 1) * sizeof (int));

	disk_buffer1 = (SSIZE *)calloc(sizeof(SSIZE),param->r2w_outSampleR * 30);
	if (disk_buffer1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fio_ch = 1;
	if (p->realCh >= 2) {
		fio_ch++;
		disk_buffer2 = (SSIZE *)calloc(sizeof(SSIZE),param->r2w_outSampleR * 30);
		if (disk_buffer2 == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
	}
	if (p->realCh >= 3 || p->LRC_process == 1) {
		fio_ch++;
		disk_buffer3 = (SSIZE *)calloc(sizeof(SSIZE),param->r2w_outSampleR * 30);
		if (disk_buffer3 == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
	}
	if (p->realCh >= 4) {
		fio_ch++;
		disk_buffer4 = (SSIZE *)calloc(sizeof(SSIZE),param->r2w_outSampleR * 30);
		if (disk_buffer4 == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
	}
	if (p->realCh >= 5) {
		fio_ch++;
		disk_buffer5 = (SSIZE *)calloc(sizeof(SSIZE),param->r2w_outSampleR * 30);
		if (disk_buffer5 == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
	}
	if (p->realCh >= 6) {
		fio_ch++;
		disk_buffer6 = (SSIZE *)calloc(sizeof(SSIZE),param->r2w_outSampleR * 30);
		if (disk_buffer6 == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;
			return;
		}
	}

	PRINT_LOG("");
	memset(&fp_r,0,sizeof (FIO));
	memset(&fp_r1,0,sizeof (FIO));
	memset(&fp_r2,0,sizeof (FIO));
	memset(&fp_r3,0,sizeof (FIO));
	memset(&fp_r4,0,sizeof (FIO));
	memset(&fp_r5,0,sizeof (FIO));
	memset(&fp_r6,0,sizeof (FIO));
	memset(&fp_w1,0,sizeof (FIO));
	memset(&fp_w2,0,sizeof (FIO));
	memset(&fp_w3,0,sizeof (FIO));
	memset(&fp_w4,0,sizeof (FIO));
	memset(&fp_w5,0,sizeof (FIO));
	memset(&fp_w6,0,sizeof (FIO));

	do {
		strcpy(temp_path,workfname);
		strcat(temp_path,".r1.001");
		PRINT_LOG(temp_path);

		fio_common.upconv_n    = fio_ch;
		//fio_open(&fp_r1,temp_path,FIO_MODE_R);
		fio_open_with_set_param(&fp_r1,temp_path,FIO_MODE_R,&fio_common);

		if (fp_r1.error) {
			PRINT_LOG("error");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_get_filesize(&fp_r1,&size);
#ifdef UPCONV_LOG
		fio_set_logfn(&fp_r1,"wav2raw:JoinFiles");
		fio_set_log_remain(&fp_r1,50);
#endif
		if (p->r_nnn_count > 1) {
			size -= data_size_adjust;
		}
		raw_files[0] = size;
		r_filesize[0] = size;
		if (raw_files[0] == 0) {
			PRINT_LOG("error");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (p->realCh >= 2) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r2.001");
			PRINT_LOG(temp_path);
			//fio_open(&fp_r2,temp_path,FIO_MODE_R);
			fio_open_with_set_param(&fp_r2,temp_path,FIO_MODE_R,&fio_common);
			if (fp_r2.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r2,&size);
			if (p->r_nnn_count > 1) {
				size -= data_size_adjust;
			}
			raw_files[1] = size;
			r_filesize[1] = size;
			if (raw_files[1] == 0) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (raw_files[0] != raw_files[1]) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
		}
		if (p->realCh >= 3 || p->LRC_process == 1) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r3.001");
			PRINT_LOG(temp_path);
			//fio_open(&fp_r3,temp_path,FIO_MODE_R);
			fio_open_with_set_param(&fp_r3,temp_path,FIO_MODE_R,&fio_common);
			if (fp_r3.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r3,&size);
			if (p->r_nnn_count > 1) {
				size -= data_size_adjust;
			}
			raw_files[2] = size;
			r_filesize[2] = size;
			if (raw_files[2] == 0) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (raw_files[0] != raw_files[1] || raw_files[1] != raw_files[2]) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
		}
		if (p->realCh >= 4) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r4.001");
			PRINT_LOG(temp_path);
			//fio_open(&fp_r4,temp_path,FIO_MODE_R);
			fio_open_with_set_param(&fp_r4,temp_path,FIO_MODE_R,&fio_common);
			if (fp_r4.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r4,&size);
			if (p->r_nnn_count > 1) {
				size -= data_size_adjust;
			}
			raw_files[3] = size;
			r_filesize[3] = size;
			if (raw_files[3] == 0) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (raw_files[0] != raw_files[1] || raw_files[1] != raw_files[2] || raw_files[2] != raw_files[3]) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
		}
		if (p->realCh >= 5) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r5.001");
			PRINT_LOG(temp_path);
			//fio_open(&fp_r5,temp_path,FIO_MODE_R);
			fio_open_with_set_param(&fp_r5,temp_path,FIO_MODE_R,&fio_common);
			if (fp_r5.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r5,&size);
			if (p->r_nnn_count > 1) {
				size -= data_size_adjust;
			}
			raw_files[4] = size;
			r_filesize[4] = size;
			if (raw_files[4] == 0) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (raw_files[0] != raw_files[1] || raw_files[1] != raw_files[2] || raw_files[2] != raw_files[3] || raw_files[3] != raw_files[4]) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
		}
		if (p->realCh >= 6) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r6.001");
			PRINT_LOG(temp_path);
			//fio_open(&fp_r6,temp_path,FIO_MODE_R);
			fio_open_with_set_param(&fp_r6,temp_path,FIO_MODE_R,&fio_common);
			if (fp_r6.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r6,&size);
			if (p->r_nnn_count > 1) {
				size -= data_size_adjust;
			}
			raw_files[5] = size;
			r_filesize[5] = size;
			if (raw_files[5] == 0) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (raw_files[0] != raw_files[1] || raw_files[1] != raw_files[2] || raw_files[2] != raw_files[3] || raw_files[3] != raw_files[4] || raw_files[4] != raw_files[5]) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
		}
		PRINT_LOG("");
		// ファイル存在チェックとファイルサイズチェック
		// 一番最後のファイル以外はサイズが一致してるはず。
		if (p->r_nnn_count > 1) {
			for (n = 2;n < p->r_nnn_count;n++) {
				if (chkAbort(((double)n / p->r_nnn_count)*100,abortInfo.abort_check_interval) == 1) exit(0);

				sprintf(temp_path,"%s.%s.%03d",workfname,"r1",n);
				fio_open(&fp_r,temp_path,FIO_MODE_R);
				if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				fio_get_filesize(&fp_r,&size);
				size -= (data_size_adjust * 2);
				if (raw_files[0] != size) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				r_filesize[0] += size;
				fio_close(&fp_r);
				if (p->realCh >= 2) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r2",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= (data_size_adjust * 2);
					if (raw_files[1] != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[1] += size;
					fio_close(&fp_r);
				}
				if (p->realCh >= 3 || p->LRC_process == 1) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r3",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= (data_size_adjust * 2);
					if (raw_files[2] != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[2] += size;
					fio_close(&fp_r);
				}
				if (p->realCh >= 4) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r4",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= (data_size_adjust * 2);
					if (raw_files[3] != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[3] += size;
					fio_close(&fp_r);
				}
				if (p->realCh >= 5) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r5",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= (data_size_adjust * 2);
					if (raw_files[4] != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[4] += size;
					fio_close(&fp_r);
				}
				if (p->realCh >= 6) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r6",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= (data_size_adjust * 2);
					if (raw_files[5] != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[5] += size;
					fio_close(&fp_r);
				}
			}
			if (n == p->r_nnn_count) {
				sprintf(temp_path,"%s.%s.%03d",workfname,"r1",n);
				fio_open(&fp_r,temp_path,FIO_MODE_R);
				if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				fio_get_filesize(&fp_r,&size);
				size -= data_size_adjust;
				raw_files_end = size;
				fio_close(&fp_r);
				if (size == 0) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				r_filesize[0] += size;
				if (p->realCh >= 2) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r2",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= data_size_adjust;
					fio_close(&fp_r);
					if (raw_files_end != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[1] += size;
				}
				if (p->realCh >= 3 || p->LRC_process == 1) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r3",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= data_size_adjust;
					fio_close(&fp_r);
					if (raw_files_end != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[2] += size;
				}
				if (p->realCh >= 4) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r4",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= data_size_adjust;
					fio_close(&fp_r);
					if (raw_files_end != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[3] += size;
				}
				if (p->realCh >= 5) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r5",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= data_size_adjust;
					fio_close(&fp_r);
					if (raw_files_end != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[4] += size;
				}
				if (p->realCh >= 6) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r6",n);
					fio_open(&fp_r,temp_path,FIO_MODE_R);
					if (fp_r.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					fio_get_filesize(&fp_r,&size);
					size -= data_size_adjust;
					fio_close(&fp_r);
					if (raw_files_end != size) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					r_filesize[5] += size;
				}
			}
		}
		if (param->err != STATUS_SUCCESS) {
			PRINT_LOG("error");
			return;
		}

	PRINT_LOG("");
		// ファイル分割時。ノーマライズ使用時か、VLA=Auto 使用時、ABEのdeClip使用時の調整
		for (n = 1;n <= p->r_nnn_count;n++) {
			if (chkAbort((((double)n / p->r_nnn_count)*100),abortInfo.abort_check_interval) == 1) exit(0);

			sprintf(temp_path,"%s.%s.%03d.param",workfname,"r1",n);
			fp = fopen(temp_path,"r");
			if (fp == NULL) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			sprintf(sss,"r1 n=%d,param_per=%lf,%lld,%lf,%lf",n,param_per,param_max,param_nx1,param_nx2);PRINT_LOG(sss);

			declip_flag[0][n] = param_nx1 != 1.0 ? 1 : 0;
			declip_flag[0][0] += declip_flag[0][n];
			if (n == 1) {
				param->param_nx[0].p1 = param_per;
				param->param_nx[0].p2 = param_max;
				param->param_nx[0].p3 = param_nx1;
				param->param_nx[0].p4 = param_nx2;
			} else {
				if (param->param_nx[0].p1 < param_per) {
					param->param_nx[0].p1 = param_per;
				}
				if (param->param_nx[0].p4 > param_nx2) {
					param->param_nx[0].p4 = param_nx2;
				}
			}

			fclose(fp);
			unlink(temp_path);
			if (p->realCh >= 2) {
				sprintf(temp_path,"%s.%s.%03d.param",workfname,"r2",n);
				fp = fopen(temp_path,"r");
				if (fp == NULL) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				sprintf(sss,"r2 n=%d,param_per=%lf,%lld,%lf,%lf",n,param_per,param_max,param_nx1,param_nx2);PRINT_LOG(sss);

				declip_flag[1][n] = param_nx1 != 1.0 ? 1 : 0;
				declip_flag[1][0] += declip_flag[1][n];
				if (n == 1) {
					param->param_nx[1].p1 = param_per;
					param->param_nx[1].p2 = param_max;
					param->param_nx[1].p3 = param_nx1;
					param->param_nx[1].p4 = param_nx2;
				} else {
					if (param->param_nx[1].p1 < param_per) {
						param->param_nx[1].p1 = param_per;
					}
					if (param->param_nx[1].p4 > param_nx2) {
						param->param_nx[1].p4 = param_nx2;
					}
				}

				fclose(fp);
				unlink(temp_path);
			}
			if (p->realCh >= 3 || p->LRC_process == 1) {
				sprintf(temp_path,"%s.%s.%03d.param",workfname,"r3",n);
				fp = fopen(temp_path,"r");
				if (fp == NULL) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				sprintf(sss,"r3 n=%d,param_per=%lf,%lld,%lf,%lf",n,param_per,param_max,param_nx1,param_nx2);PRINT_LOG(sss);

				declip_flag[2][n] = param_nx1 != 1.0 ? 1 : 0;
				declip_flag[2][0] += declip_flag[2][n];
				if (n == 1) {
					param->param_nx[2].p1 = param_per;
					param->param_nx[2].p2 = param_max;
					param->param_nx[2].p3 = param_nx1;
					param->param_nx[2].p4 = param_nx2;
				} else {
					if (param->param_nx[2].p1 < param_per) {
						param->param_nx[2].p1 = param_per;
					}
					if (param->param_nx[2].p4 > param_nx2) {
						param->param_nx[2].p4 = param_nx2;
					}
				}

				fclose(fp);
				unlink(temp_path);
			}
			if (p->realCh >= 4) {
				sprintf(temp_path,"%s.%s.%03d.param",workfname,"r4",n);
				fp = fopen(temp_path,"r");
				if (fp == NULL) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				sprintf(sss,"r4 n=%d,param_per=%lf,%lld,%lf,%lf",n,param_per,param_max,param_nx1,param_nx2);PRINT_LOG(sss);

				declip_flag[3][n] = param_nx1 != 1.0 ? 1 : 0;
				declip_flag[3][0] += declip_flag[3][n];
				if (n == 1) {
					param->param_nx[3].p1 = param_per;
					param->param_nx[3].p2 = param_max;
					param->param_nx[3].p3 = param_nx1;
					param->param_nx[3].p4 = param_nx2;
				} else {
					if (param->param_nx[3].p1 < param_per) {
						param->param_nx[3].p1 = param_per;
					}
					if (param->param_nx[3].p4 > param_nx2) {
						param->param_nx[3].p4 = param_nx2;
					}
				}

				fclose(fp);
				unlink(temp_path);
			}
			if (p->realCh >= 5) {
				sprintf(temp_path,"%s.%s.%03d.param",workfname,"r5",n);
				fp = fopen(temp_path,"r");
				if (fp == NULL) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				sprintf(sss,"r5 n=%d,param_per=%lf,%lld,%lf,%lf",n,param_per,param_max,param_nx1,param_nx2);PRINT_LOG(sss);

				declip_flag[4][n] = param_nx1 != 1.0 ? 1 : 0;
				declip_flag[4][0] += declip_flag[4][n];
				if (n == 1) {
					param->param_nx[4].p1 = param_per;
					param->param_nx[4].p2 = param_max;
					param->param_nx[4].p3 = param_nx1;
					param->param_nx[4].p4 = param_nx2;
				} else {
					if (param->param_nx[4].p1 < param_per) {
						param->param_nx[4].p1 = param_per;
					}
					if (param->param_nx[4].p4 > param_nx2) {
						param->param_nx[4].p4 = param_nx2;
					}
				}

				fclose(fp);
				unlink(temp_path);
			}
			if (p->realCh >= 6) {
				sprintf(temp_path,"%s.%s.%03d.param",workfname,"r6",n);
				fp = fopen(temp_path,"r");
				if (fp == NULL) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				if (fscanf(fp,"%lf,%llx,%lf,%lf\n",&param_per,&param_max,&param_nx1,&param_nx2) != 4) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				sprintf(sss,"r6 n=%d,param_per=%lf,%lld,%lf,%lf",n,param_per,param_max,param_nx1,param_nx2);PRINT_LOG(sss);

				declip_flag[5][n] = param_nx1 != 1.0 ? 1 : 0;
				declip_flag[5][0] += declip_flag[5][n];
				if (n == 1) {
					param->param_nx[5].p1 = param_per;
					param->param_nx[5].p2 = param_max;
					param->param_nx[5].p3 = param_nx1;
					param->param_nx[5].p4 = param_nx2;
				} else {
					if (param->param_nx[5].p1 < param_per) {
						param->param_nx[5].p1 = param_per;
					}
					if (param->param_nx[5].p4 > param_nx2) {
						param->param_nx[5].p4 = param_nx2;
					}
				}
				fclose(fp);
				unlink(temp_path);
			}
		}
		if (param->err != STATUS_SUCCESS) break;

		// deClip処理が分割ファイルのうち1つでもあり、どれかのチャンネルであるなら1にする
		declip_flag[0][0] += declip_flag[1][0] + declip_flag[2][0] + declip_flag[3][0] + declip_flag[4][0] + declip_flag[5][0];

		ptr_param_per_max = &param->param_nx[0].p1;
		param_nx2_min = 1.0;
		if (param_nx2_min > param->param_nx[0].p4) {
			param_nx2_min = param->param_nx[0].p4;
		}
		if (p->realCh >= 2) {
			if (*ptr_param_per_max < param->param_nx[1].p1) {
				ptr_param_per_max = &param->param_nx[1].p1;
			}
			if (param_nx2_min > param->param_nx[1].p4) {
				param_nx2_min = param->param_nx[1].p4;
			}
		}
		if (p->realCh >= 3 || p->LRC_process == 1) {
			if (*ptr_param_per_max < param->param_nx[2].p1) {
				ptr_param_per_max = &param->param_nx[2].p1;
			}
			if (param_nx2_min > param->param_nx[2].p4) {
				param_nx2_min = param->param_nx[2].p4;
			}
		}
		if (p->realCh >= 4) {
			if (*ptr_param_per_max < param->param_nx[3].p1) {
				ptr_param_per_max = &param->param_nx[3].p1;
			}
			if (param_nx2_min > param->param_nx[3].p4) {
				param_nx2_min = param->param_nx[3].p4;
			}
		}
		if (p->realCh >= 5) {
			if (*ptr_param_per_max < param->param_nx[4].p1) {
				ptr_param_per_max = &param->param_nx[4].p1;
			}
			if (param_nx2_min > param->param_nx[4].p4) {
				param_nx2_min = param->param_nx[4].p4;
			}
		}
		if (p->realCh >= 6) {
			if (*ptr_param_per_max < param->param_nx[5].p1) {
				ptr_param_per_max = &param->param_nx[5].p1;
			}
			if (param_nx2_min > param->param_nx[5].p4) {
				param_nx2_min = param->param_nx[5].p4;
			}
		}
		
		sprintf(sss,"ptr_param_per_max=%lf",*ptr_param_per_max);PRINT_LOG(sss);
		if (p->vla_auto == 0) {
			if (p->norm == 1 && p->norm_option == 1) {
				per_nx = (((double)0.97 / *ptr_param_per_max) * param_nx2_min);
			} else {
				per_nx = param_nx2_min;
			}
		} else {
			if (p->norm == 1 && p->norm_option == 1) {
				per_nx = (((double)0.97 / *ptr_param_per_max * 2) * param_nx2_min);
			} else {
				per_nx = param_nx2_min;
			}
		}

		param->adjPer1 = per_nx;
		if (p->realCh >= 2) param->adjPer2 = per_nx;
		if (p->realCh >= 3 || p->LRC_process == 1) param->adjPer3 = per_nx;
		if (p->realCh >= 4) param->adjPer4 = per_nx;
		if (p->realCh >= 5) param->adjPer5 = per_nx;
		if (p->realCh >= 6) param->adjPer6 = per_nx;
		sprintf(sss,"towav:adjPer1:%lf",param->adjPer1);PRINT_LOG(sss);
		sprintf(sss,"towav:adjPer2:%lf",param->adjPer2);PRINT_LOG(sss);
		sprintf(sss,"towav:adjPer3:%lf",param->adjPer3);PRINT_LOG(sss);
		sprintf(sss,"towav:adjPer4:%lf",param->adjPer4);PRINT_LOG(sss);
		sprintf(sss,"towav:adjPer5:%lf",param->adjPer5);PRINT_LOG(sss);
		sprintf(sss,"towav:adjPer6:%lf",param->adjPer6);PRINT_LOG(sss);

		strcpy(temp_path,workfname);
		strcat(temp_path,".r1.tmp");	// 結合したファイル名

		fio_common.upconv_n    = fio_ch;
		//fio_open(&fp_w1,temp_path,FIO_MODE_W);
		fio_open_with_set_param(&fp_w1,temp_path,FIO_MODE_W,&fio_common);
		sprintf(sss,"%s",temp_path);PRINT_LOG(sss);

		//fio_set_memory_limit(&fp_w1,20,p->fio);
		//fio_set_memory_limit(&fp_w1,p->outSampleR_final * sizeof (SSIZE),p->fio);

		fio_set_maxsize(&fp_w1,r_filesize[0]);

		sprintf(sss,"r_filesize[0]:%lld",r_filesize[0]);PRINT_LOG(sss);
		sprintf(sss,"r_filesize[1]:%lld",r_filesize[1]);PRINT_LOG(sss);

#ifdef UPCONV_LOG
		fio_set_logfn(&fp_w1,"wav2raw:JoinFiles");
		fio_set_log_remain(&fp_w1,50);
#endif
		if (fp_w1.error) {
			PRINT_LOG("error");
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}

		if (p->realCh >= 2) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r2.tmp");	// 結合したファイル名
			//fio_open(&fp_w2,temp_path,FIO_MODE_W);
			fio_open_with_set_param(&fp_w2,temp_path,FIO_MODE_W,&fio_common);
			sprintf(sss,"%s",temp_path);PRINT_LOG(sss);
			//fio_set_memory_limit(&fp_w2,20,p->fio);
			//fio_set_memory_limit(&fp_w2,p->outSampleR_final * sizeof (SSIZE),p->fio);
			fio_set_maxsize(&fp_w2,r_filesize[1]);
			if (fp_w2.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
		}
		if (p->realCh >= 3 || p->LRC_process == 1) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r3.tmp");	// 結合したファイル名
			//fio_open(&fp_w3,temp_path,FIO_MODE_W);
			fio_open_with_set_param(&fp_w3,temp_path,FIO_MODE_W,&fio_common);
			//fio_set_memory_limit(&fp_w3,20,p->fio);
			//fio_set_memory_limit(&fp_w3,p->outSampleR_final * sizeof (SSIZE),p->fio);
			fio_set_maxsize(&fp_w3,r_filesize[2]);
			if (fp_w3.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
		}
		if (p->realCh >= 4) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r4.tmp");	// 結合したファイル名
			//fio_open(&fp_w4,temp_path,FIO_MODE_W);
			fio_open_with_set_param(&fp_w4,temp_path,FIO_MODE_W,&fio_common);
			//fio_set_memory_limit(&fp_w4,20,p->fio);
			//fio_set_memory_limit(&fp_w4,p->outSampleR_final * sizeof (SSIZE),p->fio);
			fio_set_maxsize(&fp_w4,r_filesize[3]);
			if (fp_w4.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
		}
		if (p->realCh >= 5) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r5.tmp");	// 結合したファイル名
			//fio_open(&fp_w5,temp_path,FIO_MODE_W);
			fio_open_with_set_param(&fp_w5,temp_path,FIO_MODE_W,&fio_common);
			//fio_set_memory_limit(&fp_w5,20,p->fio);
			//fio_set_memory_limit(&fp_w5,p->outSampleR_final * sizeof (SSIZE),p->fio);
			fio_set_maxsize(&fp_w5,r_filesize[4]);
			if (fp_w5.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
		}
		if (p->realCh >= 6) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r6.tmp");	// 結合したファイル名
			//fio_open(&fp_w6,temp_path,FIO_MODE_W);
			fio_open_with_set_param(&fp_w6,temp_path,FIO_MODE_W,&fio_common);
			//fio_set_memory_limit(&fp_w6,20,p->fio);
			//fio_set_memory_limit(&fp_w6,p->outSampleR_final * sizeof (SSIZE),p->fio);
			fio_set_maxsize(&fp_w6,r_filesize[5]);
			if (fp_w6.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
		}
	PRINT_LOG("");
		for (n = 1;n <= p->r_nnn_count;n++) {
			int addp_e1,addp_e2,addp_e3,addp_e4,addp_e5,addp_e6;
			int end_flag;
			int norm_flag;
			SSIZE rd_sample1,wr_sample1;
			SSIZE rd_sample2,wr_sample2;
			SSIZE rd_sample3,wr_sample3;
			SSIZE rd_sample4,wr_sample4;
			SSIZE rd_sample5,wr_sample5;
			SSIZE rd_sample6,wr_sample6;
			double clip_nx1,clip_nx2,clip_nx3,clip_nx4,clip_nx5,clip_nx6;
			clip_nx1 = clip_nx2 = clip_nx3 = clip_nx4 = clip_nx5 = clip_nx6 = 0.00;

			if (chkAbort(((double)n / p->r_nnn_count) * 100,abortInfo.abort_check_interval) == 1) exit(0);

			do {
				addp_e1 = addp_e2 = addp_e3 = addp_e4 = addp_e5 = addp_e6 = 0;
				end_flag = 0;
				norm_flag = 0;
				if (p->r_nnn_count > 1 && n > 1) {
					fio_seek(&fp_r1,data_size_adjust,SEEK_CUR);
					if (fp_r1.error) {
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
				}
				if (p->realCh >= 2) {
					if (p->r_nnn_count > 1 && n > 1) {
						fio_seek(&fp_r2,data_size_adjust,SEEK_CUR);
						if (fp_r2.error) {
							param->err = STATUS_FILE_READ_ERR;
							break;
						}
					}
				}
				if (p->realCh >= 3 || p->LRC_process == 1) {
					if (p->r_nnn_count > 1 && n > 1) {
						fio_seek(&fp_r3,data_size_adjust,SEEK_CUR);
						if (fp_r3.error) {
							param->err = STATUS_FILE_READ_ERR;
							break;
						}
					}
				}
				if (p->realCh >= 4) {
					if (p->r_nnn_count > 1 && n > 1) {
						fio_seek(&fp_r4,data_size_adjust,SEEK_CUR);
						if (fp_r4.error) {
							param->err = STATUS_FILE_READ_ERR;
							break;
						}
					}
				}
				if (p->realCh >= 5) {
					if (p->r_nnn_count > 1 && n > 1) {
						fio_seek(&fp_r5,data_size_adjust,SEEK_CUR);
						if (fp_r5.error) {
							param->err = STATUS_FILE_READ_ERR;
							break;
						}
					}
				}
				if (p->realCh >= 6) {
					if (p->r_nnn_count > 1 && n > 1) {
						fio_seek(&fp_r6,data_size_adjust,SEEK_CUR);
						if (fp_r6.error) {
							param->err = STATUS_FILE_READ_ERR;
							break;
						}
					}
				}

				rd_sample1 = param->r2w_outSampleR * 30;
				rd_sample2 = param->r2w_outSampleR * 30;
				rd_sample3 = param->r2w_outSampleR * 30;
				rd_sample4 = param->r2w_outSampleR * 30;
				rd_sample5 = param->r2w_outSampleR * 30;
				rd_sample6 = param->r2w_outSampleR * 30;
				if ((p->norm == 1 && p->norm_option == 1) || p->vla_auto == 1) {
					norm_flag = 1;
					PRINT_LOG("raw2wav norm=1");
				}
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							clip_nx1 = 1.0;
							if (declip_flag[0][0] != 0 && declip_flag[0][n] == 0) clip_nx1 = 0.5;
							wr_sample1 = CopyData(&fp_r1,&fp_w1,rd_sample1,disk_buffer1,param->adjPer1,clip_nx1,norm_flag,&addp_e1);
						}
						#pragma omp section
						{
							if (p->realCh >= 2) {
								clip_nx2 = 1.0;
								if (declip_flag[0][0] != 0 && declip_flag[1][n] == 0) clip_nx2 = 0.5;
								wr_sample2 = CopyData(&fp_r2,&fp_w2,rd_sample2,disk_buffer2,param->adjPer2,clip_nx2,norm_flag,&addp_e2);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 3 || p->LRC_process == 1) {
								clip_nx3 = 1.0;
								if (declip_flag[0][0] != 0 && declip_flag[2][n] == 0) clip_nx3 = 0.5;
								wr_sample3 = CopyData(&fp_r3,&fp_w3,rd_sample3,disk_buffer3,param->adjPer3,clip_nx3,norm_flag,&addp_e3);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 4) {
								clip_nx4 = 1.0;
								if (declip_flag[0][0] != 0 && declip_flag[3][n] == 0) clip_nx4 = 0.5;
								wr_sample4 = CopyData(&fp_r4,&fp_w4,rd_sample4,disk_buffer4,param->adjPer4,clip_nx4,norm_flag,&addp_e4);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 5) {
								clip_nx5 = 1.0;
								if (declip_flag[0][0] != 0 && declip_flag[4][n] == 0) clip_nx5 = 0.5;
								wr_sample5 = CopyData(&fp_r5,&fp_w5,rd_sample5,disk_buffer5,param->adjPer5,clip_nx5,norm_flag,&addp_e5);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 6) {
								clip_nx6 = 1.0;
								if (declip_flag[0][0] != 0 && declip_flag[5][n] == 0) clip_nx6 = 0.5;
								wr_sample6 = CopyData(&fp_r6,&fp_w6,rd_sample6,disk_buffer6,param->adjPer6,clip_nx6,norm_flag,&addp_e6);
							}
						}
					}
				}
				sprintf(sss,"declip nx n=%d,clip_nx=%lf,%lf,%lf,%lf,%lf,%lf",n,clip_nx1,clip_nx2,clip_nx3,clip_nx4,clip_nx5,clip_nx6);
				PRINT_LOG(sss);
				sprintf(sss,"rd_sample1:%lld,wr_sample1:%lld",rd_sample1,wr_sample1);PRINT_LOG(sss);
				sprintf(sss,"rd_sample2:%lld,wr_sample2:%lld",rd_sample2,wr_sample2);PRINT_LOG(sss);
				sprintf(sss,"rd_sample3:%lld,wr_sample3:%lld",rd_sample3,wr_sample3);PRINT_LOG(sss);
				sprintf(sss,"rd_sample4:%lld,wr_sample4:%lld",rd_sample4,wr_sample4);PRINT_LOG(sss);
				sprintf(sss,"rd_sample5:%lld,wr_sample5:%lld",rd_sample5,wr_sample5);PRINT_LOG(sss);
				sprintf(sss,"rd_sample6:%lld,wr_sample6:%lld",rd_sample6,wr_sample6);PRINT_LOG(sss);
				addp_e1 = addp_e1 | addp_e2 | addp_e3 | addp_e4 | addp_e5 | addp_e6;
				if (addp_e1 != 0) {
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
				if (p->r_nnn_count > 1 && n < p->r_nnn_count) {
					break;
				}
			} while (rd_sample1 == wr_sample1);

			if (param->err != STATUS_SUCCESS) break;

			fio_close(&fp_r1);
			sprintf(temp_path,"%s.%s.%03d",workfname,"r1",n);
			unlink(temp_path);
			if (n < p->r_nnn_count) {
				sprintf(temp_path,"%s.%s.%03d",workfname,"r1",n + 1);

				fio_common.upconv_n    = fio_ch;
				//fio_open(&fp_r1,temp_path,FIO_MODE_R);
				fio_open_with_set_param(&fp_r1,temp_path,FIO_MODE_R,&fio_common);
				if (fp_r1.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				sprintf(sss,"next fio_open:%s",temp_path);
				PRINT_LOG(sss);
			}
			if (p->realCh >= 2) {
				fio_close(&fp_r2);
				sprintf(temp_path,"%s.%s.%03d",workfname,"r2",n);
				unlink(temp_path);
				if (n < p->r_nnn_count) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r2",n + 1);
					//fio_open(&fp_r2,temp_path,FIO_MODE_R);
					fio_open_with_set_param(&fp_r2,temp_path,FIO_MODE_R,&fio_common);
					if (fp_r2.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
					sprintf(sss,"next fio_open:%s",temp_path);
					PRINT_LOG(sss);
				}
			}
			if (p->realCh >= 3 || p->LRC_process == 1) {
				fio_close(&fp_r3);
				sprintf(temp_path,"%s.%s.%03d",workfname,"r3",n);
				unlink(temp_path);
				if (n < p->r_nnn_count) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r3",n + 1);
					//fio_open(&fp_r3,temp_path,FIO_MODE_R);
					fio_open_with_set_param(&fp_r3,temp_path,FIO_MODE_R,&fio_common);
					if (fp_r3.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
				}
			}
			if (p->realCh >= 4) {
				fio_close(&fp_r4);
				sprintf(temp_path,"%s.%s.%03d",workfname,"r4",n);
				unlink(temp_path);
				if (n < p->r_nnn_count) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r4",n + 1);
					//fio_open(&fp_r4,temp_path,FIO_MODE_R);
					fio_open_with_set_param(&fp_r4,temp_path,FIO_MODE_R,&fio_common);
					if (fp_r4.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
				}
			}
			if (p->realCh >= 5) {
				fio_close(&fp_r5);
				sprintf(temp_path,"%s.%s.%03d",workfname,"r5",n);
				unlink(temp_path);
				if (n < p->r_nnn_count) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r5",n + 1);
					//fio_open(&fp_r5,temp_path,FIO_MODE_R);
					fio_open_with_set_param(&fp_r5,temp_path,FIO_MODE_R,&fio_common);
					if (fp_r5.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
				}
			}
			if (p->realCh >= 6) {
				fio_close(&fp_r6);
				sprintf(temp_path,"%s.%s.%03d",workfname,"r6",n);
				unlink(temp_path);
				if (n < p->r_nnn_count) {
					sprintf(temp_path,"%s.%s.%03d",workfname,"r6",n + 1);
					//fio_open(&fp_r6,temp_path,FIO_MODE_R);
					fio_open_with_set_param(&fp_r6,temp_path,FIO_MODE_R,&fio_common);
					if (fp_r6.error) {
						PRINT_LOG("error");
						param->err = STATUS_FILE_READ_ERR;
						break;
					}
				}
			}
		}

PRINT_LOG("");
		strcpy(temp_path,workfname);
		strcat(temp_path,".r1");
		fio_setmode_r(&fp_w1,&fp_r,temp_path);
		if (fp_w1.error) {
			PRINT_LOG("error");
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}
		if (fp_r.error) {
			PRINT_LOG("error");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_close(&fp_r);
		if (p->realCh >= 2) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r2");
			fio_setmode_r(&fp_w2,&fp_r,temp_path);
			if (fp_w2.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 3 || p->LRC_process == 1) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r3");
			fio_setmode_r(&fp_w3,&fp_r,temp_path);
			if (fp_w3.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 4) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r4");
			fio_setmode_r(&fp_w4,&fp_r,temp_path);
			if (fp_w4.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 5) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r5");
			fio_setmode_r(&fp_w5,&fp_r,temp_path);
			if (fp_w5.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 6) {
			strcpy(temp_path,workfname);
			strcat(temp_path,".r6");
			fio_setmode_r(&fp_w6,&fp_r,temp_path);
			if (fp_w6.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_close(&fp_r);
		}
		// 30秒の境目で発生する段差の補正
		// 分割してないときは発生しないので分割している時だけ実施する
		if (p->r_nnn_count > 1) {
			SSIZE rd_sample,wr_sample;
			int addp_e1,addp_e2,addp_e3,addp_e4,addp_e5,addp_e6;

			addp_e1 = addp_e2 = addp_e3 = addp_e4 = addp_e5 = addp_e6 = 0;

			strcpy(temp_path,workfname);
			strcat(temp_path,".r1");

			fio_common.upconv_n    = fio_ch;
			//fio_open(&fp_r1,temp_path,FIO_MODE_R);
			fio_open_with_set_param(&fp_r1,temp_path,FIO_MODE_R,&fio_common);
			if (fp_r1.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			strcpy(temp_path,workfname);
			strcat(temp_path,".r1.tmp");
			//fio_open(&fp_w1,temp_path,FIO_MODE_W);
			fio_open_with_set_param(&fp_w1,temp_path,FIO_MODE_W,&fio_common);
			//fio_set_memory_limit(&fp_w1,20,p->fio);
			//fio_set_memory_limit(&fp_w1,p->outSampleR_final * sizeof (SSIZE),p->fio);
			fio_set_maxsize(&fp_w1,r_filesize[0]);
			if (fp_w1.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (p->realCh >= 2) {
				strcpy(temp_path,workfname);
				strcat(temp_path,".r2");
				//fio_open(&fp_r2,temp_path,FIO_MODE_R);
				fio_open_with_set_param(&fp_r2,temp_path,FIO_MODE_R,&fio_common);
				if (fp_r2.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				strcpy(temp_path,workfname);
				strcat(temp_path,".r2.tmp");
				//fio_open(&fp_w2,temp_path,FIO_MODE_W);
				fio_open_with_set_param(&fp_w2,temp_path,FIO_MODE_W,&fio_common);
				//fio_set_memory_limit(&fp_w2,20,p->fio);
				//fio_set_memory_limit(&fp_w2,p->outSampleR_final * sizeof (SSIZE),p->fio);
				fio_set_maxsize(&fp_w2,r_filesize[1]);
				if (fp_w2.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
			}
			if (p->realCh >= 3 || p->LRC_process == 1) {
				strcpy(temp_path,workfname);
				strcat(temp_path,".r3");
				//fio_open(&fp_r3,temp_path,FIO_MODE_R);
				fio_open_with_set_param(&fp_r3,temp_path,FIO_MODE_R,&fio_common);
				if (fp_r3.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				strcpy(temp_path,workfname);
				strcat(temp_path,".r3.tmp");
				//fio_open(&fp_w3,temp_path,FIO_MODE_W);
				fio_open_with_set_param(&fp_w3,temp_path,FIO_MODE_W,&fio_common);
				//fio_set_memory_limit(&fp_w3,20,p->fio);
				//fio_set_memory_limit(&fp_w3,p->outSampleR_final * sizeof (SSIZE),p->fio);
				fio_set_maxsize(&fp_w3,r_filesize[2]);
				if (fp_w3.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
			}
			if (p->realCh >= 4) {
				strcpy(temp_path,workfname);
				strcat(temp_path,".r4");
				//fio_open(&fp_r4,temp_path,FIO_MODE_R);
				fio_open_with_set_param(&fp_r4,temp_path,FIO_MODE_R,&fio_common);
				if (fp_r4.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				strcpy(temp_path,workfname);
				strcat(temp_path,".r4.tmp");
				//fio_open(&fp_w4,temp_path,FIO_MODE_W);
				fio_open_with_set_param(&fp_w4,temp_path,FIO_MODE_W,&fio_common);
				//fio_set_memory_limit(&fp_w4,20,p->fio);
				//fio_set_memory_limit(&fp_w4,p->outSampleR_final * sizeof (SSIZE),p->fio);
				fio_set_maxsize(&fp_w4,r_filesize[3]);
				if (fp_w4.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
			}
			if (p->realCh >= 5) {
				strcpy(temp_path,workfname);
				strcat(temp_path,".r5");
				//fio_open(&fp_r5,temp_path,FIO_MODE_W);
				fio_open_with_set_param(&fp_r5,temp_path,FIO_MODE_R,&fio_common);
				if (fp_r5.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				strcpy(temp_path,workfname);
				strcat(temp_path,".r5.tmp");
				//fio_open(&fp_w5,temp_path,FIO_MODE_W);
				fio_open_with_set_param(&fp_w5,temp_path,FIO_MODE_W,&fio_common);
				//fio_set_memory_limit(&fp_w5,20,p->fio);
				//fio_set_memory_limit(&fp_w5,p->outSampleR_final * sizeof (SSIZE),p->fio);
				fio_set_maxsize(&fp_w5,r_filesize[4]);
				if (fp_w5.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
			}
			if (p->realCh >= 6) {
				strcpy(temp_path,workfname);
				strcat(temp_path,".r6");
				//fio_open(&fp_r6,temp_path,FIO_MODE_R);
				fio_open_with_set_param(&fp_r6,temp_path,FIO_MODE_R,&fio_common);
				if (fp_r6.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				strcpy(temp_path,workfname);
				strcat(temp_path,".r6.tmp");
				//fio_open(&fp_w6,temp_path,FIO_MODE_W);
				fio_open_with_set_param(&fp_w6,temp_path,FIO_MODE_W,&fio_common);
				//fio_set_memory_limit(&fp_w6,20,p->fio);
				//fio_set_memory_limit(&fp_w6,p->outSampleR_final * sizeof (SSIZE),p->fio);
				fio_set_maxsize(&fp_w6,r_filesize[5]);
				if (fp_w6.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
			}

			rd_sample = param->r2w_outSampleR * 10;
			// 10秒分を最初に書き込む
			#pragma omp parallel
			{
				#pragma omp sections
				{
					#pragma omp section
					{
						wr_sample = CopyData(&fp_r1,&fp_w1,rd_sample,disk_buffer1,1.0,1.0,0,&addp_e1);
					}
					#pragma omp section
					{
						if (p->realCh >= 2) {
							CopyData(&fp_r2,&fp_w2,rd_sample,disk_buffer2,1.0,1.0,0,&addp_e2);
						}
					}
					#pragma omp section
					{
						if (p->realCh >= 3 || p->LRC_process == 1) {
							CopyData(&fp_r3,&fp_w3,rd_sample,disk_buffer3,1.0,1.0,0,&addp_e3);
						}
					}
					#pragma omp section
					{
						if (p->realCh >= 4) {
							CopyData(&fp_r4,&fp_w4,rd_sample,disk_buffer4,1.0,1.0,0,&addp_e4);
						}
					}
					#pragma omp section
					{
						if (p->realCh >= 5) {
							CopyData(&fp_r5,&fp_w5,rd_sample,disk_buffer5,1.0,1.0,0,&addp_e5);
						}
					}
					#pragma omp section
					{
						if (p->realCh >= 6) {
							CopyData(&fp_r6,&fp_w6,rd_sample,disk_buffer6,1.0,1.0,0,&addp_e6);
						}
					}
				}
			}
			addp_e1 = addp_e1 | addp_e2 | addp_e3 | addp_e4 | addp_e5 | addp_e6;
			if (addp_e1) {
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}

			// 残りのデータ
			while (rd_sample == wr_sample) {
				rd_sample = param->r2w_outSampleR * 30;
				// 30秒分を書き込む(20秒目の位置に境目がある)
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							wr_sample = CopyDataAdj(&fp_r1,&fp_w1,rd_sample,disk_buffer1,&addp_e1,param);
						}
						#pragma omp section
						{
							if (p->realCh >= 2) {
								CopyDataAdj(&fp_r2,&fp_w2,rd_sample,disk_buffer2,&addp_e2,param);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 3 || p->LRC_process == 1) {
								CopyDataAdj(&fp_r3,&fp_w3,rd_sample,disk_buffer3,&addp_e3,param);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 4) {
								CopyDataAdj(&fp_r4,&fp_w4,rd_sample,disk_buffer4,&addp_e4,param);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 5) {
								CopyDataAdj(&fp_r5,&fp_w5,rd_sample,disk_buffer5,&addp_e5,param);
							}
						}
						#pragma omp section
						{
							if (p->realCh >= 6) {
								CopyDataAdj(&fp_r6,&fp_w6,rd_sample,disk_buffer6,&addp_e6,param);
							}
						}
					}
				}
				addp_e1 = addp_e1 | addp_e2 | addp_e3 | addp_e4 | addp_e5 | addp_e6;
				if (addp_e1) {
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
			}
			fio_close(&fp_r1);
			strcpy(temp_path,workfname);
			strcat(temp_path,".r1");
			fio_setmode_r(&fp_w1,&fp_r,temp_path);
			if (fp_w1.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				PRINT_LOG("error");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_close(&fp_r);
			if (p->realCh >= 2) {
				fio_close(&fp_r2);
				strcpy(temp_path,workfname);
				strcat(temp_path,".r2");
				fio_setmode_r(&fp_w2,&fp_r,temp_path);
				if (fp_w2.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
				if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				fio_close(&fp_r);
			}
			if (p->realCh >= 3 || p->LRC_process == 1) {
				fio_close(&fp_r3);
				strcpy(temp_path,workfname);
				strcat(temp_path,".r3");
				fio_setmode_r(&fp_w3,&fp_r,temp_path);
				if (fp_w3.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
				if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				fio_close(&fp_r);
			}
			if (p->realCh >= 4) {
				fio_close(&fp_r4);
				strcpy(temp_path,workfname);
				strcat(temp_path,".r4");
				fio_setmode_r(&fp_w4,&fp_r,temp_path);
				if (fp_w4.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
				if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				fio_close(&fp_r);
			}
			if (p->realCh >= 5) {
				fio_close(&fp_r5);
				strcpy(temp_path,workfname);
				strcat(temp_path,".r5");
				fio_setmode_r(&fp_w5,&fp_r,temp_path);
				if (fp_w5.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
				if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				fio_close(&fp_r);
			}
			if (p->realCh >= 6) {
				fio_close(&fp_r6);
				strcpy(temp_path,workfname);
				strcat(temp_path,".r6");
				fio_setmode_r(&fp_w6,&fp_r,temp_path);
				if (fp_w6.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
				if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
				}
				fio_close(&fp_r);
			}
		}
		// 最終的に結合したファイルサイズのチェックを実施する
		sprintf(temp_path,"%s.%s",workfname,"r1");
		fio_open(&fp_r,temp_path,FIO_MODE_R);
		if (fp_r.error) {
			PRINT_LOG("error");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_get_filesize(&fp_r,&size);
		if (r_filesize[0] != size) {
				PRINT_LOG("error");
				sprintf(sss,"r_filesize[0]=%lld,size=%lld",r_filesize[0],size);
				PRINT_LOG(sss);
				param->err = STATUS_FILE_WRITE_ERR;
				break;
		}
		fio_close(&fp_r);
		if (p->realCh >= 2) {
			sprintf(temp_path,"%s.%s",workfname,"r2");
			fio_open(&fp_r,temp_path,FIO_MODE_R);
			if (fp_r.error) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_READ_ERR;
					break;
			}
			fio_get_filesize(&fp_r,&size);
			if (r_filesize[1] != size) {
					PRINT_LOG("error");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 3 || p->LRC_process == 1) {
			sprintf(temp_path,"%s.%s",workfname,"r3");
			fio_open(&fp_r,temp_path,FIO_MODE_R);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r,&size);
			if (r_filesize[2] != size) {
					param->err = STATUS_FILE_WRITE_ERR;
					break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 4) {
			sprintf(temp_path,"%s.%s",workfname,"r4");
			fio_open(&fp_r,temp_path,FIO_MODE_R);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r,&size);
			if (r_filesize[3] != size) {
					param->err = STATUS_FILE_WRITE_ERR;
					break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 5) {
			sprintf(temp_path,"%s.%s",workfname,"r5");
			fio_open(&fp_r,temp_path,FIO_MODE_R);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r,&size);
			if (r_filesize[4] != size) {
					param->err = STATUS_FILE_WRITE_ERR;
					break;
			}
			fio_close(&fp_r);
		}
		if (p->realCh >= 6) {
			sprintf(temp_path,"%s.%s",workfname,"r6");
			fio_open(&fp_r,temp_path,FIO_MODE_R);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r,&size);
			if (r_filesize[5] != size) {
					param->err = STATUS_FILE_WRITE_ERR;
					break;
			}
			fio_close(&fp_r);
		}
	} while (0);

	if (declip_flag[0]) free(declip_flag[0]);
	if (declip_flag[1]) free(declip_flag[1]);
	if (declip_flag[2]) free(declip_flag[2]);
	if (declip_flag[3]) free(declip_flag[3]);
	if (declip_flag[4]) free(declip_flag[4]);
	if (declip_flag[5]) free(declip_flag[5]);

	if (disk_buffer1) free(disk_buffer1);
	if (disk_buffer2) free(disk_buffer2);
	if (disk_buffer3) free(disk_buffer3);
	if (disk_buffer4) free(disk_buffer4);
	if (disk_buffer5) free(disk_buffer5);
	if (disk_buffer6) free(disk_buffer6);

	PRINT_FN_LOG(param->debug_upconv,"JoinFiles:End");

}
//---------------------------------------------------------------------------
// Function   : OutputMidSide
// Description: 検証用のファイルとしてMid/Side形式で出力する
// ---
//	fp_l	:音声データのFIO構造体(Left)
//	fp_r	:音声データのFIO構造体(Right)
//	fp_m	:音声データのFIO構造体(Mid)
//	fp_s	:音声データのFIO構造体(Side)
//	nSample :サンプル数
//	param	:パラメーター構造体
//  p       :UPCONV_PARAM
// return   : なし
void OutputMidSide(FIO *fp_l,FIO *fp_r,FIO *fp_m,FIO *fp_s,SSIZE nSample,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	SSIZE i,rdSample,rs,ws;
	SSIZE remainSample;
	SSIZE valueM,valueS;
	SSIZE wkMemSize;
	SSIZE *mem1,*mem2,*mem3,*mem4;

	PRINT_FN_LOG(param->debug_upconv,"OutputMidSide:Start");

	wkMemSize = (1 * 1024 * 1024L) * sizeof (SSIZE);

	mem1 = (SSIZE *)malloc(wkMemSize);
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		PRINT_LOG("ERROR");
		return;
	}
	mem2 = (SSIZE *)malloc(wkMemSize);
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		PRINT_LOG("ERROR");
		return;
	}
	mem3 = (SSIZE *)malloc(wkMemSize);
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		PRINT_LOG("ERROR");
		return;
	}
	mem4 = (SSIZE *)malloc(wkMemSize);
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		PRINT_LOG("ERROR");
		return;
	}

	remainSample = nSample;

	do {
		rdSample = 1 * 1024 * 1024L;
		if (remainSample < rdSample) {
			rdSample = remainSample;
		}

		rs = fio_read(mem1,sizeof (SSIZE),rdSample,fp_l);
		if (rs != rdSample) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			PRINT_LOG("ERROR");
			break;
		}
		rs = fio_read(mem2,sizeof (SSIZE),rdSample,fp_r);
		if (rs != rdSample) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			PRINT_LOG("ERROR");
			break;
		}
		memset(mem3,0,sizeof (SSIZE) * rdSample);
		memset(mem4,0,sizeof (SSIZE) * rdSample);

		for (i = 0;i < rdSample;i++) {
			valueM = (mem1[i] + mem2[i]) / 2;
			valueS = (mem1[i] - mem2[i]) / 2;
//			valueM = mem1[i] + mem2[i];
//			valueS = mem1[i] - mem2[i];
			mem3[i] = valueM;
			mem4[i] = valueS;
		}

		ws = fio_write(mem3,sizeof (SSIZE),rdSample,fp_m);
		if (ws != rdSample) {
			param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			PRINT_LOG("ERROR");
			break;
		}
		ws = fio_write(mem4,sizeof (SSIZE),rdSample,fp_s);
		if (ws != rdSample) {
			param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			PRINT_LOG("ERROR");
			break;
		}
		
		if (remainSample < 1 * 1024 * 1024L) {
			break;
		}
		remainSample -= 1 * 1024 * 1024L;
	} while (1);

	if (mem1 != NULL) free(mem1);
	if (mem2 != NULL) free(mem2);
	if (mem3 != NULL) free(mem3);
	if (mem4 != NULL) free(mem4);
	
	PRINT_FN_LOG(param->debug_upconv,"OutputMidSide:End");	
}
//---------------------------------------------------------------------------
// Function   : CopyData
// Description: ファイルのコピー
// ---
//	fp_r	:音声データのFIO構造体(読み込み)
//	fp_w	:音声データのFIO構造体(書き込み)
//	nSample :サンプル数
//	buffer  :データ用バッファ
//
// return   : 読み込んだサンプル数
SSIZE CopyData(FIO *fp_r,FIO *fp_w,SSIZE nSample,SSIZE *buffer,double per,double clip_nx,int norm_flag,int *error)
{
	SSIZE i,rd_n,wr_n;

	*error = 0;

	rd_n = fio_read(buffer,sizeof (SSIZE),nSample,fp_r);
	if (fp_r->error) {
		PRINT_LOG("CopyData Errorrrrrrrr");
		return 0;
	}
	if (rd_n > 0) {
		if (norm_flag != 0) {
			// ノーマライズ or vla_auto なので音量を調整する
			for (i = 0;i < rd_n;i++) {
				buffer[i] = (SSIZE)((double)buffer[i] * per);
			}
		}
		if (clip_nx < 1.0) {
			for (i = 0;i < rd_n;i++) {
				buffer[i] = (SSIZE)((double)buffer[i] * clip_nx);
			}
		}
		wr_n = fio_write(buffer,sizeof (SSIZE),rd_n,fp_w);
		if (fp_w->error) {
			*error = -1;
		}
		if (rd_n != wr_n) {
			*error = -1;
		}
	}
	return rd_n;
}
//---------------------------------------------------------------------------
// Function   : CopyDataAdj
// Description: ファイルのコピー(調整処理あり)
// ---
//	fp_r	:音声データのFIO構造体(読み込み)
//	fp_w	:音声データのFIO構造体(書き込み)
//	nSample :サンプル数
//	buffer  :データ用バッファ
//
// return   : 読み込んだサンプル数
SSIZE CopyDataAdj(FIO *fp_r,FIO *fp_w,SSIZE nSample,SSIZE *buffer,int *error,PARAM_INFO_TO_WAV *param)
{
	char sss[100];
	SSIZE i,ii,rd_n,wr_n;
	double temp_per1,temp_per2,temp_per3,temp_per4,temp_per5,temp_per6,temp_per7,temp_per8,temp_per9,temp_per10;

	PRINT_FN_LOG(param->debug_upconv,"CopyDataAdj:Start");

	*error = 0;

	rd_n = fio_read(buffer,sizeof (SSIZE),nSample,fp_r);
	if (fp_r->error) {
		return 0;
	}
	if (rd_n > 0) {
		if (rd_n > (20 * param->r2w_outSampleR) + 11) {
			int min_diff_index = 0;
			SSIZE val;
			for (ii = 1;ii < 10;ii++) {
				if (abs(buffer[20 * param->r2w_outSampleR - 2] - buffer[(20 * param->r2w_outSampleR - 1) +  ii]) < abs(buffer[(20 * param->r2w_outSampleR) - 2] - buffer[(20 * param->r2w_outSampleR - 1) +  min_diff_index])) {					min_diff_index = ii;
				}
			}
			for (ii = min_diff_index;ii >= 0;ii--) {
				buffer[(20 * param->r2w_outSampleR - 1) + ii] = buffer[(20 * param->r2w_outSampleR - 1) +  min_diff_index];
			}
			for (ii = -6;ii <= 6;ii++) {
				buffer[(20 * param->r2w_outSampleR - 1) + ii] = (buffer[(20 * param->r2w_outSampleR - 1) + (ii - 1)] + buffer[(20 * param->r2w_outSampleR - 1) + (ii - 0)] + buffer[(20 * param->r2w_outSampleR - 1) + (ii + 1)]) / 3;
			}
			for (ii = -6;ii <= 6;ii++) {
				buffer[(20 * param->r2w_outSampleR - 1) + ii] = (buffer[(20 * param->r2w_outSampleR - 1) + (ii - 1)] + buffer[(20 * param->r2w_outSampleR - 1) + (ii - 0)] + buffer[(20 * param->r2w_outSampleR - 1) + (ii + 1)]) / 3;
			}
			for (ii = -6;ii <= 6;ii++) {
				buffer[(20 * param->r2w_outSampleR - 1) + ii] = (buffer[(20 * param->r2w_outSampleR - 1) + (ii - 1)] + buffer[(20 * param->r2w_outSampleR - 1) + (ii - 0)] + buffer[(20 * param->r2w_outSampleR - 1) + (ii + 1)]) / 3;
			}
		}
		wr_n = fio_write(buffer,sizeof (SSIZE),rd_n,fp_w);
		if (fp_w->error) {
			*error = -1;
		}
		if (rd_n != wr_n) {
			*error = -1;
		}
	}
	PRINT_FN_LOG(param->debug_upconv,"CopyDataAdj:End");

	return rd_n;
}
//---------------------------------------------------------------------------
// Function   : updateLevelInfo
// Description: 音量情報を更新する
// ---
//	inSample 		:処理するサンプル数(ch毎)
//  ch_num			:チャンネル数
//	fp_r1			:入力ファイル(Left)
//	fp_r2			:入力ファイル(Right)
//	fp_r3			:入力ファイル(3)
//	fp_r4			:入力ファイル(4)
//	fp_r5			:入力ファイル(5)
//	fp_r6			:入力ファイル(6)
//	levelInfo_from	:音量情報
//	levelInfo_to	:音量情報
//	param			:変換パラメータ
//
int updateLevelInfo(SSIZE inSample,int ch_num,FIO *fp_r1,FIO *fp_r2,FIO *fp_r3,FIO *fp_r4,FIO *fp_r5,FIO *fp_r6,LEVEL_INFO *levelInfo_from,LEVEL_INFO *levelInfo_to,UPCONV_PARAM *p)
{
	SSIZE i,n;
	SSIZE remain;
	SSIZE wkMemSize,rd1,rd2,rd3,rd4,rd5,rd6;
	SSIZE *mem1,*mem2,*mem3,*mem4,*mem5,*mem6;
	SSIZE startInSample;
	SSIZE peak1,peak2,real_peak2;
	SSIZE avg1,avg2;
	LEVEL_INFO *levelInfo;
	double per1,per2,per3,per4,per5,per6;
	double m_per1,m_per2,m_per3,m_per4,m_per5,m_per6;
	double nx;
	double percent,per;
	char sss[300];

//	PRINT_FN_LOG("updateLevelInfo:Start");

	if (fp_r1 != NULL) fio_rewind(fp_r1);
	if (fp_r2 != NULL) fio_rewind(fp_r2);
	if (fp_r3 != NULL) fio_rewind(fp_r3);
	if (fp_r4 != NULL) fio_rewind(fp_r4);
	if (fp_r5 != NULL) fio_rewind(fp_r5);
	if (fp_r6 != NULL) fio_rewind(fp_r6);

	wkMemSize = 48000 * 10 * sizeof (SSIZE);
	mem1 = (SSIZE *)malloc(wkMemSize);
	if (mem1 == NULL) {
		return -1;
	}
	mem2 = (SSIZE *)malloc(wkMemSize);
	if (mem2 == NULL) {
		return -1;
	}
	mem3 = (SSIZE *)malloc(wkMemSize);
	if (mem3 == NULL) {
		return -1;
	}
	mem4 = (SSIZE *)malloc(wkMemSize);
	if (mem4 == NULL) {
		return -1;
	}
	mem5 = (SSIZE *)malloc(wkMemSize);
	if (mem5 == NULL) {
		return -1;
	}
	mem6 = (SSIZE *)malloc(wkMemSize);
	if (mem6 == NULL) {
		return -1;
	}

	levelInfo = levelInfo_from;
	if (levelInfo_to != NULL) levelInfo = levelInfo_to;

	for (startInSample = 0;startInSample < inSample;) {
		percent = ((double)startInSample / inSample);
		percent *= 100;
		if (percent != per) {
			if (p->simple_mode == 0) {
				Sleep(1);
				fprintf(stdout,"%s%d%%\n","[LA]",(int)percent);
				fflush(stdout);
			}
			if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
		}
		per = percent;

		if (ch_num >= 1) {
			rd1 = fio_read(mem1,sizeof (SSIZE),48000 * 10,fp_r1);
			for (i = 0;i < rd1;i++) {
				if ((mem1[i] >> 40) > 0) {
					if (levelInfo->r1_peak < (mem1[i] >> 40)) {
						levelInfo->r1_peak = (mem1[i] >> 40);
					}

					levelInfo->r1_work += (mem1[i] >> 40);
					levelInfo->r1_count++;
					if (levelInfo->r1_work >= 0x1000000000000) {
						levelInfo->r1_work /= levelInfo->r1_count;
						levelInfo->r1_count = 0;
						if (levelInfo->r1_avg > 0) {
							levelInfo->r1_avg = (levelInfo->r1_avg + levelInfo->r1_work) / 2;
						} else {
							levelInfo->r1_avg = levelInfo->r1_work;
						}
						levelInfo->r1_work = 0;
					}
				} else if ((mem1[i] >> 40) < 0) {
					if (levelInfo->r1_m_peak > (mem1[i] >> 40)) {
						levelInfo->r1_m_peak = (mem1[i] >> 40);
					}

					levelInfo->r1_m_work += (mem1[i] >> 40);
					levelInfo->r1_m_count++;
					if (levelInfo->r1_m_work <= -0x1000000000000) {
						levelInfo->r1_m_work /= levelInfo->r1_m_count;
						levelInfo->r1_m_count = 0;
						if (levelInfo->r1_m_avg > 0) {
							levelInfo->r1_m_avg = (levelInfo->r1_m_avg + levelInfo->r1_m_work) / 2;
						} else {
							levelInfo->r1_m_avg = levelInfo->r1_m_work;
						}
						levelInfo->r1_m_work = 0;
					}
				}
			}
		}
		if (ch_num >= 2) {
			rd2 = fio_read(mem2,sizeof (SSIZE),48000 * 10,fp_r2);
			if (rd1 != rd2) break;
			for (i = 0;i < rd2;i++) {
				if ((mem2[i] >> 40) > 0) {
					if (levelInfo->r2_peak < (mem2[i] >> 40)) {
						levelInfo->r2_peak = (mem2[i] >> 40);
					}

					levelInfo->r2_work += (mem2[i] >> 40);
					levelInfo->r2_count++;
					if (levelInfo->r2_work >= 0x1000000000000) {
						levelInfo->r2_work /= levelInfo->r2_count;
						levelInfo->r2_count = 0;
						if (levelInfo->r2_avg > 0) {
							levelInfo->r2_avg = (levelInfo->r2_avg + levelInfo->r2_work) / 2;
						} else {
							levelInfo->r2_avg = levelInfo->r2_work;
						}
						levelInfo->r2_work = 0;
					}
				} else if ((mem2[i] >> 40) < 0) {
					if (levelInfo->r2_m_peak > (mem2[i] >> 40)) {
						levelInfo->r2_m_peak = (mem2[i] >> 40);
					}

					levelInfo->r2_m_work += (mem2[i] >> 40);
					levelInfo->r2_m_count++;
					if (levelInfo->r2_m_work <= -0x1000000000000) {
						levelInfo->r2_m_work /= levelInfo->r2_m_count;
						levelInfo->r2_m_count = 0;
						if (levelInfo->r2_m_avg > 0) {
							levelInfo->r2_m_avg = (levelInfo->r2_m_avg + levelInfo->r2_m_work) / 2;
						} else {
							levelInfo->r2_m_avg = levelInfo->r2_m_work;
						}
						levelInfo->r2_m_work = 0;
					}
				}
			}
		}
		if (ch_num >= 3) {
			rd3 = fio_read(mem3,sizeof (SSIZE),48000 * 10,fp_r3);
			if (rd1 != rd3) break;
			for (i = 0;i < rd3;i++) {
				if ((mem3[i] >> 40) > 0) {
					if (levelInfo->r3_m_peak > (mem3[i] >> 40)) {
						levelInfo->r3_m_peak = (mem3[i] >> 40);
					}

					levelInfo->r3_work += (mem3[i] >> 40);
					levelInfo->r3_count++;
					if (levelInfo->r3_work >= 0x1000000000000) {
						levelInfo->r3_work /= levelInfo->r3_count;
						levelInfo->r3_count = 0;
						if (levelInfo->r3_avg > 0) {
							levelInfo->r3_avg = (levelInfo->r3_avg + levelInfo->r3_work) / 2;
						} else {
							levelInfo->r3_avg = levelInfo->r3_work;
						}
						levelInfo->r3_work = 0;
					}
				} else if ((mem3[i] >> 32) < 0) {
					if (levelInfo->r3_m_peak > (mem3[i] >> 40)) {
						levelInfo->r3_m_peak = (mem3[i] >> 40);
					}

					levelInfo->r3_m_work += (mem3[i] >> 40);
					levelInfo->r3_m_count++;
					if (levelInfo->r3_m_work <= -0x1000000000000) {
						levelInfo->r3_m_work /= levelInfo->r3_m_count;
						levelInfo->r3_m_count = 0;
						if (levelInfo->r3_m_avg > 0) {
							levelInfo->r3_m_avg = (levelInfo->r3_m_avg + levelInfo->r3_m_work) / 2;
						} else {
							levelInfo->r3_m_avg = levelInfo->r3_m_work;
						}
						levelInfo->r3_m_work = 0;
					}
				}
			}
		}
		if (ch_num >= 4) {
			rd4 = fio_read(mem4,sizeof (SSIZE),48000 * 10,fp_r4);
			if (rd1 != rd4) break;
			for (i = 0;i < rd4;i++) {
				if ((mem4[i] >> 40) > 0) {
					if (levelInfo->r4_m_peak > (mem4[i] >> 40)) {
						levelInfo->r4_m_peak = (mem4[i] >> 40);
					}

					levelInfo->r4_work += (mem4[i] >> 40);
					levelInfo->r4_count++;
					if (levelInfo->r4_work >= 0x1000000000000) {
						levelInfo->r4_work /= levelInfo->r4_count;
						levelInfo->r4_count = 0;
						if (levelInfo->r4_avg > 0) {
							levelInfo->r4_avg = (levelInfo->r4_avg + levelInfo->r4_work) / 2;
						} else {
							levelInfo->r4_avg = levelInfo->r4_work;
						}
						levelInfo->r4_work = 0;
					}
				} else if ((mem4[i] >> 40) < 0) {
					if (levelInfo->r4_m_peak > (mem4[i] >> 40)) {
						levelInfo->r4_m_peak = (mem4[i] >> 40);
					}

					levelInfo->r4_m_work += (mem4[i] >> 40);
					levelInfo->r4_m_count++;
					if (levelInfo->r4_m_work <= -0x1000000000000) {
						levelInfo->r4_m_work /= levelInfo->r4_m_count;
						levelInfo->r4_m_count = 0;
						if (levelInfo->r4_m_avg > 0) {
							levelInfo->r4_m_avg = (levelInfo->r4_m_avg + levelInfo->r4_m_work) / 2;
						} else {
							levelInfo->r4_m_avg = levelInfo->r4_m_work;
						}
						levelInfo->r4_m_work = 0;
					}
				}
			}
		}
		if (ch_num >= 5) {
			rd5 = fio_read(mem5,sizeof (SSIZE),48000 * 10,fp_r5);
			if (rd1 != rd5) break;
			for (i = 0;i < rd5;i++) {
				if ((mem5[i] >> 40) > 0) {
					if (levelInfo->r5_m_peak > (mem5[i] >> 40)) {
						levelInfo->r5_m_peak = (mem5[i] >> 40);
					}

					levelInfo->r5_work += (mem5[i] >> 40);
					levelInfo->r5_count++;
					if (levelInfo->r5_work >= 0x1000000000000) {
						levelInfo->r5_work /= levelInfo->r5_count;
						levelInfo->r5_count = 0;
						if (levelInfo->r5_avg > 0) {
							levelInfo->r5_avg = (levelInfo->r5_avg + levelInfo->r5_work) / 2;
						} else {
							levelInfo->r5_avg = levelInfo->r5_work;
						}
						levelInfo->r5_work = 0;
					}
				} else if ((mem5[i] >> 40) < 0) {
					if (levelInfo->r5_m_peak > (mem5[i] >> 40)) {
						levelInfo->r5_m_peak = (mem5[i] >> 40);
					}

					levelInfo->r5_m_work += (mem5[i] >> 40);
					levelInfo->r5_m_count++;
					if (levelInfo->r5_m_work <= -0x1000000000000) {
						levelInfo->r5_m_work /= levelInfo->r5_m_count;
						levelInfo->r5_m_count = 0;
						if (levelInfo->r5_m_avg > 0) {
							levelInfo->r5_m_avg = (levelInfo->r5_m_avg + levelInfo->r5_m_work) / 2;
						} else {
							levelInfo->r5_m_avg = levelInfo->r5_m_work;
						}
						levelInfo->r5_m_work = 0;
					}
				}
			}
		}
		if (ch_num >= 6) {
			rd6 = fio_read(mem6,sizeof (SSIZE),48000 * 10,fp_r6);
			if (rd1 != rd6) break;
			for (i = 0;i < rd6;i++) {
				if ((mem6[i] >> 40) > 0) {
					if (levelInfo->r6_m_peak > (mem6[i] >> 40)) {
						levelInfo->r6_m_peak = (mem6[i] >> 40);
					}

					levelInfo->r6_work += (mem6[i] >> 40);
					levelInfo->r6_count++;
					if (levelInfo->r6_work >= 0x1000000000000) {
						levelInfo->r6_work /= levelInfo->r6_count;
						levelInfo->r6_count = 0;
						if (levelInfo->r6_avg > 0) {
							levelInfo->r6_avg = (levelInfo->r6_avg + levelInfo->r6_work) / 2;
						} else {
							levelInfo->r6_avg = levelInfo->r6_work;
						}
						levelInfo->r6_work = 0;
					}
				} else if ((mem6[i] >> 40) < 0) {
					if (levelInfo->r6_m_peak > (mem6[i] >> 40)) {
						levelInfo->r6_m_peak = (mem6[i] >> 40);
					}

					levelInfo->r6_m_work += (mem6[i] >> 40);
					levelInfo->r6_m_count++;
					if (levelInfo->r6_m_work <= -0x1000000000000) {
						levelInfo->r6_m_work /= levelInfo->r6_m_count;
						levelInfo->r6_m_count = 0;
						if (levelInfo->r6_m_avg > 0) {
							levelInfo->r6_m_avg = (levelInfo->r6_m_avg + levelInfo->r6_m_work) / 2;
						} else {
							levelInfo->r6_m_avg = levelInfo->r6_m_work;
						}
						levelInfo->r6_m_work = 0;
					}
				}
			}
		}
		remain = (inSample - startInSample);
		if (remain > 48000 * 10) remain = 48000 * 10;
		startInSample += remain;
	}

	if (ch_num >= 1 && levelInfo->r1_count > 0) {
		levelInfo->r1_work /= levelInfo->r1_count;
		levelInfo->r1_count = 0;
		if (levelInfo->r1_avg > 0) {
			levelInfo->r1_avg = (levelInfo->r1_avg + levelInfo->r1_work) / 2;
		} else {
			levelInfo->r1_avg = levelInfo->r1_work;
		}
		levelInfo->r1_work = 0;
	}
	if (ch_num >= 2 && levelInfo->r2_count > 0) {
		levelInfo->r2_work /= levelInfo->r2_count;
		levelInfo->r2_count = 0;
		if (levelInfo->r2_avg > 0) {
			levelInfo->r2_avg = (levelInfo->r2_avg + levelInfo->r2_work) / 2;
		} else {
			levelInfo->r2_avg = levelInfo->r2_work;
		}
		levelInfo->r2_work = 0;
	}
	if (ch_num >= 3 && levelInfo->r3_count > 0) {
		levelInfo->r3_work /= levelInfo->r3_count;
		levelInfo->r3_count = 0;
		if (levelInfo->r3_avg > 0) {
			levelInfo->r3_avg = (levelInfo->r3_avg + levelInfo->r3_work) / 2;
		} else {
			levelInfo->r3_avg = levelInfo->r3_work;
		}
		levelInfo->r3_work = 0;
	}
	if (ch_num >= 4 && levelInfo->r4_count > 0) {
		levelInfo->r4_work /= levelInfo->r4_count;
		levelInfo->r4_count = 0;
		if (levelInfo->r4_avg > 0) {
			levelInfo->r4_avg = (levelInfo->r4_avg + levelInfo->r4_work) / 2;
		} else {
			levelInfo->r4_avg = levelInfo->r4_work;
		}
		levelInfo->r4_work = 0;
	}
	if (ch_num >= 5 && levelInfo->r5_count > 0) {
		levelInfo->r5_work /= levelInfo->r5_count;
		levelInfo->r5_count = 0;
		if (levelInfo->r5_avg > 0) {
			levelInfo->r5_avg = (levelInfo->r5_avg + levelInfo->r5_work) / 2;
		} else {
			levelInfo->r5_avg = levelInfo->r5_work;
		}
		levelInfo->r5_work = 0;
	}
	if (ch_num >= 6 && levelInfo->r6_count > 0) {
		levelInfo->r6_work /= levelInfo->r6_count;
		levelInfo->r6_count = 0;
		if (levelInfo->r6_avg > 0) {
			levelInfo->r6_avg = (levelInfo->r6_avg + levelInfo->r6_work) / 2;
		} else {
			levelInfo->r6_avg = levelInfo->r6_work;
		}
		levelInfo->r6_work = 0;
	}

	if (ch_num >= 1 && levelInfo->r1_m_count > 0) {
		levelInfo->r1_m_work /= levelInfo->r1_m_count;
		levelInfo->r1_m_count = 0;
		if (levelInfo->r1_m_avg < 0) {
			levelInfo->r1_m_avg = (levelInfo->r1_m_avg + levelInfo->r1_m_work) / 2;
		} else {
			levelInfo->r1_m_avg = levelInfo->r1_m_work;
		}
		levelInfo->r1_m_work = 0;
	}
	if (ch_num >= 2 && levelInfo->r2_m_count > 0) {
		levelInfo->r2_m_work /= levelInfo->r2_m_count;
		levelInfo->r2_m_count = 0;
		if (levelInfo->r2_m_avg < 0) {
			levelInfo->r2_m_avg = (levelInfo->r2_m_avg + levelInfo->r2_m_work) / 2;
		} else {
			levelInfo->r2_m_avg = levelInfo->r2_m_work;
		}
		levelInfo->r2_m_work = 0;
	}
	if (ch_num >= 3 && levelInfo->r3_m_count > 0) {
		levelInfo->r3_m_work /= levelInfo->r3_m_count;
		levelInfo->r3_m_count = 0;
		if (levelInfo->r3_m_avg < 0) {
			levelInfo->r3_m_avg = (levelInfo->r3_m_avg + levelInfo->r3_m_work) / 2;
		} else {
			levelInfo->r3_m_avg = levelInfo->r3_m_work;
		}
		levelInfo->r3_m_work = 0;
	}
	if (ch_num >= 4 && levelInfo->r4_m_count > 0) {
		levelInfo->r4_m_work /= levelInfo->r4_m_count;
		levelInfo->r4_m_count = 0;
		if (levelInfo->r4_m_avg < 0) {
			levelInfo->r4_m_avg = (levelInfo->r4_m_avg + levelInfo->r4_m_work) / 2;
		} else {
			levelInfo->r4_m_avg = levelInfo->r4_m_work;
		}
		levelInfo->r4_m_work = 0;
	}
	if (ch_num >= 5 && levelInfo->r5_m_count > 0) {
		levelInfo->r5_m_work /= levelInfo->r5_m_count;
		levelInfo->r5_m_count = 0;
		if (levelInfo->r5_m_avg < 0) {
			levelInfo->r5_m_avg = (levelInfo->r5_m_avg + levelInfo->r5_m_work) / 2;
		} else {
			levelInfo->r5_m_avg = levelInfo->r5_m_work;
		}
		levelInfo->r5_m_work = 0;
	}
	if (ch_num >= 6 && levelInfo->r6_m_count > 0) {
		levelInfo->r6_m_work /= levelInfo->r6_m_count;
		levelInfo->r6_m_count = 0;
		if (levelInfo->r6_m_avg < 0) {
			levelInfo->r6_m_avg = (levelInfo->r6_m_avg + levelInfo->r6_m_work) / 2;
		} else {
			levelInfo->r6_m_avg = levelInfo->r6_m_work;
		}
		levelInfo->r6_m_work = 0;
	}
	if (levelInfo->r1_peak >= (SSIZE)0x7FFF) levelInfo->clip = 1;
	if (levelInfo->r2_peak >= (SSIZE)0x7FFF) levelInfo->clip = 1;
	if (levelInfo->r3_peak >= (SSIZE)0x7FFF) levelInfo->clip = 1;
	if (levelInfo->r4_peak >= (SSIZE)0x7FFF) levelInfo->clip = 1;
	if (levelInfo->r5_peak >= (SSIZE)0x7FFF) levelInfo->clip = 1;
	if (levelInfo->r6_peak >= (SSIZE)0x7FFF) levelInfo->clip = 1;

	if (levelInfo->r1_m_peak > (SSIZE)0x7FFF * -1) levelInfo->clip = 1;
	if (levelInfo->r2_m_peak > (SSIZE)0x7FFF * -1) levelInfo->clip = 1;
	if (levelInfo->r3_m_peak > (SSIZE)0x7FFF * -1) levelInfo->clip = 1;
	if (levelInfo->r4_m_peak > (SSIZE)0x7FFF * -1) levelInfo->clip = 1;
	if (levelInfo->r5_m_peak > (SSIZE)0x7FFF * -1) levelInfo->clip = 1;
	if (levelInfo->r6_m_peak > (SSIZE)0x7FFF * -1) levelInfo->clip = 1;

	per1 = ((double)levelInfo->r1_avg / 20907) * 100;
	per2 = ((double)levelInfo->r2_avg / 20907) * 100;
	per3 = ((double)levelInfo->r2_avg / 20907) * 100;
	per4 = ((double)levelInfo->r2_avg / 20907) * 100;
	per5 = ((double)levelInfo->r2_avg / 20907) * 100;
	per6 = ((double)levelInfo->r2_avg / 20907) * 100;
	m_per1 = (((double)levelInfo->r1_avg * -1) / 20907) * 100;
	m_per2 = (((double)levelInfo->r2_avg * -1) / 20907) * 100;
	m_per3 = (((double)levelInfo->r2_avg * -1) / 20907) * 100;
	m_per4 = (((double)levelInfo->r2_avg * -1) / 20907) * 100;
	m_per5 = (((double)levelInfo->r2_avg * -1) / 20907) * 100;
	m_per6 = (((double)levelInfo->r2_avg * -1) / 20907) * 100;
	if (per1 < per2) per1 = per2;
	if (per1 < per3) per1 = per3;
	if (per1 < per4) per1 = per4;
	if (per1 < per5) per1 = per5;
	if (per1 < per6) per1 = per6;
	if (per1 < m_per1) per1 = m_per1;
	if (per1 < m_per2) per1 = m_per2;
	if (per1 < m_per3) per1 = m_per3;
	if (per1 < m_per4) per1 = m_per4;
	if (per1 < m_per5) per1 = m_per5;
	if (per1 < m_per6) per1 = m_per6;
	levelInfo->level_per = per1;

	free(mem1);
	free(mem2);
	free(mem3);
	free(mem4);
	free(mem5);
	free(mem6);

//	PRINT_FN_LOG("updateLevelInfo:End");

	PRINT_LOG("Before:levelInfo_from != NULL && levelInfo_to != NULL");
	if (levelInfo_from != NULL && levelInfo_to != NULL) {
		PRINT_LOG("In:levelInfo_from != NULL && levelInfo_to != NULL");
		if (p->norm == 0) {
			peak1 = levelInfo_from->r1_peak;
			if (peak1 < levelInfo_from->r2_peak) peak1 = levelInfo_from->r2_peak;
			if (peak1 < levelInfo_from->r3_peak) peak1 = levelInfo_from->r3_peak;
			if (peak1 < levelInfo_from->r4_peak) peak1 = levelInfo_from->r4_peak;
			if (peak1 < levelInfo_from->r5_peak) peak1 = levelInfo_from->r5_peak;
			if (peak1 < levelInfo_from->r6_peak) peak1 = levelInfo_from->r6_peak;
			if (peak1 < (levelInfo_from->r1_m_peak * -1)) peak1 = levelInfo_from->r1_m_peak * -1;
			if (peak1 < (levelInfo_from->r2_m_peak * -1)) peak1 = levelInfo_from->r2_m_peak * -1;
			if (peak1 < (levelInfo_from->r3_m_peak * -1)) peak1 = levelInfo_from->r3_m_peak * -1;
			if (peak1 < (levelInfo_from->r4_m_peak * -1)) peak1 = levelInfo_from->r4_m_peak * -1;
			if (peak1 < (levelInfo_from->r5_m_peak * -1)) peak1 = levelInfo_from->r5_m_peak * -1;
			if (peak1 < (levelInfo_from->r6_m_peak * -1)) peak1 = levelInfo_from->r6_m_peak * -1;
		} else {
			peak1 = (SSIZE)32767;
		}
		peak1 = (double)peak1 * p->vla_nx;

		peak2 = levelInfo_to->r1_peak;
		if (peak2 < levelInfo_to->r2_peak) peak2 = levelInfo_to->r2_peak;
		if (peak2 < levelInfo_to->r3_peak) peak2 = levelInfo_to->r3_peak;
		if (peak2 < levelInfo_to->r4_peak) peak2 = levelInfo_to->r4_peak;
		if (peak2 < levelInfo_to->r5_peak) peak2 = levelInfo_to->r5_peak;
		if (peak2 < levelInfo_to->r6_peak) peak2 = levelInfo_to->r6_peak;
		if (peak2 < (levelInfo_to->r1_m_peak * -1)) peak2 = levelInfo_to->r1_m_peak * -1;
		if (peak2 < (levelInfo_to->r2_m_peak * -1)) peak2 = levelInfo_to->r2_m_peak * -1;
		if (peak2 < (levelInfo_to->r3_m_peak * -1)) peak2 = levelInfo_to->r3_m_peak * -1;
		if (peak2 < (levelInfo_to->r4_m_peak * -1)) peak2 = levelInfo_to->r4_m_peak * -1;
		if (peak2 < (levelInfo_to->r5_m_peak * -1)) peak2 = levelInfo_to->r5_m_peak * -1;
		if (peak2 < (levelInfo_to->r6_m_peak * -1)) peak2 = levelInfo_to->r6_m_peak * -1;

		avg1 = levelInfo_from->r1_avg;
		if (avg1 < levelInfo_from->r2_avg) avg1 = levelInfo_from->r2_avg;
		if (avg1 < levelInfo_from->r3_avg) avg1 = levelInfo_from->r3_avg;
		if (avg1 < levelInfo_from->r4_avg) avg1 = levelInfo_from->r4_avg;
		if (avg1 < levelInfo_from->r5_avg) avg1 = levelInfo_from->r5_avg;
		if (avg1 < levelInfo_from->r6_avg) avg1 = levelInfo_from->r6_avg;
		
		if (avg1 < (levelInfo_from->r1_m_avg * -1)) avg1 = (levelInfo_from->r1_m_avg * -1);
		if (avg1 < (levelInfo_from->r2_m_avg * -1)) avg1 = (levelInfo_from->r2_m_avg * -1);
		if (avg1 < (levelInfo_from->r3_m_avg * -1)) avg1 = (levelInfo_from->r3_m_avg * -1);
		if (avg1 < (levelInfo_from->r4_m_avg * -1)) avg1 = (levelInfo_from->r4_m_avg * -1);
		if (avg1 < (levelInfo_from->r5_m_avg * -1)) avg1 = (levelInfo_from->r5_m_avg * -1);
		if (avg1 < (levelInfo_from->r6_m_avg * -1)) avg1 = (levelInfo_from->r6_m_avg * -1);

		avg2 = levelInfo_to->r1_avg;
		if (avg2 < levelInfo_to->r2_avg) avg2 = levelInfo_to->r2_avg;
		if (avg2 < levelInfo_to->r3_avg) avg2 = levelInfo_to->r3_avg;
		if (avg2 < levelInfo_to->r4_avg) avg2 = levelInfo_to->r4_avg;
		if (avg2 < levelInfo_to->r5_avg) avg2 = levelInfo_to->r5_avg;
		if (avg2 < levelInfo_to->r6_avg) avg2 = levelInfo_to->r6_avg;
		
		if (avg2 < (levelInfo_to->r1_m_avg * -1)) avg2 = (levelInfo_to->r1_m_avg * -1);
		if (avg2 < (levelInfo_to->r2_m_avg * -1)) avg2 = (levelInfo_to->r2_m_avg * -1);
		if (avg2 < (levelInfo_to->r3_m_avg * -1)) avg2 = (levelInfo_to->r3_m_avg * -1);
		if (avg2 < (levelInfo_to->r4_m_avg * -1)) avg2 = (levelInfo_to->r4_m_avg * -1);
		if (avg2 < (levelInfo_to->r5_m_avg * -1)) avg2 = (levelInfo_to->r5_m_avg * -1);
		if (avg2 < (levelInfo_to->r6_m_avg * -1)) avg2 = (levelInfo_to->r6_m_avg * -1);

		if (p->volume_per_enable == 1) {
			if (levelInfo_to->level_per > 0) {
				real_peak2 = peak2;
				nx = (double)(32767 * p->volume_per) / real_peak2;
				levelInfo_to->nx = nx;
				sprintf(sss,"updateLevelInfo(volume_level_enable=0):nx:%lf",nx);
				PRINT_LOG("----------");
				PRINT_LOG(sss);
			} else {
				levelInfo_to->nx = 0;
			}
		} else {
			if (p->volume_level_enable == 0) {
				nx = 1.0;
				real_peak2 = peak2;
				
				if (avg1 > 0 && avg2 > 0) {
					nx = (double)avg1 / avg2;
					peak2 = (SSIZE)((double)peak1 / nx);
					if (peak2 > 0) {
						nx = (double)peak1 / peak2;
					}
				}
				if (p->norm == 1) {
					nx = (double)(32767 * p->vla_nx) / real_peak2;
				}
				if (p->clipped) {
					sprintf(sss,"----------------- clipped -----------------");
					PRINT_LOG(sss);
					do {
						if (IS_CLIP_MAX(real_peak2 << 40,nx) == 0) break;
						nx -= 0.01;
					} while (nx >= 0.73);
				}
				levelInfo_to->nx = nx;
				sprintf(sss,"updateLevelInfo(volume_level_enable=0):nx:%lf",nx);
				PRINT_LOG("----------");
				PRINT_LOG(sss);
			} else {
				if (levelInfo_to->level_per > 0) {
					nx = (double)p->volume_level / levelInfo_to->level_per;
					levelInfo_to->nx = nx;
					sprintf(sss,"updateLevelInfo:nx:%lf",nx);
					PRINT_LOG("----------");
					PRINT_LOG(sss);
				} else {
					levelInfo_to->nx = 0;
				}
			}
		}

		PRINT_LOG("End:levelInfo_from != NULL && levelInfo_to != NULL");
	}

	PRINT_LOG("End:updateLevelInfo");
	if (startInSample < inSample) {
		return -1;
	}
	return 0;
}

//---------------------------------------------------------------------------
// Function   : updateLevel
// Description: レベル調整処理(最終)
// ---
//	inSample		:サンプル数
//	in_num			:処理するチャンネル
//	fp_r			:音声データのFIO構造体(入力)
//	fp_w			:音声データのFIO構造体(出力)
//	levelInfo_from  :レベル情報(元)
//	levelInfo_to	:レベル情報(変換後)
//	p				:パラメーター構造体
//
int updateLevel(SSIZE inSample,int ch_num,FIO *fp_r,FIO *fp_w,LEVEL_INFO *levelInfo_from,LEVEL_INFO *levelInfo_to,UPCONV_PARAM *p)
{
	SSIZE i,n;
	SSIZE remain;
	SSIZE wkMemSize,rd1,wr1;
	SSIZE avg1,avg2;
	SSIZE *mem1;
	SSIZE startInSample;
	double nx;

//	PRINT_FN_LOG("updateLevelInfo:Start");

	if (fp_r != NULL) fio_rewind(fp_r);

	wkMemSize = 48000 * 10 * sizeof (SSIZE);
	mem1 = (SSIZE *)malloc(wkMemSize);
	if (mem1 == NULL) {
		return -1;
	}

	for (startInSample = 0;startInSample < inSample;) {
		rd1 = fio_read(mem1,sizeof (SSIZE),48000 * 10,fp_r);
		for (i = 0;i < rd1;i++) {
			mem1[i] = (SSIZE)((double)mem1[i] * levelInfo_to->nx);
		}
		wr1 = fio_write(mem1,sizeof (SSIZE),rd1,fp_w);
		if (rd1 != wr1) {
			break;
		}

		remain = (inSample - startInSample);
		if (remain > 48000 * 10) remain = 48000 * 10;
		startInSample += remain;
	}

	free(mem1);

//	PRINT_FN_LOG("updateLevelInfo:End");

	if (startInSample < inSample) {
		return -1;
	}
	return 0;
}
//---------------------------------------------------------------------------
// Function   : Normalize
// Description: ノーマライズ処理
// ---
//	pcount	:ファイル番号のアドレス
//	inFmt	:入力ファイル音声形式情報
//	outFmt	:出力ファイル音声形式情報
//	fp_r1	:音声データのFIO構造体
//	fp_r2	:音声データのFIO構造体
//	fp_r3	:音声データのFIO構造体
//	fp_r4	:音声データのFIO構造体
//	fp_r5	:音声データのFIO構造体
//	fp_r6	:音声データのFIO構造体
//	startSample : 開始サンプル
//	nSample :処理をするサンプル数のアドレス
//	param	:パラメーター構造体
//
int Normalize(int *pCount,SOUNDFMT *inFmt,SOUNDFMT *outFmt,FIO *fp_r1,FIO *fp_r2,FIO *fp_r3,FIO *fp_r4,FIO *fp_r5,FIO *fp_r6,SSIZE *startSample,SSIZE nSample,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	SSIZE i,j,outSample;
	SSIZE outputSize;
	DWORD chMask;
	BYTE *header;
	BYTE *bwf_chunk;
	BYTE *buf1,*buf2,*buf3,*buf4,*buf5,*buf6;
	FIO *p_fp1,*p_fp2,*p_fp3,*p_fp4,*p_fp5,*p_fp6;
	long header_size;
	fio_size write_size,ws,data_size,file_size;
	fio_size data_start,data_end,data_offset;
	BROADCAST_EXT *bext;
	BYTE *chunk_data;
	long  chunk_size;
	int retCode,retCode1,retCode2,retCode3,retCode4,retCode5,retCode6;
	int nChunk;
	int bwf_enable;
	long bwf_size;
	int div_flag;
	double percent,per;
	char ppp[50];
	char sss[600],sss2[600];
	int ccc;
	
	PRINT_FN_LOG(param->debug_upconv,"Normalize:Start");
	do {
		div_flag = 0;
		header = NULL;
		buf1 = NULL;
		buf2 = NULL;
		buf3 = NULL;
		buf4 = NULL;
		buf5 = NULL;
		buf6 = NULL;
		p_fp1 = p_fp2 = p_fp3 = p_fp4 = p_fp5 = p_fp6 = NULL;

		retCode1 = retCode2 = retCode3 = retCode4 = retCode5 = retCode6 = 0;
		retCode = STATUS_SUCCESS;
		// ヘッダ処理
		if (p->raw == 0) {
			header = malloc(1 * 1024 * 1024);
			if (header == NULL) {
				retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				break;
			}
			sprintf(sss,"param->r2w_ch:%d,nSample:%lld",param->r2w_ch,nSample);
			PRINT_LOG(sss);
			if (p->mc_split == 0) {
				data_size = (param->r2w_outSampleR * (p->w / 8)) * param->r2w_ch;
				data_size *= 10;	// 10秒
			} else {
				data_size = (param->r2w_outSampleR * (p->w / 8)) * 1;
				data_size *= 10;	// 10秒
			}
			sprintf(sss,"data_size:%lld",data_size);
			PRINT_LOG(sss);
			buf1 = malloc(data_size);
			if (buf1 == NULL) {
				retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				break;
			}
			if (param->r2w_ch >= 2) {
				buf2 = malloc(data_size);
				if (buf2 == NULL) {
					retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
					break;
				}
			}
			if (param->r2w_ch >= 3) {
				buf3 = malloc(data_size);
				if (buf3 == NULL) {
					retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
					break;
				}
			}
			if (param->r2w_ch >= 4) {
				buf4 = malloc(data_size);
				if (buf4 == NULL) {
					retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
					break;
				}
			}
			if (param->r2w_ch >= 5) {
				buf5 = malloc(data_size);
				if (buf5 == NULL) {
					retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
					break;
				}
			}
			if (param->r2w_ch >= 6) {
				buf6 = malloc(data_size);
				if (buf6 == NULL) {
					retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
					break;
				}
			}
			if (p->rf64 == 0) {
				retCode = PLG_MakeHeaderWAV(inFmt,outFmt,header,1 * 1024 * 1024,&header_size);
				if (retCode != STATUS_SUCCESS) {
					PRINT_LOG("error:PLG_MakeHeaderWAV");
					break;
				}
				if (param->r2w_ch >= 3 && p->mc_split == 0) {
					chMask	= 0;
					chMask |= SPEAKER_FRONT_LEFT;
					chMask |= SPEAKER_FRONT_RIGHT;
					if (fp_r3 != NULL || fp_r6 != NULL) {
						chMask |= SPEAKER_FRONT_CENTER;
					}
					if (fp_r4 != NULL && fp_r5 != NULL) {					
						chMask |= SPEAKER_BACK_LEFT;
						chMask |= SPEAKER_BACK_RIGHT;
					}
					if (fp_r6 != NULL) {
						chMask |= SPEAKER_LOW_FREQUENCY;
					}
					(*(DWORD *)(&header[40])) = chMask;
				}
			} else {
				retCode = PLG_MakeHeaderRF64(inFmt,outFmt,header,1 * 1024 * 1024,&header_size);
				if (retCode != STATUS_SUCCESS) {
					PRINT_LOG("error:PLG_MakeHeaderRF64");
					break;
				}
			}
			if (p->mc_split == 0) {
				ws = fio_write(header,1,header_size,param->fp_w[0]);
				if (param->fp_w[0]->error || ws != header_size) {
					retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				fio_get_datasize(param->fp_w[0],&data_start);
			} else {
				for (ccc = 0;ccc < param->r2w_ch;ccc++) {
					ws = fio_write(header,1,header_size,param->fp_w[ccc]);
					if (param->fp_w[ccc]->error || ws != header_size) {
						retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
						break;
					}
					fio_get_datasize(param->fp_w[ccc],&data_start);
				}
				if (retCode != STATUS_SUCCESS) {
					break;
				}
			}
		}
		p_fp1 = fp_r1;
		p_fp2 = fp_r2;

		if (param->r2w_ch == 3) {
			if (p->chC == 1 || p->LRC_process == 1 || p->realCh == 3) {
				p_fp3 = fp_r3;
			} else if (p->chLFE == 1) {
				p_fp3 = fp_r6;
			}
		}
		if (param->r2w_ch == 4) {
			if (p->chS == 1) {
				p_fp3 = fp_r4;
				p_fp4 = fp_r5;
			}
			if (p->realCh == 4) {
				p_fp3 = fp_r3;
				p_fp4 = fp_r4;
			}
			if (p->chC == 1 && p->chLFE == 1) {
				p_fp3 = fp_r3;
				p_fp4 = fp_r6;
			}
		}
		if (param->r2w_ch == 5) {
			if ((p->chC == 1 && p->chS == 1) || p->realCh == 5) {
				p_fp3 = fp_r3;
				p_fp4 = fp_r4;
				p_fp5 = fp_r5;
			}
			if (p->chLFE == 1 && p->chS == 1) {
				p_fp3 = fp_r6;
				p_fp4 = fp_r4;
				p_fp5 = fp_r5;
			}
		}
		if (param->r2w_ch == 6) {
			if (p->realCh == 6) {
				p_fp3 = fp_r3;
				p_fp4 = fp_r4;
				p_fp5 = fp_r5;
				p_fp6 = fp_r6;
			} else {
				p_fp3 = fp_r3;
				p_fp4 = fp_r6;
				p_fp5 = fp_r4;
				p_fp6 = fp_r5;
			}
		}

		per = -1;
		outputSize = 0;
		sprintf(sss,"param->r2w_inSampleR:%ld,param->r2w->outSampleR:%ld",param->r2w_inSampleR,param->r2w_outSampleR);
		PRINT_LOG(sss);
		sprintf(sss,"nSample:%ld",nSample);
		PRINT_LOG(sss);
		for (i = *startSample;i < nSample;i += param->r2w_outSampleR * 10) {
			percent = ((double)i / nSample);
			percent *= 100;
			percent = (int)percent;
			if (percent != per) {
//				Sleep(1);
				if (p->simple_mode == 0) {
					fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
					fflush(stdout);
				} else {
					if (p->simple_mode_done_percent_div > 0) {
						fprintf(stdout,"%d%%\n",(int)((percent / p->simple_mode_done_percent_div) + p->simple_mode_done_percent));
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;

			// 10秒ずつ処理する
			outSample = param->r2w_outSampleR * 10;
			if (i + outSample > nSample) {
				outSample = nSample - i;
			}
			if (buf1) memset(buf1,0,data_size);
			if (buf2) memset(buf2,0,data_size);
			if (buf3) memset(buf3,0,data_size);
			if (buf4) memset(buf4,0,data_size);
			if (buf5) memset(buf5,0,data_size);
			if (buf6) memset(buf6,0,data_size);

			// データ順
			// Left,Right,Center,LFE,Back Left,Back Right

			PRINT_LOG("before:Normalize_Mx");
#if 1
			#pragma omp parallel
			{
				#pragma omp sections
				{
					#pragma omp section
					{
						retCode1 = Normalize_Mx(param->r2w_ch,0,p->w,p_fp1,outSample,buf1,param,p);
					}
					#pragma omp section
					{
						if (param->r2w_ch >= 2) {
							retCode2 = Normalize_Mx(param->r2w_ch,1,p->w,p_fp2,outSample,buf2,param,p);
						}
					}
					#pragma omp section
					{
						if (param->r2w_ch >= 3) {
							retCode3 = Normalize_Mx(param->r2w_ch,2,p->w,p_fp3,outSample,buf3,param,p);
						}
					}
					#pragma omp section
					{
						if (param->r2w_ch >= 4) {
							retCode4 = Normalize_Mx(param->r2w_ch,3,p->w,p_fp4,outSample,buf4,param,p);
						}
					}
					#pragma omp section
					{
						if (param->r2w_ch >= 5) {
							retCode5 = Normalize_Mx(param->r2w_ch,4,p->w,p_fp5,outSample,buf5,param,p);
						}
					}
					#pragma omp section
					{
						if (param->r2w_ch >= 6) {
							retCode6 = Normalize_Mx(param->r2w_ch,5,p->w,p_fp6,outSample,buf6,param,p);
						}
					}
				}
			}
#else
			retCode1 = Normalize_Mx(param->r2w_ch,0,p->w,p_fp1,outSample,buf1,param,p);
			if (param->r2w_ch >= 2) {
				retCode2 = Normalize_Mx(param->r2w_ch,1,p->w,p_fp2,outSample,buf2,param,p);
			}
			if (param->r2w_ch >= 3) {
				retCode3 = Normalize_Mx(param->r2w_ch,2,p->w,p_fp3,outSample,buf3,param,p);
			}
			if (param->r2w_ch >= 4) {
				retCode4 = Normalize_Mx(param->r2w_ch,3,p->w,p_fp4,outSample,buf4,param,p);
			}
			if (param->r2w_ch >= 5) {
				retCode5 = Normalize_Mx(param->r2w_ch,4,p->w,p_fp5,outSample,buf5,param,p);
			}
			if (param->r2w_ch >= 6) {
				retCode6 = Normalize_Mx(param->r2w_ch,5,p->w,p_fp6,outSample,buf6,param,p);
			}
#endif
			PRINT_LOG("after:Normalize_Mx");
			retCode = retCode1 | retCode2 | retCode3 | retCode4 | retCode5 | retCode6;
			if (retCode != STATUS_SUCCESS) {
				PRINT_FN_LOG(param->debug_upconv,"Error:Normalize_Mx");
				param->errLine = __LINE__;
				break;
			}
			if (p->mc_split == 0) {
				if (param->r2w_ch == 2) {
					#pragma omp parallel for
					for (j = 0;j < data_size;j++) {
						buf1[j] = buf1[j] | buf2[j];
					}
				} else if (param->r2w_ch == 3) {
					#pragma omp parallel for
					for (j = 0;j < data_size;j++) {
						buf1[j] = buf1[j] | buf2[j] | buf3[j];
					}
				} else if (param->r2w_ch == 4) {
					#pragma omp parallel for
					for (j = 0;j < data_size;j++) {
						buf1[j] = buf1[j] | buf2[j] | buf3[j] | buf4[j];
					}
				} else if (param->r2w_ch == 5) {
					#pragma omp parallel for
					for (j = 0;j < data_size;j++) {
						buf1[j] = buf1[j] | buf2[j] | buf3[j] | buf4[j] | buf5[j];
					}
				} else if (param->r2w_ch == 6) {
					#pragma omp parallel for
					for (j = 0;j < data_size;j++) {
						buf1[j] = buf1[j] | buf2[j] | buf3[j] | buf4[j] | buf5[j] | buf6[j];
					}
				}
				if (outSample == param->r2w_outSampleR * 10) {
					ws = fio_write(buf1,1,data_size,param->fp_w[0]);
					if (param->fp_w[0]->error || ws != data_size) {
						PRINT_FN_LOG(param->debug_upconv,"fio_write:error");
						retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
						break;
					}
				} else {
					write_size = (outSample * (p->w / 8)) * param->r2w_ch;
					ws = fio_write(buf1,1,write_size,param->fp_w[0]);
					if (param->fp_w[0]->error || ws != write_size) {
						PRINT_FN_LOG(param->debug_upconv,"fio_write:error");
						retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
						break;
					}
				}
			} else {
				if (outSample == param->r2w_outSampleR * 10) {
					BYTE *pb[6];
					pb[0] = buf1;
					pb[1] = buf2;
					pb[2] = buf3;
					pb[3] = buf4;
					pb[4] = buf5;
					pb[5] = buf6;
					for (ccc = 0;ccc < param->r2w_ch;ccc++) {
						ws = fio_write(pb[ccc],1,data_size,param->fp_w[ccc]);
						if (param->fp_w[ccc]->error || ws != data_size) {
							PRINT_FN_LOG(param->debug_upconv,"fio_write:error");
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
					}
					if (retCode != STATUS_SUCCESS) {
						PRINT_FN_LOG(param->debug_upconv,"fio_write:error");
						break;
					}
				} else {
					BYTE *pb[6];
					pb[0] = buf1;
					pb[1] = buf2;
					pb[2] = buf3;
					pb[3] = buf4;
					pb[4] = buf5;
					pb[5] = buf6;
					for (ccc = 0;ccc < param->r2w_ch;ccc++) {
						write_size = (outSample * (p->w / 8)) * 1;
						ws = fio_write(pb[ccc],1,write_size,param->fp_w[ccc]);
						if (param->fp_w[ccc]->error || ws != write_size) {
							PRINT_FN_LOG(param->debug_upconv,"fio_write:error");
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
					}
					if (retCode != STATUS_SUCCESS) {
						PRINT_FN_LOG(param->debug_upconv,"fio_write:error");
						break;
					}
				}
			}
			outputSize += outSample;
			if (p->rf64 == 0) {
				PRINT_FN_LOG(param->debug_upconv,"outputSize += outSample");
				if (p->split_size == 300 || p->split_size == 600) {
					if (outputSize >= p->split_size * param->r2w_outSampleR && (nSample - i) >= param->r2w_outSampleR * 20) {
						// 指定した秒で分割する
						*startSample = i + outSample;
						div_flag = 1;
						break;
					}
				} else {
					fio_get_datasize(param->fp_w[0],&file_size);
					if (p->wav2g == 0) {
						if (file_size > (fio_size)1 * 1024 * 1024 * 1024 && outSample == param->r2w_outSampleR * 10 && (nSample - i) >= param->r2w_outSampleR * 20) {
							// データサイズが大きいので分割する(rf64以外)
							*startSample = i + outSample;
							div_flag = 1;
							break;
						}
					} else {
						if (file_size > (fio_size)2 * 1024 * 1024 * 1024 && outSample == param->r2w_outSampleR * 10 && (nSample - i) >= param->r2w_outSampleR * 20) {
							// データサイズが大きいので分割する(rf64以外)
							*startSample = i + outSample;
							div_flag = 1;
							break;
						}
					}
				}
			}
		}

		if (retCode != STATUS_SUCCESS) {
			PRINT_FN_LOG(param->debug_upconv,"error");
			break;
		}
		fio_get_datasize(param->fp_w[0],&data_end);
		data_offset = fio_tell(param->fp_w[0]);
		if (data_offset & 1) {
			ws = fio_write(" ",1,1,param->fp_w[0]);
			if (ws != 1) {
				PRINT_FN_LOG(param->debug_upconv,"fio_write:error");
				retCode = STATUS_FILE_WRITE_ERR;
				break;
			}
		}

		PRINT_FN_LOG(param->debug_upconv,"next PLG_GetExtraChunk");

		bwf_enable = 0;
		if (p->bwf) {
			bwf_enable = 1;
		}
		if (p->raw == 0) {
			chunk_data = NULL;
			for (nChunk = 1;;) {
				PLG_GetExtraChunk(param->fromfile,nChunk,(char **)&chunk_data,&chunk_size);
				if (chunk_size == 0) {
					break;
				}
				if (chunk_size > 0 && chunk_data[0] == 'b' && chunk_data[1] == 'e' && chunk_data[2] == 'x' && chunk_data[3] == 't') {
					bwf_enable = 0;
					if (p->bwf) {
						int alloc_flag = 0;
						bwf_size = chunk_data[7];bwf_size <<= 8;
						bwf_size |= chunk_data[6];bwf_size <<= 8;
						bwf_size |= chunk_data[5];bwf_size <<= 8;
						bwf_size |= chunk_data[4];
						if (bwf_size <= sizeof (BROADCAST_EXT) + 128) {
							alloc_flag = 1;
						} else {
							bext = (BROADCAST_EXT *)&chunk_data[8];
							if (chunk_size < sizeof (BROADCAST_EXT) + strlen(bext->codingHistory) + 128) {
								alloc_flag = 1;
							}
						}
						if (alloc_flag == 1) {
							bwf_size += 256;
							bext = (BROADCAST_EXT *)malloc(bwf_size);
						}
						if (bext) {
							if (alloc_flag == 1) {
								memset(bext,0,bwf_size);
								memcpy(bext,chunk_data + 8,chunk_size);
							}
							UpdateBext(bext,inFmt,outFmt,param,bwf_size,p);
							if (p->mc_split == 0) {
								ws = fio_write("bext",1,4,param->fp_w[0]);
								if (param->fp_w[0]->error || ws != 4) {
									retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
									break;
								}
								ws = fio_write(&bwf_size,1,4,param->fp_w[0]);
								if (param->fp_w[0]->error || ws != 4) {
									retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
									break;
								}
								ws = fio_write(bext,1,bwf_size,param->fp_w[0]);
								if (param->fp_w[0]->error || ws != bwf_size) {
									retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
									break;
								}
							} else {
								for (ccc = 0;ccc < param->r2w_ch;ccc++) {
									ws = fio_write("bext",1,4,param->fp_w[ccc]);
									if (param->fp_w[ccc]->error || ws != 4) {
										retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
										break;
									}
									ws = fio_write(&bwf_size,1,4,param->fp_w[ccc]);
									if (param->fp_w[ccc]->error || ws != 4) {
										retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
										break;
									}
									ws = fio_write(bext,1,bwf_size,param->fp_w[ccc]);
									if (param->fp_w[ccc]->error || ws != bwf_size) {
										retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
										break;
									}
								}
								if (retCode != STATUS_SUCCESS) {
									break;
								}
							}
							if (alloc_flag) {
								free(bext);
							}
						}
					}
				} else {
					if (chunk_size > 0) {
						if (p->mc_split == 0) {
							int sz;
							sz = chunk_size;
							if (sz & 1) sz++;
							ws = fio_write(chunk_data,1,sz,param->fp_w[0]);
						} else {
							int sz;
							sz = chunk_size;
							if (sz & 1) sz++;
							for (ccc = 0;ccc < param->r2w_ch;ccc++) {
								ws = fio_write(chunk_data,1,sz,param->fp_w[ccc]);
								if (param->fp_w[ccc]->error || ws != sz) {
									retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
									break;
								}
							}
							if (retCode != STATUS_SUCCESS) {
								break;
							}
						}
					}
				}
				nChunk++;
			}
			if (bwf_enable) {
				bwf_chunk = malloc(512 + 256);
				if (bwf_chunk) {
					memset(bwf_chunk,0,512 + 256);
					bwf_chunk[0] = 'b';
					bwf_chunk[1] = 'e';
					bwf_chunk[2] = 'x';
					bwf_chunk[3] = 't';
					bwf_chunk[4] = (BYTE)((512 + 256) - 8);
					bwf_chunk[5] = (BYTE)(((512 + 256) - 8) >> 8);
					bwf_chunk[6] = (BYTE)(((512 + 256) - 8) >> 16);
					bwf_chunk[7] = (BYTE)(((512 + 256) - 8) >> 24);
					bext = (BROADCAST_EXT *)&bwf_chunk[8];
					UpdateBext(bext,inFmt,outFmt,param,512 + 256,p);
					if (p->mc_split == 0) {
						ws = fio_write(bwf_chunk,1,512 + 256,param->fp_w[0]);
						if (param->fp_w[0]->error || ws != 512 + 256) {
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
					} else {
						for (ccc = 0;ccc < param->r2w_ch;ccc++) {
							ws = fio_write(bwf_chunk,1,512 + 256,param->fp_w[ccc]);
							if (param->fp_w[ccc]->error || ws != 512 + 256) {
								retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
								break;
							}
						}
						if (retCode != STATUS_SUCCESS) {
							break;
						}
					}
					free(bwf_chunk);
					bwf_chunk = NULL;
				}
			}
			if (p->bwf && (div_flag == 1 || *pCount > 1)) {
				// bwf の link チャンク
				char *link,*wk_str;
				long link_size;
				link = (char *)malloc(strlen(param->tofile) + 128);
				wk_str = malloc(_MAX_PATH + 128);
				if (link != NULL && wk_str != NULL) {
					link[0] = '\0';
					strcat(link,link_start);
					if (*pCount == 1) {
						sprintf(wk_str,link_file,"actual",(int)*pCount,param->tofile);
					} else {
						sprintf(wk_str,link_file,"other",(int)*pCount,param->tofile);
					}
					strcat(link,wk_str);
					strcat(link,link_end);
					if (p->mc_split == 0) {
						ws = fio_write("link",1,4,param->fp_w[0]);
						if (param->fp_w[0]->error || ws != 4) {
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
					} else {
						for (ccc = 0;ccc < param->r2w_ch;ccc++) {
							ws = fio_write("link",1,4,param->fp_w[ccc]);
							if (param->fp_w[ccc]->error || ws != 4) {
								retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
								break;
							}
						}
						if (retCode != STATUS_SUCCESS) {
							break;
						}
					}
					link_size = strlen(link);
					if (p->mc_split == 0) {
						ws = fio_write(&link_size,1,4,param->fp_w[0]);
						if (param->fp_w[0]->error || ws != 4) {
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
					} else {
						for (ccc = 0;ccc < param->r2w_ch;ccc++) {
							ws = fio_write(&link_size,1,4,param->fp_w[ccc]);
							if (param->fp_w[ccc]->error || ws != 4) {
								retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
								break;
							}
						}
					}
					if (p->mc_split == 0) {
						ws = fio_write(link,1,link_size,param->fp_w[0]);
						if (param->fp_w[0]->error || ws != link_size) {
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
					} else {
						for (ccc = 0;ccc < param->r2w_ch;ccc++) {
							ws = fio_write(link,1,link_size,param->fp_w[ccc]);
							if (param->fp_w[ccc]->error || ws != link_size) {
								retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
								break;
							}
						}
						if (retCode != STATUS_SUCCESS) {
							break;
						}
					}
					free(link);
					free(wk_str);
					if (link_size & 0x01) {
						// padding
						if (p->mc_split == 0) {
							ws = fio_write("",1,1,param->fp_w[0]);
							if (param->fp_w[0]->error || ws != 1) {
								retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
								break;
							}
						} else {
							for (ccc = 0;ccc < param->r2w_ch;ccc++) {
								ws = fio_write("",1,1,param->fp_w[ccc]);
								if (param->fp_w[ccc]->error || ws != 1) {
									retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
									break;
								}
							}
						}
					}
				}
			}
			// デフォルトパラメーターの保存
			if (p->dp_save == 1) {
				FILE *dp_file;
				unsigned char dp_header[8];
				char *dp_chunk_data;
				char *dp_work;
				struct stat st;
				struct tm *tm;
				unsigned long dp_chunk_size = 8192;
				dp_chunk_data = (char *)malloc(dp_chunk_size);
				dp_work       = (char *)malloc(dp_chunk_size);
				if (dp_chunk_data != NULL && dp_work != NULL) {
					memset(dp_chunk_data,0x00,dp_chunk_size);
					dp_file = fopen(param->arg_dp,"r");
					if (dp_file != NULL) {
						strcat(dp_chunk_data,"Version:");
						strcat(dp_chunk_data,STR_VER);
						strcat(dp_chunk_data,",dp_mtime:");
						stat(param->arg_dp,&st);
						tm = localtime(&st.st_mtime);
						sprintf(dp_work,"%04d/%02d/%02d %02d:%02d",tm->tm_year + 1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min);
						strcat(dp_chunk_data,dp_work);
						strcat(dp_chunk_data,"\n");
						while (fgets(dp_work,4000,dp_file) != NULL) {
							strcat(dp_chunk_data,dp_work);
						}
						fclose(dp_file);
					}
					if (strlen(dp_chunk_data) > 0) {
						if (strlen(dp_chunk_data) & 0x01) {
							strcat(dp_chunk_data," ");
						}
						dp_chunk_size = strlen(dp_chunk_data);
						dp_header[0] = 'U';
						dp_header[1] = 'P';
						dp_header[2] = 'C';
						dp_header[3] = 'O';
						dp_header[4] = (unsigned char)dp_chunk_size;
						dp_header[5] = (unsigned char)(dp_chunk_size >> 8);
						dp_header[6] = (unsigned char)(dp_chunk_size >> 16);
						dp_header[7] = (unsigned char)(dp_chunk_size >> 24);
						ws = fio_write(dp_header,8,1,param->fp_w[0]);
						if (param->fp_w[0]->error || ws != 1) {
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
						ws = fio_write(dp_chunk_data,dp_chunk_size,1,param->fp_w[0]);
						if (param->fp_w[0]->error || ws != 1) {
							retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
							break;
						}
					}
				}
			}
			if (p->mc_split == 0) {
				fio_get_datasize(param->fp_w[0],&data_size);
				fio_flush(param->fp_w[0]);
				if (param->fp_w[0]->error) {
					retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				fio_rewind(param->fp_w[0]);
				if (p->rf64 == 0) {
					retCode = PLG_UpdateHeaderWAV(outFmt,data_size,data_end - data_start,header,header_size);
				} else {
					retCode = PLG_UpdateHeaderRF64(outFmt,data_size,data_end - data_start,header,header_size);
				}
				if (retCode != STATUS_SUCCESS) {
					param->errLine = __LINE__;
					break;
				}
				ws = fio_write(header,1,header_size,param->fp_w[0]);
				if (param->fp_w[0]->error || ws != header_size) {
					retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
			} else {
				fio_get_datasize(param->fp_w[0],&data_size);
				for (ccc = 0;ccc < param->r2w_ch;ccc++) {
					fio_flush(param->fp_w[ccc]);
					if (param->fp_w[ccc]->error) {
						retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
						break;
					}
					fio_rewind(param->fp_w[ccc]);
					if (p->rf64 == 0) {
						retCode = PLG_UpdateHeaderWAV(outFmt,data_size,data_end - data_start,header,header_size);
					} else {
						retCode = PLG_UpdateHeaderRF64(outFmt,data_size,data_end - data_start,header,header_size);
					}
					ws = fio_write(header,1,header_size,param->fp_w[ccc]);
					if (param->fp_w[ccc]->error || ws != header_size) {
						retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
						break;
					}
				}
			}
		}
	} while (0);
	if (div_flag == 0) {
		*pCount = 0;
	}
	fprintf(stdout,"%d%%\n",100);
	fflush(stdout);

	PRINT_FN_LOG(param->debug_upconv,"Normalize:End");

	return retCode;
}
//---------------------------------------------------------------------------
// Function   : Normalize_Mx
// Description: ノーマライズ処理
// ---
//	nCh		:チャンネル数
//	ch		:チャンネルの何番目を処理するかの指定
//	bit 	:入力データのビット数
//	fp_r	:音声データのFIO構造体
//	nSample :処理をするサンプル数
//	buffer	:出力先のバッファ
//
int Normalize_Mx(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	int retCode;

	PRINT_FN_LOG(param->debug_upconv,"Normalize_Mx:Start");
	// 音声データ出力
	switch (p->output_dither_mode) {
		case 0:
			retCode = Normalize_M0(nCh,ch,bit,fp_r,nSample,buffer,param,p);
			break;
		case 1:
			retCode = Normalize_M1(nCh,ch,bit,fp_r,nSample,buffer,param,p);
			break;
		case 2:
			retCode = Normalize_M2(nCh,ch,bit,fp_r,nSample,buffer,param,p);
			break;
		case 3:
			retCode = Normalize_M3(nCh,ch,bit,fp_r,nSample,buffer,param,p);
			break;
		default:
			retCode = -1;
			break;
	}
	PRINT_FN_LOG(param->debug_upconv,"Normalize_Mx:End");
	return retCode;
}
//---------------------------------------------------------------------------
// Function   : Normalize
// Description: ノーマライズ処理(カット)
// ---
//	nCh		:チャンネル数
//	ch		:チャンネルの何番目を処理するかの指定
//	bit 	:入力データのビット数
//	fp_r	:音声データのFIO構造体
//	nSample :処理をするサンプル数
//	buffer	:出力先のバッファ
//
int Normalize_M0(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	SSIZE s;
	SSIZE i,w_off;
	int next;
	double nx;
	short *s_os;
	long  *l_os;
	float *f_os;
	double *d_os;
	fio_size rs;
	PRINT_FN_LOG(param->debug_upconv,"Normalize_M0:Start");

	if (bit == 16) {
		next = 2 * nCh;
		w_off = 2 * ch;
		if (p->mc_split) {
			next = 2;
			w_off = 0;
		}
	} else if (bit == 24) {
		next = 3 * nCh;
		w_off = 3 * ch;
		if (p->mc_split) {
			next = 3;
			w_off = 0;
		}
	} else if (bit == 32) {
		next = 4 * nCh;
		w_off = 4 * ch;
		if (p->mc_split) {
			next = 4;
			w_off = 0;
		}
	} else if (bit == 64) {
		next = 8 * nCh;
		w_off = 8 * ch;
		if (p->mc_split) {
			next = 8;
			w_off = 0;
		}
	}

	if (ch == 0) {
		nx = param->nx1;
	} else if (ch == 1) {
		nx = param->nx2;
	} else if (ch == 2) {
		nx = param->nx3;
	} else if (ch == 3) {
		nx = param->nx4;
	} else if (ch == 4) {
		nx = param->nx5;
	} else if (ch == 5) {
		nx = param->nx6;
	}
	nx = 1.0;

	for (i = 0;i < nSample;i++) {
		rs = fio_read(&s,sizeof (SSIZE),1,fp_r);
		if (fp_r->error || rs != 1) {
			param->errLine = __LINE__;
			return STATUS_FILE_READ_ERR;
		}
		s = CLIP_NX(s,nx);
#if 0
		if (IS_CLIPPED(s)) {
			if (s > 0) {
				s--;
			} else {
				s++;
			}
		}
#endif
		if (bit == 16) {
			s = CLIP_MAX(s);
			s >>= (64 - 16);
			s_os = (short *)&buffer[w_off];
			*s_os = (short)s;
			w_off += next;
		} else if (bit == 24) {
			s = CLIP_MAX(s);
			s >>= (64 - 24);
			buffer[w_off + 0] = (unsigned char)s;
			buffer[w_off + 1] = (unsigned char)(s >> 8);
			buffer[w_off + 2] = (unsigned char)(s >> 16);
			w_off += next;
		} else if (bit == 32 && p->bitwidth_int == 1) {
			s = CLIP_MAX(s);
			s >>= (64 - 32);
			l_os = (long *)&buffer[w_off];
			*l_os = (long)s;
			w_off += next;
		} else if (bit == 32 && p->bitwidth_int == 0) {
			f_os = (float *)&buffer[w_off];
			s = CLIP_MAX(s);
			*f_os = (float)s / 0x7FFFFFFFFFFFFFFF;
			w_off += next;
		} else if (bit == 64 && p->bitwidth_int == 0) {
			d_os = (double *)&buffer[w_off];
			s = CLIP_MAX(s);
			*d_os = (double)s / 0x7FFFFFFFFFFFFFFF;
			w_off += next;
		}
	}
	PRINT_FN_LOG(param->debug_upconv,"Normalize_M0:End");
	return STATUS_SUCCESS;
}
//---------------------------------------------------------------------------
// Function   : Normalize
// Description: ノーマライズ処理(ディザ)
// ---
//	nCh		:チャンネル数
//	ch		:チャンネルの何番目を処理するかの指定
//	bit 	:入力データのビット数
//	fp_r	:音声データのFIO構造体
//	nSample :処理をするサンプル数
//	buffer	:出力先のバッファ
//
int Normalize_M1(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	SSIZE s;
	SSIZE i;
	int ignore_s;
	double nx;
	double noise;
	short *s_os;
	long  *l_os;
	float *f_os;
	double *d_os;
	fio_size rs;
	int next;
	int w_off;
	PRINT_FN_LOG(param->debug_upconv,"Normalize_M1:Start");
	
	if (bit == 16) {
		next = 2 * nCh;
		w_off = 2 * ch;
		if (p->mc_split) {
			next = 2;
			w_off = 0;
		}
	} else if (bit == 24) {
		next = 3 * nCh;
		w_off = 3 * ch;
		if (p->mc_split) {
			next = 3;
			w_off = 0;
		}
	} else if (bit == 32) {
		next = 4 * nCh;
		w_off = 4 * ch;
		if (p->mc_split) {
			next = 4;
			w_off = 0;
		}
	} else if (bit == 64) {
		next = 8 * nCh;
		w_off = 8 * ch;
		if (p->mc_split) {
			next = 8;
			w_off = 0;
		}
	}

	if (ch == 0) {
		nx = param->nx1;
	} else if (ch == 1) {
		nx = param->nx2;
	} else if (ch == 2) {
		nx = param->nx3;
	} else if (ch == 3) {
		nx = param->nx4;
	} else if (ch == 4) {
		nx = param->nx5;
	} else if (ch == 5) {
		nx = param->nx6;
	}
	nx = 1.0;

	for (i = 0;i < nSample;i++) {
		rs = fio_read(&s,sizeof (SSIZE),1,fp_r);
		if (fp_r->error || rs != 1) {
			param->errLine = __LINE__;
			return STATUS_FILE_READ_ERR;
		}

		s = CLIP_NX(s,nx);
#if 0
		if (IS_CLIPPED(s)) {
			if (s > 0) {
				s--;
				s--;
				s--;
			} else {
				s++;
				s++;
				s++;
			}
		}
#endif
		noise = normalNoise();
		if (bit == 16) {
			if (p->output_dither_option > 0) {
				noise *= (0x10000000000 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-16));
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX(s);

			s >>= (64 - 16);
			s_os = (short *)&buffer[w_off];
			*s_os = (short)s;
			w_off += next;

		} else if (bit == 24) {
			if (p->output_dither_option > 0) {
				noise *= (0x100000000 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-24));
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX(s);

			s >>= (64 - 24);
			buffer[w_off + 0] = (unsigned char)s;
			buffer[w_off + 1] = (unsigned char)(s >> 8);
			buffer[w_off + 2] = (unsigned char)(s >> 16);
			w_off += next;
		} else if (bit == 32 &&  p->bitwidth_int == 1) {
			if (p->output_dither_option > 0) {
				noise *= (0x10000000000 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-32));
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX(s);

			s >>= (64 - 32);
			l_os = (long *)&buffer[w_off];
			*l_os = (long)s;
			w_off += next;


		} else if (bit == 32 && p->bitwidth_int == 0) {
			if (p->output_dither_option > 0) {
				noise *= (0x1000000 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-32));
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX(s);

			f_os = (float *)&buffer[w_off];
			*f_os = (float)s / 0x7FFFFFFFFFFFFFFF;
			w_off += next;
		} else if (bit == 64 && p->bitwidth_int == 0) {
			if (p->output_dither_option > 0) {
				noise *= (0x100 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-48));
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX(s);

			d_os = (double *)&buffer[w_off];
			*d_os = (double)s / 0x7FFFFFFFFFFFFFFF;
			w_off += next;
		}
	}
	PRINT_FN_LOG(param->debug_upconv,"Normalize_M1:End");
	return STATUS_SUCCESS;
}
//---------------------------------------------------------------------------
// Function   : Normalize
// Description: ノーマライズ処理(誤差累積)
// ---
//	nCh		:チャンネル数
//	ch		:チャンネルの何番目を処理するかの指定
//	bit 	:入力データのビット数
//	fp_r	:音声データのFIO構造体
//	nSample :処理をするサンプル数
//	buffer	:出力先のバッファ
//
int Normalize_M2(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	SSIZE i;
	SSIZE s;
	SSIZE old_s;
	SSIZE sum,sum_2nd,sum_3rd,val;
	int ignore_s;
	double nx;
	double noise;
	short *s_os;
	long  *l_os;
	float *f_os;
	double *d_os;
	fio_size rs;
	int next;
	int w_off;
	char sss[256];

	PRINT_FN_LOG(param->debug_upconv,"Normalize_M2:Start");
	if (bit == 16) {
		next = 2 * nCh;
		w_off = 2 * ch;
		if (p->mc_split) {
			next = 2;
			w_off = 0;
		}
	} else if (bit == 24) {
		next = 3 * nCh;
		w_off = 3 * ch;
		if (p->mc_split) {
			next = 3;
			w_off = 0;
		}
	} else if (bit == 32) {
		next = 4 * nCh;
		w_off = 4 * ch;
		if (p->mc_split) {
			next = 4;
			w_off = 0;
		}
	} else if (bit == 64) {
		next = 8 * nCh;
		w_off = 8 * ch;
		if (p->mc_split) {
			next = 8;
			w_off = 0;
		}
	}

	if (ch == 0) {
		nx = param->nx1;
		sum = param->sum1;
		sum_2nd = param->sum1_2nd;
		sum_3rd = param->sum1_3rd;
		old_s	= param->old_s1;
	} else if (ch == 1) {
		nx = param->nx2;
		sum = param->sum2;
		sum_2nd = param->sum2_2nd;
		sum_3rd = param->sum2_3rd;
		old_s	= param->old_s2;
	} else if (ch == 2) {
		nx = param->nx3;
		sum = param->sum3;
		sum_2nd = param->sum3_2nd;
		sum_3rd = param->sum3_3rd;
		old_s	= param->old_s3;
	} else if (ch == 3) {
		nx = param->nx4;
		sum = param->sum4;
		sum_2nd = param->sum4_2nd;
		sum_3rd = param->sum4_3rd;
		old_s	= param->old_s4;
	} else if (ch == 4) {
		nx = param->nx5;
		sum = param->sum5;
		sum_2nd = param->sum5_2nd;
		sum_3rd = param->sum5_3rd;
		old_s	= param->old_s5;
	} else if (ch == 5) {
		nx = param->nx6;
		sum = param->sum6;
		sum_2nd = param->sum6_2nd;
		sum_3rd = param->sum6_3rd;
		old_s	= param->old_s6;
	}
	nx = 1.0;

	for (i = 0;i < nSample;i++) {
		rs = fio_read(&s,sizeof (SSIZE),1,fp_r);
		if (fp_r->error || rs != 1) {
			param->errLine = __LINE__;
			return STATUS_FILE_READ_ERR;
		}

		s = CLIP_NX(s,nx);
#if 0
		if (IS_CLIPPED(s)) {
			if (s > 0) {
				s--;
				s--;
				s--;
			} else {
				s++;
				s++;
				s++;
			}
		}
#endif
		noise = normalNoise();
		if (bit == 16) {
			// 四捨五入

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-16));
			
			s -= old_s;
			sum += s;

			s = sum >> ((64-8) - 16);
			s <<= ((64-8) - 16);
			s -= old_s;
			sum_2nd += s;
			s = sum_2nd >> ((64-8) - 16);

			s <<= ((64-8) - 16);
			s -= old_s;
			sum_3rd += s;
			s = sum_3rd >> ((64-8) - 16);

			old_s = s << ((64-8) - 16);

			if (p->output_dither_option > 0) {
				noise *= (1 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX_N(s,16);
			s_os = (short *)&buffer[w_off];
			*s_os = (short)s;
			w_off += next;
		} else if (bit == 24) {

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-24));
			s -= old_s;
			sum += s;

			s = sum >> ((64-8) - 24);
			s <<= ((64-8) - 24);
			s -= old_s;
			sum_2nd += s;
			s = sum_2nd >> ((64-8) - 24);

			s <<= ((64-8) - 24);
			s -= old_s;
			sum_3rd += s;
			s = sum_3rd >> ((64-8) - 24);

			old_s = s << ((64-8) - 24);

			if (p->output_dither_option > 0) {
				noise *= (1 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX_N(s,24);
			buffer[w_off + 0] = (unsigned char)s;
			buffer[w_off + 1] = (unsigned char)(s >> 8);
			buffer[w_off + 2] = (unsigned char)(s >> 16);
			w_off += next;
		} else if (bit == 32 && p->bitwidth_int == 1) {
			// 四捨五入

			s = CLIP_ADD(s,ROUND_NBIT((64-8)-32));
			
			s -= old_s;
			sum += s;

			s = sum >> ((64-8) - 32);
			s <<= ((64-8) - 32);
			s -= old_s;
			sum_2nd += s;
			s = sum_2nd >> ((64-8) - 32);

			s <<= ((64-8) - 32);
			s -= old_s;
			sum_3rd += s;
			s = sum_3rd >> ((64-8) - 32);

			old_s = s << ((64-8) - 32);

			if (p->output_dither_option > 0) {
				noise *= (1 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX_N(s,32);
			l_os = (long *)&buffer[w_off];
			*l_os = (long)s;
			w_off += next;

		} else if (bit == 32 && p->bitwidth_int == 0) {
			s = CLIP_ADD(s,ROUND_NBIT((64-8)-32));

			// 誤差の累積
			sum += s % 0x1000000;

			// 誤差の判定
			val = sum / 0x1000000;
			if (ignore_s == 0 && val > 0) {
				s += 0x1000000;
				sum %= 0x1000000;
			} else if (ignore_s == 0 && val < 0) {
				s -= 0x1000000;
				sum %= 0x1000000;
			}

			// 2次 誤差の累積
			sum_2nd += s % 0x1000000;
			// 誤差の判定
			val = sum_2nd / 0x1000000;
			if (val > 0) {
				s += 0x1000000;
				sum_2nd %= 0x1000000;
			} else if (val < 0) {
				s -= 0x1000000;
				sum_2nd %= 0x1000000;
			}

			// 32bit 化する
			s >>= ((64-8) - 32);
			if (p->output_dither_option > 0) {
				noise *= (1 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}

			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX_N(s,32);
			s <<= (64 - 32);
			f_os = (float *)&buffer[w_off];
			*f_os = (float)s / 0x7FFFFFFFFFFFFFFF;
			w_off += next;
		} else if (bit == 64 && p->bitwidth_int == 0) {
			// 48bit
			s = CLIP_ADD(s,ROUND_NBIT((64-8)-48));

			// 誤差の累積
			sum += s % 0x10000;

			// 誤差の判定
			val = sum / 0x10000;
			if (val > 0) {
				s += 0x10000;
				sum %= 0x10000;
			} else if (val < 0) {
				s -= 0x10000;
				sum %= 0x10000;
			}

			// 2次 誤差の累積
			sum_2nd += s % 0x10000;
			// 誤差の判定
			val = sum_2nd / 0x10000;
			if (val > 0) {
				s += 0x10000;
				sum_2nd %= 0x10000;
			} else if (val < 0) {
				s -= 0x10000;
				sum_2nd %= 0x10000;
			}

			// 64bit 化する
			s >>= ((64-8) - 48);
			if (p->output_dither_option > 0) {
				noise *= (1 << (p->output_dither_option - 1));
			} else {
				noise = 0;
			}
			s = CLIP_ADD(s,(SSIZE)noise);
			s = CLIP_MAX_N(s,48);
			s <<= (64 - 48);
			d_os = (double *)&buffer[w_off];
			*d_os = (double)s / 0x7FFFFFFFFFFFFFFF;
			w_off += next;
		}
	}
	if (ch == 0) {
		param->sum1 = sum;
		param->sum1_2nd = sum_2nd;
		param->sum1_3rd = sum_3rd;
		param->old_s1 = old_s;
	} else if (ch == 1) {
		param->sum2 = sum;
		param->sum2_2nd = sum_2nd;
		param->sum2_3rd = sum_3rd;
		param->old_s2 = old_s;
	} else if (ch == 2) {
		param->sum3 = sum;
		param->sum3_2nd = sum_2nd;
		param->sum3_3rd = sum_3rd;
		param->old_s3 = old_s;
	} else if (ch == 3) {
		param->sum4 = sum;
		param->sum4_2nd = sum_2nd;
		param->sum4_3rd = sum_3rd;
		param->old_s4 = old_s;
	} else if (ch == 4) {
		param->sum5 = sum;
		param->sum5_2nd = sum_2nd;
		param->sum5_3rd = sum_3rd;
		param->old_s5 = old_s;
	} else if (ch == 5) {
		param->sum6 = sum;
		param->sum6_2nd = sum_2nd;
		param->sum6_3rd = sum_3rd;
		param->old_s6 = old_s;
	}
	PRINT_FN_LOG(param->debug_upconv,"Normalize_M2:End");

	return STATUS_SUCCESS;
}
//---------------------------------------------------------------------------
// Function   : Normalize
// Description: ノーマライズ処理(誤差拡散)
// ---
//	nCh		:チャンネル数
//	ch		:チャンネルの何番目を処理するかの指定
//	bit 	:入力データのビット数
//	fp_r	:音声データのFIO構造体
//	nSample :処理をするサンプル数
//	buffer	:出力先のバッファ
//
int Normalize_M3(int nCh,int ch,int bit,FIO *fp_r,SSIZE nSample,BYTE *buffer,PARAM_INFO_TO_WAV *param,UPCONV_PARAM *p)
{
	SSIZE s;
	SSIZE ss[3];
	SSIZE sd[3];
	SSIZE val;
	SSIZE *buf;
	SSIZE pastSample;
	SSIZE i,remain;
	int ignore_s;
	double nx;
	double noise;
	short *s_os;
	long  *l_os;
	float *f_os;
	double *d_os;
	fio_size rs;
	int next;
	int w_off;

	PRINT_FN_LOG(param->debug_upconv,"Normalize_M3:Start");
	if (bit == 16) {
		next = 2 * nCh;
		w_off = 2 * ch;
		if (p->mc_split) {
			next = 2;
			w_off = 0;
		}
	} else if (bit == 24) {
		next = 3 * nCh;
		w_off = 3 * ch;
		if (p->mc_split) {
			next = 3;
			w_off = 0;
		}
	} else if (bit == 32) {
		next = 4 * nCh;
		w_off = 4 * ch;
		if (p->mc_split) {
			next = 4;
			w_off = 0;
		}
	} else if (bit == 64) {
		next = 8 * nCh;
		w_off = 8 * ch;
		if (p->mc_split) {
			next = 8;
			w_off = 0;
		}
	}

	if (ch == 0) {
		nx = param->nx1;
	} else if (ch == 1) {
		nx = param->nx2;
	} else if (ch == 2) {
		nx = param->nx3;
	} else if (ch == 3) {
		nx = param->nx4;
	} else if (ch == 4) {
		nx = param->nx5;
	} else if (ch == 5) {
		nx = param->nx6;
	}
	nx = 1.0;

	ss[0] = ss[1] = ss[2] = 0;
	sd[0] = sd[1] = sd[2] = 0;

	buf = malloc(nSample * sizeof (SSIZE));
	if (buf == NULL) {
		param->errLine = __LINE__;
		return STATUS_MEM_ALLOC_ERR;
	}
	
	pastSample = 0;
	do {
		remain = fio_read(buf,sizeof (SSIZE),nSample,fp_r);
		for (i = 1;i < remain + 1 && pastSample < nSample;i++,pastSample++) {
			noise = normalNoise();

			ss[0] = buf[i-1];
			if (i < remain) {
				ss[1] = buf[i];
			} else {
				ss[1] = ss[0];
			}
			if (i + 1 < remain) {
				ss[2] = buf[i+1];
			} else {
				ss[2] = ss[0];
			}

			ss[0] = CLIP_NX(ss[0],nx);
			ss[1] = CLIP_NX(ss[1],nx);
			ss[2] = CLIP_NX(ss[2],nx);

			if (bit == 16) {
				ss[1] += sd[1];
				s = ss[1];
				val = ss[1] % 0x10000000000;
				ss[1] = ss[1] - val;
				if (val > 0) {
					if (val > 0x7FFFFFFFFF) {
						ss[1] += 0x10000000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFFFFFF) {
						ss[1] -= 0x10000000000;
					}
				}
				sd[1]  = ss[1] - s;
				sd[0] += sd[1] / 2;
				sd[2] += sd[1] / 2;
				sd[1] = 0;
				ss[0] += sd[0];
				s = ss[0];
				val = ss[0] % 0x10000000000;
				ss[0] = ss[0] - val;
				if (val > 0) {
					if (val > 0x7FFFFFFFFF) {
						ss[0] += 0x10000000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFFFFFF) {
						ss[0] -= 0x10000000000;
					}
				}
				sd[0] = ss[0] - s;
				sd[1] = sd[0] / 2;

				sd[0] = sd[1];
				sd[1] = sd[2];
				sd[2] = 0;
				s = ss[0];
				s >>= ((64-8) - 16);

				if (p->output_dither_option > 0) {
					noise *= (1 << (p->output_dither_option - 1));
				} else {
					noise = 0;
				}
				s = CLIP_ADD(s,(SSIZE)noise);
				s = CLIP_MAX_N(s,16);
				s_os = (short *)&buffer[w_off];
				*s_os = (short)s;
				w_off += next;
			} else if (bit == 24) {
				ss[1] += sd[1];
				s = ss[1];
				val = ss[1] % 0x100000000;
				ss[1] = ss[1] - val;
				if (val > 0) {
					if (val > 0x7FFFFFFF) {
						ss[1] += 0x100000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFFFF) {
						ss[1] -= 0x100000000;
					}
				}
				sd[1]  = ss[1] - s;
				sd[0] += sd[1] / 2;
				sd[2] += sd[1] / 2;
				sd[1] = 0;
				ss[0] += sd[0];
				s = ss[0];
				val = ss[0] % 0x100000000;
				ss[0] = ss[0] - val;
				if (val > 0) {
					if (val > 0x7FFFFFFF) {
						ss[0] += 0x100000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFFFF) {
						ss[0] -= 0x100000000;
					}
				}
				sd[0] = ss[0] - s;
				sd[1] = sd[0] / 2;

				sd[0] = sd[1];
				sd[1] = sd[2];
				sd[2] = 0;
				s = ss[0];
				s >>= ((64-8) - 24);

				if (p->output_dither_option > 0) {
					noise *= (1 << (p->output_dither_option - 1));
				} else {
					noise = 0;
				}
				s = CLIP_ADD(s,(SSIZE)noise);
				s = CLIP_MAX_N(s,24);
				buffer[w_off + 0] = (unsigned char)s;
				buffer[w_off + 1] = (unsigned char)(s >> 8);
				buffer[w_off + 2] = (unsigned char)(s >> 16);
				w_off += next;
			} else if (bit == 32 && p->bitwidth_int == 1) {
				ss[1] += sd[1];
				s = ss[1];
				val = ss[1] % 0x1000000;
				ss[1] = ss[1] - val;
				if (val > 0) {
					if (val > 0x7FFFFF) {
						ss[1] += 0x1000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFF) {
						ss[1] -= 0x1000000;
					}
				}
				sd[1]  = ss[1] - s;
				sd[0] += sd[1] / 2;
				sd[2] += sd[1] / 2;
				sd[1] = 0;
				ss[0] += sd[0];
				s = ss[0];
				val = ss[0] % 0x1000000;
				ss[0] = ss[0] - val;
				if (val > 0) {
					if (val > 0x7FFFFF) {
						ss[0] += 0x1000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFF) {
						ss[0] -= 0x1000000;
					}
				}
				sd[0] = ss[0] - s;
				sd[1] = sd[0] / 2;

				sd[0] = sd[1];
				sd[1] = sd[2];
				sd[2] = 0;
				s = ss[0];
				s >>= ((64-8) - 32);

				if (p->output_dither_option > 0) {
					noise *= (1 << (p->output_dither_option - 1));
				} else {
					noise = 0;
				}
				s = CLIP_ADD(s,(SSIZE)noise);
				s = CLIP_MAX_N(s,32);
				l_os = (long *)&buffer[w_off];
				*l_os = (long)s;
				w_off += next;

			} else if (bit == 32 && p->bitwidth_int == 0) {
				ss[1] += sd[1];
				s = ss[1];
				val = ss[1] % 0x1000000;
				ss[1] = ss[1] - val;
				if (val > 0) {
					if (val > 0x7FFFFF) {
						ss[1] += 0x1000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFF) {
						ss[1] -= 0x1000000;
					}
				}
				sd[1]  = ss[1] - s;
				sd[0] += sd[1] / 2;
				sd[2] += sd[1] / 2;
				sd[1] = 0;
				ss[0] += sd[0];
				s = ss[0];
				val = ss[0] % 0x1000000;
				ss[0] = ss[0] - val;
				if (val > 0) {
					if (val > 0x7FFFFF) {
						ss[0] += 0x1000000;
					}
				} else {
					if ((val * -1) > 0x7FFFFF) {
						ss[0] -= 0x1000000;
					}
				}
				sd[0] = ss[0] - s;
				sd[1] = sd[0] / 2;

				sd[0] = sd[1];
				sd[1] = sd[2];
				sd[2] = 0;
				s = ss[0];
				s >>= ((64-8) - 32);

				if (p->output_dither_option > 0) {
					noise *= (1 << (p->output_dither_option - 1));
				} else {
					noise = 0;
				}
				s = CLIP_ADD(s,(SSIZE)noise);
				s = CLIP_MAX_N(s,32);
				s <<= (64 - 32);
				f_os = (float *)&buffer[w_off];
				*f_os = (float)s / 0x7FFFFFFFFFFFFFFF;
				w_off += next;
			} else if (bit == 64 && p->bitwidth_int == 0) {
				ss[1] += sd[1];
				s = ss[1];
				val = ss[1] % 0x100;
				ss[1] = ss[1] - val;
				if (val > 0) {
					if (val > 0x7F) {
						ss[1] += 0x100;
					}
				} else {
					if ((val * -1) > 0x7F) {
						ss[1] -= 0x100;
					}
				}
				sd[1]  = ss[1] - s;
				sd[0] += sd[1] / 2;
				sd[2] += sd[1] / 2;
				sd[1] = 0;
				ss[0] += sd[0];
				s = ss[0];
				val = ss[0] % 0x100;
				ss[0] = ss[0] - val;
				if (val > 0) {
					if (val > 0x7F) {
						ss[0] += 0x100;
					}
				} else {
					if ((val * -1) > 0x7F) {
						ss[0] -= 0x100;
					}
				}
				sd[0] = ss[0] - s;
				sd[1] = sd[0] / 2;

				sd[0] = sd[1];
				sd[1] = sd[2];
				sd[2] = 0;
				s = ss[0];
				s >>= ((64-8) - 48);

				if (p->output_dither_option > 0) {
					noise *= (1 << (p->output_dither_option - 1));
				} else {
					noise = 0;
				}
				s = CLIP_ADD(s,(SSIZE)noise);
				s = CLIP_MAX_N(s,48);
				s <<= (64 - 48);
				d_os = (double *)&buffer[w_off];
				*d_os = (double)s / 0x7FFFFFFFFFFFFFFF;
				w_off += next;
			}
		}
	} while (pastSample < nSample);
	free(buf);

	PRINT_FN_LOG(param->debug_upconv,"Normalize_M3:End");

	return STATUS_SUCCESS;
}

//---------------------------------------------------------------------------
// Function   : Normalize_DSD
// Description: DSD用ノーマライズ処理
// ---
//	inFmt	:入力ファイル音声形式情報
//	outFmt	:出力ファイル音声形式情報
//	fp_r1	:音声データのFIO構造体
//	fp_r2	:音声データのFIO構造体
//	nSample :処理をするサンプル数のアドレス
//	param	:パラメーター構造体
//
int Normalize_DSD(SOUNDFMT *inFmt,SOUNDFMT *outFmt,FIO *fp_r1,FIO *fp_r2,SSIZE nSample,PARAM_INFO_TO_WAV *param)
{
	SSIZE i,j,outSample;
	DWORD chMask;
	DSF *dsf;
	DSF_FMT *dsf_fmt;
	DSF_DATA *dsf_data;
	int retCode;
	SSIZE *buf1,*buf2;
	BYTE *buf1_4k,*buf2_4k;
	FIO *fp1_w,*fp2_w;
	fio_size write_size,ws,data_size,file_size,rs;
	fio_size data_start,data_end;
	double persent,per;
	SSIZE s;
	double nx1,nx2;
	char *header;
	long hs;

	// 1bit
	SSIZE min,max;
	SSIZE s1,s2;
	SSIZE ss1,ss2;
	SSIZE sss1,sss2;
	SSIZE q1,q2;
	SSIZE d1,d2;

	// 8bit,5bit,3bit
	SSIZE sm1[3],sm2[3];
	SSIZE ssm1[3],ssm2[3];
	SSIZE qm1[3],qm2[3];
	SSIZE dm1[3],dm2[3];
	
	PRINT_FN_LOG(param->debug_upconv,"Normalize_DSD:Start");
	min = pow(2,58) * -1;
	max = pow(2,58);
	s1 = s2 = 0;
	ss1 = ss2 = 0;
	sss1 = sss2 = 0;
	q1 = q2 = 0;

	sm1[0] = sm1[1] = sm1[2] = 0;
	sm2[0] = sm2[1] = sm2[3] = 0;
	ssm1[0] = ssm1[1] = ssm1[2] = 0;
	ssm2[0] = ssm2[1] = ssm2[2] = 0;
	qm1[0] = qm1[1] = qm1[2] = 0;
	qm2[0] = qm2[1] = qm2[2] = 0;
	dm1[0] = dm1[1] = dm1[2] = 0;
	dm2[0] = dm2[1] = dm2[2] = 0;

	nx1 = param->nx1;
	nx2 = param->nx2;

	do {
		retCode = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		header = malloc(1024);
		buf1 = malloc((4096 * 8) * sizeof (SSIZE));
		buf2 = malloc((4096 * 8) * sizeof (SSIZE));
		buf1_4k = malloc(4096);
		buf2_4k = malloc(4096);
		if (header == NULL || buf1 == NULL || buf2 == NULL || buf1_4k == NULL || buf2_4k == NULL) {
			break;
		}
		memset(header,0,1024);
		
		retCode = PLG_MakeHeaderDSD(inFmt,outFmt,header,1024,&hs);
		if (retCode != STATUS_SUCCESS) {
			param->errLine = __LINE__;
			break;
		}
		ws = fio_write(header,1,hs,param->fp_w[0]);
		if (param->fp_w[0]->error || ws != hs) {
			retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			break;
		}
		fio_get_datasize(param->fp_w[0],&data_start);

		retCode = STATUS_SUCCESS;
		per = -1;
		data_size = (4096 * 8) * sizeof (SSIZE);
		for (i = 0;i < nSample;i += 4096 * 8) {
			persent = ((double)i / nSample);
			persent *= 100;
			if (persent != per) {
//				Sleep(1);
				fprintf(stdout,"%d%%\n",(int)persent);
				fflush(stdout);
			}
			per = persent;
			// 1秒ずつ処理する
			outSample = 4096 * 8;
			if (i + outSample > nSample) {
				outSample = nSample - i;
			}
			if (buf1) memset(buf1,0,data_size);
			if (buf2) memset(buf2,0,data_size);
			if (param->r2w_ch == 1) {
				rs = fio_read(buf1,sizeof (SSIZE),outSample,fp_r1);
				if (fp_r1->error || rs <= 0) {
					param->errLine = __LINE__;
					return STATUS_FILE_READ_ERR;
				}
				// delta sigma modulation (1bit)
				for (j = 0;j < outSample;j++) {
					s = CLIP_NX(buf1[j],nx1);
					s1  += s - q1;
					ss1 += s1 - q1 * 2;
					
					q1 = (ss1 >= 0) ? max : min;
					buf1[j] = q1;
				}
				nbit2onebit(buf1,buf1_4k,4096 * 8);
				ws = fio_write(buf1_4k,1,4096,param->fp_w[0]);
				if (param->fp_w[0]->error || ws != 4096) {
					retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
			} else if (param->r2w_ch == 2) {
				rs = fio_read(buf1,sizeof (SSIZE),outSample,fp_r1);
				if (fp_r1->error || rs <= 0) {
					param->errLine = __LINE__;
					return STATUS_FILE_READ_ERR;
				}
				// delta sigma modulation (1bit)
				for (j = 0;j < outSample;j++) {
					s = CLIP_NX(buf1[j],nx1);
					s1  += s - q1;
					ss1 += s1 - q1 * 2;
					
					q1 = (ss1 >= 0) ? max : min;
					buf1[j] = q1;
				}
				// 2値化
				nbit2onebit(buf1,buf1_4k,4096 * 8);
				ws = fio_write(buf1_4k,1,4096,param->fp_w[0]);
				if (param->fp_w[0]->error || ws != 4096) {
					retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

				rs = fio_read(buf2,sizeof (SSIZE),outSample,fp_r2);
				if (fp_r2->error || rs <= 0) {
					param->errLine = __LINE__;
					return STATUS_FILE_READ_ERR;
				}
				// delta sigma modulation (1bit)
				for (j = 0;j < outSample;j++) {
					s = CLIP_NX(buf2[j],nx2);
					s2  += s - q2;
					ss2 += s2 - q2 * 2;
					
					q2 = (ss2 >= 0) ? max : min;
					buf2[j] = q2;
				}
				nbit2onebit(buf2,buf2_4k,4096 * 8);
				ws = fio_write(buf2_4k,1,4096,param->fp_w[0]);
				if (param->fp_w[0]->error || ws != 4096) {
					retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
			}
		}
		if (retCode != STATUS_SUCCESS) break;

		fio_flush(param->fp_w[0]);
		if (param->fp_w[0]->error) {
			retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			break;
		}
		fio_get_datasize(param->fp_w[0],&data_end);
		fio_rewind(param->fp_w[0]);

		retCode = PLG_UpdateHeaderDSD(outFmt,data_end,data_end - data_start,nSample,header,hs);
		ws = fio_write(header,1,hs,param->fp_w[0]);
		if (param->fp_w[0]->error || ws != hs) {
			retCode = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			break;
		}
		retCode = STATUS_SUCCESS;
	} while (0);

	if (header) free(header);
	if (buf1) free(buf1);
	if (buf2) free(buf2);
	if (buf1_4k) free(buf1_4k);
	if (buf2_4k) free(buf2_4k);

	PRINT_FN_LOG(param->debug_upconv,"Normalize_DSD:End");
	return retCode;
}
//---------------------------------------------------------------------------
// Function   : nbit2onebit
// Description: 1bit化
//
void nbit2onebit(SSIZE *i_buf,BYTE *o_buf,int size)
{
	int i;
	int bit,ptr_byte;

	PRINT_FN_LOG("wav2raw","nbit2onebit:Start");

	if (size > 0) {
		ptr_byte = 0;
		bit  = 0x01;
		for (i = 0;i < size;i++) {
			if ((i_buf[i]) > 0) {
				o_buf[ptr_byte] |= bit;
			} else if ((i_buf[i]) < 0) {
				o_buf[ptr_byte] &= (bit ^ 0xFF);
			}
			bit <<= 1;
			if (bit > 0x80) {
				bit = 0x01;
				ptr_byte++;
			}
		}
	}
	PRINT_FN_LOG("wav2raw","nbit2onebit:End");
}

//---------------------------------------------------------------------------
// Function   : UpdateBext
// Description: bext チャンク更新
// ---
//	bext	:bext構造体へのアドレス
//	inFmt	:入力ファイル音声形式情報
//	inFmt	:入力ファイル音声形式情報
//	outFmt	:出力ファイル音声形式情報
//	param	:パラメーター構造体
//	bwf_size:bextのバイト数
//
int UpdateBext(BROADCAST_EXT *bext,SOUNDFMT *inFmt,SOUNDFMT *outFmt,PARAM_INFO_TO_WAV *param,long bwf_size,UPCONV_PARAM *p)
{
	SSIZE timeRef;
	double d_timeRef;
	char strbuf[512];
	strbuf[0] = '\0';

	PRINT_FN_LOG(param->debug_upconv,"UpdateBext:Start");

	// version
	bext->version = 1;

	// TimeRef
	timeRef = bext->timeReferenceHigh;timeRef <<= 32;
	timeRef |= bext->timeReferenceLow;
	if (timeRef != 0) {
		d_timeRef = (double)timeRef;
		d_timeRef /= inFmt->sample;
		d_timeRef *= outFmt->sample;
		timeRef = (SSIZE)d_timeRef;
		bext->timeReferenceHigh = (DWORD)(timeRef >> 32);
		bext->timeReferenceLow	= (DWORD)timeRef;
	}
	// Coding History
	if (strlen(bext->codingHistory) == 0) {
		sprintf(strbuf,"A=PCM,F=%d,W=%d,M=%s,T=original,\r\n",inFmt->sample,inFmt->bitsPerSample,inFmt->channel == 1 ? "mono" : "stereo");
		strcat(bext->codingHistory,strbuf);
	}
	sprintf(strbuf,"A=PCM,F=%d,W=%d,M=%s,T=Upconv0.8.x;",outFmt->sample,outFmt->bitsPerSample,outFmt->channel == 1 ? "mono" : "stereo");
	strcat(bext->codingHistory,strbuf);
	if (p->abe) {
		strcpy(strbuf,"ABE;");
		strcat(bext->codingHistory,strbuf);
	}
	sprintf(strbuf,"HFA%d",p->hfa);
	strcat(bext->codingHistory,strbuf);
	strcat(bext->codingHistory,",\r\n");

	PRINT_FN_LOG(param->debug_upconv,"UpdateBext:End");
	return 0;
}

//---------------------------------------------------------------------------
// Function   : dsf_main
// Description: 引数を処理し変換関数を呼び出す
//
//
int dsf_main(int argc, char *argv[],UPCONV_PARAM *p)
{
	PARAM_INFO_TO_RAW param;
	FILE *fp;
	char workpath[_MAX_PATH];
	char fracpath[_MAX_PATH];
	char infile[_MAX_PATH];
	char outfile[_MAX_PATH];
	char tmppath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char *p1,*p2;
	char param1[512];
	int retCode;
	long temp;

	do {
		infile[0] = '\0';
		outfile[0] = '\0';
		retCode = STATUS_SUCCESS;
		memset(&param,0,sizeof (PARAM_INFO_TO_RAW));
		
		if (argc < 3) {
			break;
		}

		strcpy(infile,argv[2]);
		strcpy(outfile,argv[3]);

		_splitpath(argv[3],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"param");
		// ファイルオープン
		fp = fopen(tmppath,"r");
		if (fp == NULL) {
			retCode = STATUS_FILE_READ_ERR;
			break;
		}

		if (fgets(workpath,_MAX_PATH,fp) == NULL) {
			retCode = STATUS_FILE_READ_ERR;
			break;
		}

		if (fgets(fracpath,_MAX_PATH,fp) == NULL) {
			retCode = STATUS_FILE_READ_ERR;
			break;
		}

		if (fgets(param1,512,fp) == NULL) {
			retCode = STATUS_FILE_READ_ERR;
			break;
		}
		p1 = strrchr(workpath,'\n');if (p1 != NULL) *p1 = '\0';
		if (strlen(workpath) >= 2 && workpath[strlen(workpath) - 1] != '\\') strcat(workpath,"\\");

		param.workpath = workpath;
		fclose(fp);
		p1 = param1;
		p2 = strchr(p1,(int)' ');

		for (;p1 != NULL;) {
			if (p2 != NULL) {
				*p2 = '\0';
			}

			if (p2 == NULL) {
				break;
			}
			p1 = p2 + 1;
			p2 = strchr(p1,(int)' ');
		}


	} while (0);

	if (param.err) {
		_splitpath(argv[3],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"err");
		fp = fopen(tmppath,"a");
		if (fp) {
			switch (param.err) {
				case STATUS_FILE_READ_ERR:
					fprintf(fp,"dsf2raw:[%04d] File read error.\n",param.errLine);
					break;
				case STATUS_FILE_WRITE_ERR:
					fprintf(fp,"dsf2raw:[%04d] File write error.\n",param.errLine);
					break;
				case STATUS_MEM_ALLOC_ERR:
					fprintf(fp,"dsf2raw:[%04d] Memory Allocation error.\n",param.errLine);
					break;
				default:
					fprintf(fp,"raw2wav:Other error.\n");
			}
			fclose(fp);
		}
	}
	return retCode;
}

//---------------------------------------------------------------------------
// Function   : dsf_encode
// Description: DSF エンコード処理
// ---
// WAV ファイルを DSF ファイルへエンコードする
//
void dsf_encode(char *in_file,char *out_file,PARAM_INFO_TO_RAW *param)
{
	// 未サポート
	param->err = 1;param->errLine = __LINE__;
}
//---------------------------------------------------------------------------
// Function   : dsf_decode
// Description: DSF デコード処理
// ---
// DSF ファイルを WAV ファイルへエンコードする
//
void dsf_decode(char *in_file,char *out_file,char *argv4,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	static char tmppath[_MAX_PATH];
	static char workdrive[_MAX_DRIVE];
	static char workdir[_MAX_DIR];
	static char workfname[_MAX_FNAME];
	static char workext[_MAX_EXT];
	static char drive[_MAX_DRIVE];
	static char dir[_MAX_DIR];
	static char fname[_MAX_FNAME];
	static char ext[_MAX_EXT];
	static char tmpfile[_MAX_PATH];
	static char work[128];
	char sss[2058];
	FIO fp_r,fp_w1,fp_w2;
	FILE *fp;
	FILE *fp_files;
	DSF dsf;
	DSF_FMT dsf_fmt;
	DSF_DATA dsf_data;
	fio_size rs;
	fio_size ws;
	fio_size file_size;
	UI64 remain;
	char dbs[100];
	SSIZE inSample,outSample;
	double dd;
	int lr_flag;
	int i,j,n;
	SSIZE avg;
	FIO_COMMON_PARAM fio_common;

	memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));
	if (p->fio_sec_size > 0) {
		// 入力のn秒分
		fio_common.sector_size = (fio_size)192000 * sizeof (SSIZE);
		fio_common.sector_n    = p->fio_sec_size;
		fio_common.upconv_n    = 1;
	} else {
		// n MB 分
		fio_common.sector_size = (fio_size)1 * 1000 * 1000;
		fio_common.sector_n    = p->fio_mb_size;
		fio_common.upconv_n    = 1;
	}

#ifdef _OPENMP
	omp_set_num_threads(p->thread);
#endif

	PRINT_FN_LOG(param->debug_upconv,"dsf_decode:start");

	do {
		memset(&fp_r,0,sizeof (FIO));
		memset(&fp_w1,0,sizeof (FIO));
		memset(&fp_w2,0,sizeof (FIO));
		fp_files = NULL;
		//fio_open(&fp_r,in_file,FIO_MODE_R);
		fio_open_with_set_param(&fp_r,in_file,FIO_MODE_R,&fio_common);
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (fp_r.error) {
			PRINT_LOG("error");
		}
		rs = fio_read(&dsf,sizeof (DSF),1,&fp_r);
		if (fp_r.error || rs != 1) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		
		if (memcmp(dsf.id,"DSD ",4)) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf.chunk_size < 28) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf.file_size < (28 + 52 + 12)) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_seek(&fp_r,dsf.chunk_size,SEEK_SET);
		rs = fio_read(&dsf_fmt,sizeof (DSF_FMT),1,&fp_r);
		if (fp_r.error || rs != 1) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		
		if (memcmp(dsf_fmt.id,"fmt ",4)) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.chunk_size < 52) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.fmt_version != 1) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.fmt_id != 0) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (!(dsf_fmt.channel_type == 1 || dsf_fmt.channel_type == 2)) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (!(dsf_fmt.channel_count == 1 || dsf_fmt.channel_count == 2)) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (!(dsf_fmt.sampling == 2822400 || dsf_fmt.sampling == 2822400*2 || dsf_fmt.sampling == 2822400*4)) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.sample_bit_count != 1) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.sample_count < 1) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.block_size != 4096) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_seek(&fp_r,dsf.chunk_size + dsf_fmt.chunk_size,SEEK_SET);
		rs = fio_read(&dsf_data,sizeof (DSF_DATA),1,&fp_r);
		if (fp_r.error || rs != 1) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}

		if (dsf_data.chunk_size <= 12) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
PRINT_LOG("DSF");
		param->w2r_channel = dsf_fmt.channel_count;
		param->w2r_data_offset = fio_tell(&fp_r);
		param->dsf_decode = 1;

		_splitpath(out_file,drive,dir,fname,ext);
		_makepath(tmpfile,drive,dir,fname,"files");
		// 出力ファイルオープン
		fp_files = fopen(tmpfile,"a");
		if (fp_files == NULL) {
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}
		// 出力ファイル(Left)
		_splitpath(out_file,drive,dir,fname,ext);
		_splitpath(param->workpath,workdrive,workdir,workfname,workext);
		if (strlen(param->workpath) >= 3) {
			strcpy(workfname,fname);
		} else {
			strcpy(workdrive,drive);
			strcpy(workdir,dir);
			strcpy(workfname,fname);
		}

		if (fp_r.error) {
			PRINT_LOG("error");
		}
		_makepath(tmpfile,workdrive,workdir,workfname,"r3");
		fprintf(fp_files,"%s\n",tmpfile);

		_makepath(tmpfile,workdrive,workdir,workfname,"r1.tmp");
		fprintf(fp_files,"%s\n",tmpfile);
		fio_common.upconv_n    = dsf_fmt.channel_count;
		//fio_open(&fp_w1,tmpfile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w1,tmpfile,FIO_MODE_W,&fio_common);
		if (fp_w1.error) {
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}
		if (param->w2r_channel == 2) {
			// 出力ファイル(Right)
			_makepath(tmpfile,workdrive,workdir,workfname,"r2.tmp");
			fprintf(fp_files,"%s\n",tmpfile);
			//fio_open(&fp_w2,tmpfile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w2,tmpfile,FIO_MODE_W,&fio_common);
			if (fp_w2.error) {
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
		}

		if (param->w2r_channel == 1) {
			fio_set_maxsize(&fp_w1,(dsf_data.chunk_size - 12) / 2);
			//fio_set_memory_limit(&fp_w1,20,p->fio);
		} else {
			fio_set_maxsize(&fp_w1,(dsf_data.chunk_size - 12) / 2);
			fio_set_maxsize(&fp_w2,(dsf_data.chunk_size - 12) / 2);
			//fio_set_memory_limit(&fp_w1,20,p->fio);
			//fio_set_memory_limit(&fp_w2,20,p->fio);
		}
		if (fp_r.error) {
			PRINT_LOG("error");
		}
		_makepath(tmpfile,workdrive,workdir,workfname,"r1");
		fprintf(fp_files,"%s\n",tmpfile);
		_makepath(tmpfile,workdrive,workdir,workfname,"r2");
		fprintf(fp_files,"%s\n",tmpfile);
		_makepath(tmpfile,workdrive,workdir,workfname,"r1.tmp");
		fprintf(fp_files,"%s\n",tmpfile);
		_makepath(tmpfile,workdrive,workdir,workfname,"r2.tmp");
		fprintf(fp_files,"%s\n",tmpfile);
		_makepath(tmpfile,workdrive,workdir,workfname,"r1.tmp2");
		fprintf(fp_files,"%s\n",tmpfile);
		_makepath(tmpfile,workdrive,workdir,workfname,"r2.tmp2");
		fprintf(fp_files,"%s\n",tmpfile);

		fclose(fp_files);

		// 4096 ごとにインターリーブされているブロックをtmpファイルへ出力する(ステレオの場合)
		if (fp_r.error) {
			PRINT_LOG("error");
		}
		sprintf(sss,"w2r_channel:%d",param->w2r_channel);
		PRINT_LOG(sss);
		deinterleave(dsf_data.chunk_size - 12,&fp_r,&fp_w1,&fp_w2,param);
		if (fp_r.error) {
			PRINT_LOG("error");
		}
		if (param->err) {
			break;
		}
		fio_close(&fp_r);
		if (param->w2r_channel == 1) {
			PRINT_LOG("w2r_channel:1");
			fio_flush(&fp_w1);
		} else {
			PRINT_LOG("w2r_channel:2");
			fio_flush(&fp_w1);
			fio_flush(&fp_w2);
		}
		// 読み込みで開く
		PRINT_LOG("fio_setmode_r");
		fio_setmode_r(&fp_w1,&fp_r,NULL);
		if (fp_w1.error) {
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_get_filesize(&fp_r,&file_size);
		sprintf(sss,"dsf r1 size:%lld",file_size);
		PRINT_LOG(sss);

		param->w2r_inSampleR  = dsf_fmt.sampling;
		param->w2r_outSampleR = 192000;
		if (dsf_fmt.sampling == 2822400) {
			param->w2r_outSampleR = 192000;
			PRINT_LOG("DSF:outSampleR 192000");
			if (p->hfc == -1) param->hfc = 16000;
		} else if (dsf_fmt.sampling == 2822400*2) {
			param->w2r_outSampleR = 192000 * 2;
			if (p->hfc == -1) param->hfc = 22000;
			PRINT_LOG("DSF:outSampleR 384000");
		} else if (dsf_fmt.sampling == 2822400*4) {
			param->w2r_outSampleR = 192000 * 4;
			if (p->hfc == -1) param->hfc = 28000;
			PRINT_LOG("DSF:outSampleR 768000");
		}

		fio_common.sector_size = param->w2r_outSampleR * sizeof (SSIZE);
		fio_common.upconv_n    = 1;
		// 出力ファイル(Left)
		_makepath(tmpfile,workdrive,workdir,workfname,"r1");

		//fio_open(&fp_w1,tmpfile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w1,tmpfile,FIO_MODE_W,&fio_common);

		if (fp_w1.error) {
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}

		if (p->conv_test == 0) {
			remain = dsf_fmt.sample_count;
			remain *= param->w2r_outSampleR;
			remain /= dsf_fmt.sampling;
			remain *= sizeof (SSIZE);
		} else {
			remain = dsf_fmt.sample_count;
			if ((dsf_fmt.sample_count / dsf_fmt.sampling) > p->conv_test) {
				remain = dsf_fmt.sampling * p->conv_test;
			}
			remain *= param->w2r_outSampleR;
			remain /= dsf_fmt.sampling;
			remain *= sizeof (SSIZE);
		}

		// 出力ファイルサイズの最大値を指定
		fio_set_maxsize(&fp_w1,remain);
		//fio_set_memory_limit(&fp_w1,20,p->fio);
		remain = dsf_fmt.sample_count;
		if (p->conv_test > 0) {
			if ((dsf_fmt.sample_count / dsf_fmt.sampling) > p->conv_test) {
				remain = dsf_fmt.sampling * p->conv_test;
			}
		}
		param->n_sample = remain;
		remain *= param->w2r_outSampleR;
		remain /= dsf_fmt.sampling;

		// Left
		fftFilter(0,param->n_sample,remain,&fp_r,&fp_w1,param,p);
		if (param->err) {
			break;
		}
		fio_close(&fp_r);
		fio_close(&fp_w1);
		if (param->w2r_channel == 2) {
			// 読み込みで開く
			fio_setmode_r(&fp_w2,&fp_r,NULL);
			if (fp_w2.error) {
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			fio_get_filesize(&fp_r,&file_size);
			sprintf(sss,"dsf r2 size:%lld",file_size);
			PRINT_LOG(sss);

			// 出力ファイル(Right)
			_makepath(tmpfile,workdrive,workdir,workfname,"r2");
			//fio_open(&fp_w2,tmpfile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w2,tmpfile,FIO_MODE_W,&fio_common);
			if (fp_w2.error) {
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}

			if (p->conv_test == 0) {
				remain = dsf_fmt.sample_count;
				remain *= param->w2r_outSampleR;
				remain /= dsf_fmt.sampling;
				remain *= sizeof (SSIZE);
			} else {
				remain = dsf_fmt.sample_count;
				if ((dsf_fmt.sample_count / dsf_fmt.sampling) > p->conv_test) {
					remain = dsf_fmt.sampling * p->conv_test;
				}
				remain *= p->outSampleR;
				remain /= dsf_fmt.sampling;
				remain *= sizeof (SSIZE);
			}

			// 出力ファイルサイズの最大値を指定
			fio_set_maxsize(&fp_w2,remain);
			//fio_set_memory_limit(&fp_w2,20,p->fio);
			remain = dsf_fmt.sample_count;
			if (p->conv_test) {
				if ((dsf_fmt.sample_count / dsf_fmt.sampling) > p->conv_test) {
					remain = dsf_fmt.sampling * p->conv_test;
				}
			}
			param->n_sample = remain;
			remain *= param->w2r_outSampleR;
			remain /= dsf_fmt.sampling;

			// Right
			fftFilter(1,dsf_fmt.sample_count,remain,&fp_r,&fp_w2,param,p);
			if (param->err) {
				break;
			}
			fio_close(&fp_r);
			fio_close(&fp_w2);
		}

		sprintf(work," -is:%d -iw:%d -ch:%d",param->w2r_outSampleR,24,param->w2r_channel);
		strcat(argv4,work);
		p->inSampleR = param->w2r_outSampleR;
		p->iw = 24;
		p->ch = param->w2r_channel;
		p->realCh = p->ch;
		param->dsf_decode = 0;

		PRINT_LOG("dsf arg4");
		PRINT_LOG(argv4);
		PRINT_FN_LOG(param->debug_upconv,"dsf_decode:end");

	} while (0);
}
//---------------------------------------------------------------------------
// Function   : deinterleave
// Description: 4096ごとに記録されているチャンネルごとの1ブロックを結合する
// ---
//	fp_r		:入力ファイル用構造体
//	fp_w1		:出力ファイル用構造体(Left)
//	fp_w2		:出力ファイル用構造体(Right)
//	param		:変換パラメータ
//
void deinterleave(UI64 inByte,FIO *fp_r,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param)
{
	unsigned char *bit_buffer;
	fio_size rs,ws;
	int lr;
	char sss[100];

	PRINT_FN_LOG(param->debug_upconv,"deinterleave:Start");

	bit_buffer	 = (unsigned char *)malloc(4096);
	if (bit_buffer == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fio_seek(fp_r,param->w2r_data_offset,SEEK_SET);
	if (fp_r->error) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		return;
	}

	lr = 0;
	while (inByte > 0) {
		rs = fio_read(bit_buffer,1,4096,fp_r);
		if (fp_r->error || rs != 4096) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			break;
		}
		if (lr == 0) {
			ws = fio_write(bit_buffer,1,4096,fp_w1);
			if (fp_w1->error || ws != rs) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
		} else {
			ws = fio_write(bit_buffer,1,4096,fp_w2);
			if (fp_w2->error || ws != rs) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
		}
		lr++;
		if (lr == 2) {
			lr = 0;
		}
		if (param->w2r_channel == 1) {
			lr = 0;
		}
		inByte -= 4096;
	}
	free(bit_buffer);

	sprintf(sss,"param->err:%d,line:%d",param->err,param->errLine);
	PRINT_LOG(sss);

	PRINT_FN_LOG(param->debug_upconv,"deinterleave:End");
}
//---------------------------------------------------------------------------
// Function   : fftFilter
// Description: FFT によるフィルタ処理
// ---
//	lr			:Left/Right フラグ
//	inSample	:入力データのサンプル数(ch毎)
//	outSample	:出力データのサンプル数(ch毎)
//	fp_r		:入力ファイル用構造体
//	fp_w		:出力ファイル用構造体
//	param		:変換パラメータ
//
static void fftFilter(int lr,SSIZE inSample,SSIZE outSample,FIO *fp_r,FIO *fp_w,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	SSIZE *mem0,*mem1;
	SSIZE *mem2,*mem3,*mem4;
	SSIZE *mem5,*mem6,*mem7;
	SSIZE *mem8;
	char *inPtr;
	long wkMemSize;
	long inSampleR,outSampleR;
	long fftSizeIn,fftSizeOut,i,j,n;
	long cutOff;
	long hfc;
	long wr;
	long wkSampleR;
	long pwCnt;
	int f;
	double percent,per;
	double nx;
	double *pwBase,basePw;
	SSIZE *pIn[6],*pOut[6];
	SSIZE startInSample,inSampLen,outSampLen,nSample;
	UI64 outRemain;
	fftw_complex *fftw_io[7];
	fftw_plan fftw_p[7],fftw_ip[7];
	SSIZE membyte;
	SSIZE delay;
	SSIZE sigma;
	SSIZE a,b;
	SSIZE min,max;
	char s[50];
	SSIZE maxLv,maxLv2;
	SSIZE ns;

	PRINT_FN_LOG(param->debug_upconv,"fftFilter:Start");

	fftw_io[0] = NULL;
	fftw_io[1] = NULL;
	fftw_io[2] = NULL;
	fftw_io[3] = NULL;
	fftw_io[4] = NULL;
	fftw_io[5] = NULL;
	fftw_io[6] = NULL;

	fio_rewind(fp_r);

	if (param->dsf_decode == 0) {
		inSampleR = p->inSampleR;
		outSampleR = p->outSampleR;
	} else {
		inSampleR  = param->w2r_inSampleR;
		outSampleR = param->w2r_outSampleR;
	}
	fftSizeIn = inSampleR / 5;
	fftSizeOut = outSampleR / 5;

	wkMemSize = fftSizeIn;
	wkMemSize *= 2;

	if (lr == 0) {
		min = param->l_min;
		max = param->l_max;
		maxLv  = param->l_maxLv;
		maxLv2 = param->l_tmpLv;
		ns	   = param->l_cntLv;
	} else {
		min = param->r_min;
		max = param->r_max;
		maxLv  = param->r_maxLv;
		maxLv2 = param->r_tmpLv;
		ns	   = param->r_cntLv;
	}
	
	// 入力用(1)
	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(1)
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(1)
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(1)
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 1
	fftw_io[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2
	fftw_io[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 3
	fftw_io[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 4
	fftw_io[3] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[3] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(1)
	fftw_p[0] = fftw_plan_dft_1d(fftSizeIn,fftw_io[0],fftw_io[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[0] = fftw_plan_dft_1d(fftSizeOut,fftw_io[0],fftw_io[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(2)
	fftw_p[1] = fftw_plan_dft_1d(fftSizeIn,fftw_io[1],fftw_io[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[1] = fftw_plan_dft_1d(fftSizeOut,fftw_io[1],fftw_io[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(3)
	fftw_p[2] = fftw_plan_dft_1d(fftSizeIn,fftw_io[2],fftw_io[2],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[2] = fftw_plan_dft_1d(fftSizeOut,fftw_io[2],fftw_io[2],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	outRemain = outSample;

	delay = 0;
	per = -1;
//	for (startInSample = ((fftSizeIn + (fftSizeIn / 2)) * -1);startInSample < inSample + ((fftSizeIn * 2) + fftSizeIn / 2);startInSample += fftSizeIn * 2) {
	for (startInSample = ((fftSizeIn + (fftSizeIn / 2)) * -1);startInSample < inSample + ((fftSizeIn) + fftSizeIn / 2);startInSample += fftSizeIn) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			if (percent != per) {
				Sleep(1);
				if (param->w2r_channel == 2) {
					percent /= 2;
					if (lr == 1) {
						percent += 50;
					}
				}
				fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
				fflush(stdout);
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		// 読み込んだデータをmem1に入れる
		memset(mem1,0,wkMemSize * sizeof (SSIZE));
		onebit2nbit(startInSample,wkMemSize,mem1,fp_r,param,p);
		if (param->err) {
			break;
		}

		// 1
		memset(mem2,0,wkMemSize * sizeof (SSIZE));
		memset(mem3,0,wkMemSize * sizeof (SSIZE));
		memset(mem4,0,wkMemSize * sizeof (SSIZE));

		// DSD -> 192000 へデシメーション(1)
		param->lfc = 2;
		if (param->w2r_dsf_mode != 1) {
			param->hfc = 96000;
		} else {
			param->hfc = 24000;
		}
		param->src_flag = 1;

		// スレッド1,2,3の組とスレッド4,5,6の組でfftする
		pIn[0]	= &mem1[((fftSizeIn / 2) * 0)];
		pOut[0] = &mem2[((fftSizeOut / 2) * 0)];
		pIn[1]	= &mem1[((fftSizeIn / 2) * 1)];
		pOut[1] = &mem3[((fftSizeOut / 2) * 1)];
		pIn[2]	= &mem1[((fftSizeIn / 2) * 2)];
		pOut[2] = &mem4[((fftSizeOut / 2) * 2)];

#if 0
		#pragma omp parallel
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					// 1
					fftFilterSub(pIn[0],pOut[0],fftw_io[0],fftw_io[0],fftw_p[0],fftw_ip[0],param,0,p);
				}
				#pragma omp section
				{
					// 2
					fftFilterSub(pIn[1],pOut[1],fftw_io[1],fftw_io[1],fftw_p[1],fftw_ip[1],param,1,p);
				}
				#pragma omp section
				{
					// 3
					fftFilterSub(pIn[2],pOut[2],fftw_io[2],fftw_io[2],fftw_p[2],fftw_ip[2],param,2,p);
				}
			}
		}
#else
		#pragma omp parallel for
		for (f = 0;f < 3;f++) {
			fftFilterSub(pIn[f],pOut[f],fftw_io[f],fftw_io[f],fftw_p[f],fftw_ip[f],param,f,p);
		}

#endif
		if (param->w2r_dsf_mode != 1) {
			memset(mem1,0,fftSizeOut * 2 * sizeof (SSIZE));
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut + fftSizeOut / 2;i++) {
				mem1[i] = mem2[i] + mem3[i] + mem4[i];
			}

			for (i = fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				if (min > mem1[i]) {
					min = mem1[i];
				}
				if (max < mem1[i]) {
					max = mem1[i];
				}
				if ((mem1[i] >> 40) > 0) {
//					maxLv2 += (mem8[i] >> 40);
					maxLv2 += (mem1[i] >> 40);
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


		} else {
//			memset(mem8,0,fftSizeOut * 3 * sizeof (SSIZE));
			memset(mem1,0,fftSizeOut * 2 * sizeof (SSIZE));
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut + fftSizeOut / 2;i++) {
//				mem8[i] = mem2[i] + mem3[i] + mem4[i];
				mem1[i] = mem2[i] + mem3[i] + mem4[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				if (min > mem1[i]) {
					min = mem1[i];
				}
				if (max < mem1[i]) {
					max = mem1[i];
				}
//				if ((mem8[i] >> 40) > 0) {
				if ((mem1[i] >> 40) > 0) {
//					maxLv2 += (mem8[i] >> 40);
					maxLv2 += (mem1[i] >> 40);
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
		if (startInSample + fftSizeIn / 2 >= 0) {
			if (outRemain >= fftSizeOut) {
//				wr = fio_write(mem8 + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut * 2,fp_w);
				wr = fio_write(mem1 + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut,fp_w);
				if (wr != fftSizeOut) {
					param->err = STATUS_FILE_WRITE_ERR;
					return;
				}
			} else {
//				wr = fio_write(mem8 + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fp_w);
				wr = fio_write(mem1 + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;
					return;
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
	// 1
	fftw_destroy_plan(fftw_p[0]);
	fftw_destroy_plan(fftw_ip[0]);
	fftw_destroy_plan(fftw_p[1]);
	fftw_destroy_plan(fftw_ip[1]);
	fftw_destroy_plan(fftw_p[2]);
	fftw_destroy_plan(fftw_ip[2]);

	fftw_free(fftw_io[0]);
	fftw_free(fftw_io[1]);
	fftw_free(fftw_io[2]);
	if (lr == 0) {
		param->l_min = min;
		param->l_max = max;
		param->l_maxLv = maxLv;
		param->l_tmpLv = maxLv2;
		param->l_cntLv = ns;
	} else {
		param->r_min = min;
		param->r_max = max;
		param->r_maxLv = maxLv;
		param->r_tmpLv = maxLv2;
		param->r_cntLv = ns;
	}
	PRINT_FN_LOG(param->debug_upconv,"fftFilter:End");
}
//---------------------------------------------------------------------------
// Function   : fftFilterSub
// Description: FFT によるフィルタ処理(サブ関数)
// ---
//	param		:変換パラメータ
//
static void fftFilterSub(SSIZE *pIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,PARAM_INFO_TO_RAW *param,int id,UPCONV_PARAM *p)
{
	long inSampleR,outSampleR;
	long wkSampleR;
	long fftSizeIn,fftSizeOut,i,j,n;
	long cutOff;
	long hfc,lfc;
	double nx;
	long validIndex;
	long index1k,index15k,index19k,width1k,width_l,width_h,index_l,index_h,index_b;
	long h;
	double pw;
	double rate;
	double wid;
	if (param->dsf_decode == 0) {
		inSampleR = p->inSampleR;
		outSampleR = p->outSampleR;
	} else {
		inSampleR  = param->w2r_inSampleR;
		outSampleR = param->w2r_outSampleR;
	}
	if (param->src_flag == 2) {
		inSampleR = p->outSampleR;
	}

	fftSizeIn = inSampleR / 5;
	fftSizeOut = outSampleR / 5;

	for (i = 0;i < fftSizeOut;i++) {
		fftw_out[i][0] = 0;
		fftw_out[i][1] = 0;
	}

	// FFT 初期設定
	copyToFFTW(fftw_in,pIn,fftSizeIn);

	windowFFTW(fftw_in,fftSizeIn);
	
	// FFT
	fftw_execute(fftw_p);

	// 高域削除
	if (inSampleR <= outSampleR) {
		wkSampleR = inSampleR;
	} else {
		wkSampleR = outSampleR;
	}
	hfc = wkSampleR / 2;
	
	if (param->hfc != -1) {
		if (hfc > param->hfc) {
			hfc = param->hfc;
		}
	}
	cutOff = ((double)fftSizeOut / outSampleR) * hfc;
	cutFFTW(fftw_out,cutOff,fftSizeOut);

	if (param->lfc != -1) {
		cutOff = ((double)fftSizeOut / outSampleR) * param->lfc;
		for (i = 1;i < cutOff;i++) {
			fftw_out[i][0] = 0;
			fftw_out[i][1] = 0;
		}
	}

	// 半分のデータを復元
	for (i = 1;i < fftSizeOut / 2;i++) {
		fftw_out[fftSizeOut - i][0] = fftw_out[i][0];
		fftw_out[fftSizeOut - i][1] = fftw_out[i][1] * -1;
	}

	fftw_out[0][0] = 0;
	fftw_out[0][1] = 0;
	fftw_out[1][0] = 0;
	fftw_out[1][1] = 0;

	// invert FFT
	fftw_execute(fftw_ip);

	// 出力
	for (i = 0;i < fftSizeOut;i++) {
		pOut[i] = (SSIZE)(fftw_in[i][0] / fftSizeOut);
	}
}

//---------------------------------------------------------------------------
// Function   : LRC_fftFilter
// Description: FFT によるフィルタ処理(Left / Right / Center 分離用)
// ---
//	inSample1	:入力データのサンプル数
//	inSample2	:入力データのサンプル数
//	fio1		:入力ファイル(Left)用構造体
//	fio2		:入力ファイル(Right)用構造体
//	fio_l		:出力ファイル(Left)用構造体
//	fio_r		:出力ファイル(Right)用構造体
//	fio_c		:出力ファイル(Center)用構造体
//	param		:変換パラメータ
//
static void LRC_fftFilter(SSIZE inSample1,SSIZE inSample2,FIO *fio1,FIO *fio2,FIO *fio_l,FIO *fio_r,FIO *fio_c,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	SSIZE *l_mem0,*l_mem1,*l_mem2,*l_mem3,*l_mem4;
	SSIZE *r_mem0,*r_mem1,*r_mem2,*r_mem3,*r_mem4;
	SSIZE *mem_l,*mem_r,*mem_c;
	char *inPtr;
	char sss[100];
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
	int f;
	double percent,per;
	double nx;
	double *pwBase,basePw;
	SSIZE *l_pIn[3],*l_pOut[3];
	SSIZE *r_pIn[3],*r_pOut[3];
	SSIZE startInSample,inSampLen,outSampLen,nSample;
	SSIZE inSample;
	SSIZE outRemain;
	SSIZE tmpSample;
	SSIZE min,max;
	fftw_complex *l_fftw_in[3],*l_fftw_out[3];
	fftw_plan l_fftw_p[3],l_fftw_ip[3];

	fftw_complex *r_fftw_in[3],*r_fftw_out[3];
	fftw_plan r_fftw_p[3],r_fftw_ip[3];

	fio_rewind(fio1);
	fio_rewind(fio2);

	inSampleR  = p->inSampleR;
	outSampleR = inSampleR;

	fftSizeIn  = inSampleR * 2;
	fftSizeOut = outSampleR * 2;

	wkMemSize = fftSizeIn;
	if (wkMemSize < fftSizeOut) {
		wkMemSize = fftSizeOut;
	}
	wkMemSize *= 2;

	sprintf(sss,"hfa_LRC_CC_Phase:%lf,hfa_LRC_CC_Level:%lf",p->hfa_LRC_CC_Phase,p->hfa_LRC_CC_Level);
	PRINT_LOG(sss);

	// 入力用
	l_mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (l_mem1 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 出力用
	l_mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (l_mem2 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// ワーク用(別スレッド用)
	l_mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (l_mem3 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (l_mem4 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 入力用
	r_mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (r_mem1 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 出力用
	r_mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (r_mem2 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// ワーク用(別スレッド用)
	r_mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (r_mem3 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (r_mem4 == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 出力用
	mem_l = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem_l == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	mem_r = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem_r == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	mem_c = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem_c == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 1
	l_fftw_in[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (l_fftw_in[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_out[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (l_fftw_out[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_p[0] = fftw_plan_dft_1d(fftSizeIn,l_fftw_in[0],l_fftw_out[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (l_fftw_p[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_ip[0] = fftw_plan_dft_1d(fftSizeOut,l_fftw_out[0],l_fftw_in[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (l_fftw_ip[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 2
	l_fftw_in[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (l_fftw_in[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_out[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (l_fftw_out[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_p[1] = fftw_plan_dft_1d(fftSizeIn,l_fftw_in[1],l_fftw_out[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (l_fftw_p[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_ip[1] = fftw_plan_dft_1d(fftSizeOut,l_fftw_out[1],l_fftw_in[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (l_fftw_ip[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 3
	l_fftw_in[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (l_fftw_in[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_out[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (l_fftw_out[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_p[2] = fftw_plan_dft_1d(fftSizeIn,l_fftw_in[2],l_fftw_out[2],FFTW_FORWARD,FFTW_ESTIMATE);
	if (l_fftw_p[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	l_fftw_ip[2] = fftw_plan_dft_1d(fftSizeOut,l_fftw_out[2],l_fftw_in[2],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (l_fftw_ip[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 1
	r_fftw_in[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (r_fftw_in[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_out[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (r_fftw_out[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_p[0] = fftw_plan_dft_1d(fftSizeIn,r_fftw_in[0],r_fftw_out[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (r_fftw_p[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_ip[0] = fftw_plan_dft_1d(fftSizeOut,r_fftw_out[0],r_fftw_in[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (r_fftw_ip[0] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 2
	r_fftw_in[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (r_fftw_in[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_out[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (r_fftw_out[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_p[1] = fftw_plan_dft_1d(fftSizeIn,r_fftw_in[1],r_fftw_out[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (r_fftw_p[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_ip[1] = fftw_plan_dft_1d(fftSizeOut,r_fftw_out[1],r_fftw_in[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (r_fftw_ip[1] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 3
	r_fftw_in[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (r_fftw_in[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_out[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (r_fftw_out[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_p[2] = fftw_plan_dft_1d(fftSizeIn,r_fftw_in[2],r_fftw_out[2],FFTW_FORWARD,FFTW_ESTIMATE);
	if (r_fftw_p[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	r_fftw_ip[2] = fftw_plan_dft_1d(fftSizeOut,r_fftw_out[2],r_fftw_in[2],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (r_fftw_ip[2] == NULL) {
		param->errLine = __LINE__;
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	l_pIn[0]	= &l_mem1[((fftSizeIn / 2) * 0)];
	l_pOut[0] = &l_mem2[((fftSizeOut / 2) * 0)];
	l_pIn[1]	= &l_mem1[((fftSizeIn / 2) * 1)];
	l_pOut[1] = &l_mem3[((fftSizeOut / 2) * 1)];
	l_pIn[2]	= &l_mem1[((fftSizeIn / 2) * 2)];
	l_pOut[2] = &l_mem4[((fftSizeOut / 2) * 2)];

	r_pIn[0]	= &r_mem1[((fftSizeIn / 2) * 0)];
	r_pOut[0] = &r_mem2[((fftSizeOut / 2) * 0)];
	r_pIn[1]	= &r_mem1[((fftSizeIn / 2) * 1)];
	r_pOut[1] = &r_mem3[((fftSizeOut / 2) * 1)];
	r_pIn[2]	= &r_mem1[((fftSizeIn / 2) * 2)];
	r_pOut[2] = &r_mem4[((fftSizeOut / 2) * 2)];
	
	per = -1;
	inSample = inSample1;
	if (inSample > inSample2) {
		inSample = inSample2;
	}
	outRemain = inSample * ((double)outSampleR / inSampleR);

	for (startInSample = ((fftSizeIn * 2 + (fftSizeIn / 2)) * -1);startInSample < inSample + (fftSizeIn * 2 + fftSizeIn / 2);startInSample += fftSizeIn) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			if (percent != per) {
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) break;
				fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
				fflush(stdout);
			}
			per = percent;
		}

		memset(l_mem1,0,wkMemSize * sizeof (SSIZE));
		memset(r_mem1,0,wkMemSize * sizeof (SSIZE));

		if (startInSample >= 0 && startInSample + (fftSizeIn * 2) < inSample) {
			fio_seek(fio1,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_seek(fio2,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSizeIn * 2;
			fio_read(l_mem1,sizeof (SSIZE),nSample,fio1);
			fio_read(r_mem1,sizeof (SSIZE),nSample,fio2);
		} else {
			l_mem0 = l_mem1;
			r_mem0 = r_mem1;
			nSample = fftSizeIn * 2;
			if (startInSample >= 0) {
				fio_seek(fio1,startInSample * sizeof (SSIZE),SEEK_SET);
				fio_seek(fio2,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fio1,0,SEEK_SET);
				fio_seek(fio2,0,SEEK_SET);
				l_mem0 += (startInSample * -1);
				r_mem0 += (startInSample * -1);
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
				fio_read(l_mem0,sizeof (SSIZE),nSample,fio1);
				fio_read(r_mem0,sizeof (SSIZE),nSample,fio2);
			}
			nSample = fftSizeIn * 2;
		}

		memset(l_mem2,0,wkMemSize * sizeof (SSIZE));
		memset(l_mem3,0,wkMemSize * sizeof (SSIZE));
		memset(l_mem4,0,wkMemSize * sizeof (SSIZE));

		memset(r_mem2,0,wkMemSize * sizeof (SSIZE));
		memset(r_mem3,0,wkMemSize * sizeof (SSIZE));
		memset(r_mem4,0,wkMemSize * sizeof (SSIZE));

		memset(mem_l,0,wkMemSize * sizeof (SSIZE));
		memset(mem_r,0,wkMemSize * sizeof (SSIZE));
		memset(mem_c,0,wkMemSize * sizeof (SSIZE));

		// Center
#if 0
		#pragma omp parallel
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					// 1
					LRC_fftFilterSub(1,l_pIn[0],l_pOut[0],l_fftw_in[0],l_fftw_out[0],l_fftw_p[0],l_fftw_ip[0],r_pIn[0],r_pOut[0],r_fftw_in[0],r_fftw_out[0],r_fftw_p[0],r_fftw_ip[0],param,p,0);
				}
				#pragma omp section
				{
					// 2
					LRC_fftFilterSub(1,l_pIn[1],l_pOut[1],l_fftw_in[1],l_fftw_out[1],l_fftw_p[1],l_fftw_ip[1],r_pIn[1],r_pOut[1],r_fftw_in[1],r_fftw_out[1],r_fftw_p[1],r_fftw_ip[1],param,p,1);
				}
				#pragma omp section
				{
					// 3
					LRC_fftFilterSub(1,l_pIn[2],l_pOut[2],l_fftw_in[2],l_fftw_out[2],l_fftw_p[2],l_fftw_ip[2],r_pIn[2],r_pOut[2],r_fftw_in[2],r_fftw_out[2],r_fftw_p[2],r_fftw_ip[2],param,p,2);
				}
			}
			#pragma omp for
			for (i = 0;i < wkMemSize;i++) {
				mem_c[i] = l_mem2[i] + l_mem3[i] + l_mem4[i];
			}
		}
#else
		#pragma omp for
		for (f = 0;f < 3;f++) {
			LRC_fftFilterSub(1,l_pIn[f],l_pOut[f],l_fftw_in[f],l_fftw_out[f],l_fftw_p[f],l_fftw_ip[f],r_pIn[f],r_pOut[f],r_fftw_in[f],r_fftw_out[f],r_fftw_p[f],r_fftw_ip[f],param,p,f);
		}

		#pragma omp for
		for (i = 0;i < wkMemSize;i++) {
			mem_c[i] = l_mem2[i] + l_mem3[i] + l_mem4[i];
		}
#endif

#if 0
		// Left
		#pragma omp parallel
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					// 1
					LRC_fftFilterSub(2,l_pIn[0],l_pOut[0],l_fftw_in[0],l_fftw_out[0],l_fftw_p[0],l_fftw_ip[0],r_pIn[0],r_pOut[0],r_fftw_in[0],r_fftw_out[0],r_fftw_p[0],r_fftw_ip[0],param,p,0);
				}
				#pragma omp section
				{
					// 2
					LRC_fftFilterSub(2,l_pIn[1],l_pOut[1],l_fftw_in[1],l_fftw_out[1],l_fftw_p[1],l_fftw_ip[1],r_pIn[1],r_pOut[1],r_fftw_in[1],r_fftw_out[1],r_fftw_p[1],r_fftw_ip[1],param,p,1);
				}
				#pragma omp section
				{
					// 3
					LRC_fftFilterSub(2,l_pIn[2],l_pOut[2],l_fftw_in[2],l_fftw_out[2],l_fftw_p[2],l_fftw_ip[2],r_pIn[2],r_pOut[2],r_fftw_in[2],r_fftw_out[2],r_fftw_p[2],r_fftw_ip[2],param,p,2);
				}
			}
			#pragma omp for
			for (i = 0;i < wkMemSize;i++) {
				mem_l[i] = l_mem2[i] + l_mem3[i] + l_mem4[i];
			}
		}
#else
		// Left
		#pragma omp for
		for (f = 0;f < 3;f++) {
			LRC_fftFilterSub(2,l_pIn[f],l_pOut[f],l_fftw_in[f],l_fftw_out[f],l_fftw_p[f],l_fftw_ip[f],r_pIn[f],r_pOut[f],r_fftw_in[f],r_fftw_out[f],r_fftw_p[f],r_fftw_ip[f],param,p,f);
		}

		#pragma omp for
		for (i = 0;i < wkMemSize;i++) {
			mem_l[i] = l_mem2[i] + l_mem3[i] + l_mem4[i];
		}
#endif

#if 0
		// Right
		#pragma omp parallel
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					// 1
					LRC_fftFilterSub(3,l_pIn[0],l_pOut[0],l_fftw_in[0],l_fftw_out[0],l_fftw_p[0],l_fftw_ip[0],r_pIn[0],r_pOut[0],r_fftw_in[0],r_fftw_out[0],r_fftw_p[0],r_fftw_ip[0],param,p,0);
				}
				#pragma omp section
				{
					// 2
					LRC_fftFilterSub(3,l_pIn[1],l_pOut[1],l_fftw_in[1],l_fftw_out[1],l_fftw_p[1],l_fftw_ip[1],r_pIn[1],r_pOut[1],r_fftw_in[1],r_fftw_out[1],r_fftw_p[1],r_fftw_ip[1],param,p,1);
				}
				#pragma omp section
				{
					// 3
					LRC_fftFilterSub(3,l_pIn[2],l_pOut[2],l_fftw_in[2],l_fftw_out[2],l_fftw_p[2],l_fftw_ip[2],r_pIn[2],r_pOut[2],r_fftw_in[2],r_fftw_out[2],r_fftw_p[2],r_fftw_ip[2],param,p,2);
				}
			}
			#pragma omp for
			for (i = 0;i < wkMemSize;i++) {
				mem_r[i] = r_mem2[i] + r_mem3[i] + r_mem4[i];
			}
		}
#else
		// Right
		#pragma omp for
		for (f = 0;f < 3;f++) {
			LRC_fftFilterSub(3,l_pIn[f],l_pOut[f],l_fftw_in[f],l_fftw_out[f],l_fftw_p[f],l_fftw_ip[f],r_pIn[f],r_pOut[f],r_fftw_in[f],r_fftw_out[f],r_fftw_p[f],r_fftw_ip[f],param,p,f);
		}

		#pragma omp for
		for (i = 0;i < wkMemSize;i++) {
			mem_r[i] = r_mem2[i] + r_mem3[i] + r_mem4[i];
		}
#endif
		if (startInSample + fftSizeIn / 2 >= 0) {
			if (outRemain >= fftSizeOut) {
				wr = fio_write(mem_l + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut,fio_l);
				if (wr != fftSizeOut) {
					param->errLine = __LINE__;
					param->err =	STATUS_FILE_WRITE_ERR;
					return;
				}

				wr = fio_write(mem_r + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut,fio_r);
				if (wr != fftSizeOut) {
					param->errLine = __LINE__;
					param->err =	STATUS_FILE_WRITE_ERR;
					return;
				}

				if (startInSample + (fftSizeOut / 2) <= inSampleR) {
					memset(mem_c + (fftSizeOut / 2),0,(fftSizeOut / 2) * sizeof (SSIZE));
				}
				if (startInSample + fftSizeOut >= inSample1 - (fftSizeOut / 2)) {
					memset(mem_c + (fftSizeOut / 2),0,fftSizeOut * sizeof (SSIZE));
				}

				wr = fio_write(mem_c + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut,fio_c);
				if (wr != fftSizeOut) {
					param->errLine = __LINE__;
					param->err =	STATUS_FILE_WRITE_ERR;
					return;
				}

			} else {
				wr = fio_write(mem_l + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fio_l);
				if (wr != outRemain) {
					param->errLine = __LINE__;
					param->err =	STATUS_FILE_WRITE_ERR;
					return;
				}

				wr = fio_write(mem_r + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fio_r);
				if (wr != outRemain) {
					param->errLine = __LINE__;
					param->err =	STATUS_FILE_WRITE_ERR;
					return;
				}

				if (startInSample + (fftSizeOut / 2) + outRemain <= inSample1 - (fftSizeOut / 2)) {
					memset(mem_c + (fftSizeOut / 2),0,fftSizeOut * sizeof (SSIZE));
				}

				wr = fio_write(mem_c + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fio_c);
				if (wr != outRemain) {
					param->errLine = __LINE__;
					param->err =	STATUS_FILE_WRITE_ERR;
					return;
				}
			}
			if (outRemain >= fftSizeOut) {
				outRemain -= fftSizeOut;
			} else {
				break;
			}
		}
	}

	NormInfoWav2.maxR3 = NormInfoWav2.maxR1;
	NormInfoWav2.minR3 = NormInfoWav2.minR1;
	NormInfoWav2.maxLv3 = (NormInfoWav2.maxLv1 + NormInfoWav2.maxLv2) / 2;
	if (NormInfoWav2.maxR2 > NormInfoWav2.maxR3) {
		NormInfoWav2.maxR3 = NormInfoWav2.maxR2;
	}
	if (NormInfoWav2.minR2 < NormInfoWav2.minR3) {
		NormInfoWav2.minR3 = NormInfoWav2.minR2;
	}
	
	fio_flush(fio_l);
	fio_flush(fio_r);
	fio_flush(fio_c);
	

	al_free(l_mem1);
	al_free(l_mem2);
	al_free(l_mem3);
	al_free(l_mem4);
	
	al_free(r_mem1);
	al_free(r_mem2);
	al_free(r_mem3);
	al_free(r_mem4);

	al_free(mem_l);
	al_free(mem_r);
	al_free(mem_c);

	// 1
	fftw_destroy_plan(l_fftw_p[0]);
	fftw_destroy_plan(l_fftw_ip[0]);
	fftw_free(l_fftw_in[0]);
	fftw_free(l_fftw_out[0]);

	// 2
	fftw_destroy_plan(l_fftw_p[1]);
	fftw_destroy_plan(l_fftw_ip[1]);
	fftw_free(l_fftw_in[1]);
	fftw_free(l_fftw_out[1]);

	// 3
	fftw_destroy_plan(l_fftw_p[2]);
	fftw_destroy_plan(l_fftw_ip[2]);
	fftw_free(l_fftw_in[2]);
	fftw_free(l_fftw_out[2]);

	// 1
	fftw_destroy_plan(r_fftw_p[0]);
	fftw_destroy_plan(r_fftw_ip[0]);
	fftw_free(r_fftw_in[0]);
	fftw_free(r_fftw_out[0]);

	// 2
	fftw_destroy_plan(r_fftw_p[1]);
	fftw_destroy_plan(r_fftw_ip[1]);
	fftw_free(r_fftw_in[1]);
	fftw_free(r_fftw_out[1]);

	// 3
	fftw_destroy_plan(r_fftw_p[2]);
	fftw_destroy_plan(r_fftw_ip[2]);
	fftw_free(r_fftw_in[2]);
	fftw_free(r_fftw_out[2]);

}
//---------------------------------------------------------------------------
// Function   : LRC_fftFilterSub
// Description: FFT によるフィルタ処理(サブ関数)
// ---
//	param		:変換パラメータ
//
static void LRC_fftFilterSub(int type,SSIZE *l_pIn,SSIZE *l_pOut,fftw_complex *l_fftw_in,fftw_complex *l_fftw_out,fftw_plan l_fftw_p,fftw_plan l_fftw_ip,SSIZE *r_pIn,SSIZE *r_pOut,fftw_complex *r_fftw_in,fftw_complex *r_fftw_out,fftw_plan r_fftw_p,fftw_plan r_fftw_ip,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p,int id)
{
	long inSampleR,outSampleR;
	long wkSampleR;
	long fftSizeIn,fftSizeOut,i,j,n;
	long cutOff;
	long checkHzIndex1,checkHzIndex2,checkHzIndex3;
	long lowIndex;
	long hfc;
	double nx;
	double pp1,pp2,pp_diff;
	double phase1,phase2,phase_diff;
	double avg1,avg2;

	long validIndex;
	long h;
	char sss[200];

	inSampleR  = p->inSampleR;
	outSampleR = inSampleR;
	fftSizeIn = inSampleR * 2;
	fftSizeOut = outSampleR * 2;

	// 音量調査
	avg1 = 0;
	avg2 = 0;
	for (i = 0;i < fftSizeIn;i++) {
		avg1 += l_pIn[i];
		avg1 += r_pIn[i];
	}
	avg1 /= fftSizeIn;
	avg1 /= 2;

	// FFT 初期設定(Left)
	copyToFFTW(l_fftw_in,l_pIn,fftSizeIn);
	windowFFTW(l_fftw_in,fftSizeIn);
	for (i = 0;i < fftSizeOut;i++) {
		l_fftw_out[i][0] = 0;
		l_fftw_out[i][1] = 0;
	}

	// FFT 初期設定(Right)
	copyToFFTW(r_fftw_in,r_pIn,fftSizeIn);
	windowFFTW(r_fftw_in,fftSizeIn);
	for (i = 0;i < fftSizeOut;i++) {
		r_fftw_out[i][0] = 0;
		r_fftw_out[i][1] = 0;
	}
	
	// FFT
	fftw_execute(l_fftw_p);
	fftw_execute(r_fftw_p);

	// 高域削除
	wkSampleR = inSampleR;
	if (wkSampleR > outSampleR) {
		wkSampleR = outSampleR;
	}
	hfc = wkSampleR / 2;
	cutOff = ((double)fftSizeOut / outSampleR) * hfc;

	checkHzIndex1 = ((double)fftSizeOut / outSampleR) * 1000;
	checkHzIndex2 = ((double)fftSizeOut / outSampleR) * 2000;
	checkHzIndex3 = ((double)fftSizeOut / outSampleR) * 1625;

	if (type == 1) {
		// Center

		for (i = 0;i < fftSizeOut / 2;i++) {
			pp1 = (l_fftw_out[i][0] * l_fftw_out[i][0]) + (l_fftw_out[i][1] * l_fftw_out[i][1]);
			if (pp1 > 0) {
				pp1 = sqrt(pp1);
			}
			phase1 = atan(l_fftw_out[i][1] / l_fftw_out[i][0]);

			pp2 = (r_fftw_out[i][0] * r_fftw_out[i][0]) + (r_fftw_out[i][1] * r_fftw_out[i][1]);
			if (pp2 > 0) {
				pp2 = sqrt(pp2);
			}
			phase2 = atan(r_fftw_out[i][1] / r_fftw_out[i][0]);

			pp_diff = pp1 - pp2;if (pp_diff < 0) pp_diff = pp2 - pp1;
			phase_diff = phase1 - phase2;if (phase1 < phase2) phase_diff = phase2 - phase1;

			if (i == checkHzIndex1) {
				sprintf(sss,"Center(1000Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			if (i == checkHzIndex2) {
				sprintf(sss,"Center(2000Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			if (i == checkHzIndex3) {
				sprintf(sss,"Center(1625Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			l_fftw_out[i][0] = (l_fftw_out[i][0] + r_fftw_out[i][0]) / 2;
			l_fftw_out[i][1] = (l_fftw_out[i][1] + r_fftw_out[i][1]) / 2;

			if (phase_diff > p->hfa_LRC_CC_Phase) {
//				l_fftw_out[i][0] *= (double)0.0002;
//				l_fftw_out[i][1] *= (double)0.0002;
				l_fftw_out[i][0] *= (double)0.000001;
				l_fftw_out[i][1] *= (double)0.000001;
			} else {
				l_fftw_out[i][0] *= p->hfa_LRC_CC_Level;
				l_fftw_out[i][1] *= p->hfa_LRC_CC_Level;
			}
		}

		// カットオフ
		cutFFTW(l_fftw_out,cutOff,fftSizeOut);

		// 半分のデータを復元
		for (i = 1;i < fftSizeOut / 2;i++) {
			l_fftw_out[fftSizeOut - i][0] = l_fftw_out[i][0];
			l_fftw_out[fftSizeOut - i][1] = l_fftw_out[i][1] * -1;
		}

		// invert FFT
		fftw_execute(l_fftw_ip);

		// 出力
		for (i = 0;i < fftSizeOut;i++) {
			l_pOut[i] = (SSIZE)(l_fftw_in[i][0] / fftSizeOut);
		}

		for (i = 0;i < fftSizeOut;i++) {
			avg2 += l_pOut[i];
		}
		avg2 /= fftSizeOut;

		sprintf(sss,"Center avg1:%lf,avg2:%lf",avg1,avg2);
		PRINT_LOG(sss);

	} else if (type == 2) {
		// Left

		for (i = 0;i < fftSizeOut / 2;i++) {
			pp1 = (l_fftw_out[i][0] * l_fftw_out[i][0]) + (l_fftw_out[i][1] * l_fftw_out[i][1]);
			if (pp1 > 0) {
				pp1 = sqrt(pp1);
			}
			phase1 = atan(l_fftw_out[i][1] / l_fftw_out[i][0]);

			pp2 = (r_fftw_out[i][0] * r_fftw_out[i][0]) + (r_fftw_out[i][1] * r_fftw_out[i][1]);
			if (pp2 > 0) {
				pp2 = sqrt(pp2);
			}
			phase2 = atan(r_fftw_out[i][1] / r_fftw_out[i][0]);

			pp_diff = pp1 - pp2;if (pp_diff < 0) pp_diff = pp2 - pp1;
			phase_diff = phase1 - phase2;if (phase1 < phase2) phase_diff = phase2 - phase1;

			if (i == checkHzIndex1) {
				sprintf(sss,"Left(1000Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			if (i == checkHzIndex2) {
				sprintf(sss,"Left(2000Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			if (i == checkHzIndex3) {
				sprintf(sss,"Left(1625Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			//if (pp_diff < (double)9517704981152000) {
			//	l_fftw_out[i][0] *= 0.00001;
			//	l_fftw_out[i][1] *= 0.00001;
			//}
		}

		// カットオフ
		cutFFTW(l_fftw_out,cutOff,fftSizeOut);

		// 半分のデータを復元
		for (i = 1;i < fftSizeOut / 2;i++) {
			l_fftw_out[fftSizeOut - i][0] = l_fftw_out[i][0];
			l_fftw_out[fftSizeOut - i][1] = l_fftw_out[i][1] * -1;
		}

		// invert FFT
		fftw_execute(l_fftw_ip);

		// 出力
		for (i = 0;i < fftSizeOut;i++) {
			l_pOut[i] = (SSIZE)(l_fftw_in[i][0] / fftSizeOut);
		}

	} else if (type == 3) {
		// Right

		for (i = 0;i < fftSizeOut / 2;i++) {
			pp1 = (l_fftw_out[i][0] * l_fftw_out[i][0]) + (l_fftw_out[i][1] * l_fftw_out[i][1]);
			if (pp1 > 0) {
				pp1 = sqrt(pp1);
			}
			phase1 = atan(l_fftw_out[i][1] / l_fftw_out[i][0]);

			pp2 = (r_fftw_out[i][0] * r_fftw_out[i][0]) + (r_fftw_out[i][1] * r_fftw_out[i][1]);
			if (pp2 > 0) {
				pp2 = sqrt(pp2);
			}
			phase2 = atan(r_fftw_out[i][1] / r_fftw_out[i][0]);

			pp_diff = pp1 - pp2;if (pp_diff < 0) pp_diff = pp2 - pp1;
			phase_diff = phase1 - phase2;if (phase1 < phase2) phase_diff = phase2 - phase1;

			if (i == checkHzIndex1) {
				sprintf(sss,"Right(1000Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			if (i == checkHzIndex2) {
				sprintf(sss,"Right(2000Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			if (i == checkHzIndex3) {
				sprintf(sss,"Right(1625Hz) pp1:%lf,pp2:%lf,diff:%lf,phase1:%lf,phase2:%lf,phase_diff:%lf",pp1,pp2,pp_diff,phase1,phase2,phase_diff);
				PRINT_LOG(sss);
			}

			//if (pp_diff < (double)9517704981152000) {
			//	r_fftw_out[i][0] *= 0.00001;
			//	r_fftw_out[i][1] *= 0.00001;
			//}
		}

		// カットオフ
		cutFFTW(r_fftw_out,cutOff,fftSizeOut);

		// 半分のデータを復元
		for (i = 1;i < fftSizeOut / 2;i++) {
			r_fftw_out[fftSizeOut - i][0] = r_fftw_out[i][0];
			r_fftw_out[fftSizeOut - i][1] = r_fftw_out[i][1] * -1;
		}

		// invert FFT
		fftw_execute(r_fftw_ip);

		// 出力
		for (i = 0;i < fftSizeOut;i++) {
			r_pOut[i] = (SSIZE)(r_fftw_in[i][0] / fftSizeOut);
		}
	}
}
static void LRC_merageTempFile(char type,int normFlag,FIO *fp_r,FIO *fp_r2,FIO *fp_w,SSIZE inSample,int *p_errLine)
{
	SSIZE min,max;
	SSIZE maxLv,maxLv2;
	SSIZE remainSample;
	SSIZE ns;
	SSIZE *diskBuffer1;
	SSIZE *diskBuffer2;
	double per,percent;
	long i;
	fio_size remain1,remain2;
	fio_size wr_n;

	PRINT_FN_LOG("wav2raw","-LRC_merageTempFile:Start");

	fio_rewind(fp_r);

	if (fp_r2 != NULL) {
		fio_rewind(fp_r2);
	}

	if (fp_w != NULL) {
		fio_rewind(fp_w);
	}

	remainSample = inSample;

	if (fp_r->error) {
		*p_errLine = __LINE__;
		// エラー終了
		return;
	}

	diskBuffer1 = malloc(4 * 1024 * 1024 * sizeof (SSIZE));
	diskBuffer2 = malloc(4 * 1024 * 1024 * sizeof (SSIZE));
	if (diskBuffer1 == NULL || diskBuffer2 == NULL) {
		*p_errLine = __LINE__;
		return;
	}


	per = -1;
	do {
		percent = ((double)(inSample - remainSample) / inSample);
		percent *= 100;
		percent = (double)((int)percent);
		if (percent != per) {
			fprintf(stdout,"%d%%\n",(int)percent);
			fflush(stdout);
		}
		per = percent;

		if (type == '+') {
			if (fp_r2 == NULL) {
				remain1 = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
			} else {
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							remain1 = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
						}
						#pragma omp section
						{
							remain2 = fio_read(diskBuffer2,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r2);
						}
					}
				}
			}

			if (remain1 == 0 || remain2 == 0) {
				break;
			}
			for (i = 0;i < remain1;i++) {
				if (diskBuffer1[i] != 0) {
					diskBuffer1[i] += diskBuffer2[i];
				}
			}
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fp_w != NULL) {
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					PRINT_LOG("error LRC_merageTempFile");
					*p_errLine = __LINE__;
					break;
				}
			}
			remainSample -= remain1;
		} else if (type == '-') {
			if (fp_r2 == NULL) {
				remain1 = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
			} else {
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							remain1 = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
						}
						#pragma omp section
						{
							remain2 = fio_read(diskBuffer2,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r2);
						}
					}
				}
			}
			if (remain1 == 0) {
				break;
			}
			for (i = 0;i < remain1;i++) {
				if (diskBuffer1[i] != 0) {
					diskBuffer1[i] -= diskBuffer2[i];
				}
			}
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fp_w != NULL) {
//				fio_seek(fp_w,pos,SEEK_SET);
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					PRINT_LOG("error merageTempFile");
					*p_errLine = __LINE__;
					break;
				}
			}
			remainSample -= remain1;
		} else if (type == ' ') {
			//Sleep(1);
			remain1 = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
			if (remain1 == 0) {
				break;
			}
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fp_w != NULL) {
//				fio_seek(fp_w,pos,SEEK_SET);
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					PRINT_LOG("error merageTempFile");
					*p_errLine = __LINE__;
					break;
				}
			}
			remainSample -= remain1;
		}
	} while (remain1 == 4 * 1024 * 1024L &&  remainSample > 0);

	if (remainSample > 0) {
		*p_errLine = __LINE__;
	}
	
	if (fp_w != NULL) {
		fio_flush(fp_w);
	}

	free(diskBuffer1);
	free(diskBuffer2);

	PRINT_FN_LOG("wav2raw","-merageTempFile:End");
}

//---------------------------------------------------------------------------
// Function   : onebit2nbit
// Description: 1bit データを 64bit データへ変換して読み込む
// ---
//	offset		:入力オフセット(サンプル毎)
//	n			:サンプル数
//	buffer		:データバッファ
//	fp_r		:入力FIO
//	param		:変換パラメータ
//
void onebit2nbit(SSIZE offset,SSIZE sample,SSIZE *buffer,FIO *fp_r,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	unsigned char *bit_buffer;
	int *delay_buffer;
	unsigned char mask;
	double nbit_r;
	SSIZE nbit_data;
	SSIZE seek_ptr;
	int i,j,n,bit_count_p,bit_count_m;
	int ptr_in_byte,ptr_out_byte;
	int ptr_delay;
	int delay;
	int n_delay;
	int zero_count;
	fio_size rs;
	char sss[50];
	
	PRINT_FN_LOG(param->debug_upconv,"onebit2nbit:Start");

	memset(buffer,0,sample * sizeof (SSIZE));
	if (offset < 0) {
		if ((offset * -1) >= sample) {
			// ファイルから読み込むデータがないのでリターンする
			return;
		}
		buffer += (offset * -1);
		sample -= (offset * -1);
		offset	= 0;
	}

	if (offset + sample > param->n_sample) {
		sample = param->n_sample - offset;
	}

	if (sample <= 0) {
		return;
	}

	delay = 24;

	bit_buffer	 = (unsigned char *)malloc(4096);
	delay_buffer = (int *)malloc(delay * 2 * sizeof (int));
	if (bit_buffer == NULL || delay_buffer == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	ptr_delay = delay;
	
	// delay_buffer には3値を使う(1,-1,0)
	for (i = 0;i < delay * 2;i++) {
		delay_buffer[i] = 0;	// 無音
	}

	seek_ptr = offset - delay;
	n_delay = delay;
	if ((seek_ptr + delay) >= 0) {
		// delay バッファに以前の値をセットする
		if (seek_ptr < 0) {
			n_delay += seek_ptr;
			seek_ptr = 0;
		}

		ptr_out_byte = 0;
		while (n_delay > 0) {
			fio_seek(fp_r,(seek_ptr / 8) + param->w2r_data_offset,SEEK_SET);
			if (fp_r->error) {
				param->err = STATUS_FILE_READ_ERR;
				return;
			}
			rs = fio_read(bit_buffer,1,(n_delay + 7) / 8,fp_r);
			if (fp_r->error) {
				param->err = STATUS_FILE_READ_ERR;
				return;
			}
			if (rs == 0) {
				break;
			}
			ptr_in_byte = 0;
			mask = 0x01 << (seek_ptr % 8);
//			if (bit_buffer[ptr_in_byte] == 0x69 && bit_buffer[ptr_in_byte + 1] == 0x69 && bit_buffer[ptr_in_byte + 2] == 0x69) {
				for (i = 0;i < 8 * 3;i++) {
					delay_buffer[i] = 0;
				}
				n_delay = 0;
//			} else {
				do {
					if ((bit_buffer[ptr_in_byte] & mask)) {
						delay_buffer[ptr_out_byte] = 1;
					} else {
						delay_buffer[ptr_out_byte] = -1;
					}
					mask <<= 1;
					if (mask == 0x00) {
						mask = 0x01;
						ptr_in_byte++;
						rs--;
					}
					ptr_out_byte++;
					seek_ptr++;
					n_delay--;
				} while (rs && n_delay > 0);
//			}
		}
	}

	bit_count_p = bit_count_m = -1;
	ptr_out_byte = 0;
	zero_count = 0;

	while (sample > 0) {
		seek_ptr = offset;
		seek_ptr /= 8;			// bit
		fio_seek(fp_r,seek_ptr,SEEK_SET);
		if (fp_r->error) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		rs = fio_read(bit_buffer,1,4096,fp_r);
		if (fp_r->error) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}

		seek_ptr = offset;
		seek_ptr /= 8;			// bit
		ptr_in_byte = 0;
		mask = 0x01 << (offset % 8);

		do {
			if (bit_count_p != -1 && bit_count_m != -1) {
				if (delay_buffer[ptr_delay - delay] == 1) {
					bit_count_p--;
				} else if (delay_buffer[ptr_delay - delay] == -1) {
					bit_count_m--;
				}
			}
			if (zero_count == 0) {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					delay_buffer[ptr_delay] = 1;
					delay_buffer[ptr_delay - delay] = 1;
					if (bit_count_p != -1) {
						bit_count_p++;
					}
				} else {
					delay_buffer[ptr_delay] = -1;
					delay_buffer[ptr_delay - delay] = -1;
					if (bit_count_m != -1) {
						bit_count_m++;
					}
				}
			} else {
				delay_buffer[ptr_delay] = 0;
				delay_buffer[ptr_delay - delay] = 0;
			}
			if (bit_count_p == -1 && bit_count_m == -1) {
				for (i = 0,bit_count_p = 0,bit_count_m = 0;i < delay;i++) {
					if (delay_buffer[ptr_delay - i] == 1) {
						bit_count_p++;
					} else if (delay_buffer[ptr_delay - i] == -1) {
						bit_count_m++;
					}
				}
			}
			if ((bit_count_p + bit_count_m) > 0) {
				nbit_r = (double)1.0 * ((double)bit_count_p / (bit_count_p + bit_count_m));
				if ((bit_count_p + bit_count_m) < delay) {
					nbit_r = (nbit_r / delay) * (bit_count_p + bit_count_m) + ((double)0.5 / delay) * (delay - (bit_count_p + bit_count_m));
				}
				if (p->inSampleR == 2822400) {
					nbit_data = (SSIZE)1 << 53;
					nbit_data = (SSIZE)((double)nbit_data * nbit_r);
					nbit_data -= (SSIZE)1 << (53 - 1);
				} else if (p->inSampleR == 2822400 * 2) {
					nbit_data = (SSIZE)1 << 51;
					nbit_data = (SSIZE)((double)nbit_data * nbit_r);
					nbit_data -= (SSIZE)1 << (51 - 1);
				} else {
					nbit_data = (SSIZE)1 << 49;
					nbit_data = (SSIZE)((double)nbit_data * nbit_r);
					nbit_data -= (SSIZE)1 << (49 - 1);
				}
			} else {
				if (p->inSampleR == 2822400) {
					nbit_data = (SSIZE)1 << 53;
					nbit_data = (SSIZE)((double)nbit_data * 0.5);
					nbit_data -= (SSIZE)1 << (53 - 1);
				} else if (p->inSampleR == 2822400 * 2) {
					nbit_data = (SSIZE)1 << 51;
					nbit_data = (SSIZE)((double)nbit_data * 0.5);
					nbit_data -= (SSIZE)1 << (51 - 1);
				} else {
					nbit_data = (SSIZE)1 << 49;
					nbit_data = (SSIZE)((double)nbit_data * 0.5);
					nbit_data -= (SSIZE)1 << (49 - 1);
				}
			}
			if (zero_count > 0) {
				zero_count--;
			}
			if (sample > 0) {
				buffer[ptr_out_byte] = nbit_data;
				ptr_out_byte ++;
				offset++;
				sample--;
			}
			ptr_delay++;
			if (ptr_delay >= delay*2) {
				ptr_delay = delay;
			}
			mask <<= 1;
			if (mask == 0) {
				mask = 0x01;
				ptr_in_byte++;
			}
		} while (ptr_in_byte < rs);
	}
	free(bit_buffer);
	free(delay_buffer);
	PRINT_FN_LOG(param->debug_upconv,"onebit2nbit:End");
}
//---------------------------------------------------------------------------
// Function   : ana_abe
// Description: 高域ノイズ軽減処理
// ---
//	start		:開始位置
//	nSample		:サンプル数
//	i_buffer	:入力バッファ
//	o_buffer	:出力バッファ
//	param		:パラメーター構造体
//	
//
void ana_abe(SSIZE start,UI64 nSample,SSIZE *i_buffer,SSIZE *o_buffer,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	int i,j,n;
	
	SSIZE min,max,dd;
	PRINT_FN_LOG(param->debug_upconv,"ana_abe:Start");

	if (start < 3) {
		for (i = start + 3,j = 0;j < nSample;i++,j++) {
			for (n = 0,min = i_buffer[i],max = i_buffer[i];n < 3;n++) {
				if (min > i_buffer[i - n]) {
					min = i_buffer[i - n];
				} else if (max < i_buffer[i - n]) {
					max = i_buffer[i - n];
				}
			}
			o_buffer[i] = (min + max) / 2;
		}
	} else {
		for (i = start,j = 0;j < nSample;i++,j++) {
			for (n = 0,min = i_buffer[i],max = i_buffer[i];n < 3;n++) {
				if (min > i_buffer[i - n]) {
					min = i_buffer[i - n];
				} else if (max < i_buffer[i - n]) {
					max = i_buffer[i - n];
				}
			}
			o_buffer[i] = (min + max) / 2;
		}
	}
	if (p->inSampleR == 2822400) {
		if (start < 10) {
			for (i = start + 10,j = 0;j < nSample;i++,j++) {
				for (n = 0,min = o_buffer[i],max = o_buffer[i];n < 10;n++) {
					if (min > o_buffer[i - n]) {
						min = o_buffer[i - n];
					} else if (max < o_buffer[i - n]) {
						max = o_buffer[i - n];
					}
				}
				o_buffer[i] = (min + max) / 2;
			}
		} else {
			for (i = start,j = 0;j < nSample;i++,j++) {
				for (n = 0,min = o_buffer[i],max = o_buffer[i];n < 10;n++) {
					if (min > o_buffer[i - n]) {
						min = o_buffer[i - n];
					} else if (max < o_buffer[i - n]) {
						max = o_buffer[i - n];
					}
				}
				o_buffer[i] = (min + max) / 2;
			}
		}

		for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
			o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
		}
		for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
			o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
		}
	}
	PRINT_FN_LOG(param->debug_upconv,"ana_abe:End");
}

//---------------------------------------------------------------------------
// Function   : ms_process_1
// Description: Mid/Sideに分ける処理を実施する。
// ---
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r1		:入力ファイル(Left)
//	fp_r2		:入力ファイル(Right)
//	fp_w1		:出力ファイル(Mid)
//	fp_w2		:出力ファイル(Side)
//	param		:変換パラメータ
//
void ms_process_1(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	SSIZE i,n;
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4,*mem5;
	SSIZE *pIn,*pOut;
	SSIZE value;
	SSIZE outRemain;
	long wkMemSize;
	long hz;
	long fftSizeIn,fftSizeOut;
	SSIZE startInSample,nSample;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;

	double percent,per;

	PRINT_FN_LOG(param->debug_upconv,"ms_process_1:Start");
	fio_rewind(fp_r1);
	fio_rewind(fp_r2);

	fftSizeIn  = p->inSampleR * 2;
	fftSizeOut = fftSizeIn * 2;
	wkMemSize = (fftSizeOut * 2) * sizeof (SSIZE);
	outRemain = inSample * 2;

	mem1 = (SSIZE *)malloc(wkMemSize);
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem2 = (SSIZE *)malloc(wkMemSize);
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem3 = (SSIZE *)malloc(wkMemSize);
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem4 = (SSIZE *)malloc(wkMemSize);
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem5 = (SSIZE *)malloc(wkMemSize);
	if (mem5 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_out == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p = fftw_plan_dft_1d(fftSizeIn,fftw_in,fftw_out,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip = fftw_plan_dft_1d(fftSizeOut,fftw_out,fftw_in,FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	per = -1;
	for (startInSample = ((fftSizeIn + (fftSizeIn / 2)) * -1);startInSample < inSample + (fftSizeIn + fftSizeIn / 2);startInSample += fftSizeIn) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
				fflush(stdout);
			}
			per = percent;
		}
		memset(mem1,0,wkMemSize);
		memset(mem2,0,wkMemSize);
		memset(mem3,0,wkMemSize);
		memset(mem4,0,wkMemSize);
		memset(mem5,0,wkMemSize);

		if (startInSample >= 0 && startInSample + (fftSizeIn * 2) < inSample) {
			fio_seek(fp_r1,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_seek(fp_r2,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSizeIn * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r1);
			fio_read(mem2,sizeof (SSIZE),nSample,fp_r2);
		} else {
			mem0 = mem1;
			nSample = fftSizeIn * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r1,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r1,0,SEEK_SET);
				mem0 += (startInSample * -1);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r1);
			}
			mem0 = mem2;
			nSample = fftSizeIn * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r2,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r2,0,SEEK_SET);
				mem0 += (startInSample * -1);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r2);
			}
			nSample = fftSizeIn * 2;
		}
		if (fp_r1->error) break;
		if (fp_r2->error) break;

		for (n = 0;n < 3;n++) {
			pIn = mem1;
			// FFT 初期設定
			for (i = 0;i < fftSizeIn;i++) {
				fftw_in[i][0] = pIn[((fftSizeIn / 2) * n) + i];
				fftw_in[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSizeIn - 1) / 2;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSizeIn);
			}
			for (i = (fftSizeIn - 1) / 2;i < fftSizeIn;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSizeIn);
			}

			// FFT
			fftw_execute(fftw_p);

			// x2 のオーバーサンプリング
			for (i = fftSizeIn / 2;i < fftSizeOut;i++) {
				fftw_out[i][0] = 0;
				fftw_out[i][1] = 0;
			}
			// 半分のデータを復元
			for (i = 1;i < fftSizeOut / 2;i++) {
				fftw_out[fftSizeOut - i][0] = fftw_out[i][0];
				fftw_out[fftSizeOut - i][1] = fftw_out[i][1] * -1;
			}
			fftw_out[0][0] = 0;
			fftw_out[0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip);

			// 出力
			pOut = (SSIZE *)mem5;
			for (i = 0;i < fftSizeOut;i++) {
				pOut[((fftSizeOut / 2) * n) + i] += fftw_in[i][0] / fftSizeOut;
			}
		}
		if (startInSample + fftSizeIn / 2 >= 0) {
			memcpy(mem1,mem5 + fftSizeOut / 2,fftSizeOut * sizeof (SSIZE));
		}

		memset(mem5,0,wkMemSize);

		for (n = 0;n < 3;n++) {
			pIn = mem2;
			// FFT 初期設定
			for (i = 0;i < fftSizeIn;i++) {
				fftw_in[i][0] = pIn[((fftSizeIn / 2) * n) + i];
				fftw_in[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSizeIn - 1) / 2;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSizeIn);
			}
			for (i = (fftSizeIn - 1) / 2;i < fftSizeIn;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSizeIn);
			}

			// FFT
			fftw_execute(fftw_p);

			// x2 のオーバーサンプリング
			for (i = fftSizeIn / 2;i < fftSizeOut;i++) {
				fftw_out[i][0] = 0;
				fftw_out[i][1] = 0;
			}
			// 半分のデータを復元
			for (i = 1;i < fftSizeOut / 2;i++) {
				fftw_out[fftSizeOut - i][0] = fftw_out[i][0];
				fftw_out[fftSizeOut - i][1] = fftw_out[i][1] * -1;
			}
			fftw_out[0][0] = 0;
			fftw_out[0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip);

			// 出力
			pOut = (SSIZE *)mem5;
			for (i = 0;i < fftSizeOut;i++) {
				pOut[((fftSizeOut / 2) * n) + i] += fftw_in[i][0] / fftSizeOut;
			}
		}

		if (startInSample + fftSizeIn / 2 >= 0) {
			memcpy(mem2,mem5 + fftSizeOut / 2,fftSizeOut * sizeof (SSIZE));
		}

		for (i = 0;i < fftSizeOut;i++) {
			value = (mem1[i] + mem2[i]) / 2;
			mem3[i] = value;
			value = (mem1[i] - mem2[i]) / 2;
			mem4[i] = value;
		}
		if (startInSample + fftSizeIn / 2 >= 0) {
			if (outRemain >= fftSizeOut) {
				fio_write(mem3,sizeof (SSIZE),fftSizeOut,fp_w1);
				fio_write(mem4,sizeof (SSIZE),fftSizeOut,fp_w2);
				outRemain -= fftSizeOut;
			} else {
				fio_write(mem3,sizeof (SSIZE),outRemain,fp_w1);
				fio_write(mem4,sizeof (SSIZE),outRemain,fp_w2);
				outRemain = 0;
			}
			if (fp_w1->error) break;
			if (fp_w2->error) break;
		}
	}

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);

	free(mem1);
	free(mem2);
	free(mem3);
	free(mem4);
	free(mem5);
	PRINT_FN_LOG(param->debug_upconv,"ms_process_1:End");
}
//---------------------------------------------------------------------------
// Function   : ms_process_2
// Description: Mid/Side処理。失われたSideを補間する。
// ---
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r1		:入力ファイル(Mid)
//	fp_r2		:入力ファイル(Side)
//	fp_w1		:出力ファイル(Side)
//	param		:変換パラメータ
//
void ms_process_2(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4,*mem5,*mem6;
	long hfc,before_hfc;
	long upper_hfc;
	char dbg1[128];
	long outSampleR;
	char dbg2[128];
	long wkMemSize;
	long fftSize,fftSize_2;
	long lowIndex,highIndex,pwCount;
	long before_highIndex;
	long hz,idx;
	long i,ii,pm_i_1,pm_i_2,n,nn,nnn;
	long lowHz,highHz;
	long midHFC,sideHFC;
	long midPw_Count[100],sidePw_Count[100];
	long sidePw_2_Count[200];
	long side_log10[200];
	long pwCutFlag[200];
	long limitHFC;
	double percent,per;
	double pp;
	double midPw[100],sidePw[100];
	double sidePw_2[200];
	double total_pw;
	double side1Pw,side2Pw;
	FILE *fp_ms4;
	char sss[300];
    double hfc_auto_shresh = (double)15000000000000.0;
	double min_shresh      = (double)1829159141700882.700000;
	//double shresh          = (double)171964351869993.750000;
	double shresh          = (double)1719643518699930.750000;
	int upper_log10_16;

	int oldPwIndex;
	int pw_count;
	int side_count;
	SSIZE *pIn,*pOut;
	SSIZE startInSample,nSample;
	SSIZE total_pw_count;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;

	fftw_complex *fftw_in_2,*fftw_out_2;
	fftw_plan fftw_p_2,fftw_ip_2;

	MS_DELAY ms_delay[7];

	PRINT_FN_LOG(param->debug_upconv,"ms_process_2:Start");

	for (i = 0;i < 128;i++) {
		dbg1[i] = 0x3F;
		dbg2[i] = 0x3F;
	}

	fp_ms4 = NULL;
	if (p->ms_process == 4) {
		fp_ms4 = fopen(param->file_ms4,"wt");
		PRINT_LOG(param->file_ms4);
	}

	ms_delay[0].delay = 1;
	ms_delay[0].atr   = 0.04;
	ms_delay[1].delay = 10;
	ms_delay[1].atr   = 0.07;
	ms_delay[2].delay = 20;
	ms_delay[2].atr   = 0.10;
	ms_delay[3].delay = 30;
	ms_delay[3].atr   = 0.10;
	ms_delay[4].delay = 40;
	ms_delay[4].atr   = 0.10;
	ms_delay[5].delay = 50;
	ms_delay[5].atr   = 0.10;
	ms_delay[6].delay = 100;
	ms_delay[6].atr   = 0.10;
	outSampleR = (p->inSampleR * 2);

	fio_rewind(fp_r1);
	fio_rewind(fp_r2);

	fftSize = outSampleR;
	fftSize /= 4;

	fftSize_2 = fftSize / 5;

	upper_hfc = p->inSampleR / 2;
	if (p->hfc != -1) {
		upper_hfc = p->hfc;
	}
	if (upper_hfc > 20000) {
		upper_hfc = 20000;
	}

	midHFC = sideHFC = p->inSampleR / 2;
//	if (p->hfc != -1 && p->hfc < midHFC) {
//		midHFC  = p->hfc;
//		sideHFC = p->hfc;
//	}
	limitHFC = midHFC;
	hfc      = midHFC;

	wkMemSize = (fftSize * 2) * sizeof (SSIZE);

	mem1 = (SSIZE *)malloc(wkMemSize);
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem2 = (SSIZE *)malloc(wkMemSize);
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem3 = (SSIZE *)malloc(wkMemSize);
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem4 = (SSIZE *)malloc(wkMemSize);
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem5 = (SSIZE *)malloc(wkMemSize);
	if (mem5 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem6 = (SSIZE *)malloc(wkMemSize);
	if (mem6 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize*2);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize*2);
	if (fftw_out == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p = fftw_plan_dft_1d(fftSize,fftw_in,fftw_out,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip = fftw_plan_dft_1d(fftSize,fftw_out,fftw_in,FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_in_2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize_2*2);
	if (fftw_in_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out_2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize_2*2);
	if (fftw_out_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_p_2 = fftw_plan_dft_1d(fftSize_2,fftw_in_2,fftw_out_2,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip_2 = fftw_plan_dft_1d(fftSize_2,fftw_out_2,fftw_in_2,FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	per = -1;
	for (startInSample = ((fftSize + (fftSize / 2)) * -1);startInSample < inSample + (fftSize + (fftSize / 2));startInSample += fftSize) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
				fflush(stdout);
			}
			per = percent;
		}

		memset(mem1,0,wkMemSize);	// Mid
		memset(mem2,0,wkMemSize);	// Side

		if (startInSample >= 0 && startInSample + (fftSize * 2) < inSample) {
			nSample = fftSize * 2;
			fio_seek(fp_r1,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_seek(fp_r2,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r1);	// Mid
			fio_read(mem2,sizeof (SSIZE),nSample,fp_r2);	// Side
		} else {
			mem0 = mem1;
			nSample = fftSize * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r1,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r1,0,SEEK_SET);
				mem0 += (startInSample * -1);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r1);
			}

			mem0 = mem2;
			nSample = fftSize * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r2,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r2,0,SEEK_SET);
				mem0 += (startInSample * -1);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r2);
			}
			nSample = fftSize * 2;
		}

		memset(mem3,0,wkMemSize);
		memset(mem4,0,wkMemSize);

		memset(mem5,0,wkMemSize);
		memset(mem6,0,wkMemSize);

		oldPwIndex = -1;
		for (n = 0;n < 3;n++) {
			pIn = mem2;
			if (n == 0) {
				nn = 1;
			} else if (n == 1) {
				nn = 0;
			} else if (n == 2) {
				nn = 2;
			}
			// FFT 初期設定
			for (i = 0;i < fftSize;i++) {
				fftw_in[i][0] = pIn[((fftSize / 2) * nn) + i];
				fftw_in[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSize - 1) / 2;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSize);
			}
			for (i = (fftSize - 1) / 2;i < fftSize;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSize);
			}

			// FFT
			fftw_execute(fftw_p);

			memset(midPw,0,sizeof (double) * 100);
			memset(sidePw,0,sizeof (double) * 100);
			memset(midPw_Count,0,sizeof (long) * 100);
			memset(sidePw_Count,0,sizeof (long) * 100);
			memset(pwCutFlag,0,sizeof (long) * 200);

			// Cutoff(Dynamic)
			for (nnn = 0;nnn < 5;nnn++) {
				// FFT 初期設定
				for (i = 0;i < fftSize_2;i++) {
					fftw_in_2[i][0] = pIn[((fftSize_2 / 2) * nn * nnn) + i];
					fftw_in_2[i][1] = 0;
				}
				// 窓関数
				for (i = 0;i < (fftSize_2 - 1) / 2;i++) {
					fftw_in_2[i][0] = fftw_in_2[i][0] * (2.0 * i / (double)fftSize_2);
				}
				for (i = (fftSize_2 - 1) / 2;i < fftSize_2;i++) {
					fftw_in_2[i][0] = fftw_in_2[i][0] * (2.0 - 2.0 * i / (double)fftSize_2);
				}

				memset(sidePw_2_Count,0,sizeof (long) * 200);
				memset(sidePw_2,0,sizeof (double) * 200);

				// FFT
				fftw_execute(fftw_p_2);
				for (i = 1;i < fftSize_2 / 2;i++) {
					hz = ((double)outSampleR / fftSize_2) * i;
					if (hz < 20000) {
						pp = fftw_out_2[i][0] * fftw_out_2[i][0] + fftw_out_2[i][1] * fftw_out_2[i][1];
						if (pp != 0) {
							pp = sqrt(pp);
						}

						hz = hz / 100;

						pp = fftw_out_2[i][0] * fftw_out_2[i][0] + fftw_out_2[i][1] * fftw_out_2[i][1];
						if (pp != 0) {
							pp = sqrt(pp);
						}

						sidePw_2[hz] += pp;
						sidePw_2_Count[hz]++;
					}
				}

				for (i = 0;i < 200;i++) {
					if (sidePw_2_Count[i] > 0) {
						sidePw_2[i] = sidePw_2[i] / sidePw_2_Count[i];
					}
				}

				// Power の桁数
				upper_log10_16 = 199;
				memset(side_log10,0,sizeof (long) * 200);
				for (i = 0;i < 200;i++) {
					side_log10[i] = log10_lf(sidePw_2[i]);
				}

				lowIndex  = ((double)fftSize_2 / outSampleR) * 2000;
				highIndex = ((double)fftSize_2 / outSampleR) * 20000;
				for (i = highIndex;i > lowIndex;i--) {
					hz = ((double)outSampleR / fftSize_2) * i;	
					hz = hz / 100;
					if (sidePw_2[hz] > hfc_auto_shresh) break;
				}
				if (before_highIndex > 0 && before_highIndex < highIndex) {
					highIndex = (before_highIndex + highIndex) / 2;
				}
				before_highIndex = highIndex;
				highIndex = i;

				for (i = 200 - 1;i > 10;i--) {
					if (side_log10[i] > 15) break;
				}
				upper_log10_16 = i;

				lowHz  = (outSampleR / (double)fftSize_2) * lowIndex;
				highHz = (outSampleR / (double)fftSize_2) * highIndex;
				sprintf(sss,"lowHz:%ld,highHz:%ld,log10_16:%d",lowHz,highHz,upper_log10_16 * 100);
				PRINT_LOG(sss);

				for (i = lowHz / 100;i < highHz / 100;i++) {
					if (sidePw_2[i] < shresh && i < upper_log10_16) {
						pwCutFlag[i]++;
					}
				}
				for (i = lowHz / 100;i < highHz / 100;i++) {
					if (i < upper_log10_16 && side_log10[i] < (side_log10[upper_log10_16] - 1)) {
						pwCutFlag[i]++;
					}
				}

				sprintf(sss,"sec:%ld,index:n=%d,nnn=%d",startInSample / outSampleR,n,nnn);
				PRINT_LOG(sss);
				for (i = 0;i < 200;i++) {
					sprintf(sss,"sidePw_2[%05ld],Power:%lf,%d",i * 100,sidePw_2[i],side_log10[i]);
					PRINT_LOG(sss);
				}
			}

			side1Pw = 1;
			side2Pw = 1;

			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				if (hz <= 20000) {
					pp = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
					if (pp != 0) {
						pp = sqrt(pp);
					}
					hz = hz / 200;
					sidePw[hz] += pp;
					sidePw_Count[hz]++;
				}
			}

			for (i = 0;i < 100;i++) {
				if (sidePw_Count[i] > 0) {
					sidePw[i] = sidePw[i] / sidePw_Count[i];
				}
			}

			sprintf(sss,"sec:%ld,index:n=%d",startInSample / outSampleR,n);
			PRINT_LOG(sss);
			for (i = 0;i < 100;i++) {
				sprintf(sss,"sidePw[%05ld],Power:%lf",i * 200,sidePw[i]);
				PRINT_LOG(sss);
			}

			// 量子化誤差部分の上限カット周波数を求める
			for (i = 100 - 1;i >= 20;i--) {
				if (sidePw[i] > hfc_auto_shresh) break;
			}
			if (i * 200 < limitHFC) {
				sideHFC = i * 200;
			}

			sprintf(sss,"sideHFC:%ld",sideHFC);
			PRINT_LOG(sss);

			total_pw_count = 0;
			total_pw = 0;
			// それぞれの周波数領域でのpwの合計を求めて音量を調査する
			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				if (hz <= 20000 || hz < sideHFC) {
					pp = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
					if (pp != 0) {
						pp = sqrt(pp);
					}
					total_pw += pp;
					total_pw_count++;
				}
			}

			if (total_pw_count > 0) {
				total_pw = total_pw / total_pw_count;
			}

			sprintf(sss,"total_pw[%ld]:%lf",startInSample / outSampleR,total_pw);
			PRINT_LOG(sss);
			if (total_pw < min_shresh) {
				// 音量が小さいときは補間はオフにする。
				for (i = 0;i < 200;i++) {
					pwCutFlag[i] = 0;
				}
			}

			if (p->ms_process != 4) {
				// 圧縮により音量が小さい箇所を見つけたらそれ以降をカットするためにデータを準備する。
				for (i = 0;i < 200;i++) {
					if (pwCutFlag[i] > 3) {
						break;
					}
				}
				for (;i < sideHFC / 100;i++) {
					pwCutFlag[i] = 9;
				}
			}

			// 圧縮により削られてしまった音は除去する
			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				idx = hz / 100;
				if (pwCutFlag[idx] > 3) {
					fftw_out[i][0] = 0;
					fftw_out[i][1] = 0;
				}
			}

			// 指定周波数以降はカットする
			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				if (hz >= sideHFC) {
					fftw_out[i][0] = 0;
					fftw_out[i][1] = 0;
				}
			}

			// Side1 の PW を調べる
			pwCount = 0;
			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				if (hz >= hfc - 1000 && hz < hfc) {
					pp = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
					if (pp != 0) {
						pp = sqrt(pp);
					}
					side1Pw += pp;
					pwCount++;
				}
			}
			if (pwCount > 0) {
				side1Pw /= pwCount;
			}

			// 半分のデータを復元
			for (i = 1;i < fftSize / 2;i++) {
				fftw_out[fftSize - i][0] = fftw_out[i][0];
				fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
			}
			fftw_out[0][0] = 0;
			fftw_out[0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip);

			// 出力
			pOut = (SSIZE *)mem3;
			for (i = 0;i < fftSize;i++) {
				pOut[((fftSize / 2) * nn) + i] += fftw_in[i][0] / fftSize;
			}

			// Mid を元に Side を作る
			if (!(p->ms_process == 3 || p->ms_process == 4)) {
				pIn = mem1;
				// FFT 初期設定
				if (p->ms_process == 1) {
					for (i = 0;i < fftSize;i++) {
						pm_i_1 = (((fftSize / 2) * nn) + i) - 0;
						pm_i_2 = (((fftSize / 2) * nn) + i) + 2;
						if (pm_i_1 >= 0 && pm_i_1 < nSample) {
							fftw_in[i][0] = (pIn[pm_i_1] * 0.09);
						} else {
							fftw_in[i][0] = 0;
						}
						if (pm_i_2 >= 0 && pm_i_2 < nSample) {
							fftw_in[i][0] += (pIn[pm_i_2] * 0.09);
						} else {
							fftw_in[i][0] = 0;
						}
						fftw_in[i][1] = 0;
					}
				} else if (p->ms_process == 2) {
					for (i = 0;i < fftSize;i++) {
						pm_i_1 = (((fftSize / 2) * nn) + i) - 0;
						pm_i_2 = (((fftSize / 2) * nn) + i) + 2;
						if (pm_i_1 >= 0 && pm_i_1 < nSample) {
							fftw_in[i][0] = (pIn[pm_i_1] * 0.09);
						} else {
							fftw_in[i][0] = 0;
						}
						if (pm_i_2 >= 0 && pm_i_2 < nSample) {
							fftw_in[i][0] += (pIn[pm_i_2] * 0.09);
						} else {
							fftw_in[i][0] = 0;
						}
						fftw_in[i][1] = 0;
					}
					for (i = 0;i < fftSize;i++) {
						pm_i_1 = (((fftSize / 2) * nn) + i) - ((outSampleR / 1000) * 7 * 2);
						pm_i_2 = (((fftSize / 2) * nn) + i) + ((outSampleR / 1000) * 19 * 2);
						if (pm_i_1 >= 0 && pm_i_1 < nSample) {
							fftw_in[i][0] = (pIn[pm_i_1] * 0.04);
						} else {
							fftw_in[i][0] = 0;
						}
						if (pm_i_2 >= 0 && pm_i_2 < nSample) {
							fftw_in[i][0] += (pIn[pm_i_2] * 0.04);
						} else {
							fftw_in[i][0] = 0;
						}
						fftw_in[i][1] = 0;
					}
					for (i = 0;i < fftSize;i++) {
						pm_i_1 = (((fftSize / 2) * nn) + i) - ((outSampleR / 1000) * 14 * 2);
						pm_i_2 = (((fftSize / 2) * nn) + i) + ((outSampleR / 1000) * 56 * 2);
						if (pm_i_1 >= 0 && pm_i_1 < nSample) {
							fftw_in[i][0] = (pIn[pm_i_1] * 0.01);
						} else {
							fftw_in[i][0] = 0;
						}
						if (pm_i_2 >= 0 && pm_i_2 < nSample) {
							fftw_in[i][0] += (pIn[pm_i_2] * 0.01);
						} else {
							fftw_in[i][0] = 0;
						}
						fftw_in[i][1] = 0;
					}
				} else if (p->ms_process >= 4) {
					char s[100];
					for (i = 0;i < fftSize;i++) {
						pm_i_1 = (((fftSize / 2) * nn) + i) - ((outSampleR / 1000) * ms_delay[p->ms_process - 4].delay);
						pm_i_2 = (((fftSize / 2) * nn) + i) + ((outSampleR / 1000) * ms_delay[p->ms_process - 4].delay);
//						sprintf(s,"pm_i_1:%d",pm_i_1);
//						PRINT_LOG(s);
//						sprintf(s,"pm_i_2:%d",pm_i_2);
//						PRINT_LOG(s);
						if (pm_i_1 >= 0 && pm_i_1 < nSample) {
							fftw_in[i][0] = (SSIZE)((double)pIn[pm_i_1] * ms_delay[p->ms_process - 4].atr);
						} else {
							fftw_in[i][0] = 0;
						}
						if (pm_i_2 >= 0 && pm_i_2 < nSample) {
							fftw_in[i][0] += (SSIZE)((double)pIn[pm_i_2] * ms_delay[p->ms_process - 4].atr);
						} else {
							fftw_in[i][0] = 0;
						}
						fftw_in[i][1] = 0;
					}
				}
				// 窓関数
				for (i = 0;i < (fftSize - 1) / 2;i++) {
					fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSize);
				}
				for (i = (fftSize - 1) / 2;i < fftSize;i++) {
					fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSize);
				}

				// FFT
				fftw_execute(fftw_p);

				// 指定周波数より低い周波数はカットする
				for (i = 1;i < fftSize / 2;i++) {
					hz = ((double)outSampleR / fftSize) * i;
					if (hz <= sideHFC) {
						fftw_out[i][0] = 0;
						fftw_out[i][1] = 0;
					}
				}
				// Side2 の PW を調べる
				pwCount = 0;
				for (i = 1;i < fftSize / 2;i++) {
					hz = ((double)outSampleR / fftSize) * i;
					if (hz >= hfc && hz < hfc + 1000) {
						pp = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
						if (pp != 0) {
							pp = sqrt(pp);
						}
						side2Pw += pp;
						pwCount++;
					}
				}
				if (pwCount > 0) {
					side2Pw /= pwCount;
				}

				if (side2Pw > 0) {
					side2Pw = side1Pw / side2Pw;
					side2Pw *= 0.8;
				}
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[i][0] = fftw_out[i][0] * side2Pw;
					fftw_out[i][1] = fftw_out[i][1] * side2Pw;
				}
				// 半分のデータを復元
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[fftSize - i][0] = fftw_out[i][0];
					fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
				}
				fftw_out[0][0] = 0;
				fftw_out[0][1] = 0;

				// invert FFT
				fftw_execute(fftw_ip);

				// 出力
				pOut = (SSIZE *)mem4;
				for (i = 0;i < fftSize;i++) {
					pOut[((fftSize / 2) * nn) + i] += fftw_in[i][0] / fftSize;
				}
			}

			pIn = mem1;

			// FFT 初期設定
			for (i = 0;i < fftSize;i++) {
				fftw_in[i][0] = pIn[((fftSize / 2) * nn) + i];
				fftw_in[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSize - 1) / 2;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSize);
			}
			for (i = (fftSize - 1) / 2;i < fftSize;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSize);
			}

			// FFT
			fftw_execute(fftw_p);

			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				if (hz <= 20000) {
					pp = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
					if (pp != 0) {
						pp = sqrt(pp);
					}
					hz = hz / 200;
					midPw[hz] += pp;
					midPw_Count[hz]++;
				}
			}

			for (i = 0;i < 100;i++) {
				if (midPw_Count[i] > 0) {
					midPw[i] = midPw[i] / midPw_Count[i];
				}
			}

			// 量子化誤差部分の上限カット周波数を求める
			for (i = 100 - 1;i >= 20;i--) {
				if (midPw[i] > hfc_auto_shresh) break;
			}
			if (i * 200 < limitHFC) {
				midHFC = i * 200;
			}
			sprintf(sss,"midHFC:%ld",midHFC);
			PRINT_LOG(sss);

			// 指定周波数より高い周波数はカットする
			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				if (hz >= midHFC) {
					fftw_out[i][0] = 0;
					fftw_out[i][1] = 0;
				}
			}

			// 半分のデータを復元
			for (i = 1;i < fftSize / 2;i++) {
				fftw_out[fftSize - i][0] = fftw_out[i][0];
				fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
			}
			fftw_out[0][0] = 0;
			fftw_out[0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip);

			// 出力
			pOut = (SSIZE *)mem5;
			for (i = 0;i < fftSize;i++) {
				pOut[((fftSize / 2) * nn) + i] += fftw_in[i][0] / fftSize;
			}
		}

		// Side
		if (startInSample + fftSize / 2 >= 0) {
			for (i = 0;i < nSample;i++) {
				mem3[i] = mem3[i] + mem4[i];
			}
			outTempFile_(fp_w2,mem3 + fftSize / 2,fftSize,param);
		}

		// Mid
		if (startInSample + fftSize / 2 >= 0) {
			outTempFile_(fp_w1,mem5 + fftSize / 2,fftSize,param);
		}

		if (fp_ms4 != NULL && p->ms_process == 4) {
			int disable_count = 7;
			int enable_flag   = 1;
			if (midHFC > sideHFC) {
				if (midHFC > 16000) {
					midHFC = 16000;
				}
				fprintf(fp_ms4,"sfc:%lld,%ld\n",startInSample / outSampleR,midHFC);
			} else {
				if (sideHFC > 16000) {
					sideHFC = 16000;
				}
				fprintf(fp_ms4,"sfc:%lld,%ld\n",startInSample / outSampleR,sideHFC);
			}
#if 1
			for (i = 0;i < 200;i++) {
				if (pwCutFlag[i] > 3 && i * 100 <= sideHFC) {
					SSIZE ms4_time;
					if (enable_flag == 1 && disable_count > 0) {
						fprintf(fp_ms4,"%lld,%ld,%ld\n",startInSample / outSampleR,i * 100,100);
//						disable_count--;
						if (disable_count <= 0) {
							enable_flag = 0;
						}
					}
				}
				if (enable_flag == 0) {
					if (pwCutFlag[i] < 2) {
						disable_count++;
					}
					if (disable_count >= 3) {
						disable_count = 7;
						enable_flag   = 1;
					}
				}
			}
#endif
			if (midHFC > sideHFC && midHFC < upper_hfc) {
				fprintf(fp_ms4,"%lld,%ld,%ld\n",startInSample / outSampleR,midHFC - 250,((upper_hfc / 100) - (midHFC / 100)) * 100);
			} else if (sideHFC < upper_hfc) {
				fprintf(fp_ms4,"%lld,%ld,%ld\n",startInSample / outSampleR,sideHFC - 250,((upper_hfc / 100) - (sideHFC / 100)) * 100);
			}
		}
	}
	free(mem1);
	free(mem2);
	free(mem3);
	free(mem4);
	free(mem5);
	free(mem6);

	fftw_destroy_plan(fftw_p_2);
	fftw_destroy_plan(fftw_ip_2);
	fftw_free(fftw_in_2);
	fftw_free(fftw_out_2);

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);

	if (fp_ms4 != NULL) {
		fclose(fp_ms4);
	}

	for (i = 0;i < 128;i++) {
		if (dbg1[i] != 0x3F) break;
		if (dbg2[i] != 0x3F) break;
	}
	if (i < 128) {
		exit(0);
	}


	PRINT_FN_LOG(param->debug_upconv,"ms_process_2:End");
}

//---------------------------------------------------------------------------
// Function   : ms_process_3
// Description: Mid/SideをLeft/Rightに戻す処理を実施する。
// ---
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r1		:入力ファイル(Mid)
//	fp_r2		:入力ファイル(Side)
//	fp_w1		:出力ファイル(Left)
//	fp_w2		:出力ファイル(Right)
//	param		:変換パラメータ
//
void ms_process_3(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w1,FIO *fp_w2,PARAM_INFO_TO_RAW *param,UPCONV_PARAM *p)
{
	SSIZE i,n;
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4,*mem5;
	SSIZE valueL,valueR;
	long wkMemSize;
	SSIZE *pIn,*pOut;
	SSIZE outRemain;
	long fftSizeIn,fftSizeOut;
	SSIZE startInSample,nSample;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;
	double percent,per,nx1,nx2;
	SSIZE maxR1,minR1,maxR2,minR2;
	SSIZE L_maxLv,L_maxLv2;
	SSIZE R_maxLv,R_maxLv2;
	SSIZE L_ns,R_ns;
	SSIZE L_norm,R_norm;
	SSIZE L_normTmp,R_normTmp;
	char fname_rw1[MAX_PATH];
	char fname_rw2[MAX_PATH];
	FIO fp_rw1;
	FIO fp_rw2;
	FIO_COMMON_PARAM fio_common;

	memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));
	if (p->fio_sec_size > 0) {
		// 入力のn秒分
		fio_common.sector_size = p->outSampleR_final * sizeof (SSIZE);
		fio_common.sector_n    = p->fio_sec_size;
		fio_common.upconv_n    = 1;
	} else {
		// n MB 分
		fio_common.sector_size = (fio_size)1 * 1000 * 1000;
		fio_common.sector_n    = p->fio_mb_size;
		fio_common.upconv_n    = 1;
	}

	PRINT_FN_LOG(param->debug_upconv,"ms_process_3:Start");
	L_maxLv = L_maxLv2 = 0;
	R_maxLv = R_maxLv2 = 0;
	L_ns = R_ns = 0;

	fio_rewind(fp_r1);
	fio_rewind(fp_r2);
	strcpy(fname_rw1,fp_r1->fname);
	strcat(fname_rw1,".tmp_rw1");
	strcpy(fname_rw2,fp_r2->fname);
	strcat(fname_rw2,".tmp_rw2");
	fio_common.upconv_n    = 2;
	//fio_open(&fp_rw1,fname_rw1,FIO_MODE_W);
	//fio_open(&fp_rw2,fname_rw2,FIO_MODE_W);
	fio_open_with_set_param(&fp_rw1,fname_rw1,FIO_MODE_W,&fio_common);
	fio_open_with_set_param(&fp_rw2,fname_rw2,FIO_MODE_W,&fio_common);
	if (fp_rw1.error) {
		param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
		return;
	}
	if (fp_rw2.error) {
		param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
		return;
	}

	fftSizeIn  = p->inSampleR * 2 * 2;
	fftSizeOut = p->inSampleR * 2;
	wkMemSize = (fftSizeIn * 2) * sizeof (SSIZE);
	outRemain = inSample / 2;

	maxR1 = minR1 = maxR2 = minR2 = 0;
	mem1 = (SSIZE *)malloc(wkMemSize);
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem2 = (SSIZE *)malloc(wkMemSize);
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem3 = (SSIZE *)malloc(wkMemSize);
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem4 = (SSIZE *)malloc(wkMemSize);
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem5 = (SSIZE *)malloc(wkMemSize);
	if (mem5 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_out == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p = fftw_plan_dft_1d(fftSizeIn,fftw_in,fftw_out,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip = fftw_plan_dft_1d(fftSizeOut,fftw_out,fftw_in,FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	per = -1;
	for (startInSample = ((fftSizeIn + (fftSizeIn / 2)) * -1);startInSample < inSample + (fftSizeIn + fftSizeIn / 2);startInSample += fftSizeIn) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
				fflush(stdout);
			}
			per = percent;
		}
		memset(mem1,0,wkMemSize);
		memset(mem2,0,wkMemSize);
		memset(mem3,0,wkMemSize);
		memset(mem4,0,wkMemSize);
		memset(mem5,0,wkMemSize);

		if (startInSample >= 0 && startInSample + (fftSizeIn * 2) < inSample) {
			fio_seek(fp_r1,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_seek(fp_r2,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSizeIn * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r1);
			fio_read(mem2,sizeof (SSIZE),nSample,fp_r2);
		} else {
			mem0 = mem1;
			nSample = fftSizeIn * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r1,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r1,0,SEEK_SET);
				mem0 += (startInSample * -1);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r1);
			}
			mem0 = mem2;
			nSample = fftSizeIn * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r2,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r2,0,SEEK_SET);
				mem0 += (startInSample * -1);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r2);
			}
			nSample = fftSizeIn * 2;
		}
		if (fp_r1->error) break;
		if (fp_r2->error) break;

		for (n = 0;n < 3;n++) {
			pIn = mem1;
			// FFT 初期設定
			for (i = 0;i < fftSizeIn;i++) {
				fftw_in[i][0] = pIn[((fftSizeIn / 2) * n) + i];
				fftw_in[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSizeIn - 1) / 2;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSizeIn);
			}
			for (i = (fftSizeIn - 1) / 2;i < fftSizeIn;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSizeIn);
			}

			// FFT
			fftw_execute(fftw_p);

			// ダウンサンプリング
			for (i = fftSizeOut / 2;i < fftSizeIn;i++) {
				fftw_out[i][0] = 0;
				fftw_out[i][1] = 0;
			}
			// 半分のデータを復元
			for (i = 1;i < fftSizeOut / 2;i++) {
				fftw_out[fftSizeOut - i][0] = fftw_out[i][0];
				fftw_out[fftSizeOut - i][1] = fftw_out[i][1] * -1;
			}
			fftw_out[0][0] = 0;
			fftw_out[0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip);

			// 出力
			pOut = (SSIZE *)mem5;
			for (i = 0;i < fftSizeOut;i++) {
				pOut[((fftSizeOut / 2) * n) + i] += fftw_in[i][0] / fftSizeOut;
			}
		}
		if (startInSample + fftSizeIn / 2 >= 0) {
			memcpy(mem1,mem5 + fftSizeOut / 2,fftSizeOut * sizeof (SSIZE));
		}

		memset(mem5,0,wkMemSize);

		for (n = 0;n < 3;n++) {
			pIn = mem2;
			// FFT 初期設定
			for (i = 0;i < fftSizeIn;i++) {
				fftw_in[i][0] = pIn[((fftSizeIn / 2) * n) + i];
				fftw_in[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSizeIn - 1) / 2;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSizeIn);
			}
			for (i = (fftSizeIn - 1) / 2;i < fftSizeIn;i++) {
				fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSizeIn);
			}

			// FFT
			fftw_execute(fftw_p);

			// ダウンサンプリング
			for (i = fftSizeOut / 2;i < fftSizeIn;i++) {
				fftw_out[i][0] = 0;
				fftw_out[i][1] = 0;
			}
			// 半分のデータを復元
			for (i = 1;i < fftSizeOut / 2;i++) {
				fftw_out[fftSizeOut - i][0] = fftw_out[i][0];
				fftw_out[fftSizeOut - i][1] = fftw_out[i][1] * -1;
			}
			fftw_out[0][0] = 0;
			fftw_out[0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip);

			// 出力
			pOut = (SSIZE *)mem5;
			for (i = 0;i < fftSizeOut;i++) {
				pOut[((fftSizeOut / 2) * n) + i] += fftw_in[i][0] / fftSizeOut;
			}
		}

		if (startInSample + fftSizeIn / 2 >= 0) {
			memcpy(mem2,mem5 + fftSizeOut / 2,fftSizeOut * sizeof (SSIZE));
		}

		if (p->sfa == 0) {
			for (i = 0;i < fftSizeOut;i++) {
				valueL = (mem1[i] + mem2[i]);
				if (valueL > maxR1) maxR1 = valueL;
				if (valueL < minR1) minR1 = valueL;
				if ((valueL >> 40) > 0) {
					L_maxLv2 += (valueL >> 40);
					L_ns++;
					if (L_maxLv2 >= 0x1000000000000) {
						L_maxLv2 /= L_ns;
						if (L_maxLv > 0) {
							L_maxLv = (L_maxLv + L_maxLv2) / 2;
						} else {
							L_maxLv = L_maxLv2;
						}
						L_maxLv2 = 0;
						L_ns = 0;
					}
				}
				mem3[i] = valueL;
				valueR = (mem1[i] - mem2[i]);
				if (valueR > maxR2) maxR2 = valueR;
				if (valueR < minR2) minR2 = valueR;
				if ((valueR >> 40) > 0) {
					R_maxLv2 += (valueR >> 40);
					R_ns++;
					if (R_maxLv2 >= 0x1000000000000) {
						R_maxLv2 /= R_ns;
						if (R_maxLv > 0) {
							R_maxLv = (R_maxLv + R_maxLv2) / 2;
						} else {
							R_maxLv = R_maxLv2;
						}
						R_maxLv2 = 0;
						R_ns = 0;
					}
				}
				mem4[i] = valueR;
			}
		} else {
			for (i = 0;i < fftSizeOut;i++) {
				valueL = mem1[i];
				if (valueL > maxR1) maxR1 = valueL;
				if (valueL < minR1) minR1 = valueL;
				if ((valueL >> 40) > 0) {
					L_maxLv2 += (valueL >> 40);
					L_ns++;
					if (L_maxLv2 >= 0x1000000000000) {
						L_maxLv2 /= L_ns;
						if (L_maxLv > 0) {
							L_maxLv = (L_maxLv + L_maxLv2) / 2;
						} else {
							L_maxLv = L_maxLv2;
						}
						L_maxLv2 = 0;
						L_ns = 0;
					}
				}
				mem3[i] = valueL;
				valueR = mem2[i];
				if (valueR > maxR2) maxR2 = valueR;
				if (valueR < minR2) minR2 = valueR;
				if ((valueR >> 40) > 0) {
					R_maxLv2 += (valueR >> 40);
					R_ns++;
					if (R_maxLv2 >= 0x1000000000000) {
						R_maxLv2 /= R_ns;
						if (R_maxLv > 0) {
							R_maxLv = (R_maxLv + R_maxLv2) / 2;
						} else {
							R_maxLv = R_maxLv2;
						}
						R_maxLv2 = 0;
						R_ns = 0;
					}
				}
				mem4[i] = valueR;
			}
		}
		if (startInSample + fftSizeIn / 2 >= 0) {
			if (outRemain >= fftSizeOut) {
				fio_write(mem3,sizeof (SSIZE),fftSizeOut,&fp_rw1);
				fio_write(mem4,sizeof (SSIZE),fftSizeOut,&fp_rw2);
				outRemain -= fftSizeOut;
			} else {
				fio_write(mem3,sizeof (SSIZE),outRemain,&fp_rw1);
				fio_write(mem4,sizeof (SSIZE),outRemain,&fp_rw2);
				outRemain = 0;
			}
			if (fp_rw1.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_rw2.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
		}
	}
	if (L_ns > 0) {
		L_maxLv2 /= L_ns;
	}
	if (L_maxLv2 > 0) {
		L_maxLv = (L_maxLv + L_maxLv2) / 2;
	} else {
		L_maxLv = L_maxLv2;
	}
	if (R_ns > 0) {
		R_maxLv2 /= R_ns;
	}
	if (R_maxLv2 > 0) {
		R_maxLv = (R_maxLv + R_maxLv2) / 2;
	} else {
		R_maxLv = R_maxLv2;
	}
	L_normTmp = NormInfoWav2.tmpLv1;
	R_normTmp = NormInfoWav2.tmpLv2;
	if (NormInfoWav2.cntLv1 > 0) {
		L_normTmp /= NormInfoWav2.cntLv1;
	}
	if (NormInfoWav2.cntLv2 > 0) {
		R_normTmp /= NormInfoWav2.cntLv2;
	}
	if (NormInfoWav2.maxLv1 > 0) {
		L_norm  = (NormInfoWav2.maxLv1 + L_normTmp) / 2;
	} else {
		L_norm  = L_normTmp;
	}
	if (NormInfoWav2.maxLv2 > 0) {
		R_norm  = (NormInfoWav2.maxLv2 + R_normTmp) / 2;
	} else {
		R_norm  = R_normTmp;
	}
	nx1 = 1;
	if (maxR1 < (minR1 * -1)) maxR1 = minR1 * -1;
	if (maxR1 != 0) nx1 = (double)NormInfoWav2.maxR1 / (double)maxR1;
	if (L_norm > 0) nx1 = (double)L_norm / (double)L_maxLv;
	nx2 = 1;
	if (maxR2 < (minR2 * -1)) maxR2 = minR2 * -1;
	if (maxR2 != 0) nx2 = (double)NormInfoWav2.maxR2 / (double)maxR2;
	if (R_norm > 0) nx2 = (double)R_norm / (double)R_maxLv;
	nx1 = nx1 * 0.45;
	nx2 = nx2 * 0.45;

	if (1) {
		char s[100];
		sprintf(s,"process3 nx:%lf,%lf",nx1,nx2);
		PRINT_LOG(s);
	}

	fio_close(&fp_rw1);
	fio_close(&fp_rw2);
	//fio_open(&fp_rw1,fname_rw1,FIO_MODE_R);
	//fio_open(&fp_rw2,fname_rw2,FIO_MODE_R);
	fio_open_with_set_param(&fp_rw1,fname_rw1,FIO_MODE_R,&fio_common);
	fio_open_with_set_param(&fp_rw2,fname_rw2,FIO_MODE_R,&fio_common);
	if (fp_rw1.error) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		unlink(fname_rw1);
		unlink(fname_rw2);
		return;
	}
	if (fp_rw2.error) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		unlink(fname_rw1);
		unlink(fname_rw2);
		return;
	}

	fio_rewind(&fp_rw1);
	fio_rewind(&fp_rw2);
	per = -1;
	for (startInSample = 0;startInSample < inSample;startInSample += p->inSampleR) {
		percent = ((double)startInSample / inSample);
		percent *= 100;
		percent = (double)((int)percent);
		if (percent != per) {
			fprintf(stdout,"%d%%\n",(int)percent);
			fflush(stdout);
		}
		per = percent;

		memset(mem1,0,wkMemSize);
		memset(mem2,0,wkMemSize);
		memset(mem3,0,wkMemSize);
		memset(mem4,0,wkMemSize);

		if (startInSample + (p->inSampleR) <= inSample) {
			nSample = p->inSampleR;
		} else {
			nSample = inSample - startInSample;
		}
		fio_seek(&fp_rw1,startInSample * sizeof (SSIZE),SEEK_SET);
		fio_seek(&fp_rw2,startInSample * sizeof (SSIZE),SEEK_SET);
		fio_read(mem1,sizeof (SSIZE),nSample,&fp_rw1);
		fio_read(mem2,sizeof (SSIZE),nSample,&fp_rw2);
		//fio_seek(fp_w1,startInSample * sizeof (SSIZE),SEEK_SET);
		//fio_seek(fp_w2,startInSample * sizeof (SSIZE),SEEK_SET);
		//if (fp_w1->error) break;
		//if (fp_w2->error) break;
		for (i = 0;i < nSample;i++) {
			valueL = mem1[i];
			mem3[i] = (SSIZE)((double)valueL * nx1);
			valueR = mem2[i];
			mem4[i] = (SSIZE)((double)valueR * nx2);
		}
		fio_write(mem3,sizeof (SSIZE),nSample,fp_w1);
		fio_write(mem4,sizeof (SSIZE),nSample,fp_w2);
		if (fp_w1->error) break;
		if (fp_w2->error) break;
	}
	fio_close(&fp_rw1);
	fio_close(&fp_rw2);
	unlink(fname_rw1);
	unlink(fname_rw2);

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);

	free(mem1);
	free(mem2);
	free(mem3);
	free(mem4);
	free(mem5);
	PRINT_FN_LOG(param->debug_upconv,"ms_process_3:End");
}
//---------------------------------------------------------------------------
// Function   : outTempFile_
// Description: データをテンポラリファイルへ出力する
// ---
//	fp_w	:出力ファイル
//	in		:データのアドレス
//	size	:データー数
//	param	:パラメーター
//
static void outTempFile_(FIO *fp_w,void *in,SSIZE size,PARAM_INFO_TO_RAW *param)
{
	fio_size r;

	r = fio_write(in,1,size * sizeof (SSIZE),fp_w);
	if (r != size * sizeof (SSIZE)) {
		param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
	}
}

//---------------------------------------------------------------------------
// Function   : copyFile
// Description: 処理する音声ファイルを後処理のためにバックアップする
// ---
//	fp_r	:入力ファイル
//	fp_w	:出力ファイル
//	in		:データのアドレス
//	buffer_size : バッファのサイズ
//
static SSIZE copyFile(FIO *fp_r,FIO *fp_w,void *in,SSIZE buffer_size)
{
	fio_size r,w;
	SSIZE ret_size;
	
	ret_size = 0;

	r = fio_read(in,sizeof (SSIZE),buffer_size,fp_r);
	if (r == buffer_size) {
		w = fio_write(in,sizeof (SSIZE),buffer_size,fp_w);
		if (w == buffer_size) {
			ret_size = buffer_size;
		}
	}
	return ret_size;
}

