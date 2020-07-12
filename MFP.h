#ifndef	MFP_H
#define	MFP_H

#include <string.h>
#include "inc/usr_define.h"

enum{
	DST_none=0,
	DST_1us,
	DST_2p5us,
	DST_4us,
	DST_12p5us,
	DST_16us,
	DST_25us,
	DST_50us,
};

extern SS Init_MFP(void);
extern void interrupt Timer_D_Func(void);
extern void interrupt Hsync_Func(void);
extern void interrupt Raster_Func(void);
extern void interrupt Vsync_Func(void);
extern SS vwait(SS);
extern SS GetNowTime(UI *);		/* 現在の時間を取得する */
extern SS SetNowTime(UI);		/* 現在の時間を設定する */
extern SS GetStartTime(UI *);	/* 開始の時間を取得する */
extern SS SetStartTime(UI);		/* 開始の時間を設定する */
extern SS GetRasterPos(US *, US *, US);
extern SS GetRasterInfo(ST_RAS_INFO *);
extern SS SetRasterInfo(ST_RAS_INFO);

extern SS SetRasterVal(void *, size_t);	/* ラスター専用のバッファにコピーする */
extern SS SetRasterPal(void *, size_t);	/* ラスター専用(PAL)のバッファにコピーする */


#endif	/* MFP_H */
