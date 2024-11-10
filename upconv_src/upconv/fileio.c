/****************************************************************************/
/* fileio (C) 2011-2024 By 59414d41											*/
/* ファイル入出力IO															*/
/*																			*/
/****************************************************************************/

/*--- Log ------------------------------------------------------------------
 * Ver 0.80 <11/07/24> - 新規作成
 * Ver 1.20 <19/10/12> - 変更
 * Ver 1.21 <21/12/19> - 読み込み高速化の実験
 *
 * 各プログラムとリンクして使用する
 */

//#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <sys/stat.h>
#include <sys/unistd.h>
#include "fileio.h"

#ifndef SSIZE
typedef signed long long int SSIZE;
#endif

#define P_BUF_SIZE_N	(100)

typedef struct fio_data {
	unsigned char *buf;
	int	r_flag;
	int w_flag;
	int block_no;
	struct fio_data *next;
} FIO_DATA;

typedef struct fio_zdata {
	SSIZE *buf;
	int	r_flag;
	int w_flag;
	int block_no;
	struct fio_zdata *next;
} FIO_ZDATA;

void fio_store_buffer(FIO *fio,FIO_DATA *p_data);
void fio_store_zbuffer(FIO *fio,FIO_DATA *p_data);
void fio_copy_file(FIO *fio_r,FILE *ofp);
void fio_clear_buffer(FIO *fio);
void fio_fill_buffer(FIO *fio);
FIO_DATA *fio_alloc(FIO *fio);
FIO_DATA *fio_free(FIO *fio,FIO_DATA *p_data);
static fio_size ftell_local(FILE *fp);
static int fseek_local(FILE *fp,fio_size offset,int origin);
void *fio_memcpy(void *buf1, const void *buf2, fio_size n);

#if 0
#define	PRINT_LOG(s)	do {																	\
							FILE *log;															\
							log = fopen("d:\\fileio.log","a");									\
							if (log) {															\
								fprintf(log,"%s [%d] %s\n",__FUNCTION__,__LINE__,s);			\
								fclose(log);													\
							}																	\
						} while (0)
#define PRINT_SECTOR_LOG(b,o,l,s1,s2)	do {																\
											FILE *log;														\
											log = fopen("d:\\fileio.log","a");								\
											if (log) {														\
												fprintf(log,"blk:%ld,%ld,%ld,%s:%s\n",b,o,l,s1,s2);			\
												fclose(log);												\
											}																\
										} while (0)

#else
#define	PRINT_LOG(s)				//
#define PRINT_SECTOR_LOG(b,o,l,s1,s2)	//
#endif

//---------------------------------------------------------------------------
// Function   : fio_open
// Description: ファイルオープン処理
// ---
//	fio			: fileio 構造体
//	filename	: ファイル名
//	mode		: ファイルオープンモード(R,W)
//
void fio_open(FIO *fio,char *filename,int mode)
{
	FIO_DATA *p_data,*p_before;
	int i;
	int mmr_ok,mmw_ok;
	long page_size;
	char sss[2048];
	SYSTEM_INFO system_info;

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	memset(fio,0,sizeof (FIO));

	if (filename == NULL || !(mode == FIO_MODE_R || mode == FIO_MODE_W || FIO_MODE_RZ || FIO_MODE_WZ)) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}

	if (strlen(filename) + 1 > _MAX_PATH) {
		// ファイル名が長すぎ
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}

	sprintf(sss,"fio_open:%s",filename);
	PRINT_LOG(sss);

	fio->magic = 5915;
	fio->mode  = mode;
//	fio->ibuffer_size = 20;
	fio->ibuffer_size = 0;
//	fio->block_size = fio->ibuffer_size * 1000 * 1000L;
	fio->data_offset = 0;
	fio->block_count = 0;
	fio->block_max = 5;
	fio->upconv_n = 1;
	fio->data_maxsize = 0;
	fio->sector_size = (fio_size)1 * 1000 * 1000;
	fio->sector_n = 5;
	fio->block_size = fio->sector_size * fio->sector_n;
	fio->error = 0;
	fio->log = 1;
	fio->checked = 1;
//	fio->ibuffer = malloc(fio->block_size);
	fio->ibuffer = malloc(fio->sector_size);
	memset(fio->param,0,sizeof (char *) * 64);
	memset(fio->value,0,sizeof (char *) * 64);
	if (fio->ibuffer == NULL) {
		fio->error = FIO_ERR_MEMORY;
		PRINT_LOG("ERROR");
		return;
	}

	mmr_ok = 0;

	if (mode == FIO_MODE_R || mode == FIO_MODE_RZ) {
		fio->fp = fopen(filename,"rb");
		if (fio->fp == NULL) {
			fio->error = FIO_ERR_READ;
			return;
		}
		fio_get_filesize(fio,&fio->data_size);
		if (fio->error) {
			PRINT_LOG("ERROR");
			return;
		}
	}

	mmw_ok = 0;
	if (mode == FIO_MODE_W || mode == FIO_MODE_WZ) {
		fio->fp = fopen(filename,"wb+");
		if (fio->fp == NULL) {
			fio->error = FIO_ERR_WRITE;
			PRINT_LOG("ERROR");
			return;
		}
		fio->data_size = 0;
	}

	strcpy(fio->fname,filename);
	p_before = NULL;
	for (i = 0;i < fio->block_max;i++) {
		p_data = fio_alloc(fio);
		if (p_data == NULL && i == 0) {
			fio->error = FIO_ERR_MEMORY;
			return;
		}
		if (p_data == NULL) break;
		p_data->next = p_before;
		p_before = p_data;
		fio->p_data = p_data;
		fio->block_count++;
	}
	return;
}
//---------------------------------------------------------------------------
// Function   : fio_open_with_set_param
// Description: ファイルオープン処理(パラメータセット処理も含む)
// ---
//	fio			: fileio 構造体
//
void fio_open_with_set_param(FIO *fio,char *filename,int mode,FIO_COMMON_PARAM *fio_common)
{
	char sss[2048];
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	sprintf(sss,"fio_open_with_param:%s",filename);
	PRINT_LOG(sss);

	fio_open(fio,filename,mode);
	if (fio->error) {
		return;
	}


	fio->sector_n    = fio_common->sector_n;
	fio->upconv_n    = fio_common->upconv_n;
	fio_set_memory_limit(fio,fio_common->sector_size,0);

}
//---------------------------------------------------------------------------
// Function   : fio_close
// Description: ファイルクローズ処理
// ---
//	fio			: fileio 構造体
//
void fio_close(FIO *fio)
{
	FIO_DATA *p_data;
	FIO_DATA *p_old;
	char sss[200];
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio->magic != 5915) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}
	
	if (fio->error) {
		return;
	}
	
	if (fio->mode == FIO_MODE_W) {
		if (fio->fp == NULL) {
			fio->error = FIO_ERR_WRITE;
			PRINT_LOG("ERROR");
			return;
		}
		// バッファリングされているデータを書き込み。
		fio_store_buffer(fio,NULL);
		if (fio->error) {
			PRINT_LOG("ERROR");
			return;
		}
	}

	if (fio->mode == FIO_MODE_WZ) {
		if (fio->fp == NULL) {
			fio->error = FIO_ERR_WRITE;
			PRINT_LOG("ERROR");
			return;
		}
		// バッファリングされているデータを書き込み。
		//fio_store_zbuffer(fio,NULL);
		if (fio->error) {
			PRINT_LOG("ERROR");
			return;
		}
	}

	sprintf(sss,"cache_hit:%ld,cache_nohit:%ld",fio->cache_hit_count,fio->cache_nohit_count);
	PRINT_LOG(sss);

	// fio data cleanup
	p_data = fio->p_data;
	while (p_data != NULL) {
		p_data = fio_free(fio,p_data);
	}
	fio->p_data = NULL;
	
	if (fio->fp != NULL) {
		fclose(fio->fp);
	}
	if (fio->ibuffer != NULL) free(fio->ibuffer);
	fio->ibuffer = NULL;
	fio->fp = NULL;
	fio->error = 0;
	fio->magic = 0;
}
//---------------------------------------------------------------------------
// Function   : fio_get_filesize
// Description: ファイルサイズ取得
// ---
//	fio			: fileio 構造体
//	p_size		: サイズを返すためのアドレス
//
void fio_get_filesize(FIO *fio,fio_size *p_size)
{
	fio_size	size,pos;
	int error;
	char sss[200];
	PRINT_LOG("fio_get_filesize");
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio->magic != 5915 || p_size == NULL) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->error) {
		PRINT_LOG("fio->error");
		return;
	}

	error = 1;
	if (fio->mode == FIO_MODE_R || fio->mode == FIO_MODE_W) {
		pos = ftell_local(fio->fp);
		if (fseek_local(fio->fp,0,SEEK_END) == 0) {
			PRINT_LOG("fseek_local SEEK_END");
			size = ftell_local(fio->fp);
			sprintf(sss,"ftell_local:%lld",size);
			PRINT_LOG(sss);
			if (fseek_local(fio->fp,pos,SEEK_SET) == 0) {
				PRINT_LOG("fseek_local SEEK_SET");
				*p_size = size;
				error = 0;
			}
		}
	}
	if (error) {
		fio->error = FIO_ERR_READ;
		PRINT_LOG("ERROR");
	}
	PRINT_LOG("fio_get_filesize:end");
}
//---------------------------------------------------------------------------
// Function   : fio_get_datasize
// Description: データサイズ取得
// ---
//	fio			: fileio 構造体
//	p_size		: サイズを返すためのアドレス
//
void fio_get_datasize(FIO *fio,fio_size *p_size)
{
	fio_size	size;
	int error;

	PRINT_LOG("fio_get_datasize");
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio->magic != 5915 || p_size == NULL) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->error) {
		return;
	}
	*p_size = fio->data_size;
}
//---------------------------------------------------------------------------
// Function   : fio_setmode_r
// Description: ファイルを書き込みから読み込み専用に設定
// ---
//	fio_w		: fileio 構造体(W)
//	fio_r		: fileio 構造体(R)
//	filename	: 新しいファイル名
//
void fio_setmode_r(FIO *fio_w,FIO *fio_r,char *filename)
{
	int fio_w_mode;
	FILE *fp_tmp;
	PRINT_LOG("fio_setmode_r");
	if (fio_r == NULL || fio_w == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio_w->magic != 5915) {
		fio_w->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}

	if (fio_r->error) {
		PRINT_LOG("ERROR");
		return;
	}

	if (fio_w->error) {
		PRINT_LOG("ERROR");
		return;
	}

	if (filename != NULL) {
		if (strlen(filename) + 1 > _MAX_PATH) {
			// ファイル名が長すぎ
			fio_w->error = FIO_ERR_OTHER;
			PRINT_LOG("ERROR");
			return;
		}
	}

	fio_close(fio_w);
	if (fio_w->error) {
		PRINT_LOG("ERROR");
		return;
	}
	if (filename != NULL) {
		remove(filename);
		fp_tmp = fopen(filename,"r");
		if (fp_tmp != NULL) {
			PRINT_LOG("ERROR");
			fclose(fp_tmp);
		}
		rename(fio_w->fname,filename);
		fp_tmp = fopen(filename,"r");
		if (fp_tmp == NULL) {
			PRINT_LOG(filename);
			PRINT_LOG("ERROR");
		} else {
			fclose(fp_tmp);
		}
	}

	if (filename != NULL) {
		fio_open(fio_r,filename,FIO_MODE_R);
	} else {
		fio_open(fio_r,fio_w->fname,FIO_MODE_R);
	}

	if (fio_r->error) {
		PRINT_LOG("ERROR");
	}
	fio_r->data_size = fio_w->data_size;
	fio_r->data_maxsize = fio_w->data_maxsize;

	memset(fio_w,0,sizeof (FIO));
}
//---------------------------------------------------------------------------
// Function   : fio_set_maxsize
// Description: 書き込みデータサイズの最大値を設定
// ---
//	fio			: fileio 構造体
//	max			: 最大値
//
void fio_set_maxsize(FIO *fio,fio_size max)
{

	PRINT_LOG("fio_set_maxsize");
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio->magic != 5915 || max < 0) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->error) {
		return;
	}
	
	fio->data_maxsize = max;

	if (1) {
		char s[100];
		sprintf(s,"set maxsize:%lld",max);
		PRINT_LOG(s);
	}
}
//---------------------------------------------------------------------------
// Function   : fio_set_memory_limit
// Description: メモリ確保の最大値を設定
// ---
//	fio			: fileio 構造体
//  block_sz	: ブロックサイズ
//	max			: 最大値
//
void fio_set_memory_limit(FIO *fio,int block_sz,int max)
{
	FIO_DATA *p_data,*p_before;
	int i;
	char sss[256];
	PRINT_LOG("fio_set_memory_limit");
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio->magic != 5915 || max < 0 || max > P_BUF_SIZE_N || block_sz < 1) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->error) {
		return;
	}

	p_data = fio->p_data;
	while (p_data != NULL) {
		p_data = fio_free(fio,p_data);
	}
	fio->p_data = NULL;
	fio->block_count = 0;

	if (max == 0 && fio->sector_n > 5) {
		max = fio->sector_n / 5;
		fio->sector_n = 5;
	} else {
		max = 1;
	}

	fio->sector_size = block_sz;
	fio->block_max = max;
	if (fio->upconv_n > 1) {
		fio->block_max = max / fio->upconv_n;
		if (fio->block_max < 1) {
			fio->block_max = 1;
		}
	}

	//fio->ibuffer_size = block_sz;
	if (fio->ibuffer != NULL) free(fio->ibuffer);
	fio->ibuffer = malloc(fio->sector_size * fio->sector_n);
	if (fio->ibuffer == NULL) {
		fio->error = FIO_ERR_MEMORY;
		PRINT_LOG("ERROR");
		return;
	}
	fio->block_size  = fio->sector_size * fio->sector_n;

	p_before = NULL;
	for (i = 0;i < fio->block_max;i++) {
		p_data = fio_alloc(fio);
		if (p_data == NULL && i == 0) {
			fio->error = FIO_ERR_MEMORY;
			return;
		}
		if (p_data == NULL) break;
		p_data->next = p_before;
		p_before = p_data;
		fio->p_data = p_data;
		fio->block_count++;
	}
	sprintf(sss,"fio_set_memory_limit:%ld(sector:%ld,sector_n:%ld,block_count:%ld)",fio->sector_size * fio->sector_n * fio->block_count,fio->sector_size,fio->sector_n,fio->block_count);
	PRINT_LOG(sss);
}
//---------------------------------------------------------------------------
// Function   : fio_seek
// Description: ファイルポインタ設定
// ---
//	fio			: fileio 構造体
//	offset		: オフセット
//	orign		: オフセット指定パラメータ
//
void fio_seek(FIO *fio,fio_size offset,int orign)
{
	int error = 1;
	char s[100];

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->magic != 5915) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}
	
	if (fio->error) {
		return;
	}

	if (orign == SEEK_SET) {
		if (offset < fio->data_size && offset >= 0) {
			if (fio->log_remain > 0) {
				sprintf(s,"seek:%ld",offset);
				PRINT_LOG(s);
			}
			fio->data_offset = offset;
			error = 0;
		} else if (offset >= 0) {
			if (fio->log_remain > 0) {
				sprintf(s,"seek:%ld",offset);
				PRINT_LOG(s);
			}
			fio->data_offset = offset;
			fio->data_size	 = offset;
			error = 0;
		}
	}
	if (orign == SEEK_CUR) {
		if ((fio->data_offset + offset) >= 0) {
			fio->data_offset = fio->data_offset + offset;
			if (fio->log_remain > 0) {
				sprintf(s,"seek:%ld",fio->data_offset);
				PRINT_LOG(s);
			}
			if (fio->data_size < fio->data_offset) {
				fio->data_size = fio->data_offset;
			}
			error = 0;
		}
	}
	if (orign == SEEK_END) {
		if (fio->data_size + offset >= 0) {
			fio->data_offset = fio->data_offset + offset;
			if (fio->log_remain > 0) {
				sprintf(s,"seek:%ld",fio->data_offset);
				PRINT_LOG(s);
			}
			if (fio->data_size < fio->data_offset) {
				fio->data_size = fio->data_offset;
			}
			error = 0;
		}
	}
	
	if (error) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
	}
}

