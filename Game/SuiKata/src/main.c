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

/* グローバル構造体 */
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

/* 関数のプロトタイプ宣言 */
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

	p_stPCG = PCG_Get_Info(SP_ARROW_0);		/* 矢印 */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = X_POS_MIN - 16 - 2;
		p_stPCG->y = g_Boder_line - 8;
		p_stPCG->update	= TRUE;
	}

	p_stPCG  = PCG_Get_Info(SP_GAL_0);		/* ギャル */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = 208;
		p_stPCG->y = Mmax(Y_POS_BD, g_Boder_count);
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

	/* 当たり判定 */
	if( ((g_stBall[i].bSize)   == (g_stBall[j].bSize))	&&
		((g_stBall[i].bStatus) == (g_stBall[j].bStatus)) )
	{
		int16_t BallSize;

		if(g_ubGameClear == TRUE)
		{
			return ret;
		}

		/* 自分 */
		g_stBall[i].bStatus++;
		/* ボール設定 */
		BallSize = g_stBall[i].bSize;		

		/* 相手 */
		g_stBall[j].bStatus = (rand() % 3);
		g_stBall[j].bSize = 0;
		g_stBall[j].bValidty = ball_lost;

		p_stPCG_other->x = 0;
		p_stPCG_other->y = 0;
		p_stPCG_other->update = FALSE;


		if( g_stBall[i].bStatus > 2 )	/* 進化の上限を超えた→サイズチェンジ */
		{
			/* 自分は一旦、消える */
			g_stBall[i].bStatus = (rand() % 3);
			g_stBall[i].bSize = 0;
			g_stBall[i].bValidty = ball_lost;
			p_stPCG->x = 0;
			p_stPCG->y = 0;
			p_stPCG->update = FALSE;

			if( BallSize >= 3 )	/* 最大サイズの最終系で合体 */
			{
				g_ubGameClear = TRUE;	/* ゲームクリア */

				S_GetPos(44, 136);		/* スコア表示座標更新 */

				ret = S_Add_Score(1000000);	/* スコア更新 */
				return ret;
			}
			else
			{
				S_GetPos(x, y);	/* スコア表示座標更新 */

				ret = S_Add_Score(1000);	/* スコア更新 */
			}

			BallSize++;	/* サイズアップ */ 
			m = Mmin(BallSize, 3);	/* 最大値＋１ */

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
						pal = Mmul256(g_stST_PCG_LIST[m + 1].Pal);
						for(p=0; p < p_stPCG_new->Pat_DataMax; p++)
						{
							*(p_stPCG_new->pPatCodeTbl + p) &= 0xF0FF;
							*(p_stPCG_new->pPatCodeTbl + p) |= (pal + Mmul256(g_Ball_Color[g_stBall[n].bStatus]));	/* カラーセット */
						}
						p_stPCG_new->update = TRUE;
#if 0
						width	= Mmul16(p_stPCG_new->Pat_w);
						height	= Mmul16(p_stPCG_new->Pat_h);

						/* ビン */
						if(y > g_TB_GameLevel[g_nGameLevel] - height)	/* ビン底 */
						{
							p_stPCG_new->dy = 0;
							y = g_TB_GameLevel[g_nGameLevel] - height;
						}
						else
						{
							p_stPCG_new->dy = Y_ACC;
						}

						if(x <= X_POS_MIN)			/* ビン左 */
						{
							p_stPCG_new->dx = 0;
							x = X_POS_MIN;
						}
						else if(x > X_POS_MAX - width)	/* ビン右 */
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
		else	/* カラーチェンジ */
		{
			if( BallSize >= 3 )	/* 最大サイズの最終系で合体 */
			{
				/* 自分は一旦、消える */
				g_stBall[i].bStatus = (rand() % 3);
				g_stBall[i].bSize = 0;
				g_stBall[i].bValidty = ball_lost;
				p_stPCG->x = 0;
				p_stPCG->y = 0;
				p_stPCG->update = FALSE;

				g_ubGameClear = TRUE;	/* ゲームクリア */

				S_Set_Combo(0);			/* コンボカウンタ リセット */
				ret = S_Add_Score(1);	/* スコア更新 */
			}
			else
			{
				S_GetPos(x, y);	/* スコア表示座標更新 */

				ret = S_Add_Score(100);	/* スコア更新 */

				pal = Mmul256(g_stST_PCG_LIST[g_stBall[i].bSize + 1].Pal);
				for(k=0; k < p_stPCG->Pat_DataMax; k++)
				{
					*(p_stPCG->pPatCodeTbl + k) &= 0xF0FF;
					*(p_stPCG->pPatCodeTbl + k) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* カラーセット */
				}
#if 0
				height	= Mmul16(p_stPCG->Pat_h);
				/* ビン */
				if(y > g_TB_GameLevel[g_nGameLevel] - height)	/* ビン底 */
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

	/* 移動量 */
	sx = Mdiv256s(dx);
	sy = Mdiv256s(dy);
	x = (uint16_t)((int16_t)x + sx);
	y = (uint16_t)((int16_t)y + sy);

	/* 横移動 減衰 */
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

	/* 縦移動 減衰 */
	if(hitFlag == TRUE)
	{
	}
	else	/* 縦移動 加速 */
	{
		/* 自然落下 */
		dy += Y_ACC;
		dy = Mmin(dy, 0x1FFF);
	}

	/* ビン */
	if(y > g_TB_GameLevel[g_nGameLevel] - height)	/* ビン底 */
	{
		y = g_TB_GameLevel[g_nGameLevel] - height;
		hitFlag = TRUE;	/* 衝突 */
	}
	else
	{
	}

	if(x < X_POS_MIN)			/* ビン左 */
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
	else if(x > X_POS_MAX - width)	/* ビン右 */
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
	
	/* 加速度の更新 */
	p_stPCG->dx = dx;
	p_stPCG->dy = dy;

	/* 最終位置 */
	p_stPCG->x = x;
	p_stPCG->y = y;

	ret = hitFlag;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Ball_Check(int16_t nBallCount)
{
#ifdef DEBUG	/* デバッグコーナー */
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

#ifdef DEBUG	/* デバッグコーナー */
//	Level_meter(FALSE);	/* 処理負荷計測 */
#endif
#ifdef DEBUG	/* デバッグコーナー */
//	static int16_t count = 0;
#endif

	line = g_TB_GameLevel[g_nGameLevel];	/* 前回値 */
#if 0
	GetNowTime(&time_st);	/* タイムアウトカウンタリセット */
#endif
#ifdef DEBUG	/* デバッグコーナー */
//	printf("Ball_Check st(%d)\n", count);
#endif

	memset(overlapping_pairs, 0, sizeof(overlapping_pairs));
	memset(non_overlapping, 0, sizeof(non_overlapping));

	/* 重なり抽出 */
	for(i = 0; i < BALL_MAX - 1; i++)
	{
#ifdef DEBUG	/* デバッグコーナー */
//		Level_meter(FALSE);	/* 処理負荷計測 */
#endif
		bValid = g_stBall[i].bValidty;
		if(	(bValid == ball_lost )	||	/* 無効データ */
			(bValid == ball_now )	||	/* 発射予定ボール */
			(bValid == ball_next )	)	/* 次のボール */
		{
			if(i == g_index_next)		/* 次のボール */
			{
				/* 何もしない */
			}
			else if(i == g_index_now)	/* 発射予定ボール */
			{
				/* 何もしない */
			}
			else
			{
				p_stPCG->update	= FALSE;
			}
			continue;
		}

		p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ボール */
		if(p_stPCG == NULL)
		{
			continue;
		}

		overlapped = FALSE;	/* 初期化 */
		
		x = p_stPCG->x;
		y = p_stPCG->y;
		p_stPCG->update	= TRUE;	/* 描画 */

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
			if(	(bValid == ball_lost )	||	/* 無効データ */
				(bValid == ball_now )	||	/* 発射予定ボール */
				(bValid == ball_next )	)	/* 次のボール */
			{
				if(j == g_index_next)		/* 次のボール */
				{
					/* 何もしない */
				}
				else if(j == g_index_now)	/* 発射予定ボール */
				{
					/* 何もしない */
				}
				else
				{
					p_stPCG_other->update = FALSE;
				}
				continue;
			}

			p_stPCG_other = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num);	/* 他のボール */
			if(p_stPCG_other == NULL)
			{
				continue;
			}
			
			x1 = p_stPCG_other->x;
			y1 = p_stPCG_other->y;
			p_stPCG_other->update	= TRUE;	/* 描画 */

			width1	= Mmul16(p_stPCG_other->Pat_w);
			height1	= Mmul16(p_stPCG_other->Pat_h);
			w1 = Mdiv2(width1); 
			h1 = Mdiv2(height1);
			dw1 = Mdiv16(width1); 
			dh1 = Mdiv16(height1);
			cx1 = x1 + w1; 
			cy1 = y1 + h1;

			line = Mmin(y, line);	/* 更新 */
			line = Mmin(y1, line);	/* 更新 */

			distance = APL_distance(cx, cy , cx1, cy1);
			distance_src = Mmin((w + w1), (h + h1));
			distance_src = distance_src * distance_src;

			if(distance < (Mmul2(distance_src) + distance_src))		/* 距離が近い */
			{
				if(p_stPCG->dy != 0)	/* 減速し貫通しないようにする */
				{
					p_stPCG->dy = Y_ACC2;
				}
				if(p_stPCG_other->dy != 0)	/* 減速し貫通しないようにする */
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
//			if(distance < distance_src)		/* 距離がさらに近い */
			{
				if(p_stPCG->dy != 0)		/* 停止 */
				{
					p_stPCG->dy = 0;
				}
				if(p_stPCG_other->dy != 0)	/* 停止 */
				{
					p_stPCG_other->dy = 0;
				}
				overlapping_pairs[num_pairs][0] = i;
				overlapping_pairs[num_pairs][1] = j;
				num_pairs++;
				if((i == nBallCount) || (j == nBallCount))		/* 有効化したボール */
				{
					g_bShot_OK = TRUE;	/* 次の発射許可 */
				}
				overlapped = TRUE;	/* 重なった */
			}
		}

		if(overlapped == FALSE)
		{
			non_overlapping[num_non_overlapping++] = i;	/* 重ならなかった */
		}
	}

	if(line != g_TB_GameLevel[g_nGameLevel])
	{
		g_Boder_line = line;
	}

#ifdef DEBUG	/* デバッグコーナー */
//	printf("Ball_Check st2(%d)\n", count);
#endif

	if(s_bErase == TRUE)
	{
		if(	GetPassTime(COMBO_INTERVAL_SE_TIME, &s_WaitTime) != 0u  )	/* 500ms経過 */
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
        allRectanglesNonOverlapping = TRUE; // 一旦重なっていないと仮定

		sound = 0;

		/* 重なりデータのみ判定 */	
		for(k = 0; k < num_pairs; k++)
		{
			int16_t Prex, Prey;

			i = overlapping_pairs[k][0];
			j = overlapping_pairs[k][1];

			bValid = g_stBall[i].bValidty;
			if(	(bValid == ball_lost )	||	/* 無効データ */
				(bValid == ball_now )	||	/* 発射予定ボール */
				(bValid == ball_next )	)	/* 次のボール */
			{
				continue;
			}
			
			bValid = g_stBall[j].bValidty;
			if(	(bValid == ball_lost )	||	/* 無効データ */
				(bValid == ball_now )	||	/* 発射予定ボール */
				(bValid == ball_next )	)	/* 次のボール */
			{
				continue;
			}

			p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ボール */
			if(p_stPCG == NULL)
			{
				continue;
			}
			p_stPCG_other = PCG_Get_Info(SP_BALL_S_1 + g_stBall[j].bSP_num);	/* 他のボール */
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
				if( Ball_Combine(p_stPCG, i, p_stPCG_other, j) != 0 )	/* 合体成立 */
				{
					ADPCM_Play(Mmin(S_Get_Combo() + 7 - 1, p_list_max - 1));	/* 連鎖音 */
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

				if(distance < distance_src)		/* 距離が近い */
				{
					dy  =   0;
					dy1  =  0;
				}
				else
#endif
				{
					int16_t *pmx, *pmy;
					// 移動先の候補の位置
					int16_t mx[] = {  0, 4,-4, 0, 0 };	// 右、左、上、下、０
					int16_t my[] = {  4, 0, 0, 4, 0 };	// 右、左、上、下、０

					pmx = &mx[0];	/* 右優先 */
					pmy = &my[0];	/* 右優先 */

					distance_min = 0xFFFFFFFF;

					// 移動先の候補を試す
					for(l = 0; l < 5; l++)
					{
						x += *(pmx + l) * w;
						y += *(pmy + l) * h;

						// 移動先が他の矩形と重なっていないかチェック
						distance = APL_distance(x + Mdiv2(width), y + Mdiv2(height), cx1, cy1);
						distance_src = Mmin((w + w1), (h + h1));
						distance_src *= distance_src;
						distance_src = (Mdiv2(distance_src) - Mdiv4(distance_src)); 

						if(distance > distance_src)		/* 距離が離れた */
						{
							allRectanglesNonOverlapping = FALSE;	/* オーバーラップ回避 */

							if(distance < distance_min)
							{
								distance_min = distance;

								if( (g_stBall[i].bSize) == (g_stBall[j].bSize) )	/* 同じサイズ */
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
								else if( (g_stBall[i].bSize) < (g_stBall[j].bSize) )	/* サイズが異なる */
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
								else	/* j側が小さい */
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
							/* 動けない */
							if( (g_stBall[i].bSize) < (g_stBall[j].bSize) )	/* サイズが異なる */
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
						// 位置を戻す
						x = Prex;
						y = Prey;
					}
				}

				p_stPCG->x = x;
				p_stPCG->y = y;
				p_stPCG->dx = dx;
				p_stPCG->dy = dy;
				p_stPCG->update	= TRUE;	/* 描画 */
				Ball_Point_Update(p_stPCG, TRUE);	/* ヒット */

				p_stPCG_other->x = x1;
				p_stPCG_other->y = y1;
				p_stPCG_other->dx = dx1;
				p_stPCG_other->dy = dy1;
				p_stPCG_other->update	= TRUE;	/* 描画 */
				Ball_Point_Update(p_stPCG_other, TRUE);	/* ヒット */
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
		if(	(bValid == ball_lost )	||	/* 無効データ */
			(bValid == ball_now )	||	/* 発射予定ボール */
			(bValid == ball_next )	)	/* 次のボール */
		{
			continue;
		}

		p_stPCG = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num);	/* ボール */
		if(p_stPCG == NULL)
		{
			continue;
		}
		else
		{
			p_stPCG->update	= TRUE;	/* 描画 */

			if(Ball_Point_Update(p_stPCG, FALSE) != 0)	/* ヒットしない */
			{
				if((nBallCount == i) || (nBallCount == -1))		/* 有効化したボール */
				{
					g_bShot_OK = TRUE;	/* 次の発射許可 */
				}
			}
		}
	}

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
#endif
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
		p_stPCG_ball = PCG_Get_Info(SP_BALL_S_1 + g_stBall[i].bSP_num );		/* ボール */
		if(p_stPCG_ball != NULL)
		{
			pal = Mmul256(g_stST_PCG_LIST[g_stBall[i].bSize + 1].Pal);
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
			pal = Mmul256(g_stST_PCG_LIST[g_stBall[i].bSize + 1].Pal);
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
	if(p_stPCG_next != NULL)
	{
		p_stPCG_next->x = 224;
		p_stPCG_next->y = 16;
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

	if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
		((g_Input & KEY_A) != 0u)		||		/* A */
		((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
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
						g_stBall[j].bValidty = ball_now;
						g_stBall[i].bValidty = ball_exist;

						*nNum = i;			/* 現在のボール番号設定 */
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
	
	p_stPCG_ship = PCG_Get_Info(SP_SHIP_0);				/* 自機 */
	if(p_stPCG_ship != NULL)
	{
		if(	((g_Input_P[0] & JOY_LEFT ) != 0u )	||	/* LEFT */
			((g_Input & KEY_LEFT) != 0 )		)	/* 左 */
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
			/* アニメーション */
			p_stPCG_ship->Anime++;
			if(p_stPCG_ship->Anime >= p_stPCG_ship->Pat_AnimeMax)
			{
				p_stPCG_ship->Anime = 0;
			}
		}
		else if( ((g_Input_P[0] & JOY_RIGHT ) != 0u )	||	/* RIGHT */
				((g_Input & KEY_RIGHT) != 0 )			)	/* 右 */
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
			p_stPCG->update	= FALSE;
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
	g_stBall[1].bValidty = ball_next;	/* 次 */
	g_stBall[0].bValidty = ball_now;	/* 現在 */

	g_index = 0;
	g_index_next = 1;

	for(i = 0; i < PCG_NUM_MAX; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* キャラ */

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
						*(p_stPCG->pPatCodeTbl + j) |= (pal + Mmul256(g_Ball_Color[g_stBall[i].bStatus]));	/* カラーテーブルをリセット */
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
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t main(int16_t argc, int8_t** argv)
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

	App_Init();	/* 初期化 */
	
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

#ifdef W_BUFFER	/* デバッグコーナー */
		/* 部分クリア */
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

		if(Get_SP_Sns() == FALSE)
		{
			if(Input_Main(g_ubDemoPlay) != 0u) 	/* 入力処理 */
			{
				g_ubDemoPlay = FALSE;	/* デモ解除 */
				SetTaskInfo(SCENE_INIT);	/* シーン(初期化処理)へ設定 */
			}
		}
		
		if((g_Input & KEY_b_ESC ) != 0u)	/* ＥＳＣキー */
		{
			/* 終了 */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 5)
			{
				ADPCM_Play(1);	/* 終了音 */

				Music_Stop();	/* Stop */

				SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
			}
		}
		else if((g_Input & KEY_b_HELP ) != 0u)	/* HELPキー */
		{
			if( (stTask.bScene != SCENE_GAME_OVER_S) && (stTask.bScene != SCENE_GAME_OVER) )
			{
				/* リセット */
				pushCount = Minc(pushCount, 1);
				if(pushCount >= 5)
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
				Music_Play(3);	/* Title */

				G_VIDEO_PRI(2);	/* TX>GR>SP */

				PCG_OFF();		/* SP OFF */
				
				G_CLR();		/* グラフィッククリア */
				
				T_Clear();		/* テキストクリア */

				G_Load(1, 0, 0, 0);	/* No.1：タイトル */

//				PutGraphic_To_Symbol12("PUSH A BUTTON TO START!", X_POS_MIN, 192, F_MOJI );			/* title画面入力待ち */
				BG_TextPut("PUSH A BUTTON TO START!", 36, 224);
//				PutGraphic_To_Symbol12("VERSION 0.0.4", X_POS_MAX, 244, F_MOJI );	/* Ver */
				BG_TextPut("VER0.0.6", 192, 244);

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
				if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
					((g_Input & KEY_A) != 0u)		||		/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
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
				for (i = 0; i < NUM_STARS; ++i)
				{
					Draw_Pset(rand() % (X_MAX_DRAW/2), rand() % Y_MAX_DRAW, (i % 16) + 240);
				}
				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);		/* ビンの外は塗りつぶし */
				Draw_Fill(X_POS_MAX, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);	/* ビンの外は塗りつぶし */

				memcpy((int16_t *)0xE821F0, (int16_t *)0xE82000, sizeof(int16_t) * 16);

				T_Line2( X_POS_MIN, Y_POS_BD, X_POS_MAX, Y_POS_BD, 0x1, T_RED);
				y = g_TB_GameLevel[g_nGameLevel];
				T_Line2( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, 0xFFFF, T_GREEN);
				T_Line2( X_POS_MIN,          y-1, X_POS_MAX,   y-1, 0xFFFF, T_GREEN);
				
				G_PAGE_SET(0b0001);	/* GR0 */
				
				PutGraphic_To_Symbol12("上下", X_POS_MIN+2, 128+12*0, F_MOJI );			/* 説明 */
				PutGraphic_To_Symbol12(" サイズ変更", X_POS_MIN+2, 128+12*1, F_MOJI );	/* 説明 */
				PutGraphic_To_Symbol12("左右", X_POS_MIN+2, 128+12*2, F_MOJI );			/* 説明 */
				PutGraphic_To_Symbol12(" 初期状態変更", X_POS_MIN+2, 128+12*3, F_MOJI );	/* 説明 */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

				Ball_InitALL();	/* ボールの全初期化 */

				G_VIDEO_PRI(2);	/* TX>GR>SP */
				PCG_ON();		/* SP ON */
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

				if( ( g_Input & KEY_UPPER ) != 0) { /* 上 */
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
							/* 何もしない */
						}
						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN+1,          y-1, X_POS_MAX,   y-1, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN-2,    Y_POS_MAX, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN,      Y_POS_MAX, X_POS_MIN,     y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX+2,    Y_POS_MAX, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX,      Y_POS_MAX, X_POS_MAX,     y, 0xFFFF, T_GREEN);

						ADPCM_Play(0);	/* SELECT音 */
					}
				}
				else if( ( g_Input & KEY_LOWER ) != 0 ) { /* 下 */
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
							/* 何もしない */
						}
						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN+1,        y-1, X_POS_MAX,   y-1, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN-2,  Y_POS_MAX, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN,    Y_POS_MAX, X_POS_MIN,     y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX+2,  Y_POS_MAX, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX,    Y_POS_MAX, X_POS_MAX,     y, 0xFFFF, T_GREEN);

						ADPCM_Play(0);	/* SELECT音 */
					}
				}
				else if( ( g_Input & KEY_LEFT ) != 0)	/* 左 */
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
				else if( ( g_Input & KEY_RIGHT ) != 0)	/* 右 */
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

				if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
					((g_Input & KEY_A) != 0u)		||		/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
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
				G_PAGE_SET(0b0001);	/* GR0 */

				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1, G_BG);	/* メッセージ消去 */

				G_Load_Mem(3, 0, 48, 0);	/* No.3：ゲーム中 */

				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_POS_BD, G_BG);	/* ビンの外は塗りつぶし */
//				Draw_Fill(X_MIN_DRAW, Y_MIN_DRAW, X_POS_MIN, Y_MAX_DRAW-1, G_BG);		/* ビンの外は塗りつぶし */
				Draw_Fill(X_POS_MIN, Y_MIN_DRAW, X_POS_MAX, Y_MAX_DRAW-1, G_BG);		/* ビンの中は塗りつぶし */
				Draw_Fill(X_POS_MAX, Y_MIN_DRAW, (X_MAX_DRAW/2)-1, Y_MAX_DRAW-1, G_BG);	/* ビンの外は塗りつぶし */

				Draw_Message_To_Graphic(&g_TB_GameDiffLevel[g_nGameDifflevel][0], 0, 8, F_MOJI, F_MOJI_BAK);

#if 0	/* デバッグコーナー */
				memset(sBuf, 0, sizeof(sBuf));
				sprintf(sBuf, "FPS(%3d )", g_bFPS_PH);
				Draw_Message_To_Graphic(sBuf, 0, 24, F_MOJI, F_MOJI_BAK);	/* デバッグ */
#endif
				/* ゲーム内部の初期化 */
				g_Boder_line = 0xFF;
				g_Boder_count = 0;
				g_ubGameClear = FALSE;
				g_nGameClearCount = 0;

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

					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */
	
					SetTaskInfo(SCENE_GAME1);	/* シーン(実施処理)へ設定 */
				}
				break;
			}
			case SCENE_GAME1:
			{
#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(TRUE);	/* 処理負荷計測 */
#endif
				Ball_Move(&g_index);

#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
//				printf("Ball_Check st(%d)\n", g_index);
				Ball_Check(g_index);		/* スプライトを表示 */
//				printf("Ball_Check ed(%d)\n", g_index);
				
#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
//				printf("Boder_line st(%d)\n", g_index);

				Boder_line();	/* ボーダーライン */

//				printf("Boder_line ed(%d)\n", g_index);

#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
//				printf("Set_Pallet_Shift st(%d)\n", g_index);

				/* 背景処理 */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

//				printf("Set_Pallet_Shift ed(%d)\n", g_index);

#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
//				printf("S_Main_Score st(%d)\n", g_index);

				S_Main_Score();	/* スコアメイン処理 */

//				printf("S_Main_Score ed(%d)\n", g_index);
#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
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
						SetTaskInfo(SCENE_GAME1_E);	/* シーン(終了処理)へ設定 */
					}
				}
				else if(g_Boder_line < Y_POS_BD + 64)
				{
					/* 保持 */
				}
				else
				{
					g_Boder_count = 0;
				}

				if(g_ubGameClear == TRUE)	/* クリア条件成立 */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* エンディング(開始処理)へ設定 */
				}
