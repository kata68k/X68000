#ifndef	MFP_C
#define	MFP_C

#include <iocslib.h>
#include <interrupt.h>

#include "inc/usr_macro.h"

#include "OverKata.h"
#include "MFP.h"
#include "Moon.h"
#include "OutPut_Text.h"
#include "Raster.h"

/* 変数 */
static UC g_bTimer_D;
static UC g_bRasterSET;
static volatile UI NowTime;
static volatile UI StartTime;
static volatile US ras_count;
static volatile US Hsync_count;
static volatile US Vsync_count;

static ST_RASTER_INT g_stRasterInt[RASTER_MAX] = {0};

/* 関数のプロトタイプ宣言 */
SS MFP_INIT(void);
SS MFP_EXIT(void);
SS MFP_RESET(void);
SS TimerD_INIT(void);
SS TimerD_EXIT(void);
UC GetNowTime(UI *);	/* 現在の時間を取得する */
UC SetNowTime(UI);		/* 現在の時間を設定する */
UC GetStartTime(UI *);	/* 開始の時間を取得する */
UC SetStartTime(UI);	/* 開始の時間を設定する */
SS SetRasterIntData(void *, size_t);
SS GetRasterIntPos(US *, US *, US *, US);
static void interrupt Timer_D_Func(void);
static void interrupt Hsync_Func(void);
static void interrupt Raster_Func(void);
static void interrupt Vsync_Func(void);
SS vwait(SS);

/* 関数 */
SS MFP_INIT(void)
{
	SI	vdispst = -1;
	volatile UC *MFP_TACR = (UC *)0xe88019;
	volatile UC *MFP_TADR = (UC *)0xe8801f;

	/* ラスタ割り込み */
	g_bRasterSET = FALSE;
	ras_count = 0;
	memset(g_stRasterInt, 0, sizeof(ST_RASTER_INT) * RASTER_MAX);
	/* H-Sync割り込み */
	Hsync_count = 0;
	/* V-Sync割り込み */
	Vsync_count = 0;

	//VDISPST 初回だけバグあり（EXCEED.さん）
	*MFP_TACR = 0x00;	/* タイマーＡを止める */
	*MFP_TADR = 0x01;	/* カウンタを設定(0=256回) */
	*MFP_TACR = 0x08;	/* イベントカウントモードに設定する */
#ifdef 	MACS_MOON
	vdispst = MACS_Vsync(Vsync_Func);		/* V-Sync割り込み */
#else	/* MACS_MOON */
	vdispst = VDISPST(Vsync_Func, 0, 1);	/* V-Sync割り込み 帰線 */
#endif	/* MACS_MOON */
//	Message_Num(&vdispst,  0, 10, 2, MONI_Type_SI, "%4d");
	
	return vdispst;
}

SS MFP_EXIT(void)
{
	SI	vdispst = -1;

	g_bRasterSET = FALSE;

	CRTCRAS((void *)0, 0);		/* stop */
	HSYNCST((void *)0);			/* stop */
#ifdef 	MACS_MOON
	vdispst = MACS_Vsync_R(Vsync_Func);	/* stop */
#else	/* MACS_MOON */
	vdispst = VDISPST((void *)0, 0, 0);	/* stop */
#endif	/* MACS_MOON */
//	Message_Num(&vdispst,  6, 10, 2, MONI_Type_SI, "%4d");

	return vdispst;
}

SS MFP_RESET(void)
{
	SS	ret = 0;

	volatile UC *MFP_INTCTRL_A  = (UC *)0xE88007;	/* 割り込みイネーブルレジスタA */
	volatile UC *MFP_TADR  		= (UC *)0xE8801F;	/* タイマーAデータレジスタ */
	
	*MFP_TADR = 1;					/* カウンタ設定 */
	*MFP_INTCTRL_A |= 0b01000000;	/* CRTC Raster割り込み許可 */
//	*MFP_INTCTRL_A |= 0b00100000;	/* Timer-A割り込み許可 */
	
	return ret;
}

