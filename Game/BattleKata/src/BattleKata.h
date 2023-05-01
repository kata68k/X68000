#ifndef	BATTLEKATA_H
#define	BATTLEKATA_H

#include <usr_define.h>

/* ç\ë¢ëÃíËã` */
/* ??????????? */

/* ????? */
#define TP_ALIVE	1
#define TP_DEAD		-1
#define ENEMY_STDBY	0
#define ENEMY_ALIVE	1
#define ENEMY_BOM	2
#define ENEMY_DEAD	-1

/* ???? */
#define VELO_X	2
#define VELO_Y	2
#define VELO_SX 5
#define VELO_SY 5

#define SHOT_ST_OUT 0
#define SHOT_ST_MOVE 1
#define SHOT_ST_DOWN 2
#define SHOT_ST_TOUCH 3
#define SHOT_ST_RUN 4
#define SHOT_VX 2
#define SHOT_VY 2

/* ????? */
#define ATARI_YOKO	(0x800)
#define ATARI_TATE	(0x800)
#define ATARI_X 	(0x800)
#define ATARI_Y 	(0x800)
#define ATARI_XY	(0x400000)

/* ??????? */
#define SHOT_NUM 1

/* ??? */
#define ENEMY_NUM 18

/* ???? */
#define PCMNUM	4
#define SOUNDMOD 4*256+3

#define	GP_VDISP	(0x10)

#define SetXSPinfo(V,H,PAL,PRI)	(0xCFFFU & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) | (PRI & 0xFFU)))
#define SetBGcode(V,H,PAL,PCG)	(0xCFFFU & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) | (PCG & 0xFFU)))
#define SetBGcode2(V,H,PAL)		(0xCF00U & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) ))

/* ???????? */
typedef struct {
				int16_t flag;
				int32_t x, y;
				int16_t vx, vy;
				int16_t acc;
				int16_t angle;
				int16_t life;
} SHIP;

typedef struct {
				int16_t flag;
				int32_t x, y;
				int16_t w, h;
} AREA;

typedef struct {
				int16_t flag;
				int32_t x, y;
				int32_t ex, ey;
				int16_t vx, vy;
				int32_t acc;
				int16_t pat;
} SHOT;

typedef struct {
				int16_t flag;
				int32_t x, y;
				int16_t vx, vy;
				int16_t delay;
} ENEMY;

/* externêÈåæ */
extern void App_TimerProc(void);

#endif	/* BATTLEKATA_H */
