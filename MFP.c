#ifndef	MFP_C
#define	MFP_C

#include <stdio.h>
#include <iocslib.h>
#include <interrupt.h>

#include "inc/usr_macro.h"

#include "OverKata.h"
#include "CRTC.h"
#include "Draw.h"
#include "Graphic.h"
#include "MFP.h"
#include "Moon.h"
#include "OutPut_Text.h"
#include "PCG.h"
#include "Raster.h"

/* define定義 */

/* グローバル変数 */
uint8_t				g_bRasterSET[2];
uint16_t			g_uRasterLine[8];

volatile uint8_t	g_ubRasterFirst;
volatile uint16_t	g_uRasterFirstPos;
volatile uint16_t	g_uRasterSideNow;
volatile uint16_t	g_uRasterSide;
volatile uint16_t	g_uRas_NexrCount;
volatile uint16_t	g_uRas_Count;
volatile uint16_t	g_uRasterSkipStatus;

volatile uint16_t	g_uCRT_Tmg;

volatile uint16_t	Hsync_count;
volatile uint16_t	Vsync_count;
volatile uint16_t	Raster_count;
volatile uint16_t	RasterLine_count;

//volatile uint16_t *CRTC_R06		= (uint16_t *)0xE8000Cu;	/* 垂直表示開始位置-1 */
//volatile uint16_t *CRTC_R09		= (uint16_t *)0xE80012u;	/* ラスター割り込み位置 */
//volatile uint16_t *CRTC_R12		= (uint16_t *)0xE80018u;	/* スクリーン0 X */
//volatile uint16_t *CRTC_R14		= (uint16_t *)0xE8001Cu;	/* スクリーン1 X */
//volatile uint16_t *VIDEO_REG3		= (uint16_t *)0xE82600u;	/* グラフィック表示 */
//volatile uint16_t *BG_CTRL		= (uint16_t *)0xEB0808u;
volatile uint16_t *BG0scroll_x	= (uint16_t *)0xEB0800;
volatile uint16_t *BG0scroll_y	= (uint16_t *)0xEB0802;
volatile uint16_t *BG1scroll_x	= (uint16_t *)0xEB0804;
volatile uint16_t *BG1scroll_y	= (uint16_t *)0xEB0806;

/* スタティック変数 */
static uint8_t g_bTimer_D;
static volatile uint32_t NowTime;
static volatile uint32_t StartTime;
#if 0
static uint32_t g_unTime_cal=0, g_unTime_cal_PH=0, g_unTime_min_PH=0;
#endif

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
int16_t SetRasterIntData(uint16_t);
int16_t GetRasterIntPos(uint16_t *, uint16_t *, uint16_t *, uint16_t);
static void interrupt Timer_D_Func(void);
#if 0
static void interrupt Hsync_Func(void);
#endif
static void interrupt Raster_Func(void);
static void interrupt Vsync_Func(void);

