//---------------------------------------------------------------------------
/****************************************************************************/
/* PLG_AUDIO_IO (C) 2008-2012 By 59414d41									*/
/*																			*/
/*																			*/
/****************************************************************************/

#ifndef PLG_AUDIOIO_H
#define PLG_AUDIOIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

typedef struct {
	DWORD	sample;
	DWORD	duration_sec;
	BYTE	channel;
	BYTE	bitsPerSample;
	BYTE	bitsPerSampleInt;
	BYTE	reserved1;
	BYTE	scan_mp3_flag;
	BYTE	unknown_format;
	char	fmt[16];
	char	date[10];
	char	time[8];
	char	duration_time[10];
	char	extra[128];
} SOUNDFMT;

typedef struct {
	SSIZE	fileSile;
	SSIZE	dataOffset;
	SSIZE	dataSize;
} FILEINFO;

#pragma pack(push, 1)
typedef struct upconv_parameter_info {
	char version[64];
	char dp_mtime[64];
	char default_parameter[4096];
} UPI;
#pragma pack(pop)

/* broadcast_audio_extension typedef struct */
#pragma pack(push, 1)
typedef struct broadcast_audio_extension {
	char description[256];			/* Description */
	char originator[32];			/* originator */
	char originatorReference[32];	/* Reference of the originator */
	char originationDate[10];		/* yyyy-mm-dd */
	char originationTime[8];		/* hh-mm-ss */
	DWORD timeReferenceLow;			/* First sample count since midnight low word */
	DWORD timeReferenceHigh;		/* First sample count since midnight, high word */
	WORD version;					/* Version of the BWF; unsigned binary number */
	BYTE UMID[64];					/* Binary byte 0 of SMPTE UMID */
	BYTE reserved[190];				/* 190 bytes, reserved for future use, set to ÅgNULLÅh*/
	char codingHistory[1];			/* ASCII : History coding */
} BROADCAST_EXT;
#pragma pack(pop)

/* RF64 */
#pragma pack(push, 1)
typedef struct rf64 {
	char chunkId[4];				/* RF64 */
	DWORD chunkSize;				/* 0xFFFFFFFF */
	char type[4];					/* WAVE */
} RF64;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct rf64_ds64 {
	char	chunkId[4];				/* ds64 */
	DWORD	chunkSize;				/* size of the ds64 chunk */
	DWORD	riffSizeLow;			/* size of RF64 block */
	DWORD	riffSizeHigh;			/* size of RF64 block */
	DWORD	dataSizeLow;			/* size of data chunk */
	DWORD	dataSizeHigh;			/* size of data chunk */
	DWORD	sampleCountLow;			/* sample count of fact chunk */
	DWORD	sampleCountHigh;		/* sample count of fact chunk */
	DWORD	tableLength;			/* number of valid entries in table array */
} RF64_DS64;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct rf64_fmt {
	char	chunkId[4];				/* fmt */
	DWORD	chunkSize;				/* size of the fmt chunk */
	WORD	formatType;				/* WAVE_FORMAT_PCM */
	WORD	channelCount;			/* 1 = mono, 2 = stereo */
	DWORD	sampleRate;				/* sampling rate */
	DWORD	bytesPerSecond;			/* data rate */
	WORD	blockAlignment;			
	WORD	bitsPerSample;			/* bits per sample */
} RF64_FMT;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct rf64_data {
	char	chunkId[4];				/* fmt */
	DWORD	chunkSize;				/* 0xFFFFFFFF */
	char	data[1];				/* data */
} RF64_DATA;
#pragma pack(pop)

/* wave format */
#define WF_PCM			(0x0001)
#define WF_IEEE_FLOAT	(0x0003)
#define WF_EXTENSIBLE	(0xFFFE)

#pragma pack(push, 1)
typedef struct {
	WORD	wFormatTag;
	WORD	nChannels;
	DWORD	nSamplesPerSec;
	DWORD	nAvgBytesPerSec;
	WORD	nBlockAlign;
	WORD	wBitsPerSample;
	WORD	cbSize;
} WFE;

typedef struct {
	WFE		Format;
	union {
		WORD wValidBitsPerSample;
		WORD wSamplesPerBlock;
		WORD wReserved;
	} Samples;
	DWORD	 dwChannelMask;
	char	 subFormat[16];
} WFEX;
#pragma pack(pop)

#define SPEAKER_FRONT_LEFT				(0x1)
#define SPEAKER_FRONT_RIGHT				(0x2)
#define SPEAKER_FRONT_CENTER			(0x4)
#define SPEAKER_LOW_FREQUENCY			(0x8)
#define SPEAKER_BACK_LEFT				(0x10)
#define SPEAKER_BACK_RIGHT				(0x20)

/* Function Prototype */
int PLG_InfoAudioData(char *filename,SOUNDFMT *pFmt,SSIZE *pAudioSize,FILEINFO *pFileinfo);
int PLG_MakeHeaderWAV(SOUNDFMT *pInFmt,SOUNDFMT *pOutFmt,char *header,long size,long *header_size);
int PLG_UpdateHeaderWAV(SOUNDFMT *pOutFmt,__int64 filesize,__int64 datasize,char *header,long header_size);
int PLG_MakeHeaderRF64(SOUNDFMT *pInFmt,SOUNDFMT *pOutFmt,char *header,long size,long *header_size);
int PLG_UpdateHeaderRF64(SOUNDFMT *pOutFmt,__int64 filesize,__int64 datasize,char *header,long header_size);
int PLG_MakeHeaderDSD(SOUNDFMT *pInFmt,SOUNDFMT *pOutFmt,char *header,long size,long *header_size);
int PLG_UpdateHeaderDSD(SOUNDFMT *pOutFmt,__int64 filesize,__int64 datasize,__int64 sample_count,char *header,long header_size);
int PLG_GetExtraChunk(char *filename,int nChunk,char **infoChunk,long *infoSize);
int PLG_GetChunkInfo(char *filename,char *info,int infoSize,char *picture,int *picSize,char *picTag);
int PLG_GetUpcTagInfo(char *filename,UPI *upi);
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
