#ifndef	RASTER_H
#define	RASTER_H

#include "inc/usr_define.h"

#define	ROADDATA_MAX	(1024u)

/* 構造体定義 */
/* ラスター情報の構造体 */
typedef struct
{
	US	st;			/* 開始位置 */
	US	mid;		/* 中間位置 */
	US	ed;			/* 終了位置 */
	US	size;		/* 開始と終了の差分 */
	US	horizon;	/* 水平線 */
}	ST_RAS_INFO;

/* ラスター割り込み情報の構造体 */
typedef struct
{
	US	x;
	US	y;
	US	pat;
}	ST_RASTER_INT;

/* ロード情報の構造体 */
typedef struct
{
	SS	Horizon;		/* 水平位置 */
	SS	Horizon_tmp;	/* 仮水平位置 */
	SS	Horizon_offset;	/* 水平位置のオフセット */
	
	SS	offset_x;		/*  */
	SS	offset_y;		/*  */
	SS	offset_val;		/*  */
	SS	height;			/*  */
	SS	slope;			/*  */
	SS	angle;			/*  */
	SS	distance;		/*  */
	SS	object;			/*  */
	
}	ST_ROAD_INFO;

/* extern宣言 */
extern void	Raster_Init(void);
extern void Raster_Main(UC);
extern SS GetRasterInfo(ST_RAS_INFO *);
extern SS SetRasterInfo(ST_RAS_INFO);
extern SS GetRasterPos(US *, US *, US);

extern void Road_Init(US);
extern UL GetRoadDataAddr(void);
extern SS GetRoadCycleCount(US *);

#endif	/* RASTER_H */