SS	TimerD_INIT()
{
	/* Timer-D割り込み */
	NowTime = 0;
	StartTime = 0;
	TIMERDST(Timer_D_Func, 7, 20);	/* Timer-Dセット */	/* 50us(7) x 20cnt = 1ms */
	g_bTimer_D =TRUE;
	SetNowTime(NowTime);	/* 時間の初期化 */

	return 0;
}

SS	TimerD_EXIT()
{
	TIMERDST((void *)0, 0, 1);	/* stop */
	g_bTimer_D = FALSE;

	return 0;
}

UC GetNowTime(UI *time)	/* 現在の時間を取得する */
{
	*time = NowTime;
	return g_bTimer_D;
}

UC SetNowTime(UI time)	/* 現在の時間を設定する */
{
	NowTime = time;
	return g_bTimer_D;
}

UC GetStartTime(UI *time)	/* 開始の時間を取得する */
{
	*time = StartTime;
	return g_bTimer_D;
}

UC SetStartTime(UI time)	/* 開始の時間を設定する */
{
	StartTime = time;
	return g_bTimer_D;
}

SS SetRasterIntData(void *pSrc, size_t n)
{
	SS ret = 0;
	if(memcpy(g_stRasterInt, pSrc, n) == NULL)
	{
		ret = -1;
	}
	else
	{
		g_bRasterSET = TRUE;
	}
	return ret;
}

SS GetRasterIntPos(US *x, US *y, US *pat, US uNum)
{
	SS	ret = 0;

	if(uNum < 256)
	{
		if(x !=  NULL)
		{
			*x = g_stRasterInt[uNum].x;
		}
		else
		{
			/* nop */
		}
		
		if(y !=  NULL)
		{
			*y = g_stRasterInt[uNum].y;
		}
		else
		{
			/* nop */
		}

		if(pat !=  NULL)
		{
			*pat = g_stRasterInt[uNum].pat;
		}
		else
		{
			/* nop */
		}
	}
	
	return ret;
}

/* 割り込み関数は修飾子にinterruptを入れること */	/* くにちこ（Kunihiko Ohnaka）さんのアドバイス */