//---------------------------------------------------------------------------
// Function   : fio_read
// Description: ファイル読み込み
// ---
//	buf			: 読み込みデータ格納アドレス
//	size		: 読み込みバイト数
//	n			: 読み込み個数
//	fio			: fileio構造体
//
fio_size fio_read(void *buf,fio_size size,fio_size n,FIO *fio)
{
	fio_size	read_size;
	fio_size	rd;
	fio_size	block;
	fio_size	remain;
	fio_size	tmp_offset;
	fio_size	tmp_size;
	int 		error;
	int			i;
	char		s[100];
	unsigned char *p;
	FIO_DATA *p_data,*p_before;

	memset(buf,0,size * n);

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return 0;
	}

	if (fio->magic != 5915 || buf == NULL || size < 0 || n < 0) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return 0;
	}

	if (fio->error) {
		PRINT_LOG("ERROR");
		return 0;
	}

	remain = size * n;

	if (remain > fio->data_size - fio->data_offset) {
		remain = fio->data_size - fio->data_offset;
	}
	if (fio->data_maxsize > 0 && remain > fio->data_maxsize - fio->data_offset) {
		remain = fio->data_maxsize - fio->data_offset;
	}

	if (remain == 0) {
		if (fio->log_remain > 0) {
			PRINT_LOG("zero");
		}
		return 0;
	}

	read_size = 0;

	for (;remain > 0;) {
		// 読み込むデータのブロックを求める
		block = fio->data_offset;
		block /= fio->block_size;

		p_before = NULL;
		p_data = fio->p_data;
		while (p_data != NULL) {
			if (p_data->block_no == block) {
				break;
			}
			if (p_data->next == NULL) {
				if (fio->log_remain > 0) {
					PRINT_LOG("find block:none");
				}
				break;
			}
			p_before = p_data;
			p_data = p_data->next;
		}

		if (p_data == NULL) {
			fio->error = FIO_ERR_OTHER;
			PRINT_LOG("ERROR");
			return 0;
		}


		if (p_data->block_no != block) {
			// バッファにない
			if (p_before != NULL) {
				// 新しいデータを先頭に持ってくる
				p_before->next = p_data->next;
				p_data->next = fio->p_data;
				fio->p_data = p_data;
			}
			if (fio->mode == FIO_MODE_R) {
				fio->cache_nohit_count++;

				// バッファをクリアして新たに読み込みしなおす。
				if (fio->log_remain > 0) {
					PRINT_LOG("fio_read()no cache");
				}
				fio_clear_buffer(fio);
				fio_fill_buffer(fio);
			}
			p_data->r_flag = 0;
			p_data->w_flag = 0;
			p_data->block_no = -1;
			error = 1;
			tmp_offset = (fio->data_offset / fio->block_size) * fio->block_size;
			tmp_size   = fio->data_size - tmp_offset;
			if (fio->data_maxsize > 0 && fio->data_size > fio->data_maxsize) {
				tmp_size   = fio->data_maxsize - tmp_offset;
			}
			if (tmp_size > fio->block_size) {
				tmp_size = fio->block_size;
			}
			if (fseek_local(fio->fp,tmp_offset,SEEK_SET) == 0) {
				error = 2;
				rd = fread(p_data->buf,1,tmp_size,fio->fp);
				if (rd == tmp_size) {
					p_data->r_flag = 1;
					p_data->w_flag = 0;
					p_data->block_no = block;
					error = 0;
				}
			}
			if (error) {
				sprintf(s,"data_size:%ld\n",fio->data_size);
				PRINT_LOG(s);
				sprintf(s,"data_maxsize:%ld\n",fio->data_maxsize);
				PRINT_LOG(s);
				sprintf(s,"tmp_offset:%ld\n",tmp_offset);
				PRINT_LOG(s);
				sprintf(s,"tmp_size:%ld\n",tmp_size);
				PRINT_LOG(s);
				sprintf(s,"rd:%ld\n",rd);
				PRINT_LOG(s);
				PRINT_LOG("read zero");
				sprintf(s,"error:%d",error);
				PRINT_LOG(s);
				return 0;
			}
		} else {
			fio->cache_hit_count++;
			if (fio->log_remain > 0) {
				PRINT_LOG("fio_read(): cache");
			}
		}

		if (p_data == NULL || p_data->buf == NULL || p_data->block_no != block || p_data->r_flag != 1) {
			fio->error = FIO_ERR_OTHER;
			PRINT_LOG("ERROR");
			return 0;
		}

		tmp_offset = fio->data_offset % fio->block_size;
		tmp_size   = fio->block_size - tmp_offset;
		if (remain < tmp_size) {
			tmp_size = remain;
		}

		if (fio->log_fn != NULL && fio->log_remain > 0) {
			PRINT_SECTOR_LOG(block,tmp_offset,tmp_size,fio->log_fn,"fio_read");
			fio->log_remain--;
		}
		memcpy(buf,p_data->buf + tmp_offset,tmp_size);
		remain -= tmp_size;
		buf	+= tmp_size;
		fio->data_offset += tmp_size;
		read_size += tmp_size;
	}

	if (read_size != size * n) {
		char s[100];
		sprintf(s,"read_size:%lld,size:%lld,n:%lld",read_size,size,n);
		PRINT_LOG(s);
	}
	if (read_size == 0) {
		if (fio->log_remain > 0) {
			PRINT_LOG("zero");
		}
	}
	return (read_size / size);
}
//---------------------------------------------------------------------------
// Function   : fio_write
// Description: ファイル読み込み
// ---
//	buf			: 書き込みデータ格納アドレス
//	size		: 書き込みバイト数
//	n			: 書き込み個数
//	fio			: fileio構造体
//
fio_size fio_write(void *buf,fio_size size,fio_size n,FIO *fio)
{
	fio_size	write_size;
	fio_size	block;
	fio_size	remain;
	fio_size	tmp_offset;
	fio_size	tmp_size;
	fio_size	rd;
	int			alloc_error;
	int			i;
	char		s[50];
	unsigned char *p;
	int log;
	int error;
	FIO_DATA *p_data,*p_before;

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return 0;
	}

	if (fio->magic != 5915 || buf == NULL || size < 0 || n < 0) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return 0;
	}

	if (fio->log_remain > 0) {
//		PRINT_LOG("fio_write");
	}
	if (fio->error) {
		return 0;
	}
	
	if (fio->mode != FIO_MODE_W) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return 0;
	}

	log = 1;

	write_size = 0;
	remain = size * n;
	
	if (remain == 0) {
		return 0;
	}

	for (;remain > 0;) {
		block = fio->data_offset;
		block /= fio->block_size;

		p_before = NULL;
		p_data = fio->p_data;
		while (p_data != NULL) {
			if (p_data->block_no == block) {
//PRINT_LOG("find block:ok");
				break;
			}
			if (p_data->next == NULL) {
				break;
			}
			p_before = p_data;
			p_data = p_data->next;
		}

		if (p_data == NULL) {
			fio->error = FIO_ERR_OTHER;
			PRINT_LOG("ERROR");
			return 0;
		}

		if (p_data->block_no != block) {
			fio->cache_nohit_count++;
			if (p_before != NULL) {
				// 新しいデータを先頭に持ってくる
				p_before->next = p_data->next;
				p_data->next = fio->p_data;
				fio->p_data = p_data;
			}
			if (fio->log_remain > 0) {
				PRINT_LOG("fio_write() store buffer");
			}
			fio_store_buffer(fio,p_data);
			if (fio->error) {
				PRINT_LOG("ERROR");
				return 0;
			}
			p_data->r_flag = 0;
			p_data->w_flag = 0;
			p_data->block_no = -1;
			error = 0;
			tmp_offset = (fio->data_offset / fio->block_size) * fio->block_size;
			tmp_size   = fio->data_size;
			if (fio->data_maxsize > 0 && fio->data_size > fio->data_maxsize) {
				tmp_size   = fio->data_maxsize;
			}
			if (tmp_offset < tmp_size) {
				tmp_size = tmp_size - tmp_offset;
				if (tmp_size > fio->block_size) {
					tmp_size = fio->block_size;
				}
				if (fseek_local(fio->fp,tmp_offset,SEEK_SET) == 0) {
					error = 2;
					rd = fread(p_data->buf,1,tmp_size,fio->fp);
					if (rd == tmp_size) {
						p_data->r_flag = 1;
						p_data->w_flag = 0;
						p_data->block_no = block;
						error = 0;
					}
				}
				if (error) {
					sprintf(s,"data_size:%ld\n",fio->data_size);
					PRINT_LOG(s);
					sprintf(s,"data_maxsize:%ld\n",fio->data_maxsize);
					PRINT_LOG(s);
					sprintf(s,"tmp_offset:%ld\n",tmp_offset);
					PRINT_LOG(s);
					sprintf(s,"tmp_size:%ld\n",tmp_size);
					PRINT_LOG(s);
					sprintf(s,"rd:%ld\n",rd);
					PRINT_LOG(s);
					PRINT_LOG("read zero");
					sprintf(s,"error:%d",error);
					PRINT_LOG(s);
					return 0;
				}
			} else {
				// 最大の書き込みサイズを超えている場合はここで正常終了する。
				if (fio->data_maxsize > 0 && fio->data_offset > fio->data_maxsize) {
					return n;
				}
			}
		} else {
			fio->cache_hit_count++;
			if (fio->log_remain > 0) {
				PRINT_LOG("fio_write() memory");
			}
		}
		tmp_offset = fio->data_offset % fio->block_size;
		tmp_size   = fio->block_size - tmp_offset;
		if (remain < tmp_size) {
			tmp_size = remain;
		}
		if (fio->log_fn != NULL && fio->log_remain > 0) {
			PRINT_SECTOR_LOG(block,tmp_offset,tmp_size,fio->log_fn,"fio_write");
			fio->log_remain--;
		}
		memcpy(p_data->buf + tmp_offset,buf,tmp_size);
		p_data->w_flag = 1;
		p_data->r_flag = 1;
		p_data->block_no = block;
		remain -= tmp_size;
		buf	+= tmp_size;
		fio->data_offset += tmp_size;
		if (fio->data_offset > fio->data_size) {
			fio->data_size	 = fio->data_offset;
		}
		write_size += tmp_size;
	}
	if (write_size != size * n) {
		char s[100];
		sprintf(s,"write_size:%lld,size:%lld,n:%lld",write_size,size,n);
		PRINT_LOG(s);
	}

	return (write_size / size);
}
//---------------------------------------------------------------------------
// Function   : fio_flush
// Description: ファイル書き込みフラッシュ
// ---
//	fio			: fileio構造体
//
void fio_flush(FIO *fio)
{

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->magic != 5915) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}
	
	if (fio->error) {
		return;
	}

	if (fio->mode == FIO_MODE_W) {
		if (fio->log_remain > 0) {
			PRINT_LOG("fio_flush");
		}
		fio_store_buffer(fio,NULL);
	}
}
//---------------------------------------------------------------------------
// Function   : fio_rewind
// Description: ファイルポインタゼロリセット
// ---
//	fio			: fileio構造体
//
void fio_rewind(FIO *fio)
{
	PRINT_LOG("fio_rewind:start");

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->magic != 5915) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}
	
	if (fio->error) {
		PRINT_LOG("ERROR");
		return;
	}

	fio->data_offset = 0;

	PRINT_LOG("fio_rewind:end");
}
//---------------------------------------------------------------------------
// Function   : fio_tell
// Description: ファイルポインタ取得
// ---
//	fio			: fileio構造体
//
fio_size fio_tell(FIO *fio)
{

	if (fio->magic != 5915) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return 0;
	}
	
	if (fio->error) {
		return 0;
	}

	return fio->data_offset;
}
//---------------------------------------------------------------------------
// Function   : fio_store_buffer
// Description: ファイルバッファ書き込み
// ---
//	fio			: fileio構造体
//	p_data		: 書き込みデータ
//
void fio_store_buffer(FIO *fio,FIO_DATA *p_data)
{
	fio_size remain;
	fio_size data_size;
	fio_size offset;
	fio_size r_off;
	fio_size wr_n;
	FIO_DATA **fio_all = NULL;
	FIO_DATA *wk_data,*wk_tmp;
	int i,j,c;
	int all;
	char s[50];

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->magic != 5915 || fio->mode != FIO_MODE_W) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}
	
	if (fio->fp == NULL) {
		fio->error = FIO_ERR_WRITE;
		PRINT_LOG("ERROR");
		return;
	}

	if (fio->log_remain > 0) {
		PRINT_LOG("fio_store_buffer");
	}

	if (fio->error) {
		return;
	}

	all = 0;
	if (p_data == NULL) {
		all = 1;
		fio_all = (FIO_DATA **)malloc(fio->block_count * sizeof (FIO_DATA *));
		if (fio_all != NULL) {
			for (i = 0;i < fio->block_count;i++) {
				fio_all[i] = NULL;
			}
			for (i = 0,wk_data = fio->p_data;i < fio->block_count && wk_data != NULL;i++,wk_data = wk_data->next) {
				if (wk_data->block_no != -1 && wk_data->w_flag == 1) {
					for (j = 0;j < fio->block_count;j++) {
						if (fio_all[j] == NULL) {
							break;
						}
					}
					fio_all[j] = wk_data;
					c = j;
					if (c > 0) {
						for (--j;j >= 0;j--,c--) {
							if (fio_all[j]->block_no > fio_all[c]->block_no) {
								wk_tmp = fio_all[j];
								fio_all[j] = fio_all[c];
								fio_all[c] = wk_tmp;
							} else {
								break;
							}
						}
					}
				}
			}
		}
		if (fio_all) {
			c = 0;
			p_data = fio_all[c];
		}
	}

	if (fio->data_size > 0) {
		while (p_data != NULL) {
			if (p_data->block_no != -1 && p_data->w_flag == 1) {
				offset = p_data->block_no * (fio_size)fio->block_size;
				data_size = fio->data_size;
				if (fio->data_maxsize > 0 && data_size > fio->data_maxsize) {
					data_size = fio->data_maxsize;
				}
				if (offset < data_size) {
					remain = data_size - offset;
					wr_n = 0;
					fflush(fio->fp);
					if (fseek_local(fio->fp,offset,SEEK_SET) == 0) {
						r_off = ftell_local(fio->fp);
						if (offset != r_off) {
							char s[100];
							sprintf(s,"ERROR fseeko64 bad:%lld,%lld",offset,r_off);
							PRINT_LOG(s);
							fio->error = FIO_ERR_WRITE;
							return;
						}
						if (remain >= fio->block_size) {
							wr_n = fwrite(p_data->buf,fio->block_size,1,fio->fp);
							if (wr_n == 0) {
								char s[100];
								if (fio->log_remain > 0) {
									sprintf(s,"block_size:%ld",fio->block_size);
									PRINT_LOG(s);
								}
							}
						} else {
							wr_n = fwrite(p_data->buf,remain,1,fio->fp);
							if (wr_n == 0) {
								char s[100];
								sprintf(s,"remain_size:%lld",remain);
								PRINT_LOG(s);
							}
						}
					} else {
						r_off = ftell_local(fio->fp);
						sprintf(s,"SEEKERR offset:%lld,r_off:%lld,data_size:%lld",offset,r_off,data_size);
						PRINT_LOG(s);
						fio->error = FIO_ERR_WRITE;
						return;
					}
					if (wr_n == 0) {
						char s[100];
						sprintf(s,"i:%d,wr_n:%lld,remain:%lld,block_size:%ld",i,wr_n,remain,fio->block_size);
						fio->error = FIO_ERR_WRITE;
						PRINT_LOG(s);
						return;
					}
				}
				p_data->w_flag = 0;
			}
			if (all == 0) {
				break;
			} else {
				if (fio_all) {
					if (c + 1 < fio->block_count) {
						c++;
						p_data = fio_all[c];
					} else {
						break;
					}
				} else {
					p_data = p_data->next;
				}
			}
		}
	}
	if (fio_all) {
		free(fio_all);
	}
}
//---------------------------------------------------------------------------
// Function   : fio_copy_file
// Description: ファイルコピー
// ---
//	fio			: fileio構造体
//
void fio_copy_file(FIO *fio_r,FILE *ofp)
{
	fio_size remain;
	size_t	 n;
	char	s[100];

	remain = fio_r->data_size;
	if (fio_r->data_maxsize > 0 && fio_r->data_size > fio_r->data_maxsize) {
		remain = fio_r->data_maxsize;
	}
	
	fseek_local(fio_r->fp,0,SEEK_SET);

	while (remain) {
		if (remain >= fio_r->sector_size * fio_r->sector_n) {
			n = fread(fio_r->ibuffer,fio_r->sector_size * fio_r->sector_n,1,fio_r->fp);
		} else {
			n = fread(fio_r->ibuffer,remain,1,fio_r->fp);
		}
		if (n != 1) {
			fio_r->error = FIO_ERR_READ;
			return;
		}

		if (remain >= fio_r->sector_size * fio_r->sector_n) {
			n = fwrite(fio_r->ibuffer,fio_r->sector_size * fio_r->sector_n,1,ofp);
			remain -= fio_r->sector_size * fio_r->sector_n;
		} else {
			n = fwrite(fio_r->ibuffer,remain,1,ofp);
			remain = 0;
		}
		if (n != 1) {
			fio_r->error = FIO_ERR_WRITE;
			return;
		}
	}
}
//---------------------------------------------------------------------------
// Function   : fio_clear_buffer
// Description: バッファクリア
// ---
//	fio			: fileio構造体
//
void fio_clear_buffer(FIO *fio)
{
	FIO_DATA *p_data;
	FIO_DATA *p_old;
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio->magic != 5915) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}
	
	if (fio->error) {
		return;
	}
	
	if (fio->mode == FIO_MODE_W) {
		return;
	}
	// fio data cleanup
	p_data = fio->p_data;
	while (p_data != NULL) {
		p_data->r_flag = 0;
		p_data->w_flag = 0;
		p_data->block_no = -1;
		p_data = p_data->next;
	}
}
//---------------------------------------------------------------------------
// Function   : fio_fill_buffer
// Description: バッファ先読み
// ---
//	fio			: fileio構造体
//
void fio_fill_buffer(FIO *fio)
{
	FIO_DATA *p_data;
	fio_size offset;
	fio_size remain;
	int block;
	long rs;
	int error;
	char sss[300];

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio->magic != 5915) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return;
	}
	
	if (fio->error) {
		return;
	}
	
	if (fio->mode == FIO_MODE_W) {
		return;
	}
	
	offset = fio->data_offset;
	offset = (offset / fio->block_size) * fio->block_size;
	remain = fio->data_size - offset;
	if (remain == 0) {
		return;
	}

	p_data = fio->p_data;
	while (p_data != NULL) {
		remain = fio->data_size - offset;
		if (fio->data_maxsize > 0 && fio->data_size > fio->data_maxsize) {
			if (offset >= fio->data_maxsize) {
				return;
			}
			remain = fio->data_maxsize - offset;
		}
		if (remain > fio->block_size) {
			remain = fio->block_size;
		}
		error = 1;
		if (fseek_local(fio->fp,offset,SEEK_SET) == 0) {
			rs = fread(p_data->buf,1,remain,fio->fp);
			if (rs == remain) {
				p_data->r_flag = 1;
				p_data->w_flag = 0;
				p_data->block_no = offset / fio->block_size;
				error = 0;
			}
		}
		if (error) {
			sprintf(sss,"fseek_local_error:offset:%lld",offset);
			PRINT_LOG(sss);
			fio->error = FIO_ERR_READ;
			return;
		}
		p_data = p_data->next;
		offset += fio->block_size;
		if (offset >= fio->data_size) {
			return;
		}
	}
}
//---------------------------------------------------------------------------
// Function   : fio_alloc
// Description: fio_data メモリ確保
// ---
//	fio_data  : fio_data構造体
//
FIO_DATA *fio_alloc(FIO *fio)
{
	FIO_DATA *p_data;

	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return NULL;
	}

	p_data = malloc(sizeof (FIO_DATA));
	if (p_data == NULL) {
		return NULL;
	}
	memset(p_data,0,sizeof (FIO_DATA));
	p_data->r_flag = 0;
	p_data->w_flag = 0;
	p_data->block_no = -1;
	p_data->next = NULL;
	p_data->buf = malloc(fio->block_size);
	if (p_data->buf == NULL) {
		free(p_data);
		return NULL;
	}
	memset(p_data->buf,0,fio->block_size);
	return p_data;
}

