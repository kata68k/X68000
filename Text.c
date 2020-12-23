#ifndef	TEXT_C
#define	TEXT_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "Text.h"

#include "MFP.h"
#include "MyCar.h"
#include "OutPut_Text.h"

/* 構造体 */
static ST_TEXTINFO	g_stTextInfo = {0};

/* 関数のプロトタイプ宣言 */
void T_INIT(void);
void T_EXIT(void);
void T_Clear(void);
void T_PALET(void);
void T_SetBG_to_Text(void);
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
	
	g_stTextInfo.uScoreMax = 10000;
}

void T_EXIT(void)
{
	B_CURON();	/* カーソルを表示します */
	T_Clear();	/* テキストクリア */
}

void T_Clear(void)
{
	struct _txfillptr stTxFill;
	
	stTxFill.vram_page = 0;
	stTxFill.x = 0;
	stTxFill.y = 0;
	stTxFill.x1= 1023;
	stTxFill.y1= 1023;
	stTxFill.fill_patn = 0;
	
	_iocs_scroll(8, 0, 0);	/* テキスト画面 */
//	_iocs_txrascpy(256 / 4 * 256, 256 / 4, 0b1111);	/* テキスト画面クリア */
	_iocs_txfill(&stTxFill);
	stTxFill.vram_page = 1;
	_iocs_txfill(&stTxFill);
	stTxFill.vram_page = 2;
	_iocs_txfill(&stTxFill);
	stTxFill.vram_page = 3;
	_iocs_txfill(&stTxFill);
}

void T_PALET(void)
{
	/* テキストパレットの初期化(Pal0はSPと共通) */
	TPALET2( 0, SetRGB( 0,  0,  0));	/* Black */
	TPALET2( 1, SetRGB( 1,  0,  0));	/* Black2 */
	TPALET2( 2, SetRGB(31,  0,  0));	/* Red */
	TPALET2( 3, SetRGB(30, 26, 16));	/* Red2 */
	TPALET2( 4, SetRGB(31, 31, 31));	/* White */
	TPALET2( 5, SetRGB(30,  8,  0));	/* Orenge */
	TPALET2( 6, SetRGB(30, 30,  0));	/* Yellow */
	TPALET2( 7, SetRGB( 0, 31,  0));	/* Green */
	TPALET2( 8, SetRGB( 8,  8,  8));	/* Glay */
}

void T_SetBG_to_Text(void)
{
	SS	i;
	US	x, y;

	/* テキストエリアに作業データ展開1 */
	BG_TextPut("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 240);//232
	/* テキストエリアに作業データ展開2 */
	for(i=0; i < 10; i++)
	{
		x = 0;
		y = 248;//240	248+8=256以降はNG
		BG_PutToText(   0x80+ (i<<1) + 0, x + BG_WIDTH * i,	y,				BG_Normal, TRUE);	/* 数字大（上側）*/
		BG_PutToText(   0x80+ (i<<1) + 1, x + BG_WIDTH * i,	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 数字大（下側）*/
	}
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
	static UI time_old = 0;
	UI unStart_time, unTimer;
	US uTimeCounter;
	static UI unPassTime = 0;

	ST_CARDATA	stMyCar = {0};
	GetMyCar(&stMyCar);			/* 自車の情報を取得 */

	/* 現在時間 */
	GetNowTime(&time_now);
	/* 開始時間 */
	GetStartTime(&unStart_time);
	
	/* Time Count */
	if(time_old != 0)
	{
		unPassTime += (unStart_time - time_old);
		unTimer = 120000 - unPassTime;
		if(120000 < unTimer)
		{
			uTimeCounter = 0;
		}
		else
		{
			uTimeCounter = (US)(unTimer / 1000);
		}
		g_stTextInfo.uTimeCounter = uTimeCounter;
	}
	time_old = unStart_time;
	
	/* Score */
	g_stTextInfo.uScore = unPassTime / 1000;

	/* Top Score */
	g_stTextInfo.uScoreMax = Mmax(g_stTextInfo.uScore, g_stTextInfo.uScoreMax);
	
	/* Speed */
	g_stTextInfo.uVs = stMyCar.VehicleSpeed;
	
	/* Gear */
	g_stTextInfo.uShiftPos = (US)stMyCar.ubShiftPos;

	/* 描画 */
	PutTextInfo(g_stTextInfo);
}

#endif	/* TEXT_C */

