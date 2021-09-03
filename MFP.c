#ifndef	MFP_C
#define	MFP_C

#include <stdio.h>
#include <iocslib.h>
#include <interrupt.h>

#include "inc/usr_macro.h"

#include "OverKata.h"
#include "MFP.h"
#include "Moon.h"
#include "OutPut_Text.h"
#include "PCG.h"
#include "Raster.h"

/* define定義 */

/* グローバル変数 */

/* スタティック変数 */
static uint8_t g_bTimer_D;
static uint8_t g_bRasterSET;
static volatile uint32_t NowTime;
static volatile uint32_t StartTime;
static volatile uint16_t g_uRas_Count;
static volatile uint16_t Hsync_count;
static volatile uint16_t Vsync_count;

/* 関数のプロトタイプ宣言 */
int16_t MFP_INIT(void);
int16_t MFP_EXIT(void);
int16_t MFP_RESET(void);
int16_t TimerD_INIT(void);
int16_t TimerD_EXIT(void);
uint8_t GetNowTime(uint32_t *);	/* 現在の時間を取得する */
uint8_t SetNowTime(uint32_t);		/* 現在の時間を設定する */
uint8_t GetStartTime(uint32_t *);	/* 開始の時間を取得する */
uint8_t SetStartTime(uint32_t);	/* 開始の時間を設定する */
uint8_t GetPassTime(uint32_t, uint32_t *);	/* 経過タイマー */
int16_t SetRasterIntData(void *, size_t);
int16_t GetRasterIntPos(uint16_t *, uint16_t *, uint16_t *, uint16_t);
static void interrupt Timer_D_Func(void);
#if 0
static void interrupt Hsync_Func(void);
#endif
static void interrupt Raster_Func(void);
static void interrupt Vsync_Func(void);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t MFP_INIT(void)
{
	int32_t	vdispst = -1;
	volatile uint8_t *MFP_TACR = (uint8_t *)0xe88019;
	volatile uint8_t *MFP_TADR = (uint8_t *)0xe8801f;

	/* ラスタ割り込み */
	g_bRasterSET = FALSE;
	g_uRas_Count = 0;
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t MFP_EXIT(void)
{
	int32_t	vdispst = -1;

	g_bRasterSET = FALSE;

	CRTCRAS((void *)0, 0);		/* stop */
	puts("MFP_EXIT CRTCRAS");
	
	HSYNCST((void *)0);			/* stop */
	puts("MFP_EXIT HSYNCST");

#ifdef 	MACS_MOON
	vdispst = MACS_Vsync_R(Vsync_Func);	/* stop */
	puts("MFP_EXIT MACS_Vsync_R");
#else	/* MACS_MOON */
	vdispst = VDISPST((void *)0, 0, 0);	/* stop */
	puts("MFP_EXIT VDISPST");
#endif	/* MACS_MOON */
	
//	Message_Num(&vdispst,  6, 10, 2, MONI_Type_SI, "%4d");

	return vdispst;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t MFP_RESET(void)
{
	int16_t	ret = 0;

	volatile uint8_t *MFP_INTCTRL_A  = (uint8_t *)0xE88007;	/* 割り込みイネーブルレジスタA */
	volatile uint8_t *MFP_TADR  		= (uint8_t *)0xE8801F;	/* タイマーAデータレジスタ */
	
	*MFP_TADR = 1;					/* カウンタ設定 */
	*MFP_INTCTRL_A |= 0b01000000;	/* CRTC Raster割り込み許可 */
//	*MFP_INTCTRL_A |= 0b00100000;	/* Timer-A割り込み許可 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	TimerD_INIT()
{
	int16_t CpuCount=0;
	/* Timer-D割り込み */
	NowTime = 0;
	StartTime = 0;
	TIMERDST(Timer_D_Func, 7, 20);	/* Timer-Dセット */	/* 50us(7) x 20cnt = 1ms */
	g_bTimer_D =TRUE;
	SetNowTime(NowTime);	/* 時間の初期化 */
	/* マイコンクロックを計測 */
	do
	{
		CpuCount++;
	}
	while(NowTime==0);

	return CpuCount;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	TimerD_EXIT()
{
	TIMERDST((void *)0, 0, 1);	/* stop */
	g_bTimer_D = FALSE;

	return 0;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint8_t GetNowTime(uint32_t *time)	/* 現在の時間を取得する */
{
	*time = NowTime;
	return g_bTimer_D;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint8_t SetNowTime(uint32_t time)	/* 現在の時間を設定する */
{
	NowTime = time;
	return g_bTimer_D;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint8_t GetStartTime(uint32_t *time)	/* 開始の時間を取得する */
{
	*time = StartTime;
	return g_bTimer_D;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint8_t SetStartTime(uint32_t time)	/* 開始の時間を設定する */
{
	StartTime = time;
	return g_bTimer_D;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	分解能1ms */
/*===========================================================================================*/
uint8_t GetPassTime(uint32_t uSetTimer, uint32_t *pOldTimer)	/* 経過タイマー */
{
	uint8_t ret = FALSE;
	uint32_t uTime, uTimeDiff, uTimeMem;
	
	uTime = NowTime;
	uTimeMem = (*pOldTimer);
	
	uTimeDiff = uTime - uTimeMem;
	
	if(uSetTimer <= uTimeDiff)
	{
		*pOldTimer = uTime;
		ret = TRUE;
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t SetRasterIntData(void *pSrc, size_t n)
{
	int16_t ret = 0;
#if	1
	g_bRasterSET = TRUE;
#else
	if(memcpy(g_stRasterInt, pSrc, n) == NULL)
	{
		ret = -1;
	}
	else
	{
		g_bRasterSET = TRUE;
	}
#endif
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t GetRasterIntPos(uint16_t *x, uint16_t *y, uint16_t *pat, uint16_t uNum)
{
	int16_t	ret = 0;

	if(uNum >= RASTER_MAX)
	{
		uNum = RASTER_MAX - 1;
		ret = -1;
	}
	
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
	
	return ret;
}

/* 割り込み関数は修飾子にinterruptを入れること */	/* くにちこ（Kunihiko Ohnaka）さんのアドバイス */

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void interrupt Timer_D_Func(void)
{
	NowTime++;

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
#if 0
static void interrupt Hsync_Func(void)
{
	HSYNCST((void *)0);			/* stop */
	Hsync_count++;
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}
#endif
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* ラスター割り込みの処理内は必要最低限だけ */	/* EXCEED.さんのアドバイス */
/* ２ライン飛ばしぐらいが精度の限界 */	/* EXCEED.さんのアドバイス */
static void interrupt Raster_Func(void)
{
	volatile uint16_t *BG0scroll_x	= (uint16_t *)0xEB0800;
	volatile uint16_t *BG0scroll_y	= (uint16_t *)0xEB0802;
	volatile uint16_t *BG1scroll_x	= (uint16_t *)0xEB0804;
	volatile uint16_t *BG1scroll_y	= (uint16_t *)0xEB0806;
	volatile uint16_t *CRTC_R09		= (uint16_t *)0xE80012u;	/* ラスター割り込み位置 */
//	volatile uint16_t *CRTC_R12		= (uint16_t *)0xE80018u;	/* スクリーン0 X */
//	volatile uint16_t *CRTC_R14		= (uint16_t *)0xE8001Cu;	/* スクリーン1 X */
	volatile uint16_t *VIDEO_REG3	= (uint16_t *)0xE82600;	/* グラフィック表示 */

	uint16_t nNum = g_uRas_Count - SP_Y_OFFSET;	/* おそらくここはSP_Y_OFFSETではなく、CRTC_R06分を引くのが正解 */

#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	g_uRas_Count += RASTER_NEXT;		/* 次のラスタ割り込み位置の計算 */
	*CRTC_R09 = g_uRas_Count;			/* 次のラスタ割り込み位置の設定 */

//	*CRTC_R12		= g_stRasterInt[nNum].x + X_OFFSET;			/* GRのX座標の設定 */
	if( g_stRasterInt[0].y < nNum )
	{
		/* 背景消去(ラスター割り込み開始位置からグラフィック表示をやめることで、あたかもBGより裏側に見せる) */
		*VIDEO_REG3		= Mbset(*VIDEO_REG3,   0x0C, 0b0000000000000000);	/* GRSC1(GR3,GR4)=OFF */
		/* BG0 */
		*BG0scroll_x	= g_stRasterInt[nNum].x;							/* BG0のX座標の設定 */
		*BG0scroll_y	= g_stRasterInt[nNum].y + g_stRasterInt[nNum].pat;	/* BG0のY座標の設定 */
		/* BG1 */
		*BG1scroll_x	= g_stRasterInt[nNum].x;							/* BG1のX座標の設定 */
		*BG1scroll_y	= g_stRasterInt[nNum].y + g_stRasterInt[nNum].pat;	/* BG1のY座標の設定 */
	}
	else
	{
		/* BG0 */
		*BG0scroll_x	= 256;					/* BG0のX座標の設定 */
		*BG0scroll_y	= g_stRasterInt[1].y;	/* BG0のY座標の設定 */
		/* BG1 */
		*BG1scroll_x	= 256;					/* BG1のX座標の設定 */
		*BG1scroll_y	= g_stRasterInt[1].y;	/* BG1のY座標の設定 */
	}
	
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void interrupt Vsync_Func(void)
{
	volatile uint16_t *VIDEO_REG3	= (uint16_t *)0xE82600;
	volatile uint16_t *BG0scroll_x	= (uint16_t *)0xEB0800;
	volatile uint16_t *BG0scroll_y	= (uint16_t *)0xEB0802;
	volatile uint16_t *BG1scroll_x	= (uint16_t *)0xEB0804;
	volatile uint16_t *BG1scroll_y	= (uint16_t *)0xEB0806;
//	volatile uint16_t *BGCTRL		= (uint16_t *)0xEB0808;
	volatile uint16_t *CRTC_R06		= (uint16_t *)0xE8000Cu;	/* 垂直表示開始位置-1 */
//	volatile uint16_t *CRTC_R09		= (uint16_t *)0xE80012u;

	/* V-Sync割り込み処理 */
#ifdef 	MACS_MOON
	/* 何もしない */
#else	/* MACS_MOON */
	vdispst = VDISPST((void *)0, 0, 0);	/* stop */
#endif	/* MACS_MOON */

	/* 背景表示（割り込み開始前は、背景を表示させておく） */
	*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x0C, 0b0000000000001100);	/* GR2(GR3,GR4)=ON */

//	*BGCTRL = Mbclr(*BGCTRL, Bit_9);
	/* ここはラスタ割り込み開始位置分のズレは考慮しなくてよい */
	*BG0scroll_x	= 256;					/* BG0のX座標の設定 *//* 空のコントロール */
	*BG0scroll_y	= g_stRasterInt[1].y;	/* BG0のY座標の設定 *//* 空のコントロール */
	*BG1scroll_x	= 256;					/* BG1のX座標の設定 *//* 空のコントロール */
	*BG1scroll_y	= g_stRasterInt[1].y;	/* BG1のY座標の設定 *//* 空のコントロール */
	
	/* H-Sync割り込み処理 */
	Hsync_count = 0;
#if 0
	HSYNCST(Hsync_Func);	/* H-Sync割り込み */
#endif
	
	/* ラスタ割り込み */
	CRTCRAS((void *)0, 0);	/* stop */
	if(g_bRasterSET == TRUE)
	{
#if 1	/* ノンインターレースモードの場合 */
		/* モニタの表示開始位置はCRTC_R06が最初の位置となる */
		/* ラスタ配列の要素番号＝画面のY座標 */
		/* ただしSP/BGは、X+16,Y+16のオフセットがあるので */
		/* ラスタ配列からオフセット分を足す必要がある */
		g_uRas_Count = *CRTC_R06 + g_stRasterInt[0].y + SP_Y_OFFSET;	/* 割り込み開始位置 */	/* 初回は0で始まること */
#else	/* インターレースモードの場合 */
		g_uRas_Count = ((*CRTC_R09 - *CRTC_R06) * 2) + g_stRasterInt[0].y + SP_Y_OFFSET;	/* 割り込み開始位置 */	/* 初回は0で始まること */
#endif
		CRTCRAS( Raster_Func, g_uRas_Count );	/* ラスター割り込み */
	}

	/* V-Sync割り込み処理 */
	Vsync_count++;
#ifdef 	MACS_MOON
	/* 何もしない */
#else	/* MACS_MOON */
	vdispst = VDISPST(Vsync_Func, 0, 1);	/* V-Sync割り込み 帰線 */
#endif	/* MACS_MOON */

#ifdef	MACS_MOON
	IRTS();	/* MACSDRVではrtsで扱う必要あり */
#else	/* MACS_MOON */
	IRTE();	/* 割り込み関数の最後で必ず実施 */
#endif	/* MACS_MOON */
	
}

#endif	/* MFP_H */