//---------------------------------------------------------------------------
// Function   : fio_free
// Description: fio_data メモリ開放
// ---
//	fio_data  : fio_data構造体
//
FIO_DATA *fio_free(FIO *fio,FIO_DATA *p_data)
{
	FIO_DATA *next;

	next = NULL;
	if (fio == NULL) {
		PRINT_LOG("ERROR");
		return NULL;
	}
	if (p_data == NULL) {
		fio->error = FIO_ERR_OTHER;
		PRINT_LOG("ERROR");
		return NULL;
	}

	next = p_data->next;
	if (p_data->buf != NULL) {
		free(p_data->buf);
		p_data->buf = NULL;
	}
	free(p_data);
	return next;
}
static fio_size ftell_local(FILE *fp)
{
	fio_size pos;
#ifdef __GNUC__
	pos = ftello64(fp);
#else
	pos = _ftelli64(fp);
#endif
	return pos;
}
static int fseek_local(FILE *fp,fio_size offset,int origin)
{
	int ret;
#ifdef __GNUC__
	ret = fseeko64(fp,offset,origin);
#else
	ret = _fseeki64(fp,offset,origin);
#endif
	return ret;
}
//---------------------------------------------------------------------------
// Function   : fio_set_logfn
// Description: ログ出力時にどの関数から呼ばれたかの情報をセットする
// ---
//	fio		: fileio 構造体
//  log_fn  : ログ出力用関数名
//
void fio_set_logfn(FIO *fio,char *log_fn)
{
	if (fio != NULL) {
		if (fio->log_fn != NULL) {
			free(fio->log_fn);
			fio->log_fn = NULL;
		}
		
		if (log_fn != NULL && strlen(log_fn) > 0) {
			fio->log_fn = strdup(log_fn);
		}
	}
}
//---------------------------------------------------------------------------
// Function   : fio_set_log_remain
// Description: ログ出力時のログ出力カウントを設定する
// ---
//	fio		   : fileio 構造体
//  log_remain : ログ出力数(残り数)
//
void fio_set_log_remain(FIO *fio,int log_remain)
{
	if (fio != NULL) {
		fio->log_remain = log_remain;
	}
}
//---------------------------------------------------------------------------
// Function   : fio_set_readahead
// Description: 読み込み時にできるだけメモリ上に読み込むようにする
// ---
//	fio		      : fileio 構造体
//  max_block_num : 最大数
//
void fio_set_readahead(FIO *fio1,FIO *fio2,FIO *fio3,FIO *fio4,FIO *fio5,FIO *fio6,int max_block_num)
{
	FIO_DATA *p_data,*p_before;
	int i;
	int error;
	int block_num;
	int before_block_max1;
	int before_block_max2;
	int before_block_max3;
	int before_block_max4;
	int before_block_max5;
	int before_block_max6;
	int fio1_ok;
	int fio2_ok;
	int fio3_ok;
	int fio4_ok;
	int fio5_ok;
	int fio6_ok;
	int fio_count;
	char sss[100];

	PRINT_LOG("fio_set_readahead");

	if (fio1 == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio1 == NULL || max_block_num < 0) {
		PRINT_LOG("ERROR");
		return;
	}

	if (fio1 != NULL && (fio1->magic != 5915 || fio1->error || fio1->mode != FIO_MODE_R)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio2 != NULL && (fio2->magic != 5915 || fio2->error || fio2->mode != FIO_MODE_R)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio3 != NULL && (fio3->magic != 5915 || fio3->error || fio3->mode != FIO_MODE_R)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio4 != NULL && (fio4->magic != 5915 || fio4->error || fio4->mode != FIO_MODE_R)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio5 != NULL && (fio5->magic != 5915 || fio5->error || fio5->mode != FIO_MODE_R)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio6 != NULL && (fio6->magic != 5915 || fio6->error || fio6->mode != FIO_MODE_R)) {
		PRINT_LOG("ERROR");
		return;
	}

	sprintf(sss,"max_block_num:%d",max_block_num);
	PRINT_LOG(sss);

	fio_count = 1;

	// FIO1
	p_data = fio1->p_data;
	while (p_data != NULL) {
		p_data = fio_free(fio1,p_data);
	}
	fio1->p_data = NULL;
	fio1->block_count = 0;

	// FIO2
	if (fio2 != NULL) {
		p_data = fio2->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio2,p_data);
		}
		fio2->p_data = NULL;
		fio2->block_count = 0;
		fio_count++;
	}

	// FIO3
	if (fio3 != NULL) {
		p_data = fio3->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio3,p_data);
		}
		fio3->p_data = NULL;
		fio3->block_count = 0;
		fio_count++;
	}

	// FIO4
	if (fio4 != NULL) {
		p_data = fio4->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio4,p_data);
		}
		fio4->p_data = NULL;
		fio4->block_count = 0;
		fio_count++;
	}

	// FIO5
	if (fio5 != NULL) {
		p_data = fio5->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio5,p_data);
		}
		fio5->p_data = NULL;
		fio5->block_count = 0;
		fio_count++;
	}

	// FIO6
	if (fio6 != NULL) {
		p_data = fio6->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio6,p_data);
		}
		fio6->p_data = NULL;
		fio6->block_count = 0;
		fio_count++;
	}

	if (fio_count == 1) {
		if (max_block_num * fio1->block_size > fio1->data_size) {
			max_block_num = (fio1->data_size / fio1->block_size) + 1;
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}

		before_block_max1 = fio1->block_max;
		fio1->block_max = max_block_num;
	} else if (fio_count == 2) {
		if (max_block_num < 2) max_block_num = 2;
		if ((max_block_num / 2) * fio1->block_size > fio1->data_size && (max_block_num / 2) * fio2->block_size > fio2->data_size) {
			if (fio1->data_size >= fio2->data_size) {
				max_block_num = (fio1->data_size / fio1->block_size) + 1;
				max_block_num *= 2;
			} else {
				max_block_num = (fio2->data_size / fio2->block_size) + 1;
				max_block_num *= 2;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		fio1->block_max = max_block_num / 2;
		fio2->block_max = max_block_num / 2;
	} else if (fio_count == 3) {
		if (max_block_num < 3) max_block_num = 3;
		if ((max_block_num / 3) * fio1->block_size > fio1->data_size && (max_block_num / 3) * fio2->block_size > fio2->data_size && (max_block_num / 3) * fio3->block_size > fio3->data_size) {
			if (fio1->data_size >= fio2->data_size && fio2->data_size >= fio3->data_size) {
				max_block_num = (fio1->data_size / fio1->block_size) + 1;
				max_block_num *= 3;
			} else if (fio2->data_size >= fio3->data_size && fio3->data_size >= fio1->data_size) {
				max_block_num = (fio2->data_size / fio2->block_size) + 1;
				max_block_num *= 3;
			} else {
				max_block_num = (fio3->data_size / fio3->block_size) + 1;
				max_block_num *= 3;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		fio1->block_max = max_block_num / 3;
		fio2->block_max = max_block_num / 3;
		fio3->block_max = max_block_num / 3;
	} else if (fio_count == 4) {
		if (max_block_num < 4) max_block_num = 4;
		if ((max_block_num / 4) * fio1->block_size > fio1->data_size && (max_block_num / 4) * fio2->block_size > fio2->data_size && (max_block_num / 4) * fio3->block_size > fio3->data_size && (max_block_num / 4) * fio4->block_size > fio4->data_size) {
			if (fio1->data_size >= fio2->data_size && fio2->data_size >= fio3->data_size && fio3->data_size >= fio4->data_size) {
				max_block_num = (fio1->data_size / fio1->block_size) + 1;
				max_block_num *= 4;
			} else if (fio2->data_size >= fio3->data_size && fio3->data_size >= fio4->data_size && fio4->data_size >= fio1->data_size) {
				max_block_num = (fio2->data_size / fio2->block_size) + 1;
				max_block_num *= 4;
			} else if (fio3->data_size >= fio4->data_size && fio4->data_size >= fio1->data_size && fio1->data_size >= fio2->data_size) {
				max_block_num = (fio3->data_size / fio3->block_size) + 1;
				max_block_num *= 4;
			} else {
				max_block_num = (fio4->data_size / fio4->block_size) + 1;
				max_block_num *= 3;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		before_block_max4 = fio4->block_max;
		fio1->block_max = max_block_num / 4;
		fio2->block_max = max_block_num / 4;
		fio3->block_max = max_block_num / 4;
		fio4->block_max = max_block_num / 4;
	} else if (fio_count == 5) {
		if (max_block_num < 5) max_block_num = 5;
		if ((max_block_num / 5) * fio1->block_size > fio1->data_size && (max_block_num / 5) * fio2->block_size > fio2->data_size && (max_block_num / 5) * fio3->block_size > fio3->data_size && (max_block_num / 5) * fio4->block_size > fio4->data_size && (max_block_num / 5) * fio5->block_size > fio5->data_size) {
			if (fio1->data_size >= fio2->data_size && fio2->data_size >= fio3->data_size && fio3->data_size >= fio4->data_size && fio4->data_size >= fio5->data_size) {
				max_block_num = (fio1->data_size / fio1->block_size) + 1;
				max_block_num *= 5;
			} else if (fio2->data_size >= fio3->data_size && fio3->data_size >= fio4->data_size && fio4->data_size >= fio5->data_size && fio5->data_size >= fio1->data_size) {
				max_block_num = (fio2->data_size / fio2->block_size) + 1;
				max_block_num *= 5;
			} else if (fio3->data_size >= fio4->data_size && fio4->data_size >= fio5->data_size && fio5->data_size >= fio1->data_size && fio1->data_size >= fio2->data_size) {
				max_block_num = (fio3->data_size / fio3->block_size) + 1;
				max_block_num *= 5;
			} else if (fio4->data_size >= fio5->data_size && fio5->data_size >= fio1->data_size && fio1->data_size >= fio2->data_size && fio2->data_size >= fio3->data_size) {
				max_block_num = (fio4->data_size / fio4->block_size) + 1;
				max_block_num *= 5;
			} else {
				max_block_num = (fio5->data_size / fio5->block_size) + 1;
				max_block_num *= 5;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		before_block_max4 = fio4->block_max;
		before_block_max5 = fio5->block_max;
		fio1->block_max = max_block_num / 5;
		fio2->block_max = max_block_num / 5;
		fio3->block_max = max_block_num / 5;
		fio4->block_max = max_block_num / 5;
		fio5->block_max = max_block_num / 5;
	} else if (fio_count == 6) {
		if (max_block_num < 6) max_block_num = 6;
		if ((max_block_num / 6) * fio1->block_size > fio1->data_size && (max_block_num / 6) * fio2->block_size > fio2->data_size && (max_block_num / 6) * fio3->block_size > fio3->data_size && (max_block_num / 6) * fio4->block_size > fio4->data_size && (max_block_num / 6) * fio5->block_size > fio5->data_size && (max_block_num / 6) * fio6->block_size > fio6->data_size) {
			if (fio1->data_size >= fio2->data_size && fio2->data_size >= fio3->data_size && fio3->data_size >= fio4->data_size && fio4->data_size >= fio5->data_size && fio5->data_size >= fio1->data_size) {
				max_block_num = (fio1->data_size / fio1->block_size) + 1;
				max_block_num *= 6;
			} else if (fio2->data_size >= fio3->data_size && fio3->data_size >= fio4->data_size && fio4->data_size >= fio5->data_size && fio5->data_size >= fio6->data_size && fio6->data_size >= fio1->data_size) {
				max_block_num = (fio2->data_size / fio2->block_size) + 1;
				max_block_num *= 6;
			} else if (fio3->data_size >= fio4->data_size && fio4->data_size >= fio5->data_size && fio5->data_size >= fio6->data_size && fio6->data_size >= fio1->data_size &&  fio1->data_size >= fio2->data_size) {
				max_block_num = (fio3->data_size / fio3->block_size) + 1;
				max_block_num *= 6;
			} else if (fio4->data_size >= fio5->data_size && fio5->data_size >= fio6->data_size && fio6->data_size >= fio1->data_size && fio1->data_size >= fio2->data_size && fio2->data_size >= fio3->data_size) {
				max_block_num = (fio4->data_size / fio4->block_size) + 1;
				max_block_num *= 6;
			} else if (fio5->data_size >= fio6->data_size && fio6->data_size >= fio1->data_size && fio1->data_size >= fio2->data_size && fio2->data_size >= fio3->data_size && fio3->data_size >= fio4->data_size) {
				max_block_num = (fio5->data_size / fio5->block_size) + 1;
				max_block_num *= 6;
			} else {
				max_block_num = (fio6->data_size / fio6->block_size) + 1;
				max_block_num *= 6;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		before_block_max4 = fio4->block_max;
		before_block_max5 = fio5->block_max;
		before_block_max6 = fio6->block_max;
		fio1->block_max = max_block_num / 6;
		fio2->block_max = max_block_num / 6;
		fio3->block_max = max_block_num / 6;
		fio4->block_max = max_block_num / 6;
		fio5->block_max = max_block_num / 6;
		fio6->block_max = max_block_num / 6;
	}
	if (fio1 != NULL && fio1->block_max < 1) fio1->block_max = 1;
	if (fio2 != NULL && fio2->block_max < 1) fio2->block_max = 1;
	if (fio3 != NULL && fio3->block_max < 1) fio3->block_max = 1;
	if (fio4 != NULL && fio4->block_max < 1) fio4->block_max = 1;
	if (fio5 != NULL && fio5->block_max < 1) fio5->block_max = 1;
	if (fio6 != NULL && fio6->block_max < 1) fio6->block_max = 1;

	do {
		fio1_ok = fio2_ok = fio3_ok = fio4_ok = fio5_ok = fio6_ok = 1;

		if (fio1 != NULL) {
			sprintf(sss,"try fio1->block_max:%ld",fio1->block_max);
			PRINT_LOG(sss);
		}
		if (fio2 != NULL) {
			sprintf(sss,"try fio2->block_max:%ld",fio2->block_max);
			PRINT_LOG(sss);
		}
		if (fio3 != NULL) {
			sprintf(sss,"try fio3->block_max:%ld",fio3->block_max);
			PRINT_LOG(sss);
		}
		if (fio4 != NULL) {
			sprintf(sss,"try fio4->block_max:%ld",fio4->block_max);
			PRINT_LOG(sss);
		}
		if (fio5 != NULL) {
			sprintf(sss,"try fio5->block_max:%ld",fio5->block_max);
			PRINT_LOG(sss);
		}
		if (fio6 != NULL) {
			sprintf(sss,"try fio6->block_max:%ld",fio6->block_max);
			PRINT_LOG(sss);
		}

		// FIO1
		if (fio1 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio1->block_max;i++) {
				p_data = fio_alloc(fio1);
				if (p_data == NULL) {
					fio1_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio1->p_data = p_data;
				fio1->block_count++;
			}
		}

		// FIO2
		if (fio2 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio2->block_max;i++) {
				p_data = fio_alloc(fio2);
				if (p_data == NULL) {
					fio2_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio2->p_data = p_data;
				fio2->block_count++;
			}
		}

		// FIO3
		if (fio3 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio3->block_max;i++) {
				p_data = fio_alloc(fio3);
				if (p_data == NULL) {
					fio3_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio3->p_data = p_data;
				fio3->block_count++;
			}
		}

		// FIO4
		if (fio4 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio4->block_max;i++) {
				p_data = fio_alloc(fio4);
				if (p_data == NULL) {
					fio4_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio4->p_data = p_data;
				fio4->block_count++;
			}
		}

		// FIO5
		if (fio5 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio5->block_max;i++) {
				p_data = fio_alloc(fio5);
				if (p_data == NULL) {
					fio5_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio5->p_data = p_data;
				fio5->block_count++;
			}
		}

		// FIO6
		if (fio6 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio6->block_max;i++) {
				p_data = fio_alloc(fio6);
				if (p_data == NULL) {
					fio6_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio6->p_data = p_data;
				fio6->block_count++;
			}
		}
		
		if (fio1_ok == 0 || fio2_ok == 0 || fio3_ok == 0 || fio4_ok == 0 || fio5_ok == 0 || fio6_ok == 0) {
			if (fio1 != NULL && fio1->block_count < 2) {
				fio1->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio2 != NULL && fio2->block_count < 2) {
				fio2->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio3 != NULL && fio3->block_count < 2) {
				fio3->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio4 != NULL && fio4->block_count < 2) {
				fio4->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio5 != NULL && fio5->block_count < 2) {
				fio5->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio6 != NULL && fio6->block_count < 2) {
				fio6->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			// FIO1
			if (fio1 != NULL) {
				p_data = fio1->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio1,p_data);
				}
				fio1->p_data = NULL;
				fio1->block_count = 0;
			}

			// FIO2
			if (fio2 != NULL) {
				p_data = fio2->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio2,p_data);
				}
				fio2->p_data = NULL;
				fio2->block_count = 0;
			}

			// FIO3
			if (fio3 != NULL) {
				p_data = fio3->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio3,p_data);
				}
				fio3->p_data = NULL;
				fio3->block_count = 0;
			}

			// FIO4
			if (fio4 != NULL) {
				p_data = fio4->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio4,p_data);
				}
				fio4->p_data = NULL;
				fio4->block_count = 0;
			}

			// FIO5
			if (fio5 != NULL) {
				p_data = fio5->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio5,p_data);
				}
				fio5->p_data = NULL;
				fio5->block_count = 0;
			}

			// FIO6
			if (fio6 != NULL) {
				p_data = fio6->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio6,p_data);
				}
				fio6->p_data = NULL;
				fio6->block_count = 0;
			}

			if (fio1 != NULL && fio1->block_max > 20) {
				fio1->block_max -= 20;
			} else if (fio1 != NULL && fio1->block_max > 5) {
				fio1->block_max -= 5;
			} else if (fio1 != NULL && fio1->block_max > 1) {
				fio1->block_max -= 1;
			}

			if (fio2 != NULL && fio2->block_max > 20) {
				fio2->block_max -= 20;
			} else if (fio2 != NULL && fio2->block_max > 5) {
				fio2->block_max -= 5;
			} else if (fio2 != NULL && fio2->block_max > 1) {
				fio2->block_max -= 1;
			}

			if (fio3 != NULL && fio3->block_max > 20) {
				fio3->block_max -= 20;
			} else if (fio3 != NULL && fio3->block_max > 5) {
				fio3->block_max -= 5;
			} else if (fio3 != NULL && fio3->block_max > 1) {
				fio3->block_max -= 1;
			}

			if (fio4 != NULL && fio4->block_max > 20) {
				fio4->block_max -= 20;
			} else if (fio4 != NULL && fio4->block_max > 5) {
				fio4->block_max -= 5;
			} else if (fio4 != NULL && fio4->block_max > 1) {
				fio4->block_max -= 1;
			}

			if (fio5 != NULL && fio5->block_max > 20) {
				fio5->block_max -= 20;
			} else if (fio5 != NULL && fio5->block_max > 5) {
				fio5->block_max -= 5;
			} else if (fio5 != NULL && fio5->block_max > 1) {
				fio5->block_max -= 1;
			}

			if (fio6 != NULL && fio6->block_max > 20) {
				fio6->block_max -= 20;
			} else if (fio6 != NULL && fio6->block_max > 5) {
				fio6->block_max -= 5;
			} else if (fio6 != NULL && fio6->block_max > 1) {
				fio6->block_max -= 1;
			}
		}
	} while (fio1_ok == 0 || fio2_ok == 0 || fio3_ok == 0 || fio4_ok == 0 || fio5_ok == 0 || fio6_ok == 0);

	if (fio1 != NULL) {
		sprintf(sss,"fio1->block_max:%ld",fio1->block_max);
		PRINT_LOG(sss);
	}
	if (fio2 != NULL) {
		sprintf(sss,"fio2->block_max:%ld",fio2->block_max);
		PRINT_LOG(sss);
	}
	if (fio3 != NULL) {
		sprintf(sss,"fio3->block_max:%ld",fio3->block_max);
		PRINT_LOG(sss);
	}
	if (fio4 != NULL) {
		sprintf(sss,"fio4->block_max:%ld",fio4->block_max);
		PRINT_LOG(sss);
	}
	if (fio5 != NULL) {
		sprintf(sss,"fio5->block_max:%ld",fio5->block_max);
		PRINT_LOG(sss);
	}
	if (fio6 != NULL) {
		sprintf(sss,"fio6->block_max:%ld",fio6->block_max);
		PRINT_LOG(sss);
	}

	if (fio1 != NULL) fio1->readahead = 1;
	if (fio2 != NULL) fio2->readahead = 1;
	if (fio3 != NULL) fio3->readahead = 1;
	if (fio4 != NULL) fio4->readahead = 1;
	if (fio5 != NULL) fio5->readahead = 1;
	if (fio6 != NULL) fio6->readahead = 1;
}

