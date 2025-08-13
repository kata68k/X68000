#ifndef	SCORE_C
#define	SCORE_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usr_macro.h>
#include "BIOS_MFP.h"
#include "APL_Score.h"
#include "IF_Draw.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"
#include "IF_Input.h"
#include "IF_Math.h"
#include "IF_Mouse.h"
#include "IF_Music.h"
#include "IF_PCG.h"
#include "IF_Text.h"

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

uint32_t	g_uSymbolTable[6] = {'.','*','/','-','+',' '};

/* グローバル変数 */
static 	ST_SCORE		g_stScore = {0};
static 	ST_SCORE_POS	g_stScorePos = {0};
static	int8_t	g_bUpdateScore[3];
static	uint32_t g_uScoreRanking[10] = {0};
static	int8_t  g_bNameRanking[10][5] = {0};
static	uint16_t g_ComboCount_Old;
static	int8_t  s_bScoreMode = TRUE;

#if COMBO_IS_TIMEOUT
static	uint32_t g_ComboTime = PASSTIME_INIT;
#endif

/* 構造体定義 */

/* 関数のプロトタイプ宣言 */
int16_t S_Get_ScoreInfo(ST_SCORE *);
int16_t S_Set_ScoreInfo(ST_SCORE);
int16_t S_Set_ScoreMode(int8_t);
int16_t S_Add_Score(uint64_t);
int16_t S_Add_Score_Point(uint64_t);
int16_t S_Set_Combo(int16_t);
int16_t S_Get_Combo(void);
int16_t S_IsUpdate_Combo(void);
int16_t S_All_Init_Score(void);
int16_t S_Init_Score(void);
int16_t S_GetPos(int16_t, int16_t);
int16_t S_Clear_Score(void);
int16_t S_Main_Score(void);
int16_t S_Score_Board(void);
static int16_t S_Update_Score_Board(void);
int16_t S_Score_NameInput(int16_t, int8_t, int8_t);
int16_t S_Score_Name_Main(int16_t);

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
int16_t S_Set_ScoreMode(int8_t bFlag)
{
	s_bScoreMode = bFlag;

	return (int16_t)s_bScoreMode;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Get_ScoreMode(void)
{
	return (int16_t)s_bScoreMode;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Add_Score(uint64_t ulNum)
{
	int16_t	ret = 0;
	uint64_t	val64;
	
	if(s_bScoreMode == FALSE)return ret;

	val64 = g_stScore.ulScore;

//	ret = S_Set_Combo(g_stScore.uCombo + 1);

	g_stScore.ulScoreAdd = ulNum;
	val64 += g_stScore.ulScoreAdd * g_stScore.uCombo;	/* 加算 */
	
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
	
	g_bUpdateScore[0] = TRUE;	/* 更新 */
	g_bUpdateScore[1] = TRUE;	/* 更新 */

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

	if(s_bScoreMode == FALSE)return ret;

	val64 = g_stScore.ulScore;
	
	g_stScore.ulScoreAdd = ulNum;

	val64 += g_stScore.ulScoreAdd;	/* 加算 */
	
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
	
	g_bUpdateScore[0] = TRUE;	/* 更新 */
	g_bUpdateScore[2] = TRUE;	/* 更新 */

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

	if(uNum != 0)
	{
#if COMBO_IS_TIMEOUT
		GetNowTime(&g_ComboTime);	/* コンボタイムアウトカウンタリセットで延長 */
#endif
	}
	
	g_ComboCount_Old = g_stScore.uCombo;	/* 前回値 */

	g_stScore.uCombo = uNum;

	g_stScore.uCombo = Mmin(g_stScore.uCombo, 255);	/* コンボ加算 */
	g_stScore.uComboMax = Mmax(g_stScore.uCombo, g_stScore.uComboMax);

	ret = g_stScore.uCombo;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Get_Combo(void)
{
	return g_stScore.uCombo;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_IsUpdate_Combo(void)
{
	int16_t	ret = 0;

	ret = (g_stScore.uCombo > g_ComboCount_Old);

	g_ComboCount_Old = g_stScore.uCombo;	/* 前回値 */

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

	int16_t	i;

	/* リセット */
	g_stScore.ulScore		= 0;
	g_stScore.uCombo		= 0;
	g_stScore.uComboMax 	= 0;

	g_bUpdateScore[0] = TRUE;	/* 更新 */

	if(S_Score_Load() < 0)	/* スコアデータの読み出し */
	{
		/* 不一致の場合初期化 */

		g_stScore.ulScoreMax	= 5000;

		for(i=0; i < 10; i++)
		{
			g_uScoreRanking[i] = g_stScore.ulScoreMax - (i * 500);
			sprintf(g_bNameRanking[i], "%c%c%c", 'A'+i, 'A'+i, 'A'+i); 
		}

		S_Score_Save();	/* セーブ */
	}
	else
	{
		g_stScore.ulScoreMax	= g_uScoreRanking[0];
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

	int8_t	sBuf[32];

	/* リセット */
	g_stScore.ulScore		= 0;
	g_stScore.uCombo		= 0;
	g_stScore.uComboMax 	= 0;
	g_stScorePos.x = 32;
	g_stScorePos.y = 16;
	g_stScorePos.x_old = g_stScorePos.x;
	g_stScorePos.y_old = g_stScorePos.y;

	g_bUpdateScore[0] = TRUE;	/* 更新 */
	/* スコア */
	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "SCORE");
#ifdef SC_GR	
	Draw_Message_To_Graphic(sBuf, 0, 0, G_L_BLUE, G_BLUE);	/* スコア表示 */
#endif
	BG_put_String(0, 0, BG_PAL, sBuf, strlen(sBuf));

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "HI-SCORE");
#ifdef SC_GR	
	sprintf(sBuf, "Score %10d   Hi-Score %10d", g_stScore.ulScore, g_stScore.ulScoreMax);
	Draw_Message_To_Graphic(sBuf, 0, 0, F_MOJI, F_MOJI_BAK);	/* スコア表示 */
#endif
	BG_put_String(128, 0, BG_PAL, sBuf, strlen(sBuf));

#if 0
	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "NEXT");
#ifdef SC_GR	
	Draw_Message_To_Graphic(sBuf, 0, 0, G_L_BLUE, G_BLUE);	/* スコア表示 */
#endif
	BG_put_String(192, 16, BG_PAL, sBuf, strlen(sBuf));
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
int16_t S_GetPos(int16_t x, int16_t y)
{
	int16_t	ret = 0;
	int8_t	sBuf[32];

	g_stScorePos.x_old = g_stScorePos.x;
	g_stScorePos.y_old = g_stScorePos.y;
	g_stScorePos.x = x;
	g_stScorePos.y = y;

	/* クリア処理 */
#ifdef SC_GR	
	G_CLR_AREA(g_stScorePos.x_old, 64, g_stScorePos.y_old, 12, 0);	/* ページ 0 */
#endif
	memset(sBuf, 0x20, sizeof(sBuf));
	sBuf[31] = 0x00;
	BG_put_String(g_stScorePos.x_old, g_stScorePos.y_old, BG_PAL, sBuf, strlen(sBuf));

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
#ifdef SC_GR	
	int8_t	sBuf[128];
#endif

	S_GetPos(32, 8);	/* スコア表示座標更新 */
	S_Add_Score_Point(g_stScore.uComboMax);
	
	S_Add_Score(10000);	/* スコア更新 */
//	Draw_Fill(g_stScorePos.x_old, g_stScorePos.y_old+1, g_stScorePos.x_old + 48, g_stScorePos.y_old + 11, 0);
	S_Main_Score();		/* スコア更新 */

#ifdef SC_GR	
	G_CLR_AREA(0, 255, 16, 255, 0);	/* ページ 0 */

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "STAGE CLEAR!!");
	PutGraphic_To_Symbol24(sBuf, 48 + 1, 32 + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, 48, 32, F_MOJI);
	
	/* 獲得したスコア */
	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "Clear Bonus +%d x %d", g_stScore.ulScoreAdd, g_stScore.uCombo);	/* ライフカウンタ(コンボじゃない) */
	Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "Max Combo Bonus +%d", g_stScore.uComboMax);	/* 最大コンボ */
	Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y + 12, F_MOJI, F_MOJI_BAK);

	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "%d", g_stScore.ulScore);
	PutGraphic_To_Symbol24(sBuf, g_stScorePos.x + 32 + 1, g_stScorePos.y + 32 + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, g_stScorePos.x + 32, g_stScorePos.y + 32, F_MOJI);
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
int16_t S_Main_Score(void)
{
	int16_t	ret = 0;

//	printf("S_Main_Score st(%d)\n", g_index);
#if COMBO_IS_TIMEOUT
	if(	GetPassTime(COMBO_TIMEOUT, &g_ComboTime) != 0u)	/* 1000ms経過 */
	{
		S_Set_Combo(0);	/* コンボカウンタ リセット */
	}
#endif
	if(g_bUpdateScore[0] == TRUE)
	{
		int8_t	sBuf[32];

		g_bUpdateScore[0] = FALSE;

		S_GetPos(48, 8);     /* 追加点表示位置 */

		if(g_bUpdateScore[1] == TRUE)
		{
			g_bUpdateScore[1] = FALSE;


			/* 獲得したスコア */
			memset(sBuf, 0, sizeof(sBuf));
			if(g_stScore.uCombo > 1)
			{
				sprintf(sBuf, "+%8d", g_stScore.ulScoreAdd * g_stScore.uCombo);
			}
			else
			{
				sprintf(sBuf, "+%8d", g_stScore.ulScoreAdd );
			}
#ifdef SC_GR	
			Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
#endif
			BG_put_String(g_stScorePos.x, g_stScorePos.y, BG_PAL, sBuf, strlen(sBuf));
		}

		if(g_bUpdateScore[2] == TRUE)
		{
			g_bUpdateScore[2] = FALSE;

			/* 獲得したスコア */
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "+%8d", g_stScore.ulScoreAdd );
#ifdef SC_GR	
			Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
#endif
			BG_put_String(g_stScorePos.x, g_stScorePos.y, BG_PAL, sBuf, strlen(sBuf));
		}

		/* スコア */
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%8d", g_stScore.ulScore);
#ifdef SC_GR
		Draw_Message_To_Graphic(sBuf, 32, 0, G_L_BLUE, G_BLUE);	/* スコア表示 */
#endif
		BG_put_String(56, 0, BG_PAL, sBuf, strlen(sBuf));
//		BG_put_Number10(48, 0, 2, g_stScore.ulScore);

		/* ハイスコア */
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%8d", g_stScore.ulScoreMax);
#ifdef SC_GR
		Draw_Message_To_Graphic(sBuf, 168, 0, F_MOJI, F_MOJI_BAK);	/* スコア表示 */
#endif
		BG_put_String(192, 0, BG_PAL, sBuf, strlen(sBuf));
//		BG_put_Number10(192, 0, 2, g_stScore.ulScoreMax);

	}

