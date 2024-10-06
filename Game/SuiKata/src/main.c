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
#define	FPS_MONI	/* FPS�v�� */
//#define	CPU_MONI	/* CPU�v�� */
#define	MEM_MONI	/* MEM�v�� */

enum
{
	ball_lost,	/* �������{�[�� */
	ball_now,	/* ���˃{�[�� */
	ball_next,	/* ���̔��˃{�[�� */
	ball_exist,	/* ���݂���{�[�� */
};

#define		BALL_MAX	(74)
#define 	X_POS_MIN	( 64+16+2)
#define 	X_POS_MAX	(192-16-2)
#define 	X_POS_W		(X_POS_MAX - X_POS_MIN)
#define 	Y_POS_MIN	(16)
#define 	Y_POS_BD	(48)
#define 	Y_POS_MAX	(192)
#define 	Y_POS_H		(Y_POS_MAX - Y_POS_MIN)

#define 	X_ACC		(0x100)
#define 	X_ACC2		(0x200)
#define 	X_DEC		(0x010)
#define 	Y_ACC		(0x100)
#define 	Y_ACC2		(0x200)

#define 	FONT24H		(24)
#define 	FONT12W		(12)

#define		NUM_STARS	(128)

#define		BG_MAP_PAL	(2)

#define		START_LOGO_INTERVAL_TIME	(1000)
#define		PLAYER_INPUT_INTERVAL_TIME	(800)
#define		FRAME_RATIO_TIME			(12)
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
#ifdef FPS_MONI
uint8_t		g_bFPS_PH = 0u;
#endif
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

/* �O���[�o���ϐ� */
int16_t		g_Ball_Color[5] = {0, 1, 2, 3, 4};
int16_t		g_Ball_Table[5] = {SP_BALL_S_1, SP_BALL_M_1, SP_BALL_L_1, SP_BALL_XL_1, PCG_NUM_MAX};
uint8_t		g_bBoder_line = 0xFF;
uint8_t		g_bBoder_count = 0;
int16_t		g_TB_GameLevel[3] = {256, 224, Y_POS_MAX};
int16_t		g_nGameLevel = 0;
int8_t		g_TB_GameDiffLevel[2][8] = {"EASY", "HARD"};
int16_t		g_nGameDifflevel = 0;
int8_t		g_bShot_OK = TRUE;
uint8_t		g_ubDemoPlay = FALSE;
uint8_t		g_ubGameClear = FALSE;
int16_t		g_nGameClearCount = 0;
uint8_t		g_bAREA[256][256] = {0};

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
uint16_t	g_uDebugNum = 0;
//uint16_t	g_uDebugNum = (Bit_7|Bit_4|Bit_0);
#endif

