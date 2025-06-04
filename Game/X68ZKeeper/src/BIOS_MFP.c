#ifndef	BIOS_MFP_C
#define	BIOS_MFP_C

#include <stdio.h>
#include <iocslib.h>
#include <interrupt.h>

#include <usr_macro.h>

#include "BIOS_MFP.h"
#include "BIOS_MPU.h"
#include "BIOS_Moon.h"
#include "BIOS_CRTC.h"
#include "BIOS_PCG.h"
#include "main.h"


/* define定義 */

/* グローバル変数 */
int8_t				g_bMFP = FALSE;
volatile uint16_t	g_uHsync_count;
volatile uint16_t	g_uVsync_count;
volatile uint16_t	g_uRaster_count;

/* スタティック変数 */
static volatile uint8_t 	g_bTimer_D;
static volatile uint32_t	g_NowTime;
static volatile uint32_t	g_StartTime;
static volatile uint32_t	g_CPU_Time;
static volatile uint8_t		g_bRasterUse;
static volatile uint16_t	g_uRasterPos;
static volatile uint16_t	g_uRasterPos_first;
static volatile uint16_t	g_uRasterPos_clip;

#if 0
static uint32_t g_unTime_cal=0, g_unTime_cal_PH=0, g_unTime_min_PH=0;
#endif

/* 関数のプロトタイプ宣言 */
int16_t MFP_INIT(void);
int16_t MFP_EXIT(void);
int16_t MFP_RESET(void);
uint16_t TimerD_INIT(void);
int16_t TimerD_EXIT(void);
uint8_t GetNowTime(uint32_t *);	/* 現在の時間を取得する */
uint8_t SetNowTime(uint32_t);		/* 現在の時間を設定する */
uint8_t GetStartTime(uint32_t *);	/* 開始の時間を取得する */
uint8_t SetStartTime(uint32_t);	/* 開始の時間を設定する */
uint8_t GetPassTime(uint32_t, uint32_t *);	/* 経過タイマー */
uint32_t Get_CPU_Time(void);
int16_t StartRasterInt(void);
int16_t StopRasterInt(void);
uint8_t GetRasterCount(uint16_t *);
uint8_t	SetRasterCount(uint16_t, uint16_t);

static void interrupt Timer_D_Func(void);
static void interrupt Hsync_Func(void);
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
#if CNF_VDISP
#if CNF_XSP
#else
	volatile uint8_t *MFP_TACR = (uint8_t *)0xe88019;
	volatile uint8_t *MFP_TADR = (uint8_t *)0xe8801f;
#endif	/* CNF_XSP */
#endif	/* CNF_VDISP */

	g_bMFP = FALSE;

#if CNF_HDISP	
	/* H-Sync割り込み */
	g_uHsync_count = 0;
#endif /* CNF_HDISP */

	g_CPU_Time = TimerD_INIT();		/* Timer-D初期化 */
	
#if CNF_RASTER
	/* ラスター割り込み */
 #if CNF_XSP
	/* ラスタ割り込み開始(IF_PCG.cで実施) */
 #else
	g_uRaster_count = 0;
	StopRasterInt();	/* ラスター割り込み処理を停止 */
	g_uRasterPos = 0;
	g_uRasterPos_first = 0;
	g_uRasterPos_clip = 0xFFFF;
 #endif
#endif /* CNF_RASTER */

#if CNF_VDISP
	/* V-Sync割り込み */
	g_uVsync_count = 0;
#if CNF_XSP
#else
	//VDISPST 初回だけバグあり（EXCEED.さん）
	*MFP_TACR = 0x00;	/* タイマーＡを止める */
	*MFP_TADR = 0x01;	/* カウンタを設定(0=256回) */
	*MFP_TACR = 0x08;	/* イベントカウントモードに設定する */
#endif	/* CNF_XSP */

#if CNF_XSP
	/* 帰線期間割り込み開始(IF_PCG.cで実施) */
