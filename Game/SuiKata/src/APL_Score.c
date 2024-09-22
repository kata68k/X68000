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
#include "IF_Music.h"
#include "IF_PCG.h"
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
static	uint16_t g_ComboCount_Old;
static	uint32_t g_ComboTime = PASSTIME_INIT;

/* �\���̒�` */

/* �֐��̃v���g�^�C�v�錾 */
int16_t S_Get_ScoreInfo(ST_SCORE *);
int16_t S_Set_ScoreInfo(ST_SCORE);
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
int16_t S_Score_Name_Main(int16_t, int16_t);

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

	ret = S_Set_Combo(g_stScore.uCombo + 1);

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

	if(uNum != 0)
	{
		GetNowTime(&g_ComboTime);	/* �R���{�^�C���A�E�g�J�E���^���Z�b�g�ŉ��� */
	}
	
	g_ComboCount_Old = g_stScore.uCombo;	/* �O��l */

	g_stScore.uCombo = uNum;

	g_stScore.uCombo = Mmin(g_stScore.uCombo, 255);	/* �R���{���Z */
	g_stScore.uComboMax = Mmax(g_stScore.uCombo, g_stScore.uComboMax);

	ret = g_stScore.uCombo;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Get_Combo(void)
{
	return g_stScore.uCombo;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_IsUpdate_Combo(void)
{
	int16_t	ret = 0;

	ret = (g_stScore.uCombo > g_ComboCount_Old);

	g_ComboCount_Old = g_stScore.uCombo;	/* �O��l */

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

	int8_t	sBuf[32];

	/* ���Z�b�g */
	g_stScore.ulScore		= 0;
	g_stScore.uCombo		= 0;
	g_stScore.uComboMax 	= 0;
	g_stScorePos.x = 128;
	g_stScorePos.y = 128;
	g_stScorePos.x_old = g_stScorePos.x;
	g_stScorePos.y_old = g_stScorePos.y;

	g_bUpdateScore[0] = TRUE;	/* �X�V */
	/* �X�R�A */
	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "SCORE");
#ifdef SC_GR	
	Draw_Message_To_Graphic(sBuf, 0, 0, G_L_BLUE, G_BLUE);	/* �X�R�A�\�� */
#endif
	BG_put_String(0, 0, BG_PAL, sBuf, strlen(sBuf));

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "HI-SCORE");
#ifdef SC_GR	
	sprintf(sBuf, "Score %10d   Hi-Score %10d", g_stScore.ulScore, g_stScore.ulScoreMax);
	Draw_Message_To_Graphic(sBuf, 0, 0, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
#endif
	BG_put_String(128, 0, BG_PAL, sBuf, strlen(sBuf));

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "NEXT");
#ifdef SC_GR	
	Draw_Message_To_Graphic(sBuf, 0, 0, G_L_BLUE, G_BLUE);	/* �X�R�A�\�� */
#endif
	BG_put_String(192, 16, BG_PAL, sBuf, strlen(sBuf));
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
	int8_t	sBuf[32];

	g_stScorePos.x_old = g_stScorePos.x;
	g_stScorePos.y_old = g_stScorePos.y;

	/* �N���A���� */
#ifdef SC_GR	
	G_CLR_AREA(g_stScorePos.x_old, 64, g_stScorePos.y_old, 12, 0);	/* �y�[�W 0 */
#endif
	memset(sBuf, 0x20, sizeof(sBuf));
	sBuf[31] = 0x00;
	BG_put_String(g_stScorePos.x_old, g_stScorePos.y_old, BG_PAL, sBuf, strlen(sBuf));

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
#ifdef SC_GR	
	int8_t	sBuf[128];
#endif

	S_GetPos(56, 96);	/* �X�R�A�\�����W�X�V */
	S_Add_Score_Point(g_stScore.uComboMax);
	
	S_Add_Score(10000);	/* �X�R�A�X�V */
//	Draw_Fill(g_stScorePos.x_old, g_stScorePos.y_old+1, g_stScorePos.x_old + 48, g_stScorePos.y_old + 11, 0);
	S_Main_Score();		/* �X�R�A�X�V */

