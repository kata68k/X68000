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

//#define 	W_BUFFER	/* ダブルバッファリングモード */
#define	FPS_MONI	/* FPS計測 */
//#define	CPU_MONI	/* CPU計測 */
#define	MEM_MONI	/* MEM計測 */

enum
{
	ball_lost,	/* 消えたボール */
	ball_now,	/* 発射ボール */
	ball_next,	/* 次の発射ボール */
	ball_exist,	/* 存在するボール */
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
	uint8_t		bSP_num;		/* プレーン番号の先頭 */
	uint8_t		bSize;			/* サイズ */
	uint8_t		bStatus;		/* ステータス */
	uint8_t		bValidty;		/* 有効 */
}	ST_BALL;

/* グローバル変数 */
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

/* グローバル構造体 */
ST_BALL		g_stBall[BALL_MAX];

/* グローバル変数 */
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

#ifdef DEBUG	/* デバッグコーナー */
uint16_t	g_uDebugNum = 0;
//uint16_t	g_uDebugNum = (Bit_7|Bit_4|Bit_0);
#endif

/* 関数のプロトタイプ宣言 */
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

void (*usr_abort)(void);	/* ユーザのアボート処理関数 */

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Boder_line(void)
{
#if 0
	ST_TASK stTask;
#endif
	static uint32_t s_TimeStamp = PASSTIME_INIT;

	ST_PCG	*p_stPCG = NULL;

#ifdef DEBUG	/* デバッグコーナー */
//	printf("Boder_line st(%d)\n", g_index);
#endif

	p_stPCG = PCG_Get_Info(SP_ARROW_0);		/* 矢印 */
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

	p_stPCG  = PCG_Get_Info(SP_GAL_0);		/* ギャル */
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


		if(	GetPassTime(GAL_ANIME_INT_TIME, &s_TimeStamp) != 0u)	/* 112ms経過 */
		{
			/* アニメーション */
			p_stPCG->Anime++;
			if(p_stPCG->Anime >= p_stPCG->Pat_AnimeMax)
			{
				p_stPCG->Anime = 0;
			}
		}
	}

#if 0
	GetTaskInfo(&stTask);	/* タスク取得 */

	switch(stTask.bScene)
	{
		case SCENE_GAME1:
		case SCENE_GAME2:
		{
			if(g_bFPS_PH < 4)	/* 4フレーム */
			{

			}
			else if(g_bFPS_PH < 8)	/* 8フレーム */
			{
				PCG_COL_SHIFT(7, 1);	/* パレットシフト */
			}
			else if(g_bFPS_PH < 16)	/* 16フレーム */
			{
				PCG_COL_SHIFT(6, 1);	/* パレットシフト */
			}
			else
			{
				/* 何もしない */
			}
			break;
		}
		default:
		{
			/* 何もしない */
			break;
		}
	}
