#ifndef	GRAPHIC_H
#define	GRAPHIC_H

#include "inc/usr_define.h"

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
	Object0_G,
	Object1_G,
	Object2_G,
	Object3_G,
	Object4_G,
	Object5_G,
	Object6_G,
	Object7_G,
	Object8_G,
	Object9_G,
	ObjectA_G,
	ObjectB_G,
	ObjectC_G,
	ObjectD_G,
	ObjectE_G,
	ObjectF_G,
	Enemy1_G,
	Enemy2_G,
	Enemy3_G,
	Enemy4_G,
	MyCar_G,
	Debug_View_G,
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
	uint16_t	bfType;			/* ファイルタイプ */
	uint64_t	bfSize;			/* ファイルサイズ */
	uint16_t	bfReserved1;	/* 予約領域１ */
	uint16_t	bfReserved2;	/* 予約領域２ */
	uint64_t	bfOffBits;		/* 先頭から画像データまでのオフセット */
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	uint64_t	biSize;			/* ヘッダサイズ */
	int64_t	biWidth;		/* 画像の幅(px) */
	int64_t	biHeight;		/* 画像の高さ(px) */
	uint16_t	biPlanes;		/* プレーン数 常に１ */
	uint16_t	biBitCount;		/* １画素あたりのデータサイズ */
	uint64_t	biCompression;	/* 圧縮形式 */
	uint64_t	biSizeImage;	/* 画像データ部のサイズ */
	int64_t	biXPixPerMeter;	/* 横方向解像度(dot/m) */
	int64_t	biYPixPerMeter;	/* 縦方向解像度(dot/m) */
	uint64_t	biClrUsed;		/* 格納されているパレット数(使用色数) */
	uint64_t	biClrImportant;	/* 重要なパレットのインデックス */
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	uint8_t	rgbBlue;		/* 青０～２５５ */
	uint8_t	rgbGreen;		/* 緑０～２５５ */
	uint8_t	rgbRed;			/* 赤０～２５５ */
	uint8_t	rgbReserved;	/* 予約領域０ */
} RGBQUAD;

#pragma pack (pop)

typedef struct tagPICIMAGE {
	BITMAPFILEHEADER *pBMf;
	BITMAPINFOHEADER *pBMi;
	uint16_t	*pImageData;
} PICIMAGE;

typedef struct tagCG_LIST {
	int8_t	bFileName[256];		/* パス */
	uint8_t	ubType;				/* 0:通常、1:スプライトライク */
	uint8_t	ubTransPal;			/* 透過色パレット番号 */
} CG_LIST;

extern	int16_t	Get_CG_FileList_MaxNum(uint32_t *);
extern	uint8_t	*Get_CG_FileBuf(uint8_t);
extern	int16_t	Get_PicImageInfo(uint8_t , uint32_t *, uint32_t *, uint32_t *);
extern	int16_t	Get_PicImagePallet(uint8_t);
extern	int16_t	Set_PicImagePallet(uint8_t);
extern	void	CG_File_Load(void);
extern	void	G_INIT(void);
extern	void	G_HOME(void);
extern	void	G_VIEW(uint8_t);
extern	void	G_Palette_INIT(void);
extern	void	G_Palette(void);
extern	int16_t	G_Stretch_Pict( int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t, int16_t, uint16_t, uint8_t );
extern	int16_t	G_BitBlt(int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t , int16_t , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
extern	int32_t	G_CLR(void);
extern	int16_t	G_CLR_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t);
extern	int16_t	G_CLR_ALL_OFFSC(uint8_t);
extern	int16_t	G_FILL_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t, uint8_t);
extern	int16_t	G_Load(uint8_t, uint16_t, uint16_t, uint16_t);
extern	int16_t	G_Load_Mem(uint8_t, uint16_t, uint16_t, uint16_t);
extern	int16_t	APICG_DataLoad2G(int8_t *, uint64_t, uint64_t, uint16_t);
extern	int16_t	APICG_DataLoad2M(uint8_t, uint64_t, uint64_t, uint16_t, uint16_t *);
extern	int16_t	G_Subtractive_Color(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t, uint32_t);
extern	int16_t	PutGraphic_To_Text(uint8_t , uint16_t , uint16_t );
extern	int16_t	PutGraphic_To_Symbol(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t	G_Scroll(int16_t, int16_t, uint8_t);

#endif	/* GRAPHIC_H */
