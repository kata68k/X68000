#ifndef	RASTER_H
#define	RASTER_H

#include "inc/usr_define.h"

/* 構造体定義 */

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
extern void Raster_Main(UC);
extern void Road_Pat(void);
extern SS GetRasterInfo(ST_RAS_INFO *);
extern SS SetRasterInfo(ST_RAS_INFO);
extern UL GetRoadDataAddr(void);

#endif	/* RASTER_H */
