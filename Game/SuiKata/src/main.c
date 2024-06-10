#ifndef	MAIN_C
#define	MAIN_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>
#include <time.h>
#include <interrupt.h>

#include <usr_macro.h>
#include <apicglib.h>

#include "main.h"

#include "BIOS_CRTC.h"
#include "BIOS_DMAC.h"
#include "BIOS_MFP.h"
#include "BIOS_PCG.h"
#include "BIOS_MPU.h"
#include "IF_Draw.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"
#include "IF_Input.h"
#include "IF_MACS.h"
#include "IF_Math.h"
#include "IF_Memory.h"
#include "IF_Mouse.h"
#include "IF_Music.h"
#include "IF_PCG.h"
#include "IF_Text.h"
#include "IF_Task.h"
#include "APL_PCG.h"
#include "APL_Score.h"

//#define 	W_BUFFER	/* �_�u���o�b�t�@�����O���[�h */ 
enum
{
	ball_lost,
	ball_now,
	ball_next,
	ball_exist,
};

#define		BALL_MAX	(60)
#define 	X_POS_MIN	( 64+16+2)
#define 	X_POS_MAX	(192-16-2)
#define 	X_POS_W		(X_POS_MAX - X_POS_MIN)
#define 	Y_POS_MIN	(16)
#define 	Y_POS_BD	(48)
#define 	Y_POS_MAX	(192)
#define 	Y_POS_H		(Y_POS_MAX - Y_POS_MIN)
#define 	X_ACC		(256)
#define 	Y_ACC		(256)
#define 	Y_ACC2		(512)

#define 	FONT24H		(24)
#define 	FONT12W		(12)

#define		NUM_STARS	(128)

#define		BG_MAP_PAL	(2)

#define		START_LOGO_INTERVAL_TIME	(1000)
#define		PLAYER_INPUT_INTERVAL_TIME	(800)
#define		COMBO_RECOVER_COUNT			(5)
#define		COMBO_INTERVAL_SE_TIME		(500)
#define		GAL_ANIME_INT_TIME			(112)
#define		DEMO_LOGO_INTERVAL_TIME		(1000)
#define		DEMO_START_END_TIME			(30000)
#define		DEMO_GAMEOVER_TIME			(5000)


typedef struct
{
	uint8_t		bSP_num;		/* �v���[���ԍ��̐擪 */
	uint8_t		bSize;			/* �T�C�Y */
	uint8_t		bStatus;		/* �X�e�[�^�X */
	uint8_t		bValidty;		/* �L�� */
}	ST_BALL;

/* �O���[�o���ϐ� */
uint32_t	g_unTime_cal = 0u;
uint32_t	g_unTime_cal_PH = 0u;
uint32_t	g_unTime_Pass = 0xFFFFFFFFul;
int32_t		g_nSuperchk = 0;
int32_t		g_nCrtmod = 0;
int32_t		g_nMaxUseMemSize;
int16_t		g_CpuTime = 0;
uint8_t		g_mode;
uint8_t		g_mode_rev;
uint8_t		g_Vwait = 1;
uint8_t		g_bFlip = FALSE;
uint8_t		g_bFlip_old = FALSE;
uint8_t		g_bFPS_PH = 0u;
uint8_t		g_bExit = TRUE;

int16_t		g_numbers[BALL_MAX];
int16_t		g_index;
int16_t		g_index_now;
int16_t		g_index_next;
int16_t		g_OK_num = 0;
int16_t		g_Circle_Table[360][2];
int16_t		g_Circle_Table_x_Min;
int16_t		g_Circle_Table_x_Max;
int16_t		g_Circle_Table_y_Min;
int16_t		g_Circle_Table_y_Max;
int16_t		g_Sign_Table[BALL_MAX];
int16_t		g_Rand_Table4[BALL_MAX];
int16_t		g_Rand_Table8[BALL_MAX];
int16_t		g_Rand_Table16[BALL_MAX];
int16_t		g_Rand_Table32[BALL_MAX];
int16_t		g_Rand_num = 0;
int16_t		g_GameSpeed;

static int16_t s_G1_sc_y = 0;
static uint32_t s_PassTime;
static uint32_t s_DemoTime;

/* �O���[�o���\���� */
ST_BALL		g_stBall[BALL_MAX];
int16_t		g_Ball_Color[5] = {0, 1, 2, 3, 4};
int16_t		g_Ball_Table[5] = {SP_BALL_S_1, SP_BALL_M_1, SP_BALL_L_1, SP_BALL_XL_1, PCG_NUM_MAX};
int16_t		g_Boder_line = 0xFF;
int16_t		g_Boder_count = 0;
int16_t		g_TB_GameLevel[3] = {256, 224, Y_POS_MAX};
int16_t		g_nGameLevel = 0;
int8_t		g_TB_GameDiffLevel[2][8] = {"EASY", "HARD"};
int16_t		g_nGameDifflevel = 0;
int8_t		g_bShot_OK = TRUE;
uint8_t		g_ubDemoPlay = FALSE;
uint8_t		g_ubGameClear = FALSE;
int16_t		g_nGameClearCount = 0;

/* �֐��̃v���g�^�C�v�錾 */
static void Boder_line(void);
static void Level_meter(int8_t);
static int16_t Ball_Combine(ST_PCG *, int16_t, ST_PCG *, int16_t);
static int8_t Ball_Point_Update(ST_PCG*, int8_t);
static void Ball_Check(int16_t);

int16_t main(int16_t, int8_t**);
static void App_Init(void);
static void App_exit(void);
int16_t	BG_main(uint32_t);
void App_TimerProc( void );
int16_t App_RasterProc( uint16_t * );
void App_VsyncProc( void );
void App_HsyncProc( void );
int16_t	App_FlipProc( void );
int16_t	SetFlip(uint8_t);
int16_t	GetGameMode(uint8_t *);
int16_t	SetGameMode(uint8_t);

void (*usr_abort)(void);	/* ���[�U�̃A�{�[�g�����֐� */

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Boder_line(void)
{
#if 0
	ST_TASK stTask;
#endif
	static uint32_t s_TimeStamp = PASSTIME_INIT;

	ST_PCG	*p_stPCG = NULL;

	p_stPCG = PCG_Get_Info(SP_ARROW_0);		/* ��� */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = X_POS_MIN - 16 - 2;
		p_stPCG->y = g_Boder_line - 8;
		p_stPCG->update	= TRUE;
	}

	p_stPCG  = PCG_Get_Info(SP_GAL_0);		/* �M���� */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = 208;
		p_stPCG->y = Mmax(Y_POS_BD, g_Boder_count);
		p_stPCG->update		= TRUE;


		if(	GetPassTime(GAL_ANIME_INT_TIME, &s_TimeStamp) != 0u)	/* 112ms�o�� */
		{
			/* �A�j���[�V���� */
			p_stPCG->Anime++;
			if(p_stPCG->Anime >= p_stPCG->Pat_AnimeMax)
			{
				p_stPCG->Anime = 0;
			}
		}
	}

#if 0
	GetTaskInfo(&stTask);	/* �^�X�N�擾 */

	switch(stTask.bScene)
	{
		case SCENE_GAME1:
		case SCENE_GAME2:
		{
			if(g_bFPS_PH < 4)	/* 4�t���[�� */
			{

			}
			else if(g_bFPS_PH < 8)	/* 8�t���[�� */
			{
				PCG_COL_SHIFT(7, 1);	/* �p���b�g�V�t�g */
			}
			else if(g_bFPS_PH < 16)	/* 16�t���[�� */
			{
				PCG_COL_SHIFT(6, 1);	/* �p���b�g�V�t�g */
			}
			else
			{
				/* �������Ȃ� */
			}
			break;
		}
		default:
		{
			/* �������Ȃ� */
			break;
		}
	}
