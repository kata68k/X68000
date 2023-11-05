#ifndef	APL_TEXT_C
#define	APL_TEXT_C

#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>
#include "APL_Text.h"

#include "BIOS_CRTC.h"
#include "BIOS_MFP.h"
#include "BIOS_PCG.h"
#include "IF_Draw.h"
#include "IF_Graphic.h"
#include "IF_Text.h"
#include "APL_Graphic.h"
#include "IF_Math.h"
#include "APL_MyCar.h"
#include "APL_Score.h"

/* グローバル変数 */
int32_t g_TpalDef[16];
uint8_t g_bTpalDef_Flag = TRUE;
static	uint8_t g_bTimerStopFlag;
static	uint8_t g_bTimerResetFlag;

/* 構造体 */
static ST_TEXTINFO	g_stTextInfo = {0};

/* 関数のプロトタイプ宣言 */
void T_DataInit(void);
void T_SetBG_to_Text(void);
void T_TopScore(void);
void T_Time(void);
void T_Score(void);
void T_Speed(void);
void T_Gear(void);
void T_TimerStop(void);
void T_TimerStart(void);
void T_TimerReset(void);
uint16_t T_Main(uint16_t *);
int16_t T_PutTextInfo(ST_TEXTINFO);
int16_t T_Get_TextInfo(ST_TEXTINFO *);
int16_t T_Set_TextInfo(ST_TEXTINFO);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_DataInit(void)
{
	g_bTpalDef_Flag = FALSE;
	g_bTimerStopFlag = TRUE;	/* stop */
	g_bTimerResetFlag = FALSE;
	
	g_stTextInfo.uTimeCounter = 100;
	g_stTextInfo.ulScore = 0;
	g_stTextInfo.ulScoreMax = 10000;
	g_stTextInfo.uVs = 0;
	g_stTextInfo.uShiftPos = 1;
	g_stTextInfo.uPosX = 0;
	g_stTextInfo.uPosY = 0;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_SetBG_to_Text(void)
{
	int16_t	i;
	uint16_t	x, y;

	/* テキストエリアに作業データ展開1 */
	x = 0;
	y = 256;
	BG_TextPut("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", x, y);//232
	
	/* テキストエリアに作業データ展開2 */
	x = 0;
	y += 8;//240	248+8=256以降はNG
	for(i=0; i < 10; i++)
	{
		BG_PutToText(   0x80+ (i<<1) + 0, x + BG_WIDTH * i,	y,				BG_Normal, TRUE);	/* 数字大（上側）*/
		BG_PutToText(   0x80+ (i<<1) + 1, x + BG_WIDTH * i,	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 数字大（下側）*/
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_TopScore(void)
{
	uint32_t	i,e;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	x = 0, y = 0;
	
	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	
	for(i=0; i < MYCAR_IMAGE_MAX; i++)
	{
		/* TOP */
		BG_PutToText(  1, (BG_WIDTH * 0) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 左上 */
		BG_PutToText(  2, (BG_WIDTH * 1) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 2) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 3) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  3, (BG_WIDTH * 4) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 右上 */
		for(e = 0; e < 5; e++)
		{
			BG_PutToText(e+4, (BG_WIDTH * e) + (uWidth * x),  8 + (Y_OFFSET * y) , BG_Normal, FALSE);
		}
		BG_PutToText(  1, (BG_WIDTH * 0) + (uWidth * x),  16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 左下 */
		BG_PutToText(  2, (BG_WIDTH * 1) + (uWidth * x),  16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 2) + (uWidth * x),  16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 3) + (uWidth * x),  16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  3, (BG_WIDTH * 4) + (uWidth * x),  16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 右下 */
		
		x++;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_Time(void)
{
	uint32_t	i,e;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	x = 0, y = 0;
	
	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */

	for(i=0; i < MYCAR_IMAGE_MAX; i++)
	{
		/* TIME */
		BG_PutToText(  1, (BG_WIDTH * 12) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 左上 */
		BG_PutToText(  2, (BG_WIDTH * 13) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 14) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 15) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 16) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  3, (BG_WIDTH * 17) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 右上 */
		BG_PutToText(  4, (BG_WIDTH * 12) + (uWidth * x),  8 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 左 */
		for(e = 0; e < 5; e++)
		{
			BG_PutToText(e+10, (BG_WIDTH * (13 + e)) + (uWidth * x),  8 + (Y_OFFSET * y) , BG_Normal, FALSE);
		}
		BG_PutToText(  1, (BG_WIDTH * 12) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 左下 */
		BG_PutToText(  2, (BG_WIDTH * 13) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 14) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 15) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 16) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  3, (BG_WIDTH * 17) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 右下 */

		x++;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_Score(void)
{
	uint32_t	i,e;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	x = 0, y = 0;
	
	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */

	for(i=0; i < MYCAR_IMAGE_MAX; i++)
	{
		/* SCORE */
		BG_PutToText(  1, (BG_WIDTH * 17) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 左上 */
		BG_PutToText(  2, (BG_WIDTH * 18) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 19) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 20) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 21) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText(  2, (BG_WIDTH * 22) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 上 */
		BG_PutToText( 15, (BG_WIDTH * 23) + (uWidth * x),  0 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 右上 */
		BG_PutToText(  4, (BG_WIDTH * 17) + (uWidth * x),  8 + (Y_OFFSET * y) , BG_Normal, FALSE);	/* 左 */
		for(e = 0; e < 6; e++)
		{
			BG_PutToText(e+16, (BG_WIDTH * (18 + e)) + (uWidth * x),  8 + (Y_OFFSET * y) , BG_Normal, FALSE);
		}
		BG_PutToText(  1, (BG_WIDTH * 17) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 左下 */
		BG_PutToText(  2, (BG_WIDTH * 18) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 19) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 20) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 21) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText(  2, (BG_WIDTH * 22) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 下 */
		BG_PutToText( 15, (BG_WIDTH * 23) + (uWidth * x), 16 + (Y_OFFSET * y) , BG_H_rev, FALSE);	/* 右下 */

		x++;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_Speed(void)
{
	uint32_t	i;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	x = 0, y = 0;
	
	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */

	for(i=0; i < MYCAR_IMAGE_MAX; i++)
	{
		/* SPEED */
		BG_TextPut("SPEED", 148 + (uWidth * x), 24 + (Y_OFFSET * y) );
		/* KM */
		BG_TextPut("KM",    216 + (uWidth * x), 24 + (Y_OFFSET * y) );

		x++;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_Gear(void)
{
	uint32_t	i;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	x = 0, y = 0;
	
	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */

	for(i=0; i < MYCAR_IMAGE_MAX; i++)
	{
		/* GEAR */
		BG_TextPut("GEAR", 152 + (uWidth * x), 32 + (Y_OFFSET * y) );

		x++;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_TimerStop(void)
{
	g_bTimerStopFlag = TRUE;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_TimerStart(void)
{
	g_bTimerStopFlag = FALSE;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_TimerReset(void)
{
	SetNowTime(0);
	g_bTimerResetFlag = TRUE;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint16_t T_Main(uint16_t *Timer)
{
	uint16_t uRet = 0u;
	
	uint32_t time_now;
	static uint32_t time_old = 0;
	uint32_t unStart_time, unTimer;
	uint16_t uTimeCounter;
	static uint32_t unPassTime = 0;
	ST_CARDATA	stMyCar = {0};
	ST_SCORE stScore;
	
	S_Get_ScoreInfo(&stScore);	/* スコア情報の取得 */

	GetMyCar(&stMyCar);			/* 自車の情報を取得 */

	/* 現在時間 */
	GetNowTime(&time_now);
	/* 開始時間 */
	GetStartTime(&unStart_time);
	
	/* タイマーリセット処理 */
	if(g_bTimerResetFlag == TRUE)	/* タイマーリセット要求あり */
	{
		g_bTimerResetFlag = FALSE;	/* タイマーリセット要求解除 */
		time_old = time_now;		/* リセット */
		unPassTime = 0;				/* リセット */
	}
	
	/* Time Count */
	if(time_old != 0)
	{
		if(g_bTimerStopFlag == FALSE)
		{
			unPassTime += (unStart_time - time_old);
		}
		unTimer = 120000 - unPassTime;
		if(120000 < unTimer)
		{
			uTimeCounter = 0;
		}
		else
		{
			uTimeCounter = (uint16_t)(unTimer / 1000);
		}
		g_stTextInfo.uTimeCounter = uTimeCounter;
	}
	time_old = unStart_time;
	*Timer = g_stTextInfo.uTimeCounter;
	
	/* Score */
	g_stTextInfo.ulScore = stScore.ulScore;

	/* Top Score */
	g_stTextInfo.ulScoreMax = stScore.ulScoreMax;
	
	/* Speed */
	g_stTextInfo.uVs = stMyCar.VehicleSpeed;
	
	/* Gear */
	g_stTextInfo.uShiftPos = (uint16_t)stMyCar.ubShiftPos;
	
	/* 描画 */
	T_PutTextInfo(g_stTextInfo);
	
	return uRet;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t T_PutTextInfo(ST_TEXTINFO stTextInfo)
{
	int16_t	ret = 0;
	
	uint16_t	x, y;
	uint32_t	uWidth, uHeight, uFileSize;

	x = stTextInfo.uPosX;
	y = stTextInfo.uPosY;

	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */

	/* Top Score */
	Text_To_Text2(stTextInfo.ulScoreMax,				(uWidth * x) +  36, (Y_OFFSET * y) +  8, FALSE, "%8d");
	/* Score */
	Text_To_Text2(stTextInfo.ulScore,					(uWidth * x) + 192, (Y_OFFSET * y) +  8, FALSE, "%8d");
	/* Time Count */
	Text_To_Text2((uint64_t)stTextInfo.uTimeCounter,	(uWidth * x) + 108, (Y_OFFSET * y) + 24,  TRUE, "%3d");
	/* Speed */
	Text_To_Text2((uint64_t)stTextInfo.uVs,				(uWidth * x) + 190, (Y_OFFSET * y) + 24, FALSE, "%3d");
	/* Gear */
	Text_To_Text2((uint64_t)stTextInfo.uShiftPos,		(uWidth * x) + 206, (Y_OFFSET * y) + 32, FALSE,  "%d");

	/* 表示座標変更 */
	T_Scroll( (uWidth * x), (Y_OFFSET * y) );	/* テキスト画面スクロール */
	
	return ret;
}




/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t T_Get_TextInfo(ST_TEXTINFO *pTextInfo)
{
	int16_t	ret = 0;

	*pTextInfo = g_stTextInfo;	/* 現情報を取得 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t T_Set_TextInfo(ST_TEXTINFO TextInfo)
{
	int16_t	ret = 0;

	g_stTextInfo = TextInfo;	/* 更新 */
	
	return ret;
}

#endif	/* APL_TEXT_C */

