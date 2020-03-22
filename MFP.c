#ifndef	MFP_C
#define	MFP_C

#include <iocslib.h>
#include <interrupt.h>

#include "inc/usr_define.h"
#include "inc/usr_macro.h"

#include "OverKata.h"
#include "MFP.h"

/* 変数 */
static volatile US NowTime;
static volatile US ras_count;
static volatile US Hsync_count;
static volatile US Vsync_count;

static US ras_val[1024];
static US ras_pal[1024];

/* 関数のプロトタイプ宣言 */
UI Init_MFP(void);
void interrupt Timer_D_Func(void);
SI GetNowTime(US *);	/* 現在の時間を取得する */
SI SetNowTime(US);		/* 現在の時間を設定する */
void interrupt Hsync_Func(void);
void interrupt Raster_Func(void);

SI SetRasterVal(void *, size_t);		/* ラスター専用のバッファにコピーする */
SI SetRasterPal(void *, size_t);		/* ラスター専用(PAL)のバッファにコピーする */
void interrupt Vsync_Func(void);
SI vwait(SI);

/* 関数 */
UI Init_MFP(void)	/* 現在の時間を取得する */
{
	NowTime = 0;
	ras_pal[0] = 0;
	ras_count = RASTER_ST;
	Hsync_count = 0;
	Vsync_count = 0;
	
	return 0;
}

void interrupt Timer_D_Func(void)
{
	NowTime++;

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

SI GetNowTime(US *time)	/* 現在の時間を取得する */
{
	*time = NowTime;
	return 0;
}

SI SetNowTime(US time)	/* 現在の時間を設定する */
{
	NowTime = time;
	return 0;
}


void interrupt Hsync_Func(void)
{
	Hsync_count++;
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

void interrupt Raster_Func(void)
{
	volatile US *scroll_x     = (US *)0xE80018;
	volatile US *BG0scroll_x  = (US *)0xEB0800;
	volatile US *BG0scroll_y  = (US *)0xEB0802;
	volatile US *BG1scroll_x  = (US *)0xEB0804;
	volatile US *BG1scroll_y  = (US *)0xEB0806;
	volatile US *raster_addr  = (US *)0xE80012;

	US nNum = ras_count;
	
	ras_count += RASTER_NEXT;		/* 次のラスタ割り込み位置の計算 */
	*raster_addr = ras_count;		/* 次のラスタ割り込み位置の設定 */

	*BG0scroll_x	= ras_val[nNum];	/* BG0のX座標の設定 */
	*BG0scroll_y	= ras_pal[nNum];	/* BG0のY座標の設定 */
	*BG1scroll_x	= 256;				/* BG1のX座標の設定 */
	*BG1scroll_y	= 0;				/* BG1のY座標の設定 */
	*scroll_x		= ras_pal[nNum];	/* GRのX座標の設定 */
	
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

SI SetRasterVal(void *pSrc, size_t n)
{
	SI ret = 0;
	if(memcpy(ras_val, pSrc, n) == NULL)
	{
		ret = -1;
	}
	return ret;
}

SI SetRasterPal(void *pSrc, size_t n)
{
	SI ret = 0;
	if(memcpy(ras_pal, pSrc, n) == NULL)
	{
		ret = -1;
	}
	return ret;
}

void interrupt Vsync_Func(void)
{
	volatile US *BG0scroll_x  = (US *)0xEB0800;
	volatile US *BG0scroll_y  = (US *)0xEB0802;
	volatile US *BG1scroll_x  = (US *)0xEB0804;
	volatile US *BG1scroll_y  = (US *)0xEB0806;

	//	VDISPST((void *)0, 0, 0);	/* stop */
	
	if((Vsync_count % 10) < 5)
	{
		GPALET( 1, SetRGB(16, 16, 16));	/* Glay */
		GPALET( 2, SetRGB(15, 15, 15));	/* D-Glay */
		
		GPALET( 5, SetRGB(31,  0,  0));	/* Red */
		GPALET(15, SetRGB(31, 31, 31));	/* White */
		
		GPALET( 8, SetRGB( 0, 31,  0));	/* Green */
		GPALET(11, SetRGB( 0, 28,  0));	/* Green */
	}
	else
	{
		GPALET( 2, SetRGB(16, 16, 16));	/* Glay */
		GPALET( 1, SetRGB(15, 15, 15));	/* D-Glay */
		
		GPALET(15, SetRGB(31,  0,  0));	/* Red */
		GPALET( 5, SetRGB(31, 31, 31));	/* White */
		
		GPALET(11, SetRGB( 0, 31,  0));	/* Green */
		GPALET( 8, SetRGB( 0, 28,  0));	/* Green */
	}

	ras_count = RASTER_ST;
	CRTCRAS(Raster_Func, ras_count );	/* ラスター割り込み */
	Hsync_count = 0;
//	HSYNCST(Hsync_Func);				/* H-Sync割り込み */
	Vsync_count++;
//	VDISPST(Vsync_Func, 0, 1);			/* V-Sync割り込み 帰線 */
	
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

SI vwait(SI count)				/* 約count／60秒待つ	*/
{
	volatile UC *mfp = (UC *)0xe88001;
	
	while(count--){
		while(!((*mfp) & 0b00010000));	/* 垂直表示期間待ち	*/
		while((*mfp) & 0b00010000);	/* 垂直帰線期間待ち	*/
	}
}

#endif	/* MFP_H */
