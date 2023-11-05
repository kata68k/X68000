#ifndef	APL_RASTER_H
#define	APL_RASTER_H

#include <usr_define.h>

#define	ROADDATA_MAX	(2048u)

/* ?\???????` */
/* ???X?^?[???????\???? */
typedef struct
{
	uint16_t	st;			/* ?J?n???u */
	uint16_t	mid_H;		/* ???????u(H) */
	uint16_t	mid;		/* ???????u(L) */
	uint16_t	ed;			/* ?I?????u */
	uint16_t	size;		/* ?J?n???I???????? */
}	ST_RAS_INFO;

/* ???X?^?[???????????????\???? */
typedef struct
{
	uint16_t	x;
	uint16_t	y;
	uint16_t	pat;
}	ST_RASTER_INT;

/* ???[?h???????\???? */
typedef struct
{
	int16_t	Horizon;		/* ???????u */
	int16_t	Horizon_Base;	/* ???????€???u */
	int16_t	offset_x;		/*  */
	int16_t	offset_y;		/*  */
	int16_t	offset_val;		/*  */
	int16_t	height;			/*  */
	int16_t	slope;			/*  */
	int16_t	angle;			/*  */
	int16_t	distance;		/*  */
	int16_t	object;			/*  */
	int16_t	Courselength;	/* ?R?[?X???S?? */
}	ST_ROAD_INFO;

/* ?R?[?X???\???? */
typedef struct
{
	uint8_t	bHeight;	/* ?????W??	(0x80?Z???^?[) */
	uint8_t	bWidth;		/* ??????	(0x80?Z???^?[) */
	uint8_t	bAngle;		/* ?????p?x	(0x80?Z???^?[) */
	uint8_t	bfriction;	/* ?????€?C	(0x80?Z???^?[) */
	uint8_t	bPat;		/* ????????	 */
	uint8_t	bObject;	/* ?o???|?C???g???I?u?W?F?N?g?????? */
	uint8_t	bRepeatCount;	/* ?J?????????? */
}	ST_ROADDATA;


/* extern???? */
extern uint16_t g_uRasterLine[8];
extern volatile uint16_t Raster_count;
extern volatile uint16_t RasterLine_count;
extern volatile uint16_t	g_uRasterSkipStatus;


//extern	ST_RASTER_INT	g_stRasterInt[RASTER_H_MAX];
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
extern int16_t SetRasterIntData(uint16_t);
extern int16_t GetRasterIntPos(uint16_t *, uint16_t *, uint16_t *, uint16_t, uint8_t);
extern int16_t RasterVcyncProc(void);
extern int16_t RasterProc(uint16_t *);

#endif	/* APL_RASTER_H */
