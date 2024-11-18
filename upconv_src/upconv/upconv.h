#ifndef UPCONV_H
#define UPCONV_H

#ifndef BYTE
typedef unsigned char 	BYTE;
#endif
#ifndef WORD
typedef unsigned short	WORD;
#endif
#ifndef DWORD
typedef unsigned long	DWORD;
#endif
#ifndef SSIZE
typedef signed long long int SSIZE;
#endif

/* Return Status */
#define STATUS_SUCCESS			(0)		/* 正常終了 */
#define STATUS_NOT_IMPLEMENT	(-1)	/* その機能はインプリメントされていない */
#define STATUS_UNKNOWN_FORMAT	(-2)	/* 未知のフォーマット */
#define STATUS_DATA_ERR			(-3)	/* データが壊れている */
#define STATUS_MEM_ALLOC_ERR	(-4)	/* メモリーが確保出来ない */
#define STATUS_PARAMETER_ERR	(-5)
#define STATUS_FILE_READ_ERR	(-6)	/* ファイルリードエラー */
#define STATUS_FILE_WRITE_ERR	(-7)	/* ファイルライトエラー */
#define STATUS_EXEC_FAIL		(-8)	/* プロセス実行エラー */
#define STATUS_PLUGIN_ERR		(-15)	/* 内部エラー */

// ノーマライズ情報
typedef struct {
	SSIZE	min;
	SSIZE	max;
	SSIZE	avg;
} NORM_INFO;

// 音量情報
typedef struct {
	SSIZE	r1_avg;
	SSIZE	r2_avg;
	SSIZE	r3_avg;
	SSIZE	r4_avg;
	SSIZE	r5_avg;
	SSIZE	r6_avg;
	SSIZE	r1_peak;
	SSIZE	r2_peak;
	SSIZE	r3_peak;
	SSIZE	r4_peak;
	SSIZE	r5_peak;
	SSIZE	r6_peak;
	SSIZE	r1_count;
	SSIZE	r2_count;
	SSIZE	r3_count;
	SSIZE	r4_count;
	SSIZE	r5_count;
	SSIZE	r6_count;
	SSIZE	r1_work;
	SSIZE	r2_work;
	SSIZE	r3_work;
	SSIZE	r4_work;
	SSIZE	r5_work;
	SSIZE	r6_work;
	double	level_per;
	SSIZE	r1_m_avg;
	SSIZE	r2_m_avg;
	SSIZE	r3_m_avg;
	SSIZE	r4_m_avg;
	SSIZE	r5_m_avg;
	SSIZE	r6_m_avg;
	SSIZE	r1_m_peak;
	SSIZE	r2_m_peak;
	SSIZE	r3_m_peak;
	SSIZE	r4_m_peak;
	SSIZE	r5_m_peak;
	SSIZE	r6_m_peak;
	SSIZE	r1_m_count;
	SSIZE	r2_m_count;
	SSIZE	r3_m_count;
	SSIZE	r4_m_count;
	SSIZE	r5_m_count;
	SSIZE	r6_m_count;
	SSIZE	r1_m_work;
	SSIZE	r2_m_work;
	SSIZE	r3_m_work;
	SSIZE	r4_m_work;
	SSIZE	r5_m_work;
	SSIZE	r6_m_work;
	int		clip;
	double	nx;
} LEVEL_INFO;

// 中断情報
typedef struct {
	char abort_filename[_MAX_PATH];
	char error_filename[_MAX_PATH];
	int abort_percent;
	int abort_check_interval;
} ABORT_INFO;