static void interrupt Timer_D_Func(void)
{
	NowTime++;

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

static void interrupt Hsync_Func(void)
{
	HSYNCST((void *)0);			/* stop */
	Hsync_count++;
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

/* ラスター割り込みの処理内は必要最低限だけ */	/* EXCEED.さんのアドバイス */
/* ２ライン飛ばしぐらいが精度の限界 */	/* EXCEED.さんのアドバイス */
static void interrupt Raster_Func(void)
{
	volatile US *BG0scroll_x  = (US *)0xEB0800;
	volatile US *BG0scroll_y  = (US *)0xEB0802;
//	volatile US *BG1scroll_x  = (US *)0xEB0804;
//	volatile US *BG1scroll_y  = (US *)0xEB0806;
	volatile US *CRTC_R09 = (US *)0xE80012u;	/* ラスター割り込み位置 */
//	volatile US *CRTC_R12 = (US *)0xE80018u;	/* スクリーン0 X */
//	volatile US *CRTC_R14 = (US *)0xE8001Cu;	/* スクリーン1 X */

	US nNum = ras_count;

#ifdef DEBUG	/* デバッグコーナー */
	UC	bDebugMode;
	US	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	ras_count += RASTER_NEXT;		/* 次のラスタ割り込み位置の計算 */
	*CRTC_R09 = ras_count;			/* 次のラスタ割り込み位置の設定 */

	*BG0scroll_x	= g_stRasterInt[nNum].x;	/* BG0のX座標の設定 */
	*BG0scroll_y	= g_stRasterInt[nNum].y + g_stRasterInt[nNum].pat;	/* BG0のY座標の設定 */
//	*BG0scroll_y	= uDebugNum + g_stRasterInt[nNum].pat;	/* BG0のY座標の設定 */
//	*BG1scroll_x	= g_stRasterInt[nNum].x;	/* BG1のX座標の設定 */
//	*BG1scroll_y	= g_stRasterInt[nNum].y + g_stRasterInt[nNum].pat;	/* BG1のY座標の設定 */
//	*BG1scroll_x	= 256;						/* BG1のX座標の設定 *//* 空のコントロール */
//	*BG1scroll_y	= g_stRasterInt[1].y;		/* BG1のY座標の設定 *//* 空のコントロール */
//	*CRTC_R12		= g_stRasterInt[nNum].x + X_OFFSET;			/* GRのX座標の設定 */

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

static void interrupt Vsync_Func(void)
{
	volatile US *BG0scroll_x  = (US *)0xEB0800;
	volatile US *BG0scroll_y  = (US *)0xEB0802;
	volatile US *BG1scroll_x  = (US *)0xEB0804;
	volatile US *BG1scroll_y  = (US *)0xEB0806;
//	volatile US *BGCTRL		  = (US *)0xEB0808;

	/* V-Sync割り込み処理 */
#ifdef 	MACS_MOON
	/* 何もしない */
#else	/* MACS_MOON */
//	SI	vdispst = -1;
	vdispst = VDISPST((void *)0, 0, 0);	/* stop */
#endif	/* MACS_MOON */

//	*BGCTRL = Mbclr(*BGCTRL, Bit_9);
	*BG0scroll_x	= 256;					/* BG0のX座標の設定 *//* 空のコントロール */
	*BG0scroll_y	= g_stRasterInt[1].y;	/* BG0のY座標の設定 *//* 空のコントロール */
	*BG1scroll_x	= 256;					/* BG1のX座標の設定 *//* 空のコントロール */
	*BG1scroll_y	= g_stRasterInt[1].y;	/* BG1のY座標の設定 *//* 空のコントロール */

	/* H-Sync割り込み処理 */
	Hsync_count = 0;
//	HSYNCST(Hsync_Func);					/* H-Sync割り込み */

	/* ラスタ割り込み */
	CRTCRAS((void *)0, 0);	/* stop */
	if(g_bRasterSET == TRUE)
	{
		ras_count = g_stRasterInt[0].y;		/* 割り込み開始位置 */	/* 初回は0で始まること */
		CRTCRAS( Raster_Func, ras_count );	/* ラスター割り込み */
	}
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

	/* V-Sync割り込み処理 */
	Vsync_count++;
#ifdef 	MACS_MOON
	/* 何もしない */
#else	/* MACS_MOON */
	vdispst = VDISPST(Vsync_Func, 0, 1);	/* V-Sync割り込み 帰線 */
#endif	/* MACS_MOON */

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

//	Message_Num(&g_bRasterSET,  0, 10, 2, MONI_Type_UC, "%4d");
//	Message_Num(&Vsync_count, 	6, 10, 2, MONI_Type_US, "%4d");

#ifdef	MACS_MOON
	IRTS();	/* MACSDRVではrtsで扱う必要あり */
#else	/* MACS_MOON */
	IRTE();	/* 割り込み関数の最後で必ず実施 */
#endif	/* MACS_MOON */
}

SS vwait(SS count)				/* 約count／60秒待つ	*/
{
	SS ret = 0;
	volatile UC *mfp = (UC *)0xe88001;
//	volatile US *BGCTRL = (US *)0xEB0808;
//	*BGCTRL = Mbset(*BGCTRL, Bit_9, Bit_9);
	
	while(count--){
		while(!((*mfp) & 0b00010000));	/* 垂直表示期間待ち	*/
		while((*mfp) & 0b00010000);	/* 垂直帰線期間待ち	*/
	}

	return ret;
}

#endif	/* MFP_H */