#elif CNF_MACS	
 #ifdef 	MACS_MOON
	vdispst = MACS_Vsync(Vsync_Func);		/* V-Sync割り込み */
 #else	/* MACS_MOON */
	vdispst = VDISPST(Vsync_Func, 0, 1);	/* V-Sync割り込み 帰線 */
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
	vdispst = VDISPST(Vsync_Func, 0, 1);	/* V-Sync割り込み 帰線 */
#endif	/* CNF_MACS */
//	Message_Num(&vdispst,  0, 10, 2, MONI_Type_SI, "%4d");
#endif	/* CNF_VDISP */
	g_bMFP = TRUE;

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

	if(g_bMFP == TRUE)
	{
		g_bMFP = FALSE;

		TimerD_EXIT();			/* Timer-D 終了 */
#if CNF_RASTER
		StopRasterInt();		/* ラスター割り込み処理を停止 */
		puts("MFP_EXIT CRTCRAS");
#endif /* CNF_RASTER */

#if CNF_HDISP
		_iocs_hsyncst((void *)0);		/* stop */
		puts("MFP_EXIT HSYNCST");
#endif	/* CNF_HDISP */

#if CNF_VDISP
#if CNF_XSP

#elif CNF_MACS
 #ifdef 	MACS_MOON
		vdispst = MACS_Vsync_R(Vsync_Func);	/* stop */
		puts("MFP_EXIT MACS_Vsync_R");
 #else	/* MACS_MOON */
		vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
		puts("MFP_EXIT VDISPST");
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
		vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
		puts("MFP_EXIT VDISPST");
#endif	/* CNF_MACS */
//		Message_Num(&vdispst,  6, 10, 2, MONI_Type_SI, "%4d");
#endif	/* CNF_VDISP */
	}

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
uint16_t	TimerD_INIT()
{
	uint16_t CpuCount=0;
	int32_t nUse;
	/* Timer-D割り込み */
	g_NowTime = 0;
	g_StartTime = 0;

	nUse = _iocs_timerdst(Timer_D_Func, 7, 20);	/* Timer-Dセット */	/* 50us(7) x 20cnt = 1ms */
//	TIMERDST(Timer_D_Func, 3, 5);	/* Timer-Dセット */	/* 4us(3) x 5cnt = 20us */
	if(nUse == 0)
	{
		g_bTimer_D =TRUE;
		SetNowTime(g_NowTime);	/* 時間の初期化 */

		CpuCount = (uint16_t)Mdiv65536(mpu_stat_chk() & 0xFFFF0000);
		if(CpuCount != 0)
		{
			/* bit 31～16	クロックスピード(0.1MHz単位) */
		}
		else
		{
			/* マイコンクロックを計測 */
			do
			{
				CpuCount++;
			}
			while(g_NowTime==0);
		}
	}
	else
	{
		puts("TimerDが使えませんでした");		
	}

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
	if(g_bTimer_D == TRUE)
	{
		_iocs_timerdst((void *)0, 0, 1);	/* stop */
		g_bTimer_D = FALSE;
	}

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
	*time = g_NowTime;
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
	g_NowTime = time;
	
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
	*time = g_StartTime;
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
	g_StartTime = time;
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
	
	uTime = g_NowTime;
	uTimeMem = (*pOldTimer);

	if(*pOldTimer == PASSTIME_INIT)
	{
		*pOldTimer = uTime;
		ret = TRUE;			/* 経過した */
	}
	else
	{
		uTimeDiff = uTime - uTimeMem;
		
		if(uSetTimer <= uTimeDiff)
		{
			*pOldTimer = uTime;
			ret = TRUE;			/* 経過した */
		}
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
int16_t StartRasterInt(void)
{
	int16_t ret = 0;

	g_bRasterUse = TRUE;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t StopRasterInt(void)
{
	int16_t ret = 0;
	
	if(g_bRasterUse == TRUE)
	{
		g_bRasterUse = FALSE;
		_iocs_crtcras((void *)0, 0);	/* stop */
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
uint8_t	GetRasterCount(uint16_t *uCount)
{
	uint8_t	ret = 0;

	*uCount = g_uRasterPos;
	ret = g_bRasterUse;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint8_t	SetRasterCount(uint16_t uCount, uint16_t uClip)
{
	uint8_t	ret = 0;

	g_uRasterPos = uCount;
	g_uRasterPos_first = uCount;
	g_uRasterPos_clip = uClip;
	ret = g_bRasterUse;
	
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
	g_NowTime++;

	App_TimerProc();	/* タイマー割り込み処理 */

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint8_t Timer_D_Less_NowTime(void)
{
	if(g_bTimer_D == FALSE)
	{
		 g_NowTime += 15;	/* 15ms(1ms分解能) */
	}

	return	g_bTimer_D;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint32_t Get_CPU_Time(void)
{
	return g_CPU_Time;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void interrupt Hsync_Func(void)
{
#if CNF_HDISP	/* H-Sync割り込み処理 */
	_iocs_hsyncst((void *)0);			/* stop */

	g_uHsync_count++;

#endif	/* CNF_HDISP */
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}
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
#if CNF_RASTER	/* ラスター割り込み処理 */
	uint16_t uRaster_pos;

	g_uRaster_count++;		/* ラスター割り込み処理回数 */

	uRaster_pos = g_uRasterPos - g_uRasterPos_first;

	/* アプリケーション処理 */
	App_RasterProc( &uRaster_pos );
	
	g_uRasterPos = Mmin((g_uRasterPos_first + uRaster_pos), g_uRasterPos_clip);	/* クリップ処理 */
	CRTC_Rastar_SetPos(g_uRasterPos);

#endif	/* CNF_RASTER */

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
#if CNF_VDISP	/* V-Sync割り込み処理 */
	int32_t	vdispst = -1;	
	
	g_uVsync_count++;

#if CNF_XSP
	vdispst = 0;
#elif CNF_MACS
 #ifdef 	MACS_MOON
	/* 何もしない */
	/* MACS_Vsync経由であれば割り込み禁止状態で実行されているため、割り込み禁止処置不要 */
 #else	/* MACS_MOON */
	Set_DI();	/* 割り込み禁止設定 */
	vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
	Set_EI();	/* 割り込み禁止解除 */
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
	Set_DI();	/* 割り込み禁止設定 */
	vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
	Set_EI();	/* 割り込み禁止解除 */
#endif	/* CNF_MACS */
	
	App_VsyncProc();	/* アプリ V-Sync割り込み処理 */

#if CNF_RASTER	/* ラスター割り込み処理 */
	/* ラスタ割り込み */
	if(g_bRasterUse == TRUE)
	{
#if CNF_XSP
#else	/* CNF_XSP */
		_iocs_crtcras((void *)0, 0);	/* stop */
		_iocs_crtcras( Raster_Func, g_uRasterPos );	/* ラスター割り込み */
#endif
	}
#endif	/* CNF_RASTER */
	
#if CNF_HDISP	/* H-Sync割り込み処理 */
	Hsync_count = 0;
	_iocs_hsyncst((void *)0);		/* stop */
	_iocs_hsyncst(Hsync_Func);	/* H-Sync割り込み */
#endif

#if CNF_XSP
	vdispst = 0;
#elif CNF_MACS
 #ifdef 	MACS_MOON
	/* 何もしない */
	IRTS();	/* MACSDRVではrtsで扱う必要あり */
 #else	/* MACS_MOON */
	vdispst = _iocs_vdispst(Vsync_Func, 0, 1);	/* V-Sync割り込み 帰線 */
	IRTE();	/* 割り込み関数の最後で必ず実施 */
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
	vdispst = _iocs_vdispst(Vsync_Func, 0, 1);	/* V-Sync割り込み 帰線 */
	IRTE();	/* 割り込み関数の最後で必ず実施 */
#endif	/* CNF_MACS */
#else	/* CNF_VDISP */
	IRTE();	/* 割り込み関数の最後で必ず実施 */
#endif	/* CNF_VDISP */
}

#endif	/* BIOS_MFP_H */