const uint16_t D_RASMAX[2] = { RASTER_L_MAX, RASTER_H_MAX };	/* 0:15kHz 1:31kHz */
const uint16_t D_RAS_INT_MAX[2] = { Y_MAX_WINDOW + 16, 320 + 32};	/* 0:15kHz 1:31kHz */

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
	g_ubRasterFirst = TRUE;
	g_uRasterSide = 0;
	g_uRasterSideNow = 0;
	g_bRasterSET[0] = FALSE;
	g_bRasterSET[1] = FALSE;
	g_uRas_Count = 0;
	g_uRas_NexrCount = RASTER_NEXT;	/* ラスター割り込みスキップカウント最小値で初期化 */
	g_uCRT_Tmg = 0;
	memset(g_stRasterInt, 0, sizeof(ST_RASTER_INT) * RASTER_H_MAX);
	/* H-Sync割り込み */
	Hsync_count = 0;
	/* V-Sync割り込み */
	Vsync_count = 0;
	/* ラスター割り込み */
	Raster_count = 0;

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

	g_bRasterSET[0] = FALSE;
	g_bRasterSET[1] = FALSE;

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

	volatile uint8_t *MFP_INTCTRL_A = (uint8_t *)0xE88007;	/* 割り込みイネーブルレジスタA */
	volatile uint8_t *MFP_TADR  	= (uint8_t *)0xE8801F;	/* タイマーAデータレジスタ */
	
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
//	TIMERDST(Timer_D_Func, 3, 5);	/* Timer-Dセット */	/* 4us(3) x 5cnt = 20us */
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
int16_t SetRasterIntData(uint16_t uSide)
{
	int16_t ret = 0;
	
	g_bRasterSET[1] = TRUE;
	g_uRasterSide = uSide;
	
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

	/* 現在アクティブなラスター割り込みデータを取得 */
	uNum = Mmul2(uNum) + g_uRasterSide;

	if(uNum >= D_RASMAX[0])
	{
		uNum = D_RASMAX[0] - 1;
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
	uint16_t nNum;
	
	Set_DI();	/* 割り込み禁止設定 */
	
	nNum = (g_uRas_Count - g_uRasterFirstPos) + g_uRasterSideNow;
	
	g_uRas_Count += g_uRas_NexrCount;		/* 次のラスタ割り込み位置の計算 */
	*CRTC_R09 = g_uRas_Count;				/* 次のラスタ割り込み位置の設定 */
	
	if(g_uRas_Count < D_RAS_INT_MAX[g_uCRT_Tmg])	/* ラスター割り込み位置の最後 */
	{
		
		uint16_t x, y;
		
		g_bRasterSET[0] = TRUE;
		
		//	*CRTC_R12		= g_stRasterInt[nNum].x + X_OFFSET;			/* GRのX座標の設定 */
		if(g_ubRasterFirst == TRUE)
		{
			g_ubRasterFirst = FALSE;	/* 初回処理終了 */
			/* 背景消去(ラスター割り込み開始位置からグラフィック表示をやめることで、あたかもBGより裏側に見せる) */
			*VIDEO_REG3		= Mbset(*VIDEO_REG3,   0x0C, 0b0000000000000000);	/* GRSC1(GR3,GR4)=OFF */

			y = g_stRasterInt[nNum].y;
			
		}
		else
		{
			y = g_stRasterInt[nNum].y + g_stRasterInt[nNum].pat;
		}
		x = g_stRasterInt[nNum].x;

		/* BG0 */
		*BG0scroll_x	= x;	/* BG0のX座標の設定 */
		*BG0scroll_y	= y;	/* BG0のY座標の設定 */
		/* BG1 */
		*BG1scroll_x	= x;	/* BG1のX座標の設定 */
		*BG1scroll_y	= y;	/* BG1のY座標の設定 */
	}
	else
	{
		g_bRasterSET[0] = FALSE;
		
		CRTCRAS((void *)0, 0);		/* stop */
		g_uRas_Count = g_uRasterFirstPos;		/* 最初に戻しておく */
		*CRTC_R09 = 255;			/* 次のラスタ割り込み位置の設定 */
		
		*BG0scroll_y	= D_RAS_INT_MAX[g_uCRT_Tmg];	/* BG0のY座標の設定 */
		*BG1scroll_y	= D_RAS_INT_MAX[g_uCRT_Tmg];	/* BG1のY座標の設定 */
		
		Raster_count++;	/* ラスタ割り込み一連の処理完了回数 */
	}
	
	RasterLine_count++;	/* ラスター割り込み処理回数 */
	
	Set_EI();	/* 割り込み禁止解除 */
	
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
	uint16_t x, y;
#if 0
	uint32_t time_now;
	static uint32_t unTime_old = 0;
#endif
	
//	Set_DI();	/* 割り込み禁止設定 */
	
	/* V-Sync割り込み処理 */
	Vsync_count++;
	
	/* V-Sync割り込み処理 */
#ifdef 	MACS_MOON
	/* 何もしない */
	/* MACS_Vsync経由であれば割り込み禁止状態で実行されているため、割り込み禁止処置不要 */
#else	/* MACS_MOON */
	Set_DI();	/* 割り込み禁止設定 */
	vdispst = VDISPST((void *)0, 0, 0);	/* stop */
	Set_EI();	/* 割り込み禁止解除 */
#endif	/* MACS_MOON */
	
	if( g_bRasterSET[1] == TRUE )	/* ラスタ割り込み可能 */
	{
		/* 背景表示（割り込み開始前は、背景を表示させておく） */
		*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x0C, 0b0000000000001100);	/* GR2(GR3,GR4)=ON */

		if(g_bRasterSET[0] == TRUE)	/* ラスタ割り込み処理中 */
		{
			/* ラスタ割り込み処理中なので何もしない */
			g_uRasterSkipStatus = g_uGameStatus;
		}
		else						/* ラスタ割り込み処理中でない */
		{
			uint16_t uSide;
			
			g_uRasterSkipStatus = 0;
			
			/* ラスタ割り込み */
			CRTCRAS((void *)0, 0);	/* stop */

			uSide = g_uRasterSide;
			
			/* ラスタ割り込み設定 */
			g_ubRasterFirst = TRUE;	/* ラスタ割り込みで初回だけ処理するためのフラグ */

			if(g_uCRT_Tmg != 0)	/* ラスタ２度読みの場合(31kHz) */
			{
				/* モニタの表示開始位置はCRTC_R06が最初の位置となる */
				/* 割り込み開始表示位置(表示画面開始位置＋スプライトオフセット＋割り込みしたい座標) */
				/* SP/BGは、原点-16のオフセットがあるのでSP/BGに対して割り込みを行う場合、オフセット分を足す必要がある */
				g_uRasterFirstPos = *CRTC_R06 + g_stRasterInt[(RASTER_H_MAX - 2) + uSide].y;
			}
			else				/* ノンインターレースモードの場合(15kHz) */
			{
				/* ラスタ */
				g_uRasterFirstPos = *CRTC_R06 + g_stRasterInt[(RASTER_H_MAX - 2) + uSide].y + SP_Y_OFFSET;	/* 割り込み開始位置 */	/* 第一フィールドのみ */
			}
	//		インターレースの場合	g_uRasterFirstPos = Mmul2((*CRTC_R09 - *CRTC_R06)) + g_stRasterInt[0 + uSide].y + SP_Y_OFFSET;	/* 割り込み開始位置 */	/* 第一フィールドのみ */
			
			/* ラスタ割り込み */
			g_uRas_Count = g_uRasterFirstPos;
			g_uRasterSideNow = uSide;
			CRTCRAS( Raster_Func, g_uRas_Count );	/* ラスター割り込み */

			/* BG */
			x = 256;
			y = g_stRasterInt[(RASTER_H_MAX - 4) + uSide].y;
	//		*BG_CTRL = Mbset(*BG_CTRL, 0x200, 0x000);	/* DISP/CPU=OFF */
			*BG0scroll_x	= x;	/* BG0のX座標の設定 *//* 空のコントロール */
			*BG0scroll_y	= y;	/* BG0のY座標の設定 *//* 空のコントロール */
			*BG1scroll_x	= x;	/* BG1のX座標の設定 *//* 空のコントロール */
			*BG1scroll_y	= y;	/* BG1のY座標の設定 *//* 空のコントロール */
	//		*BG_CTRL = Mbset(*BG_CTRL, 0x200, 0x200);	/* DISP/CPU=ON */

		}
	}
	else	/* ラスタ割り込み不可 */
	{
#if 0	/* デバッグ表示 */
		/* ラスター割り込み */
		Raster_count = 0;
		
		g_unTime_cal = 0;
		g_unTime_cal_PH = 0;
		g_unTime_min_PH = 0;
#endif
	}
	
	if(g_bRasterSET[0] == TRUE)	/* ラスタ割り込み処理中 */
	{
		/* ラスタ割り込み処理中なので何もしない */
	}
	else						/* ラスタ割り込み処理中でない */
	{
		/* 画面切り替え */
		FlipProc();
	}
	
#if 0	/* デバッグ表示 */
	if( g_bRasterSET[1] == TRUE )
	{
		g_uRasterLine[7] = g_uRasterLine[6];
		g_uRasterLine[6] = g_uRasterLine[5];
		g_uRasterLine[5] = g_uRasterLine[4];
		g_uRasterLine[4] = g_uRasterLine[3];
		g_uRasterLine[3] = g_uRasterLine[2];
		g_uRasterLine[2] = g_uRasterLine[1];
		g_uRasterLine[1] = g_uRasterLine[0];
		g_uRasterLine[0] = RasterLine_count + (g_uRasterSkipStatus * 100);
		
		/* 処理時間計測 */
		GetNowTime(&time_now);
		g_unTime_cal = time_now - unTime_old;	/* LSB:1 UNIT:ms */
		if(unTime_old != 0)
		{
			g_unTime_min_PH = Mmin(g_unTime_cal, g_unTime_min_PH);
			g_unTime_cal_PH = Mmax(g_unTime_cal, g_unTime_cal_PH);
		}
		unTime_old = time_now;
		
		if(g_bRasterSET[0] == TRUE)	/* ラスタ割り込み処理中 */
		{
//			Debug_View(Vsync_count);	/* デバッグ情報表示 */
		}
		
		RasterLine_count = 0;
	}
#endif
	
	/* H-Sync割り込み処理 */
	Hsync_count = 0;
#if 0
	HSYNCST(Hsync_Func);	/* H-Sync割り込み */
#endif
	
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