// パラメータ用構造体
typedef struct {
	long inSampleR;
	long outSampleR;
	long outSampleR_ms;
	long outSampleR_final;
	long validOutSampleR;
	int w;
	int iw;
	int hfa;
	int enable_pre_eq;
	int enable_post_eq;
	int enable_post_eq2;
	int index_pre_eq;
	int index_post_eq;
	int index_post_eq2;
	int eq_analyze;
	int enable_hfc;
	int enable_lfc;
	long hfc;
	long lfc;
	long lpf;
	int no_rate_conv;
	int abe;
	int post_abe;
	int post_nr;
	int oversamp;
	int enable_nr;
	long nr;
	int nr_option;
	int cut_high_dither;
	int low_adjust;
	int hfc_auto;
	int deEmphasis;
	int addp;
	int addp_overrap;
	int cpu_pri;
	int thread;
	int mc_flag;
	int dsd_fmt;
	long fio;
	long fio_auto;
	long fio_max;
	long fio_sec_size;
	long fio_mb_size;
	int split_size;
	int vla_auto;
	int prof_mode;
	int level_scan_only;
	int volume_level_enable;
	int volume_level;
	int volume_per_enable;
	int simple_mode;
	int simple_mode_done_percent;
	int simple_mode_done_percent_div;
	double volume_per;
	double vla_nx;

	// ABE
	int abeNX;
	int cutLowData;
	int smLowData;
	int abeFnLevel;
	int abe_2val_smooth;
	int abe_3val_smooth;
	int abe_5val_smooth;
	int abe_9val_smooth;
	int abe_17val_smooth;
	int abe_31val_smooth;
	int abe_47val_smooth;
	int abe_rep_smooth;
	int abe_version;
	int abe_declip;
	int abe_declick;
	int abe_ad_filter;
	int abe_clld_level;
	int abe_10x_wlt_level;
	int abe_10x_cut_level;
	int abe_10x_cut_mode;
	int abe_10x_random_noise;
	// HFA
	int sig1Enb,sig2Enb,sig3Enb;
	int sig1AvgLineNx;
	int sig1Phase,sig2Phase,sig3Phase;
	int hfa_analysis_limit;
	int hfc_auto_adjust;
	int hfa3_analysis_limit_adjust;
	int hfa3_freq_start;
	int hfa3_freq_len;
	int hfa3_analyze_step;
	int hfa_level_adjust_width;
	int hfa_level_adjust_nx;
	int hfa3_sig_level_limit;
	int hfa3_cp_analyze;
	int hfa3_pp;
	int hfa3_freq_start_LRC;
	int hfa3_freq_len_LRC;
	int hfa3_analyze_step_LRC;
	int hfa_level_adjust_width_LRC;
	int hfa_level_adjust_nx_LRC;
	int hfa3_sig_level_limit_LRC;
	int hfa3_fft_window_size;
	int hfa3_fft_window_lvl;
	int hfa3_fft_window_per_1s;
	int hfa3_pp_LRC;
	int hfa3_param_test[2];
	int hfa_preset;
	int hfa3_version;
	int hfa_level;
	int hfaFast;
	int hfaWide;
	int hfaNB;
	int sfa;
	int enable_center_hfc;
	long center_hfc;
	int out_mp4;
	double hfa3_max;
	double hfa_filter1;
	double hfa_filter2;
	double hfa_filter3;
	double hfa_filter4;
	double hfa_LRC_CC_Phase;
	double hfa_LRC_CC_Level;

	// wav2raw
	int bitwidth_int;
	int conv_test;
	int ms_process;
	int LRC_process;
	int LRC_process_Center;
	int MS_LRC_mode;
	long skip_start_offset;
	int clipped;
	
	// raw2wav
	double median_per;
	int print_median_per;
	int ch;
	int realCh;
	int genCh;
	int r_nnn;
	int r_nnn_count;
	int output_dither_mode;
	int output_dither_option;
	int output_mid_side;
	int output_left_right_center;
	int norm;
	int norm_option;
	int enable_eq_analyze;
	int bwf;
	int raw;
	int rf64;
	int w64;
	int wav2g;
	int wav_auto;
	int chC;
	int chS;
	int chLFE;
	int mc_split;
	int mc_downmix;
	int encorder;
	int dp_save;
	char *analyze_filename;

	// overwrite
	long ovwrite_s;
	long ovwrite_w;
	long ovwrite_w_int;
	int enable_ovwrite_sw;
	int ovwrite_hfa;
	int enable_ovwrite_hfa;
	int ovwrite_norm;
	int enable_ovwrite_norm;
	int enable_overwrite_abe;
	int enable_overwrite_post_abe;
	int enable_overwrite_nr;
	int enable_overwrite_ms;
	int enable_overwrite_lrc;
} UPCONV_PARAM;


#endif

