#ifndef	MFP_C
#define	MFP_C

#include <iocslib.h>
#include <interrupt.h>

#include "inc/usr_macro.h"

#include "OverKata.h"
#include "MFP.h"

/* 変数 */
static volatile UI NowTime;
static volatile US ras_count;
static volatile US Hsync_count;
static volatile US Vsync_count;

static US ras_val[256];
static US ras_pal[256];

/* 関数のプロトタイプ宣言 */
SS Init_MFP(void);
void interrupt Timer_D_Func(void);
SS GetNowTime(UI *);	/* 現在の時間を取得する */
SS SetNowTime(UI);		/* 現在の時間を設定する */
void interrupt Hsync_Func(void);
void interrupt Raster_Func(void);

SS SetRasterVal(void *, size_t);		/* ラスター専用のバッファにコピーする */
SS SetRasterPal(void *, size_t);		/* ラスター専用(PAL)のバッファにコピーする */
void interrupt Vsync_Func(void);
SS vwait(SS);

/* 関数 */
SS Init_MFP(void)	/* 現在の時間を取得する */
{
	NowTime = 0;
	ras_pal[0] = 0;
	ras_count = RASTER_MAX;
	Hsync_count = 0;
	Vsync_count = 0;
	
	return 0;
}

void interrupt Timer_D_Func(void)
{
	NowTime++;

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

SS GetNowTime(UI *time)	/* 現在の時間を取得する */
{
	*time = NowTime;
	return 0;
}

SS SetNowTime(UI time)	/* 現在の時間を設定する */
{
	NowTime = time;
	return 0;
}

/* 割り込み関数は修飾子にinterruptを入れること */	/* くにちこ（Kunihiko Ohnaka）さんのアドバイス */
void interrupt Hsync_Func(void)
{
	Hsync_count++;
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

/* ラスター割り込みの処理内は必要最低限だけ */	/* EXCEED.さんのアドバイス */
/* ２ライン飛ばしぐらいが精度の限界 */	/* EXCEED.さんのアドバイス */
void interrupt Raster_Func(void)
{
	volatile US *BG0scroll_x  = (US *)0xEB0800;
	volatile US *BG0scroll_y  = (US *)0xEB0802;
	volatile US *BG1scroll_x  = (US *)0xEB0804;
	volatile US *BG1scroll_y  = (US *)0xEB0806;
	volatile US *CRTC_R09 = (US *)0xE80012u;	/* ラスター割り込み位置 */
	volatile US *CRTC_R12 = (US *)0xE80018u;	/* スクリーン0 X */
	volatile US *CRTC_R14 = (US *)0xE8001Cu;	/* スクリーン1 X */

	US nNum = ras_count;
	
	ras_count += RASTER_NEXT;		/* 次のラスタ割り込み位置の計算 */
	*CRTC_R09 = ras_count;			/* 次のラスタ割り込み位置の設定 */

	*BG0scroll_x	= ras_val[nNum];	/* BG0のX座標の設定 */
	*BG0scroll_y	= ras_pal[nNum];	/* BG0のY座標の設定 */
	*BG1scroll_x	= 256;				/* BG1のX座標の設定 */
	*BG1scroll_y	= ras_pal[1];		/* BG1のY座標の設定 */
//	*CRTC_R12		= ras_val[nNum] + X_OFFSET;			/* GRのX座標の設定 */
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

SS SetRasterVal(void *pSrc, size_t n)
{
	SS ret = 0;
	if(memcpy(ras_val, pSrc, n) == NULL)
	{
		ret = -1;
	}
	return ret;
}

SS SetRasterPal(void *pSrc, size_t n)
{
	SS ret = 0;
	if(memcpy(ras_pal, pSrc, n) == NULL)
	{
		ret = -1;
	}
	return ret;
}

void interrupt Vsync_Func(void)
{

	//	VDISPST((void *)0, 0, 0);	/* stop */
#if 0
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
#endif
	
	ras_count = ras_pal[0];
	CRTCRAS( Raster_Func, ras_count );	/* ラスター割り込み */
	Hsync_count = 0;
//	HSYNCST(Hsync_Func);				/* H-Sync割り込み */
	Vsync_count++;
//	VDISPST(Vsync_Func, 0, 1);			/* V-Sync割り込み 帰線 */

#if 0
	{
		volatile US *CRTC_R21 = (US *)0xE8002Au;	/* テキスト・アクセス・セット、クリアーP.S */
		volatile US *CRTC_480 = (US *)0xE80480u;	/* CRTC動作ポート */

		if((*CRTC_480 & 0x02u) == 0u)		/* クリア実行でない */
		{
			*CRTC_R21 = Mbset(*CRTC_R21, 0x0Fu, 0x0Cu);	/* SCREEN1 高速クリアON / SCREEN0 高速クリアOFF */
			*CRTC_480 = Mbset(*CRTC_480, 0x02u, 0x02u);	/* クリア実行 */
		}
	}
#endif

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

SS vwait(SS count)				/* 約count／60秒待つ	*/
{
	volatile UC *mfp = (UC *)0xe88001;
	
	while(count--){
		while(!((*mfp) & 0b00010000));	/* 垂直表示期間待ち	*/
		while((*mfp) & 0b00010000);	/* 垂直帰線期間待ち	*/
	}
}

#endif	/* MFP_H */
