#ifndef	GRAPHIC_H
#define	GRAPHIC_H

#include "inc/usr_define.h"

#define	CRT_MAX		(3)

enum
{
	POS_TOP,
	POS_CENTER,
	POS_BOTTOM,
	POS_LEFT,
	POS_MID,
	POS_RIGHT,
};

enum
{
	Clear_G=0,
	BackGround_G,
	Object1_G,
	Object2_G,
	Object3_G,
	Object4_G,
	Object5_G,
	Object6_G,
	Enemy1_G,
	Enemy2_G,
	Enemy3_G,
	Enemy4_G,
	Flip_G
};

#define	CG_MAX	(16)
#define	TRANS_PAL	(0x00)

#define FILE_TYPE			(0x424D)	/* "BM"値 */
#define FILE_HEADER_SIZE	(14)		/* BMPファイルヘッダサイズ */
#define INFO_HEADER_SIZE	(40)		/* Windowsヘッダサイズ */
#define DEFAULT_HEADER_SIZE	(FILE_HEADER_SIZE + INFO_HEADER_SIZE)	/* 標準のヘッダサイズ */

#pragma pack (push, 1)   //パディングを１byteに変更

typedef struct tagBITMAPFILEHEADER {
	US	bfType;			/* ファイルタイプ */
	UL	bfSize;			/* ファイルサイズ */
	US	bfReserved1;	/* 予約領域１ */
	US	bfReserved2;	/* 予約領域２ */
	UL	bfOffBits;		/* 先頭から画像データまでのオフセット */
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	UL	biSize;			/* ヘッダサイズ */
	SL	biWidth;		/* 画像の幅(px) */
	SL	biHeight;		/* 画像の高さ(px) */
	US	biPlanes;		/* プレーン数 常に１ */
	US	biBitCount;		/* １画素あたりのデータサイズ */
	UL	biCompression;	/* 圧縮形式 */
	UL	biSizeImage;	/* 画像データ部のサイズ */
	SL	biXPixPerMeter;	/* 横方向解像度(dot/m) */
	SL	biYPixPerMeter;	/* 縦方向解像度(dot/m) */
	UL	biClrUsed;		/* 格納されているパレット数(使用色数) */
	UL	biClrImportant;	/* 重要なパレットのインデックス */
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	UC	rgbBlue;		/* 青０～２５５ */
	UC	rgbGreen;		/* 緑０～２５５ */
	UC	rgbRed;			/* 赤０～２５５ */
	UC	rgbReserved;	/* 予約領域０ */
} RGBQUAD;

#pragma pack (pop)

typedef struct tagPICIMAGE {
	BITMAPFILEHEADER *pBMf;
	BITMAPINFOHEADER *pBMi;
	US	*pImageData;
} PICIMAGE;

typedef struct tagCG_LIST {
	SC	bFileName[256];		/* パス */
	UC	ubType;				/* 0:通常、1:スプライトライク */
	UC	ubTransPal;			/* 透過色パレット番号 */
} CG_LIST;

extern	SS	GetCRT(ST_CRT *, SS);
extern	SS	SetCRT(ST_CRT, SS);
extern	SS	CRT_INIT(void);
extern	SS	Get_CG_FileList_MaxNum(UI *);
extern	UC	*Get_CG_FileBuf(UC);
extern	SS	Get_PicImageInfo(UC , UI *, UI *, UI *);
extern	SS	Get_PicImagePallet(UC);
extern	SS	Set_PicImagePallet(UC);
extern	void	CG_File_Load(void);
extern	void	G_INIT(void);
extern	void	G_HOME(void);
extern	void	G_Palette_INIT(void);
extern	void	G_Palette(void);
extern	SS	G_Stretch_Pict( SS , US , SS , US , UC , SS , US, SS, US, UC );
extern	SS	G_BitBlt(SS , US , SS , US , UC , SS , US , SS , US , UC , UC , UC , UC );
extern	SI	G_CLR(void);
extern	SS	G_CLR_AREA(SS, US, SS, US, UC);
extern	SS	G_CLR_ALL_OFFSC(UC);
extern	SS	G_Load(UC, US, US, US);
extern	SS	G_Load_Mem(UC, US, US, US);
extern	SS	APICG_DataLoad2G(SC *, UL, UL, US);
extern	SS	APICG_DataLoad2M(UC, UL, UL, US, US *);
extern	SS	G_Subtractive_Color(US *, US *, US, US, US, UI);

#endif	/* GRAPHIC_H */