//	printf("S_Main_Score ed(%d)\n", g_index);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Score_Board(void)
{
	int16_t	ret = 0;

	int16_t	i;
	int16_t x, y;

	int8_t	sBuf[128];

	x = SCORE_BOARD_X;
	y = NAME_INP_Y;

	ret = S_Update_Score_Board();

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "No.   Score   Name");
	PutGraphic_To_Symbol24(sBuf, x + 1, y + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, x,     y, F_MOJI);

	x = NAME_INP_X;

	for(i=0; i < 8; i++)
	{
		y = NAME_INP_Y + ((i+1) * 24);

		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%d %9d   %s", i + 1, g_uScoreRanking[i], g_bNameRanking[i]);

		PutGraphic_To_Symbol24(sBuf, x + 1, y + 1, F_MOJI_BAK);
		if(i == ret)
		{
			PutGraphic_To_Symbol24(sBuf, x,     y, G_RED);
		}
		else
		{
			PutGraphic_To_Symbol24(sBuf, x,     y, F_MOJI);
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
static int16_t S_Update_Score_Board(void)
{
	int16_t	ret = 0;
	int16_t	i;
	int16_t	rank;

	rank = -1;

	for(i=0; i<8; i++)
	{
		if(g_uScoreRanking[i] < g_stScore.ulScore)
		{
			rank = i;
			break;
		}
	}

	if(rank != -1)
	{
		for(i=7; i>=rank; i--)
		{
			g_uScoreRanking[i+1] = g_uScoreRanking[i];		/* ズレてもらう */
			strcpy(g_bNameRanking[i+1], g_bNameRanking[i]);
		}
		g_uScoreRanking[rank] = g_stScore.ulScore;		/* 更新 */
		strcpy(g_bNameRanking[rank], "You");
	}
	ret = rank;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Score_NameInput(int16_t nRank, int8_t bInput_c, int8_t bMoji)
{
	int16_t	ret = 0;

	int16_t	i;
	int16_t x, y;

	int8_t	sBuf[8];

	
	i = nRank;

	x = 184 + (bInput_c * 12);
	y = NAME_INP_Y + ((i+1) * 24);

	G_CLR_AREA(x, 12, y, 24, 0);	/* ページ 0 */

	
	if((bMoji- 'A') < 26)
	{
		g_bNameRanking[i][bInput_c] = bMoji & 0xFF;
	}
	else
	{
		g_bNameRanking[i][bInput_c] = g_uSymbolTable[(bMoji - 26) % 6];
	}
	g_bNameRanking[i][3] = 0x00;


	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "%c", g_bNameRanking[i][bInput_c]);

	PutGraphic_To_Symbol24(sBuf, x + 1, y + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, x,     y, G_RED);


	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Score_Save(void)
{
	int16_t	ret = 0;
	uint32_t crc;
	int8_t	sBuf[94];
	size_t dataLength;
	/* score + name + crc */
	/* (4 * 10) + (1 * 10 * 5) + 4 */
	dataLength = sizeof(g_uScoreRanking) / sizeof(g_uScoreRanking[0]);

	crc = APL_calculateCRC((uint8_t *)&g_uScoreRanking[0], dataLength);
	
	memcpy(&sBuf[0], g_uScoreRanking, 40);
	memcpy(&sBuf[40], g_bNameRanking, 50);
	memcpy(&sBuf[90], &crc, 4);

	dataLength = sizeof(sBuf) / sizeof(sBuf[0]);

	File_Save_OverWrite("data/score.dat", sBuf, sizeof(int8_t), dataLength);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t S_Score_Load(void)
{
	int16_t	ret = 0;
	uint32_t crc;
	int8_t	sBuf[94];
	size_t dataLength;

	dataLength = sizeof(sBuf) / sizeof(sBuf[0]);

	if(File_Load("data/score.dat", sBuf, sizeof(int8_t), dataLength) < 0)
	{
		S_Score_Save();
		return ret;
	}

	memcpy(g_uScoreRanking, &sBuf[0], 40);
	memcpy(g_bNameRanking, &sBuf[40], 50);
	memcpy(&crc, &sBuf[90], 4);

	/* score + name + crc */
	/* (4 * 10) + (1 * 10 * 5) + 4 */
	dataLength = sizeof(g_uScoreRanking) / sizeof(g_uScoreRanking[0]);

	if(crc != APL_calculateCRC((uint8_t *)&g_uScoreRanking[0], dataLength))
	{
		ret = -1;
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
int16_t S_Score_Name_Main(int16_t rank)
{
	int16_t	ret = 0;
	static int16_t nCol = 0;
	static int8_t bInput_c = 0;
	static int8_t bMoji = 0;
	static int8_t bFlagInput = FALSE;
	static int8_t bFlagInputAB = FALSE;
	int16_t nColSin, nColCos;
	
    int8_t ms_x;
    int8_t ms_y;
    int8_t ms_left;
    int8_t ms_right;
    int32_t ms_pos_x;
    int32_t ms_pos_y;

    /* マウス操作 */
    Mouse_GetDataPos(&ms_x, &ms_y, &ms_left, &ms_right);

    Mouse_GetPos(&ms_pos_x, &ms_pos_y);

    if(ms_left != 0)
    {
        SetInput(KEY_b_Z);
    }
    if(ms_right != 0)
    {
        SetInput(KEY_b_X);
    }


	nColSin = Mdiv256(31 * APL_Sin(nCol++));
	nColCos = Mdiv256(31 * APL_Cos(nCol++));
	G_PaletteSet(G_RED, SetRGB(nColCos, nColSin, nColCos));	/* パレットアニメ */
	
    if(	((GetInput_P1() & JOY_UP ) != 0u )	||	/* UP */
        ((GetInput() & KEY_UPPER) != 0 )	)	/* 上 */
	{
		if(bFlagInput == FALSE)
		{
			ADPCM_Play(SE_SELECT);	/* SELECT音 */
			bFlagInput = TRUE;
			bMoji++;
			if(bMoji > 26 + 6)
			{
				bMoji = 0;
			}
		}
	}
	else if(((GetInput_P1() & JOY_DOWN ) != 0u )	||	/* UP */
       		((GetInput() & KEY_LOWER) != 0 )	)		/* 下 */
	{
		if(bFlagInput == FALSE)
		{
			ADPCM_Play(SE_SELECT);	/* SELECT音 */
			bFlagInput = TRUE;
			bMoji--;
			if(bMoji < 0)
			{
				bMoji = 26 + 6;
			}
		}
	}
	else /* なし */
	{
		bFlagInput = FALSE;

		if((ms_x != 0) || (ms_y != 0))
        {
			bMoji = Mmax(0, Mmin(26 + 6, ms_pos_y / 8));
        }
	}

    if(	((GetInput_P1() & JOY_A ) != 0u)	||	/* A */
        ((GetInput() & KEY_b_Z  ) != 0u)	||	/* A(z) */
        ((GetInput() & KEY_b_SP ) != 0u)	)	/* スペースキー */
	{
		if(bFlagInputAB == FALSE)
		{
			ADPCM_Play(SE_OK);	/* 決定音 */
			bFlagInputAB = TRUE;
			bInput_c++;
		}
	}
	else if(((GetInput_P1() & JOY_B ) != 0u)	||	/* B */
        	((GetInput() & KEY_b_X  ) != 0u)	)	/* B(z) */
	{
		if(bFlagInputAB == FALSE)
		{
			ADPCM_Play(SE_CANSEL);	/* キャンセル音 */
			bFlagInputAB = TRUE;
			bInput_c--;
		}
	}
	else /* なし */
	{
		bFlagInputAB = FALSE;
	}

	if(bInput_c >= 3)
	{
		bInput_c = 0;
		bMoji = 0;
		S_Score_Save();

		ret = 3;
	}
	else
	{
		if(bInput_c < 0)
		{
			bInput_c = 0;
		}
		else /* なし */
		{

		}
		S_Score_NameInput(rank, bInput_c, bMoji + 'A');
		ret = bInput_c;
	}

	return ret;
}

#endif	/* SCORE_C */