#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
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

				_iocs_tpalet2( 7, SetRGB(31,  0,  0));	/* Red */

				SetTaskInfo(SCENE_GAME2);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_GAME2:
			{
#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(TRUE);	/* 処理負荷計測 */
#endif
				Ball_Move(&g_index);

#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				Ball_Check(g_index);		/* スプライトを表示 */
				
#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
#endif
				Boder_line();	/* ボーダーライン */

				/* 背景処理 */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				S_Main_Score();	/* スコアメイン処理 */

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
						p_stPCG  = PCG_Get_Info(SP_GAL_0);		/* ギャル */
						if(p_stPCG != NULL)
						{
							int16_t height;
							height	= Mmul16(p_stPCG->Pat_h);
							if(g_Boder_count >= (256 - height))
							{
								SetTaskInfo(SCENE_GAME2_E);	/* シーン(終了処理)へ設定 */
							}
						}
					}
				}
				else if(g_Boder_line < Y_POS_BD + 64)
				{
					/* 保持 */
				}
				else
				{
					g_Boder_count = Mdec(g_Boder_count, 1);
					if(g_Boder_count == 0)
					{
						SetTaskInfo(SCENE_GAME1_S);	/* シーン(開始処理)へ設定 */
					}
				}

				if(g_ubGameClear == TRUE)	/* クリア条件成立 */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* エンディング(開始処理)へ設定 */
				}