/* �֐��̃v���g�^�C�v�錾 */
static void Boder_line(void);
static void Level_meter(int8_t);
static uint8_t Ball_check_collision(uint16_t* , uint16_t* , uint16_t, uint16_t, uint8_t, uint8_t*);
static void Ball_mark_area(ST_PCG* , int8_t, uint8_t);
static void print_area(void);
static int8_t Ball_Point_Update(ST_PCG* , uint8_t );
static int16_t Ball_Combine(ST_PCG *, int16_t, ST_PCG *, int16_t);
static int8_t Ball_Check(int16_t);
static void Ball_input(void);
static void Ball_Output(void);

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

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Boder_line st(%d)\n", g_index);
#endif

	p_stPCG = PCG_Get_Info(SP_ARROW_0);		/* ��� */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = (X_POS_MIN - Mmul16(p_stPCG->Pat_w) - 2) << PCG_LSB;
		if(g_bBoder_line < 8)
		{
			p_stPCG->y = 0;
		}
		else
		{
			p_stPCG->y = (g_bBoder_line - 8) << PCG_LSB;
		}
		p_stPCG->update	= TRUE;
	}

	p_stPCG  = PCG_Get_Info(SP_GAL_0);		/* �M���� */
	if(p_stPCG != NULL)
	{
		int16_t y_max;
		y_max = (256 - Mmul16(p_stPCG->Pat_h));
		if(g_bBoder_count >= y_max)
		{
			p_stPCG->x = ((208) + (rand() % 2)) << PCG_LSB;
		}
		else
		{
			p_stPCG->x = (208) << PCG_LSB;
		}
		p_stPCG->y = Mmin(Mmax(Y_POS_BD, g_bBoder_count), y_max) << PCG_LSB;
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
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Boder_line ed(%d)\n", g_index);
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
	uint16_t x, y;
	uint16_t x1, y1;
	int16_t i, j, k, m, n, p;
	int16_t width, height;
	int16_t pal;
	int16_t list_num;

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
		list_num = g_stBall[j].bSize + 1;
		Ball_mark_area(p_stPCG_other, -1, list_num); // �Â��ʒu���N���A

		/* ���� */
		list_num = g_stBall[i].bSize + 1;
		Ball_mark_area(p_stPCG, -1, list_num); // �Â��ʒu���N���A

		/* ���� */
		g_stBall[i].bStatus++;
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		if( (g_uDebugNum & Bit_6) != 0u )	/* �{�[���̃X�e�[�^�X����MAX */
		{
			g_stBall[i].bStatus = 3;
		}
#endif
		/* �{�[���ݒ� */
		BallSize = g_stBall[i].bSize;		

		if( g_stBall[i].bStatus > 2 )	/* �i���̏���𒴂������T�C�Y�`�F���W */
		{
			if( BallSize >= 3 )	/* �ő�T�C�Y�̍ŏI�n�ō��� */
			{
				g_ubGameClear = TRUE;	/* �Q�[���N���A */
				S_Set_Combo(0);			/* �R���{�J�E���^ ���Z�b�g */
				S_GetPos(44, 136);		/* �X�R�A�\�����W�X�V */
				ret = S_Add_Score(1000000);	/* �X�R�A�X�V */

				BallSize = 0;	/* �T�C�Y0 */ 
				m = 0;
				list_num = m + 1;	/* 0��BG�Ȃ̂� */
			}
			else
			{
				S_GetPos((x >> PCG_LSB), (y >> PCG_LSB));	/* �X�R�A�\�����W�X�V */

				ret = S_Add_Score(1000);	/* �X�R�A�X�V */

				BallSize++;	/* �T�C�Y�A�b�v */ 
				m = Mmin(BallSize, 3);	/* �ő�l�{�P */
				list_num = m + 1;	/* 0��BG�Ȃ̂� */
			}


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
						pal = Mmul256(g_stST_PCG_LIST[list_num].Pal);
						for(p=0; p < p_stPCG_new->Pat_DataMax; p++)
						{
							*(p_stPCG_new->pPatCodeTbl + p) &= 0xF0FF;
							*(p_stPCG_new->pPatCodeTbl + p) |= (pal + Mmul256(g_Ball_Color[g_stBall[n].bStatus]));	/* �J���[�Z�b�g */
						}
						p_stPCG_new->update = TRUE;
						p_stPCG_new->status = FALSE;

						width	= Mmul16(p_stPCG_new->Pat_w);
						height	= Mmul16(p_stPCG_new->Pat_h);

						/* �r�� */
						if( (y1 >> PCG_LSB) > (g_TB_GameLevel[g_nGameLevel] - height) )	/* �r���� */
						{
							y1 = (g_TB_GameLevel[g_nGameLevel] - height) << PCG_LSB;
						}
						else
						{
						}

						if((x1 >> PCG_LSB) <= X_POS_MIN)			/* �r���� */
						{
							x1 = X_POS_MIN << PCG_LSB;
						}
						else if((x1 >> PCG_LSB) > (X_POS_MAX - width))	/* �r���E */
						{
							x1 = (X_POS_MAX - width) << PCG_LSB;
						}

						p_stPCG_new->dx = 0;
						p_stPCG_new->dy = 0;
						p_stPCG_new->x = x1;
						p_stPCG_new->y = y1;

					    Ball_mark_area(p_stPCG_new, 1, list_num); // �V�����ʒu���Z�b�g
						break;
					}
				}
			}
			/* ���� */
			p_stPCG_other->x = 0;
			p_stPCG_other->y = 0;
			p_stPCG_other->update = FALSE;
			p_stPCG_other->status = FALSE;
			
			g_stBall[j].bStatus = (rand() % 3);
			g_stBall[j].bSize = 0;
			g_stBall[j].bValidty = ball_lost;

			/* ���� */
			p_stPCG->x = 0;
			p_stPCG->y = 0;
			p_stPCG->update = FALSE;
			p_stPCG->status = FALSE;

			g_stBall[i].bStatus = (rand() % 3);	
			g_stBall[i].bSize = 0;
			g_stBall[i].bValidty = ball_lost;
		}
		else	/* �J���[�`�F���W */
		{
			/* ���� */
			list_num = g_stBall[j].bSize + 1;
			Ball_mark_area(p_stPCG_other, -1, list_num); // �Â��ʒu���N���A

			g_stBall[j].bStatus = (rand() % 3);
			g_stBall[j].bSize = 0;
			g_stBall[j].bValidty = ball_lost;
			p_stPCG_other->x = 0;
			p_stPCG_other->y = 0;
			p_stPCG_other->update = FALSE;
			p_stPCG_other->status = FALSE;

			/* �����͈�U�A������ */
			list_num = g_stBall[i].bSize + 1;
			Ball_mark_area(p_stPCG, -1, list_num); // �Â��ʒu���N���A

			if( BallSize >= 3 )	/* �ő�T�C�Y�̍ŏI�n�ō��� */
			{
				g_stBall[i].bStatus = (rand() % 3);
				g_stBall[i].bSize = 0;
				g_stBall[i].bValidty = ball_lost;

				p_stPCG->x = 0;
				p_stPCG->y = 0;
				p_stPCG->update = FALSE;
				p_stPCG->status = FALSE;

				g_ubGameClear = TRUE;	/* �Q�[���N���A */
				S_Set_Combo(0);			/* �R���{�J�E���^ ���Z�b�g */
				S_GetPos(44, 136);		/* �X�R�A�\�����W�X�V */
				ret = S_Add_Score(1000000);	/* �X�R�A�X�V */
			}
			else
			{
				S_GetPos((x >> PCG_LSB), (y >> PCG_LSB));	/* �X�R�A�\�����W�X�V */

				ret = S_Add_Score(100);	/* �X�R�A�X�V */

				pal = Mmul256(g_stST_PCG_LIST[list_num].Pal);
				for(k=0; k < p_stPCG->Pat_DataMax; k++)
				{
					*(p_stPCG->pPatCodeTbl + k) &= 0xF0FF;
					*(p_stPCG->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* �J���[�Z�b�g */
				}
				p_stPCG->update = TRUE;
				p_stPCG->status = FALSE;
				p_stPCG->x = x1;
				p_stPCG->y = y1;
				p_stPCG->dy = 0;
				p_stPCG->dx = 0;

			    Ball_mark_area(p_stPCG, 1, list_num); // �V�����ʒu���Z�b�g
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
static uint8_t Ball_check_collision(uint16_t* new_x, uint16_t* new_y, uint16_t width, uint16_t height, uint8_t bListNum, uint8_t* pbStat)
{
	uint8_t ret = FALSE;
	uint16_t x, y;
	uint16_t x_OK, y_OK;
	uint16_t x_Ofst_s, x_Ofst_e;
	uint16_t y_Ofst_s, y_Ofst_e;

    uint16_t left;
    uint16_t right;
    uint16_t top;
    uint16_t bottom;

	uint8_t bFlag = 0;

    if ( *new_x <= X_POS_MIN )
	{
		*new_x = X_POS_MIN;
		bFlag |= Bit_4;	/* X���� */
    }
    if ((*new_x + width) >= X_POS_MAX )
	{
		*new_x = X_POS_MAX - width;
		bFlag |= Bit_5;	/* X���� */
    }
    if (*new_y <= 0)
	{
		*new_y = 0;
		bFlag |= Bit_6;	/* Y���� */
    }
    if ((*new_y + height) >= g_TB_GameLevel[g_nGameLevel]) 
	{
		*new_y = g_TB_GameLevel[g_nGameLevel] - height;
		bFlag |= Bit_7;	/* Y���� */
    }

#if 1
    x		= *new_x + g_stST_PCG_LIST[bListNum].hit_x;
	y		= *new_y + g_stST_PCG_LIST[bListNum].hit_y;
	left	= x & 0xFF;
	top		= y & 0xFF;

	width	= g_stST_PCG_LIST[bListNum].hit_width;
	height	= g_stST_PCG_LIST[bListNum].hit_height;
    right	= (left + width) & 0xFF;
    bottom	= (top + height) & 0xFF;
#else
    left = *new_x;
	top = *new_y;
    right = *new_x + width;
    bottom = *new_y + height;
#endif
	
#if 1
	x_OK = left;
	x_Ofst_s = left;
	x_Ofst_e = right;

	y_OK = bottom;
	y_Ofst_s = bottom;
	y_Ofst_e = top;
#else
	x_OK = left;
	x_Ofst_s = x_OK + Mdiv4(width);
	x_Ofst_e = x_Ofst_s + Mdiv2(width);

	y_OK = bottom;
	y_Ofst_s = y_OK - Mdiv2(height);	/* ���������� */
	y_Ofst_e = top;
#endif

#if 1
	x = x_Ofst_s;
    for (y = y_Ofst_s; y >= y_Ofst_e; y--)	/* ���� */
	{
		if (g_bAREA[y][x] != 0) /* �{�[�������݂��� */
		{
			bFlag |= Bit_0;	/* X���� */
			break;
		}
	}

	x = x_Ofst_e;
    for (y = y_Ofst_s; y >= y_Ofst_e; y--)	/* �E�� */
	{
		if (g_bAREA[y][x] != 0) /* �{�[�������݂��� */
		{
			bFlag |= Bit_1;	/* X���� */
			break;
		}
	}

	y = y_Ofst_e;
	for ( x = x_Ofst_s; x < x_Ofst_e; x++ )	/* ��� */
	{
		if (g_bAREA[y][x] != 0) /* �{�[�������݂��� */
		{
			bFlag |= Bit_2;	/* Y���� */
			break;
		}
	}

	y = y_Ofst_s;
	for ( x = x_Ofst_s; x < x_Ofst_e; x++ )	/* ���� */
	{
		if (g_bAREA[y][x] != 0) /* �{�[�������݂��� */
		{
			bFlag |= Bit_3;	/* Y���� */
			break;
		}
	}

	if( bFlag != 0u )
	{
		*pbStat = Mbset(*pbStat, 0xFF, bFlag);	/* �t���O�Z�b�g */
		ret = TRUE;
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	
	if( (g_uDebugNum & Bit_7) != 0u )	/* �ڐG���� */
	{
		if((*pbStat & Bit_0) != 0u)	/* �������ɂ��� */
		{
			Draw_Line(x_Ofst_s+1, y_Ofst_e, x_Ofst_s+1, y_Ofst_s, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_1) != 0u)	/* �E�����ɂ��� */
		{
			Draw_Line(x_Ofst_e -1 , y_Ofst_e, x_Ofst_e -1, y_Ofst_s, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_2) != 0u)	/* ������ɂ��� */
		{
			Draw_Line(x_Ofst_s, y_Ofst_e+1, x_Ofst_e, y_Ofst_e+1, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_3) != 0u)	/* �������ɂ��� */
		{
			Draw_Line(x_Ofst_s, y_Ofst_s -1, x_Ofst_e, y_Ofst_s - 1, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_4) != 0u)	/* ���[�����ɂ��� */
		{
			Draw_Line(x_Ofst_s, y_Ofst_e, x_Ofst_s, y_Ofst_s, G_L_BLUE, 0xFFFF);
		}
		if((*pbStat & Bit_5) != 0u)	/* �E�����ɂ��� */
		{
			Draw_Line(x_Ofst_e, y_Ofst_e, x_Ofst_e, y_Ofst_s, G_L_BLUE, 0xFFFF);
		}
		if((*pbStat & Bit_6) != 0u)	/* ��[�����ɂ��� */
		{
			Draw_Line(x_Ofst_s, y_Ofst_e, x_Ofst_e, y_Ofst_e, G_L_BLUE, 0xFFFF);
		}
		if((*pbStat & Bit_7) != 0u)	/* ���[�����ɂ��� */
		{
			Draw_Line(x_Ofst_s, y_Ofst_s, x_Ofst_e, y_Ofst_s, G_L_BLUE, 0xFFFF);
		}
	}
#endif
	
#else
    for (y = y_OK; y >= top; y--)
	{
		for ( x = left; x < right; x++ )
		{
            if (g_bAREA[y][x] == 1) /* �{�[�������݂��� */
			{
				if(y == y_OK)
				{
					if((x >= x_Ofst_s) && (x < x_Ofst_e))	/* ���ʂ̒��� */
					{
						*new_y = 0;	/* ���ʂɉ��������� */
						*pbStat = Mbset(*pbStat, 0xFF, Bit_2);	/* Y���� */
						return TRUE;
					}
				}
				else
				{
					*new_y = y;	/* ���ʂɉ��������� */
					*pbStat = Mbset(*pbStat, 0xFF, Bit_3);	/* Y���� */
	                return TRUE;
				}
            }
        }
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
static void Ball_mark_area(ST_PCG* p_stPCG, int8_t bValue, uint8_t bListNum)
{
	uint16_t x, y;
	uint16_t width, height;
    uint16_t left;
    uint16_t right;
    uint16_t top;
    uint16_t bottom;

	x = p_stPCG->x >> PCG_LSB;
	y = p_stPCG->y >> PCG_LSB;
#if 1
	x += g_stST_PCG_LIST[bListNum].hit_x;
	y += g_stST_PCG_LIST[bListNum].hit_y;
	width	= g_stST_PCG_LIST[bListNum].hit_width;
	height	= g_stST_PCG_LIST[bListNum].hit_height;
#else
	width	= Mmul16(p_stPCG->Pat_w);
	height	= Mmul16(p_stPCG->Pat_h);
#endif

    left = x;
    right = x + width;
    top = y;
    bottom = y + height;

	if(left > 0xFF)return;
	if(right > 0xFF)right = 0xFF;
	if(top > 0xFF)return;
	if(bottom > 0xFF)bottom = 0xFF;

    for ( y = top; y < bottom; y++ ) 
	{
        for ( x = left; x < right; x += SP_W2 ) 
		{
			if(g_bAREA[y][x] == 0)
			{
				if(bValue > 0)
				{
		            g_bAREA[y][x] += bValue;
				}
			}
			else
			{
	            g_bAREA[y][x] += bValue;
			}
        }
    }
	
	x = right;
    for ( y = top; y < bottom; y++ ) 
	{
		if(g_bAREA[y][x] == 0)
		{
			if(bValue > 0)
			{
				g_bAREA[y][x] += bValue;
			}
		}
		else
		{
			g_bAREA[y][x] += bValue;
		}
	}

    for ( y = top; y < bottom; y += SP_H2 ) 
	{
        for ( x = left; x < right; x++ ) 
		{
			if(g_bAREA[y][x] == 0)
			{
				if(bValue > 0)
				{
		            g_bAREA[y][x] += bValue;
				}
			}
			else
			{
	            g_bAREA[y][x] += bValue;
			}
        }
    }

	y = bottom;
	for ( x = left; x < right; x++ ) 
	{
		if(g_bAREA[y][x] == 0)
		{
			if(bValue > 0)
			{
				g_bAREA[y][x] += bValue;
			}
		}
		else
		{
			g_bAREA[y][x] += bValue;
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
static void print_area(void) 
{
 	uint16_t x, y;

    for ( y = 0; y < 256; y += 1)
	{
        for ( x = X_POS_MIN; x < X_POS_MAX; x += 1)
		{
			switch(g_bAREA[y][x])
			{
				case 0:
				{
		            Draw_Pset( x, y, G_BG);	/* �����Ȃ� */
					break;
				}
				case 1:
				{
		            Draw_Pset( x, y, G_INDIGO);	/* �������� */
					break;
				}
				case 2:
				{
		            Draw_Pset( x, y, G_RED);	/* �������� */
					break;
				}
				case 3:
				{
		            Draw_Pset( x, y, G_GREEN);	/* �������� */
					break;
				}
				default:
				{
		            Draw_Pset( x, y, G_YELLOW);	/* �������� */
					break;
				}
			}
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
static int8_t Ball_Point_Update(ST_PCG* p_stPCG, uint8_t list_num)
{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	int8_t sBuf[128] = {0};
#endif
	int8_t	ret = TRUE;
 	uint16_t x, y;
	int16_t dx, dy;
	uint16_t width, height;
	uint16_t h_width, h_height;
	uint16_t new_x, new_y;
	uint16_t old_x, old_y;
	uint16_t end_y;
	uint8_t	bStat = 0;

	/* ���݂̈ʒu */
#if 0
	x += g_stST_PCG_LIST[bListNum].hit_x;
	y += g_stST_PCG_LIST[bListNum].hit_y;
#else
	x = p_stPCG->x;
	y = p_stPCG->y;
#endif
	dx = p_stPCG->dx;
	dy = p_stPCG->dy;
	width	= Mmul16(p_stPCG->Pat_w);
	height	= Mmul16(p_stPCG->Pat_h);
	h_width		= g_stST_PCG_LIST[list_num].hit_width;
	h_height	= g_stST_PCG_LIST[list_num].hit_height;
	old_x = x >> PCG_LSB;
	old_y = y >> PCG_LSB;

	end_y = g_TB_GameLevel[g_nGameLevel] - height;

	/* �V�����ʒu */
    p_stPCG->dy += Y_ACC;
	p_stPCG->dy = Mmin(p_stPCG->dy, 0x1000);
    new_x = (x + p_stPCG->dx) >> PCG_LSB;
    new_y = (y + p_stPCG->dy) >> PCG_LSB;

	/* �����蔻�肻�̂P */
    if (Ball_check_collision(&new_x, &new_y, width, height, list_num, &bStat))	/* �g�O�������̓q�b�g */
	{
		/* -------------------- < 4 > -------------------- */
		if((bStat & (Bit_0 | Bit_1 | Bit_2 | Bit_3)) == (Bit_0 | Bit_1 | Bit_2 | Bit_3))	/* �S�Ăɏ�Q���������� */
		{
			p_stPCG->dx = 0; // �~�߂�
			p_stPCG->dy = 0; // �~�߂�
		}
		/* -------------------- < 3 > -------------------- */
		else if((bStat & (Bit_0 | Bit_1 | Bit_3 )) == (Bit_0 | Bit_1 | Bit_3 ))	/* �オ�J���Ă��� */
		{
			p_stPCG->dx = 0; // �~�߂�
			p_stPCG->dy = -Y_ACC2;	/* ������� */
		}
		else if((bStat & (Bit_0 | Bit_1 | Bit_2 )) == (Bit_0 | Bit_1 | Bit_2 ))	/* �����J���Ă��� */
		{
			p_stPCG->dx = 0; // �~�߂�
			/* ���� */
		}
		else if((bStat & (Bit_1 | Bit_2 | Bit_3)) == (Bit_1 | Bit_2 | Bit_3))	/* �����J���Ă��� */
		{
			if(p_stPCG->dx >= 0)p_stPCG->dx = -X_ACC;	/* ���� */
			p_stPCG->dy = 0; // �~�߂�
		}
		else if((bStat & (Bit_0 | Bit_2 | Bit_3)) == (Bit_0 | Bit_2 | Bit_3))	/* �E���J���Ă��� */
		{
			if(p_stPCG->dx <= 0)p_stPCG->dx = X_ACC;	/* �E�� */
			p_stPCG->dy = 0; // �~�߂�
		}
		/* -------------------- < 2 > -------------------- */
		else if((bStat & (Bit_2 | Bit_3)) == (Bit_2 | Bit_3))	/* �㉺�ɏ�Q���������� */
		{
			/* ���E */
			p_stPCG->dy = 0; // �~�߂�
		}
		else if((bStat & (Bit_0 | Bit_1)) == (Bit_0 | Bit_1))	/* ���E�ɏ�Q���������� */
		{
			p_stPCG->dx = 0; // �~�߂�
			/* �㉺ */
		}
		else if((bStat & (Bit_0 | Bit_3)) == (Bit_0 | Bit_3))	/* �����ɏ�Q���������� */
		{
			if(p_stPCG->dx <= 0)p_stPCG->dx = X_ACC;	/* �E�� */
			p_stPCG->dy = 0; // �~�߂�
		}
		else if((bStat & (Bit_1 | Bit_3)) == (Bit_1 | Bit_3))	/* �E���ɏ�Q���������� */
		{
			if(p_stPCG->dx >= 0)p_stPCG->dx = -X_ACC;	/* ���� */
			p_stPCG->dy = 0; // �~�߂�
		}
		else if((bStat & (Bit_0 | Bit_2)) == (Bit_0 | Bit_2))	/* ����ɏ�Q���������� */
		{
			if(p_stPCG->dx <= 0)p_stPCG->dx = X_ACC;	/* �E�� */
			/* ���� */
		}
		else if((bStat & (Bit_1 | Bit_2)) == (Bit_1 | Bit_2))	/* �E��ɏ�Q���������� */
		{
			if(p_stPCG->dx >= 0)p_stPCG->dx = -X_ACC;	/* ���� */
			/* ���� */
		}
		/* -------------------- < 1 > -------------------- */
		else if((bStat & Bit_3) == Bit_3)	/* ���ɏ�Q���������� */
		{
			/* ���E */
			p_stPCG->dy = 0; // �~�߂�
		}
		else
		{

		}

		/* -------------------- < �[�̏��� > -------------------- */
		if((bStat & Bit_4) == Bit_4)	/* ���[�̂� */
		{
			if(p_stPCG->dx < 0)p_stPCG->dx = X_ACC;	// ���]
			else p_stPCG->dx = 0; // �~�߂�
			/* ���� */
		}
		else if((bStat & Bit_5) == Bit_5)	/* �E�[�̂� */
		{
			if(p_stPCG->dx > 0)p_stPCG->dx = -X_ACC;	// ���]
			else p_stPCG->dx = 0; // �~�߂�
			/* ���� */
		}
		else
		{

		}

		if((bStat & Bit_6) == Bit_6)	/* �V��ɏ�Q���������� */
		{
			/* ���E */
			/* ���� */
		}
		else if((bStat & Bit_7) == Bit_7)	/* ��ӂɏ�Q���������� */
		{
			/* ���E */
			if(p_stPCG->dy > 0)p_stPCG->dy = 0; // �~�߂�
		}
		else	/* �ǂ��炩�ɏ�Q���������� */
		{
			/* ���E */
			/* ���� */
		}

		/* �Čv�Z */
		new_x = (x + p_stPCG->dx) >> PCG_LSB;
		new_y = (y + p_stPCG->dy) >> PCG_LSB;

		p_stPCG->status = FALSE;	/* �ړ���~ */

		ret = FALSE;
    }
	else
	{
		if(new_y < old_y)	/* �����蔻��̂Ȃ��㏸���~�߂� */
		{
			p_stPCG->dy = Y_ACC;
			new_y = old_y;
		}
	}

	if(new_y > old_y)	/* ����l�𒴂���ꍇ�́A�����グ�� */
	{
		if((new_y - old_y) > 200)
		{
			p_stPCG->dy = Y_ACC;
			new_y = 0;
		}
	}

	if(new_y > end_y)	/* �����l�𒴂���ꍇ�́A�����グ�� */
	{
		new_y = end_y;
	}

	p_stPCG->x = new_x << PCG_LSB;	/* �� �X�V */
	p_stPCG->y = new_y << PCG_LSB;	/* �c �X�V */

	if((p_stPCG->dx == 0) && (p_stPCG->dy == 0))
	{
		p_stPCG->status = FALSE;	/* �ړ���~ */
		ret = FALSE;
	}
	
	/* ����p */
	if(p_stPCG->dx == 0)
	{
		/* �������Ȃ� */
	}
	else if(old_x < new_x)
	{
		p_stPCG->dx -= X_DEC;		/* ���� */
	}
	else
	{
		p_stPCG->dx += X_DEC;		/* ���� */
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	if( (g_uDebugNum & Bit_4) != 0u )	/* �����x�\�� */
	{
		x = p_stPCG->x >> PCG_LSB;
		y = p_stPCG->y >> PCG_LSB;
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%d,%d", (int8_t)(p_stPCG->dx >> PCG_LSB), (int8_t)(p_stPCG->dy >> PCG_LSB));
		PutGraphic_To_Symbol12(sBuf, x, y, F_MOJI );	
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
static int8_t Ball_Check(int16_t nBallCount)
{
	int8_t	ret = FALSE;
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	int8_t sBuf[128] = {0};
#endif
	int16_t	i, j;

	uint16_t x, y;
	uint16_t w, h;
//	uint16_t dw, dh;
	uint16_t cx, cy;
	int16_t dx, dy;
	uint16_t width, height;
	uint16_t width_h, height_h;
	uint16_t xs, ys, xe, ye;
	int16_t list_num;

	uint16_t x1, y1;
	uint16_t w1, h1;
//	uint16_t dw1, dh1;
	uint16_t cx1, cy1;
	int16_t dx1, dy1;
	uint16_t width1, height1;
	uint16_t width1_h, height1_h;
	uint16_t xs1, ys1, xe1, ye1;
	int16_t list_num1;

	int16_t line = 0xFF;
	int8_t bValid, bValid1;
#if 0
	int8_t overlapping_pairs[BALL_MAX][2] = {0};
	int16_t num_pairs = 0;
 	int16_t touching[BALL_MAX] = {FALSE};
#endif

	uint8_t bNoMoreMove = TRUE;

	static uint32_t s_WaitTime = PASSTIME_INIT;
	static uint8_t s_bErase = FALSE;
	static uint8_t s_bErase_old = FALSE;
	static uint16_t ef_x, ef_y;
	static uint16_t ef_x1, ef_y1;
	static uint16_t e_w, e_h;

	static uint16_t s_Resume = 0;
	static uint16_t s_Resume_old = 0;
	static uint32_t s_AbortTime = PASSTIME_INIT;
	static uint32_t s_AbortTime_old = PASSTIME_INIT;

//	uint8_t allRectanglesNonOverlapping;
#if 0
	uint32_t time_st;
#endif
	ST_PCG	*p_stPCG = NULL;
	ST_PCG	*p_stPCG_other = NULL;

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	static int16_t count = 0;

//	printf("Ball_Check st(%d)\n", g_index);
#endif

#if 0
	GetNowTime(&time_st);	/* �^�C���A�E�g�J�E���^���Z�b�g */
#endif
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Ball_Check st(%d)\n", count);
	if( g_uDebugNum != 0u )		/* �f�o�b�O�� */
	{
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%3d", g_uDebugNum);
		BG_TextPut(sBuf, 178, 239);	/* �f�o�b�O�\�� */
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%08b  ", g_uDebugNum);
		BG_TextPut(sBuf, 178, 247);	/* �f�o�b�O�\�� */

		Draw_Fill(X_POS_MIN, Y_MIN_DRAW, X_POS_MAX, Y_MAX_DRAW-1, G_BG);		/* �r���̒��͓h��Ԃ� */
	}

	if( (g_uDebugNum & Bit_5) != 0u )	/* �����蔻�� */
	{
		print_area();	/* �h��Ԃ� */
	}
#endif
	if(g_GameSpeed > 3)
	{
		if((s_bErase == TRUE) || ((s_bErase == FALSE) && (s_bErase_old == TRUE)))	/* �������͍��W��X�e�[�^�X���X�V���Ȃ� */
		{
			T_Clear();		/* �e�L�X�g�N���A */
		}
	}
	s_bErase_old = s_bErase;

	s_Resume_old = s_Resume;
	GetNowTime(&s_AbortTime);

	/* �d�Ȃ蒊�o */
	for(i = s_Resume; i < BALL_MAX - 1; i++)
	{
		uint8_t bEraseTmp = FALSE;

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
		Level_meter(FALSE);	/* �������׌v�� */
#endif
		bValid = g_stBall[i].bValidty;
		if( bValid != ball_exist )		/* ���݂���{�[���ȊO */
		{
			continue;
		}

		p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* �{�[�� */
		if(p_stPCG == NULL)
		{
			continue;
		}
		p_stPCG->update = TRUE;
		
		if(s_bErase == TRUE)	/* �������͍��W��X�e�[�^�X���X�V���Ȃ� */
		{
			if(	GetPassTime(COMBO_INTERVAL_SE_TIME, &s_WaitTime) != 0u  )	/* 500ms�o�� */
			{
				s_bErase = FALSE;
				g_bShot_OK = TRUE;	/* ���̔��ˋ��� */
			}
			else
			{
				if(g_GameSpeed > 3)
				{
					/* �A�j���[�V���� */
					T_Circle(ef_x, ef_y, e_w, e_h, 0xA5A5, rand()%15 + 1);
					T_Circle(ef_x1, ef_y1, e_w, e_h, 0xA5A5, rand()%15 + 1);
				}
			}
			continue;
		}
#if 1
		if(s_Resume_old != s_Resume)
		{
			s_AbortTime_old = s_AbortTime;
			if(	GetPassTime(FRAME_RATIO_TIME, &s_AbortTime) != 0u )	/* �t���[�����[�g�d�� */
			{
				bNoMoreMove = FALSE;	/* �ړ��������Ă��Ȃ� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				memset(sBuf, 0, sizeof(sBuf));
//				sprintf(sBuf, "%2d(%2d)%ld", s_Resume, s_Resume_old, s_AbortTime - s_AbortTime_old);
//				Draw_Message_To_Graphic(sBuf, 0, 20, F_MOJI, F_MOJI_BAK);	/* �f�o�b�O */
#endif
				break;
			}
		}
#endif

		dx = p_stPCG->dx;
		dy = p_stPCG->dy;

		x = p_stPCG->x >> PCG_LSB;
		y = p_stPCG->y >> PCG_LSB;

		width	= Mmul16(p_stPCG->Pat_w);
		height	= Mmul16(p_stPCG->Pat_h);

		w = Mdiv2(width);
		h = Mdiv2(height);
		cx = x + w; 
		cy = y + h;

		list_num = g_stBall[i].bSize + 1;
#if 1
		width_h		= g_stST_PCG_LIST[list_num].hit_width;
		height_h	= g_stST_PCG_LIST[list_num].hit_height;

		xs = x + Mdiv2(g_stST_PCG_LIST[list_num].hit_x);
		ys = y + Mdiv2(g_stST_PCG_LIST[list_num].hit_y);
		xe = x + width - Mdiv2((x + width) - (x + g_stST_PCG_LIST[list_num].hit_x + width_h));
		ye = y + height - Mdiv2((y + height) - (y + g_stST_PCG_LIST[list_num].hit_y + height_h));
#else
		xs = x;
		ys = y;
		xe = x + width;
		ye = y + height;
#endif

		for(j = i + 1; j < BALL_MAX; j++)
		{
			bValid1 = g_stBall[j].bValidty;
			if( bValid1 != ball_exist )		/* ���݂���{�[���ȊO */
			{
				continue;
			}

			p_stPCG_other = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num);	/* ���̃{�[�� */
			if(p_stPCG_other == NULL)
			{
				continue;
			}
			p_stPCG_other->update = TRUE;
			
			dx1 = p_stPCG_other->dx;
			dy1 = p_stPCG_other->dy;

			x1 = p_stPCG_other->x >> PCG_LSB;
			y1 = p_stPCG_other->y >> PCG_LSB;
			width1	= Mmul16(p_stPCG_other->Pat_w);
			height1	= Mmul16(p_stPCG_other->Pat_h);
			w1 = Mdiv2(width1); 
			h1 = Mdiv2(height1);

			cx1 = x1 + w1; 
			cy1 = y1 + h1;

#if 1
			list_num1 = g_stBall[j].bSize + 1;
			width1_h	= g_stST_PCG_LIST[list_num1].hit_width;
			height1_h	= g_stST_PCG_LIST[list_num1].hit_height;

			xs1 = x1 + Mdiv2(g_stST_PCG_LIST[list_num1].hit_x);
			ys1 = y1 + Mdiv2(g_stST_PCG_LIST[list_num1].hit_y);
			xe1 = x1 + width1 - Mdiv2((x1 + width1) - (x1 + g_stST_PCG_LIST[list_num1].hit_x + width1_h));
			ye1 = y1 + height1 - Mdiv2((y1 + height1) - (y1 + g_stST_PCG_LIST[list_num1].hit_y + height1_h));
#else
			xs1 = x1;
			ys1 = y1;
			xe1 = x1 + width1;
			ye1 = y1 + height1;
#endif
			if( (xs <= xe1) && (xe >= xs1) && (ys <= ye1) && (ye >= ys1) )
			{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//				T_Box(xs, ys, xe, ye, 0xFFFF, T_RED2);
//				T_Box(xs1, ys1, xe1, ye1, 0xFFFF, T_YELLOW);
//				Draw_Box(xs, ys, xe, ye, G_D_RED, 0xFFFF);
//				Draw_Box(xs1, ys1, xe1, ye1, G_YELLOW, 0xFFFF);
#endif
#if 0
				overlapping_pairs[num_pairs][0] = i;
				overlapping_pairs[num_pairs][1] = j;
				touching[i] = TRUE;
				touching[j] = TRUE;
				num_pairs++;
#endif
				if((i == nBallCount) || (j == nBallCount))		/* �L���������{�[�� */
				{
					g_bShot_OK = TRUE;	/* ���̔��ˋ��� */
				}

				if( Ball_Combine(p_stPCG, i, p_stPCG_other, j) != 0 )	/* ���̐��� */
				{
					ADPCM_Play(Mmin(S_Get_Combo() + 7 - 1, p_list_max - 1));	/* �A���� */

					ef_x = x;
					ef_y = y;
					ef_x1 = x1;
					ef_y1 = y1;
					e_w = width;
					e_h = height;

					if(g_GameSpeed > 3)
					{
						/* �A�j���[�V���� */
						T_Circle(ef_x, ef_y, e_w, e_h, 0xA5A5, rand()%15 + 1);
						T_Circle(ef_x1, ef_y1, e_w, e_h, 0xA5A5, rand()%15 + 1);
					}
					s_bErase = TRUE;
					bEraseTmp = TRUE;
					break;
				}
				else
				{
					/* �Ԃ��������� */
					if(cx == cx1)
					{
						/* �������Ȃ� */
					}
					else if(cx < cx1)
					{
						if((p_stPCG->dx == 0) && (p_stPCG->dy > Y_ACC2))
						{
							p_stPCG->dx -= X_ACC;
						}
					}
					else
					{
						if((p_stPCG->dx == 0) && (p_stPCG->dy > Y_ACC2))
						{
							p_stPCG->dx += X_ACC;
						}
					}
				}
			}
			else
			{
				/* �������Ȃ� */
			}
		}

		if(bEraseTmp == TRUE)
		{
			break;	/* �A�������o�� */
		}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//		memset(sBuf, 0, sizeof(sBuf));
//		sprintf(sBuf, "(%d,%d)(%d,%d)", x, y, p_stPCG->dx, p_stPCG->dy);
//		BG_TextPut(sBuf, x, y);	/* �f�o�b�O�\�� */
#endif

		bValid = g_stBall[i].bValidty;
		if( bValid != ball_exist )		/* ���݂���{�[���ȊO */
		{
			continue;
		}

		if (p_stPCG->status == FALSE)	/* �ړ���~ */
		{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
			if( (g_uDebugNum & Bit_3) != 0u )	/* �ړ������ς�SP */
			{
				x = p_stPCG->x >> PCG_LSB;
				y = p_stPCG->y >> PCG_LSB;
				width	= Mmul16(p_stPCG->Pat_w);
				height	= Mmul16(p_stPCG->Pat_h);
				Draw_Box(x, y, x + width, y + height, G_L_PINK, 0xFFFF);
			}
#endif
		}
		else
		{
		    Ball_mark_area(p_stPCG, -1, list_num); // �Â��ʒu���N���A

			if(Ball_Point_Update(p_stPCG, list_num) == FALSE)	/* �ړ��ς݁A�Q��q�b�g�A�� */
			{
				if((nBallCount == i) || (nBallCount == -1))		/* �L���������{�[�� */
				{
					g_bShot_OK = TRUE;	/* ���̔��ˋ��� */
				}
			}
			else
			{
			}
			
			Ball_mark_area(p_stPCG, 1, list_num); // �V�����ʒu���}�[�N

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
			if( (g_uDebugNum & Bit_1) != 0u )	/* �ړ���SP */
			{
				if( (g_uDebugNum & Bit_2) != 0u )	/* �����蔻�� */
				{
					x += g_stST_PCG_LIST[list_num].hit_x;
					y += g_stST_PCG_LIST[list_num].hit_y;
					width	= g_stST_PCG_LIST[list_num].hit_width;
					height	= g_stST_PCG_LIST[list_num].hit_height;
					Draw_Box(x, y, x + width, y + height, G_PURPLE, 0xFFFF);
				}
				else	/* Bit_1 SP�̃T�C�Y�͈� */
				{
					x = p_stPCG->x >> PCG_LSB;
					y = p_stPCG->y >> PCG_LSB;
					width	= Mmul16(p_stPCG->Pat_w);
					height	= Mmul16(p_stPCG->Pat_h);
					Draw_Box(x, y, x + width, y + height, G_YELLOW, 0xFFFF);
				}
			}
#endif
		}

		if(p_stPCG->status == TRUE)	/* �ړ��� */
		{
			bNoMoreMove = FALSE;	/* �ړ��������Ă��Ȃ� */
		}

		s_Resume = i;	/* �����ł������� */
	}

	if(bNoMoreMove == TRUE)
	{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		if( (g_uDebugNum & Bit_0) != 0u )	/* �X�e�[�^�X */
		{
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "NMMOVE(%3d.%3d)", nBallCount, g_bBoder_line);
			BG_TextPut(sBuf, 0, 48);	/* �f�o�b�O�\�� */
		}
#endif
		for (i = 0; i < BALL_MAX; i++)
		{
			p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* �{�[�� */
			if(p_stPCG == NULL)
			{
				continue;
			}
			
			bValid = g_stBall[i].bValidty;
			if( bValid == ball_lost )		/* ���݂��Ȃ��{�[�� */
			{
				p_stPCG->update = FALSE;
				continue;
			}
			else if( bValid == ball_exist )		/* ���݂���{�[�� */
			{
				y = p_stPCG->y >> PCG_LSB;
				line = Mmin(y , line);	/* �X�V */

				if(line != g_bBoder_line)
				{
					g_bBoder_line = line;
				}
			}

			if(p_stPCG->status == FALSE)
			{
				p_stPCG->status = TRUE;		/* �ړ��ĊJ */
			}
		}

		s_Resume = 0;	/* ���� */

		if(g_bBoder_line < Y_POS_BD)
		{
			g_bBoder_count = Minc(g_bBoder_count, 1);
		}
		else if(g_bBoder_line >= Y_POS_BD + 64)
		{
			g_bBoder_count = Mdec(g_bBoder_count, 1);
		}
		else
		{
			/* �ێ� */
		}
		
		if(S_IsUpdate_Combo() != 0)
		{
			if(S_Get_Combo() >= COMBO_RECOVER_COUNT)
			{
				g_bBoder_count = Mdec(g_bBoder_count, S_Get_Combo());
			}
		}

		ret = TRUE;
	}
	else
	{
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		if( (g_uDebugNum & Bit_0) != 0u )	/* �X�e�[�^�X */
		{
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "  MOVE(%3d.%3d)", nBallCount, g_bBoder_line);
			BG_TextPut(sBuf, 0, 48);	/* �f�o�b�O�\�� */
		}
#endif
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	printf("Ball_Check st2(%d)\n", count);
#endif

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
//	printf("Ball_Check ed(%d)\n", g_index);
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
//	int8_t sBuf[128] = {0};
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
			k = Mmin(g_stBall[i].bSize, 3) + 1;	/* �ő�l�{�P */
			pal = Mmul256(g_stST_PCG_LIST[k].Pal);
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
	g_stBall[i].bValidty = ball_now;

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
			k = Mmin(g_stBall[j].bSize, 3) + 1;	/* �ő�l�{�P */
			pal = Mmul256(g_stST_PCG_LIST[k].Pal);
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
	g_stBall[j].bValidty = ball_next;	/* �� */

	if(p_stPCG_next != NULL)
	{
		p_stPCG_next->x = 224 << PCG_LSB;
		p_stPCG_next->y = 16 << PCG_LSB;
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

	if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
		((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
		((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
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
						g_stBall[i].bValidty = ball_exist;	/* ���݂���{�[�� */

						g_stBall[j].bValidty = ball_now;	/* ���˃{�[�� */

						*nNum = i;			/* ���݂̃{�[���ԍ��ݒ� */

						if(p_stPCG_ball != NULL)
						{
							p_stPCG_ball->dx = 0;
							p_stPCG_ball->dy = Y_ACC;
							p_stPCG_ball->status = TRUE;
							p_stPCG_ball->update = TRUE;
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//							memset(sBuf, 0, sizeof(sBuf));
//							sprintf(sBuf, "i(%2d)j(%2d)in(%2d)", s_nNum_old, s_nNext_old, *nNum);
//							Draw_Message_To_Graphic(sBuf, 0, 112, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
#endif
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
		printf("0x%X, 0x%X\n", GetInput_P1(), GetInput());
#endif
//	PCG_PUT_1x1(p_stPCG_ship->x, p_stPCG_ship->y, nNum, SetXSPinfo(0, 0, 0x0A, 0x30));
	
	p_stPCG_ship = PCG_Get_Info(SP_SHIP_0);				/* ���@ */
	if(p_stPCG_ship != NULL)
	{
		int16_t width;

		if(	((GetInput_P1() & JOY_LEFT ) != 0u )	||	/* LEFT */
			((GetInput() & KEY_LEFT) != 0 )		)	/* �� */
		{
			if(	((GetInput_P1() & JOY_B ) != 0u)	||		/* B */
				((GetInput() & KEY_b_X) != 0u)		)		/* B(x) */
			{
				p_stPCG_ship->dx = (-2) << PCG_LSB;
			}
			else
			{
				p_stPCG_ship->dx = (-1) << PCG_LSB;
			}
			/* �A�j���[�V���� */
			p_stPCG_ship->Anime++;
			if(p_stPCG_ship->Anime >= p_stPCG_ship->Pat_AnimeMax)
			{
				p_stPCG_ship->Anime = 0;
			}
		}
		else if( ((GetInput_P1() & JOY_RIGHT ) != 0u )	||	/* RIGHT */
				((GetInput() & KEY_RIGHT) != 0 )			)	/* �E */
		{
			if(	((GetInput_P1() & JOY_B ) != 0u)	||		/* B */
				((GetInput() & KEY_b_X) != 0u)		)		/* B(x) */
			{
				p_stPCG_ship->dx = (2) << PCG_LSB;
			}
			else
			{
				p_stPCG_ship->dx = (1) << PCG_LSB;
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

		width	= Mmul16(p_stPCG_ship->Pat_w);
		
		if(p_stPCG_ship->x < ((X_POS_MIN) << PCG_LSB))
		{
			p_stPCG_ship->x = (X_POS_MIN) << PCG_LSB;
			p_stPCG_ship->dx = 0;
		}
		else if(p_stPCG_ship->x > ((X_POS_MAX - width) << PCG_LSB))
		{
			p_stPCG_ship->x = (X_POS_MAX - width) << PCG_LSB;
			p_stPCG_ship->dx = 0;
		}
		else
		{

		}
		p_stPCG_ship->update	= TRUE;

		if(p_stPCG_ball != NULL)
		{
//			p_stPCG_ball->dx = 0;
//			p_stPCG_ball->dy = 0;
			p_stPCG_ball->x = p_stPCG_ship->x;
			p_stPCG_ball->y = p_stPCG_ship->y + (Mmul16(p_stPCG_ship->Pat_h) << PCG_LSB);
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
			p_stPCG->status = TRUE;
			p_stPCG->update	= FALSE;
		}
	}
	g_index = 0;
	g_index_now = 0;
	g_index_next = 0;
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
	int16_t	i, j, k;
	int16_t	pal;
	
	ST_PCG	*p_stPCG = NULL;

	g_index = 0;

	for (i = 0; i < BALL_MAX; i++) {
		g_stBall[i].bSize = 0;
		g_stBall[i].bSP_num = i;
		g_stBall[i].bStatus = (rand() % 3);
		g_stBall[i].bValidty = ball_lost;
	}

#if 1
#else
	if(g_nGameDifflevel == 0)
	{
	}
	else
	{
		for (i = SP_BALL_S_1; i < (SP_BALL_S_40 - SP_BALL_S_1); i++) {
			g_stBall[i].bValidty = ball_exist;
			g_index++;
		}
	}
#endif

	for(i = SP_BALL_S_1; i < PCG_NUM_MAX; i++)
	{
		if(i < BALL_MAX)
		{
			p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* �{�[�� */
		}
		else
		{
			p_stPCG = PCG_Get_Info(i);	/* �L���� */
		}

		if(p_stPCG != NULL)
		{
			switch(i)
			{
				case BG_DATA:
				{
					break;
				}
				case SP_SHIP_0:
				{
					p_stPCG->x = (128) << PCG_LSB;
					p_stPCG->y = (16) << PCG_LSB;
					p_stPCG->dx = 0;
					p_stPCG->dy = 0;
					p_stPCG->status = TRUE;
					p_stPCG->update	= TRUE;
					break;
				}
				case SP_GAL_0:
				{
					p_stPCG->x = (208) << PCG_LSB;
					p_stPCG->y = (Y_POS_BD) << PCG_LSB;
					p_stPCG->dx = 0;
					p_stPCG->dy = 0;
					p_stPCG->status = TRUE;
					p_stPCG->update	= TRUE;
					break;
				}
				case SP_ARROW_0:
				{
					p_stPCG->x = (X_POS_MIN - 16) << PCG_LSB;
					p_stPCG->y = (g_bBoder_line - 8) << PCG_LSB;
					p_stPCG->dx = 0;
					p_stPCG->dy = 0;
					p_stPCG->status = TRUE;
					p_stPCG->update	= TRUE;
					break;
				}
				default:
				{
#if 1
					p_stPCG->x = (128) << PCG_LSB;
					p_stPCG->y = (Y_POS_BD) << PCG_LSB;
					p_stPCG->dx = 0;
					p_stPCG->dy = 0;
					p_stPCG->update	= FALSE;
#else
					if(g_nGameDifflevel == 0)
					{
						p_stPCG->x = (128) << PCG_LSB;
						p_stPCG->y = (Y_POS_BD) << PCG_LSB;
						p_stPCG->dx = 0;
						p_stPCG->dy = 0;
						p_stPCG->update	= FALSE;
					}
					else
					{
						if(g_stBall[i].bValidty == ball_exist)
						{
							p_stPCG->x = (X_POS_MIN + (rand() % 64)) << PCG_LSB;
							p_stPCG->y = (Y_POS_BD + 64 + (rand() % 64)) << PCG_LSB;
							p_stPCG->dx = 0;
							p_stPCG->dy = Y_ACC;
							p_stPCG->update	= TRUE;
						}
						else
						{
							p_stPCG->x = (128) << PCG_LSB;
							p_stPCG->y = (Y_POS_BD) << PCG_LSB;
							p_stPCG->dx = 0;
							p_stPCG->dy = 0;
							p_stPCG->update	= FALSE;
						}
					}
#endif			
					p_stPCG->status = TRUE;

					k = Mmin(g_stBall[i].bSize, 3) + 1;	/* �ő�l�{�P */
					pal = Mmul256(g_stST_PCG_LIST[k].Pal);
					for(j=0; j < p_stPCG->Pat_DataMax; j++)
					{
						*(p_stPCG->pPatCodeTbl + j) &= 0xF0FF;
						*(p_stPCG->pPatCodeTbl + j) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* �J���[�e�[�u�������Z�b�g */
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

	if(g_nGameDifflevel == 0)
	{
	}
	else
	{
		Ball_input();
	}

	g_index_next = g_index + 1;
	g_stBall[g_index_next].bValidty = ball_next;	/* �� */
	g_index_now = g_index;
	g_stBall[g_index_now].bValidty 	= ball_now;		/* ���� */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Ball_input(void)
{
	int16_t	i, j ,k;
	int16_t	pal;
	int8_t sBuf[256] = {0};
	ST_PCG	*p_stPCG = NULL;
	uint16_t dat[BALL_MAX][5] = {0};
	uint16_t	x_max, y_max;

	sprintf(sBuf, "data/Resume%d.dat", g_nGameLevel);

	/* Resume�f�[�^�ǂݍ��� */
	File_Load_CSV( sBuf, (uint16_t *)&dat[0][0], &x_max, &y_max);

	if(x_max > 5)
	{
		puts("ERR:format X�T�C�Y");
		return;
	}
	if(y_max > BALL_MAX)
	{
		puts("ERR:format Y�T�C�Y");
		return;
	}

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	memset(sBuf, 0, sizeof(sBuf));
//	sprintf(sBuf, "(%d,%d)", x_max, y_max);
//	Draw_Message_To_Graphic(sBuf, 0, 240, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
#endif
	k = 0;		

	for (i = 0; i < BALL_MAX; i++)
	{
		if(k >= y_max)
		{
			break;
		}

		if(dat[k][0] != i)
		{
			continue;
		}
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//		memset(sBuf, 0, sizeof(sBuf));
//		sprintf(sBuf, "i%d,sp(%d)s(%d)x(0x%x)y(0x%x)", i, dat[k][0], dat[k][1], dat[k][2], dat[k][3]);
//		Draw_Message_To_Graphic(sBuf, 0, k*12, F_MOJI, F_MOJI_BAK);	/* �X�R�A�\�� */
#endif
		g_stBall[i].bSP_num = dat[k][0];
		g_stBall[i].bSize	= dat[k][1]; 
		g_stBall[i].bStatus = dat[k][2];
		g_stBall[i].bValidty = ball_exist;

		p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ball */
		if(p_stPCG != NULL)
		{
			p_stPCG->x = dat[k][3]; 
			p_stPCG->y = dat[k][4]; 
			p_stPCG->update	= TRUE;
			p_stPCG->status = TRUE;

			pal = Mmin(g_stBall[i].bSize, 3) + 1;	/* �ő�l�{�P */
			pal = Mmul256(g_stST_PCG_LIST[pal].Pal);
			for(j=0; j < p_stPCG->Pat_DataMax; j++)
			{
				*(p_stPCG->pPatCodeTbl + j) &= 0xF0FF;
				*(p_stPCG->pPatCodeTbl + j) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* �J���[�e�[�u�������Z�b�g */
			}
			k++;
			g_index++;
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
static void Ball_Output(void)
{
	int16_t	i;
	int8_t sBuf[256] = {0};
	ST_PCG	*p_stPCG = NULL;
	FILE *fp;

	sprintf(sBuf, "data/Resume%d.dat", g_nGameLevel);

	/* �t�@�C�����J���� */
	fp = fopen(sBuf, "w+");

	for (i = 0; i < BALL_MAX; i++) {

		if(g_stBall[i].bValidty == ball_exist)
		{
			p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ball */

			if(p_stPCG != NULL)
			{
				fprintf(fp, "%d,%d,%d,%d,%d\n",
						g_stBall[i].bSP_num,
						g_stBall[i].bSize,
						g_stBall[i].bStatus,
						p_stPCG->x,
						p_stPCG->y);
			}
		}
	}

	/* �t�@�C������� */
	fclose (fp);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t main_Task(void)
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

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
		Level_meter(TRUE);	/* �������׌v��(�J�n) */
#endif
		if(Get_SP_Sns() == FALSE)
		{
			if(Input_Main(g_ubDemoPlay) != 0u) 	/* ���͏��� */
			{
				g_ubDemoPlay = FALSE;	/* �f������ */
				SetTaskInfo(SCENE_INIT);	/* �V�[��(����������)�֐ݒ� */
			}
		}
		
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		DirectInputKeyNum(&g_uDebugNum, 3);	/* �L�[�{�[�h���琔������� */
#endif

		if((GetInput() & KEY_b_ESC ) != 0u)	/* �d�r�b�L�[ */
		{
			/* �I�� */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 6)
			{

			}
			else if(pushCount >= 5)
			{
				ADPCM_Play(1);	/* �I���� */

				Music_Stop();	/* Stop */

				SetTaskInfo(SCENE_EXIT);	/* �I���V�[���֐ݒ� */
			}
		}
		else if((GetInput() & KEY_b_F6 ) != 0u)	/* F6�L�[ */
		{
			/* ���W���[�� */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 6)
			{

			}
			else if(pushCount >= 5)
			{
				ADPCM_Play(1);	/* �I���� */

				Ball_Output();	/* ��Ԃ�ۑ� */
			}
		}
		else if((GetInput() & KEY_b_HELP ) != 0u)	/* HELP�L�[ */
		{
			if( (stTask.bScene != SCENE_GAME_OVER_S) && (stTask.bScene != SCENE_GAME_OVER) )
			{
				/* ���Z�b�g */
				pushCount = Minc(pushCount, 1);
				if(pushCount >= 6)
				{

				}
				else if(pushCount >= 5)
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
				int8_t sStr[12] = {0};

				Music_Play(3);	/* Title */

				G_VIDEO_PRI(2);	/* TX>GR>SP */

			    PCG_VIEW(0);        /* SP OFF */
				
				G_CLR();		/* �O���t�B�b�N�N���A */
				
				T_Clear();		/* �e�L�X�g�N���A */

				G_Load(1, 0, 0, 0);	/* No.1�F�^�C�g�� */

//				PutGraphic_To_Symbol12("PUSH A BUTTON TO START!", X_POS_MIN, 192, F_MOJI );			/* title��ʓ��͑҂� */
				BG_TextPut("PUSH A BUTTON TO START!", 36, 224);
//				PutGraphic_To_Symbol12("VERSION 0.0.4", X_POS_MAX, 244, F_MOJI );	/* Ver */
				sprintf(sStr, "VER%d.%d.%d", MAJOR_VER, MINOR_VER, PATCH_VER);
				BG_TextPut(sStr, 192, 244);

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
				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
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
				if(g_GameSpeed > 3)
				{
					for (i = 0; i < NUM_STARS; ++i)
					{
						Draw_Pset(rand() % (X_MAX_DRAW/2), rand() % Y_MAX_DRAW, (i % 16) + 240);
					}
				}
				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);		/* �r���̊O�͓h��Ԃ� */
				Draw_Fill(X_POS_MAX, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);	/* �r���̊O�͓h��Ԃ� */

				memcpy((int16_t *)0xE821F0, (int16_t *)0xE82000, sizeof(int16_t) * 16);

				G_PAGE_SET(0b0001);	/* GR0 */

				Draw_Line( X_POS_MIN, Y_POS_BD, X_POS_MAX, Y_POS_BD, 0x1, T_RED);
				y = g_TB_GameLevel[g_nGameLevel];
				Draw_Line( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MIN,          y-1, X_POS_MAX,   y-1, G_GREEN, 0xFFFF);
				
				PutGraphic_To_Symbol12("�㉺", X_POS_MIN+2, 128+12*0, F_MOJI );			/* ���� */
				PutGraphic_To_Symbol12(" �T�C�Y�ύX", X_POS_MIN+2, 128+12*1, F_MOJI );	/* ���� */
				PutGraphic_To_Symbol12("���E", X_POS_MIN+2, 128+12*2, F_MOJI );			/* ���� */
				PutGraphic_To_Symbol12(" ������ԕύX", X_POS_MIN+2, 128+12*3, F_MOJI );	/* ���� */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

				Ball_InitALL();	/* �{�[���̑S������ */

				G_VIDEO_PRI(2);	/* TX>GR>SP */
			    PCG_VIEW(1);    /* SP ON */
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

				if( ( GetInput() & KEY_UPPER ) != 0) { /* �� */
					if(s_bFlagInput == FALSE)
					{
						s_bFlagInput = TRUE;

						y = g_TB_GameLevel[g_nGameLevel];
						Draw_Line( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, G_BG, 0xFFFF);
						Draw_Line( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, G_BG, 0xFFFF);
						Draw_Line( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, G_BG, 0xFFFF);
						Draw_Line( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, G_BG, 0xFFFF);
						Draw_Line( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, G_BG, 0xFFFF);

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
						Draw_Line( X_POS_MIN+1,          y-1, X_POS_MAX,   y-1, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN-2,    Y_POS_MAX, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN,      Y_POS_MAX, X_POS_MIN,     y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX+2,    Y_POS_MAX, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX,      Y_POS_MAX, X_POS_MAX,     y, G_GREEN, 0xFFFF);

						ADPCM_Play(0);	/* SELECT�� */
					}
				}
				else if( ( GetInput() & KEY_LOWER ) != 0 ) { /* �� */
					if(s_bFlagInput == FALSE)
					{
						s_bFlagInput = TRUE;

						y = g_TB_GameLevel[g_nGameLevel];
						Draw_Line( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, G_BG, 0xFFFF);
						Draw_Line( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, G_BG, 0xFFFF);
						Draw_Line( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, G_BG, 0xFFFF);
						Draw_Line( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, G_BG, 0xFFFF);
						Draw_Line( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, G_BG, 0xFFFF);

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
						Draw_Line( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, G_GREEN, 0xFFFF);

						ADPCM_Play(0);	/* SELECT�� */
					}
				}
				else if( ( GetInput() & KEY_LEFT ) != 0)	/* �� */
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
				else if( ( GetInput() & KEY_RIGHT ) != 0)	/* �E */
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

				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
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
#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
				int8_t sBuf[16] = {0};
#endif
				G_PAGE_SET(0b0001);	/* GR0 */

//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1, G_BG);	/* ���b�Z�[�W���� */

				G_Load_Mem(3, 0, 48, 0);	/* No.3�F�Q�[���� */

//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_POS_BD, G_BG);					/* �r���̊O�͓h��Ԃ� */
//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);						/* �r���̊O�͓h��Ԃ� */
				Draw_Fill(X_POS_MIN+1, Y_MIN_DRAW, X_POS_MAX-2, g_TB_GameLevel[g_nGameLevel]-3, G_BG);	/* �r���̒��͓h��Ԃ� */
				Draw_Fill(X_POS_MAX+3, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);				/* �r���̊O�͓h��Ԃ� */
				Draw_Line(X_POS_MIN, Y_POS_BD, X_POS_MAX, Y_POS_BD, G_RED, 0x0F0F);	/* �{�[�_�[���C�� */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
				memset(sBuf, 0, sizeof(sBuf));
				strcpy(sBuf, "FPS");
				Draw_Message_To_Graphic(sBuf, 0, 20, F_MOJI, F_MOJI_BAK);	/* �f�o�b�O */
#endif
				/* �Q�[�������̏����� */
				g_bBoder_line = 0xFF;
				g_bBoder_count = 0;
				g_ubGameClear = FALSE;
				g_nGameClearCount = 0;
				
				memset(g_bAREA, 0, sizeof(g_bAREA));

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

					G_PaletteSet(G_GREEN, SetRGB(  0, 31,  0));	/* Green */
//					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */

					SetTaskInfo(SCENE_GAME1);	/* �V�[��(���{����)�֐ݒ� */
				}
				break;
			}
			case SCENE_GAME1:
			{
				Ball_Move(&g_index);

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif

				Ball_Check(g_index);	/* �X�v���C�g��\�� */

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif

				Boder_line();	/* �{�[�_�[���C�� */

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif
//				printf("Set_Pallet_Shift st(%d)\n", g_index);

				if(g_GameSpeed > 3)
				{
					/* �w�i���� */
					Set_Pallet_Shift(15, 1);
					CRTC_G1_Scroll_8(0, s_G1_sc_y++);
				}

//				printf("Set_Pallet_Shift ed(%d)\n", g_index);

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif

				S_Main_Score();	/* �X�R�A���C������ */

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif
				if(g_ubGameClear == TRUE)	/* �N���A�������� */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* �G���f�B���O(�J�n����)�֐ݒ� */
				}

				if(g_bBoder_count >= 64)
				{
					SetTaskInfo(SCENE_GAME1_E);	/* �V�[��(�I������)�֐ݒ� */
				}

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
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

				G_PaletteSet(G_GREEN, SetRGB(31,  0,  0));	/* Green -> Red */
				//_iocs_tpalet2( 7, SetRGB(31,  0,  0));	/* Red */

				SetTaskInfo(SCENE_GAME2);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_GAME2:
			{
				Ball_Move(&g_index);

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif
				Ball_Check(g_index);	/* �X�v���C�g��\�� */

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif
				Boder_line();	/* �{�[�_�[���C�� */

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif
				if(g_GameSpeed > 3)
				{
					/* �w�i���� */
					Set_Pallet_Shift(15, 1);
					CRTC_G1_Scroll_8(0, s_G1_sc_y++);
				}

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif
				S_Main_Score();	/* �X�R�A���C������ */

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
#endif
				if(g_ubGameClear == TRUE)	/* �N���A�������� */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* �G���f�B���O(�J�n����)�֐ݒ� */
				}

				if(g_bBoder_count == 0)	/* ���A */
				{
					SetTaskInfo(SCENE_GAME1_S);	/* �V�[��(�J�n����)�֐ݒ� */
				}
				else if(g_bBoder_count >= 0xFF)	/* �I�� */
				{
					SetTaskInfo(SCENE_GAME2_E);	/* �V�[��(�I������)�֐ݒ� */
				}
				else
				{
					/* �������Ȃ� */
				}

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
				Level_meter(FALSE);	/* �������׌v�� */
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

				BG_TextPut("   CONGRATULATIONS!    ", 44, 128);
				BG_TextPut("    PUSH A BUTTON!     ", 44, 152);

				/* �w�i���� */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
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

						G_PAGE_SET(0b0001);	/* GR0 */
						y = g_TB_GameLevel[g_nGameLevel];
						Draw_Line( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, G_GREEN, 0xFFFF);

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
				g_bBoder_count = 0;

				SetTaskInfo(SCENE_GAME1_S);	/* �V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_HI_SCORE_S:	/* �n�C�X�R�A�����L���O�V�[��(�J�n����) */
			{
				int16_t y;

				G_VIDEO_PRI(0);	/* GR>TX>SP */

				G_PAGE_SET(0b0001);	/* GR0 */

				y = g_TB_GameLevel[g_nGameLevel];
				Draw_Line( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, G_BG, 0xFFFF);	/* ���� */
				Draw_Line( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, G_BG, 0xFFFF);	/* ���O�� */
				Draw_Line( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, G_BG, 0xFFFF);	/* ������ */
				Draw_Line( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, G_BG, 0xFFFF);	/* �E�O�� */
				Draw_Line( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, G_BG, 0xFFFF);	/* �E����*/

				y = NAME_INP_Y;
				G_CLR_AREA(0, 255, y, 255, 0);	/* �y�[�W 0 */

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
					if(S_Score_Name_Main(GetInput(), rank) >= 3)	/* �X�R�A�l�[������ */
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* �n�C�X�R�A�����L���O�V�[��(�I������)�֐ݒ� */
					}
				}
				else
				{
					if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
						((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
						((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
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
//				BG_OFF(0);
//				BG_OFF(1);
			    PCG_VIEW(0);        /* SP OFF */

				if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
				{
					Music_Play(6);	/* Game Over */
				}
				else
				{
					int16_t y;

					G_VIDEO_PRI(0);	/* GR>TX>SP */

					G_PAGE_SET(0b0001);	/* GR0 */
					
					y = g_TB_GameLevel[g_nGameLevel];
					Draw_Line( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, G_BG, 0xFFFF);
					Draw_Line( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, G_BG, 0xFFFF);
					Draw_Line( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, G_BG, 0xFFFF);
					Draw_Line( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, G_BG, 0xFFFF);
					Draw_Line( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, G_BG, 0xFFFF);

					y = NAME_INP_Y;
					G_CLR_AREA(0, 255, y, 255, 0);	/* �y�[�W 0 */
					
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
					if(	((GetInput_P1() & JOY_A ) != 0u)	||	/* A�{�^�� */
						((GetInput() & KEY_b_Z) != 0u)		||	/* KEY A(z) */
						((GetInput() & KEY_b_SP ) != 0u)	)	/* �X�y�[�X�L�[ */
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
					G_PaletteSet(G_GREEN, SetRGB(  0, 31,  0));	/* Green */
//					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */

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

#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
		if( (stTask.bScene == SCENE_GAME1) || (stTask.bScene == SCENE_GAME2) )	/* �Q�[���� */
		{
			int8_t sBuf[16] = {0};
			/* FPS */
			sprintf(sBuf, "%3d", g_bFPS_PH);
			Draw_Message_To_Graphic(sBuf, 32, 20, F_MOJI, F_MOJI_BAK);	/* �f�o�b�O */
		}
#endif

#ifdef CPU_MONI	/* �f�o�b�O�R�[�i�[ */
		Level_meter(FALSE);	/* �������׌v�� */
#endif

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
#endif

#ifdef MEM_MONI	/* �f�o�b�O�R�[�i�[ */
		GetFreeMem();	/* �󂫃������T�C�Y�m�F */
#endif

	}
	while( loop );

	g_bExit = FALSE;

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
	int16_t nNum;
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �J�n");
#endif
#ifdef MEM_MONI	/* �f�o�b�O�R�[�i�[ */
	g_nMaxUseMemSize = GetFreeMem();
	printf("FreeMem(%d[kb])\n", g_nMaxUseMemSize);	/* �󂫃������T�C�Y�m�F */
	puts("App_Init ������");
#endif
	/* MFP */
	MFP_INIT();	/* V-Sync���荞�ݓ��̏��������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	printf("App_Init MFP(%d)\n", Get_CPU_Time());
#endif
	if(SetNowTime(0) == FALSE)
	{
		/*  */
	}
	nNum = Get_CPU_Time();	/* 300 = 10MHz� */
	if(nNum  < 400)
	{
		g_GameSpeed = 0;
		printf("Normal Speed(%d)\n", g_GameSpeed);
	}
	else if(nNum < 640)
	{
		g_GameSpeed = 2;
		printf("XVI Speed(%d)\n", g_GameSpeed);
	}
	else if(nNum < 800)
	{
		g_GameSpeed = 4;
		printf("RedZone Speed(%d)\n", g_GameSpeed);
	}
	else if(nNum < 2000)
	{
		g_GameSpeed = 7;
		printf("030 Speed(%d)\n", g_GameSpeed);
	}
	else
	{
		g_GameSpeed = 10;
		printf("Another Speed(%d)\n", g_GameSpeed);
	}
	puts("App_Init Input");
	Input_Init();			/* ���͏����� */

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
    PCG_VIEW(1);        /* SP ON / BG0 OFF / BG1 OFF */
	PCG_INIT_CHAR();	/* �X�v���C�g���a�f��`�Z�b�g */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �X�v���C�g�^�a�f");
#endif

	g_Vwait = 1;

#if CNF_MACS
	/* ���� */
	MOV_INIT();		/* ���������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init Movie(MACS)");
#endif
#endif

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
	
#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
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
#ifdef W_BUFFER
		ST_CRT		stCRT;
		GetCRT(&stCRT, g_mode);	/* ��ʍ��W�擾 */
#endif
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

		/* �����N���A */
		G_CLR_AREA(	stCRT.hide_offset_x, WIDTH,
					stCRT.hide_offset_y, HEIGHT, 0);
#endif
		
#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
		bFPS++;
#endif
		ret = 1;
	}

#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
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
#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
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
	Timer_D_Less_NowTime();

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

	App_Init();		/* ������ */
	
	main_Task();	/* ���C������ */

	App_exit();		/* �I������ */
	
	return ret;
}

#endif	/* MAIN_C */
