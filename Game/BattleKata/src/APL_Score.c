#ifndef	SCORE_C
#define	SCORE_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usr_macro.h>
#include "APL_Score.h"
#include "IF_Draw.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"
#include "IF_Math.h"
#include "IF_Text.h"

/* �f�[�^ */
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

/* �O���[�o���ϐ� */
static 	ST_SCORE		g_stScore = {0};
static 	ST_SCORE_POS	g_stScorePos = {0};
static	int8_t	g_bUpdateScore[3];
static	uint32_t g_uScoreRanking[10] = {0};
static	int8_t  g_bNameRanking[10][5] = {0};

/* �\���̒�` */

/* �֐��̃v���g�^�C�v�錾 */
int16_t S_Get_ScoreInfo(ST_SCORE *);
int16_t S_Set_ScoreInfo(ST_SCORE);
int16_t S_Add_Score(uint64_t);
int16_t S_Add_Score_Point(uint64_t);
int16_t S_Set_Combo(int16_t);
int16_t S_All_Init_Score(void);
int16_t S_Init_Score(void);
int16_t S_GetPos(int16_t, int16_t);
int16_t S_Clear_Score(void);
int16_t S_Main_Score(void);
int16_t S_Score_Board(void);
static int16_t S_Update_Score_Board(void);
int16_t S_Score_NameInput(int16_t, int8_t, int8_t);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Get_ScoreInfo(ST_SCORE *p_stScore)
{
	int16_t	ret = 0;

	*p_stScore = g_stScore;	/* �������擾 */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Set_ScoreInfo(ST_SCORE stScore)
{
	int16_t	ret = 0;

	g_stScore = stScore;	/* �X�V */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Add_Score(uint64_t ulNum)
{
	int16_t	ret = 0;
	uint64_t	val64;

	val64 = g_stScore.ulScore;
	
	g_stScore.uCombo++;
	g_stScore.uCombo = Mmin(g_stScore.uCombo, 255);	/* �R���{���Z */
	g_stScore.uComboMax = Mmax(g_stScore.uCombo, g_stScore.uComboMax);

	g_stScore.ulScoreAdd = ulNum;
	val64 += g_stScore.ulScoreAdd * g_stScore.uCombo;	/* ���Z */
	
	/* ���݂̓_�� */
	if(g_stScore.ulScore > val64)	/* �I�[�o�[�t���[�΍� */
	{
		g_stScore.ulScore = -1;	/* �J���X�g */
	}
	else
	{
		g_stScore.ulScore = val64;	/* �X�V */
	}

	/* �ō��̓_�� */
	g_stScore.ulScoreMax = Mmax(g_stScore.ulScoreMax, g_stScore.ulScore);
	
	g_bUpdateScore[0] = TRUE;	/* �X�V */
	g_bUpdateScore[1] = TRUE;	/* �X�V */

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Add_Score_Point(uint64_t ulNum)
{
	int16_t	ret = 0;
	uint64_t	val64;

	val64 = g_stScore.ulScore;
	
	g_stScore.ulScoreAdd = ulNum;

	val64 += g_stScore.ulScoreAdd;	/* ���Z */
	
	/* ���݂̓_�� */
	if(g_stScore.ulScore > val64)	/* �I�[�o�[�t���[�΍� */
	{
		g_stScore.ulScore = -1;	/* �J���X�g */
	}
	else
	{
		g_stScore.ulScore = val64;	/* �X�V */
	}

	/* �ō��̓_�� */
	g_stScore.ulScoreMax = Mmax(g_stScore.ulScoreMax, g_stScore.ulScore);
	
	g_bUpdateScore[0] = TRUE;	/* �X�V */
	g_bUpdateScore[2] = TRUE;	/* �X�V */

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Set_Combo(int16_t uNum)
{
	int16_t	ret = 0;

	g_stScore.uCombo = uNum;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_All_Init_Score(void)
{
	int16_t	ret = 0;

	int16_t	i;

	/* ���Z�b�g */
	g_stScore.ulScore		= 0;
	g_stScore.uCombo		= 0;
	g_stScore.uComboMax 	= 0;

	g_bUpdateScore[0] = TRUE;	/* �X�V */

	if(S_Score_Load() < 0)	/* �X�R�A�f�[�^�̓ǂݏo�� */
	{
		/* �s��v�̏ꍇ������ */

		g_stScore.ulScoreMax	= 50000;

		for(i=0; i < 10; i++)
		{
			g_uScoreRanking[i] = g_stScore.ulScoreMax - (i * 5000);
			sprintf(g_bNameRanking[i], "%c%c%c", 'A'+i, 'A'+i, 'A'+i); 
		}
	}
	else
	{
		g_stScore.ulScoreMax	= g_uScoreRanking[0];
	}

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Init_Score(void)
{
	int16_t	ret = 0;

	/* ���Z�b�g */
	g_stScore.ulScore		= 0;
	g_stScore.uCombo		= 0;
	g_stScore.uComboMax 	= 0;

	g_bUpdateScore[0] = TRUE;	/* �X�V */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_GetPos(int16_t x, int16_t y)
{
	int16_t	ret = 0;

	g_stScorePos.x_old = g_stScorePos.x;
	g_stScorePos.y_old = g_stScorePos.y;

	G_CLR_AREA(g_stScorePos.x_old, 64, g_stScorePos.y_old, 12, 0);	/* �y�[�W 0 */

	if(x > 200)x = 200;
	if(y < 80)y = 80;

	g_stScorePos.x = x;
	g_stScorePos.y = y;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Clear_Score(void)
{
	int16_t	ret = 0;
	int8_t	sBuf[128];

	S_GetPos(56, 96);	/* �X�R�A�\�����W�X�V */
	S_Add_Score_Point(g_stScore.uComboMax);
	
	S_Add_Score(10000);	/* �X�R�A�X�V */
//	Draw_Fill(g_stScorePos.x_old, g_stScorePos.y_old+1, g_stScorePos.x_old + 48, g_stScorePos.y_old + 11, 0);
	S_Main_Score();		/* �X�R�A�X�V */

	G_CLR_AREA(0, 255, 16, 255, 0);	/* �y�[�W 0 */

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "STAGE CLEAR!!");
	PutGraphic_To_Symbol24(sBuf, 48 + 1, 32 + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, 48, 32, F_MOJI);
	
	/* �l�������X�R�A */
	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "Clear Bonus +%d x %d", g_stScore.ulScoreAdd, g_stScore.uCombo);	/* ���C�t�J�E���^(�R���{����Ȃ�) */
	Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "Max Combo Bonus +%d", g_stScore.uComboMax);	/* �ő�R���{ */
	Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y + 12, F_MOJI, F_MOJI_BAK);

	memset(sBuf, 0, sizeof(sBuf));
	sprintf(sBuf, "%d", g_stScore.ulScore);
	PutGraphic_To_Symbol24(sBuf, g_stScorePos.x + 32 + 1, g_stScorePos.y + 32 + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, g_stScorePos.x + 32, g_stScorePos.y + 32, F_MOJI);
	
	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Main_Score(void)
{
	int16_t	ret = 0;

	if(g_bUpdateScore[0] == TRUE)
	{
		int8_t	sBuf[128];

		g_bUpdateScore[0] = FALSE;

//		Draw_Fill(g_stScorePos.x_old, g_stScorePos.y_old+1, g_stScorePos.x_old + 48, g_stScorePos.y_old + 11, 0);
//		G_CLR_AREA(0, 255, 0, 255, 0);	/* �y�[�W 0 */

		if(g_bUpdateScore[1] == TRUE)
		{
			g_bUpdateScore[1] = FALSE;

			/* �l�������X�R�A */
			memset(sBuf, 0, sizeof(sBuf));
			if(g_stScore.uCombo > 1)
			{
//				sprintf(sBuf, "0x%x+%d", g_stScore.ulScoreAdd, g_stScore.uCombo);
				sprintf(sBuf, "+%dx%d", g_stScore.ulScoreAdd, g_stScore.uCombo);
			}
			else
			{
//				sprintf(sBuf, "0x%x", g_stScore.ulScoreAdd );
				sprintf(sBuf, "+%d", g_stScore.ulScoreAdd );
			}
			Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
		}

		if(g_bUpdateScore[2] == TRUE)
		{
			g_bUpdateScore[2] = FALSE;

			/* �l�������X�R�A */
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "+%d", g_stScore.ulScoreAdd );
			Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
		}

		/* �X�R�A */
		G_CLR_AREA(0, 200, 0, 12, 0);	/* �y�[�W 0 */
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "Score %7d Hi-Score %7d", g_stScore.ulScore, g_stScore.ulScoreMax);
		Draw_Message_To_Graphic(sBuf, 0, 0, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
	}

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Score_Board(void)
{
	int16_t	ret = 0;

	int16_t	i;
	int16_t x, y;
	int8_t	sBuf[128];
	
	G_CLR_AREA(0, 255, 16, 255, 0);	/* �y�[�W 0 */
				
	ret = S_Update_Score_Board();

	x = 16;
	y = 24;
	
	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "No.  Score  Name");
	PutGraphic_To_Symbol24(sBuf, x + 1, y + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, x,     y, F_MOJI);

	x = 28;

	for(i=0; i < 8; i++)
	{
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%d %8d %s", i + 1, g_uScoreRanking[i], g_bNameRanking[i]);
		y = 24 + ((i+1) * 24);
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
			g_uScoreRanking[i+1] = g_uScoreRanking[i];		/* �Y���Ă��炤 */
			strcpy(g_bNameRanking[i+1], g_bNameRanking[i]);
		}
		g_uScoreRanking[rank] = g_stScore.ulScore;		/* �X�V */
		strcpy(g_bNameRanking[rank], "You");
	}
	ret = rank;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Score_NameInput(int16_t nRank, int8_t bInput_c, int8_t bMoji)
{
	int16_t	ret = 0;

	int16_t	i;
	int16_t x, y;
	int8_t	sBuf[8];
	
	i = nRank;

	x = 160 + (bInput_c * 12);
	y = 24 + ((i+1) * 24);
	G_CLR_AREA(x, 12, y, 24, 0);	/* �y�[�W 0 */
	
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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

	File_Save_OverWrite("data/BK_sc.dat", sBuf, sizeof(int8_t), dataLength);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Score_Load(void)
{
	int16_t	ret = 0;
	uint32_t crc;
	int8_t	sBuf[94];
	size_t dataLength;

	dataLength = sizeof(sBuf) / sizeof(sBuf[0]);

	File_Load("data/BK_sc.dat", sBuf, sizeof(int8_t), dataLength);

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

#endif	/* SCORE_C */