#ifdef DEBUG	/* デバッグコーナー */
//				Level_meter(FALSE);	/* 処理負荷計測 */
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

				/* 背景処理 */
				Set_Pallet_Shift(15, 1);
				CRTC_G1_Scroll_8(0, s_G1_sc_y++);

				if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
					((g_Input & KEY_A) != 0u)		||		/* A */
					((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
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

						y = g_TB_GameLevel[g_nGameLevel];
						T_Line2( X_POS_MIN-2,  Y_POS_MIN, X_POS_MIN-2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MIN,    Y_POS_MIN, X_POS_MIN,     y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX+2,  Y_POS_MIN, X_POS_MAX+2,   y, 0xFFFF, T_GREEN);
						T_Line2( X_POS_MAX,    Y_POS_MIN, X_POS_MAX,     y, 0xFFFF, T_GREEN);

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
				g_Boder_count = 0;

				SetTaskInfo(SCENE_GAME1_S);	/* シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_HI_SCORE_S:	/* ハイスコアランキングシーン(開始処理) */
			{
				G_VIDEO_PRI(0);	/* GR>TX>SP */

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
					if(S_Score_Name_Main(g_Input, rank) >= 3)	/* スコアネーム入力 */
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* ハイスコアランキングシーン(終了処理)へ設定 */
					}
				}
				else
				{
					if(	((g_Input_P[0] & JOY_A ) != 0u)	||		/* A */
						((g_Input & KEY_A) != 0u)		||		/* A */
						((g_Input & KEY_b_SP ) != 0u)		)	/* スペースキー */
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
				PCG_OFF();		/* SP OFF */
			
				if(g_ubDemoPlay == FALSE)	/* デモ以外 */
				{
					Music_Play(6);	/* Game Over */
				}
				else
				{
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
					if(	((g_Input_P[0] & JOY_A ) != 0u)	||	/* Aボタン */
						((g_Input & KEY_A) != 0u)		||	/* KEY A */
						((g_Input & KEY_b_SP ) != 0u)	)	/* スペースキー */
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
					_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */

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
		GetFreeMem();	/* 空きメモリサイズ確認 */
#endif
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
	g_nMaxUseMemSize = GetFreeMem();
	printf("FreeMem(%d[kb])\n", g_nMaxUseMemSize);	/* 空きメモリサイズ確認 */
	puts("App_Init メモリ");
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
	PCG_XSP_INIT();		/* XSP初期化 */
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
	g_Vwait = 1;

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
	
#ifdef DEBUG	/* デバッグコーナー */
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
#endif
				
		bFPS++;

		ret = 1;
	}

#ifdef DEBUG	/* デバッグコーナー */
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
#ifdef DEBUG	/* デバッグコーナー */
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

#endif	/* MAIN_C */
