#ifndef	MFP_H
#define	MFP_H

#include <string.h>
#include "inc/usr_style.h"

extern UI Init_MFP(void);
extern void interrupt Timer_D_Func(void);
extern void interrupt Hsync_Func(void);
extern void interrupt Raster_Func(void);
extern void interrupt Vsync_Func(void);
extern SI vwait(SI);
extern SI GetNowTime(US *);	/* 現在の時間を取得する */
extern SI SetNowTime(US);		/* 現在の時間を設定する */
extern SI SetRasterVal(void *, size_t);	/* ラスター専用のバッファにコピーする */
extern SI SetRasterPal(void *, size_t);	/* ラスター専用(PAL)のバッファにコピーする */


#endif	/* MFP_H */