//---------------------------------------------------------------------------
// Function   : fio_set_writecache
// Description: 書き込み時にできるだけメモリ上に書き込むようにする
// ---
//	fio		      : fileio 構造体
//  max_block_num : 最大数
//
void fio_set_writecache(FIO *fio1,FIO *fio2,FIO *fio3,FIO *fio4,FIO *fio5,FIO *fio6,int max_block_num)
{
	FIO_DATA *p_data,*p_before;
	int i;
	int error;
	int block_num;
	int before_block_max1;
	int before_block_max2;
	int before_block_max3;
	int before_block_max4;
	int before_block_max5;
	int before_block_max6;
	int fio1_ok;
	int fio2_ok;
	int fio3_ok;
	int fio4_ok;
	int fio5_ok;
	int fio6_ok;
	int fio_count;
	char sss[100];

	PRINT_LOG("fio_set_writecache");

	if (fio1 == NULL) {
		PRINT_LOG("ERROR");
		return;
	}

	// check magic no
	if (fio1 == NULL || max_block_num < 0) {
		PRINT_LOG("ERROR");
		return;
	}

	if (fio1 != NULL && (fio1->magic != 5915 || fio1->error || fio1->mode != FIO_MODE_W)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio2 != NULL && (fio2->magic != 5915 || fio2->error || fio2->mode != FIO_MODE_W)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio3 != NULL && (fio3->magic != 5915 || fio3->error || fio3->mode != FIO_MODE_W)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio4 != NULL && (fio4->magic != 5915 || fio4->error || fio4->mode != FIO_MODE_W)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio5 != NULL && (fio5->magic != 5915 || fio5->error || fio5->mode != FIO_MODE_W)) {
		PRINT_LOG("ERROR");
		return;
	}
	if (fio6 != NULL && (fio6->magic != 5915 || fio6->error || fio6->mode != FIO_MODE_W)) {
		PRINT_LOG("ERROR");
		return;
	}

	sprintf(sss,"max_block_num:%d",max_block_num);
	PRINT_LOG(sss);

	fio_count = 1;

	// FIO1
	p_data = fio1->p_data;
	while (p_data != NULL) {
		p_data = fio_free(fio1,p_data);
	}
	fio1->p_data = NULL;
	fio1->block_count = 0;

	// FIO2
	if (fio2 != NULL) {
		p_data = fio2->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio2,p_data);
		}
		fio2->p_data = NULL;
		fio2->block_count = 0;
		fio_count++;
	}

	// FIO3
	if (fio3 != NULL) {
		p_data = fio3->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio3,p_data);
		}
		fio3->p_data = NULL;
		fio3->block_count = 0;
		fio_count++;
	}

	// FIO4
	if (fio4 != NULL) {
		p_data = fio4->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio4,p_data);
		}
		fio4->p_data = NULL;
		fio4->block_count = 0;
		fio_count++;
	}

	// FIO5
	if (fio5 != NULL) {
		p_data = fio5->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio5,p_data);
		}
		fio5->p_data = NULL;
		fio5->block_count = 0;
		fio_count++;
	}

	// FIO6
	if (fio6 != NULL) {
		p_data = fio6->p_data;
		while (p_data != NULL) {
			p_data = fio_free(fio6,p_data);
		}
		fio6->p_data = NULL;
		fio6->block_count = 0;
		fio_count++;
	}

	if (fio_count == 1) {
		if (max_block_num * fio1->block_size > fio1->data_maxsize) {
			max_block_num = (fio1->data_maxsize / fio1->block_size) + 1;
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}

		before_block_max1 = fio1->block_max;
		fio1->block_max = max_block_num;
	} else if (fio_count == 2) {
		if (max_block_num < 2) max_block_num = 2;
		if ((max_block_num / 2) * fio1->block_size > fio1->data_maxsize && (max_block_num / 2) * fio2->block_size > fio2->data_maxsize) {
			if (fio1->data_maxsize >= fio2->data_maxsize) {
				max_block_num = (fio1->data_maxsize / fio1->block_size) + 1;
				max_block_num *= 2;
			} else {
				max_block_num = (fio2->data_maxsize / fio2->block_size) + 1;
				max_block_num *= 2;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		fio1->block_max = max_block_num / 2;
		fio2->block_max = max_block_num / 2;
	} else if (fio_count == 3) {
		if (max_block_num < 3) max_block_num = 3;
		if ((max_block_num / 3) * fio1->block_size > fio1->data_maxsize && (max_block_num / 3) * fio2->block_size > fio2->data_maxsize && (max_block_num / 3) * fio3->block_size > fio3->data_maxsize) {
			if (fio1->data_maxsize >= fio2->data_maxsize && fio2->data_maxsize >= fio3->data_maxsize) {
				max_block_num = (fio1->data_maxsize / fio1->block_size) + 1;
				max_block_num *= 3;
			} else if (fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio1->data_maxsize) {
				max_block_num = (fio2->data_maxsize / fio2->block_size) + 1;
				max_block_num *= 3;
			} else {
				max_block_num = (fio3->data_maxsize / fio3->block_size) + 1;
				max_block_num *= 3;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		fio1->block_max = max_block_num / 3;
		fio2->block_max = max_block_num / 3;
		fio3->block_max = max_block_num / 3;
	} else if (fio_count == 4) {
		if (max_block_num < 4) max_block_num = 4;
		if ((max_block_num / 4) * fio1->block_size > fio1->data_maxsize && (max_block_num / 4) * fio2->block_size > fio2->data_maxsize && (max_block_num / 4) * fio3->block_size > fio3->data_maxsize && (max_block_num / 4) * fio4->block_size > fio4->data_maxsize) {
			if (fio1->data_maxsize >= fio2->data_maxsize && fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio4->data_maxsize) {
				max_block_num = (fio1->data_maxsize / fio1->block_size) + 1;
				max_block_num *= 4;
			} else if (fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio1->data_maxsize) {
				max_block_num = (fio2->data_maxsize / fio2->block_size) + 1;
				max_block_num *= 4;
			} else if (fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio1->data_maxsize && fio1->data_maxsize >= fio2->data_maxsize) {
				max_block_num = (fio3->data_maxsize / fio3->block_size) + 1;
				max_block_num *= 4;
			} else {
				max_block_num = (fio4->data_maxsize / fio4->block_size) + 1;
				max_block_num *= 3;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		before_block_max4 = fio4->block_max;
		fio1->block_max = max_block_num / 4;
		fio2->block_max = max_block_num / 4;
		fio3->block_max = max_block_num / 4;
		fio4->block_max = max_block_num / 4;
	} else if (fio_count == 5) {
		if (max_block_num < 5) max_block_num = 5;
		if ((max_block_num / 5) * fio1->block_size > fio1->data_maxsize && (max_block_num / 5) * fio2->block_size > fio2->data_maxsize && (max_block_num / 5) * fio3->block_size > fio3->data_maxsize && (max_block_num / 5) * fio4->block_size > fio4->data_maxsize && (max_block_num / 5) * fio5->block_size > fio5->data_maxsize) {
			if (fio1->data_maxsize >= fio2->data_maxsize && fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio5->data_maxsize) {
				max_block_num = (fio1->data_maxsize / fio1->block_size) + 1;
				max_block_num *= 5;
			} else if (fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio5->data_maxsize && fio5->data_maxsize >= fio1->data_maxsize) {
				max_block_num = (fio2->data_maxsize / fio2->block_size) + 1;
				max_block_num *= 5;
			} else if (fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio5->data_maxsize && fio5->data_maxsize >= fio1->data_maxsize && fio1->data_maxsize >= fio2->data_maxsize) {
				max_block_num = (fio3->data_maxsize / fio3->block_size) + 1;
				max_block_num *= 5;
			} else if (fio4->data_maxsize >= fio5->data_maxsize && fio5->data_maxsize >= fio1->data_maxsize && fio1->data_maxsize >= fio2->data_maxsize && fio2->data_maxsize >= fio3->data_maxsize) {
				max_block_num = (fio4->data_maxsize / fio4->block_size) + 1;
				max_block_num *= 5;
			} else {
				max_block_num = (fio5->data_maxsize / fio5->block_size) + 1;
				max_block_num *= 5;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		before_block_max4 = fio4->block_max;
		before_block_max5 = fio5->block_max;
		fio1->block_max = max_block_num / 5;
		fio2->block_max = max_block_num / 5;
		fio3->block_max = max_block_num / 5;
		fio4->block_max = max_block_num / 5;
		fio5->block_max = max_block_num / 5;
	} else if (fio_count == 6) {
		if (max_block_num < 6) max_block_num = 6;
		if ((max_block_num / 6) * fio1->block_size > fio1->data_maxsize && (max_block_num / 6) * fio2->block_size > fio2->data_maxsize && (max_block_num / 6) * fio3->block_size > fio3->data_maxsize && (max_block_num / 6) * fio4->block_size > fio4->data_maxsize && (max_block_num / 6) * fio5->block_size > fio5->data_maxsize && (max_block_num / 6) * fio6->block_size > fio6->data_maxsize) {
			if (fio1->data_maxsize >= fio2->data_maxsize && fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio5->data_maxsize && fio5->data_maxsize >= fio1->data_maxsize) {
				max_block_num = (fio1->data_maxsize / fio1->block_size) + 1;
				max_block_num *= 6;
			} else if (fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio5->data_maxsize && fio5->data_maxsize >= fio6->data_maxsize && fio6->data_maxsize >= fio1->data_maxsize) {
				max_block_num = (fio2->data_maxsize / fio2->block_size) + 1;
				max_block_num *= 6;
			} else if (fio3->data_maxsize >= fio4->data_maxsize && fio4->data_maxsize >= fio5->data_maxsize && fio5->data_maxsize >= fio6->data_maxsize && fio6->data_maxsize >= fio1->data_maxsize &&  fio1->data_maxsize >= fio2->data_maxsize) {
				max_block_num = (fio3->data_maxsize / fio3->block_size) + 1;
				max_block_num *= 6;
			} else if (fio4->data_maxsize >= fio5->data_maxsize && fio5->data_maxsize >= fio6->data_maxsize && fio6->data_maxsize >= fio1->data_maxsize && fio1->data_maxsize >= fio2->data_maxsize && fio2->data_maxsize >= fio3->data_maxsize) {
				max_block_num = (fio4->data_maxsize / fio4->block_size) + 1;
				max_block_num *= 6;
			} else if (fio5->data_maxsize >= fio6->data_maxsize && fio6->data_maxsize >= fio1->data_maxsize && fio1->data_maxsize >= fio2->data_maxsize && fio2->data_maxsize >= fio3->data_maxsize && fio3->data_maxsize >= fio4->data_maxsize) {
				max_block_num = (fio5->data_maxsize / fio5->block_size) + 1;
				max_block_num *= 6;
			} else {
				max_block_num = (fio6->data_maxsize / fio6->block_size) + 1;
				max_block_num *= 6;
			}
			sprintf(sss,"resize max_block_num:%d",max_block_num);
			PRINT_LOG(sss);
		}
		before_block_max1 = fio1->block_max;
		before_block_max2 = fio2->block_max;
		before_block_max3 = fio3->block_max;
		before_block_max4 = fio4->block_max;
		before_block_max5 = fio5->block_max;
		before_block_max6 = fio6->block_max;
		fio1->block_max = max_block_num / 6;
		fio2->block_max = max_block_num / 6;
		fio3->block_max = max_block_num / 6;
		fio4->block_max = max_block_num / 6;
		fio5->block_max = max_block_num / 6;
		fio6->block_max = max_block_num / 6;
	}
	if (fio1 != NULL && fio1->block_max < 1) fio1->block_max = 1;
	if (fio2 != NULL && fio2->block_max < 1) fio2->block_max = 1;
	if (fio3 != NULL && fio3->block_max < 1) fio3->block_max = 1;
	if (fio4 != NULL && fio4->block_max < 1) fio4->block_max = 1;
	if (fio5 != NULL && fio5->block_max < 1) fio5->block_max = 1;
	if (fio6 != NULL && fio6->block_max < 1) fio6->block_max = 1;

	do {
		fio1_ok = fio2_ok = fio3_ok = fio4_ok = fio5_ok = fio6_ok = 1;

		if (fio1 != NULL) {
			sprintf(sss,"try fio1->block_max:%ld",fio1->block_max);
			PRINT_LOG(sss);
		}
		if (fio2 != NULL) {
			sprintf(sss,"try fio2->block_max:%ld",fio2->block_max);
			PRINT_LOG(sss);
		}
		if (fio3 != NULL) {
			sprintf(sss,"try fio3->block_max:%ld",fio3->block_max);
			PRINT_LOG(sss);
		}
		if (fio4 != NULL) {
			sprintf(sss,"try fio4->block_max:%ld",fio4->block_max);
			PRINT_LOG(sss);
		}
		if (fio5 != NULL) {
			sprintf(sss,"try fio5->block_max:%ld",fio5->block_max);
			PRINT_LOG(sss);
		}
		if (fio6 != NULL) {
			sprintf(sss,"try fio6->block_max:%ld",fio6->block_max);
			PRINT_LOG(sss);
		}

		// FIO1
		if (fio1 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio1->block_max;i++) {
				p_data = fio_alloc(fio1);
				if (p_data == NULL) {
					fio1_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio1->p_data = p_data;
				fio1->block_count++;
			}
		}

		// FIO2
		if (fio2 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio2->block_max;i++) {
				p_data = fio_alloc(fio2);
				if (p_data == NULL) {
					fio2_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio2->p_data = p_data;
				fio2->block_count++;
			}
		}

		// FIO3
		if (fio3 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio3->block_max;i++) {
				p_data = fio_alloc(fio3);
				if (p_data == NULL) {
					fio3_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio3->p_data = p_data;
				fio3->block_count++;
			}
		}

		// FIO4
		if (fio4 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio4->block_max;i++) {
				p_data = fio_alloc(fio4);
				if (p_data == NULL) {
					fio4_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio4->p_data = p_data;
				fio4->block_count++;
			}
		}

		// FIO5
		if (fio5 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio5->block_max;i++) {
				p_data = fio_alloc(fio5);
				if (p_data == NULL) {
					fio5_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio5->p_data = p_data;
				fio5->block_count++;
			}
		}

		// FIO6
		if (fio6 != NULL) {
			p_before = NULL;
			for (i = 0;i < fio6->block_max;i++) {
				p_data = fio_alloc(fio6);
				if (p_data == NULL) {
					fio6_ok = 0;
					break;
				}
				p_data->next = p_before;
				p_before = p_data;
				fio6->p_data = p_data;
				fio6->block_count++;
			}
		}
		
		if (fio1_ok == 0 || fio2_ok == 0 || fio3_ok == 0 || fio4_ok == 0 || fio5_ok == 0 || fio6_ok == 0) {
			if (fio1 != NULL && fio1->block_count < 2) {
				fio1->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio2 != NULL && fio2->block_count < 2) {
				fio2->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio3 != NULL && fio3->block_count < 2) {
				fio3->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio4 != NULL && fio4->block_count < 2) {
				fio4->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio5 != NULL && fio5->block_count < 2) {
				fio5->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			if (fio6 != NULL && fio6->block_count < 2) {
				fio6->error = FIO_ERR_MEMORY;
				PRINT_LOG("ERROR");
				return;
			}

			// FIO1
			if (fio1 != NULL) {
				p_data = fio1->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio1,p_data);
				}
				fio1->p_data = NULL;
				fio1->block_count = 0;
			}

			// FIO2
			if (fio2 != NULL) {
				p_data = fio2->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio2,p_data);
				}
				fio2->p_data = NULL;
				fio2->block_count = 0;
			}

			// FIO3
			if (fio3 != NULL) {
				p_data = fio3->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio3,p_data);
				}
				fio3->p_data = NULL;
				fio3->block_count = 0;
			}

			// FIO4
			if (fio4 != NULL) {
				p_data = fio4->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio4,p_data);
				}
				fio4->p_data = NULL;
				fio4->block_count = 0;
			}

			// FIO5
			if (fio5 != NULL) {
				p_data = fio5->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio5,p_data);
				}
				fio5->p_data = NULL;
				fio5->block_count = 0;
			}

			// FIO6
			if (fio6 != NULL) {
				p_data = fio6->p_data;
				while (p_data != NULL) {
					p_data = fio_free(fio6,p_data);
				}
				fio6->p_data = NULL;
				fio6->block_count = 0;
			}

			if (fio1 != NULL && fio1->block_max > 20) {
				fio1->block_max -= 20;
			} else if (fio1 != NULL && fio1->block_max > 5) {
				fio1->block_max -= 5;
			} else if (fio1 != NULL && fio1->block_max > 1) {
				fio1->block_max -= 1;
			}

			if (fio2 != NULL && fio2->block_max > 20) {
				fio2->block_max -= 20;
			} else if (fio2 != NULL && fio2->block_max > 5) {
				fio2->block_max -= 5;
			} else if (fio2 != NULL && fio2->block_max > 1) {
				fio2->block_max -= 1;
			}

			if (fio3 != NULL && fio3->block_max > 20) {
				fio3->block_max -= 20;
			} else if (fio3 != NULL && fio3->block_max > 5) {
				fio3->block_max -= 5;
			} else if (fio3 != NULL && fio3->block_max > 1) {
				fio3->block_max -= 1;
			}

			if (fio4 != NULL && fio4->block_max > 20) {
				fio4->block_max -= 20;
			} else if (fio4 != NULL && fio4->block_max > 5) {
				fio4->block_max -= 5;
			} else if (fio4 != NULL && fio4->block_max > 1) {
				fio4->block_max -= 1;
			}

			if (fio5 != NULL && fio5->block_max > 20) {
				fio5->block_max -= 20;
			} else if (fio5 != NULL && fio5->block_max > 5) {
				fio5->block_max -= 5;
			} else if (fio5 != NULL && fio5->block_max > 1) {
				fio5->block_max -= 1;
			}

			if (fio6 != NULL && fio6->block_max > 20) {
				fio6->block_max -= 20;
			} else if (fio6 != NULL && fio6->block_max > 5) {
				fio6->block_max -= 5;
			} else if (fio6 != NULL && fio6->block_max > 1) {
				fio6->block_max -= 1;
			}
		}
	} while (fio1_ok == 0 || fio2_ok == 0 || fio3_ok == 0 || fio4_ok == 0 || fio5_ok == 0 || fio6_ok == 0);

	if (fio1 != NULL) {
		sprintf(sss,"fio1->block_max:%ld",fio1->block_max);
		PRINT_LOG(sss);
	}
	if (fio2 != NULL) {
		sprintf(sss,"fio2->block_max:%ld",fio2->block_max);
		PRINT_LOG(sss);
	}
	if (fio3 != NULL) {
		sprintf(sss,"fio3->block_max:%ld",fio3->block_max);
		PRINT_LOG(sss);
	}
	if (fio4 != NULL) {
		sprintf(sss,"fio4->block_max:%ld",fio4->block_max);
		PRINT_LOG(sss);
	}
	if (fio5 != NULL) {
		sprintf(sss,"fio5->block_max:%ld",fio5->block_max);
		PRINT_LOG(sss);
	}
	if (fio6 != NULL) {
		sprintf(sss,"fio6->block_max:%ld",fio6->block_max);
		PRINT_LOG(sss);
	}

	if (fio1 != NULL) fio1->writecache = 1;
	if (fio2 != NULL) fio2->writecache = 1;
	if (fio3 != NULL) fio3->writecache = 1;
	if (fio4 != NULL) fio4->writecache = 1;
	if (fio5 != NULL) fio5->writecache = 1;
	if (fio6 != NULL) fio6->writecache = 1;
}
//---------------------------------------------------------------------------
// Function   : fio_set_parameter
// Description: パラメータ設定
// ---
//	fio		  : fileio 構造体
//  name      : パラメータ名
//  value     : パラメータ値
//
void fio_set_parameter(FIO *fio,char *name,long value)
{
	int i,n;
	char str[20];
	if (fio->error) {
		return;
	}

	for (i = 0;i < 64;i++) {
		if (fio->param[i] == NULL || strcmp(fio->param[i],name) == 0) break;
	}
	if (i >= 64) {
		fio->error = FIO_ERR_OTHER;
		return;
	}

	if (fio->param[i] == NULL) {
		fio->param[i] = malloc(strlen(name) + 1);
		if (fio->param[i] == NULL) {
			fio->error = FIO_ERR_MEMORY;
			return;
		}
	}
	if (fio->value[i] == NULL) {
		sprintf(str,"%ld",value);
		fio->value[i] = malloc(strlen(str) + 1);
		if (fio->value[i] == NULL) {
			fio->error = FIO_ERR_MEMORY;
			return;
		}
	}

	strcpy(fio->param[i],name);
	strcpy(fio->value[i],str);
	
	if (strcmp(name,"upconv_n") == 0) {
		fio->upconv_n = value;
	}
	if (strcmp(name,"fio_sec_size") == 0) {
		fio->sector_n = value;
	}
}
#if defined(__APPLE__)
//---------------------------------------------------------------------------
// Function   : _splitpath
// Description: パスを結合する
// ---
//
void _splitpath(const char *path,char *drive,char *dir,char *fname,char *ext)
{
	char *p1,*p2;

	if (drive != NULL) drive[0] = '\0';
	if (dir   != NULL) dir[0] = '\0';
	if (fname != NULL) fname[0] = '\0';
	if (ext   != NULL) ext[0] = '\0';
	
	if (!(path == NULL || strlen(path) == 0)) {
		if (dir != NULL) {
			strcpy(dir,path);
			p1 = dir;
			do {
				p1 = strchr(p1,'\\');
				if (p1 != NULL) {
					*p1 = '/';
					p1++;
					if (strlen(p1) < 1) break;
				} else {
					break;
				}
			} while (1);
			p1 = strrchr(dir,'/');
			if (p1 != NULL && dir != p1) {
				if (strlen(p1 + 1) > 0) p1[1] = '\0';
			} else {
				dir[0] = '/';
				dir[1] = '\0';
			}
		}

		if (fname != NULL) {
			p1 = strrchr(path,'\\');
			if (p1 != NULL && strlen(p1 + 1) > 0) {
				strcpy(fname,p1 + 1);
			} else {
				p1 = strrchr(path,'/');
				if (p1 != NULL && strlen(p1 + 1) > 0) {
					strcpy(fname,p1 + 1);
				} else if (p1 == NULL) {
					strcpy(fname,path);
				}
			}
			p1 = strrchr(fname,'.');
			if (p1 != NULL) {
				*p1 = '\0';
			}
		}
		if (ext != NULL) {
			p1 = strrchr(path,'\\');
			if (p1 == NULL) {
				p1 = strrchr(path,'/');
				if (p1 == NULL) {
					p1 = path;
				} else {
					p1++;
				}
			} else {
				p1++;
			}
			if (strlen(p1) > 0) {
				p2 = strrchr(p1,'.');
				if (p2 != NULL) {
					strcpy(ext,p2);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
// Function   : _makepath
// Description: パスを結合する
// ---
//
void _makepath(char *path,const char *drive,const char *dir,const char *fname,const char *ext)
{
	int len;
	if (path != NULL) {
		path[0] = '\0';
		
		if (drive != NULL) {
			strcat(path,drive);
		}
		if (dir != NULL) {
			strcat(path,dir);
			len = strlen(path);
			if (len > 0 && path[len - 1] != '/') {
				strcat(path,"/");
			}
		}
		if (fname != NULL) {
			strcat(path,fname);
		}
		if (ext != NULL) {
			strcat(path,ext);
		}
	}
}
#endif

