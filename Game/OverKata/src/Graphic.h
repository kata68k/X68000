#ifndef	GRAPHIC_H
#define	GRAPHIC_H

#include "inc/usr_define.h"

enum
{
	POS_TOP=0,
	POS_CENTER,
	POS_BOTTOM,
	POS_LEFT,
	POS_MID,
	POS_RIGHT,
};

/* 画面内登場数 */
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
	Enemy1_G,
	Enemy2_G,
	StartPoint_G,
	GoalPoint_G,
	MyCar_G,
	Debug_View_G,
	Flip_G,
	MAX_G
};

/* グラフィックパターン定義数 0-15 max16 */
#define DUMMY_CG		(0)		/* ダミー		(1) */
#define BG_CG			(1)		/* 背景			(1) */
#define COURSE_OBJ_CG	(2)		/* 障害物		(kind of 4) */
#define ENEMYCAR_CG		(6)		/* ライバル車	(kind of 8) */
#define START_PT_CG		(14)	/* スタート位置	(1) */
#define GOAL_PT_CG		(15)	/* ゴール位置	(1) */
#define PAL_MAX_CG		(16)	/* 最大16パターン登録 */
/* グラフィックテキストパターン定義数 max 3 */
#define MYCAR_CG		(16)	/* 自車			(kind of 3) */
/* フルグラフィック */
#define TITLE_CG		(19)	/* タイトル */
#define DEMO_CG			(20)	/* デモ */
	/* … */
#define	CG_MAX			(27)	/* グラフィックパレット数に依存 */

#define	TRANS_PAL		(0x00)

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
extern	int16_t CG_File_Load(uint16_t);
extern	void	G_INIT(void);
extern	void	G_VIDEO_INIT(void);
extern	void	G_HOME(uint8_t);
extern	void	G_VIEW(uint8_t);
extern	void	G_Palette_INIT(void);
extern	void	G_PaletteSetZero(void);
extern	int16_t	G_Stretch_Pict( int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t, int16_t, uint16_t, uint8_t );
extern	int16_t G_Stretch_Pict_toVRAM(	int16_t, int16_t, uint8_t, uint8_t, uint8_t , uint8_t , uint16_t *, uint16_t, uint16_t, int8_t, uint8_t);
extern	int16_t G_Stretch_Pict_To_Mem( uint16_t	*, uint16_t, uint16_t, uint16_t	*, uint16_t, uint16_t);
extern	int16_t G_Copy_Pict_To_Mem(	uint16_t *, uint16_t , uint16_t , uint16_t *, uint16_t , uint16_t);
extern	int16_t	G_BitBlt(int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t , int16_t , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
extern	int16_t G_BitBlt_From_Mem(	int16_t, int16_t , uint8_t , uint16_t *, uint16_t , uint16_t , uint8_t , uint8_t , uint8_t, uint8_t );
extern	int32_t	G_CLR(void);
extern	int16_t G_CLR_HS(void);
extern	int16_t	G_CLR_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t);
extern	int16_t	G_CLR_ALL_OFFSC(void);
extern	int16_t	G_FILL_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t, uint8_t);
extern	int16_t	G_Load(uint8_t, uint16_t, uint16_t, uint16_t);
extern	int16_t	G_Load_Mem(uint8_t, int16_t, int16_t, uint16_t);
extern	int16_t	APICG_DataLoad2G(int8_t *, uint64_t, uint64_t, uint16_t);
extern	int16_t	APICG_DataLoad2M(uint8_t, uint64_t, uint64_t, uint16_t, uint16_t *);
extern	int16_t	G_Subtractive_Color(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t, uint32_t);
extern	int16_t	PutGraphic_To_Text(uint8_t , uint16_t , uint16_t );
extern	int16_t	PutGraphic_To_Symbol(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t	G_Scroll(int16_t, int16_t, uint8_t);

#endif	/* GRAPHIC_H */
