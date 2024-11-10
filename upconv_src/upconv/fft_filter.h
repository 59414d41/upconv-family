
#ifndef FFT_FILTER_H
#define FFT_FILTER_H

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

// サンプルを処理するデータ型
#define SSIZE	signed long long int

typedef struct _fft_cut_info {
	SSIZE sec;
	long  cut_start;
	long  cut_count;
	long  sfc;
	struct _fft_cut_info *next;
} FFT_CUTINFO;

typedef struct {
	long inSampleR;
	long outSampleR;
	long hfc;
	long lfc;
	int  analyze_mode;
	int  disable_eq;
	int  hi_sample_mode;
	int  eq_flag;
	int  lpf_flag;
	int  lfa_flag;
	int  hfa_flag;
	int  declip_flag;
	int  pre_eq_flag;
	int  post_eq_flag;
	int  post_eq2_flag;
	int  cut_high_dither;
	int  lvadj_flag;
	int  fft_cut_info_flag;
	int abort_percent;
	int print_stdout;
	int simple_mode;
	int simple_mode_done_percent;
	int simple_mode_done_percent_div;
	int err;
	double *eq_ref_max;
	double *eq_ref_avg;
	double *eq;
	double *lfa_eq;
	double *declip_eq;
	double *hfa_eq;
	double *pre_post_eq;
	double *analyze_eq_pw;
	long   eq_ref_count;
	long   analyze_eq_pw_count;
	int deEmphasis;
	int dsd_fmt;
	SSIZE maxMemorySize;
	SSIZE startSample;
	FFT_CUTINFO	*fft_cut_info;
	char pr_str[40];
} FFT_PARAM;

void fftFilter(SSIZE inSample,SSIZE outSample,FIO *fp,FIO *tmp,FFT_PARAM *param);
void fftFilterSub(SSIZE *pIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,FFT_PARAM *param,int id);
void copyToFFTW(fftw_complex *fftw,SSIZE *buf,long size);
void windowFFTW(fftw_complex *fftw,long size);
void windowData(SSIZE *data,long size);
void cutFFTW(fftw_complex *fftw,long index,long size);
void *al_malloc(SSIZE size);
void *al_free(void *ptr);

#endif
