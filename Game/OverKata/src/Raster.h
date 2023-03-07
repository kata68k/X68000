#ifndef	RASTER_H
#define	RASTER_H

#include <usr_define.h>

#define	ROADDATA_MAX	(2048u)

/* 構造体定義 */
/* ラスター情報の構造体 */
typedef struct
{
	uint16_t	st;			/* 開始位置 */
	uint16_t	mid_H;		/* 中間位置(H) */
	uint16_t	mid;		/* 中間位置(L) */
	uint16_t	ed;			/* 終了位置 */
	uint16_t	size;		/* 開始と終了の差分 */
}	ST_RAS_INFO;

/* ラスター割り込み情報の構造体 */
typedef struct
{
	uint16_t	x;
	uint16_t	y;
	uint16_t	pat;
}	ST_RASTER_INT;

/* ロード情報の構造体 */
typedef struct
{
	int16_t	Horizon;		/* 水平位置 */
	int16_t	Horizon_Base;	/* 水平基準位置 */
	int16_t	offset_x;		/*  */
	int16_t	offset_y;		/*  */
	int16_t	offset_val;		/*  */
	int16_t	height;			/*  */
	int16_t	slope;			/*  */
	int16_t	angle;			/*  */
	int16_t	distance;		/*  */
	int16_t	object;			/*  */
	int16_t	Courselength;	/* コースの全長 */
}	ST_ROAD_INFO;

/* コースの構造体 */
typedef struct
{
	uint8_t	bHeight;	/* 道の標高	(0x80センター) */
	uint8_t	bWidth;		/* 道の幅	(0x80センター) */
	uint8_t	bAngle;		/* 道の角度	(0x80センター) */
	uint8_t	bfriction;	/* 道の摩擦	(0x80センター) */
	uint8_t	bPat;		/* 道の種類	 */
	uint8_t	bObject;	/* 出現ポイントのオブジェクトの種類 */
	uint8_t	bRepeatCount;	/* 繰り返し回数 */
}	ST_ROADDATA;


/* extern宣言 */
extern	ST_RASTER_INT	g_stRasterInt[RASTER_H_MAX];
extern	ST_ROAD_INFO	g_stRoadInfo;
extern	ST_ROADDATA		g_stRoadData[ROADDATA_MAX];

extern void	Raster_Init(void);
extern int16_t Raster_Main(void);
extern int16_t GetRasterInfo(ST_RAS_INFO *);
extern int16_t SetRasterInfo(ST_RAS_INFO);
extern int16_t GetRasterPos(uint16_t *, uint16_t *, uint16_t);

extern int16_t	GetRoadInfo(ST_ROAD_INFO *);
extern int16_t	SetRoadInfo(ST_ROAD_INFO);
extern void Road_Init(uint16_t);
extern void Road_BG_Init(uint16_t);
extern uint64_t GetRoadDataAddr(uint16_t);
extern int16_t	Road_Pat_Main(uint16_t *);
extern int16_t GetRoadCycleCount(uint16_t *);
extern int16_t Road_Map_Draw(uint8_t);

#endif	/* RASTER_H */