#ifdef SC_GR	
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
#endif

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

	if(	GetPassTime(COMBO_TIMEOUT, &g_ComboTime) != 0u)	/* 1000ms�o�� */
	{
		S_Set_Combo(0);	/* �R���{�J�E���^ ���Z�b�g */
	}

	if(g_bUpdateScore[0] == TRUE)
	{
		int8_t	sBuf[32];

		g_bUpdateScore[0] = FALSE;

		if(g_bUpdateScore[1] == TRUE)
		{
			g_bUpdateScore[1] = FALSE;

			/* �l�������X�R�A */
			memset(sBuf, 0, sizeof(sBuf));
			if(g_stScore.uCombo > 1)
			{
				sprintf(sBuf, "+%dX%d", g_stScore.ulScoreAdd, g_stScore.uCombo);
			}
			else
			{
				sprintf(sBuf, "+%d", g_stScore.ulScoreAdd );
			}
#ifdef SC_GR	
			Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
#endif
			BG_put_String(g_stScorePos.x, g_stScorePos.y, BG_PAL, sBuf, strlen(sBuf));
		}

		if(g_bUpdateScore[2] == TRUE)
		{
			g_bUpdateScore[2] = FALSE;

			/* �l�������X�R�A */
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "+%d", g_stScore.ulScoreAdd );
#ifdef SC_GR	
			Draw_Message_To_Graphic(sBuf, g_stScorePos.x, g_stScorePos.y, F_MOJI, F_MOJI_BAK);
#endif
			BG_put_String(g_stScorePos.x, g_stScorePos.y, BG_PAL, sBuf, strlen(sBuf));
		}

		/* �X�R�A */
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%8d", g_stScore.ulScore);
#ifdef SC_GR
		Draw_Message_To_Graphic(sBuf, 32, 0, G_L_BLUE, G_BLUE);	/* �X�R�A�\�� */
#endif
		BG_put_String(56, 0, BG_PAL, sBuf, strlen(sBuf));
//		BG_put_Number10(48, 0, 2, g_stScore.ulScore);

		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%8d", g_stScore.ulScoreMax);
#ifdef SC_GR
		Draw_Message_To_Graphic(sBuf, 168, 0, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
#endif
		BG_put_String(192, 0, BG_PAL, sBuf, strlen(sBuf));
//		BG_put_Number10(192, 0, 2, g_stScore.ulScoreMax);

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

	x = SCORE_BOARD_X;
	y = NAME_INP_Y;

	ret = S_Update_Score_Board();

	memset(sBuf, 0, sizeof(sBuf));
	strcpy(sBuf, "No.  Score  Name");
	PutGraphic_To_Symbol24(sBuf, x + 1, y + 1, F_MOJI_BAK);
	PutGraphic_To_Symbol24(sBuf, x,     y, F_MOJI);


	x = NAME_INP_X;

	for(i=0; i < 8; i++)
	{
		y = NAME_INP_Y + ((i+1) * 24);

		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%d %9d %s", i + 1, g_uScoreRanking[i], g_bNameRanking[i]);

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

	x = 172 + (bInput_c * 12);
	y = NAME_INP_Y + ((i+1) * 24);

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

	File_Save_OverWrite("data/score.dat", sBuf, sizeof(int8_t), dataLength);

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

	File_Load("data/score.dat", sBuf, sizeof(int8_t), dataLength);

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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t S_Score_Name_Main(int16_t nInput, int16_t rank)
{
	int16_t	ret = 0;
	static int16_t nCol = 0;
	static int8_t bInput_c = 0;
	static int8_t bMoji = 0;
	static int8_t bFlagInput = FALSE;
	static int8_t bFlagInputAB = FALSE;
	int16_t nColSin, nColCos;
	
	nColSin = Mdiv256(31 * APL_Sin(nCol++));
	nColCos = Mdiv256(31 * APL_Cos(nCol++));
	G_PaletteSet(G_RED, SetRGB(nColCos, nColSin, nColCos));	/* �p���b�g�A�j�� */
	
	if( ( nInput & KEY_UPPER ) != 0) { /* �� */
		if(bFlagInput == FALSE)
		{
			ADPCM_Play(0);	/* SELECT�� */
			bFlagInput = TRUE;
			bMoji++;
			if(bMoji > 26 + 6)
			{
				bMoji = 0;
			}
		}
	}
	else if( ( nInput & KEY_LOWER ) != 0 ) { /* �� */
		if(bFlagInput == FALSE)
		{
			ADPCM_Play(0);	/* SELECT�� */
			bFlagInput = TRUE;
			bMoji--;
			if(bMoji < 0)
			{
				bMoji = 26 + 6;
			}
		}
	}
	else /* �Ȃ� */
	{
		bFlagInput = FALSE;
	}

	if(nInput == KEY_b_Z)		/* A�{�^��(z Key) */
	{
		if(bFlagInputAB == FALSE)
		{
			ADPCM_Play(4);	/* ���艹 */
			bFlagInputAB = TRUE;
			bInput_c++;
		}
	}
	else if(nInput == KEY_b_X)	/* B�{�^��(x Key) */
	{
		if(bFlagInputAB == FALSE)
		{
			ADPCM_Play(3);	/* �L�����Z���� */
			bFlagInputAB = TRUE;
			bInput_c--;
		}
	}
	else /* �Ȃ� */
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
		else /* �Ȃ� */
		{

		}
		S_Score_NameInput(rank, bInput_c, bMoji + 'A');
		ret = bInput_c;
	}

	return ret;
}

#endif	/* SCORE_C */

