#ifndef	TEXT_C
#define	TEXT_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "Text.h"

#include "MFP.h"
#include "MyCar.h"
#include "OutPut_Text.h"

/* 構造体 */
ST_TEXTINFO	stTextInfo = {0};

/* 関数のプロトタイプ宣言 */
void T_INIT(void);
void T_EXIT(void);
void T_SetBG_to_Text(void);
void T_Clear(void);
void T_TopScore(void);
void T_Time(void);
void T_Score(void);
void T_Speed(void);
void T_Gear(void);
void T_Main(void);

/* 関数 */
void T_INIT(void)
{
	B_CUROFF();			/* カーソルを消します */
	MS_CUROF();			/* マウスカーソルを消します */
	SKEY_MOD(0, 0, 0);	/* ソフトウェアキーボードを消します */
	T_Clear();			/* テキストクリア */

	/* テキストパレットの初期化(Pal0はSPと共通) */
	TPALET2( 0, SetRGB( 0,  0,  0));	/* Black */
	TPALET2( 1, SetRGB( 1,  0,  0));	/* Black2 */
	TPALET2( 2, SetRGB(31,  0,  0));	/* Red */
	TPALET2( 3, SetRGB(30, 26, 16));	/* Red2 */
	TPALET2( 4, SetRGB(31, 31, 31));	/* White */
	TPALET2( 5, SetRGB(30,  8,  0));	/* Orenge */
	TPALET2( 6, SetRGB(30, 30,  0));	/* Yellow */
	TPALET2( 7, SetRGB( 0, 31,  0));	/* Green */
	
	stTextInfo.uScoreMax = 10000;
}

void T_EXIT(void)
{
	B_CURON();	/* カーソルを表示します */
	T_Clear();	/* テキストクリア */
}

void T_SetBG_to_Text(void)
{
	SS	i;
	US	x, y;

	/* テキストエリアに作業データ展開1 */
	BG_TextPut("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 232);
	/* テキストエリアに作業データ展開2 */
	for(i=0; i < 10; i++)
	{
		x = 0;
		y = 240;
		BG_PutToText(   0x80+ (i<<1) + 0, x + BG_WIDTH * i,	y,				BG_Normal, TRUE);	/* 数字大（上側）*/
		BG_PutToText(   0x80+ (i<<1) + 1, x + BG_WIDTH * i,	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 数字大（下側）*/
	}
}

void T_Clear(void)
{
	_iocs_txrascpy(256 / 4 * 256, 256 / 4, 0b1111);	/* テキスト画面クリア */
}

void T_TopScore(void)
{
	UI	e;
	/* TOP */
	BG_PutToText(  1, (BG_WIDTH * 0),  0, BG_Normal, FALSE);	/* 左上 */
	BG_PutToText(  2, (BG_WIDTH * 1),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 2),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 3),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  3, (BG_WIDTH * 4),  0, BG_Normal, FALSE);	/* 右上 */
	for(e = 0; e < 5; e++)
	{
		BG_PutToText(e+4, (BG_WIDTH * e),  8, BG_Normal, FALSE);
	}
	BG_PutToText(  1, (BG_WIDTH * 0),  16, BG_H_rev, FALSE);	/* 左下 */
	BG_PutToText(  2, (BG_WIDTH * 1),  16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 2),  16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 3),  16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  3, (BG_WIDTH * 4),  16, BG_H_rev, FALSE);	/* 右下 */
}

void T_Time(void)
{
	UI	e;
	/* TIME */
	BG_PutToText(  1, (BG_WIDTH * 12),  0, BG_Normal, FALSE);	/* 左上 */
	BG_PutToText(  2, (BG_WIDTH * 13),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 14),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 15),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 16),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  3, (BG_WIDTH * 17),  0, BG_Normal, FALSE);	/* 右上 */
	BG_PutToText(  4, (BG_WIDTH * 12),  8, BG_Normal, FALSE);	/* 左 */
	for(e = 0; e < 5; e++)
	{
		BG_PutToText(e+10, (BG_WIDTH * (13 + e)),  8, BG_Normal, FALSE);
	}
	BG_PutToText(  1, (BG_WIDTH * 12), 16, BG_H_rev, FALSE);	/* 左下 */
	BG_PutToText(  2, (BG_WIDTH * 13), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 14), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 15), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 16), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  3, (BG_WIDTH * 17), 16, BG_H_rev, FALSE);	/* 右下 */

}

void T_Score(void)
{
	UI	e;
	/* SCORE */
	BG_PutToText(  1, (BG_WIDTH * 17),  0, BG_Normal, FALSE);	/* 左上 */
	BG_PutToText(  2, (BG_WIDTH * 18),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 19),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 20),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 21),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText(  2, (BG_WIDTH * 22),  0, BG_Normal, FALSE);	/* 上 */
	BG_PutToText( 15, (BG_WIDTH * 23),  0, BG_Normal, FALSE);	/* 右上 */
	BG_PutToText(  4, (BG_WIDTH * 17),  8, BG_Normal, FALSE);	/* 左 */
	for(e = 0; e < 6; e++)
	{
		BG_PutToText(e+16, (BG_WIDTH * (18 + e)),  8, BG_Normal, FALSE);
	}
	BG_PutToText(  1, (BG_WIDTH * 17), 16, BG_H_rev, FALSE);	/* 左下 */
	BG_PutToText(  2, (BG_WIDTH * 18), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 19), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 20), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 21), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText(  2, (BG_WIDTH * 22), 16, BG_H_rev, FALSE);	/* 下 */
	BG_PutToText( 15, (BG_WIDTH * 23), 16, BG_H_rev, FALSE);	/* 右下 */

}

void T_Speed(void)
{
	/* SPEED */
	BG_TextPut("SPEED", 164, 24);
	/* KM */
	BG_TextPut("KM", 232, 24);
}

void T_Gear(void)
{
	/* GEAR */
	BG_TextPut("GEAR", 172, 32);
}

void T_Main(void)
{
	UI time_now;
	UI unStart_time, unPassTime, unTimer;
	US uTimeCounter;

	ST_CARDATA	stMyCar = {0};
	GetMyCar(&stMyCar);			/* 自車の情報を取得 */

	/* 開始時間 */
	GetStartTime(&unStart_time);
	
	/* Score */
	stTextInfo.uScore = 0;

	/* Top Score */
	stTextInfo.uScoreMax = Mmax(stTextInfo.uScore, stTextInfo.uScoreMax);
	
	/* Time Count */
	GetNowTime(&time_now);
	unPassTime = (time_now - unStart_time);
	unTimer = 120000 - unPassTime;
	if(120000 < unTimer)
	{
		uTimeCounter = 0;
	}
	else
	{
		uTimeCounter = (US)(unTimer / 1000);
	}
	stTextInfo.uTimeCounter = uTimeCounter;
	
	/* Speed */
	stTextInfo.uVs = stMyCar.VehicleSpeed;
	
	/* Gear */
	stTextInfo.uShiftPos = (US)stMyCar.ubShiftPos;

	/* 描画 */
	PutTextInfo(stTextInfo);
}

#endif	/* TEXT_C */

