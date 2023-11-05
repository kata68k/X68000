#ifndef	SCORE_C
#define	SCORE_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usr_macro.h>
#include "APL_Score.h"
#include "IF_Draw.h"
#include "IF_Graphic.h"
#include "IF_Text.h"
#include "IF_Music.h"
#include "BIOS_PCG.h"

/* データ */
uint32_t	g_uScoreTable[8] = {
	10,		/* 0 */
	20,		/* 1 */
	40,		/* 2 */
	80,		/* 3 */
	160,	/* 4 */
	320,	/* 5 */
	640,	/* 6 */
	1280	/* 7 */
};

/* グローバル変数 */
static 	ST_SCORE	g_stScore = {0};
static 	ST_SCORE_POS	g_stScorePos = {0};
static 	uint8_t		g_bSetScore = FALSE;
static 	uint16_t	g_uScoreNum = 0u;
static 	uint16_t	g_uPassCarNum = 0u;
/* 構造体定義 */

/* 関数のプロトタイプ宣言 */
int16_t S_Get_ScoreInfo(ST_SCORE *);
int16_t S_Set_ScoreInfo(ST_SCORE);
int16_t S_Add_Score(void);
int16_t S_Add_Score_Point(uint64_t);
int16_t S_Set_Combo(int16_t);
int16_t S_Reset_ScoreID(void);
int16_t S_All_Init_Score(void);
int16_t S_Init_Score(void);
int16_t S_GetPos(int16_t, int16_t);
int16_t S_Clear_Score(void);
int16_t S_Main_Score(void);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Get_ScoreInfo(ST_SCORE *p_stScore)
{
	int16_t	ret = 0;

	*p_stScore = g_stScore;	/* 現情報を取得 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Set_ScoreInfo(ST_SCORE stScore)
{
	int16_t	ret = 0;

	g_stScore = stScore;	/* 更新 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Add_Score(void)
{
	int16_t	ret = 0;
	uint64_t	val64;

	val64 = g_stScore.ulScore;
	
	g_uScoreNum = g_uScoreTable[g_stScore.ubScoreID];
	g_uPassCarNum++;
	g_bSetScore = TRUE;

	val64 += g_uScoreNum;	/* 加算 */
	
	/* 現在の点数 */
	if(g_stScore.ulScore > val64)	/* オーバーフロー対策 */
	{
		g_stScore.ulScore = -1;	/* カンスト */
	}
	else
	{
		g_stScore.ulScore = val64;	/* 更新 */
	}

	/* 最高の点数 */
	g_stScore.ulScoreMax = Mmax(g_stScore.ulScoreMax, g_stScore.ulScore);
	
	/* スコアIDの加算 */
	if(g_stScore.ubScoreID >= 7)
	{
		/* 何もしない */
	}
	else
	{
		g_stScore.ubScoreID++;
	}
	//ADPCM_Play(18);	/* SE：PON */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Add_Score_Point(uint64_t ulNum)
{
	int16_t	ret = 0;
	uint64_t	val64;

	val64 = g_stScore.ulScore;
	
	val64 += ulNum;	/* 加算 */
	
	/* 現在の点数 */
	if(g_stScore.ulScore > val64)	/* オーバーフロー対策 */
	{
		g_stScore.ulScore = -1;	/* カンスト */
	}
	else
	{
		g_stScore.ulScore = val64;	/* 更新 */
	}

	/* 最高の点数 */
	g_stScore.ulScoreMax = Mmax(g_stScore.ulScoreMax, g_stScore.ulScore);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Set_Combo(int16_t uNum)
{
	int16_t	ret = 0;

	g_stScore.uCombo = uNum;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Reset_ScoreID(void)
{
	int16_t	ret = 0;

	g_stScore.ubScoreID = 0;	/* リセット */
	g_uPassCarNum = 0;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_All_Init_Score(void)
{
	int16_t	ret = 0;
	int16_t	x, y;
	int16_t	i;
	ST_PCG	*p_stPCG = NULL;

	/* リセット */
	g_stScore.ulScore		= 0;
	g_stScore.ubScoreID		= 0;
	g_stScore.ulScoreMax	= 10000;

	g_uScoreNum = 0;
	g_uPassCarNum = 0;
	g_bSetScore = FALSE;

	x = 176;
	y = 128;
	
	for(i = 0; i < 4; i++)
	{
		p_stPCG = PCG_Get_Info(SCORE_PCG_1 + i);	/* スコア */
		if(p_stPCG != NULL)
		{
			p_stPCG->Anime = 0;
			p_stPCG->x = SP_X_OFFSET + x - (Mdiv2(SP_W) * i);
			p_stPCG->y = SP_Y_OFFSET + y;
			p_stPCG->update	= FALSE;
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
int16_t S_Init_Score(void)
{
	int16_t	ret = 0;

	/* リセット */
	g_stScore.ulScore		= 0;
	g_stScore.ubScoreID		= 0;

	g_uPassCarNum = 0;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_GetPos(int16_t x, int16_t y)
{
	int16_t	ret = 0;

	g_stScorePos.x_old = g_stScorePos.x;
	g_stScorePos.y_old = g_stScorePos.y;

	if(x > 200)x = 200;
	if(y < 80)y = 80;

	g_stScorePos.x = x;
	g_stScorePos.y = y;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Clear_Score(void)
{
	int16_t	ret = 0;
	int8_t	sBuf[128];

//	Draw_Fill(g_stScorePos.x_old, g_stScorePos.y_old+1, g_stScorePos.x_old + 48, g_stScorePos.y_old + 11, 0);
	G_CLR_AREA(0, 255, 0, 255, 0);	/* ページ 0 */

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "STAGE CLEAR!!");
	PutGraphic_To_Symbol24(sBuf, 48 + 1, 32 + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, 48, 32, F_MOJI);
	
	/* 獲得したスコア */
	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "Clear Bonus +%d x %d", g_stScore.ulScoreAdd, g_stScore.uCombo);
	Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);

	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "%d", g_stScore.ulScore);
	PutGraphic_To_Symbol24(sBuf, g_stScorePos.x + 32 + 1, g_stScorePos.y + 24 + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, g_stScorePos.x + 32, g_stScorePos.y + 24, F_MOJI);
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Main_Score(void)
{
	int16_t	ret = 0;
	int16_t	x, y;
	int16_t	i;
	int16_t	digit=0;
	uint16_t	number;

	static uint8_t bJump = FALSE;

	ST_PCG	*p_stPCG = NULL;
	
	x = 176;
	y = 128;
	
	if(bJump == FALSE)
	{
		if(g_bSetScore == TRUE)
		{
			bJump = TRUE;
			
			for(i = 0; i < 4; i++)
			{
				p_stPCG = PCG_Get_Info(SCORE_PCG_1 + i);	/* スコア */
				if(p_stPCG != NULL)
				{
					p_stPCG->dx = -18;
					p_stPCG->x = SP_X_OFFSET + x - (Mdiv2(SP_W) * i);
					p_stPCG->y = y;
					p_stPCG->update	= TRUE;
				}
			}
		}
	}
	
	if(bJump == TRUE)
	{
		for(i = 0; i < 4; i++)
		{
			p_stPCG = PCG_Get_Info(SCORE_PCG_1 + i);	/* スコア */
			if(p_stPCG != NULL)
			{
				p_stPCG->dx += 3;
			}
		}
	}
	number = g_uPassCarNum;
	
	do{
		number = number / 10;
		digit++;
	}while(number!=0);
	
	number = g_uPassCarNum;
	
	for(i = 0; i < 4; i++)
	{
		p_stPCG = PCG_Get_Info(SCORE_PCG_1 + i);	/* スコア */
		if(p_stPCG != NULL)
		{
			int16_t	pos;

			p_stPCG->Anime = number % 10;
			number /= 10;
			
			if(digit > i)	/* 表示桁内 */
			{
				p_stPCG->x += p_stPCG->dx;
				p_stPCG->y = y;
			}
			else
			{
				p_stPCG->x = 0;
				p_stPCG->y = 0;
			}
			p_stPCG->update	= TRUE;
			
			pos = SP_X_OFFSET + x - (Mdiv2(SP_W) * i);	/* 閾値 */

			if(p_stPCG->x > pos)
			{
				p_stPCG->x = pos;
				if(i == 0)
				{
					bJump = FALSE;
					g_bSetScore = FALSE;
				}
			}
		}
	}
	
	return ret;
}
#endif	/* SCORE_C */

