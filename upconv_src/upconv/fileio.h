/****************************************************************************/
/* fileio (C) 2011 By 59414d41												*/
/* テンポラリファイル入出力IO												*/
/*																			*/
/****************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>

typedef signed long long fio_size;

#ifndef _MAX_DIR
#define _MAX_DIR	(256)
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE	(3)
#endif
#ifndef _MAX_EXT
#define _MAX_EXT	(256)
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME	(256)
#endif
#ifndef _MAX_PATH
#define _MAX_PATH	(260)
#endif

#ifdef __APPLE__
#define off64_t off_t
#define fopen64 	fopen
#define fseeko64	fseek
#define ftello64	ftell
#endif

#define FIO_MODE_R		(1)				// 読み取り専用
#define FIO_MODE_W		(2)				// 書き込み専用
#define FIO_MODE_RZ		(11)			// 読み取り専用(可逆圧縮)
#define FIO_MODE_WZ		(12)			// 書き込み専用(可逆圧縮)

#define FIO_ERR_READ	(-1)
#define FIO_ERR_WRITE	(-2)
#define FIO_ERR_MEMORY	(-3)
#define FIO_ERR_OTHER	(-4)

typedef struct {
	FILE *fp;
	fio_size data_size;
	fio_size data_offset;
	fio_size data_maxsize;
	fio_size sector_size;
	void *p_data;
	int magic;
	int	mode;
	int checked;
	int error;
	int block_count;
	int	block_max;
	int upconv_n;
	long cache_hit_count;
	long cache_nohit_count;
	int	log_remain;
	int log;
	int readahead;
	int writecache;
	long block_size;
	long sector_n;
	unsigned char *ibuffer;
	int ibuffer_size;
	char fname[_MAX_PATH];
	char *log_fn;
	char *param[64];
	char *value[64];
} FIO;

typedef struct {
	fio_size sector_size;
	long sector_n;
	int upconv_n;
} FIO_COMMON_PARAM;

typedef struct {
	long sampling_rate;
	short fio_version;
	short fio_mode;
} FIO_HEADER;

void fio_open(FIO *fio,char *filename,int mode);
void fio_open_with_set_param(FIO *fio,char *filename,int mode,FIO_COMMON_PARAM *fio_common);
void fio_close(FIO *fio);
void fio_get_filesize(FIO *fio,fio_size *size);
void fio_get_datasize(FIO *fio,fio_size *size);
void fio_set_tmpfile(FIO *fio,char *filename);
void fio_set_memory_limit(FIO *fio,int block_sz,int max);
void fio_set_maxsize(FIO *fio,fio_size max);
void fio_setmode_r(FIO *fiow,FIO *fior,char *filename);
void fio_set_logfn(FIO *fio,char *log_fn);
void fio_set_log_remain(FIO *fio,int log_remain);
void fio_seek(FIO *fio,fio_size offset,int orign);
void fio_set_readahead(FIO *fio1,FIO *fio2,FIO *fio3,FIO *fio4,FIO *fio5,FIO *fio6,int max_block_num);
void fio_set_writecache(FIO *fio1,FIO *fio2,FIO *fio3,FIO *fio4,FIO *fio5,FIO *fio6,int max_block_num);
void fio_set_sectorsize(FIO *fio,fio_size size);
void fio_set_parameter(FIO *fio,char *name,long value);
fio_size fio_read(void *buf,fio_size size,fio_size n,FIO *fio);
fio_size fio_write(void *buf,fio_size size,fio_size n,FIO *fio);
void fio_flush(FIO *fio);
void fio_rewind(FIO *fio);
fio_size fio_tell(FIO *fio);

#ifdef __APPLE__
void _splitpath(const char *path,char *drive,char *dir,char *fname,char *ext);
void _makepath(const char *path,char *drive,char *dir,char *fname,char *ext);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

