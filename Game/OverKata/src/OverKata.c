#ifndef	OVERKATA_C
#define	OVERKATA_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>
#include <interrupt.h>

#include "inc/usr_macro.h"
#include "inc/apicglib.h"

#include "OverKata.h"

#include "APL_MACS.h"
#include "APL_Math.h"
#include "CRTC.h"
#include "Course_Obj.h"
#include "DMAC.h"
#include "Draw.h"
#include "EnemyCAR.h"
#include "FileManager.h"
#include "Graphic.h"
#include "Input.h"
#include "MFP.h"
#include "Music.h"
#include "MyCar.h"
#include "Output_Text.h"
#include "PCG.h"
#include "Raster.h"
#include "Score.h"
#include "Task.h"
#include "Text.h"
#include "Trap14.h"

#define	_DEBUG_MODE	/* デバッグするならコメントアウトを外す */

#define	BG_SKIP_TIME	(8u)
#define	BG_TIME_OUT		(56u)

/* グローバル変数 */
int32_t	g_nSuperchk = 0;
int32_t	g_nIntLevel = 0;
int32_t	g_nIntCount = 0;
int32_t	g_nCrtmod = 0;
int32_t	g_nMaxMemSize;
int16_t	g_CpuTime = 0;
int16_t	g_Stage;
uint8_t	g_mode;
uint8_t	g_mode_rev;
uint8_t	g_Vwait = 1;
uint32_t	g_unTime_cal = 0u;
uint32_t	g_unTime_cal_PH = 0u;
uint8_t	g_bFlip = FALSE;
uint8_t	g_bFlip_old = FALSE;
uint8_t	g_bExit = TRUE;

static uint16_t s_uStartCount = 0;
static uint16_t s_uGoalCount = 0;

volatile uint16_t	g_uGameStatus;

#ifdef DEBUG	/* デバッグコーナー */
uint8_t		g_bDebugMode = FALSE;
uint8_t		g_bFPS_PH = 0u;
uint16_t	g_uDebugNum = 0; 
uint32_t	g_unTime_Pass[MAX_G] = {0u};
int16_t		g_DebugPosX, g_DebugPosY;
int16_t		g_DebugHis[10];
#endif

uint16_t	uRoad = 0;
int16_t		Update = 0;
static int16_t Torque = 0xFFFF;

#ifdef DEBUG	/* デバッグコーナー */
	int16_t dst_x, dst_y;
	int16_t	x_min, y_min;
	int16_t	x_max, y_max;
	uint16_t src_w, src_h;
	uint8_t	bFlag;
#endif

/* enum */
#ifdef DEBUG	/* デバッグコーナー */
enum{
	DEBUG_NONE,
	DEBUG_COURSE_OBJ,
	DEBUG_ENEMYCAR,
	DEBUG_MYCAR,
	DEBUG_ROAD,
	DEBUG_RASTER,
	DEBUG_INPUT,
	DEBUG_CPUTIME,
	DEBUG_MEMORY,
	DEBUG_FREE,
	DEBUG_MAX,
};
#endif

/* グローバル構造体 */

/* 関数のプロトタイプ宣言 */
int16_t main(int16_t, int8_t**);
static void App_Init(void);
static void App_exit(void);
int16_t	BG_main(uint32_t);
int16_t	FlipProc(void);
int16_t	SetFlip(uint8_t);
void Set_DI(void);
void Set_EI(void);
int16_t	GetGameMode(uint8_t *);
int16_t	SetGameMode(uint8_t);

#ifdef DEBUG	/* デバッグコーナー */
int16_t	GetDebugNum(uint16_t *);
int16_t	SetDebugNum(uint16_t);
int16_t	GetDebugMode(uint8_t *);
int16_t	SetDebugMode(uint8_t);
int16_t	GetDebugPos(int16_t *, int16_t *);
int16_t	SetDebugPos(int16_t, int16_t);
int16_t	GetDebugHis(int16_t *, int16_t);
int16_t	SetDebugHis(int16_t);
void Debug_View(uint16_t);
#endif

void (*usr_abort)(void);	/* ユーザのアボート処理関数 */

/* 関数 */
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

	uint32_t	unTime_cal = 0u;
	uint32_t	unTime_cal_PH = 0u;
	uint32_t	unPass_time = 0xFFFFFFFFu;
	int16_t		DemoCount = 0;

	uint16_t	uFreeRunCount=0u;

	int16_t	loop = 1;
	
#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode = TRUE;
	uint8_t	bDebugMode_flag;
	uint32_t	unDebugCounter1 = 0u;
	uint32_t	unDebugCounter2 = 0u;
#endif
	
	uint8_t	bCRTMode = TRUE;
	uint8_t	bCRTMode_flag;

	uint8_t	bMode;
	
	ST_TASK		stTask = {0}; 
	
	/* 変数の初期化 */
	SetGameMode(1);
	g_Stage = 1;
#ifdef DEBUG	/* デバッグコーナー */
	g_uDebugNum = 0x80;
	bDebugMode = TRUE;