#endif

}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Level_meter(int8_t bFlag)
{
	static int16_t nCount = 0;
	uint32_t time_now;
	int16_t m, n;

	if(bFlag == TRUE)
	{
		nCount = 0;
		for(m=0; m < 10; m++)
		{
			Draw_Line( X_POS_MIN + (10 * m), Y_POS_BD - 1, X_POS_MIN + (10 * m) + 1, Y_POS_BD - 1, G_WHITE, 0xFFFF);	/* ���� */
		}
	}

	GetNowTime(&time_now);
	m = (int16_t)(time_now - g_unTime_Pass);	
	n = nCount;
	Draw_Line( X_POS_MIN, Y_POS_BD + n, X_POS_MAX, Y_POS_BD + n, G_BG, 0xFFFF);	/* ���� */
	n = nCount++;
	Draw_Line( X_POS_MIN, Y_POS_BD + n, X_POS_MIN + m, Y_POS_BD + n, n, 0xFFFF);	/* �`�� */
	
	g_unTime_Pass = time_now;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static int16_t Ball_Combine(ST_PCG *p_stPCG, int16_t mine, ST_PCG *p_stPCG_other, int16_t other)
{
	int16_t ret = 0;
	int16_t x, y;
	int16_t x1, y1;
	int16_t i, j, k, m, n, p;
//	int16_t width, height;
	int16_t pal;

	ST_PCG	*p_stPCG_new = NULL;

	i = mine;
	j = other;

	x = p_stPCG->x;
	y = p_stPCG->y;
	x1 = Mdiv2(x + p_stPCG_other->x);
	y1 = Mdiv2(y + p_stPCG_other->y);

	/* �����蔻�� */
	if( ((g_stBall[i].bSize)   == (g_stBall[j].bSize))	&&
		((g_stBall[i].bStatus) == (g_stBall[j].bStatus)) )
	{
		int16_t BallSize;

		if(g_ubGameClear == TRUE)
		{
			return ret;
		}

		/* ���� */
		g_stBall[i].bStatus++;
		/* �{�[���ݒ� */
		BallSize = g_stBall[i].bSize;		

		/* ���� */
		g_stBall[j].bStatus = (rand() % 3);
		g_stBall[j].bSize = 0;
		g_stBall[j].bValidty = ball_lost;

		p_stPCG_other->x = 0;
		p_stPCG_other->y = 0;
		p_stPCG_other->update = FALSE;


		if( g_stBall[i].bStatus > 2 )	/* �i���̏���𒴂������T�C�Y�`�F���W */
		{
			/* �����͈�U�A������ */
			g_stBall[i].bStatus = (rand() % 3);
			g_stBall[i].bSize = 0;
			g_stBall[i].bValidty = ball_lost;
			p_stPCG->x = 0;
			p_stPCG->y = 0;
			p_stPCG->update = FALSE;

			if( BallSize >= 3 )	/* �ő�T�C�Y�̍ŏI�n�ō��� */
			{
				g_ubGameClear = TRUE;	/* �Q�[���N���A */

				S_GetPos(44, 136);		/* �X�R�A�\�����W�X�V */

				ret = S_Add_Score(1000000);	/* �X�R�A�X�V */
				return ret;
			}
			else
			{
				S_GetPos(x, y);	/* �X�R�A�\�����W�X�V */

				ret = S_Add_Score(1000);	/* �X�R�A�X�V */
			}

			BallSize++;	/* �T�C�Y�A�b�v */ 
			m = Mmin(BallSize, 3);	/* �ő�l�{�P */

			for(k = g_Ball_Table[m]; k < g_Ball_Table[m + 1]; k++)	/* �V�����e�[�u������󂫂𒲍����� */
			{
				n = k - g_Ball_Table[0];	/*  */
				
				if(	g_stBall[n].bValidty == ball_lost )	/* �����f�[�^ */
				{
					g_stBall[n].bSize = m;		/* ���̃T�C�Y�̈�� */
					g_stBall[n].bStatus = 0;	/* �ŏ��̃J���[ */
					g_stBall[n].bValidty = ball_exist;

					/* �X�v���C�g�ݒ� */
					p_stPCG_new = PCG_Get_Info(SP_BALL_S_1 + g_stBall[n].bSP_num);	/* New�{�[�� */
					if(p_stPCG_new != NULL)
					{
						pal = Mmul256(g_stST_PCG_LIST[m + 1].Pal);
						for(p=0; p < p_stPCG_new->Pat_DataMax; p++)
						{
							*(p_stPCG_new->pPatCodeTbl + p) &= 0xF0FF;
							*(p_stPCG_new->pPatCodeTbl + p) |= (pal + Mmul256(g_Ball_Color[g_stBall[n].bStatus]));	/* �J���[�Z�b�g */
						}
						p_stPCG_new->update = TRUE;
#if 0
						width	= Mmul16(p_stPCG_new->Pat_w);
						height	= Mmul16(p_stPCG_new->Pat_h);

						/* �r�� */
						if(y > g_TB_GameLevel[g_nGameLevel] - height)	/* �r���� */
						{
							p_stPCG_new->dy = 0;
							y = g_TB_GameLevel[g_nGameLevel] - height;
						}
						else
						{
							p_stPCG_new->dy = Y_ACC;
						}

						if(x <= X_POS_MIN)			/* �r���� */
						{
							p_stPCG_new->dx = 0;
							x = X_POS_MIN;
						}
						else if(x > X_POS_MAX - width)	/* �r���E */
						{
							p_stPCG_new->dx = 0;
							x = X_POS_MAX - width;
						}
#else
						p_stPCG_new->x = x1;
						p_stPCG_new->y = y1;
						p_stPCG_new->dy = 0;
						p_stPCG_new->dx = 0;
#endif						
						break;
					}
				}
			}
		}
		else	/* �J���[�`�F���W */
		{
			if( BallSize >= 3 )	/* �ő�T�C�Y�̍ŏI�n�ō��� */
			{
				/* �����͈�U�A������ */
				g_stBall[i].bStatus = (rand() % 3);
				g_stBall[i].bSize = 0;
				g_stBall[i].bValidty = ball_lost;
				p_stPCG->x = 0;
				p_stPCG->y = 0;
				p_stPCG->update = FALSE;

				g_ubGameClear = TRUE;	/* �Q�[���N���A */

				S_Set_Combo(0);			/* �R���{�J�E���^ ���Z�b�g */
				ret = S_Add_Score(1);	/* �X�R�A�X�V */
			}
			else
			{
				S_GetPos(x, y);	/* �X�R�A�\�����W�X�V */

				ret = S_Add_Score(100);	/* �X�R�A�X�V */

				pal = Mmul256(g_stST_PCG_LIST[g_stBall[i].bSize + 1].Pal);
				for(k=0; k < p_stPCG->Pat_DataMax; k++)
				{
					*(p_stPCG->pPatCodeTbl + k) &= 0xF0FF;
					*(p_stPCG->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* �J���[�Z�b�g */
				}
#if 0
				height	= Mmul16(p_stPCG->Pat_h);
				/* �r�� */
				if(y > g_TB_GameLevel[g_nGameLevel] - height)	/* �r���� */
				{
					p_stPCG->dy = 0;
					y = g_TB_GameLevel[g_nGameLevel] - height;
				}
				else
				{
					p_stPCG->dy = Y_ACC;
				}
#else
				p_stPCG->x = x1;
				p_stPCG->y = y1;
				p_stPCG->dy = 0;
				p_stPCG->dx = 0;
#endif						
			}
		}
	}
	else if( ((g_stBall[i].bSize) == (g_stBall[j].bSize)) )
	{
		/* �������Ȃ� */
	}
	else
	{
		/* �������Ȃ� */
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
static int8_t Ball_Point_Update(ST_PCG* p_stPCG, int8_t hitFlag)
{
	int8_t	ret = FALSE;
 	int16_t x, y, dx, dy, sx, sy;
	int16_t width, height;

	x = p_stPCG->x;
	y = p_stPCG->y;
	dx = p_stPCG->dx;
	dy = p_stPCG->dy;
	width	= Mmul16(p_stPCG->Pat_w);
	height	= Mmul16(p_stPCG->Pat_h);

	/* �ړ��� */
	sx = Mdiv256s(dx);
	sy = Mdiv256s(dy);
	x = (uint16_t)((int16_t)x + sx);
	y = (uint16_t)((int16_t)y + sy);

	/* ���ړ� ���� */
	if(hitFlag == TRUE)
	{

	}
	else
	{
		if(Mabs(dx) <= X_ACC)
		{
			dx = 0;
		}
		else
		{
			if(dx > 0)
			{
				dx -= (int16_t)X_ACC;
			}
			else
			{
				dx += (int16_t)X_ACC;
			}
		}
	}

	/* �c�ړ� ���� */
	if(hitFlag == TRUE)
	{
	}
	else	/* �c�ړ� ���� */
	{
		/* ���R���� */
		dy += Y_ACC;
		dy = Mmin(dy, 0x1FFF);
	}

	/* �r�� */
	if(y > g_TB_GameLevel[g_nGameLevel] - height)	/* �r���� */
	{
		y = g_TB_GameLevel[g_nGameLevel] - height;
		hitFlag = TRUE;	/* �Փ� */
	}
	else
	{
	}

	if(x < X_POS_MIN)			/* �r���� */
	{
		if(Mabs(dx) <= X_ACC)
		{
			dx = 0;
		}
		else
		{
			if(dx > 0)
			{
				dx -= (int16_t)X_ACC;
			}
			else
			{
				dx += (int16_t)X_ACC;
			}
		}
		x = X_POS_MIN;
	}
	else if(x > X_POS_MAX - width)	/* �r���E */
	{
		if(Mabs(dx) <= X_ACC)
		{
			dx = 0;
		}
		else
		{
			if(dx > 0)
			{
				dx -= (int16_t)X_ACC;
			}
			else
			{
				dx += (int16_t)X_ACC;
			}
		}
		x = X_POS_MAX - width;
	}
	
	/* �����x�̍X�V */
	p_stPCG->dx = dx;
	p_stPCG->dy = dy;

	/* �ŏI�ʒu */
	p_stPCG->x = x;
	p_stPCG->y = y;

	ret = hitFlag;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Ball_Check(int16_t nBallCount)
{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	//int8_t sBuf[128];
#endif
	int16_t	i, j, k, l;
	int16_t x, y;
	int16_t w, h;
	int16_t dw, dh;
	int16_t cx, cy;
	int16_t dx, dy;
	int16_t xs, ys, xe, ye;
	int16_t width, height;
	int16_t line = 0xFF;
	int8_t bValid;

	int16_t x1, y1;
	int16_t w1, h1;
	int16_t dw1, dh1;
	int16_t cx1, cy1;
	int16_t dx1, dy1;
	int16_t width1, height1;
	int16_t xs1, ys1, xe1, ye1;

	uint32_t distance,  distance_src, distance_min;

	int8_t overlapping_pairs[BALL_MAX][2];
	int16_t num_pairs = 0;
	int8_t overlapped = FALSE;
	int8_t non_overlapping[BALL_MAX];
	int16_t num_non_overlapping = 0;

	int16_t	sound = 0;
	static uint32_t s_WaitTime = PASSTIME_INIT;
	static uint8_t s_bErase = FALSE;
	uint8_t bEraseTmp = FALSE;

	uint8_t allRectanglesNonOverlapping;
#if 0
	uint32_t time_st;
#endif
	ST_PCG	*p_stPCG = NULL;
	ST_PCG	*p_stPCG_other = NULL;

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	Level_meter(FALSE);	/* �������׌v�� */
#endif
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	static int16_t count = 0;
#endif

	line = g_TB_GameLevel[g_nGameLevel];	/* �O��l */
#if 0
	GetNowTime(&time_st);	/* �^�C���A�E�g�J�E���^���Z�b�g */
#endif
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Ball_Check st(%d)\n", count);
#endif

	memset(overlapping_pairs, 0, sizeof(overlapping_pairs));
	memset(non_overlapping, 0, sizeof(non_overlapping));

	/* �d�Ȃ蒊�o */
	for(i = 0; i < BALL_MAX - 1; i++)
	{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//		Level_meter(FALSE);	/* �������׌v�� */
#endif
		bValid = g_stBall[i].bValidty;
		if(	(bValid == ball_lost )	||	/* �����f�[�^ */
			(bValid == ball_now )	||	/* ���˗\��{�[�� */
			(bValid == ball_next )	)	/* ���̃{�[�� */
		{
			if(i == g_index_next)		/* ���̃{�[�� */
			{
				/* �������Ȃ� */
			}
			else if(i == g_index_now)	/* ���˗\��{�[�� */
			{
				/* �������Ȃ� */
			}
			else
			{
				p_stPCG->update	= FALSE;
			}
			continue;
		}

		p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* �{�[�� */
		if(p_stPCG == NULL)
		{
			continue;
		}

		overlapped = FALSE;	/* ������ */
		
		x = p_stPCG->x;
		y = p_stPCG->y;
		p_stPCG->update	= TRUE;	/* �`�� */

		width	= Mmul16(p_stPCG->Pat_w);
		height	= Mmul16(p_stPCG->Pat_h);
		w = Mdiv2(width);
		h = Mdiv2(height);
		dw = Mdiv16(width);
		dh = Mdiv16(height);
		cx = x + w; 
		cy = y + h;

		for(j = i + 1; j < BALL_MAX; j++)
		{
			bValid = g_stBall[j].bValidty;
			if(	(bValid == ball_lost )	||	/* �����f�[�^ */
				(bValid == ball_now )	||	/* ���˗\��{�[�� */
				(bValid == ball_next )	)	/* ���̃{�[�� */
			{
				if(j == g_index_next)		/* ���̃{�[�� */
				{
					/* �������Ȃ� */
				}
				else if(j == g_index_now)	/* ���˗\��{�[�� */
				{
					/* �������Ȃ� */
				}
				else
				{
					p_stPCG_other->update = FALSE;
				}
				continue;
			}

			p_stPCG_other = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num);	/* ���̃{�[�� */
			if(p_stPCG_other == NULL)
			{
				continue;
			}
			
			x1 = p_stPCG_other->x;
			y1 = p_stPCG_other->y;
			p_stPCG_other->update	= TRUE;	/* �`�� */

			width1	= Mmul16(p_stPCG_other->Pat_w);
			height1	= Mmul16(p_stPCG_other->Pat_h);
			w1 = Mdiv2(width1); 
			h1 = Mdiv2(height1);
			dw1 = Mdiv16(width1); 
			dh1 = Mdiv16(height1);
			cx1 = x1 + w1; 
			cy1 = y1 + h1;

			line = Mmin(y, line);	/* �X�V */
			line = Mmin(y1, line);	/* �X�V */

			distance = APL_distance(cx, cy , cx1, cy1);
			distance_src = Mmin((w + w1), (h + h1));
			distance_src = distance_src * distance_src;

			if(distance < (Mmul2(distance_src) + distance_src))		/* �������߂� */
			{
				if(p_stPCG->dy != 0)	/* �������ђʂ��Ȃ��悤�ɂ��� */
				{
					p_stPCG->dy = Y_ACC2;
				}
				if(p_stPCG_other->dy != 0)	/* �������ђʂ��Ȃ��悤�ɂ��� */
				{
					p_stPCG_other->dy = Y_ACC2;
				}
			}			
#if 1
			xs = x + dw;
			ys = y + dh;
			xe = x + width - dw;
			ye = y + height - dh;

			xs1 = x1 + dw1;
			ys1 = y1 + dh1;
			xe1 = x1 + width1 - dw1;
			ye1 = y1 + height1 - dh1;
#else
			xs = x;
			ys = y;
			xe = x + width;
			ye = y + height;

			xs1 = x1;
			ys1 = y1;
			xe1 = x1 + width1;
			ye1 = y1 + height1;
#endif
			if( (xs < xe1) && (xe > xs1) && (ys < ye1) && (ye > ys1) )
//			if(distance < distance_src)		/* ����������ɋ߂� */
			{
				if(p_stPCG->dy != 0)		/* ��~ */
				{
					p_stPCG->dy = 0;
				}
				if(p_stPCG_other->dy != 0)	/* ��~ */
				{
					p_stPCG_other->dy = 0;
				}
				overlapping_pairs[num_pairs][0] = i;
				overlapping_pairs[num_pairs][1] = j;
				num_pairs++;
				if((i == nBallCount) || (j == nBallCount))		/* �L���������{�[�� */
				{
					g_bShot_OK = TRUE;	/* ���̔��ˋ��� */
				}
				overlapped = TRUE;	/* �d�Ȃ��� */
			}
		}

		if(overlapped == FALSE)
		{
			non_overlapping[num_non_overlapping++] = i;	/* �d�Ȃ�Ȃ����� */
		}
	}

	if(line != g_TB_GameLevel[g_nGameLevel])
	{
		g_Boder_line = line;
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Ball_Check st2(%d)\n", count);
#endif

	if(s_bErase == TRUE)
	{
		if(	GetPassTime(COMBO_INTERVAL_SE_TIME, &s_WaitTime) != 0u  )	/* 500ms�o�� */
		{
			s_bErase = FALSE;
		}
		else
		{
			//return;
		}
	}

    allRectanglesNonOverlapping = FALSE;

//	while (!allRectanglesNonOverlapping)
	{
        allRectanglesNonOverlapping = TRUE; // ��U�d�Ȃ��Ă��Ȃ��Ɖ���

		sound = 0;

		/* �d�Ȃ�f�[�^�̂ݔ��� */	
		for(k = 0; k < num_pairs; k++)
		{
			int16_t Prex, Prey;

			i = overlapping_pairs[k][0];
			j = overlapping_pairs[k][1];

			bValid = g_stBall[i].bValidty;
			if(	(bValid == ball_lost )	||	/* �����f�[�^ */
				(bValid == ball_now )	||	/* ���˗\��{�[�� */
				(bValid == ball_next )	)	/* ���̃{�[�� */
			{
				continue;
			}
			
			bValid = g_stBall[j].bValidty;
			if(	(bValid == ball_lost )	||	/* �����f�[�^ */
				(bValid == ball_now )	||	/* ���˗\��{�[�� */
				(bValid == ball_next )	)	/* ���̃{�[�� */
			{
				continue;
			}

			p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* �{�[�� */
			if(p_stPCG == NULL)
			{
				continue;
			}
			p_stPCG_other = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num);	/* ���̃{�[�� */
			if(p_stPCG_other == NULL)
			{
				continue;
			}
			// ----------------------------------
			x = p_stPCG->x;
			y = p_stPCG->y;

			dx = p_stPCG->dx;
			dy = p_stPCG->dy;
			width	= Mmul16(p_stPCG->Pat_w);
			height	= Mmul16(p_stPCG->Pat_h);

			w = Mdiv2(width);
			h = Mdiv2(height);
			cx = x + w; 
			cy = y + h;

			xs = x;// + w;
			ys = y;// + h;
			xe = x + width;// - w;
			ye = y + height;// - h;

			// ----------------------------------
			x1 = p_stPCG_other->x;
			y1 = p_stPCG_other->y;

			dx1 = p_stPCG_other->dx;
			dy1 = p_stPCG_other->dy;
			width1	= Mmul16(p_stPCG_other->Pat_w);
			height1	= Mmul16(p_stPCG_other->Pat_h);

			w1 = Mdiv2(width1);
			h1 = Mdiv2(height1);
			cx1 = x1 + w1; 
			cy1 = y1 + h1;

			xs1 = x1;// + w1;
			ys1 = y1;// + h1;
			xe1 = x1 + width1;// - w1;
			ye1 = y1 + height1;// - h1;

			Prex = x;
			Prey = y;

			if(s_bErase == FALSE)
			{
				if( Ball_Combine(p_stPCG, i, p_stPCG_other, j) != 0 )	/* ���̐��� */
				{
					ADPCM_Play(Mmin(S_Get_Combo() + 7 - 1, p_list_max - 1));	/* �A���� */
					s_bErase = TRUE;
					bEraseTmp = TRUE;
				}
				else
				{
					bEraseTmp = FALSE;
				}
			}
			else
			{
				bEraseTmp = FALSE;
			}

			if(bEraseTmp == FALSE)
			{
#if 0
				distance = APL_distance(cx, cy, cx1, cy1);
				distance_src = Mmin((w + w1), (h + h1));

				if(distance < distance_src)		/* �������߂� */
				{
					dy  =   0;
					dy1  =  0;
				}
				else
#endif
				{
					int16_t *pmx, *pmy;
					// �ړ���̌��̈ʒu
					int16_t mx[] = {  0, 4,-4, 0, 0 };	// �E�A���A��A���A�O
					int16_t my[] = {  4, 0, 0, 4, 0 };	// �E�A���A��A���A�O

					pmx = &mx[0];	/* �E�D�� */
					pmy = &my[0];	/* �E�D�� */

					distance_min = 0xFFFFFFFF;

					// �ړ���̌�������
					for(l = 0; l < 5; l++)
					{
						x += *(pmx + l) * w;
						y += *(pmy + l) * h;

						// �ړ��悪���̋�`�Əd�Ȃ��Ă��Ȃ����`�F�b�N
						distance = APL_distance(x + Mdiv2(width), y + Mdiv2(height), cx1, cy1);
						distance_src = Mmin((w + w1), (h + h1));
						distance_src *= distance_src;
						distance_src = (Mdiv2(distance_src) - Mdiv4(distance_src)); 

						if(distance > distance_src)		/* ���������ꂽ */
						{
							allRectanglesNonOverlapping = FALSE;	/* �I�[�o�[���b�v��� */

							if(distance < distance_min)
							{
								distance_min = distance;

								if( (g_stBall[i].bSize) == (g_stBall[j].bSize) )	/* �����T�C�Y */
								{
									if(cy < cy1)
									{
										dy   = -Y_ACC;
										dy1  =  Y_ACC;
									}
									else
									{
										dy  =   Y_ACC;
										dy1  = -Y_ACC;
									}

									if(cx < cx1)
									{
										dx   = -X_ACC;
										dx1  =  X_ACC;
									}
									else
									{
										dx   =  X_ACC;
										dx1  = -X_ACC;
									}
								}
								else if( (g_stBall[i].bSize) < (g_stBall[j].bSize) )	/* �T�C�Y���قȂ� */
								{
									if(cy < cy1)
									{
										dy   = -Y_ACC;
										dy1  =  0;
									}
									else
									{
										dy  =   Y_ACC;
										dy1  =  0;
									}

									if(cx < cx1)
									{
										dx   = -X_ACC;
										dx1  =  0;
									}
									else
									{
										dx   =  X_ACC;
										dx1  =  0;
									}
								}
								else	/* j���������� */
								{
									if(cy < cy1)
									{
										dy   =  0;
										dy1  =  Y_ACC;
									}
									else
									{
										dy   =  0;
										dy1  =  -Y_ACC;
									}

									if(cx < cx1)
									{
										dx   = 0;
										dx1  = X_ACC;
									}
									else
									{
										dx   = 0;
										dx1  = -X_ACC;
									}
								}
							}
						}
						else
						{
							/* �����Ȃ� */
							if( (g_stBall[i].bSize) < (g_stBall[j].bSize) )	/* �T�C�Y���قȂ� */
							{
								if(cy < cy1)
								{
									dy   = -Y_ACC;
									dy1  =  Y_ACC;
								}
								else
								{
									dy  =  -Y_ACC;
									dy1  =  0;
								}
							}
							else
							{
								if(cy < cy1)
								{
									dy1  = Y_ACC;
								}
								else
								{
									dy  = Y_ACC;
								}
							}
						}
						// �ʒu��߂�
						x = Prex;
						y = Prey;
					}
				}

				p_stPCG->x = x;
				p_stPCG->y = y;
				p_stPCG->dx = dx;
				p_stPCG->dy = dy;
				p_stPCG->update	= TRUE;	/* �`�� */
				Ball_Point_Update(p_stPCG, TRUE);	/* �q�b�g */

				p_stPCG_other->x = x1;
				p_stPCG_other->y = y1;
				p_stPCG_other->dx = dx1;
				p_stPCG_other->dy = dy1;
				p_stPCG_other->update	= TRUE;	/* �`�� */
				Ball_Point_Update(p_stPCG_other, TRUE);	/* �q�b�g */
			}

			if(g_ubGameClear == TRUE)
			{
				break;
			}
		}
	}

	for(k = 0; k < num_non_overlapping; k++)
	{
		i = non_overlapping[k];
		
		bValid = g_stBall[i].bValidty;
		if(	(bValid == ball_lost )	||	/* �����f�[�^ */
			(bValid == ball_now )	||	/* ���˗\��{�[�� */
			(bValid == ball_next )	)	/* ���̃{�[�� */
		{
			continue;
		}

		p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* �{�[�� */
		if(p_stPCG == NULL)
		{
			continue;
		}
		else
		{
			p_stPCG->update	= TRUE;	/* �`�� */

			if(Ball_Point_Update(p_stPCG, FALSE) != 0)	/* �q�b�g���Ȃ� */
			{
				if((nBallCount == i) || (nBallCount == -1))		/* �L���������{�[�� */
				{
					g_bShot_OK = TRUE;	/* ���̔��ˋ��� */
				}
			}
		}
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Ball_Check st3(%d)\n", count);
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Ball_Check ed(%d)\n", count);
//	count++;
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	memset(sBuf, 0, sizeof(sBuf));
//	sprintf(sBuf, "(%2d)(%d)(%d)(%d)", nBallCount, g_bShot_OK, num_pairs, num_non_overlapping);
//	Draw_Message_To_Graphic(sBuf, 0, 24, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
#endif
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Ball_Move(int16_t *nNum)
{
	int16_t i, j, k;
	int16_t pal;
	static int16_t s_bFlag = FALSE;
	static int16_t s_nNum_old = -1;
	static int16_t s_nNext_old = -1;
	static uint32_t s_TimeStamp;

	ST_PCG	*p_stPCG_ship = {NULL};
	ST_PCG	*p_stPCG_ball = {NULL};
	ST_PCG	*p_stPCG_next = {NULL};

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	int8_t sBuf[128];
#endif	/* DEBUG */
	for(i = 0; i < g_Ball_Table[1]; i++)
	{
		if(	g_stBall[i].bValidty == ball_now )	/* Now */
		{
			break;
		}
	}

	if(s_nNum_old != i)
	{
		s_nNum_old = i;
		p_stPCG_ball = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num );		/* �{�[�� */
		if(p_stPCG_ball != NULL)
		{
			pal = Mmul256(g_stST_PCG_LIST[g_stBall[i].bSize + 1].Pal);
			for(k=0; k < p_stPCG_ball->Pat_DataMax; k++)
			{
				*(p_stPCG_ball->pPatCodeTbl + k) &= 0xF0FF;
				*(p_stPCG_ball->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* �J���[�e�[�u�������Z�b�g */
			}
		}
	}
	else
	{
		i = s_nNum_old;
		p_stPCG_ball = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num );		/* �{�[�� */
	}
	g_index_now = i;	/* ���˗\��{�[�� */

	for(j = 0; j < g_Ball_Table[1]; j++)
	{
		if(	g_stBall[j].bValidty == ball_next )	/* ���̃f�[�^ */
		{
			break;
		}
		if(	g_stBall[j].bValidty == ball_lost )	/* �����f�[�^ */
		{
			break;
		}
	}

	if(s_nNext_old != j)
	{
		s_nNext_old = j;
		p_stPCG_next = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num );		/* �{�[�� */
		if(p_stPCG_next != NULL)
		{
			pal = Mmul256(g_stST_PCG_LIST[g_stBall[i].bSize + 1].Pal);
			for(k=0; k < p_stPCG_next->Pat_DataMax; k++)
			{
				*(p_stPCG_next->pPatCodeTbl + k) &= 0xF0FF;
				*(p_stPCG_next->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[j].bStatus]));	/* �J���[�e�[�u�������Z�b�g */
			}
		}
	}
	else
	{
		j = s_nNext_old;
		p_stPCG_next = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num );		/* �{�[�� */
	}
	g_index_next = j;	/* ���̃{�[���̔ԍ��ݒ� */
	if(p_stPCG_next != NULL)
	{
		p_stPCG_next->x = 224;
		p_stPCG_next->y = 16;
		p_stPCG_next->update	= TRUE;
	}

	if( (i == *nNum) || (j == *nNum) )	/* �����\��{�[���Ɣ�����ꍇ */
	{
		*nNum = -1;
		g_bShot_OK = TRUE;
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	memset(sBuf, 0, sizeof(sBuf));
//	sprintf(sBuf, "i(%2d)j(%2d)in(%2d)", s_nNum_old, s_nNext_old, *nNum);
//	Draw_Message_To_Graphic(sBuf, 0, 112, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
#endif

	if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
		((g_Input & KEY_A) != 0u)		||		/* A */
		((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
	{
		if(s_bFlag == FALSE)
		{
			s_bFlag = TRUE;

			if(	GetPassTime(PLAYER_INPUT_INTERVAL_TIME, &s_TimeStamp) != 0u)	/* 800ms�o�� */
			{
				if(g_bShot_OK == TRUE)
				{
					g_bShot_OK = FALSE;

					if(	g_stBall[i].bValidty == ball_now )	/* ���˗\��{�[�� */
					{
						g_stBall[j].bValidty = ball_now;
						g_stBall[i].bValidty = ball_exist;

						*nNum = i;			/* ���݂̃{�[���ԍ��ݒ� */
						s_nNum_old = s_nNext_old;

						if(p_stPCG_ball != NULL)
						{
							p_stPCG_ball->dx = 0;
							p_stPCG_ball->dy = Y_ACC;
							p_stPCG_ball->update = TRUE;
						}
						ADPCM_Play(6);	/* shot */
					}
				}
			}
		}
	}
	else
	{
		s_bFlag = FALSE;
	}
#if 0		
		printf("0x%X, 0x%X\n", g_Input_P[0], g_Input);
#endif
//	PCG_PUT_1x1(p_stPCG_ship->x, p_stPCG_ship->y, nNum, SetXSPinfo(0, 0, 0x0A, 0x30));
	
	p_stPCG_ship = PCG_Get_Info(SP_SHIP_0);				/* ���@ */
	if(p_stPCG_ship != NULL)
	{
		if(	((g_Input_P[0] & JOY_LEFT ) != 0u )	||	/* LEFT */
			((g_Input & KEY_LEFT) != 0 )		)	/* �� */
		{
			if(	((g_Input_P[0] & JOY_B ) != 0u)	||		/* B */
				((g_Input & KEY_B) != 0u)		)		/* B */
			{
				p_stPCG_ship->dx = -2;
			}
			else
			{
				p_stPCG_ship->dx = -1;
			}
			/* �A�j���[�V���� */
			p_stPCG_ship->Anime++;
			if(p_stPCG_ship->Anime >= p_stPCG_ship->Pat_AnimeMax)
			{
				p_stPCG_ship->Anime = 0;
			}
		}
		else if( ((g_Input_P[0] & JOY_RIGHT ) != 0u )	||	/* RIGHT */
				((g_Input & KEY_RIGHT) != 0 )			)	/* �E */
		{
			if(	((g_Input_P[0] & JOY_B ) != 0u)	||		/* B */
				((g_Input & KEY_B) != 0u)		)		/* B */
			{
				p_stPCG_ship->dx = 2;
			}
			else
			{
				p_stPCG_ship->dx = 1;
			}
			/* �A�j���[�V���� */
			p_stPCG_ship->Anime++;
			if(p_stPCG_ship->Anime >= p_stPCG_ship->Pat_AnimeMax)
			{
				p_stPCG_ship->Anime = 0;
			}
		}
		else
		{
			p_stPCG_ship->dx = 0;
			/* �A�j���[�V���� */
			p_stPCG_ship->Anime = 0;
		}
		p_stPCG_ship->dy = 0;

		p_stPCG_ship->x += p_stPCG_ship->dx;
		p_stPCG_ship->y += p_stPCG_ship->dy;
		if(p_stPCG_ship->x < X_POS_MIN)
		{
			p_stPCG_ship->x = X_POS_MIN;
		}
		if(p_stPCG_ship->x > X_POS_MAX - 16)
		{
			p_stPCG_ship->x = X_POS_MAX - 16;
		}
		p_stPCG_ship->update	= TRUE;

		if(p_stPCG_ball != NULL)
		{
			p_stPCG_ball->x = p_stPCG_ship->x;
			p_stPCG_ball->y = p_stPCG_ship->y + Mmul16(p_stPCG_ship->Pat_h);
			p_stPCG_ball->update	= TRUE;
		}
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Ball_InitALL(void)
{
	int16_t	i;
	
	ST_PCG	*p_stPCG = NULL;

	for (i = 0; i < BALL_MAX; i++) {
		g_stBall[i].bSize = 0;
		g_stBall[i].bSP_num = i;
		g_stBall[i].bStatus = 0;
		g_stBall[i].bValidty = ball_lost;
	}

	for(i = 0; i < PCG_NUM_MAX; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* �L���� */

		if(p_stPCG != NULL)
		{
			p_stPCG->x = 0;
			p_stPCG->y = 0;
			p_stPCG->dx = 0;
			p_stPCG->dy = 0;
			p_stPCG->update	= FALSE;
		}
	}
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Ball_Init(void)
{
	int16_t	i, j;
	int16_t	pal;
	
	ST_PCG	*p_stPCG = NULL;

	for (i = 0; i < BALL_MAX; i++) {
		g_stBall[i].bSize = 0;
		g_stBall[i].bSP_num = i;
		g_stBall[i].bStatus = (rand() % 3);
		g_stBall[i].bValidty = ball_lost;
	}

	if(g_nGameDifflevel == 0)
	{
	}
	else
	{
		for (i = 0; i < (SP_BALL_S_24 - SP_BALL_S_1); i++) {
			g_stBall[i].bValidty = ball_exist;
		}
	}
	g_stBall[1].bValidty = ball_next;	/* �� */
	g_stBall[0].bValidty = ball_now;	/* ���� */

	g_index = 0;
	g_index_next = 1;

	for(i = 0; i < PCG_NUM_MAX; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* �L���� */

		if(p_stPCG != NULL)
		{
			switch(i)
			{
				case SP_SHIP_0:
				{
					p_stPCG->x = 128;
					p_stPCG->y = 16;
					p_stPCG->dx = 0;
					p_stPCG->dy = 0;
					p_stPCG->update	= TRUE;
					break;
				}
				case SP_GAL_0:
				{
					p_stPCG->x = 208;
					p_stPCG->y = Y_POS_BD;
					p_stPCG->dx = 0;
					p_stPCG->dy = 0;
					p_stPCG->update	= TRUE;
					break;
				}
				case SP_ARROW_0:
				{
					p_stPCG->x = X_POS_MIN - 16;
					p_stPCG->y = g_Boder_line - 8;
					p_stPCG->dx = 0;
					p_stPCG->dy = 0;
					p_stPCG->update	= TRUE;
					break;
				}
				default:
				{
					if(g_nGameDifflevel == 0)
					{
						p_stPCG->x = 128;
						p_stPCG->y = Y_POS_BD;
						p_stPCG->dx = 0;
						p_stPCG->dy = 0;
					}
					else
					{
						if(g_stBall[i].bValidty == ball_exist)
						{
							p_stPCG->x = X_POS_MIN + (rand() % 64);
							p_stPCG->y = Y_POS_BD + 64 + (rand() % 64);
							p_stPCG->dx = 0;
							p_stPCG->dy = Y_ACC2;

							g_index++;
							g_index_next++;
						}
						else
						{
							p_stPCG->x = 128;
							p_stPCG->y = Y_POS_BD;
							p_stPCG->dx = 0;
							p_stPCG->dy = 0;
						}
					}

					pal = Mmul256(g_stST_PCG_LIST[g_stBall[i].bSize + 1].Pal);
					for(j=0; j < p_stPCG->Pat_DataMax; j++)
					{
						*(p_stPCG->pPatCodeTbl + j) &= 0xF0FF;
						*(p_stPCG->pPatCodeTbl + j) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* �J���[�e�[�u�������Z�b�g */
					}
					if(g_stBall[i].bValidty != ball_lost)
					{
						p_stPCG->update	= TRUE;
					}
					else
					{
						p_stPCG->update	= FALSE;
					}
					break;
				}
			}
#if 0
			if((p_stPCG->dx == 0) && (p_stPCG->dy == 0))
			{
				int16_t sign;

				sign = ((rand() % 2) == 0)?1:-1;
				p_stPCG->dx = sign;
				sign = ((rand() % 2) == 0)?1:-1;
				p_stPCG->dy = sign;
			}
#endif			
		}
	}
	g_index_now = g_index;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t main(int16_t argc, int8_t** argv)
{
	int16_t	ret = 0;

	int16_t	rank = -1;
	uint16_t	uFreeRunCount=0u;
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	uint32_t	unTime_cal = 0u;
	uint32_t	unTime_cal_PH = 0u;
#endif

	int16_t	loop;
	int16_t	pushCount = 0;

	uint8_t	bMode;
	
	ST_TASK		stTask = {0}; 
	ST_CRT		stCRT;
	
	/* �ϐ��̏����� */
#ifdef W_BUFFER
	SetGameMode(1);
#else
	SetGameMode(0);
#endif	
	/* COPY�L�[������ */
	init_trap12();
	/* ��O�n���h�����O���� */
	usr_abort = App_exit;	/* ��O�����ŏI�����������{���� */
	init_trap14();			/* �f�o�b�O�p�v���I�G���[�n���h�����O */
#if 0	/* �A�h���X�G���[���� */
	{
		char buf[10];
		int *A = (int *)&buf[1];
		int B = *A;
		return B;
	}
#endif

	App_Init();	/* ������ */
	
	loop = 1;
	
	/* ���� */
	srandom(TIMEGET());	/* �����̏����� */
	
	do	/* ���C�����[�v���� */
	{
		uint32_t time_st;
#if 0
		ST_CRT	stCRT;
#endif
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		uint32_t time_now;
		GetNowTime(&time_now);
#endif

#ifdef W_BUFFER	/* �f�o�b�O�R�[�i�[ */
		/* �����N���A */
		G_CLR_AREA(	stCRT.hide_offset_x + X_POS_MIN, X_POS_W,
					stCRT.hide_offset_y + Y_POS_BD, Y_POS_H, 0);
#endif

#ifdef DEBUG
//		Draw_Box(	stCRT.hide_offset_x + X_POS_MIN - 1,
//					stCRT.hide_offset_y + Y_POS_BD + 1,
//					stCRT.hide_offset_x + X_POS_MAX - 1,
//					stCRT.hide_offset_y + g_TB_GameLevel[g_nGameLevel] + 1,
//					G_PURPLE, 0xFFFF);
#endif
		PCG_START_SYNC(g_Vwait);	/* �����J�n */

		/* �I������ */
		if(loop == 0)
		{
			break;
		}
		
		/* �����ݒ� */
		GetNowTime(&time_st);	/* ���C�������̊J�n�������擾 */
		SetStartTime(time_st);	/* ���C�������̊J�n�������L�� */
		
		/* �^�X�N���� */
		TaskManage();				/* �^�X�N���Ǘ����� */
		GetTaskInfo(&stTask);		/* �^�X�N�̏��𓾂� */

		/* ���[�h�����n�� */
		GetGameMode(&bMode);
		GetCRT(&stCRT, bMode);	/* ��ʍ��W�擾 */

		if(Get_SP_Sns() == FALSE)
		{
			if(Input_Main(g_ubDemoPlay) != 0u) 	/* ���͏��� */
			{
				g_ubDemoPlay = FALSE;	/* �f������ */
				SetTaskInfo(SCENE_INIT);	/* �V�[��(����������)�֐ݒ� */
			}
		}
		
		if((g_Input & KEY_b_ESC ) != 0u)	/* �d�r�b�L�[ */
		{
			/* �I�� */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 5)
			{
				ADPCM_Play(1);	/* �I���� */

				Music_Stop();	/* Stop */

				SetTaskInfo(SCENE_EXIT);	/* �I���V�[���֐ݒ� */
			}
		}
		else if((g_Input & KEY_b_HELP ) != 0u)	/* HELP�L�[ */
		{
			if( (stTask.bScene != SCENE_GAME_OVER_S) && (stTask.bScene != SCENE_GAME_OVER) )
			{
				/* ���Z�b�g */
				pushCount = Minc(pushCount, 1);
				if(pushCount >= 5)
				{
					ADPCM_Play(1);	/* �I���� */

					Music_Stop();	/* Stop */

					SetTaskInfo(SCENE_GAME_OVER_S);	/* �I���V�[���֐ݒ� */
				}
			}
		}		
		else
		{
			pushCount = 0;
		}

		if(g_ubDemoPlay == TRUE)	/* �f�� */
		{
			static int8_t s_bFlip = FALSE;

			if(	GetPassTime(DEMO_LOGO_INTERVAL_TIME, &s_PassTime) != 0u)	/* 1000ms�o�� */
			{
				if(s_bFlip == FALSE)
				{
					BG_TextPut("          ", 88, 128);
					s_bFlip = TRUE;
				}
				else
				{
					BG_TextPut("DEMO PLAY!", 88, 128);
					s_bFlip = FALSE;
				}
			}

			if(	GetPassTime(DEMO_START_END_TIME, &s_DemoTime) != 0u)	/* 30s�o�� */
			{
				SetTaskInfo(SCENE_GAME_OVER_S);		/* �Q�[���I�[�o�[�V�[��(�J�n����)�֐ݒ� */
			}
		}

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* �������V�[�� */
			{
				int16_t i, j;
				int16_t temp;
				int8_t sPath[64];
				memset(sPath, 0, sizeof(sPath));

				Set_CRT_Contrast(0);	/* �^���Âɂ��� */

				// �����̏�����
				srand(time(NULL));

				// Fisher-Yates�A���S���Y�����g�p���Đ������V���b�t��
				for (i = (BALL_MAX - 1); i > 0; --i) {
					j = rand() % (i + 1);
					// ����������
					temp = g_numbers[i];
					g_numbers[i] = g_numbers[j];
					g_numbers[j] = temp;
					g_Sign_Table[i] = ((rand() % 2) == 0)?-1:1;	/* �������� */
					g_Rand_Table4[i]  = rand() % 4;		/* �����e�[�u�� */
					g_Rand_Table8[i]  = rand() % 8;		/* �����e�[�u�� */
					g_Rand_Table16[i] = rand() % 16;	/* �����e�[�u�� */
					g_Rand_Table32[i] = Mmax((rand() % 32), 16);	/* �����e�[�u�� */
				}

				BG_put_Clr(0, 0, 0, 512, 512);	/* BG0 �N���A */
				BG_put_Clr(1, 0, 0, 512, 512);	/* BG1 �N���A */
				sprintf(sPath, "%s\\sp\\map.csv", Get_DataList_Path());
				BG_TEXT_SET(sPath, BG_P1, BG_MAP_PAL, 256,   0);	/* �}�b�v�f�[�^�ɂ��a�f�̔z�u */
				BG_put_Clr(1, 256 + X_POS_MIN-2, Y_POS_MAX, 256 + X_POS_MAX+2, g_TB_GameLevel[0]);
				BG_TEXT_SET(sPath, BG_P1, BG_MAP_PAL,   0, 256);	/* �}�b�v�f�[�^�ɂ��a�f�̔z�u */
				BG_put_Clr(1, X_POS_MIN-2, 256 + Y_POS_MAX, X_POS_MAX+2, 256 + g_TB_GameLevel[1]);
				BG_TEXT_SET(sPath, BG_P1, BG_MAP_PAL, 256, 256);	/* �}�b�v�f�[�^�ɂ��a�f�̔z�u */

				SetTaskInfo(SCENE_TITLE_S);	/* �V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_TITLE_S:
			{
				Music_Play(3);	/* Title */

				G_VIDEO_PRI(2);	/* TX>GR>SP */

				PCG_OFF();		/* SP OFF */
				
				G_CLR();		/* �O���t�B�b�N�N���A */
				
				T_Clear();		/* �e�L�X�g�N���A */

				G_Load(1, 0, 0, 0);	/* No.1�F�^�C�g�� */

//				PutGraphic_To_Symbol12("PUSH A BUTTON TO START!", X_POS_MIN, 192, F_MOJI );			/* title��ʓ��͑҂� */
				BG_TextPut("PUSH A BUTTON TO START!", 36, 224);
//				PutGraphic_To_Symbol12("VERSION 0.0.4", X_POS_MAX, 244, F_MOJI );	/* Ver */
				BG_TextPut("VER0.0.6", 192, 244);

				GetNowTime(&s_PassTime);	/* �^�C���A�E�g�J�E���^���Z�b�g */
				GetNowTime(&s_DemoTime);	/* �f���J�n�J�E���^���Z�b�g */
				g_ubDemoPlay = FALSE;		/* �f���ȊO */

				Set_CRT_Contrast(-1);	/* ���Ƃɖ߂� */

				SetTaskInfo(SCENE_TITLE);	/* �V�[��(����)�֐ݒ� */
				break;
			}
			case SCENE_TITLE:
			{
				static int8_t s_bFlagInputAB = FALSE;
#if 1
				static int8_t s_bFlip = FALSE;
#endif
				if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
					((g_Input & KEY_A) != 0u)		||		/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
				{
					if(s_bFlagInputAB == FALSE)
					{
						s_bFlagInputAB = TRUE;

						ADPCM_Play(4);	/* ���艹 */

						SetTaskInfo(SCENE_TITLE_E);	/* �V�[��(�I������)�֐ݒ� */
					}
				}
				else /* �Ȃ� */
				{
					s_bFlagInputAB = FALSE;
				}
#if 1
				if(	GetPassTime(START_LOGO_INTERVAL_TIME, &s_PassTime) != 0u)	/* 1000ms�o�� */
				{
					if(s_bFlip == FALSE)
					{
						BG_TextPut("                       ", 36, 224);
						s_bFlip = TRUE;
					}
					else
					{
						BG_TextPut("PUSH A BUTTON TO START!", 36, 224);
						s_bFlip = FALSE;
					}
				}
				else
				{
					/* �������Ȃ� */
				}
#endif
				if(	GetPassTime(DEMO_START_END_TIME, &s_DemoTime) != 0u)	/* 30s�o�� */
				{
					g_ubDemoPlay = TRUE;	/* �f���J�n */
					SetTaskInfo(SCENE_TITLE_E);	/* �V�[��(�I������)�֐ݒ� */
				}
				break;
			}
			case SCENE_TITLE_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					G_CLR();		/* �O���t�B�b�N�N���A */
					T_Clear();		/* �e�L�X�g�N���A */
					Music_Play(1);	/* Stop */

					SetTaskInfo(SCENE_START_S);	/* �V�[��(�J�n����)�֐ݒ� */
				}
				break;
			}
			case SCENE_START_S:	/* �V�[��(�J�n����) */
			{
				int16_t i;
				int16_t y;

				if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
				{
					Music_Play(7);	/* Config */
				}
				G_PaletteSetZero();	/* 0�ԃp���b�g�ύX */
//				G_PAGE_SET(0b0001);	/* GR0 */
//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW, Y_MAX_DRAW, G_BG);	/* �h��Ԃ� */

				/* �T�C�h�^�C�g�� */
				G_PAGE_SET(0b0001);	/* GR0 */
				G_Load_Mem(2, 0, 48, 0);	/* No.2�F�Z�b�e�B���O�� */

				_iocs_window( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1);	/* �`��͈͐ݒ� */
				/* �F����� */
				G_PAGE_SET(0b0010);	/* GR1 */
				for (i = 0; i < NUM_STARS; ++i)
				{
					Draw_Pset(rand() % (X_MAX_DRAW/2), rand() % Y_MAX_DRAW, (i % 16) + 240);
				}
				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);		/* �r���̊O�͓h��Ԃ� */
				Draw_Fill(X_POS_MAX, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);	/* �r���̊O�͓h��Ԃ� */

				memcpy((int16_t *)0xE821F0, (int16_t *)0xE82000, sizeof(int16_t) * 16);

				T_Line2( X_POS_MIN, Y_POS_BD, X_POS_MAX, Y_POS_BD, 0x1, T_RED);
				y = g_TB_GameLevel[g_nGameLevel];
				T_Line2( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MIN,          y-1, X_POS_MAX,   y-1, 0xFFFF, T_GREEN);
				
				G_PAGE_SET(0b0001);	/* GR0 */
				
				PutGraphic_To_Symbol12("�㉺", X_POS_MIN+2, 128+12*0, F_MOJI );			/* ���� */
				PutGraphic_To_Symbol12(" �T�C�Y�ύX", X_POS_MIN+2, 128+12*1, F_MOJI );	/* ���� */
				PutGraphic_To_Symbol12("���E", X_POS_MIN+2, 128+12*2, F_MOJI );			/* ���� */
				PutGraphic_To_Symbol12(" ������ԕύX", X_POS_MIN+2, 128+12*3, F_MOJI );	/* ���� */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

				Ball_InitALL();	/* �{�[���̑S������ */

				G_VIDEO_PRI(2);	/* TX>GR>SP */
				PCG_ON();		/* SP ON */
				BG_ON(0);
				BG_ON(1);

				SetTaskInfo(SCENE_START);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_START:	/* �V�[��(���{����) */
			{
				static int8_t s_bFlagInput = FALSE;
				static int8_t s_bFlagInputAB = FALSE;
				int16_t y;

				if( ( g_Input & KEY_UPPER ) != 0) { /* �� */
					if(s_bFlagInput == FALSE)
					{
						s_bFlagInput = TRUE;

						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, 0xFFFF, T_BLACK1);

						g_nGameLevel++;
						if(g_nGameLevel > 2)
						{
							g_nGameLevel = 0;
						}
						else
						{
							/* �������Ȃ� */
						}
						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN+1,          y-1, X_POS_MAX,   y-1, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN-2,    Y_POS_MAX, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN,      Y_POS_MAX, X_POS_MIN,     y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX+2,    Y_POS_MAX, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX,      Y_POS_MAX, X_POS_MAX,     y, 0xFFFF, T_GREEN);

						ADPCM_Play(0);	/* SELECT�� */
					}
				}
				else if( ( g_Input & KEY_LOWER ) != 0 ) { /* �� */
					if(s_bFlagInput == FALSE)
					{
						s_bFlagInput = TRUE;

						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, 0xFFFF, T_BLACK1);
						T_Line2( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, 0xFFFF, T_BLACK1);

						g_nGameLevel--;
						if(g_nGameLevel < 0)
						{
							g_nGameLevel = 2;
						}
						else
						{
							/* �������Ȃ� */
						}
						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, 0xFFFF, T_GREEN);

						ADPCM_Play(0);	/* SELECT�� */
					}
				}
				else if( ( g_Input & KEY_LEFT ) != 0)	/* �� */
				{
					if(s_bFlagInput == FALSE)
					{
						s_bFlagInput = TRUE;

						g_nGameDifflevel--;
						if(g_nGameDifflevel < 0)
						{
							g_nGameDifflevel = 1;
						}
						Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);
					}
				}
				else if( ( g_Input & KEY_RIGHT ) != 0)	/* �E */
				{
					if(s_bFlagInput == FALSE)
					{
						s_bFlagInput = TRUE;

						g_nGameDifflevel++;
						if(g_nGameDifflevel >= 2)
						{
							g_nGameDifflevel = 0;
						}
						Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);
					}
				}
				else /* �Ȃ� */
				{
					s_bFlagInput = FALSE;
				}

				if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
					((g_Input & KEY_A) != 0u)		||		/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
				{
					if(s_bFlagInputAB == FALSE)
					{
						s_bFlagInputAB = TRUE;

						ADPCM_Play(4);	/* ���艹 */

						SetTaskInfo(SCENE_START_E);	/* �V�[��(�I������)�֐ݒ� */
					}
				}
				else /* �Ȃ� */
				{
					s_bFlagInputAB = FALSE;
				}

				switch(g_nGameLevel)
				{
					case 0:
					{
						BG_Scroll(1, 256,   0);
						break;
					}
					case 1:
					{
						BG_Scroll(1,   0, 256);
						break;
					}
					case 2:
					{
						BG_Scroll(1, 256, 256);
						break;
					}
					default:
					{
						break;
					}
				}
				break;
			}
			case SCENE_START_E:	/* �V�[��(�I������) */
			{
				G_PAGE_SET(0b0001);	/* GR0 */

				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1, G_BG);	/* ���b�Z�[�W���� */

				G_Load_Mem(3, 0, 48, 0);	/* No.3�F�Q�[���� */

				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_POS_BD, G_BG);	/* �r���̊O�͓h��Ԃ� */
//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);		/* �r���̊O�͓h��Ԃ� */
				Draw_Fill(X_POS_MIN, Y_MIN_DRAW, X_POS_MAX, Y_MAX_DRAW-1, G_BG);		/* �r���̒��͓h��Ԃ� */
				Draw_Fill(X_POS_MAX, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);	/* �r���̊O�͓h��Ԃ� */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

#if 0	/* �f�o�b�O�R�[�i�[ */
				memset(sBuf, 0, sizeof(sBuf));
				sprintf(sBuf, "FPS(%3d )", g_bFPS_PH);
				Draw_Message_To_Graphic(sBuf, 0, 24, F_MOJI, F_MOJI_BAK);	/* �f�o�b�O */
#endif
				/* �Q�[�������̏����� */
				g_Boder_line = 0xFF;
				g_Boder_count = 0;
				g_ubGameClear = FALSE;
				g_nGameClearCount = 0;

				Ball_Init();	/* �{�[���̏����� */

				S_Init_Score();	/* �X�R�A������ */
				
				SetTaskInfo(SCENE_GAME1_S);	/* �V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_GAME1_S:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
					{
						Music_Play(4);	/* Play */
					}

					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */
	
					SetTaskInfo(SCENE_GAME1);	/* �V�[��(���{����)�֐ݒ� */
				}
				break;
			}
			case SCENE_GAME1:
			{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(TRUE);	/* �������׌v�� */
#endif
				Ball_Move(&g_index);

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
//				printf("Ball_Check st(%d)\n", g_index);
				Ball_Check(g_index);		/* �X�v���C�g��\�� */
//				printf("Ball_Check ed(%d)\n", g_index);
				
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
//				printf("Boder_line st(%d)\n", g_index);

				Boder_line();	/* �{�[�_�[���C�� */

//				printf("Boder_line ed(%d)\n", g_index);

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
//				printf("Set_Pallet_Shift st(%d)\n", g_index);

				/* �w�i���� */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

//				printf("Set_Pallet_Shift ed(%d)\n", g_index);

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
//				printf("S_Main_Score st(%d)\n", g_index);

				S_Main_Score();	/* �X�R�A���C������ */

//				printf("S_Main_Score ed(%d)\n", g_index);
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif

				if(g_Boder_line < Y_POS_BD + 48)
				{
					if(S_IsUpdate_Combo() != 0)
					{
						if(S_Get_Combo() >= COMBO_RECOVER_COUNT)
						{
							g_Boder_count = Mdec(g_Boder_count, S_Get_Combo());
						}
					}
					else
					{
						g_Boder_count = Minc(g_Boder_count, 1);
					}

					if(g_Boder_count >= 64)
					{
						SetTaskInfo(SCENE_GAME1_E);	/* �V�[��(�I������)�֐ݒ� */
					}
				}
				else if(g_Boder_line < Y_POS_BD + 64)
				{
					/* �ێ� */
				}
				else
				{
					g_Boder_count = 0;
				}

				if(g_ubGameClear == TRUE)	/* �N���A�������� */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* �G���f�B���O(�J�n����)�֐ݒ� */
				}
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
				break;
			}
			case SCENE_GAME1_E:
			{
				SetTaskInfo(SCENE_GAME2_S);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_GAME2_S:
			{
				if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
				{
					Music_Play(5);	/* Play2 */
				}

				_iocs_tpalet2( 7, SetRGB(31,  0,  0));	/* Red */

				SetTaskInfo(SCENE_GAME2);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_GAME2:
			{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(TRUE);	/* �������׌v�� */
#endif
				Ball_Move(&g_index);

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
				Ball_Check(g_index);		/* �X�v���C�g��\�� */
				
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
				Boder_line();	/* �{�[�_�[���C�� */

				/* �w�i���� */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				S_Main_Score();	/* �X�R�A���C������ */

				if(g_Boder_line < Y_POS_BD)
				{
					if(S_IsUpdate_Combo() != 0)
					{
						if(S_Get_Combo() >= COMBO_RECOVER_COUNT)
						{
							g_Boder_count = Mdec(g_Boder_count, S_Get_Combo());
						}
					}
					else
					{
						g_Boder_count = Minc(g_Boder_count, 1);
					}

					{
						ST_PCG	*p_stPCG = NULL;
						p_stPCG  = PCG_Get_Info(SP_GAL_0);		/* �M���� */
						if(p_stPCG != NULL)
						{
							int16_t height;
							height	= Mmul16(p_stPCG->Pat_h);
							if(g_Boder_count >= (256 - height))
							{
								SetTaskInfo(SCENE_GAME2_E);	/* �V�[��(�I������)�֐ݒ� */
							}
						}
					}
				}
				else if(g_Boder_line < Y_POS_BD + 64)
				{
					/* �ێ� */
				}
				else
				{
					g_Boder_count = Mdec(g_Boder_count, 1);
					if(g_Boder_count == 0)
					{
						SetTaskInfo(SCENE_GAME1_S);	/* �V�[��(�J�n����)�֐ݒ� */
					}
				}

				if(g_ubGameClear == TRUE)	/* �N���A�������� */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* �G���f�B���O(�J�n����)�֐ݒ� */
				}
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				Level_meter(FALSE);	/* �������׌v�� */
#endif
				break;
			}
			case SCENE_GAME2_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					ADPCM_Play(5);	/* SET */

					if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
					{
						SetTaskInfo(SCENE_HI_SCORE_S);	/* �V�[��(���{����)�֐ݒ� */
					}
					else
					{
						SetTaskInfo(SCENE_GAME_OVER_S);		/* �Q�[���I�[�o�[�V�[��(�J�n����)�֐ݒ� */
					}
				}
				break;
			}
			case SCENE_NEXT_STAGE_S:	/* �G���f�B���O(�J�n����)  */
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					uint8_t sBuf[8];

					BG_TextPut("                       ", 44, 128);
					BG_TextPut("   CONGRATULATIONS!    ", 44, 128);
					BG_TextPut("                       ", 44, 152);
					BG_TextPut("    PUSH A BUTTON!     ", 44, 152);

					S_GetPos(44, 136);		/* �X�R�A�\�����W�X�V */
					S_Add_Score(1000000);	/* �X�R�A�X�V */

					g_nGameClearCount++;

					memset(sBuf, 0, sizeof(sBuf));
					sprintf(sBuf, "COUNT(%d)", g_nGameClearCount);
					Draw_Message_To_Graphic(sBuf, 0, 20, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */

					ADPCM_Play(2);	/* �Q�[���N���A�� */

					SetTaskInfo(SCENE_NEXT_STAGE);	/* �G���f�B���O(����)�֐ݒ� */
				}
				break;
			}
			case SCENE_NEXT_STAGE:		/* �G���f�B���O(����)  */
			{
				static int8_t s_bFlagInputAB = FALSE;

				Ball_Check(g_index);		/* �X�v���C�g��\�� */

				/* �w�i���� */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
					((g_Input & KEY_A) != 0u)		||		/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
				{
					if(s_bFlagInputAB == FALSE)
					{
						int16_t y;

						s_bFlagInputAB = TRUE;

						if( g_ubGameClear == TRUE)
						{
							g_ubGameClear = FALSE;
						}

						BG_TextPut("                       ", 44, 128);
						BG_TextPut("                       ", 44, 152);

						ADPCM_Play(4);	/* ���艹 */

						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, 0xFFFF, T_GREEN);

						SetTaskInfo(SCENE_NEXT_STAGE_E);	/* �G���f�B���O(�I������)�֐ݒ� */
					}
				}
				else /* �Ȃ� */
				{
					s_bFlagInputAB = FALSE;
				}
				break;
			}
			case SCENE_NEXT_STAGE_E:	/* �G���f�B���O(�I������)  */
			{
				g_Boder_count = 0;

				SetTaskInfo(SCENE_GAME1_S);	/* �V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_HI_SCORE_S:	/* �n�C�X�R�A�����L���O�V�[��(�J�n����) */
			{
				G_VIDEO_PRI(0);	/* GR>TX>SP */

				rank = S_Score_Board();	/* �n�C�X�R�A�\�� */
				
				if(rank >= 0)
				{
					Music_Play(8);	/* Hi-SCORE */
				}
				else
				{
					Music_Play(9);	/* Ranking view */
				}

				SetTaskInfo(SCENE_HI_SCORE);	/* �n�C�X�R�A�����L���O�V�[���֐ݒ� */
				break;
			}
			case SCENE_HI_SCORE:	/* �n�C�X�R�A�����L���O�V�[�� */
			{
				if(rank >= 0)
				{
					if(S_Score_Name_Main(g_Input, rank) >= 3)	/* �X�R�A�l�[������ */
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* �n�C�X�R�A�����L���O�V�[��(�I������)�֐ݒ� */
					}
				}
				else
				{
					if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
						((g_Input & KEY_A) != 0u)		||		/* A */
						((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
					{
						ADPCM_Play(4);	/* ���艹 */
						SetTaskInfo(SCENE_HI_SCORE_E);	/* �n�C�X�R�A�����L���O�V�[��(�I������)�֐ݒ� */
					}
				}
				
				/* �w�i���� */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				break;
			}
			case SCENE_HI_SCORE_E:	/* �n�C�X�R�A�����L���O�V�[��(�I������) */
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					SetTaskInfo(SCENE_GAME_OVER_S);		/* �Q�[���I�[�o�[�V�[��(�J�n����)�֐ݒ� */
				}
				break;
			}
			case SCENE_GAME_OVER_S:
			{
				int16_t x, y;

				T_Clear();	/* �e�L�X�g�N���A */
				BG_Scroll(1, 0, 0);	/* BG1 �����Ȃ��G���A */
				PCG_OFF();		/* SP OFF */
			
				if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
				{
					Music_Play(6);	/* Game Over */
				}
				else
				{
					S_Score_Board();	/* �n�C�X�R�A�\�� */
					GetNowTime(&s_DemoTime);	/* �f���J�n�J�E���^���Z�b�g */
				}

				x = stCRT.view_offset_x + 76;
				y = stCRT.view_offset_y + 0;
				Draw_Fill( x, y + 6, x + 12*9, y + 24*1 + 6, G_BLACK);	/* �h��Ԃ� */
				Draw_Box( x, y + 6, x + 12*9, y + 24*1 + 6, G_D_RED, 0xFFFF);	/* �g */
				PutGraphic_To_Symbol24("GAME OVER", x + 1, y + 1 + 6, G_RED);
				PutGraphic_To_Symbol24("GAME OVER", x, y + 6, G_L_PINK);

				SetTaskInfo(SCENE_GAME_OVER);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_GAME_OVER:
			{
				/* �w�i���� */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
				{
					if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* A�{�^�� */
						((g_Input & KEY_A) != 0u)		||	/* KEY A */
						((g_Input & KEY_b_SP ) != 0u)	)	/* �X�y�[�X�L�[ */
					{
						ADPCM_Play(4);	/* ���艹 */
						SetTaskInfo(SCENE_GAME_OVER_E);	/* �V�[��(�I������)�֐ݒ� */
					}
				}
				else
				{
					if(	GetPassTime(DEMO_GAMEOVER_TIME, &s_DemoTime) != 0u)	/* 5000ms�o�� */
					{
						SetTaskInfo(SCENE_GAME_OVER_E);	/* �V�[��(�I������)�֐ݒ� */
					}
				}

				break;
			}
			case SCENE_GAME_OVER_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */

					SetTaskInfo(SCENE_INIT);	/* �V�[��(����������)�֐ݒ� */
				}
				break;
			}
			default:	/* �ُ�V�[�� */
			{
				loop = 0;	/* ���[�v�I�� */
				break;
			}
		}
		SetFlip(TRUE);	/* �t���b�v���� */

		PCG_END_SYNC(g_Vwait);	/* �X�v���C�g�̏o�� */

		uFreeRunCount++;	/* 16bit �t���[�����J�E���^�X�V */

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		/* �������Ԍv�� */
		GetNowTime(&time_now);
		unTime_cal = time_now - time_st;	/* LSB:1 UNIT:ms */
		g_unTime_cal = unTime_cal;
		if( stTask.bScene == SCENE_GAME1 )
		{
			unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);
			g_unTime_cal_PH = unTime_cal_PH;
		}
		GetFreeMem();	/* �󂫃������T�C�Y�m�F */
#endif
	}
	while( loop );

	g_bExit = FALSE;
	App_exit();	/* �I������ */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void App_Init(void)
{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �J�n");
	g_nMaxUseMemSize = GetFreeMem();
	printf("FreeMem(%d[kb])\n", g_nMaxUseMemSize);	/* �󂫃������T�C�Y�m�F */
	puts("App_Init ������");
#endif

	puts("App_Init FileList");
	Init_FileList_Load();	/* ���X�g�t�@�C���̓ǂݍ��� */

	puts("App_Init Music");
	/* ���y */
	Init_Music();	/* ������(�X�[�p�[�o�C�U�[���[�h���O)	*/

	Music_Play(0);	/* Init */
	Music_Play(1);	/* Stop */
	Music_Play(2);	/* Loading */

	/* �X�[�p�[�o�C�U�[���[�h�J�n */
	g_nSuperchk = _iocs_b_super(0);
	if( g_nSuperchk < 0 ) {
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		puts("App_Init ���ł�_iocs_b_super");
#endif
	} else {
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		puts("App_Init _iocs_b_super�J�n");
#endif
	}

	/* ��� */
//	g_nCrtmod = CRTC_INIT(0);	/* mode=0 512x512 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(1);	/* mode=1 512x512 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(2);	/* mode=2 256x256 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(3);	/* mode=3 256x256 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(4);	/* mode=4 512x512 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(5);	/* mode=5 512x512 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(6);	/* mode=6 256x256 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(7);	/* mode=7 256x256 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(0x100+8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(9);	/* mode=9 512x512 col:16/256 15kHz */
	g_nCrtmod = CRTC_INIT(10);	/* mode=10 256x256 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(11);	/* mode=11 256x256 col:16/256 15kHz */
//	g_nCrtmod = CRTC_INIT(12);	/* mode=12 512x512 col:16/65536 31kHz */
//	g_nCrtmod = CRTC_INIT(13);	/* mode=13 512x512 col:16/65536 15kHz */
//	g_nCrtmod = CRTC_INIT(14);	/* mode=14 256x256 col:16/65536 31kHz */
//	g_nCrtmod = CRTC_INIT(15);	/* mode=15 256x256 col:16/65536 15kHz */
//	g_nCrtmod = CRTC_INIT(16);	/* mode=16 764x512 col:16 31kHz */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init CRTC(���)");
//	KeyHitESC();	/* �f�o�b�O�p */
#endif

	/* �O���t�B�b�N */
	G_INIT();			/* ��ʂ̏����ݒ� */
	G_CLR();			/* �N���b�s���O�G���A�S�J������ */
	G_HOME(0);			/* �z�[���ʒu�ݒ� */
	G_VIDEO_INIT();		/* �r�f�I�R���g���[���[�̏����� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �O���t�B�b�N");
#endif

	/* �X�v���C�g�^�a�f */
	PCG_INIT();			/* �X�v���C�g�^�a�f�̏����� */
	PCG_DATA_LOAD();	/* �X�v���C�g�̃f�[�^�ǂݍ��� */
	PCG_XSP_INIT();		/* XSP������ */
	PCG_INIT_CHAR();	/* �X�v���C�g���a�f��`�Z�b�g */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �X�v���C�g�^�a�f");
#endif

#if CNF_MACS
	/* ���� */
	MOV_INIT();		/* ���������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init Movie(MACS)");
#endif
#endif

	/* MFP */
	MFP_INIT();	/* V-Sync���荞�ݓ��̏��������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	printf("App_Init MFP(%d)\n", Get_CPU_Time());
#endif
	g_GameSpeed = Mmin(Get_CPU_Time() / 300, 7);	/* 300 = 10MHz� */
	g_Vwait = 1;

	/* Task */
	TaskManage_Init();
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init Task");
#endif
	
	/* �}�E�X������ */
	Mouse_Init();	/* �}�E�X������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �}�E�X");
#endif
	
	/* �e�L�X�g */
	T_INIT();	/* �e�L�X�g�u�q�`�l������ */
	T_PALET();	/* �e�L�X�g�p���b�g������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init T_INIT");
#endif

	/* �X�R�A������ */
	S_All_Init_Score();
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init S_All_Init_Score");
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �I��");
#endif
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void App_exit(void)
{
	int16_t	ret = 0;

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �J�n");
#endif
	
	if(g_bExit == TRUE)
	{
		puts("�G���[���L���b�`�I ESC to skip");
		KeyHitESC();	/* �f�o�b�O�p */
	}
	g_bExit = TRUE;

	/* �O���t�B�b�N */
	G_CLR();			/* �N���b�s���O�G���A�S�J������ */

	/* �X�v���C�g���a�f */
	PCG_OFF();		/* �X�v���C�g���a�f��\�� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �X�v���C�g");
#endif

	/* MFP */
	ret = MFP_EXIT();				/* MFP�֘A�̉��� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	printf("App_exit MFP(%d)\n", ret);
#endif

	/* ��� */
	CRTC_EXIT(0x100 + g_nCrtmod);	/* ���[�h�����Ƃɖ߂� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit ���");
#endif

	Mouse_Exit();	/* �}�E�X��\�� */

	/* �e�L�X�g */
	T_EXIT();				/* �e�L�X�g�I������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �e�L�X�g");
#endif
	
	MyMfree(0);				/* �S�Ẵ���������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	printf("MaxUseMem(%d[kb])\n", g_nMaxUseMemSize - GetMaxFreeMem());
	puts("App_exit ������");
#endif

	_dos_kflushio(0xFF);	/* �L�[�o�b�t�@���N���A */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �L�[�o�b�t�@�N���A");
#endif

	/*�X�[�p�[�o�C�U�[���[�h�I��*/
	_iocs_b_super(g_nSuperchk);
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �X�[�p�[�o�C�U�[���[�h�I��");
#endif
	/* ���y */
	Exit_Music();			/* ���y��~ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit Music");
#endif

#if 0	/* �f�o�b�O�R�[�i�[ */
	{
		int16_t i;
		ST_PCG	*p_stPCG;
		
		printf("g_index = %d\n", g_index);
		printf("BALL(i,sp,size,status,validty)\n");
		for (i = 0; i < BALL_MAX; i++) {
			printf("i=%2d,%2d,%2d,%2d,%1d ",i,
					g_stBall[i].bSP_num,
					g_stBall[i].bSize,
					g_stBall[i].bStatus,
					g_stBall[i].bValidty);

			if(((i+1) % 5) == 0)
			{
				printf("\n");
			}
		}
		printf("\n");

		printf("SP(i,(x,y)(dx,dy))\n");
		for(i = SP_BALL_S_1; i < PCG_NUM_MAX; i++)
		{
			p_stPCG = PCG_Get_Info(i);	/* �L���� */

			if(p_stPCG != NULL)
			{
				printf("i=%2d,(%3d,%3d),(%6d,%6d) ",i,
						p_stPCG->x, p_stPCG->y, p_stPCG->dx/256, p_stPCG->dy/256);
			}

			if(((i+1) % 3) == 0)
			{
				printf("\n");
			}
		}
		printf("\n");

//		for(i=0; i<360; i++)
		{
//			printf("[%d](%3d, %3d),", i, g_Circle_Table[i][0], g_Circle_Table[i][1]);
//			if((i%4) == 0)printf("\n");
		}
	}
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �I��");
#endif
}


/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	App_FlipProc(void)
{
	int16_t	ret = 0;
	
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	uint32_t time_now;
	static uint8_t	bFPS = 0u;
	static uint32_t	unTime_FPS = 0u;
#endif

	if(g_bFlip == FALSE)	/* �`�撆�Ȃ̂Ńt���b�v���Ȃ� */
	{
		return ret;
	}
	else
	{
		SetFlip(FALSE);			/* �t���b�v�֎~ */
					
#ifdef W_BUFFER
		/* ���[�h�`�F���W */
		if(g_mode == 1u)		/* �㑤���� */
		{
			SetGameMode(2);
		}
		else if(g_mode == 2u)	/* �������� */
		{
			SetGameMode(1);
		}
		else					/* ���̑� */
		{
			SetGameMode(0);
		}

		/* ��\����ʂ�\����ʂ֐؂�ւ� */
		G_HOME(g_mode);
#endif
				
		bFPS++;

		ret = 1;
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	GetNowTime(&time_now);
	if( (time_now - unTime_FPS) >= 1000ul )
	{
		g_bFPS_PH = bFPS;
		unTime_FPS = time_now;
		bFPS = 0;
	}
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
int16_t	SetFlip(uint8_t bFlag)
{
	int16_t	ret = 0;

	Set_DI();	/* ���荞�݋֎~ */
	g_bFlip_old = g_bFlip;	/* �t���b�v�O��l�X�V */
	g_bFlip = bFlag;
	Set_EI();	/* ���荞�݋��� */

	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void App_TimerProc( void )
{
	ST_TASK stTask;

	TaskManage();
	GetTaskInfo(&stTask);	/* �^�X�N�擾 */
	/* ������ ���̊Ԃɏ��������� ������ */
#if 0
	if(stTask.b96ms == TRUE)	/* 96ms���� */
	{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		int8_t	sBuf[8];
#endif
		switch(stTask.bScene)
		{
			case SCENE_GAME1:
			case SCENE_GAME2:
			{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
				memset(sBuf, 0, sizeof(sBuf));
				sprintf(sBuf, "%3d", g_bFPS_PH);
				Draw_Message_To_Graphic(sBuf, 24, 24, F_MOJI, F_MOJI_BAK);	/* �f�o�b�O */
#endif
				break;
			}
			default:
				break;
		}
	}
#endif
	/* ������ ���̊Ԃɏ��������� ������ */

	/* �^�X�N���� */
	UpdateTaskInfo();		/* �^�X�N�̏����X�V */

}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t App_RasterProc( uint16_t *pRaster_cnt )
{
	int16_t	ret = 0;
#if CNF_RASTER
	RasterProc(pRaster_cnt);	/*�@���X�^�[���荞�ݏ��� */
#endif /* CNF_RASTER */
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void App_VsyncProc( void )
{
//	puts("App_VsyncProc");
#if CNF_VDISP
	
	if(Input_Main(g_ubDemoPlay) != 0u) 	/* ���͏��� */
	{
		g_ubDemoPlay = FALSE;	/* �f������ */

		SetTaskInfo(SCENE_INIT);	/* �V�[��(����������)�֐ݒ� */
	}
	
	App_FlipProc();	/* ��ʐ؂�ւ� */

#endif /* CNF_VDISP */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	GetGameMode(uint8_t *bMode)
{
	int16_t	ret = 0;
	
	*bMode = g_mode;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	SetGameMode(uint8_t bMode)
{
	int16_t	ret = 0;
	
	g_mode = bMode;
	if(bMode == 1)
	{
		g_mode_rev = 2;
	}
	else
	{
		g_mode_rev = 1;
	}
	
	return ret;
}

#endif	/* MAIN_C */