#endif
#ifdef DEBUG	/* デバッグコーナー */
//	printf("Boder_line ed(%d)\n", g_index);
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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
			Draw_Line( X_POS_MIN + (10 * m), Y_POS_BD - 1, X_POS_MIN + (10 * m) + 1, Y_POS_BD - 1, G_WHITE, 0xFFFF);	/* 消去 */
		}
	}

	GetNowTime(&time_now);
	m = (int16_t)(time_now - g_unTime_Pass);	
	n = nCount;
	Draw_Line( X_POS_MIN, Y_POS_BD + n, X_POS_MAX, Y_POS_BD + n, G_BG, 0xFFFF);	/* 消去 */
	n = nCount++;
	Draw_Line( X_POS_MIN, Y_POS_BD + n, X_POS_MIN + m, Y_POS_BD + n, n, 0xFFFF);	/* 描画 */
	
	g_unTime_Pass = time_now;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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

	/* 当たり判定 */
	if( ((g_stBall[i].bSize)   == (g_stBall[j].bSize))	&&
		((g_stBall[i].bStatus) == (g_stBall[j].bStatus)) )
	{
		int16_t BallSize;

		if(g_ubGameClear == TRUE)
		{
			return ret;
		}

		/* 相手 */
		list_num = g_stBall[j].bSize + 1;
		Ball_mark_area(p_stPCG_other, -1, list_num); // 古い位置をクリア

		/* 自分 */
		list_num = g_stBall[i].bSize + 1;
		Ball_mark_area(p_stPCG, -1, list_num); // 古い位置をクリア

		/* 自分 */
		g_stBall[i].bStatus++;
#ifdef DEBUG	/* デバッグコーナー */
		if( (g_uDebugNum & Bit_6) != 0u )	/* ボールのステータス強制MAX */
		{
			g_stBall[i].bStatus = 3;
		}
#endif
		/* ボール設定 */
		BallSize = g_stBall[i].bSize;		

		if( g_stBall[i].bStatus > 2 )	/* 進化の上限を超えた→サイズチェンジ */
		{
			if( BallSize >= 3 )	/* 最大サイズの最終系で合体 */
			{
				g_ubGameClear = TRUE;	/* ゲームクリア */
				S_Set_Combo(0);			/* コンボカウンタ リセット */
				S_GetPos(44, 136);		/* スコア表示座標更新 */
				ret = S_Add_Score(1000000);	/* スコア更新 */

				BallSize = 0;	/* サイズ0 */ 
				m = 0;
				list_num = m + 1;	/* 0はBGなので */
			}
			else
			{
				S_GetPos((x >> PCG_LSB), (y >> PCG_LSB));	/* スコア表示座標更新 */

				ret = S_Add_Score(1000);	/* スコア更新 */

				BallSize++;	/* サイズアップ */ 
				m = Mmin(BallSize, 3);	/* 最大値＋１ */
				list_num = m + 1;	/* 0はBGなので */
			}


			for(k = g_Ball_Table[m]; k < g_Ball_Table[m + 1]; k++)	/* 新しいテーブルから空きを調査する */
			{
				n = k - g_Ball_Table[0];	/*  */
				
				if(	g_stBall[n].bValidty == ball_lost )	/* 無効データ */
				{
					g_stBall[n].bSize = m;		/* 次のサイズ領域へ */
					g_stBall[n].bStatus = 0;	/* 最初のカラー */
					g_stBall[n].bValidty = ball_exist;

					/* スプライト設定 */
					p_stPCG_new = PCG_Get_Info(SP_BALL_S_1 + g_stBall[n].bSP_num);	/* Newボール */
					if(p_stPCG_new != NULL)
					{
						pal = Mmul256(g_stST_PCG_LIST[list_num].Pal);
						for(p=0; p < p_stPCG_new->Pat_DataMax; p++)
						{
							*(p_stPCG_new->pPatCodeTbl + p) &= 0xF0FF;
							*(p_stPCG_new->pPatCodeTbl + p) |= (pal + Mmul256(g_Ball_Color[g_stBall[n].bStatus]));	/* カラーセット */
						}
						p_stPCG_new->update = TRUE;
						p_stPCG_new->status = FALSE;

						width	= Mmul16(p_stPCG_new->Pat_w);
						height	= Mmul16(p_stPCG_new->Pat_h);

						/* ビン */
						if( (y1 >> PCG_LSB) > (g_TB_GameLevel[g_nGameLevel] - height) )	/* ビン底 */
						{
							y1 = (g_TB_GameLevel[g_nGameLevel] - height) << PCG_LSB;
						}
						else
						{
						}

						if((x1 >> PCG_LSB) <= X_POS_MIN)			/* ビン左 */
						{
							x1 = X_POS_MIN << PCG_LSB;
						}
						else if((x1 >> PCG_LSB) > (X_POS_MAX - width))	/* ビン右 */
						{
							x1 = (X_POS_MAX - width) << PCG_LSB;
						}

						p_stPCG_new->dx = 0;
						p_stPCG_new->dy = 0;
						p_stPCG_new->x = x1;
						p_stPCG_new->y = y1;

					    Ball_mark_area(p_stPCG_new, 1, list_num); // 新しい位置をセット
						break;
					}
				}
			}
			/* 相手 */
			p_stPCG_other->x = 0;
			p_stPCG_other->y = 0;
			p_stPCG_other->update = FALSE;
			p_stPCG_other->status = FALSE;
			
			g_stBall[j].bStatus = (rand() % 3);
			g_stBall[j].bSize = 0;
			g_stBall[j].bValidty = ball_lost;

			/* 消す */
			p_stPCG->x = 0;
			p_stPCG->y = 0;
			p_stPCG->update = FALSE;
			p_stPCG->status = FALSE;

			g_stBall[i].bStatus = (rand() % 3);	
			g_stBall[i].bSize = 0;
			g_stBall[i].bValidty = ball_lost;
		}
		else	/* カラーチェンジ */
		{
			/* 相手 */
			list_num = g_stBall[j].bSize + 1;
			Ball_mark_area(p_stPCG_other, -1, list_num); // 古い位置をクリア

			g_stBall[j].bStatus = (rand() % 3);
			g_stBall[j].bSize = 0;
			g_stBall[j].bValidty = ball_lost;
			p_stPCG_other->x = 0;
			p_stPCG_other->y = 0;
			p_stPCG_other->update = FALSE;
			p_stPCG_other->status = FALSE;

			/* 自分は一旦、消える */
			list_num = g_stBall[i].bSize + 1;
			Ball_mark_area(p_stPCG, -1, list_num); // 古い位置をクリア

			if( BallSize >= 3 )	/* 最大サイズの最終系で合体 */
			{
				g_stBall[i].bStatus = (rand() % 3);
				g_stBall[i].bSize = 0;
				g_stBall[i].bValidty = ball_lost;

				p_stPCG->x = 0;
				p_stPCG->y = 0;
				p_stPCG->update = FALSE;
				p_stPCG->status = FALSE;

				g_ubGameClear = TRUE;	/* ゲームクリア */
				S_Set_Combo(0);			/* コンボカウンタ リセット */
				S_GetPos(44, 136);		/* スコア表示座標更新 */
				ret = S_Add_Score(1000000);	/* スコア更新 */
			}
			else
			{
				S_GetPos((x >> PCG_LSB), (y >> PCG_LSB));	/* スコア表示座標更新 */

				ret = S_Add_Score(100);	/* スコア更新 */

				pal = Mmul256(g_stST_PCG_LIST[list_num].Pal);
				for(k=0; k < p_stPCG->Pat_DataMax; k++)
				{
					*(p_stPCG->pPatCodeTbl + k) &= 0xF0FF;
					*(p_stPCG->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* カラーセット */
				}
				p_stPCG->update = TRUE;
				p_stPCG->status = FALSE;
				p_stPCG->x = x1;
				p_stPCG->y = y1;
				p_stPCG->dy = 0;
				p_stPCG->dx = 0;

			    Ball_mark_area(p_stPCG, 1, list_num); // 新しい位置をセット
			}
		}
	}
	else if( ((g_stBall[i].bSize) == (g_stBall[j].bSize)) )
	{
		/* 何もしない */
	}
	else
	{
		/* 何もしない */
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
		bFlag |= Bit_4;	/* X方向 */
    }
    if ((*new_x + width) >= X_POS_MAX )
	{
		*new_x = X_POS_MAX - width;
		bFlag |= Bit_5;	/* X方向 */
    }
    if (*new_y <= 0)
	{
		*new_y = 0;
		bFlag |= Bit_6;	/* Y方向 */
    }
    if ((*new_y + height) >= g_TB_GameLevel[g_nGameLevel]) 
	{
		*new_y = g_TB_GameLevel[g_nGameLevel] - height;
		bFlag |= Bit_7;	/* Y方向 */
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
	y_Ofst_s = y_OK - Mdiv2(height);	/* 半分から上へ */
	y_Ofst_e = top;
#endif

#if 1
	x = x_Ofst_s;
    for (y = y_Ofst_s; y >= y_Ofst_e; y--)	/* 左線 */
	{
		if (g_bAREA[y][x] != 0) /* ボールが存在する */
		{
			bFlag |= Bit_0;	/* X方向 */
			break;
		}
	}

	x = x_Ofst_e;
    for (y = y_Ofst_s; y >= y_Ofst_e; y--)	/* 右線 */
	{
		if (g_bAREA[y][x] != 0) /* ボールが存在する */
		{
			bFlag |= Bit_1;	/* X方向 */
			break;
		}
	}

	y = y_Ofst_e;
	for ( x = x_Ofst_s; x < x_Ofst_e; x++ )	/* 上線 */
	{
		if (g_bAREA[y][x] != 0) /* ボールが存在する */
		{
			bFlag |= Bit_2;	/* Y方向 */
			break;
		}
	}

	y = y_Ofst_s;
	for ( x = x_Ofst_s; x < x_Ofst_e; x++ )	/* 下線 */
	{
		if (g_bAREA[y][x] != 0) /* ボールが存在する */
		{
			bFlag |= Bit_3;	/* Y方向 */
			break;
		}
	}

	if( bFlag != 0u )
	{
		*pbStat = Mbset(*pbStat, 0xFF, bFlag);	/* フラグセット */
		ret = TRUE;
	}

#ifdef DEBUG	/* デバッグコーナー */
	
	if( (g_uDebugNum & Bit_7) != 0u )	/* 接触部分 */
	{
		if((*pbStat & Bit_0) != 0u)	/* 左方向にあり */
		{
			Draw_Line(x_Ofst_s+1, y_Ofst_e, x_Ofst_s+1, y_Ofst_s, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_1) != 0u)	/* 右方向にあり */
		{
			Draw_Line(x_Ofst_e -1 , y_Ofst_e, x_Ofst_e -1, y_Ofst_s, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_2) != 0u)	/* 上方向にあり */
		{
			Draw_Line(x_Ofst_s, y_Ofst_e+1, x_Ofst_e, y_Ofst_e+1, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_3) != 0u)	/* 下方向にあり */
		{
			Draw_Line(x_Ofst_s, y_Ofst_s -1, x_Ofst_e, y_Ofst_s - 1, G_L_PINK, 0xFFFF);
		}
		if((*pbStat & Bit_4) != 0u)	/* 左端方向にあり */
		{
			Draw_Line(x_Ofst_s, y_Ofst_e, x_Ofst_s, y_Ofst_s, G_L_BLUE, 0xFFFF);
		}
		if((*pbStat & Bit_5) != 0u)	/* 右方向にあり */
		{
			Draw_Line(x_Ofst_e, y_Ofst_e, x_Ofst_e, y_Ofst_s, G_L_BLUE, 0xFFFF);
		}
		if((*pbStat & Bit_6) != 0u)	/* 上端方向にあり */
		{
			Draw_Line(x_Ofst_s, y_Ofst_e, x_Ofst_e, y_Ofst_e, G_L_BLUE, 0xFFFF);
		}
		if((*pbStat & Bit_7) != 0u)	/* 下端方向にあり */
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
            if (g_bAREA[y][x] == 1) /* ボールが存在する */
			{
				if(y == y_OK)
				{
					if((x >= x_Ofst_s) && (x < x_Ofst_e))	/* 下面の中央 */
					{
						*new_y = 0;	/* 下面に何かあった */
						*pbStat = Mbset(*pbStat, 0xFF, Bit_2);	/* Y方向 */
						return TRUE;
					}
				}
				else
				{
					*new_y = y;	/* 下面に何かあった */
					*pbStat = Mbset(*pbStat, 0xFF, Bit_3);	/* Y方向 */
	                return TRUE;
				}
            }
        }
    }
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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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
		            Draw_Pset( x, y, G_BG);	/* 何もなし */
					break;
				}
				case 1:
				{
		            Draw_Pset( x, y, G_INDIGO);	/* 何かある */
					break;
				}
				case 2:
				{
		            Draw_Pset( x, y, G_RED);	/* 何かある */
					break;
				}
				case 3:
				{
		            Draw_Pset( x, y, G_GREEN);	/* 何かある */
					break;
				}
				default:
				{
		            Draw_Pset( x, y, G_YELLOW);	/* 何かある */
					break;
				}
			}
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
static int8_t Ball_Point_Update(ST_PCG* p_stPCG, uint8_t list_num)
{
#ifdef DEBUG	/* デバッグコーナー */
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

	/* 現在の位置 */
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

	/* 新しい位置 */
    p_stPCG->dy += Y_ACC;
	p_stPCG->dy = Mmin(p_stPCG->dy, 0x1000);
    new_x = (x + p_stPCG->dx) >> PCG_LSB;
    new_y = (y + p_stPCG->dy) >> PCG_LSB;

	/* 当たり判定その１ */
    if (Ball_check_collision(&new_x, &new_y, width, height, list_num, &bStat))	/* 枠外もしくはヒット */
	{
		/* -------------------- < 4 > -------------------- */
		if((bStat & (Bit_0 | Bit_1 | Bit_2 | Bit_3)) == (Bit_0 | Bit_1 | Bit_2 | Bit_3))	/* 全てに障害物があった */
		{
			p_stPCG->dx = 0; // 止める
			p_stPCG->dy = 0; // 止める
		}
		/* -------------------- < 3 > -------------------- */
		else if((bStat & (Bit_0 | Bit_1 | Bit_3 )) == (Bit_0 | Bit_1 | Bit_3 ))	/* 上が開いていた */
		{
			p_stPCG->dx = 0; // 止める
			p_stPCG->dy = -Y_ACC2;	/* 上方向へ */
		}
		else if((bStat & (Bit_0 | Bit_1 | Bit_2 )) == (Bit_0 | Bit_1 | Bit_2 ))	/* 下が開いていた */
		{
			p_stPCG->dx = 0; // 止める
			/* 下へ */
		}
		else if((bStat & (Bit_1 | Bit_2 | Bit_3)) == (Bit_1 | Bit_2 | Bit_3))	/* 左が開いていた */
		{
			if(p_stPCG->dx >= 0)p_stPCG->dx = -X_ACC;	/* 左へ */
			p_stPCG->dy = 0; // 止める
		}
		else if((bStat & (Bit_0 | Bit_2 | Bit_3)) == (Bit_0 | Bit_2 | Bit_3))	/* 右が開いていた */
		{
			if(p_stPCG->dx <= 0)p_stPCG->dx = X_ACC;	/* 右へ */
			p_stPCG->dy = 0; // 止める
		}
		/* -------------------- < 2 > -------------------- */
		else if((bStat & (Bit_2 | Bit_3)) == (Bit_2 | Bit_3))	/* 上下に障害物があった */
		{
			/* 左右 */
			p_stPCG->dy = 0; // 止める
		}
		else if((bStat & (Bit_0 | Bit_1)) == (Bit_0 | Bit_1))	/* 左右に障害物があった */
		{
			p_stPCG->dx = 0; // 止める
			/* 上下 */
		}
		else if((bStat & (Bit_0 | Bit_3)) == (Bit_0 | Bit_3))	/* 左下に障害物があった */
		{
			if(p_stPCG->dx <= 0)p_stPCG->dx = X_ACC;	/* 右へ */
			p_stPCG->dy = 0; // 止める
		}
		else if((bStat & (Bit_1 | Bit_3)) == (Bit_1 | Bit_3))	/* 右下に障害物があった */
		{
			if(p_stPCG->dx >= 0)p_stPCG->dx = -X_ACC;	/* 左へ */
			p_stPCG->dy = 0; // 止める
		}
		else if((bStat & (Bit_0 | Bit_2)) == (Bit_0 | Bit_2))	/* 左上に障害物があった */
		{
			if(p_stPCG->dx <= 0)p_stPCG->dx = X_ACC;	/* 右へ */
			/* 下へ */
		}
		else if((bStat & (Bit_1 | Bit_2)) == (Bit_1 | Bit_2))	/* 右上に障害物があった */
		{
			if(p_stPCG->dx >= 0)p_stPCG->dx = -X_ACC;	/* 左へ */
			/* 下へ */
		}
		/* -------------------- < 1 > -------------------- */
		else if((bStat & Bit_3) == Bit_3)	/* 下に障害物があった */
		{
			/* 左右 */
			p_stPCG->dy = 0; // 止める
		}
		else
		{

		}

		/* -------------------- < 端の処理 > -------------------- */
		if((bStat & Bit_4) == Bit_4)	/* 左端のみ */
		{
			if(p_stPCG->dx < 0)p_stPCG->dx = X_ACC;	// 反転
			else p_stPCG->dx = 0; // 止める
			/* 下へ */
		}
		else if((bStat & Bit_5) == Bit_5)	/* 右端のみ */
		{
			if(p_stPCG->dx > 0)p_stPCG->dx = -X_ACC;	// 反転
			else p_stPCG->dx = 0; // 止める
			/* 下へ */
		}
		else
		{

		}

		if((bStat & Bit_6) == Bit_6)	/* 天井に障害物があった */
		{
			/* 左右 */
			/* 下へ */
		}
		else if((bStat & Bit_7) == Bit_7)	/* 底辺に障害物があった */
		{
			/* 左右 */
			if(p_stPCG->dy > 0)p_stPCG->dy = 0; // 止める
		}
		else	/* どちらかに障害物があった */
		{
			/* 左右 */
			/* 下へ */
		}

		/* 再計算 */
		new_x = (x + p_stPCG->dx) >> PCG_LSB;
		new_y = (y + p_stPCG->dy) >> PCG_LSB;

		p_stPCG->status = FALSE;	/* 移動停止 */

		ret = FALSE;
    }
	else
	{
		if(new_y < old_y)	/* 当たり判定のない上昇を止める */
		{
			p_stPCG->dy = Y_ACC;
			new_y = old_y;
		}
	}

	if(new_y > old_y)	/* 上限値を超える場合は、持ち上げる */
	{
		if((new_y - old_y) > 200)
		{
			p_stPCG->dy = Y_ACC;
			new_y = 0;
		}
	}

	if(new_y > end_y)	/* 下限値を超える場合は、持ち上げる */
	{
		new_y = end_y;
	}

	p_stPCG->x = new_x << PCG_LSB;	/* 横 更新 */
	p_stPCG->y = new_y << PCG_LSB;	/* 縦 更新 */

	if((p_stPCG->dx == 0) && (p_stPCG->dy == 0))
	{
		p_stPCG->status = FALSE;	/* 移動停止 */
		ret = FALSE;
	}
	
	/* 次回用 */
	if(p_stPCG->dx == 0)
	{
		/* 何もしない */
	}
	else if(old_x < new_x)
	{
		p_stPCG->dx -= X_DEC;		/* 減速 */
	}
	else
	{
		p_stPCG->dx += X_DEC;		/* 減速 */
	}

#ifdef DEBUG	/* デバッグコーナー */
	if( (g_uDebugNum & Bit_4) != 0u )	/* 加速度表示 */
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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int8_t Ball_Check(int16_t nBallCount)
{
	int8_t	ret = FALSE;
#ifdef DEBUG	/* デバッグコーナー */
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

#ifdef DEBUG	/* デバッグコーナー */
//	static int16_t count = 0;

//	printf("Ball_Check st(%d)\n", g_index);
#endif

#if 0
	GetNowTime(&time_st);	/* タイムアウトカウンタリセット */
#endif
#ifdef DEBUG	/* デバッグコーナー */
//	printf("Ball_Check st(%d)\n", count);
	if( g_uDebugNum != 0u )		/* デバッグ中 */
	{
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%3d", g_uDebugNum);
		BG_TextPut(sBuf, 178, 239);	/* デバッグ表示 */
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%08b  ", g_uDebugNum);
		BG_TextPut(sBuf, 178, 247);	/* デバッグ表示 */

		Draw_Fill(X_POS_MIN, Y_MIN_DRAW, X_POS_MAX, Y_MAX_DRAW-1, G_BG);		/* ビンの中は塗りつぶし */
	}

	if( (g_uDebugNum & Bit_5) != 0u )	/* 当たり判定 */
	{
		print_area();	/* 塗りつぶし */
	}
#endif
	if(g_GameSpeed > 3)
	{
		if((s_bErase == TRUE) || ((s_bErase == FALSE) && (s_bErase_old == TRUE)))	/* 消去中は座標やステータスを更新しない */
		{
			T_Clear();		/* テキストクリア */
		}
	}
	s_bErase_old = s_bErase;

	s_Resume_old = s_Resume;
	GetNowTime(&s_AbortTime);

	/* 重なり抽出 */
	for(i = s_Resume; i < BALL_MAX - 1; i++)
	{
		uint8_t bEraseTmp = FALSE;

#ifdef CPU_MONI	/* デバッグコーナー */
		Level_meter(FALSE);	/* 処理負荷計測 */
#endif
		bValid = g_stBall[i].bValidty;
		if( bValid != ball_exist )		/* 存在するボール以外 */
		{
			continue;
		}

		p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ボール */
		if(p_stPCG == NULL)
		{
			continue;
		}
		p_stPCG->update = TRUE;
		
		if(s_bErase == TRUE)	/* 消去中は座標やステータスを更新しない */
		{
			if(	GetPassTime(COMBO_INTERVAL_SE_TIME, &s_WaitTime) != 0u  )	/* 500ms経過 */
			{
				s_bErase = FALSE;
				g_bShot_OK = TRUE;	/* 次の発射許可 */
			}
			else
			{
				if(g_GameSpeed > 3)
				{
					/* アニメーション */
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
			if(	GetPassTime(FRAME_RATIO_TIME, &s_AbortTime) != 0u )	/* フレームレート重視 */
			{
				bNoMoreMove = FALSE;	/* 移動完了していない */
#ifdef DEBUG	/* デバッグコーナー */
//				memset(sBuf, 0, sizeof(sBuf));
//				sprintf(sBuf, "%2d(%2d)%ld", s_Resume, s_Resume_old, s_AbortTime - s_AbortTime_old);
//				Draw_Message_To_Graphic(sBuf, 0, 20, F_MOJI, F_MOJI_BAK);	/* デバッグ */
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
			if( bValid1 != ball_exist )		/* 存在するボール以外 */
			{
				continue;
			}

			p_stPCG_other = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num);	/* 他のボール */
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
#ifdef DEBUG	/* デバッグコーナー */
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
				if((i == nBallCount) || (j == nBallCount))		/* 有効化したボール */
				{
					g_bShot_OK = TRUE;	/* 次の発射許可 */
				}

				if( Ball_Combine(p_stPCG, i, p_stPCG_other, j) != 0 )	/* 合体成立 */
				{
					ADPCM_Play(Mmin(S_Get_Combo() + 7 - 1, p_list_max - 1));	/* 連鎖音 */

					ef_x = x;
					ef_y = y;
					ef_x1 = x1;
					ef_y1 = y1;
					e_w = width;
					e_h = height;

					if(g_GameSpeed > 3)
					{
						/* アニメーション */
						T_Circle(ef_x, ef_y, e_w, e_h, 0xA5A5, rand()%15 + 1);
						T_Circle(ef_x1, ef_y1, e_w, e_h, 0xA5A5, rand()%15 + 1);
					}
					s_bErase = TRUE;
					bEraseTmp = TRUE;
					break;
				}
				else
				{
					/* ぶつかっただけ */
					if(cx == cx1)
					{
						/* 何もしない */
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
				/* 何もしない */
			}
		}

		if(bEraseTmp == TRUE)
		{
			break;	/* 連鎖感を出す */
		}

#ifdef DEBUG	/* デバッグコーナー */
//		memset(sBuf, 0, sizeof(sBuf));
//		sprintf(sBuf, "(%d,%d)(%d,%d)", x, y, p_stPCG->dx, p_stPCG->dy);
//		BG_TextPut(sBuf, x, y);	/* デバッグ表示 */
#endif

		bValid = g_stBall[i].bValidty;
		if( bValid != ball_exist )		/* 存在するボール以外 */
		{
			continue;
		}

		if (p_stPCG->status == FALSE)	/* 移動停止 */
		{
#ifdef DEBUG	/* デバッグコーナー */
			if( (g_uDebugNum & Bit_3) != 0u )	/* 移動完了済みSP */
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
		    Ball_mark_area(p_stPCG, -1, list_num); // 古い位置をクリア

			if(Ball_Point_Update(p_stPCG, list_num) == FALSE)	/* 移動済み、２回ヒット、底 */
			{
				if((nBallCount == i) || (nBallCount == -1))		/* 有効化したボール */
				{
					g_bShot_OK = TRUE;	/* 次の発射許可 */
				}
			}
			else
			{
			}
			
			Ball_mark_area(p_stPCG, 1, list_num); // 新しい位置をマーク

#ifdef DEBUG	/* デバッグコーナー */
			if( (g_uDebugNum & Bit_1) != 0u )	/* 移動中SP */
			{
				if( (g_uDebugNum & Bit_2) != 0u )	/* 当たり判定 */
				{
					x += g_stST_PCG_LIST[list_num].hit_x;
					y += g_stST_PCG_LIST[list_num].hit_y;
					width	= g_stST_PCG_LIST[list_num].hit_width;
					height	= g_stST_PCG_LIST[list_num].hit_height;
					Draw_Box(x, y, x + width, y + height, G_PURPLE, 0xFFFF);
				}
				else	/* Bit_1 SPのサイズ範囲 */
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

		if(p_stPCG->status == TRUE)	/* 移動中 */
		{
			bNoMoreMove = FALSE;	/* 移動完了していない */
		}

		s_Resume = i;	/* 完了できた処理 */
	}

	if(bNoMoreMove == TRUE)
	{
#ifdef DEBUG	/* デバッグコーナー */
		if( (g_uDebugNum & Bit_0) != 0u )	/* ステータス */
		{
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "NMMOVE(%3d.%3d)", nBallCount, g_bBoder_line);
			BG_TextPut(sBuf, 0, 48);	/* デバッグ表示 */
		}
#endif
		for (i = 0; i < BALL_MAX; i++)
		{
			p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ボール */
			if(p_stPCG == NULL)
			{
				continue;
			}
			
			bValid = g_stBall[i].bValidty;
			if( bValid == ball_lost )		/* 存在しないボール */
			{
				p_stPCG->update = FALSE;
				continue;
			}
			else if( bValid == ball_exist )		/* 存在するボール */
			{
				y = p_stPCG->y >> PCG_LSB;
				line = Mmin(y , line);	/* 更新 */

				if(line != g_bBoder_line)
				{
					g_bBoder_line = line;
				}
			}

			if(p_stPCG->status == FALSE)
			{
				p_stPCG->status = TRUE;		/* 移動再開 */
			}
		}

		s_Resume = 0;	/* 完了 */

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
			/* 保持 */
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
#ifdef DEBUG	/* デバッグコーナー */
		if( (g_uDebugNum & Bit_0) != 0u )	/* ステータス */
		{
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "  MOVE(%3d.%3d)", nBallCount, g_bBoder_line);
			BG_TextPut(sBuf, 0, 48);	/* デバッグ表示 */
		}
#endif
	}

#ifdef DEBUG	/* デバッグコーナー */
//	printf("Ball_Check st2(%d)\n", count);
#endif

#ifdef DEBUG	/* デバッグコーナー */
//	printf("Ball_Check st3(%d)\n", count);
#endif

#ifdef DEBUG	/* デバッグコーナー */
//	printf("Ball_Check ed(%d)\n", count);
//	count++;
#endif

#ifdef DEBUG	/* デバッグコーナー */
//	memset(sBuf, 0, sizeof(sBuf));
//	sprintf(sBuf, "(%2d)(%d)(%d)(%d)", nBallCount, g_bShot_OK, num_pairs, num_non_overlapping);
//	Draw_Message_To_Graphic(sBuf, 0, 24, F_MOJI, F_MOJI_BAK);	/* スコア表示 */
//	printf("Ball_Check ed(%d)\n", g_index);
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

#ifdef DEBUG	/* デバッグコーナー */
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
		p_stPCG_ball = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num );		/* ボール */
		if(p_stPCG_ball != NULL)
		{
			k = Mmin(g_stBall[i].bSize, 3) + 1;	/* 最大値＋１ */
			pal = Mmul256(g_stST_PCG_LIST[k].Pal);
			for(k=0; k < p_stPCG_ball->Pat_DataMax; k++)
			{
				*(p_stPCG_ball->pPatCodeTbl + k) &= 0xF0FF;
				*(p_stPCG_ball->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* カラーテーブルをリセット */
			}
		}
	}
	else
	{
		i = s_nNum_old;
		p_stPCG_ball = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num );		/* ボール */
	}
	g_index_now = i;	/* 発射予定ボール */
	g_stBall[i].bValidty = ball_now;

	for(j = 0; j < g_Ball_Table[1]; j++)
	{
		if(	g_stBall[j].bValidty == ball_next )	/* 次のデータ */
		{
			break;
		}
		if(	g_stBall[j].bValidty == ball_lost )	/* 無効データ */
		{
			break;
		}
	}

	if(s_nNext_old != j)
	{
		s_nNext_old = j;
		p_stPCG_next = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num );		/* ボール */
		if(p_stPCG_next != NULL)
		{
			k = Mmin(g_stBall[j].bSize, 3) + 1;	/* 最大値＋１ */
			pal = Mmul256(g_stST_PCG_LIST[k].Pal);
			for(k=0; k < p_stPCG_next->Pat_DataMax; k++)
			{
				*(p_stPCG_next->pPatCodeTbl + k) &= 0xF0FF;
				*(p_stPCG_next->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[j].bStatus]));	/* カラーテーブルをリセット */
			}
		}
	}
	else
	{
		j = s_nNext_old;
		p_stPCG_next = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num );		/* ボール */
	}
	g_index_next = j;	/* 次のボールの番号設定 */
	g_stBall[j].bValidty = ball_next;	/* 次 */

	if(p_stPCG_next != NULL)
	{
		p_stPCG_next->x = 224 << PCG_LSB;
		p_stPCG_next->y = 16 << PCG_LSB;
		p_stPCG_next->update	= TRUE;
	}

	if( (i == *nNum) || (j == *nNum) )	/* 落下予定ボールと被った場合 */
	{
		*nNum = -1;
		g_bShot_OK = TRUE;
	}