#endif

	
	
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
	
	SetTaskInfo(SCENE_INIT);	/* 初期化シーンへ設定 */
	
	/* 乱数 */
	srandom(TIMEGET());	/* 乱数の初期化 */
	
	do	/* メインループ処理 */
	{
		uint32_t time_st;
#ifdef DEBUG	/* デバッグコーナー */
		uint32_t time_now;
		uint8_t	bTimePass = 0;
#endif
		/* 終了処理 */
		if(loop == 0)
		{
			break;
		}
		
		/* 時刻設定 */
		GetNowTime(&time_st);	/* メイン処理の開始時刻を取得 */
		SetStartTime(time_st);	/* メイン処理の開始時刻を記憶 */
		
#ifdef DEBUG	/* デバッグコーナー */
		if(g_bDebugMode == TRUE)
		{
			g_unTime_Pass[0] = time_st;
			bTimePass++;
		}
#endif
		/* タスク処理 */
		TaskManage();				/* タスクを管理する */
		GetTaskInfo(&stTask);		/* タスクの情報を得る */

		/* モード引き渡し */
		GetGameMode(&bMode);
		
		/* 終了 */
		if((g_Input & KEY_b_ESC ) != 0u)	/* ＥＳＣキー */
		{
			SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
		}

#if 1
		/* CRT 31kHz/15kHz切替 */
		if(ChatCancelSW((g_Input & KEY_b_HELP)!=0u, &bCRTMode_flag) == TRUE)	/* HELPで31kHz/15kHz切替 */
		{
			if(bCRTMode == TRUE)
			{
				CRTC_INIT(0);	/* 31kHz */
				bCRTMode = FALSE;
			}
			else
			{
				CRTC_INIT(1);	/* 15kHz */
				bCRTMode = TRUE;
			}
		}
#endif
		
#ifdef DEBUG	/* デバッグコーナー */
		if(ChatCancelSW((g_Input & KEY_b_SP)!=0u, &bDebugMode_flag) == TRUE)	/* スペースでデバッグON/OFF */
		{
			if(bDebugMode == FALSE)	bDebugMode = TRUE;
			else					bDebugMode = FALSE;
		}
		SetDebugMode(bDebugMode);
		
		DirectInputKeyNum(&g_uDebugNum, 3);	/* キーボードから数字を入力 */
#endif
#ifdef DEBUG	/* デバッグコーナー */
		if(g_bDebugMode == TRUE)
		{
			GetNowTime(&time_now);
			g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 1 */
			bTimePass++;
			g_unTime_Pass[0] = time_now;	/* 一時保存 */
		}
#endif
		
		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* 初期化シーン */
			{
				/* 変数の初期化 */
				SetGameMode(1);
				SetGameMode(2);
				
#ifdef DEBUG	/* デバッグコーナー */
				g_uDebugNum = 0x80;
#endif
				uFreeRunCount = 0;
				
				/* スプライト＆ＢＧ */
				PCG_VIEW(0x00u);	/* スプライト＆ＢＧ非表示 */
				/* テキスト */
				T_Clear();	/* テキストクリア */
				T_PALET();	/* テキストパレット設定 */

#ifndef	_DEBUG_MODE	/* デバッグするなら */
				/* リリース */
				SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
#else
				/* デバッグコーナー */
				SetTaskInfo(SCENE_DEBUG_S);	/* デバッグ(開始処理)へ設定 */
#endif
				break;
			}
			case SCENE_DEBUG_S:
			{
				puts("デバッグコーナー 開始");

				T_Clear();			/* テキストクリア */
				
				/* ラスター情報の初期化 */
				Raster_Init();
				
				/* コースデータの初期化 */
				Road_Init(g_Stage);
				
				/* ゲーム内容の初期化 */
				S_Init_Score();	/* スコアの初期化 */
				
				/* スプライト＆ＢＧ表示 */
				PCG_INIT();		/* スプライト／ＢＧの初期化 */
				PCG_VIEW(0x07u);	/* スプライト＆ＢＧ表示 */

				/* 自車 */
				//MyCar_G_Load();		/* 自車の画像読み込み */
				MyCarInfo_Init();	/* 自車の情報初期化 */
				
				/* グラフィック表示 */
				G_HOME(0);			/* ホーム位置設定 */
				G_CLR();			/* グラフィッククリア */
				G_PaletteSetZero();	/* グラフィックパレットの基本設定 */
				
				Music_Play(14);	/* ローディング中 */
				
				/* ライバル車の初期化 */
				InitEnemyCAR();
				
				//G_Load_Mem( START_PT_CG, 0, 0, 0 );	/* スタートゲート */
				//G_Load_Mem( GOAL_PT_CG, 0, 0, 0 );	/* スタートゲート */
				/* コースのオブジェクトの初期化 */
				//InitCourseObj();
				
				/* コースの背景 */
				Road_BG_Init(1);

				Debug_View(uFreeRunCount);	/* デバッグ情報表示 */
				
				/* ゲームスタートに関する変数の初期化 */
				s_uStartCount = 0;
				s_uGoalCount = 0;
				g_uGameStatus = 0;	/* ゲーム中 */
				
				SetTaskInfo(SCENE_DEBUG);	/* デバッグ(処理)へ設定 */
				break;
			}
			case SCENE_DEBUG:
			{
				Torque = 0;
				Update = 0;
				
				if( (g_bFlip_old == TRUE) && (g_bFlip == FALSE) )	/* 切り替え直後判定 */
				{
					/* 自車の処理 */
					Update |= MyCarInfo_Update(g_Input, &Torque);
				}
				
				if(stTask.b16ms == TRUE)
				{
					Update |= MyCarInfo_Update16ms(Torque);
				}
				
				if( (g_bFlip_old == TRUE) && (g_bFlip == FALSE) )	/* 切り替え直後判定 */
				{
					/* ラスター処理 */
					Update |= Road_Pat_Main(&uRoad);	/* コースデータの更新 */

					g_uGameStatus = 1;	/* ゲーム中 */

					Update |= Raster_Main();	/* コースの処理 */
				}
				
				/* 余った時間で処理 */
				BG_main(time_st);	/* バックグランド処理 */
				
				g_Vwait = 0;	/* No Wait */
				
				break;
			}
			case SCENE_DEBUG_E:
			{
				puts("デバッグコーナー 終了");

				loop = 0;	/* ループ終了 */

				break;
			}
			case SCENE_TITLE_S:	/* タイトルシーン(開始処理) */
			{
				SetFlip(FALSE);			/* フリップ禁止 */
				
				Music_Play(2);	/* タイトル曲 */
				
				/* グラフィック */
				G_HOME(1);				/* ホーム位置設定 */
				G_Load_Mem( TITLE_CG, X_OFFSET, 0, 0 );	/* タイトル画像 */
			
				/* テキスト */
				T_Clear();		/* テキストクリア */
				
				BG_TextPut("OVER KATA", 96, 128);		/* タイトル文字 */
				BG_TextPut("PUSH A BUTTON", 80, 160);	/* ボタン押してね */

				GetPassTime( 100, &unPass_time );		/* 初期化 */

				SetTaskInfo(SCENE_TITLE);	/* タイトルシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_TITLE:	/* タイトルシーン */
			{
				int8_t	sStageMes[256] = {0};
				
				if(g_Input == KEY_A)	/* Aボタン */
				{
					Music_Stop();	/* 音楽再生 停止 */
					
					ADPCM_Play(10);	/* SE:決定 */
				
					GetPassTime( 100, &unPass_time );		/* 初期化 */
					
					SetTaskInfo(SCENE_TITLE_E);	/* タイトルシーン(開始処理)へ設定 */
				}
				
				if((g_Input & KEY_LEFT) != 0u)	/* 左 */
				{
					GetPassTime( 100, &unPass_time );		/* 初期化 */
					g_Stage = Mmax(g_Stage--, 1);
					ADPCM_Play(4);	/* SE:左 */
				}
				else if((g_Input & KEY_RIGHT) != 0u)	/* 右 */
				{
					GetPassTime( 100, &unPass_time );		/* 初期化 */
					g_Stage = Mmin(g_Stage++, 9);
					ADPCM_Play(4);	/* SE:右 */
				}
				else
				{
				}
				g_Vwait = 1;	/* Wait 1 */
				
				sprintf(sStageMes, "COURSE%d", g_Stage);
				BG_TextPut(sStageMes, 104, 144);	/* ステージ */
				
				
				if( GetPassTime( 30000, &unPass_time ) != 0u )	/* 所定時間何もなければデモシーンへ */
				{
					Music_Stop();	/* 音楽再生 停止 */
					
					SetTaskInfo(SCENE_DEMO_S);	/* デモシーン(開始処理)へ設定 */
				}
				break;
			}
			case SCENE_TITLE_E:	/* タイトルシーン(終了処理) */
			{
				/* スプライト＆ＢＧ */
				PCG_VIEW(0x00u);	/* スプライト＆ＢＧ非表示 */

				/* テキスト表示 */
				T_Clear();	/* テキストクリア */

				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */

				SetTaskInfo(SCENE_START_S);	/* ゲーム開始シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_DEMO_S:	/* デモシーン(開始処理) */
			{
				uint16_t demo;
				DemoCount = 0;
				
				/* スプライト＆ＢＧ */
				PCG_VIEW(0x00u);	/* スプライト＆ＢＧ非表示 */

				/* テキスト表示 */
				T_Clear();	/* テキストクリア */

				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */
				
				Music_Play(6);	/* デモ曲 */
				
				for(demo=0; demo<6; demo++)
				{
					G_Load_Mem(DEMO_CG + demo, X_OFFSET, Y_OFFSET + 32, 0);	/* グラフィックの読み込み */
//					CG_File_Load(DEMO_CG + demo);		/* グラフィックの読み込み */
				}

				SetTaskInfo(SCENE_DEMO);	/* ゲーム開始シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_DEMO:	/* デモシーン */
			{
				if( GetPassTime( 2000, &unPass_time ) != 0u )	/* 所定時間何もなければ実行 */
				{
					if( DemoCount < 6u )	/* デモシーケンス満了 */
					{
						G_Load_Mem( DEMO_CG + DemoCount, X_OFFSET, 32, 0 );	/* デモ画像 */
					}
					DemoCount++;	/* 次の画像 */
				}
				
				if( (DemoCount >= 10u) || /* デモシーケンス満了 */
					(g_Input == KEY_A) )	/* Aボタン */
				{
					Music_Stop();	/* 音楽再生 停止 */
				
					SetTaskInfo(SCENE_DEMO_E);	/* デモシーン(終了処理)へ設定 */
				}
				break;
			}
			case SCENE_DEMO_E:	/* デモシーン(終了処理) */
			{
				/* スプライト＆ＢＧ */
				PCG_VIEW(0x00u);	/* スプライト＆ＢＧ非表示 */

				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */
				
				/* テキスト表示 */
				T_Clear();	/* テキストクリア */

				SetTaskInfo(SCENE_TITLE_S);	/* ゲームスタートタスクへ設定 */
				break;
			}
			case SCENE_START_S:	/* ゲーム開始シーン(開始処理) */
			{
				Music_Stop();	/* 音楽再生 停止 */
				
				/* スプライト＆ＢＧ */
				PCG_VIEW(0x00u);	/* スプライト＆ＢＧ非表示 */

				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */
				
				/* テキスト表示 */
				T_Clear();	/* テキストクリア */
				
				/* 動画 */
				if(g_uGameStatus == 0)
				{
					MOV_Play(0);	/* スタート */
				}
				else
				{
					MOV_Play(3);	/* ウソをつくな */
				}

				//Music_Play(10);	/* TRUTH */
				Music_Play(11);	/* Mach-1 */
				
				/* ラスター情報の初期化 */
				Raster_Init();
				
				/* コースデータの初期化 */
				Road_Init(g_Stage);
				
				SetTaskInfo(SCENE_START);	/* ゲームスタートタスクへ設定 */
				break;
			}
			case SCENE_START:	/* ゲーム開始シーン */
			{
				/* コースを描画する */
				if(g_Input == KEY_B)	/* Bボタン */
				{
					g_Vwait = 0;	/* No Wait */
				}
				else
				{
					g_Vwait = 1;	/* Wait 1 */
				}
				
				if( Road_Map_Draw(0) < 0 )	/* コース描画完了判定 */
				{
					g_Vwait = 1;	/* Wait 1 */
					SetTaskInfo(SCENE_START_E);	/* ゲームスタートタスクへ設定 */
				}
				break;
			}
			case SCENE_START_E:	/* ゲーム開始シーン(終了処理) */
			{
				Music_Play(1);	/* ローディング中 */

				Set_CRT_Contrast(0);	/* コントラスト暗 */
				
				/* テキスト表示 */
				T_Clear();			/* テキストクリア */
				
				/* ゲーム内容の初期化 */
				S_Init_Score();	/* スコアの初期化 */
				
				/* スプライト＆ＢＧ表示 */
				PCG_INIT();		/* スプライト／ＢＧの初期化 */
				PCG_VIEW(0x07u);	/* スプライト＆ＢＧ表示 */

				/* 自車 */
				MyCar_G_Load();		/* 自車の画像読み込み */
				MyCarInfo_Init();	/* 自車の情報初期化 */
				
				/* ライバル車の初期化 */
				InitEnemyCAR();

				/* コースのオブジェクトの初期化 */
				InitCourseObj();
				
				/* コースの背景 */
				Road_BG_Init(1);
				
				G_Load_Mem( START_PT_CG, 0, 0, 0 );	/* スタートゲート */
				G_Load_Mem( GOAL_PT_CG, 0, 0, 0 );	/* ゴールゲート */
				
				/* テキスト表示 */
				T_PALET();		/* テキストパレット設定 */
				T_TopScore();	/* TOP */
				T_Time();		/* TIME */
				T_Score();		/* SCORE */
				T_Speed();		/* SPEED */
				T_Gear();		/* GEAR */
				T_SetBG_to_Text();	/* テキスト用作業用データ展開 */
				T_TimerStop();	/* タイマー停止 */
				T_TimerReset();	/* タイマーリセット */
				
				/* グラフィック表示 */
				G_HOME(0);			/* ホーム位置設定 */
				G_CLR();			/* グラフィッククリア */
				G_PaletteSetZero();	/* グラフィックパレットの基本設定 */
				
				Debug_View(uFreeRunCount);	/* デバッグ情報表示 */
				
				Music_Stop();	/* 音楽再生 停止 */
				
				g_uGameStatus = 0;	/* ゲーム中 */
				
				SetTaskInfo(SCENE_GAME_S);	/* ゲーム(開始処理)タスクへ設定 */
				break;
			}
			case SCENE_GAME_S:	/* ゲームシーン開始処理 */
			{
				int16_t	i;
				int16_t	x, y;
				static int16_t	sig_count = 0;

				ST_PCG	*p_stPCG = NULL;

				g_uGameStatus = 2;	/* スタート状態 */
				
				if(sig_count == 0)
				{
					sig_count = 1;
					
					Music_Play(3);	/* メインBGM */
					M_SetMusic(0);	/* 効果音再生の設定 */

					Set_CRT_Contrast(-1);	/* コントラストdef */
					
					/* 自車の情報を取得 */
					MyCarInfo_Update(g_Input, &Torque);
					MyCarInfo_Update16ms(Torque);
					
					/* ラスター処理 */
					Road_Pat_Main(&uRoad);	/* コースデータの更新 */
					Raster_Main();			/* コースの処理 */
					
					/* ゲームスタートに関する変数の初期化 */
					s_uStartCount = 0;
					s_uGoalCount = 0;
				}
				else
				{
					MyCarInfo_Update16ms(0);
				}

				for(i=0; i<3; i++)
				{
					p_stPCG = PCG_Get_Info(ROAD_PCG_SIGNAL_1+i);	/* シグナルランプ */
					/* シグナルランプ */
					x =  104 + (i * 16) + SP_X_OFFSET;
					y =  72 + SP_Y_OFFSET;
					
					if(p_stPCG != NULL)
					{
						p_stPCG->x = x;
						p_stPCG->y = y;
						if(sig_count < 5)
						{
							p_stPCG->update	= TRUE;
						}
						else
						{
							p_stPCG->update	= FALSE;
						}
					}
				}
				
				if( GetPassTime( 1000, &unPass_time ) != 0u )	/* 所定時間何もなければ実行 */
				{
					switch(sig_count)
					{
					case 1:
						ADPCM_Play(3);	/* SE:3 */
						p_stPCG = PCG_Get_Info(ROAD_PCG_SIGNAL_1);	/* シグナルランプ */
						p_stPCG->pPatCodeTbl[0] = SetBGcode(0, 0, 0x01, 0xFF);
						break;
					case 2:
						ADPCM_Play(2);	/* SE:2 */
						p_stPCG = PCG_Get_Info(ROAD_PCG_SIGNAL_2);	/* シグナルランプ */
						p_stPCG->pPatCodeTbl[0] = SetBGcode(0, 0, 0x01, 0xFF);
						break;
					case 3:
						ADPCM_Play(1);	/* SE:1 */
						p_stPCG = PCG_Get_Info(ROAD_PCG_SIGNAL_3);	/* シグナルランプ */
						p_stPCG->pPatCodeTbl[0] = SetBGcode(0, 0, 0x01, 0xFF);
						break;
					default:
						ADPCM_Play(16);	/* SE:WAKAME */
						p_stPCG = PCG_Get_Info(ROAD_PCG_SIGNAL_1);	/* シグナルランプ */
						p_stPCG->pPatCodeTbl[0] = SetBGcode(0, 0, 0x0A, 0xFF);
						p_stPCG = PCG_Get_Info(ROAD_PCG_SIGNAL_2);	/* シグナルランプ */
						p_stPCG->pPatCodeTbl[0] = SetBGcode(0, 0, 0x0A, 0xFF);
						p_stPCG = PCG_Get_Info(ROAD_PCG_SIGNAL_3);	/* シグナルランプ */
						p_stPCG->pPatCodeTbl[0] = SetBGcode(0, 0, 0x0A, 0xFF);
						break;
					}
					sig_count++;
				}
				
				/* 余った時間で処理 */
				BG_main(time_st);	/* バックグランド処理 */
				
				if(sig_count >= 5)
				{
					sig_count = 0;
					T_TimerStart();	/* タイマー開始 */
					
					SetTaskInfo(SCENE_GAME);	/* ゲームタスクへ設定 */
				}
				break;
			}
			case SCENE_GAME:	/* ゲームシーン */
			{
				if((g_Input & KEY_b_Q) != 0u)	/* Ｑ */
				{
					SetTaskInfo(SCENE_GAME_E);	/* ゲームシーン(終了処理)へ設定 */
				}
				
				
				if( (g_bFlip_old == TRUE) && (g_bFlip == FALSE) )	/* 切り替え直後判定 */
				{
					/* 自車の情報を取得 */
					Update |= MyCarInfo_Update(g_Input, &Torque);	/* 自車の情報を更新1 */
//					unDebugCounter1++;
				}
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 2 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
				if(stTask.b16ms == TRUE)
				{
					Update |= MyCarInfo_Update16ms(Torque);	/* 自車の情報を更新2 */
//					unDebugCounter2++;
				}
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 3 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
				if( (g_bFlip_old == TRUE) && (g_bFlip == FALSE) )	/* 切り替え直後判定 */
				{
					/* ロードパターン */
					Update |= Road_Pat_Main(&uRoad);	/* コースデータの更新 */
					if(uRoad == 0xFFFF)
					{
						g_uGameStatus = 4;	/* ゴール */
					}
					else if(uRoad >= g_stRoadInfo.Courselength - 16)
					{
						g_uGameStatus = 3;	/* ゴール直前 */
					}
					else if(uRoad <= 8)
					{
						g_uGameStatus = 2;	/* スタート */
					}
					else
					{
						g_uGameStatus = 1;	/* ゲーム中 */
					}
					if(Update != 0)
					{
						/* ラスター処理 */
						Update |= Raster_Main();	/* コースのラスター処理 */
					}
				}
				
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 4 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
#ifdef DEBUG	/* デバッグコーナー */
				//SetDebugPos(unDebugCounter1, unDebugCounter2);
#endif
				/* 余った時間で処理 */
				BG_main(time_st);	/* バックグランド処理 */
				
				/* 次のタスクへ */
				if(g_uGameStatus >= 4u)
				{
					GetPassTime( 100, &unPass_time );		/* 初期化 */
					SetTaskInfo(SCENE_GAME_E);	/* ゲームシーン(終了処理)へ設定 */
				}
				
				//g_Vwait = 1;	/* Wait */
				g_Vwait = 0;	/* No Wait */
				
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 5 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
				
				break;
			}
			case SCENE_GAME_E:	/* ゲームシーン(終了処理) */
			{
				StopRaster();	/* ラスター割り込み処理ストップ */
				Music_Stop();	/* 音楽再生 停止 */
				
				switch(g_uGameStatus)
				{
					case 0:	/* 次のステージ */
					case 1:	/* 次のステージ */
					case 2:	/* 次のステージ */
					case 3:	/* 次のステージ */
					{
						break;
					}
					case 4:	/* 次のステージ */
					{
						if( GetPassTime( 2000, &unPass_time ) != 0u )	/* 所定時間何もなければ実行 */
						{
							SetTaskInfo(SCENE_NEXT_STAGE_S);	/* 次のステージへ(開始処理)へ設定 */
						}
						break;
					}
					case 5:	/* 時間切れ */
					{
						SetTaskInfo(SCENE_GAME_OVER_S);		/* ゲームオーバーシーン(開始処理)へ設定 */
						break;
					}
					default:	/* 意図しない遷移 */
					{
						SetTaskInfo(SCENE_GAME_OVER_S);		/* ゲームオーバーシーン(開始処理)へ設定 */
						break;
					}
				}
				break;
			}
			case SCENE_GAME_OVER_S:	/* ゲームオーバーシーン(開始処理) */
			{
				SetFlip(FALSE);			/* フリップ禁止 */
				
				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */
				
				/* スプライト＆ＢＧ */
				PCG_VIEW(0x00u);	/* スプライト＆ＢＧ非表示 */

				/* テキスト */
				T_Clear();		/* テキストクリア */

				/* 動画 */
				MOV_Play(4);	/* ポリンキー */
				
				BG_TextPut("G A M E  O V E R", 64, 128);		/* GAME OVER文字 */
				
				Music_Play(16);	/* GameOver曲 */
				
				SetTaskInfo(SCENE_GAME_OVER);	/* ゲームオーバーシーンタスクへ設定 */
				break;
			}
			case SCENE_GAME_OVER:	/* ゲームオーバーシーン */
			{
				if(g_Input == KEY_B)	/* Bボタン */
				{
					SetTaskInfo(SCENE_GAME_OVER_E);	/* ゲームオーバーシーン(終了処理)タスクへ設定 */
				}
				break;
			}
			case SCENE_GAME_OVER_E:	/* ゲームオーバーシーン(終了処理) */
			{
				SetTaskInfo(SCENE_HI_SCORE_S);	/* ハイスコアランキングシーン(開始処理)タスクへ設定 */
				break;
			}
			case SCENE_NEXT_STAGE_S:
			{
				SetFlip(FALSE);			/* フリップ禁止 */
				
				/* グラフィック表示 */
				G_HOME(0);			/* ホーム位置設定 */
				G_CLR();			/* グラフィッククリア */
				G_PaletteSetZero();	/* グラフィックパレットの基本設定 */
				
				/* スプライト＆ＢＧ */
				PCG_VIEW(0x00u);	/* スプライト＆ＢＧ非表示 */

				/* テキスト */
				T_Clear();		/* テキストクリア */

				/* 動画 */
				MOV_Play(5);	/* やるじゃない */
				
				BG_TextPut("WE ARE NOW RUSHING INTO B ZONE.", 0, 128);	/* STAGE クリア文字 */
				BG_TextPut("BE ON YOUR GUARD", 60, 140				);		/* STAGE クリア文字 */
				
				Music_Play(15);	/* クリア */
				
				g_Stage = Mmin(g_Stage++, 9);	/* 次のステージへ */

				SetTaskInfo(SCENE_NEXT_STAGE);	/* 次のステージシーンへ設定 */
				break;
			}
			case SCENE_NEXT_STAGE:
			{
				if(g_Input == KEY_B)	/* Bボタン */
				{
					SetTaskInfo(SCENE_NEXT_STAGE_E);	/* 次のステージシーン(終了処理)へ設定 */
				}
				break;
			}
			case SCENE_NEXT_STAGE_E:
			{
				SetTaskInfo(SCENE_START_S);	/* ゲームシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_HI_SCORE_S:	/* ハイスコアランキングシーン(開始処理) */
			case SCENE_HI_SCORE:	/* ハイスコアランキングシーン */
			case SCENE_HI_SCORE_E:	/* ハイスコアランキングシーン(終了処理) */
			{
				SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_OPTION_S:		/* オプションシーン */
			case SCENE_OPTION:		/* オプションシーン */
			case SCENE_OPTION_E:		/* オプションシーン(終了処理) */
			{
				break;
			}
			case SCENE_EXIT:		/* 終了シーン */
			{
				Music_Stop();	/* 音楽再生 停止 */

				/* 動画 */
				MOV_Play(2);	/* バイバイ */
				
				loop = 0;	/* ループ終了 */
				break;
			}
			default:	/* 異常シーン */
			{
				loop = 0;	/* ループ終了 */
				break;
			}
		}

		/* タスク処理 */
		UpdateTaskInfo();		/* タスクの情報を更新 */

		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */
		
#ifdef DEBUG	/* デバッグコーナー */
		/* 処理時間計測 */
		GetNowTime(&time_now);
		unTime_cal = time_now - time_st;	/* LSB:1 UNIT:ms */
		g_unTime_cal = unTime_cal;
		if( stTask.bScene == SCENE_GAME )
		{
			unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);
			g_unTime_cal_PH = unTime_cal_PH;
		}
#endif

		/* 同期待ち */
		wait_vdisp(g_Vwait);

	}
	while( loop );

	g_bExit = FALSE;
	App_exit();	/* 終了処理 */
	
#ifdef DEBUG	/* デバッグコーナー */
	printf("処理時間:Real=%3d[ms] PH=%3d[ms]\n", unTime_cal, unTime_cal_PH);
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
static void App_Init(void)
{
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 開始");
#endif

	g_nMaxMemSize = MaxMemSize(1);
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init MaxMemSize");
#endif
	
	/* ゲーム内容の初期化 */
	S_All_Init_Score();	/* スコアの初期化 */
	SetDebugNum(0x80);	/* デバッグ値初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init ゲーム内容の初期化");
#endif

	/* MFP */
	g_CpuTime = TimerD_INIT();	/* タイマーD初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init MFP");
#endif

	/* リストファイルの読み込み */
	Init_FileList_Load();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init リストファイルの読み込み");
#endif
	
	/* 音楽 */
	Init_Music();	/* 初期化(スーパーバイザーモードより前)	*/
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 音楽");
#endif
	Music_Play(1);	/* ローディング中 */

	/* 動画 */
	MOV_INIT();	/* 初期化処理 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 動画");
#endif

	/* テキストクリア */
	T_Clear();	/* テキストクリア */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init テキスト");
#endif
	
	/* 画面 */
	g_nCrtmod = CRT_INIT();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 画面");
#endif
	
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

	/* テキスト */
	T_INIT();	/* テキストＶＲＡＭ初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init T_INIT");
#endif
	
	/* グラフィック */
	G_INIT();			/* 画面の初期設定 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init グラフィック");
#endif
	
	/* スプライト／ＢＧ */
	PCG_INIT();	/* スプライト／ＢＧの初期化 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init スプライト／ＢＧ");
#endif

	/* MFP */
	MFP_INIT();	/* V-Sync割り込み等の初期化処理 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init MFP");
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
	
	/* テキストクリア */
	T_Clear();	/* テキストクリア */
	
	/* スプライト＆ＢＧ */
	PCG_VIEW(0x00u);		/* スプライト＆ＢＧ非表示 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit スプライト");
#endif

	/* 画面 */
	CRTMOD(0x100 + g_nCrtmod);	/* モードをもとに戻す */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit 画面");
#endif

	/* 音楽 */
	Exit_Music();			/* 音楽停止 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit 音楽");
#endif
	
	if(g_bExit == TRUE)
	{
		puts("エラーをキャッチ！ ESC to skip");
		KeyHitESC();	/* デバッグ用 */
	}
	
	/* MFP */
	TimerD_EXIT();			/* Timer-Dの解除 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit Timer-D");
#endif

	MFP_EXIT();				/* MFP関連の解除 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit MFP");
#endif

	/* テキスト */
	T_EXIT();				/* テキスト終了処理 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit テキスト");
#endif

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
		uint32_t i=0, j=0;
		uint16_t x, y;
		int16_t pat;
		ST_RAS_INFO	stRasInfo;
		GetRasterInfo(&stRasInfo);
		
		printf("====================\n");
		printf("stRasInfo st,mid,ed,size=(%4d,%4d,%4d,%4d)\n", stRasInfo.st, stRasInfo.mid, stRasInfo.ed, stRasInfo.size);
		printf("GetRasterIntPos[i]=(x,y,pat)=(H_pos)\n");
		printf("====================\n");
		for(i=0; i < stRasInfo.size; i++)	/* 2枚分表示 */
		{
			SetRasterIntData(0);	/* 1枚目 */
			GetRasterIntPos( &x, &y, &pat, i, FALSE);
			printf("[%3d,%3d]=(%4d,%4d,%4d), ", j++, i, x, y, pat);
			SetRasterIntData(1);	/* 2枚目 */
			GetRasterIntPos( &x, &y, &pat, i, FALSE );
			printf("[%3d,%3d]=(%4d,%4d,%4d), ", j++, i, x, y, pat);
			printf("\n");
		}
		printf("==========割り込み位置の設定==========\n");
		SetRasterIntData(0);	/* 1枚目 */
		GetRasterIntPos( &x, &y, &pat, (RASTER_H_MAX - 2), TRUE);
		printf("[%3d]=(%4d,%4d,%4d), ", (RASTER_H_MAX - 2), x, y, pat);
		SetRasterIntData(1);	/* 2枚目 */
		GetRasterIntPos( &x, &y, &pat, (RASTER_H_MAX - 2), TRUE );
		printf("[%3d]=(%4d,%4d,%4d), ", (RASTER_H_MAX - 2), x, y, pat);
		printf("\n");
		printf("==========割り込みがかかるまでの表示位置==========\n");
		SetRasterIntData(0);	/* 1枚目 */
		GetRasterIntPos( &x, &y, &pat, (RASTER_H_MAX - 4), TRUE);
		printf("[%3d]=(%4d,%4d,%4d), ", (RASTER_H_MAX - 4), x, y, pat);
		SetRasterIntData(1);	/* 2枚目 */
		GetRasterIntPos( &x, &y, &pat, (RASTER_H_MAX - 4), TRUE );
		printf("[%3d]=(%4d,%4d,%4d), ", (RASTER_H_MAX - 4), x, y, pat);
		printf("\n");
		printf("====================\n");
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
int16_t BG_main(uint32_t ulTimes)
{
	int16_t	ret = 0;
	uint32_t	time_now;
	uint16_t	BGprocces_ct = 0;
	uint16_t	uCount;
	uint8_t	bMode, bMode_rev;
	uint8_t	bNum;
	uint8_t	bNumCourse_Obj_Max;
	uint8_t	bNumEnemyCar_Max;
	uint8_t	bFlipStateOld;
	ST_TASK		stTask = {0}; 
	
	static uint8_t	bFlipState = Clear_G;
	static uint16_t uTimer_old = 0xFFFFu;
	
	if(g_bFlip == TRUE)		/* 画面切り替え中 */
	{
		return ret;
	}
	
	GetTaskInfo(&stTask);	/* タスクの情報を得る */

	bFlipStateOld = bFlipState;

	if( g_CpuTime < 300 )	/* 低速MPU */
	{
		bNumCourse_Obj_Max = 2;
		bNumEnemyCar_Max = 1;
	}
	else
	{
		bNumCourse_Obj_Max = COURSE_OBJ_MAX;
		bNumEnemyCar_Max = ENEMYCAR_MAX;
	}
	
	/* モード引き渡し */
	GetGameMode(&bMode);
	bMode_rev = g_mode_rev;

	/* コースサイクルカウンタ */
	GetRoadCycleCount(&uCount);
	
	do
	{
		/* 背景の処理 */
		switch(bFlipState)
		{
			/* 描画のクリア処理 */
			case Clear_G:
			{
				/* グラフィック */
				G_CLR_ALL_OFFSC();	/* グラフィックを消去 */
				break;
			}
			/* 背景 */
			case BackGround_G:
			{
#if 1
				Move_Course_BG(bMode);	/* コースの動きにあわせて背景を動かす */
#endif
				break;
			}
			/* ヤシの木(E:右側 / O:左側) */
			case Object0_G:
			case Object1_G:
			case Object2_G:
			case Object3_G:
			case Object4_G:
			case Object5_G:
			case Object6_G:
			case Object7_G:
			{
#if 1
				bNum = bFlipState - Object0_G;
				if(bNum < bNumCourse_Obj_Max)
				{
					/* 描画順をソートする */
					if(bNum == 0)
					{
						Sort_Course_Obj();		/* コースオブジェクト */
					}
					Course_Obj_main(bNum, bMode, bMode_rev);
				}
#endif
				break;
			}
			/* ライバル車 */
			case Enemy1_G:
			case Enemy2_G:
			{
#if 1
				bNum = bFlipState - Enemy1_G;
				if(bNum < bNumEnemyCar_Max)
				{
					/* 描画順をソートする */
					if(bNum == 0)
					{
						Sort_EnemyCAR();		/* ライバル車 */
					}
					
					EnemyCAR_main(bNum, bMode, bMode_rev);
				}
#endif
				break;
			}
			/* スタート位置 */
			case StartPoint_G:
			{
				if(g_uGameStatus == 2)
				{
					Set_Course_TmpObject(uCount - s_uStartCount + 2, START_PT_CG);	/* スタートゲート */
				}
				else
				{
					s_uStartCount = uCount;
				}
				break;
			}
			/* ゴール位置 */
			case GoalPoint_G:
			{
				if(g_uGameStatus == 3)
				{
					Set_Course_TmpObject(uCount - s_uGoalCount, GOAL_PT_CG);	/* ゴールゲート */
				}
				else
				{
					s_uGoalCount = uCount;
				}
#ifdef DEBUG	/* デバッグコーナー */
				/* デバッグ時スキップタスク */
				if(stTask.bScene == SCENE_DEBUG)
				{
					Set_Course_TmpObject(g_uDebugNum - 0x80, GOAL_PT_CG);	/* ゴールゲート */
				}
#endif
				break;
			}
			/* 自車の処理 */
			case MyCar_G:
			{
				uint16_t uTimer;
				
				MyCar_Interior();		/* 自車のインテリア処理 */
				S_Main_Score();			/* スコア表示 */
				PCG_Main();				/* スプライト出力 */
				T_Main(&uTimer);		/* テキスト画面の処理 */
				if(uTimer_old != uTimer)
				{
					if((uTimer > 0) && (uTimer < 10))
					{
						int16_t PCM_num;
						PCM_num = 20 + uTimer;
						ADPCM_Play(PCM_num);	/* SE:1～9 */
					}
				}
				uTimer_old = uTimer;
				if(uTimer == 0u)
				{
					g_uGameStatus = 5;	/* タイムオーバー */
				}
				break;
			}
#ifdef DEBUG	/* デバッグコーナー */
			case Debug_View_G:
			{
				Debug_View(BGprocces_ct);	/* デバッグ情報表示 */
				break;
			}
#endif
			/* 画面をフリップする */
			case Flip_G:	
			{
				SetFlip(TRUE);	/* フリップ許可 */
				break;
			}
			default:
			{
				break;
			}
		}
		
		bFlipState++;
		
#ifdef DEBUG	/* デバッグコーナー */
		/* デバッグ時スキップタスク */
		if(stTask.bScene == SCENE_DEBUG)
		{
			if(bFlipState == Object0_G)
			{
				bFlipState = Enemy1_G;
			}
		}
#endif
		
		if(bFlipState > Flip_G)
		{
			bFlipState = 0;
			ret = 1;	/* 画面をフリップする */
		}
		BGprocces_ct++;

		GetNowTime(&time_now);	/* 現在時刻を取得 */
		
		if(ret != 0)	/* ループ脱出判定 */
		{
			break;
		}
#if 1
		else if((time_now - ulTimes) >= BG_SKIP_TIME)	/* BG_SKIP_TIME[ms]以上なら処理しない */
		{
			ret = 0;	/* 画面をフリップしない */
//			break;
		}
		else if((time_now - ulTimes) >= BG_TIME_OUT)	/* BG_TIME_OUT[ms]以上なら処理しない */
		{
			ret = 0;	/* 画面をフリップしない */
//			break;
		}
		else
		{
			/* 何もしない */
		}
#endif
	}
	while(1);
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	FlipProc(void)
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
	
	SetFlip(FALSE);			/* フリップ禁止 */
	
	/* 画面切り替え */
	G_HOME(0);
	
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
		SetGameMode(2);
	}
	
#ifdef DEBUG	/* デバッグコーナー */
	bFPS++;
	GetNowTime(&time_now);
	if( (time_now - unTime_FPS) >= 1000ul )
	{
		g_bFPS_PH = bFPS;
		unTime_FPS = time_now;
		bFPS = 0;
	}
#endif
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
void Set_DI(void)
{
	if(g_nIntCount == 0)
	{
#if 0
		/*スーパーバイザーモード終了*/
		if(g_nSuperchk > 0)
		{
			_dos_super(g_nSuperchk);
		}
#endif
		g_nIntLevel = intlevel(6);	/* 割禁設定 */
		g_nIntCount = Minc(g_nIntCount, 1);
		
#if 0
		/* スーパーバイザーモード開始 */
		g_nSuperchk = _dos_super(0);
#endif
	}
	else
	{
		g_nIntCount = Minc(g_nIntCount, 1);
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Set_EI(void)
{
	if(g_nIntCount == 1)
	{
		g_nIntCount = Mdec(g_nIntCount, 1);
		
		/* スプリアス割り込みの中の人も(以下略)より */
		/* MFPでデバイス毎の割込み禁止設定をする際には必ずSR操作で割込みレベルを上げておく。*/
		asm ("\ttst.b $E9A001\n\tnop\n");
		/*
			*8255(ｼﾞｮｲｽﾃｨｯｸ)の空読み
			nop		*直前までの命令パイプラインの同期
					*早い話、この命令終了までには
					*それ以前のバスサイクルなどが
					*完了していることが保証される。
		*/
#if 0
		/*スーパーバイザーモード終了*/
		if(g_nSuperchk > 0)
		{
			_dos_super(g_nSuperchk);
		}
#endif
		intlevel(g_nIntLevel);	/* 割禁解除 */
#if 0
		/* スーパーバイザーモード開始 */
		g_nSuperchk = _dos_super(0);
#endif
	}
	else
	{
		g_nIntCount = Mdec(g_nIntCount, 1);
	}
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
int16_t	GetDebugNum(uint16_t *uNum)
{
	int16_t	ret = 0;
	*uNum = g_uDebugNum;
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetDebugNum(uint16_t uNum)
{
	int16_t	ret = 0;
	g_uDebugNum = uNum;
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetDebugMode(uint8_t *bMode)
{
	int16_t	ret = 0;
	*bMode = g_bDebugMode;
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetDebugMode(uint8_t bMode)
{
	int16_t	ret = 0;
	g_bDebugMode = bMode;
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetDebugPos(int16_t *p_PosX, int16_t *p_PosY)
{
	int16_t	ret = 0;
	*p_PosX = g_DebugPosX;
	*p_PosY = g_DebugPosY;
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetDebugPos(int16_t PosX, int16_t PosY)
{
	int16_t	ret = 0;
	g_DebugPosX = PosX;
	g_DebugPosY = PosY;
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetDebugHis(int16_t *p_His, int16_t Num)
{
	int16_t	ret = 0;
	if(Num < 0)Num = 0;
	if(Num >= 10)Num = 9;
	*p_His = g_DebugHis[Num];
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetDebugHis(int16_t His)
{
	int16_t	ret = 0;
	int16_t	i;
	static int16_t g_DebugHisCnt = 0;
	
	if(g_DebugHisCnt >= 10)
	{
		for(i=0; i<10-1; i++)
		{
			g_DebugHis[i] = g_DebugHis[i+1];
		}
		g_DebugHis[10-1] = His;
	}
	else
	{
		g_DebugHis[g_DebugHisCnt] = His;
		g_DebugHisCnt++;
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
void Debug_View(uint16_t uFreeRunCount)
{
	uint8_t bMode, bMode_rev;
	
	/* モード引き渡し */
	bMode = g_mode;
	bMode_rev = g_mode_rev;
	
	if(g_bDebugMode == TRUE)
	{
		uint8_t	str[256] = {0};
		
		int16_t	col;
		int16_t	view_offset_x, view_offset_y;
		int16_t	hide_offset_x, hide_offset_y;
		int16_t	BG_offset_x, BG_offset_y;
		
		static uint8_t ubDispNum = DEBUG_FREE;
		static uint8_t ubDispNum_flag = 0;

		ST_CRT	stCRT = {0};
		
		/* モード切替による設定値の変更 */
		GetCRT(&stCRT, bMode);
		view_offset_x	= stCRT.view_offset_x;
		view_offset_y	= stCRT.view_offset_y;
		hide_offset_x	= stCRT.hide_offset_x;
		hide_offset_y	= stCRT.hide_offset_y;
		BG_offset_x		= stCRT.BG_offset_x;
		BG_offset_y		= stCRT.BG_offset_y;
		

		if(ChatCancelSW((g_Input & KEY_b_RLUP)!=0u, &ubDispNum_flag) == TRUE)	/* ロールアップで表示切替 */
		{
			ubDispNum++;
			if(DEBUG_MAX <= ubDispNum)
			{
				ubDispNum = DEBUG_NONE;
			}
		}

		{
			switch(ubDispNum)
			{
			case DEBUG_NONE:
				{
					/* 非表示 */
				}
				break;
			case DEBUG_COURSE_OBJ:
				{
#if 1	/* 障害物情報 */
					uint32_t	i = 0;
					ST_COURSE_OBJ	stCourse_Obj = {0};
					i = Mmin(Mmax(g_uDebugNum - 0x80, 0), COURSE_OBJ_MAX-1);
					GetCourseObj(&stCourse_Obj, i);	/* 障害物の情報 */
					sprintf(str, "C_Obj[%d](%4d,%3d,%d)(%d),Debug(%3d)",
						i, stCourse_Obj.x, stCourse_Obj.y, stCourse_Obj.z, stCourse_Obj.ubAlive,
						g_uDebugNum);	/* 障害物の情報 */
#endif
				}
				break;
			case DEBUG_ENEMYCAR:
				{
#if 1	/* 敵車情報 */
					uint32_t	i = 0;
					ST_ENEMYCARDATA	stEnemyCar = {0};
					i = Mmin(Mmax(uFreeRunCount % ENEMYCAR_MAX, 0), ENEMYCAR_MAX-1);
					GetEnemyCAR(&stEnemyCar, i);	/* ライバル車の情報 */
					sprintf(str, "E[%d] (%d)(%4d,%4d,%4d),spd(%3d)", i,
						stEnemyCar.ubAlive,
						stEnemyCar.x,
						stEnemyCar.y,
						stEnemyCar.z,
						stEnemyCar.VehicleSpeed
					);
					/* ライバル車の当たり判定 */
					Draw_Box(	stCRT.hide_offset_x + stEnemyCar.sx,
								stCRT.hide_offset_y + stEnemyCar.sy,
								stCRT.hide_offset_x + stEnemyCar.ex,
								stCRT.hide_offset_y + stEnemyCar.ey, 0x03, 0xFFFF);
#endif
				}
				break;
			case DEBUG_MYCAR:
				{
#if 1	/* 自車情報 */
					int16_t speed;
					int16_t	myCarSx, myCarEx, myCarSy, myCarEy;
					
					ST_CARDATA	stMyCar;
					GetMyCar(&stMyCar);	/* 自車 */
					GetMyCarSpeed(&speed);
					
					/* クルマの向き */
					col = 0x5;
					Draw_Line(	hide_offset_x + Mdiv2(WIDTH)  + Mdiv256(64 * APL_Cos(stMyCar.Angle - 90)),
								hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + Mdiv256(64 * APL_Sin(stMyCar.Angle - 90)),
								hide_offset_x + Mdiv2(WIDTH)  + 1, 
								hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + 1,
								col,
								0xFFFF);
					
					/* 道の向き */
					col = 0xB;
					Draw_Line(	hide_offset_x + Mdiv2(WIDTH)  + Mdiv256(64 * APL_Cos(g_stRoadInfo.angle - 90)),
								hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + Mdiv256(64 * APL_Sin(g_stRoadInfo.angle - 90)),
								hide_offset_x + Mdiv2(WIDTH)  + 1, 
								hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + 1,
								col,
								0xFFFF);
					
					switch(stMyCar.ubOBD)
					{
						case OBD_NORMAL:
						{
							col = 0x0B;
							break;
						}
						case OBD_DAMAGE:
						{
							col = 0x0A;
							break;
						}
						case OBD_SPIN_L:
						case OBD_SPIN_R:
						{
							col = 0x0C;
							break;
						}
						case OBD_COURSEOUT:
						{
							col = 0x0E;
							break;
						}
						default:
						{
							col = 0x00;
							break;
						}
					}
					
					/* クルマの位置 */
					myCarSx = ROAD_CT_POINT + APL_AngleDiff(g_stRoadInfo.angle, stMyCar.Angle) - 8;	/* 車と道路の角度差分で車の位置が変わる */
					myCarEx = myCarSx + 16;
					myCarSy = Y_MAX_WINDOW - 32;
					myCarEy = myCarSy + 16;
					
					Draw_Box(
						stCRT.hide_offset_x + myCarSx,
						stCRT.hide_offset_y + myCarSy,
						stCRT.hide_offset_x + myCarEx,
						stCRT.hide_offset_y + myCarEy, col, 0xFFFF);
						
					sprintf(str, "C[%d](%4d,%d,%5d,%3d,%3d,%d,%d,%4d,%4d,%d,%d)",
							stMyCar.ubCarType,			/* 車の種類 */
							stMyCar.uEngineRPM,			/* エンジン回転数 */
							stMyCar.ubShiftPos,			/* ギア段 */
							stMyCar.Steering,			/* ステア */
							stMyCar.Angle,				/* 向き */
							stMyCar.ubThrottle,			/* スロットル開度 */
							stMyCar.ubBrakeLights,		/* ブレーキライト */
							stMyCar.ubHeadLights,		/* ヘッドライト */
							stMyCar.ubWiper,			/* ワイパー *//* （仮）マップトルク */
							stMyCar.bTire,				/* タイヤの状態 *//* （仮）トルク */
							stMyCar.ubOBD,				/* 故障の状態 */
							speed						/* 車速 */
					);	/* 自車の情報 */
#endif
				}
				break;
			case DEBUG_ROAD:
				{
#if 1	/* 道路情報 */
					/* 道の向き */
					col = 0xB;
					Draw_Line(	hide_offset_x + Mdiv2(WIDTH)  + Mdiv256(64 * APL_Cos(g_stRoadInfo.angle - 90)),
								hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + Mdiv256(64 * APL_Sin(g_stRoadInfo.angle - 90)),
								hide_offset_x + Mdiv2(WIDTH)  + 1, 
								hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + 1,
								col,
								0xFFFF);
					
					sprintf(str, "R=h%3d,s%3d,a%3d,d%3d,o%3d,C%3d,H%3d,Hb%3d,ox%3d,oy%3d,ov%3d",
						g_stRoadInfo.height,		/* コースの高さ */
						g_stRoadInfo.slope,			/* コースの曲がり具合 */
						g_stRoadInfo.angle,			/* コースの方向（角度） */
						g_stRoadInfo.distance,		/* コースの距離？ */
						g_stRoadInfo.object,		/* コースのオブジェクト */
						g_stRoadInfo.Courselength,	/* コースの全長 */
						g_stRoadInfo.Horizon,		/* 水平位置 */
						g_stRoadInfo.Horizon_Base,	/* 水平基準位置 */
						g_stRoadInfo.offset_x,		/* コースのパターンX座標？ */
						g_stRoadInfo.offset_y,		/* コースのパターンY座標 */
						g_stRoadInfo.offset_val		/* コースのパターンY座標更新値 */
					);
#endif
				}
				break;
			case DEBUG_RASTER:
				{

#if 1	/* ラスター情報 */
					uint16_t x, y;
					int16_t pat;
					int16_t pos;
					int16_t count, linCount;
					ST_RAS_INFO	stRasInfo;
					GetRasterInfo(&stRasInfo);

					{
						/* ラスター開始位置 */
						col = 0x3;
						Draw_Line(	hide_offset_x + 0,
									hide_offset_y + stRasInfo.st,
									hide_offset_x + WIDTH, 
									hide_offset_y + stRasInfo.st,
									col,
									0xFFFF);
						/* ラスター中間位置 */
						col = 0xF;
						Draw_Line(	hide_offset_x + 0,
									hide_offset_y + stRasInfo.mid,
									hide_offset_x + WIDTH, 
									hide_offset_y + stRasInfo.mid,
									col,
									0xFFFF);
						/* ラスター終了位置 */
						col = 0xB;
						Draw_Line(	hide_offset_x + 0,
									hide_offset_y + stRasInfo.ed,
									hide_offset_x + WIDTH, 
									hide_offset_y + stRasInfo.ed,
									col,
									0xFFFF);

#if 0						
						/* 視界差分 */
						col = 0x4;
						Draw_Line(	hide_offset_x + Mdiv2(WIDTH)  + Mdiv256(64 * APL_Cos(ViewAngle - 90)),
									hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + Mdiv256(64 * APL_Sin(ViewAngle - 90)),
									hide_offset_x + Mdiv2(WIDTH)  + 1, 
									hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + 1,
									col,
									0xFFFF);
#endif
						/* 曲がり具合 */
						col = 0x3;
						Draw_Circle(hide_offset_x + Mdiv2(WIDTH) + 1, 
									hide_offset_y + Mdiv4(HEIGHT) + Mdiv8(HEIGHT) + 1,
									Mmax(Mdiv4(g_stRoadInfo.slope) + 1, 8),
									col,
									0, 360, 256);
#if 0						
						/* x */
						col = 0xE;
						/* センター */
						if( ras_cal_x < 256 )	/* 左カーブ */
						{
							point_x = ROAD_CT_POINT + (  0 - ras_cal_x);
						}
						else	/* 右カーブ */
						{
							point_x = ROAD_CT_POINT + (512 - ras_cal_x);
						}
						Draw_Pset(	hide_offset_x + point_x,
									hide_offset_y + g_stRoadInfo.Horizon + uRas_y,
									col);

						/* y */
						col = 0x0C;
						Draw_Pset(	hide_offset_x + uRas_y + 32,
									hide_offset_y + Y_MAX_WINDOW - ras_cal_y,
									col);
#endif
					}
					
					pos = Mmax(Mmin( g_uDebugNum, stRasInfo.size ), 0);
					GetRasterIntPos( &x, &y, &pat, pos, FALSE );
					count = GetRasterCount(&linCount);
					
					sprintf(str, "Ras(%d)s(%d,%d,%d,%d)i(%3d,%3d)[%d]",
						pos,
						stRasInfo.st, stRasInfo.mid, stRasInfo.ed, stRasInfo.size,
						count, linCount,
						bMode
						//x, y, pat
					);	/* ラスター情報 */
#endif
				}
				break;
			case DEBUG_INPUT:
				{
#if 1	/* 入力情報 */
					int16_t	AnalogMode = 0;
					JOY_ANALOG_BUF stAnalog_Info;

					AnalogMode = GetAnalog_Info(&stAnalog_Info);	/* アナログ情報取得 */
					
					if(AnalogMode == 0)	/* アナログモード */
					{
						sprintf(str,"R(0x%02x 0x%02x)L(0x%02x 0x%02x)B(%04b|%04b|%04b)",
							stAnalog_Info.r_stk_ud,
							stAnalog_Info.r_stk_lr,
							stAnalog_Info.l_stk_ud,
							stAnalog_Info.l_stk_lr,
							(stAnalog_Info.btn_data & 0xF00) >> 8,
							(stAnalog_Info.btn_data & 0x0F0) >> 4,
							(stAnalog_Info.btn_data & 0x00F)
						);
					}
					else
					{
						sprintf(str,"U(%d)D(%d)L(%d)R(%d)B(%d)A(%d)",
							g_Input & KEY_UPPER,
							g_Input & KEY_LOWER,
							g_Input & KEY_LEFT,
							g_Input & KEY_RIGHT,
							g_Input & KEY_B,
							g_Input & KEY_A
						);
					}
#endif
				}
				break;
			case DEBUG_CPUTIME:
				{
#if 1	/* CPU情報 */
//							sprintf(str, "CPU Time%2d[ms](MAX%2d[ms]),Debug(%3d)", g_unTime_cal, g_unTime_cal_PH, g_uDebugNum);	/* 処理負荷 */
					sprintf(str, "%d T%2d(Max%2d),%2d,%2d,%2d,%2d,%2d F(%2d)", g_CpuTime, g_unTime_cal, g_unTime_cal_PH, 
							g_unTime_Pass[BackGround_G],
							g_unTime_Pass[Object0_G],
							g_unTime_Pass[Enemy1_G],
							g_unTime_Pass[MyCar_G],
							g_unTime_Pass[Debug_View_G],
							g_bFPS_PH
					);
#endif
				}
				break;
			case DEBUG_MEMORY:
				{
#if 1	/* メモリ情報 */
					int d;
					d = (int)_dos_malloc(-1);
					sprintf(str, "Use %d/%d[KB] Free %d[KB]", g_nMaxMemSize - Mdiv1024(d-0x81000000), g_nMaxMemSize, Mdiv1024(d-0x81000000));
#endif
				}
				break;
			case DEBUG_FREE:
				{
					int16_t	i;
					int16_t	dim[10];
					int16_t	x, y;
#if 0	/* 何でもOK */
					int16_t x, y, col;
					x = X_MIN_DRAW + X_OFFSET + (uFreeRunCount & 0xFF);
					y = ((Y_MIN_DRAW + Y_OFFSET) * (bMode - 1)) + 120 + g_nIntCount;
						col = 4;
						col = 5;
					
					/* フリップ処理確認 */
					Draw_Pset( x, y, col);
#endif
#if 0
					{
						int16_t x, y, col;
						x = X_MIN_DRAW + X_OFFSET + (uFreeRunCount & 0xFF);
						y = ((Y_MIN_DRAW + Y_OFFSET) * (bMode - 1)) + 120 - Mmul8(Raster) - Mmul32(g_bFlip);
						if(bMode == 1)
						{
							col = Mdiv256(uFreeRunCount & 0x300) + 1;
						}
						else
						{
							col = 5;
						}
						/* フリップ処理確認 */
						Draw_Pset( x, y, col );
					}
#endif
#if 0
			//			sprintf(str, "Ras %3d, %3d, %3d, V %3d", min, Raster_count, max);
					
			//			sprintf(str, "R=%3d,V=%3d,D=%3d,L=%3d,%3d,%3d,%3d,%3d",
				
					sprintf(str, "V=%3d,L=%3d,R=%4d,%4d,%4d,%4d,%4d,%4d,%4d,%4d",
						(Vsync_count & 0xFF),
						(Raster_count & 0xFF),
						g_uRasterLine[0],
						g_uRasterLine[1],
						g_uRasterLine[2],
						g_uRasterLine[3],
						g_uRasterLine[4],
						g_uRasterLine[5],
						g_uRasterLine[6],
						g_uRasterLine[7]
					);
#endif
#if 0
					sprintf(str, "T=%2d x 20[us](%2d)", g_unTime_cal, g_unTime_cal_PH);
#endif
#if 0
					int16_t	ret_a, ret_b;
					static int16_t	vx, vy;
					
					src_w = 64;
					src_h = 64;
					
					if((g_Input & KEY_UPPER) != 0u)	/* 上 */
					{
						vy--;
					}
					else
					{
					}
					
					if((g_Input & KEY_LOWER) != 0u)	/* 下 */
					{
						vy++;
					}
					else
					{
					}
					dst_y = hide_offset_y + vy;

					if((g_Input & KEY_LEFT) != 0u)	/* 左 */
					{
						vx--;
						if(bFlag == FALSE)
						{
						}
						else
						{
						}

					}
					else
					{
					}
					
					if((g_Input & KEY_RIGHT) != 0u)	/* 右 */
					{
						vx++;
						
						if(bFlag == FALSE)
						{
						}
						else
						{
						}
					}
					else
					{
					}
					dst_x = hide_offset_x + vx;
					
					if((g_Input & KEY_A) != 0u)	/* Aボタン */
					{
						KeyHitESC();	/* デバッグ用 */
					}
					else
					{
					}
					
					if((g_Input & KEY_B) != 0u)	/* Bボタン */
					{
						SetTaskInfo(SCENE_DEBUG_E);	/* デバッグ(終了処理)へ設定 */
					}
					else
					{
					}
					/* 描画したいエリアから完全にはみ出てるかどうかチェック */
					ret_a = G_ClipAREA_Chk_Width(&dst_x, &x_min, &x_max, src_w, bMode_rev, POS_RIGHT);
					
					ret_b = G_ClipAREA_Chk_Height(&dst_y, &y_min, &y_max, src_h, bMode_rev, POS_CENTER);
				
					Draw_Box(dst_x, dst_y, dst_x + src_w, dst_y + src_h, 0x03u, 0xFFFF);	/* 四角全体 */
					
					sprintf(str, "(%3d,%3d)(%3d,%3d)(%3d,%3d,%3d,%3d)(%d,%d)", vx, vy, dst_x, dst_y, x_min, y_min, x_max, y_max, ret_a, ret_b);
#endif
#if 0	/* デバッグ表示 */
					if((g_Input & KEY_UPPER)!=0u)dy = Mdec(dy, 1);
					if((g_Input & KEY_LOWER)!=0u)dy = Minc(dy, 1);
#endif
#ifdef DEBUG	/* デバッグコーナー */
//					GetDebugPos(&x, &y);
//					sprintf(str, "(%4d, %4d)", x, y);
#endif
#ifdef DEBUG	/* デバッグコーナー */
					for(i=0; i<10; i++)
					{
						GetDebugHis(&dim[i], i);
					}
					sprintf(str, "(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
						dim[0], dim[1], dim[2], dim[3], dim[4],
						dim[5], dim[6], dim[7], dim[8], dim[9]);
#endif
				}
				break;
			default:
				break;
			}
			/* 表示 */
			Put_Message_To_Graphic(str, bMode);	/* グラフィックのデバッグエリアにメッセージ描画 */
		}
	}
}

#endif	/* OVERKATA_C */
