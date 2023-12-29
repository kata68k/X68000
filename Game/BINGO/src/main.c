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

#define		BALL_MAX	(75)
#define		TBL_CELL	(32)
#define		TBL_COLUMN	(5)
#define		TBL_LINE	(15)
#define 	X_POS_MAX	(511)
#define 	Y_POS_MAX	(511)

#define 	FONT24H		(24)
#define 	FONT12W		(12)

/* �O���[�o���ϐ� */
uint32_t	g_unTime_cal = 0u;
uint32_t	g_unTime_cal_PH = 0u;
int32_t		g_nSuperchk = 0;
int32_t		g_nCrtmod = 0;
int32_t		g_nMaxMemSize;
int16_t		g_CpuTime = 0;
uint8_t		g_mode;
uint8_t		g_mode_rev;
uint8_t		g_Vwait = 1;
uint8_t		g_bFlip = FALSE;
uint8_t		g_bFlip_old = FALSE;
uint8_t		g_bExit = TRUE;

int16_t		g_numbers[BALL_MAX];
int16_t		g_index;
int16_t		g_fly_count = 0;
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

const uint8_t	g_Colors[TBL_COLUMN] = {G_RED, G_YELLOW, G_WHITE, G_GREEN, G_BLUE };
const uint8_t	g_Colors_Dark[TBL_COLUMN] = {G_D_RED, G_D_YELLOW, G_GRAY, G_D_GREEN, G_D_BLUE };
const int16_t	g_OK_tabel[3][3] ={	{ 0, 1, 1 },
									{ 1, 0, 1 },
									{ 1, 1, 0 }};
const uint8_t	g_Ball_Max[8] = { 16, 24, 32, 40, 56, 64, 72, BALL_MAX };


/* �O���[�o���\���� */

