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

/* グローバル変数 */
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

int16_t		g_numbers[75];
int16_t		g_index;
int16_t		g_fly_count = 0;
int16_t		g_OK_num = 0;
int16_t		g_Circle_Table[360][2];
int16_t		g_Sign_Table[75];
int16_t		g_Rand_Table4[75];
int16_t		g_Rand_Table8[75];
int16_t		g_Rand_Table16[75];
int16_t		g_Rand_Table32[75];
int16_t		g_Rand_num = 0;
int16_t		g_GameSpeed;

const uint8_t	g_Colors[5] = {G_RED, G_YELLOW, G_WHITE, G_GREEN, G_BLUE };
const uint8_t	g_Colors_Dark[5] = {G_D_RED, G_D_YELLOW, G_GRAY, G_D_GREEN, G_D_BLUE };
const int16_t	g_OK_tabel[3][3] ={	{ 0, 1, 1 },
									{ 1, 0, 1 },
									{ 1, 1, 0 }};
const uint8_t	g_Ball_Max[8] = { 16, 24, 32, 40, 56, 64, 72, 75 };


/* グローバル構造体 */

/* 関数のプロトタイプ宣言 */
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

void (*usr_abort)(void);	/* ユーザのアボート処理関数 */

/* 関数 */

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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
		/* 2 */
		px = cx + Mdiv256( (w * APL_Cos(i+90)) );
		py = cy + Mdiv256( (h * APL_Sin(i+90)) );
		g_Circle_Table[i+90][0] = px;
		g_Circle_Table[i+90][1] = py;

		/* 3 */
		px = cx + Mdiv256( (w * APL_Cos(i+180)) );
		py = cy + Mdiv256( (h * APL_Sin(i+180)) );
		g_Circle_Table[i+180][0] = px;
		g_Circle_Table[i+180][1] = py;

		/* 4 */
		px = cx + Mdiv256( (w * APL_Cos(i+270)) );
		py = cy + Mdiv256( (h * APL_Sin(i+270)) );
		g_Circle_Table[i+270][0] = px;
		g_Circle_Table[i+270][1] = py;
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
static void Bingo_table_all(void)
{
	int16_t i;
	int16_t x, y;
	int8_t	sStr[256];

	/*格子*/
	for(i = 0; i < 16+1; i++)
	{
		T_xLine(511 - (32 * 5), Mmax((i * 32) - 1, 0), 32 * 5, 0xFFFF, 1);
	}
	for(i = 0; i < 5+1; i++)
	{
		x = 511 - (32 * 5) + (i * 32);
		y = 0;
		T_yLine(x, y, 32 * 16, 0xFFFF, 1);

		Draw_Fill(x, y, x + 32, y + 511, g_Colors_Dark[i]);
	}
	x = 511 - (32 * 5);
	y = 0;
	Draw_Fill(x, y, x + 32 * 5, y + 31, G_INDIGO);
	for(i = 0; i < 5+1; i++)
	{
		x = 511 - (32 * 5) + (i * 32);
		y = 4;
		switch(i)
		{
			case 0:
			{
				PutGraphic_To_Symbol24("B", x + 12 + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("B", x + 12, y, G_RED);
				break;
			}
			case 1:
			{
				PutGraphic_To_Symbol24("I", x + 12 + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("I", x + 12, y, G_YELLOW);
				break;
			}
			case 2:
			{
				PutGraphic_To_Symbol24("N", x + 12 + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("N", x + 12, y, G_WHITE);
				break;
			}
			case 3:
			{
				PutGraphic_To_Symbol24("G", x + 12 + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("G", x + 12, y, G_GREEN);
				break;
			}
			case 4:
			{
				PutGraphic_To_Symbol24("O", x + 12 + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("O", x + 12, y, G_BLUE);
				break;
			}
		}
	}

	// 1から75までの数字の配列を作成
	for (i = 0; i < 75; ++i) {
		g_numbers[i] = i + 1;
		sprintf(sStr, "%2d", g_numbers[i]);
		x = 511 - (32 * 5) + ((i / 15) * 32) + 6;
		y = ((i % 15) * 32) + 32 + 2;
		PutGraphic_To_Symbol24(sStr, x, y, G_BLACK);
		PutGraphic_To_Symbol24(sStr, x-1, y-1, G_BLACK);
//		Message_Num(&g_numbers[i], (392-1) + (i / 15) * 16, (i % 15) * 16, 4, MONI_Type_SS, "%d");
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_TableOpen(void)
{
	int16_t j;
	int16_t x, y;
	int8_t	sStr[256];

	j = g_numbers[g_index];

	x = 511 - (32 * 5) + (((j-1)/15) * 32);
	y = 32 + (((j-1)%15) * 32);
	Draw_Fill(x, y, x + 32, y + 31, g_Colors[(j-1)/15]);

	x = 511 - (32 * 5) + (((j-1) / 15) * 32) + 6;
	y = (((j-1) % 15) * 32) + 32 + 2;
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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_Close(void)
{
	int16_t x, y;

	x = 15;
	y = 64;

	Draw_Fill(x, y, x + (24 * 15) - 26, y + (24 * 14) - 4, G_BG);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_Open(void)
{
	int16_t j;
	int16_t x, y;
	int8_t	sStr[256];

	Bingo_Close();	/* 表示を消す */

	j = g_numbers[g_index];
	if(j < 10)
	{
		x = 96;
		y = 48;

		sprintf(sStr, "%d", j);
		PutGraphic_To_Symbol24_Xn(sStr, x + 4, y + 4, G_BLACK, 15);
		PutGraphic_To_Symbol24_Xn(sStr, x, y, g_Colors[(j-1)/15], 15);
	}
	else
	{
		x = 0;
		y = 48;

		sprintf(sStr, "%2d", j);
		PutGraphic_To_Symbol24_Xn(sStr, x + 4, y + 4, G_BLACK, 15);
		PutGraphic_To_Symbol24_Xn(sStr, x, y, g_Colors[(j-1)/15], 15);
	}
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_3_Open(int16_t x, int16_t y, int16_t num)
{
	int16_t j;
	int8_t	sStr[256];

	Draw_Fill(x, y, x + (24 * 4) - 26, y + (24 * 3) - 4, G_BG);

	j = num;

//	Draw_FillCircle( 47, 398, 64, 64, 0xFFFF, T_GREEN );	/* 円A */
//	Draw_FillCircle(147, 398, 64, 64, 0xFFFF, T_YELLOW);	/* 円B */
//	Draw_FillCircle(247, 398, 64, 64, 0xFFFF, T_RED   );	/* 円C */
	Draw_FillCircle( x + 32, y + 32, 32, g_Colors[(j-1)/15],  0, 360, 255);	/* 円A */

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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_2_Close(int16_t x, int16_t y)
{
	int8_t str[3] = {0xEB,0x9F,0x00};	/* X */
//	strcpy(str, "罰");
//	printf("0x%x,0x%p, %x, %x\n", str, str, str[0], str[1]);
	PutGraphic_To_Symbol24_Xn(str, x+8,         y,       G_BLACK, 2);
	PutGraphic_To_Symbol24_Xn(str, x+8 - 1,     y-1,     G_D_BLUE, 2);
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_History(int16_t index, int16_t nHis)
{
	int16_t i, j;
	int16_t x, y;
	int8_t	sStr[256];

	x = 0;
	y = 488;

	Draw_Fill(x, y, x + 348, y + 32, G_BG);
	for (i = 0; i < nHis; ++i)
	{
		y = 488;
		x = i * 32;
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
		PutGraphic_To_Symbol24(sStr, x, y, g_Colors[(j-1)/15]);
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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

	ST_PCG	*p_stPCG[75] = {NULL};
	ST_PCG	*p_stPCG_other[75] = {NULL};

	GetNowTime(&uTimer);	/* 現在時刻を取得 */

	for(i = 0; i < Mmin(nBallCount + g_index, 75); i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + g_numbers[i] - 1);	/* ステアリングポジション */
	
		if(p_stPCG[i] != NULL)
		{
			if(i <= (g_index - 1))
			{
				p_stPCG[i]->update	= FALSE;
				p_stPCG[i]->x = -16; 
				p_stPCG[i]->y = -16;
				continue;
			}
			x = p_stPCG[i]->x;
			y = p_stPCG[i]->y;
			hitFlag = FALSE;

			for(j = i; j < Mmin(nBallCount + g_index, 75); j++)
			{
				if(i == j)
				{
					/* 自分は何もしない */
				}
				else
				{
					p_stPCG_other[j] = PCG_Get_Info(SP_BALL_1 + g_numbers[j] - 1);	/* ステアリングポジション */

					if(p_stPCG[j] != NULL)
					{
						x2 = p_stPCG_other[j]->x;
						y2 = p_stPCG_other[j]->y;
						dx = McmpSub(x2, x);
						dy = McmpSub(y2, y);
						if((dx < 8) && (dy < 8))
						{
#ifdef DEBUG	/* デバッグコーナー */
							printf("[%d,%d](%3d, %3d)(%3d, %3d)(%3d, %3d)\n", i, j, dx, dy, x, y, x2, y2);
#endif
							hitFlag = TRUE;	/* 衝突 */

							if(p_stPCG[i]->dx >= 0)
							{
								p_stPCG[i]->dx = g_Rand_Table8[g_Rand_num] * -1;
							}
							else
							{
								p_stPCG[i]->dx = g_Rand_Table8[g_Rand_num];
							}
							if(p_stPCG[i]->dy >= 0)
							{
								p_stPCG[i]->dy = g_Rand_Table8[g_Rand_num] * -1;
							}
							else
							{
								p_stPCG[i]->dy = g_Rand_Table8[g_Rand_num];
							}

							if(p_stPCG[j]->dx >= 0)
							{
								p_stPCG[j]->dx = g_Rand_Table8[g_Rand_num] * -1;
							}
							else
							{
								p_stPCG[j]->dx = g_Rand_Table8[g_Rand_num];
							}
							if(p_stPCG[j]->dy >= 0)
							{
								p_stPCG[j]->dy = g_Rand_Table8[g_Rand_num] * -1;
							}
							else
							{
								p_stPCG[j]->dy = g_Rand_Table8[g_Rand_num];
							}

							g_Rand_num++;
							if(g_Rand_num >= 75)g_Rand_num=0;

							break;
						}
					}
				}
			}

			if(hitFlag == FALSE)
			{
				/* ビンの中 */
				if( x < 163 )
				{
					if(y < 196)
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
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num];
								g_Rand_num++;
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->x = hx;
								p_stPCG[i]->y = hy;
								hitFlag = TRUE;	/* 衝突 */
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
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->dy = (g_Rand_Table16[g_Rand_num]) * -1;
								g_Rand_num++;
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->x = hx;
								p_stPCG[i]->y = (hy - SP_Y_OFFSET);
								hitFlag = TRUE;	/* 衝突 */
								break;
							}
						}
					}
				}
				else
				{
					if(y < 196)
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
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->dy = g_Rand_Table16[g_Rand_num];
								g_Rand_num++;
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->x = (hx - SP_X_OFFSET);
								p_stPCG[i]->y = hy;
								hitFlag = TRUE;	/* 衝突 */
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
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->dy = (g_Rand_Table16[g_Rand_num]) * -1;
								g_Rand_num++;
								if(g_Rand_num >= 75)g_Rand_num=0;
								p_stPCG[i]->x = (hx - SP_X_OFFSET);
								p_stPCG[i]->y = (hy - SP_Y_OFFSET);
								hitFlag = TRUE;	/* 衝突 */
								break;
							}
						}
					}
				}
			}

			if(hitFlag == FALSE)
			{
				/* ビンの外 */
				if(x < 32)
				{
					p_stPCG[i]->dx = g_Rand_Table4[g_Rand_num];
					p_stPCG[i]->x = 32;
					hitFlag = TRUE;	/* 衝突 */
				}
				if(x > 334)
				{
					p_stPCG[i]->dx = (g_Rand_Table4[g_Rand_num]) * -1;
					p_stPCG[i]->x = 334;
					hitFlag = TRUE;	/* 衝突 */
				}

				if(y < 80)
				{
					p_stPCG[i]->dy = g_Rand_Table4[g_Rand_num];
					p_stPCG[i]->y = 80;
					hitFlag = TRUE;	/* 衝突 */
				}
				if(y > 376)
				{
					p_stPCG[i]->dy = (g_Rand_Table4[g_Rand_num]) * -1;
					p_stPCG[i]->y = 376;
					hitFlag = TRUE;	/* 衝突 */
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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_Fly(int16_t nBallCount, int8_t bFinal)
{
	int16_t	i;
	int16_t x, y;
	int16_t sign;
	static int8_t bFlag = TRUE;
	
	ST_PCG	*p_stPCG[75] = {NULL};

	for(i = 0; i < 75; i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + g_numbers[i] - 1);	/* ステアリングポジション */
	
		if(p_stPCG[i] != NULL)
		{
			if(i <= (g_index - 1))
			{
				p_stPCG[i]->update	= FALSE;
				p_stPCG[i]->x = -16; 
				p_stPCG[i]->y = -16;
				continue;
			}
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

	for(i = nBallCount; i < Mmin(nBallCount + 1, 75); i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + g_numbers[i] - 1);	/* ステアリングポジション */
	
		if(p_stPCG[i] != NULL)
		{
			x = p_stPCG[i]->x;
			y = p_stPCG[i]->y;
			
			if( y < 0 )
			{
				/* 画面外 */
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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Bingo_Ball_Init(void)
{
	int16_t	i;
	int16_t sign;
	
	ST_PCG	*p_stPCG[75] = {NULL};

	for(i = 0; i < 75; i++)
	{
		p_stPCG[i] = PCG_Get_Info(SP_BALL_1 + i);	/* ステアリングポジション */

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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t main(int16_t argc, int8_t** argv)
{
	int16_t	ret = 0;


	uint16_t	uFreeRunCount=0u;

	int16_t	loop;
	int16_t	pushCount = 0;

	uint8_t	bMode;
	
	ST_TASK		stTask = {0}; 
	
	/* 変数の初期化 */
	SetGameMode(1);
	
	/* 例外ハンドリング処理 */
	usr_abort = App_exit;	/* 例外発生で終了処理を実施する */
	init_trap14();			/* デバッグ用致命的エラーハンドリング */
#if 0	/* アドレスエラー発生 */
	{
		char buf[10];
		int *A = (int *)&buf[1];
		int B = *A;
		return B;
	}
#endif

	App_Init();	/* 初期化 */
	
	loop = 1;
	
	/* 乱数 */
	srandom(TIMEGET());	/* 乱数の初期化 */
	
	do	/* メインループ処理 */
	{
		uint32_t time_st;

		/* 終了処理 */
		if(loop == 0)
		{
			break;
		}
		
		/* 時刻設定 */
		GetNowTime(&time_st);	/* メイン処理の開始時刻を取得 */
		SetStartTime(time_st);	/* メイン処理の開始時刻を記憶 */
		
		/* タスク処理 */
		TaskManage();				/* タスクを管理する */
		GetTaskInfo(&stTask);		/* タスクの情報を得る */

		/* モード引き渡し */
		GetGameMode(&bMode);
		
		/* 終了 */
		if((g_Input & KEY_b_ESC ) != 0u)	/* ＥＳＣキー */
		{
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 5)
			{
				ADPCM_Play(5);	/* SET */

				Music_Play(1);	/* Stop */

				SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
			}
		}
		else
		{
			pushCount = 0;
		}

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* 初期化シーン */
			{
				printf("BINGOマシンを起動します。\n");
				T_Clear();	/* テキストクリア */

				G_PaletteSetZero();	/* 0番パレット変更 */

				Bingo_Ball_Init();

				Set_CRT_Contrast(0);	/* 真っ暗にする */

				SetTaskInfo(SCENE_TITLE_S);	/* シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_TITLE_S:
			{
				int16_t x, y;

				Music_Play(3);	/* BINGO */
				
				G_PAGE_SET(2);	/* 奥 */
				Draw_Fill(0, 0, 511, 511, G_PURPLE);
				G_PAGE_SET(1);	/* 手前 */

				G_Load_Mem(1, 16, 64, 1);	/* バニーガール */

				Bingo_table_all();	/* ビンゴテーブル描画 */

				x = 0;
				y = 0;
				PutGraphic_To_Symbol24_Xn("BINGOでPRO-68k", x + 1, y + 1, G_BLACK, 2);
				PutGraphic_To_Symbol24_Xn("BINGOでPRO-68k", x, y, G_WHITE, 2);

				x = 0;
				y = 464;
				PutGraphic_To_Symbol24("Push Space Key or A button", x + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("Push Space Key or A button", x, y, G_WHITE);

				PCG_ON();	/* スプライトON */

				Set_CRT_Contrast(-1);	/* もとに戻す */

				g_Vwait = 1;

				SetTaskInfo(SCENE_TITLE);	/* シーン(処理)へ設定 */
				break;
			}
			case SCENE_TITLE:
			{
				if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
				{
					ADPCM_Play(5);	/* SET */
					SetTaskInfo(SCENE_TITLE_E);	/* シーン(終了処理)へ設定 */
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
					G_PAGE_SET(1);	/* 手前 */
					Draw_Fill(x, y, 350, 511, G_BG);

					SetTaskInfo(SCENE_START_S);	/* シーン(開始処理)へ設定 */
				}
				break;
			}
			case SCENE_START_S:	/* シーン(開始処理) */
			{
				int16_t i, j, k;
				int16_t x, y;
				int16_t temp;
				
				Music_Play(4);	/* Play */
				
				// 乱数の初期化
				srand(time(NULL));

				// Fisher-Yatesアルゴリズムを使用して数字をシャッフル
				for (i = 74; i > 0; --i) {
					j = rand() % (i + 1);
					// 数字を交換
					temp = g_numbers[i];
					g_numbers[i] = g_numbers[j];
					g_numbers[j] = temp;
					g_Sign_Table[i] = ((rand() % 2) == 0)?-1:1;	/* 符号乱数 */
					g_Rand_Table4[i]  = rand() % 4;		/* 乱数テーブル */
					g_Rand_Table8[i]  = rand() % 8;		/* 乱数テーブル */
					g_Rand_Table16[i] = rand() % 16;	/* 乱数テーブル */
					g_Rand_Table32[i] = rand() % 32;	/* 乱数テーブル */
				}

				k = 70;
#if 0
				for (i = 0; i < k; ++i)
				{
					Bingo_TableOpen();	/* ビンゴをひとつあける */
					Bingo_Open();	/* 大画面 */
					Bingo_History(g_index, 10);	/* 現在の値から過去１０個分 */
					g_index++;
				}
#endif				
				T_Circle(17, 70, 308, 322, 0xFFFE, T_WHITE);	/* ビンの中 */
				HIT_Circle_Area(17, 70, 308, 322);	/* ビンの中の当たり判定 */

				x = 0;
				y = 464;
				PutGraphic_To_Symbol24("History of the last 10 times", x + 1, y + 1, G_BLACK);
				PutGraphic_To_Symbol24("History of the last 10 times", x, y, G_WHITE);

				SetTaskInfo(SCENE_START);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_START:	/* シーン(実施処理) */
			{
				static int16_t bFlag = FALSE;

				if( (g_Input_P[0] != 0) || 
					(g_Input != 0u)		)
				{
					if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* A */
						((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
					{
						if(bFlag == FALSE)
						{
							bFlag = TRUE;

							ADPCM_Play(5);	/* SET */

							SetTaskInfo(SCENE_START_E);	/* シーン(終了処理)へ設定 */
						}
					}
				}
				else
				{
					bFlag = FALSE;
				}

				break;
			}
			case SCENE_START_E:	/* シーン(終了処理) */
			{
				if(ADPCM_SNS() == 0)
				{
					if(g_index >= 75)
					{
						SetTaskInfo(SCENE_GAME_OVER_S);	/* シーン(開始処理)へ設定 */
					}
					else
					{
						Music_Play(5);	/* Select */

						g_OK_num = rand() % 3;
						
						Bingo_Close();	/* 表示を消す */

						SetTaskInfo(SCENE_GAME_S);	/* シーン(開始処理)へ設定 */
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
						g_fly_count = 0;
						nNum_old = -1;

						PCG_OFF();			/* SP OFF */
						/* 動画 */
						MOV_Play(rand() % 6);		/* カットイン */

						SetTaskInfo(SCENE_GAME);	/* シーン(実施処理)へ設定 */
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
							nNum = rand() % nMaxRange;	/* 最大数からランダムで抽選 */
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
							nNum_old = nNum;	/* 1～残り */
						}
						nNum = g_index + nNum;
	
						ADPCM_Play(1 + g_fly_count);	/* ショット */

						x = 47 + (g_fly_count * 100);
						y = 398;
						w = 60;
						h = 60;
						Bingo_3_Open( x, y, g_numbers[nNum]);	/* オープン */
						T_Circle( x, y, w, h, 0xFFFF, T_BLACK1 );	/* 円A */
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

						ADPCM_Play(4);	/* ヒット */
						
						Bingo_TableOpen();	/* ビンゴをひとつあける */
						Bingo_Open();	/* 大画面 */
						Bingo_History(g_index, 10);	/* 現在の値から過去１０個分 */
						g_index++;

						SetTaskInfo(SCENE_GAME_E);	/* シーン(終了処理)へ設定 */
					}
					else
					{
						if(g_OK_tabel[g_OK_num][g_fly_count] == 0)
						{
							/* 何もしない */
						}
						else
						{
							ADPCM_Play(0);	/* BOM2 */
							Bingo_2_Close(47 + (g_OK_tabel[g_OK_num][g_fly_count] * (g_fly_count * 100)), 398);	/* クローズ */
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

					SetTaskInfo(SCENE_START);	/* シーン(実施処理)へ設定 */
				}
				break;
			}
			case SCENE_GAME_OVER_S:
			{
				Music_Play(6);	/* Game Over */

				SetTaskInfo(SCENE_GAME_OVER);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_GAME_OVER:
			{
				if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
				{
					ADPCM_Play(5);	/* SET */
					SetTaskInfo(SCENE_GAME_OVER_E);	/* シーン(終了処理)へ設定 */
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
					G_PAGE_SET(1);	/* 手前 */
					Draw_Fill(x, y, 350, 511, G_BG);

					SetTaskInfo(SCENE_INIT);	/* シーン(初期化処理)へ設定 */
				}
				break;
			}
			default:	/* 異常シーン */
			{
				loop = 0;	/* ループ終了 */
				break;
			}
		}

		PCG_Main();	/* スプライト管理 */

		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */

		/* 同期待ち */
		wait_vdisp(g_Vwait);

	}
	while( loop );

	g_bExit = FALSE;
	App_exit();	/* 終了処理 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void App_Init(void)
{
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 開始");
#endif

	puts("App_Init FileList");
	Init_FileList_Load();	/* リストファイルの読み込み */

	puts("App_Init Music");
	/* 音楽 */
	Init_Music();	/* 初期化(スーパーバイザーモードより前)	*/

	Music_Play(0);	/* Init */
	Music_Play(1);	/* Stop */
	Music_Play(2);	/* Loading */

	/* スーパーバイザーモード開始 */
	g_nSuperchk = _dos_super(0);
	if( g_nSuperchk < 0 ) {
#ifdef DEBUG	/* デバッグコーナー */
		puts("App_Init すでに_dos_super");
#endif
	} else {
#ifdef DEBUG	/* デバッグコーナー */
		puts("App_Init _dos_super開始");
#endif
	}

	/* 画面 */
	g_nCrtmod = CRTC_INIT(8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(0x100+8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(10);	/* mode=10 256x256 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(12);	/* mode=12 512x512 col:16/65536 31kHz */
//	g_nCrtmod = CRTC_INIT(16);	/* mode=16 764x512 col:16 31kHz */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init CRTC(画面)");
//	KeyHitESC();	/* デバッグ用 */
#endif

	/* グラフィック */
	G_INIT();			/* 画面の初期設定 */
	G_CLR();			/* クリッピングエリア全開＆消す */
	G_HOME(0);			/* ホーム位置設定 */
//	G_VIDEO_INIT();		/* ビデオコントローラーの初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init グラフィック");
#endif

	/* スプライト／ＢＧ */
	PCG_INIT();	/* スプライト／ＢＧの初期化 */
	PCG_INIT_CHAR();	/* スプライト＆ＢＧ定義セット */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init スプライト／ＢＧ");
#endif

#if CNF_MACS
	/* 動画 */
	MOV_INIT();		/* 初期化処理 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init Movie(MACS)");
#endif
#endif

	/* MFP */
	MFP_INIT();	/* V-Sync割り込み等の初期化処理 */
#ifdef DEBUG	/* デバッグコーナー */
	printf("App_Init MFP(%d)\n", Get_CPU_Time());
#endif
	g_GameSpeed = Mmin(Get_CPU_Time() / 300, 7);	/* 300 = 10MHz基準 */

	/* Task */
	TaskManage_Init();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init Task");
#endif

	/* テキスト */
	T_INIT();	/* テキストＶＲＡＭ初期化 */
	T_PALET();	/* テキストパレット初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init T_INIT");
#endif
	
	/* マウス初期化 */
//	Mouse_Init();	/* マウス初期化 */
#ifdef DEBUG	/* デバッグコーナー */
//	puts("App_Init マウス");
#endif
	
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 終了");
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void App_exit(void)
{
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit 開始");
#endif
	
	if(g_bExit == TRUE)
	{
		puts("エラーをキャッチ！ ESC to skip");
		KeyHitESC();	/* デバッグ用 */
	}
	
	/* グラフィック */
	G_CLR();			/* クリッピングエリア全開＆消す */

	/* スプライト＆ＢＧ */
	PCG_OFF();		/* スプライト＆ＢＧ非表示 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit スプライト");
#endif

	/* 画面 */
	CRTMOD(0x100 + g_nCrtmod);	/* モードをもとに戻す */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit 画面");
#endif

	/* テキスト */
	T_EXIT();				/* テキスト終了処理 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit テキスト");
#endif
	
	/* MFP */
	MFP_EXIT();				/* MFP関連の解除 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit MFP");
#endif

	/* 音楽 */
	Exit_Music();			/* 音楽停止 */
	puts("App_exit Music");

	Mouse_Exit();	/* マウス非表示 */

	MyMfree(0);				/* 全てのメモリを解放 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit メモリ");
#endif

	_dos_kflushio(0xFF);	/* キーバッファをクリア */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit キーバッファクリア");
#endif

	/*スーパーバイザーモード終了*/
	_dos_super(g_nSuperchk);
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit スーパーバイザーモード終了");
#endif

#ifdef DEBUG	/* デバッグコーナー */
	{
//		int16_t i;
//		for(i=0; i<360; i++)
		{
//			printf("[%d](%3d, %3d),", i, g_Circle_Table[i][0], g_Circle_Table[i][1]);
//			if((i%4) == 0)printf("\n");
		}
	}
#endif

#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit 終了");
#endif
}


/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	App_FlipProc(void)
{
	int16_t	ret = 0;
	
	if(g_bFlip == FALSE)	/* 描画中なのでフリップしない */
	{
		return ret;
	}
	
	SetFlip(FALSE);			/* フリップ禁止 */
	
	/* 画面切り替え */
	G_HOME(g_mode);
	
	/* モードチェンジ */
	if(g_mode == 1u)		/* 上側判定 */
	{
		SetGameMode(2);
	}
	else if(g_mode == 2u)	/* 下側判定 */
	{
		SetGameMode(1);
	}
	else					/* その他 */
	{
		SetGameMode(0);
	}
	
	ret = 1;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetFlip(uint8_t bFlag)
{
	int16_t	ret = 0;

	Set_DI();	/* 割り込み禁止 */
	g_bFlip_old = g_bFlip;	/* フリップ前回値更新 */
	g_bFlip = bFlag;
	Set_EI();	/* 割り込み許可 */
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void App_TimerProc( void )
{
	ST_TASK stTask;
	
	TaskManage();
	GetTaskInfo(&stTask);	/* タスク取得 */
	/* ↓↓↓ この間に処理を書く ↓↓↓ */

	if(stTask.b16ms == TRUE)	/* 8ms周期 */
	{
		switch(stTask.bScene)
		{
			case SCENE_INIT:
			case SCENE_TITLE_S:
			case SCENE_TITLE:
			case SCENE_TITLE_E:
			case SCENE_START_S:
			{
				Bingo_Fly(75, TRUE);	/* スプライトを表示 */
				break;
			}
			case SCENE_START:
			{
				Bingo_Shuffle(g_Ball_Max[g_GameSpeed]);	/* スプライトを表示 */
				break;
			}
			case SCENE_GAME_S:
			{
				Bingo_Fly(g_index + g_fly_count, (g_fly_count == 3));	/* スプライトを表示 */
				break;
			}
		}
	}
	/* ↑↑↑ この間に処理を書く ↑↑↑ */

	/* タスク処理 */
	UpdateTaskInfo();		/* タスクの情報を更新 */

}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t App_RasterProc( uint16_t *pRaster_cnt )
{
	int16_t	ret = 0;

//	RasterProc(pRaster_cnt);	/*　ラスター割り込み処理 */

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void App_VsyncProc( void )
{
	/* 入力処理 */
	Input_Main();
	
	/* ラスター割り込み処理セット */
//	RasterVcyncProc();
	
	/* 画面切り替え */
	App_FlipProc();

}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetGameMode(uint8_t *bMode)
{
	int16_t	ret = 0;
	
	*bMode = g_mode;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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