#ifdef DEBUG	/* デバッグコーナー */
//	memset(sBuf, 0, sizeof(sBuf));
//	sprintf(sBuf, "i(%2d)j(%2d)in(%2d)", s_nNum_old, s_nNext_old, *nNum);
//	Draw_Message_To_Graphic(sBuf, 0, 112, F_MOJI, F_MOJI_BAK);	/* スコア表示 */
#endif

	if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
		((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
		((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
	{
		if(s_bFlag == FALSE)
		{
			s_bFlag = TRUE;

			if(	GetPassTime(PLAYER_INPUT_INTERVAL_TIME, &s_TimeStamp) != 0u)	/* 800ms経過 */
			{
				if(g_bShot_OK == TRUE)
				{
					g_bShot_OK = FALSE;

					if(	g_stBall[i].bValidty == ball_now )	/* 発射予定ボール */
					{
						g_stBall[i].bValidty = ball_exist;	/* 存在するボール */

						g_stBall[j].bValidty = ball_now;	/* 発射ボール */

						*nNum = i;			/* 現在のボール番号設定 */

						if(p_stPCG_ball != NULL)
						{
							p_stPCG_ball->dx = 0;
							p_stPCG_ball->dy = Y_ACC;
							p_stPCG_ball->status = TRUE;
							p_stPCG_ball->update = TRUE;
#ifdef DEBUG	/* デバッグコーナー */
//							memset(sBuf, 0, sizeof(sBuf));
//							sprintf(sBuf, "i(%2d)j(%2d)in(%2d)", s_nNum_old, s_nNext_old, *nNum);
//							Draw_Message_To_Graphic(sBuf, 0, 112, F_MOJI, F_MOJI_BAK);	/* スコア表示 */
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
	
	p_stPCG_ship = PCG_Get_Info(SP_SHIP_0);				/* 自機 */
	if(p_stPCG_ship != NULL)
	{
		int16_t width;

		if(	((GetInput_P1() & JOY_LEFT ) != 0u )	||	/* LEFT */
			((GetInput() & KEY_LEFT) != 0 )		)	/* 左 */
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
			/* アニメーション */
			p_stPCG_ship->Anime++;
			if(p_stPCG_ship->Anime >= p_stPCG_ship->Pat_AnimeMax)
			{
				p_stPCG_ship->Anime = 0;
			}
		}
		else if( ((GetInput_P1() & JOY_RIGHT ) != 0u )	||	/* RIGHT */
				((GetInput() & KEY_RIGHT) != 0 )			)	/* 右 */
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
			/* アニメーション */
			p_stPCG_ship->Anime++;
			if(p_stPCG_ship->Anime >= p_stPCG_ship->Pat_AnimeMax)
			{
				p_stPCG_ship->Anime = 0;
			}
		}
		else
		{
			p_stPCG_ship->dx = 0;
			/* アニメーション */
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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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
		p_stPCG = PCG_Get_Info(i);	/* キャラ */

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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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
			p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ボール */
		}
		else
		{
			p_stPCG = PCG_Get_Info(i);	/* キャラ */
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

					k = Mmin(g_stBall[i].bSize, 3) + 1;	/* 最大値＋１ */
					pal = Mmul256(g_stST_PCG_LIST[k].Pal);
					for(j=0; j < p_stPCG->Pat_DataMax; j++)
					{
						*(p_stPCG->pPatCodeTbl + j) &= 0xF0FF;
						*(p_stPCG->pPatCodeTbl + j) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* カラーテーブルをリセット */
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
	g_stBall[g_index_next].bValidty = ball_next;	/* 次 */
	g_index_now = g_index;
	g_stBall[g_index_now].bValidty 	= ball_now;		/* 現在 */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
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

	/* Resumeデータ読み込み */
	File_Load_CSV( sBuf, (uint16_t *)&dat[0][0], &x_max, &y_max);

	if(x_max > 5)
	{
		puts("ERR:format Xサイズ");
		return;
	}
	if(y_max > BALL_MAX)
	{
		puts("ERR:format Yサイズ");
		return;
	}

#ifdef DEBUG	/* デバッグコーナー */
//	memset(sBuf, 0, sizeof(sBuf));
//	sprintf(sBuf, "(%d,%d)", x_max, y_max);
//	Draw_Message_To_Graphic(sBuf, 0, 240, F_MOJI, F_MOJI_BAK);	/* スコア表示 */
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
#ifdef DEBUG	/* デバッグコーナー */
//		memset(sBuf, 0, sizeof(sBuf));
//		sprintf(sBuf, "i%d,sp(%d)s(%d)x(0x%x)y(0x%x)", i, dat[k][0], dat[k][1], dat[k][2], dat[k][3]);
//		Draw_Message_To_Graphic(sBuf, 0, k*12, F_MOJI, F_MOJI_BAK);	/* スコア表示 */
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

			pal = Mmin(g_stBall[i].bSize, 3) + 1;	/* 最大値＋１ */
			pal = Mmul256(g_stST_PCG_LIST[pal].Pal);
			for(j=0; j < p_stPCG->Pat_DataMax; j++)
			{
				*(p_stPCG->pPatCodeTbl + j) &= 0xF0FF;
				*(p_stPCG->pPatCodeTbl + j) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* カラーテーブルをリセット */
			}
			k++;
			g_index++;
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
static void Ball_Output(void)
{
	int16_t	i;
	int8_t sBuf[256] = {0};
	ST_PCG	*p_stPCG = NULL;
	FILE *fp;

	sprintf(sBuf, "data/Resume%d.dat", g_nGameLevel);

	/* ファイルを開ける */
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

	/* ファイルを閉じる */
	fclose (fp);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t main_Task(void)
{
	int16_t	ret = 0;

	int16_t	rank = -1;
	uint16_t	uFreeRunCount=0u;
#ifdef DEBUG	/* デバッグコーナー */
	uint32_t	unTime_cal = 0u;
	uint32_t	unTime_cal_PH = 0u;
#endif

	int16_t	loop;
	int16_t	pushCount = 0;

	uint8_t	bMode;
	
	ST_TASK		stTask = {0}; 
	ST_CRT		stCRT;
	
	/* 変数の初期化 */
#ifdef W_BUFFER
	SetGameMode(1);
#else
	SetGameMode(0);
#endif	
	loop = 1;
	
	/* 乱数 */
	srandom(TIMEGET());	/* 乱数の初期化 */
	
	do	/* メインループ処理 */
	{
		uint32_t time_st;
#if 0
		ST_CRT	stCRT;
#endif
#ifdef DEBUG	/* デバッグコーナー */
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
		PCG_START_SYNC(g_Vwait);	/* 同期開始 */

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
		GetCRT(&stCRT, bMode);	/* 画面座標取得 */

#ifdef CPU_MONI	/* デバッグコーナー */
		Level_meter(TRUE);	/* 処理負荷計測(開始) */
#endif
		if(Get_SP_Sns() == FALSE)
		{
			if(Input_Main(g_ubDemoPlay) != 0u) 	/* 入力処理 */
			{
				g_ubDemoPlay = FALSE;	/* デモ解除 */
				SetTaskInfo(SCENE_INIT);	/* シーン(初期化処理)へ設定 */
			}
		}
		
#ifdef DEBUG	/* デバッグコーナー */
		DirectInputKeyNum(&g_uDebugNum, 3);	/* キーボードから数字を入力 */
#endif

		if((GetInput() & KEY_b_ESC ) != 0u)	/* ＥＳＣキー */
		{
			/* 終了 */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 6)
			{

			}
			else if(pushCount >= 5)
			{
				ADPCM_Play(1);	/* 終了音 */

				Music_Stop();	/* Stop */

				SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
			}
		}
		else if((GetInput() & KEY_b_F6 ) != 0u)	/* F6キー */
		{
			/* レジューム */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 6)
			{

			}
			else if(pushCount >= 5)
			{
				ADPCM_Play(1);	/* 終了音 */

				Ball_Output();	/* 状態を保存 */
			}
		}
		else if((GetInput() & KEY_b_HELP ) != 0u)	/* HELPキー */
		{
			if( (stTask.bScene != SCENE_GAME_OVER_S) && (stTask.bScene != SCENE_GAME_OVER) )
			{
				/* リセット */
				pushCount = Minc(pushCount, 1);
				if(pushCount >= 6)
				{

				}
				else if(pushCount >= 5)
				{
					ADPCM_Play(1);	/* 終了音 */

					Music_Stop();	/* Stop */

					SetTaskInfo(SCENE_GAME_OVER_S);	/* 終了シーンへ設定 */
				}
			}
		}		
		else
		{
			pushCount = 0;
		}

		if(g_ubDemoPlay == TRUE)	/* デモ */
		{
			static int8_t s_bFlip = FALSE;

			if(	GetPassTime(DEMO_LOGO_INTERVAL_TIME, &s_PassTime) != 0u)	/* 1000ms経過 */
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

			if(	GetPassTime(DEMO_START_END_TIME, &s_DemoTime) != 0u)	/* 30s経過 */
			{
				SetTaskInfo(SCENE_GAME_OVER_S);		/* ゲームオーバーシーン(開始処理)へ設定 */
			}
		}

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* 初期化シーン */
			{
				int16_t i, j;
				int16_t temp;
				int8_t sPath[64];
				memset(sPath, 0, sizeof(sPath));

				Set_CRT_Contrast(0);	/* 真っ暗にする */

				// 乱数の初期化
				srand(time(NULL));

				// Fisher-Yatesアルゴリズムを使用して数字をシャッフル
				for (i = (BALL_MAX - 1); i > 0; --i) {
					j = rand() % (i + 1);
					// 数字を交換
					temp = g_numbers[i];
					g_numbers[i] = g_numbers[j];
					g_numbers[j] = temp;
					g_Sign_Table[i] = ((rand() % 2) == 0)?-1:1;	/* 符号乱数 */
					g_Rand_Table4[i]  = rand() % 4;		/* 乱数テーブル */
					g_Rand_Table8[i]  = rand() % 8;		/* 乱数テーブル */
					g_Rand_Table16[i] = rand() % 16;	/* 乱数テーブル */
					g_Rand_Table32[i] = Mmax((rand() % 32), 16);	/* 乱数テーブル */
				}

				BG_put_Clr(0, 0, 0, 512, 512);	/* BG0 クリア */
				BG_put_Clr(1, 0, 0, 512, 512);	/* BG1 クリア */
				sprintf(sPath, "%s\\sp\\map.csv", Get_DataList_Path());
				BG_TEXT_SET(sPath, BG_P1, BG_MAP_PAL, 256,   0);	/* マップデータによるＢＧの配置 */
				BG_put_Clr(1, 256 + X_POS_MIN-2, Y_POS_MAX, 256 + X_POS_MAX+2, g_TB_GameLevel[0]);
				BG_TEXT_SET(sPath, BG_P1, BG_MAP_PAL,   0, 256);	/* マップデータによるＢＧの配置 */
				BG_put_Clr(1, X_POS_MIN-2, 256 + Y_POS_MAX, X_POS_MAX+2, 256 + g_TB_GameLevel[1]);
				BG_TEXT_SET(sPath, BG_P1, BG_MAP_PAL, 256, 256);	/* マップデータによるＢＧの配置 */

				SetTaskInfo(SCENE_TITLE_S);	/* シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_TITLE_S:
			{
				int8_t sStr[12] = {0};

				Music_Play(3);	/* Title */

				G_VIDEO_PRI(2);	/* TX>GR>SP */

			    PCG_VIEW(0);        /* SP OFF */
				
				G_CLR();		/* グラフィッククリア */
				
				T_Clear();		/* テキストクリア */

				G_Load(1, 0, 0, 0);	/* No.1：タイトル */

//				PutGraphic_To_Symbol12("PUSH A BUTTON TO START!", X_POS_MIN, 192, F_MOJI );			/* title画面入力待ち */
				BG_TextPut("PUSH A BUTTON TO START!", 36, 224);
//				PutGraphic_To_Symbol12("VERSION 0.0.4", X_POS_MAX, 244, F_MOJI );	/* Ver */
				sprintf(sStr, "VER%d.%d.%d", MAJOR_VER, MINOR_VER, PATCH_VER);
				BG_TextPut(sStr, 192, 244);

				GetNowTime(&s_PassTime);	/* タイムアウトカウンタリセット */
				GetNowTime(&s_DemoTime);	/* デモ開始カウンタリセット */
				g_ubDemoPlay = FALSE;		/* デモ以外 */

				Set_CRT_Contrast(-1);	/* もとに戻す */

				SetTaskInfo(SCENE_TITLE);	/* シーン(処理)へ設定 */
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
					((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
				{
					if(s_bFlagInputAB == FALSE)
					{
						s_bFlagInputAB = TRUE;

						ADPCM_Play(4);	/* 決定音 */

						SetTaskInfo(SCENE_TITLE_E);	/* シーン(終了処理)へ設定 */
					}
				}
				else /* なし */
				{
					s_bFlagInputAB = FALSE;
				}
#if 1
				if(	GetPassTime(START_LOGO_INTERVAL_TIME, &s_PassTime) != 0u)	/* 1000ms経過 */
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
					/* 何もしない */
				}
#endif
				if(	GetPassTime(DEMO_START_END_TIME, &s_DemoTime) != 0u)	/* 30s経過 */
				{
					g_ubDemoPlay = TRUE;	/* デモ開始 */
					SetTaskInfo(SCENE_TITLE_E);	/* シーン(終了処理)へ設定 */
				}
				break;
			}
			case SCENE_TITLE_E:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					G_CLR();		/* グラフィッククリア */
					T_Clear();		/* テキストクリア */
					Music_Play(1);	/* Stop */

					SetTaskInfo(SCENE_START_S);	/* シーン(開始処理)へ設定 */
				}
				break;
			}
			case SCENE_START_S:	/* シーン(開始処理) */
			{
				int16_t i;
				int16_t y;

				if(g_ubDemoPlay == FALSE)	/* デモ以外 */
				{
					Music_Play(7);	/* Config */
				}
				G_PaletteSetZero();	/* 0番パレット変更 */
//				G_PAGE_SET(0b0001);	/* GR0 */
//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW, Y_MAX_DRAW, G_BG);	/* 塗りつぶし */

				/* サイドタイトル */
				G_PAGE_SET(0b0001);	/* GR0 */
				G_Load_Mem(2, 0, 48, 0);	/* No.2：セッティング中 */

				_iocs_window( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1);	/* 描画範囲設定 */
				/* 宇宙空間 */
				G_PAGE_SET(0b0010);	/* GR1 */
				if(g_GameSpeed > 3)
				{
					for (i = 0; i < NUM_STARS; ++i)
					{
						Draw_Pset(rand() % (X_MAX_DRAW/2), rand() % Y_MAX_DRAW, (i % 16) + 240);
					}
				}
				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);		/* ビンの外は塗りつぶし */
				Draw_Fill(X_POS_MAX, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);	/* ビンの外は塗りつぶし */

				memcpy((int16_t *)0xE821F0, (int16_t *)0xE82000, sizeof(int16_t) * 16);

				G_PAGE_SET(0b0001);	/* GR0 */

				Draw_Line( X_POS_MIN, Y_POS_BD, X_POS_MAX, Y_POS_BD, 0x1, T_RED);
				y = g_TB_GameLevel[g_nGameLevel];
				Draw_Line( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, G_GREEN, 0xFFFF);
				Draw_Line( X_POS_MIN,          y-1, X_POS_MAX,   y-1, G_GREEN, 0xFFFF);
				
				PutGraphic_To_Symbol12("上下", X_POS_MIN+2, 128+12*0, F_MOJI );			/* 説明 */
				PutGraphic_To_Symbol12(" サイズ変更", X_POS_MIN+2, 128+12*1, F_MOJI );	/* 説明 */
				PutGraphic_To_Symbol12("左右", X_POS_MIN+2, 128+12*2, F_MOJI );			/* 説明 */
				PutGraphic_To_Symbol12(" 初期状態変更", X_POS_MIN+2, 128+12*3, F_MOJI );	/* 説明 */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

				Ball_InitALL();	/* ボールの全初期化 */

				G_VIDEO_PRI(2);	/* TX>GR>SP */
			    PCG_VIEW(1);    /* SP ON */
				BG_ON(0);
				BG_ON(1);

				SetTaskInfo(SCENE_START);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_START:	/* シーン(実施処理) */
			{
				static int8_t s_bFlagInput = FALSE;
				static int8_t s_bFlagInputAB = FALSE;
				int16_t y;

				if( ( GetInput() & KEY_UPPER ) != 0) { /* 上 */
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
							/* 何もしない */
						}
						y = g_TB_GameLevel[g_nGameLevel];
						Draw_Line( X_POS_MIN+1,          y-1, X_POS_MAX,   y-1, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN-2,    Y_POS_MAX, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN,      Y_POS_MAX, X_POS_MIN,     y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX+2,    Y_POS_MAX, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX,      Y_POS_MAX, X_POS_MAX,     y, G_GREEN, 0xFFFF);

						ADPCM_Play(0);	/* SELECT音 */
					}
				}
				else if( ( GetInput() & KEY_LOWER ) != 0 ) { /* 下 */
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
							/* 何もしない */
						}
						y = g_TB_GameLevel[g_nGameLevel];
						Draw_Line( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, G_GREEN, 0xFFFF);

						ADPCM_Play(0);	/* SELECT音 */
					}
				}
				else if( ( GetInput() & KEY_LEFT ) != 0)	/* 左 */
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
				else if( ( GetInput() & KEY_RIGHT ) != 0)	/* 右 */
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
				else /* なし */
				{
					s_bFlagInput = FALSE;
				}

				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
				{
					if(s_bFlagInputAB == FALSE)
					{
						s_bFlagInputAB = TRUE;

						ADPCM_Play(4);	/* 決定音 */

						SetTaskInfo(SCENE_START_E);	/* シーン(終了処理)へ設定 */
					}
				}
				else /* なし */
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
			case SCENE_START_E:	/* シーン(終了処理) */
			{
#ifdef FPS_MONI	/* デバッグコーナー */
				int8_t sBuf[16] = {0};
#endif
				G_PAGE_SET(0b0001);	/* GR0 */

//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1, G_BG);	/* メッセージ消去 */

				G_Load_Mem(3, 0, 48, 0);	/* No.3：ゲーム中 */

//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_POS_BD, G_BG);					/* ビンの外は塗りつぶし */
//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);						/* ビンの外は塗りつぶし */
				Draw_Fill(X_POS_MIN+1, Y_MIN_DRAW, X_POS_MAX-2, g_TB_GameLevel[g_nGameLevel]-3, G_BG);	/* ビンの中は塗りつぶし */
				Draw_Fill(X_POS_MAX+3, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);				/* ビンの外は塗りつぶし */
				Draw_Line(X_POS_MIN, Y_POS_BD, X_POS_MAX, Y_POS_BD, G_RED, 0x0F0F);	/* ボーダーライン */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

#ifdef FPS_MONI	/* デバッグコーナー */
				memset(sBuf, 0, sizeof(sBuf));
				strcpy(sBuf, "FPS");
				Draw_Message_To_Graphic(sBuf, 0, 20, F_MOJI, F_MOJI_BAK);	/* デバッグ */
#endif
				/* ゲーム内部の初期化 */
				g_bBoder_line = 0xFF;
				g_bBoder_count = 0;
				g_ubGameClear = FALSE;
				g_nGameClearCount = 0;
				
				memset(g_bAREA, 0, sizeof(g_bAREA));

				Ball_Init();	/* ボールの初期化 */

				S_Init_Score();	/* スコア初期化 */
				
				SetTaskInfo(SCENE_GAME1_S);	/* シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_GAME1_S:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					if(g_ubDemoPlay == FALSE)	/* デモ以外 */
					{
						Music_Play(4);	/* Play */
					}

					G_PaletteSet(G_GREEN, SetRGB(  0, 31,  0));	/* Green */
//					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */

					SetTaskInfo(SCENE_GAME1);	/* シーン(実施処理)へ設定 */
				}
				break;
			}
			case SCENE_GAME1:
			{
				Ball_Move(&g_index);

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif

				Ball_Check(g_index);	/* スプライトを表示 */

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif

				Boder_line();	/* ボーダーライン */

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
//				printf("Set_Pallet_Shift st(%d)\n", g_index);

				if(g_GameSpeed > 3)
				{
					/* 背景処理 */
					Set_Pallet_Shift(15, 1);
					CRTC_G1_Scroll_8(0, s_G1_sc_y++);
				}

//				printf("Set_Pallet_Shift ed(%d)\n", g_index);

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif

				S_Main_Score();	/* スコアメイン処理 */

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				if(g_ubGameClear == TRUE)	/* クリア条件成立 */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* エンディング(開始処理)へ設定 */
				}

				if(g_bBoder_count >= 64)
				{
					SetTaskInfo(SCENE_GAME1_E);	/* シーン(終了処理)へ設定 */
				}

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				break;
			}
			case SCENE_GAME1_E:
			{
				SetTaskInfo(SCENE_GAME2_S);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_GAME2_S:
			{
				if(g_ubDemoPlay == FALSE)	/* デモ以外 */
				{
					Music_Play(5);	/* Play2 */
				}

				G_PaletteSet(G_GREEN, SetRGB(31,  0,  0));	/* Green -> Red */
				//_iocs_tpalet2( 7, SetRGB(31,  0,  0));	/* Red */

				SetTaskInfo(SCENE_GAME2);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_GAME2:
			{
				Ball_Move(&g_index);

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				Ball_Check(g_index);	/* スプライトを表示 */

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				Boder_line();	/* ボーダーライン */

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				if(g_GameSpeed > 3)
				{
					/* 背景処理 */
					Set_Pallet_Shift(15, 1);
					CRTC_G1_Scroll_8(0, s_G1_sc_y++);
				}

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				S_Main_Score();	/* スコアメイン処理 */

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				if(g_ubGameClear == TRUE)	/* クリア条件成立 */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* エンディング(開始処理)へ設定 */
				}

				if(g_bBoder_count == 0)	/* 復帰 */
				{
					SetTaskInfo(SCENE_GAME1_S);	/* シーン(開始処理)へ設定 */
				}
				else if(g_bBoder_count >= 0xFF)	/* 終了 */
				{
					SetTaskInfo(SCENE_GAME2_E);	/* シーン(終了処理)へ設定 */
				}
				else
				{
					/* 何もしない */
				}

#ifdef CPU_MONI	/* デバッグコーナー */
				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				break;
			}
			case SCENE_GAME2_E:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					ADPCM_Play(5);	/* SET */

					if(g_ubDemoPlay == FALSE)	/* デモ以外 */
					{
						SetTaskInfo(SCENE_HI_SCORE_S);	/* シーン(実施処理)へ設定 */
					}
					else
					{
						SetTaskInfo(SCENE_GAME_OVER_S);		/* ゲームオーバーシーン(開始処理)へ設定 */
					}
				}
				break;
			}
			case SCENE_NEXT_STAGE_S:	/* エンディング(開始処理)  */
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					uint8_t sBuf[8];

					BG_TextPut("                       ", 44, 128);
					BG_TextPut("   CONGRATULATIONS!    ", 44, 128);
					BG_TextPut("                       ", 44, 152);
					BG_TextPut("    PUSH A BUTTON!     ", 44, 152);

					S_GetPos(44, 136);		/* スコア表示座標更新 */
					S_Add_Score(1000000);	/* スコア更新 */

					g_nGameClearCount++;

					memset(sBuf, 0, sizeof(sBuf));
					sprintf(sBuf, "COUNT(%d)", g_nGameClearCount);
					Draw_Message_To_Graphic(sBuf, 0, 20, F_MOJI, F_MOJI_BAK);	/* スコア表示 */

					ADPCM_Play(2);	/* ゲームクリア音 */

					SetTaskInfo(SCENE_NEXT_STAGE);	/* エンディング(処理)へ設定 */
				}
				break;
			}
			case SCENE_NEXT_STAGE:		/* エンディング(処理)  */
			{
				static int8_t s_bFlagInputAB = FALSE;

				Ball_Check(g_index);		/* スプライトを表示 */

				BG_TextPut("   CONGRATULATIONS!    ", 44, 128);
				BG_TextPut("    PUSH A BUTTON!     ", 44, 152);

				/* 背景処理 */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
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

						ADPCM_Play(4);	/* 決定音 */

						G_PAGE_SET(0b0001);	/* GR0 */
						y = g_TB_GameLevel[g_nGameLevel];
						Draw_Line( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, G_GREEN, 0xFFFF);
						Draw_Line( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, G_GREEN, 0xFFFF);

						SetTaskInfo(SCENE_NEXT_STAGE_E);	/* エンディング(終了処理)へ設定 */
					}
				}
				else /* なし */
				{
					s_bFlagInputAB = FALSE;
				}
				break;
			}
			case SCENE_NEXT_STAGE_E:	/* エンディング(終了処理)  */
			{
				g_bBoder_count = 0;

				SetTaskInfo(SCENE_GAME1_S);	/* シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_HI_SCORE_S:	/* ハイスコアランキングシーン(開始処理) */
			{
				int16_t y;

				G_VIDEO_PRI(0);	/* GR>TX>SP */

				G_PAGE_SET(0b0001);	/* GR0 */

				y = g_TB_GameLevel[g_nGameLevel];
				Draw_Line( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, G_BG, 0xFFFF);	/* 下線 */
				Draw_Line( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, G_BG, 0xFFFF);	/* 左外線 */
				Draw_Line( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, G_BG, 0xFFFF);	/* 左内線 */
				Draw_Line( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, G_BG, 0xFFFF);	/* 右外線 */
				Draw_Line( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, G_BG, 0xFFFF);	/* 右内線*/

				y = NAME_INP_Y;
				G_CLR_AREA(0, 255, y, 255, 0);	/* ページ 0 */

				rank = S_Score_Board();	/* ハイスコア表示 */
				
				if(rank >= 0)
				{
					Music_Play(8);	/* Hi-SCORE */
				}
				else
				{
					Music_Play(9);	/* Ranking view */
				}

				SetTaskInfo(SCENE_HI_SCORE);	/* ハイスコアランキングシーンへ設定 */
				break;
			}
			case SCENE_HI_SCORE:	/* ハイスコアランキングシーン */
			{
				if(rank >= 0)
				{
					if(S_Score_Name_Main(GetInput(), rank) >= 3)	/* スコアネーム入力 */
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* ハイスコアランキングシーン(終了処理)へ設定 */
					}
				}
				else
				{
					if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
						((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
						((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
					{
						ADPCM_Play(4);	/* 決定音 */
						SetTaskInfo(SCENE_HI_SCORE_E);	/* ハイスコアランキングシーン(終了処理)へ設定 */
					}
				}
				
				/* 背景処理 */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				break;
			}
			case SCENE_HI_SCORE_E:	/* ハイスコアランキングシーン(終了処理) */
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					SetTaskInfo(SCENE_GAME_OVER_S);		/* ゲームオーバーシーン(開始処理)へ設定 */
				}
				break;
			}
			case SCENE_GAME_OVER_S:
			{
				int16_t x, y;

				T_Clear();	/* テキストクリア */
				BG_Scroll(1, 0, 0);	/* BG1 何もなしエリア */
//				BG_OFF(0);
//				BG_OFF(1);
			    PCG_VIEW(0);        /* SP OFF */

				if(g_ubDemoPlay == FALSE)	/* デモ以外 */
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
					G_CLR_AREA(0, 255, y, 255, 0);	/* ページ 0 */
					
					S_Score_Board();	/* ハイスコア表示 */
					GetNowTime(&s_DemoTime);	/* デモ開始カウンタリセット */
				}

				x = stCRT.view_offset_x + 76;
				y = stCRT.view_offset_y + 0;
				Draw_Fill( x, y + 6, x + 12*9, y + 24*1 + 6, G_BLACK);	/* 塗りつぶし */
				Draw_Box( x, y + 6, x + 12*9, y + 24*1 + 6, G_D_RED, 0xFFFF);	/* 枠 */
				PutGraphic_To_Symbol24("GAME OVER", x + 1, y + 1 + 6, G_RED);
				PutGraphic_To_Symbol24("GAME OVER", x, y + 6, G_L_PINK);

				SetTaskInfo(SCENE_GAME_OVER);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_GAME_OVER:
			{
				/* 背景処理 */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				if(g_ubDemoPlay == FALSE)	/* デモ以外 */
				{
					if(	((GetInput_P1() & JOY_A ) != 0u)	||	/* Aボタン */
						((GetInput() & KEY_b_Z) != 0u)		||	/* KEY A(z) */
						((GetInput() & KEY_b_SP ) != 0u)	)	/* スペースキー */
					{
						ADPCM_Play(4);	/* 決定音 */
						SetTaskInfo(SCENE_GAME_OVER_E);	/* シーン(終了処理)へ設定 */
					}
				}
				else
				{
					if(	GetPassTime(DEMO_GAMEOVER_TIME, &s_DemoTime) != 0u)	/* 5000ms経過 */
					{
						SetTaskInfo(SCENE_GAME_OVER_E);	/* シーン(終了処理)へ設定 */
					}
				}

				break;
			}
			case SCENE_GAME_OVER_E:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					G_PaletteSet(G_GREEN, SetRGB(  0, 31,  0));	/* Green */
//					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */

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
		SetFlip(TRUE);	/* フリップ許可 */

		PCG_END_SYNC(g_Vwait);	/* スプライトの出力 */

		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */

#ifdef FPS_MONI	/* デバッグコーナー */
		if( (stTask.bScene == SCENE_GAME1) || (stTask.bScene == SCENE_GAME2) )	/* ゲーム中 */
		{
			int8_t sBuf[16] = {0};
			/* FPS */
			sprintf(sBuf, "%3d", g_bFPS_PH);
			Draw_Message_To_Graphic(sBuf, 32, 20, F_MOJI, F_MOJI_BAK);	/* デバッグ */
		}
#endif

#ifdef CPU_MONI	/* デバッグコーナー */
		Level_meter(FALSE);	/* 処理負荷計測 */
#endif

#ifdef DEBUG	/* デバッグコーナー */
		/* 処理時間計測 */
		GetNowTime(&time_now);
		unTime_cal = time_now - time_st;	/* LSB:1 UNIT:ms */
		g_unTime_cal = unTime_cal;
		if( stTask.bScene == SCENE_GAME1 )
		{
			unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);
			g_unTime_cal_PH = unTime_cal_PH;
		}
#endif

#ifdef MEM_MONI	/* デバッグコーナー */
		GetFreeMem();	/* 空きメモリサイズ確認 */
#endif

	}
	while( loop );

	g_bExit = FALSE;

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
	int16_t nNum;
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 開始");
#endif
#ifdef MEM_MONI	/* デバッグコーナー */
	g_nMaxUseMemSize = GetFreeMem();
	printf("FreeMem(%d[kb])\n", g_nMaxUseMemSize);	/* 空きメモリサイズ確認 */
	puts("App_Init メモリ");
#endif
	/* MFP */
	MFP_INIT();	/* V-Sync割り込み等の初期化処理 */
#ifdef DEBUG	/* デバッグコーナー */
	printf("App_Init MFP(%d)\n", Get_CPU_Time());
#endif
	if(SetNowTime(0) == FALSE)
	{
		/*  */
	}
	nNum = Get_CPU_Time();	/* 300 = 10MHz基準 */
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
	Input_Init();			/* 入力初期化 */

	puts("App_Init FileList");
	Init_FileList_Load();	/* リストファイルの読み込み */

	puts("App_Init Music");
	/* 音楽 */
	Init_Music();	/* 初期化(スーパーバイザーモードより前)	*/

	Music_Play(0);	/* Init */
	Music_Play(1);	/* Stop */
	Music_Play(2);	/* Loading */

	/* スーパーバイザーモード開始 */
	g_nSuperchk = _iocs_b_super(0);
	if( g_nSuperchk < 0 ) {
#ifdef DEBUG	/* デバッグコーナー */
		puts("App_Init すでに_iocs_b_super");
#endif
	} else {
#ifdef DEBUG	/* デバッグコーナー */
		puts("App_Init _iocs_b_super開始");
#endif
	}

	/* 画面 */
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
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init CRTC(画面)");
//	KeyHitESC();	/* デバッグ用 */
#endif

	/* グラフィック */
	G_INIT();			/* 画面の初期設定 */
	G_CLR();			/* クリッピングエリア全開＆消す */
	G_HOME(0);			/* ホーム位置設定 */
	G_VIDEO_INIT();		/* ビデオコントローラーの初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init グラフィック");
#endif

	/* スプライト／ＢＧ */
	PCG_INIT();			/* スプライト／ＢＧの初期化 */
	PCG_DATA_LOAD();	/* スプライトのデータ読み込み */
    PCG_VIEW(1);        /* SP ON / BG0 OFF / BG1 OFF */
	PCG_INIT_CHAR();	/* スプライト＆ＢＧ定義セット */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init スプライト／ＢＧ");
#endif

	g_Vwait = 1;

#if CNF_MACS
	/* 動画 */
	MOV_INIT();		/* 初期化処理 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init Movie(MACS)");
#endif
#endif

	/* Task */
	TaskManage_Init();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init Task");
#endif
	
	/* マウス初期化 */
	Mouse_Init();	/* マウス初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init マウス");
#endif
	
	/* テキスト */
	T_INIT();	/* テキストＶＲＡＭ初期化 */
	T_PALET();	/* テキストパレット初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init T_INIT");
#endif

	/* スコア初期化 */
	S_All_Init_Score();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init S_All_Init_Score");
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
	int16_t	ret = 0;

#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit 開始");
#endif
	
	if(g_bExit == TRUE)
	{
		puts("エラーをキャッチ！ ESC to skip");
		KeyHitESC();	/* デバッグ用 */
	}
	g_bExit = TRUE;

	/* グラフィック */
	G_CLR();			/* クリッピングエリア全開＆消す */

	/* スプライト＆ＢＧ */
	PCG_OFF();		/* スプライト＆ＢＧ非表示 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit スプライト");
#endif

	/* MFP */
	ret = MFP_EXIT();				/* MFP関連の解除 */
#ifdef DEBUG	/* デバッグコーナー */
	printf("App_exit MFP(%d)\n", ret);
#endif

	/* 画面 */
	CRTC_EXIT(0x100 + g_nCrtmod);	/* モードをもとに戻す */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit 画面");
#endif

	Mouse_Exit();	/* マウス非表示 */

	/* テキスト */
	T_EXIT();				/* テキスト終了処理 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit テキスト");
#endif
	
	MyMfree(0);				/* 全てのメモリを解放 */
#ifdef DEBUG	/* デバッグコーナー */
	printf("MaxUseMem(%d[kb])\n", g_nMaxUseMemSize - GetMaxFreeMem());
	puts("App_exit メモリ");
#endif

	_dos_kflushio(0xFF);	/* キーバッファをクリア */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit キーバッファクリア");
#endif

	/*スーパーバイザーモード終了*/
	_iocs_b_super(g_nSuperchk);
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit スーパーバイザーモード終了");
#endif
	/* 音楽 */
	Exit_Music();			/* 音楽停止 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit Music");
#endif

#if 0	/* デバッグコーナー */
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
			p_stPCG = PCG_Get_Info(i);	/* キャラ */

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
	
#ifdef FPS_MONI	/* デバッグコーナー */
	uint32_t time_now;
	static uint8_t	bFPS = 0u;
	static uint32_t	unTime_FPS = 0u;
#endif

	if(g_bFlip == FALSE)	/* 描画中なのでフリップしない */
	{
		return ret;
	}
	else
	{
#ifdef W_BUFFER
		ST_CRT		stCRT;
		GetCRT(&stCRT, g_mode);	/* 画面座標取得 */
#endif
		SetFlip(FALSE);			/* フリップ禁止 */
					
#ifdef W_BUFFER
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

		/* 非表示画面を表示画面へ切り替え */
		G_HOME(g_mode);

		/* 部分クリア */
		G_CLR_AREA(	stCRT.hide_offset_x, WIDTH,
					stCRT.hide_offset_y, HEIGHT, 0);
#endif
		
#ifdef FPS_MONI	/* デバッグコーナー */
		bFPS++;
#endif
		ret = 1;
	}

#ifdef FPS_MONI	/* デバッグコーナー */
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
#if 0
	if(stTask.b96ms == TRUE)	/* 96ms周期 */
	{
#ifdef DEBUG	/* デバッグコーナー */
		int8_t	sBuf[8];
#endif
		switch(stTask.bScene)
		{
			case SCENE_GAME1:
			case SCENE_GAME2:
			{
#ifdef FPS_MONI	/* デバッグコーナー */
				memset(sBuf, 0, sizeof(sBuf));
				sprintf(sBuf, "%3d", g_bFPS_PH);
				Draw_Message_To_Graphic(sBuf, 24, 24, F_MOJI, F_MOJI_BAK);	/* デバッグ */
#endif
				break;
			}
			default:
				break;
		}
	}
#endif
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
#if CNF_RASTER
	RasterProc(pRaster_cnt);	/*　ラスター割り込み処理 */
#endif /* CNF_RASTER */
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
//	puts("App_VsyncProc");
#if CNF_VDISP
	Timer_D_Less_NowTime();

	if(Input_Main(g_ubDemoPlay) != 0u) 	/* 入力処理 */
	{
		g_ubDemoPlay = FALSE;	/* デモ解除 */

		SetTaskInfo(SCENE_INIT);	/* シーン(初期化処理)へ設定 */
	}
	
	App_FlipProc();	/* 画面切り替え */

#endif /* CNF_VDISP */
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

	/* COPYキー無効化 */
	init_trap12();
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

	App_Init();		/* 初期化 */
	
	main_Task();	/* メイン処理 */

	App_exit();		/* 終了処理 */
	
	return ret;
}

#endif	/* MAIN_C */