/* �֐��̃v���g�^�C�v�錾 */
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
int16_t HIT_Circle_Area(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t	ret = 0;
	int16_t	i;
	int16_t	w, h;
	int16_t	cx, cy;
	int16_t	px, py;
	
	x2 += x1;
	y2 += y1;
	
	w = Mdiv2(x2 - x1);
	h = Mdiv2(y2 - y1);
	cx = x1 + w;
	cy = y1 + h;
	
	for(i=0; i<90; i++)
	{
		/* 1 */
		px = cx + Mdiv256( (w * APL_Cos(i) ) );
		py = cy + Mdiv256( (h * APL_Sin(i) ) );
		g_Circle_Table[i][0] = px;
		g_Circle_Table[i][1] = py;
		g_Circle_Table_x_Min = Mmin(g_Circle_Table_x_Min, px);
		g_Circle_Table_x_Max = Mmax(g_Circle_Table_x_Max, px);
		g_Circle_Table_y_Min = Mmin(g_Circle_Table_y_Min, py);
		g_Circle_Table_y_Max = Mmax(g_Circle_Table_y_Max, py);

		/* 2 */
		px = cx + Mdiv256( (w * APL_Cos(i+90)) );
		py = cy + Mdiv256( (h * APL_Sin(i+90)) );
		g_Circle_Table[i+90][0] = px;
		g_Circle_Table[i+90][1] = py;
		g_Circle_Table_x_Min = Mmin(g_Circle_Table_x_Min, px);
		g_Circle_Table_x_Max = Mmax(g_Circle_Table_x_Max, px);
		g_Circle_Table_y_Min = Mmin(g_Circle_Table_y_Min, py);
		g_Circle_Table_y_Max = Mmax(g_Circle_Table_y_Max, py);

		/* 3 */
		px = cx + Mdiv256( (w * APL_Cos(i+180)) );
		py = cy + Mdiv256( (h * APL_Sin(i+180)) );
		g_Circle_Table[i+180][0] = px;
		g_Circle_Table[i+180][1] = py;
		g_Circle_Table_x_Min = Mmin(g_Circle_Table_x_Min, px);
		g_Circle_Table_x_Max = Mmax(g_Circle_Table_x_Max, px);
		g_Circle_Table_y_Min = Mmin(g_Circle_Table_y_Min, py);
		g_Circle_Table_y_Max = Mmax(g_Circle_Table_y_Max, py);

		/* 4 */
		px = cx + Mdiv256( (w * APL_Cos(i+270)) );
		py = cy + Mdiv256( (h * APL_Sin(i+270)) );
		g_Circle_Table[i+270][0] = px;
		g_Circle_Table[i+270][1] = py;
		g_Circle_Table_x_Min = Mmin(g_Circle_Table_x_Min, px);
		g_Circle_Table_x_Max = Mmax(g_Circle_Table_x_Max, px);
		g_Circle_Table_y_Min = Mmin(g_Circle_Table_y_Min, py);
		g_Circle_Table_y_Max = Mmax(g_Circle_Table_y_Max, py);
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
static void Bingo_table_all(void)
{
	int16_t i;
	int16_t x, y;
	int8_t	sStr[256];

	/*�i�q*/
	for(i = 0; i < TBL_LINE+1+1; i++)
	{
		T_xLine(X_POS_MAX - (TBL_CELL * TBL_COLUMN), Mmax((i * TBL_CELL) - 1, 0), TBL_CELL * TBL_COLUMN, 0xFFFF, 1);
	}
	for(i = 0; i < TBL_COLUMN+1; i++)
	{
		x = X_POS_MAX - (TBL_CELL * TBL_COLUMN) + (i * TBL_CELL);
		y = 0;
		T_yLine(x, y, TBL_CELL * (TBL_LINE+1), 0xFFFF, 1);

		Draw_Fill(x, y, x + TBL_CELL, y + Y_POS_MAX, g_Colors_Dark[i]);
	}
	x = X_POS_MAX - (TBL_CELL * TBL_COLUMN);
	y = 0;
	Draw_Fill(x, y, x + TBL_CELL * TBL_COLUMN, y + 31, G_INDIGO);
	for(i = 0; i < TBL_COLUMN+1; i++)
	{
		x = X_POS_MAX - (TBL_CELL * TBL_COLUMN) + (i * TBL_CELL);
		y = 4;
		switch(i)
		{
			case 0:
			{
				PutGraphic_To_Symbol24("B", x + FONT12W + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("B", x + FONT12W, y, G_RED);
				break;
			}
			case 1:
			{
				PutGraphic_To_Symbol24("I", x + FONT12W + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("I", x + FONT12W, y, G_YELLOW);
				break;
			}
			case 2:
			{
				PutGraphic_To_Symbol24("N", x + FONT12W + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("N", x + FONT12W, y, G_WHITE);
				break;
			}
			case 3:
			{
				PutGraphic_To_Symbol24("G", x + FONT12W + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("G", x + FONT12W, y, G_GREEN);
				break;
			}
			case 4:
			{
				PutGraphic_To_Symbol24("O", x + FONT12W + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("O", x + FONT12W, y, G_BLUE);
				break;
			}
		}
	}

	// 1����BALL_MAX�܂ł̐����̔z����쐬
	for (i = 0; i < BALL_MAX; ++i) {
		g_numbers[i] = i + 1;
		sprintf(sStr, "%2d", g_numbers[i]);
		x = X_POS_MAX - (TBL_CELL * TBL_COLUMN) + ((i / TBL_LINE) * TBL_CELL) + 6;
		y = ((i % TBL_LINE) * TBL_CELL) + TBL_CELL + 2;
		PutGraphic_To_Symbol24(sStr, x, y, G_BLACK);
		PutGraphic_To_Symbol24(sStr, x-1, y-1, G_BLACK);
//		Message_Num(&g_numbers[i], (392-1) + (i / TBL_LINE) * 16, (i % TBL_LINE) * 16, 4, MONI_Type_SS, "%d");
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Bingo_TableOpen(void)
{
	int16_t j;
	int16_t x, y;
	int8_t	sStr[256];

	j = g_numbers[g_index];

	x = X_POS_MAX - (TBL_CELL * TBL_COLUMN) + (((j-1)/TBL_LINE) * TBL_CELL);
	y = TBL_CELL + (((j-1)%TBL_LINE) * TBL_CELL);
	Draw_Fill(x, y, x + TBL_CELL, y + 31, g_Colors[(j-1)/TBL_LINE]);

	x = X_POS_MAX - (TBL_CELL * TBL_COLUMN) + (((j-1) / TBL_LINE) * TBL_CELL) + 6;
	y = (((j-1) % TBL_LINE) * TBL_CELL) + TBL_CELL + 2;
	sprintf(sStr, "%2d", j);
	if((16 <= j) && (j <= 60))
	{
		PutGraphic_To_Symbol24(sStr, x, y, G_WHITE);
		PutGraphic_To_Symbol24(sStr, x-1, y-1, G_BLACK);
	}
	else
	{
		PutGraphic_To_Symbol24(sStr, x, y, G_BLACK);
		PutGraphic_To_Symbol24(sStr, x-1, y-1, G_WHITE);
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Bingo_Close(void)
{
	int16_t x, y;

	x = 15;
	y = 64;

	Draw_Fill(x, y, x + (FONT24H * 15) - 26, y + (FONT24H * 14) - 4, G_BG);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Bingo_Open(void)
{
	int16_t j;
	int16_t x, y;
	int8_t	sStr[256];

	Bingo_Close();	/* �\�������� */

	j = g_numbers[g_index];
	if(j < 10)
	{
		x = 96;
		y = 48;

		sprintf(sStr, "%d", j);
		PutGraphic_To_Symbol24_Xn(sStr, x + 4, y + 4, G_BLACK, 15);
		PutGraphic_To_Symbol24_Xn(sStr, x, y, g_Colors[(j-1)/TBL_LINE], 15);
	}
	else
	{
		x = 0;
		y = 48;

		sprintf(sStr, "%2d", j);
		PutGraphic_To_Symbol24_Xn(sStr, x + 4, y + 4, G_BLACK, 15);
		PutGraphic_To_Symbol24_Xn(sStr, x, y, g_Colors[(j-1)/TBL_LINE], 15);
	}
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Bingo_3_Open(int16_t x, int16_t y, int16_t num)
{
	int16_t j;
	int8_t	sStr[256];

	Draw_Fill(x, y, x + (FONT24H * 4) - 26, y + (FONT24H * 3) - 4, G_BG);

	j = num;

//	Draw_FillCircle( 47, 398, 64, 64, 0xFFFF, T_GREEN );	/* �~A */
//	Draw_FillCircle(147, 398, 64, 64, 0xFFFF, T_YELLOW);	/* �~B */
//	Draw_FillCircle(247, 398, 64, 64, 0xFFFF, T_RED   );	/* �~C */
	Draw_FillCircle( x + TBL_CELL, y + TBL_CELL, TBL_CELL, g_Colors[(j-1)/TBL_LINE],  0, 360, 255);	/* �~A */

	if(j < 10)
	{
		sprintf(sStr, "%d", j);
		PutGraphic_To_Symbol24_Xn(sStr, x+20,       y+8,   G_BLACK, 2);
		PutGraphic_To_Symbol24_Xn(sStr, x+20-1,     y+8-1, G_WHITE, 2);
	}
	else
	{
		sprintf(sStr, "%2d", j);
		if((16 <= j) && (j <= 60))
		{
			PutGraphic_To_Symbol24_Xn(sStr, x+8,     y+8,     G_WHITE, 2);
			PutGraphic_To_Symbol24_Xn(sStr, x+8 - 1, y+8 - 1, G_BLACK, 2);
		}
		else
		{
			PutGraphic_To_Symbol24_Xn(sStr, x+8,     y+8,     G_BLACK, 2);
			PutGraphic_To_Symbol24_Xn(sStr, x+8 - 1, y+8 - 1, G_WHITE, 2);
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
static void Bingo_2_Close(int16_t x, int16_t y)
{
	int8_t str[3] = {0xEB,0x9F,0x00};	/* X */
//	strcpy(str, "��");
//	printf("0x%x,0x%p, %x, %x\n", str, str, str[0], str[1]);
	PutGraphic_To_Symbol24_Xn(str, x+8,         y,       G_BLACK, 2);
	PutGraphic_To_Symbol24_Xn(str, x+8 - 1,     y-1,     G_D_BLUE, 2);
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Bingo_History(int16_t index, int16_t nHis)
{
	int16_t i, j;
	int16_t x, y;
	int8_t	sStr[256];

	x = 0;
	y = 488;

	Draw_Fill(x, y, x + 348, y + TBL_CELL, G_BG);
	for (i = 0; i < nHis; ++i)
	{
		y = 488;
		x = i * TBL_CELL;
		if(index < nHis)
		{
			j = g_numbers[i];
			if(index < i)break;
		}
		else
		{
			j = g_numbers[index - (nHis - 1) + i];
		}
		sprintf(sStr, "%2d", j);
		PutGraphic_To_Symbol24(sStr, x + 1, y + 1, G_BLACK);
		PutGraphic_To_Symbol24(sStr, x, y, g_Colors[(j-1)/TBL_LINE]);
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Bingo_Shuffle(int16_t nBallCount)
{
	int16_t	i, j, k;
	int16_t x, y;
	int16_t x2, y2;
	int16_t dx, dy;
	int16_t hx, hy;
	int8_t hitFlag = FALSE;
	uint32_t uTimer;

	ST_PCG	*p_stPCG[BALL_MAX] = {NULL};
	ST_PCG	*p_stPCG_other[BALL_MAX] = {NULL};

	GetNowTime(&uTimer);	/* ���ݎ������擾 */

	for(i = 0; i < Mmin(nBallCount, BALL_MAX); i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + (g_numbers[i + g_index] - 1));	/* �{�[�� */
	
		if(p_stPCG[i] != NULL)
		{
//			if(i <= (g_index - 1))
//			{
//				p_stPCG[i]->update	= FALSE;
//				p_stPCG[i]->x = -16; 
//				p_stPCG[i]->y = -16;
//				continue;
//			}
			x = p_stPCG[i]->x - 8;
			y = p_stPCG[i]->y - 8;
			hitFlag = FALSE;

			for(j = i; j < Mmin(nBallCount, BALL_MAX); j++)
			{
				if(i == j)
				{
					/* �����͉������Ȃ� */
				}
				else
				{
					p_stPCG_other[j] = PCG_Get_Info(SP_BALL_1 + (g_numbers[j + g_index] - 1));	/* ���̃{�[�� */

					if(p_stPCG_other[j] != NULL)
					{
						x2 = p_stPCG_other[j]->x - 8;
						y2 = p_stPCG_other[j]->y - 8;
						dx = McmpSub(x2, x);
						dy = McmpSub(y2, y);

						if((dx < 16) && (dy < 16))
						{
							hitFlag = TRUE;	/* �Փ� */

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//							printf("[%d,%d](%3d, %3d)(%3d, %3d)(%3d, %3d)\n", i, j, dx, dy, x, y, x2, y2);
#endif
							if(dx == dy)	/* �p */
							{
								/* ��{�[���ݒ� */
								if(p_stPCG[i]->dx >= 0)
								{
									p_stPCG[i]->dx = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG[i]->dx = g_Rand_Table16[g_Rand_num];
								}
								if(p_stPCG[i]->dy >= 0)
								{
									p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num];
								}

								/* ���{�[���ݒ� */
								if(p_stPCG_other[j]->dx >= 0)
								{
									p_stPCG_other[j]->dx = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG_other[j]->dx = g_Rand_Table16[g_Rand_num];
								}
								if(p_stPCG_other[j]->dy >= 0)
								{
									p_stPCG_other[j]->dy = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG_other[j]->dy = g_Rand_Table16[g_Rand_num];
								}
							}
							else if(dx > dy)	/* ���Փ� */
							{
								/* ��{�[���ݒ� */
								if(p_stPCG[i]->dx >= 0)
								{
									p_stPCG[i]->dx = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG[i]->dx = g_Rand_Table16[g_Rand_num];
								}

								/* ���{�[���ݒ� */
								if(p_stPCG_other[j]->dx >= 0)
								{
									p_stPCG_other[j]->dx = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG_other[j]->dx = g_Rand_Table16[g_Rand_num];
								}
							}
							else	/* �c�Փ� */
							{
								/* ��{�[���ݒ� */
								if(p_stPCG[i]->dy >= 0)
								{
									p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num];
								}

								/* ���{�[���ݒ� */
								if(p_stPCG_other[j]->dy >= 0)
								{
									p_stPCG_other[j]->dy = g_Rand_Table16[g_Rand_num] * -1;
								}
								else
								{
									p_stPCG_other[j]->dy = g_Rand_Table16[g_Rand_num];
								}
							}


							g_Rand_num++;
							if(g_Rand_num >= BALL_MAX)g_Rand_num=0;

							break;
						}
					}
				}
			}

			if(hitFlag == FALSE)
			{
				/* �r���̒� */
				if( x < g_Circle_Table_x_Min + Mdiv2(g_Circle_Table_x_Max - g_Circle_Table_x_Min) )
				{
					if(y < g_Circle_Table_y_Min + Mdiv2(g_Circle_Table_y_Max - g_Circle_Table_y_Min) )
					{
						/* 2 */
						for(k=0; k<90; k++)
						{
							hx = g_Circle_Table[k+180][0];
							hy = g_Circle_Table[k+180][1];
							if((x < hx) && (y < hy))
							{
								p_stPCG[i]->dx = g_Rand_Table16[g_Rand_num];
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num];
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->x = hx;
								p_stPCG[i]->y = hy;
								hitFlag = TRUE;	/* �Փ� */
								break;
							}
						}
					}
					else
					{
						/* 3 */
						for(k=0; k<90; k++)
						{
							hx = g_Circle_Table[k+90][0];
							hy = g_Circle_Table[k+90][1];
							if((x < hx) && (y > (hy - SP_Y_OFFSET)))
							{
								p_stPCG[i]->dx = g_Rand_Table16[g_Rand_num];
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->dy = (g_Rand_Table16[g_Rand_num]) * -1;
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->x = hx;
								p_stPCG[i]->y = (hy - SP_Y_OFFSET);
								hitFlag = TRUE;	/* �Փ� */
								break;
							}
						}
					}
				}
				else
				{
					if(y < g_Circle_Table_y_Min + Mdiv2(g_Circle_Table_y_Max - g_Circle_Table_y_Min))
					{
						/* 1 */
						for(k=0; k<90; k++)
						{
							hx = g_Circle_Table[k+270][0];
							hy = g_Circle_Table[k+270][1];
							if((x > (hx - SP_X_OFFSET)) && (y < hy))
							{
								p_stPCG[i]->dx = (g_Rand_Table16[g_Rand_num]) * -1;
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num];
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->x = (hx - SP_X_OFFSET);
								p_stPCG[i]->y = hy;
								hitFlag = TRUE;	/* �Փ� */
								break;
							}
						}
					}
					else
					{
						/* 4 */
						for(k=0; k<90; k++)
						{
							hx = g_Circle_Table[k][0];
							hy = g_Circle_Table[k][1];
							if((x > (hx - SP_X_OFFSET)) && (y > (hy - SP_Y_OFFSET)))
							{
								p_stPCG[i]->dx = (g_Rand_Table16[g_Rand_num]) * -1;
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->dy = (g_Rand_Table16[g_Rand_num]) * -1;
								g_Rand_num++;
								if(g_Rand_num >= BALL_MAX)g_Rand_num=0;
								p_stPCG[i]->x = (hx - SP_X_OFFSET);
								p_stPCG[i]->y = (hy - SP_Y_OFFSET);
								hitFlag = TRUE;	/* �Փ� */
								break;
							}
						}
					}
				}
			}

			if(hitFlag == FALSE)
			{
				/* �r���̊O */
				if(x < g_Circle_Table_x_Min)
				{
					p_stPCG[i]->dx = g_Rand_Table4[g_Rand_num];
					p_stPCG[i]->x = g_Circle_Table_x_Min;
					hitFlag = TRUE;	/* �Փ� */
				}
				if(x > g_Circle_Table_x_Max)
				{
					p_stPCG[i]->dx = (g_Rand_Table4[g_Rand_num]) * -1;
					p_stPCG[i]->x = g_Circle_Table_x_Max;
					hitFlag = TRUE;	/* �Փ� */
				}

				if(y < g_Circle_Table_y_Min)
				{
					p_stPCG[i]->dy = g_Rand_Table4[g_Rand_num];
					p_stPCG[i]->y = g_Circle_Table_y_Min;
					hitFlag = TRUE;	/* �Փ� */
				}
				if(y > g_Circle_Table_y_Max)
				{
					p_stPCG[i]->dy = (g_Rand_Table4[g_Rand_num]) * -1;
					p_stPCG[i]->y = g_Circle_Table_y_Max;
					hitFlag = TRUE;	/* �Փ� */
				}
			}

			if(hitFlag == FALSE)
			{
				p_stPCG[i]->dx = (p_stPCG[i]->dx > 0) ? Mmax(p_stPCG[i]->dx - 1, -1) : (p_stPCG[i]->dx + 1);
				p_stPCG[i]->dy = (p_stPCG[i]->dy > 0) ? Mmax(p_stPCG[i]->dy + 1, 8)  : (p_stPCG[i]->dy + 1);
			}
			p_stPCG[i]->update	= TRUE;

			if(GetPassTime(3, &uTimer) == TRUE)break;
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
static void Bingo_Fly(int16_t nBallCount, int8_t bFinal)
{
	int16_t	i;
	int16_t x, y;
	int16_t sign;
	static int8_t bFlag = TRUE;
	
	ST_PCG	*p_stPCG[BALL_MAX] = {NULL};

	for(i = g_index; i < BALL_MAX; i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + (g_numbers[i] - 1));	/* �{�[�� */
	
		if(p_stPCG[i] != NULL)
		{
//			if(i <= (g_index - 1))
//			{
//				p_stPCG[i]->update	= FALSE;
//				p_stPCG[i]->x = -16; 
//				p_stPCG[i]->y = -16;
//				continue;
//			}
			if(bFinal == TRUE)
			{
				if(bFlag == TRUE)
				{
					sign = ((rand() % 2) == 0)?1:-1;
					p_stPCG[i]->dx = (rand() % 16) * sign;
					sign = ((rand() % 2) == 0)?1:-1;
					p_stPCG[i]->dy = (rand() % 16) * sign;

				}
			}
			else
			{
				bFlag = TRUE;
				p_stPCG[i]->dx = 0;
				p_stPCG[i]->dy = 0;
			}
			p_stPCG[i]->update	= TRUE;
		}
	}

	if(bFinal == TRUE)
	{
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
		}
	}

	for(i = nBallCount; i < Mmin(nBallCount + 1, BALL_MAX); i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + (g_numbers[i + g_index] - 1));	/* �{�[�� */
	
		if(p_stPCG[i] != NULL)
		{
			x = p_stPCG[i]->x;
			y = p_stPCG[i]->y;
			
			if( y < 0 )
			{
				/* ��ʊO */
				p_stPCG[i]->y = -32;
				p_stPCG[i]->dx = 0;
				p_stPCG[i]->dy = 0;
				continue;
			}

			p_stPCG[i]->dx = 0;
			p_stPCG[i]->dy -= 32;

			p_stPCG[i]->update	= TRUE;
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
static void Bingo_Ball_Init(void)
{
	int16_t	i;
	int16_t sign;
	
	ST_PCG	*p_stPCG[BALL_MAX] = {NULL};

	for(i = 0; i < BALL_MAX; i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + i);	/* �{�[�� */

		if(p_stPCG[i] != NULL)
		{
			p_stPCG[i]->x = rand() % 350;
			p_stPCG[i]->y = rand() % 400;
			sign = ((rand() % 2) == 0)?1:-1;
			p_stPCG[i]->dx = (rand() % 4) * sign;
			sign = ((rand() % 2) == 0)?1:-1;
			p_stPCG[i]->dy = (rand() % 4) * sign;

			if((p_stPCG[i]->dx == 0) && (p_stPCG[i]->dy == 0))
			{
				sign = ((rand() % 2) == 0)?1:-1;
				p_stPCG[i]->dx = sign;
				sign = ((rand() % 2) == 0)?1:-1;
				p_stPCG[i]->dy = sign;
			}
			p_stPCG[i]->update	= TRUE;
		}
	}
	g_index = 0;
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


	uint16_t	uFreeRunCount=0u;

	int16_t	loop;
	int16_t	pushCount = 0;

	uint8_t	bMode;
	
	ST_TASK		stTask = {0}; 
	
	/* �ϐ��̏����� */
	SetGameMode(1);
	
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
		
		if((g_Input & KEY_b_ESC ) != 0u)	/* �d�r�b�L�[ */
		{
			/* �I�� */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 5)
			{
				ADPCM_Play(5);	/* SET */

				Music_Play(1);	/* Stop */

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
					ADPCM_Play(5);	/* SET */

					Music_Play(1);	/* Stop */

					SetTaskInfo(SCENE_GAME_OVER_S);	/* �I���V�[���֐ݒ� */
				}
			}
		}		
		else
		{
			pushCount = 0;
		}

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* �������V�[�� */
			{
				printf("BINGO�}�V�����N�����܂��B\n");
				T_Clear();	/* �e�L�X�g�N���A */

				G_PaletteSetZero();	/* 0�ԃp���b�g�ύX */

				Set_CRT_Contrast(0);	/* �^���Âɂ��� */

				SetTaskInfo(SCENE_TITLE_S);	/* �V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_TITLE_S:
			{
				int16_t x, y;

				Bingo_Ball_Init();	/* �{�[���̏����� */

				G_PAGE_SET(2);	/* �� */
				Draw_Fill(0, 0, X_POS_MAX, Y_POS_MAX, G_PURPLE);
				G_PAGE_SET(1);	/* ��O */

				G_Load_Mem(1, 16, 64, 1);	/* �o�j�[�K�[�� */

				Bingo_table_all();	/* �r���S�e�[�u���`�� */

				x = 0;
				y = 0;
				PutGraphic_To_Symbol24_Xn("BINGO��PRO-68k", x + 1, y + 1, G_BLACK, 2);
				PutGraphic_To_Symbol24_Xn("BINGO��PRO-68k", x, y, G_WHITE, 2);

				x = 280;
				y = 48;
				PutGraphic_To_Symbol12("ver 0.9.2", x + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol12("ver 0.9.2", x, y, G_WHITE);

				x = 0;
				y = 464;
				PutGraphic_To_Symbol24("Push Space Key or A button", x + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("Push Space Key or A button", x, y, G_WHITE);

				PCG_ON();	/* �X�v���C�gON */

				Music_Play(3);	/* BINGO */
				
				Set_CRT_Contrast(-1);	/* ���Ƃɖ߂� */

				g_Vwait = 1;

				SetTaskInfo(SCENE_TITLE);	/* �V�[��(����)�֐ݒ� */
				break;
			}
			case SCENE_TITLE:
			{
				if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
				{
					ADPCM_Play(5);	/* SET */
					SetTaskInfo(SCENE_TITLE_E);	/* �V�[��(�I������)�֐ݒ� */
				}
				break;
			}
			case SCENE_TITLE_E:
			{
				if(ADPCM_SNS() == 0)
				{
					int16_t x, y;
					
					x = 0;
					y = 464;
					G_PAGE_SET(1);	/* ��O */
					Draw_Fill(x, y, 350, Y_POS_MAX, G_BG);

					SetTaskInfo(SCENE_START_S);	/* �V�[��(�J�n����)�֐ݒ� */
				}
				break;
			}
			case SCENE_START_S:	/* �V�[��(�J�n����) */
			{
				int16_t i, j, k;
				int16_t x, y;
				int16_t temp;
				
				Music_Play(4);	/* Play */
				
				// �����̏�����
				srand(time(NULL));

				// Fisher-Yates�A���S���Y�����g�p���Đ������V���b�t��
				for (i = 74; i > 0; --i) {
					j = rand() % (i + 1);
					// ����������
					temp = g_numbers[i];
					g_numbers[i] = g_numbers[j];
					g_numbers[j] = temp;
					g_Sign_Table[i] = ((rand() % 2) == 0)?-1:1;	/* �������� */
					g_Rand_Table4[i]  = rand() % 4;		/* �����e�[�u�� */
					g_Rand_Table8[i]  = rand() % 8;		/* �����e�[�u�� */
					g_Rand_Table16[i] = rand() % 16;	/* �����e�[�u�� */
					g_Rand_Table32[i] = rand() % 32;	/* �����e�[�u�� */
				}

				k = 70;
#if 0
				for (i = 0; i < k; ++i)
				{
					Bingo_TableOpen();	/* �r���S���ЂƂ����� */
					Bingo_Open();	/* ���� */
					Bingo_History(g_index, 10);	/* ���݂̒l����ߋ��P�O�� */
					g_index++;
				}
#endif				
				T_Circle(17, 70, 308, 322, 0xFFFE, T_WHITE);	/* �r���̒� */
				HIT_Circle_Area(17, 70, 308, 322);	/* �r���̒��̓����蔻�� */

				x = 0;
				y = 464;
				PutGraphic_To_Symbol24("History of the last 10 times", x + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("History of the last 10 times", x, y, G_WHITE);

				SetTaskInfo(SCENE_START);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_START:	/* �V�[��(���{����) */
			{
				static int16_t bFlag = FALSE;

				if( (g_Input_P[0] != 0) || 
					(g_Input != 0u)		)
				{
					if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* A */
						((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
					{
						if(bFlag == FALSE)
						{
							bFlag = TRUE;

							ADPCM_Play(5);	/* SET */

							SetTaskInfo(SCENE_START_E);	/* �V�[��(�I������)�֐ݒ� */
						}
					}
				}
				else
				{
					bFlag = FALSE;
				}

				break;
			}
			case SCENE_START_E:	/* �V�[��(�I������) */
			{
				if(ADPCM_SNS() == 0)
				{
					if(g_index >= BALL_MAX)
					{
						SetTaskInfo(SCENE_GAME_OVER_S);	/* �V�[��(�J�n����)�֐ݒ� */
					}
					else
					{
						Music_Play(5);	/* Select */

						g_OK_num = rand() % 3;
						
						Bingo_Close();	/* �\�������� */

						SetTaskInfo(SCENE_GAME_S);	/* �V�[��(�J�n����)�֐ݒ� */
					}
				}
				break;
			}
			case SCENE_GAME_S:
			{
				if(ADPCM_SNS() == 0)
				{
					static int16_t nNum_old = -1;

					if(g_fly_count >= 3)
					{
						int16_t nPlayNum = 0;
						g_fly_count = 0;
						nNum_old = -1;

						nPlayNum = rand() % 6;
						/* ���� */
						if(nPlayNum > 3)
						{
							G_Scroll(17+28, 70, 0);
							G_Scroll(17+28, 70, 1);
							MOV_Play2(nPlayNum, 0x64);		/* �J�b�g�C�� */
							G_Scroll(0, 0, 0);
							G_Scroll(0, 0, 1);
						}
						else
						{
							PCG_OFF();				/* SP OFF */
							MOV_Play(nPlayNum);		/* �J�b�g�C�� */
						}
						CRTC_INIT(0x100 + 8);			/* �X�v���C�g�̃Y���΍� */

						SetTaskInfo(SCENE_GAME);	/* �V�[��(���{����)�֐ݒ� */
					}
					else
					{
						int16_t nNum;
						int16_t x, y;
						int16_t w, h;

						if(g_OK_tabel[g_OK_num][g_fly_count] == 0)
						{
							nNum = 0;
						}
						else
						{
							int16_t nMaxRange;
							nMaxRange = g_Ball_Max[7] - g_index + 1;
							nNum = rand() % nMaxRange;	/* �ő吔���烉���_���Œ��I */
							nNum += 1;

							if(nNum_old == -1)
							{
							}
							else
							{
								if(nNum == nNum_old)
								{
									nNum += 1; 
								}
							}

							if(nNum >= nMaxRange)
							{
								nNum = 1;
								if(nNum >= nMaxRange)
								{
									nNum = 0;
								}
							}
							nNum_old = nNum;	/* 1�`�c�� */
						}
						nNum = g_index + nNum;
	
						ADPCM_Play(1 + g_fly_count);	/* �V���b�g */

						x = 47 + (g_fly_count * 100);
						y = 398;
						w = 60;
						h = 60;
						Bingo_3_Open( x, y, g_numbers[nNum]);	/* �I�[�v�� */
						T_Circle( x, y, w, h, 0xFFFF, T_BLACK1 );	/* �~A */
						g_fly_count++;
					}
				}
				break;
			}
			case SCENE_GAME:
			{
				if(ADPCM_SNS() == 0)
				{
					if(g_fly_count >= 3)
					{
						g_fly_count = 0;

						ADPCM_Play(4);	/* �q�b�g */
						
						Bingo_TableOpen();	/* �r���S���ЂƂ����� */
						Bingo_Open();	/* ���� */
						Bingo_History(g_index, 10);	/* ���݂̒l����ߋ��P�O�� */
						g_index++;

						SetTaskInfo(SCENE_GAME_E);	/* �V�[��(�I������)�֐ݒ� */
					}
					else
					{
						if(g_OK_tabel[g_OK_num][g_fly_count] == 0)
						{
							/* �������Ȃ� */
						}
						else
						{
							ADPCM_Play(0);	/* BOM2 */
							Bingo_2_Close(47 + (g_OK_tabel[g_OK_num][g_fly_count] * (g_fly_count * 100)), 398);	/* �N���[�Y */
						}
						g_fly_count++;
					}
				}
				break;
			}
			case SCENE_GAME_E:
			{
				if(ADPCM_SNS() == 0)
				{
					Music_Play(4);	/* Play */

					PCG_ON();		/* SP ON */

					SetTaskInfo(SCENE_START);	/* �V�[��(���{����)�֐ݒ� */
				}
				break;
			}
			case SCENE_GAME_OVER_S:
			{
				int16_t x, y;

				Music_Play(6);	/* Game Over */

				x = 0;
				y = 464;
				G_PAGE_SET(1);	/* ��O */
				Draw_Fill(x, y, 350, Y_POS_MAX, G_BG);

				PutGraphic_To_Symbol24("Game Over", x + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("Game Over", x, y, G_WHITE);

				SetTaskInfo(SCENE_GAME_OVER);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_GAME_OVER:
			{
				if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
				{
					ADPCM_Play(5);	/* SET */
					SetTaskInfo(SCENE_GAME_OVER_E);	/* �V�[��(�I������)�֐ݒ� */
				}
				break;
			}
			case SCENE_GAME_OVER_E:
			{
				if(ADPCM_SNS() == 0)
				{
					int16_t x, y;
					
					x = 0;
					y = 60;
					G_PAGE_SET(1);	/* ��O */
					Draw_Fill(x, y, 350, Y_POS_MAX, G_BG);

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

		PCG_Main();	/* �X�v���C�g�Ǘ� */

		uFreeRunCount++;	/* 16bit �t���[�����J�E���^�X�V */

		/* �����҂� */
		wait_vdisp(g_Vwait);

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
	g_nSuperchk = _dos_super(0);
	if( g_nSuperchk < 0 ) {
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		puts("App_Init ���ł�_dos_super");
#endif
	} else {
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		puts("App_Init _dos_super�J�n");
#endif
	}

	/* ��� */
	g_nCrtmod = CRTC_INIT(8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(0x100+8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(10);	/* mode=10 256x256 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(12);	/* mode=12 512x512 col:16/65536 31kHz */
//	g_nCrtmod = CRTC_INIT(16);	/* mode=16 764x512 col:16 31kHz */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init CRTC(���)");
//	KeyHitESC();	/* �f�o�b�O�p */
#endif

	/* �O���t�B�b�N */
	G_INIT();			/* ��ʂ̏����ݒ� */
	G_CLR();			/* �N���b�s���O�G���A�S�J������ */
	G_HOME(0);			/* �z�[���ʒu�ݒ� */
//	G_VIDEO_INIT();		/* �r�f�I�R���g���[���[�̏����� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �O���t�B�b�N");
#endif

	/* �X�v���C�g�^�a�f */
	PCG_INIT();	/* �X�v���C�g�^�a�f�̏����� */
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

	/* Task */
	TaskManage_Init();
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init Task");
#endif

	/* �e�L�X�g */
	T_INIT();	/* �e�L�X�g�u�q�`�l������ */
	T_PALET();	/* �e�L�X�g�p���b�g������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init T_INIT");
#endif
	
	/* �}�E�X������ */
//	Mouse_Init();	/* �}�E�X������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//	puts("App_Init �}�E�X");
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
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �J�n");
#endif
	
	if(g_bExit == TRUE)
	{
		puts("�G���[���L���b�`�I ESC to skip");
		KeyHitESC();	/* �f�o�b�O�p */
	}
	
	/* �O���t�B�b�N */
	G_CLR();			/* �N���b�s���O�G���A�S�J������ */

	/* �X�v���C�g���a�f */
	PCG_OFF();		/* �X�v���C�g���a�f��\�� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �X�v���C�g");
#endif

	/* ��� */
	CRTC_EXIT(0x100 + g_nCrtmod);	/* ���[�h�����Ƃɖ߂� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit ���");
#endif

	/* �e�L�X�g */
	T_EXIT();				/* �e�L�X�g�I������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �e�L�X�g");
#endif
	
	/* MFP */
	MFP_EXIT();				/* MFP�֘A�̉��� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit MFP");
#endif

	/* ���y */
	Exit_Music();			/* ���y��~ */
	puts("App_exit Music");

	Mouse_Exit();	/* �}�E�X��\�� */

	MyMfree(0);				/* �S�Ẵ���������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit ������");
#endif

	_dos_kflushio(0xFF);	/* �L�[�o�b�t�@���N���A */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �L�[�o�b�t�@�N���A");
#endif

	/*�X�[�p�[�o�C�U�[���[�h�I��*/
	_dos_super(g_nSuperchk);
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �X�[�p�[�o�C�U�[���[�h�I��");
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	{
//		int16_t i;
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
	
	if(g_bFlip == FALSE)	/* �`�撆�Ȃ̂Ńt���b�v���Ȃ� */
	{
		return ret;
	}
	
	SetFlip(FALSE);			/* �t���b�v�֎~ */
	
	/* ��ʐ؂�ւ� */
	G_HOME(g_mode);
	
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
	
	ret = 1;

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

	if(stTask.b16ms == TRUE)	/* 8ms���� */
	{
		switch(stTask.bScene)
		{
			case SCENE_INIT:
			case SCENE_TITLE_S:
			case SCENE_TITLE:
			case SCENE_TITLE_E:
			case SCENE_START_S:
			{
				Bingo_Fly(BALL_MAX, TRUE);	/* �X�v���C�g��\�� */
				break;
			}
			case SCENE_START:
			{
				Bingo_Shuffle(g_Ball_Max[g_GameSpeed]);	/* �X�v���C�g��\�� */
				break;
			}
			case SCENE_GAME_S:
			{
				Bingo_Fly(g_fly_count, (g_fly_count == 3));	/* �X�v���C�g��\�� */
				break;
			}
		}
	}
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

//	RasterProc(pRaster_cnt);	/*�@���X�^�[���荞�ݏ��� */

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
	/* ���͏��� */
	Input_Main();
	
	/* ���X�^�[���荞�ݏ����Z�b�g */
//	RasterVcyncProc();
	
	/* ��ʐ؂�ւ� */
	App_FlipProc();

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
