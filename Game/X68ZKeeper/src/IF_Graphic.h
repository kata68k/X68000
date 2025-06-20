#ifndef	IF_GRAPHIC_H
#define	IF_GRAPHIC_H

#include <usr_define.h>

enum
{
	POS_TOP=0,
	POS_CENTER,
	POS_BOTTOM,
	POS_LEFT,
	POS_MID,
	POS_RIGHT,
};

#define G_COL_OFST	(0x00)
#define G_BG		(0x00)
#define G_BLACK		(0x01 + G_COL_OFST)
#define G_D_GRAY	(0x02 + G_COL_OFST)
#define G_GRAY		(0x03 + G_COL_OFST)
#define G_D_WHITE	(0x04 + G_COL_OFST)
#define G_WHITE		(0x05 + G_COL_OFST)
#define G_D_GREEN	(0x06 + G_COL_OFST)
#define G_D_RED		(0x07 + G_COL_OFST)
#define G_D_BLUE	(0x08 + G_COL_OFST)
#define G_INDIGO	(0x09 + G_COL_OFST)
#define G_RED		(0x0A + G_COL_OFST)
#define G_GREEN		(0x0B + G_COL_OFST)
#define G_YELLOW	(0x0C + G_COL_OFST)
#define G_BLUE		(0x0D + G_COL_OFST)
#define G_L_PINK	(0x0E + G_COL_OFST)
#define G_L_BLUE	(0x0F + G_COL_OFST)

#define F_MOJI		(G_WHITE)
#define F_MOJI_BAK	(G_D_GRAY)

#define	TRANS_PAL		(0x00)
#define	CG_MAX			(27)		/* グラフィックパレット数に依存 */

#define FILE_TYPE			(0x424D)	/* "BM"値 */
#define FILE_HEADER_SIZE	(14)		/* BMPファイルヘッダサイズ */
#define INFO_HEADER_SIZE	(40)		/* Windowsヘッダサイズ */
#define DEFAULT_HEADER_SIZE	(FILE_HEADER_SIZE + INFO_HEADER_SIZE)	/* 標準のヘッダサイズ */

//ビットマップバイト幅の算出マクロ
#ifndef WIDTHBYTES
#define WIDTHBYTES(bits)    (((bits)+31)/32*4)
#endif//WIDTHBYTES

//YUV平均値を求める際の作業テーブル
typedef struct tagAVGYUV{
	double	y;
	double	u;
	double	v;
	int32_t cnt;
}AVGYUV,*PAVGYUV;

//ＲＧＢ変換で使用
typedef struct tagYUV{
	float	y;
	float	u;
	float	v;
	int32_t no;
} YUV,*PYUV;

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
	int64_t		biWidth;		/* 画像の幅(px) */
	int64_t		biHeight;		/* 画像の高さ(px) */
	uint16_t	biPlanes;		/* プレーン数 常に１ */
	uint16_t	biBitCount;		/* １画素あたりのデータサイズ */
	uint64_t	biCompression;	/* 圧縮形式 */
	uint64_t	biSizeImage;	/* 画像データ部のサイズ */
	int64_t		biXPixPerMeter;	/* 横方向解像度(dot/m) */
	int64_t		biYPixPerMeter;	/* 縦方向解像度(dot/m) */
	uint64_t	biClrUsed;		/* 格納されているパレット数(使用色数) */
	uint64_t	biClrImportant;	/* 重要なパレットのインデックス */
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	uint8_t	rgbBlue;		/* 青０〜２５５ */
	uint8_t	rgbGreen;		/* 緑０〜２５５ */
	uint8_t	rgbRed;			/* 赤０〜２５５ */
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
	int8_t	bType;				/* 0:通常、1:スプライトライク */
	int16_t	TransPal;			/* 透過色パレット番号 */
} CG_LIST;

/* 外部参照 変数 */
extern	volatile uint16_t *VIDEO_REG1;
extern	volatile uint16_t *VIDEO_REG2;
extern	volatile uint16_t *VIDEO_REG3;

extern	uint32_t	g_CG_List_Max;
extern	CG_LIST		g_stCG_LIST[CG_MAX];

/* 外部参照 関数 */
extern	int16_t	Get_CG_FileList_MaxNum(uint32_t *);
extern	uint8_t	*Get_CG_FileBuf(uint8_t);
extern	uint16_t	*Get_PicImageInfo(uint8_t , uint32_t *, uint32_t *, uint32_t *);
extern	int16_t	Get_PicImagePallet(uint8_t);
extern	int16_t	Set_PicImagePallet(uint8_t);
extern	int16_t Set_PicImagePalletALL(void);
extern	void Set_Pallet_Shift(int16_t, int16_t);
extern	int16_t CG_File_Load(uint16_t);
extern	void	G_INIT(void);
extern	void	G_VIDEO_INIT(void);
extern	void	G_VIDEO_PRI(uint8_t);
extern	void	G_HOME(uint8_t);
extern	void	G_VIEW(uint8_t);
extern	void	G_Palette_INIT(void);
extern	void	G_PaletteSetZero(void);
extern	void	G_PaletteSet(int16_t, int16_t);
extern	void	G_Palette_HALF(void);
extern	int16_t	G_Stretch_Pict( int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t, int16_t, uint16_t, uint8_t );
extern	int16_t G_Stretch_Pict_toVRAM(	int16_t, int16_t, uint8_t, uint8_t, uint8_t , uint8_t , uint16_t *, uint16_t, uint16_t, int8_t, uint8_t);
extern	int16_t G_Stretch_Pict_To_Mem( uint16_t	*, uint16_t, uint16_t, uint16_t	*, uint16_t, uint16_t);
extern	int16_t G_Copy_Pict_To_Mem(	uint16_t *, uint16_t , uint16_t , uint16_t *, uint16_t , uint16_t);
extern	int16_t	G_BitBlt(int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t , int16_t , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
extern	int16_t G_BitBlt_From_Mem(	int16_t, int16_t , uint8_t , uint16_t *, uint16_t , uint16_t , uint8_t , uint8_t , uint8_t, uint8_t );
extern	int32_t G_PAGE_SET(uint16_t);
extern	int32_t	G_CLR(void);
extern	int16_t G_CLR_HS(void);
extern	int16_t	G_CLR_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t);
extern	int16_t	G_CLR_ALL_OFFSC(uint8_t);
extern	int16_t	G_FILL_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t, uint8_t);
extern	int16_t	G_Load(uint8_t, uint16_t, uint16_t, uint16_t);
extern	int16_t	G_Load_Mem(uint8_t, int16_t, int16_t, uint16_t);
extern	int16_t	APICG_DataLoad2G(int8_t *, uint64_t, uint64_t, uint16_t);
extern	int16_t	APICG_DataLoad2M(uint8_t, uint64_t, uint64_t, uint16_t, uint16_t *);
extern	int16_t	G_Subtractive_Color(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t, uint32_t);
extern	int16_t	PutGraphic_To_Text(uint8_t , uint16_t , uint16_t, uint16_t);
extern	int16_t	PutGraphic_To_Symbol12(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t	PutGraphic_To_Symbol16(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t	PutGraphic_To_Symbol24(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t PutGraphic_To_Symbol24_Xn(uint8_t *, uint16_t, uint16_t, uint16_t, uint16_t);
extern	int16_t	G_Scroll(int16_t, int16_t, uint8_t);

#endif	/* IF_GRAPHIC_H */
