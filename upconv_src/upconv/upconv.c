//---------------------------------------------------------------------------
/****************************************************************************/
/* upconv(C) 2007-2024 By 59414d41											*/
/*																			*/
/*																			*/
/****************************************************************************/

/*--- Log ------------------------------------------------------------------
 * Ver 0.10 <07/03/15> - 作成
 * Ver 0.20 <07/04/19> - DFT バージョン追加
 * Ver 0.30 <08/08/17> - 新方式に変更
 * Ver 0.40 <09/02/20> - fftw バージョンに変更しいったんFix
 * Ver 0.50 <09/05/18> - 高域の補間方法をいったんFix
 * Ver 0.60 <09/06/02> - BCB6とVC6に対応
 * Ver 0.61 <09/06/06> - メモリマップドファイルを使用しないようにした
 * Ver 0.70 <09/06/28> - 処理方法を変更
 * Ver 0.80 <09/07/10> - hfa2 の処理を改良しマルチコアに対応するために処理を分離
 * Ver 0.90 <09/09/24> - メモリ使用量削減
 * Ver 0.91 <09/09/27> - ダウンサンプリング時のバグ修正、ノーマライズ時のバグ修正
 * Ver 0.92 <09/09/29> - hfa2の補間方法を変更
 * Ver 0.93 <09/10/06> - hfa2の補間処理を変更
 * Ver 0.94 <09/11/01> - hfa3追加、バグ修正、パラメータファイルの採用
 * Ver 0.95 <09/11/11> - hfa3にノイズとのブレンド具合を指定可能にした
 * Ver 0.96 <09/11/15> - hfa補間時の周波数特性を修正
 * Ver 0.97 <09/11/22> - ビット拡張処理追加、hfa補間時の周波数特性を指定可能にした
 * Ver 0.98 <10/01/11> - デエンファシスに対応
 * Ver 0.99 <10/03/14> - hfa3の補間方法を変更
 * Ver 1.00 <10/03/14> - GCC(MinGW)に対応
 * Ver 1.01 <10/04/11> - OpenMPに対応
 * Ver 1.02 <10/07/26> - スピーカーごとの調整機能追加
 * Ver 1.03 <10/09/14> - hfc autoに対応
 * Ver 1.04 <10/11/02> - スピーカーごとの調整機能バグ修正
 * Ver 1.05 <10/12/27> - イコライザ機能修正
 * Ver 1.06 <11/01/07> - lfa 対応、ソースコードの整理
 * Ver 1.07 <11/10/01> - テンポラリファイル対応
 * Ver 1.08 <12/02/28> - fio 対応
 * Ver 1.09 <13/04/27> - hfa3の補間方法変更
 * Ver 1.10 <18/10/06> - サンプリングレート追加
 * Ver 1.20 <19/10/12> - いろいろな改良
 * Ver 1.21 <20/05/17> - 呼び出し方法を修正
 * Ver 1.22 <20/07/02> - 音量の調整処理を追加
 * Ver 1.23 <21/01/30> - 0.8.4用に機能修正
 * Ver 1.24 <21/01/30> - イコライザ用に機能追加
 * Ver 1.25 <21/11/23> - ログファイル出力用にパラメータ追加
                         中断時のファイル監視間隔を変更可能にするための対応
 * Ver 1.26 <21/12/20> - パラメータまわりの処理修正。
 *                     - Windows 固有の処理を分離
 *          <22/05/04> - Left / Right / Center を分けて処理する仕組みを追加
 * Ver 1.27 <22/09/17> - 音量関連の処理追加・修正
 * Ver 1.13 <24/11/09> - ソースコードの整理と音量維持の方法修正(バージョンをupconvfeに合わせた)

 */

// [仕様]
// upconv.exe 入力ファイル名 出力ファイル名 デフォルトパラメータファイル名 パラメーター
//
// [作業用ファイル]
// ◇ 出力ファイル名.param
// 1行目は元のwavファイル or 元のmp3ファイル or flacファイル or wavpackファイルの名前
// ...

// ◇ 出力ファイル名.files
// 作業用に作成したファイル名
// ...

// ◇ 出力ファイル名.level
// 変換前、変換後の音量レベルを記載したファイル
// ...

// ◇ 出力ファイル名.r1.NNN
// 変換中の1Ch分のデータ(64bit,有効56bitのrawデータ)
// ...

// ◇出力ファイル名.r1.NNN.param
// 1Ch分の変換前データのノーマライズ用音量データ
// 1Ch分の変換後データのノーマライズ用音量データ

#define STR_COPYRIGHT	"upconv.exe (c) 2024 Ver 1.0.x By 59414d41\n\n"
#define STR_USAGE		"upconv.exe\tin-file out-file def_paramfile parameter logfile\t\tin-file out-file parameter"

#define CLIP_NX(v,nx)	(v) == 0 ? ((SSIZE)0) : \
							((v) > 0 ?	\
								(((SSIZE)(v) * nx) >= (SSIZE)(0x007FFFFFFFFFFFFFF) ?		(SSIZE)(0x7FFFFFFFFFFFFEFF)		: ((SSIZE)((v) * nx))) : \
								(((SSIZE)(v) * nx) <= ((SSIZE)0x007FFFFFFFFFFFFFF * -1) ? ((SSIZE)0x7FFFFFFFFFFFFFF * -1) : ((SSIZE)((v) * nx))))
								
#define PEAK_DIFF_THRESH	(1000)
#define FAST_DIFF_PW_THRESH	((double)200033560873692)
#define DIFF_PW_CUT_THRESH	(30)

//#define NOCLEANUP

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#define _WIN_CP	(1)
#endif
#include <unistd.h>
#include <math.h>
#include<gsl/gsl_wavelet.h>
#include <time.h>
#include "upconv.h"
#include "start_exec.h"
#include "fileio.h"
#include "fftw3.h"
#include "fft_filter.h"
#include "./../PLG_AUDIO_IO/PLG_AudioIO.h"

#ifdef _OPENMP
#include <omp.h>
#endif

typedef struct {
	long cutOff[257];
} BITEXT_INFO;

typedef struct {
	int process_id;
	int upconv;
	int r_nnn;
	int print_stdout;
	int err;
	int errLine;
	int disRc;
	int disEq;
	int channel_count;
	char timecode[20];
	char pr_str[40];
	double *pre_eq;
	double *post_eq;
	double *post_eq2;
	long fio;
	long fio_auto;
	int Adjust_enable;
	int hfaDiffMin;
	int hfa_analysis_limit;
	// 
	BITEXT_INFO beInfo;
	char tempPath[4];
	int downSample;
	int ana;
	int    eq_pwcount;
	long   ana_avgcount;
	double *declip_eq;
	double *hfa_eq;
	double *analyze_eq_pw;
	char *sp_path;
	int r1_flag;
	int eq_flag;
	int enable_sfa;
	int enable_filter;
	int pwAdj;
	char debug_upconv[10];
	double max_level2;
	int debug_print_disable;
	int hfa3_sig1_print_disable;
	int hfa3_sig2_print_disable;
	int hfa3_sig3_print_disable;
	char *argv2;
	SSIZE diff_max[10];
	SSIZE step_max[10];
	SSIZE outSample;
	SSIZE maxMemorySize;
	UPCONV_PARAM *p;
	NORM_INFO *normInfo;
	NORM_INFO *normInfoOrg;
	NORM_INFO *normInfoFinal;
	ABORT_INFO *abortInfo;
} PARAM_INFO;

typedef struct {
	double	power[76800*4];
	double	power_org[76800*4];
	double *power_ptr;
	double	phase[76800*4];
	double	pw[76800*4];
	double	diff[76800*4];
	double	avg[76800*4];
	double  from_avg;
	double  to_avg;
	double  from_top10[10];
	double  to_top10[10];
	int		peak[76800*4];
	double	base[76800*4];
	double	baseToAdj[76800*4];
	double  cp_analyze[76800*4];
	double	sfa_base[76800*4];
	double	hfa4_coeff[76800*4];
	double  hfa4_lag[76800*4];
	size_t	hfa4_index[76800*4];
	SSIZE	startInSample;
	int		base_freq;
	int		sign[76800*4];
	int		pw_cnt[76800*4];
	int		sfa_level[76800*4];
	int		nSample;
	int		validSamplingRate;
	long	samplingRate;
	long	sfa_sec;
	long	sfa_hfc;
	long	fftSize;
	long	fftFastSize;
	int		do2Idx[360];
	double	*phaseX;
	double	*phaseY;
	int log;
	int hfa3_log;
	int index;
	int kp_flag;
	int fast_index;
} OVERTONE_INFO;

typedef struct {
	long    pw_0_count[20*10];
	double  pw_0[20*10];
	double  pw_1[20*10];
	double  pw_2[20*10];
	double  pw_3[20*10];
	double  pw_4[20*10];
	double  pw_5[20*10];
	double  pw_6[20*10];
	double  pw_7[20*10];
	double  pw_8[20*10];
	double  pw_9[20*10];
	double  pw_10[20*10];
	double  pw_11[20*10];
	double  pw_12[20*10];
	double  pw_13[20*10];
	double  pw_14[20*10];
	double  pw_15[20*10];
	double  pw_16[20*10];
	double  pw_17[20*10];
	double  pw_18[20*10];
	double  pw_19[20*10];
	double  pw_20[20*10];
	double  pw_21[20*10];
	double  pw_22[20*10];
	double  pw_23[20*10];
	double  pw_24[20*10];
	double  pw_25[20*10];
	double  pw_26[20*10];
	double  pw_27[20*10];
	double  pw_28[20*10];
	double  pw_29[20*10];
	double	power[76800*4];
	double	base[76800*4];
} SFA_INFO;

// 1つのファイルを分割して変換するための情報
// 分割しない場合でも使用する
typedef struct {
	int		state;
	int		r_nnn;
	STARTEXEC_PROCESS_INFO sp_info;
} STARTEXEC_INFO;

typedef struct {
	int *done_flag;
	STARTEXEC_INFO startexec_info[24];
} QUEUE_INFO;

LEVEL_INFO Wav2RawLevelInfo;
LEVEL_INFO Raw2WavLevelInfo;
LEVEL_INFO Raw2WavLevelInfo2;
SSIZE *diskBuffer1;
SSIZE *diskBuffer2;
char log_filename[_MAX_PATH];
int enable_upconv_log;
ABORT_INFO abortInfo;

#ifdef UPCONV_LOG
#define	PRINT_LOG(timecode,s)	do {																			\
									FILE *log;																	\
									if (enable_upconv_log) {													\
										log = fopen(log_filename,"a");											\
										if (log) {																\
											fprintf(log,"%s [%d] %s:%s\n",__FUNCTION__,__LINE__,timecode,s);	\
											fclose(log);														\
										}																		\
									}																			\
								} while (0)

#define	PRINT_FN_LOG(pn,s)		do {																			\
									FILE *log;																	\
									if (enable_upconv_log) {													\
										log = fopen(log_filename,"a");											\
										if (log) {																\
											fprintf(log,"[%05d]%s:%s\n",__LINE__,pn,s);							\
											fclose(log);														\
										}																		\
									}																			\
								} while (0)

#define PRINT_LINE(s)			do {																			\
									printf("%d:%s\n",__LINE__,s);												\
									fflush(stdout);																\
								} while (0)

#else
#define	PRINT_LOG(s,s2)		//
#define	PRINT_FN_LOG(s,s2)	//
#define PRINT_LINE(s)		//
#endif

extern long beinfo_init[257];
extern double declip_eq_init[];
extern double hfa_eq_init[];
/*--- wav2raw Function Prototype ---------------------------------------------------*/
extern int to_raw_main(int argc, char *argv[],UPCONV_PARAM *p);
extern int dsf_main(int argc, char *argv[]);
extern int to_wav_main(int argc, char *argv[],UPCONV_PARAM *p);

//extern pid_t fork(void);

/*--- Function Prototype ---------------------------------------------------*/
int get_next_r_nnn(int *done_flag,int r_nnn_count);
void UpconvProcessControl(PARAM_INFO *param);
void SamplingRateConvert(int argc,char *argv[],char *rawFile,PARAM_INFO *param);
void anaLFA_Param(FFT_PARAM *param);
void anaHFC_AutoParam(FFT_PARAM *param);
void spAnalyze(SSIZE inSample,FIO *fp,PARAM_INFO *param);
void SideFreqAdjust(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w,PARAM_INFO *param,FFT_CUTINFO *cutinfo);
void abe_deClipNoise(SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void abe_deClickNoise(SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void adjBitExtension(SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void adjBitExtension_0774(SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void adjBitExtension_086(SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void adjBitExtension_10x(SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void genNoise(long hfc,SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void genOverTone(long hfc,SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param,FFT_PARAM *fft_param);
void genOverToneSub(long hfc,SSIZE *pIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,fftw_complex *fftw_fast_in,fftw_complex *fftw_fast_out,fftw_plan fftw_fast_p,fftw_plan fftw_fast_ip,OVERTONE_INFO *ovInfo,PARAM_INFO *param,FFT_PARAM *fft_param);

void noiseCut(long nfs,SSIZE inSample,FIO *fio_r,FIO *fio_w,PARAM_INFO *param);
void anaOverToneHFA2(OVERTONE_INFO *ovInfo,PARAM_INFO *param);
void anaOverToneHFA3(OVERTONE_INFO *ovInfo,PARAM_INFO *param);
void anaOverToneHFA4(OVERTONE_INFO *ovInfo,PARAM_INFO *param);
double hfa4_calculate_mean(double *data,int n);
double hfa4_autocorrelation_coefficient(double *data,int n,int lag);
void outTempFile(FIO *fio,void *in,SSIZE size,PARAM_INFO *param);
double normalNoise(void);
void adjPinkFilter(int mode,long fftSizeOut,fftw_complex *fftw_out2,PARAM_INFO *param);
void adjPinkFilter_0774(int mode,long fftSizeOut,fftw_complex *fftw_out2,PARAM_INFO *param);
void adjVLA(SSIZE inSample,FIO *fio_r,double u_nx,double nx,double *clip_rate,PARAM_INFO *param);
void adjClickNoise(SSIZE inSample,FIO *fp_r,FIO *fp_w,double u_nx,PARAM_INFO *param);
void merageTempFile(char type,int normFlag,FIO *in1,FIO *in2,FIO *out,SSIZE inSample,PARAM_INFO *param);
void calcNormInfo(FIO *fp_r,SSIZE inSample,PARAM_INFO *param,NORM_INFO *normInfo);
static void volumeAdjustFile(double nx,FIO *in,FIO *out,SSIZE inSample,PARAM_INFO *param,NORM_INFO *normInfo);
void copyToFFTW(fftw_complex *fftw,SSIZE *buf,long size);
void windowFFTW(fftw_complex *fftw,long size);
void cutFFTW(fftw_complex *fftw,long index,long size);
int chkAbort(int percent,int diff);
int file_exists(char *fn);
int log10_lf(double n);
void get_time_string(char *str);
double generate1fNoise(double prevValue);
#if defined(__APPLE__)
extern void _splitpath(const char *path,char *drive,char *dir,char *fname,char *ext);
extern void _makepath(char *path,const char *drive,const char *dir,const char *fname,const char *ext);
#endif

//---------------------------------------------------------------------------
// Function   : upconv_main
// Description: upconv メイン関数
//
//
int main(int argc, char *argv[])
{
	FILE *fp;
	FILE *fp_files;
	static char workpath[_MAX_PATH];
	static char workstr[_MAX_PATH];
	static char tmppath[_MAX_PATH];
	static char workdrive[_MAX_DRIVE];
	static char workdir[_MAX_DIR];
	static char workfname[_MAX_FNAME];
	static char workext[_MAX_EXT];
	static char drive[_MAX_DRIVE];
	static char dir[_MAX_DIR];
	static char fname[_MAX_FNAME];
	static char ext[_MAX_EXT];
	static char work[3072];
	static char pparam[3072];
	char ddBuf[100];
	char sss[2058];
	long paramFlag;
	char *rn_fname;
	char *p1,*p2;
	double dd;
	double temp_f;
	long i;
	UPCONV_PARAM UpconvParam;		// Upconv コマンドラインパラメーター用構造体
	STARTEXEC_PROCESS_INFO sp_info[6*24];
	NORM_INFO NormInfo;				// 基本的に音量調査はupconvで実施する(wav2rawの後)
	NORM_INFO NormInfoOrg;			// upconvでは入力ファイルの音量と同じにする
	NORM_INFO NormInfoFinal;		// ノーマライズはraw2wavで実施
	SOUNDFMT inFmt;
	SSIZE inSample;
	SSIZE r1_size,r2_size,r3_size,r4_size,r5_size,r6_size;
	FIO   fp_r1,fp_r2,fp_r3,fp_r4,fp_r5,fp_r6;
	PARAM_INFO param;
	QUEUE_INFO q_info[6];
	long temp,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9;
	int retCode = 0;
	SSIZE max,avg;
	fio_size size;
	double percent;
	long read_size;
	int flag_done = 0;
	int r_nnn_count;
	int r_nnn_done_or_error;
	int r_nnn_remain;
	int rip;
	int retry;
	int abort_flag;

	strcpy(log_filename,"C:\\ProgramData\\upconv\\upconv.log");

	memset(&NormInfo,0,sizeof (NORM_INFO));
	memset(&NormInfoOrg,0,sizeof (NORM_INFO));
	memset(&NormInfoFinal,0,sizeof (NORM_INFO));

	memset(&Wav2RawLevelInfo,0,sizeof (LEVEL_INFO));
	memset(&Raw2WavLevelInfo,0,sizeof (LEVEL_INFO));
	memset(&Raw2WavLevelInfo2,0,sizeof (LEVEL_INFO));

	memset(&fp_r1,0,sizeof (FIO));
	memset(&fp_r2,0,sizeof (FIO));
	memset(&fp_r3,0,sizeof (FIO));
	memset(&fp_r4,0,sizeof (FIO));
	memset(&fp_r5,0,sizeof (FIO));
	memset(&fp_r6,0,sizeof (FIO));

	workpath[0] = '\0';

	sss[2048] = '?';
	sss[2049] = '?';
	sss[2050] = '?';
	sss[2051] = '?';
	sss[2052] = '?';
	sss[2053] = '?';
	sss[2054] = '?';
	sss[2055] = '?';
	sss[2056] = '?';
	sss[2057] = '?';

	enable_upconv_log = 1;

	do {
		memset(&UpconvParam,0,sizeof (UPCONV_PARAM));
		memset(&param,0,sizeof (PARAM_INFO));
		param.p = &UpconvParam;
		memset(q_info,0,sizeof (QUEUE_INFO) * 6);

		strcpy(param.debug_upconv,"upconv:0");

PRINT_LINE("init");
		abort_flag   = 0;
		param.p->simple_mode = 0;
		param.p->simple_mode_done_percent = 0;
		param.p->simple_mode_done_percent_div = 0;

		param.process_id = 0;
		param.ana_avgcount = 0;
		param.print_stdout = 0;

		param.p->split_size = 0;
		param.p->no_rate_conv = -1;
		param.p->w   = -1;
		param.p->hfc = -1;
		param.p->lfc = -1;
		param.p->lpf = -1;
		param.p->nr = -1;
		param.p->thread = 1;
		param.p->upconv_total = 0;
		param.p->post_nr = -1;
		param.p->index_pre_eq = -1;
		param.p->index_post_eq = -1;
		param.p->index_post_eq2 = -1;
		param.p->fio = -1;
		param.p->fio_sec_size = 0;
		param.p->fio_mb_size  = 0;
		param.p->abe_version = 1;
		param.p->clipped = 0;
		param.p->fftw_multi_thread = 0;

		param.err = STATUS_SUCCESS;
		param.disRc = 0;
		param.disEq = 0;
//		param.sp_ana = 0;
//		param.hiDither = 0;
		param.r1_flag = 0;
		param.eq_flag = 0;
		param.p->abeNX = 1;
		param.p->abe_ad_filter = 0;
		param.pwAdj = 0;
		param.p->LRC_process = 0;
		param.p->LRC_process_Center = 0;
		param.p->MS_LRC_mode = 1;
		param.p->hfa_LRC_CC_Phase = 0.18;
		param.p->hfa_LRC_CC_Level = 0.89;

		param.p->abe_declip = 0;
		param.p->abe_declick = 0;
		param.p->abe_clld_level = 8;
		param.p->cut_high_dither = 0;
		param.p->abe_2val_smooth = 1;
		param.p->abe_3val_smooth = 1;
		param.p->abe_5val_smooth = 1;
		param.p->abe_9val_smooth = 1;
		param.p->abe_17val_smooth = 1;
		param.p->abe_31val_smooth = 1;
		param.p->abe_47val_smooth = 1;
		param.p->abe_rep_smooth   = 1;
		param.p->abe_10x_wlt_level = 0;
		param.p->abe_10x_cut_level = 0;
		param.p->abe_10x_cut_mode  = 1;
		param.p->abe_10x_random_noise = 0;
		param.enable_filter = 0;
		param.Adjust_enable = 0;
		param.p->hfc_auto_adjust = 0;
		param.p->cutLowData = 0;
		param.p->dsd_fmt  = -1;
		param.p->hfa_preset = 1;
		param.p->hfa3_version = 1;
		param.p->hfa_level = 0;
		param.p->sig1Enb = 1;
		param.p->sig2Enb = 1;
		param.p->sig3Enb = 1;
		param.p->sig1AvgLineNx = 3;
		param.p->sig1Phase = -4;
		param.p->sig2Phase = -1;
		param.p->sig3Phase = -3;
		param.p->hfaNB = 0;
		param.p->hfa_filter1 = 1.6;
		param.p->hfa_filter2 = 1.3;
		param.p->hfa_filter3 = 1.5;
		param.p->hfa_filter4 = 1.5;
		param.p->hfa3_analysis_limit_adjust = 0;
		param.p->hfa_analysis_limit = -1;
		param.p->hfa3_analyze_step = 273;
		param.p->hfa3_cp_analyze = 1;
		param.p->hfa_level_adjust_width = 2000;
		param.p->hfa_level_adjust_nx = 95;
		param.p->hfa3_sig_level_limit = 0;
		param.p->hfa3_fft_window_size = 0;
		param.p->hfa3_fft_window_lvl = 0;
		param.p->hfa3_fft_window_per_1s = 0;
		param.p->enable_center_hfc = 0;
		param.p->center_hfc = -1;

		param.upconv   = 0;	// upconv 使用フラグ
		param.channel_count = 0;
		param.p->mc_flag = 0;
		param.p->cpu_pri = 0;
		param.max_level2 = 1.0;
		param.p->vla_auto = 0;
		param.p->vla_nx = 1.0;
		param.p->hfa3_param_test[0] = param.p->hfa3_param_test[1] = 0;
		param.p->hfa3_pp = 0;
		param.p->addp_overrap = 1;

		// overwrite
		param.p->ovwrite_s = 44100;
		param.p->ovwrite_w = 16;
		param.p->enable_ovwrite_sw = 0;
		param.p->ovwrite_hfa = 0;
		param.p->enable_ovwrite_hfa = 0;
		param.p->ovwrite_norm = 0;
		param.p->enable_ovwrite_norm = 0;

		param.p->enable_overwrite_abe = 0;
		param.p->enable_overwrite_post_abe = 0;
		param.p->enable_overwrite_nr = 0;
		param.p->enable_overwrite_ms = 0;
		param.p->enable_overwrite_lrc = 0;

		param.argv2 = argv[2];
		sprintf(param.timecode,"0:00:00");

		abortInfo.abort_percent = 0;
		abortInfo.abort_check_interval = 5;


		param.normInfo = &NormInfo;
		param.normInfoOrg = &NormInfoOrg;
		param.normInfoFinal = &NormInfoFinal;

		paramFlag = 0;
		pparam[0] = '\0';

		if (argc == 4 || argc == 5 || argc == 6) {
			// default parameter ファイル
			if (argc != 4) {
PRINT_LINE("argc != 4");
				fp = fopen(argv[3],"r");
				if (fp == NULL) {
					retCode = STATUS_PARAMETER_ERR;param.errLine = __LINE__;
					break;
				}
				
				// パラメータの読みこみ
				if (fgets(work,3000,fp) == NULL) {
					retCode = STATUS_PARAMETER_ERR;param.errLine = __LINE__;
					break;
				}
				p1 = strrchr(work,'\n');if (p1 != NULL) *p1 = '\0';
				strcat(pparam,work);
				strcat(pparam," ");

				// デフォルトパラメータとパラメータをくっつける
				if (strlen(argv[4]) >= 1) strcat(pparam,argv[4]);

				// tmpファイル用の作業ディレクトリ
				if (fgets(workpath,_MAX_PATH,fp) == NULL) {
					retCode = STATUS_PARAMETER_ERR;param.errLine = __LINE__;
					break;
				}
				p1 = strrchr(workpath,'\n');if (p1 != NULL) *p1 = '\0';
				if (strlen(workpath) >= 2 && workpath[strlen(workpath) - 1] != '\\') strcat(workpath,"\\");

				fclose(fp);

			} else {
PRINT_LINE("argc == 4");
				strcat(pparam," -abe -abe_option:1,1,1 -abe_ad_filter:0 -abe_version:4 -abe_declip:0 -abe_declick:0 -abe_clld_level:3 -abe_10x_cut_level_db:0 -abe_10x_cut_mode:0 -hfa:0 -wav_auto -fio_mb_size:100 -norm:0 -thread:4 -fio:10 -cpu_pri:0 ");
				strcat(pparam,argv[3]);
				strcat(pparam," ");
				param.p->simple_mode = 1;
			}

			fp = NULL;

			// upconv.exe のパラメータの解析部分
			// wav2raw.c でも実施していたパラメータの解析処理をここに移動した

			p1 = pparam;
			p2 = strchr(p1,(int)' ');
			for (;p1 != NULL;) {
				if (p2 != NULL) {
					*p2 = '\0';
				}

				// 変換後のサンプリングレート
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
						case 1411200:
						case 1536000:
							paramFlag |= 0x0002;
							param.p->outSampleR = temp;
							param.p->validOutSampleR = temp;
							param.p->outSampleR_final = temp;
							break;
					}
				}

				// Overwrite
				if (sscanf(p1,"-ovwrite_s:%ld",&temp) == 1) {
					switch (temp) {
						case 0:
							param.p->ovwrite_s = 32000;
							break;
						case 1:
							param.p->ovwrite_s = 44100;
							break;
						case 2:
							param.p->ovwrite_s = 48000;
							break;
						case 3:
							param.p->ovwrite_s = 88200;
							break;
						case 4:
							param.p->ovwrite_s = 96000;
							break;
						case 5:
							param.p->ovwrite_s = 176400;
							break;
						case 6:
							param.p->ovwrite_s = 192000;
							break;
						case 7:
							param.p->ovwrite_s = 352800;
							break;
						case 8:
							param.p->ovwrite_s = 384000;
							break;
						case 9:
							param.p->ovwrite_s = 705600;
							break;
						case 10:
							param.p->ovwrite_s = 768000;
							break;
						case 11:
							param.p->ovwrite_s = 1411200;
							break;
						case 12:
							param.p->ovwrite_s = 1536000;
							break;
							break;
					}
				}

				// 入力ファイルのサンプリングレート
				if (sscanf(p1,"-is:%ld",&temp) == 1) {
					switch (temp) {
						case 22050:
						case 24000:
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
						case 1411200:
						case 1536000:
							param.p->inSampleR = temp;
							paramFlag |= 0x0001;
							break;
					}
				}
				// マルチチャンネル生成用サンプリング―レート
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
						case 1411200:
						case 1536000:
							paramFlag |= 0x0002;
							param.p->outSampleR_ms = temp;
							break;
					}
				}

				// 出力ビット数
				if (sscanf(p1,"-w:%ld",&temp) == 1) {
					if (temp == 16 || temp == 24 || temp == 32) {
						param.p->w = (int)temp;
					}
				}

				if (sscanf(p1,"-ovwrite_w:%ld",&temp) == 1) {
					switch (temp) {
						case 0:
							param.p->ovwrite_w = 16;
							break;
						case 1:
							param.p->ovwrite_w = 24;
							break;
						case 2:
							param.p->ovwrite_w = 32;
							param.p->ovwrite_w_int = 0;
							break;
						case 3:
							param.p->ovwrite_w = 64;
							param.p->ovwrite_w_int = 0;
							break;
						case 4:
							param.p->ovwrite_w = 32;
							param.p->ovwrite_w_int = 0;
							break;
					}
				}
				if (sscanf(p1,"-enable_ovwrite_sw:%ld",&temp) == 1) {
					if (temp == 0 || temp == 1) {
						param.p->enable_ovwrite_sw = (int)temp;
					}
				}

				// 入力ビット数
				if (sscanf(p1,"-iw:%ld",&temp) == 1) {
					if (temp == 16 || temp == 24 || temp == 32) {
						param.p->iw = (int)temp;
					}
				}
				// 補間モード
				if (sscanf(p1,"-hfa:%ld",&temp) == 1) {
					switch (temp) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
							paramFlag |= 0x0004;
							param.p->hfa = (int)temp;
							break;
					}
				}
				if (sscanf(p1,"-ovwrite_hfa:%ld",&temp) == 1) {
					switch (temp) {
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
							param.p->ovwrite_hfa = (int)temp;
							break;
					}
				}
				if (sscanf(p1,"-enable_ovwrite_hfa:%ld",&temp) == 1) {
					if (temp == 0 || temp == 1) {
						param.p->enable_ovwrite_hfa = (int)temp;
					}
				}

				// 変換前のイコライザ補正
				if (sscanf(p1,"-enable_pre_eq:%ld",&temp) == 1 && temp == 1) {
					param.p->enable_pre_eq = 1;
				}

				// 変換後のイコライザ補正
				if (sscanf(p1,"-enable_post_eq:%ld",&temp) == 1 && temp == 1) {
					param.p->enable_post_eq = 1;
				}
				// 変換後のイコライザ補正
				if (sscanf(p1,"-enable_post_eq2:%ld",&temp) == 1 && temp == 1) {
					param.p->enable_post_eq2 = 1;
				}

				// 変換前のイコライザ補正用ファイル番号
				if (sscanf(p1,"-pre_eq:%ld",&temp) == 1 && temp >= 0 && temp <= 999) {
					param.p->index_pre_eq = temp;
				}
				// 変換後のイコライザ補正用ファイル番号
				if (sscanf(p1,"-post_eq:%ld",&temp) == 1 && temp >= 1 && temp <= 999) {
					param.p->index_post_eq = temp;
				}
				// 変換後のイコライザ補正用ファイル番号
				if (sscanf(p1,"-post_eq2:%ld",&temp) == 1 && temp >= 1 && temp <= 999) {
					param.p->index_post_eq2 = temp;
				}
				// 解析モード
				if (sscanf(p1,"-enable_eq_analyze:%d",&temp) == 1 && temp >= 0 && temp <= 2) {
					param.p->eq_analyze = temp;
				}

				// HFC
				if (sscanf(p1,"-enable_hfc:%ld",&temp) == 1) {
					if (temp == 1) param.p->enable_hfc = 1;
				}
				if (sscanf(p1,"-hfc:%ld",&temp) == 1) {
					if (temp >= 4000 && temp <= (384000 / 2)) {
						param.p->hfc = temp;
					}
				}

				// LFC
				if (sscanf(p1,"-enable_lfc:%ld",&temp) == 1) {
					if (temp == 1) param.p->enable_lfc = 1;
				}
				if (sscanf(p1,"-lfc:%ld",&temp) == 1) {
					if (temp >= 20 && temp <= (384000 / 2)) {
						param.p->lfc = temp;
					}
				}

				// イコライザアプリから呼ばれる場合に使用する
				if (sscanf(p1,"-no_rate_conv:%ld",&temp) == 1 && temp >= 0 && temp < 4) {
					// upconv で変換するがイコライザの反映のみとする場合に使用する
					// 自動的にoversampling x2 とする
					param.p->no_rate_conv = temp;
				}
				// ABE
				if (strcmp(p1,"-abe") == 0) {
					param.p->abe = 1;
				}

				// LRC Process
				if (strcmp(p1,"-LRC_process:1") == 0) {
					param.p->LRC_process = 1;
				}

				// LRC Process
				if (strcmp(p1,"-LRC_process_Center:1") == 0) {
					param.p->LRC_process_Center = 1;
				}
				// MS_LRC_mode
				if (sscanf(p1,"-MS_LRC_mode:%ld",&temp) == 1 && temp >= 1 && temp <= 2) {
					param.p->MS_LRC_mode = temp;
				}
				
				// Post ABE
				if (strcmp(p1,"-post_abe") == 0) {
					param.p->post_abe = 1;
				}
				// Post NR
				if (strcmp(p1,"-post_nr") == 0) {
					param.p->post_nr = 1;
				}

				// ABE option
				if (sscanf(p1,"-abe_option:%d,%d,%d",&temp,&temp2,&temp3) == 3) {
					if (temp == 1) param.p->cutLowData = 1;
					if (temp2 == 1) param.p->smLowData = 1;
					if (temp3 == 1) {
						param.p->abeFnLevel = 2;
					} else if (temp3 == 2) {
						param.p->abeFnLevel = 5;
					} else if (temp3 == 3) {
						param.p->abeFnLevel = 10;
					} else if (temp3 == 4) {
						param.p->abeFnLevel = 15;
					} else if (temp3 == 5) {
						param.p->abeFnLevel = 20;
					}
				}
				if (sscanf(p1,"-abe_version:%d",&temp) == 1) {
					if (temp >= 1 && temp <= 4) {
						param.p->abe_version = temp;
					}
				}
				if (sscanf(p1,"-abe_declip:%d",&temp) == 1) {
					if (temp == 0 || temp == 1) {
						param.p->abe_declip = temp;
					}
				}
				if (sscanf(p1,"-abe_declick:%d",&temp) == 1) {
					if (temp == 0 || temp == 1) {
						param.p->abe_declick = temp;
					}
				}
				if (sscanf(p1,"-abe_ad_filter:%d",&temp) == 1) {
					if (temp == 0 || temp == 1) {
						if (temp == 1) param.p->abe_ad_filter = 1;
					}
				}
				if (sscanf(p1,"-abe_clld_level:%d",&temp) == 1) {
					if (temp >= 1 && temp <= 10) param.p->abe_clld_level = temp;
				}
				if (sscanf(p1,"-abe_10x_wlt_level:%d",&temp) == 1 && temp >= 0 && temp <= 5) {
					param.p->abe_10x_wlt_level = temp;
				}
				if (sscanf(p1,"-abe_10x_cut_level_db:%d",&temp) == 1 && temp >= -20 && temp <= 20) {
					param.p->abe_10x_cut_level = temp;
				}
				if (sscanf(p1,"-abe_10x_cut_mode:%d",&temp) == 1 && temp >= 0 && temp <= 2) {
					param.p->abe_10x_cut_mode = temp;
				}
				if (sscanf(p1,"-abe_10x_random_noise:%d",&temp) == 1 && temp == 1) {
					param.p->abe_10x_random_noise = temp;
				}

				// HFA 関連オプション
				if (sscanf(p1,"-hfc_auto_adjust:%d",&temp) == 1) {
					if (temp == 0) param.p->hfc_auto_adjust = 0;
					if (temp == 1) param.p->hfc_auto_adjust = -1000;
					if (temp == 2) param.p->hfc_auto_adjust = -2000;
					if (temp == 3) param.p->hfc_auto_adjust = -3000;
					if (temp == 4) param.p->hfc_auto_adjust = -4000;
					if (temp == 5) param.p->hfc_auto_adjust = -5000;
					if (temp == 6) param.p->hfc_auto_adjust = -6000;
					if (temp == 7) param.p->hfc_auto_adjust = -7000;
					if (temp == 8) param.p->hfc_auto_adjust = -8000;
					if (temp == 9) param.p->hfc_auto_adjust = -9000;
				}
				if (sscanf(p1,"-hfa3_analysis_limit_adjust:%d",&temp) == 1 && temp >= -4 && temp <= 4) {
					param.p->hfa3_analysis_limit_adjust = temp * 1000;
				}
				if (sscanf(p1,"-hfa3_freq_start:%d",&temp) == 1 && temp >= 4000 && temp <= 12000) {
					param.p->hfa3_freq_start = temp;
				}
				if (sscanf(p1,"-hfa3_freq_len:%d",&temp) == 1 && temp >= 1500 && temp <= 4000) {
					param.p->hfa3_freq_len = temp;
				}
				if (sscanf(p1,"-hfa3_analyze_step:%d",&temp) == 1 && temp >= 70 && temp <= 300) {
					param.p->hfa3_analyze_step = temp;
				}
				if (sscanf(p1,"-hfa_level_adj_width:%d",&temp) == 1 && temp >= 1000 && temp <= 4000) {
					param.p->hfa_level_adjust_width = temp;
				}
				if (sscanf(p1,"-hfa_level_adj_nx:%d",&temp) == 1 && temp >= 10 && temp <= 100) {
					param.p->hfa_level_adjust_nx = temp;
				}
				if (sscanf(p1,"-hfa3_sig_level_limit:%d",&temp) == 1 && (temp == 0 || (temp >= 10 && temp <= 16))) {
					param.p->hfa3_sig_level_limit = temp;
				}
				if (sscanf(p1,"-hfa3_cp_analyze:%d",&temp) == 1 && temp >= 0 && temp <= 1) {
					param.p->hfa3_cp_analyze = temp;
				}
				if (sscanf(p1,"-hfa3_pp:%d",&temp) == 1 && temp >= 1 && temp <= 2) {
					param.p->hfa3_pp = temp;
				}

				if (sscanf(p1,"-hfa3_freq_start_LRC:%d",&temp) == 1 && temp >= 4000 && temp <= 12000) {
					param.p->hfa3_freq_start_LRC = temp;
				}
				if (sscanf(p1,"-hfa3_freq_len_LRC:%d",&temp) == 1 && temp >= 1500 && temp <= 4000) {
					param.p->hfa3_freq_len_LRC = temp;
				}
				if (sscanf(p1,"-hfa3_analyze_step_LRC:%d",&temp) == 1 && temp >= 70 && temp <= 300) {
					param.p->hfa3_analyze_step_LRC = temp;
				}
				if (sscanf(p1,"-hfa_level_adj_width_LRC:%d",&temp) == 1 && temp >= 1000 && temp <= 4000) {
					param.p->hfa_level_adjust_width_LRC = temp;
				}
				if (sscanf(p1,"-hfa_level_adj_nx_LRC:%d",&temp) == 1 && temp >= 10 && temp <= 100) {
					param.p->hfa_level_adjust_nx_LRC = temp;
				}
				if (sscanf(p1,"-hfa3_sig_level_limit_LRC:%d",&temp) == 1 && (temp == 0 || (temp >= 10 && temp <= 16))) {
					param.p->hfa3_sig_level_limit_LRC = temp;
				}
				if (sscanf(p1,"-hfa3_pp_LRC:%d",&temp) == 1 && temp >= 1 && temp <= 2) {
					param.p->hfa3_pp_LRC = temp;
				}
				if (sscanf(p1,"-hfa3_LRC_CC_Phase:%lf",&dd) == 1 && dd >= 0.01 && dd <= 0.35) {
					param.p->hfa_LRC_CC_Phase = dd;
				}
				if (sscanf(p1,"-hfa3_LRC_CC_Level:%lf",&dd) == 1 && dd >= 0.75  && dd <= 1.00) {
					param.p->hfa_LRC_CC_Level = dd;
				}

				if (sscanf(p1,"-hfa3_test_param:%d,%d",&temp,&temp2) == 2 && (temp >= 0 && temp <= 1) && (temp2 >= 0 && temp2 <= 1)) {
					if (temp == 1) {
						param.p->hfa3_param_test[0] = 1;
					}
					if (temp2 == 1) {
						param.p->hfa3_param_test[1] = 1;
					}
				}
				if (sscanf(p1,"-hfa3_fft_window_size:%d",&temp) == 1 && (temp >= 10 && temp <= 30)) {
					param.p->hfa3_fft_window_size = temp;
				}
				if (sscanf(p1,"-hfa3_fft_window_lvl:%d",&temp) == 1 && (temp >= 4 && temp <= 20)) {
					param.p->hfa3_fft_window_lvl = temp;
				}
				if (sscanf(p1,"-hfa3_fft_window_per_1s:%d",&temp) == 1 && (temp >= 0 && temp <= 2)) {
					param.p->hfa3_fft_window_per_1s = temp;
				}
				if (strcmp(p1,"-enable_center_hfc:1") == 0) {
					param.p->enable_center_hfc = 1;
				}
				if (sscanf(p1,"-center_hfc:%d",&temp) == 1 && (temp >= 1000)) {
					param.p->center_hfc = temp;
				}

				if (strcmp(p1,"-cut_high_dither") == 0) {
					param.p->cut_high_dither = 1;
				}
				if (strcmp(p1,"-low_adjust") == 0) {
					param.p->low_adjust = 1;
				}
				if (strcmp(p1,"-hfc_auto") == 0) {
					param.p->hfc_auto = 1;
				}

				// オーバーサンプリング関連
				if (sscanf(p1,"-oversamp:%ld",&temp) == 1) {
					if (temp >= 0 || temp <= 6) {
						param.p->oversamp = (int)temp;
					}
				}

#if 0
				if (sscanf(p1,"-lpf:%ld",&temp) == 1) {
					if (temp >= 1000 && temp <= (384000 / 2)) {
						param.p->lpf = temp;
					}
				}
#endif
				if (sscanf(p1,"-enable_nr:%d",&temp) == 1) {
					if (temp == 1) param.p->enable_nr = 1;
				}
				if (sscanf(p1,"-nr:%ld",&temp) == 1) {
					if (temp >= 100 && temp <= (384000 / 2)) {
						param.p->nr = temp;
					}
				}
				if (sscanf(p1,"-nr_option:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 5) {
						param.p->nr_option = (int)temp - 1;
					}
				}
				if (sscanf(p1,"-hfa_preset:%d",&temp) == 1) {
					if (temp >= 1 && temp <= 10) {
						param.p->hfa_preset = temp;
					}
				}
				if (sscanf(p1,"-hfa3_option:%d",&temp) == 1) {
					if (temp >= 1 && temp <= 3) {
						param.p->hfa3_version = temp;
					}
				}
				if (sscanf(p1,"-hfa_level:%d",&temp) == 1) {
					if (temp >= 1 && temp <= 6) {
						param.p->hfa_level = temp;
					}
				}
				if (sscanf(p1,"-hfa_filter:%d,%d,%d,%d",&temp,&temp2,&temp3,&temp4) == 4) {
					if ((temp >= 1 && temp <= 9) && (temp2 >= 1 && temp2 <= 9) && (temp3 >= 1 && temp3 <= 9) && (temp4 >= 1 && temp4 <= 9)) {
						param.p->hfa_filter1 = (temp / 10.0) + 1.0;
						param.p->hfa_filter2 = (temp2 / 10.0) + 1.0;
						param.p->hfa_filter3 = (temp3 / 10.0) + 1.0;
						param.p->hfa_filter4 = (temp4 / 10.0) + 1.0;
					}
				}
				if (sscanf(p1,"-thread:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 24) {
						param.p->thread = (int)temp;
					}
				}
				if (strcmp(p1,"-fftw_multi_thread") == 0) {
					param.p->fftw_multi_thread = 1;
				}
				if (sscanf(p1,"-upconv_total:%ld",&temp) == 1) {
					if (temp > 0) {
						param.p->upconv_total = (int)temp;
					}
				}

				if (strcmp(p1,"-hfaFast") == 0) {
					param.p->hfaFast = 1;
				}
				if (strcmp(p1,"-hfaWide") == 0) {
					param.p->hfaWide = 1;
				}
				if (sscanf(p1,"-deE:%ld",&temp) == 1) {
					if (temp == 0 || temp == 1 || temp == 2) {
						param.p->deEmphasis = (int)temp;
					}
				}
#if 0
				if (sscanf(p1,"-sfa:%ld",&temp) == 1) {
					if (temp == 1 || temp == 2) {
						param.p->sfa = temp;
					}
				}
#endif
#if 0
				if (strcmp(p1,"-adjFreq") == 0) {
					param.ana = 1;
				}
				if (strcmp(p1,"-spAna") == 0) {
					// 192khz 固定で解析
					param.sp_ana = 1;
				}
#endif
				if (sscanf(p1,"-cpu_pri:%d",&temp) == 1 && temp >= 0 && temp <= 2) {
					param.p->cpu_pri = temp;
				}

				if (sscanf(p1,"-fio:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 50) {
						param.p->fio = temp;
					} else {
						param.p->fio = 5;
					}
				}
				if (sscanf(p1,"-fio_auto:%ld",&temp) == 1) {
					if (temp == 1) {
						param.p->fio_auto = 1;
					}
				}
				if (sscanf(p1,"-fio_max:%ld",&temp) == 1) {
					param.p->fio_max = temp;
				}

				if (sscanf(p1,"-fio_sec_size:%ld",&temp) == 1) {
					param.p->fio_sec_size = temp;
				}

				if (sscanf(p1,"-fio_mb_size:%ld",&temp) == 1) {
					param.p->fio_mb_size = temp;
				}

				if (sscanf(p1,"-split_size:%d",&temp) == 1) {
					if (temp == 300 || temp == 600) {
						param.p->split_size = temp;
					}
				}

//				if (strcmp(p1,"-eq") == 0) {
//					param.eq_flag = 1;
//				}
//				if (sscanf(p1,"-temp:%c%c%c",tmppath,tmppath+1,tmppath+2) == 3) {
//					param.tempPath[0] = tmppath[0];
//					param.tempPath[1] = tmppath[1];
//					param.tempPath[2] = '\0';
//					param.tempPath[3] = '\0';
//				}
//				if (sscanf(p1,"-dsd_fmt:%ld",&temp) == 1) {
//					if ((temp == 64 || temp == 128 || temp == 256)) {
//						param.dsd_fmt = temp;
//						if (param.hfc == -1) {
//							param.hfc = 24000;
//						}
//					}
//				}

				// Volume 関連
				if (strcmp(p1,"-vla:auto") == 0) {
					param.p->vla_auto = 1;
					param.p->vla_nx = 1.0;
				} else if (sscanf(p1,"-vla:%.2f%%",&temp_f) == 1 && (temp_f >= 0.5 && temp_f <= 1.50)) {
					param.p->vla_nx = temp_f;
				} else if (sscanf(p1,"-vla:%d",&temp) == 1) {
					param.p->vla_auto = 0;
					if (temp >= 0 && temp <= 10) {
						switch (temp) {
							case 0:
								param.p->vla_nx = 1.0;
								break;
							case 1:
								param.p->vla_nx = 0.95;
								break;
							case 2:
								param.p->vla_nx = 0.90;
								break;
							case 3:
								param.p->vla_nx = 0.85;
								break;
							case 4:
								param.p->vla_nx = 0.80;
								break;
							case 5:
								param.p->vla_nx = 0.75;
								break;
							case 6:
								param.p->vla_nx = 0.70;
								break;
							case 7:
								param.p->vla_nx = 0.65;
								break;
							case 8:
								param.p->vla_nx = 0.60;
								break;
							case 9:
								param.p->vla_nx = 0.55;
								break;
							case 10:
								param.p->vla_nx = 0.50;
								break;
						}
					}
				}
				
				// wav2raw
				if (sscanf(p1,"-conv_test:%ld",&temp) == 1) {
					param.p->conv_test = temp;
				}

				// 分割変換
				if (sscanf(p1,"-addp:%ld",&temp) == 1) {
					if (temp == 0 || temp == 2 || temp == 4 || temp == 6 || temp == 8 || temp == 10 || temp == 12 || temp == 14 || temp == 16 || temp == 18 || temp == 20 || temp == 22 || temp == 24) {
						param.p->addp = temp;
					}
				}

				if (sscanf(p1,"-ms_process:%ld",&temp) == 1 && temp >= 1 && temp <= 10) {
					param.p->ms_process = temp;
					if (temp == 5) {
						param.p->ms_process = 4;
						param.p->sfa = 1;
					}
				}

				// 開始オフセット
				if (sscanf(p1,"-skip_start_offset:%ld",&temp) == 1) {
					// 秒単位でスキップする秒を指定する
					param.p->skip_start_offset = temp;
				}

				// 変換後のファイルを32bit Int にする
				if (strcmp(p1,"-w_int") == 0) {
					if (param.p->w == 32) {
						param.p->bitwidth_int = 1;
					}
				}

				// raw2wav
				if (strcmp(p1,"-C") == 0 || strcmp(p1,"-SLR") == 0 || strcmp(p1,"-LFE") == 0) {
					param.p->mc_flag = 1;
				}

				if (sscanf(p1,"-ch:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 6) {
						param.p->ch = temp;
						param.p->realCh = temp;
					}
				}
				if (sscanf(p1,"-r_nnn_count:%ld",&temp) == 1) {
					if (temp > 0) {
						param.p->r_nnn_count = temp;
					}
				}

				if (sscanf(p1,"-output_dither:%ld",&temp) == 1) {
					switch (temp) {
						case 0:
						case 1:
						case 2:
						case 3:
							param.p->output_dither_mode = temp;
							break;
					}
				}
				if (sscanf(p1,"-output_dither_option:%ld",&temp) == 1) {
					if (temp >= 0 && temp <= 16) {
						param.p->output_dither_option = temp;
					}
				}
				if (strcmp(p1,"-output_mid_side") == 0) {
					param.p->output_mid_side = 1;
				}
				if (strcmp(p1,"-output_left_right_center") == 0) {
					param.p->output_left_right_center = 1;
				}

				if (sscanf(p1,"-norm:%ld",&temp) == 1) {
					switch (temp) {
						case 0:
							param.p->norm = 0;
							param.p->norm_option = 1;
							break;
						case 1:
							param.p->norm = 1;
							param.p->norm_option = 1;
							break;
						case 2:
							param.p->norm = 0;
							param.p->norm_option = 1;
							break;
						case 3:
							param.p->norm = 1;
							param.p->norm_option = 1;
							break;
					}
				}

				if (sscanf(p1,"-ovwrite_norm:%ld",&temp) == 1) {
					switch (temp) {
						case 0:
							param.p->ovwrite_norm = 0;
							break;
						case 1:
							param.p->ovwrite_norm = 1;
							break;
						case 2:
							param.p->ovwrite_norm = 0;
							break;
						case 3:
							param.p->ovwrite_norm = 1;
							break;
					}
				}

				if (sscanf(p1,"-enable_ovwrite_norm:%ld",&temp) == 1) {
					if (temp == 0 || temp == 1) {
						param.p->enable_ovwrite_norm = (int)temp;
					}
				}

				if (sscanf(p1,"-volume_level:%d",&temp) == 1 && temp > 0 && temp <= 100) {
					param.p->volume_level = temp;
					param.p->volume_level_enable = 1;
				}

				if (sscanf(p1,"-volume_per:%d",&temp) == 1 && temp > 0 && temp <= 120) {
					param.p->volume_per = temp;
					param.p->volume_per = param.p->volume_per / 100.0;
					param.p->volume_per_enable = 1;
				}

				if (strcmp(p1,"-bwf") == 0) {
					param.p->bwf = 1;
				}
				if (strcmp(p1,"-raw") == 0) {
					param.p->raw = 1;
				}
				if (strcmp(p1,"-rf64") == 0) {
					param.p->rf64 = 1;
				}
				if (strcmp(p1,"-w64") == 0) {
					param.p->rf64 = 1;
					param.p->w64 = 1;
				}
				if (strcmp(p1,"-wav2g") == 0) {
					param.p->wav2g = 1;
				}
				if (strcmp(p1,"-wav_auto") == 0) {
					param.p->wav_auto = 1;
				}

				if (strcmp(p1,"-C") == 0) {
					param.p->chC = 1;
					param.p->genCh++;
				}
				if (strcmp(p1,"-SLR") == 0) {
					param.p->chS = 1;
					param.p->genCh += 2;
				}
				if (strcmp(p1,"-LFE") == 0) {
					param.p->chLFE = 1;
					param.p->genCh++;
				}
				if (sscanf(p1,"-MC_Option:%d,%d,%d,%d",&temp,&temp2,&temp3,&temp4) == 4) {
					if (temp == 1) {
						param.p->mc_split = 1;
					}
					if (temp2 == 1) {
						param.p->mc_downmix = 1;
					}
				}
				if (sscanf(p1,"-encorder:%ld",&temp) == 1) {
					if (temp >= 0) param.p->encorder = temp;
				}

				if (sscanf(p1,"-dsd_fmt:%ld",&temp) == 1) {
					if (temp == 64 || temp == 128 || temp == 256) {
						param.p->dsd_fmt = (int)temp;
					}
				}
				if (strcmp(p1,"-out_mp4") == 0) {
					param.p->out_mp4 = 1;
				}

				if (strcmp(p1,"-dp_save") == 0) {
					param.p->dp_save = 1;
				}

				// 変換管理用
				if (sscanf(p1,"-upconv:%ld",&temp) == 1) {
					param.upconv = temp;
					if (param.upconv != 0) {
//						enable_upconv_log = 0;
					}
				}
				if (sscanf(p1,"-r_nnn:%ld",&temp) == 1) {
					param.r_nnn = temp;
				}

				if (sscanf(p1,"-process_id:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 3) {
						param.process_id = temp;
					}
				}
				if (strcmp(p1,"-print_stdout") == 0) {
					param.print_stdout = 1;
				}
				if (sscanf(p1,"-prof_mode:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 3) {
						param.p->prof_mode = temp;
					}
				}
				if (strcmp(p1,"-level_scan_only") == 0) {
					param.p->level_scan_only = 1;
				}
				if (sscanf(p1,"-enable_overwrite_abe:%ld",&temp) == 1) {
					param.p->enable_overwrite_abe = temp;
				}
				if (sscanf(p1,"-enable_overwrite_post_abe:%ld",&temp) == 1) {
					param.p->enable_overwrite_post_abe = temp;
				}
				if (sscanf(p1,"-enable_overwrite_nr:%ld",&temp) == 1) {
					param.p->enable_overwrite_nr = temp;
				}
				if (sscanf(p1,"-enable_overwrite_ms:%ld",&temp) == 1) {
					param.p->enable_overwrite_ms = temp;
				}
				if (sscanf(p1,"-enable_overwrite_lrc:%ld",&temp) == 1) {
					param.p->enable_overwrite_lrc = temp;
				}
				
				if (p2 == NULL) {
					break;
				}
				p1 = p2 + 1;
				p2 = strchr(p1,(int)' ');
			}
PRINT_LINE("cmdline parse end");
			// ログ出力
			if (argc == 6 && strlen(argv[5])) {
				strcpy(log_filename,argv[5]);
			}
			// 中断検出
			if (param.p->outSampleR >= 705600 || param.p->inSampleR >= 705600 || param.p->oversamp == 3 || param.p->oversamp == 5 || param.p->oversamp == 6) {
				abortInfo.abort_check_interval = 1;
			}

			if (param.p->LRC_process == 1 && param.p->ms_process != 0 && param.p->MS_LRC_mode == 2) {
			} else {
				param.p->MS_LRC_mode = 1;
			}

			// Overwrite

			if (param.p->enable_overwrite_abe != param.p->abe) {
				param.p->abe = param.p->enable_overwrite_abe;
			}
			if (param.p->enable_overwrite_post_abe != param.p->post_abe) {
				param.p->post_abe = param.p->enable_overwrite_post_abe;
			}
			if (param.p->enable_overwrite_nr != param.p->enable_nr) {
				param.p->enable_nr = param.p->enable_overwrite_nr;
			}
			if (param.p->ms_process != 0 && param.p->enable_overwrite_ms == 0) {
				param.p->ms_process = 0;
			}
			if (param.p->LRC_process == 1 && param.p->enable_overwrite_lrc == 0) {
				param.p->LRC_process = 0;
			}
			
			// LRC Process
			if (param.p->LRC_process_Center == 1) {
				PRINT_LOG(param.debug_upconv,"LRC_process_Center");
				param.p->hfa3_freq_start = param.p->hfa3_freq_start_LRC;
				param.p->hfa3_freq_len   = param.p->hfa3_freq_len_LRC;
				param.p->hfa3_analyze_step = param.p->hfa3_analyze_step_LRC;
				param.p->hfa_level_adjust_width = param.p->hfa_level_adjust_width_LRC;
				param.p->hfa_level_adjust_nx = param.p->hfa_level_adjust_nx_LRC;
				param.p->hfa3_sig_level_limit = param.p->hfa3_sig_level_limit_LRC;
				param.p->hfa3_pp = param.p->hfa3_pp_LRC;
				if (param.p->enable_center_hfc == 1 && param.p->center_hfc >= 0) {
					param.p->enable_hfc = param.p->enable_center_hfc;
					sprintf(work,"enable_center_hfc:1");PRINT_LOG(param.debug_upconv,work);
					param.p->hfc = param.p->center_hfc;
					sprintf(work,"center_hfc:%d",param.p->center_hfc);PRINT_LOG(param.debug_upconv,work);
				}
				sprintf(work,"hfa3_freq_start:%d",param.p->hfa3_freq_start);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa3_freq_len:%d",param.p->hfa3_freq_len);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa3_analyze_step:%d",param.p->hfa3_analyze_step);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa_level_adjust_width:%d",param.p->hfa_level_adjust_width);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa_level_adjust_nx:%d",param.p->hfa_level_adjust_nx);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa3_sig_level_limit:%d",param.p->hfa3_sig_level_limit);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa3_pp:%d",param.p->hfa3_pp);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa_LRC_CC_Phase:%lf",param.p->hfa_LRC_CC_Phase);PRINT_LOG(param.debug_upconv,work);
				sprintf(work,"hfa_LRC_CC_Level:%lf",param.p->hfa_LRC_CC_Level);PRINT_LOG(param.debug_upconv,work);
			}

			if (param.p->enable_hfc == 0) {
				param.p->hfc = -1;
			}
			if (param.p->enable_lfc == 0) {
				param.p->lfc = -1;
			}
			if (param.p->enable_nr == 0) {
				param.p->nr = -1;
			}
			if (param.p->hfc != -1 && param.p->hfc_auto == 1) {
				param.p->hfc_auto = 0;
			}

			if (param.p->volume_level_enable == 1) {
				param.p->norm = 0;
				param.p->norm_option = 1;
			}

#if 0
			if (param.sp_ana == 1) {
				param.outSampleR = 192000;
				param.hfa		 = 0;
				param.overSamp	 = 0;
				param.hfc		 = -1;
				param.lfc		 = -1;
				param.lpf		 = -1;
				param.nr		 = -1;
				param.hfc_auto	 = 0;
				param.eq_flag	 = 0;
				param.post_abe	 = 0;
			}
#endif

			PRINT_FN_LOG(param.debug_upconv,"main:start");
//			if (argc != 4) {
//				sprintf(work,"argc:%d,argv:[%s,%s,%s,%s,%s]",argc,argv[0],argv[1],argv[2],argv[3],argv[4]);
//			} else {
//				sprintf(work,"argc:%d,argv:[%s,%s,%s,%s]",argc,argv[0],argv[1],argv[2],argv[3]);
//			}
//			PRINT_LOG(param.debug_upconv,work);

			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"err");
			strcpy(abortInfo.error_filename,tmppath);
//			sprintf(sss,"error filename:%s",abortInfo.error_filename);
//			PRINT_LOG(param.debug_upconv,sss);
			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"abort");
			strcpy(abortInfo.abort_filename,tmppath);
//			sprintf(sss,"abort filename:%s",abortInfo.abort_filename);
//			PRINT_LOG(param.debug_upconv,sss);

#ifdef _OPENMP
			omp_set_num_threads(param.p->thread);
#endif
#ifdef FFTW_MULTI_THREAD
			if (param.upconv == 1 && param.p->fftw_multi_thread == 0) {
				param.p->fftw_multi_thread = 1;
			}

			if (param.p->fftw_multi_thread == 1) {
				char m[200];
				fftw_init_threads();
				fftw_plan_with_nthreads(param.p->thread);
				sprintf(m,"fftw_plan_with_nthreads:%d",param.p->thread);
				PRINT_LOG("upconv",m);
			}
#endif
			if (param.process_id == 0) {
				int arg_count;
				char *arg[6];
				char buffer[2048];
				STARTEXEC_PROCESS_INFO sp_info[1];
				int file_found;

PRINT_LINE("process_id:0");
				PRINT_LOG(param.debug_upconv,"param.process_id:0");
				// ワークファイルの削除処理
				_makepath(tmppath,drive,dir,fname,"files");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"err");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"abort");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				_makepath(tmppath,drive,dir,fname,"level");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				_makepath(tmppath,drive,dir,fname,"param");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r1");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r1.tmp");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r1.tmp2");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				_makepath(tmppath,drive,dir,fname,"r2");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r2.tmp");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r2.tmp2");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				_makepath(tmppath,drive,dir,fname,"r3");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r3.tmp");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r3.tmp2");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				_makepath(tmppath,drive,dir,fname,"r4");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r4.tmp");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r4.tmp2");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				_makepath(tmppath,drive,dir,fname,"r5");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r5.tmp");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r5.tmp2");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				_makepath(tmppath,drive,dir,fname,"r6");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r6.tmp");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);
				_makepath(tmppath,drive,dir,fname,"r6.tmp2");
//				PRINT_LOG(param.debug_upconv,tmppath);
				unlink(tmppath);

				for (i = 1;i <= 999;i++) {
					int delete_flag = 0;
//					sprintf(tmppath,"%s%s%s%s.%03d",drive,dir,fname,".r1",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.param",drive,dir,fname,".r1",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp",drive,dir,fname,".r1",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp2",drive,dir,fname,".r1",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);

//					sprintf(tmppath,"%s%s%s%s.%03d",drive,dir,fname,".r2",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.param",drive,dir,fname,".r2",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp",drive,dir,fname,".r2",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp2",drive,dir,fname,".r2",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);

//					sprintf(tmppath,"%s%s%s%s.%03d",drive,dir,fname,".r3",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.param",drive,dir,fname,".r3",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp",drive,dir,fname,".r3",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp2",drive,dir,fname,".r3",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);

//					sprintf(tmppath,"%s%s%s%s.%03d",drive,dir,fname,".r4",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.param",drive,dir,fname,".r4",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp",drive,dir,fname,".r4",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp2",drive,dir,fname,".r4",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);

//					sprintf(tmppath,"%s%s%s%s.%03d",drive,dir,fname,".r5",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.param",drive,dir,fname,".r5",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp",drive,dir,fname,".r5",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp2",drive,dir,fname,".r5",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);

//					sprintf(tmppath,"%s%s%s%s.%03d",drive,dir,fname,".r6",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.param",drive,dir,fname,".r6",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp",drive,dir,fname,".r6",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
//					sprintf(tmppath,"%s%s%s%s.%03d.tmp2",drive,dir,fname,".r6",i);
//					PRINT_LOG(param.debug_upconv,tmppath);
					if (file_exists(tmppath)) delete_flag = 1;
					unlink(tmppath);
					if (delete_flag == 0) break;
				}

				arg[0] = strdup(argv[0]);
				arg[1] = malloc(4096);if (arg[1] != NULL) strcpy(arg[1],argv[1]);
				arg[2] = strdup(argv[2]);
				if (argc != 4) {
//PRINT_LINE("argc != 4");
					arg[3] = strdup(argv[3]);
					arg[4] = (char *)malloc(2048);
				} else {
//PRINT_LINE("argc == 4");
					arg[3] = (char *)malloc(3082);
				}
				if (argc == 4) {
//PRINT_LINE("argc == 4");
					arg[4] = NULL;
					arg[5] = NULL;
					arg_count = 4;
					if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL || arg[3] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;
						param.errLine = __LINE__;
						break;
					}
				} else if (argc == 5) {
//PRINT_LINE("argc == 5");
					arg[5] = NULL;
					arg_count = 5;
					if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL || arg[3] == NULL || arg[4] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;
						param.errLine = __LINE__;
						break;
					}
				} else if (argc == 6) {
//PRINT_LINE("argc == 6");
					arg[5] = strdup(argv[5]);
					arg_count = 6;
					if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL || arg[3] == NULL || arg[4] == NULL || arg[5] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;
						param.errLine = __LINE__;
						break;
					}
				}
				if (argc != 4) {
//PRINT_LINE("argc != 4");
					strcpy(arg[4],argv[4]);
					strcat(arg[4]," -process_id:1");
					strcat(arg[4]," -upconv:0");
					if (param.p->mc_flag) {
						sprintf(sss," -ms:%d",param.p->outSampleR * 2);
						strcat(arg[4],sss);
					}
				} else {
//PRINT_LINE("argc == 4");
					strcpy(arg[3],argv[3]);
					strcat(arg[3]," -process_id:1");
					strcat(arg[3]," -upconv:0");
				}
				memset(&sp_info[0],0,sizeof (STARTEXEC_PROCESS_INFO));
				sprintf(sss,"start_exec(upconv.exe):2nd start");
				PRINT_LOG(param.debug_upconv,sss);
				if (start_exec(arg_count,arg,param.p->cpu_pri,&sp_info[0]) == -1) {
					retCode = STATUS_EXEC_FAIL;
					param.errLine = __LINE__;
					break;
				}
//PRINT_LINE("start_exec end");
				PRINT_LOG(param.debug_upconv,"start_exec(upconv.exe):OK");
				q_info[0].startexec_info[0].state = 1;
				if (retCode != 0) break;

				for (;;) {
					read_size = read_stdout(buffer,2000,&sp_info[0]);
					if (read_size < 0) {
						PRINT_LOG(param.debug_upconv,"pipe done");
						break; // pipe done
					} else {
						// Error
//						PRINT_LOG(param.debug_upconv,"pipe error");
						break;
					}
					if (read_size == 0) {
						continue;
					}
					buffer[read_size] = '\0';
					fprintf(stdout,"%s",buffer);
					fflush(stdout);
				}
//PRINT_LINE("pipe done");
				PRINT_LOG(param.debug_upconv,"read_stdout(upconv.exe):done");
				wait_exec(&sp_info[0],1);
				PRINT_LOG(param.debug_upconv,"wait_exec(upconv.exe):done");
				finish_exec(&sp_info[0]);
				PRINT_LOG(param.debug_upconv,"finish_exec(upconv.exe):done");

				PRINT_LOG(param.debug_upconv,"cleanup");
				_splitpath(argv[2],drive,dir,fname,ext);
				_makepath(tmppath,drive,dir,fname,"files");
#ifndef NOCLEANUP
				for (i = 0,file_found = 0;i < 30;i++) {
					fp_files = fopen(tmppath,"r");
					if (fp_files != NULL) {
						while (fgets(workstr,_MAX_PATH,fp_files) != NULL) {
							p1 = strrchr(workstr,'\n');if (p1 != NULL) *p1 = '\0';
							unlink(workstr);
						}
						rewind(fp_files);
						while (fgets(workstr,_MAX_PATH,fp_files) != NULL) {
							p1 = strrchr(workstr,'\n');if (p1 != NULL) *p1 = '\0';
							fp = fopen(workstr,"r");
							if (fp) {
								fclose(fp);
								file_found = 1;
							}
						}
						fclose(fp_files);
						if (file_found) {
							// 削除されていないファイルがある。
							Sleep(3);
						} else {
							// ファイルがすべて削除された
							break;
						}
					} else {
						break;
					}
				}
#endif
//PRINT_LINE("end NOCLEANUP");
				_splitpath(argv[2],drive,dir,fname,ext);
				_makepath(tmppath,drive,dir,fname,"files");
				unlink(tmppath);
				fprintf(stdout,"\n");
				fflush(stdout);
				fprintf(stdout,"End\n");
				fflush(stdout);
				paramFlag = 0x0007;
//PRINT_LINE("process_id:0 end");
			} else if (param.process_id == 1) {
				// upconv.exe 2段目(wav -> raw,upconv,raw -> wav制御部分)
				// wav2raw 変換処理
				int ip;
				int retStatus;
				int arg_count;
				char *arg[6];
				char buffer[2048];
				paramFlag = 0x07;
//PRINT_LINE("process_id:1");

				PRINT_LOG(param.debug_upconv,"upconv=0");
				arg[0] = strdup(argv[0]);
				arg[1] = malloc(4096);if (arg[1] != NULL) strcpy(arg[1],argv[1]);
				arg[2] = strdup(argv[2]);
				if (argc == 4) {
					arg[3] = (char *)malloc(4096);
				} else {
					arg[3] = strdup(argv[3]);
					arg[4] = (char *)malloc(4096);
				}
				if (argc == 4) {
					arg_count = 4;
					if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL || arg[3] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;param.errLine = __LINE__;
						break;
					}
					strcpy(arg[3],argv[3]);
				} else if (argc == 5) {
					arg_count = 5;
					if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL || arg[3] == NULL || arg[4] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;param.errLine = __LINE__;
						break;
					}
					strcpy(arg[4],argv[4]);
				} else if (argc == 6) {
					arg_count = 6;
					arg[5] = strdup(argv[5]);
					if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL || arg[3] == NULL || arg[4] == NULL || arg[5] == NULL) {
						retCode = STATUS_MEM_ALLOC_ERR;param.errLine = __LINE__;
						break;
					}
					strcpy(arg[4],argv[4]);
					strcpy(arg[5],argv[5]);
				}
				PRINT_LOG(param.debug_upconv,"to_raw_main");

				param.p->simple_mode_done_percent = 0;
				param.p->simple_mode_done_percent_div = 5;
//PRINT_LINE("before to_raw_main");
				retStatus = to_raw_main(arg_count,arg,param.p);
//PRINT_LINE("after to_raw_main");
				if (retStatus != STATUS_SUCCESS) {
					break;
				}
				if (param.p->level_scan_only == 1) {
					break;
				}

				PRINT_LOG(param.debug_upconv,"to_raw_main:done");
				strcpy(param.debug_upconv,"upconv:0");
				if (param.p->prof_mode == 1) {
					strcpy(work,argv[2]);
					strcat(work,".txt");
					fp = fopen(work,"w");
					if (fp) {
						fprintf(fp,"%s \"%s\" \"%s\" \"%s\" \"%s\"\n",argv[0],argv[1],argv[2],argv[3],argv[4]);
						fclose(fp);
						exit(0);
					}
				}
				
				param.channel_count = param.p->ch;
				if (param.channel_count == 2 && param.p->LRC_process == 1) {
					param.channel_count++;
				}
				//// wav ファイルのチャンネル数をもらう
				//p1 = strstr(arg[4],"-ch:");
				//if (p1 != NULL) {
				//	if (sscanf(p1,"-ch:%d",&temp) == 1) {
				//		param.channel_count = temp;
				//	}
				//}
				if (param.channel_count == 0) {
					retCode = STATUS_UNKNOWN_FORMAT;param.errLine = __LINE__;
					break;
				}
				
				// wav ファイルの分割数
				r_nnn_count = param.p->r_nnn_count;

				if (r_nnn_count == 0) {
					retCode = STATUS_UNKNOWN_FORMAT;param.errLine = __LINE__;
					break;
				}

				PRINT_LOG(param.debug_upconv,"Init addp");
				// 分割変換の準備をする。分割しない場合は1として動作させる。
				for (ip = 0;ip < param.channel_count;ip++) {
					q_info[ip].done_flag = malloc((r_nnn_count + 1) * sizeof(int));
					if (q_info[ip].done_flag == NULL) break;
					memset(q_info[ip].done_flag,0,(r_nnn_count + 1) * sizeof (int));
				}
				if (ip != param.channel_count) {
					retCode = STATUS_MEM_ALLOC_ERR;param.errLine = __LINE__;
					break;
				}

				if (param.p->prof_mode == 2) {
					param.channel_count = 1;
				}
				// upconv 部分
				do {
					int sp_i;
					int r_nnn_done;
//					PRINT_LOG(param.debug_upconv,"1");
					memset(&sp_info[0],0,sizeof (STARTEXEC_PROCESS_INFO) * 6*24);
//					PRINT_LOG(param.debug_upconv,"2");

					r_nnn_remain = r_nnn_count;
					if (r_nnn_remain == 0) r_nnn_remain = 1;
					if (param.p->addp == 0) {
						r_nnn_remain = 1;
					} else if (r_nnn_remain > param.p->addp) {
						r_nnn_remain = param.p->addp;
					}

//					PRINT_LOG(param.debug_upconv,"3");

					r_nnn_done = 0;
					for (rip = 1;rip <= r_nnn_count;rip++) {
						if (q_info[0].done_flag[rip] != 2) {
							break;
						}
						r_nnn_done++;
					}

					if (r_nnn_count > 1) {
						sprintf(sss,"[ADDP] %d / %d\n",r_nnn_done,r_nnn_count);PRINT_LOG(param.debug_upconv,sss);
						fprintf(stdout,sss);fflush(stdout);
						fprintf(stdout,sss);fflush(stdout);
						fprintf(stdout,sss);fflush(stdout);
						fprintf(stdout,sss);fflush(stdout);
						fprintf(stdout,sss);fflush(stdout);
					}
					// 変換プロセス起動部分
					r_nnn_done_or_error = 1;
					for (rip = 0;rip < r_nnn_remain;rip++) {
//						PRINT_LOG(param.debug_upconv,"4");
						for (ip = 0;ip < param.channel_count;ip++) {
							// チャンネルごとに子プロセス(upconv.exe)を起動する。
							char s[200];
//							PRINT_LOG(param.debug_upconv,"5");
							
							if (param.p->LRC_process == 1 && ip == 2) {
								sprintf(s,"-process_id:2 -upconv:%d -upconv_total:%d -is:%ld -s:%ld -iw:%d -w:%d -LRC_process_Center:1",ip + 1,r_nnn_remain * param.channel_count,param.p->inSampleR,param.p->outSampleR,param.p->iw,param.p->w);
							} else {
								sprintf(s,"-process_id:2 -upconv:%d -upconv_total:%d -is:%ld -s:%ld -iw:%d -w:%d",ip + 1,r_nnn_remain * param.channel_count,param.p->inSampleR,param.p->outSampleR,param.p->iw,param.p->w);
							}
							if (argc != 4) {
								p1 = strstr(arg[4],"-process_id:");
								if (p1 != NULL) {
									sprintf(p1,s);
								} else {
									strcat(arg[4],s);
								}
							} else {
								p1 = strstr(arg[3],"-process_id:");
								if (p1 != NULL) {
									sprintf(p1,s);
								} else {
									strcat(arg[3],s);
								}
							}
							PRINT_LOG("upconv",s);
							q_info[ip].startexec_info[rip].r_nnn = get_next_r_nnn(q_info[ip].done_flag,r_nnn_count);
//							sprintf(sss,"get_next_r_nnn:%d",q_info[ip].startexec_info[rip].r_nnn);
//							PRINT_LOG(param.debug_upconv,sss);
							if (q_info[ip].startexec_info[rip].r_nnn != -1) {
								r_nnn_done_or_error = 0;
								sprintf(s," -r_nnn:%d ",q_info[ip].startexec_info[rip].r_nnn);
								if (argc != 4) {
									strcat(arg[4],s);
								} else {
									strcat(arg[3],s);
								}
								if (param.p->mc_flag) {
									sprintf(s,"-ms:%d ",param.p->outSampleR * 2);
									strcat(arg[4],s);
								}
								if (ip == 0 && rip == 0) {
									if (argc != 4) {
										p1 = strstr(arg[4],"-print_stdout");
										if (p1 == NULL) {
											strcat(arg[4],"-print_stdout");
										}
									} else {
										p1 = strstr(arg[3],"-print_stdout");
										if (p1 == NULL) {
											strcat(arg[3],"-print_stdout");
										}
									}
#ifdef _WIN_CP
									q_info[ip].startexec_info[rip].sp_info.flag_stdout = 1;
#endif
								} else {
									if (argc != 4) {
										p1 = strstr(arg[4],"-print_stdout");
										if (p1 != NULL) {
											strcpy(p1,"             ");
										}
									} else {
										p1 = strstr(arg[3],"-print_stdout");
										if (p1 != NULL) {
											strcpy(p1,"             ");
										}
									}
#ifdef _WIN_CP
									q_info[ip].startexec_info[rip].sp_info.flag_stdout = 0;
#endif
								}
//								sprintf(sss,"start_exec(upconv.exe:%d,%d)",rip,ip);
//								PRINT_LOG(param.debug_upconv,sss);
//								PRINT_LOG(param.debug_upconv,"6");
								if (start_exec(arg_count,arg,param.p->cpu_pri,&q_info[ip].startexec_info[rip].sp_info) == -1) {
									retCode = STATUS_EXEC_FAIL;param.errLine = __LINE__;
									break;
								}
//								PRINT_LOG(param.debug_upconv,"7");

								q_info[ip].startexec_info[rip].state = 1;
								q_info[ip].done_flag[q_info[ip].startexec_info[rip].r_nnn] = 1;	// 変換中
							}
						}
						if (retCode != 0) {
//							PRINT_LOG(param.debug_upconv,"retCode != 0");
							break;
						}
					}
					if (retCode != 0) break;
//					PRINT_LOG(param.debug_upconv,"8");
					
					if (r_nnn_done_or_error == 1) {
//						PRINT_LOG(param.debug_upconv,"r_nnn_done_or_error == 1");
						break;
					}

//					PRINT_LOG(param.debug_upconv,"9");
					PRINT_LOG(param.debug_upconv,"stdout process");
					flag_done = 0;
					// 変換中。進行状況の取得(標準出力)
					for (;flag_done == 0;) {
						for (rip = 0;rip < r_nnn_remain;rip++) {
							for (ip = 0;ip < param.channel_count;ip++) {
								if (!(rip == 0 && ip == 0)) {
									// 読まない
								} else {
									// 1番目のupconv.exeのみ標準出力を取得し再び出力する。
									read_size = read_stdout(buffer,2000,&q_info[ip].startexec_info[rip].sp_info);
									if (read_size < 0) {
										flag_done = 1;
										break;
									}
									if (read_size == 0) {
										continue;
									}
									buffer[read_size] = '\0';
									fprintf(stdout,"%s",buffer);
									fflush(stdout);
									//PRINT_LOG(param.debug_upconv,buffer);
								}
							}
							if (flag_done == 1) break;
						}
					}
//					PRINT_LOG(param.debug_upconv,"read_stdout(upconv.exe):done");

					// 実行した upconv.exeプロセス終了待ち
					sp_i = 0;
					for (rip = 0;rip < r_nnn_remain;rip++) {
						for (ip = 0;ip < param.channel_count;ip++) {
							if (q_info[ip].startexec_info[rip].state == 1) {
								memcpy(&sp_info[sp_i++],&q_info[ip].startexec_info[rip].sp_info,sizeof (STARTEXEC_PROCESS_INFO));
							}
						}
					}

//					sprintf(sss,"wait_exec 2nd (upconv.exe):%d",sp_i);
//					PRINT_LOG(param.debug_upconv,sss);
					if (sp_i > 0) {
						wait_exec(&sp_info[0],sp_i);
//						PRINT_LOG(param.debug_upconv,"wait_exec 2nd (upconv.exe):done");
						for (rip = 0;rip < r_nnn_remain;rip++) {
							for (ip = 0;ip < param.channel_count;ip++) {
								if (q_info[ip].startexec_info[rip].state == 1) {
									finish_exec(&q_info[ip].startexec_info[rip].sp_info);
									q_info[ip].startexec_info[rip].state = 0;
//									sprintf(sss,"done_flag index:%d",q_info[ip].startexec_info[rip].r_nnn);
//									PRINT_LOG(param.debug_upconv,sss);
									q_info[ip].done_flag[q_info[ip].startexec_info[rip].r_nnn] = 2;
								}
							}
						}
					}

//					PRINT_LOG(param.debug_upconv,"upconv.exe end loop end");
#if 0
					for (ip = 0;ip < param.channel_count;ip++) {
						char str_done_flag[100];
						int done_i;
						sprintf(sss,"startexec_info upconv:0,ip:%d,%d,%d,%d,%d,%d,%d,%d,%d",ip,q_info[ip].startexec_info[0].state,q_info[ip].startexec_info[1].state,q_info[ip].startexec_info[2].state,q_info[ip].startexec_info[3].state,q_info[ip].startexec_info[4].state,q_info[ip].startexec_info[5].state,q_info[ip].startexec_info[6].state,q_info[ip].startexec_info[7].state);
						PRINT_LOG(param.debug_upconv,sss);
						sprintf(sss,"done_flag upconv:0,ip=%d:",ip);
						for (done_i = 0;done_i < r_nnn_count;done_i++) {
							sprintf(str_done_flag,"%d",q_info[ip].done_flag[done_i + 1]);
							strcat(sss,str_done_flag);
						}
						PRINT_LOG(param.debug_upconv,sss);
					}
#endif
				} while (1);

				if (param.p->prof_mode == 2) {
					strcpy(work,argv[2]);
					strcat(work,".txt");
					fp = fopen(work,"w");
					if (fp) {
						fprintf(fp,"%s \"%s\" \"%s\" \"%s\" \"%s\"\n",arg[0],arg[1],arg[2],arg[3],arg[4]);
						fclose(fp);
						exit(0);
					}
				}

				// raw -> wav 変換部分
				param.upconv = 0;
				PRINT_LOG(param.debug_upconv,"to_wav_main");
				if (chkAbort(0,0)) {
					abort_flag = 1;
					break;
				}

#if 0
				r1_size = 0;
				r2_size = 0;
				r3_size = 0;
				r4_size = 0;
				r5_size = 0;
				r6_size = 0;

				_splitpath(argv[2],drive,dir,fname,ext);
				_makepath(tmppath,drive,dir,fname,"r1");
				PRINT_LOG(param.debug_upconv,tmppath);
				fio_open(&fp_r1,tmppath,FIO_MODE_R);
				if (fp_r1.error == 0) {
					fio_get_filesize(&fp_r1,&r1_size);
					fio_close(&fp_r1);
				} else {
					PRINT_LOG(param.debug_upconv,"Error");
				}
				if (param.channel_count >= 2) {
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(tmppath,drive,dir,fname,"r2");
					PRINT_LOG(param.debug_upconv,tmppath);
					fio_open(&fp_r2,tmppath,FIO_MODE_R);
					if (fp_r2.error == 0) {
						fio_get_filesize(&fp_r2,&r2_size);
						fio_close(&fp_r2);
					} else {
						PRINT_LOG(param.debug_upconv,"Error");
					}
				}
				if (param.channel_count >= 3) {
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(tmppath,drive,dir,fname,"r3");
					PRINT_LOG(param.debug_upconv,tmppath);
					fio_open(&fp_r3,tmppath,FIO_MODE_R);
					if (fp_r3.error == 0) {
						fio_get_filesize(&fp_r3,&r3_size);
						fio_close(&fp_r3);
					} else {
						PRINT_LOG(param.debug_upconv,"Error");
					}
				}
				if (param.channel_count >= 4) {
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(tmppath,drive,dir,fname,"r4");
					PRINT_LOG(param.debug_upconv,tmppath);
					fio_open(&fp_r4,tmppath,FIO_MODE_R);
					if (fp_r4.error == 0) {
						fio_get_filesize(&fp_r4,&r4_size);
						fio_close(&fp_r4);
					} else {
						PRINT_LOG(param.debug_upconv,"Error");
					}
				}
				if (param.channel_count >= 5) {
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(tmppath,drive,dir,fname,"r5");
					PRINT_LOG(param.debug_upconv,tmppath);
					fio_open(&fp_r5,tmppath,FIO_MODE_R);
					if (fp_r5.error == 0) {
						fio_get_filesize(&fp_r5,&r5_size);
						fio_close(&fp_r5);
					} else {
						PRINT_LOG(param.debug_upconv,"Error");
					}
				}
				if (param.channel_count >= 6) {
					_splitpath(argv[2],drive,dir,fname,ext);
					_makepath(tmppath,drive,dir,fname,"r6");
					PRINT_LOG(param.debug_upconv,tmppath);
					fio_open(&fp_r6,tmppath,FIO_MODE_R);
					if (fp_r6.error == 0) {
						fio_get_filesize(&fp_r6,&r6_size);
						fio_close(&fp_r6);
					} else {
						PRINT_LOG(param.debug_upconv,"Error");
					}
				}
				if (r1_size == 0) {
					PRINT_LOG(param.debug_upconv,"r1_size=0");
					retStatus = STATUS_FILE_WRITE_ERR;
					param.errLine = __LINE__;
					break;
				}
				if (param.channel_count >= 2) {
					if (r1_size != r2_size) {
						PRINT_LOG(param.debug_upconv,"r1_size!=r2_size");
						retStatus = STATUS_FILE_WRITE_ERR;
						param.errLine = __LINE__;
						break;
					}
				}
				if (param.channel_count >= 3) {
					if (r1_size != r3_size) {
						PRINT_LOG(param.debug_upconv,"r1_size!=r3_size");
						retStatus = STATUS_FILE_WRITE_ERR;
						param.errLine = __LINE__;
						break;
					}
				}
				if (param.channel_count >= 4) {
					if (r1_size != r4_size) {
						PRINT_LOG(param.debug_upconv,"r1_size!=r4_size");
						retStatus = STATUS_FILE_WRITE_ERR;
						param.errLine = __LINE__;
						break;
					}
				}
				if (param.channel_count >= 5) {
					if (r1_size != r5_size) {
						PRINT_LOG(param.debug_upconv,"r1_size!=r5_size");
						retStatus = STATUS_FILE_WRITE_ERR;
						param.errLine = __LINE__;
						break;
					}
				}
				if (param.channel_count >= 6) {
					if (r1_size != r6_size) {
						PRINT_LOG(param.debug_upconv,"r1_size!=r6_size");
						retStatus = STATUS_FILE_WRITE_ERR;
						param.errLine = __LINE__;
						break;
					}
				}
#endif
				retStatus = to_wav_main(arg_count,arg,param.p);
				if (retStatus != STATUS_SUCCESS) {
					break;
				}
//				PRINT_LOG(param.debug_upconv,"to_wav_main:done");
				paramFlag = 0x0007;
				PRINT_LOG(param.debug_upconv,"14");
				if (param.p->prof_mode == 3) {
					strcpy(work,argv[2]);
					strcat(work,".txt");
					fp = fopen(work,"w");
					if (fp) {
						fprintf(fp,"%s \"%s\" \"%s\" \"%s\" \"%s\"\n",arg[0],arg[1],arg[2],arg[3],arg[4]);
						fclose(fp);
						exit(0);
					}
				}

			} else if (param.process_id == 2 && param.upconv > 0) {
//				PRINT_LOG(param.debug_upconv,"upconv.exe SamplingRate");

#ifdef FFTW_MULTI_THREAD
			int nCpu;
			char m[200];
			nCpu = param.p->thread;
			if (param.p->upconv_total > 0) {
				nCpu /= param.p->upconv_total;
				if (nCpu < 2) {
					nCpu = 2;
				}
			}
			if (param.p->fftw_multi_thread == 1) {
				sprintf(m,"omp_get_max_threads:%d,upconv_total:%d",nCpu,param.p->upconv_total);
				PRINT_LOG("upconv",m);
				omp_set_num_threads(nCpu);
			}
#endif

				if (chkAbort(0,0)) {
					abort_flag = 1;
					break;
				}


				if (param.p->hfc != -1) {
					param.p->hfc_auto = 0;
				}

				if (param.p->simple_mode == 1) {
					param.p->abe = 1;
					param.p->cutLowData = 1;
					param.p->smLowData = 1;
					param.p->abeFnLevel = 2;
					param.p->abe_ad_filter = 0;
					param.p->abe_version = 4;
					param.p->abe_declip = 0;
					param.p->abe_declick = 0;
					param.p->abe_clld_level = 3;
					param.p->abe_10x_cut_level = 0;
					param.p->abe_10x_cut_mode = 0;
				}

				_splitpath(workpath,workdrive,workdir,workfname,workext);
				_splitpath(argv[2],drive,dir,fname,ext);
				if (strlen(workpath) >= 3) {
					_makepath(workpath,workdrive,workdir,fname,ext);
					rn_fname = workpath;
				} else {
					rn_fname = argv[2];
				}

				// 入力ファイル名の生成
				_splitpath(rn_fname,drive,dir,fname,ext);
				sprintf(ext,"r%d.%03d",param.upconv,param.r_nnn);
				_makepath(rn_fname,drive,dir,fname,ext);
				paramFlag = 0x0007;
				if (paramFlag == 0x0007 || (paramFlag == 0x0005 && param.p->no_rate_conv != -1)) {
					SamplingRateConvert(argc,argv,rn_fname,&param);
					sprintf(sss,"upconv:%d",param.upconv);
					PRINT_LOG(param.debug_upconv,sss);
					if (param.err == STATUS_SUCCESS) {
						double nx;
						double per,org_per;
						double nx_org,nx_upconv;
						SSIZE org_avg;
						SSIZE org_max;
						FIO fp_r;
						FIO fp_w;
						FIO_COMMON_PARAM fio_common;
						memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));

						if (param.p->fio_sec_size > 0) {
							// 入力のn秒分
							fio_common.sector_size = param.p->outSampleR * sizeof (SSIZE);
							fio_common.sector_n    = param.p->fio_sec_size;
							fio_common.upconv_n    = 1;
						} else {
							// n MB 分
							fio_common.sector_size = (fio_size)1 * 1000 * 1000;
							fio_common.sector_n    = param.p->fio_mb_size;
							fio_common.upconv_n    = 1;
						}

						memset(&fp_r,0,sizeof (FIO));
						memset(&fp_w,0,sizeof (FIO));
						PRINT_LOG(param.debug_upconv,"volumeAdjustFile filename:");
						PRINT_LOG(param.debug_upconv,rn_fname);
						//fio_open(&fp_r,rn_fname,FIO_MODE_R);
						fio_open_with_set_param(&fp_r,rn_fname,FIO_MODE_R,&fio_common);
						if (fp_r.error) {
							param.err = STATUS_FILE_READ_ERR;param.errLine = __LINE__;
							break;
						}
						strcpy(tmppath,rn_fname);
						strcat(tmppath,".tmp");
						//fio_open(&fp_w,tmppath,FIO_MODE_W);
						fio_open_with_set_param(&fp_w,tmppath,FIO_MODE_W,&fio_common);
						if (fp_w.error) {
							param.err = STATUS_FILE_READ_ERR;param.errLine = __LINE__;
							break;
						}
						// 元音声の音量を記録する。
						if (NormInfoOrg.max < 0) {
							NormInfoOrg.max *= -1;
						}
						if (NormInfoOrg.min < 0) {
							NormInfoOrg.min *= -1;
						}
						max = NormInfoOrg.max;
						if (max < NormInfoOrg.min) {
							max = NormInfoOrg.min;
						}
						org_per = (double)max / (double)0x7FFFFFFFFFFFFF;
						org_avg = NormInfoOrg.avg << 40;
						org_max = max;
						strcpy(tmppath,rn_fname);
						strcat(tmppath,".param");
						fp = fopen(tmppath,"w");
						if (fp == NULL) {
							param.err = STATUS_FILE_WRITE_ERR;param.errLine = __LINE__;
							break;
						}
						// 元々の音量データ
						if (fprintf(fp,"%.10lf,%llx,%lf,%lf\n",org_per,org_avg,1.0,1.0) == EOF) {
							param.err = STATUS_FILE_WRITE_ERR;param.errLine = __LINE__;
							break;
						}

						// upconv で変換したあとの音量データ
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
						per = (double)max / (double)0x7FFFFFFFFFFFFF;
						avg = NormInfo.avg << 40;
						if (fprintf(fp,"%.10lf,%llx,%lf,%lf\n",per,avg,param.max_level2,param.p->vla_nx) == EOF) {
							param.err = STATUS_FILE_WRITE_ERR;param.errLine = __LINE__;
							break;
						}

						nx = 0;
						if (avg > 10000000000) {
							nx = (double)org_avg / avg;
							if (nx > 0) {
								max = (SSIZE)((double)org_max / nx);
							}
						}
						nx = 1.0;
						if (max > 0) {
							nx = (double)org_max / max;
						}

						if (param.p->vla_auto == 1) {
							double clip_rate;
							clip_rate = 0;
							for (param.p->vla_nx = (double)1;param.p->vla_nx >= (double)0.5;param.p->vla_nx -= (double)0.05) {
								fio_rewind(&fp_r);
								adjVLA(param.outSample,&fp_r,nx,param.p->vla_nx,&clip_rate,&param);
								sprintf(sss,"VLA(%lf) clip_rate:%lf",param.p->vla_nx,clip_rate);
								PRINT_LOG(param.debug_upconv,sss);
								if ((int)clip_rate == 0) break;
								
							}
							nx = (nx * param.max_level2) * (double)0.5;
						} else {
							nx = (nx * param.max_level2) * param.p->vla_nx;
						}
						sprintf(sss,"upconv:%d,outSample:%lld",param.upconv,param.outSample);
						PRINT_LOG(param.debug_upconv,sss);
						sprintf(sss,"volumeAdjustFile:nx:%lf",nx);
						PRINT_LOG(param.debug_upconv,sss);
						volumeAdjustFile(nx,&fp_r,&fp_w,param.outSample,&param,&NormInfoFinal);
						if (param.err != STATUS_SUCCESS) {
							break;
						}
						if (param.p->eq_analyze > 0) {
							for (i = 0;i < 384000;i++) {
								param.analyze_eq_pw[i] *= nx;
							}
						}
						fio_close(&fp_r);
						fio_setmode_r(&fp_w,&fp_r,rn_fname);
						if (fp_w.error) {
							param.err = STATUS_FILE_WRITE_ERR;param.errLine = __LINE__;
							break;
						}
						if (fp_r.error) {
							param.err = STATUS_FILE_READ_ERR;param.errLine = __LINE__;
							break;
						}
						fio_get_filesize(&fp_r,&size);
						sprintf(sss,"upconv:%d,volueAdjustFile fp_r size:%lld",param.upconv,size);
						PRINT_LOG(param.debug_upconv,sss);
						if (NormInfoFinal.max < 0) {
							NormInfoFinal.max *= -1;
						}
						if (NormInfoFinal.min < 0) {
							NormInfoFinal.min *= -1;
						}
						max = NormInfoFinal.max;
						if (max < NormInfoFinal.min) {
							max = NormInfoFinal.min;
						}
						per = (double)max / (double)0x7FFFFFFFFFFFFF;
						avg = NormInfoFinal.avg << 40;
						// upconv で変換したあとに音量を調整したあとのデータ
						sprintf(sss,"NormInfoFinal:%.10lf,%llx",per,avg);
						PRINT_LOG(param.debug_upconv,sss);

						if (fprintf(fp,"%.10lf,%llx,%lf,%lf\n",per,avg,param.max_level2,param.p->vla_nx) == EOF) {
							param.err = STATUS_FILE_WRITE_ERR;param.errLine = __LINE__;
							break;
						}
						fclose(fp);
						if (param.p->eq_analyze > 0 && param.p->index_pre_eq > 0 && param.upconv <= 2) {
							if (param.p->eq_analyze == 1) {
								sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze1.r%d",param.p->index_pre_eq,param.upconv);
							} else if (param.p->eq_analyze == 2) {
								sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze2.r%d",param.p->index_pre_eq,param.upconv);
							}
							fp = fopen(tmppath,"wt");
							if (fp != NULL) {
								double p;
								if ((param.p->inSampleR / 2) < 384000) {
									for (i = param.p->inSampleR / 2;i < 384000;i++) {
										param.analyze_eq_pw[i] = 1.0;
									}
								}
								for (i = 0;i < 384000;i++) {
									fprintf(fp,"%lf\n",param.analyze_eq_pw[i]);
								}
								sprintf(sss,"EQ Analyze output(upconv):%s",tmppath);
								PRINT_LOG(param.debug_upconv,sss);
								fclose(fp);
								param.p->analyze_filename = malloc(strlen(tmppath) + 1);
								if (param.p->analyze_filename != NULL) {
									strcpy(param.p->analyze_filename,tmppath);
								}
							}
						}
					}
				}
			}
		}
#ifdef FFTW_MULTI_THREAD
		fftw_cleanup_threads();
#endif
		if (!(argc == 4 || argc == 5 || argc == 6) || !(paramFlag == 0x0007 || (paramFlag == 0x0005 && param.p->no_rate_conv != -1))) {
			PRINT_LOG(param.debug_upconv,"argc err");
			printf(STR_COPYRIGHT);
			printf(STR_USAGE);
			exit(0);
		}
	} while (0);

	if (abort_flag == 0 && param.err != STATUS_SUCCESS) {
		fp = fopen(abortInfo.error_filename,"a");
		if (fp) {
			switch (param.err) {
				case STATUS_PARAMETER_ERR:
					fprintf(fp,"%s[%d]:Parameter error.\n",param.upconv == 0 ? "upconv_main" : "upconv",param.errLine);
					break;
				case STATUS_FILE_READ_ERR:
					fprintf(fp,"%s[%d]:File read error.\n",param.upconv == 0 ? "upconv_main" : "upconv",param.errLine);
					break;
				case STATUS_FILE_WRITE_ERR:
					fprintf(fp,"%s[%d]:File write error.\n",param.upconv == 0 ? "upconv_main" : "upconv",param.errLine);
					break;
				case STATUS_MEM_ALLOC_ERR:
					fprintf(fp,"%s[%d]:Memory Allocation error.\n",param.upconv == 0 ? "upconv_main" : "upconv",param.errLine);
					break;
				default:
					fprintf(fp,"%s[%d]:Other error.\n",param.upconv == 0 ? "upconv_main" : "upconv",param.errLine);
			}
			fclose(fp);
		}
		PRINT_LOG(param.debug_upconv,"param.err");
	}
	if (sss[2048] != '?' || sss[2049] != '?' || sss[2050] != '?' || sss[2051] != '?' || sss[2052] != '?' || sss[2053] != '?' || sss[2054] != '?' || sss[2055] != '?' || sss[2056] != '?' || sss[2057] != '?') {
		PRINT_LOG(param.debug_upconv,"sss buffer crush!");
	}

	exit(retCode);
	return 0;
}
//---------------------------------------------------------------------------
// Function     : get_next_r_nnn
// Description  : 次に処理すべき分割されたr_nnnファイルを返す。
//                無ければ-1
//	done_flag   : 完了状態を示す配列
//	r_nnn_count	: 分割数
//
int get_next_r_nnn(int *done_flag,int r_nnn_count)
{
	int i;
	for (i = 1;i <= r_nnn_count;i++) {
		if (done_flag[i] == 0) break;
	}
	
	if (i <= r_nnn_count) {
		return i;
	} else {
		return -1;
	}
}
//---------------------------------------------------------------------------
// Function   : SamplingRateConvert
// Description: サンプリングレート変換処理をする
// ---
//	rawFile	: RAWデータファイル名
//	param	: 変換パラメータ構造体
//
void SamplingRateConvert(int argc,char *argv[],char *rawFile,PARAM_INFO *param)
/*
 *	サンプリングレート変換
 */
{
	char outFile[_MAX_PATH];
	char tmpFile[_MAX_PATH];
	char ms4File[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char workstr[2048];
	char tmppath[_MAX_PATH];
	char work[2048];
	char sss[2058];
	long temp,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9;
	FIO fp_r;
	FIO fp_w;
	FIO fp_r2;
	FILE *fp;
	fio_size size,size2;
	fio_size file_size;
	int c;
	int declip_flag = 0;

	SSIZE inSample,outSample;
	int hfc;
	long double dd;
	DWORD svInSampleR;
	int svIw;
	int downSampleFlag = FALSE;
	DWORD svOutSampleR;
	int validIndex;
	int i;
	int abort_flag;
	static FFT_PARAM fftParam;
	SFA_INFO sfa_info;
	FIO_COMMON_PARAM fio_common;
	memset(&fio_common,0,sizeof (FIO_COMMON_PARAM));

	if (param->p->fio_sec_size > 0) {
		// 入力のn秒分
		fio_common.sector_size = param->p->inSampleR * sizeof (SSIZE);
		fio_common.sector_n    = param->p->fio_sec_size;
		fio_common.upconv_n    = 1;
	} else {
		// n MB 分
		fio_common.sector_size = (fio_size)1 * 1000 * 1000;
		fio_common.sector_n    = param->p->fio_mb_size;
		fio_common.upconv_n    = 1;
	}

	PRINT_FN_LOG(param->debug_upconv,"SamplingRateConvert:Start");

	memset(&sfa_info,0,sizeof (SFA_INFO));

	if (param->p->mc_flag) {
		param->p->outSampleR = param->p->outSampleR_ms;
	}

	// 初期処理
	diskBuffer1	= (SSIZE *)calloc(4 * 1024 * 1024L,sizeof (SSIZE));
	diskBuffer2 = (SSIZE *)calloc(4 * 1024 * 1024L,sizeof (SSIZE));

	ms4File[0] = '\0';

	param->declip_eq = (double *)malloc(384001 * sizeof (double));
	param->hfa_eq = (double *)malloc(384001 * sizeof (double));
	param->pre_eq = (double *)malloc(384001 * sizeof (double));
	param->post_eq = (double *)malloc(384001 * sizeof (double));
	param->post_eq2 = (double *)malloc(384001 * sizeof (double));
	param->analyze_eq_pw = (double *)malloc(384001 * sizeof (double));
	if (diskBuffer1 == NULL || diskBuffer2 == NULL || param->declip_eq == NULL || param->hfa_eq == NULL || param->pre_eq == NULL || param->post_eq == NULL || param->post_eq2 == NULL || param->analyze_eq_pw == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		param->errLine = __LINE__;
		return;
	}

	for (i = 0;i < 384000;i++) {
		param->declip_eq[i]     = 1.0;
		param->hfa_eq[i]        = 1.0;
		param->pre_eq[i]        = 1.0;
		param->post_eq[i]       = 1.0;
		param->post_eq2[i]       = 1.0;
		param->analyze_eq_pw[i] = 0;
	}

	// overwrite
	if (param->p->enable_ovwrite_sw == 1) {
		param->p->outSampleR = param->p->ovwrite_s;
		param->p->validOutSampleR = param->p->ovwrite_s;
		param->p->outSampleR_final = param->p->ovwrite_s;
		param->p->w = param->p->ovwrite_w;
		param->p->bitwidth_int = param->p->ovwrite_w_int;
	}
	if (param->p->enable_ovwrite_hfa == 1) {
		param->p->hfa = param->p->ovwrite_hfa;
	}
	if (param->p->enable_ovwrite_norm == 1) {
		param->p->norm = param->p->ovwrite_norm;
	}
	
	if (param->p->split_size != 0 || param->p->no_rate_conv >= 0) {
		// 分割時、またはeq_mode時はパラメータを固定にする
		if (param->p->no_rate_conv == -1) {
			param->p->outSampleR = param->p->inSampleR;
			param->p->validOutSampleR = param->p->inSampleR;
			param->p->w = param->p->iw;
		}
		param->p->hfa = 0;
		param->p->hfc = -1;
		param->p->lfc = -1;
		param->p->lpf = -1;
		param->p->nr = -1;
		param->p->abe = 0;
		param->p->post_abe = 0;
		param->p->cut_high_dither = 0;
		param->p->low_adjust = 0;
		param->p->hfc_auto = 0;
		param->p->vla_auto = 0;
		param->p->vla_nx = 1.0;
		if (param->p->split_size != 0) {
			PRINT_LOG(param->debug_upconv,"reset enable_pre_eq");
			param->p->enable_pre_eq = 0;
			param->p->oversamp = 0;
		}
		param->p->enable_post_eq = 0;
		param->p->mc_flag = 0;
	}

	// r1 ファイルかをチェックする(sp_ana)
	if (param->upconv == 1) {
		param->r1_flag = 1;
	}

#if 0
	// 未使用
	// GenNoise 用のデータパス
	_splitpath(argv[0],drive,dir,fname,ext);
	_makepath(tmppath,drive,dir,"nd","dat");
	param->nd_path = malloc(strlen(tmppath) + 1);
	if (param->nd_path != NULL) {
		strcpy(param->nd_path,tmppath);
	}
#endif

#if 0
	// ビット拡張テーブル
	_splitpath(argv[0],drive,dir,fname,ext);
	_makepath(tmppath,drive,dir,"bit_extend_table","");
	
	fp = fopen(tmppath,"r");
	if (fp) {
		for (i = 0;i < 256;i++) {
			if (fscanf(fp,"%d,",&c) == 1) {
				param->beInfo.cutOff[i] = c;
			}
		}
		fclose(fp);
	}
#else
	for (i = 0;i < 256;i++) {
		param->beInfo.cutOff[i] = beinfo_init[i];
	}

#endif

#if 0
	// declip イコライザ値
	_splitpath(argv[0],drive,dir,fname,ext);
	_makepath(tmppath,drive,dir,"declip_eq","dat");
	fp = fopen(tmppath,"r");
	if (fp) {
		double dp;
		for (i = 0;i < 192000;i++) {
			if (fscanf(fp,"%lf",&dp) == 1) {
				param->declip_eq[i] = dp;
			}
		}
		for (;i < 384000;i++) {
			param->declip_eq[i] = param->declip_eq[192000-1];
		}
		fclose(fp);
	}
#else
	for (i = 0;i < 192000;i++) {
		param->declip_eq[i] = declip_eq_init[i];
	}
	for (;i < 384000;i++) {
		param->declip_eq[i] = param->declip_eq[192000-1];
	}
#endif

	// upconv_eq 用
	if (param->p->index_pre_eq > 0) {
		if (param->p->eq_analyze == 1) {
			sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze1",param->p->index_pre_eq);
		} else if (param->p->eq_analyze == 2) {
			sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.analyze2",param->p->index_pre_eq);
		}
		unlink(tmppath);
	}

	if (param->p->enable_pre_eq == 1 && param->p->index_pre_eq >= 0) {
		if (param->p->eq_analyze == 0) {
			_splitpath(argv[3],drive,dir,fname,ext);
			_makepath(tmppath,drive,"","","");
			sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.txt",param->p->index_pre_eq);
			fp = fopen(tmppath,"r");
			if (fp) {
				double dp;
				PRINT_LOG(param->debug_upconv,tmppath);
				for (i = 0;i < 192000;i++) {
					if (fscanf(fp,"%lf",&dp) == 1) {
						param->pre_eq[i] = dp;
					}
				}
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->pre_eq[i] = dp;
					i++;
					for (;i < 384000;i++) {
						if (fscanf(fp,"%lf",&dp) == 1) {
							param->pre_eq[i] = dp;
						}
					}
				} else {
					for (;i < 384000;i++) {
						param->pre_eq[i] = param->pre_eq[192000-1];
					}
				}
				fclose(fp);
			}
		} else {
			for (i = 0;i < 384000;i++) {
				param->pre_eq[i] = 1.0;
			}
		}
	}
	if (param->p->enable_post_eq == 1 && param->p->index_post_eq > 0) {
		_splitpath(argv[3],drive,dir,fname,ext);
		_makepath(tmppath,drive,"","","");
		sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.txt",param->p->index_post_eq);
		fp = fopen(tmppath,"r");
		if (fp) {
			double dp;
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->post_eq[i] = dp;
				}
			}
			if (fscanf(fp,"%lf",&dp) == 1) {
				param->post_eq[i] = dp;
				i++;
				for (;i < 384000;i++) {
					if (fscanf(fp,"%lf",&dp) == 1) {
						param->post_eq[i] = dp;
					}
				}
			} else {
				for (;i < 384000;i++) {
					param->post_eq[i] = param->post_eq[192000-1];
				}
			}
			fclose(fp);
		}
	}
	if (param->p->enable_post_eq2 == 1 && param->p->index_post_eq2 > 0) {
		_splitpath(argv[3],drive,dir,fname,ext);
		_makepath(tmppath,drive,"","","");
		sprintf(tmppath,"C:\\ProgramData\\upconv_eq\\upconv_eq_%03d.txt",param->p->index_post_eq2);
		fp = fopen(tmppath,"r");
		if (fp) {
			double dp;
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->post_eq2[i] = dp;
				}
			}
			if (fscanf(fp,"%lf",&dp) == 1) {
				param->post_eq2[i] = dp;
				i++;
				for (;i < 384000;i++) {
					if (fscanf(fp,"%lf",&dp) == 1) {
						param->post_eq2[i] = dp;
					}
				}
			} else {
				for (;i < 384000;i++) {
					param->post_eq2[i] = param->post_eq2[192000-1];
				}
			}
			fclose(fp);
		}
	}
#if 0
	_splitpath(argv[0],drive,dir,fname,ext);
	_makepath(tmppath,drive,dir,"hfa_eq","dat");
	fp = fopen(tmppath,"r");
	if (fp) {
		double dp;
		if (param->p->hfa_level == 1) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->hfa_eq[i] = dp;
				}
			}
		} else if (param->p->hfa_level == 2) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->hfa_eq[i] = dp * dp;
				}
			}
		} else if (param->p->hfa_level == 3) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->hfa_eq[i] = dp * dp * dp;
				}
			}
		} else if (param->p->hfa_level == 4) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->hfa_eq[i] = dp * dp * dp * dp * dp * dp;
				}
			}
		} else if (param->p->hfa_level == 5) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->hfa_eq[i] = dp * dp * dp * dp * dp * dp * dp * dp;
				}
			}
		} else if (param->p->hfa_level == 6) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
					param->hfa_eq[i] = dp;
				}
			}
		}
		for (;i < 384000;i++) {
			param->hfa_eq[i] = param->hfa_eq[192000-1];
		}
		fclose(fp);
	}
#else
	if (1) {
		double dp;
		if (param->p->hfa_level == 1) {
			for (i = 0;i < 192000;i++) {
				dp = hfa_eq_init[i];
				param->hfa_eq[i] = dp;
			}
		} else if (param->p->hfa_level == 2) {
			for (i = 0;i < 192000;i++) {
				dp = hfa_eq_init[i];
				param->hfa_eq[i] = dp * dp;
			}
		} else if (param->p->hfa_level == 3) {
			for (i = 0;i < 192000;i++) {
				dp = hfa_eq_init[i];
				param->hfa_eq[i] = dp * dp * dp;
			}
		} else if (param->p->hfa_level == 4) {
			for (i = 0;i < 192000;i++) {
				dp = hfa_eq_init[i];
				param->hfa_eq[i] = dp * dp * dp * dp * dp * dp;
			}
		} else if (param->p->hfa_level == 5) {
			for (i = 0;i < 192000;i++) {
				dp = hfa_eq_init[i];
				param->hfa_eq[i] = dp * dp * dp * dp * dp * dp * dp * dp;
			}
		} else if (param->p->hfa_level == 6) {
			for (i = 0;i < 192000;i++) {
				dp = hfa_eq_init[i];
				param->hfa_eq[i] = dp;
			}
		}
		for (;i < 384000;i++) {
			param->hfa_eq[i] = param->hfa_eq[192000-1];
		}
	}
#endif

	// hfa2/3 Preset
	_splitpath(argv[3],drive,dir,fname,ext);
	_makepath(tmppath,drive,dir,"preset","dat");
	PRINT_LOG(param->debug_upconv,"hfa2/3 preset path");
	PRINT_LOG(param->debug_upconv,tmppath);
	fp = fopen(tmppath,"r");
	if (fp) {
		for (i = 1;i <= param->p->hfa_preset;i++) {
			if (fgets(work,2000,fp) != NULL) {
				if (sscanf(work,"%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",workstr,&temp,&temp2,&temp3,&temp4,&temp5,&temp6,&temp7,&temp8,&temp9) == 10) {
					if (temp == 0) {
						param->p->sig1Enb = 0;
					} else {
						param->p->sig1Enb = 1;
					}
					if (temp2 >= 1 && temp2 <= 25) param->p->sig1AvgLineNx = temp2;
					if (temp3 >= -44 && temp3 <= 44) param->p->sig1Phase = temp3;
					if (temp4 == 0) {
						param->p->sig2Enb = 0;
					} else {
						param->p->sig2Enb = 1;
					}
					if (temp5 >= -44 && temp5 <= 44) param->p->sig2Phase = temp5;
					if (temp7 >= 0 && temp7 <= 100)  param->p->hfaNB = temp7;
					if (temp8 == 0) {
						param->p->sig3Enb = 0;
					} else {
						param->p->sig3Enb = 1;
					}
					if (temp9 >= -44 && temp9 <= 44) param->p->sig3Phase = temp9;
				}
			}
		}
		fclose(fp);
	}
	sprintf(work,"HFA Preset:%d,%d,%d,%d,%d,%d,%d,%d",param->p->sig1Enb,param->p->sig1AvgLineNx,param->p->sig1Phase,param->p->sig2Enb,param->p->sig2Phase,param->p->sig3Enb,param->p->sig3Phase,param->p->hfaNB);
	PRINT_LOG(param->debug_upconv,work);

	memset(&fp_r,0,sizeof (FIO));
	memset(&fp_r2,0,sizeof (FIO));
	memset(&fp_w,0,sizeof (FIO));

	memset(&fftParam,0,sizeof (FFT_PARAM));

	if (param->p->hfc == -1 && param->p->inSampleR > param->p->outSampleR) {
		param->p->hfc = param->p->outSampleR / 2;
	}

	if (param->p->eq_analyze > 0 && param->p->index_pre_eq <= 0) {
		param->p->eq_analyze = 0;
	}

	abort_flag = 0;

	fftParam.inSampleR  = param->p->inSampleR;
	fftParam.outSampleR = param->p->outSampleR;
	fftParam.hfc        = param->p->hfc;
	fftParam.lfc        = param->p->lfc;
	fftParam.cut_high_dither   = param->p->cut_high_dither;

	fftParam.eq_ref_max = (double *)malloc(384001 * sizeof (double));
	fftParam.eq_ref_avg = (double *)malloc(384001 * sizeof (double));
	fftParam.eq         = (double *)malloc(384001 * sizeof (double));
	fftParam.lfa_eq     = (double *)malloc(384001 * sizeof (double));
	fftParam.declip_eq  = (double *)malloc(384001 * sizeof (double));
	fftParam.hfa_eq     = (double *)malloc(384001 * sizeof (double));
	fftParam.pre_post_eq = (double *)malloc(384001 * sizeof (double));
	fftParam.analyze_eq_pw = (double *)malloc(384001 * sizeof (double));
	fftParam.eq_ref_count = 0;
	fftParam.deEmphasis = param->p->deEmphasis;
	fftParam.dsd_fmt    = -1;
	fftParam.maxMemorySize  = 0;
	fftParam.abort_percent  = 0;
	fftParam.simple_mode = param->p->simple_mode;
	fftParam.simple_mode_done_percent = 20;
	fftParam.simple_mode_done_percent_div = 5;

	do {
		if (fftParam.eq_ref_max == NULL || fftParam.eq_ref_avg == NULL || fftParam.eq == NULL || fftParam.lfa_eq == NULL || fftParam.declip_eq == NULL || fftParam.hfa_eq == NULL || fftParam.pre_post_eq == NULL || fftParam.analyze_eq_pw == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			break;
		}
		// テンポラリファイル名の作成
//		if (strlen(param->tempPath) > 0) {
//			_splitpath(rawFile,drive,dir,fname,ext);
//			strcpy(dir,"\\");
//			_makepath(outFile,param->tempPath,dir,fname,ext);
//		} else {
			strcpy(outFile,rawFile);
//		}
		strcat(outFile,".tmp");
		strcpy(tmpFile,outFile);
		strcat(tmpFile,"2");

		if (param->p->ms_process == 4) {
			char *ms4_pp;
			FILE *fp_ms4 = NULL;
			FFT_CUTINFO *last_cut_info = NULL;
			FFT_CUTINFO *cutinfo;
			SSIZE ms4_sec;
			long  ms4_start_hz,ms4_count,ms4_sfc;
			long  ms4_temp;
			strcpy(ms4File,outFile);
			_splitpath(ms4File,drive,dir,fname,ext);
			ms4_pp = strrchr(fname,'.');
			PRINT_LOG("ms4_pp",ms4_pp);
			if (ms4_pp != NULL) {
				*ms4_pp = '\0';
				ms4_pp = strrchr(fname,'.');
				if (ms4_pp != NULL) {
					*ms4_pp = '\0';
					PRINT_LOG("ms4_pp",ms4_pp);
				}
			}
			_makepath(tmpFile,drive,dir,fname,"ms4");
			strcpy(ms4File,tmpFile);
			
			sprintf(sss,"upconv:%d,ms4File:%s",param->upconv,ms4File);
			PRINT_LOG("",sss);

			fp_ms4 = fopen(ms4File,"rt");
			if (fp_ms4 != NULL) {
				while (fgets(work,256,fp_ms4) != NULL) {
					ms4_sfc = -1;
					if (sscanf(work,"sfc:%lld,%ld\n",&ms4_sec,&ms4_temp) == 2) {
						if (ms4_temp >= 2000 && ms4_temp <= 20000) {
							cutinfo = malloc(sizeof (FFT_CUTINFO));
							if (cutinfo != NULL) {
								cutinfo->sec = ms4_sec;
								cutinfo->cut_start = 0;
								cutinfo->cut_count = 0;
								cutinfo->sfc = ms4_temp;
								cutinfo->next = NULL;
								if (last_cut_info != NULL) {
									last_cut_info->next = cutinfo;
								} else {
									fftParam.fft_cut_info = cutinfo;
								}
							}
							last_cut_info = cutinfo;
						}
					}
					if (sscanf(work,"%lld,%ld,%ld\n",&ms4_sec,&ms4_start_hz,&ms4_count) == 3) {
						cutinfo = malloc(sizeof (FFT_CUTINFO));
						if (cutinfo != NULL) {
							cutinfo->sec = ms4_sec;
							cutinfo->cut_start = ms4_start_hz;
							cutinfo->cut_count = ms4_count;
							cutinfo->sfc = 0;
							cutinfo->next = NULL;
							if (last_cut_info != NULL) {
								last_cut_info->next = cutinfo;
							} else {
								fftParam.fft_cut_info = cutinfo;
							}
						}
						last_cut_info = cutinfo;
					}
				}
				fclose(fp_ms4);
				cutinfo = fftParam.fft_cut_info;
				while (cutinfo != NULL) {
					sprintf(sss,"cutinfo:%lld,%ld,%d",cutinfo->sec,cutinfo->cut_start,cutinfo->cut_count);
					PRINT_LOG("",sss);
					cutinfo = cutinfo->next;
				}
			}
			strcpy(tmpFile,outFile);
			strcat(tmpFile,"2");
		}
		fftParam.fft_cut_info_flag = 0;
		
		// リード専用ファイルオープン関数(バッファリング機能付)
		sprintf(sss,"upconv:%d,rawFile:fp_r:%s",param->upconv,rawFile);
		PRINT_LOG("",sss);
		sprintf(sss,"upconv:%d,outFile:fp_w:%s",param->upconv,outFile);
		PRINT_LOG("",sss);

		if (param->p->fio_sec_size > 0) {
			fio_common.sector_size = param->p->inSampleR * sizeof (SSIZE);
		}
		fio_common.upconv_n    = 1;
		//fio_open(&fp_r,rawFile,FIO_MODE_R);
		fio_open_with_set_param(&fp_r,rawFile,FIO_MODE_R,&fio_common);
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			break;
		}

		// ファイルサイズ取得
		outSample = 0;

		fio_get_filesize(&fp_r,&size);
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			break;
		}
		sprintf(sss,"upconv:%d,filesize:%lld",param->upconv,size);
		PRINT_LOG("",sss);

		// 出力サンプル数を計算する。
		inSample = (SSIZE)(size / sizeof (SSIZE));
		outSample = inSample;

		if (outSample == 0) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			break;
		}

		sprintf(sss,"upconv:%d, fp_r inSample:%lld",param->upconv,outSample);
		PRINT_LOG("",sss);

#if 0
		// readahead
		//fio_set_memory_limit(&fp_r,50,param->p->fio);
		fio_set_memory_limit(&fp_r,param->p->inSampleR * sizeof (SSIZE),param->p->fio);
		if (param->p->fio_auto == 1) {
			//fio_set_memory_limit(&fp_r,50,5);
			fio_set_memory_limit(&fp_r,param->p->inSampleR * sizeof (SSIZE),5);
			fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
		}
#endif

		// 音量調査
		calcNormInfo(&fp_r,inSample,param,param->normInfoOrg);

		if (param->p->sfa > 0) {
			long sfa_outSampleR;
			int  sfa_hfa3_freq_start;
			int  sfa_hfa3_freq_len;
			int  sfa_hfa3_analyze_step;
			int  sfa_hfa_level_adj_width;
			int  sfa_hfa_level_adj_nx;
			int  sfa_hfa3_sig_level_limit;
			int  sfa_hfa;
			int  sfa_hfa_ver;

			PRINT_LOG("","MS4 SFA");
			sfa_outSampleR = param->p->outSampleR;

			param->p->outSampleR = param->p->inSampleR;
			fftParam.inSampleR  = param->p->inSampleR;
			fftParam.outSampleR = fftParam.inSampleR;

			sfa_hfa3_freq_start = param->p->hfa3_freq_start;
			sfa_hfa3_freq_len       = param->p->hfa3_freq_len;
			sfa_hfa3_analyze_step   = param->p->hfa3_analyze_step;
			sfa_hfa_level_adj_width = param->p->hfa_level_adjust_width;
			sfa_hfa_level_adj_nx    = param->p->hfa_level_adjust_nx;
			sfa_hfa3_sig_level_limit = param->p->hfa3_sig_level_limit;
			sfa_hfa = param->p->hfa;
			sfa_hfa_ver = param->p->hfa3_version;
			param->p->hfa = 3;
			param->p->hfa3_version = 3;
			param->p->hfa3_freq_start = 7000;
			param->p->hfa3_freq_len   = 3000;
			param->p->hfa3_analyze_step = 193;
			param->p->hfa_level_adjust_width = 2000;
			param->p->hfa_level_adjust_nx = 95;
			param->p->hfa3_sig_level_limit = 12000;

			if (param->print_stdout == 1) {
				if (param->p->simple_mode == 0) {
					strcpy(param->pr_str,"[MS4]");
					fprintf(stdout,"\n");
					fprintf(stdout,"%s0%%\n",param->pr_str);
					fflush(stdout);
				}
			}

			hfc = param->p->inSampleR / 2;

			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}

			// ファイルに出力するサイズを制限する(outSample数)
			fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				PRINT_LOG(param->debug_upconv,"fio_auto(write)");
				//fio_set_memory_limit(&fp_w,50,5);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			fftParam.disable_eq = 1;
			fftParam.lfc = -1;
			fftParam.hfc = hfc;
#ifdef UPCONV_LOG
			fio_set_logfn(&fp_r,"upconv:genOverTone");
			fio_set_log_remain(&fp_r,50);
			fio_set_logfn(&fp_w,"upconv:genOverTone");
			fio_set_log_remain(&fp_w,50);
#endif
			param->enable_sfa = 1;
			genOverTone(hfc,outSample,&fp_r,&fp_w,param,&fftParam);
			param->enable_sfa = 0;
			if (fp_r.error) {
				PRINT_LOG("","genOverTone after fp_r error");
				break;
			}
			PRINT_LOG("","return genOverTone");
			if (param->err) {
				break;
			}
			PRINT_LOG("","no error:genOverTone");
			if (fp_w.error) {
				PRINT_LOG("","genOverTone fp_w.error");
			}
			if (fp_r.error) {
				PRINT_LOG("","genOverTone fp_r.error");
			}
			fio_setmode_r(&fp_w,&fp_r2,tmpFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r2.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}

			fio_get_filesize(&fp_r,&size);
			size = size / sizeof (SSIZE);
			sprintf(sss,"upconv:%d, fp_r1 after GenOverTone end inSample:%lld",param->upconv,size);
			PRINT_LOG("",sss);

			fio_get_filesize(&fp_r2,&size2);
			size2 = size2 / sizeof (SSIZE);
			sprintf(sss,"upconv:%d, fp_r2 after GenOverTone end inSample:%lld",param->upconv,size2);
			PRINT_LOG("",sss);

			if (size != size2) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}

			PRINT_LOG("","fio_open fp_w");
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}

#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			//fio_set_memory_limit(&fp_r2,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				//fio_set_memory_limit(&fp_r2,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,&fp_r2,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif

			// ファイルに出力するサイズを制限する(outSample数)
			sprintf(sss,"after genovertone fp_w limit:%lld",(fio_size)outSample * sizeof (SSIZE));
			PRINT_LOG("",sss);
			fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				PRINT_LOG(param->debug_upconv,"fio_auto(write)");
				//fio_set_memory_limit(&fp_w,50,5);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[MS5]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			SideFreqAdjust(outSample,&fp_r,&fp_r2,&fp_w,param,fftParam.fft_cut_info);
			if (param->err) {
				break;
			}
			fio_close(&fp_r);
			fio_close(&fp_r2);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r2.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			param->p->hfa = sfa_hfa;
			param->p->hfa3_version = sfa_hfa_ver;
			param->p->hfa3_freq_start = sfa_hfa3_freq_start;
			param->p->hfa3_freq_len   = sfa_hfa3_freq_len;
			param->p->hfa3_analyze_step = sfa_hfa3_analyze_step;
			param->p->hfa_level_adjust_width = sfa_hfa_level_adj_width;
			param->p->hfa_level_adjust_nx = sfa_hfa_level_adj_nx;
			param->p->hfa3_sig_level_limit = sfa_hfa3_sig_level_limit;

			fftParam.outSampleR  = sfa_outSampleR;
			param->p->outSampleR = sfa_outSampleR;
		}
		PRINT_LOG("","SFA OK");

		dd = inSample;
		dd /= param->p->inSampleR;
		dd *= param->p->outSampleR;
		outSample = (SSIZE)dd;

		if (param->p->dsd_fmt == -1) {
			if (param->p->oversamp == 1) {
				// 2倍のサンプリングレートで計算し後でダウンサンプルする。
				svOutSampleR = fftParam.outSampleR;
				fftParam.outSampleR *= 2;
				param->p->outSampleR *= 2;
				PRINT_LOG("","oversamp:1, x2");
			} else if (param->p->oversamp == 2) {
				if (fftParam.outSampleR < 384000) {
					// 384000のサンプリングレートで計算し後でダウンサンプルする。
					svOutSampleR = fftParam.outSampleR;
					fftParam.outSampleR = 384000;
					param->p->outSampleR   = 384000;
					PRINT_LOG("","oversamp:2, 384000");
				} else {
					param->p->oversamp = 0;
				}
			} else if (param->p->oversamp == 3) {
				if (fftParam.outSampleR < 1536000) {	// このif文は意味がない
					svOutSampleR = fftParam.outSampleR;
					fftParam.outSampleR = 1536000;
					param->p->outSampleR   = 1536000;
					PRINT_LOG("","oversamp:3, 1536000");
				} else {
					param->p->oversamp = 0;
				}
			} else if (param->p->oversamp == 4) {
				// 192000のサンプリングレートで計算し後でダウンサンプルする。
				if (fftParam.outSampleR < 192000) {
					svOutSampleR = fftParam.outSampleR;
					fftParam.outSampleR = 192000;
					param->p->outSampleR   = 192000;
					PRINT_LOG("","oversamp:4, 192000");
				} else {
					param->p->oversamp = 0;
				}
			} else if (param->p->oversamp == 5) {
				// 768000のサンプリングレートで計算し後でダウンサンプルする。
				if (fftParam.outSampleR < 768000) {
					svOutSampleR = fftParam.outSampleR;
					fftParam.outSampleR = 768000;
					param->p->outSampleR   = 768000;
					PRINT_LOG("","oversamp:5, 768000");
				} else {
					param->p->oversamp = 0;
				}
			} else if (param->p->oversamp == 6) {
				if (fftParam.outSampleR < 3072000) {	// このif文は意味がない
					svOutSampleR = fftParam.outSampleR;
					fftParam.outSampleR = 3072000;
					param->p->outSampleR   = 3072000;
					PRINT_LOG("","oversamp:6, 3072000");
				} else {
					param->p->oversamp = 0;
				}
			}
		}

		dd = inSample;
		dd /= fftParam.inSampleR;
		dd *= fftParam.outSampleR;
		outSample = (SSIZE)dd;

		if (param->p->abe != 0) {
			if (param->p->abe_declip != 0) {
				if (param->print_stdout == 1) {
					strcpy(param->pr_str,"[DeClip]");
					fprintf(stdout,"\n");
					fprintf(stdout,"%s0%%\n",param->pr_str);
					fflush(stdout);
				}
				if (param->p->fio_sec_size > 0) {
					fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
				}
				fio_common.upconv_n    = 1;

				// 出力用にファイルオープン
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				//fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				abe_deClipNoise(inSample,&fp_r,&fp_w,param);
				if (param->err) {
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				fio_seek(&fp_r,0,SEEK_SET);
				if (param->p->abe_declip == 2) {
					PRINT_LOG("","abe_declip = 2");
					abe_deClipNoise(inSample,&fp_r,&fp_w,param);
					param->p->abe_declip = 0;
					if (param->err) {
						break;
					}
					fio_close(&fp_r);
					if (fp_r.error) {
						param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
						break;
					}
					fio_setmode_r(&fp_w,&fp_r,rawFile);
					if (fp_w.error) {
						param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
						break;
					}
					if (fp_r.error) {
						param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
						break;
					}
#if 0
					// readahead
					//fio_set_memory_limit(&fp_r,20,param->p->fio);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
					if (param->p->fio_auto == 1) {
						//fio_set_memory_limit(&fp_r,50,5);
						fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
						fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
					}
#endif
				} else {
					fio_close(&fp_w);
				}
			}
			if (param->p->abe_declick != 0) {
				if (param->print_stdout == 1) {
					strcpy(param->pr_str,"[DeClick]");
					fprintf(stdout,"\n");
					fprintf(stdout,"%s0%%\n",param->pr_str);
					fflush(stdout);
				}
				if (param->p->fio_sec_size > 0) {
					fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
				}
				fio_common.upconv_n    = 1;
				// 出力用にファイルオープン
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				//fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);

				abe_deClickNoise(inSample,&fp_r,&fp_w,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
			}
			if (param->print_stdout == 1) {
				if (param->p->simple_mode == 0) {
					strcpy(param->pr_str,"[ABE]");
					fprintf(stdout,"\n");
					fprintf(stdout,"%s0%%\n",param->pr_str);
					fflush(stdout);
				}
			}
			get_time_string(sss);
			strcat(sss,"ABE Start");
			PRINT_LOG("",sss);
			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;

			// 出力用にファイルオープン
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
#ifdef UPCONV_LOG
			fio_set_logfn(&fp_r,"upconv:ABE");
			fio_set_log_remain(&fp_r,50);
			fio_set_logfn(&fp_w,"upconv:ABE");
			fio_set_log_remain(&fp_w,50);
#endif

			param->p->simple_mode_done_percent_div = 5;
			param->p->simple_mode_done_percent = 20;

			adjBitExtension(inSample,&fp_r,&fp_w,param);
			if (param->err) {
				break;
			}
			fio_close(&fp_r);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			param->p->abe = 2;
			get_time_string(sss);
			strcat(sss,"ABE End");
			PRINT_LOG("",sss);
		}
		fprintf(stdout,"\n");
		fflush(stdout);

		sprintf(sss,"upconv:%d, maxMemorySize(after ABE):%lld",param->upconv,param->maxMemorySize);
		PRINT_LOG("",sss);

		if (chkAbort(0,0)) {
			abort_flag = 1;
			break;
		}

		if (param->print_stdout == 1) {
			if (param->p->simple_mode == 0) {
				strcpy(param->pr_str,"[SRC]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
		}
		if (inSample > outSample) {
			downSampleFlag = TRUE;
		}
		if (param->p->fio_sec_size > 0) {
			fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
		}
		fio_common.upconv_n    = 1;
		//fio_open(&fp_w,outFile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			break;
		}

		fio_get_filesize(&fp_r,&size);
		size = size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d, fp_r after ABE inSample:%lld",param->upconv,size);
		PRINT_LOG("",sss);

		// ファイルに出力するサイズを制限する(outSample数)
		fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
		//fio_set_memory_limit(&fp_w,20,param->p->fio);
		fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
		if (param->p->fio_auto == 1) {
			PRINT_LOG(param->debug_upconv,"fio_auto(write)");
			//fio_set_memory_limit(&fp_w,50,5);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
			fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
		}
#endif
		for (i = 0;i < 384000;i++) {
			fftParam.eq_ref_max[i] = 0;
			fftParam.eq_ref_avg[i] = 0;
			fftParam.eq[i] = 0;
			fftParam.lfa_eq[i] = 0;
			fftParam.declip_eq[i] = 0;
			fftParam.hfa_eq[i] = 0;
			fftParam.pre_post_eq[i] = 0;
			fftParam.analyze_eq_pw[i] = 0;
		}

		fftParam.analyze_mode = 1;
		if (param->p->oversamp >= 3 || param->p->dsd_fmt != -1) {
			fftParam.hi_sample_mode = 1;
		}
		strcpy(fftParam.pr_str,param->pr_str);
		fftParam.print_stdout = param->print_stdout;
		param->p->simple_mode_done_percent = 40;
		param->p->simple_mode_done_percent_div = 5;
		fftParam.simple_mode_done_percent  = param->p->simple_mode_done_percent;
		fftParam.simple_mode_done_percent_div = param->p->simple_mode_done_percent_div;

		fftFilter(inSample,outSample,&fp_r,&fp_w,&fftParam);
		if (fftParam.err) {
			param->err = fftParam.err;param->errLine = __LINE__;
			break;
		}

		if (param->p->eq_analyze > 0 && param->p->index_pre_eq > 0) {
			int i,ii,count;
			double ana_avg;
			for (i = 0;i < 384000;i++) {
				if (fftParam.analyze_eq_pw_count > 0) {
					param->analyze_eq_pw[i] = fftParam.analyze_eq_pw[i] / fftParam.analyze_eq_pw_count;
				} else {
					param->analyze_eq_pw[i] = 0;
				}
			}

			for (i = 1;i < 384000;i+=300) {
				for (ii = i,count = 0,ana_avg = 0;count < 300;ii++,count++) {
					ana_avg += param->analyze_eq_pw[ii];
				}
				ana_avg /= count;
				for (ii = i,count = 0,ana_avg = 0;count < 300;ii++,count++) {
					param->analyze_eq_pw[i] = ana_avg;
				}
			}
		}

		if (fftParam.analyze_mode == 1) {
			int i,j,n;
			double avg1,avg2,p1_avg,p2_avg,p1_step,p1_per,p2_per;
			int p1_index,p2_index,p1_e_index,p2_e_index;

			validIndex = param->p->inSampleR / 2;
			if (validIndex > 384000) validIndex = 384000;

			if (fftParam.eq_ref_count > 0) {
				for (i = 1;i < 384000;i++) {
					fftParam.eq_ref_avg[i] /= fftParam.eq_ref_count;
				}
			}
			for (i = 0;i < validIndex - 10;i++) {
				avg1 = 0;
				avg2 = 0;
				for (j = 0;j < 10;j++) {
					avg1 += fftParam.eq_ref_max[i + j];
					avg2 += fftParam.eq_ref_avg[i + j];
				}
				avg1 /= 10;
				avg2 /= 10;
				for (j = 0;j < 10;j++) {
					fftParam.eq_ref_max[i + j] = avg1;
					fftParam.eq_ref_avg[i + j] = avg2;
				}
			}

			if (fftParam.cut_high_dither == 1) {
				p1_index   = 5000;
				p1_e_index = 6000;
				p2_index   = 10000;
				p2_e_index = 11000;
				p1_avg = 0;
				p2_avg = 0;
				for (i = p1_index,n = 0;i < p1_e_index;i++,n++) {
					p1_avg += fftParam.eq_ref_avg[i];
				}
				if (n > 0) {
					p1_avg /= n;
				}
				for (i = p2_index,n = 0;i < p2_e_index;i++,n++) {
					p2_avg += fftParam.eq_ref_avg[i];
				}
				if (n > 0) {
					p2_avg /= n;
				}

				// 4kHz ～ 5kHz の間のpowerと8kHz～9kHzのpowerを比べ、高音が大きい場合は調整用のデータを生成する。
				p1_per = p2_avg / p1_avg;
				if (p1_per > 0.53) {
					p1_index   = 6000;
					p2_index   = 10000;
					p1_per = ((double)0.47 / p1_per);
					p1_step = ((double)1.0 - p1_per) / (p2_index - p1_index);
					for (i = 1;i < validIndex;i++) {
						if (i > p2_index) {
							fftParam.eq[i] = fftParam.eq[i - 1];
						} else if (i > p1_index) {
							fftParam.eq[i] = fftParam.eq[i - 1] - p1_step;
						} else {
							fftParam.eq[i] = 1;
						}
					}
				} else {
					fftParam.cut_high_dither = 0;
				}
			}
		}

		// 音量調査
		fio_flush(&fp_w);
		//merageTempFile(' ',1,&fp_w,NULL,NULL,outSample,param);
		//if (param->err) {
		//	break;
		//}

		fio_close(&fp_w);

		// hfc auto 用パラメーター設定(hfc)
		fprintf(stdout,"\n");
		fflush(stdout);
		if (param->p->hfc_auto == 1) {
			anaHFC_AutoParam(&fftParam);
			param->p->hfc = fftParam.hfc;
			if (param->p->hfc + param->p->hfc_auto_adjust >= 10000) {
				param->p->hfc = param->p->hfc + param->p->hfc_auto_adjust;
				fftParam.hfc = param->p->hfc;
			} else {
				param->p->hfc = 10000;
				fftParam.hfc = param->p->hfc;
			}
		}
		if (param->p->hfa_level == 6) {
			if (param->p->hfc >= 10000 && param->p->hfc < 11000) {
				for (i = 0;i < 384000;i++) {
					param->hfa_eq[i] = param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i];
				}
			} else if (param->p->hfc >= 11000 && param->p->hfc < 12000) {
				for (i = 0;i < 384000;i++) {
					param->hfa_eq[i] = param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i];
				}
			} else if (param->p->hfc >= 12000 && param->p->hfc < 13000) {
				for (i = 0;i < 384000;i++) {
					param->hfa_eq[i] = param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i];
				}
			} else if (param->p->hfc >= 13000 && param->p->hfc < 14000) {
				for (i = 0;i < 384000;i++) {
					param->hfa_eq[i] = param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i] * param->hfa_eq[i];
				}
			} else if (param->p->hfc >= 14000 && param->p->hfc < 15000) {
				for (i = 0;i < 384000;i++) {
					param->hfa_eq[i] = param->hfa_eq[i];
				}
			} else if (param->p->hfc >= 15000 && param->p->hfc < 16000) {
				for (i = 0;i < 384000;i++) {
					param->hfa_eq[i] = 1;
				}
			} else if (param->p->hfc >= 16000 && param->p->hfc < 17000) {
				for (i = 0;i < 384000;i++) {
					param->hfa_eq[i] = 1;
				}
			}
		}

		// lfa 用パラメーター作成(lfa_eq)
		if (param->p->low_adjust != 0) {
			anaLFA_Param(&fftParam);
		}
		// hfc 用パラメーターの調整
		if (param->p->hfa3_analysis_limit_adjust != 0 && param->p->hfc != -1) {
			param->p->hfa_analysis_limit = param->p->hfc + param->p->hfa3_analysis_limit_adjust;
			if (param->p->hfa_analysis_limit < 11000) param->p->hfa_analysis_limit = 11000;
			if (param->p->hfa_analysis_limit > (param->p->inSampleR / 2)) param->p->hfa_analysis_limit = (param->p->inSampleR / 2);
			if (param->p->hfa_analysis_limit > (svOutSampleR / 2)) param->p->hfa_analysis_limit = (svOutSampleR / 2);
		}
		if (param->p->fio_sec_size > 0) {
			fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
		}
		fio_common.upconv_n    = 1;
		//fio_open(&fp_w,outFile,FIO_MODE_W);
		fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			break;
		}

		// ファイルに出力するサイズを制限する(outSample数)
		fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
		//fio_set_memory_limit(&fp_w,20,param->p->fio);
		fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
		if (param->p->fio_auto == 1) {
			PRINT_LOG(param->debug_upconv,"fio_auto(write)");
			//fio_set_memory_limit(&fp_w,50,5);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
			fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
		}
#endif
		// fft
		fprintf(stdout,"\n");
		fflush(stdout);

		if (param->print_stdout == 1) {
			if (param->p->simple_mode == 0) {
				strcpy(param->pr_str,"[SRC]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
		}
		fftParam.analyze_mode = 0;
		fftParam.lfa_flag = param->p->low_adjust;
		fftParam.lpf_flag = param->p->lpf;
		fftParam.declip_eq = param->declip_eq;
		fftParam.hfa_eq   = param->hfa_eq;
		fftParam.pre_eq_flag = param->p->enable_pre_eq;
		fftParam.declip_flag = 0;

		param->p->simple_mode_done_percent = 60;
		param->p->simple_mode_done_percent_div = 5;
		fftParam.simple_mode_done_percent  = param->p->simple_mode_done_percent;
		fftParam.simple_mode_done_percent_div = param->p->simple_mode_done_percent_div;

		if (param->p->abe_declip == 2) {
			fftParam.declip_flag = 1;
		}
		if (param->p->enable_pre_eq == 1) {
			char ssss[300];
			PRINT_LOG("","Copy PreEQ");
			for (i = 0;i < 384000;i++) {
				fftParam.pre_post_eq[i] = param->pre_eq[i];
//				sprintf(ssss,"pre_eq[%d]:%lf\n",i,param->pre_eq[i]);
//				PRINT_LOG("",ssss);
			}
		}
		fftParam.print_stdout = param->print_stdout;
		strcpy(fftParam.pr_str,param->pr_str);
#ifdef UPCONV_LOG
			fio_set_logfn(&fp_r,"upconv:fftFilter");
			fio_set_log_remain(&fp_r,50);
			fio_set_logfn(&fp_w,"upconv:fftFilter");
			fio_set_log_remain(&fp_w,50);
#endif
		// ファイルサイズ取得
		fio_get_filesize(&fp_r,&file_size);
		sprintf(sss,"upconv:%d,filesize:%lld",param->upconv,file_size);
		PRINT_LOG("",sss);

		fftFilter(inSample,outSample,&fp_r,&fp_w,&fftParam);
		if (fftParam.err) {
			param->err = fftParam.err;param->errLine = __LINE__;
			break;
		}
		fio_close(&fp_r);
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			break;
		}

		// 生成したデータを読み込み用に設定する。
		fio_setmode_r(&fp_w,&fp_r,rawFile);
		if (fp_w.error) {
			param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			break;
		}
		if (fp_r.error) {
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			break;
		}
#if 0
		// readahead
		if (param->p->fio_auto == 1) {
			//fio_set_memory_limit(&fp_r,50,5);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
			fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
		}
#endif
		param->p->cut_high_dither = 0;
		param->p->low_adjust = 0;
		param->p->lpf = 0;
		param->ana = 0;
		param->eq_flag = 0;
		param->p->abe_declip = 0;
		param->p->enable_pre_eq = 0;
		
		fftParam.lfa_flag = 0;
		fftParam.lpf_flag = 0;
		fftParam.cut_high_dither = 0;
		fftParam.declip_flag = 0;
		fftParam.pre_eq_flag = 0;

//		if (param->eq_pw != NULL) {
//			free(param->eq_pw);
//		}

		// スピーカー用周波数解析
//		if (param->sp_ana == 1) {
//			spAnalyze(outSample,&fp_r,param);
//		}

		fio_get_filesize(&fp_r,&size);
		size = size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d, fp_r after SRC inSample:%lld",param->upconv,size);
		PRINT_LOG("",sss);

		if (chkAbort(0,0)) {
			abort_flag = 1;
			break;
		}

		//
		// ノイズカット
		fprintf(stdout,"\n");
		fflush(stdout);
		if (param->p->nr != -1) {
			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[NR]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			declip_flag = 1;

			if (param->p->hfc != -1) {
				hfc = param->p->hfc;
			} else {
				if (downSampleFlag == FALSE) {
					hfc = param->p->inSampleR / 2;
				} else {
					hfc = param->p->outSampleR / 2;
				}
			}
			if (downSampleFlag == FALSE) {
				if (hfc > param->p->inSampleR / 2) {
					hfc = param->p->inSampleR / 2;
				}
			} else {
				if (hfc > param->p->outSampleR / 2) {
					hfc = param->p->outSampleR / 2;
				}
			}
			if (param->p->nr < hfc) {
				if (param->p->fio_sec_size > 0) {
					fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
				}
				fio_common.upconv_n    = 1;
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
#ifdef UPCONV_LOG
				fio_set_logfn(&fp_r,"upconv:noiseCut");
				fio_set_log_remain(&fp_r,50);
				fio_set_logfn(&fp_w,"upconv:noiseCut");
				fio_set_log_remain(&fp_w,50);
#endif

				noiseCut(param->p->nr,outSample,&fp_r,&fp_w,param);
				if (param->err) {
					break;
				}
				fio_setmode_r(&fp_w,&fp_r2,tmpFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				//fio_set_memory_limit(&fp_r2,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					//fio_set_memory_limit(&fp_r2,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,&fp_r2,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				//bpFilter(-1,hfc,outSample,&fp_r2,&fp_w,param);
				fftParam.disable_eq = 1;
				fftParam.lfc = -1;
				fftParam.hfc = hfc;
				fftParam.inSampleR = fftParam.outSampleR;
				fftParam.print_stdout = param->print_stdout;
				strcpy(fftParam.pr_str,param->pr_str);
				fftFilter(outSample,outSample,&fp_r2,&fp_w,&fftParam);
				if (fftParam.err) {
					param->err = fftParam.err;param->errLine = __LINE__;
					break;
				}
				fio_close(&fp_r2);
				fio_setmode_r(&fp_w,&fp_r2,tmpFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				//fio_set_memory_limit(&fp_r2,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					//fio_set_memory_limit(&fp_r2,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,&fp_r2,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

#ifdef UPCONV_LOG
				fio_set_logfn(&fp_r,"upconv:mergeTempFile");
				fio_set_log_remain(&fp_r,50);
				fio_set_logfn(&fp_w,"upconv:mergeTempFile");
				fio_set_log_remain(&fp_w,50);
#endif
				merageTempFile('-',0,&fp_r,&fp_r2,&fp_w,outSample,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				fio_close(&fp_r2);
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
			}
		}

		fio_get_filesize(&fp_r,&size);
		size = size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d, fp_r after NR inSample:%lld",param->upconv,size);
		PRINT_LOG("",sss);

		sprintf(sss,"upconv:%d, maxMemorySize(after NR):%lld",param->upconv,param->maxMemorySize);
		PRINT_LOG("",sss);

		//
		// 高域補間処理
		fprintf(stdout,"\n");
		fflush(stdout);

		if (chkAbort(0,0)) {
			abort_flag = 1;
			break;
		}

		if (param->p->hfa != 0) {
			fflush(stdout);

			if (downSampleFlag == FALSE) {
				hfc = param->p->inSampleR / 2;
				if (param->p->hfc != -1 && hfc > param->p->hfc) {
					hfc = param->p->hfc;
				}
			} else {
				hfc = param->p->outSampleR / 2;
				if (param->p->hfc != -1 && hfc > param->p->hfc) {
					hfc = param->p->hfc;
				}
			}

			if (param->p->hfa != 1) {
				declip_flag = 1;

				if (param->p->fio_sec_size > 0) {
					fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
				}
				fio_common.upconv_n    = 1;
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				if (param->print_stdout == 1) {
					sprintf(param->pr_str,"[HFA%d]",param->p->hfa);
					fprintf(stdout,"\n");
					fprintf(stdout,"%s0%%\n",param->pr_str);
					fflush(stdout);
				}
				fftParam.disable_eq = 1;
				fftParam.lfc = -1;
				fftParam.hfc = hfc;
				fftParam.inSampleR = fftParam.outSampleR;
#ifdef UPCONV_LOG
				fio_set_logfn(&fp_r,"upconv:genOverTone");
				fio_set_log_remain(&fp_r,50);
				fio_set_logfn(&fp_w,"upconv:genOverTone");
				fio_set_log_remain(&fp_w,50);
#endif
				genOverTone(hfc,outSample,&fp_r,&fp_w,param,&fftParam);
				PRINT_LOG("","return genOverTone");
				if (param->err) {
					break;
				}
				PRINT_LOG("","no error:genOverTone");
				if (fp_w.error) {
					PRINT_LOG("","genOverTone fp_w.error");
				}
				if (fp_r.error) {
					PRINT_LOG("","genOverTone fp_r.error");
				}
				PRINT_LOG("","fio_set_mode_r");
				PRINT_LOG("",tmpFile);
				fio_setmode_r(&fp_w,&fp_r2,tmpFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				fio_get_filesize(&fp_r2,&size);
				size = size / sizeof (SSIZE);
				sprintf(sss,"upconv:%d, fp_r2 after GenOverTone end inSample:%lld",param->upconv,size);
				PRINT_LOG("",sss);

				PRINT_LOG("","fio_open fp_w");
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				//fio_set_memory_limit(&fp_r2,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					//fio_set_memory_limit(&fp_r2,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,&fp_r2,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

				PRINT_LOG("","fftFilter");
				fftParam.disable_eq = 1;
				fftParam.lfc = hfc;
				fftParam.hfc = -1;
				fftParam.inSampleR = fftParam.outSampleR;
				if (param->p->hfa_level > 0) {
					fftParam.hfa_flag = 1;
				}
				fftParam.print_stdout = param->print_stdout;
				strcpy(fftParam.pr_str,param->pr_str);
				fftFilter(outSample,outSample,&fp_r2,&fp_w,&fftParam);
				if (fftParam.err) {
					sprintf(sss,"upconv:%d,err:%d,fftFilter:MemSize:%lld",param->upconv,fftParam.err,fftParam.maxMemorySize);
					PRINT_LOG("",sss);
					param->err = fftParam.err;param->errLine = __LINE__;
					break;
				}
				fftParam.hfa_flag = 0;
				fio_close(&fp_r2);
				fio_setmode_r(&fp_w,&fp_r2,tmpFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				//fio_set_memory_limit(&fp_r2,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					//fio_set_memory_limit(&fp_r2,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,&fp_r2,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

				PRINT_LOG("","merageTempFile(+)");

				merageTempFile('+',0,&fp_r,&fp_r2,&fp_w,outSample,param);
				if (param->err) {
					PRINT_LOG("","error");
					break;
				}
				fio_close(&fp_r);
				fio_close(&fp_r2);
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
			}
			PRINT_LOG("","before HFA1");
			if (param->p->hfa == 1 || param->p->hfaNB > 0) {
				if (param->print_stdout == 1) {
					strcpy(param->pr_str,"[HFA1]");
					fprintf(stdout,"\n");
					fprintf(stdout,"%s0%%\n",param->pr_str);
					fflush(stdout);
				}
				if (param->p->fio_sec_size > 0) {
					fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
				}
				fio_common.upconv_n    = 1;
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

#ifdef UPCONV_LOG
				fio_set_logfn(&fp_r,"upconv:genNoise");
				fio_set_log_remain(&fp_r,50);
				fio_set_logfn(&fp_w,"upconv:genNoise");
				fio_set_log_remain(&fp_w,50);
#endif
				genNoise(hfc,outSample,&fp_r,&fp_w,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
			}
		}

		fio_get_filesize(&fp_r,&size);
		size = size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d, fp_r after HFA inSample:%lld",param->upconv,size);
		PRINT_LOG("",sss);
		sprintf(sss,"upconv:%d, maxMemorySize(after NR):%lld",param->upconv,param->maxMemorySize);
		PRINT_LOG("",sss);

		//
		// ノイズカット
		fprintf(stdout,"\n");
		fflush(stdout);

		if (chkAbort(0,0)) {
			abort_flag = 1;
			break;
		}

		if (param->p->post_nr != -1) {
			if (downSampleFlag == FALSE) {
				hfc = param->p->inSampleR / 2;
				if (param->p->hfc != -1 && hfc > param->p->hfc) {
					hfc = param->p->hfc;
				}
			} else {
				hfc = param->p->outSampleR / 2;
				if (param->p->hfc != -1 && hfc > param->p->hfc) {
					hfc = param->p->hfc;
				}
			}
			if (hfc > 24000) {
				hfc = 24000;
			}

			param->p->hfc = -1;
			param->p->nr = hfc;
			if (param->p->nr <= hfc) {
				if (param->print_stdout == 1) {
					strcpy(param->pr_str,"[Post NR]");
					fprintf(stdout,"\n");
					fprintf(stdout,"%s0%%\n",param->pr_str);
					fflush(stdout);
				}
				if (param->p->fio_sec_size > 0) {
					fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
				}
				fio_common.upconv_n    = 1;
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				noiseCut(param->p->nr,outSample,&fp_r,&fp_w,param);
				if (param->err) {
					break;
				}
				fio_setmode_r(&fp_w,&fp_r2,tmpFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				//fio_set_memory_limit(&fp_r2,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					//fio_set_memory_limit(&fp_r2,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,&fp_r2,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				//bpFilter(-1,hfc,outSample,&fp_r2,&fp_w,param);
				if (downSampleFlag == FALSE) {
					hfc = param->p->inSampleR / 2;
				} else {
					hfc = param->p->outSampleR / 2;
				}
				fftParam.disable_eq = 1;
				fftParam.lfc = -1;
				fftParam.hfc = hfc;
				fftParam.inSampleR = fftParam.outSampleR;
				fftParam.print_stdout = param->print_stdout;
				strcpy(fftParam.pr_str,param->pr_str);
				fftFilter(outSample,outSample,&fp_r2,&fp_w,&fftParam);
				if (param->err) {
					break;
				}
				fio_close(&fp_r2);
				fio_setmode_r(&fp_w,&fp_r2,tmpFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				//fio_set_memory_limit(&fp_r2,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					//fio_set_memory_limit(&fp_r2,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_memory_limit(&fp_r2,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,&fp_r2,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif

				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				merageTempFile('-',0,&fp_r,&fp_r2,&fp_w,outSample,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				fio_close(&fp_r2);
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r2.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
			}
		}

		fio_get_filesize(&fp_r,&size);
		size = size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d, fp_r after post NR inSample:%lld",param->upconv,size);
		PRINT_LOG("",sss);
		sprintf(sss,"upconv:%d, maxMemorySize(after post NR):%lld",param->upconv,param->maxMemorySize);
		PRINT_LOG("",sss);

		if (chkAbort(0,0)) {
			abort_flag = 1;
			break;
		}

		if (param->p->post_abe == 1) {
			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[Post ABE]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			// ファイルに出力するサイズを制限する(outSample数)
			fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				PRINT_LOG(param->debug_upconv,"fio_auto(write)");
				//fio_set_memory_limit(&fp_w,50,5);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			svInSampleR = param->p->inSampleR;
			svIw		= param->p->iw;
			param->p->inSampleR = param->p->outSampleR;
			param->p->iw = param->p->w;
			if (param->p->abe_version < 4) {
				param->p->cutLowData = 0;
				param->p->abeFnLevel = 0;
				param->p->smLowData  = 0;
				param->p->abe_ad_filter = 0;
				param->p->abe_declip = 0;
				param->p->abeNX = 0;
			} else {
				param->p->abe_2val_smooth = 0;
				param->p->abe_3val_smooth = 0;
				param->p->abe_5val_smooth = 0;
				param->p->abe_9val_smooth = 0;
				param->p->abe_17val_smooth = 0;
				param->p->abe_31val_smooth = 0;
				param->p->abe_47val_smooth = 0;
				param->p->abe_rep_smooth   = 1;
				param->p->cutLowData = 0;
				param->p->abeFnLevel = 5;
				param->p->smLowData  = 0;
				param->p->abe_ad_filter = 0;
				param->p->abe_declip = 0;
				param->p->abeNX = 0;
				
				param->p->abe_10x_wlt_level = 0;
				param->p->abe_10x_cut_level = 0;

			}
			if (param->p->abe_declick != 0) {
				param->p->abe_declick = 2;
				abe_deClickNoise(outSample,&fp_r,&fp_w,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
			}
#ifdef UPCONV_LOG
			fio_set_logfn(&fp_r,"upconv:PostABE");
			fio_set_log_remain(&fp_r,50);
			fio_set_logfn(&fp_w,"upconv:PostABE");
			fio_set_log_remain(&fp_w,50);
#endif
			adjBitExtension(outSample,&fp_r,&fp_w,param);
			if (param->err) {
				break;
			}
			param->p->inSampleR = svInSampleR;
			param->p->iw = svIw;

			fio_close(&fp_r);
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
		}

		fio_get_filesize(&fp_r,&size);
		size = size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d, fp_r after post ABE inSample:%lld",param->upconv,size);
		PRINT_LOG("",sss);
		sprintf(sss,"upconv:%d, maxMemorySize(after post ABE):%lld",param->upconv,param->maxMemorySize);
		PRINT_LOG("",sss);

		if (chkAbort(0,0)) {
			abort_flag = 1;
			break;
		}

		// オーバーサンプリング用のダウンサンプル処理
		if (param->p->dsd_fmt == -1 && param->p->oversamp != 0) {
			char sssss[128];
			SSIZE wkSample;
			memset(param->normInfo,0,sizeof (NORM_INFO));
			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);

			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[SRC(DS)]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			fftParam.hfc = -1;
			fftParam.lfc = -1;
			fftParam.hi_sample_mode = 1;
			fftParam.inSampleR  = param->p->outSampleR;
			param->p->inSampleR    = param->p->outSampleR;
			dd = outSample;
			dd /= param->p->outSampleR;
			fftParam.outSampleR = svOutSampleR;
			param->p->outSampleR   = svOutSampleR;
			dd *= param->p->outSampleR;
			wkSample = (SSIZE)dd;
			// ファイルに出力するサイズを制限する(outSample数)
			fio_set_maxsize(&fp_w,(fio_size)wkSample * sizeof (SSIZE));

			fftParam.disable_eq = 1;
			//fftParam.lvadj_flag = 1;
			strcpy(fftParam.pr_str,param->pr_str);
			fftParam.print_stdout = param->print_stdout;
			fftFilter(outSample,wkSample,&fp_r,&fp_w,&fftParam);
			if (param->err) {
				break;
			}
			fio_close(&fp_r);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			outSample = wkSample;
		} else if (param->p->dsd_fmt != -1) {
			SSIZE wkSample;
			memset(param->normInfo,0,sizeof (NORM_INFO));
			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			//fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);

			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[SRC(DS)]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			param->p->inSampleR    = param->p->outSampleR;
			fftParam.inSampleR  = param->p->outSampleR;
			if (param->p->dsd_fmt == 64) {
				fftParam.hfc = 34100;
				param->p->outSampleR   = 2822400;
				fftParam.outSampleR = 2822400;
			} else if (param->p->dsd_fmt == 128) {
				fftParam.hfc = 44100;
				param->p->outSampleR   = 2822400 * 2;
				fftParam.outSampleR = 2822400 * 2;
			} else if (param->p->dsd_fmt == 256) {
				fftParam.hfc = 66100;
				param->p->outSampleR   = 2822400 * 4;
				fftParam.outSampleR = 2822400 * 4;
			}
			fftParam.lfc = 20;
			fftParam.hi_sample_mode = 1;
			fftParam.dsd_fmt = param->p->dsd_fmt;
			fftParam.analyze_mode = 0;
			dd = outSample;
			dd *= param->p->outSampleR;
			dd /= param->p->inSampleR;
			wkSample = (SSIZE)dd;
			// ファイルに出力するサイズを制限する(outSample数)
			fio_set_maxsize(&fp_w,(fio_size)wkSample * sizeof (SSIZE));

			fftParam.disable_eq = 1;
			strcpy(fftParam.pr_str,param->pr_str);
			fftParam.print_stdout = param->print_stdout;
#ifdef UPCONV_LOG
			fio_set_logfn(&fp_r,"upconv:DownSampling");
			fio_set_log_remain(&fp_r,50);
			fio_set_logfn(&fp_w,"upconv:DownSampling");
			fio_set_log_remain(&fp_w,50);
#endif
			fftFilter(outSample,wkSample,&fp_r,&fp_w,&fftParam);
			if (param->err) {
				break;
			}
			fio_close(&fp_r);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			outSample = wkSample;
		}

		fio_get_filesize(&fp_r,&size);
		size = size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d, fp_r after SRC(DS) inSample:%lld",param->upconv,size);
		PRINT_LOG("",sss);

		if (chkAbort(0,0)) {
			abort_flag = 1;
			break;
		}

		// Post EQ
		fprintf(stdout,"\n");
		fflush(stdout);
		if (param->p->enable_post_eq) {
			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[Post EQ]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				PRINT_LOG(param->debug_upconv,"fio_auto(write)");
				//fio_set_memory_limit(&fp_w,50,5);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			fftParam.hfc = -1;
			fftParam.lfc = -1;
			fftParam.hi_sample_mode = 0;
			fftParam.inSampleR  = param->p->outSampleR;
			fftParam.post_eq_flag = 1;
			fftParam.pre_post_eq = param->post_eq;
			strcpy(fftParam.pr_str,param->pr_str);
			fftParam.print_stdout = param->print_stdout;
			fftFilter(outSample,outSample,&fp_r,&fp_w,&fftParam);
			if (param->err) {
				break;
			}
			fio_close(&fp_r);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
		}

		fio_get_filesize(&fp_r,&file_size);
		size = file_size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d,file_size:%lld,inSample:%lld",param->upconv,file_size,size);
		PRINT_LOG("",sss);

		// Post EQ
		fprintf(stdout,"\n");
		fflush(stdout);
		if (param->p->enable_post_eq2) {
			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[Post EQ]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				PRINT_LOG(param->debug_upconv,"fio_auto(write)");
				//fio_set_memory_limit(&fp_w,50,5);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			fftParam.hfc = -1;
			fftParam.lfc = -1;
			fftParam.hi_sample_mode = 0;
			fftParam.inSampleR  = param->p->outSampleR;
			fftParam.post_eq_flag = 1;
			fftParam.pre_post_eq = param->post_eq2;
			strcpy(fftParam.pr_str,param->pr_str);
			fftParam.print_stdout = param->print_stdout;
			fftFilter(outSample,outSample,&fp_r,&fp_w,&fftParam);
			if (param->err) {
				break;
			}
			fio_close(&fp_r);
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
		}

		fio_get_filesize(&fp_r,&file_size);
		size = file_size / sizeof (SSIZE);
		sprintf(sss,"upconv:%d,file_size:%lld,inSample:%lld",param->upconv,file_size,size);
		PRINT_LOG("",sss);

		if (declip_flag) {
			param->p->post_abe = 2;
		}

		if (param->p->post_abe == 2) {
			if (param->print_stdout == 1) {
				strcpy(param->pr_str,"[Post ABE]");
				fprintf(stdout,"\n");
				fprintf(stdout,"%s0%%\n",param->pr_str);
				fflush(stdout);
			}
			if (param->p->fio_sec_size > 0) {
				fio_common.sector_size = param->p->outSampleR * sizeof (SSIZE);
			}
			fio_common.upconv_n    = 1;
			//fio_open(&fp_w,outFile,FIO_MODE_W);
			fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			// ファイルに出力するサイズを制限する(outSample数)
			fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
			//fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				PRINT_LOG(param->debug_upconv,"fio_auto(write)");
				//fio_set_memory_limit(&fp_w,50,5);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
			svInSampleR = param->p->inSampleR;
			svIw		= param->p->iw;
			param->p->inSampleR = param->p->outSampleR;
			param->p->iw = param->p->w;
			if (param->p->abe_version < 4) {
				param->p->cutLowData = 0;
				param->p->abeFnLevel = 0;
				param->p->smLowData  = 0;
				param->p->abe_ad_filter = 0;
				param->p->abe_declip = 0;
				param->p->abeNX = 0;
			} else {
				param->p->abe_2val_smooth = 0;
				param->p->abe_3val_smooth = 0;
				param->p->abe_5val_smooth = 0;
				param->p->abe_9val_smooth = 0;
				param->p->abe_17val_smooth = 0;
				param->p->abe_31val_smooth = 0;
				param->p->abe_47val_smooth = 0;
				param->p->abe_rep_smooth   = 1;
				param->p->cutLowData = 0;
				param->p->abeFnLevel = 5;
				param->p->smLowData  = 0;
				param->p->abe_ad_filter = 0;
				param->p->abe_declip = 0;
				param->p->abeNX = 0;
				
				param->p->abe_10x_wlt_level = 0;
				param->p->abe_10x_cut_level = 0;

			}
			if (param->p->abe_declick != 0) {
				param->p->abe_declick = 2;
				abe_deClickNoise(outSample,&fp_r,&fp_w,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				//fio_open(&fp_w,outFile,FIO_MODE_W);
				fio_open_with_set_param(&fp_w,outFile,FIO_MODE_W,&fio_common);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}

#if 0
				// readahead
				//fio_set_memory_limit(&fp_r,20,param->p->fio);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					//fio_set_memory_limit(&fp_r,50,5);
					fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
				// ファイルに出力するサイズを制限する(outSample数)
				fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
#if 0
				//fio_set_memory_limit(&fp_w,20,param->p->fio);
				fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
				if (param->p->fio_auto == 1) {
					PRINT_LOG(param->debug_upconv,"fio_auto(write)");
					//fio_set_memory_limit(&fp_w,50,5);
					fio_set_memory_limit(&fp_w,param->p->outSampleR * sizeof (SSIZE),5);
					fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
#endif
			}
#ifdef UPCONV_LOG
			fio_set_logfn(&fp_r,"upconv:PostABE");
			fio_set_log_remain(&fp_r,50);
			fio_set_logfn(&fp_w,"upconv:PostABE");
			fio_set_log_remain(&fp_w,50);
#endif
			adjBitExtension(outSample,&fp_r,&fp_w,param);
			if (param->err) {
				break;
			}
			param->p->inSampleR = svInSampleR;
			param->p->iw = svIw;

			fio_close(&fp_r);
			fio_setmode_r(&fp_w,&fp_r,rawFile);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			if (fp_r.error) {
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
#if 0
			// readahead
			//fio_set_memory_limit(&fp_r,20,param->p->fio);
			fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),param->p->fio);
			if (param->p->fio_auto == 1) {
				//fio_set_memory_limit(&fp_r,50,5);
				fio_set_memory_limit(&fp_r,param->p->outSampleR * sizeof (SSIZE),5);
				fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
#endif
		}

		// クリックノイズ除去(ひげのようなもの) & 音量調査
		param->normInfo->min = 0;
		param->normInfo->max = 0;
		param->normInfo->avg = 0;
		merageTempFile(' ',1,&fp_r,NULL,NULL,outSample,param);
		fio_get_filesize(&fp_r,&size);
		if (param->err) {
			break;
		}
		if (1) {
			char sss[100];
			sprintf(sss,"upconv:%d,Fin outSample:%lld",param->upconv,outSample);
			PRINT_LOG("",sss);
			sprintf(sss,"Fin get filesize:%lld",size);
			PRINT_LOG("",sss);
		}
#if 0
		param->vla_nx = 1;
		if (param->upconv == 1 || param->upconv == 2) {
			char pparam[2048];
			double perR1,perR2,per,nx,u_nx,clip_rate;
			SSIZE avgR1,avgR2,avg;
			SSIZE maxR1,maxR2,min,max;
			FILE *fp;

			clip_rate = 0;

			param->diff_max[0] = 0;
			param->diff_max[1] = 0;
			param->diff_max[2] = 0;
			param->diff_max[3] = 0;
			param->diff_max[4] = 0;
			param->diff_max[5] = 0;
			param->diff_max[6] = 0;
			param->diff_max[7] = 0;
			param->diff_max[8] = 0;
			param->diff_max[9] = 0;
			param->step_max[0] = 0;
			param->step_max[1] = 0;
			param->step_max[2] = 0;
			param->step_max[3] = 0;
			param->step_max[4] = 0;
			param->step_max[5] = 0;
			param->step_max[6] = 0;
			param->step_max[7] = 0;
			param->step_max[8] = 0;
			param->step_max[9] = 0;

			fio_open(&fp_w,outFile,FIO_MODE_W);
			if (fp_w.error) {
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
			fio_set_memory_limit(&fp_w,20,param->p->fio);
			fio_set_maxsize(&fp_w,(fio_size)outSample * sizeof (SSIZE));
			if (param->p->fio_auto == 1) {
				PRINT_LOG(param->debug_upconv,"fio_auto(write)");
				fio_set_memory_limit(&fp_w,50,5);
				fio_set_writecache(&fp_w,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
			}
			
			// 音量の調整
			max = param->normInfo->max;
			if (max < 0) {
				max *= -1;
			}
			min = param->normInfo->min;
			if (min < 0) {
				min *= -1;
			}
			if (max < min) max = min;
			avg = param->normInfo->avg << 40;

			_splitpath(param->argv2,drive,dir,fname,ext);
			_makepath(tmpFile,drive,dir,fname,"param");
			// ファイルオープン
			fp = fopen(tmpFile,"r");
			if (fp == NULL) {
				fclose(fp);
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			if (fgets(pparam,_MAX_PATH,fp) == NULL) {
				fclose(fp);
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			perR1 = perR2 = 0;
			avgR1 = avgR2 = 0;
			if (fscanf(fp,"r1=%lf,%llx\n",&perR1,&avgR1) != 2) {
				fclose(fp);
				param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
				break;
			}
			if (param->upconv == 2) {
				if (fscanf(fp,"r2=%lf,%llx\n",&perR2,&avgR2) != 2) {
					fclose(fp);
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
			}
			fclose(fp);

			if (param->upconv == 1) {
				// Left
				nx = 0;
				if (avg > 10000000000) {
					nx = (double)avgR1 / avg;
					if (nx > 0) {
						max = (SSIZE)((double)max * nx);
					}
				} else {
					max = (SSIZE)((double)max * nx);
				}
				u_nx = nx;
			}
			if (param->upconv == 2) {
				// Right
				nx = 0;
				if (avg > 10000000000) {
					nx = (double)avgR2 / avg;
					if (nx > 0) {
						max = (SSIZE)((double)max * nx);
					}
				} else {
					max = (SSIZE)((double)max * nx);
				}
				u_nx = nx;
			}
			if (param->vla_auto == 1) {
				adjClickNoise(outSample,&fp_r,&fp_w,u_nx,param);
				if (param->err) {
					break;
				}
				fio_close(&fp_r);
				fio_setmode_r(&fp_w,&fp_r,rawFile);
				if (fp_w.error) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
				if (fp_r.error) {
					param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
					break;
				}
				// readahead
				if (param->p->fio_auto == 1) {
					fio_set_memory_limit(&fp_r,50,5);
					fio_set_readahead(&fp_r,NULL,NULL,NULL,NULL,NULL,param->p->fio_max / 50);
				}
			} else {
				fio_close(&fp_w);
			}
			if (param->vla_auto == 1 && max > (SSIZE)(0x007FFFFFFFFFFFFF)) {
				for (nx = (double)1;nx >= (double)0.5;nx -= (double)0.05) {
					adjVLA(outSample,&fp_r,u_nx,nx,&clip_rate,param);
					sprintf(pparam,"VLA(%lf) clip_rate:%lf",nx,clip_rate);
					if ((int)clip_rate == 0) break;
				}
				param->vla_nx = nx;
			}
		}
#endif
		fio_close(&fp_r);
		unlink(tmpFile);
		// 終わり
	} while (0);
	PRINT_FN_LOG(param->debug_upconv,"SamplingRateConvert:End");
	param->outSample = outSample;
}

//---------------------------------------------------------------------------
// Function   : anaLfaParam
// Description: LFA パラメーター作成
// ---
//	param		:変換パラメータ
//
void anaLFA_Param(FFT_PARAM *param)
{
	int i,ii,n;
	long double ref_pw,pw;

	PRINT_FN_LOG("upconv:x","-anaLFA_Param:Start");

	ref_pw = 0;
	for (i = 200,n = 0;n < 20;i++,n++) {
		ref_pw += param->eq_ref_avg[i];
	}
	if (n > 0) {
		ref_pw /= n;
	}

	for (i = 210;i >= 20;) {
		pw = 0;
		for (ii = i,n = 0;n < 20;ii++,n++) {
			pw += param->eq_ref_avg[ii];
		}
		if (n > 0) {
			pw /= n;
		}
		if ((pw / ref_pw) >= 0.8 && (pw / ref_pw) <= 1.0) {
			for (ii = i,n = 0;n < 20;ii++,n++) {
				param->lfa_eq[ii] = 0.96 / (pw / ref_pw);
			}
		}
		i -= n;
	}
	PRINT_FN_LOG("upconv:x","-anaLFA_Param:End");
}
//---------------------------------------------------------------------------
// Function   : anaHFC_autoParam
// Description: HFC Auto パラメーター作成
// ---
//	param		:変換パラメータ
//
void anaHFC_AutoParam(FFT_PARAM *param)
{
	
	long index,i,n;
	double p;
	static double pw[384001];

	PRINT_FN_LOG("upconv:x","-anaHFC_AutoParam:Start");

	for (i = 0;i < 384000;i++) {
		pw[i]  = 0;
	}

	// 200Hz ごとの平均をとる
	for (index = 1;index + 100 < 384000;index += 100) {
		p = 0;
		for (i = index,n = 0;n < 100;n++,i++) {
			p += param->eq_ref_avg[i];
		}
		if (n > 0) {
			p /= n;
		}
		for (i = index,n = 0;n < 100;n++,i++) {
			pw[i] = p;
		}
	}

	for (i = 384000;i > 0;i--) {
		if (pw[i] < (double)10000000000000000) {
			pw[i] = 0;
		} else {
			break;
		}
	}

	for (i = 384000;i > 0;i--) {
		if (pw[i] != 0) {
			break;
		}
	}

	if (i < param->inSampleR / 2) {
		param->hfc = i;
		if (param->hfc < 11000) param->hfc = 11000;
	} else {
		param->hfc = param->inSampleR / 2;
	}
	PRINT_FN_LOG("upconv:x","-anaHFC_AutoParam:End");
}

//---------------------------------------------------------------------------
// Function   : adjBitExtension
// Description: ビット分解能を高める処理
// ---
//	inSample 	:処理するサンプル数
//	fp			:入力ファイル
//	tmpFp		:出力ファイル
//	param		:変換パラメータ
//
void adjBitExtension(SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4;
	SSIZE level,level2;
	SSIZE memTotal;
	double nx;
	long wkMemSize;
	long inSampleR;
	long fftSize,i,j,k,n;
	long cnt;
	long cutOff;
	long hfc;
	long wr;
	double percent,per;
	SSIZE *pIn,*pIn2,*pOut;
	SSIZE d1,d2,d3,d4,d5,d6,d7,d8;
	SSIZE dd1,dd2,dd3,dd4;
	SSIZE startInSample,nSample;
	SSIZE outRemain;
	SSIZE max_level;
	SSIZE avg_maxper;
	SSIZE count_max;
	SSIZE count_sample;
	SSIZE data_max_level;
	double ad_per;
	int    ad_cnt;
	double samp[9];
	double power;
	double shresh;
	int next;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;
	int sm_ignore;
	SSIZE sm_avg;

	memTotal = 0;

	if (param->p->post_abe == 2) {
		// NR や HFA 処理によるノイズ除去処理(音量が小さくなってしまう問題の回避)
		return adjBitExtension_10x(inSample,fp_r,fp_w,param);
	}

	if (param->p->abe_version == 1) {
		return adjBitExtension_0774(inSample,fp_r,fp_w,param);
	} else if (param->p->abe_version == 3) {
		return adjBitExtension_086(inSample,fp_r,fp_w,param);
	} else if (param->p->abe_version == 4) {
		return adjBitExtension_10x(inSample,fp_r,fp_w,param);
	}

	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension:Start");

	fio_rewind(fp_r);

	inSampleR = param->p->inSampleR;

	ad_per = ((double)44100 / param->p->inSampleR);

	fftSize = inSampleR * 2;
	wkMemSize = fftSize * 2;

	memTotal += (wkMemSize * sizeof (SSIZE));
	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += (wkMemSize * sizeof (SSIZE));
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += (wkMemSize * sizeof (SSIZE));
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += (wkMemSize * sizeof (SSIZE));
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += sizeof(fftw_complex) * wkMemSize;
	fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * wkMemSize;
	fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
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

	if (param->p->iw == 16) {
		if (param->p->abe_clld_level == 10) {
			shresh = (double)6400000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 9) {
			shresh = (double)5500000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 8) {
			shresh = (double)4200000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 7) {
			shresh = (double)3200000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 6) {
			shresh = (double)2300000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 5) {
			shresh = (double)1800000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 4) {
			shresh = (double)1500000000000000.0;	// wavegen 400Hz -96db
		} else if (param->p->abe_clld_level == 3) {
			shresh = (double)1200000000000000.0;	// wavegen 400Hz -96db
		} else if (param->p->abe_clld_level == 2) {
			shresh = (double)900000000000000.0;	// wavegen 400Hz -96db
		} else if (param->p->abe_clld_level == 1) {
			shresh = (double)600000000000000.0;	// wavegen 400Hz -96db
		}
	} else if (param->p->iw == 24) {
		if (param->p->abe_clld_level == 10) {
			shresh = (double)72000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 9) {
			shresh = (double)67000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 8) {
			shresh = (double)62000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 7) {
			shresh = (double)47000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 6) {
			shresh = (double)42000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 5) {
			shresh = (double)37000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 4) {
			shresh = (double)32000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 3) {
			shresh = (double)27000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 2) {
			shresh = (double)22000000000000.0;	// 1k
		} else if (param->p->abe_clld_level == 1) {
			shresh = (double)12000000000000.0;	// 1k
		}
	} else {
		shresh = -1;
	}
	outRemain = inSample;
	per = -1;

	avg_maxper = 0;
	data_max_level = 0;

	for (startInSample = (((fftSize * 2) + (fftSize / 2)) * -1);startInSample < inSample + (fftSize * 1);startInSample += fftSize) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
//				Sleep(1);
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		memset(mem1,00,wkMemSize * sizeof (SSIZE));
		nSample = fftSize * 2;

		if (startInSample >= 0 && startInSample + (fftSize * 1) < inSample + fftSize) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSize * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else if (startInSample + (fftSize * 1) >= 0 || startInSample < inSample) {
			mem0 = mem1;
			nSample = 0;
			if (startInSample < 0) {
				fio_seek(fp_r,0,SEEK_SET);
				if ((startInSample * -1) < fftSize * 2) {
					mem0 += (startInSample * -1);
					nSample = (fftSize * 2) + startInSample;
				}
			} else if (startInSample < inSample) {
//				fio_seek(fp_r,startInSample,SEEK_SET);
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			}
			if (nSample > 0 && startInSample < inSample && startInSample + nSample > inSample) {
				nSample = inSample - startInSample;
			}

			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}

		// 音のレベルを調査しておく
		if (param->p->abeNX) {
			level = 0;
			for (i = fftSize / 2,j = 0,n = 0;n < fftSize;i++,j++,n++) {
				if (mem1[i] > 0) {
					level += mem1[i] >> (56 - 16);
					j++;
				}
			}
			if (j > 0) {
				level /= j;
			}
		}

#if 1
		if (param->p->abeFnLevel > 0) {
			// ディザキャンセラ
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			// ２値平均
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
//				if (dd1 < 0) {
//					dd1 *= -1;
//				}
//				if (dd2 < 0) {
//					dd2 *= -1;
//				}
//				if (dd3 < 0) {
//					dd3 *= -1;
//				}
				dd1 /= param->p->abeFnLevel;
				dd2 /= param->p->abeFnLevel;
				dd3 /= param->p->abeFnLevel;
				sm_ignore = 1;
				if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
					sm_ignore = 0;
				}
				if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
					sm_ignore = 0;
				}

				if (sm_ignore == 0) {
					sm_avg = 0;
					sm_avg += pIn[i + 0];
					sm_avg += pIn[i + 1];
					sm_avg /= 2;
					pOut[i] = sm_avg;
				}
			}
			if (param->p->abeFnLevel > 1) {
				// 3値平均
				for (i = nSample - 1;i + 1 > 0;i--) {
					d1 = pIn[i - 0];
					d2 = pIn[i - 1];
					d3 = pIn[i - 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
//					if (dd1 < 0) {
//						dd1 *= -1;
//					}
//					if (dd2 < 0) {
//						dd2 *= -1;
//					}
//					if (dd3 < 0) {
//						dd3 *= -1;
//					}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}

					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i - 0];
						sm_avg += pIn[i - 1];
						sm_avg += pIn[i - 2];
						sm_avg /= 3;
						pOut[i] = sm_avg;
					}
				}
			}
			if (param->p->abeFnLevel > 3) {
				// 3値平均
				for (i = 0;i + 1 < nSample;i++) {
					d1 = pIn[i + 0];
					d2 = pIn[i + 1];
					d3 = pIn[i + 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
//					if (dd1 < 0) {
//						dd1 *= -1;
//					}
//					if (dd2 < 0) {
//						dd2 *= -1;
//					}
//					if (dd3 < 0) {
//						dd3 *= -1;
//					}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}

					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i + 0];
						sm_avg += pIn[i + 1];
						sm_avg += pIn[i + 2];
						sm_avg /= 3;
						pOut[i] = sm_avg;
					}
				}
			}
		}
#endif
#if 1
		if (param->p->smLowData > 0) {
			// 2値同値でその左右隣が異なる値の調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 3 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				d4 = pIn[i + 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i++;
					}
				}
			}
			// 2値同値でその左右隣が異なる値の調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 2;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				d4 = pIn[i - 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i--;
					}
				}
			}
			// 山や谷の形の波形調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 山や谷の形の波形調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 2;
				dd2 >>= 2;
				dd3 >>= 2;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 3;
				dd2 >>= 3;
				dd3 >>= 3;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
		}
#endif

#if 1
		if (param->p->cutLowData && shresh > 0) {
			// 閾値より低いパワーの音は削減する
			pIn = mem1;
			pIn2 = mem2;
			pOut = mem3;
			memset(mem2,1,wkMemSize * sizeof (SSIZE));	// 無音フラグ(無音だと0)
			memset(mem3,0,wkMemSize * sizeof (SSIZE));
			for (i = 0;i < wkMemSize;i++) {
				if (pIn[i] == 0) pIn2[i] = 0;
			}
			for (n = 0;n < 3;n++) {
				// FFT 初期設定
				copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);

				// 窓関数
				windowFFTW(fftw_in,fftSize);

				// FFT
				fftw_execute(fftw_p);

				// 削除
				for (i = 1;i < fftSize / 2;i++) {
					power = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
					if (power > 0) {
						power = sqrt(power);
					}
					if (power < shresh) {
						fftw_out[i][0] = 0;
						fftw_out[i][1] = 0;
					}
				}

				// 半分のデータを復元
				#pragma omp parallel for
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[fftSize - i][0] = fftw_out[i][0];
					fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
				}

				// invert FFT
				fftw_execute(fftw_ip);

				// 出力
				pOut = (SSIZE *)mem3;
				pOut = &pOut[(fftSize / 2) * n];
				#pragma omp parallel for
				for (i = 0;i < fftSize;i++) {
					pOut[i] += fftw_in[i][0] / fftSize;
				}
			}
			pOut = (SSIZE *)mem3;
			for (i = 0;i < wkMemSize;i++) {
				if (pIn2[i] == 0) pOut[i] = 0;
			}
			memcpy(mem1,mem3,wkMemSize * sizeof (SSIZE));
		}
#endif

#if 1
		// 適応型フィルター処理
		// 同値が続く場合に同値の個数に応じたfftで高域カットフィルターをする。
		// フィルター後は波形がなめらかになるように制御する。
		memset(mem2,0,wkMemSize * sizeof (SSIZE));
		memset(mem3,0,wkMemSize * sizeof (SSIZE));
		memset(mem4,0,wkMemSize * sizeof (SSIZE));
		if (param->p->abe_ad_filter == 1) {
			pIn = mem1;							// 入力波形
			pOut = mem2;						// 同値個数
			// mem2に同値が続く場合に同値の個数を記録していく。
			if (param->p->abe_declip == 0) {
				for (i = 0,j = 1,cnt = 1;j < nSample;j++) {
					d1 = pIn[i] >> (56 - param->p->iw);
					d2 = pIn[j] >> (56 - param->p->iw);
					if (d1 == d2 && cnt < 255) {
						cnt++;
					} else {
						for (k = i;k < j;k++) {
							pOut[k] = cnt;
						}
						i = j;
						cnt = 1;
					}
				}
			} else {
				for (i = 0,j = 1,cnt = 1;j < nSample;j++) {
					d1 = pIn[i] >> (56 - param->p->iw);
					d2 = pIn[j] >> (56 - param->p->iw);
					if (d1 != 0 && d1 == d2 && cnt < 2048 * 4 && ((pIn[j] > 0 && pIn[j] >= max_level) || (pIn[j] < 0 && pIn[j] <= max_level * -1))) {
						cnt++;
					} else {
						for (k = i;k < j;k++) {
							pOut[k] = cnt;
						}
						i = j;
						cnt = 1;
					}
				}
				if (cnt >= 3) {
					
				}
			}
			for (i = 0;i < nSample;i++) {
				if (pOut[i] >= 0 && pOut[i] < 2) {
					pOut[i] = 0;
				}
			}
			// 同値が3つ以上続くものに対して、fft をかける
			do {
				pIn = mem1;
				pIn2 = mem2;
				memset(mem3,0,wkMemSize * sizeof (SSIZE));
				cnt = 0;
				for (i = 0;i < nSample;i++) {
					if (pIn2[i] > 0) {
						cnt = pIn2[i];
						break;
					}
				}
				if (cnt == 0) {
					break;
				}
				for (n = 0;n < 3;n++) {
					// FFT 初期設定
					copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);
					
					// 窓関数
					windowFFTW(fftw_in,fftSize);

					// FFT
					fftw_execute(fftw_p);

					if (cnt > 0) {
						// 高域削除
						if (((double)cnt * ad_per) > 0) {
							hfc = (SSIZE)(22050.0 / ((double)cnt * ad_per));
							hfc = (SSIZE)((double)hfc * 1.1);
						} else {
							hfc = 0;
						}
						cutOff = ((double)fftSize / inSampleR) * hfc;
						if (cutOff > 0 && hfc >= 100) {
							for (i = cutOff;i < fftSize;i++) {
								fftw_out[i][0] = 0;
								fftw_out[i][1] = 0;
							}
						}
					}
					// 半分のデータを復元
					//#pragma omp parallel for
					for (i = 1;i < fftSize / 2;i++) {
						fftw_out[fftSize - i][0] = fftw_out[i][0];
						fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
					}

					// invert FFT
					fftw_execute(fftw_ip);

					// 出力
					pOut = (SSIZE *)&mem3[((fftSize / 2) * n)];
					//#pragma omp parallel for
					for (i = 0;i < fftSize;i++) {
						pOut[i] += fftw_in[i][0] / fftSize;
					}
				}
				pIn   = (SSIZE *)mem3;
				pIn2  = (SSIZE *)mem2;
				pOut  = (SSIZE *)mem4;
				for (i = 0;i < nSample;i++) {
					if (pIn2[i] > 0 && pIn2[i] == cnt) {
						pOut[i] = pIn[i];
						pIn2[i] *= -1;
					}
				}
			} while (1);
			pIn = (SSIZE *)mem4;
			pIn2 = (SSIZE *)mem2;
			pOut = (SSIZE *)mem1;
			if (param->p->abe_declip == 0) {
				for (i = 0;i < nSample;i++) {
					d1 = pIn[i];
					d1 >>= (56 - param->p->iw);
					d2 = pOut[i];
					d2 >>= (56 - param->p->iw);
					if (d1 <= d2 && (d2 - d1) <= 3) {
						pOut[i] = pIn[i];
					} else if (d1 > d2 && (d1 - d2) <= 3) {
						pOut[i] = pIn[i];
					}
				}
			} else {
				int init_cnt;
				for (i = 0,init_cnt = 0;i < nSample;i++) {
					if (init_cnt > 0 && pIn2[i] != init_cnt * -1) init_cnt = pIn2[i] * -1;
					if ((pOut[i] > 0 && pIn[i] > 0)) {
						if (i > 0 && init_cnt == 0) {
							init_cnt = pIn2[i] * -1;
							d1 = pIn[i];
							d2 = pIn[i + 1];
							d3 = pOut[i-1];
							d1 = (d3 - d1) + (d2 - d1);
						}
						d1 = 0;
						pOut[i] = pIn[i] + d1;
					}
					if (pOut[i] < 0 && pIn[i] < 0) {
						if (i > 0 && init_cnt == 0) {
							init_cnt = pIn2[i] * -1;
							d1 = pIn[i];
							d2 = pIn[i + 1];
							d3 = pOut[i-1];
							d1 = (d1 - d3) + (d1 - d2);
						}
						d1 = 0;
						pOut[i] = pIn[i] - d1;
					}
				}
			}
		}
#endif

#if 1
		// 波形の調整処理
		// 上がりや、下がりの波形の量子化誤差を少なくする
		pIn  = (SSIZE *)mem1;
		pOut = (SSIZE *)mem1;
		for (i = 4;i + 3 < nSample;) {
			next = 1;
			d1 = pIn[i - 4];
			d2 = pIn[i - 3];
			d3 = pIn[i - 2];
			d4 = pIn[i - 1];
			d5 = pIn[i];
			d6 = pIn[i + 1];
			d7 = pIn[i + 2];
			d8 = pIn[i + 3];
			if ((d2 < d3 && d3 <= d4 && d4 < d5 && d5 <= d6 && d6 < d7) ||
						(d2 <= d3 && d3 < d4 && d4 <= d5 && d5 < d6 && d6 <= d7)) {
				// 上がり波形
				samp[1] = pIn[i - 2] - pIn[i - 3];
				samp[2] = pIn[i - 1] - pIn[i - 2];
				samp[3] = pIn[i] - pIn[i - 1];
				samp[4] = pIn[i + 1] - pIn[i];
				samp[5] = pIn[i + 2] - pIn[i + 1];
				for (j = 1;j < 5;j++) {
					for (k = j + 1;k < 6;k++) {
						if (samp[j] > samp[k]) {
							samp[8] = samp[j];
							samp[j] = samp[k];
							samp[k] = samp[8];
						}
					}
				}
				samp[2] = samp[2] + samp[3] + samp[4];
				samp[2] /= 3;
				d1 = pIn[i];
				d2 = pIn[i - 1] + (SSIZE)samp[2];
				d1 >>= (56 - param->p->iw);
				d2 >>= (56 - param->p->iw);
				if (d1 == d2) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 < d2 && (d2 - d1) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 > d2 && (d1 - d2) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				}

			} else if ((d2 > d3 && d3 >= d4 && d4 > d5 && d5 >= d6 && d6 > d7) ||
						(d2 >= d3 && d3 > d4 && d4 >= d5 && d5 > d6 && d6 >= d7)) {
				// 下がり波形
				samp[1] = pIn[i - 2] - pIn[i - 3];
				samp[2] = pIn[i - 1] - pIn[i - 2];
				samp[3] = pIn[i] - pIn[i - 1];
				samp[4] = pIn[i + 1] - pIn[i];
				samp[5] = pIn[i + 2] - pIn[i + 1];
				for (j = 1;j < 5;j++) {
					for (k = j + 1;k < 6;k++) {
						if (samp[j] > samp[k]) {
							samp[8] = samp[j];
							samp[j] = samp[k];
							samp[k] = samp[8];
						}
					}
				}
				samp[2] = samp[2] + samp[3] + samp[4];
				samp[2] /= 3;
				d1 = pIn[i];
				d2 = pIn[i - 1] + (SSIZE)samp[2];
				d1 >>= (56 - param->p->iw);
				d2 >>= (56 - param->p->iw);
				if (d1 == d2) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 < d2 && (d2 - d1) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 > d2 && (d1 - d2) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				}
			}
			i += next;
		}
#endif

		if (startInSample + (fftSize / 2) >= 0) {
#if 0
			// 音のレベルに変化がないか調査
			if (param->p->abeNX == 1) {
				level2 = 0;
				for (i = fftSize / 2,j = 0,n = 0;n < fftSize;i++,j++,n++) {
					if (mem1[i] > 0) {
						level2 += mem1[i] >> (56 - 16);
						j++;
					}
				}
				if (j > 0) {
					level2 /= j;
				}
				if (level > 0 && level2 > 0) {
					nx = ((double)level / (double)level2);
					for (i = fftSize / 2,n = 0;n < fftSize;i++,n++) {
						mem1[i] = (SSIZE)((double)mem1[i] * nx);
					}
				}
			}
#endif
			if (outRemain >= fftSize) {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),fftSize,fp_w);
				if (wr != fftSize) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			} else {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			}
			if (outRemain >= fftSize) {
				outRemain -= fftSize;
			} else {
				break;
			}
		}
	}
	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);

	if (param->maxMemorySize < memTotal) {
		param->maxMemorySize = memTotal;
	}

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);
	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension:End");
}

//---------------------------------------------------------------------------
// Function   : adjBitExtension0774
// Description: ビット分解能を高める処理
// ---
//	inSample 	:処理するサンプル数
//	fp			:入力ファイル
//	tmpFp		:出力ファイル
//	param		:変換パラメータ
//
void adjBitExtension_0774(SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4;
	SSIZE level,level2;
	double nx;
	long wkMemSize;
	long inSampleR;
	long fftSize,i,j,k,n;
	long cnt;
	long cutOff;
	long hfc;
	long wr;
	double percent,per;
	SSIZE *pIn,*pIn2,*pOut;
	SSIZE d1,d2,d3,d4,d5,d6,d7,d8;
	SSIZE dd1,dd2,dd3,dd4;
	SSIZE startInSample,nSample;
	SSIZE outRemain;
	double samp[9];
	double power;
	double shresh;
	int next;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;
	int sm_ignore;
	SSIZE sm_avg;

	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension_0774:Start");

	fio_rewind(fp_r);

	inSampleR = param->p->inSampleR;

	fftSize = inSampleR * 2;
	wkMemSize = fftSize * 2;

	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
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

	if (param->p->iw == 16) {
		shresh = (double)12592500000000000.0;	// 1k
		shresh = (double)4200000000000000.0;	// 1k
	} else if (param->p->iw == 24) {
		shresh = (double)85120000000000.0;	// 1k

	} else {
		shresh = -1;
	}
	outRemain = inSample;
	per = -1;
	for (startInSample = (((fftSize * 2) + (fftSize / 2)) * -1);startInSample < inSample + (fftSize * 1);startInSample += fftSize) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			if (percent != per) {
//				Sleep(1);
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		memset(mem1,00,wkMemSize * sizeof (SSIZE));
		nSample = fftSize * 2;

		if (startInSample >= 0 && startInSample + (fftSize * 1) < inSample + fftSize) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSize * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else if (startInSample + (fftSize * 1) >= 0 || startInSample < inSample) {
			mem0 = mem1;
			nSample = 0;
			if (startInSample < 0) {
				fio_seek(fp_r,0,SEEK_SET);
				if ((startInSample * -1) < fftSize * 2) {
					mem0 += (startInSample * -1);
					nSample = (fftSize * 2) + startInSample;
				}
			} else if (startInSample < inSample) {
//				fio_seek(fp_r,startInSample,SEEK_SET);
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			}
			if (nSample > 0 && startInSample < inSample && startInSample + nSample > inSample) {
				nSample = inSample - startInSample;
			}

			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}
		
		// 音のレベルを調査しておく
		if (param->p->abeNX) {
			level = 0;
			for (i = fftSize / 2,j = 0,n = 0;n < fftSize;i++,j++,n++) {
				if (mem1[i] > 0) {
					level += mem1[i] >> (56 - 16);
					j++;
				}
			}
			if (j > 0) {
				level /= j;
			}
		}
#if 1
		if (param->p->abeFnLevel > 0) {
			// ディザキャンセラ
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			// ２値平均
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
//				if (dd1 < 0) {
//					dd1 *= -1;
//				}
//				if (dd2 < 0) {
//					dd2 *= -1;
//				}
//				if (dd3 < 0) {
//					dd3 *= -1;
//				}
				dd1 /= param->p->abeFnLevel;
				dd2 /= param->p->abeFnLevel;
				dd3 /= param->p->abeFnLevel;
				sm_ignore = 1;
				if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
					sm_ignore = 0;
				}
				if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
					sm_ignore = 0;
				}

				if (sm_ignore == 0) {
					sm_avg = 0;
					sm_avg += pIn[i + 0];
					sm_avg += pIn[i + 1];
					sm_avg /= 2;
					pOut[i] = sm_avg;
				}
			}
			if (param->p->abeFnLevel > 1) {
				// 3値平均
				for (i = nSample - 1;i + 1 > 0;i--) {
					d1 = pIn[i - 0];
					d2 = pIn[i - 1];
					d3 = pIn[i - 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
//					if (dd1 < 0) {
//						dd1 *= -1;
//					}
//					if (dd2 < 0) {
//						dd2 *= -1;
//					}
//					if (dd3 < 0) {
//						dd3 *= -1;
//					}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}

					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i - 0];
						sm_avg += pIn[i - 1];
						sm_avg += pIn[i - 2];
						sm_avg /= 3;
						pOut[i] = sm_avg;
					}
				}
			}
			if (param->p->abeFnLevel > 3) {
				// 3値平均
				for (i = 0;i + 1 < nSample;i++) {
					d1 = pIn[i + 0];
					d2 = pIn[i + 1];
					d3 = pIn[i + 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
//					if (dd1 < 0) {
//						dd1 *= -1;
//					}
//					if (dd2 < 0) {
//						dd2 *= -1;
//					}
//					if (dd3 < 0) {
//						dd3 *= -1;
//					}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}

					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i + 0];
						sm_avg += pIn[i + 1];
						sm_avg += pIn[i + 2];
						sm_avg /= 3;
						pOut[i] = sm_avg;
					}
				}
			}
		}
#endif
#if 1
		if (param->p->smLowData > 0) {
			// 2値同値でその左右隣が異なる値の調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 3 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				d4 = pIn[i + 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i++;
					}
				}
			}
			// 2値同値でその左右隣が異なる値の調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 2;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				d4 = pIn[i - 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i--;
					}
				}
			}
			// 山や谷の形の波形調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 山や谷の形の波形調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 2;
				dd2 >>= 2;
				dd3 >>= 2;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 3;
				dd2 >>= 3;
				dd3 >>= 3;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
		}
#endif

		if (param->p->cutLowData && shresh > 0) {
			// 閾値より低いパワーの音は削除する
			pIn = mem1;
			pIn2 = mem2;
			pOut = mem3;
			memset(mem2,1,wkMemSize * sizeof (SSIZE));	// 無音フラグ(無音だと0)
			memset(mem3,0,wkMemSize * sizeof (SSIZE));
			for (i = 0;i < wkMemSize;i++) {
				if (pIn[i] == 0) pIn2[i] = 0;
			}
			for (n = 0;n < 3;n++) {
				// FFT 初期設定
				copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);

				// 窓関数
				windowFFTW(fftw_in,fftSize);

				// FFT
				fftw_execute(fftw_p);

				// 削除
				for (i = 1;i < fftSize / 2;i++) {
					power = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
					if (power > 0) {
						power = sqrt(power);
					}
					if (power < shresh) {
						fftw_out[i][0] = 0;
						fftw_out[i][1] = 0;
					}
				}

				// 半分のデータを復元
				#pragma omp parallel for
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[fftSize - i][0] = fftw_out[i][0];
					fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
				}

				// invert FFT
				fftw_execute(fftw_ip);

				// 出力
				pOut = (SSIZE *)mem3;
				pOut = &pOut[(fftSize / 2) * n];
				#pragma omp parallel for
				for (i = 0;i < fftSize;i++) {
					pOut[i] += fftw_in[i][0] / fftSize;
				}
			}
			pOut = (SSIZE *)mem3;
			for (i = 0;i < wkMemSize;i++) {
				if (pIn2[i] == 0) pOut[i] = 0;
			}
			memcpy(mem1,mem3,wkMemSize * sizeof (SSIZE));
		}

#if 1
		// 適応型フィルター処理
		// 同値が続く場合に同値の個数に応じたfftで高域カットフィルターをする。
		// フィルター後は波形がなめらかになるように制御する。
		memset(mem2,0,wkMemSize * sizeof (SSIZE));
		memset(mem3,0,wkMemSize * sizeof (SSIZE));
		memset(mem4,0,wkMemSize * sizeof (SSIZE));
		if (param->p->abe_ad_filter == 1) {
			pIn = mem1;							// 入力波形
			pOut = mem2;						// 同値個数
			// mem2に同値が続く場合に同値の個数を記録していく。
			for (i = 0,j = 1,cnt = 1;j < nSample;j++) {
				d1 = pIn[i] >> (56 - param->p->iw);
				d2 = pIn[j] >> (56 - param->p->iw);
				if (d1 == d2 && cnt < 255) {
					cnt++;
				} else {
					for (k = i;k < j;k++) {
						pOut[k] = cnt;
					}
					i = j;
					cnt = 1;
				}
			}
			for (i = 0;i < nSample;i++) {
				if (pOut[i] >= 0 && pOut[i] < 2) {
					pOut[i] = 0;
				}
			}
			// 同値が3つ以上続くものに対して、fft をかける
			do {
				pIn = mem1;
				pIn2 = mem2;
				memset(mem3,0,wkMemSize * sizeof (SSIZE));
				cnt = 0;
				for (i = 0;i < nSample;i++) {
					if (pIn2[i] > 0) {
						cnt = pIn2[i];
						break;
					}
				}
				if (cnt == 0) {
					break;
				}
				for (n = 0;n < 3;n++) {
					// FFT 初期設定
					copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);
					
					// 窓関数
					windowFFTW(fftw_in,fftSize);

					// FFT
					fftw_execute(fftw_p);

					if (param->beInfo.cutOff[cnt] > 0) {
						// 高域削除
						hfc = inSampleR / param->beInfo.cutOff[cnt];
						cutOff = ((double)fftSize / inSampleR) * hfc;
						for (i = cutOff;i < fftSize;i++) {
							fftw_out[i][0] = 0;
							fftw_out[i][1] = 0;
						}
					}
					// 半分のデータを復元
					//#pragma omp parallel for
					for (i = 1;i < fftSize / 2;i++) {
						fftw_out[fftSize - i][0] = fftw_out[i][0];
						fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
					}

					// invert FFT
					fftw_execute(fftw_ip);

					// 出力
					pOut = (SSIZE *)&mem3[((fftSize / 2) * n)];
					//#pragma omp parallel for
					for (i = 0;i < fftSize;i++) {
						pOut[i] += fftw_in[i][0] / fftSize;
					}
				}
				pIn   = (SSIZE *)mem3;
				pIn2  = (SSIZE *)mem2;
				pOut  = (SSIZE *)mem4;
				for (i = 0;i < nSample;i++) {
					if (pIn2[i] > 0 && pIn2[i] == cnt) {
						pOut[i] = pIn[i];
						pIn2[i] *= -1;
					}
				}
			} while (1);
			pIn = (SSIZE *)mem4;
			pIn2 = (SSIZE *)mem2;
			pOut = (SSIZE *)mem1;
			for (i = 0;i < nSample;i++) {
				d1 = pIn[i];
				d1 >>= (56 - param->p->iw);
				d2 = pOut[i];
				d2 >>= (56 - param->p->iw);
				if (pIn2[i] < 0) {
					if (d1 <= d2 && (d2 - d1) <= 3) {
						pOut[i] = pIn[i];
					} else if (d1 > d2 && (d1 - d2) <= 3) {
						pOut[i] = pIn[i];
					}
				}
			}
		}
#endif

#if 1
		// 波形の調整処理
		// 上がりや、下がりの波形の量子化誤差を少なくする
		pIn  = (SSIZE *)mem1;
		pOut = (SSIZE *)mem1;
		for (i = 4;i + 3 < nSample;) {
			next = 1;
			d1 = pIn[i - 4];
			d2 = pIn[i - 3];
			d3 = pIn[i - 2];
			d4 = pIn[i - 1];
			d5 = pIn[i];
			d6 = pIn[i + 1];
			d7 = pIn[i + 2];
			d8 = pIn[i + 3];
			if ((d2 < d3 && d3 <= d4 && d4 < d5 && d5 <= d6 && d6 < d7) ||
						(d2 <= d3 && d3 < d4 && d4 <= d5 && d5 < d6 && d6 <= d7)) {
				// 上がり波形
				samp[1] = pIn[i - 2] - pIn[i - 3];
				samp[2] = pIn[i - 1] - pIn[i - 2];
				samp[3] = pIn[i] - pIn[i - 1];
				samp[4] = pIn[i + 1] - pIn[i];
				samp[5] = pIn[i + 2] - pIn[i + 1];
				for (j = 1;j < 5;j++) {
					for (k = j + 1;k < 6;k++) {
						if (samp[j] > samp[k]) {
							samp[8] = samp[j];
							samp[j] = samp[k];
							samp[k] = samp[8];
						}
					}
				}
				samp[2] = samp[2] + samp[3] + samp[4];
				samp[2] /= 3;
				d1 = pIn[i];
				d2 = pIn[i - 1] + (SSIZE)samp[2];
				d1 >>= (56 - param->p->iw);
				d2 >>= (56 - param->p->iw);
				if (d1 == d2) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 < d2 && (d2 - d1) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 > d2 && (d1 - d2) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				}

			} else if ((d2 > d3 && d3 >= d4 && d4 > d5 && d5 >= d6 && d6 > d7) ||
						(d2 >= d3 && d3 > d4 && d4 >= d5 && d5 > d6 && d6 >= d7)) {
				// 下がり波形
				samp[1] = pIn[i - 2] - pIn[i - 3];
				samp[2] = pIn[i - 1] - pIn[i - 2];
				samp[3] = pIn[i] - pIn[i - 1];
				samp[4] = pIn[i + 1] - pIn[i];
				samp[5] = pIn[i + 2] - pIn[i + 1];
				for (j = 1;j < 5;j++) {
					for (k = j + 1;k < 6;k++) {
						if (samp[j] > samp[k]) {
							samp[8] = samp[j];
							samp[j] = samp[k];
							samp[k] = samp[8];
						}
					}
				}
				samp[2] = samp[2] + samp[3] + samp[4];
				samp[2] /= 3;
				d1 = pIn[i];
				d2 = pIn[i - 1] + (SSIZE)samp[2];
				d1 >>= (56 - param->p->iw);
				d2 >>= (56 - param->p->iw);
				if (d1 == d2) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 < d2 && (d2 - d1) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 > d2 && (d1 - d2) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				}
			}
			i += next;
		}
#endif

		if (startInSample + (fftSize / 2) >= 0) {
#if 0
			// 音のレベルに変化がないか調査
			if (param->p->abeNX == 1) {
				level2 = 0;
				for (i = fftSize / 2,j = 0,n = 0;n < fftSize;i++,j++,n++) {
					if (mem1[i] > 0) {
						level2 += mem1[i] >> (56 - 16);
						j++;
					}
				}
				if (j > 0) {
					level2 /= j;
				}
				if (level > 0 && level2 > 0) {
					nx = ((double)level / (double)level2);
					for (i = fftSize / 2,n = 0;n < fftSize;i++,n++) {
						mem1[i] = (SSIZE)((double)mem1[i] * nx);
					}
				}
			}
#endif
			if (outRemain >= fftSize) {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),fftSize,fp_w);
				if (wr != fftSize) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			} else {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			}
			if (outRemain >= fftSize) {
				outRemain -= fftSize;
			} else {
				break;
			}
		}
	}
	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);
	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension_0774:End");
}

//---------------------------------------------------------------------------
// Function   : adjBitExtension_086
// Description: ビット分解能を高める処理
// ---
//	inSample 	:処理するサンプル数
//	fp			:入力ファイル
//	tmpFp		:出力ファイル
//	param		:変換パラメータ
//
void adjBitExtension_086(SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4;
	SSIZE level,level2;
	double nx;
	long wkMemSize;
	long inSampleR;
	long fftSize,i,j,k,n;
	long cnt;
	long cutOff;
	long hfc;
	long wr;
	long low_index;
	double percent,per;
	SSIZE *pIn,*pIn2,*pOut;
	SSIZE d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11;
	SSIZE dd1,dd2,dd3,dd4,dd5,dd6,dd7,dd8,dd9,dd10,dd11;
	SSIZE startInSample,nSample;
	SSIZE outRemain;
	double samp[9];
	double power;
	double shresh;
	double shresh_pw;

	int next;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;
	int sm_ignore;
	SSIZE sm_avg;
	char sss[256];

	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension_086:Start");

	fio_rewind(fp_r);

	inSampleR = param->p->inSampleR;

	fftSize = inSampleR * 2;
	wkMemSize = fftSize * 2;

	shresh_pw = 0;
	
	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
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

	if (param->p->iw == 16) {
		shresh = (double)1607008082939057.0;	// 44.1(20Hz)
	} else if (param->p->iw == 24) {
		shresh = (double)4910414169247.0;		// 44.1(20Hz)
	} else {
		shresh = -1;
	}
	outRemain = inSample;
	per = -1;
	for (startInSample = (((fftSize * 2) + (fftSize / 2)) * -1);startInSample < inSample + (fftSize * 1);startInSample += fftSize) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			if (percent != per) {
//				Sleep(1);
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		memset(mem1,00,wkMemSize * sizeof (SSIZE));
		nSample = fftSize * 2;

		if (startInSample >= 0 && startInSample + (fftSize * 1) < inSample + fftSize) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSize * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else if (startInSample + (fftSize * 1) >= 0 || startInSample < inSample) {
			mem0 = mem1;
			nSample = 0;
			if (startInSample < 0) {
				fio_seek(fp_r,0,SEEK_SET);
				if ((startInSample * -1) < fftSize * 2) {
					mem0 += (startInSample * -1);
					nSample = (fftSize * 2) + startInSample;
				}
			} else if (startInSample < inSample) {
//				fio_seek(fp_r,startInSample,SEEK_SET);
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			}
			if (nSample > 0 && startInSample < inSample && startInSample + nSample > inSample) {
				nSample = inSample - startInSample;
			}

			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}
		
		// 音のレベルを調査しておく
		if (param->p->abeNX) {
			level = 0;
			for (i = fftSize / 2,j = 0,n = 0;n < fftSize;i++,j++,n++) {
				if (mem1[i] > 0) {
					level += mem1[i] >> (56 - 16);
					j++;
				}
			}
			if (j > 0) {
				level /= j;
			}
		}

#if 1
		// 適応型フィルター処理
		// 同値が続く場合に同値の個数に応じたfftで高域カットフィルターをする。
		// フィルター後は波形がなめらかになるように制御する。
		memset(mem2,0,wkMemSize * sizeof (SSIZE));
		memset(mem3,0,wkMemSize * sizeof (SSIZE));
		memset(mem4,0,wkMemSize * sizeof (SSIZE));
		if (param->p->abe_ad_filter == 1) {
			pIn = mem1;							// 入力波形
			pOut = mem2;						// 同値個数
			// mem2に同値が続く場合に同値の個数を記録していく。
			for (i = 0,j = 1,cnt = 1;j < nSample;j++) {
				d1 = pIn[i] >> (56 - param->p->iw);
				d2 = pIn[j] >> (56 - param->p->iw);
				if (d1 == d2 && cnt < 255) {
					cnt++;
				} else {
					for (k = i;k < j;k++) {
						pOut[k] = cnt;
					}
					i = j;
					cnt = 1;
				}
			}
			for (i = 0;i < nSample;i++) {
				if (pOut[i] >= 0 && pOut[i] < 2) {
					pOut[i] = 0;
				}
			}
			// 同値が3つ以上続くものに対して、fft をかける
			do {
				pIn = mem1;
				pIn2 = mem2;
				memset(mem3,0,wkMemSize * sizeof (SSIZE));
				cnt = 0;
				for (i = 0;i < nSample;i++) {
					if (pIn2[i] > 0) {
						cnt = pIn2[i];
						break;
					}
				}
				if (cnt == 0) {
					break;
				}
				for (n = 0;n < 3;n++) {
					// FFT 初期設定
					copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);
					
					// 窓関数
					windowFFTW(fftw_in,fftSize);

					// FFT
					fftw_execute(fftw_p);

					if (param->beInfo.cutOff[cnt] > 0) {
						// 高域削除
						hfc = inSampleR / param->beInfo.cutOff[cnt];
						cutOff = ((double)fftSize / inSampleR) * hfc;
						for (i = cutOff;i < fftSize;i++) {
							fftw_out[i][0] = 0;
							fftw_out[i][1] = 0;
						}
					}
					// 半分のデータを復元
					//#pragma omp parallel for
					for (i = 1;i < fftSize / 2;i++) {
						fftw_out[fftSize - i][0] = fftw_out[i][0];
						fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
					}

					// invert FFT
					fftw_execute(fftw_ip);

					// 出力
					pOut = (SSIZE *)&mem3[((fftSize / 2) * n)];
					//#pragma omp parallel for
					for (i = 0;i < fftSize;i++) {
						pOut[i] += fftw_in[i][0] / fftSize;
					}
				}
				pIn   = (SSIZE *)mem3;
				pIn2  = (SSIZE *)mem2;
				pOut  = (SSIZE *)mem4;
				for (i = 0;i < nSample;i++) {
					if (pIn2[i] > 0 && pIn2[i] == cnt) {
						pOut[i] = pIn[i];
						pIn2[i] *= -1;
					}
				}
			} while (1);
			pIn = (SSIZE *)mem4;
			pIn2 = (SSIZE *)mem2;
			pOut = (SSIZE *)mem1;
			for (i = 0;i < nSample;i++) {
				d1 = pIn[i];
				d1 >>= (56 - param->p->iw);
				d2 = pOut[i];
				d2 >>= (56 - param->p->iw);
				if (pIn2[i] < 0) {
					if (d1 <= d2 && (d2 - d1) <= 3) {
						pOut[i] = pIn[i];
					} else if (d1 > d2 && (d1 - d2) <= 3) {
						pOut[i] = pIn[i];
					}
				}
			}
		}
#endif

#if 1
		if (param->p->cutLowData && shresh > 0) {
			// 閾値より低いパワーの音は削除する
			pIn = mem1;
			pIn2 = mem2;
			pOut = mem3;
			memset(mem2,1,wkMemSize * sizeof (SSIZE));	// 無音フラグ(無音だと0)
			memset(mem3,0,wkMemSize * sizeof (SSIZE));
			for (i = 0;i < wkMemSize;i++) {
				if (pIn[i] == 0) pIn2[i] = 0;
			}
			for (n = 0;n < 3;n++) {
				// FFT 初期設定
				copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);

				// 窓関数
				windowFFTW(fftw_in,fftSize);

				// FFT
				fftw_execute(fftw_p);

				// 削除
				for (i = 1;i < fftSize / 2;i++) {
					int sub_i;
					power = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
					if (power > 0) {
						power = sqrt(power);
					}
					low_index = ((double)fftSize / inSampleR) * 4000;
					if (i == low_index && shresh_pw < power) {
						shresh_pw = power;
					}
					if (power < shresh) {
						fftw_out[i][0] = fftw_out[i][0] / 10000;
						fftw_out[i][1] = fftw_out[i][1] / 10000;
					}
				}

				// 半分のデータを復元
				#pragma omp parallel for
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[fftSize - i][0] = fftw_out[i][0];
					fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
				}

				// invert FFT
				fftw_execute(fftw_ip);

				// 出力
				pOut = (SSIZE *)mem3;
				pOut = &pOut[(fftSize / 2) * n];
				#pragma omp parallel for
				for (i = 0;i < fftSize;i++) {
					pOut[i] += fftw_in[i][0] / fftSize;
				}
			}
			pOut = (SSIZE *)mem3;
			for (i = 0;i < wkMemSize;i++) {
				if (pIn2[i] == 0) pOut[i] = 0;
			}
			memcpy(mem1,mem3,wkMemSize * sizeof (SSIZE));
		}
#endif

#if 0	// 廃止予定(よくない作用のため)
		if (param->p->smLowData > 0) {
			// 2値同値でその左右隣が異なる値の調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 3 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				d4 = pIn[i + 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i++;
					}
				}
			}
			// 2値同値でその左右隣が異なる値の調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 2;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				d4 = pIn[i - 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i--;
					}
				}
			}
			// 山や谷の形の波形調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 山や谷の形の波形調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 2;
				dd2 >>= 2;
				dd3 >>= 2;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 3;
				dd2 >>= 3;
				dd3 >>= 3;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
		}
#endif

#if 1
		if ((param->p->abe == 0 || param->p->abe == 2) && param->p->abeFnLevel > 0) {
			// ディザキャンセラ
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			if (param->p->abe_2val_smooth) {
				// 2値平均
				for (i = 0;i + 2 < nSample;i++) {
					d1 = pIn[i + 0];
					d2 = pIn[i + 1];
					d3 = pIn[i + 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
	//				if (dd1 < 0) {
	//					dd1 *= -1;
	//				}
	//				if (dd2 < 0) {
	//					dd2 *= -1;
	//				}
	//				if (dd3 < 0) {
	//					dd3 *= -1;
	//				}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}

					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i + 0];
						sm_avg += pIn[i + 1];
						sm_avg /= 2;
						pOut[i] = sm_avg;
					}
				}
			}

			if (param->p->abeFnLevel > 1) {
				if (param->p->abe_3val_smooth) {
					// 3値平均
					for (i = nSample - 1;i + 1 > 0;i--) {
						d1 = pIn[i - 0];
						d2 = pIn[i - 1];
						d3 = pIn[i - 2];
						dd1 = d1;
						dd2 = d2;
						dd3 = d3;
						dd1 >>= (56 - param->p->iw);
						dd2 >>= (56 - param->p->iw);
						dd3 >>= (56 - param->p->iw);
	//					if (dd1 < 0) {
	//						dd1 *= -1;
	//					}
	//					if (dd2 < 0) {
	//						dd2 *= -1;
	//					}
	//					if (dd3 < 0) {
	//						dd3 *= -1;
	//					}
						dd1 /= param->p->abeFnLevel;
						dd2 /= param->p->abeFnLevel;
						dd3 /= param->p->abeFnLevel;
						sm_ignore = 1;
						if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
							sm_ignore = 0;
						}
						if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
							sm_ignore = 0;
						}
						if (sm_ignore == 0) {
							sm_avg = 0;
							sm_avg += pIn[i - 0];
							sm_avg += pIn[i - 1];
							sm_avg += pIn[i - 2];
							sm_avg /= 3;
							pOut[i] = sm_avg;
						}
					}
				}
				if (param->p->abeFnLevel > 3) {
					// 3値平均
					for (i = 0;i + 1 < nSample;i++) {
						d1 = pIn[i + 0];
						d2 = pIn[i + 1];
						d3 = pIn[i + 2];
						dd1 = d1;
						dd2 = d2;
						dd3 = d3;
						dd1 >>= (56 - param->p->iw);
						dd2 >>= (56 - param->p->iw);
						dd3 >>= (56 - param->p->iw);
	//					if (dd1 < 0) {
	//						dd1 *= -1;
	//					}
	//					if (dd2 < 0) {
	//						dd2 *= -1;
	//					}
	//					if (dd3 < 0) {
	//						dd3 *= -1;
	//					}
						dd1 /= param->p->abeFnLevel;
						dd2 /= param->p->abeFnLevel;
						dd3 /= param->p->abeFnLevel;
						sm_ignore = 1;
						if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
							sm_ignore = 0;
						}
						if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
							sm_ignore = 0;
						}
						if (sm_ignore == 0) {
							sm_avg = 0;
							sm_avg += pIn[i + 0];
							sm_avg += pIn[i + 1];
							sm_avg += pIn[i + 2];
							sm_avg /= 3;
							pOut[i] = sm_avg;
						}
					}
				}
			}
			if (param->p->abeFnLevel > 4) {
				if (param->p->abe_5val_smooth) {
					// 5値平均
					for (i = 0;i + 5 < nSample;i++) {
						d1 = pIn[i + 0];
						d2 = pIn[i + 1];
						d3 = pIn[i + 2];
						dd1 = d1;
						dd2 = d2;
						dd3 = d3;
						dd1 >>= (56 - param->p->iw);
						dd2 >>= (56 - param->p->iw);
						dd3 >>= (56 - param->p->iw);
	//					if (dd1 < 0) {
	//						dd1 *= -1;
	//					}
	//					if (dd2 < 0) {
	//						dd2 *= -1;
	//					}
	//					if (dd3 < 0) {
	//						dd3 *= -1;
	//					}
						dd1 /= param->p->abeFnLevel;
						dd2 /= param->p->abeFnLevel;
						dd3 /= param->p->abeFnLevel;
	//					sm_ignore = 1;
						if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
							sm_ignore = 0;
						}
						if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
							sm_ignore = 0;
						}
	sm_ignore = 0;
						if (sm_ignore == 0) {
							sm_avg = 0;
							sm_avg += pIn[i + 0];
							sm_avg += pIn[i + 1];
							sm_avg += pIn[i + 2];
							sm_avg += pIn[i + 3];
							sm_avg += pIn[i + 4];
							sm_avg /= 5;
							pOut[i] = sm_avg;
						}
					}
				}
			}
			if (param->p->abeFnLevel > 4) {
				if (param->p->abe_9val_smooth) {
					// 9値平均
					for (i = 0;i + 9 < nSample;i++) {
						d1 = pIn[i + 0];
						d2 = pIn[i + 1];
						d3 = pIn[i + 2];
						dd1 = d1;
						dd2 = d2;
						dd3 = d3;
						dd1 >>= (56 - param->p->iw);
						dd2 >>= (56 - param->p->iw);
						dd3 >>= (56 - param->p->iw);
	//					if (dd1 < 0) {
	//						dd1 *= -1;
	//					}
	//					if (dd2 < 0) {
	//						dd2 *= -1;
	//					}
	//					if (dd3 < 0) {
	//						dd3 *= -1;
	//					}
						dd1 /= param->p->abeFnLevel;
						dd2 /= param->p->abeFnLevel;
						dd3 /= param->p->abeFnLevel;
	//					sm_ignore = 1;
						if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
							sm_ignore = 0;
						}
						if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
							sm_ignore = 0;
						}
	sm_ignore = 0;
						if (sm_ignore == 0) {
							sm_avg = 0;
							sm_avg += pIn[i + 0];
							sm_avg += pIn[i + 1];
							sm_avg += pIn[i + 2];
							sm_avg += pIn[i + 3];
							sm_avg += pIn[i + 4];
							sm_avg += pIn[i + 5];
							sm_avg += pIn[i + 6];
							sm_avg += pIn[i + 7];
							sm_avg += pIn[i + 8];
							sm_avg /= 9;
							pOut[i] = sm_avg;
						}
					}
				}
			}
			if (param->p->abeFnLevel > 4) {
				if (param->p->abe_17val_smooth) {
					// 17値平均
					for (i = 0;i + 17 < nSample;i++) {
						d1 = pIn[i + 0];
						d2 = pIn[i + 1];
						d3 = pIn[i + 2];
						dd1 = d1;
						dd2 = d2;
						dd3 = d3;
						dd1 >>= (56 - param->p->iw);
						dd2 >>= (56 - param->p->iw);
						dd3 >>= (56 - param->p->iw);
	//					if (dd1 < 0) {
	//						dd1 *= -1;
	//					}
	//					if (dd2 < 0) {
	//						dd2 *= -1;
	//					}
	//					if (dd3 < 0) {
	//						dd3 *= -1;
	//					}
						dd1 /= param->p->abeFnLevel;
						dd2 /= param->p->abeFnLevel;
						dd3 /= param->p->abeFnLevel;
	//					sm_ignore = 1;
						if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
							sm_ignore = 0;
						}
						if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
							sm_ignore = 0;
						}
	sm_ignore = 0;
						if (sm_ignore == 0) {
							sm_avg = 0;
							sm_avg += pIn[i + 0];
							sm_avg += pIn[i + 1];
							sm_avg += pIn[i + 2];
							sm_avg += pIn[i + 3];
							sm_avg += pIn[i + 4];
							sm_avg += pIn[i + 5];
							sm_avg += pIn[i + 6];
							sm_avg += pIn[i + 7];
							sm_avg += pIn[i + 8];
							sm_avg += pIn[i + 9];
							sm_avg += pIn[i + 10];
							sm_avg += pIn[i + 11];
							sm_avg += pIn[i + 12];
							sm_avg += pIn[i + 13];
							sm_avg += pIn[i + 14];
							sm_avg += pIn[i + 15];
							sm_avg += pIn[i + 16];
							sm_avg /= 17;
							pOut[i] = sm_avg;
						}
					}
				}
			}
			if (param->p->abeFnLevel > 4) {
				if (param->p->abe_47val_smooth) {
					// 47値平均
					for (i = 0;i + 47 < nSample;i++) {
						d1 = pIn[i + 0];
						d2 = pIn[i + 1];
						d3 = pIn[i + 2];
						dd1 = d1;
						dd2 = d2;
						dd3 = d3;
						dd1 >>= (56 - param->p->iw);
						dd2 >>= (56 - param->p->iw);
						dd3 >>= (56 - param->p->iw);
	//					if (dd1 < 0) {
	//						dd1 *= -1;
	//					}
	//					if (dd2 < 0) {
	//						dd2 *= -1;
	//					}
	//					if (dd3 < 0) {
	//						dd3 *= -1;
	//					}
						dd1 /= param->p->abeFnLevel;
						dd2 /= param->p->abeFnLevel;
						dd3 /= param->p->abeFnLevel;
	//					sm_ignore = 1;
						if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
							sm_ignore = 0;
						}
						if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
							sm_ignore = 0;
						}
	sm_ignore = 0;
						if (sm_ignore == 0) {
							sm_avg = 0;
							sm_avg += pIn[i + 0];
							sm_avg += pIn[i + 1];
							sm_avg += pIn[i + 2];
							sm_avg += pIn[i + 3];
							sm_avg += pIn[i + 4];
							sm_avg += pIn[i + 5];
							sm_avg += pIn[i + 6];
							sm_avg += pIn[i + 7];
							sm_avg += pIn[i + 8];
							sm_avg += pIn[i + 9];
							sm_avg += pIn[i + 10];
							sm_avg += pIn[i + 11];
							sm_avg += pIn[i + 12];
							sm_avg += pIn[i + 13];
							sm_avg += pIn[i + 14];
							sm_avg += pIn[i + 15];
							sm_avg += pIn[i + 16];
							sm_avg += pIn[i + 17];
							sm_avg += pIn[i + 18];
							sm_avg += pIn[i + 19];
							sm_avg += pIn[i + 20];
							sm_avg += pIn[i + 21];
							sm_avg += pIn[i + 22];
							sm_avg += pIn[i + 23];
							sm_avg += pIn[i + 24];
							sm_avg += pIn[i + 25];
							sm_avg += pIn[i + 26];
							sm_avg += pIn[i + 27];
							sm_avg += pIn[i + 28];
							sm_avg += pIn[i + 29];
							sm_avg += pIn[i + 30];
							sm_avg += pIn[i + 31];
							sm_avg += pIn[i + 32];
							sm_avg += pIn[i + 33];
							sm_avg += pIn[i + 34];
							sm_avg += pIn[i + 35];
							sm_avg += pIn[i + 36];
							sm_avg += pIn[i + 37];
							sm_avg += pIn[i + 38];
							sm_avg += pIn[i + 39];
							sm_avg += pIn[i + 40];
							sm_avg += pIn[i + 41];
							sm_avg += pIn[i + 42];
							sm_avg += pIn[i + 43];
							sm_avg += pIn[i + 44];
							sm_avg += pIn[i + 45];
							sm_avg += pIn[i + 46];
							sm_avg /= 47;
							pOut[i] = sm_avg;
						}
					}
				}
			}
			if (param->p->abeFnLevel > 4) {
				if (param->p->abe_31val_smooth) {
					// 31値平均
					for (i = 0;i + 31 < nSample;i++) {
						d1 = pIn[i + 0];
						d2 = pIn[i + 1];
						d3 = pIn[i + 2];
						dd1 = d1;
						dd2 = d2;
						dd3 = d3;
						dd1 >>= (56 - param->p->iw);
						dd2 >>= (56 - param->p->iw);
						dd3 >>= (56 - param->p->iw);
	//					if (dd1 < 0) {
	//						dd1 *= -1;
	//					}
	//					if (dd2 < 0) {
	//						dd2 *= -1;
	//					}
	//					if (dd3 < 0) {
	//						dd3 *= -1;
	//					}
						dd1 /= param->p->abeFnLevel;
						dd2 /= param->p->abeFnLevel;
						dd3 /= param->p->abeFnLevel;
	//					sm_ignore = 1;
						if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
							sm_ignore = 0;
						}
						if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
							sm_ignore = 0;
						}
	sm_ignore = 0;
						if (sm_ignore == 0) {
							sm_avg = 0;
							sm_avg += pIn[i + 0];
							sm_avg += pIn[i + 1];
							sm_avg += pIn[i + 2];
							sm_avg += pIn[i + 3];
							sm_avg += pIn[i + 4];
							sm_avg += pIn[i + 5];
							sm_avg += pIn[i + 6];
							sm_avg += pIn[i + 7];
							sm_avg += pIn[i + 8];
							sm_avg += pIn[i + 9];
							sm_avg += pIn[i + 10];
							sm_avg += pIn[i + 11];
							sm_avg += pIn[i + 12];
							sm_avg += pIn[i + 13];
							sm_avg += pIn[i + 14];
							sm_avg += pIn[i + 15];
							sm_avg += pIn[i + 16];
							sm_avg += pIn[i + 17];
							sm_avg += pIn[i + 18];
							sm_avg += pIn[i + 19];
							sm_avg += pIn[i + 20];
							sm_avg += pIn[i + 21];
							sm_avg += pIn[i + 22];
							sm_avg += pIn[i + 23];
							sm_avg += pIn[i + 24];
							sm_avg += pIn[i + 25];
							sm_avg += pIn[i + 26];
							sm_avg += pIn[i + 27];
							sm_avg += pIn[i + 28];
							sm_avg += pIn[i + 29];
							sm_avg += pIn[i + 30];
							sm_avg /= 31;
							pOut[i] = sm_avg;
						}
					}
				}
			}
		}
#endif
#if 0
		if ((param->p->abe == 0 || param->p->abe == 2) && param->p->abe_rep_smooth == 1) {
			int abe_n;
			// ディザキャンセラ
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (abe_n = 0;abe_n < 48;abe_n++) {
				// 3値平均を繰り返す
				for (i = 0;i + 2 < nSample;i++) {
					d1 = pIn[i + 0];
					d2 = pIn[i + 1];
					d3 = pIn[i + 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
	//				if (dd1 < 0) {
	//					dd1 *= -1;
	//				}
	//				if (dd2 < 0) {
	//					dd2 *= -1;
	//				}
	//				if (dd3 < 0) {
	//					dd3 *= -1;
	//				}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}
					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i + 0];
						sm_avg += pIn[i + 1];
						sm_avg += pIn[i + 2];
						sm_avg /= 3;
						pOut[i] = sm_avg;
					}
				}
			}
		}
#endif
#if 1
		// 波形の調整処理
		// 上がりや、下がりの波形の量子化誤差を少なくする
		pIn  = (SSIZE *)mem1;
		pOut = (SSIZE *)mem1;
		for (i = 4;i + 3 < nSample;) {
			next = 1;
			d1 = pIn[i - 4];
			d2 = pIn[i - 3];
			d3 = pIn[i - 2];
			d4 = pIn[i - 1];
			d5 = pIn[i];
			d6 = pIn[i + 1];
			d7 = pIn[i + 2];
			d8 = pIn[i + 3];
			if ((d2 < d3 && d3 <= d4 && d4 < d5 && d5 <= d6 && d6 < d7) ||
						(d2 <= d3 && d3 < d4 && d4 <= d5 && d5 < d6 && d6 <= d7)) {
				// 上がり波形
				samp[1] = pIn[i - 2] - pIn[i - 3];
				samp[2] = pIn[i - 1] - pIn[i - 2];
				samp[3] = pIn[i] - pIn[i - 1];
				samp[4] = pIn[i + 1] - pIn[i];
				samp[5] = pIn[i + 2] - pIn[i + 1];
				for (j = 1;j < 5;j++) {
					for (k = j + 1;k < 6;k++) {
						if (samp[j] > samp[k]) {
							samp[8] = samp[j];
							samp[j] = samp[k];
							samp[k] = samp[8];
						}
					}
				}
				samp[2] = samp[2] + samp[3] + samp[4];
				samp[2] /= 3;
				d1 = pIn[i];
				d2 = pIn[i - 1] + (SSIZE)samp[2];
				d1 >>= (56 - param->p->iw);
				d2 >>= (56 - param->p->iw);
				if (d1 == d2) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 < d2 && (d2 - d1) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 > d2 && (d1 - d2) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				}

			} else if ((d2 > d3 && d3 >= d4 && d4 > d5 && d5 >= d6 && d6 > d7) ||
						(d2 >= d3 && d3 > d4 && d4 >= d5 && d5 > d6 && d6 >= d7)) {
				// 下がり波形
				samp[1] = pIn[i - 2] - pIn[i - 3];
				samp[2] = pIn[i - 1] - pIn[i - 2];
				samp[3] = pIn[i] - pIn[i - 1];
				samp[4] = pIn[i + 1] - pIn[i];
				samp[5] = pIn[i + 2] - pIn[i + 1];
				for (j = 1;j < 5;j++) {
					for (k = j + 1;k < 6;k++) {
						if (samp[j] > samp[k]) {
							samp[8] = samp[j];
							samp[j] = samp[k];
							samp[k] = samp[8];
						}
					}
				}
				samp[2] = samp[2] + samp[3] + samp[4];
				samp[2] /= 3;
				d1 = pIn[i];
				d2 = pIn[i - 1] + (SSIZE)samp[2];
				d1 >>= (56 - param->p->iw);
				d2 >>= (56 - param->p->iw);
				if (d1 == d2) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 < d2 && (d2 - d1) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				} else if (d1 > d2 && (d1 - d2) <= 3) {
					pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
				}
			}
			i += next;
		}
#endif

#if 0
		if (param->p->abe == 0 ||  param->p->abe == 2) {
			memset(mem2,0,wkMemSize * sizeof (SSIZE));
			pIn = mem1;							// 入力波形
			pOut = mem2;						// 出力波形

			for (n = 0;n < 3;n++) {
				// FFT 初期設定
				copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);
				
				// 窓関数
				windowFFTW(fftw_in,fftSize);

				// FFT
				fftw_execute(fftw_p);

				// 高域削除
				cutOff = ((double)fftSize / inSampleR) * ((param->p->outSampleR_final / 2) - 100);
				for (i = cutOff;i < fftSize;i++) {
					fftw_out[i][0] = 0;
					fftw_out[i][1] = 0;
				}
				// 半分のデータを復元
				//#pragma omp parallel for
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[fftSize - i][0] = fftw_out[i][0];
					fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
				}

				// invert FFT
				fftw_execute(fftw_ip);

				// 出力
				pOut = (SSIZE *)mem2;
				pOut = &pOut[(fftSize / 2) * n];
				//#pragma omp parallel for
				for (i = 0;i < fftSize;i++) {
					pOut[i] += fftw_in[i][0] / fftSize;
				}
			}
			memcpy(mem1,mem2,wkMemSize * sizeof (SSIZE));
		}
#endif

		if (startInSample + (fftSize / 2) >= 0) {
#if 0
			// 音のレベルに変化がないか調査
			if (param->p->abeNX == 1) {
				level2 = 0;
				for (i = fftSize / 2,j = 0,n = 0;n < fftSize;i++,j++,n++) {
					if (mem1[i] > 0) {
						level2 += mem1[i] >> (56 - 16);
						j++;
					}
				}
				if (j > 0) {
					level2 /= j;
				}
				if (level > 0 && level2 > 0) {
					nx = ((double)level / (double)level2);
					for (i = fftSize / 2,n = 0;n < fftSize;i++,n++) {
						mem1[i] = (SSIZE)((double)mem1[i] * nx);
					}
				}
			}
#endif
			if (outRemain >= fftSize) {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),fftSize,fp_w);
				if (wr != fftSize) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			} else {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			}
			if (outRemain >= fftSize) {
				outRemain -= fftSize;
			} else {
				break;
			}
		}
	}
	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);

	sprintf(sss,"shresh_pw:%lf",shresh_pw);
	PRINT_LOG("abe",sss);

	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension_086:End");
}

//---------------------------------------------------------------------------
// Function   : adjBitExtension_10x
// Description: ビット分解能を高める処理
// ---
//	inSample 	:処理するサンプル数
//	fp			:入力ファイル
//	tmpFp		:出力ファイル
//	param		:変換パラメータ
//
void adjBitExtension_10x(SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4,*mem5,*mem6;
	SSIZE level,level2;
	SSIZE non_zero_start,non_zero_end;
	SSIZE zero_start,zero_len;
	int   non_zero_flag;
	double nx;
	long wkMemSize;
	long inSampleR;
	SSIZE d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11;
	SSIZE dd1,dd2,dd3,dd4,dd5,dd6,dd7,dd8,dd9,dd10,dd11;
	SSIZE sm_avg;
	int sm_ignore;
	long wltSize,fftSize,i,j,k,n;
	long cnt;
	long cutOff;
	long hfc;
	long wr;
	long low_index;
	// 32000,44100,48000,88200,96000,176400,192000,352800,384000,705600,768000,1411200,1536000
	int  wlt_size_index[]={32000,44100,48000,88200,96000,176400,192000,352800,384000,705600,768000,1411200,1536000,3072000};
	long wlt_size_table[]={32768,65536,65536,131072,131072,262144,262144,524288,524288,1048576,1048576,2097152,2097152,2097152};
	double fft_level_16bit_table[]={400000000000000.0,500000000000000.0,600000000000000.0,700000000000000.0,800000000000000.0,900000000000000.0,1000000000000000.0,2000000000000000.0,3000000000000000.0,4000000000000000.0,
							 		4100000000000000.0,4200000000000000.0,4300000000000000.0,4400000000000000.0,4500000000000000.0,4600000000000000.0,4700000000000000.0,4800000000000000.0,4900000000000000.0,5000000000000000.0};
	double fft_level_24bit_table[]={4100000000.0,4200000000.0,4300000000.0,4400000000.0,4500000000.0,4600000000.0,4700000000.0,4800000000.0,4900000000.0,5000000000.0,
							 		5100000000.0,5200000000.0,5300000000.0,5400000000.0,5500000000.0,5600000000.0,5700000000.0,5800000000.0,5900000000.0,6000000000.0};

	int fft_level_shresh_table[]={-9,-8,-7,-6,-5,-4,-3,-2,-1,0,2,4,6,8,10,12,14,16,18,20};
	double fft_shresh;
	double percent,per;
	SSIZE *pIn,*pIn2,*pOut;
	SSIZE startInSample,nSample;
	SSIZE outRemain;
	double *gsl_wlt_data;
	double *gsl_wlt_abscoeff;
	size_t *gsl_wlt_p;
	long  wlt_index;
	long  wlt_noise_remain;
	signed long long int cut_mode;
	int shresh;
	double samp[9];
	double power;
	double shresh_pw;

	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;

	fftw_complex *fftw_in2,*fftw_out2;
	fftw_plan fftw_p2,fftw_ip2;

	int next;
	gsl_wavelet *wavelet;
	gsl_wavelet_workspace *gsl_workspace;
	char sss[256];

	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension_10x:Start");

	fio_rewind(fp_r);

	inSampleR = param->p->inSampleR;
	shresh_pw = 0;

	for (i = 0;i < sizeof (wlt_size_index);i++) {
		if (inSampleR == wlt_size_index[i]) break;
	}
	if (i == sizeof (wlt_size_index)) {
		param->err = STATUS_EXEC_FAIL;param->errLine = __LINE__;
		return;
	}
	
	wltSize = wlt_size_table[i];
	fftSize = wltSize;
	wkMemSize = wltSize * 2;

#if 0
	// ウェーブレット閾値
	shresh = -1;
	if (param->p->abe_10x_wlt_level > 0) {
		shresh = param->p->abe_10x_wlt_level;
		shresh = shresh + 10;
	}
#endif

	// FFT 閾値
	fft_shresh = -1;
	if (param->p->iw == 16) {
		if (param->p->abe_10x_cut_level > 0) {
			fft_shresh = 1;
		}
	}
	if (param->p->iw == 24) {
		if (param->p->abe_10x_cut_level > 0) {
			fft_shresh = 1;
		}
	}
	cut_mode = 10000000;
	if (param->p->abe_10x_cut_mode == 0) {
		cut_mode = 10000;
	} else if (param->p->abe_10x_cut_mode == 2) {
		cut_mode = 0;
	}


#if 0
	sprintf(sss,"Wavelet Size:%d",wltSize);
	PRINT_LOG("abe10x",sss);
	sprintf(sss,"abe_wlt_level:%d",param->p->abe_10x_wlt_level);
	PRINT_LOG("abe10x",sss);
#endif

	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem5 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem5 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	mem6 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem6 == NULL) {
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

	fftw_in2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_out == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p2 = fftw_plan_dft_1d(fftSize,fftw_in2,fftw_out2,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip2 = fftw_plan_dft_1d(fftSize,fftw_out2,fftw_in2,FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	
#if 0
	gsl_wlt_data = (double *)malloc(wltSize * sizeof (double));
	if (gsl_wlt_data == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	gsl_wlt_abscoeff = (double *)malloc(wltSize * sizeof (double));
	if (gsl_wlt_abscoeff == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	gsl_wlt_p = (size_t *)malloc(wltSize * sizeof (size_t));
	if (gsl_wlt_p == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
#endif

#if 0
	// Daubechies 14ウェーブレット
	//wavelet = gsl_wavelet_alloc(gsl_wavelet_daubechies_centered,4);
	// haar center
	wavelet = gsl_wavelet_alloc(gsl_wavelet_haar_centered,2);
	gsl_workspace = gsl_wavelet_workspace_alloc(wltSize);
#endif

	// 最初の無音区間と最後の無音区間を調べておく
	non_zero_start = non_zero_end = -1;
	non_zero_flag  = 1;
	for (startInSample = 0;startInSample < inSample;startInSample += wltSize * 2) {
		memset(mem1,0,wkMemSize * sizeof (SSIZE));
		nSample = wltSize * 2;

		fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
		if (startInSample + nSample < inSample) {
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else {
			nSample = inSample - startInSample;
			if (nSample > 0) {
				fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
			}
		}

		if (nSample <= 0) break;

		if (non_zero_flag == 1) {
			d1 = mem1[0];
			non_zero_flag = 0;
		}
		
		if (non_zero_start == -1) {
			for (i = 0;i < nSample;i++) {
				if (d1 != mem1[i]) break;
			}
		}
		if (i < nSample) {
			non_zero_start = startInSample + i;
			break;
		}
	}

	if (non_zero_start != -1) {
		non_zero_flag  = 1;
		startInSample = inSample - (wltSize * 2);
		if (startInSample < 0) {
			startInSample = 0;
		}
		for (;;) {
			memset(mem1,0,wkMemSize * sizeof (SSIZE));
			nSample = wltSize * 2;

			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			if (startInSample + nSample < inSample) {
				fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
			} else {
				nSample = inSample - startInSample;
				if (nSample > 0) {
					fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
				}
			}

			if (nSample <= 0) break;

			if (non_zero_flag == 1) {
				d1 = mem1[nSample - 1];
				non_zero_flag = 0;
			}
			
			if (non_zero_end == -1) {
				for (i = nSample - 1;i >= 0;i--) {
					if (d1 != mem1[i]) break;
				}
			}
			if (i >= 0) {
				non_zero_end = startInSample + i;
				break;
			}

			if (startInSample <= 0) break;
			
			if (startInSample > wltSize * 2) {
				startInSample = startInSample - wltSize * 2;
			} else {
				startInSample = 0;
			}
		}
	}

//	sprintf(sss,"non_zero_start:%lld",non_zero_start);
//	PRINT_LOG("ABE10x",sss);
//	sprintf(sss,"non_zero_end:%lld",non_zero_end);
//	PRINT_LOG("ABE10x",sss);

	outRemain = inSample;
	per = -1;
	for (startInSample = (((wltSize * 2) + (wltSize / 2)) * -1);startInSample < inSample + (wltSize * 1);startInSample += wltSize) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			if (percent != per) {
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						if (param->p->simple_mode == 0) {
							fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
							fflush(stdout);
						} else {
							if (param->p->simple_mode_done_percent_div > 0) {
								fprintf(stdout,"%d%%\n",(int)((percent / param->p->simple_mode_done_percent_div) + param->p->simple_mode_done_percent));
								fflush(stdout);
							}
						}
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		// 読み込んだ音
		memset(mem1,0,wkMemSize * sizeof (SSIZE));

		// 作業中の音
		memset(mem2,0,wkMemSize * sizeof (SSIZE));

		// 最終データ
		memset(mem3,0,wkMemSize * sizeof (SSIZE));

		// 読み込んだ音のバックアップ
		memset(mem4,0,wkMemSize * sizeof (SSIZE));

		nSample = wltSize * 2;

		if (startInSample >= 0 && startInSample + (wltSize * 1) < inSample + wltSize) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = wltSize * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else if (startInSample + (wltSize * 1) >= 0 || startInSample < inSample) {
			mem0 = mem1;
			nSample = 0;
			if (startInSample < 0) {
				fio_seek(fp_r,0,SEEK_SET);
				if ((startInSample * -1) < wltSize * 2) {
					mem0 += (startInSample * -1);
					nSample = (wltSize * 2) + startInSample;
				}
			} else if (startInSample < inSample) {
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			}
			if (nSample > 0 && startInSample < inSample && startInSample + nSample > inSample) {
				nSample = inSample - startInSample;
			}

			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = wltSize * 2;
		}

		memcpy(mem4,mem1,wkMemSize * sizeof (SSIZE));

		memcpy(mem3,mem1,wkMemSize * sizeof (SSIZE));

#if 1
		if (param->p->post_abe != 2 && param->p->abeFnLevel > 0) {
			// ディザキャンセラ
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			// ２値平均
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
//				if (dd1 < 0) {
//					dd1 *= -1;
//				}
//				if (dd2 < 0) {
//					dd2 *= -1;
//				}
//				if (dd3 < 0) {
//					dd3 *= -1;
//				}
				dd1 /= param->p->abeFnLevel;
				dd2 /= param->p->abeFnLevel;
				dd3 /= param->p->abeFnLevel;
				sm_ignore = 1;
				if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
					sm_ignore = 0;
				}
				if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
					sm_ignore = 0;
				}

				if (sm_ignore == 0) {
					sm_avg = 0;
					sm_avg += pIn[i + 0];
					sm_avg += pIn[i + 1];
					sm_avg /= 2;
					pOut[i] = sm_avg;
				}
			}
			if (param->p->abeFnLevel > 1) {
				// 3値平均
				for (i = nSample - 1;i + 1 > 0;i--) {
					d1 = pIn[i - 0];
					d2 = pIn[i - 1];
					d3 = pIn[i - 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
//					if (dd1 < 0) {
//						dd1 *= -1;
//					}
//					if (dd2 < 0) {
//						dd2 *= -1;
//					}
//					if (dd3 < 0) {
//						dd3 *= -1;
//					}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}

					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i - 0];
						sm_avg += pIn[i - 1];
						sm_avg += pIn[i - 2];
						sm_avg /= 3;
						pOut[i] = sm_avg;
					}
				}
			}
			if (param->p->abeFnLevel > 3) {
				// 3値平均
				for (i = 0;i + 1 < nSample;i++) {
					d1 = pIn[i + 0];
					d2 = pIn[i + 1];
					d3 = pIn[i + 2];
					dd1 = d1;
					dd2 = d2;
					dd3 = d3;
					dd1 >>= (56 - param->p->iw);
					dd2 >>= (56 - param->p->iw);
					dd3 >>= (56 - param->p->iw);
//					if (dd1 < 0) {
//						dd1 *= -1;
//					}
//					if (dd2 < 0) {
//						dd2 *= -1;
//					}
//					if (dd3 < 0) {
//						dd3 *= -1;
//					}
					dd1 /= param->p->abeFnLevel;
					dd2 /= param->p->abeFnLevel;
					dd3 /= param->p->abeFnLevel;
					sm_ignore = 1;
					if (dd1 + 1 == dd2 && dd2 == dd3 + 1) {
						sm_ignore = 0;
					}
					if (dd1 == dd2 + 1 && dd2 + 1 == dd3) {
						sm_ignore = 0;
					}

					if (sm_ignore == 0) {
						sm_avg = 0;
						sm_avg += pIn[i + 0];
						sm_avg += pIn[i + 1];
						sm_avg += pIn[i + 2];
						sm_avg /= 3;
						pOut[i] = sm_avg;
					}
				}
			}
		}
#endif
#if 1
		if (param->p->post_abe != 2 && param->p->smLowData > 0) {
			// 2値同値でその左右隣が異なる値の調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 3 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				d4 = pIn[i + 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i++;
					}
				}
			}
			// 2値同値でその左右隣が異なる値の調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 2;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				d4 = pIn[i - 3];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd4 = d4;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				dd4 >>= (56 - param->p->iw);
				if (dd1 != dd2 && dd2 == dd3 && dd3 != dd4) {
					sm_ignore = 0;
					if ((dd1 > dd2 && (dd1 - dd2) > 2) || (dd1 < dd2 && (dd2 - dd1) > 2)) {
						sm_ignore = 1;
					}
					if ((dd3 > dd4 && (dd3 - dd4) > 2) || (dd3 < dd4 && (dd4 - dd3) > 2)) {
						sm_ignore = 1;
					}
					if (sm_ignore == 0) {
						sm_avg = (d1 + d2 + d3) / 3;
						pOut[i + 1] = sm_avg;
						sm_avg = (d2 + d3 + d4) / 3;
						pOut[i + 2] = sm_avg;
						i--;
					}
				}
			}
			// 山や谷の形の波形調整
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 山や谷の形の波形調整(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (d1 < d2 && d2 > d3 && (dd2 - dd1) <= 2 && (dd2 - dd3) <= 2) {
					// 山
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				} else if (d1 > d2 && d2 < d3 && (dd1 - dd2) <= 2 && (dd3 - dd2) <= 2) {
					// 谷
					sm_avg = (((d1 + d3) / 2) + d2) / 2;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i + 2 < nSample;i++) {
				d1 = pIn[i + 0];
				d2 = pIn[i + 1];
				d3 = pIn[i + 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 2;
				dd2 >>= 2;
				dd3 >>= 2;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
			// 同値以外の移動平均(逆順)
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = nSample - 1;i > 1;i--) {
				d1 = pIn[i - 0];
				d2 = pIn[i - 1];
				d3 = pIn[i - 2];
				dd1 = d1;
				dd2 = d2;
				dd3 = d3;
				dd1 >>= (56 - param->p->iw);
				dd2 >>= (56 - param->p->iw);
				dd3 >>= (56 - param->p->iw);
				if (dd1 < 0) {
					dd1 *= -1;
				}
				if (dd2 < 0) {
					dd2 *= -1;
				}
				if (dd3 < 0) {
					dd3 *= -1;
				}
				dd1 >>= 3;
				dd2 >>= 3;
				dd3 >>= 3;
				sm_ignore = 1;
				if (dd1 == dd2 && dd2 == dd3) {
					sm_ignore = 0;
				}
				if (sm_ignore == 0) {
					sm_avg = (d1 + d2 + d3) / 3;
					pOut[i + 1] = sm_avg;
				}
			}
		}
#endif

#if 0
		if (param->p->abe_10x_wlt_level > 0) {
			for (n = 0;n < 3;n++) {
				for (i = 0;i < wltSize;i++) {
					gsl_wlt_data[i] = mem1[i + ((wltSize / 2) * n)];
				}
				// ウェーブレット変換
				gsl_wavelet_transform_forward(wavelet,gsl_wlt_data,1,wltSize,gsl_workspace);

				// 閾値操作
				for (i = 0;i < wltSize;i++) {
					gsl_wlt_abscoeff[i] = fabs(gsl_wlt_data[i]);
				}

				for (i = wltSize / 2;i < wltSize;i++) {
					if (gsl_wlt_abscoeff[i] > 0 && log10(gsl_wlt_abscoeff[i]) <= shresh) {
						gsl_wlt_data[i] = 0;
					}
				}
				// ウェーブレット逆変換
				gsl_wavelet_transform_inverse(wavelet,gsl_wlt_data,1,wltSize,gsl_workspace);
				for (i = 0;i < wltSize;i++) {
					mem2[i + ((wltSize / 2) * n)] = gsl_wlt_data[i];
				}
				// 三角窓関数
				#pragma omp parallel for
				for (i = 0;i < (wltSize - 1) / 2;i++) {
					mem2[i + ((wltSize / 2) * n)] = mem2[i + ((wltSize / 2) * n)] * (2.0 * i / (double)wltSize);
				}
				#pragma omp parallel for
				for (i = (wltSize - 1) / 2;i < wltSize;i++) {
					mem2[i + ((wltSize / 2) * n)] = mem2[i + ((wltSize / 2) * n)] * (2.0 - 2.0 * i / (double)wltSize);
				}
				for (i = 0;i < wltSize;i++) {
					mem3[i + ((wltSize / 2) * n)] += mem2[i + ((wltSize / 2) * n)];
				}
			}
			memcpy(mem1,mem3,wkMemSize * sizeof (SSIZE));
		}
#endif

#if 1
		hfc = param->p->inSampleR / 2;
		if (param->p->hfc != -1 && hfc > param->p->hfc) {
			hfc = param->p->hfc;
		}

		if (param->p->post_abe != 2 && (param->p->cutLowData && fft_shresh > 0)) {
			// 閾値より低いパワーの音は削除する
			double db_shift;
			double window_correction = 20.0 * log10(2.0 / fftSize);	// 三角窓の平均補正

			if (param->p->iw == 16) {
				db_shift = 20.0 * log10(pow(2, 56 - 16));
			}

			if (param->p->iw == 24) {
				db_shift = 20.0 * log10(pow(2, 56 - 24));
			}

			pIn = mem1;
			pIn2 = mem2;
			pOut = mem5;
			// mem1										// もともとの音
			memset(mem2,1,wkMemSize * sizeof (SSIZE));	// 無音フラグ(無音だと0)
			memset(mem5,0,wkMemSize * sizeof (SSIZE));	// 消したい量子化ノイズ成分(あとでマイナスする)
			memset(mem6,0,wkMemSize * sizeof (SSIZE));	// 足したいランダムノイズ成分(あとでプラスする)

			for (i = 0;i < wkMemSize;i++) {
				if (pIn[i] == 0) pIn2[i] = 0;
			}

			// 量子化ノイズ成分と思う成分を抽出しあとで元の音からマイナスする
			for (n = 0;n < 3;n++) {
				// FFT 初期設定
				copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);

				// 窓関数
				windowFFTW(fftw_in,fftSize);

				// FFT
				fftw_execute(fftw_p);

				// 量子化ノイズと思われるものをmem5に入れる
				for (i = 1;i < fftSize / 2;i++) {
					int hz;
					power = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
					power = sqrt(power);
					if (power > 0) {
						power = 20 * log10(power);
						power = power - db_shift;
						power = power - window_correction;
						power = power - 250;
					}
					hz = (int)(((double)inSampleR / fftSize) * i);

#if 0
					if (startInSample / inSampleR > 4 && startInSample / inSampleR <= 6 && param->print_stdout) {
						// 解析のためのログ出力
						sprintf(sss,"pw:%d,%lf",hz,power);
						PRINT_LOG("ABE10x",sss);
					}
#endif
					if (param->p->iw == 16) {
						if (power > (-97 + param->p->abe_10x_cut_level)) {
							fftw_out[i][0] = 0;
							fftw_out[i][1] = 0;
						}
					} else if (param->p->iw == 24) {
						if (power > (-144 + param->p->abe_10x_cut_level)) {
							fftw_out[i][0] = 0;
							fftw_out[i][1] = 0;
						}
					}
				}

				// 半分のデータを復元
				#pragma omp parallel for
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[fftSize - i][0] = fftw_out[i][0];
					fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
				}

				// invert FFT
				fftw_execute(fftw_ip);

				// 出力
				pOut = (SSIZE *)mem5;
				pOut = &pOut[(fftSize / 2) * n];
				#pragma omp parallel for
				for (i = 0;i < fftSize;i++) {
					pOut[i] += fftw_in[i][0] / fftSize;
				}
			}
			
			// 無音部分を処理対象外にする
			pOut = (SSIZE *)mem5;
			for (i = 0;i < wkMemSize;i++) {
				if (pIn2[i] == 0) pOut[i] = 0;
			}

			// ランダムノイズ成分の付加
			pIn = mem5;
			pIn2 = (SSIZE *)mem3;
			#pragma omp parallel for
			for (i = 0;i < wkMemSize;i++) {
				pIn2[i] = normalNoise() * 0x1000000000;
			}
			pOut = mem6;

			// 取り除いた量子化ノイズ成分と同等のパワーのノイズを付加するため、ノイズ成分を作成する
			for (n = 0;n < 3;n++) {
				long h;
				long nn;
				long lowIndex,highIndex;
				long hz;
				double avg_pw_mem5,avg_pw_mem3;
				double pw_nx;

				// FFT 初期設定
				copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);

				// 窓関数
				windowFFTW(fftw_in,fftSize);

				// FFT
				fftw_execute(fftw_p);

				// FFT 初期設定
				copyToFFTW(fftw_in2,&pIn2[((fftSize / 2) * n)],fftSize);

				// 窓関数
				windowFFTW(fftw_in2,fftSize);

				// FFT
				fftw_execute(fftw_p2);

				// 量子化ノイズと思われるものをmem5に入れる
				for (i = 0,h = 0;h < hfc - 500;i++,h += 500) {
					// 250hz 範囲のパワーを調べる
					lowIndex  = ((double)fftSize / inSampleR) * h;
					highIndex = ((double)fftSize / inSampleR) * (h + 500);
					
					avg_pw_mem5 = 0;
					avg_pw_mem3 = 0;
					for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
						power = (fftw_out[j][0] * fftw_out[j][0]) + (fftw_out[j][1] * fftw_out[j][1]);
						power = sqrt(power);
						avg_pw_mem5 += power;
					}
					if (nn > 0) {
						avg_pw_mem5 /= nn;
					}

					for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
						power = (fftw_out2[j][0] * fftw_out2[j][0]) + (fftw_out2[j][1] * fftw_out2[j][1]);
						power = sqrt(power);
						avg_pw_mem3 += power;
					}
					if (nn > 0) {
						avg_pw_mem3 /= nn;
					}
					pw_nx = 0;
					if (avg_pw_mem3 > 0) {
						pw_nx = avg_pw_mem5 / avg_pw_mem3;
					}
#if 0
					if (startInSample / inSampleR > 4 && startInSample / inSampleR <= 6 && param->print_stdout) {
						sprintf(sss,"pw_nx:%lf",pw_nx);
						PRINT_LOG("ABE1xx",sss);
					}
#endif
					for (j = lowIndex;j < highIndex;j++) {
						hz = (int)(((double)inSampleR / fftSize) * j);

						// 量子化ノイズ成分のパワー
						power = (fftw_out[j][0] * fftw_out[j][0]) + (fftw_out[j][1] * fftw_out[j][1]);
						power = sqrt(power);
						if (hz >= 7000 && power > 0) {
							fftw_out2[j][0] = fftw_out2[j][0] * pw_nx;
							fftw_out2[j][1] = fftw_out2[j][1] * pw_nx;
						} else {
							fftw_out2[j][0] = 0;
							fftw_out2[j][1] = 0;
						}
					}
				}

				// 半分のデータを復元
				#pragma omp parallel for
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out2[fftSize - i][0] = fftw_out2[i][0];
					fftw_out2[fftSize - i][1] = fftw_out2[i][1] * -1;
				}

				// invert FFT
				fftw_execute(fftw_ip2);

				// 出力
				pOut = (SSIZE *)mem6;
				pOut = &pOut[(fftSize / 2) * n];
				#pragma omp parallel for
				for (i = 0;i < fftSize;i++) {
					pOut[i] += fftw_in2[i][0] / fftSize;
				}
			}
			
			// 無音部分を処理対象外にする
			pIn2 = (SSIZE *)mem2;
			pOut = (SSIZE *)mem6;
			for (i = 0;i < wkMemSize;i++) {
				if (pIn2[i] == 0) pOut[i] = 0;
			}

			// 量子化ノイズをマイナスする
			pIn  = (SSIZE *)mem5;
			pOut = (SSIZE *)mem1;
			for (i = 0;i < wkMemSize;i++) {
				pOut[i] = pOut[i] - pIn[i];
			}

			if (param->p->abe_10x_random_noise == 1 && param->print_stdout) {
				PRINT_LOG("ABE10x","random_noise");
				// ランダムノイズ成分をプラスする
				pIn  = (SSIZE *)mem6;
				pOut = (SSIZE *)mem1;
				for (i = 0;i < wkMemSize;i++) {
					pOut[i] = pOut[i] + pIn[i];
				}
			}
		}
#endif
		if (param->p->post_abe == 2) {
			int abe_diff_thresh;
			// 波形の調整処理
			// NR や HFA 処理で付加されたピーク値を除去する
			for (abe_diff_thresh = 0;abe_diff_thresh < 3;abe_diff_thresh++) {
				pIn  = (SSIZE *)mem1;
				pOut = (SSIZE *)mem1;
				for (i = 1;i + 1 < nSample;) {
					next = 1;
					d1 = pIn[i - 1] >> 40;
					d2 = pIn[i] >> 40;
					d3 = pIn[i + 1] >> 40;
					if ((d1 < d2 && d2 > d3) && (d2 - ((d1 + d3) / 2)) >= PEAK_DIFF_THRESH) {
						// 飛び出てるピークを少なくする
						d1 = pIn[i - 1];
						d2 = pIn[i];
						d3 = pIn[i + 1];
						pOut[i] = (d1 + d3) / 2;
						next = 2;
					} else if ((d1 > d2 && d2 < d3) && (((d1 + d3) / 2) - d2) >= PEAK_DIFF_THRESH) {
						// 飛び出てるピークを少なくする
						d1 = pIn[i - 1];
						d2 = pIn[i];
						d3 = pIn[i + 1];
						pOut[i] = (d1 + d3) / 2;
						next = 2;
					}
					i += next;
				}
			}

			for (abe_diff_thresh = 0;abe_diff_thresh < 2;abe_diff_thresh++) {
				pIn  = (SSIZE *)mem1;
				pOut = (SSIZE *)mem1;
				for (i = 1;i + 1 < nSample;) {
					next = 1;
					d1 = pIn[i - 1] >> 40;
					d2 = pIn[i] >> 40;
					d3 = pIn[i + 1] >> 40;
					d4 = pIn[i + 2] >> 40;
					if ((d1 < d2 && d3 > d4 && d2 == d3) && (d2 - ((d1 + d4) / 2)) >= PEAK_DIFF_THRESH) {
						// 飛び出てるピークを少なくする
						d1 = pIn[i - 1];
						d2 = pIn[i];
						d3 = pIn[i + 1];
						d4 = pIn[i + 2];
						pOut[i] = (d1 + d3) / 2;
						pOut[i+1] = (d2 + d4) / 2;
						next = 3;
					} else if ((d1 > d2 && d3 < d4 && d2 == d3) && (((d1 + d4) / 2) - d2) >= PEAK_DIFF_THRESH) {
						// 飛び出てるピークを少なくする
						d1 = pIn[i - 1];
						d2 = pIn[i];
						d3 = pIn[i + 1];
						d4 = pIn[i + 2];
						pOut[i] = (d1 + d3) / 2;
						pOut[i+1] = (d2 + d3) / 2;
						next = 3;
					}
					i += next;
				}
			}
			for (abe_diff_thresh = 0;abe_diff_thresh < 3;abe_diff_thresh++) {
				pIn  = (SSIZE *)mem1;
				pOut = (SSIZE *)mem1;
				for (i = 1;i + 1 < nSample;) {
					next = 1;
					d1 = pIn[i - 1] >> 40;
					d2 = pIn[i] >> 40;
					d3 = pIn[i + 1] >> 40;
					if ((d1 < d2 && d2 > d3) && (d2 - ((d1 + d3) / 2)) >= PEAK_DIFF_THRESH) {
						// 飛び出てるピークを少なくする
						d1 = pIn[i - 1];
						d2 = pIn[i];
						d3 = pIn[i + 1];
						pOut[i] = (d1 + d3) / 2;
						next = 2;
					} else if ((d1 > d2 && d2 < d3) && (((d1 + d3) / 2) - d2) >= PEAK_DIFF_THRESH) {
						// 飛び出てるピークを少なくする
						d1 = pIn[i - 1];
						d2 = pIn[i];
						d3 = pIn[i + 1];
						pOut[i] = (d1 + d3) / 2;
						next = 2;
					}
					i += next;
				}
			}
		}

		if (param->p->post_abe != 2) {
			// 波形の調整処理
			// 上がりや、下がりの波形の量子化誤差を少なくする
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 4;i + 3 < nSample;) {
				next = 1;
				d1 = pIn[i - 4];
				d2 = pIn[i - 3];
				d3 = pIn[i - 2];
				d4 = pIn[i - 1];
				d5 = pIn[i];
				d6 = pIn[i + 1];
				d7 = pIn[i + 2];
				d8 = pIn[i + 3];
				if ((d2 < d3 && d3 <= d4 && d4 < d5 && d5 <= d6 && d6 < d7) ||
							(d2 <= d3 && d3 < d4 && d4 <= d5 && d5 < d6 && d6 <= d7)) {
					// 上がり波形
					samp[1] = pIn[i - 2] - pIn[i - 3];
					samp[2] = pIn[i - 1] - pIn[i - 2];
					samp[3] = pIn[i] - pIn[i - 1];
					samp[4] = pIn[i + 1] - pIn[i];
					samp[5] = pIn[i + 2] - pIn[i + 1];
					for (j = 1;j < 5;j++) {
						for (k = j + 1;k < 6;k++) {
							if (samp[j] > samp[k]) {
								samp[8] = samp[j];
								samp[j] = samp[k];
								samp[k] = samp[8];
							}
						}
					}
					samp[2] = samp[2] + samp[3] + samp[4];
					samp[2] /= 3;
					d1 = pIn[i];
					d2 = pIn[i - 1] + (SSIZE)samp[2];
					d1 >>= (56 - param->p->iw);
					d2 >>= (56 - param->p->iw);
					if (d1 == d2) {
						pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
					} else if (d1 < d2 && (d2 - d1) <= 3) {
						pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
					} else if (d1 > d2 && (d1 - d2) <= 3) {
						pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
					}

				} else if ((d2 > d3 && d3 >= d4 && d4 > d5 && d5 >= d6 && d6 > d7) ||
							(d2 >= d3 && d3 > d4 && d4 >= d5 && d5 > d6 && d6 >= d7)) {
					// 下がり波形
					samp[1] = pIn[i - 2] - pIn[i - 3];
					samp[2] = pIn[i - 1] - pIn[i - 2];
					samp[3] = pIn[i] - pIn[i - 1];
					samp[4] = pIn[i + 1] - pIn[i];
					samp[5] = pIn[i + 2] - pIn[i + 1];
					for (j = 1;j < 5;j++) {
						for (k = j + 1;k < 6;k++) {
							if (samp[j] > samp[k]) {
								samp[8] = samp[j];
								samp[j] = samp[k];
								samp[k] = samp[8];
							}
						}
					}
					samp[2] = samp[2] + samp[3] + samp[4];
					samp[2] /= 3;
					d1 = pIn[i];
					d2 = pIn[i - 1] + (SSIZE)samp[2];
					d1 >>= (56 - param->p->iw);
					d2 >>= (56 - param->p->iw);
					if (d1 == d2) {
						pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
					} else if (d1 < d2 && (d2 - d1) <= 3) {
						pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
					} else if (d1 > d2 && (d1 - d2) <= 3) {
						pOut[i] = pIn[i - 1] + (SSIZE)samp[2];
					}
				}
				i += next;
			}
		}
		if (param->p->post_abe != 2 && (param->p->cutLowData && fft_shresh > 0)) {
			// 波形の調整処理
			// 量子化誤差により0値になってる箇所の補正

			SSIZE level_avg;
			SSIZE cnt = inSampleR / 50;
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i < nSample;) {
				level_avg = 0;
				for (j = 0;j < cnt && (i + j) < nSample;j++) {
					if (pIn[i + j] >= 0) {
						level_avg += pIn[i + j];
					} else {
						level_avg += (pIn[i + j] * -1);
					}
				}
				if (j > 0) {
					level_avg /= j;
				}

				if (level_avg < 30) {
					// 音量が小さい区間の中だけの処理
					for (j = 0;j < cnt && (i + j) < nSample;j += zero_len) {
						zero_len = 1;
						if (pIn[i + j] == 0) {
							zero_start = i + j;
							for (k = 0;k < 35 && (i + j + k) < nSample;k++) {
								if (pIn[i + j + k] != 0) {
									break;
								}
								zero_len++;
							}
						}
						if (zero_start > 0 && zero_len > 1) {
							double ddd1,ddd2,ddd3;
							zero_start--;
							zero_len++;
							ddd1 = pIn[zero_start];
							ddd3 = pIn[zero_start + zero_len];
							for (k = 0;k < zero_len;k++) {
								pOut[zero_start + k] = (SSIZE)(ddd1 + ((ddd3 - ddd1) / zero_len) * k);
							}
						}
					}
				}
				i += cnt / 2;
			}
		}

		// 音声ファイルの最初の無音区間の保持
		if (non_zero_start != -1 && startInSample >= 0 && startInSample < non_zero_start) {
			for (i = 0;i < nSample;i++) {
				if (startInSample + i < non_zero_start) {
					mem1[i] = mem4[i];
				}
			}
		}

		// 音声ファイルの最後の無音区間の保持
		if (non_zero_end != -1 && non_zero_end < startInSample + nSample) {
			for (i = 0;i < nSample;i++) {
				if (startInSample + i > non_zero_end) {
					mem1[i] = mem4[i];
				}
			}
		}

		// 書き出し
		if (startInSample + (wltSize / 2) >= 0) {
			if (outRemain >= wltSize) {
				wr = fio_write(mem1 + (wltSize / 2),sizeof (SSIZE),wltSize,fp_w);
				if (wr != wltSize) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			} else {
				wr = fio_write(mem1 + (wltSize / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			}
			if (outRemain >= wltSize) {
				outRemain -= wltSize;
			} else {
				break;
			}
		}
	}

#if 0
	gsl_wavelet_free(wavelet);
	gsl_wavelet_workspace_free(gsl_workspace);
	free(gsl_wlt_data);
	free(gsl_wlt_abscoeff);
	free(gsl_wlt_p);
#endif

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);

	fftw_destroy_plan(fftw_p2);
	fftw_destroy_plan(fftw_ip2);
	fftw_free(fftw_in2);
	fftw_free(fftw_out2);

	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);
	al_free(mem5);
	al_free(mem6);

	PRINT_FN_LOG(param->debug_upconv,"-adjBitExtension_10x:End");
}

//---------------------------------------------------------------------------
// Function   : deClipNoise
// Description: Clipした音の調整
// ---
//	inSample 	:処理するサンプル数
//	fp			:入力ファイル
//	tmpFp		:出力ファイル
//	param		:変換パラメータ
//
void abe_deClipNoise(SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4;
	SSIZE level,level2;
	SSIZE memTotal;
	double nx;
	long wkMemSize;
	long inSampleR;
	long fftSize,i,j,k,n;
	long cnt;
	long cutOff;
	long hfc;
	long wr;
	double percent,per;
	SSIZE *pIn,*pIn2,*pOut;
	SSIZE d1,d2,d3,d4,d5,d6,d7,d8;
	SSIZE dd1,dd2,dd3,dd4;
	SSIZE startInSample,nSample;
	SSIZE outRemain;
	SSIZE max_level;
	SSIZE avg_maxper;
	SSIZE count_max;
	SSIZE count_sample;
	SSIZE data_max_level;
	double ad_per;
	int    ad_cnt;
	double samp[9];
	double power;
	double shresh;
	int next;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;
	int sm_ignore;
	SSIZE sm_avg;

	PRINT_FN_LOG(param->debug_upconv,"-abe_deClipNoise:Start");

	memTotal = 0;

	fio_rewind(fp_r);

	inSampleR = param->p->inSampleR;

	ad_per = ((double)44100 / param->p->inSampleR);

	fftSize = inSampleR * 2;
	wkMemSize = fftSize * 2;

	memTotal += wkMemSize * sizeof (SSIZE);
	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize * sizeof (SSIZE);
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize * sizeof (SSIZE);
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize * sizeof (SSIZE);
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += sizeof(fftw_complex) * wkMemSize;
	fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * wkMemSize;
	fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
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

	outRemain = inSample;
	per = -1;

	avg_maxper = 0;
	data_max_level = 0;

	for (startInSample = (((fftSize * 2) + (fftSize / 2)) * -1);startInSample < inSample + (fftSize * 1);startInSample += fftSize) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
//				Sleep(1);
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		memset(mem1,00,wkMemSize * sizeof (SSIZE));
		nSample = fftSize * 2;

		if (startInSample >= 0 && startInSample + (fftSize * 1) < inSample + fftSize) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSize * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else if (startInSample + (fftSize * 1) >= 0 || startInSample < inSample) {
			mem0 = mem1;
			nSample = 0;
			if (startInSample < 0) {
				fio_seek(fp_r,0,SEEK_SET);
				if ((startInSample * -1) < fftSize * 2) {
					mem0 += (startInSample * -1);
					nSample = (fftSize * 2) + startInSample;
				}
			} else if (startInSample < inSample) {
//				fio_seek(fp_r,startInSample,SEEK_SET);
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			}
			if (nSample > 0 && startInSample < inSample && startInSample + nSample > inSample) {
				nSample = inSample - startInSample;
			}

			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}
		
		if (param->p->abe_declip == 1) {
			// 音圧が強すぎて聞きづらい音の調整

			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			count_max  = 0;
			count_sample = 0;
			max_level = (SSIZE)1 << (56 - 1);
			max_level = (max_level - 1) / 2;

			for (i = 0;i < nSample;i++) {
				d1 = pIn[i];
				if (d1 < 0) d1 *= -1;
				count_sample++;
				if (d1 > max_level) count_max++;
				if (data_max_level < d1) data_max_level = d1;
			}
			if (i > 0) {
				if (count_max > 0 && count_max > (count_sample / 100)) {
					avg_maxper = avg_maxper + (((double)count_max / count_sample) * 100);
					avg_maxper /= 2;
				}
			}
			continue;
		}
		if (param->p->abe_declip == 0) {
			continue;
		}
		if (param->p->abe_declip == 2) {
			// De Clip のちの適応型フィルタにて削減する

			max_level = (SSIZE)1 << (56 - 1);
			max_level = (max_level - 1) / 100;
			max_level = max_level * 95;
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i < nSample;i++) {
				d1 = pIn[i];
				if (d1 > max_level) {
					pOut[i] = max_level * 1.6;
				} else if (d1 < 0 && d1 < (max_level * -1)) {
					pOut[i] = (max_level * 1.6) * -1;
				}
			}
			max_level = (SSIZE)1 << (56 - 1);
			max_level = (max_level - 1) / 100;
			max_level = max_level * 90;
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i < nSample;i++) {
				d1 = pIn[i];
				if (d1 > max_level) {
					pOut[i] = max_level * 1.6;
				} else if (d1 < 0 && d1 < (max_level * -1)) {
					pOut[i] = (max_level * 1.6) * -1;
				}
			}

			max_level = (SSIZE)1 << (56 - 1);
			max_level = (max_level - 1) / 100;
			max_level = max_level * 87;
			pIn  = (SSIZE *)mem1;
			pOut = (SSIZE *)mem1;
			for (i = 0;i < nSample;i++) {
				d1 = pIn[i];
				if (d1 > max_level) {
					pOut[i] = max_level * 1.4;
				} else if (d1 < 0 && d1 < (max_level * -1)) {
					pOut[i] = (max_level * 1.4) * -1;
				}
			}
			param->max_level2 = 0.5;
		}

		// 適応型フィルター処理
		// 同値が続く場合に同値の個数に応じたfftで高域カットフィルターをする。
		// フィルター後は波形がなめらかになるように制御する。
		memset(mem2,0,wkMemSize * sizeof (SSIZE));
		memset(mem3,0,wkMemSize * sizeof (SSIZE));
		memset(mem4,0,wkMemSize * sizeof (SSIZE));
		pIn = mem1;							// 入力波形
		pOut = mem2;						// 同値個数
		// mem2に同値が続く場合に同値の個数を記録していく。
		if (param->p->abe_declip == 0) {
			for (i = 0,j = 1,cnt = 1;j < nSample;j++) {
				d1 = pIn[i] >> (56 - param->p->iw);
				d2 = pIn[j] >> (56 - param->p->iw);
				if (d1 == d2 && cnt < 255) {
					cnt++;
				} else {
					for (k = i;k < j;k++) {
						pOut[k] = cnt;
					}
					i = j;
					cnt = 1;
				}
			}
		} else {
			for (i = 0,j = 1,cnt = 1;j < nSample;j++) {
				d1 = pIn[i] >> (56 - param->p->iw);
				d2 = pIn[j] >> (56 - param->p->iw);
				if (d1 != 0 && d1 == d2 && cnt < 2048 * 4 && ((pIn[j] > 0 && pIn[j] >= max_level) || (pIn[j] < 0 && pIn[j] <= max_level * -1))) {
					cnt++;
				} else {
					for (k = i;k < j;k++) {
						pOut[k] = cnt;
					}

					i = j;
					cnt = 1;
				}
			}
			if (cnt >= 3) {
				
			}
		}
		for (i = 0;i < nSample;i++) {
			if (pOut[i] >= 0 && pOut[i] < 2) {
				pOut[i] = 0;
			}
		}
		// 同値が3つ以上続くものに対して、fft をかける
		do {
			pIn = mem1;
			pIn2 = mem2;
			memset(mem3,0,wkMemSize * sizeof (SSIZE));
			cnt = 0;
			for (i = 0;i < nSample;i++) {
				if (pIn2[i] > 0) {
					cnt = pIn2[i];
					break;
				}
			}
			if (cnt == 0) {
				break;
			}
			for (n = 0;n < 3;n++) {
				// FFT 初期設定
				copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);
				
				// 窓関数
				windowFFTW(fftw_in,fftSize);

				// FFT
				fftw_execute(fftw_p);

				if (cnt > 0) {
					// 高域削除
					if (((double)cnt * ad_per) > 0) {
						hfc = (SSIZE)(22050.0 / ((double)cnt * ad_per));
						hfc = (SSIZE)((double)hfc * 1.1);
					} else {
						hfc = 0;
					}
					cutOff = ((double)fftSize / inSampleR) * hfc;
					if (cutOff > 0 && hfc >= 100) {
						for (i = cutOff;i < fftSize;i++) {
							fftw_out[i][0] = 0;
							fftw_out[i][1] = 0;
						}
					}
				}
				// 半分のデータを復元
				//#pragma omp parallel for
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[fftSize - i][0] = fftw_out[i][0];
					fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
				}

				// invert FFT
				fftw_execute(fftw_ip);

				// 出力
				pOut = (SSIZE *)&mem3[((fftSize / 2) * n)];
				//#pragma omp parallel for
				for (i = 0;i < fftSize;i++) {
					pOut[i] += fftw_in[i][0] / fftSize;
				}
			}
			pIn   = (SSIZE *)mem3;
			pIn2  = (SSIZE *)mem2;
			pOut  = (SSIZE *)mem4;
			for (i = 0;i < nSample;i++) {
				if (pIn2[i] > 0 && pIn2[i] == cnt) {
					pOut[i] = pIn[i];
					pIn2[i] *= -1;
				}
			}
		} while (1);
		pIn = (SSIZE *)mem4;
		pIn2 = (SSIZE *)mem2;
		pOut = (SSIZE *)mem1;
		if (param->p->abe_declip == 0) {
			for (i = 0;i < nSample;i++) {
				d1 = pIn[i];
				d1 >>= (56 - param->p->iw);
				d2 = pOut[i];
				d2 >>= (56 - param->p->iw);
				if (d1 <= d2 && (d2 - d1) <= 3) {
					pOut[i] = pIn[i];
				} else if (d1 > d2 && (d1 - d2) <= 3) {
					pOut[i] = pIn[i];
				}
			}
		} else {
			int init_cnt;
			for (i = 0,init_cnt = 0;i < nSample;i++) {
				if (init_cnt > 0 && pIn2[i] != init_cnt * -1) init_cnt = pIn2[i] * -1;
				if ((pOut[i] > 0 && pIn[i] > 0)) {
					if (i > 0 && init_cnt == 0) {
						init_cnt = pIn2[i] * -1;
						d1 = pIn[i];
						d2 = pIn[i + 1];
						d3 = pOut[i-1];
						d1 = (d3 - d1) + (d2 - d1);
					}
					d1 = 0;
					pOut[i] = pIn[i] + d1;
				}
				if (pOut[i] < 0 && pIn[i] < 0) {
					if (i > 0 && init_cnt == 0) {
						init_cnt = pIn2[i] * -1;
						d1 = pIn[i];
						d2 = pIn[i + 1];
						d3 = pOut[i-1];
						d1 = (d1 - d3) + (d1 - d2);
					}
					d1 = 0;
					pOut[i] = pIn[i] - d1;
				}
			}
		}
#if 0
		if (param->p->abe_declip == 2) {
			pIn = (SSIZE *)mem1;
			pIn2 = (SSIZE *)mem2;
			pOut = (SSIZE *)mem1;
			for (i = 1;i < nSample - 1;i++) {
				d1 = pIn[i - 1];
				d2 = pIn[i - 0];
				d3 = pIn[i + 1];
				if (d2 > 0 && d1 < d2 && d2 > d3 && (d2 - ((d1 + d3) / 2) > max_level / 4)) {
					if (pIn2[i] < 0) pOut[i] = d1 + (d2 / 12);
				} else if (d2 < 0 && d1 > d2 && d2 < d3 && (((d1 + d3) / 2) - d1) < max_level / 4) {
					if (pIn2[i] < 0) pOut[i] = d1 - (d2 / 12);
				}
			}
			for (i = 2;i < nSample - 2;i++) {
				d1 = pIn[i - 2];
				d2 = pIn[i - 1];
				d3 = pIn[i - 0];
				d4 = pIn[i + 1];
				d5 = pIn[i + 1];
				if (d3 > 0 && d1 < d2 && d2 < d3 && d3 - ((d1 + d5) / 2) > max_level / 4) {
					if (pIn2[i] < 0) pOut[i] = (((d1 + d2) / 2) + ((d4 + d5) / 2)) / 2;
					if (pIn2[i-1] < 0) pOut[i-1] = ((d1 + d3) / 2);
					if (pIn2[i+1] < 0) pOut[i+1] = ((d5 + d3) / 2);
				} else if (d3 < 0 && d1 > d2 && d2 > d3 && ((d1 + d5) - d3) > max_level / 4) {
					if (pIn2[i] < 0) pOut[i] = (((d1 + d2) / 2) + ((d4 + d5) / 2)) / 2;
					if (pIn2[i-1] < 0) pOut[i-1] = ((d1 + d3) / 2);
					if (pIn2[i+1] < 0) pOut[i+1] = ((d5 + d3) / 2);
				}
			}
		}
#endif
		if (startInSample + (fftSize / 2) >= 0) {
			if (outRemain >= fftSize) {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),fftSize,fp_w);
				if (wr != fftSize) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			} else {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			}
			if (outRemain >= fftSize) {
				outRemain -= fftSize;
			} else {
				break;
			}
		}
	}

	if (param->p->abe_declip == 1 && avg_maxper >= 7) {
		param->p->abe_declip = 2;
	} else if (param->p->abe_declip == 1) {
		param->p->abe_declip = 0;
	}
	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);

	if (param->maxMemorySize < memTotal) {
		param->maxMemorySize = memTotal;
	}

	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);

	PRINT_FN_LOG(param->debug_upconv,"-abe_deClipNoise:End");
}

//---------------------------------------------------------------------------
// Function   : abe_deClickNoise
// Description: クリックノイズリダクション
// ---
//	inSample 	:処理するサンプル数
//	fp			:入力ファイル
//	tmpFp		:出力ファイル
//	param		:変換パラメータ
//
void abe_deClickNoise(SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1;
	SSIZE level,level2;
	double nx;
	long wkMemSize;
	long inSampleR;
	long fftSize,i,j,k,n;
	long cnt;
	long cutOff;
	long hfc;
	long wr;
	int sm_ignore;
	double percent,per;
	SSIZE *pIn,*pIn2,*pOut;
	SSIZE d1,d2,d3,d4,d5,d6,d7,d8;
	SSIZE dd1,dd2,dd3,dd4;
	SSIZE startInSample,nSample;
	SSIZE outRemain;
	SSIZE max_level;
	SSIZE avg_maxper;
	SSIZE count_max;
	SSIZE count_sample;
	SSIZE data_max_level;
	double ad_per;
	int    ad_cnt;
	double samp[9];
	double power;
	double shresh;
	int next;
	fio_rewind(fp_r);

	PRINT_FN_LOG(param->debug_upconv,"-abe_deClickNoise:Start");

	inSampleR = param->p->inSampleR;

	fftSize = inSampleR * 2;
	wkMemSize = fftSize * 2;

	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	outRemain = inSample;
	per = -1;

	for (startInSample = (((fftSize * 2) + (fftSize / 2)) * -1);startInSample < inSample + (fftSize * 1);startInSample += fftSize) {
		if (startInSample >= 0 && startInSample  < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
//				Sleep(1);
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		memset(mem1,00,wkMemSize * sizeof (SSIZE));
		nSample = fftSize * 2;

		if (startInSample >= 0 && startInSample + (fftSize * 1) < inSample + fftSize) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = fftSize * 2;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else if (startInSample + (fftSize * 1) >= 0 || startInSample < inSample) {
			mem0 = mem1;
			nSample = 0;
			if (startInSample < 0) {
				fio_seek(fp_r,0,SEEK_SET);
				if ((startInSample * -1) < fftSize * 2) {
					mem0 += (startInSample * -1);
					nSample = (fftSize * 2) + startInSample;
				}
			} else if (startInSample < inSample) {
//				fio_seek(fp_r,startInSample,SEEK_SET);
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			}
			if (nSample > 0 && startInSample < inSample && startInSample + nSample > inSample) {
				nSample = inSample - startInSample;
			}

			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}
		pIn  = (SSIZE *)mem1;
		pOut = (SSIZE *)mem1;
		for (i = 1;i + 1 < nSample;i++) {
			d1 = pIn[i - 1];
			d2 = pIn[i + 0];
			d3 = pIn[i + 1];
			dd1 = d1 >> (56 - 16);
			dd2 = d2 >> (56 - 16);
			dd3 = d3 >> (56 - 16);
			sm_ignore = 1;
			if (dd1 < dd2 && dd2 > dd1) {
				if (dd2 - ((dd1 + dd3) / 2) >= 1800) {
					sm_ignore = 0;
				}
			}
			if (dd1 > dd2 && dd2 < dd1) {
				if (((dd1 + dd3) / 2) - dd2 >= 1800) {
					sm_ignore = 0;
				}
			}
			sm_ignore = 0;
			if (sm_ignore == 0) {
				d2 = (d1 + d3 + d1 + d3 + d1 + d3 + d2) / 7;
				pOut[i + 0] = d2;
				i++;
			}
		}

		if (startInSample + (fftSize / 2) >= 0) {
			if (outRemain >= fftSize) {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),fftSize,fp_w);
				if (wr != fftSize) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			} else {
				wr = fio_write(mem1 + (fftSize / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					return;
				}
			}
			if (outRemain >= fftSize) {
				outRemain -= fftSize;
			} else {
				break;
			}
		}
	}
	al_free(mem1);
	PRINT_FN_LOG(param->debug_upconv,"-abe_deClickNoise:End");
}

//---------------------------------------------------------------------------
// Function   : genNoise
// Description: 失われた高域の再現処理(正規分布のノイズ付加)
// ---
//	hfc		 	:高域のカットオフ周波数(この周波数以上の領域にデータを追加する)
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r		:入力ファイル
//	fp_w		:出力ファイル
//	param		:変換パラメータ
//
void genNoise(long hfc,SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1,*mem2,*mem3;
	SSIZE memTotal;
	long outSampleR;
	long wkMemSize;
	long fftSize,i,j,n,nn;
	
	long lowIndex,highIndex;
	long nrIndex;
	double percent,per;
	double nx;
	double p,refPw,noisePw;
	SSIZE *pIn,*pIn2,*pOut;
	SSIZE startInSample,nSample;
	
	fftw_complex *fftw_in[2],*fftw_out[2],*fftw_in2;
	fftw_plan fftw_p[2],fftw_ip[2],fftw_p2;
	double hfaNB;

	PRINT_FN_LOG(param->debug_upconv,"-genNoise:Start");

	memTotal = 0;
	outSampleR = param->p->outSampleR;
	fio_rewind(fp_r);

	fftSize = param->p->outSampleR * 2;

	wkMemSize = (fftSize * 2) * sizeof (SSIZE);
	memTotal += wkMemSize;
	mem1 = (SSIZE *)al_malloc(wkMemSize);
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize;
	mem2 = (SSIZE *)al_malloc(wkMemSize);
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize;
	mem3 = (SSIZE *)al_malloc(wkMemSize);
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += sizeof(fftw_complex) * fftSize;
	fftw_in[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_in[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * fftSize;
	fftw_in[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_in[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * fftSize;
	fftw_out[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_out[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * fftSize;
	fftw_out[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_out[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * fftSize / 100;
	fftw_in2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize / 100);
	if (fftw_in2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_p[0] = fftw_plan_dft_1d(fftSize,fftw_in[0],fftw_out[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p[1] = fftw_plan_dft_1d(fftSize,fftw_in[1],fftw_out[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip[0] = fftw_plan_dft_1d(fftSize,fftw_out[0],fftw_in[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip[1] = fftw_plan_dft_1d(fftSize,fftw_out[1],fftw_in[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p2 = fftw_plan_dft_1d(fftSize / 100,fftw_in2,fftw_in2,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	per = -1;
	nrIndex = 0;
	for (startInSample = ((fftSize + (fftSize / 2)) * -1);startInSample < inSample + (fftSize + (fftSize / 2));startInSample += fftSize) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				if (param->print_stdout == 1) {
					fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
					fflush(stdout);
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		memset(mem1,0,wkMemSize);
		memset(mem3,0,wkMemSize);

		// mem2 には正規分布のノイズを格納する
		pIn2 = (SSIZE *)mem2;
		#pragma omp parallel for
		for (i = 0;i < fftSize * 2;i++) {
			pIn2[i] = normalNoise() * 0x100;
		}

		if (startInSample >= 0 && startInSample + (fftSize * 2) < inSample) {
			nSample = fftSize * 2;
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else {
			mem0 = mem1;
			nSample = fftSize * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r,0,SEEK_SET);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}

		pIn = (SSIZE *)mem1;
		for (n = 0;n < 3;n++) {
			lowIndex = ((double)fftSize / outSampleR) * (hfc - 2000);
			highIndex = ((double)fftSize / outSampleR) * hfc;
			// FFT 初期設定
			copyToFFTW(fftw_in[0],&pIn[((fftSize / 2) * n)],fftSize);
			windowFFTW(fftw_in[0],fftSize);

			// FFT
			fftw_execute(fftw_p[0]);

			// 元信号の高域のパワーを調べる
			refPw = 0;
			for (i = lowIndex,nn = 0;i < highIndex;i++,nn++) {
				p = fftw_out[0][i][0] * fftw_out[0][i][0] + fftw_out[0][i][1] * fftw_out[0][i][1];
				if (p != 0) {
					p = sqrt(p);
				}
				refPw += p;
			}
			if (nn > 0) {
				refPw /= nn;
			}

			// 付加する信号
			copyToFFTW(fftw_in[1],&pIn2[((fftSize / 2) * n)],fftSize);
			windowFFTW(fftw_in[1],fftSize);

			// FFT
			fftw_execute(fftw_p[1]);
			// 1/f 信号にする
			adjPinkFilter(1,fftSize,fftw_out[1],param);

			// 付加する信号のパワーを調べる
			noisePw = 0;
			lowIndex = ((double)fftSize / outSampleR) * hfc;
			highIndex = ((double)fftSize / outSampleR) * (hfc + 2000);
			for (i = lowIndex,nn = 0;i < highIndex;i++,nn++) {
				p = fftw_out[1][i][0] * fftw_out[1][i][0] + fftw_out[1][i][1] * fftw_out[1][i][1];
				if (p != 0) {
					p = sqrt(p);
				}
				noisePw += p;
			}
			if (nn > 0) {
				noisePw /= nn;
			}
			if (noisePw == 0) {
				noisePw = 1;
			}
			nx = refPw / noisePw;
			highIndex = ((double)fftSize / outSampleR) * hfc;

			#pragma omp parallel for
			for (i = highIndex;i < fftSize / 2;i++) {
				fftw_out[1][i][0] *= nx;
				fftw_out[1][i][1] *= nx;
			}

			if (param->p->hfa != 1 && param->p->hfaNB > 0) {
				hfaNB = param->p->hfaNB / 100.0;
				// hfa1 信号
				for (i = highIndex;i < fftSize / 2;i++) {
					fftw_out[1][i][0] *= hfaNB;
					fftw_out[1][i][1] *= hfaNB;
				}
				hfaNB = 1.0 - hfaNB;
				// hfa2 信号
				for (i = highIndex;i < fftSize / 2;i++) {
					fftw_out[0][i][0] *= hfaNB;
					fftw_out[0][i][1] *= hfaNB;
				}
				// 合成
				for (i = highIndex;i < fftSize / 2;i++) {
					fftw_out[1][i][0] += fftw_out[0][i][0];
					fftw_out[1][i][1] += fftw_out[0][i][1];
				}
			}

			// 低域カット
			highIndex = ((double)fftSize / outSampleR) * hfc;

			#pragma omp parallel for
			for (i = 1;i < highIndex;i++) {
				fftw_out[1][i][0] = 0;
				fftw_out[1][i][1] = 0;
			}

			// 半分のデータを復元
			#pragma omp parallel for
			for (i = 1;i < fftSize / 2;i++) {
				fftw_out[1][fftSize - i][0] = fftw_out[1][i][0];
				fftw_out[1][fftSize - i][1] = fftw_out[1][i][1] * -1;
			}
			// 直流部分除去
			fftw_out[1][0][0] = 0;
			fftw_out[1][0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip[1]);

			pOut = (SSIZE *)mem3;
			pOut = &pOut[(fftSize / 2) * n];
			#pragma omp parallel for
			for (i = 0;i < fftSize;i++) {
				pOut[i] += fftw_in[1][i][0] / fftSize;
			}
		}
		if (startInSample + fftSize / 2 >= 0) {
			if (param->p->hfa == 1) {
				pIn = (SSIZE *)mem1;
			} else {
				pIn = (SSIZE *)mem3;
			}
			pIn = pIn + (fftSize / 2);
			pOut = (SSIZE *)mem3;
			pOut = pOut + (fftSize / 2);
			for (i = 0;i < 100;i++) {
				// 元信号に音があるかを調べる
				for (j = 0;j < fftSize / 100;j++) {
					fftw_in2[j][0] = pIn[j];
					fftw_in2[j][1] = 0;
				}
				// 窓関数
				for (j = 0;j < ((fftSize / 100) - 1) / 2;j++) {
					fftw_in2[j][0] = fftw_in2[j][0] * (2.0 * j / ((double)fftSize / 100));
				}
				for (j = ((fftSize / 100) - 1) / 2;j < (fftSize / 100);j++) {
					fftw_in2[j][0] = fftw_in2[j][0] * (2.0 - 2.0 * j / ((double)fftSize / 100));
				}

				// FFT
				fftw_execute(fftw_p2);

				// 元信号の高域のパワーを調べる
				refPw = 0;
				lowIndex = (((double)fftSize / 100) / outSampleR) * (hfc - 2000);
				highIndex = (((double)fftSize / 100) / outSampleR) * hfc;
				for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
					p = fftw_in2[j][0] * fftw_in2[j][0] + fftw_in2[j][1] * fftw_in2[j][1];
					if (p != 0) {
						p = sqrt(p);
					}
					refPw += p;
				}
				if (nn > 0) {
					refPw /= nn;
				}
				// 付加信号の高域のパワーを調べる
				for (j = 0;j < fftSize / 100;j++) {
					fftw_in2[j][0] = pOut[j];
					fftw_in2[j][1] = 0;
				}
				// 窓関数
				for (j = 0;j < ((fftSize / 100) - 1) / 2;j++) {
					fftw_in2[j][0] = fftw_in2[j][0] * (2.0 * j / ((double)fftSize / 100));
				}
				for (j = ((fftSize / 100) - 1) / 2;j < (fftSize / 100);j++) {
					fftw_in2[j][0] = fftw_in2[j][0] * (2.0 - 2.0 * j / ((double)fftSize / 100));
				}

				// FFT
				fftw_execute(fftw_p2);

				noisePw = 0;
				lowIndex = (((double)fftSize / 100) / outSampleR) * hfc;
				highIndex = (((double)fftSize / 100) / outSampleR) * (hfc + 2000);
				for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
					p = fftw_in2[j][0] * fftw_in2[j][0] + fftw_in2[j][1] * fftw_in2[j][1];
					if (p != 0) {
						p = sqrt(p);
					}
					noisePw += p;
				}
				if (nn > 0) {
					noisePw /= nn;
				}
				if (refPw > 0) {
					if (param->p->hfa == 1) {
						for (j = 0;j < fftSize / 100;j++) {
							pOut[j] *= ((refPw / noisePw) * 0.70);
						}
					} else {
						for (j = 0;j < fftSize / 100;j++) {
							pOut[j] *= (refPw / noisePw);
						}
					}
				} else {
					for (j = 0;j < fftSize / 100;j++) {
						pOut[j] = 0;
					}
				}
				pIn  += fftSize / 100;
				pOut += fftSize / 100;
			}
			// 再び低域カット処理をする
			pIn = (SSIZE *)mem1;
			pOut = (SSIZE *)mem3;
			for (n = 0;n < 3;n++) {
				// FFT 初期設定
				for (i = 0;i < fftSize;i++) {
					fftw_in[1][i][0] = pOut[((fftSize / 2) * n) + i];
				   	fftw_in[1][i][1] = 0;
				}
				// 窓関数
				for (i = 0;i < (fftSize - 1) / 2;i++) {
					fftw_in[1][i][0] = fftw_in[1][i][0] * (2.0 * i / (double)fftSize);
				}
				for (i = (fftSize - 1) / 2;i < fftSize;i++) {
					fftw_in[1][i][0] = fftw_in[1][i][0] * (2.0 - 2.0 * i / (double)fftSize);
				}

				// FFT
				fftw_execute(fftw_p[1]);

				highIndex = ((double)fftSize / outSampleR) * (hfc - 1000);

				// 低域カット
				for (i = 1;i < highIndex;i++) {
					fftw_out[1][i][0] = 0;
					fftw_out[1][i][1] = 0;
				}

				// 半分のデータを復元
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[1][fftSize - i][0] = fftw_out[1][i][0];
					fftw_out[1][fftSize - i][1] = fftw_out[1][i][1] * -1;
				}

				// 直流部分除去
				fftw_out[1][0][0] = 0;
				fftw_out[1][0][1] = 0;

				// invert FFT
				fftw_execute(fftw_ip[1]);

				// 出力
				for (i = 0;i < fftSize;i++) {
					pOut[((fftSize / 2) * n) + i] += fftw_in[1][i][0] / fftSize;
				}
			}
			for (i = 0;i < fftSize;i++) {
				pOut[(fftSize / 2) + i] += pIn[(fftSize / 2) + i];
			}
			fio_seek(fp_w,(startInSample + (fftSize / 2)) * sizeof (SSIZE),SEEK_SET);
			outTempFile(fp_w,mem3 + fftSize / 2,fftSize,param);
			if (param->err) {
				break;
			}
		}
	}

	fio_flush(fp_w);

	if (param->maxMemorySize < memTotal) {
		param->maxMemorySize = memTotal;
	}

	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	fftw_destroy_plan(fftw_p[0]);
	fftw_destroy_plan(fftw_p[1]);
	fftw_destroy_plan(fftw_ip[0]);
	fftw_destroy_plan(fftw_ip[1]);
	fftw_destroy_plan(fftw_p2);
	fftw_free(fftw_in[0]);
	fftw_free(fftw_in[1]);
	fftw_free(fftw_out[0]);
	fftw_free(fftw_out[1]);
	fftw_free(fftw_in2);

	PRINT_FN_LOG(param->debug_upconv,"-genNoise:End");

}
//---------------------------------------------------------------------------
// Function   : genOverTone
// Description: 失われた高域の再現処理(倍音解析)
// ---
//	hfc		 	:高域のカットオフ周波数(この周波数以上の領域にデータを追加する)
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r		:入力ファイル
//	fp_w		:出力ファイル
//	param		:変換パラメータ
//
void genOverTone(long hfc,SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param,FFT_PARAM *fft_param)
{
	long outSampleR;
	SSIZE *mem0,*mem1,*mem2,*mem3,*mem4;
	SSIZE *mem5,*mem6;
	SSIZE *mem7,*mem8,*mem9,*mem10;
	
	SSIZE memTotal;
	long wkMemSize;
	long fftSize,fftFastSize,fftSize_LevelAdj,i,ii,j,k,nn;
	long stepFftSize;
	long LevelAdjCount;
	long LevelAdjHz;
	long lowIndex,highIndex;
	long deadenIndex;
	long fft_malloc_size;
	long sfa_sec;
	long fast_index,fast_index_count;
	long fast_diff_pw_flag;
	double percent,per;
	double nx;
	double p,refPw,ovTonePw,refLowHzPw;
	double spike_diff,spike_work,spike[10];
	double avg;
	double hfa_nx;
	SSIZE *pIn[9],*pOut[9];
	SSIZE startInSample,nSample;
	SSIZE s[2];
	SSIZE p1,p2,p3;
	SSIZE hh,mm,ss;
	fftw_complex *fftw_in[9],*fftw_out[9],*fftw_in2,*fftw_in3;
	fftw_plan fftw_p[9],fftw_ip[9],fftw_p2,fftw_p3;

	// 時間分解能を高めるためのFFTW用変数(1/3サイズ)
	fftw_complex *fftw_fast_in[9],*fftw_fast_out[9];
	fftw_plan fftw_fast_p[9],fftw_fast_ip[9];

	// カットオフ周波数付近のpowerを保存しておく変数(配列0番前回、配列1番以降から4つは今回の比較用)
	double *pw_tres[5];

	OVERTONE_INFO *ovInfo[9];
	double *phaseX,*phaseY;
	double table_hfa_level[5] = {0.5, 0.1, 0.05, 0.03, 0.01};
	char sss[200];

	memTotal = 0;
	ovInfo[0] = NULL;
	ovInfo[1] = NULL;
	ovInfo[2] = NULL;
	ovInfo[3] = NULL;
	ovInfo[4] = NULL;
	ovInfo[5] = NULL;
	ovInfo[6] = NULL;
	ovInfo[7] = NULL;
	ovInfo[8] = NULL;
	pw_tres[0] = NULL;
	pw_tres[1] = NULL;
	pw_tres[2] = NULL;
	pw_tres[3] = NULL;
	pw_tres[4] = NULL;

	PRINT_FN_LOG(param->debug_upconv,"-genOverTone:Start");

	outSampleR = param->p->outSampleR;
	fio_rewind(fp_r);
	fio_rewind(fp_w);
param->p->hfa3_max = 0;
	if (param->p->hfa3_version == 1) {
		fftSize = 4096;
		hfa_nx = (double)param->p->hfa_level_adjust_nx / 100.0;
		sprintf(sss,"hfa3_version:%d,hfa_nx:%lf",param->p->hfa3_version,hfa_nx);
		PRINT_LOG("",sss);
	} else if (param->p->hfa3_version == 2) {
		fftSize = outSampleR / 14;
		hfa_nx = (double)param->p->hfa_level_adjust_nx / 100.0;
		sprintf(sss,"hfa3_version:%d,hfa_nx:%lf",param->p->hfa3_version,hfa_nx);
		PRINT_LOG("",sss);
	} else {
		fftSize = outSampleR / 14;
		hfa_nx = (double)param->p->hfa_level_adjust_nx / 100.0;
		sprintf(sss,"hfa3_version:%d,hfa_nx:%lf",param->p->hfa3_version,hfa_nx);
		PRINT_LOG("",sss);
	}
	
	// 88.2kHz or 96kHz
	if (outSampleR == 44100 * 2 || outSampleR == 48000 * 2) {
		fftSize = outSampleR / 10;
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 16;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 4096 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 14;
		} else {
			fftSize = outSampleR / 14;
		}
	}

	// 176kHz or 192kHz
	if (outSampleR == 32000 * 6 || outSampleR == 44100 * 4 || outSampleR == 48000 * 4) {
		fftSize = outSampleR / 10;
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 17;
			fftSize = outSampleR / 12;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 8192 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 14;
		} else {
			fftSize = outSampleR / 14;
		}
	}

	// 352kHz or 384kHz
	if (outSampleR == 32000 * 12 || outSampleR == 44100 * 8 || outSampleR == 48000 * 8) {
		fftSize = outSampleR / 10;
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 18;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 16384 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 16;
		} else {
			fftSize = outSampleR / 16;
			fftSize = outSampleR / 32;
		}
	}

	// 704kHz or 768kHz
	if (outSampleR == 32000 * 24 || outSampleR == 44100 * 16 || outSampleR == 48000 * 16) {
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 14;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 32768 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 16;
		} else {
			fftSize = outSampleR / 16;
		}
	}

	// 1408kHz or 1536kHz
	if (outSampleR == 32000 * 48 || outSampleR == 44100 * 32 || outSampleR == 48000 * 32) {
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 14;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 65536 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 18;
		} else {
			fftSize = outSampleR / 18;
		}
	}

	// 2822kHz or 3072kHz
	if (outSampleR == 32000 * 64 || outSampleR == 44100 * 64 || outSampleR == 48000 * 64) {
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 14;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 65536 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 20;
		} else {
			fftSize = outSampleR / 20;
		}
	}

	if (param->p->hfa3_fft_window_size != 0) {
		fftSize = outSampleR / param->p->hfa3_fft_window_size;
	}
	sprintf(sss,"GenOverTone:fftSize:%ld",fftSize);
	PRINT_LOG("HFA4",sss);

	fftSize_LevelAdj = fftSize / 6;
	LevelAdjCount = 6;
	LevelAdjHz = 2200;

	if (param->p->hfa3_version >= 3) {
		if (param->p->hfa3_fft_window_lvl != 0) {
			fftSize_LevelAdj = fftSize / param->p->hfa3_fft_window_lvl;
			LevelAdjCount = param->p->hfa3_fft_window_lvl;
		}
	}
	fftFastSize = fftSize / 3;	// 現時点では未使用(無効化)

	wkMemSize = (fftSize + (fftSize * 2)) * sizeof (SSIZE);

	memTotal += wkMemSize;
	mem1 = (SSIZE *)al_malloc(wkMemSize);
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize;
	mem2 = (SSIZE *)al_malloc(wkMemSize);
	if (mem2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize;
	mem3 = (SSIZE *)al_malloc(wkMemSize);
	if (mem3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += wkMemSize;
	mem4 = (SSIZE *)al_malloc(wkMemSize);
	if (mem4 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

//	sprintf(sss,"GenOverTone:hfa3_fft_window_per_1s:%d",param->p->hfa3_fft_window_per_1s);
//	PRINT_LOG("HFA4",sss);

	if (param->p->hfa3_fft_window_per_1s > 0) {
		memTotal += wkMemSize;
		mem5 = (SSIZE *)al_malloc(wkMemSize);
		if (mem5 == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		memTotal += wkMemSize;
		mem6 = (SSIZE *)al_malloc(wkMemSize);
		if (mem6 == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}

		if (param->p->hfa3_fft_window_per_1s > 1) {
			memTotal += wkMemSize;
			mem7 = (SSIZE *)al_malloc(wkMemSize);
			if (mem7 == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += wkMemSize;
			mem8 = (SSIZE *)al_malloc(wkMemSize);
			if (mem8 == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}

			memTotal += wkMemSize;
			mem9 = (SSIZE *)al_malloc(wkMemSize);
			if (mem9 == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += wkMemSize;
			mem10 = (SSIZE *)al_malloc(wkMemSize);
			if (mem10 == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
		}
	}

	memTotal += 76800 * 4 * sizeof (double);
	phaseX = (double *)al_malloc(76800 * 4 * sizeof (double));
	if (phaseX == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += 76800 * 4 * sizeof (double);
	phaseY = (double *)al_malloc(76800 * 4 * sizeof (double));
	if (phaseY == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += sizeof (OVERTONE_INFO);
	ovInfo[0] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
	if (ovInfo[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof (OVERTONE_INFO);
	ovInfo[1] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
	if (ovInfo[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof (OVERTONE_INFO);
	ovInfo[2] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
	if (ovInfo[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	if (param->p->hfa3_fft_window_per_1s > 0) {
		memTotal += sizeof (OVERTONE_INFO);
		ovInfo[3] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
		if (ovInfo[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		memTotal += sizeof (OVERTONE_INFO);
		ovInfo[4] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
		if (ovInfo[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		if (param->p->hfa3_fft_window_per_1s > 1) {
			memTotal += sizeof (OVERTONE_INFO);
			ovInfo[5] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
			if (ovInfo[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof (OVERTONE_INFO);
			ovInfo[6] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
			if (ovInfo[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof (OVERTONE_INFO);
			ovInfo[7] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
			if (ovInfo[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof (OVERTONE_INFO);
			ovInfo[8] = (OVERTONE_INFO *)malloc(sizeof (OVERTONE_INFO));
			if (ovInfo[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
		}
	}

	memset(ovInfo[0],0,sizeof (OVERTONE_INFO));
	memset(ovInfo[1],0,sizeof (OVERTONE_INFO));
	memset(ovInfo[2],0,sizeof (OVERTONE_INFO));
	if (param->p->hfa3_fft_window_per_1s > 0) {
		memset(ovInfo[3],0,sizeof (OVERTONE_INFO));
		memset(ovInfo[4],0,sizeof (OVERTONE_INFO));
		if (param->p->hfa3_fft_window_per_1s > 1) {
			memset(ovInfo[5],0,sizeof (OVERTONE_INFO));
			memset(ovInfo[6],0,sizeof (OVERTONE_INFO));
			memset(ovInfo[7],0,sizeof (OVERTONE_INFO));
			memset(ovInfo[8],0,sizeof (OVERTONE_INFO));
		}
	}
	// 1
	fft_malloc_size = 76800 * 4;
	if (fft_malloc_size < fftSize) fft_malloc_size = fftSize;
	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_in[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_in[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_out[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_out[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	// 2
	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_in[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_in[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_out[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_out[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	// 3
	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_in[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_in[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_out[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_out[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	// 1(fast)
	memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
	fftw_fast_in[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
	if (fftw_fast_in[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
	fftw_fast_out[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
	if (fftw_fast_out[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	// 2(fast)
	memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
	fftw_fast_in[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
	if (fftw_fast_in[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
	fftw_fast_out[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
	if (fftw_fast_out[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	// 3(fast)
	memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
	fftw_fast_in[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
	if (fftw_fast_in[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	memTotal += sizeof(fftw_complex) * (fft_malloc_size / 4);
	fftw_fast_out[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 4));
	if (fftw_fast_out[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	if (param->p->hfa3_fft_window_per_1s > 0) {
		// 4
		memTotal += sizeof(fftw_complex) * fft_malloc_size;
		fftw_in[3] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
		if (fftw_in[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		memTotal += sizeof(fftw_complex) * fft_malloc_size;
		fftw_out[3] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
		if (fftw_out[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		// 5
		memTotal += sizeof(fftw_complex) * fft_malloc_size;
		fftw_in[4] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
		if (fftw_in[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		memTotal += sizeof(fftw_complex) * fft_malloc_size;
		fftw_out[4] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
		if (fftw_out[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		// 4(fast)
		memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
		fftw_fast_in[3] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
		if (fftw_fast_in[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
		fftw_fast_out[3] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
		if (fftw_fast_out[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		// 5(fast)
		memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
		fftw_fast_in[4] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
		if (fftw_fast_in[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
		fftw_fast_out[4] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
		if (fftw_fast_out[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}

		if (param->p->hfa3_fft_window_per_1s > 1) {
			// 6
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_in[5] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_in[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_out[5] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_out[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 7
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_in[6] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_in[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_out[6] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_out[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 8
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_in[7] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_in[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_out[7] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_out[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 9
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_in[8] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_in[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * fft_malloc_size;
			fftw_out[8] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
			if (fftw_out[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}

			// 6(fast)
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_in[5] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_in[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_out[5] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_out[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 7(fast)
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_in[6] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_in[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_out[6] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_out[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 8(fast)
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_in[7] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_in[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_out[7] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_out[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 9(fast)
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_in[8] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_in[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			memTotal += sizeof(fftw_complex) * (fft_malloc_size / 3);
			fftw_fast_out[8] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fft_malloc_size / 3));
			if (fftw_fast_out[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
		}
	}

	lowIndex  = (((double)fftFastSize) / outSampleR) * (hfc - 3000);
	highIndex = (((double)fftFastSize) / outSampleR) * hfc;
	pw_tres[0] = (double *)malloc((highIndex - lowIndex) * sizeof (double));
	pw_tres[1] = (double *)malloc((highIndex - lowIndex) * sizeof (double));
	pw_tres[2] = (double *)malloc((highIndex - lowIndex) * sizeof (double));
	pw_tres[3] = (double *)malloc((highIndex - lowIndex) * sizeof (double));
	pw_tres[4] = (double *)malloc((highIndex - lowIndex) * sizeof (double));
	if (pw_tres[0] == NULL || pw_tres[1] == NULL || pw_tres[2] == NULL || pw_tres[3] == NULL || pw_tres[4] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	for (i = 0;i < highIndex - lowIndex;i++) {
		pw_tres[0][i] = 0;
	}

	// 1
	fftw_p[0] = fftw_plan_dft_1d(fftSize,fftw_in[0],fftw_out[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip[0] = fftw_plan_dft_1d(fftSize,fftw_out[0],fftw_in[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	// 2
	fftw_p[1] = fftw_plan_dft_1d(fftSize,fftw_in[1],fftw_out[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip[1] = fftw_plan_dft_1d(fftSize,fftw_out[1],fftw_in[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	// 3
	fftw_p[2] = fftw_plan_dft_1d(fftSize,fftw_in[2],fftw_out[2],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip[2] = fftw_plan_dft_1d(fftSize,fftw_out[2],fftw_in[2],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	// 1(fast)
	fftw_fast_p[0] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[0],fftw_fast_out[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_fast_p[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_fast_ip[0] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[0],fftw_fast_in[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_fast_ip[0] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	// 2(fast)
	fftw_fast_p[1] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[1],fftw_fast_out[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_fast_p[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_fast_ip[1] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[1],fftw_fast_in[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_fast_ip[1] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	// 3(fast)
	fftw_fast_p[2] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[2],fftw_fast_out[2],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_fast_p[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_fast_ip[2] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[2],fftw_fast_in[2],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_fast_ip[2] == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	if (param->p->hfa3_fft_window_per_1s > 0) {
		// 4
		fftw_p[3] = fftw_plan_dft_1d(fftSize,fftw_in[3],fftw_out[3],FFTW_FORWARD,FFTW_ESTIMATE);
		if (fftw_p[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		fftw_ip[3] = fftw_plan_dft_1d(fftSize,fftw_out[3],fftw_in[3],FFTW_BACKWARD,FFTW_ESTIMATE);
		if (fftw_ip[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		// 5
		fftw_p[4] = fftw_plan_dft_1d(fftSize,fftw_in[4],fftw_out[4],FFTW_FORWARD,FFTW_ESTIMATE);
		if (fftw_p[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		fftw_ip[4] = fftw_plan_dft_1d(fftSize,fftw_out[4],fftw_in[4],FFTW_BACKWARD,FFTW_ESTIMATE);
		if (fftw_ip[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}

		// 4(fast)
		fftw_fast_p[3] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[3],fftw_fast_out[3],FFTW_FORWARD,FFTW_ESTIMATE);
		if (fftw_fast_p[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		fftw_fast_ip[3] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[3],fftw_fast_in[3],FFTW_BACKWARD,FFTW_ESTIMATE);
		if (fftw_fast_ip[3] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		// 5(fast)
		fftw_fast_p[4] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[4],fftw_fast_out[4],FFTW_FORWARD,FFTW_ESTIMATE);
		if (fftw_fast_p[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		fftw_fast_ip[4] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[4],fftw_fast_in[4],FFTW_BACKWARD,FFTW_ESTIMATE);
		if (fftw_fast_ip[4] == NULL) {
			param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
			return;
		}
		if (param->p->hfa3_fft_window_per_1s > 0) {
			// 6
			fftw_p[5] = fftw_plan_dft_1d(fftSize,fftw_in[5],fftw_out[5],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_p[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_ip[5] = fftw_plan_dft_1d(fftSize,fftw_out[5],fftw_in[5],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_ip[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 7
			fftw_p[6] = fftw_plan_dft_1d(fftSize,fftw_in[6],fftw_out[6],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_p[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_ip[6] = fftw_plan_dft_1d(fftSize,fftw_out[6],fftw_in[6],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_ip[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 8
			fftw_p[7] = fftw_plan_dft_1d(fftSize,fftw_in[7],fftw_out[7],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_p[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_ip[7] = fftw_plan_dft_1d(fftSize,fftw_out[7],fftw_in[7],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_ip[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 9
			fftw_p[8] = fftw_plan_dft_1d(fftSize,fftw_in[8],fftw_out[8],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_p[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_ip[8] = fftw_plan_dft_1d(fftSize,fftw_out[8],fftw_in[8],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_ip[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}

			// 6(fast)
			fftw_fast_p[5] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[5],fftw_fast_out[5],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_fast_p[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_fast_ip[5] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[5],fftw_fast_in[5],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_fast_ip[5] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 7(fast)
			fftw_fast_p[6] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[6],fftw_fast_out[6],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_fast_p[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_fast_ip[6] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[6],fftw_fast_in[6],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_fast_ip[6] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 8(fast)
			fftw_fast_p[7] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[7],fftw_fast_out[7],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_fast_p[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_fast_ip[7] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[7],fftw_fast_in[7],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_fast_ip[7] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			// 9(fast)
			fftw_fast_p[8] = fftw_plan_dft_1d(fftFastSize,fftw_fast_in[8],fftw_fast_out[8],FFTW_FORWARD,FFTW_ESTIMATE);
			if (fftw_fast_p[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
			fftw_fast_ip[8] = fftw_plan_dft_1d(fftFastSize,fftw_fast_out[8],fftw_fast_in[8],FFTW_BACKWARD,FFTW_ESTIMATE);
			if (fftw_fast_ip[8] == NULL) {
				param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
				return;
			}
		}
	}

	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_in2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_in2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_p2 = fftw_plan_dft_1d(fftSize_LevelAdj,fftw_in2,fftw_in2,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	memTotal += sizeof(fftw_complex) * fft_malloc_size;
	fftw_in3 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_malloc_size);
	if (fftw_in3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_p3 = fftw_plan_dft_1d(fftSize_LevelAdj,fftw_in3,fftw_in3,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p3 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	for (i = 0;i < 10;i++) {
		spike[i] = 0;
	}
	
	avg = 0;
	for (i = 1;i < fftSize / 2;i++) {
		double t = ((2 * M_PI) / ((fftSize / 2) - 1)) * i;
		if (i >= 76800 * 4) {
			exit(0);
		}
		phaseX[i] = sin(t) * 1000000;
		phaseY[i] = cos(t) * 1000000;
		p = (phaseX[i] * phaseX[i]) + (phaseY[i] * phaseY[i]);
		if (p != 0) {
			p = sqrt(p);
		}
		avg += p;
	}
	if (avg != 0) {
		avg /= (fftSize / 2);
	}
	for (i = 1;i < fftSize / 2;i++) {
		if (i >= 76800 * 4) {
			exit(0);
		}
		p = (phaseX[i] * phaseX[i]) + (phaseY[i] * phaseY[i]);
		if (p != 0) {
			p = sqrt(p);
			nx = avg / p;
			phaseX[i] *= nx;
			phaseY[i] *= nx;
		}
	}

	for (i = 0;i < 360;i++) {
		ovInfo[0]->do2Idx[i] = -1;
		ovInfo[1]->do2Idx[i] = -1;
		ovInfo[2]->do2Idx[i] = -1;
	}
	if (param->p->hfa3_fft_window_per_1s > 0) {
		for (i = 0;i < 360;i++) {
			ovInfo[3]->do2Idx[i] = -1;
			ovInfo[4]->do2Idx[i] = -1;
		}
	}
	if (param->p->hfa3_fft_window_per_1s > 1) {
		for (i = 0;i < 360;i++) {
			ovInfo[5]->do2Idx[i] = -1;
			ovInfo[6]->do2Idx[i] = -1;
			ovInfo[7]->do2Idx[i] = -1;
			ovInfo[8]->do2Idx[i] = -1;
		}
	}

	ovInfo[0]->nSample = fftSize / 2;
	ovInfo[0]->validSamplingRate = hfc;
	ovInfo[0]->samplingRate = outSampleR;
	ovInfo[0]->phaseX = phaseX;
	ovInfo[0]->phaseY = phaseY;
	ovInfo[0]->index = 0;
	ovInfo[0]->fftSize = fftSize;
	ovInfo[0]->fftFastSize = fftFastSize;

	ovInfo[1]->nSample = fftSize / 2;
	ovInfo[1]->validSamplingRate = hfc;
	ovInfo[1]->samplingRate = outSampleR;
	ovInfo[1]->phaseX = phaseX;
	ovInfo[1]->phaseY = phaseY;
	ovInfo[1]->log = 1;
	ovInfo[1]->index = 1;
	ovInfo[1]->fftSize = fftSize;
	ovInfo[1]->fftFastSize = fftFastSize;

	ovInfo[2]->nSample = fftSize / 2;
	ovInfo[2]->validSamplingRate = hfc;
	ovInfo[2]->samplingRate = outSampleR;
	ovInfo[2]->phaseX = phaseX;
	ovInfo[2]->phaseY = phaseY;
	ovInfo[2]->index = 2;
	ovInfo[2]->fftSize = fftSize;
	ovInfo[2]->fftFastSize = fftFastSize;
	if (param->p->hfa3_fft_window_per_1s > 0) {
		ovInfo[3]->nSample = fftSize / 2;
		ovInfo[3]->validSamplingRate = hfc;
		ovInfo[3]->samplingRate = outSampleR;
		ovInfo[3]->phaseX = phaseX;
		ovInfo[3]->phaseY = phaseY;
		ovInfo[3]->index = 3;
		ovInfo[3]->fftSize = fftSize;
		ovInfo[3]->fftFastSize = fftFastSize;

		ovInfo[4]->nSample = fftSize / 2;
		ovInfo[4]->validSamplingRate = hfc;
		ovInfo[4]->samplingRate = outSampleR;
		ovInfo[4]->phaseX = phaseX;
		ovInfo[4]->phaseY = phaseY;
		ovInfo[4]->index = 4;
		ovInfo[4]->fftSize = fftSize;
		ovInfo[4]->fftFastSize = fftFastSize;
	}
	if (param->p->hfa3_fft_window_per_1s > 1) {
		ovInfo[5]->nSample = fftSize / 2;
		ovInfo[5]->validSamplingRate = hfc;
		ovInfo[5]->samplingRate = outSampleR;
		ovInfo[5]->phaseX = phaseX;
		ovInfo[5]->phaseY = phaseY;
		ovInfo[5]->index = 5;
		ovInfo[5]->fftSize = fftSize;
		ovInfo[5]->fftFastSize = fftFastSize;

		ovInfo[6]->nSample = fftSize / 2;
		ovInfo[6]->validSamplingRate = hfc;
		ovInfo[6]->samplingRate = outSampleR;
		ovInfo[6]->phaseX = phaseX;
		ovInfo[6]->phaseY = phaseY;
		ovInfo[6]->index = 6;
		ovInfo[6]->fftSize = fftSize;
		ovInfo[6]->fftFastSize = fftFastSize;

		ovInfo[7]->nSample = fftSize / 2;
		ovInfo[7]->validSamplingRate = hfc;
		ovInfo[7]->samplingRate = outSampleR;
		ovInfo[7]->phaseX = phaseX;
		ovInfo[7]->phaseY = phaseY;
		ovInfo[7]->index = 7;
		ovInfo[7]->fftSize = fftSize;
		ovInfo[7]->fftFastSize = fftFastSize;

		ovInfo[8]->nSample = fftSize / 2;
		ovInfo[8]->validSamplingRate = hfc;
		ovInfo[8]->samplingRate = outSampleR;
		ovInfo[8]->phaseX = phaseX;
		ovInfo[8]->phaseY = phaseY;
		ovInfo[8]->index = 8;
		ovInfo[8]->fftSize = fftSize;
		ovInfo[8]->fftFastSize = fftFastSize;
	}

	stepFftSize = fftSize;
	if (param->p->hfa3_fft_window_per_1s >= 1) {
		stepFftSize /= 2;
	}
	if (param->p->hfa3_fft_window_per_1s >= 2) {
		stepFftSize /= 2;
	}

	pIn[0]	= &mem1[((fftSize / 2) * 0)];
	pOut[0] = &mem2[((fftSize / 2) * 0)];
	pIn[1]	= &mem1[((fftSize / 2) * 1)];
	pOut[1] = &mem3[((fftSize / 2) * 1)];
	pIn[2]	= &mem1[((fftSize / 2) * 2)];
	pOut[2] = &mem4[((fftSize / 2) * 2)];
	if (param->p->hfa3_fft_window_per_1s > 0) {
		pIn[3]	= &mem1[(((fftSize / 2) * 0)) + fftSize / 4];
		pOut[3] = &mem5[(((fftSize / 2) * 0)) + fftSize / 4];

		pIn[4]	= &mem1[(((fftSize / 2) * 1)) + fftSize / 4];
		pOut[4] = &mem6[(((fftSize / 2) * 1)) + fftSize / 4];
	}
	if (param->p->hfa3_fft_window_per_1s > 1) {
		pIn[5]	= &mem1[(((fftSize / 2) * 0)) + fftSize / 4 + fftSize / 8];
		pOut[5] = &mem5[(((fftSize / 2) * 0)) + fftSize / 4 + fftSize / 8];

		pIn[6]	= &mem1[(((fftSize / 2) * 0)) + fftSize / 4 + fftSize / 8];
		pOut[6] = &mem6[(((fftSize / 2) * 0)) + fftSize / 4 + fftSize / 8];

		pIn[7]	= &mem1[(((fftSize / 2) * 1)) + fftSize / 4 + fftSize / 8];
		pOut[7] = &mem5[(((fftSize / 2) * 1)) + fftSize / 4 + fftSize / 8];

		pIn[8]	= &mem1[(((fftSize / 2) * 1)) + fftSize / 4 + fftSize / 8];
		pOut[8] = &mem6[(((fftSize / 2) * 1)) + fftSize / 4 + fftSize / 8];
	}
	per = -1;

	sprintf(param->timecode,"0:00:00");

	for (startInSample = ((fftSize + (fftSize / 2)) * -1);startInSample < inSample + ((fftSize / 2));startInSample += fftSize) {
		sprintf(sss,"genOverTone sec:%lld",startInSample / outSampleR);
		PRINT_LOG("",sss);
		if (startInSample >= 0 && startInSample < inSample) {
			hh = ((startInSample / outSampleR) / (60 * 60));
			mm = (((startInSample / outSampleR) / 60) % 60);
			ss = ((startInSample / outSampleR) % 60);
			sprintf(param->timecode,"%lld:%02lld:%02lld",hh,mm,ss);
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}
		memset(mem1,0,wkMemSize);

		if (startInSample >= 0 && startInSample + (fftSize * 2) < inSample) {
			nSample = fftSize * 2;
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else {
			mem0 = mem1;
			nSample = fftSize * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r,0,SEEK_SET);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}

		memset(mem2,0,wkMemSize);
		memset(mem3,0,wkMemSize);
		memset(mem4,0,wkMemSize);
		if (param->p->hfa3_fft_window_per_1s >= 1) {
			memset(mem5,0,wkMemSize);
			memset(mem6,0,wkMemSize);
		}
		if (param->p->hfa3_fft_window_per_1s >= 2) {
			memset(mem7,0,wkMemSize);
			memset(mem8,0,wkMemSize);
			memset(mem9,0,wkMemSize);
			memset(mem10,0,wkMemSize);
		}
		ovInfo[0]->base_freq = 0;
		ovInfo[1]->base_freq = 0;
		ovInfo[2]->base_freq = 0;
		if (param->p->hfa3_fft_window_per_1s >= 1) {
			ovInfo[3]->base_freq = 0;
			ovInfo[4]->base_freq = 0;
		}
		if (param->p->hfa3_fft_window_per_1s >= 2) {
			ovInfo[5]->base_freq = 0;
			ovInfo[6]->base_freq = 0;
			ovInfo[7]->base_freq = 0;
			ovInfo[8]->base_freq = 0;
		}

		ovInfo[0]->sfa_sec = startInSample / outSampleR;
		ovInfo[1]->sfa_sec = startInSample / outSampleR;
		ovInfo[2]->sfa_sec = startInSample / outSampleR;
		ovInfo[0]->startInSample = startInSample;
		ovInfo[1]->startInSample = startInSample;
		ovInfo[2]->startInSample = startInSample;
		if (param->p->hfa3_fft_window_per_1s >= 1) {
			ovInfo[3]->sfa_sec = startInSample / outSampleR;
			ovInfo[4]->sfa_sec = startInSample / outSampleR;
			ovInfo[3]->startInSample = startInSample;
			ovInfo[4]->startInSample = startInSample;
		}
		if (param->p->hfa3_fft_window_per_1s >= 2) {
			ovInfo[5]->sfa_sec = startInSample / outSampleR;
			ovInfo[6]->sfa_sec = startInSample / outSampleR;
			ovInfo[7]->sfa_sec = startInSample / outSampleR;
			ovInfo[8]->sfa_sec = startInSample / outSampleR;
			ovInfo[5]->startInSample = startInSample;
			ovInfo[6]->startInSample = startInSample;
			ovInfo[7]->startInSample = startInSample;
			ovInfo[8]->startInSample = startInSample;
		}

		if (param->enable_sfa == 1) {
			int sfa_hfc;
			// SFA 用の HFC 設定(データがある上限の周波数を使用する

			FFT_CUTINFO *cutinfo;

			cutinfo = fft_param->fft_cut_info;

			sfa_sec = startInSample / outSampleR;

			sfa_hfc = 0;
			while (cutinfo != NULL) {
				if (cutinfo->sec == sfa_sec && cutinfo->cut_count == 0) {
					if (sfa_hfc != 0 && sfa_hfc > cutinfo->sfc) {
						sfa_hfc = cutinfo->sfc;
					} else if (sfa_hfc == 0) {
						sfa_hfc = cutinfo->sfc;
					}
					break;
				}
				cutinfo = cutinfo->next;
			}
			if (sfa_hfc == 0) {
				sfa_hfc = outSampleR / 2;
			}
			ovInfo[0]->sfa_hfc = sfa_hfc;
			ovInfo[1]->sfa_hfc = sfa_hfc;
			ovInfo[2]->sfa_hfc = sfa_hfc;
			if (param->p->hfa3_fft_window_per_1s > 0) {
				ovInfo[3]->sfa_hfc = sfa_hfc;
				ovInfo[4]->sfa_hfc = sfa_hfc;
			}
			ovInfo[0]->validSamplingRate = sfa_hfc;
			ovInfo[1]->validSamplingRate = sfa_hfc;
			ovInfo[2]->validSamplingRate = sfa_hfc;
			if (param->p->hfa3_fft_window_per_1s > 0) {
				ovInfo[3]->validSamplingRate = sfa_hfc;
				ovInfo[4]->validSamplingRate = sfa_hfc;
			}
			hfc = sfa_hfc;
		}

		// 周波数の変化を調べる
		ovInfo[0]->nSample = fftSize / 2;
		ovInfo[1]->nSample = fftSize / 2;
		ovInfo[2]->nSample = fftSize / 2;
		ovInfo[0]->fast_index = -1;
		ovInfo[1]->fast_index = -1;
		ovInfo[2]->fast_index = -1;

		if (param->p->hfa3_fft_window_per_1s > 0) {
			ovInfo[3]->nSample = fftSize / 2;
			ovInfo[4]->nSample = fftSize / 2;
			ovInfo[3]->fast_index = -1;
			ovInfo[4]->fast_index = -1;
		}
		if (param->p->hfa3_fft_window_per_1s > 1) {
			ovInfo[5]->nSample = fftSize / 2;
			ovInfo[6]->nSample = fftSize / 2;
			ovInfo[7]->nSample = fftSize / 2;
			ovInfo[8]->nSample = fftSize / 2;
			ovInfo[5]->fast_index = -1;
			ovInfo[6]->fast_index = -1;
			ovInfo[7]->fast_index = -1;
			ovInfo[8]->fast_index = -1;
		}

		fast_index = 0;
		fast_index_count = 1;

		if (startInSample + fftFastSize / 2 >= 0) {
			SSIZE *pIn2;
			pIn2  = (SSIZE *)mem1;
			pIn2  += (fftFastSize / 2);
			double fastDiffPw = 0;
			double tempFastdiffPw;
			long   fastDiffPwCount = 0;
			for (i = 0;i < 3;i++) {
				for (j = 0;j < fftFastSize;j++) {
					fftw_fast_in[0][j][0] = pIn2[j];
					fftw_fast_in[0][j][1] = 0;
				}
				pIn2 += fftFastSize;
				// 窓関数
				for (j = 0;j < (fftFastSize - 1) / 2;j++) {
					fftw_fast_in[0][j][0] = fftw_fast_in[0][j][0] * (2.0 * j / (double)fftFastSize);
				}
				for (j = (fftFastSize - 1) / 2;j < fftFastSize;j++) {
					fftw_fast_in[0][j][0] = fftw_fast_in[0][j][0] * (2.0 - 2.0 * j / (double)fftFastSize);
				}
				// FFT
				fftw_execute(fftw_fast_p[0]);

				// 元信号の高域のパワーを調べる
				lowIndex = ((double)fftFastSize / outSampleR)  * (hfc - 3000);
				highIndex = ((double)fftFastSize / outSampleR) * hfc;
				for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
					p = fftw_fast_in[0][j][0] * fftw_fast_in[0][j][0] + fftw_fast_in[0][j][1] * fftw_fast_in[0][j][1];
					if (p != 0) {
						p = sqrt(p);
					}
					if (nn >= 76800 * 4) {
						exit(0);
					}
					pw_tres[i + 1][nn] = p;
				}
			}
			for (i = 0;i < 3;i++) {
				for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
					if (pw_tres[i + 1][nn] >= pw_tres[i][nn]) {
						tempFastdiffPw = pw_tres[i + 1][nn] - pw_tres[i][nn];
					} else {
						tempFastdiffPw = pw_tres[i][nn] - pw_tres[i + 1][nn];
					}
					fastDiffPw += tempFastdiffPw;
					fastDiffPwCount++;
				}
				if (i == 0) {
					// index3 の値に前回の値を設定する
					for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
						pw_tres[0][nn] = pw_tres[3][nn];
					}
				}
			}
			if (fastDiffPwCount > 0) {
				fastDiffPw /= fastDiffPwCount;
			}
//			if (1) {
//				char sss[200];
//				sprintf(sss,"fastDiffPw: sec:%lld,%lf",startInSample / outSampleR,fastDiffPw);
//				PRINT_LOG("HFA4",sss);
//			}

			fast_diff_pw_flag = 0;
			if (fastDiffPw >= (double)3000000000000000) {
				fast_diff_pw_flag = 1;
			}
#if 0
			if (fast_diff_pw_flag == 1) {
				if (1) {
					char sss[200];
					sprintf(sss,"fastDiffPw: sec:%lld,%lf",startInSample / outSampleR,fastDiffPw);
					PRINT_LOG("HFA4",sss);
				}
				fast_index_count = 3;
				ovInfo[0]->nSample = fftFastSize / 2;
				ovInfo[1]->nSample = fftFastSize / 2;
				ovInfo[2]->nSample = fftFastSize / 2;
				ovInfo[0]->fast_index = 0;
				ovInfo[1]->fast_index = 0;
				ovInfo[2]->fast_index = 0;
				if (param->p->hfa3_fft_window_per_1s > 0) {
					ovInfo[3]->nSample = fftFastSize / 2;
					ovInfo[4]->nSample = fftFastSize / 2;
					ovInfo[3]->fast_index = 0;
					ovInfo[4]->fast_index = 0;
				}
				if (param->p->hfa3_fft_window_per_1s > 1) {
					ovInfo[5]->nSample = fftFastSize / 2;
					ovInfo[6]->nSample = fftFastSize / 2;
					ovInfo[7]->nSample = fftFastSize / 2;
					ovInfo[8]->nSample = fftFastSize / 2;
					ovInfo[5]->fast_index = 0;
					ovInfo[6]->fast_index = 0;
					ovInfo[7]->fast_index = 0;
					ovInfo[8]->fast_index = 0;
				}
			}
#endif
		}
		for (;fast_index < fast_index_count;fast_index++) {
			if (param->p->hfa3_fft_window_per_1s == 2) {
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							// 1
							genOverToneSub(hfc,pIn[0],pOut[0],fftw_in[0],fftw_out[0],fftw_p[0],fftw_ip[0],fftw_fast_in[0],fftw_fast_out[0],fftw_fast_p[0],fftw_fast_ip[0],ovInfo[0],param,fft_param);
						}
						#pragma omp section
						{
							// 2
							genOverToneSub(hfc,pIn[1],pOut[1],fftw_in[1],fftw_out[1],fftw_p[1],fftw_ip[1],fftw_fast_in[1],fftw_fast_out[1],fftw_fast_p[1],fftw_fast_ip[1],ovInfo[1],param,fft_param);
						}
						#pragma omp section
						{
							// 3
							genOverToneSub(hfc,pIn[2],pOut[2],fftw_in[2],fftw_out[2],fftw_p[2],fftw_ip[2],fftw_fast_in[2],fftw_fast_out[2],fftw_fast_p[2],fftw_fast_ip[2],ovInfo[2],param,fft_param);
						}
						#pragma omp section
						{
							// 4
							genOverToneSub(hfc,pIn[3],pOut[3],fftw_in[3],fftw_out[3],fftw_p[3],fftw_ip[3],fftw_fast_in[3],fftw_fast_out[3],fftw_fast_p[3],fftw_fast_ip[3],ovInfo[3],param,fft_param);
						}
						#pragma omp section
						{
							// 5
							genOverToneSub(hfc,pIn[4],pOut[4],fftw_in[4],fftw_out[4],fftw_p[4],fftw_ip[4],fftw_fast_in[4],fftw_fast_out[4],fftw_fast_p[4],fftw_fast_ip[4],ovInfo[4],param,fft_param);
						}
						#pragma omp section
						{
							// 6
							genOverToneSub(hfc,pIn[5],pOut[5],fftw_in[5],fftw_out[5],fftw_p[5],fftw_ip[5],fftw_fast_in[5],fftw_fast_out[5],fftw_fast_p[5],fftw_fast_ip[5],ovInfo[5],param,fft_param);
						}
						#pragma omp section
						{
							// 7
							genOverToneSub(hfc,pIn[6],pOut[6],fftw_in[6],fftw_out[6],fftw_p[6],fftw_ip[6],fftw_fast_in[6],fftw_fast_out[6],fftw_fast_p[6],fftw_fast_ip[6],ovInfo[6],param,fft_param);
						}
						#pragma omp section
						{
							// 8
							genOverToneSub(hfc,pIn[7],pOut[7],fftw_in[7],fftw_out[7],fftw_p[7],fftw_ip[7],fftw_fast_in[7],fftw_fast_out[7],fftw_fast_p[7],fftw_fast_ip[7],ovInfo[7],param,fft_param);
						}
						#pragma omp section
						{
							// 9
							genOverToneSub(hfc,pIn[8],pOut[8],fftw_in[8],fftw_out[8],fftw_p[8],fftw_ip[8],fftw_fast_in[8],fftw_fast_out[8],fftw_fast_p[8],fftw_fast_ip[8],ovInfo[8],param,fft_param);
						}
					}
				}
				windowData(mem2,fftSize * 2);
				windowData(mem3,fftSize * 2);
				windowData(mem4,fftSize * 2);
				windowData(mem5,fftSize * 2);
				windowData(mem6,fftSize * 2);
				windowData(mem7,fftSize * 2);
				windowData(mem8,fftSize * 2);
				windowData(mem9,fftSize * 2);
				windowData(mem10,fftSize * 2);

				windowData(mem2,fftSize * 2);
				windowData(mem3,fftSize * 2);
				windowData(mem4,fftSize * 2);
				windowData(mem5,fftSize * 2);
				windowData(mem6,fftSize * 2);
				windowData(mem7,fftSize * 2);
				windowData(mem8,fftSize * 2);
				windowData(mem9,fftSize * 2);
				windowData(mem10,fftSize * 2);
				for (i = 0;i < fftSize * 2;i++) {
					mem2[i] += mem3[i] + mem4[i] + mem5[i] + mem6[i] + mem7[i] + mem8[i] + mem9[i] + mem10[i];
				}
			} else if (param->p->hfa3_fft_window_per_1s == 1) {
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							// 1
							genOverToneSub(hfc,pIn[0],pOut[0],fftw_in[0],fftw_out[0],fftw_p[0],fftw_ip[0],fftw_fast_in[0],fftw_fast_out[0],fftw_fast_p[0],fftw_fast_ip[0],ovInfo[0],param,fft_param);
						}
						#pragma omp section
						{
							// 2
							genOverToneSub(hfc,pIn[1],pOut[1],fftw_in[1],fftw_out[1],fftw_p[1],fftw_ip[1],fftw_fast_in[1],fftw_fast_out[1],fftw_fast_p[1],fftw_fast_ip[1],ovInfo[1],param,fft_param);
						}
						#pragma omp section
						{
							// 3
							genOverToneSub(hfc,pIn[2],pOut[2],fftw_in[2],fftw_out[2],fftw_p[2],fftw_ip[2],fftw_fast_in[2],fftw_fast_out[2],fftw_fast_p[2],fftw_fast_ip[2],ovInfo[2],param,fft_param);
						}
						#pragma omp section
						{
							// 4
							genOverToneSub(hfc,pIn[3],pOut[3],fftw_in[3],fftw_out[3],fftw_p[3],fftw_ip[3],fftw_fast_in[3],fftw_fast_out[3],fftw_fast_p[3],fftw_fast_ip[3],ovInfo[3],param,fft_param);
						}
						#pragma omp section
						{
							// 5
							genOverToneSub(hfc,pIn[4],pOut[4],fftw_in[4],fftw_out[4],fftw_p[4],fftw_ip[4],fftw_fast_in[4],fftw_fast_out[4],fftw_fast_p[4],fftw_fast_ip[4],ovInfo[4],param,fft_param);
						}
					}
				}
				windowData(mem2,fftSize * 2);
				windowData(mem3,fftSize * 2);
				windowData(mem4,fftSize * 2);
				windowData(mem5,fftSize * 2);
				windowData(mem6,fftSize * 2);
				for (i = 0;i < fftSize * 2;i++) {
					mem2[i] += mem3[i] + mem4[i] + mem5[i] + mem6[i];
				}
			} else {
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							// 1
							genOverToneSub(hfc,pIn[0],pOut[0],fftw_in[0],fftw_out[0],fftw_p[0],fftw_ip[0],fftw_fast_in[0],fftw_fast_out[0],fftw_fast_p[0],fftw_fast_ip[0],ovInfo[0],param,fft_param);
						}
						#pragma omp section
						{
							// 2
							genOverToneSub(hfc,pIn[1],pOut[1],fftw_in[1],fftw_out[1],fftw_p[1],fftw_ip[1],fftw_fast_in[1],fftw_fast_out[1],fftw_fast_p[1],fftw_fast_ip[1],ovInfo[1],param,fft_param);
						}
						#pragma omp section
						{
							// 3
							genOverToneSub(hfc,pIn[2],pOut[2],fftw_in[2],fftw_out[2],fftw_p[2],fftw_ip[2],fftw_fast_in[2],fftw_fast_out[2],fftw_fast_p[2],fftw_fast_ip[2],ovInfo[2],param,fft_param);
						}
					}
				}
				for (i = 0;i < fftSize * 2;i++) {
					mem2[i] += mem3[i] + mem4[i];
				}
			}
			if (ovInfo[0]->fast_index >= 0)	ovInfo[0]->fast_index++;
			if (ovInfo[1]->fast_index >= 0)	ovInfo[1]->fast_index++;
			if (ovInfo[2]->fast_index >= 0)	ovInfo[2]->fast_index++;
			if (param->p->hfa3_fft_window_per_1s > 0) {
				if (ovInfo[3]->fast_index >= 0)	ovInfo[3]->fast_index++;
				if (ovInfo[4]->fast_index >= 0)	ovInfo[4]->fast_index++;
			}
			if (param->p->hfa3_fft_window_per_1s > 1) {
				if (ovInfo[5]->fast_index >= 0)	ovInfo[5]->fast_index++;
				if (ovInfo[6]->fast_index >= 0)	ovInfo[6]->fast_index++;
				if (ovInfo[7]->fast_index >= 0)	ovInfo[7]->fast_index++;
				if (ovInfo[8]->fast_index >= 0)	ovInfo[8]->fast_index++;
			}
		}
		if (startInSample + fftSize / 2 >= 0) {
			SSIZE *pIn2,*pOut2;
			// レベル調整
			pIn2  = (SSIZE *)mem1;
			pOut2 = (SSIZE *)mem2;
			pIn2  += (fftSize / 2);
			pOut2 += (fftSize / 2);
			for (i = 0;i < LevelAdjCount;i++) {
				for (j = 0;j < fftSize / LevelAdjCount;j++) {
					fftw_in2[j][0] = pIn2[j];
					fftw_in2[j][1] = 0;
				}
				pIn2 += fftSize / LevelAdjCount;
				// 窓関数
				for (j = 0;j < ((fftSize / LevelAdjCount) - 1) / 2;j++) {
					fftw_in2[j][0] = fftw_in2[j][0] * (2.0 * j / ((double)fftSize / LevelAdjCount));
				}
				for (j = ((fftSize / LevelAdjCount) - 1) / 2;j < (fftSize / LevelAdjCount);j++) {
					fftw_in2[j][0] = fftw_in2[j][0] * (2.0 - 2.0 * j / ((double)fftSize / LevelAdjCount));
				}
				// FFT
				fftw_execute(fftw_p2);

				// 元信号の高域のパワーを調べる
				refLowHzPw = 0;
				lowIndex = 1;
				highIndex = (((double)fftSize / LevelAdjCount) / outSampleR) * 5000;
				memset(ovInfo[0]->avg,0,sizeof (double));
				for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
					p = fftw_in2[j][0] * fftw_in2[j][0] + fftw_in2[j][1] * fftw_in2[j][1];
					if (p != 0) {
						p = sqrt(p);
					}
					if (nn >= 76800 * 4) {
						exit(0);
					}
					ovInfo[0]->avg[nn] = p;
				}
				for (j = 0;j + 1 < nn;j++) {
					for (k = j;k < nn;k++) {
						if (ovInfo[0]->avg[j] > ovInfo[0]->avg[k]) {
							p = ovInfo[0]->avg[j];
							ovInfo[0]->avg[j] = ovInfo[0]->avg[k];
							ovInfo[0]->avg[k] = p;
						}
					}
				}
				for (j = 0;j + 2 < nn;j++) {
					refLowHzPw += ovInfo[0]->avg[j];
				}
				if (j > 0) {
					refLowHzPw /= j;
				}

				refPw = 0;
				lowIndex = (((double)fftSize / LevelAdjCount) / outSampleR) * (hfc - LevelAdjHz);
				highIndex = (((double)fftSize / LevelAdjCount) / outSampleR) * hfc;
				memset(ovInfo[0]->avg,0,sizeof (double));
				for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
					p = fftw_in2[j][0] * fftw_in2[j][0] + fftw_in2[j][1] * fftw_in2[j][1];
					if (p != 0) {
						p = sqrt(p);
					}
					if (nn >= 76800 * 4) {
						exit(0);
					}
					ovInfo[0]->avg[nn] = p;
				}
				for (j = 0;j + 1 < nn;j++) {
					for (k = j;k < nn;k++) {
						if (ovInfo[0]->avg[j] > ovInfo[0]->avg[k]) {
							p = ovInfo[0]->avg[j];
							ovInfo[0]->avg[j] = ovInfo[0]->avg[k];
							ovInfo[0]->avg[k] = p;
						}
					}
				}
				for (j = 0;j + 2 < nn;j++) {
					refPw += ovInfo[0]->avg[j];
				}
				if (j > 0) {
					refPw /= j;
				}
				refPw -= (refPw / 10);

				// 付加信号の高域のパワーを調べる
				for (j = 0;j < fftSize / LevelAdjCount;j++) {
					fftw_in3[j][0] = pOut2[j];
					fftw_in3[j][1] = 0;
				}
				// 窓関数
				for (j = 0;j < ((fftSize / LevelAdjCount) - 1) / 2;j++) {
					fftw_in3[j][0] = fftw_in3[j][0] * (2.0 * j / ((double)fftSize / LevelAdjCount));
				}
				for (j = ((fftSize / LevelAdjCount) - 1) / 2;j < (fftSize / LevelAdjCount);j++) {
					fftw_in3[j][0] = fftw_in3[j][0] * (2.0 - 2.0 * j / ((double)fftSize / LevelAdjCount));
				}
				// FFT
				fftw_execute(fftw_p3);

				ovTonePw = 0;
				lowIndex = (((double)fftSize / LevelAdjCount) / outSampleR) * hfc;
				highIndex = (((double)fftSize / LevelAdjCount) / outSampleR) * (hfc + LevelAdjHz);
				memset(ovInfo[0]->avg,0,sizeof (double));
				for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
					p = fftw_in3[j][0] * fftw_in3[j][0] + fftw_in3[j][1] * fftw_in3[j][1];
					if (p != 0) {
						p = sqrt(p);
					}
					ovInfo[0]->avg[nn] = p;
				}
				for (j = 0;j + 1 < nn;j++) {
					for (k = j;k < nn;k++) {
						if (ovInfo[0]->avg[j] > ovInfo[0]->avg[k]) {
							p = ovInfo[0]->avg[j];
							ovInfo[0]->avg[j] = ovInfo[0]->avg[k];
							ovInfo[0]->avg[k] = p;
						}
					}
				}
				for (j = 0;j + 3 < nn;j++) {
					ovTonePw += ovInfo[0]->avg[j];
				}
				if (j > 0) {
					ovTonePw /= j;
				}

//				if (startInSample / outSampleR > 88) {
//					char sss[200];
//					sprintf(sss,"LevelAdjPw: sec:%lld,refLowHzPw:%lf,refPw:%lf,ovTonePw:%lf",startInSample / outSampleR,refLowHzPw,refPw,ovTonePw);
//					PRINT_LOG("HFA4",sss);
//				}

				if (refPw >= 9000000000) {
					for (j = 0;j < fftSize / LevelAdjCount;j++) {
						pOut2[j] *= ((refPw / ovTonePw));
						pOut2[j] *= hfa_nx;
					}
					if (param->p->hfa_level > 0) {
						pOut2[j] *= table_hfa_level[param->p->hfa_level - 1];
					}
				} else {
					for (j = 0;j < fftSize / LevelAdjCount;j++) {
						pOut2[j] = 0;
					}
				}

				pOut2 += fftSize / LevelAdjCount;
			}
			if (LevelAdjCount == 0) {
				memcpy(mem2,mem1,sizeof (SSIZE) * fftSize);
			}

			// パルス除去
			pIn2  = (SSIZE *)mem2;
			for (i = 1;i + 1 < fftSize;i++) {
				p1 = pIn2[i - 1];
				p2 = pIn2[i];
				p3 = pIn2[i + 1];
				p1 >>= (56 - param->p->w);
				p2 >>= (56 - param->p->w);
				p3 >>= (56 - param->p->w);
				if (p1 < p2 && p2 > p3) {
					if (p2 - ((p1 + p3) / 2) >= 4) {
						pIn2[i] = ((pIn2[i - 1] + pIn2[i + 1]) / 2);
					}
				} else if (p1 > p2 && p2 < p3) {
					if (((p1 + p3) / 2) - p2 >= 4) {
						pIn2[i] = ((pIn2[i - 1] + pIn2[i + 1]) / 2);
					}
				}
			}
			outTempFile(fp_w,mem2 + fftSize / 2,fftSize,param);
			if (param->err) {
				break;
			}
		}
	}
	fio_flush(fp_w);
	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);
	if (param->p->hfa3_fft_window_per_1s > 0) {
		al_free(mem5);
		al_free(mem6);
	}
	if (param->p->hfa3_fft_window_per_1s > 1) {
		al_free(mem7);
		al_free(mem8);
		al_free(mem9);
		al_free(mem10);
	}

	al_free(phaseX);
	al_free(phaseY);

	free(ovInfo[0]);
	free(ovInfo[1]);
	free(ovInfo[2]);
	if (param->p->hfa3_fft_window_per_1s > 0) {
		free(ovInfo[3]);
		free(ovInfo[4]);
	}
	if (param->p->hfa3_fft_window_per_1s > 1) {
		free(ovInfo[5]);
		free(ovInfo[6]);
		free(ovInfo[7]);
		free(ovInfo[8]);
	}
	fftw_destroy_plan(fftw_p[0]);
	fftw_destroy_plan(fftw_ip[0]);

	fftw_destroy_plan(fftw_p[1]);
	fftw_destroy_plan(fftw_ip[1]);

	fftw_destroy_plan(fftw_p[2]);
	fftw_destroy_plan(fftw_ip[2]);

	fftw_destroy_plan(fftw_fast_p[0]);
	fftw_destroy_plan(fftw_fast_ip[0]);

	fftw_destroy_plan(fftw_fast_p[1]);
	fftw_destroy_plan(fftw_fast_ip[1]);

	fftw_destroy_plan(fftw_fast_p[2]);
	fftw_destroy_plan(fftw_fast_ip[2]);

	if (param->p->hfa3_fft_window_per_1s > 0) {
		fftw_destroy_plan(fftw_p[3]);
		fftw_destroy_plan(fftw_ip[3]);

		fftw_destroy_plan(fftw_p[4]);
		fftw_destroy_plan(fftw_ip[4]);

		fftw_destroy_plan(fftw_fast_p[3]);
		fftw_destroy_plan(fftw_fast_ip[3]);

		fftw_destroy_plan(fftw_fast_p[4]);
		fftw_destroy_plan(fftw_fast_ip[4]);
	}
	if (param->p->hfa3_fft_window_per_1s > 1) {
		fftw_destroy_plan(fftw_p[5]);
		fftw_destroy_plan(fftw_ip[5]);

		fftw_destroy_plan(fftw_p[6]);
		fftw_destroy_plan(fftw_ip[6]);

		fftw_destroy_plan(fftw_p[7]);
		fftw_destroy_plan(fftw_ip[7]);

		fftw_destroy_plan(fftw_p[8]);
		fftw_destroy_plan(fftw_ip[8]);

		fftw_destroy_plan(fftw_fast_p[5]);
		fftw_destroy_plan(fftw_fast_ip[5]);

		fftw_destroy_plan(fftw_fast_p[6]);
		fftw_destroy_plan(fftw_fast_ip[6]);

		fftw_destroy_plan(fftw_fast_p[7]);
		fftw_destroy_plan(fftw_fast_ip[7]);

		fftw_destroy_plan(fftw_fast_p[8]);
		fftw_destroy_plan(fftw_fast_ip[8]);
	}

	fftw_destroy_plan(fftw_p2);
	fftw_destroy_plan(fftw_p3);

	fftw_free(fftw_in[0]);
	fftw_free(fftw_out[0]);

	fftw_free(fftw_in[1]);
	fftw_free(fftw_out[1]);

	fftw_free(fftw_in[2]);
	fftw_free(fftw_out[2]);

	fftw_free(fftw_fast_in[0]);
	fftw_free(fftw_fast_out[0]);

	fftw_free(fftw_fast_in[1]);
	fftw_free(fftw_fast_out[1]);

	fftw_free(fftw_fast_in[2]);
	fftw_free(fftw_fast_out[2]);

	if (param->p->hfa3_fft_window_per_1s > 0) {
		fftw_free(fftw_in[3]);
		fftw_free(fftw_out[3]);
		fftw_free(fftw_in[4]);
		fftw_free(fftw_out[4]);

		fftw_free(fftw_fast_in[3]);
		fftw_free(fftw_fast_out[3]);
		fftw_free(fftw_fast_in[4]);
		fftw_free(fftw_fast_out[4]);
	}

	if (param->p->hfa3_fft_window_per_1s > 1) {
		fftw_free(fftw_in[5]);
		fftw_free(fftw_out[5]);
		fftw_free(fftw_in[6]);
		fftw_free(fftw_out[6]);
		fftw_free(fftw_in[7]);
		fftw_free(fftw_out[7]);
		fftw_free(fftw_in[8]);
		fftw_free(fftw_out[8]);

		fftw_free(fftw_fast_in[5]);
		fftw_free(fftw_fast_out[5]);
		fftw_free(fftw_fast_in[6]);
		fftw_free(fftw_fast_out[6]);
		fftw_free(fftw_fast_in[7]);
		fftw_free(fftw_fast_out[7]);
		fftw_free(fftw_fast_in[8]);
		fftw_free(fftw_fast_out[8]);
	}

	fftw_free(fftw_in2);
	fftw_free(fftw_in3);
	
	if (param->maxMemorySize < memTotal) {
		param->maxMemorySize = memTotal;
	}
	PRINT_FN_LOG(param->debug_upconv,"-genOverTone:End");
}

//---------------------------------------------------------------------------
// Function   : genOverToneSub
// Description: 失われた高域の再現処理のサブ関数(倍音解析)
// ---
//	hfc		 			:高域のカットオフ周波数(この周波数以上の領域にデータを追加する)
//	pIn					:入力バッファ
//	pOut				:出力バッファ
//	arg_fftw_in			:FFTW 入力
//	arg_fftw_out		:FFTW 出力
//	arg_fftw_p			:FFTW プラン
//	arg_fftw_ip			:FFTW プラン
//	arg_fftw_fast_in	:FFTW 入力(fast)
//	arg_fftw_fast_out	:FFTW 出力(fast)
//	arg_fftw_fast_p		:FFTW プラン(fast)
//	arg_fftw_fast_ip	:FFTW プラン(fast)
//	ovInfo		:高域生成用構造体
//	param		:変換パラメータ
//
void genOverToneSub(long hfc,SSIZE *pIn,SSIZE *pOut,fftw_complex *arg_fftw_in,fftw_complex *arg_fftw_out,fftw_plan arg_fftw_p,fftw_plan arg_fftw_ip,fftw_complex *arg_fftw_fast_in,fftw_complex *arg_fftw_fast_out,fftw_plan arg_fftw_fast_p,fftw_plan arg_fftw_fast_ip,OVERTONE_INFO *ovInfo,PARAM_INFO *param,FFT_PARAM *fft_param)
{
	long outSampleR;
	long fftSize,fftFastSize,i,ii,j,n,nn,idx;
	long lowIndex,highIndex,workIndex,workCount;
	long sfa_width;
	long sfa_hfc;
	long avg_count;
	double nx;
	double p,p2,p3,refPw,ovTonePw;
	double phaseTemp;
	double from_avg_top10;
	double to_avg_top10;
	double thresh_diff_pw;
	double adj_avg,adj_peak;
	int    from_avg_top10_count;
	int	   to_avg_top10_count;
	int    hz;
	double hfa_nx;
	int d,d2;
	int overToneNotFound;
	int avg_peak_count;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan    fftw_p,fftw_ip;
	static double sfa_test_pw[400];
	static int    sfa_test_count[400];

	outSampleR = param->p->outSampleR;
	if (ovInfo->fast_index == -1) {
		fftw_in   = arg_fftw_in;
		fftw_out  = arg_fftw_out;
		fftw_p    = arg_fftw_p;
		fftw_ip   = arg_fftw_ip;
		fftSize   = ovInfo->fftSize;
	} else {
		fftw_in   = arg_fftw_fast_in;
		fftw_out  = arg_fftw_fast_out;
		fftw_p    = arg_fftw_fast_p;
		fftw_ip   = arg_fftw_fast_ip;
		fftSize   = ovInfo->fftFastSize;
		pIn       = &pIn[(fftSize * ovInfo->fast_index)];
		pOut      = &pOut[(fftSize * ovInfo->fast_index)];
	}

#if 0
	if (param->p->hfa3_version == 1) {
		fftSize = 4096;
	} else if (param->p->hfa3_version == 2) {
		fftSize = outSampleR / 14;
	} else {
		fftSize = outSampleR / 14;
	}
	// 96000
	if (outSampleR == 44100 * 2 || outSampleR == 48000 * 2) {
		fftSize = outSampleR / 10;
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 16;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 4096 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 14;
		} else {
			fftSize = outSampleR / 14;
		}
	}
	// 192000
	if (outSampleR == 32000 * 6 || outSampleR == 44100 * 4 || outSampleR == 48000 * 4) {
		fftSize = outSampleR / 10;
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 17;
			fftSize = outSampleR / 12;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 8192 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 14;
		} else {
			fftSize = outSampleR / 14;
		}
	}
	// 384000
	if (outSampleR == 32000 * 12 || outSampleR == 44100 * 8 || outSampleR == 48000 * 8) {
		fftSize = outSampleR / 10;
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 18;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 16384 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 16;
		} else {
			fftSize = outSampleR / 16;
			fftSize = outSampleR / 32;
		}
	}
	// 768000
	if (outSampleR == 32000 * 24 || outSampleR == 44100 * 16 || outSampleR == 48000 * 16) {
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 14;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 32768 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 16;
		} else {
			fftSize = outSampleR / 16;
		}
	}
	// 1536000
	if (outSampleR == 32000 * 48 || outSampleR == 44100 * 32 || outSampleR == 48000 * 32) {
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 14;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 65536 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 18;
		} else {
			fftSize = outSampleR / 18;
		}
	}
	// 3072000
	if (outSampleR == 32000 * 64 || outSampleR == 44100 * 64 || outSampleR == 48000 * 64) {
		if (param->p->hfa == 4) {
			fftSize = outSampleR / 14;
		} else if (param->p->hfa3_version == 1) {
			fftSize = 65536 * 2;
		} else if (param->p->hfa3_version == 2) {
			fftSize = outSampleR / 20;
		} else {
			fftSize = outSampleR / 20;
		}
	}
	if (param->p->hfa3_fft_window_size != 0) {
		fftSize = outSampleR / param->p->hfa3_fft_window_size;
	}
#endif

	if (param->p->hfa3_version == 1) {
		hfa_nx = (double)param->p->hfa_level_adjust_nx / 100.0;
	} else if (param->p->hfa3_version == 2) {
		hfa_nx = (double)param->p->hfa_level_adjust_nx / 100.0;
	} else if (param->p->hfa3_version == 3) {
		hfa_nx = (double)param->p->hfa_level_adjust_nx / 100.0;
	}

	// 初期設定
	memset(ovInfo->pw_cnt,0,307200 * sizeof (int));
	for (i = 0;i < 307200;i++) {
		ovInfo->peak[i] = 0;
	}
	// 倍音解析
	memset(ovInfo->power,0,307200 * sizeof (double));
	memset(ovInfo->power_org,0,307200 * sizeof (double));
	memset(ovInfo->phase,0,307200 * sizeof (double));
	memset(ovInfo->pw,0,307200 * sizeof (double));
	memset(ovInfo->avg,0,307200 * sizeof (double));
	memset(ovInfo->diff,0,307200 * sizeof (double));
	memset(ovInfo->base,0,307200 * sizeof (double));
	memset(ovInfo->baseToAdj,0,307200 * sizeof (double));
	memset(ovInfo->from_top10,0,10 * sizeof (double));
	memset(ovInfo->to_top10,0,10 * sizeof (double));
	memset(ovInfo->hfa4_coeff,0,307200 * sizeof (double));

	// SFA用
	memset(ovInfo->sfa_base,0,307200 * sizeof (double));
	for (i = 0;i < 307200;i++) {
		ovInfo->sfa_level[i] = 1;
	}

	// FFT 初期設定
	copyToFFTW(fftw_in,pIn,fftSize);
	// 窓関数
	windowFFTW(fftw_in,fftSize);

	// FFT
	fftw_execute(fftw_p);

	// 元信号の高域のパワーを調べる
	refPw = 0;
	lowIndex = ((double)fftSize / outSampleR) * (hfc - 2000);
	highIndex = ((double)fftSize / outSampleR) * hfc;

	for (i = lowIndex;i < highIndex;i++) {
		p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
		if (p != 0) {
			p = sqrt(p);
			p = log10(p) * 20;
			p /= 3;
		}
		for (j = lowIndex;j < highIndex;j++) {
			p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
				p2 /= 3;
			}
			if (p > 0 || p2 > 0) {
				if ((long)p == (long)p2) {
					ovInfo->pw_cnt[i]++;
				}
			}
		}
	}
	for (i = lowIndex + 1,nn = lowIndex;i < highIndex;i++) {
		if (ovInfo->pw_cnt[i] > ovInfo->pw_cnt[nn]) {
			nn = i;
		}
	}
	refPw = fftw_out[nn][0] * fftw_out[nn][0] + fftw_out[nn][1] * fftw_out[nn][1];
	if (refPw > 0) {
		refPw = sqrt(refPw);
	}

	// 元信号の音のパワーを調べる(0.8.4)
	if (param->p->hfa3_version == 3) {
		lowIndex = ((double)fftSize / outSampleR) * (hfc - param->p->hfa_level_adjust_width);
		highIndex = ((double)fftSize / outSampleR) * hfc;
		avg_count = 0;
		avg_peak_count = 0;
		ovInfo->from_avg = 0;
		for (i = lowIndex;i < highIndex;i++,avg_count++) {
			p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
			if (p != 0) {
				p = sqrt(p);
				p = log10(p) * 20;
			}
			ovInfo->from_avg += p;
		}
		if (avg_count > 0) {
			ovInfo->from_avg /= avg_count;
		} else {
			ovInfo->from_avg = 0;
		}
		// ピークの検出
		for (i = lowIndex;i < highIndex;i++) {
			p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
			if (p != 0) {
				p = sqrt(p);
				p = log10(p) * 20;
			}
			p2 = fftw_out[i-1][0] * fftw_out[i-1][0] + fftw_out[i-1][1] * fftw_out[i-1][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
			}
			p3 = fftw_out[i+1][0] * fftw_out[i+1][0] + fftw_out[i+1][1] * fftw_out[i+1][1];
			if (p3 != 0) {
				p3 = sqrt(p3);
				p3 = log10(p3) * 20;
			}
			if (ovInfo->from_top10[0] < p) {
				ovInfo->from_top10[0] = p;
			}
			if ((p - p2) > 0) {
				ovInfo->peak[i] = 1;
			} else if ((p2 - p) > 0) {
				ovInfo->peak[i] = -1;
			}
			if ((p - p3) > 0 && ovInfo->peak[i] == 1) {
				ovInfo->peak[i] = 0;
			} else if ((p3 - p) > 0 && ovInfo->peak[i] == -1) {
				ovInfo->peak[i] = 0;
			}
		}
		// ピークのTop10の調査(後で音量調節に使う)
		for (i = lowIndex;i < highIndex;i++) {
			if (ovInfo->peak[i] == 0) {
				if (ovInfo->peak[i - 1] == 1 && ovInfo->peak[i + 1] == -1) {
					p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
					if (p != 0) {
						p = sqrt(p);
						p = log10(p) * 20;
					}
					for (j = 10;j > 0;j--) {
						if (p <= ovInfo->from_top10[j - 1]) {
							break;
						}
					}
					if (j < 10) {
						ovInfo->from_top10[j] = p;
					}
				}
			}
		}
		from_avg_top10 = 0;
		from_avg_top10_count = 0;
		for (i = 0;i < 10;i++) {
			if ((int)ovInfo->from_top10[i] > 0) {
				from_avg_top10 += ovInfo->from_top10[i];
				from_avg_top10_count++;
			}
		}
		if (from_avg_top10_count > 0) {
			from_avg_top10 /= from_avg_top10_count;
		} else {
			from_avg_top10 = 0;
		}
	}

	for (i = 1;i < fftSize / 2;i++) {
		p = (fftw_out[i][0] * fftw_out[i][0]) + (fftw_out[i][1] * fftw_out[i][1]);
		if (p != 0) {
			p = sqrt(p);
		}
		if (fftw_out[i][0] != 0 && fftw_out[i][1] != 0) {
			phaseTemp = atan2(fftw_out[i][1],fftw_out[i][0]) * 180 / M_PI;
			phaseTemp += 180;
		} else {
			phaseTemp = 0;
		}
		ovInfo->phase[i] = phaseTemp;
		ovInfo->power[i] = p;
		ovInfo->power_org[i] = p;
	}
	if (param->p->hfa == 2) {
		anaOverToneHFA2(ovInfo,param);
	} else if (param->p->hfa == 3) {
		anaOverToneHFA3(ovInfo,param);
	} else if (param->p->hfa == 4) {
		anaOverToneHFA4(ovInfo,param);
	}

	//
	// 信号生成
	for (i = 1;i < fftSize / 2;i++) {
		if (ovInfo->pw[i] > 0) {
			d = ovInfo->phase[i];
			if (d < 0 || d >= 360) {
				d = 0;
			}
			if (ovInfo->do2Idx[d] == -1) {
				for (j = 1;j < fftSize / 2;j++) {
					if (ovInfo->phaseX[j] != 0 && ovInfo->phaseY[j] != 0) {
						phaseTemp = atan2(ovInfo->phaseY[j],ovInfo->phaseX[j]) * 180 / M_PI;
						phaseTemp += 180;
					} else {
						phaseTemp = 0;
					}
					d2 = phaseTemp;
					if (d == d2) {
						break;
					}
				}
			} else {
				j = ovInfo->do2Idx[d];
			}
			if (j < fftSize / 2) {
				// 位相が一致
				fftw_out[i][0] = ovInfo->phaseX[j];
				fftw_out[i][1] = ovInfo->phaseY[j];
				ovInfo->do2Idx[d] = j;
			} else {
				ovInfo->pw[i] = 0;
			}
		}
	}
	overToneNotFound = 1;
	for (i = 1;i < fftSize / 2;i++) {
		if (ovInfo->pw[i] > 0) {
			fftw_out[i][0] *= ovInfo->pw[i];
			fftw_out[i][1] *= ovInfo->pw[i];
			overToneNotFound = 0;
		} else {
			fftw_out[i][0] = 0;
			fftw_out[i][1] = 0;
		}
	}

	memset(ovInfo->pw_cnt,0,307200 * sizeof (int));
	lowIndex = ((double)fftSize / outSampleR) * (hfc);
	highIndex = ((double)fftSize / outSampleR) * (hfc + 2000);
	for (i = lowIndex;i < highIndex;i++) {
		p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
		if (p != 0) {
			p = sqrt(p);
			p = log10(p) * 20;
			p /= 3;
		}

		for (j = lowIndex;j < highIndex;j++) {
			p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
				p2 /= 3;
			}
			if (p > 0 || p2 > 0) {
				if ((long)p == (long)p2) {
					ovInfo->pw_cnt[i]++;
				}
			}
		}
	}
	for (i = lowIndex + 1,nn = lowIndex;i < highIndex;i++) {
		if (ovInfo->pw_cnt[i] > ovInfo->pw_cnt[nn]) {
			nn = i;
		}
	}
	ovTonePw = fftw_out[nn][0] * fftw_out[nn][0] + fftw_out[nn][1] * fftw_out[nn][1];
	if (ovTonePw > 0) {
		ovTonePw = sqrt(ovTonePw);
	} else {
		ovTonePw = 1;
	}
	if (overToneNotFound == 0) {
		// 付加する信号のパワーを調べる
		nx = refPw / ovTonePw;
		nx *= 0.9;

		for (i = 1;i < fftSize / 2;i++) {
			fftw_out[i][0] *= nx;
			fftw_out[i][1] *= nx;
		}
		adjPinkFilter(2,fftSize,fftw_out,param);

		if (param->p->hfa3_version == 3) {
			// 付加信号の音のパワーを調べる(0.8.4)
			lowIndex = ((double)fftSize / outSampleR) * hfc;
			highIndex = ((double)fftSize / outSampleR) * (hfc + param->p->hfa_level_adjust_width);
			avg_count = 0;
			ovInfo->to_avg = 0;
			for (i = lowIndex;i < highIndex;i++,avg_count++) {
				p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
				if (p != 0) {
					p = sqrt(p);
					p = log10(p) * 20;
				}
				ovInfo->to_avg += p;
			}
			if (avg_count > 0) {
				ovInfo->to_avg /= avg_count;
			} else {
				ovInfo->to_avg = 0;
			}
			nx = ovInfo->from_avg / ovInfo->to_avg;
			nx = nx * hfa_nx;
			if (param->p->hfa3_param_test[0] == 0) {
				for (i = 1;i < fftSize / 2;i++) {
					fftw_out[i][0] *= nx;
					fftw_out[i][1] *= nx;
				}
			}
			// ピークの検出
			for (i = lowIndex;i < highIndex;i++) {
				p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
				if (p != 0) {
					p = sqrt(p);
					p = log10(p) * 20;
				}
				p2 = fftw_out[i-1][0] * fftw_out[i-1][0] + fftw_out[i-1][1] * fftw_out[i-1][1];
				if (p2 != 0) {
					p2 = sqrt(p2);
					p2 = log10(p2) * 20;
				}
				p3 = fftw_out[i+1][0] * fftw_out[i+1][0] + fftw_out[i+1][1] * fftw_out[i+1][1];
				if (p3 != 0) {
					p3 = sqrt(p3);
					p3 = log10(p3) * 20;
				}
				if (ovInfo->to_top10[0] < p) {
					ovInfo->to_top10[0] = p;
				}
				if ((p - p2) > 0) {
					ovInfo->peak[i] = 1;
				} else if ((p2 - p) > 0) {
					ovInfo->peak[i] = -1;
				}
				if ((p - p3) > 0 && ovInfo->peak[i] == 1) {
					ovInfo->peak[i] = 0;
				} else if ((p3 - p) > 0 && ovInfo->peak[i] == -1) {
					ovInfo->peak[i] = 0;
				}
			}
			// ピークのTop10の調査(後で音量調節に使う)
			for (i = lowIndex;i < highIndex;i++) {
				if (ovInfo->peak[i] == 0) {
					if (ovInfo->peak[i - 1] == 1 && ovInfo->peak[i + 1] == -1) {
						p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
						if (p != 0) {
							p = sqrt(p);
							p = log10(p) * 20;
						}
						for (j = 10;j > 0;j--) {
							if (p <= ovInfo->to_top10[j - 1]) {
								break;
							}
						}
						if (j < 10) {
							ovInfo->to_top10[j] = p;
						}
					}
				}
			}
			to_avg_top10 = 0;
			to_avg_top10_count = 0;
			for (i = 0;i < 10;i++) {
				if ((int)ovInfo->to_top10[i] > 0) {
					to_avg_top10 += ovInfo->to_top10[i];
					to_avg_top10_count++;
				}
			}
			if (to_avg_top10_count > 0) {
				to_avg_top10 /= to_avg_top10_count;
			} else {
				to_avg_top10 = 0;
			}
			if (param->p->hfa3_sig_level_limit != 0 && param->p->hfa3_sig_level_limit < hfc && ((int)from_avg_top10 > 0 && (int)to_avg_top10 > 0) && from_avg_top10 < to_avg_top10) {
				int nx_flag;
				for (nx = 1.00;nx > 0.65;nx -= 0.05) {
					nx_flag = 0;
					for (i = 1;i < fftSize / 2;i++) {
						p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
						if (p != 0) {
							p = sqrt(p);
							p = log10(p) * 20;
						}
						if (p > 0 && (p * nx) > from_avg_top10) {
							nx_flag = 1;
							break;
						}
					}
					if (nx_flag == 0) break;
				}
				if (nx < 1.00) {
					char sss[100];
					for (i = 1;i < fftSize / 2;i++) {
						fftw_out[i][0] *= nx;
						fftw_out[i][1] *= nx;
					}
				}
			}
		}

		if (param->enable_sfa == 1) {
			// SFA 用の音生成時は強い音は出さないようにする。
			lowIndex = ((double)fftSize / outSampleR) * 4000;
			highIndex = ((double)fftSize / outSampleR) * (hfc);
			workCount = ((double)fftSize / outSampleR) * (400);
			do {
				ovTonePw = 0;
				for (i = lowIndex,nn = 0;nn < workCount && i + nn < highIndex;i++,nn++) {
					p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
					if (p != 0) {
						p = sqrt(p);
					}
					ovTonePw += p;
				}
				if (nn > 0) {
					ovTonePw /= nn;
				}
				for (i = lowIndex,nn = 0;nn < workCount && i + nn < highIndex;i++,nn++) {
					ovInfo->sfa_base[i] = ovTonePw;
				}
				lowIndex += workCount;
			} while (lowIndex < highIndex);

			for (i = lowIndex;i < highIndex;i++) {
				p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
				if (p != 0) {
					p = sqrt(p);
				}
				if (p != 0 && p * 1.1 > ovInfo->sfa_base[i]) {
					nx = (ovInfo->sfa_base[i] / p);
					fftw_out[i][0] = fftw_out[i][0] * nx;
					fftw_out[i][1] = fftw_out[i][1] * nx;
				}
			}
		}

		if (param->enable_sfa == 0) {
			lowIndex = ((double)fftSize / outSampleR) * (hfc);
			for (i = 1;i < lowIndex;i++) {
				ovInfo->sfa_level[i] = 1;
			}
			for (;i < fftSize / 2;i++) {
				ovInfo->sfa_level[i] = 0;
			}
		} else {
			lowIndex = ((double)fftSize / outSampleR) * (4000);
			for (i = 1;i < lowIndex;i++) {
				ovInfo->sfa_level[i] = 1;
			}
			highIndex = ((double)fftSize / outSampleR) * (outSampleR / 2);
			for (;i < fftSize / 2;i++) {
				ovInfo->sfa_level[i] = 0;
			}
		}

		// 高域補間時のカットオフ周波数付近を調べる
		lowIndex = ((double)fftSize / outSampleR) * hfc;
		highIndex = ((double)fftSize / outSampleR) * (hfc + 1000);
		adj_avg = 0;
		adj_peak = 0;
		avg_count = 0;
		for (j = lowIndex;j < highIndex;j++) {
			p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
			}
			adj_avg += p2;
			avg_count++;
		}
		if (avg_count > 0) {
			adj_avg /= avg_count;
		}
		for (j = lowIndex;j < highIndex;j++) {
			p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
			}
			if (adj_peak < p2) {
				adj_peak = p2;
			}
		}
		if (adj_avg < adj_peak) {
			if ((adj_peak - adj_avg) > DIFF_PW_CUT_THRESH) {
				double nx = adj_avg / adj_peak;
				for (j = lowIndex;j < highIndex;j++) {
					p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
					if (p2 != 0) {
						p2 = sqrt(p2);
						p2 = log10(p2) * 20;
					}
					if (adj_avg + 20 < p2) {
						fftw_out[j][0] = fftw_out[j][0] * nx;
						fftw_out[j][1] = fftw_out[j][1] * nx;
					}
				}
			}
		}

		// ナイキスト周波数付近のカットオフ周波数付近を調べる
		lowIndex  = ((double)fftSize / outSampleR) * ((param->p->outSampleR_final / 2) - 1000);
		highIndex  = ((double)fftSize / outSampleR) * ((param->p->outSampleR_final / 2) - 1);
		adj_avg = 0;
		adj_peak = 0;
		avg_count = 0;
		for (j = lowIndex;j < highIndex;j++) {
			p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
			}
			adj_avg += p2;
			avg_count++;
		}
		if (avg_count > 0) {
			adj_avg /= avg_count;
		}
		for (j = lowIndex;j < highIndex;j++) {
			p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
			}
			if (adj_peak < p2) {
				adj_peak = p2;
			}
		}
		if (adj_avg < adj_peak) {
			if ((adj_peak - adj_avg) > DIFF_PW_CUT_THRESH) {
				double nx = adj_avg / adj_peak;
				for (j = lowIndex;j < highIndex;j++) {
					p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
					if (p2 != 0) {
						p2 = sqrt(p2);
						p2 = log10(p2) * 20;
					}
					if (adj_avg + 20 < p2) {
						fftw_out[j][0] = fftw_out[j][0] * nx;
						fftw_out[j][1] = fftw_out[j][1] * nx;
					}
				}
			}
		}

		// 高域補間時のカットオフ周波数付近のパワーを調べる
		lowIndex = ((double)fftSize / outSampleR) * hfc;
		highIndex = ((double)fftSize / outSampleR) * (hfc + 1000);
		adj_avg = 0;
		avg_count = 0;
		for (j = lowIndex;j < highIndex;j++) {
			p2 = fftw_out[j][0] * fftw_out[j][0] + fftw_out[j][1] * fftw_out[j][1];
			if (p2 != 0) {
				p2 = sqrt(p2);
				p2 = log10(p2) * 20;
			}
			adj_avg += p2;
			avg_count++;
		}
		if (avg_count > 0) {
			adj_avg /= avg_count;
		}
		if (param->upconv == 2 && ovInfo->index == 1 && ovInfo->startInSample / outSampleR > 80) {
			char sss[200];
			sprintf(sss,"genOverTonePw: sec:%lld,%lf",ovInfo->startInSample / outSampleR,adj_avg);
			PRINT_LOG("HFA4",sss);
		}
		if (adj_avg < 100) {
			// 指定パワー以下はカットする
			for (j = lowIndex;j < fftSize / 2;j++) {
				fftw_out[j][0] = 0;
				fftw_out[j][1] = 0;;
			}
		}

		// 付加する音以外カット
		for (i = 1;i < fftSize / 2;i++) {
			if (ovInfo->sfa_level[i] == 1) {
				fftw_out[i][0] = 0;
				fftw_out[i][1] = 0;
			}
		}

		// 半分のデータを復元
		for (i = 1;i < fftSize / 2;i++) {
			fftw_out[fftSize - i][0] = fftw_out[i][0];
			fftw_out[fftSize - i][1] = fftw_out[i][1] * -1;
		}

		// 直流成分を削除
		fftw_out[0][0] = 0;
		fftw_out[0][1] = 0;

		// invert FFT
		fftw_execute(fftw_ip);

		// 窓関数
		for (i = 0;i < (fftSize - 1) / 2;i++) {
			fftw_in[i][0] = fftw_in[i][0] * (2.0 * i / (double)fftSize);
		}
		for (i = (fftSize - 1) / 2;i < fftSize;i++) {
			fftw_in[i][0] = fftw_in[i][0] * (2.0 - 2.0 * i / (double)fftSize);
		}
	} else {
		// 無音生成
		for (i = 0;i < fftSize;i++) {
			fftw_in[i][0] = 0;
		}
	}
	// 出力
	for (i = 0;i < fftSize;i++) {
		pOut[i] += (fftw_in[i][0] / fftSize);
	}
}

//---------------------------------------------------------------------------
// Function   : anaOverToneHFA2
// Description: 倍音解析
//				window幅ごとにpowerの平均値をとりpowerが大きいものを抽出する
// ---
//	ovInfo :倍音生成用構造体
//	param  :パラメータ
//
void anaOverToneHFA2(OVERTONE_INFO *ovInfo,PARAM_INFO *param)
{
	DWORD ofs,window,width,swidth;
	DWORD n,i;
	double avg,maxAvg;
	double avgLine;
	int maxOfs,maxWin;
	
	
	int lowIndex,highIndex;
	int pha;
	
	
	long nSample;
	long lowHz,wid;
	long skipCnt;
	nSample = ovInfo->nSample;

	PRINT_FN_LOG(param->debug_upconv,"-anaOverToneHFA2:Start");

	//
	// 計算対象のインデックスを求める
	for (i = 1;i < ovInfo->nSample;i++) {
		ovInfo->pw[i] = 0;
	}

	lowHz = 9000;
	wid   = 2500;
	if (lowHz + wid > ovInfo->validSamplingRate) {
		lowHz = ovInfo->validSamplingRate - wid;
	}
	if (lowHz < 4000) {
		// 高域の情報がないので解析しない
		return;
	}

	swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 200;
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
	highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	if (swidth == 0) {
		swidth = 1;
	}

	// パワーの平均を計算する
	//
	avg = 0;
	for (i = lowIndex,n = 0;i < highIndex;i++) {
		avg += ovInfo->power[i];
		n++;
	}
	if (n > 0) {
		avg /= n;
	}
	avgLine = avg;
	ofs = lowIndex;
	//
	// powerが強いものを採用する(sig1)
	if (param->p->sig1Enb == 1) {
		window = swidth;
		maxOfs = ofs;
		maxWin = swidth;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			maxAvg = -1;
			skipCnt = 0;
			for (window = swidth;window < width;window++) {
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 4) != 0) {
					continue;
				}
				avg = 0;
				for (i = ofs,n = 0;i < highIndex;i += window) {
					avg += ovInfo->power[i];
					n++;
				}
				if (n > 0) {
					avg /= n;
				}

				if (maxAvg == -1 || maxAvg < avg) {
					maxAvg = avg;
					maxOfs = ofs;
					maxWin = window;
				}

			}
			if (avgLine * param->p->sig1AvgLineNx < maxAvg) {
				pha = ovInfo->phase[maxOfs];
				for (i = maxOfs,n = 1;i < nSample;i += maxWin,n++) {
					if (ovInfo->pw[i] < maxAvg / n) {
						ovInfo->pw[i] = maxAvg / n;
						ovInfo->phase[i] = pha;

						pha += param->p->sig1Phase;
						if (pha >= 360) {
							pha -= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
					}
				}
			}
		}
	}

	//
	// window間隔の前後のpowerの差の累積が少ないものを採用する(sig2)
	if (param->p->sig2Enb == 1) {
		for (ofs = lowIndex;ofs < lowIndex + width;ofs++) {
			maxAvg = -1;
			skipCnt = 0;
			for (window = swidth;window < width;window++) {
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 4) != 0) {
					continue;
				}
				avg = 0;
				for (i = ofs,n = 0;i < highIndex;i += window) {
					if (i >= window) {
						if (1 + window < i) {
							if (ovInfo->power[i - window] <= ovInfo->power[i]) {
								avg += ovInfo->power[i] - ovInfo->power[i - window];
							} else {
								avg += ovInfo->power[i] - ovInfo->power[i - window];
							}
							n++;
						}
					}
				}
				if (n > 0) {
					avg /= n;
				}

				if (maxAvg == -1 || maxAvg > avg) {
					maxAvg = avg;
					maxOfs = ofs;
					maxWin = window;
				}

			}
			avg = 0;
			for (i = maxOfs,n = 0;i < highIndex;i += maxWin) {
				avg += ovInfo->power[i];
				n++;
			}
			if (n > 0) {
				avg /= n;
			}
			maxAvg = avg;
			pha = ovInfo->phase[maxOfs];
			for (i = maxOfs,n = 1;i < nSample;i += maxWin,n++) {
				if (ovInfo->pw[i] > maxAvg) {
					ovInfo->pw[i] = maxAvg;
					ovInfo->phase[i] = pha;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha -= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
				} else if (ovInfo->pw[i] == 0) {
					ovInfo->pw[i] = maxAvg / 10;
					ovInfo->phase[i] = pha;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha -= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
				}
			}
		}
	}
	PRINT_LOG(param->debug_upconv,"-anaOverToneHFA2:End");
}
#if 1	// 0.7 の HFA3
void anaOverToneHFA3(OVERTONE_INFO *ovInfo,PARAM_INFO *param)
{
	DWORD ofs,window,width,swidth;
	DWORD validIndex;
	DWORD n,i,j,k;
	double avg;
	double avgRef;
	double avgLine;
	double diff,diff0,diff1,diff2,diff3,diff4,diff5,diffP;
	double refPw[8];
	double avgPw,avgPw2,avgPw3;
	double tmpAvgPw,tmpAvgPw2,tmpAvgPw3;
	int    avgPwNX,avgPwNX2,avgPwNX3;
	long   step;
	long skipCnt;
	double tbl_hfaDiffMin[5] = {0.84,1.04,1.24,1.74,2.14};
	// 予測との最小パラメータ保存
	int minWin;
	int minType;
	int max_i;
	double minDiff;
	int nn;
	int odd;
	double hz;
	DWORD  baseOfs;
	double tmpPw,tmpPw2;
	int lowIndex,highIndex;
	int lowRange,highRange;
	int minWidth;
	int pha;
	double phaRand;
	int phaTmp = 0;
	long nSample;
	long lowHz,wid;
	double areaAvg;
	nSample = ovInfo->nSample;

	PRINT_LOG(param->debug_upconv,"-anaOverToneHFA3:Start");

	//
	// 初期化
	for (i = 1;i < ovInfo->nSample;i++) {
		ovInfo->pw[i] = 0;
		ovInfo->diff[i] = -1;
		ovInfo->base[i] = 0;
		ovInfo->baseToAdj[i] = 0;
		ovInfo->sign[i] = 0;
	}
	swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1600;

	ovInfo->power_ptr = ovInfo->power;
	if (param->p->hfa3_version == 3) {
		ovInfo->power_ptr = ovInfo->power_org;
	}

#if 1
	for (i = 1;i < ovInfo->nSample;i+= swidth) {
		avg = 0;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			avg += ovInfo->power_ptr[j];
		}
		avg /= n;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			ovInfo->baseToAdj[j] = ovInfo->power[j] - avg;
			ovInfo->power[j] -= ovInfo->baseToAdj[j];
			ovInfo->base[j] = avg;
		}
	}
#endif
//	if (ovInfo->validSamplingRate < 8000) {
	if (ovInfo->validSamplingRate < 8000) {
		// 高域の情報がないので解析しない
		return;
	}

	if (param->p->hfa3_version == 1) {
		lowHz	= 8000;
		wid		= 3000;
		step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;	// 20181007
		if (lowHz + wid  >= ovInfo->validSamplingRate) {
			wid = ovInfo->validSamplingRate - lowHz;
		}
	} else if (param->p->hfa3_version == 2) {
		lowHz	= 8000;
		wid		= 3000;
		if (lowHz + wid  >= ovInfo->validSamplingRate) {
			wid = ovInfo->validSamplingRate - lowHz;
		}
		step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 221;	// 20191024
		step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 248;	// 20191024
		step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 209;	// 20191024
		step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 257;	// 20191024
		step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 273;	// 20191024
	} else if (param->p->hfa3_version == 3) {
		if (param->p->hfa3_param_test[1] == 0) {
			lowHz	= 4000;
			wid		= 3000;
		} else {
			lowHz	= 8000;
			wid		= 3000;
			lowHz = param->p->hfa3_freq_start;
			wid   = param->p->hfa3_freq_len;
		}
		step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
		if (lowHz + wid  >= ovInfo->validSamplingRate) {
			wid = ovInfo->validSamplingRate - lowHz;
			if (wid < 1500) {
				wid = 1500;
				lowHz = ovInfo->validSamplingRate - wid;
			}
		}
	}
	if (param->debug_print_disable == 0) {
		char s[100];
		sprintf(s,"hfa3_freq_start:%d",lowHz);
		PRINT_LOG("",s);
		sprintf(s,"hfa3_freq_len:%d",wid);
		PRINT_LOG("",s);
		sprintf(s,"step:%d",param->p->hfa3_analyze_step);
		PRINT_LOG("",s);
		param->debug_print_disable = 1;
	}

	swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 190;
	if (param->p->hfa3_version == 1) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else if (param->p->hfa3_version == 2) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	}
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
	minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
	if (ovInfo->validSamplingRate < 16000) {
char s[100];
sprintf(s,"validSampingRate:%d",ovInfo->validSamplingRate);
if (ovInfo->hfa3_log == 0) {
	PRINT_LOG("",s);
}
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	} else {
char s[100];
sprintf(s,"validSampingRate:%d",ovInfo->validSamplingRate);
if (ovInfo->hfa3_log == 0) {
	PRINT_LOG("",s);
}
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
	}
	if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
char s[100];
sprintf(s,"hfa_analysis_limit:%d",param->p->hfa_analysis_limit);
if (ovInfo->hfa3_log == 0) {
	PRINT_LOG("",s);
}
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
	}

	if (swidth == 0) {
		swidth = 1;
	}
	if (step == 0) {
		step = 1;
	}
	avg = 0;
	for (i = lowIndex,n = 0;i < highIndex;i++) {
		avg += ovInfo->power[i];
		n++;
	}
	if (n > 0) {
		avg /= n;
	}
	avgLine = avg;

	if (param->p->sig2Enb == 1) {
		if (ovInfo->hfa3_log == 0) {
			PRINT_LOG("","param->sig2Enb == 1");
		}
		// 前後のwindowで振幅の差が少ない音声の補間
		window = width;
		minWin = window;
		minType = 0;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;

			if (param->p->hfa3_version == 1) {
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;	// 20181007
			} else if (param->p->hfa3_version == 2) {
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 221;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 248;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 209;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 257;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 273;	// 20191024
			} else {
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
			}

			for (window = swidth;window < width;window+=step) {
//				step = 1;
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}

				// スペクトル成分の遷移を調べる
				diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;	// 奇数倍のみ倍音がある
				refPw[0] = -1;
				refPw[1] = -1;
				refPw[2] = -1;
				refPw[3] = -1;
				refPw[4] = -1;
				refPw[5] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				ovInfo->sign[baseOfs] = 0;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						refPw[1] = ovInfo->power[i] * n;
						refPw[2] = ovInfo->power[i] * odd;
						refPw[3] = ovInfo->power[i] * (odd * odd);
						refPw[4] = ovInfo->power[i];
						refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					}
					// 平均より大きい音声か、小さい音声か
					if (ovInfo->power[i] > ovInfo->base[i]) {
						ovInfo->sign[baseOfs]++;
					} else if (ovInfo->power[i] < ovInfo->base[i]) {
						ovInfo->sign[baseOfs]--;
					}
					
					// 前後のパワーの差の計算
					if (i >= window) {
						if (i - window >= ofs) {
							if (ovInfo->power[i - window] >= ovInfo->power[i]) {
								diff = ovInfo->power[i - window] - ovInfo->power[i];
							} else {
								diff = ovInfo->power[i] - ovInfo->power[i - window];
							}
						}
					} else {
						continue;
					}
					diffP += (diff * tbl_hfaDiffMin[param->hfaDiffMin - 1]);

					avgPw += ovInfo->power[i];
					avgPwNX++;
					if ((avgPwNX & 0x01) == 0) {
						avgPw2 += ovInfo->power[i];
						avgPwNX2++;
					}
					if ((avgPwNX % 3) == 0) {
						avgPw3 += ovInfo->power[i];
						avgPwNX3++;
					}
					
					// 1/f(振幅が1/fになっているもの)
					diff = refPw[0] / hz;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff0 += diff;

					// 鋸波(nの逆数で小さくなる)
					diff = refPw[1] / n;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff1 += diff;

					// 短形波(奇数倍音,nの逆数で小さくなる)
					diff = refPw[2] / odd;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff2 += diff;

					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					diff = refPw[3] / (odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff3 += diff;

					// パルス(n番目の倍音でもパワーは同じ)
					diff = refPw[4];
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff4 += diff;

					// その他(もっとパワーが小さくなるパターン)
					diff = refPw[5] / (odd * odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff5 += diff;

					nn++;
				}

//				diff0 += diffP * 2;
//				diff1 += diffP * 2;
//				diff2 += diffP * 2;
//				diff3 += diffP * 2;
//				diff4 += diffP * 2;
//				diff5 += diffP * 2;

				if (nn > 0) {
					diff0 /= nn;
					diff1 /= nn;
					diff2 /= nn;
					diff3 /= nn;
					diff4 /= nn;
					diff5 /= nn;
					diffP /= nn;
//					if (refPw[4] > 0) {
//						diff0 /= refPw[4];
//						diff1 /= refPw[4];
//						diff2 /= refPw[4];
//						diff3 /= refPw[4];
//						diff4 /= refPw[4];
//						diff5 /= refPw[4];
//						diffP /= refPw[4];
//					}
				}

				if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
					tmpAvgPw  = avgPw / avgPwNX;
					tmpAvgPw2 = avgPw2 / avgPwNX2;
					tmpAvgPw3 = avgPw3 / avgPwNX3;
					if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
					}
				}

				if (minDiff == -1 || minDiff > diffP) {
					minDiff = diffP;
					minWin = window;
					minType = 0;
				}
				if (minDiff > diff1) {
					minDiff = diff1;
					minWin = window;
					minType = 1;
				}
				if (minDiff > diff2) {
					minDiff = diff2;
					minWin = window;
					minType = 2;
				}
				if (minDiff > diff3) {
					minDiff = diff3;
					minWin = window;
					minType = 3;
				}
				if (minDiff > diff4) {
					minDiff = diff4;
					minWin = window;
					minType = 4;
				}
				if (minDiff > diff5) {
					minDiff = diff5;
					minWin = window;
					minType = 5;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある
			odd = 1;
			refPw[0] = -1;
			refPw[4] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}

			if (param->hfa3_sig2_print_disable == 0) {
				param->hfa3_sig2_print_disable = 1;
				PRINT_LOG("","hfa3 sig2 output");
			}
			for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[i] * hz;
					refPw[1] = ovInfo->power[i] * n;
					refPw[2] = ovInfo->power[i] * odd;
					refPw[3] = ovInfo->power[i] * (odd * odd);
					refPw[4] = ovInfo->power[i];
					refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				if (minType == 0) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
					}
				} else if (minType == 1) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[1] / n;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
					}
				} else if (minType == 2) {
					// 短形波(奇数倍音,nの逆数で小さくなる)
					tmpPw = refPw[2] / odd;
					phaRand = 1;
					pha = ovInfo->phase[max_i];
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
					}
				} else if (minType == 3) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
					}
				} else if (minType == 4) {
					// パルス(n番目の倍音でもパワーは同じ)
					tmpPw = refPw[4];
					phaRand = rand() * 6;
					phaRand -= 3;
					pha += phaRand;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.18;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.18;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.18;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.18;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
					}
				} else if (minType == 5) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
					}
				}
			}
		}
		if (param->p->hfa3_version == 3 && param->p->hfa3_param_test[1] == 0) {
			param->debug_print_disable = 0;
			if (param->p->hfa3_freq_start != 0) {
				lowHz = param->p->hfa3_freq_start;
			}
			if (param->p->hfa3_freq_len != 0) {
				wid   = param->p->hfa3_freq_len;
			}
			if (lowHz + wid  >= ovInfo->validSamplingRate) {
				wid = ovInfo->validSamplingRate - lowHz;
				if (wid < 1500) {
					wid = 1500;
					lowHz = ovInfo->validSamplingRate - wid;
				}
			}
			step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
			if (param->debug_print_disable == 0) {
				char s[100];
				sprintf(s,"2nd hfa3_freq_start:%d",lowHz);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				sprintf(s,"2nd hfa3_freq_len:%d",wid);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				sprintf(s,"2nd step:%d",param->p->hfa3_analyze_step);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				param->debug_print_disable = 1;
			}

			swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
			width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
			lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
			minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
			if (ovInfo->validSamplingRate < 16000) {
				char s[100];
				sprintf(s,"validSampingRate:%d",ovInfo->validSamplingRate);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
			} else {
				char s[100];
				sprintf(s,"validSampingRate:%d",ovInfo->validSamplingRate);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
			}
			if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
				char s[100];
				sprintf(s,"hfa_analysis_limit:%d",param->p->hfa_analysis_limit);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
			}

			if (swidth == 0) {
				swidth = 1;
			}
			if (step == 0) {
				step = 1;
			}
			avg = 0;
			for (i = lowIndex,n = 0;i < highIndex;i++) {
				avg += ovInfo->power[i];
				n++;
			}
			if (n > 0) {
				avg /= n;
			}
			avgLine = avg;


			// 2nd
			if (ovInfo->hfa3_log == 0) {
				PRINT_LOG("","param->sig2Enb == 1");
			}
			// 前後のwindowで振幅の差が少ない音声の補間
			window = width;
			minWin = window;
			minType = 0;

			for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
				minDiff = -1;
				skipCnt = 0;

				if (param->p->hfa3_version == 1) {
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;	// 20181007
				} else if (param->p->hfa3_version == 2) {
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 221;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 248;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 209;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 257;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 273;	// 20191024
				} else {
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
				}

				for (window = swidth;window < width;window+=step) {
	//				step = 1;
					if (window < minWidth) {
						if ((ofs - lowIndex) > window * 1) {
							continue;
						}
					} else {
						if ((ofs - lowIndex) > window) {
							continue;
						}
					}
					skipCnt++;
					if (param->p->hfaFast && (skipCnt % 8) != 0) {
						continue;
					}

					// スペクトル成分の遷移を調べる
					diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
					baseOfs = ofs - ((ofs / window) * window);
					if (baseOfs == 0) {
						baseOfs = window;
					}
					n = 1;		// 奇数偶数倍すべてに倍音がある
					odd = 1;	// 奇数倍のみ倍音がある
					refPw[0] = -1;
					refPw[1] = -1;
					refPw[2] = -1;
					refPw[3] = -1;
					refPw[4] = -1;
					refPw[5] = -1;
					avgPw  = 0;
					avgPw2 = 0;
					avgPw3 = 0;
					avgPwNX  = 0;
					avgPwNX2 = 0;
					avgPwNX3 = 0;
					ovInfo->sign[baseOfs] = 0;
					for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
						hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
						if (hz < lowHz) {
							continue;
						}

						if (refPw[0] == -1) {
							refPw[0] = ovInfo->power[i] * hz;
							refPw[1] = ovInfo->power[i] * n;
							refPw[2] = ovInfo->power[i] * odd;
							refPw[3] = ovInfo->power[i] * (odd * odd);
							refPw[4] = ovInfo->power[i];
							refPw[5] = ovInfo->power[i] * (odd * odd * odd);
						}
						// 平均より大きい音声か、小さい音声か
						if (ovInfo->power[i] > ovInfo->base[i]) {
							ovInfo->sign[baseOfs]++;
						} else if (ovInfo->power[i] < ovInfo->base[i]) {
							ovInfo->sign[baseOfs]--;
						}
						
						// 前後のパワーの差の計算
						if (i >= window) {
							if (i - window >= ofs) {
								if (ovInfo->power[i - window] >= ovInfo->power[i]) {
									diff = ovInfo->power[i - window] - ovInfo->power[i];
								} else {
									diff = ovInfo->power[i] - ovInfo->power[i - window];
								}
							}
						} else {
							continue;
						}
						diffP += (diff * tbl_hfaDiffMin[param->hfaDiffMin - 1]);

						avgPw += ovInfo->power[i];
						avgPwNX++;
						if ((avgPwNX & 0x01) == 0) {
							avgPw2 += ovInfo->power[i];
							avgPwNX2++;
						}
						if ((avgPwNX % 3) == 0) {
							avgPw3 += ovInfo->power[i];
							avgPwNX3++;
						}
						
						// 1/f(振幅が1/fになっているもの)
						diff = refPw[0] / hz;
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff0 += diff;

						// 鋸波(nの逆数で小さくなる)
						diff = refPw[1] / n;
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff1 += diff;

						// 短形波(奇数倍音,nの逆数で小さくなる)
						diff = refPw[2] / odd;
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff2 += diff;

						// 三角波(奇数倍音,n^2の逆数で小さくなる)
						diff = refPw[3] / (odd * odd);
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff3 += diff;

						// パルス(n番目の倍音でもパワーは同じ)
						diff = refPw[4];
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff4 += diff;

						// その他(もっとパワーが小さくなるパターン)
						diff = refPw[5] / (odd * odd * odd);
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff5 += diff;

						nn++;
					}

	//				diff0 += diffP * 2;
	//				diff1 += diffP * 2;
	//				diff2 += diffP * 2;
	//				diff3 += diffP * 2;
	//				diff4 += diffP * 2;
	//				diff5 += diffP * 2;

					if (nn > 0) {
						diff0 /= nn;
						diff1 /= nn;
						diff2 /= nn;
						diff3 /= nn;
						diff4 /= nn;
						diff5 /= nn;
						diffP /= nn;
	//					if (refPw[4] > 0) {
	//						diff0 /= refPw[4];
	//						diff1 /= refPw[4];
	//						diff2 /= refPw[4];
	//						diff3 /= refPw[4];
	//						diff4 /= refPw[4];
	//						diff5 /= refPw[4];
	//						diffP /= refPw[4];
	//					}
					}

					if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
						tmpAvgPw  = avgPw / avgPwNX;
						tmpAvgPw2 = avgPw2 / avgPwNX2;
						tmpAvgPw3 = avgPw3 / avgPwNX3;
						if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
						}
					}

					if (minDiff == -1 || minDiff > diffP) {
						minDiff = diffP;
						minWin = window;
						minType = 0;
					}
					if (minDiff > diff1) {
						minDiff = diff1;
						minWin = window;
						minType = 1;
					}
					if (minDiff > diff2) {
						minDiff = diff2;
						minWin = window;
						minType = 2;
					}
					if (minDiff > diff3) {
						minDiff = diff3;
						minWin = window;
						minType = 3;
					}
					if (minDiff > diff4) {
						minDiff = diff4;
						minWin = window;
						minType = 4;
					}
					if (minDiff > diff5) {
						minDiff = diff5;
						minWin = window;
						minType = 5;
					}
				}

				// 一番予測誤差が少なかったものを採用する。

				baseOfs = ofs - ((ofs / minWin) * minWin);
				if (baseOfs == 0) {
					baseOfs = minWin;
				}

				pha = ovInfo->phase[baseOfs];
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;
				refPw[0] = -1;
				refPw[4] = -1;
				if (minWin == swidth || minWin == width - 1) {
					continue;
				}

				if (param->hfa3_sig2_print_disable == 0) {
					param->hfa3_sig2_print_disable = 1;
					if (ovInfo->hfa3_log == 0) {
						PRINT_LOG("","hfa3 sig2 output");
					}
				}
				for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						refPw[1] = ovInfo->power[i] * n;
						refPw[2] = ovInfo->power[i] * odd;
						refPw[3] = ovInfo->power[i] * (odd * odd);
						refPw[4] = ovInfo->power[i];
						refPw[5] = ovInfo->power[i] * (odd * odd * odd);
						max_i = i;
						pha = ovInfo->phase[max_i];
					}

					if (minType == 0) {
						// 1/f(振幅が1/fになっているもの)
						tmpPw = refPw[0] / hz;
						phaRand = 1;
						pha += param->p->sig2Phase;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 1) {
						// 1/f(振幅が1/fになっているもの)
						tmpPw = refPw[1] / n;
						phaRand = 1;
						pha += param->p->sig2Phase;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 2) {
						// 短形波(奇数倍音,nの逆数で小さくなる)
						tmpPw = refPw[2] / odd;
						phaRand = 1;
						pha = ovInfo->phase[max_i];
						phaTmp = pha;
						if (n & 0x01) {
							phaTmp = pha + 180;
						}
						if (phaTmp >= 360) {
							phaTmp %= 360;
						}
						if (phaTmp < 0) {
							phaTmp += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 3) {
						// 三角波(奇数倍音,n^2の逆数で小さくなる)
						tmpPw = refPw[3] / (odd * odd);
						phaRand = 1;
	//					pha = ovInfo->phase[max_i];
						pha += param->p->sig2Phase;
						phaTmp = pha;
						if (n & 0x01) {
							phaTmp = pha + 180;
						}
						if (phaTmp >= 360) {
							phaTmp %= 360;
						}
						if (phaTmp < 0) {
							phaTmp += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 4) {
						// パルス(n番目の倍音でもパワーは同じ)
						tmpPw = refPw[4];
						phaRand = rand() * 6;
						phaRand -= 3;
						pha += phaRand;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.18;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.18;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.18;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.18;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 5) {
						// 三角波(奇数倍音,n^2の逆数で小さくなる)
						tmpPw = refPw[3] / (odd * odd * odd);
						phaRand = 1;
	//					pha = ovInfo->phase[max_i];
						pha += param->p->sig2Phase;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				}
			}
		}
	}

#if 1
	if (param->p->sig1Enb == 1) {
		if (ovInfo->hfa3_log == 0) {
			PRINT_LOG("","param->sig1Enb == 1");
		}
		// powerが強いもの優先で補間する
		lowHz	= 9500;
		wid		= 2500;
		if (lowHz + wid  >= ovInfo->validSamplingRate) {
			wid = ovInfo->validSamplingRate - lowHz;
		}

		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 190;
		width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
		minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
		if (ovInfo->validSamplingRate < 16000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < param->p->inSampleR / 2) {
			char s[100];
			sprintf(s,"hfa_analysis_limit:%d",param->p->hfa_analysis_limit);
			if (ovInfo->hfa3_log == 0) {
				PRINT_LOG("",s);
			}
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}

		if (swidth == 0) {
			swidth = 1;
		}

		areaAvg = 0;
		for (i = lowIndex,n = 0;n < width;i++,n++) {
			areaAvg += ovInfo->power_ptr[i];
		}
		if (n > 0) {
			areaAvg /= n;
		}

		window = width;
		minWin = window;
		minType = 0;
		if (param->p->hfa3_version < 3) {
			for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
				minDiff = -1;
				skipCnt = 0;
				if (ovInfo->power_ptr[ofs] < areaAvg) {
					continue;
				}
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 50;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 103;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;
				for (window = swidth;window < (width / 2);window+=step) {
					if (window < minWidth) {
						if ((ofs - lowIndex) > window * 1) {
							continue;
						}
					} else {
						if ((ofs - lowIndex) > window) {
							continue;
						}
					}
					skipCnt++;
					if (param->p->hfaFast && (skipCnt % 8) != 0) {
						continue;
					}
					//step = 1;
					// スペクトル成分の遷移を調べる
					diff0 = 0;
					baseOfs = ofs - ((ofs / window) * window);
					if (baseOfs == 0) {
						baseOfs = window;
					}
					n = 1;		// 奇数偶数倍すべてに倍音がある
					refPw[0] = -1;
					avgPw  = 0;
					avgPw2 = 0;
					avgPw3 = 0;
					avgPwNX  = 0;
					avgPwNX2 = 0;
					avgPwNX3 = 0;
					ovInfo->sign[baseOfs] = 0;
					for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
						hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
						if (hz < lowHz) {
							continue;
						}

						// 平均より大きい音声か、小さい音声か
						if (ovInfo->power_ptr[i] > ovInfo->base[i]) {
							ovInfo->sign[baseOfs]++;
						} else if (ovInfo->power_ptr[i] < ovInfo->base[i]) {
							ovInfo->sign[baseOfs]--;
						}

						if (i >= window) {
							if (i - window >= ofs) {
								if (ovInfo->power_ptr[i - window] < ovInfo->power_ptr[i]) {
									avgPw /= 75;
								}
							}
						} else {
							continue;
						}
						if (refPw[0] == -1) {
							refPw[0] = ovInfo->power_ptr[i] * hz;
							max_i = i;
						}
						if (ovInfo->power_ptr[i] > areaAvg) {
							avgPw += ovInfo->power_ptr[i];
						} else {
							avgPw /= 75;
						}
						avgPwNX++;
						nn++;
					}

					if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
						tmpAvgPw  = avgPw / avgPwNX;
						tmpAvgPw2 = avgPw2 / avgPwNX2;
						tmpAvgPw3 = avgPw3 / avgPwNX3;
						if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {
							continue;
						}
					}
					if (avgPwNX > 0) {
						avgPw /= avgPwNX;
					}
					if (minDiff == -1 || minDiff < avgPw) {
						minDiff = avgPw;
						minWin = window;
						minType = 0;
					}
				}

				// 一番累積のパワーが強いものを採用する。

				baseOfs = ofs - ((ofs / minWin) * minWin);
				if (baseOfs == 0) {
					baseOfs = minWin;
				}

				pha = ovInfo->phase[baseOfs];
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;
				refPw[0] = -1;
				if (minWin == swidth || minWin == width - 1) {
					continue;
				}
				for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power_ptr[i] * hz;
						max_i = i;
						pha = ovInfo->phase[max_i];
					}

					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig1Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] == 0 && ovInfo->sign[baseOfs] > 16) {
						ovInfo->pw[i] = tmpPw * 0.55;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = -1;
						if (param->hfa3_sig1_print_disable == 0) {
							param->hfa3_sig1_print_disable = 1;
							if (ovInfo->hfa3_log == 0) {
								PRINT_LOG("","hfa3_sig1 output");
							}
						}
					}
				}
			}
		} else {
			for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
				minDiff = -1;
				skipCnt = 0;
				if (ovInfo->power_ptr[ofs] < areaAvg) {
					continue;
				}
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 50;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 103;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;
				for (window = swidth;window < (width / 2);window+=step) {
					if (window < minWidth) {
						if ((ofs - lowIndex) > window * 1) {
							continue;
						}
					} else {
						if ((ofs - lowIndex) > window) {
							continue;
						}
					}
					skipCnt++;
					if (param->p->hfaFast && (skipCnt % 8) != 0) {
						continue;
					}
					//step = 1;
					// スペクトル成分の遷移を調べる
					diff0 = 0;
					baseOfs = ofs - ((ofs / window) * window);
					if (baseOfs == 0) {
						baseOfs = window;
					}
					n = 1;		// 奇数偶数倍すべてに倍音がある
					refPw[0] = -1;
					avgPw  = 0;
					avgPw2 = 0;
					avgPw3 = 0;
					avgPwNX  = 0;
					avgPwNX2 = 0;
					avgPwNX3 = 0;
					ovInfo->sign[baseOfs] = 0;
					for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
						hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
						if (hz < lowHz) {
							continue;
						}

						// 平均より大きい音声か、小さい音声か
						if (ovInfo->power_ptr[i] > ovInfo->base[i]) {
							ovInfo->sign[baseOfs]++;
						} else if (ovInfo->power_ptr[i] < ovInfo->base[i]) {
							ovInfo->sign[baseOfs]--;
						}

						if (i >= window) {
							if (i - window >= ofs) {
								if (ovInfo->power_ptr[i - window] < ovInfo->power_ptr[i]) {
									avgPw /= 75;
								}
							}
						} else {
							continue;
						}
						if (refPw[0] == -1) {
							refPw[0] = ovInfo->power_ptr[i] * hz;
							max_i = i;
						}
						if (ovInfo->power_ptr[i] > areaAvg) {
							avgPw += ovInfo->power_ptr[i];
						} else {
							avgPw /= 75;
						}
						avgPwNX++;
						nn++;
					}

					if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
						tmpAvgPw  = avgPw / avgPwNX;
						tmpAvgPw2 = avgPw2 / avgPwNX2;
						tmpAvgPw3 = avgPw3 / avgPwNX3;
						if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
						}
					}
					if (avgPwNX > 0) {
						avgPw /= avgPwNX;
					}
					if (minDiff == -1 || minDiff < avgPw) {
						minDiff = avgPw;
						minWin = window;
						minType = 0;
					}
				}

				// 一番累積のパワーが強いものを採用する。

				baseOfs = ofs - ((ofs / minWin) * minWin);
				if (baseOfs == 0) {
					baseOfs = minWin;
				}

				pha = ovInfo->phase[baseOfs];
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;
				refPw[0] = -1;
				if (minWin == swidth || minWin == width - 1) {
					continue;
				}
				for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power_ptr[i] * hz;
						max_i = i;
						pha = ovInfo->phase[max_i];
					}

					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig1Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] == 0 && ovInfo->sign[baseOfs] > 16) {
					// if (ovInfo->pw[i] == 0 || ovInfo->pw[i] < tmpPw * 0.55) {
						ovInfo->pw[i] = tmpPw * 0.55;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = -1;
						if (param->hfa3_sig1_print_disable == 0) {
							param->hfa3_sig1_print_disable = 1;
							if (ovInfo->hfa3_log == 0) {
								PRINT_LOG("","hfa3_sig1 output");
							}
						}
					}
				}
			}
		}
	}
#endif

	if (param->p->sig3Enb == 1) {
		if (ovInfo->hfa3_log == 0) {
			PRINT_LOG("","param->sig3Enb == 1");
		}
//	if (0) {
		// powerが弱いものを補間する
		lowHz	= 8000;
		wid		= 4000;
		if (lowHz + wid  >= ovInfo->validSamplingRate) {
			wid = ovInfo->validSamplingRate - lowHz;
		}

		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 180;
		width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
		minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
		if (ovInfo->validSamplingRate < 16000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < param->p->inSampleR / 2) {
			char s[100];
			sprintf(s,"hfa_analysis_limit:%d",param->p->hfa_analysis_limit);
			if (ovInfo->hfa3_log == 0) {
				PRINT_LOG("",s);
			}
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * (ovInfo->validSamplingRate + 2000);
		}

		if (swidth == 0) {
			swidth = 1;
		}

		window = width;
		minWin = swidth;

		if (param->p->hfa3_version < 3) {
			for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
				minDiff = -1;
				skipCnt = 0;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 50;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 66;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;
				for (window = swidth;window < (width / 2);window+=step) {
					if (window < minWidth) {
						if ((ofs - lowIndex) > window * 1) {
							continue;
						}
					} else {
						if ((ofs - lowIndex) > window) {
							continue;
						}
					}
					skipCnt++;
					if (param->p->hfaFast && (skipCnt % 8) != 0) {
						continue;
					}
					step = 1;
					// スペクトル成分の遷移を調べる
					diff0 = 0;
					baseOfs = ofs - ((ofs / window) * window);
					if (baseOfs == 0) {
						baseOfs = window;
					}
					n = 1;		// 奇数偶数倍すべてに倍音がある
					refPw[0] = -1;
					avgPw  = 0;
					avgPw2 = 0;
					avgPw3 = 0;
					avgPwNX  = 0;
					avgPwNX2 = 0;
					avgPwNX3 = 0;
					ovInfo->sign[baseOfs] = 0;
					for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
						hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
						if (hz < lowHz) {
							continue;
						}
						// 平均より大きい音声か、小さい音声か
						if (ovInfo->power_ptr[i] > ovInfo->base[i]) {
							ovInfo->sign[baseOfs]++;
						} else if (ovInfo->power_ptr[i] < ovInfo->base[i]) {
							ovInfo->sign[baseOfs]--;
						}

						if (refPw[0] == -1) {
							refPw[0] = ovInfo->power_ptr[i] * hz;
							max_i = i;
						}
						avgPw += ovInfo->power_ptr[i];
						avgPwNX++;
						nn++;
					}

					if (avgPwNX > 0) {
						avgPw /= avgPwNX;
					}
					if (minDiff == -1 || minDiff > avgPw) {
						minDiff = avgPw;
						minWin = window;
						minType = 0;
					}
				}

				// 一番累積のパワーが弱いものを採用する。

				baseOfs = ofs - ((ofs / minWin) * minWin);
				if (baseOfs == 0) {
					baseOfs = minWin;
				}

				pha = ovInfo->phase[baseOfs];
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;
				refPw[0] = -1;
				if (minWin == swidth || minWin == width - 1) {
					continue;
				}

				for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power_ptr[i] * hz;
						max_i = i;
						pha = ovInfo->phase[max_i];
					}

					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig3Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					
					if (ovInfo->pw[i] == 0 && ovInfo->sign[baseOfs] < -16) {
						ovInfo->pw[i] = tmpPw * 0.18;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = -1;
						if (param->hfa3_sig3_print_disable == 0) {
							param->hfa3_sig3_print_disable = 1;
							if (ovInfo->hfa3_log == 0) {
								PRINT_LOG("","hfa3 sig3 output");
							}
						}
					}
				}
			}
		} else {
			for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
				minDiff = -1;
				skipCnt = 0;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 50;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 66;
				step = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;
				for (window = swidth;window < (width / 2);window+=step) {
					if (window < minWidth) {
						if ((ofs - lowIndex) > window * 1) {
							continue;
						}
					} else {
						if ((ofs - lowIndex) > window) {
							continue;
						}
					}
					skipCnt++;
					if (param->p->hfaFast && (skipCnt % 8) != 0) {
						continue;
					}
					step = 1;
					// スペクトル成分の遷移を調べる
					diff0 = 0;
					baseOfs = ofs - ((ofs / window) * window);
					if (baseOfs == 0) {
						baseOfs = window;
					}
					n = 1;		// 奇数偶数倍すべてに倍音がある
					refPw[0] = -1;
					avgPw  = 0;
					avgPw2 = 0;
					avgPw3 = 0;
					avgPwNX  = 0;
					avgPwNX2 = 0;
					avgPwNX3 = 0;
					ovInfo->sign[baseOfs] = 0;
					for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
						hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
						if (hz < lowHz) {
							continue;
						}
						// 平均より大きい音声か、小さい音声か
						if (ovInfo->power_ptr[i] > ovInfo->base[i]) {
							ovInfo->sign[baseOfs]++;
						} else if (ovInfo->power_ptr[i] < ovInfo->base[i]) {
							ovInfo->sign[baseOfs]--;
						}

						if (refPw[0] == -1) {
							refPw[0] = ovInfo->power_ptr[i] * hz;
							max_i = i;
						}
						avgPw += ovInfo->power_ptr[i];
						avgPwNX++;
						nn++;
					}

					if (avgPwNX > 0) {
						avgPw /= avgPwNX;
					}
					if (minDiff == -1 || minDiff > avgPw) {
						minDiff = avgPw;
						minWin = window;
						minType = 0;
					}
				}

				// 一番累積のパワーが弱いものを採用する。

				baseOfs = ofs - ((ofs / minWin) * minWin);
				if (baseOfs == 0) {
					baseOfs = minWin;
				}

				pha = ovInfo->phase[baseOfs];
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;
				refPw[0] = -1;
				if (minWin == swidth || minWin == width - 1) {
					continue;
				}

				for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power_ptr[i] * hz;
						max_i = i;
						pha = ovInfo->phase[max_i];
					}

					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig3Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					
	//				if (ovInfo->pw[i] == 0 && ovInfo->sign[baseOfs] < 0 /* -16 */) {
					if (ovInfo->pw[i] == 0 && ovInfo->sign[baseOfs] < -16) {
					// if (ovInfo->pw[i] == 0 || ovInfo->pw[i] > tmpPw * 0.18) {
						ovInfo->pw[i] = tmpPw * 0.18;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = -1;
						if (param->hfa3_sig3_print_disable == 0) {
							param->hfa3_sig3_print_disable = 1;
							if (ovInfo->hfa3_log == 0) {
								PRINT_LOG("","hfa3 sig3 output");
							}
						}
					}
				}
			}
		}
	}

	if (param->p->sig2Enb == 1 && param->p->hfaWide) {
		PRINT_LOG("","param->sig2Enb == 1 && hfaWide");
		lowHz	= 7000;
		wid		= 4000;
		if (lowHz + wid  >= ovInfo->validSamplingRate) {
			wid = ovInfo->validSamplingRate - lowHz;
		}

		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 180;
		width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
		minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
		if (ovInfo->validSamplingRate < 16000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
		}
		if (swidth == 0) {
			swidth = 1;
		}

		// 前後のwindowで振幅の差が少ない音声の補間
		window = width;
		minWin = window;
		minType = 0;

		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;
			for (window = swidth;window < (width / 2);window+=step) {
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}

				// スペクトル成分の遷移を調べる
				diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;	// 奇数倍のみ倍音がある
				refPw[0] = -1;
				refPw[1] = -1;
				refPw[2] = -1;
				refPw[3] = -1;
				refPw[4] = -1;
				refPw[5] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						refPw[1] = ovInfo->power[i] * n;
						refPw[2] = ovInfo->power[i] * odd;
						refPw[3] = ovInfo->power[i] * (odd * odd);
						refPw[4] = ovInfo->power[i];
						refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					}
					// 前後のパワーの差の計算
					if (i >= window) {
						if (i - window >= ofs) {
							if (ovInfo->power[i - window] >= ovInfo->power[i]) {
								diff = ovInfo->power[i - window] - ovInfo->power[i];
							} else {
								diff = ovInfo->power[i] - ovInfo->power[i - window];
							}
						}
					} else {
						continue;
					}
					diffP += (diff * 1.48);

					avgPw += ovInfo->power[i];
					avgPwNX++;
					if ((avgPwNX & 0x01) == 0) {
						avgPw2 += ovInfo->power[i];
						avgPwNX2++;
					}
					if ((avgPwNX % 3) == 0) {
						avgPw3 += ovInfo->power[i];
						avgPwNX3++;
					}
					
					// 1/f(振幅が1/fになっているもの)
					diff = refPw[0] / hz;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff0 += diff;

					// 鋸波(nの逆数で小さくなる)
					diff = refPw[1] / n;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff1 += diff;

					// 短形波(奇数倍音,nの逆数で小さくなる)
					diff = refPw[2] / odd;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff2 += diff;

					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					diff = refPw[3] / (odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff3 += diff;

					// パルス(n番目の倍音でもパワーは同じ)
					diff = refPw[4];
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff4 += diff;

					// その他(もっとパワーが小さくなるパターン)
					diff = refPw[5] / (odd * odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff5 += diff;

					nn++;
				}

				diff0 += diffP * 4;
				diff1 += diffP * 4;
				diff2 += diffP * 4;
				diff3 += diffP * 4;
				diff4 += diffP * 4;
				diff5 += diffP * 4;

				if (nn > 0) {
					diff0 /= nn;
					diff1 /= nn;
					diff2 /= nn;
					diff3 /= nn;
					diff4 /= nn;
					diff5 /= nn;
					diffP /= nn;
				}

				if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
					tmpAvgPw  = avgPw / avgPwNX;
					tmpAvgPw2 = avgPw2 / avgPwNX2;
					tmpAvgPw3 = avgPw3 / avgPwNX3;
					if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
					}
				}

				if (minDiff == -1 || minDiff > diffP) {
					minDiff = diffP;
					minWin = window;
					minType = 0;
				}
				if (minDiff > diff1) {
					minDiff = diff1;
					minWin = window;
					minType = 1;
				}
				if (minDiff > diff2) {
					minDiff = diff2;
					minWin = window;
					minType = 2;
				}
				if (minDiff > diff3) {
					minDiff = diff3;
					minWin = window;
					minType = 3;
				}
				if (minDiff > diff4) {
					minDiff = diff4;
					minWin = window;
					minType = 4;
				}
				if (minDiff > diff5) {
					minDiff = diff5;
					minWin = window;
					minType = 5;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある
			odd = 1;
			refPw[0] = -1;
			refPw[4] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}

			for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[i] * hz;
					refPw[1] = ovInfo->power[i] * n;
					refPw[2] = ovInfo->power[i] * odd;
					refPw[3] = ovInfo->power[i] * (odd * odd);
					refPw[4] = ovInfo->power[i];
					refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				if (minType == 0) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					
					if (ovInfo->pw[i] != 0 && ovInfo->pw[i] * 0.77 > tmpPw) {
						ovInfo->pw[i] = tmpPw * 0.32;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					}
				} else if (minType == 1) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[1] / n;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] != 0 && ovInfo->pw[i] * 0.77 > tmpPw) {
						ovInfo->pw[i] = tmpPw * 0.28;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					}
				} else if (minType == 2) {
					// 短形波(奇数倍音,nの逆数で小さくなる)
					tmpPw = refPw[2] / odd;
					phaRand = 1;
					pha = ovInfo->phase[max_i];
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					if (ovInfo->pw[i] != 0 && ovInfo->pw[i] * 0.77 > tmpPw) {
						ovInfo->pw[i] = tmpPw * 0.28;
						ovInfo->phase[i] = phaTmp;
						ovInfo->diff[i] = minDiff;
					}
				} else if (minType == 3) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					if (ovInfo->pw[i] != 0 && ovInfo->pw[i] * 0.77 > tmpPw) {
						ovInfo->pw[i] = tmpPw * 0.28;
						ovInfo->phase[i] = phaTmp;
						ovInfo->diff[i] = minDiff;
					}
				} else if (minType == 4) {
					// パルス(n番目の倍音でもパワーは同じ)
					tmpPw = refPw[4];
					phaRand = rand() * 6;
					phaRand -= 3;
					pha += phaRand;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] != 0 && ovInfo->pw[i] * 0.77 > tmpPw) {
						ovInfo->pw[i] = tmpPw * 0.28;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					}
				} else if (minType == 5) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] != 0 && ovInfo->pw[i] * 0.77 > tmpPw) {
						ovInfo->pw[i] = tmpPw * 0.28;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					}
				}
			}
		}
	}

//	if (ovInfo->log) {
//		FILE *ofp;
//		ofp = fopen("d:\\fft.csv","a");
//		if (ofp) {
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			for (i = 300;i < 750;i++) {
//				fprintf(ofp,"%lf,",ovInfo->pw[i]);
//			}
//			fprintf(ofp,"\n");
//			fflush(ofp);
//			fclose(ofp);
//		}
//	}

//	if (ovInfo->log) {
//		FILE *ofp;
//		ofp = fopen("d:\\fft.csv","a");
//		if (ofp) {
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			for (i = 300;i < 750;i++) {
//				fprintf(ofp,"%lf,",ovInfo->pw[i]);
//			}
//			fprintf(ofp,"\n");
//			fflush(ofp);
//			fclose(ofp);
//		}
//	}

#if 0
	//
	// 位相の調整
	for (i = 1;i < validIndex;i++) {
		ovInfo->diff[i] = -1;
	}
	window = width;
	for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
		minDiff = -1;
		skipCnt = 0;
		for (window = swidth;window < width;window++) {
			if (window < minWidth) {
				if ((ofs - lowIndex) > window * 1) {
					continue;
				}
			} else {
				if ((ofs - lowIndex) > window) {
					continue;
				}
			}
			skipCnt++;
			if (param->p->hfaFast && (skipCnt % 8) != 0) {
				continue;
			}
			// 位相を調べる
			diffP = 0;
			baseOfs = ofs - ((ofs / window) * window);
			if (baseOfs == 0) {
				baseOfs = window;
			}
			n = 1;
			refPw[0] = -1;
			for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}

				if (refPw[0] == -1) {
					refPw[0] = ovInfo->phase[i];
				}
				// 前後の位相の差の計算
				if (i >= window) {
					if (i - window >= ofs) {
						if (refPw[0] <= ovInfo->phase[i]) {
							diff = ovInfo->phase[i] - refPw[0];
						} else {
							diff = refPw[0] - ovInfo->phase[i];
						}
					}
				} else {
					continue;
				}
				diffP += diff;
				nn++;
			}

			if (nn > 0) {
				diffP /= nn;
			}

			if (minDiff == -1 || minDiff > diffP) {
				minDiff = diffP;
				minWin = window;
			}
		}

		// 一番予測誤差が少なかったものを採用する。

		baseOfs = ofs - ((ofs / minWin) * minWin);
		if (baseOfs == 0) {
			baseOfs = minWin;
		}

		pha = ovInfo->phase[baseOfs];
		n = 1;		// 奇数偶数倍すべてに倍音がある

		refPw[0] = -1;

		for (i = baseOfs;i < validIndex;i += minWin,n++) {
			hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
			if (hz < lowHz) {
				continue;
			}
			if (refPw[0] == -1) {
				refPw[0] = ovInfo->phase[i];
			}

			phaRand = rand() * 2;
			phaRand -= 1;
			pha = refPw[0];
			//pha += phaRand;
			if (pha >= 360) {
				pha %= 360;
			}
			if (pha < 0) {
				pha += 360;
			}
			if (ovInfo->diff[i] == -1 || ovInfo->diff[i] > minDiff) {
				ovInfo->phase[i] = pha;
				ovInfo->diff[i] = minDiff;
			}
		}
	}
#endif
	// 補間されていない箇所の対応
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					ovInfo->phase[i] -= 360;
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
	if (param->p->sig3Phase >= -6 || param->p->sig3Phase <= 6) {
		// 位相の修正
		window = width;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;
			for (window = swidth;window < (width / 2);window++) {
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}

				// スペクトル成分の遷移を調べる
				diff0 = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				refPw[0] = -1;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->phase[i];
					}
					
					diff = refPw[0];
					if (diff >= ovInfo->phase[i]) {
						diff = diff - ovInfo->phase[i];
					} else {
						diff = ovInfo->phase[i] - diff;
					}
					diff0 += diff;
					nn++;
				}

				if (nn > 0) {
					diff0 /= nn;
				}

				if (minDiff == -1 || minDiff > diff0) {
					minDiff = diff0;
					minWin = window;
					minType = 0;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある

			refPw[0] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}
			for (i = baseOfs;i < nSample;i += minWin) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				if (pha >= 360) {
					pha %= 360;
				}
				if (pha < 0) {
					pha += 360;
					pha += 360;
					pha += 360;
					pha %= 360;
				}
				if (ovInfo->pw[i] != 0) {
					if (ovInfo->diff[i] == -1 || ovInfo->diff[i] > minDiff) {
						ovInfo->phase[i] = pha;
					}
				}
			}
		}
	}
#if 1
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * (ovInfo->validSamplingRate - 5000);
	highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * (ovInfo->validSamplingRate - 1000);
	i = 1;
	do {
		for (j = i,n = 0,k = lowIndex;n < highIndex - lowIndex && j < ovInfo->nSample;j++,n++) {
			ovInfo->pw[j] += ovInfo->baseToAdj[k];
			k++;
			if (k > highIndex) {
				k = lowIndex;
			}
		}
		i += n;
	} while (i < ovInfo->nSample);
#endif
#if 0
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	highIndex = nSample;
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 5000;
	i = lowIndex;
	do {
		avg = 0;
		for (j = i,n = 0;n < width && j < ovInfo->nSample;j++,n++) {
			avg += ovInfo->pw[j];
		}
		if (n > 0) {
			avg /= n;
			avg = log10(avg) * 20;
		}
		for (j = i,n = 0;n < width && j < ovInfo->nSample;j++,n++) {
			tmpPw = ovInfo->pw[j];
			if (tmpPw > 0) {
				tmpPw = log10(tmpPw) * 20;
			}
			if (tmpPw + 15 < avg) {
				ovInfo->pw[j] *= 13;
			} else if (tmpPw > avg + 15) {
				ovInfo->pw[j] /= 13;
			}
		}
		i = j;
	} while (i < ovInfo->nSample);
#endif
//	if (ovInfo->log) {
//		FILE *ofp;
//		ofp = fopen("d:\\fft.csv","a");
//		if (ofp) {
//			for (i = 0;i < 4096;i++) {
//				fprintf(ofp,"%lf,%lf\n",ovInfo->pw[i],ovInfo->phase[i]);
//			}
//			fprintf(ofp,"\n");
//			fflush(ofp);
//			fclose(ofp);
//		}
//		ovInfo->log = 0;
//	}

#if 1
	if (param->p->hfaWide) {
		// 解析情報を平均化し、急激な変化を抑える
		for (i = baseOfs;i + 1< nSample;i++) {
			hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
			if (hz < lowHz) {
				continue;
			}
			if (ovInfo->pw[i] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
#endif
	if (1) {
		double nx;
		// 付加した広域で強すぎるものがあれば弱める。
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		highIndex = nSample;
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 5000;
		i = lowIndex;
		nx = 6.7;
		if (ovInfo->validSamplingRate <= 12000) {
			nx = 3.2;
		}
		do {
			avg = 0;
			for (j = i,n = 0;n < swidth && j < ovInfo->nSample;j++,n++) {
				avg += ovInfo->pw[j];
			}
			if (n > 0) {
				avg /= n;
			}
			for (j = i,n = 0;n < width && j < ovInfo->nSample;j++,n++) {
				if (avg * nx < ovInfo->pw[j]) {
					ovInfo->pw[j] *= avg * nx / ovInfo->pw[j];
				}
			}
			i = j;
		} while (i < ovInfo->nSample);
	}
if (0) {
	if (ovInfo->pw[i] > 0 && ovInfo->pw[i + 1] > 0) {
		ovInfo->pw[i] = (ovInfo->pw[i] + ovInfo->pw[i+1]) / 2;
		ovInfo->phase[i] = (ovInfo->phase[i] + ovInfo->phase[i+1]) / 2;
		if (ovInfo->phase[i] >= 360) {
			pha = ovInfo->phase[i];
			ovInfo->phase[i] = (pha % 360);
		}
		if (ovInfo->phase[i] < 0) {
			ovInfo->phase[i] += 360;
		}
	}
}
	ovInfo->hfa3_log = 1;

	PRINT_FN_LOG(param->debug_upconv,"-anaOverToneHFA3:End");
}
#else
void anaOverToneHFA3(OVERTONE_INFO *ovInfo,PARAM_INFO *param)
{
	DWORD ofs,window,width,swidth;
	DWORD n,i,j,k,lll;
	int ii,jj;
	DWORD validN;
	double avg,minAvg,maxAvg,peekPw,diffPw,diffAvg;
	double keyPw;
	double avgLine;
	double diff,diff0,diff1,diff2,diff3,diff4,diff5,diff6,diff7,diffP;
	double refPw[8];
	double avgPw,avgPw2,avgPw3;
	double tmpAvgPw,tmpAvgPw2,tmpAvgPw3;
	int    avgPwNX,avgPwNX2,avgPwNX3;
	double maxPwAvg;
	long skipCnt;

	// 予測との最小パラメータ保存
	int minWin;
	int minType;
	int max_i;
	double minDiff;

	int nd,nn;
	int odd;
	double ndLv;
	double hz,hz2;
	DWORD  baseOfs;
	int minOfs,maxOfs,maxWin,maxType;
	double tmpPw;
	double nx,nx2;
	int tmpOfs,tmpWin;
	int lowIndex,highIndex;
	int minWidth;
	int miOffset;
	int pha;
	double phaRand;
	int phaTmp = 0;
	long nSample;
	long lowHz,wid;
	double areaAvg;
	nSample = ovInfo->nSample;

	//
	// 初期化
	for (i = 1;i < ovInfo->nSample;i++) {
		ovInfo->pw[i] = 0;
		ovInfo->diff[i] = -1;
		ovInfo->baseToAdj[i] = 0;
	}
	swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 2000;
	//swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 400;
#if 0
	for (i = 1;i < ovInfo->nSample;i+= swidth) {
		avg = 0;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			avg += ovInfo->power[j];
		}
		avg /= n;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			ovInfo->baseToAdj[j] = ovInfo->power[j] - avg;
			ovInfo->power[j] -= ovInfo->baseToAdj[j];
		}
	}
#endif
	lowHz	= 6000;
	wid		= 3500;
	if (param->p->hfaWide) {
		lowHz	= 4500;
		wid		= 5500;
	}
	if (lowHz + wid + 2000 >= ovInfo->validSamplingRate) {
		wid = 2000;
		lowHz = 4500;
	}

	if (lowHz < 4000) {
		// 高域の情報がないので解析しない
		return;
	}

	swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 180;
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
	minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
	if (ovInfo->validSamplingRate < 16000) {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	} else {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
	}
	if (swidth == 0) {
		swidth = 1;
	}

	avg = 0;
	for (i = lowIndex,n = 0;i < highIndex;i++) {
		avg += ovInfo->power[i];
		n++;
	}
	if (n > 0) {
		avg /= n;
	}
	avgLine = avg;

	if (param->sig2Enb == 1) {
		// 前後のwindowで振幅の差が少ない音声の補間
		window = width;
		minWin = window;
		minType = 0;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;
			for (window = swidth;window < (width / 2);window++) {
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}

				// スペクトル成分の遷移を調べる
				diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;	// 奇数倍のみ倍音がある
				refPw[0] = -1;
				refPw[1] = -1;
				refPw[2] = -1;
				refPw[3] = -1;
				refPw[4] = -1;
				refPw[5] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						refPw[1] = ovInfo->power[i] * n;
						refPw[2] = ovInfo->power[i] * odd;
						refPw[3] = ovInfo->power[i] * (odd * odd);
						refPw[4] = ovInfo->power[i];
						refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					}
					// 前後のパワーの差の計算
					if (i >= window) {
						if (i - window >= ofs) {
							if (ovInfo->power[i - window] >= ovInfo->power[i]) {
								diff = ovInfo->power[i - window] - ovInfo->power[i];
							} else {
								diff = ovInfo->power[i] - ovInfo->power[i - window];
							}
						}
					} else {
						continue;
					}
					diffP += diff;

					avgPw += ovInfo->power[i];
					avgPwNX++;
					if ((avgPwNX & 0x01) == 0) {
						avgPw2 += ovInfo->power[i];
						avgPwNX2++;
					}
					if ((avgPwNX % 3) == 0) {
						avgPw3 += ovInfo->power[i];
						avgPwNX3++;
					}
					
					// 1/f(振幅が1/fになっているもの)
					diff = refPw[0] / hz;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff0 += diff;

					// 鋸波(nの逆数で小さくなる)
					diff = refPw[1] / n;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff1 += diff;

					// 短形波(奇数倍音,nの逆数で小さくなる)
					diff = refPw[2] / odd;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff2 += diff;

					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					diff = refPw[3] / (odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff3 += diff;

					// パルス(n番目の倍音でもパワーは同じ)
					diff = refPw[4];
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff4 += diff;

					// その他(もっとパワーが小さくなるパターン)
					diff = refPw[5] / (odd * odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff5 += diff;

					nn++;
				}

				if (nn > 0) {
					diff0 /= nn;
					diff1 /= nn;
					diff2 /= nn;
					diff3 /= nn;
					diff4 /= nn;
					diff5 /= nn;
					diffP /= nn;
				}

				if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
					tmpAvgPw  = avgPw / avgPwNX;
					tmpAvgPw2 = avgPw2 / avgPwNX2;
					tmpAvgPw3 = avgPw3 / avgPwNX3;
					if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
					}
				}

				if (minDiff == -1 || minDiff > diffP) {
					minDiff = diffP;
					minWin = window;
					minType = 0;
				}
				if (minDiff > diff1) {
					minDiff = diff1;
					minWin = window;
					minType = 1;
				}
				if (minDiff > diff2) {
					minDiff = diff2;
					minWin = window;
					minType = 2;
				}
				if (minDiff > diff3) {
					minDiff = diff3;
					minWin = window;
					minType = 3;
				}
				if (minDiff > diff4) {
					minDiff = diff4;
					minWin = window;
					minType = 4;
				}
				if (minDiff > diff5) {
					minDiff = diff5;
					minWin = window;
					minType = 5;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある
			odd = 1;
			refPw[0] = -1;
			refPw[4] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}
			for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[i] * hz;
					refPw[1] = ovInfo->power[i] * n;
					refPw[2] = ovInfo->power[i] * odd;
					refPw[3] = ovInfo->power[i] * (odd * odd);
					refPw[4] = ovInfo->power[i];
					refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				if (minType == 0) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.42;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->pw[i] > tmpPw) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
					}
				} else if (minType == 1) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[1] / n;
					phaRand = 1;
					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.42;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->pw[i] > tmpPw) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
					}
				} else if (minType == 2) {
					// 短形波(奇数倍音,nの逆数で小さくなる)
					tmpPw = refPw[2] / odd;
					phaRand = 1;
					pha = ovInfo->phase[max_i];
					phaTmp = pha + param->p->sig2Phase;
					if (n & 0x01) {
						phaTmp += 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.42;
						ovInfo->phase[i] = phaTmp;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = phaTmp;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->pw[i] > tmpPw) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = phaTmp;
							ovInfo->diff[i] = minDiff;
						}
					}
				} else if (minType == 3) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					phaTmp = pha + param->p->sig2Phase;
					if (n & 0x01) {
						phaTmp += 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.42;
						ovInfo->phase[i] = phaTmp;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = phaTmp;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->pw[i] > tmpPw) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = phaTmp;
							ovInfo->diff[i] = minDiff;
						}
					}
				} else if (minType == 4) {
					// パルス(n番目の倍音でもパワーは同じ)
					tmpPw = refPw[4];
					phaRand = rand() * 6;
					phaRand -= 3;
					pha += phaRand;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.42;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->pw[i] > tmpPw) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
					}
				} else if (minType == 5) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.42;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
						if (ovInfo->pw[i] > tmpPw) {
							ovInfo->pw[i] = tmpPw * 0.42;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						}
					}
				}
			}
		}
	}
//	if (ovInfo->log) {
//		FILE *ofp;
//		ofp = fopen("d:\\fft.csv","a");
//		if (ofp) {
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			for (i = 300;i < 750;i++) {
//				fprintf(ofp,"%lf,",ovInfo->pw[i]);
//			}
//			fprintf(ofp,"\n");
//			fflush(ofp);
//			fclose(ofp);
//		}
//	}

#if 0
	if (param->sig3Enb == 1 && (param->sig3Phase < -6 || param->sig3Phase > 6)) {
		// 位相の修正
		window = width;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;
			for (window = swidth;window < (width / 2);window++) {
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}

				// スペクトル成分の遷移を調べる
				diff0 = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				refPw[0] = -1;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->phase[i];
					}
					
					diff = refPw[0];
					if (diff >= ovInfo->phase[i]) {
						diff = diff - ovInfo->phase[i];
					} else {
						diff = ovInfo->phase[i] - diff;
					}
					diff0 += diff;
					nn++;
				}

				if (nn > 0) {
					diff0 /= nn;
				}

				if (minDiff == -1 || minDiff > diff0) {
					minDiff = diff0;
					minWin = window;
					minType = 0;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある

			refPw[0] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}
			for (i = baseOfs;i < nSample;i += minWin) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				if (pha >= 360) {
					pha %= 360;
				}
				if (pha < 0) {
					pha += 360;
					pha += 360;
					pha += 360;
					pha %= 360;
				}
				if (ovInfo->pw[i] != 0) {
					if (ovInfo->diff[i] == -1 || ovInfo->diff[i] > minDiff) {
						ovInfo->phase[i] = pha;
					}
				}
			}
		}
	}
#endif
	if (param->sig1Enb == 1) {

		// powerが強いもの優先で補間する

		areaAvg = 0;
		for (i = lowIndex,n = 0;n < width;i++,n++) {
			areaAvg += ovInfo->power[i];
		}
		if (n > 0) {
			areaAvg /= n;
		}

		window = width;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;
			if (ovInfo->power[ofs] < areaAvg) {
				continue;
			}
			
			for (window = swidth;window < (width / 2);window++) {
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}
				// スペクトル成分の遷移を調べる
				diff0 = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				refPw[0] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (i >= window) {
						if (i - window >= ofs) {
							if (ovInfo->power[i - window] < ovInfo->power[i]) {
								avgPw /= 75;
							}
						}
					} else {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						max_i = i;
					}
					if (ovInfo->power[i] > areaAvg) {
						avgPw += ovInfo->power[i];
					} else {
						avgPw /= 75;
					}
					avgPwNX++;
					nn++;
				}

//				if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
//					tmpAvgPw  = avgPw / avgPwNX;
//					tmpAvgPw2 = avgPw2 / avgPwNX2;
//					tmpAvgPw3 = avgPw3 / avgPwNX3;
//					if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {//						continue;
//					}
//				}
				if (avgPwNX > 0) {
					avgPw /= avgPwNX;
				}
				if (minDiff == -1 || minDiff < avgPw) {
					minDiff = avgPw;
					minWin = window;
					minType = 0;
				}
			}

			// 一番累積のパワーが強いものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある

			refPw[0] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}
			for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[i] * hz;
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				// 1/f(振幅が1/fになっているもの)
				tmpPw = refPw[0] / hz;
				phaRand = 1;
				pha += param->sig1Phase;
				if (pha >= 360) {
					pha %= 360;
				}
				if (pha < 0) {
					pha += 360;
				}
				if (ovInfo->pw[i] == 0) {
					ovInfo->pw[i] = (tmpPw * 0.70);
					ovInfo->phase[i] = pha;
					ovInfo->diff[i] = -1;
				}
			}
		}
	}
//	if (ovInfo->log) {
//		FILE *ofp;
//		ofp = fopen("d:\\fft.csv","a");
//		if (ofp) {
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			fprintf(ofp,"\n");
//			for (i = 300;i < 750;i++) {
//				fprintf(ofp,"%lf,",ovInfo->pw[i]);
//			}
//			fprintf(ofp,"\n");
//			fflush(ofp);
//			fclose(ofp);
//		}
//	}

	if (param->sig3Enb == 1) {
		if (ovInfo->hfa3_log == 0) {
			PRINT_LOG("","sig3Enb");
		}
		// powerが弱いものと補間値がないものを補間する

		window = width;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;
			for (window = swidth;window < (width / 2);window++) {
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}
				// スペクトル成分の遷移を調べる
				diff0 = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				refPw[0] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						max_i = i;
					}
					avgPw += ovInfo->power[i];
					avgPwNX++;
					nn++;
				}

				if (avgPwNX > 0) {
					avgPw /= avgPwNX;
				}
				if (minDiff == -1 || minDiff > avgPw) {
					minDiff = avgPw;
					minWin = window;
					minType = 0;
				}
			}

			// 一番累積のパワーが弱いものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある
			odd = 1;
			refPw[0] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}
			for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[i] * hz;
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				// 1/f(振幅が1/fになっているもの)
				tmpPw = refPw[0] / hz;
				phaRand = 1;
				pha += param->sig3Phase;
				if (pha >= 360) {
					pha %= 360;
				}
				if (pha < 0) {
					pha += 360;
				}
				if (ovInfo->pw[i] == 0) {
					ovInfo->pw[i] = (tmpPw * 0.35);
					ovInfo->phase[i] = pha;
					ovInfo->diff[i] = -1;
				}
			}
		}
	}

	// 補間されていない箇所の対応
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					ovInfo->phase[i] -= 360;
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					ovInfo->phase[i] -= 360;
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					ovInfo->phase[i] -= 360;
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					ovInfo->phase[i] -= 360;
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
	if (param->sig3Phase >= -6 || param->sig3Phase <= 6) {
		// 位相の修正
		window = width;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;
			for (window = swidth;window < (width / 2);window++) {
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}

				// スペクトル成分の遷移を調べる
				diff0 = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				refPw[0] = -1;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->phase[i];
					}
					
					diff = refPw[0];
					if (diff >= ovInfo->phase[i]) {
						diff = diff - ovInfo->phase[i];
					} else {
						diff = ovInfo->phase[i] - diff;
					}
					diff0 += diff;
					nn++;
				}

				if (nn > 0) {
					diff0 /= nn;
				}

				if (minDiff == -1 || minDiff > diff0) {
					minDiff = diff0;
					minWin = window;
					minType = 0;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある

			refPw[0] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}
			for (i = baseOfs;i < nSample;i += minWin) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				if (pha >= 360) {
					pha %= 360;
				}
				if (pha < 0) {
					pha += 360;
					pha += 360;
					pha += 360;
					pha %= 360;
				}
				if (ovInfo->pw[i] != 0) {
					if (ovInfo->diff[i] == -1 || ovInfo->diff[i] > minDiff) {
						ovInfo->phase[i] = pha;
					}
				}
			}
		}
	}
#if 0
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * (ovInfo->validSamplingRate - 5000);
	highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * (ovInfo->validSamplingRate - 2000);
	i = 1;
	do {
		for (j = i,n = 0,k = lowIndex;n < highIndex - lowIndex && j < ovInfo->nSample;j++,n++) {
			ovInfo->pw[j] += ovInfo->baseToAdj[k];
			k++;
			if (k > highIndex) {
				k = lowIndex;
			}
		}
		i += n;
	} while (i < ovInfo->nSample);
#endif
#if 0
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	highIndex = nSample;
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 5000;
	i = lowIndex;
	do {
		avg = 0;
		for (j = i,n = 0;n < width && j < ovInfo->nSample;j++,n++) {
			avg += ovInfo->pw[j];
		}
		if (n > 0) {
			avg /= n;
			avg = log10(avg) * 20;
		}
		for (j = i,n = 0;n < width && j < ovInfo->nSample;j++,n++) {
			tmpPw = ovInfo->pw[j];
			if (tmpPw > 0) {
				tmpPw = log10(tmpPw) * 20;
			}
			if (tmpPw + 15 < avg) {
				ovInfo->pw[j] *= 13;
			} else if (tmpPw > avg + 15) {
				ovInfo->pw[j] /= 13;
			}
		}
		i = j;
	} while (i < ovInfo->nSample);
#endif

#if 1
	if (param->p->hfaWide) {
		// 解析情報を平均化し、急激な変化を抑える
		for (i = baseOfs;i + 1< nSample;i++) {
			hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
			if (hz < lowHz) {
				continue;
			}
			if (ovInfo->pw[i] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
#endif

//	if (ovInfo->log) {
//		FILE *ofp;
//		ofp = fopen("d:\\fft.csv","a");
//		if (ofp) {
//			for (i = 0;i < 4096;i++) {
//				fprintf(ofp,"%lf,%lf\n",ovInfo->pw[i],ovInfo->phase[i]);
//			}
//			fprintf(ofp,"\n");
//			fflush(ofp);
//			fclose(ofp);
//		}
//		ovInfo->log = 0;
//	}
}
#endif
void anaOverToneHFA4(OVERTONE_INFO *ovInfo,PARAM_INFO *param)
{
	DWORD ofs,window,swidth;
	DWORD validIndex;
	long n,i,j,jj,k;
	double avg;
	double avgRef;
	double avgLine;
	double diff,diff0,diff1,diff2,diff3,diff4,diff5,diffP;
	double refPw[8];
	double avgPw,avgPw2,avgPw3;
	double tmpAvgPw,tmpAvgPw2,tmpAvgPw3;
	double w_1f,p_1f;
	double lag1_low,lag1_high,lag2_low,lag2_high;
	double diff_1st_2nd_3rd[3];
	int    diff_1st_2nd_3rd_count;
	int    avgPwNX,avgPwNX2,avgPwNX3;
	long   step;
	long skipCnt;
	double tbl_hfaDiffMin[5] = {0.84,1.04,1.24,1.74,2.14};
	// 予測との最小パラメータ保存
	int minWin;
	int minType;
	int max_i;
	double minDiff;
	int nn;
	int odd;
	double hz;
	double minWidth;
	DWORD  baseOfs;
	double tmpPw,tmpPw2;
	int lowIndex,highIndex;
	int currentBox;
	int maxBox,minBox;
	int maxWidth;
	int width,wid;
	int maxLag;
	int lowHz;
	int minHz;
	int turningPointIndex;
	int pha;
	int flag_lag_low,flag_lag_high;
	int flag_lag_random;
	double phaRand;
	int phaTmp = 0;
	long nSample;
	double areaAvg;
	char *sss;
	char *tstr;
	double pp,db_shift;
	double window_correction = 20.0 * log10(2.0 / ovInfo->nSample);	// 三角窓の平均補正
	double *adjH_pw;
	double *adjH_diff;
	size_t *adjH_index;
	nSample = ovInfo->nSample;

	if (param->upconv == 1 && ovInfo->index == 1) {
		PRINT_LOG(param->debug_upconv,"-anaOverToneHFA4:Start");
	}

	sss = malloc(256);
	tstr = malloc(256);
	adjH_pw = (double *)malloc(nSample * sizeof (double));
	adjH_diff = (double *)malloc(nSample * sizeof (double));
	adjH_index = (size_t *)malloc(nSample * sizeof (size_t));

	//
	// 初期化
	for (i = 1;i < ovInfo->nSample;i++) {
		ovInfo->pw[i] = 0;
		ovInfo->diff[i] = -1;
		ovInfo->base[i] = 0;
		ovInfo->baseToAdj[i] = 0;
		ovInfo->sign[i] = 0;
		ovInfo->hfa4_coeff[i] = 0;
		ovInfo->hfa4_lag[i] = 0;
	}
	param->hfaDiffMin = 3;

#if 0
	// 補間の対象とするか否かを検証する
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 100;
	highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 5000;
	avg = 0;
	for (i = lowIndex,n = 0;i < highIndex;i++,n++) {
		avg += ovInfo->power[i];
	}
	if (n > 0) {
		avg /= n;
	}
	if (1) {
		char sss[200];
		sprintf(sss,"HFA4GenOverTonePw: sec:%lld,%lf",ovInfo->startInSample / ovInfo->samplingRate,avg);
		PRINT_LOG("HFA4",sss);
	}
#endif
	if (ovInfo->validSamplingRate < 6000) {
		// 高域の情報がないので解析しない
		return;
	}

	swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
	ovInfo->power_ptr = ovInfo->power;
	if (param->p->hfa3_version == 3) {
		ovInfo->power_ptr = ovInfo->power_org;
	}

	for (i = 1;i < ovInfo->nSample;i+= swidth) {
		avg = 0;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			avg += ovInfo->power_ptr[j];
		}
		avg /= n;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			ovInfo->baseToAdj[j] = ovInfo->power[j] - avg;
			ovInfo->power[j] -= ovInfo->baseToAdj[j];
			ovInfo->base[j] = avg;
		}
	}
	if (ovInfo->validSamplingRate < 6000) {
		// 高域の情報がないので解析しない
		return;
	}

	// 9000 から 12000 Hz の間
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 9000;
	if (ovInfo->validSamplingRate < 12000) {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	} else {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 12000;
	}
	if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
	}
	if (ovInfo->validSamplingRate <= 12000) {
		// 8000 から 11000 Hz の間
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 8000;
		if (ovInfo->validSamplingRate < 11000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 11000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
	}
	if (ovInfo->validSamplingRate <= 10000) {
		// 6000 から 9000 Hz の間
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 6000;
		if (ovInfo->validSamplingRate < 9000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 9000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
	}
	if (ovInfo->validSamplingRate <= 8000) {
		// 4000 から 7000 Hz の間
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 4000;
		if (ovInfo->validSamplingRate < 7000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 7000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
	}
	lag1_low  = -0.15;
	lag1_high = 0.15;
	lag2_low  = 0.20;

	lag1_low  = -0.16;
	lag1_high = 0.16;
	lag2_low  = 0.22;
	lag2_high = 0.90;

	// Center 用の処理(ボーカルを含む)
	if (param->p->LRC_process_Center) {
#if 0
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 9000;
		if (ovInfo->validSamplingRate < 12000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 12000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
		// 8000 から 11000 Hz の間
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 8000;
		if (ovInfo->validSamplingRate < 11000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 11000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
		// 6000 から 9000 Hz の間
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 6000;
		if (ovInfo->validSamplingRate < 9000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 9000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
		// 4000 から 7000 Hz の間
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 4000;
		if (ovInfo->validSamplingRate < 7000) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 7000;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
#endif
		// 8500 から 11500 Hz の間
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 8500;
		if (ovInfo->validSamplingRate < 11500) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		} else {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 11500;
		}
		if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
			highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
		}
		lag1_low  = -0.16;
		lag1_high = 0.16;
		lag2_low  = 0.22;
		lag2_high = 0.90;
	}

	maxWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 3000;
	step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 150;
	maxLag    = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1700;
	minHz	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 174;

	swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 190;
	if (param->p->hfa3_version == 1) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else if (param->p->hfa3_version == 2) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	}
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 3000;
	lowHz     = 4000;
	avg = 0;
	for (i = lowIndex,n = 0;i < highIndex;i++) {
		avg += ovInfo->power[i];
		n++;
	}
	if (n > 0) {
		avg /= n;
	}
	avgLine = avg;

	window = maxWidth;
	minWin = window;
	minType = 0;

	if (param->p->iw == 16) {
		db_shift = 20.0 * log10(pow(2, 56 - 16));
	}

	if (param->p->iw == 24) {
		db_shift = 20.0 * log10(pow(2, 56 - 24));
	}

	// 相関が無いデータの補間(自然音など)
	// 8kHz～12kHz
	currentBox = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 4000;
	currentBox = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 3650;
	maxWidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 2500;
	if (param->p->LRC_process_Center) {
		currentBox = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 3500;
		maxWidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 2500;
	}

	for (i = 0;i < currentBox;i++) {
		double pp;
		int hz;
		hz = (int)((((double)ovInfo->samplingRate / 2) / ovInfo->nSample) * (i + lowIndex));
		pp = ovInfo->power[lowIndex + i];
		pp = 20 * log10(pp);
		pp = pp - db_shift;
		pp = pp - window_correction;
		pp = pp - 190;
		ovInfo->hfa4_coeff[i] = pp;
//		if (param->print_stdout && ovInfo->index == 1) {
//			sprintf(sss,"%s : %d,%lf",param->timecode,hz,pp);
//			PRINT_LOG("HFA4",sss);
//		}
	}
	n = i;
	for (i = 0;i < maxLag;i++) {
		ovInfo->hfa4_lag[i] = hfa4_autocorrelation_coefficient(ovInfo->hfa4_coeff,n,i);
	}
	
	for (turningPointIndex = 1;turningPointIndex < n;turningPointIndex++) {
		if (ovInfo->hfa4_lag[turningPointIndex - 1] < ovInfo->hfa4_lag[turningPointIndex] && turningPointIndex > minHz) break;
	}

//	if (param->upconv == 1 && ovInfo->index == 1) {
//		sprintf(sss,"---- nSample:%d ----",ovInfo->nSample);
//		PRINT_LOG("HFA4",sss);
//	}

#if 0
	if (param->upconv == 1 && ovInfo->index == 1) {
		for (i = 0;i < maxLag;i++) {
			sprintf(sss,"lag[%d]:%lf,maxLag:%d",i,ovInfo->hfa4_lag[i],maxLag);
			PRINT_LOG("HFA4",sss);
		}
	}
#endif

	// ソート
	gsl_sort_index(ovInfo->hfa4_index,ovInfo->hfa4_lag,1,n);

	for (ofs = lowIndex;ofs < lowIndex + maxWidth;ofs+=3) {
		// 自己相関が高いindexから順に倍音成分を調べる
		for (i = n - 1;i >= 0;i--) {
			if ((ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] == 0 || (ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] < 0 && ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] > 0)) && 
				ovInfo->hfa4_index[i] >= turningPointIndex && 
				((ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] < lag1_high && ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] > lag1_low) || (ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] > lag2_low && ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] < lag2_high))) {
				minDiff = -1;
				skipCnt = 0;
				
				window = ovInfo->hfa4_index[i];
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * window;
//				if (param->upconv == 1 && ovInfo->index == 1) {
//					sprintf(sss,"i:%d,lag:%lf,window:%ld,hz:%lf",i,ovInfo->hfa4_lag[i],window,hz);
//					PRINT_LOG("HFA4",sss);
//				}

				// スペクトル成分の遷移を調べる
				diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;	// 奇数倍のみ倍音がある

				refPw[0] = -1;
				refPw[1] = -1;
				refPw[2] = -1;
				refPw[3] = -1;
				refPw[4] = -1;
				refPw[5] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				ovInfo->sign[baseOfs] = 0;
				for (j = baseOfs,nn = 0;j < highIndex; j+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * j;
					if (hz < lowHz) {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[j] * hz;
						refPw[1] = ovInfo->power[j] * n;
						refPw[2] = ovInfo->power[j] * odd;
						refPw[3] = ovInfo->power[j] * (odd * odd);
						refPw[4] = ovInfo->power[j];
						refPw[5] = ovInfo->power[j] * (odd * odd * odd);
					}
					// 平均より大きい音声か、小さい音声か
					if (ovInfo->power[j] > ovInfo->base[j]) {
						ovInfo->sign[baseOfs]++;
					} else if (ovInfo->power[j] < ovInfo->base[j]) {
						ovInfo->sign[baseOfs]--;
					}
					// 前後のパワーの差の計算
					if (j > window) {
						if (j - window >= ofs) {
							if (ovInfo->power[j - window] >= ovInfo->power[j]) {
								diff = ovInfo->power[j - window] - ovInfo->power[j];
							} else {
								diff = ovInfo->power[j] - ovInfo->power[j - window];
							}
						}
					} else {
						continue;
					}
					diffP += (diff * tbl_hfaDiffMin[param->hfaDiffMin - 1]);

					avgPw += ovInfo->power[j];
					avgPwNX++;
					if ((avgPwNX & 0x01) == 0) {
						avgPw2 += ovInfo->power[j];
						avgPwNX2++;
					}
					if ((avgPwNX % 3) == 0) {
						avgPw3 += ovInfo->power[j];
						avgPwNX3++;
					}
					
					// 1/f(振幅が1/fになっているもの)
					diff = refPw[0] / hz;
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff0 += diff;

					// 鋸波(nの逆数で小さくなる)
					diff = refPw[1] / n;
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff1 += diff;

					// 短形波(奇数倍音,nの逆数で小さくなる)
					diff = refPw[2] / odd;
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff2 += diff;

					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					diff = refPw[3] / (odd * odd);
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff3 += diff;

					// パルス(n番目の倍音でもパワーは同じ)
					diff = refPw[4];
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff4 += diff;

					// その他(もっとパワーが小さくなるパターン)
					diff = refPw[5] / (odd * odd * odd);
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff5 += diff;

					nn++;
				}
				if (nn > 0) {
					diff0 /= nn;
					diff1 /= nn;
					diff2 /= nn;
					diff3 /= nn;
					diff4 /= nn;
					diff5 /= nn;
					diffP /= nn;
				}

				if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
					tmpAvgPw  = avgPw / avgPwNX;
					tmpAvgPw2 = avgPw2 / avgPwNX2;
					tmpAvgPw3 = avgPw3 / avgPwNX3;
					if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {
						continue;
					}
				}

				if (minDiff == -1 || minDiff > diffP) {
					minDiff = diffP;
					minWin = window;
					minType = 0;
				}
				if (minDiff > diff1) {
					minDiff = diff1;
					minWin = window;
					minType = 1;
				}
				if (minDiff > diff2) {
					minDiff = diff2;
					minWin = window;
					minType = 2;
				}
				if (minDiff > diff3) {
					minDiff = diff3;
					minWin = window;
					minType = 3;
				}
				if (minDiff > diff4) {
					minDiff = diff4;
					minWin = window;
					minType = 4;
				}
				if (minDiff > diff5) {
					minDiff = diff5;
					minWin = window;
					minType = 5;
				}
			}
			// 一番予測誤差が少なかったものを採用する。
			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}
			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある
			odd = 1;
			refPw[0] = -1;
			refPw[4] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}

			if (param->hfa3_sig2_print_disable == 0) {
				param->hfa3_sig2_print_disable = 1;
			}

			// 1/f 揺らぎ成分
			w_1f = 0;
			p_1f = 0;

			flag_lag_low  = 0;
			flag_lag_high = 0;
			flag_lag_random = 0;
			if (ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] < 0.15 && ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] > -0.15) {
				flag_lag_low = 1;
			}
			if (ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] > 0.68 && ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] < 0.88) {
				flag_lag_high = 1;
			}
			if (ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] == 0) {
				flag_lag_random = 1;
			}
			for (j = baseOfs;j < nSample;j += minWin,n++,odd+=2) {
				//w_1f = generate1fNoise(w_1f);
				//p_1f = generate1fNoise(p_1f);
				//p_1f = 1;
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * j;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[j] * hz;
					refPw[1] = ovInfo->power[j] * n;
					refPw[2] = ovInfo->power[j] * odd;
					refPw[3] = ovInfo->power[j] * (odd * odd);
					refPw[4] = ovInfo->power[j];
					refPw[5] = ovInfo->power[j] * (odd * odd * odd);
					max_i = j;
					pha = ovInfo->phase[max_i];
				}

				if (minType == 0) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					if (flag_lag_low) {
						tmpPw = tmpPw / 10;
					}
					if (flag_lag_random) {
						tmpPw = tmpPw / 5;
					}
					phaRand = 1;
					pha += 180;				// 位相が逆になる
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 1) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[1] / n;
					if (flag_lag_low) {
						tmpPw = tmpPw / 10;
					}
					if (flag_lag_random) {
						tmpPw = tmpPw / 5;
					}
					phaRand = 1;
					//pha += param->p->sig2Phase;
					pha += -1;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 2) {
					// 短形波(奇数倍音,nの逆数で小さくなる)
					tmpPw = refPw[2] / odd;
					if (flag_lag_low) {
						tmpPw = tmpPw / 10;
					}
					if (flag_lag_random) {
						tmpPw = tmpPw / 5;
					}
					phaRand = 1;
					pha = ovInfo->phase[max_i];
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 3) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd);
					if (flag_lag_low) {
						tmpPw = tmpPw / 10;
					}
					if (flag_lag_random) {
						tmpPw = tmpPw / 5;
					}
					phaRand = 1;
	//					pha = ovInfo->phase[max_i];
					//pha += param->p->sig2Phase;
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 4) {
					// パルス(n番目の倍音でもパワーは同じ)
					tmpPw = refPw[4];
					if (flag_lag_low) {
						tmpPw = tmpPw / 10;
					}
					if (flag_lag_random) {
						tmpPw = tmpPw / 5;
					}
					phaRand = rand() * 6;
					phaRand -= 3;
					pha += phaRand;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.18;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.18;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.18;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.18;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 5) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd * odd);
					if (flag_lag_low) {
						tmpPw = tmpPw / 10;
					}
					if (flag_lag_random) {
						tmpPw = tmpPw / 5;
					}
					phaRand = 1;
	//							pha = ovInfo->phase[max_i];
					//pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				}
			}
		}
	}

#if 0
	swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 500;
	for (i = 1;i < ovInfo->nSample;i+= swidth) {
		avg = 0;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			avg += ovInfo->pw[j];
		}
		avg /= n;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			if (ovInfo->pw[j] > avg) {
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;	//
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
			} else {
				ovInfo->pw[j] = (ovInfo->pw[j] + avg + avg + avg) / 4;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg + avg + avg) / 4;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
//				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
			}
		}
	}
#endif

#if 0
	// 7000 から 11000 Hz の間
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 7000;
	if (ovInfo->validSamplingRate < 16000) {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	} else {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
	}
	if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
	}
	maxWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 3000;
	step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;
	maxLag    = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 2500;

	swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 190;
	if (param->p->hfa3_version == 1) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else if (param->p->hfa3_version == 2) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	}
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 3000;
	lowHz     = 4000;

	window = maxWidth;
	minWin = window;
	minType = 0;

	// 4kHz～12kHz
	currentBox = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 2000;
	minBox     = currentBox / 5;
	maxWidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 2000;

	for (i = 0;i < currentBox;i++) {
		ovInfo->hfa4_coeff[i] = ovInfo->power[lowIndex + i];
	}
	n = i;
	for (i = 0;i < maxLag;i++) {
		ovInfo->hfa4_lag[i] = hfa4_autocorrelation_coefficient(ovInfo->hfa4_coeff,n,i);
	}
	
	for (turningPointIndex = 1;turningPointIndex < n;turningPointIndex++) {
		if (ovInfo->hfa4_lag[turningPointIndex - 1] < ovInfo->hfa4_lag[turningPointIndex]) break;
	}

	// ソート
	gsl_sort_index(ovInfo->hfa4_index,ovInfo->hfa4_lag,1,n);

	for (ofs = lowIndex;ofs < lowIndex + maxWidth;ofs++) {
		// 自己相関が高いindexから順に倍音成分を調べる
		for (i = n - 1;i >= 0;i--) {
			if (ovInfo->hfa4_index[i] >= turningPointIndex && ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] > 0.65 && ovInfo->hfa4_lag[ovInfo->hfa4_index[i]] < 0.8) {
				minDiff = -1;
				skipCnt = 0;
				
				window = ovInfo->hfa4_index[i];

				// スペクトル成分の遷移を調べる
				diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;	// 奇数倍のみ倍音がある
				refPw[0] = -1;
				refPw[1] = -1;
				refPw[2] = -1;
				refPw[3] = -1;
				refPw[4] = -1;
				refPw[5] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				ovInfo->sign[baseOfs] = 0;
				for (j = baseOfs,nn = 0;j < highIndex; j+= window,n++) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * j;
					if (hz < lowHz) {
						continue;
					}

					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[j] * hz;
						refPw[1] = ovInfo->power[j] * n;
						refPw[2] = ovInfo->power[j] * odd;
						refPw[3] = ovInfo->power[j] * (odd * odd);
						refPw[4] = ovInfo->power[j];
						refPw[5] = ovInfo->power[j] * (odd * odd * odd);
					}
					// 平均より大きい音声か、小さい音声か
					if (ovInfo->power[j] > ovInfo->base[j]) {
						ovInfo->sign[baseOfs]++;
					} else if (ovInfo->power[j] < ovInfo->base[j]) {
						ovInfo->sign[baseOfs]--;
					}
					
					// 前後のパワーの差の計算
					if (j > window) {
						if (j - window >= ofs) {
							if (ovInfo->power[j - window] >= ovInfo->power[j]) {
								diff = ovInfo->power[j - window] - ovInfo->power[j];
							} else {
								diff = ovInfo->power[j] - ovInfo->power[j - window];
							}
						}
					} else {
						continue;
					}
					diffP += (diff * tbl_hfaDiffMin[param->hfaDiffMin - 1]);

					avgPw += ovInfo->power[j];
					avgPwNX++;
					if ((avgPwNX & 0x01) == 0) {
						avgPw2 += ovInfo->power[j];
						avgPwNX2++;
					}
					if ((avgPwNX % 3) == 0) {
						avgPw3 += ovInfo->power[j];
						avgPwNX3++;
					}
					
					// 1/f(振幅が1/fになっているもの)
					diff = refPw[0] / hz;
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff0 += diff;

					// 鋸波(nの逆数で小さくなる)
					diff = refPw[1] / n;
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff1 += diff;

					// 短形波(奇数倍音,nの逆数で小さくなる)
					diff = refPw[2] / odd;
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff2 += diff;

					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					diff = refPw[3] / (odd * odd);
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff3 += diff;

					// パルス(n番目の倍音でもパワーは同じ)
					diff = refPw[4];
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff4 += diff;

					// その他(もっとパワーが小さくなるパターン)
					diff = refPw[5] / (odd * odd * odd);
					if (diff >= ovInfo->power[j]) {
						diff = diff - ovInfo->power[j];
					} else {
						diff = ovInfo->power[j] - diff;
					}
					diff5 += diff;

					nn++;
				}

				if (nn > 0) {
					diff0 /= nn;
					diff1 /= nn;
					diff2 /= nn;
					diff3 /= nn;
					diff4 /= nn;
					diff5 /= nn;
					diffP /= nn;
				}

				if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
					tmpAvgPw  = avgPw / avgPwNX;
					tmpAvgPw2 = avgPw2 / avgPwNX2;
					tmpAvgPw3 = avgPw3 / avgPwNX3;
					if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
					}
				}

				if (minDiff == -1 || minDiff > diffP) {
					minDiff = diffP;
					minWin = window;
					minType = 0;
				}
				if (minDiff > diff1) {
					minDiff = diff1;
					minWin = window;
					minType = 1;
				}
				if (minDiff > diff2) {
					minDiff = diff2;
					minWin = window;
					minType = 2;
				}
				if (minDiff > diff3) {
					minDiff = diff3;
					minWin = window;
					minType = 3;
				}
				if (minDiff > diff4) {
					minDiff = diff4;
					minWin = window;
					minType = 4;
				}
				if (minDiff > diff5) {
					minDiff = diff5;
					minWin = window;
					minType = 5;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある
			odd = 1;
			refPw[0] = -1;
			refPw[4] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}

			if (param->hfa3_sig2_print_disable == 0) {
				param->hfa3_sig2_print_disable = 1;
			}

			for (j = baseOfs;j < nSample;j += minWin,n++,odd+=2) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * j;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[j] * hz;
					refPw[1] = ovInfo->power[j] * n;
					refPw[2] = ovInfo->power[j] * odd;
					refPw[3] = ovInfo->power[j] * (odd * odd);
					refPw[4] = ovInfo->power[j];
					refPw[5] = ovInfo->power[j] * (odd * odd * odd);
					max_i = j;
					pha = ovInfo->phase[max_i];
				}

				if (minType == 0) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += 180;				// 位相が逆になる
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 1) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[1] / n;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[i] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 2) {
					// 短形波(奇数倍音,nの逆数で小さくなる)
					tmpPw = refPw[2] / odd;
					phaRand = 1;
					pha = ovInfo->phase[max_i];
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 3) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd);
					phaRand = 1;
	//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 4) {
					// パルス(n番目の倍音でもパワーは同じ)
					tmpPw = refPw[4];
					phaRand = rand() * 6;
					phaRand -= 3;
					pha += phaRand;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.18;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.18;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.18;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.18;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				} else if (minType == 5) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd * odd);
					phaRand = 1;
//							pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[j];
					if (ovInfo->pw[j] == 0) {
						ovInfo->pw[j] = tmpPw * 0.41;
						ovInfo->phase[j] = pha;
						ovInfo->diff[j] = minDiff;
					} else {
						if (ovInfo->diff[j] != -1 && ovInfo->diff[j] > minDiff) {
							ovInfo->pw[j] = tmpPw * 0.41;
							ovInfo->phase[j] = pha;
							ovInfo->diff[j] = minDiff;
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								ovInfo->pw[j] = tmpPw * 0.41;
								ovInfo->phase[j] = pha;
								ovInfo->diff[j] = minDiff;
							}
						}
					}
				}
			}
		}
	}

#if 0
	swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 600;
	for (i = 1;i < ovInfo->nSample;i+= swidth) {
		avg = 0;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			avg += ovInfo->pw[j];
		}
		avg /= n;
		for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
			if (ovInfo->pw[j] > avg) {
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
			} else {
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
				ovInfo->pw[j] = (ovInfo->pw[j] + avg) / 2;
			}
		}
	}
#endif
#endif
#if 0
	// 補間されていない箇所の対応
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
#endif
	// HFA3方式
	lowHz	= 8000;
	wid		= 3000;
	lowHz = param->p->hfa3_freq_start;
	wid   = param->p->hfa3_freq_len;
	step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
	if (lowHz + wid  >= ovInfo->validSamplingRate) {
		wid = ovInfo->validSamplingRate - lowHz;
		if (wid < 1500) {
			wid = 1500;
			lowHz = ovInfo->validSamplingRate - wid;
		}
	}
#if 0
	if (param->debug_print_disable == 0) {
		char s[100];
		sprintf(s,"hfa3_freq_start:%d",lowHz);
		PRINT_LOG("",s);
		sprintf(s,"hfa3_freq_len:%d",wid);
		PRINT_LOG("",s);
		sprintf(s,"step:%d",param->p->hfa3_analyze_step);
		PRINT_LOG("",s);
		param->debug_print_disable = 1;
	}
#endif
	swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 190;
	if (param->p->hfa3_version == 1) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else if (param->p->hfa3_version == 2) {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	} else {
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
	}
	width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
	minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
	if (ovInfo->validSamplingRate < 16000) {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
	} else {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
	}
	if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
		highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
	}

	if (swidth == 0) {
		swidth = 1;
	}
	if (step == 0) {
		step = 1;
	}
	avg = 0;
	for (i = lowIndex,n = 0;i < highIndex;i++) {
		avg += ovInfo->power[i];
		n++;
	}
	if (n > 0) {
		avg /= n;
	}
	avgLine = avg;
	if (1) {
		// 前後のwindowで振幅の差が少ない音声の補間
		window = width;
		minWin = window;
		minType = 0;
		for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
			minDiff = -1;
			skipCnt = 0;

			if (param->p->hfa3_version == 1) {
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;	// 20181007
			} else if (param->p->hfa3_version == 2) {
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 221;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 248;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 209;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 257;	// 20191024
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 273;	// 20191024
			} else {
				step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
			}

			diff_1st_2nd_3rd[0] = 0;
			diff_1st_2nd_3rd[1] = 0;
			diff_1st_2nd_3rd[2] = 0;
			diff_1st_2nd_3rd_count = 2;
			for (window = swidth;window < width;window+=step) {
//				step = 1;
				if (window < minWidth) {
					if ((ofs - lowIndex) > window * 1) {
						continue;
					}
				} else {
					if ((ofs - lowIndex) > window) {
						continue;
					}
				}
				skipCnt++;
				if (param->p->hfaFast && (skipCnt % 8) != 0) {
					continue;
				}

				// スペクトル成分の遷移を調べる
				diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
				baseOfs = ofs - ((ofs / window) * window);
				if (baseOfs == 0) {
					baseOfs = window;
				}
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;	// 奇数倍のみ倍音がある
				refPw[0] = -1;
				refPw[1] = -1;
				refPw[2] = -1;
				refPw[3] = -1;
				refPw[4] = -1;
				refPw[5] = -1;
				avgPw  = 0;
				avgPw2 = 0;
				avgPw3 = 0;
				avgPwNX  = 0;
				avgPwNX2 = 0;
				avgPwNX3 = 0;
				ovInfo->sign[baseOfs] = 0;
				for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
					double pp;
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}

					pp = ovInfo->power[i];
					pp = 20 * log10(pp);
					pp = pp - db_shift;
					pp = pp - window_correction;
					pp = pp - 190;
					if (diff_1st_2nd_3rd_count >= 0) {
						diff_1st_2nd_3rd_count--;
						diff_1st_2nd_3rd[0] = diff_1st_2nd_3rd[1];
						diff_1st_2nd_3rd[1] = diff_1st_2nd_3rd[2];
						diff_1st_2nd_3rd[2] = pp;

						if (diff_1st_2nd_3rd[0] != 0 && diff_1st_2nd_3rd[1] != 0 && diff_1st_2nd_3rd[2] != 0) {
							if (abs(diff_1st_2nd_3rd[0]) > abs(diff_1st_2nd_3rd[1])) {
								if ((abs(diff_1st_2nd_3rd[0]) - abs(diff_1st_2nd_3rd[1]) >= 11)) {
									continue;
								}
							} else {
								if ((abs(diff_1st_2nd_3rd[1]) - abs(diff_1st_2nd_3rd[0]) >= 11)) {
									continue;
								}
							}
							if (abs(diff_1st_2nd_3rd[1]) > abs(diff_1st_2nd_3rd[2])) {
								if ((abs(diff_1st_2nd_3rd[1]) - abs(diff_1st_2nd_3rd[2]) >= 11)) {
									continue;
								}
							} else {
								if ((abs(diff_1st_2nd_3rd[2]) - abs(diff_1st_2nd_3rd[1]) >= 11)) {
									continue;
								}
							}
						}
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						refPw[1] = ovInfo->power[i] * n;
						refPw[2] = ovInfo->power[i] * odd;
						refPw[3] = ovInfo->power[i] * (odd * odd);
						refPw[4] = ovInfo->power[i];
						refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					}
					// 平均より大きい音声か、小さい音声か
					if (ovInfo->power[i] > ovInfo->base[i]) {
						ovInfo->sign[baseOfs]++;
					} else if (ovInfo->power[i] < ovInfo->base[i]) {
						ovInfo->sign[baseOfs]--;
					}
					
					// 前後のパワーの差の計算
					if (i >= window) {
						if (i - window >= ofs) {
							if (ovInfo->power[i - window] >= ovInfo->power[i]) {
								diff = ovInfo->power[i - window] - ovInfo->power[i];
							} else {
								diff = ovInfo->power[i] - ovInfo->power[i - window];
							}
						}
					} else {
						continue;
					}
					diffP += (diff * tbl_hfaDiffMin[param->hfaDiffMin - 1]);

					avgPw += ovInfo->power[i];
					avgPwNX++;
					if ((avgPwNX & 0x01) == 0) {
						avgPw2 += ovInfo->power[i];
						avgPwNX2++;
					}
					if ((avgPwNX % 3) == 0) {
						avgPw3 += ovInfo->power[i];
						avgPwNX3++;
					}
					
					// 1/f(振幅が1/fになっているもの)
					diff = refPw[0] / hz;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff0 += diff;

					// 鋸波(nの逆数で小さくなる)
					diff = refPw[1] / n;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff1 += diff;

					// 短形波(奇数倍音,nの逆数で小さくなる)
					diff = refPw[2] / odd;
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff2 += diff;

					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					diff = refPw[3] / (odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff3 += diff;

					// パルス(n番目の倍音でもパワーは同じ)
					diff = refPw[4];
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff4 += diff;

					// その他(もっとパワーが小さくなるパターン)
					diff = refPw[5] / (odd * odd * odd);
					if (diff >= ovInfo->power[i]) {
						diff = diff - ovInfo->power[i];
					} else {
						diff = ovInfo->power[i] - diff;
					}
					diff5 += diff;

					nn++;
				}

//				diff0 += diffP * 2;
//				diff1 += diffP * 2;
//				diff2 += diffP * 2;
//				diff3 += diffP * 2;
//				diff4 += diffP * 2;
//				diff5 += diffP * 2;

				if (nn > 0) {
					diff0 /= nn;
					diff1 /= nn;
					diff2 /= nn;
					diff3 /= nn;
					diff4 /= nn;
					diff5 /= nn;
					diffP /= nn;
//					if (refPw[4] > 0) {
//						diff0 /= refPw[4];
//						diff1 /= refPw[4];
//						diff2 /= refPw[4];
//						diff3 /= refPw[4];
//						diff4 /= refPw[4];
//						diff5 /= refPw[4];
//						diffP /= refPw[4];
//					}
				}

				if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
					tmpAvgPw  = avgPw / avgPwNX;
					tmpAvgPw2 = avgPw2 / avgPwNX2;
					tmpAvgPw3 = avgPw3 / avgPwNX3;
					if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
					}
				}

				if (minDiff == -1 || minDiff > diffP) {
					minDiff = diffP;
					minWin = window;
					minType = 0;
				}
				if (minDiff > diff1) {
					minDiff = diff1;
					minWin = window;
					minType = 1;
				}
				if (minDiff > diff2) {
					minDiff = diff2;
					minWin = window;
					minType = 2;
				}
				if (minDiff > diff3) {
					minDiff = diff3;
					minWin = window;
					minType = 3;
				}
				if (minDiff > diff4) {
					minDiff = diff4;
					minWin = window;
					minType = 4;
				}
				if (minDiff > diff5) {
					minDiff = diff5;
					minWin = window;
					minType = 5;
				}
			}

			// 一番予測誤差が少なかったものを採用する。

			baseOfs = ofs - ((ofs / minWin) * minWin);
			if (baseOfs == 0) {
				baseOfs = minWin;
			}

			pha = ovInfo->phase[baseOfs];
			n = 1;		// 奇数偶数倍すべてに倍音がある
			odd = 1;
			refPw[0] = -1;
			refPw[4] = -1;
			if (minWin == swidth || minWin == width - 1) {
				continue;
			}

			if (param->hfa3_sig2_print_disable == 0) {
				param->hfa3_sig2_print_disable = 1;
//				PRINT_LOG("","hfa3 sig2 output");
			}
			for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
				hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
				if (hz < lowHz) {
					continue;
				}
				if (refPw[0] == -1) {
					refPw[0] = ovInfo->power[i] * hz;
					refPw[1] = ovInfo->power[i] * n;
					refPw[2] = ovInfo->power[i] * odd;
					refPw[3] = ovInfo->power[i] * (odd * odd);
					refPw[4] = ovInfo->power[i];
					refPw[5] = ovInfo->power[i] * (odd * odd * odd);
					max_i = i;
					pha = ovInfo->phase[max_i];
				}

				if (minType == 0) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[0] / hz;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							if (ovInfo->pw[i] == 0) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				} else if (minType == 1) {
					// 1/f(振幅が1/fになっているもの)
					tmpPw = refPw[1] / n;
					phaRand = 1;
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							if (ovInfo->pw[i] == 0) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				} else if (minType == 2) {
					// 短形波(奇数倍音,nの逆数で小さくなる)
					tmpPw = refPw[2] / odd;
					phaRand = 1;
					pha = ovInfo->phase[max_i];
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							if (ovInfo->pw[i] == 0) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				} else if (minType == 3) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					phaTmp = pha;
					if (n & 0x01) {
						phaTmp = pha + 180;
					}
					if (phaTmp >= 360) {
						phaTmp %= 360;
					}
					if (phaTmp < 0) {
						phaTmp += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							if (ovInfo->pw[i] == 0) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				} else if (minType == 4) {
					// パルス(n番目の倍音でもパワーは同じ)
					tmpPw = refPw[4];
					phaRand = rand() * 6;
					phaRand -= 3;
					pha += phaRand;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.18;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							if (ovInfo->pw[i] == 0) {
								ovInfo->pw[i] = tmpPw * 0.18;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.18;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.18;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				} else if (minType == 5) {
					// 三角波(奇数倍音,n^2の逆数で小さくなる)
					tmpPw = refPw[3] / (odd * odd * odd);
					phaRand = 1;
//					pha = ovInfo->phase[max_i];
					pha += param->p->sig2Phase;
					if (pha >= 360) {
						pha %= 360;
					}
					if (pha < 0) {
						pha += 360;
					}
					tmpPw2 = ovInfo->pw[i];
					if (ovInfo->pw[i] == 0) {
						ovInfo->pw[i] = tmpPw * 0.41;
						ovInfo->phase[i] = pha;
						ovInfo->diff[i] = minDiff;
					} else {
						if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
							if (ovInfo->pw[i] == 0) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
						}
						if (ovInfo->sign[baseOfs] > 8) {
							if (tmpPw2 < tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						} else if (ovInfo->sign[baseOfs] < 0) {
							if (tmpPw2 > tmpPw) {
								if (ovInfo->pw[i] == 0) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				}
			}
		}
		if (param->p->hfa3_version == 3 && param->p->hfa3_param_test[1] == 0) {
			param->debug_print_disable = 0;
			if (param->p->hfa3_freq_start != 0) {
				lowHz = param->p->hfa3_freq_start;
			}
			if (param->p->hfa3_freq_len != 0) {
				wid   = param->p->hfa3_freq_len;
			}
			if (lowHz + wid  >= ovInfo->validSamplingRate) {
				wid = ovInfo->validSamplingRate - lowHz;
				if (wid < 1500) {
					wid = 1500;
					lowHz = ovInfo->validSamplingRate - wid;
				}
			}
			step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
#if 0
			if (param->debug_print_disable == 0) {
				char s[100];
				sprintf(s,"2nd hfa3_freq_start:%d",lowHz);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				sprintf(s,"2nd hfa3_freq_len:%d",wid);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				sprintf(s,"2nd step:%d",param->p->hfa3_analyze_step);
				if (ovInfo->hfa3_log == 0) {
					PRINT_LOG("",s);
				}
				param->debug_print_disable = 1;
			}
#endif
			swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 177;
			width	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * wid;
			lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * lowHz;
			minWidth  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 1000;
			if (ovInfo->validSamplingRate < 16000) {
				char s[100];
				sprintf(s,"validSampingRate:%d",ovInfo->validSamplingRate);
				if (ovInfo->hfa3_log == 0) {
//					PRINT_LOG("",s);
				}
				highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
			} else {
				char s[100];
				sprintf(s,"validSampingRate:%d",ovInfo->validSamplingRate);
				if (ovInfo->hfa3_log == 0) {
//					PRINT_LOG("",s);
				}
				highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 16000;
			}
			if (param->p->hfa3_version >= 2 && param->p->hfa_analysis_limit != -1 && param->p->hfa_analysis_limit < (param->p->inSampleR / 2)) {
				char s[100];
				sprintf(s,"hfa_analysis_limit:%d",param->p->hfa_analysis_limit);
				if (ovInfo->hfa3_log == 0) {
//					PRINT_LOG("",s);
				}
				highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa_analysis_limit;
			}

			if (swidth == 0) {
				swidth = 1;
			}
			if (step == 0) {
				step = 1;
			}
			avg = 0;
			for (i = lowIndex,n = 0;i < highIndex;i++) {
				avg += ovInfo->power[i];
				n++;
			}
			if (n > 0) {
				avg /= n;
			}
			avgLine = avg;


			// 2nd
			if (ovInfo->hfa3_log == 0) {
				PRINT_LOG("","param->sig2Enb == 1");
			}
			// 前後のwindowで振幅の差が少ない音声の補間
			window = width;
			minWin = window;
			minType = 0;

			for (ofs = lowIndex;ofs < lowIndex + window;ofs++) {
				minDiff = -1;
				skipCnt = 0;

				if (param->p->hfa3_version == 1) {
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 77;	// 20181007
				} else if (param->p->hfa3_version == 2) {
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 221;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 248;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 209;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 257;	// 20191024
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 273;	// 20191024
				} else {
					step	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * param->p->hfa3_analyze_step;	// 20191024
				}

				for (window = swidth;window < width;window+=step) {
	//				step = 1;
					if (window < minWidth) {
						if ((ofs - lowIndex) > window * 1) {
							continue;
						}
					} else {
						if ((ofs - lowIndex) > window) {
							continue;
						}
					}
					skipCnt++;
					if (param->p->hfaFast && (skipCnt % 8) != 0) {
						continue;
					}

					// スペクトル成分の遷移を調べる
					diff0 = diff1 = diff2 = diff3 = diff4 = diff5 = diffP = 0;
					baseOfs = ofs - ((ofs / window) * window);
					if (baseOfs == 0) {
						baseOfs = window;
					}
					n = 1;		// 奇数偶数倍すべてに倍音がある
					odd = 1;	// 奇数倍のみ倍音がある
					refPw[0] = -1;
					refPw[1] = -1;
					refPw[2] = -1;
					refPw[3] = -1;
					refPw[4] = -1;
					refPw[5] = -1;
					avgPw  = 0;
					avgPw2 = 0;
					avgPw3 = 0;
					avgPwNX  = 0;
					avgPwNX2 = 0;
					avgPwNX3 = 0;
					ovInfo->sign[baseOfs] = 0;
					for (i = baseOfs,nn = 0;i < highIndex; i+= window,n++) {
						hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
						if (hz < lowHz) {
							continue;
						}

						if (refPw[0] == -1) {
							refPw[0] = ovInfo->power[i] * hz;
							refPw[1] = ovInfo->power[i] * n;
							refPw[2] = ovInfo->power[i] * odd;
							refPw[3] = ovInfo->power[i] * (odd * odd);
							refPw[4] = ovInfo->power[i];
							refPw[5] = ovInfo->power[i] * (odd * odd * odd);
						}
						// 平均より大きい音声か、小さい音声か
						if (ovInfo->power[i] > ovInfo->base[i]) {
							ovInfo->sign[baseOfs]++;
						} else if (ovInfo->power[i] < ovInfo->base[i]) {
							ovInfo->sign[baseOfs]--;
						}
						
						// 前後のパワーの差の計算
						if (i >= window) {
							if (i - window >= ofs) {
								if (ovInfo->power[i - window] >= ovInfo->power[i]) {
									diff = ovInfo->power[i - window] - ovInfo->power[i];
								} else {
									diff = ovInfo->power[i] - ovInfo->power[i - window];
								}
							}
						} else {
							continue;
						}
						diffP += (diff * tbl_hfaDiffMin[param->hfaDiffMin - 1]);

						avgPw += ovInfo->power[i];
						avgPwNX++;
						if ((avgPwNX & 0x01) == 0) {
							avgPw2 += ovInfo->power[i];
							avgPwNX2++;
						}
						if ((avgPwNX % 3) == 0) {
							avgPw3 += ovInfo->power[i];
							avgPwNX3++;
						}
						
						// 1/f(振幅が1/fになっているもの)
						diff = refPw[0] / hz;
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff0 += diff;

						// 鋸波(nの逆数で小さくなる)
						diff = refPw[1] / n;
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff1 += diff;

						// 短形波(奇数倍音,nの逆数で小さくなる)
						diff = refPw[2] / odd;
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff2 += diff;

						// 三角波(奇数倍音,n^2の逆数で小さくなる)
						diff = refPw[3] / (odd * odd);
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff3 += diff;

						// パルス(n番目の倍音でもパワーは同じ)
						diff = refPw[4];
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff4 += diff;

						// その他(もっとパワーが小さくなるパターン)
						diff = refPw[5] / (odd * odd * odd);
						if (diff >= ovInfo->power[i]) {
							diff = diff - ovInfo->power[i];
						} else {
							diff = ovInfo->power[i] - diff;
						}
						diff5 += diff;

						nn++;
					}

	//				diff0 += diffP * 2;
	//				diff1 += diffP * 2;
	//				diff2 += diffP * 2;
	//				diff3 += diffP * 2;
	//				diff4 += diffP * 2;
	//				diff5 += diffP * 2;

					if (nn > 0) {
						diff0 /= nn;
						diff1 /= nn;
						diff2 /= nn;
						diff3 /= nn;
						diff4 /= nn;
						diff5 /= nn;
						diffP /= nn;
	//					if (refPw[4] > 0) {
	//						diff0 /= refPw[4];
	//						diff1 /= refPw[4];
	//						diff2 /= refPw[4];
	//						diff3 /= refPw[4];
	//						diff4 /= refPw[4];
	//						diff5 /= refPw[4];
	//						diffP /= refPw[4];
	//					}
					}

					if (avgPwNX > 0 && avgPwNX2 > 0 && avgPwNX3 > 0) {
						tmpAvgPw  = avgPw / avgPwNX;
						tmpAvgPw2 = avgPw2 / avgPwNX2;
						tmpAvgPw3 = avgPw3 / avgPwNX3;
						if ((tmpAvgPw  - (tmpAvgPw / 10)) > tmpAvgPw2 || tmpAvgPw + (tmpAvgPw / 10) < tmpAvgPw2 || (tmpAvgPw2 - (tmpAvgPw2 / 10)) > tmpAvgPw3 || tmpAvgPw2 + (tmpAvgPw2 / 10) < tmpAvgPw3) {						continue;
						}
					}

					if (minDiff == -1 || minDiff > diffP) {
						minDiff = diffP;
						minWin = window;
						minType = 0;
					}
					if (minDiff > diff1) {
						minDiff = diff1;
						minWin = window;
						minType = 1;
					}
					if (minDiff > diff2) {
						minDiff = diff2;
						minWin = window;
						minType = 2;
					}
					if (minDiff > diff3) {
						minDiff = diff3;
						minWin = window;
						minType = 3;
					}
					if (minDiff > diff4) {
						minDiff = diff4;
						minWin = window;
						minType = 4;
					}
					if (minDiff > diff5) {
						minDiff = diff5;
						minWin = window;
						minType = 5;
					}
				}

				// 一番予測誤差が少なかったものを採用する。

				baseOfs = ofs - ((ofs / minWin) * minWin);
				if (baseOfs == 0) {
					baseOfs = minWin;
				}

				pha = ovInfo->phase[baseOfs];
				n = 1;		// 奇数偶数倍すべてに倍音がある
				odd = 1;
				refPw[0] = -1;
				refPw[4] = -1;
				if (minWin == swidth || minWin == width - 1) {
					continue;
				}

				if (param->hfa3_sig2_print_disable == 0) {
					param->hfa3_sig2_print_disable = 1;
					if (ovInfo->hfa3_log == 0) {
						PRINT_LOG("","hfa3 sig2 output");
					}
				}
				for (i = baseOfs;i < nSample;i += minWin,n++,odd+=2) {
					hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
					if (hz < lowHz) {
						continue;
					}
					if (refPw[0] == -1) {
						refPw[0] = ovInfo->power[i] * hz;
						refPw[1] = ovInfo->power[i] * n;
						refPw[2] = ovInfo->power[i] * odd;
						refPw[3] = ovInfo->power[i] * (odd * odd);
						refPw[4] = ovInfo->power[i];
						refPw[5] = ovInfo->power[i] * (odd * odd * odd);
						max_i = i;
						pha = ovInfo->phase[max_i];
					}

					if (minType == 0) {
						// 1/f(振幅が1/fになっているもの)
						tmpPw = refPw[0] / hz;
						phaRand = 1;
						pha += param->p->sig2Phase;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 1) {
						// 1/f(振幅が1/fになっているもの)
						tmpPw = refPw[1] / n;
						phaRand = 1;
						pha += param->p->sig2Phase;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 2) {
						// 短形波(奇数倍音,nの逆数で小さくなる)
						tmpPw = refPw[2] / odd;
						phaRand = 1;
						pha = ovInfo->phase[max_i];
						phaTmp = pha;
						if (n & 0x01) {
							phaTmp = pha + 180;
						}
						if (phaTmp >= 360) {
							phaTmp %= 360;
						}
						if (phaTmp < 0) {
							phaTmp += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 3) {
						// 三角波(奇数倍音,n^2の逆数で小さくなる)
						tmpPw = refPw[3] / (odd * odd);
						phaRand = 1;
	//					pha = ovInfo->phase[max_i];
						pha += param->p->sig2Phase;
						phaTmp = pha;
						if (n & 0x01) {
							phaTmp = pha + 180;
						}
						if (phaTmp >= 360) {
							phaTmp %= 360;
						}
						if (phaTmp < 0) {
							phaTmp += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 4) {
						// パルス(n番目の倍音でもパワーは同じ)
						tmpPw = refPw[4];
						phaRand = rand() * 6;
						phaRand -= 3;
						pha += phaRand;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.18;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.18;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.18;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.18;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					} else if (minType == 5) {
						// 三角波(奇数倍音,n^2の逆数で小さくなる)
						tmpPw = refPw[3] / (odd * odd * odd);
						phaRand = 1;
	//					pha = ovInfo->phase[max_i];
						pha += param->p->sig2Phase;
						if (pha >= 360) {
							pha %= 360;
						}
						if (pha < 0) {
							pha += 360;
						}
						tmpPw2 = ovInfo->pw[i];
						if (ovInfo->pw[i] == 0) {
							ovInfo->pw[i] = tmpPw * 0.41;
							ovInfo->phase[i] = pha;
							ovInfo->diff[i] = minDiff;
						} else {
							if (ovInfo->diff[i] != -1 && ovInfo->diff[i] > minDiff) {
								ovInfo->pw[i] = tmpPw * 0.41;
								ovInfo->phase[i] = pha;
								ovInfo->diff[i] = minDiff;
							}
							if (ovInfo->sign[baseOfs] > 8) {
								if (tmpPw2 < tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							} else if (ovInfo->sign[baseOfs] < 0) {
								if (tmpPw2 > tmpPw) {
									ovInfo->pw[i] = tmpPw * 0.41;
									ovInfo->phase[i] = pha;
									ovInfo->diff[i] = minDiff;
								}
							}
						}
					}
				}
			}
		}
	}

	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					ovInfo->phase[i] -= 360;
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			if (i - 2 >= 0 && i + 2 < nSample && ovInfo->pw[i - 2] > 0 && ovInfo->pw[i + 2] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-2] + ovInfo->pw[i+2]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-2] + ovInfo->phase[i+2]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
			if (ovInfo->pw[i - 1] > 0 && ovInfo->pw[i + 1] > 0) {
				ovInfo->pw[i] = (ovInfo->pw[i-1] + ovInfo->pw[i+1]) / 2;
				ovInfo->phase[i] = (ovInfo->phase[i-1] + ovInfo->phase[i+1]) / 2;
				if (ovInfo->phase[i] >= 360) {
					pha = ovInfo->phase[i];
					ovInfo->phase[i] = (pha % 360);
				}
				if (ovInfo->phase[i] < 0) {
					ovInfo->phase[i] += 360;
				}
			}
		}
	}
#if 0
	for (i = baseOfs;i + 1< nSample;i++) {
		hz = ((ovInfo->samplingRate / 2) / (double)ovInfo->nSample) * i;
		if (hz < lowHz) {
			continue;
		}
		if (ovInfo->pw[i] == 0) {
			ovInfo->pw[i] = ovInfo->noise_pw[i];
			ovInfo->phase[i] = 0 + (int)(rand() * (359 - 0 + 1.0) / (1.0 + RAND_MAX));
		}
	}
#endif
#if 1
	lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * (ovInfo->validSamplingRate - 5000);
	highIndex = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * (ovInfo->validSamplingRate - 1000);
	i = 1;
	do {
		for (j = i,n = 0,k = lowIndex;n < highIndex - lowIndex && j < ovInfo->nSample;j++,n++) {
			ovInfo->pw[j] += ovInfo->baseToAdj[k];
			k++;
			if (k > highIndex) {
				k = lowIndex;
			}
		}
		i += n;
	} while (i < ovInfo->nSample);
#endif

	if (1) {
		double nx;
		// 付加した広域で強すぎるものがあれば弱める。
		lowIndex  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * ovInfo->validSamplingRate;
		highIndex = nSample;
		swidth	  = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 5000;
		i = lowIndex;
		nx = 6.7;
		if (ovInfo->validSamplingRate <= 12000) {
			nx = 3.2;
		}
		do {
			avg = 0;
			for (j = i,n = 0;n < swidth && j < ovInfo->nSample;j++,n++) {
				avg += ovInfo->pw[j];
			}
			if (n > 0) {
				avg /= n;
			}
			for (j = i,n = 0;n < width && j < ovInfo->nSample;j++,n++) {
				if (avg * nx < ovInfo->pw[j]) {
					ovInfo->pw[j] *= avg * nx / ovInfo->pw[j];
				}
			}
			i = j;
		} while (i < ovInfo->nSample);
	}
	ovInfo->hfa3_log = 1;

	if (adjH_pw != NULL && adjH_diff != NULL && adjH_index != NULL) {
		int min_offset;
		double adj_nx;
		// 付加した倍音、周期がある音声の調整(100Kzごとに平均をとり調整をする
		swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 100;
		if (swidth < 1) swidth = 1;
		sprintf(sss,"adjH:swidth:%ld",swidth);
		PRINT_LOG("HFA4",sss);
		for (i = lowIndex;i < highIndex;i+= swidth) {
			avg = 0;
			for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
				avg += ovInfo->pw[j];
			}
			avg /= n;
			for (j = i,n = 0;n < swidth && j < ovInfo->nSample;n++) {
				adjH_pw[j] = avg;
				adjH_diff[j] = 0;
				adjH_index[j] = 0;
			}
		}
		PRINT_LOG("HFA4","adjH_pw init");
		// lowIndex の値と lowIndex + 500Hz ずらした位置との差分をとる
		// それを 2500 だけ累積する
		// 差分がいちばん少ないずれを採用する
		swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 500;
		step   = (long)swidth;
		if (step < 1) step = 1;
		swidth = ((double)ovInfo->nSample / (ovInfo->samplingRate / 2)) * 2500;
		if (swidth < 1) swidth = 1;
		sprintf(sss,"adjH to_end:swidth:%ld",swidth);
		PRINT_LOG("HFA4",sss);
		for (i = lowIndex;i < lowIndex + swidth;i++) {
			for (j = 0;j < (long)swidth;j++) {
				if (adjH_pw[i + j] <= adjH_pw[i + j + step]) {
					adjH_diff[i] += (adjH_pw[i + j + step] - adjH_pw[i + j]);
				} else {
					adjH_diff[i] += (adjH_pw[i + j] - adjH_pw[i + j + step]);
				}
			}
		}
		for (i = 0;i < swidth;i++) {
			adjH_diff[i] = adjH_diff[i + lowIndex];
			adjH_index[i] = adjH_index[i + lowIndex];
		}
		PRINT_LOG("HFA4","adjH_pw diff end");

		memset(adjH_index,0,sizeof (size_t) * nSample);
		gsl_sort_index(adjH_index,adjH_diff,1,highIndex - lowIndex);
		for (i = 0;i < highIndex - lowIndex;i++) {
			if (adjH_diff[adjH_index[i]] > 0) break;
		}
		if (i < highIndex - lowIndex) {
			PRINT_LOG("HFA4","adjust pw");
			// i には差が少ないindexが入ってる
			min_offset = adjH_index[i];
			// lowIndex から highIndex の間のpowerと生成したpowerを比べて音量が大きいのをカットする
			for (i = lowIndex;i < highIndex;i++) {
				for (j = highIndex;j < (double)ovInfo->nSample;j += min_offset) {
					if (ovInfo->pw[i] < ovInfo->pw[j]) {
						adj_nx = ovInfo->pw[i] / ovInfo->pw[j];
						ovInfo->pw[j] = ovInfo->pw[j] * adj_nx;
					}
				}
			}
		}
	}

	if (adjH_pw) free(adjH_pw);
	if (adjH_diff) free(adjH_diff);
	if (adjH_index) free(adjH_index);
	if (sss != NULL) free(sss);
	if (tstr != NULL) free(tstr);

	if (param->upconv == 1 && ovInfo->index == 1) {
		PRINT_FN_LOG(param->debug_upconv,"-anaOverToneHFA4:End");
	}
}
//---------------------------------------------------------------------------
// Function   : hfa4_calculate_mean
// Description: 自己相関で使用する平均の計算関数
// ---
//	data		:データ(power)
//  n           :データの個数
//
double hfa4_calculate_mean(double *data,int n)
{
	int i;
	double sum = 0;
	for (i = 0;i < n;i++) {
		sum += data[i];
	}
	
	if (n > 0) {
		sum = sum / n;
	}

    return sum;
}
//---------------------------------------------------------------------------
// Function   : hfa4_calculate_mean
// Description: 自己相関係数の計算(-1.0～1.0)
// ---
//	data		:データ(power)
//  n           :データの個数
//  lag         :ずらす量
//

// 自己相関係数を計算する関数
double hfa4_autocorrelation_coefficient(double *data,int n,int lag)
{
	double mean = hfa4_calculate_mean(data,n);

	double numerator = 0, denominator = 0;
	int i;
	for (i = 0;i < n - lag;i++) {
		numerator += (data[i] - mean) * (data[i + lag] - mean);
	}
	for (i = 0;i < n;i++) {
		denominator += (data[i] - mean) * (data[i] - mean);
	}
	return numerator / denominator;
}

//---------------------------------------------------------------------------
// Function   : noiseCut
// Description: ノイズカット処理
// ---
//	nfs		 	:ノイズカットオフ周波数(この周波数以上の領域のノイズをカットする)
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r		:入力ファイル
//	fp_w		:出力ファイル
//	param		:変換パラメータ
//
void noiseCut(long nfs,SSIZE inSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1,*mem2,*mem3;
	long hfc;
	long outSampleR;
	long wkMemSize;
	long fftSize,i,j,n,nn,h;
	long lowIndex,highIndex;
	long hz,idx;
	double percent,per;
	int ignore_flag;
	double p;
	static double refPw[10000];
	double *pw;
	double cutLV[5]={0.99,1.06,1.12,1.18,1.30};
	SSIZE *pIn,*pOut;
	SSIZE startInSample,nSample;
	fftw_complex *fftw_in,*fftw_out;
	fftw_plan fftw_p,fftw_ip;

	PRINT_FN_LOG(param->debug_upconv,"-noiseCut:Start");

   	outSampleR = param->p->outSampleR;
	if (param->p->hfc != -1) {
		hfc = param->p->hfc;
	} else {
		hfc = param->p->inSampleR / 2;
	}
	if (hfc > param->p->inSampleR / 2) {
		hfc = param->p->inSampleR / 2;
	}

	fio_rewind(fp_r);
	fio_rewind(fp_w);

	if ((outSampleR == 32000) || (outSampleR == 44100) || (outSampleR == 48000)) {
		fftSize = 4096 / 2;
		fftSize = outSampleR / 14;
		fftSize = outSampleR / 8;
		fftSize = outSampleR / 10;
	}
	if ((outSampleR == 44100 * 2) || (outSampleR == 48000 * 2)) {
		fftSize = 8192 / 2;
		fftSize = outSampleR / 14;
		fftSize = outSampleR / 8;
		fftSize = outSampleR / 10;
	}
	if ((outSampleR == 32000 * 6) || (outSampleR == 44100 * 4) || (outSampleR == 48000 * 4)) {
		fftSize = 16384 / 2;
		fftSize = outSampleR / 14;
		fftSize = outSampleR / 8;
		fftSize = outSampleR / 10;
	}
	if ((outSampleR == 32000 * 12) || (outSampleR == 44100 * 8) || (outSampleR == 48000 * 8)) {
		fftSize = 32768 / 2;
		fftSize = outSampleR / 14;
		fftSize = outSampleR / 8;
		fftSize = outSampleR / 10;
	}
	if ((outSampleR == 32000 * 24) || (outSampleR == 44100 * 16) || (outSampleR == 48000 * 16)) {
		fftSize = 65536 / 2;
		fftSize = outSampleR / 14;
		fftSize = outSampleR / 8;
		fftSize = outSampleR / 10;
	}
	if ((outSampleR == 32000 * 48) || (outSampleR == 44100 * 32) || (outSampleR == 48000 * 32) || (outSampleR == 44100 * 64) || (outSampleR == 48000 * 64)) {
		fftSize = 65536;
		fftSize = outSampleR / 14;
		fftSize = outSampleR / 8;
		fftSize = outSampleR / 10;
	}

	wkMemSize = (fftSize * 32) * sizeof (SSIZE);

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

	pw = (double *)malloc(fftSize*2 * sizeof (double));
	if (pw == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize*2*2);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize*2*2);
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

	per = -1;
	for (startInSample = ((fftSize + (fftSize / 2)) * -1);startInSample < inSample + (fftSize + (fftSize / 2));startInSample += fftSize) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		memset(mem1,0,wkMemSize);

		if (startInSample >= 0 && startInSample + (fftSize * 12) < inSample) {
			nSample = fftSize * 12;
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else {
			mem0 = mem1;
			nSample = fftSize * 12;
			if (startInSample >= 0) {
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r,0,SEEK_SET);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 12;
		}

		memset(mem2,0,wkMemSize);
		memset(mem3,0,wkMemSize);
		memset(pw,0,fftSize*2 * sizeof (double));

		pIn = (SSIZE *)mem1;
		for (n = 0;n < 12;n++) {
			// FFT 初期設定
			for (i = 0;i < fftSize;i++) {
				fftw_in[i][0] = pIn[((fftSize / 2) * n) + i];
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

			// 元信号のパワーを累積する
			for (i = 1;i < fftSize / 2;i++) {
				p = fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1];
				if (p != 0) {
					p = sqrt(p);
				}
				pw[i] += p;
			}
		}
#if 1
		for (i = 0;i < fftSize / 2;i++) {
			pw[i] /= 12;
		}
#endif
		for (i = 0,h = 0;h < hfc - 100;i++,h += 100) {
			// 100hz 範囲のパワーを調べる
			lowIndex  = ((double)fftSize / outSampleR) * h;
			highIndex = ((double)fftSize / outSampleR) * (h + 100);
			refPw[i] = 0;
			for (j = lowIndex,nn = 0;j < highIndex;j++,nn++) {
				refPw[i] += pw[j];
			}
			if (nn > 0) {
				refPw[i] /= nn;
			}
		}
		for (n = 0;n < 3;n++) {
			// FFT 初期設定
			for (i = 0;i < fftSize;i++) {
				fftw_in[i][0] = pIn[((fftSize / 2) * n) + i];
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
			// 閾値より大きい音はカットする
			ignore_flag = 0;
			for (i = 1;i < fftSize / 2;i++) {
				hz = ((double)outSampleR / fftSize) * i;
				if (pw[i] > (double)0x100000) {
					if (ignore_flag == 0 && hz >= 100 && hz >= param->p->nr) {
						idx = hz / 100;
						if (pw[i] >= refPw[idx] * cutLV[param->p->nr_option]) {
							fftw_out[i][0] = 0;
							fftw_out[i][1] = 0;
						} else {
							fftw_out[i][0] *= 0.45;
							fftw_out[i][1] *= 0.45;
						}
					} else {
						fftw_out[i][0] = 0;
						fftw_out[i][1] = 0;
					}
				} else {
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
			pOut = (SSIZE *)mem2;
			for (i = 0;i < fftSize;i++) {
				pOut[((fftSize / 2) * n) + i] += fftw_in[i][0] / fftSize;
			}
		}
		if (startInSample + fftSize / 2 >= 0) {
			outTempFile(fp_w,mem2 + fftSize / 2,fftSize,param);
		}
	}
	free(mem1);
	free(mem2);
	free(mem3);
	free(pw);
	fftw_destroy_plan(fftw_p);
	fftw_destroy_plan(fftw_ip);
	fftw_free(fftw_in);
	fftw_free(fftw_out);

	PRINT_FN_LOG(param->debug_upconv,"-noiseCut:End");

}
//---------------------------------------------------------------------------
// Function   : SideFreqAdjust
// Description: MP3/MP4 などの圧縮による特定の周波数の音の除去を検出し復元する処理
//              GenOverToneで生成した音をマージする
// ---
//	afs		 	:検出周波数(この周波数以上の領域の音を復元する)
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r		:入力ファイル
//	fp_w		:出力ファイル
//	param		:変換パラメータ
//
void SideFreqAdjust(SSIZE inSample,FIO *fp_r1,FIO *fp_r2,FIO *fp_w,PARAM_INFO *param,FFT_CUTINFO *cutinfo)
{
	SSIZE *mem0,*mem1,*mem2,*mem3;
	long hfc,sfa_sec;
	long sfc;
	long outSampleR;
	long wkMemSize;
	long fftSize,i,j,n,nn,h;
	long lowIndex,highIndex,widthCount;
	long rangeIndex;
	long hz,idx;
	long high_limit;
	long ignore_flag;
	int l10;
	double percent,per;
	double p,p1,p2,ref_pw,sfa_pw,nx;
	double shresh          = (double)919643518699930.750000;
	double shresh_diff     = (double)3452976130945.0;
	double *basePw;
	double *baseMaxPw;
	double *baseMinPw;

	int *basePw_l10;
	FILE *ms4_out;
	static int out_flag[200];

	SSIZE *pIn,*pOut;
	SSIZE startInSample,nSample;
	FFT_CUTINFO *cutinfo_work;
	char sss[300];
	fftw_complex *fftw_in_1,*fftw_out_1;
	fftw_plan fftw_p_1,fftw_ip_1;

	fftw_complex *fftw_in_2,*fftw_out_2;
	fftw_plan fftw_p_2,fftw_ip_2;

	PRINT_FN_LOG(param->debug_upconv,"-SideFreqAdjust:Start");

	PRINT_LOG("","1");
   	outSampleR = param->p->outSampleR;
	if (param->p->hfc != -1) {
		hfc = param->p->hfc;
	} else {
		hfc = param->p->inSampleR / 2;
	}
	if (hfc > param->p->inSampleR / 2) {
		hfc = param->p->inSampleR / 2;
	}

	ms4_out = fopen("d:\\out.ms4.txt","w");

	fio_rewind(fp_r1);
	fio_rewind(fp_r2);
	fio_rewind(fp_w);

	fftSize = outSampleR / 5;

	wkMemSize = fftSize * 2 * sizeof (SSIZE);

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

	basePw = (double *)malloc(fftSize * sizeof (double));
	if (basePw == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	baseMinPw = (double *)malloc(fftSize * sizeof (double));
	if (baseMinPw == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	baseMaxPw = (double *)malloc(fftSize * sizeof (double));
	if (baseMaxPw == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	basePw_l10 = (int *)malloc(fftSize * sizeof (int));
	if (basePw_l10 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_in_1 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_in_1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out_1 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_out_1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p_1 = fftw_plan_dft_1d(fftSize,fftw_in_1,fftw_out_1,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p_1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip_1 = fftw_plan_dft_1d(fftSize,fftw_out_1,fftw_in_1,FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip_1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_in_2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_in_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_out_2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_out_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_p_2 = fftw_plan_dft_1d(fftSize,fftw_in_2,fftw_out_2,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	fftw_ip_2 = fftw_plan_dft_1d(fftSize,fftw_out_2,fftw_in_2,FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip_2 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	PRINT_LOG("","2");

	per = -1;
	for (startInSample = ((fftSize + (fftSize / 2)) * -1);startInSample < inSample + (fftSize + (fftSize / 2));startInSample += fftSize) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				if (((int)percent % 10) == 0 || ((int)percent % 10) == 5 || percent >= 85) {
					if (param->print_stdout == 1) {
						fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
						fflush(stdout);
					}
				}
				if (chkAbort(percent,abortInfo.abort_check_interval) == 1) exit(0);
			}
			per = percent;
		}

		sfa_sec = startInSample / outSampleR;

		sfc = 0;
		cutinfo_work = cutinfo;
		while (cutinfo_work != NULL) {
			if (cutinfo_work->sec == sfa_sec && cutinfo_work->cut_count == 0) {
				sfc = cutinfo_work->sfc;
				break;
			}
			cutinfo_work = cutinfo_work->next;
		}
		if (sfc == 0) {
			sfc = outSampleR / 2;
		}

		memset(mem1,0,wkMemSize);
		memset(mem3,0,wkMemSize);

		if (startInSample >= 0 && startInSample + (fftSize * 2) < inSample) {
			nSample = fftSize * 2;
			fio_seek(fp_r1,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r1);

			fio_seek(fp_r2,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem3,sizeof (SSIZE),nSample,fp_r2);

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
			nSample = fftSize * 2;

			mem0 = mem3;
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

		memset(mem2,0,wkMemSize);

		for (n = 0;n < 3;n++) {

			ignore_flag = 1;

			pIn = (SSIZE *)mem1;
			// FFT 初期設定
			for (i = 0;i < fftSize;i++) {
				fftw_in_1[i][0] = pIn[((fftSize / 2) * n) + i];
				fftw_in_1[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSize - 1) / 2;i++) {
				fftw_in_1[i][0] = fftw_in_1[i][0] * (2.0 * i / (double)fftSize);
			}
			for (i = (fftSize - 1) / 2;i < fftSize;i++) {
				fftw_in_1[i][0] = fftw_in_1[i][0] * (2.0 - 2.0 * i / (double)fftSize);
			}

			// FFT
			fftw_execute(fftw_p_1);

			pIn = (SSIZE *)mem3;
			// FFT 初期設定
			for (i = 0;i < fftSize;i++) {
				fftw_in_2[i][0] = pIn[((fftSize / 2) * n) + i];
				fftw_in_2[i][1] = 0;
			}
			// 窓関数
			for (i = 0;i < (fftSize - 1) / 2;i++) {
				fftw_in_2[i][0] = fftw_in_2[i][0] * (2.0 * i / (double)fftSize);
			}
			for (i = (fftSize - 1) / 2;i < fftSize;i++) {
				fftw_in_2[i][0] = fftw_in_2[i][0] * (2.0 - 2.0 * i / (double)fftSize);
			}

			// FFT
			fftw_execute(fftw_p_2);

			memset(basePw,0,sizeof (double) * fftSize);
			memset(baseMinPw,0,sizeof (double) * fftSize);
			memset(baseMaxPw,0,sizeof (double) * fftSize);

			lowIndex  = ((double)fftSize / outSampleR) * (0);
			highIndex = ((double)fftSize / outSampleR) * (20000);
			widthCount = ((double)fftSize / outSampleR) * (500);
			do {
				sfa_pw = 0;
				hz = (outSampleR / (double)fftSize) * lowIndex;
				for (i = lowIndex,nn = 0;nn < widthCount && i + nn < highIndex;i++,nn++) {
					p1 = fftw_out_1[i][0] * fftw_out_1[i][0] + fftw_out_1[i][1] * fftw_out_1[i][1];
					if (p1 != 0) {
						p1 = sqrt(p1);
					}
					sfa_pw += p1;
				}
				if (nn > 0) {
					sfa_pw /= nn;
				}
				l10 = log10_lf(sfa_pw);
				if (hz == 0 && l10 > 15) {
					ignore_flag = 0;
				}
				if (sfa_sec < 10 && param->upconv == 1) {
					sprintf(sss,"SFA%03d[%05d],Pw:%lf,l10:%d",sfa_sec,hz,sfa_pw,l10);
					PRINT_LOG("",sss);
				}
				lowIndex += widthCount;
			} while (lowIndex < highIndex);

			if (ignore_flag == 0) {
				// SFC より上はカットする。
				if (sfc < hfc) {
					lowIndex  = ((double)fftSize / outSampleR) * (sfc);
				} else {
					lowIndex  = ((double)fftSize / outSampleR) * (hfc);
					sfc = hfc;
				}
				highIndex = ((double)fftSize / outSampleR) * (outSampleR / 2);
				for (i = lowIndex;i < highIndex;i++) {
					fftw_out_1[i][0] = 0;
					fftw_out_1[i][1] = 0;
				}

				// 平均を調べる
				highIndex = lowIndex;
				lowIndex   = 0;
				widthCount = ((double)fftSize / outSampleR) * (250);
				rangeIndex = ((double)fftSize / outSampleR) * (1500);

				do {
					double minPw,maxPw;
					sfa_pw = 0;
					minPw = maxPw = 0;

					hz = (outSampleR / (double)fftSize) * lowIndex;

					for (i = lowIndex,nn = 0;nn < widthCount && i + nn < highIndex;i++,nn++) {
						p1 = fftw_out_1[i][0] * fftw_out_1[i][0] + fftw_out_1[i][1] * fftw_out_1[i][1];
						if (p1 != 0) {
							p1 = sqrt(p1);
						}
						if (minPw == 0 || minPw > p1) {
							minPw = p1;
						}
						if (maxPw == 0 || maxPw < p1) {
							maxPw = p1;
						}

						sfa_pw += p1;
					}
					if (nn > 0) {
						sfa_pw /= nn;
					}

					if ((sfa_sec == 10 || sfa_sec == 185) && param->upconv == 1) {
						l10 = log10_lf(sfa_pw);
						sprintf(sss,"SFA_BASE%03d[%05d],basePw:%lf,l10:%d",sfa_sec,hz,sfa_pw,l10);
						PRINT_LOG("",sss);
					}
					for (i = lowIndex,nn = 0;nn < widthCount;i++,nn++) {
						basePw[i] = sfa_pw;
						baseMinPw[i] = minPw;
						baseMaxPw[i] = maxPw;
					}
					lowIndex += widthCount;
				} while (lowIndex < highIndex);

				for (i = 0;i < fftSize / 2;i++) {
					l10 = log10_lf(basePw[i]);
					basePw_l10[i] = l10;
				}
				
				// 音がカットされてしまっている部分のbasePwの修正(大きく落ち込んでいるため)。
				// これをもとに補間する音の音量を調整する。
				for (i = 0;i < fftSize / 2;i++) {
					if (basePw_l10[i] <= 16 && i - rangeIndex > 0 && i + rangeIndex < highIndex) {
						if (basePw[i] < ((basePw[i - rangeIndex] + basePw[i + rangeIndex]) / 2)) {
							basePw[i] = ((basePw[i - rangeIndex] + basePw[i + rangeIndex]) / 2);
						}
					}
				}

				if ((sfa_sec == 10 || sfa_sec == 185 || sfa_sec == 120 || sfa_sec == 167) && param->upconv == 1) {
					for (i = 0;i < fftSize / 2;i++) {
						hz = (outSampleR / (double)fftSize) * i;
						l10 = log10_lf(sfa_pw);
						sprintf(sss,"SFA%03d[%05d],basePw:%lf,Pw:%lf,minPw:%lf,maxPw:%lf,l10:%d",sfa_sec,hz,basePw[i],p1,baseMinPw[i],baseMaxPw[i],l10);
						PRINT_LOG("",sss);
					}
				}

				cutinfo_work = cutinfo;
				while (cutinfo_work != NULL) {
					if (cutinfo_work->sec == sfa_sec && cutinfo_work->cut_count > 0) {

						// 補間するかを判定する
						lowIndex  = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start);
						highIndex = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start + cutinfo_work->cut_count);
						ref_pw = 0;
						for (i = lowIndex,nn = 0;i < highIndex;i++,nn++) {
							p1 = fftw_out_1[i][0] * fftw_out_1[i][0] + fftw_out_1[i][1] * fftw_out_1[i][1];
							if (p1 != 0) {
								p1 = sqrt(p1);
							}
							ref_pw += p1;
						}
						if (nn > 0) {
							ref_pw /= nn;
						}

						if (ref_pw < shresh || cutinfo_work->cut_count > 100) {
							lowIndex  = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start - 500);
							if (cutinfo_work->cut_count <= 100) {
								highIndex = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start + cutinfo_work->cut_count);
							} else {
								if (hfc >= 7000 && hfc < cutinfo_work->cut_start) {
									lowIndex  = ((double)fftSize / outSampleR) * (hfc - 1000);
									highIndex = ((double)fftSize / outSampleR) * (hfc);
								} else {
									lowIndex  = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start - 1000);
									highIndex = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start);
								}
							}
							ref_pw = sfa_pw = 0;
							for (i = lowIndex,nn = 0;i < highIndex;i++,nn++) {
								p = fftw_out_1[i][0] * fftw_out_1[i][0] + fftw_out_1[i][1] * fftw_out_1[i][1];
								if (p != 0) {
									p = sqrt(p);
								}
								ref_pw += p;

								p = fftw_out_2[i][0] * fftw_out_2[i][0] + fftw_out_2[i][1] * fftw_out_2[i][1];
								if (p != 0) {
									p = sqrt(p);
								}
								sfa_pw += p;
							}
							if (nn > 0) {
								ref_pw /= nn;
								sfa_pw /= nn;
							}
							if (sfa_pw > 0) {
								nx = (ref_pw / sfa_pw);
								if (log10_lf(ref_pw) <= 16) {
									nx = (basePw[lowIndex] / sfa_pw);
								}
							} else {
								nx = 1;
							}
							nx = nx * 0.88;
							if (hfc >= 7000 && hfc < cutinfo_work->cut_start && cutinfo->cut_count > 100) {
								lowIndex  = ((double)fftSize / outSampleR) * (hfc - 500);
							} else {
								lowIndex  = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start);
							}
							highIndex = ((double)fftSize / outSampleR) * (cutinfo_work->cut_start + cutinfo_work->cut_count);

							memset(out_flag,0,sizeof (int) * 200);

							for (i = lowIndex;i < highIndex;i++) {
								p1 = fftw_out_1[i][0] * fftw_out_1[i][0] + fftw_out_1[i][1] * fftw_out_1[i][1];
								if (p1 != 0) {
									p1 = sqrt(p1);
								}
								p2 = fftw_out_2[i][0] * fftw_out_2[i][0] + fftw_out_2[i][1] * fftw_out_2[i][1];
								if (p2 != 0) {
									p2 = sqrt(p2);
								}
								hz = (outSampleR / (double)fftSize) * i;

								if (hz < sfc) {
									if (hz >= 7000 && (basePw_l10[i] <= 15 || (basePw[i] - p1) > shresh_diff) || (hz >= hfc)) {
										fftw_out_1[i][0] = fftw_out_2[i][0] * nx;
										fftw_out_1[i][1] = fftw_out_2[i][1] * nx;
										fftw_out_1[i][0] *= 0.4;
										fftw_out_1[i][1] *= 0.4;
										p1 = fftw_out_1[i][0] * fftw_out_1[i][0] + fftw_out_1[i][1] * fftw_out_1[i][1];
										if (p1 > 0) {
											p1 = sqrt(p1);
										}

										if (hz < 20000 && ms4_out != NULL && hz < 14000) {
											if (out_flag[hz / 100] == 0) {
												sprintf(sss,"SFA%03d[%05d],%lf\n",sfa_sec,hz,p1);
												fprintf(ms4_out,sss);
												out_flag[hz / 100] = 1;
											}
										}
									}
								} else {
									fftw_out_1[i][0] = fftw_out_2[i][0] * (ref_pw / sfa_pw);
									fftw_out_1[i][1] = fftw_out_2[i][1] * (ref_pw / sfa_pw);
								}
							}

							for (i = lowIndex;i < highIndex;i++) {
								p1 = fftw_out_1[i][0] * fftw_out_1[i][0] + fftw_out_1[i][1] * fftw_out_1[i][1];
								if (p1 != 0) {
									p1 = sqrt(p1);
								}
								hz = (outSampleR / (double)fftSize) * i;

								if (hz < sfc) {
									if (hz >= 7000 && (hz < hfc)) {
										if (log10_lf(p1) <= 15) {
											fftw_out_1[i][0] = fftw_out_2[i][0] * 10;
											fftw_out_1[i][1] = fftw_out_2[i][1] * 10;
										}
									}
								}
							}
						}
					}
					cutinfo_work = cutinfo_work->next;
				}
			}

			// 半分のデータを復元
			for (i = 1;i < fftSize / 2;i++) {
				fftw_out_1[fftSize - i][0] = fftw_out_1[i][0];
				fftw_out_1[fftSize - i][1] = fftw_out_1[i][1] * -1;
			}
			fftw_out_1[0][0] = 0;
			fftw_out_1[0][1] = 0;

			// invert FFT
			fftw_execute(fftw_ip_1);

			// 出力
			pOut = (SSIZE *)mem2;
			for (i = 0;i < fftSize;i++) {
				pOut[((fftSize / 2) * n) + i] += fftw_in_1[i][0] / fftSize;
			}
		}
		if (startInSample + fftSize / 2 >= 0) {
			outTempFile(fp_w,mem2 + fftSize / 2,fftSize,param);
			if (param->err) {
				PRINT_LOG("","ERR");
			}
		}
	}

	free(mem1);
	free(mem2);
	free(mem3);
	free(basePw);
	free(baseMinPw);
	free(baseMaxPw);
	free(basePw_l10);
	fftw_destroy_plan(fftw_p_1);
	fftw_destroy_plan(fftw_ip_1);
	fftw_free(fftw_in_1);
	fftw_free(fftw_out_1);

	fftw_destroy_plan(fftw_p_2);
	fftw_destroy_plan(fftw_ip_2);
	fftw_free(fftw_in_2);
	fftw_free(fftw_out_2);

	if (ms4_out != NULL) {
		fclose(ms4_out);
	}

	PRINT_FN_LOG(param->debug_upconv,"-SideFreqAdjust:End");
}

//---------------------------------------------------------------------------
// Function   : adjVLA
// Description: VLA が Auto のときに音量を下げる
// ---
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r		:入力ファイル
//  u_nx		:upconv 音量調整値
//  nx			:音量調整値(1.0～0.5)
//  clip_rate	:clip 率
//	param		:変換パラメータ
//
void adjVLA(SSIZE inSample,FIO *fp_r,double u_nx,double nx,double *clip_rate,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1;
	SSIZE val,v[3],diff;
	long wkMemSize;
	long outSampleR;
	SSIZE *pIn;
	SSIZE i,ii,iii,startInSample,nSample;
	SSIZE outRemain;
	SSIZE clip_found,clip_count;
	char s[300];

	PRINT_FN_LOG(param->debug_upconv,"-adjVLA:Start");
	fio_rewind(fp_r);

	outSampleR = param->p->outSampleR;
	wkMemSize = outSampleR * 2;

	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	outRemain = outSampleR;
	clip_found = clip_count = 0;
	v[0] = v[1] = v[2] = 0;

	for (startInSample = 0;startInSample < inSample;startInSample += wkMemSize) {
		memset(mem1,0,wkMemSize * sizeof (SSIZE));
		nSample = wkMemSize;

		if (startInSample >= 0 && startInSample + nSample < inSample) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = wkMemSize;
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else if (startInSample < inSample) {
			mem0 = mem1;
			nSample = 0;
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = inSample - startInSample;
			if (nSample > 0) {
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = wkMemSize;
		}

		pIn  = (SSIZE *)mem1;
		for (i = 0;i < nSample;i++) {
			val = (SSIZE)(((double)pIn[i] * u_nx) * nx);
			if (val > (SSIZE)0x007FFFFFFFFFFFFF) {
				clip_found++;
			}
			if (val < (SSIZE)0x007FFFFFFFFFFFFF * -1) {
				clip_found++;
			}
			if (val < 0) val = val * -1;
			v[0] = v[1];
			v[1] = v[2];
			v[2] = val;
			//v[2] = v[2] >> (56 - 16);
			if (v[0] < v[1] && v[1] > v[2]) {
				diff = v[1] - ((v[0] + v[2]) / 2);
			}
			if (v[0] > v[1] && v[1] < v[2]) {
				diff = ((v[0] + v[2]) / 2) - v[1];
			}
			if (diff < 0) diff = diff * -1;

			for (ii = 0;ii < 10;ii++) {
				if (param->diff_max[ii] <= diff) break;
			}
			if (ii < 10 && param->diff_max[ii] != diff) {
				for (iii = 8;iii >= ii;iii--) {
					param->diff_max[iii + 1] = param->diff_max[iii];
				}
				param->diff_max[ii] = diff;
			}
			if (v[0] < v[1] && v[1] < v[2]) {
				diff = v[1] - v[0];
			}
			if (v[0] > v[1] && v[1] > v[2]) {
				diff = v[0] - v[1];
			}
			for (ii = 0;ii < 10;ii++) {
				if (param->step_max[ii] <= diff) break;
			}
			if (ii < 10 && param->step_max[ii] != diff) {
				for (iii = 8;iii >= ii;iii--) {
					param->step_max[iii + 1] = param->step_max[iii];
				}
				param->step_max[ii] = diff;
			}

			clip_count++;
		}
	}
	sprintf(s,"adjVLA(%lf),clip_found:%lld,clip_count:%lld",nx,clip_found,clip_count);
	PRINT_LOG("",s);
	*clip_rate = clip_found;
	*clip_rate /= clip_count / (outSampleR * 10);
	al_free(mem1);

	PRINT_FN_LOG(param->debug_upconv,"-adjVLA:End");
}
//---------------------------------------------------------------------------
// Function   : adjClickNoise
// Description: ひげのようなクリックノイズを除去する
// ---
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r		:入力ファイル
//	fp_w		:出力ファイル
//  u_nx		:upconv 音量調整値
//	param		:変換パラメータ
//
void adjClickNoise(SSIZE inSample,FIO *fp_r,FIO *fp_w,double u_nx,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1;
	SSIZE val,v[3],diff;
	long wkMemSize;
	long outSampleR;
	SSIZE *pIn;
	SSIZE i,ii,iii,startInSample,nSample;
	SSIZE outRemain;
	SSIZE clip_found,clip_count;
	char s[300];

	PRINT_FN_LOG(param->debug_upconv,"-adjClickNoise:Start");

	fio_rewind(fp_r);

	outSampleR = param->p->outSampleR;
	nSample = wkMemSize = outSampleR * 5;

	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	outRemain = outSampleR;
	clip_found = clip_count = 0;
	v[0] = v[1] = v[2] = 0;

	for (startInSample = 0;startInSample < inSample;startInSample += wkMemSize) {
		memset(mem1,0,wkMemSize * sizeof (SSIZE));

		if (startInSample >= 0 && startInSample + nSample < inSample) {
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else {
			nSample = 0;
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			nSample = inSample - startInSample;
			if (nSample > 0) {
				fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
			}
			//nSample = wkMemSize;
		}

		pIn  = (SSIZE *)mem1;
		for (i = 0;i < nSample;i++) {
			val = (SSIZE)(((double)pIn[i] * u_nx));
			if (val < 0) val = val * -1;
			v[0] = v[1];
			v[1] = v[2];
			v[2] = val;
			if (v[0] < v[1] && v[1] > v[2]) {
				diff = v[1] - ((v[0] + v[2]) / 2);
			}
			if (v[0] > v[1] && v[1] < v[2]) {
				diff = ((v[0] + v[2]) / 2) - v[1];
			}
			if (diff < 0) diff = diff * -1;

			if (diff >> (56 - 16) > 4000) {
				if (i > 1 && i < nSample) {
					pIn[i - 1] = (pIn[i - 1] + pIn[i - 2] + pIn[i - 2] + pIn[i - 2] + pIn[i - 2] + pIn[i - 2] + pIn[i - 2] + pIn[i] + pIn[i] + pIn[i] + pIn[i] + pIn[i] + pIn[i]) / 13;
				}
			}
		}
		if (nSample > 0) {
			fio_write(mem1,sizeof (SSIZE),nSample,fp_w);
		}
	}

	al_free(mem1);
	PRINT_FN_LOG(param->debug_upconv,"-adjClickNoise:End");
}
//---------------------------------------------------------------------------
// Function   : spAnalyze
// Description: スピーカーの周波数特性に応じて調整値パラメーターを出力する
// ---
//	inSample 	:処理するサンプル数(ch毎)
//	fp_r		:入力ファイル
//	param		:変換パラメータ
//
void spAnalyze(SSIZE inSample,FIO *fp_r,PARAM_INFO *param)
{
	SSIZE *mem1,*mem0;
	long outSampleR;
	long inSampleR;
	long wkMemSize;
	long fftSize,i,n,hz;
	long validIndex,adjIndex,adjWidth;
	double percent,per;
	double div,step;
	long cnt;
	double p;
	static double adjData[192000];
	SSIZE *pIn;
	SSIZE startInSample,nSample;
	fftw_complex *fftw_in;
	fftw_plan fftw_p;
	double *adjFrom,*adjTo,*adjNx;
	FILE *ofp;
	outSampleR = param->p->outSampleR;
	inSampleR = param->p->inSampleR;

	PRINT_FN_LOG(param->debug_upconv,"-spAnalyze:Start");

	fio_rewind(fp_r);

	fftSize = param->p->outSampleR * 2;

	wkMemSize = (fftSize * 2) * sizeof (SSIZE);

	validIndex = ((double)fftSize / param->p->outSampleR) * (inSampleR / 2);
	adjIndex   = ((double)fftSize / param->p->outSampleR) * (2000);
	adjWidth   = ((double)fftSize / param->p->outSampleR) * (1000);
	if (validIndex < adjIndex) {
		adjIndex = validIndex;
	}

	adjFrom = (double *)malloc(sizeof (double) * fftSize);
	adjTo	= (double *)malloc(sizeof (double) * fftSize);
	adjNx	= (double *)malloc(sizeof (double) * fftSize);
	if (adjFrom == NULL || adjTo == NULL || adjNx == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}
	for (i = 0;i < fftSize;i++) {
		adjFrom[i] = 0;
		adjTo[i] = 0;
		adjNx[i] = 0;
	}
	
	mem1 = (SSIZE *)malloc(wkMemSize);
	if (mem1 == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	if (fftw_in == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fftw_p = fftw_plan_dft_1d(fftSize,fftw_in,fftw_in,FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	per = -1;
	cnt = 0;
	for (startInSample = ((fftSize + (fftSize / 2)) * -1);startInSample < inSample + fftSize + (fftSize / 2);startInSample += fftSize) {
		if (startInSample >= 0 && startInSample < inSample) {
			percent = ((double)startInSample / inSample);
			percent *= 100;
			percent = (double)((int)percent);
			if (percent != per) {
				if (param->print_stdout == 1) {
					fprintf(stdout,"%s%d%%\n",param->pr_str,(int)percent);
					fflush(stdout);
				}
			}
			per = percent;
//			Sleep(1);
		}

		memset(mem1,0,wkMemSize);

		if (startInSample >= 0 && startInSample + (fftSize * 2) < inSample) {
			nSample = fftSize * 2;
			fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			fio_read(mem1,sizeof (SSIZE),nSample,fp_r);
		} else {
			mem0 = mem1;
			nSample = fftSize * 2;
			if (startInSample >= 0) {
				fio_seek(fp_r,startInSample * sizeof (SSIZE),SEEK_SET);
			} else {
				fio_seek(fp_r,0,SEEK_SET);
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
				fio_read(mem0,sizeof (SSIZE),nSample,fp_r);
			}
			nSample = fftSize * 2;
		}

		pIn = (SSIZE *)mem1;
		for (n = 0;n < 3;n++) {
			// FFT 初期設定
			copyToFFTW(fftw_in,&pIn[((fftSize / 2) * n)],fftSize);

			// 窓関数
			windowFFTW(fftw_in,fftSize);

			// FFT
			fftw_execute(fftw_p);
			
			for (i = 1;i < validIndex;i++) {
				adjTo[i] = fftw_in[i][0] * fftw_in[i][0] + fftw_in[i][1] * fftw_in[i][1];
				if (adjTo[i] != 0) {
					adjTo[i] = sqrt(adjTo[i]);
				}
			}
			
			// 128 サイズの移動平均
			for (i = 1;i + 128 < validIndex;i++) {
				p = 0;
				for (n = 0;n < 128;n++) {
					p += adjTo[i + n];
				}
				if (p > 0) {
					p /= 128;
				}
				adjFrom[i] = p;
			}
			for (;i < fftSize / 2;i++) {
				adjFrom[i] = p;
			}

			p = 0;
			for (i = 1;i < 101;i++) {
				p += adjFrom[i];
			}
			p /= 100;
			
			adjTo[0] = 0;
			for (i = 1;i < fftSize / 2;i++) {
				adjTo[i] = p;
			}
			for (i = 1;i < fftSize / 2;i++) {
				if (adjFrom[i] != 0) {
					adjNx[i] += (adjTo[i] / adjFrom[i]);
				}
			}
			cnt++;
		}
	}
	if (cnt > 0) {
		for (i = 0;i < fftSize / 2;i++) {
			adjNx[i] /= cnt;
		}
	}
	
	p = 0;
	for (i = adjIndex,n = 0;n < adjWidth;adjIndex++,n++) {
		p += adjNx[i];
	}
	if (n > 0) {
		p /= n;
	}
	for (i = 1;i < adjIndex;i++) {
		adjNx[i] = p;
	}
	div = 1.0;
	step = 1.0 / 192000;
	for (i = adjIndex;i < fftSize / 2;i++) {
		adjNx[i] *= div;
		if (div - step > 0) {
			div -= step;
		} else {
			div = 0.08;
		}
	}

	if (param->r1_flag == 1) {
		unlink(param->sp_path);
		ofp = fopen(param->sp_path,"w");
		if (ofp) {
			for (i = 0;i < fftSize / 2;i++) {
				hz = ((double)param->p->outSampleR / fftSize) * i;
				adjData[hz] = adjNx[i];
			}
			for (i = 1;i < 192000;i++) {
				fprintf(ofp,"%lf\n",adjData[i]);
			}
			fclose(ofp);
		}
	}
	free(adjFrom);
	free(adjTo);
	free(adjNx);

	free(mem1);

	fftw_destroy_plan(fftw_p);
	fftw_free(fftw_in);

	PRINT_FN_LOG(param->debug_upconv,"-spAnalyze:End");
}

//---------------------------------------------------------------------------
// Function   : outTempFile
// Description: データをテンポラリファイルへ出力する
// ---
//	fp_w	:出力ファイル
//	in		:データのアドレス
//	size	:データー数
//	param	:パラメーター
//
void outTempFile(FIO *fp_w,void *in,SSIZE size,PARAM_INFO *param)
{
	fio_size r;
//	PRINT_FN_LOG(param->debug_upconv,"-outTempFile:Start");

	r = fio_write(in,1,size * sizeof (SSIZE),fp_w);
	if (r != size * sizeof (SSIZE)) {
		param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
	}

//	PRINT_FN_LOG(param->debug_upconv,"-outTempFile:End");
}
//---------------------------------------------------------------------------
// Function   : normalNoise
// Description: 正規乱数生成
//
double normalNoise(void)
/*
 * 正規乱数
 */
{
	double x1,x2;

	x1 = (double)rand() / RAND_MAX;
	x1 = 0.99999 * x1 + 0.00001;
	x2 = (double)rand() / RAND_MAX;
	return sqrt(-log(x1)) * cos(2.0 * 3.1415926 * x2) / 3.5;
}
//---------------------------------------------------------------------------
// Function   : adjPinkFilter
// Description: 1/f 特性にするフィルター
// ---
//	mode	  :モード(0,1,2,3)
//	fftSizeOut:FFT数
//	fftw_out2 :FFTW OUT 変数
//	param	  :変換パラメータ
//
void adjPinkFilter(int mode,long fftSizeOut,fftw_complex *fftw_out2,PARAM_INFO *param)
{
	long i;
	long startIdx,endIdx;
	double hz,div,step,old_div;
	int adj_disable;
	long cutOff;
	long outSampleR;

	if (param->p->hfa3_version == 1 || param->p->hfa3_pp == 1 || param->p->hfa == 4) {
		adjPinkFilter_0774(mode,fftSizeOut,fftw_out2,param);
		return;
	}

	PRINT_FN_LOG(param->debug_upconv,"-adjPinkFilter:Start");

	adj_disable = 0;
	outSampleR = param->p->outSampleR;
	if (mode == 4 && param->p->lpf != -1) {
		startIdx = ((double)fftSizeOut / outSampleR) * param->p->lpf;
		endIdx	 = ((double)fftSizeOut / outSampleR) * (param->p->lpf + 20000);
		step = 1.00 / ((endIdx - startIdx) * 1.55);

		div = 1;
		for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
			if (div - step > 0) {
		   		div -= step;
			} else {
		   		div = 0.01;
			}
		}
		for (;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
		}
		div = 1;
		for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
			if (div - step > 0) {
		   		div -= step;
			} else {
		   		div = 0.01;
			}
		}
		for (;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
		}
		return;
	}

	if (mode == 1) {
		// 1/f 特性にするフィルター(hfa1)
		for (i = 1;i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			if (hz > 0) {
				fftw_out2[i][0] /= hz;
				fftw_out2[i][1] /= hz;
			}
		}
	}
	if (param->p->hfa == 4) {
		for (i = 1;i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			if (hz > 0) {
				fftw_out2[i][0] /= ((double)hz * 1.38);
				fftw_out2[i][1] /= ((double)hz * 1.38);
			}
		}
		div = 1;
		step = (double)1.00 / 140000;
		for (i = 1;i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			if (hz > 60000) {
				fftw_out2[i][0] *= div;
				fftw_out2[i][1] *= div;
				if (div - step > 0) {
			   		div -= step;
				} else {
					div = 0.01;
				}
			}
		}
		return;
	}

	if (mode != 3) {
		// hfa1、hfa2、hfa3用の高域補間時の周波数調整
		if (param->p->hfa != 0 && param->p->hfc >= 8000 && param->p->hfc <= 23000) {
			startIdx = ((double)fftSizeOut / outSampleR) * 13000;
			endIdx	 = ((double)fftSizeOut / outSampleR) * 19000;
			step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter1);
			step *= 1;
			div = 1;
			adj_disable = 0;
			if (adj_disable == 0) {
				for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
					hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
					if (hz >= (param->p->hfc + 1500)) {
						fftw_out2[i][0] *= div;
						fftw_out2[i][1] *= div;
						if (div - step > 0) {
					   		div -= step;
						} else {
					   		div = 0.01;
						}
					}
				}
				for (;i < fftSizeOut / 2;i++) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
				}
			}
			startIdx = ((double)fftSizeOut / outSampleR) * 17000;	// 0.8.0(TEST2)
			endIdx	 = ((double)fftSizeOut / outSampleR) * 25000;
			step = 1.00 / ((endIdx - startIdx) * 1.50);				// 0.8.0(TEST2)
			step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter2);	// 0.8.0(TEST2)
			step *= 1.0;				// 0.8.0(TEST2)
			adj_disable = 0;
			div = 1;
			if (adj_disable == 0) {
				for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
					hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
					if (hz >= (param->p->hfc + 1500)) {
						fftw_out2[i][0] *= div;
						fftw_out2[i][1] *= div;
						if (div - step > 0) {
					   		div -= step;
						} else {
					   		div = 0.01;
						}
					}
				}
				old_div = div;
				div = 1.0 - ((1.0 - div) / 2.618);
				for (;i < fftSizeOut / 2;i++) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
				}
			}
			if (outSampleR / 2 <= 24000) {
				adj_disable = 1;
			} else if (outSampleR / 2 <= 48000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 24000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 44000;
				step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter3);
				step *= 1.11;
				adj_disable = 0;
			} else if (outSampleR / 2 <= 96000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 24000;	// 0.8.0(TEST2)
				endIdx	 = ((double)fftSizeOut / outSampleR) * 44000;
				step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter3);				// 0.8.0(TEST2)
				step *= 1.11;
				adj_disable = 0;
			} else if (outSampleR / 2 <= 192000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 24000;	// 0.8.0(TEST2)
				endIdx	 = ((double)fftSizeOut / outSampleR) * 44000;
				step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter3);				// 0.8.0(TEST2)
				step *= 1.11;
				adj_disable = 0;
			} else {
				startIdx = ((double)fftSizeOut / outSampleR) * 24000;	// 0.8.0(TEST2)
				endIdx	 = ((double)fftSizeOut / outSampleR) * 44000;
				step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter3);				// 0.8.0(TEST2)
				step *= 1.11;
				adj_disable = 0;
			}
			div = 1;
			if (adj_disable == 0) {
				for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
					hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
					if (hz >= (param->p->hfc + 1500)) {
						fftw_out2[i][0] *= div;
						fftw_out2[i][1] *= div;
						if (div - step > 0) {
					   		div -= step;
						} else {
					   		div = 0.01;
						}
					}
				}
				old_div = div;
				div = 1.0 - ((1.0 - div) / 1.218);
				for (;i < fftSizeOut / 2;i++) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
				}
			}
			
			if (outSampleR / 2 <= 24000) {
				adj_disable = 1;
			} else if (outSampleR / 2 <= 48000) {
				adj_disable = 1;
			} else if (outSampleR / 2 <= 96000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 49000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 93000;
				step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter4);
				step *= 0.85;
				adj_disable = 0;
			} else if (outSampleR / 2 <= 192000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 49000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 93000;
				step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter4);
				step *= 0.85;
				adj_disable = 0;
			} else {
				startIdx = ((double)fftSizeOut / outSampleR) * 49000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 93000;
				step = 1.00 / ((endIdx - startIdx) * param->p->hfa_filter4);
				step *= 0.85;
				adj_disable = 0;
			}
			if (adj_disable) {
				div = 1;
				for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
					hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
					if (hz >= (param->p->hfc + 1500)) {
						fftw_out2[i][0] *= div;
						fftw_out2[i][1] *= div;
						if (div - step > 0) {
					   		div -= step;
						} else {
					   		div = 0.01;
						}
					}
				}
				old_div = div;
				div = 1.0 - ((1.0 - div) / 1.618);
				for (;i < fftSizeOut / 2;i++) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
				}
			}
			if (outSampleR / 2 <= 24000) {
				adj_disable = 1;
			} else if (outSampleR / 2 <= 48000) {
				adj_disable = 1;
			} else if (outSampleR / 2 <= 96000) {
				adj_disable = 1;
			} else if (outSampleR / 2 <= 192000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 89000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 156000;
				step = 1.00 / ((endIdx - startIdx) * 1.3);
				step *= 0.75;
				adj_disable = 0;
			} else {
				startIdx = ((double)fftSizeOut / outSampleR) * 89000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 186000;
				step = 1.00 / ((endIdx - startIdx) * 1.3);
				step *= 0.75;
				adj_disable = 0;
			}
			if (adj_disable) {
				div = 1;
				for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
					hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
					if (hz >= (param->p->hfc + 1500)) {
						fftw_out2[i][0] *= div;
						fftw_out2[i][1] *= div;
						if (div - step > 0) {
					   		div -= step;
						} else {
					   		div = 0.01;
						}
					}
				}
				for (;i < fftSizeOut / 2;i++) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
				}
			}
		}

		if (mode == 2) {
			// 独自のローパスフィルター
			cutOff = 40000;
			if (param->p->lpf > 1 && cutOff > param->p->lpf) {
				cutOff = param->p->lpf;
			}
			if ((outSampleR / 2) >= cutOff) {
				startIdx  = ((double)fftSizeOut / outSampleR) * cutOff;
				if (param->p->oversamp == 0) {
					endIdx = fftSizeOut / 2;
				} else {
					endIdx = fftSizeOut / 4;
				}
				step = 1.00 / ((endIdx - startIdx) * 1.30);

				div = 1;
				for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
					if (div - step > 0) {
						div -= step;
					} else {
						div = 0.01;
					}
				}
			}
		}
	} else {
		// デエンファシス用の処理
		if (param->p->deEmphasis == 1) {
			startIdx = ((double)fftSizeOut / outSampleR) * 3180;
			endIdx	 = ((double)fftSizeOut / outSampleR) * 10600;
		} else {
			startIdx = ((double)fftSizeOut / outSampleR) * 2100;
			endIdx	 = ((double)fftSizeOut / outSampleR) * 9520;
		}
		step = 1.00 / ((endIdx - startIdx) * 1.75);

		div = 1;
		for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
			if (div - step > 0) {
		   		div -= step;
			} else {
		   		div = 0.01;
			}
		}
		for (;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
		}
	}

	if (param->p->oversamp == 0) {
		for (i = (fftSizeOut / 2) - 5;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] = 0;
			fftw_out2[i][1] = 0;
		}
	} else {
		for (i = (fftSizeOut / 4) - 5;i < fftSizeOut / 4;i++) {
			fftw_out2[i][0] = 0;
			fftw_out2[i][1] = 0;
		}
	}

	PRINT_FN_LOG(param->debug_upconv,"-adjPinkFilter:End");
}

//---------------------------------------------------------------------------
// Function   : adjPinkFilter_0774
// Description: 1/f 特性にするフィルター
// ---
//	mode	  :モード(0,1,2,3)
//	fftSizeOut:FFT数
//	fftw_out2 :FFTW OUT 変数
//	param	  :変換パラメータ
//
void adjPinkFilter_0774(int mode,long fftSizeOut,fftw_complex *fftw_out2,PARAM_INFO *param)
{
	long i;
	long startIdx,endIdx;
	double hz,div,step;
	
	long cutOff;
	long outSampleR;

	PRINT_FN_LOG(param->debug_upconv,"-adjPinkFilter_0774:Start");

if (param->upconv == 2) {
	PRINT_LOG("HFA4","22005");
}
	outSampleR = param->p->outSampleR;
	if (mode == 4 && param->p->lpf != -1) {
		startIdx = ((double)fftSizeOut / outSampleR) * param->p->lpf;
		endIdx	 = ((double)fftSizeOut / outSampleR) * (param->p->lpf + 20000);
		step = 1.00 / ((endIdx - startIdx) * 1.55);

		div = 1;
		for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
			if (div - step > 0) {
		   		div -= step;
			} else {
		   		div = 0.01;
			}
		}
		for (;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
		}
		div = 1;
		for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
			if (div - step > 0) {
		   		div -= step;
			} else {
		   		div = 0.01;
			}
		}
		for (;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
		}
		return;
	}
if (param->upconv == 2) {
	PRINT_LOG("HFA4","22046");
}
	if (mode == 1) {
		// 1/f 特性にするフィルター(hfa1)
		for (i = 1;i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			if (hz > 0) {
				fftw_out2[i][0] /= hz;
				fftw_out2[i][1] /= hz;
			}
		}
	}
if (param->upconv == 2) {
	PRINT_LOG("HFA4","22059");
}
	if (mode != 3) {
		// hfa1、hfa2、hfa3用の高域補間時の周波数調整
		if (param->p->hfa != 0 && param->p->hfc >= 8000 && param->p->hfc <= 23000) {
			startIdx = ((double)fftSizeOut / outSampleR) * 13000;
			endIdx	 = ((double)fftSizeOut / outSampleR) * 19000;
			step = 1.00 / ((endIdx - startIdx) * 1.50);
			step = 1.00 / ((endIdx - startIdx) * 1.46);
			step = 1.00 / ((endIdx - startIdx) * 1.57);
			div = 1;
			for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
				hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
				if (hz >= (param->p->hfc + 1500)) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
					if (div - step > 0) {
				   		div -= step;
					} else {
				   		div = 0.01;
					}
				}
			}
			for (;i < fftSizeOut / 2;i++) {
				fftw_out2[i][0] *= div;
				fftw_out2[i][1] *= div;
			}
			if (param->p->validOutSampleR <= 96000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 15000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 36000;
				step = 1.00 / ((endIdx - startIdx) * 1.20);
				startIdx = ((double)fftSizeOut / outSampleR) * 17000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 24000;
				step = 1.00 / ((endIdx - startIdx) * 1.46);
			} else {
				startIdx = ((double)fftSizeOut / outSampleR) * 17000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 29000;
				step = 1.00 / ((endIdx - startIdx) * 1.50);
			}
			div = 1;
			for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
				hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
				if (hz >= (param->p->hfc + 1500)) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
					if (div - step > 0) {
				   		div -= step;
					} else {
				   		div = 0.01;
					}
				}
			}
			for (;i < fftSizeOut / 2;i++) {
				fftw_out2[i][0] *= div;
				fftw_out2[i][1] *= div;
			}
			if (param->p->validOutSampleR <= 96000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 30000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 68000;
				step = 1.00 / ((endIdx - startIdx) * 1.2);
				startIdx = ((double)fftSizeOut / outSampleR) * 23000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 47000;
				step = 1.00 / ((endIdx - startIdx) * 1.61);
			} else {
				startIdx = ((double)fftSizeOut / outSampleR) * 28000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 57000;
				step = 1.00 / ((endIdx - startIdx) * 1.71);
			}
			div = 1;
			for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
				hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
				if (hz >= (param->p->hfc + 1500)) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
					if (div - step > 0) {
				   		div -= step;
					} else {
				   		div = 0.01;
					}
				}
			}
			for (;i < fftSizeOut / 2;i++) {
				fftw_out2[i][0] *= div;
				fftw_out2[i][1] *= div;
			}

			if (param->p->validOutSampleR <= 96000) {
				startIdx = ((double)fftSizeOut / outSampleR) * 80000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 120000;
				step = 1.00 / ((endIdx - startIdx) * 1.2);
				startIdx = ((double)fftSizeOut / outSampleR) * 77000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 120000;
				step = 1.00 / ((endIdx - startIdx) * 1.81);
			} else {
				startIdx = ((double)fftSizeOut / outSampleR) * 80000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 120000;
				step = 1.00 / ((endIdx - startIdx) * 1.2);
				startIdx = ((double)fftSizeOut / outSampleR) * 96000;
				endIdx	 = ((double)fftSizeOut / outSampleR) * 120000;
				step = 1.00 / ((endIdx - startIdx) * 1.91);
			}
			div = 1;
			for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
				hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
				if (hz >= (param->p->hfc + 1500)) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
					if (div - step > 0) {
				   		div -= step;
					} else {
				   		div = 0.01;
					}
				}
			}
			for (;i < fftSizeOut / 2;i++) {
				fftw_out2[i][0] *= div;
				fftw_out2[i][1] *= div;
			}
		}

		if (mode == 2) {
			// 独自のローパスフィルター
			cutOff = 40000;
			if (param->p->lpf > 1 && cutOff > param->p->lpf) {
				cutOff = param->p->lpf;
			}
			if ((outSampleR / 2) >= cutOff) {
				startIdx  = ((double)fftSizeOut / outSampleR) * cutOff;
				if (param->p->oversamp == 0) {
					endIdx = fftSizeOut / 2;
				} else {
					endIdx = fftSizeOut / 4;
				}
				step = 1.00 / ((endIdx - startIdx) * 1.30);

				div = 1;
				for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
					fftw_out2[i][0] *= div;
					fftw_out2[i][1] *= div;
					if (div - step > 0) {
						div -= step;
					} else {
						div = 0.01;
					}
				}
			}
		}
	} else {
		// デエンファシス用の処理
		if (param->p->deEmphasis == 1) {
			startIdx = ((double)fftSizeOut / outSampleR) * 3180;
			endIdx	 = ((double)fftSizeOut / outSampleR) * 10600;
		} else {
			startIdx = ((double)fftSizeOut / outSampleR) * 2100;
			endIdx	 = ((double)fftSizeOut / outSampleR) * 9520;
		}
		step = 1.00 / ((endIdx - startIdx) * 1.75);

		div = 1;
		for (i = startIdx;i < endIdx && i < fftSizeOut / 2;i++) {
			hz = (((double)(outSampleR / 2)) / (fftSizeOut / 2)) * i;
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
			if (div - step > 0) {
		   		div -= step;
			} else {
		   		div = 0.01;
			}
		}
		for (;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] *= div;
			fftw_out2[i][1] *= div;
		}
	}
if (param->upconv == 2) {
	PRINT_LOG("HFA4","22234");
}
	if (param->p->oversamp == 0) {
		for (i = (fftSizeOut / 2) - 5;i < fftSizeOut / 2;i++) {
			fftw_out2[i][0] = 0;
			fftw_out2[i][1] = 0;
		}
	} else {
		for (i = (fftSizeOut / 4) - 5;i < fftSizeOut / 4;i++) {
			fftw_out2[i][0] = 0;
			fftw_out2[i][1] = 0;
		}
	}
	PRINT_FN_LOG(param->debug_upconv,"-adjPinkFilter_0774:End");
}

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
	char sss[300];
	SSIZE min,max;
	SSIZE maxLv,maxLv2;
	SSIZE remainSample;
	SSIZE ns;
	long i;
	fio_size remain1,remain2;
	fio_size wr_n;

	PRINT_FN_LOG(param->debug_upconv,"-merageTempFile:Start");

	if (fp_r != NULL) {
		if (fp_r->error) {
			sprintf(sss,"fp_r->error:%d",fp_r->error);
			PRINT_LOG("",sss);
			return;
		}
		fio_rewind(fp_r);
	}

	if (fp_r2 != NULL) {
		if (fp_r2->error) {
			sprintf(sss,"fp_r2->error:%d",fp_r2->error);
			PRINT_LOG("",sss);
			return;
		}
		fio_rewind(fp_r2);
	}

	if (fp_w != NULL) {
		if (fp_w->error) {
			sprintf(sss,"fp_w->error:%d",fp_w->error);
			PRINT_LOG("",sss);
			return;
		}
		fio_rewind(fp_w);
	}

	min = max = 0;
	ns	= 0;
	maxLv = 0;
	maxLv2 = 0;
	remainSample = inSample;

	if (param->err) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		// エラー終了
		return;
	}

	do {
		if (type == '+') {
			if (fp_r2 == NULL) {
				remain1 = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
				if (fp_r->error) {
					sprintf(sss,"fio_read:error:%d",fp_r->error);
					PRINT_LOG("",sss);
				}
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
				if (fp_r->error) {
					sprintf(sss,"fio_read:error:%d",fp_r->error);
					PRINT_LOG("",sss);
				}
				if (fp_r2->error) {
					sprintf(sss,"fio_read2:error:%d",fp_r->error);
					PRINT_LOG("",sss);
				}
			}

			if (remain1 == 0 || remain2 == 0) {
				sprintf(sss,"fio_read:remain0");
				PRINT_LOG("",sss);
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
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					PRINT_LOG("","error merageTempFile");
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
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
					PRINT_LOG("","error merageTempFile");
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
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
						ns = 0;
					}
				}
			}
			remain1 =  remain1 < remainSample ? remain1 : remainSample;
			if (fp_w != NULL) {
//				fio_seek(fp_w,pos,SEEK_SET);
				wr_n = fio_write(diskBuffer1,sizeof (SSIZE),remain1,fp_w);
				if (wr_n != remain1) {
					PRINT_LOG("","error merageTempFile");
					param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
					break;
				}
			}
			remainSample -= remain1;
		}
	} while (remain1 == 4 * 1024 * 1024L &&  remainSample > 0);

	if (remainSample > 0 && param->err == STATUS_SUCCESS) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		PRINT_LOG("","remain error");
	}

	if (param->err) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		// エラー終了
		return;
	}
	
	if (fp_w != NULL) {
		fio_flush(fp_w);
	}
	if (normFlag == 1) {
		if (max > param->normInfo->max) {
			param->normInfo->max = max;
		}
		if (min < param->normInfo->min) {
			param->normInfo->min = min;
		}
		if (ns > 0) {
			maxLv2 /= ns;
		}
		if (maxLv > 0) {
			maxLv = (maxLv + maxLv2) / 2;
		} else {
			maxLv = maxLv2;
		}
		param->normInfo->avg = maxLv;
	}
	PRINT_FN_LOG(param->debug_upconv,"-merageTempFile:End");
}
//---------------------------------------------------------------------------
// Function   : calcNormInfo
// Description: 音量情報を計算する
// ---
//	fp_r 	 :入力ファイル1
//	inSample :サンプル数
//	param	 :パラメーター
//
void calcNormInfo(FIO *fp_r,SSIZE inSample,PARAM_INFO *param,NORM_INFO *normInfo)
{
	SSIZE min,max;
	SSIZE maxLv,maxLv2;
	SSIZE remainSample;
	SSIZE ns;
	long i;
	fio_size remain1;

	PRINT_FN_LOG(param->debug_upconv,"-calcNormInfo:Start");

	fio_rewind(fp_r);
	min = max = 0;
	ns	= 0;
	maxLv = 0;
	maxLv2 = 0;
	remainSample = inSample;

	do {
		remain1 = fio_read(diskBuffer1,sizeof (SSIZE),(fio_size)4 * 1024 * 1024,fp_r);
		if (remain1 == 0) {
			break;
		}
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
					ns = 0;
					maxLv2 = 0;	// ☆
				}
			}
		}
		remainSample -= remain1;
	} while (remain1 == 4 * 1024 * 1024L &&  remainSample > 0);

	if (remainSample > 0 && param->err == STATUS_SUCCESS) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
	}

	if (param->err) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		// エラー終了
		return;
	}
	
	if (max > normInfo->max) {
		normInfo->max = max;
	}
	if (min < normInfo->min) {
		normInfo->min = min;
	}
	if (ns > 0) {
		maxLv2 /= ns;
	}
	if (maxLv > 0) {
		maxLv = (maxLv + maxLv2) / 2;
	} else {
		maxLv = maxLv2;
	}
	normInfo->avg = maxLv;
	fio_rewind(fp_r);
	PRINT_FN_LOG(param->debug_upconv,"-calcNormInfo:End");
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

	PRINT_FN_LOG(param->debug_upconv,"-volumeAdjustFile:Start");
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
			param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
			break;
		}
		remainSample -= remain;
	} while (remain == 4 * 1024 * 1024L && remainSample > 0);

	if (remainSample > 0 && param->err == STATUS_SUCCESS) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
	}

	if (param->err) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		// エラー終了
		return;
	}
	
	fio_flush(fp_w);
	if (max > normInfo->max) {
		normInfo->max = max;
	}
	if (min < normInfo->min) {
		normInfo->min = min;
	}
	if (ns > 0) {
		maxLv2 /= ns;
	}
	if (maxLv > 0) {
		maxLv = (maxLv + maxLv2) / 2;
	} else {
		maxLv = maxLv2;
	}
	normInfo->avg = maxLv;
	PRINT_FN_LOG(param->debug_upconv,"-volumeAdjustFile:End");
}

//---------------------------------------------------------------------------
// Function   : chkAbort
// Description: 中断検出処理
// ---
//	param		:変換パラメータ
//
int chkAbort(int percent,int percent_diff)
{
	FILE *fp;
	int diff = abortInfo.abort_percent - percent;
	
	if (diff == 0 || diff > percent_diff || diff < (percent_diff * -1) || percent == 0 || percent == 100) {
		abortInfo.abort_percent = percent;
		if (abortInfo.abort_filename[0] != '\0') {
			fp = fopen(abortInfo.abort_filename,"r");
			if (fp) {
				fclose(fp);
				PRINT_LOG("upconv","------------- Abort ");
				return 1;
			}
		}
		if (abortInfo.error_filename[0] != '\0') {
			fp = fopen(abortInfo.error_filename,"r");
			if (fp) {
				fclose(fp);
				PRINT_LOG("upconv","------------- Error ");
				return 1;
			}
		}
	}
	return 0;
}
int file_exists(char *fn)
{
	FILE *ef;
	int ef_flag = 0;
	ef = fopen(fn,"r");
	if (ef != NULL) {
		fclose(ef);
		ef_flag = 1;
	}
	return ef_flag;
}

//---------------------------------------------------------------------------
// Function   : log10_lf
// Description: double の桁数を返す。
// ---
//	n 	:音のパワー値
int log10_lf(double n)
{
	int ret = 0;
	while (n > 1) {
		ret++;
		n = n /= 10;
	}
	return ret;
}
//---------------------------------------------------------------------------
// Function   : get_time_string()
// Description: 現在時間を返す
// ---
void get_time_string(char *str)
{
	time_t		timep;
    struct tm	*time_inf;

	timep = time(NULL);
	time_inf = localtime(&timep);
	sprintf(str,"%02d:%02d:%02d",time_inf->tm_hour,time_inf->tm_min,time_inf->tm_sec);
}
//---------------------------------------------------------------------------
// Function   : generate1fNoise()
// Description: 1/f ノイズ
// ---
double generate1fNoise(double prevValue) {
	double newValue;

	newValue = prevValue + (rand() / (double)RAND_MAX - 0.5);

	return newValue;
}
