#ifndef	SCORE_C
#define	SCORE_C

#include <stdio.h>

#include "inc/usr_macro.h"
#include "Score.h"

#include "Music.h"
#include "PCG.h"

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
static 	uint8_t		g_bSetScore = FALSE;
static 	uint16_t	g_uScoreNum;
static 	uint16_t	g_uPassCarNum;

/* 構造体定義 */

/* 関数のプロトタイプ宣言 */
int16_t S_Get_ScoreInfo(ST_SCORE *);
int16_t S_Set_ScoreInfo(ST_SCORE);
int16_t S_Add_Score(void);
int16_t S_Add_Score_Point(uint64_t);
int16_t S_Reset_ScoreID(void);
int16_t S_All_Init_Score(void);
int16_t S_Init_Score(void);
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
	ADPCM_Play(18);	/* SE：PON */
	
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

#if 0
	x = 128;
	y = 160;
#else
	x = 176;
	y = 128;
#endif
	
	for(i = SCORE_PCG_1; i <= SCORE_PCG_4; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* スコア */
		if(p_stPCG != NULL)
		{
			p_stPCG->Anime = 0;
			p_stPCG->x = SP_X_OFFSET + x - (Mdiv2(SP_W) * (i - SCORE_PCG_1));
			p_stPCG->y = SP_Y_OFFSET + y;
			p_stPCG->update	= TRUE;
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
int16_t S_Main_Score(void)
{
	int16_t	ret = 0;
	int16_t	x, y;
	int16_t	i;
	int16_t	digit=0;
	uint16_t	number;

	static uint8_t bJump = FALSE;
	static int16_t uAcc;

	ST_PCG	*p_stPCG = NULL;
	
#if 0
	x = 128;
	y = 160;
#else
	x = 176;
	y = 128;
#endif
	
	if(bJump == FALSE)
	{
		if(g_bSetScore == TRUE)
		{
			uAcc = -18;
			bJump = TRUE;
			
			for(i = SCORE_PCG_1; i <= SCORE_PCG_4; i++)
			{
				p_stPCG = PCG_Get_Info(i);	/* スコア */
				if(p_stPCG != NULL)
				{
					p_stPCG->x = SP_X_OFFSET + x - (Mdiv2(SP_W) * (i - SCORE_PCG_1));
					p_stPCG->y = y;
					p_stPCG->update	= TRUE;
				}
			}
		}
	}
	
	if(bJump == TRUE)
	{
		uAcc += 3;
	}
	number = g_uPassCarNum;
	
	do{
		number = number / 10;
		digit++;
	}while(number!=0);
	
	number = g_uPassCarNum;
	
	for(i = SCORE_PCG_1; i <= SCORE_PCG_4; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* スコア */
		if(p_stPCG != NULL)
		{
			p_stPCG->Anime = number % 10;
			number /= 10;
			
			if(digit > (i - SCORE_PCG_1))	/* 表示桁内 */
			{
#if 0
				p_stPCG->x = SP_X_OFFSET + x - (Mdiv2(SP_W) * (i - SCORE_PCG_1));
				p_stPCG->y += uAcc;
#else
				p_stPCG->x += uAcc;
				p_stPCG->y = y;
#endif
				p_stPCG->update	= TRUE;
			}
			else
			{
#if 0
				p_stPCG->x = SP_X_OFFSET + x - (Mdiv2(SP_W) * (i - SCORE_PCG_1));
				p_stPCG->y = SP_Y_OFFSET + y;
#else
				x = 128;
				y = 160;
				p_stPCG->x = x;
				p_stPCG->y = y;
#endif
				p_stPCG->update	= TRUE;
			}

#if 0
			if(p_stPCG->y > (SP_Y_OFFSET + y))
			{
				p_stPCG->y = SP_Y_OFFSET + y;
				if(i == SCORE_PCG_1)
				{
					bJump = FALSE;
					g_bSetScore = FALSE;
					p_stPCG->update	= FALSE;
				}
			}
#else
			if(p_stPCG->x > (SP_X_OFFSET + x - (Mdiv2(SP_W) * (i - SCORE_PCG_1))))
			{
				p_stPCG->x = SP_X_OFFSET + x - (Mdiv2(SP_W) * (i - SCORE_PCG_1));
				if(i == SCORE_PCG_1)
				{
					bJump = FALSE;
					g_bSetScore = FALSE;
					p_stPCG->update	= FALSE;
				}
			}
#endif
		}
	}
	
	return ret;
}
#endif	/* SCORE_C */

