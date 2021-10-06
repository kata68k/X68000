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

#define	BG_SKIP_TIME	(8u)
#define	BG_TIME_OUT		(56u)

/* グローバル変数 */
int32_t	g_nSuperchk = 0;
int32_t	g_nIntLevel = 0;
int32_t	g_nIntCount = 0;
int32_t	g_nCrtmod = 0;
int32_t	g_nMaxMemSize;
int16_t	g_CpuTime = 0;
int16_t	g_Input;
uint8_t	g_mode = 1;
uint8_t	g_mode_rev = 2;
uint8_t	g_Vwait = 1;
uint32_t	g_unTime_cal = 0u;
uint32_t	g_unTime_cal_PH = 0u;
uint8_t	g_bFlip = FALSE;
uint8_t	g_bFlip_old = FALSE;
uint8_t	g_bExit = TRUE;

#ifdef DEBUG	/* デバッグコーナー */
uint8_t		g_bDebugMode = FALSE;
uint8_t		g_bFPS_PH = 0u;
uint16_t	g_uDebugNum = 0; 
uint32_t	g_unTime_Pass[MAX_G] = {0u};
#endif

/* enum */
#ifdef DEBUG	/* デバッグコーナー */
enum{
	DEBUG_NONE,
	DEBUG_COURSE_OBJ,
	DEBUG_ENEMYCAR,
	DEBUG_MYCAR,
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
int16_t main(void);
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
int16_t main(void)
{
	int16_t	ret = 0;

	uint32_t	unTime_cal = 0u;
	uint32_t	unTime_cal_PH = 0u;
	uint32_t	unDemo_time = 0xFFFFFFFFu;
	int16_t		DemoCount = 0;

	uint16_t	uFreeRunCount=0u;

	int16_t	loop = 1;
	
#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode = TRUE;
	uint8_t	bDebugMode_flag;
#endif
	
	uint8_t	bAnalogStickMode = FALSE;
	uint8_t	bAnalogStickMode_flag;
	
	uint8_t	bCRTMode = TRUE;
	uint8_t	bCRTMode_flag;

	uint8_t	bBG = TRUE;
	uint8_t	bBG_flag;
	
	uint8_t	bMode;
	
	ST_TASK		stTask = {0}; 
	
	/* 変数の初期化 */
	g_mode = 1;
	g_mode_rev = 2u;
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

	/* デバッグコーナー */
#if 0
	puts("デバッグコーナー 開始");
	/* ↓自由にコードを書いてね */
	{
		uint8_t bFlag = FALSE;
		uint8_t bHit = FALSE;
		int32_t	count = 0, count_old = 0;
		
		/* リストファイルの読み込み */
		Init_FileList_Load();
		
		/* スーパーバイザーモード開始 */
		g_nSuperchk = _dos_super(0);
		/* 画面 */
		g_nCrtmod = CRT_INIT();
		/* グラフィック */
		G_INIT();			/* 画面の初期設定 */
		G_Palette_INIT();	/* グラフィックパレットの初期化 */
		/* テキスト */
		T_INIT();			/* テキストＶＲＡＭ初期化 */
		T_PALET();			/* テキストパレット設定 */
		/* スプライト／ＢＧ */
		PCG_INIT();			/* スプライト／ＢＧの初期化 */
		PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */

		loop = 1;
		do
		{
			int16_t	input = 0;

			get_keyboard(&input, 0, 1);		/* キーボード入力 */
	
			if((input & KEY_b_ESC ) != 0u)		/* ＥＳＣキー */
			{
				loop = 0;	/* ループ終了 */
			}
			
			if(ChatCancelSW((input & KEY_A)!=0u, &bFlag) == TRUE)	/* Aボタン */
			{
				bHit = TRUE;
			}
			else
			{
				bHit = FALSE;
			}
			
			
			if((input & KEY_B) != 0u)	/* Bボタン */
			{
				bHit = TRUE;
			}
			else
			{
			}

			if((input & KEY_UPPER) != 0u)
			{
			}
			else if((input & KEY_LOWER) != 0u)
			{
			}
			else
			{
			}
			
			if((input & KEY_LEFT) != 0u)	
			{
			}
			else if((input & KEY_RIGHT) != 0u)
			{
			}
			else
			{
			}
			
//			printf("p_stPCG[%d]=0x%p\n", i, p_stPCG);
//			KeyHitESC();	/* デバッグ用 */

			if(loop == 0)break;

			/* 同期待ち */
			vwait(1);
		}
		while( loop );

		/* 画面 */
		CRTMOD(g_nCrtmod);		/* モードをもとに戻す */
		
		/*スーパーバイザーモード終了*/
		_dos_super(g_nSuperchk);
		
		_dos_kflushio(0xFF);	/* キーバッファをクリア */
	}
	/* ↑自由にコードを書いてね */
	puts("デバッグコーナー 終了");
	
	exit(0);
#endif
	
	App_Init();	/* 初期化 */

	SetTaskInfo(SCENE_INIT);	/* 初期化シーンへ設定 */
	
	/* 乱数 */
	srandom(TIMEGET());	/* 乱数の初期化 */
	
	do	/* メインループ処理 */
	{
		uint32_t time_st, time_now;
		int16_t	input = 0;
#ifdef DEBUG	/* デバッグコーナー */
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
		bMode = g_mode;

		/* 入力処理 */
		get_keyboard(&input, 0, 1);		/* キーボード入力 */
		if(bAnalogStickMode == TRUE)
		{
			get_ajoy(&input, 0, 1, 1);	/* アナログジョイスティック入力 0:X680x0 1:etc */
		}
		else
		{
			get_djoy(&input, 0, 1);		/* ジョイスティック入力 */
		}
		g_Input = input;
		
		/* 終了 */
		if((input & KEY_b_ESC ) != 0u)	/* ＥＳＣキー */
		{
			SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
		}
		/* アナログスティック／デジタルスティック切替 */
		if(ChatCancelSW((input & KEY_b_TAB)!=0u, &bAnalogStickMode_flag) == TRUE)	/* TABでアナログスティックON/OFF */
		{
			if(bAnalogStickMode == FALSE)	bAnalogStickMode = TRUE;
			else							bAnalogStickMode = FALSE;
		}
		/* CRT 31kHz/15kHz切替 */
		if(ChatCancelSW((input & KEY_b_HELP)!=0u, &bCRTMode_flag) == TRUE)	/* HELPで31kHz/15kHz切替 */
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
		/* BG */
		if(ChatCancelSW((input & KEY_b_G)!=0u, &bBG_flag) == TRUE)	/* デバッグ検証用 */
		{
			if(bBG == FALSE)	bBG = TRUE;
			else				bBG = FALSE;
		}
		
#ifdef DEBUG	/* デバッグコーナー */
		if(ChatCancelSW((input & KEY_b_SP)!=0u, &bDebugMode_flag) == TRUE)	/* スペースでデバッグON/OFF */
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
				g_mode = 1;
				g_mode_rev = 2u;
#ifdef DEBUG	/* デバッグコーナー */
				g_uDebugNum = 0x80;
#endif
				uFreeRunCount = 0;
				
				/* グラフィック */
				G_HOME();			/* グラフィック標準位置 */
				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */
				/* テキスト */
				T_INIT();			/* テキストＶＲＡＭ初期化 */
				T_PALET();			/* テキストパレット設定 */

				SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_TITLE_S:	/* タイトルシーン(開始処理) */
			{
				Music_Play(2);	/* タイトル曲 */
				
				if(CG_File_Load(TITLE_CG) >= 0)	/* グラフィックの読み込み */
				{
					G_Load_Mem( TITLE_CG, X_OFFSET, Y_OFFSET, 0 );	/* タイトル画像 */
				}
			
				BG_TextPut("OVER KATA", 96, 128);		/* タイトル文字 */
				BG_TextPut("PUSH A BUTTON", 80, 160);	/* ボタン押してね */

				GetPassTime( 100, &unDemo_time );		/* 初期化 */

				SetTaskInfo(SCENE_TITLE);	/* タイトルシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_TITLE:	/* タイトルシーン */
			{
				if(input == KEY_A)	/* Aボタン */
				{
					Music_Stop();	/* 音楽再生 停止 */
					
					ADPCM_Play(10);	/* SE:決定 */
				
					SetTaskInfo(SCENE_TITLE_E);	/* タイトルシーン(開始処理)へ設定 */
				}
				
				if( GetPassTime( 30000, &unDemo_time ) != 0u )	/* 所定時間何もなければデモシーンへ */
				{
					Music_Stop();	/* 音楽再生 停止 */
					
//					SetTaskInfo(SCENE_DEMO_S);	/* デモシーン(開始処理)へ設定 */
				}
				break;
			}
			case SCENE_TITLE_E:	/* タイトルシーン(終了処理) */
			{
				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */

				/* テキスト表示 */
				T_INIT();		/* テキストクリア */

				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */

				/* MFP */
				MFP_INIT();	/* 初期化処理 */
				
				SetTaskInfo(SCENE_START_S);	/* ゲーム開始シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_DEMO_S:	/* デモシーン(開始処理) */
			{
				uint16_t demo;
				DemoCount = 0;
				
				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */

				/* テキスト表示 */
				T_INIT();		/* テキストクリア */

				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */
				
				Music_Play(6);	/* デモ曲 */
				
				for(demo=0; demo<6; demo++)
				{
					CG_File_Load(DEMO_CG + demo);		/* グラフィックの読み込み */
				}

				SetTaskInfo(SCENE_DEMO);	/* ゲーム開始シーン(開始処理)へ設定 */
				break;
			}
			case SCENE_DEMO:	/* デモシーン */
			{
				if( GetPassTime( 2000, &unDemo_time ) != 0u )	/* 所定時間何もなければ実行 */
				{
					if( DemoCount < 6u )	/* デモシーケンス満了 */
					{
						G_Load_Mem( DEMO_CG + DemoCount, X_OFFSET, Y_OFFSET+32, 0 );	/* デモ画像 */
					}
					DemoCount++;	/* 次の画像 */
				}
				
				if( (DemoCount >= 10u) || /* デモシーケンス満了 */
					(input == KEY_A) )	/* Aボタン */
				{
					Music_Stop();	/* 音楽再生 停止 */
				
					SetTaskInfo(SCENE_DEMO_E);	/* デモシーン(終了処理)へ設定 */
				}
				break;
			}
			case SCENE_DEMO_E:	/* デモシーン(終了処理) */
			{
				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */

				/* グラフィック表示 */
				G_CLR();		/* グラフィッククリア */
				
				/* テキスト表示 */
				T_INIT();		/* テキストクリア */

				SetTaskInfo(SCENE_TITLE_S);	/* ゲームスタートタスクへ設定 */
				break;
			}
			case SCENE_START_S:	/* ゲーム開始シーン(開始処理) */
			{
				/* 動画 */
				MOV_Play(0);	/* スタート */

				//Music_Play(10);	/* TRUTH */
				Music_Play(11);	/* Mach-1 */
				
				/* ラスター情報の初期化 */
				Raster_Init();
				
				/* コースデータの初期化 */
				Road_Init(1);
				
				G_PaletteSetZero();			/* グラフィックのパレット初期化 */
				
				SetTaskInfo(SCENE_START);	/* ゲームスタートタスクへ設定 */
				break;
			}
			case SCENE_START:	/* ゲーム開始シーン */
			{
				/* コースを描画する */
				if(input == KEY_B)	/* Bボタン */
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
					Music_Play(12);	/* Mach-2 */
					SetTaskInfo(SCENE_START_E);	/* ゲームスタートタスクへ設定 */
				}
				break;
			}
			case SCENE_START_E:	/* ゲーム開始シーン(終了処理) */
			{
//				Music_Play(1);	/* ローディング中 */

				Set_CRT_Contrast(0);	/* コントラスト暗 */
				
				/* ゲーム内容の初期化 */
				S_Init_Score();	/* スコアの初期化 */
				
				/* スプライト＆ＢＧ表示 */
				PCG_INIT();		/* スプライト／ＢＧの初期化 */
				PCG_VIEW(TRUE);	/* スプライト＆ＢＧ表示 */

				/* テキスト表示 */
				T_Clear();			/* テキストクリア */
				MyCar_G_Load();		/* 自車の画像読み込み */
				MyCarInfo_Init();	/* 自車の情報初期化 */
				
				/* ライバル車の初期化 */
				InitEnemyCAR();

				/* コースのオブジェクトの初期化 */
				InitCourseObj();
				
				/* コースの背景 */
				Road_BG_Init(1);
				
				/* テキスト表示 */
				T_PALET();		/* テキストパレット設定 */
				T_TopScore();	/* TOP */
				T_Time();		/* TIME */
				T_Score();		/* SCORE */
				T_Speed();		/* SPEED */
				T_Gear();		/* GEAR */
				T_SetBG_to_Text();	/* テキスト用作業用データ展開 */
				
				SetTaskInfo(SCENE_GAME_S);	/* ゲーム(開始処理)タスクへ設定 */
				break;
			}
			case SCENE_GAME_S:	/* ゲームシーン開始処理 */
			{
//				Music_Play(3);	/* メインBGM */
//				Music_Play(13);	/* Mach-3 */
				Music_Stop();	/* 音楽再生 停止 */
				M_SetMusic(0);	/* 効果音再生の設定 */

				Set_CRT_Contrast(-1);	/* コントラストdef */
				
				SetTaskInfo(SCENE_GAME);	/* ゲームタスクへ設定 */
				break;
			}
			case SCENE_GAME:	/* ゲームシーン */
			{
				int16_t	Torque = 0;
				int16_t	Raster = 0;

				if((input & KEY_b_Q) != 0u)	/* Ｑ */
				{
					SetTaskInfo(SCENE_GAME_E);	/* ゲームシーン(終了処理)へ設定 */
				}
				
				if( (g_bFlip_old == TRUE) && (g_bFlip == FALSE) )	/* 切り替え直後判定 */
				{
					if(bBG == FALSE)
					{
						/* 自車の情報を取得 */
						Torque = MyCarInfo_Update(input);	/* 自車の情報を更新 */
#ifdef DEBUG	/* デバッグコーナー */
						if(g_bDebugMode == TRUE)
						{
							GetNowTime(&time_now);
							g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 2 */
							bTimePass++;
							g_unTime_Pass[0] = time_now;	/* 一時保存 */
						}
#endif
						MyCarInfo_Update16ms(Torque);

#ifdef DEBUG	/* デバッグコーナー */
						if(g_bDebugMode == TRUE)
						{
							GetNowTime(&time_now);
							g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 3 */
							bTimePass++;
							g_unTime_Pass[0] = time_now;	/* 一時保存 */
						}
#endif
						/* ラスター処理 */
						Raster = Raster_Main(bMode);	/* コースの処理 */

#ifdef DEBUG	/* デバッグコーナー */
						if(g_bDebugMode == TRUE)
						{
							GetNowTime(&time_now);
							g_unTime_Pass[bTimePass] = Mmax(time_now - g_unTime_Pass[0], g_unTime_Pass[bTimePass]);	/* 4 */
							bTimePass++;
							g_unTime_Pass[0] = time_now;	/* 一時保存 */
						}
#endif
					}
					else
					{
						/* ラスター処理 */
						Raster = Raster_Main(bMode);	/* コースの処理 */
					}
				}
				

				/* 余った時間で処理 */
				if(bBG == FALSE)
				{
					BG_main(time_st);	/* バックグランド処理 */
					
					//g_Vwait = 1;	/* Wait */
					g_Vwait = 0;	/* No Wait */
				}
				else
				{
					/* BG_mainの一部 */
					G_CLR_ALL_OFFSC(bMode);	/* グラフィックを消去 */

#ifdef DEBUG	/* デバッグコーナー */
					Debug_View(uFreeRunCount);	/* デバッグ情報表示 */
#endif
					//					Set_DI();	/* 割り込み禁止設定 */
					
					SetFlip(TRUE);	/* フリップ許可 */

//					Set_EI();	/* 割り込み禁止解除 */
					
					if(input == KEY_B)	/* Bボタン */
					{
						g_Vwait = 1;	/* Wait 1 */
					}
					else
					{
						g_Vwait = 0;	/* No Wait */
					}
				}
				
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
				Music_Stop();	/* 音楽再生 停止 */

				/* 動画 */
				MOV_Play(1);	/* うふふ */

				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */

				/* テキスト */
				T_Clear();		/* テキストクリア */
				
				/* MFP */
				MFP_EXIT();		/* MFP関連の解除 */
				
				SetTaskInfo(SCENE_INIT);	/* 初期化シーンへ設定 */
				break;
			}
			case SCENE_GAME_OVER:	/* ゲームオーバーシーン */
			{
				break;
			}
			case SCENE_HI_SCORE:	/* ハイスコアランキングシーン */
			{
				break;
			}
			case SCENE_OPTION:		/* オプションシーン */
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
	{
		uint32_t i=0, j=4;
		ST_RAS_INFO	stRasInfo;
		GetRasterInfo(&stRasInfo);
		
		printf("====================\n");
		printf("stRasInfo st,mid,ed,size=(%4d,%4d,%4d,%4d)\n", stRasInfo.st, stRasInfo.mid, stRasInfo.ed, stRasInfo.size);
		printf("GetRasterIntPos[i]=(x,y,pat)=(H_pos)\n");
		printf("====================\n");
		for(i=0; i < stRasInfo.size; i++)	/* 2枚分表示 */
		{
			uint16_t x, y;
			int16_t pat;
			
			SetRasterIntData(0);	/* 1枚目 */
			GetRasterIntPos( &x, &y, &pat, i );
			printf("[%3d,%3d]=(%4d,%4d,%4d), ", j++, i, x, y, pat);
			SetRasterIntData(1);	/* 2枚目 */
			GetRasterIntPos( &x, &y, &pat, i );
			printf("[%3d,%3d]=(%4d,%4d,%4d), ", j++, i, x, y, pat);
			printf("\n");
		}
		printf("====================\n");
	}
#endif
	printf("処理時間:Real=%3d[ms] PH=%3d[ms]\n", unTime_cal, unTime_cal_PH);
	
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
	puts("App_Init 開始");

	g_nMaxMemSize = MaxMemSize(1);
	
	/* ゲーム内容の初期化 */
	S_All_Init_Score();	/* スコアの初期化 */
	SetDebugNum(0x80);	/* デバッグ値初期化 */

	/* MFP */
	g_CpuTime = TimerD_INIT();	/* タイマーD初期化 */

	/* リストファイルの読み込み */
	Init_FileList_Load();
	
	/* 音楽 */
	Init_Music();	/* 初期化(スーパーバイザーモードより前)	*/
	Music_Play(1);	/* ローディング中 */

	/* 動画 */
	MOV_INIT();	/* 初期化処理 */

	/* 画面 */
	g_nCrtmod = CRT_INIT();
	
	/* スーパーバイザーモード開始 */
	g_nSuperchk = _dos_super(0);
	if( g_nSuperchk < 0 ) {
		puts("App_Init すでにスーパーバイザーモード");
	} else {
		puts("App_Init スーパーバイザーモード開始");
	}

	/* テキスト */
	T_INIT();	/* テキストＶＲＡＭ初期化 */
	
	/* グラフィック */
	G_INIT();			/* 画面の初期設定 */
	G_Palette_INIT();	/* グラフィックパレットの初期化 */

	/* スプライト／ＢＧ */
	PCG_INIT();	/* スプライト／ＢＧの初期化 */

	puts("App_Init 終了");
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
	puts("App_exit 開始");
	
	/* スプライト＆ＢＧ */
	PCG_VIEW(FALSE);		/* スプライト＆ＢＧ非表示 */
	puts("App_exit スプライト");

	/* 画面 */
	CRTMOD(0x100 + g_nCrtmod);	/* モードをもとに戻す */
	puts("App_exit 画面");

	/* 音楽 */
	Exit_Music();			/* 音楽停止 */
	puts("App_exit 音楽");
	
	if(g_bExit == TRUE)
	{
		puts("エラーをキャッチ！ ESC to skip");
		KeyHitESC();	/* デバッグ用 */
	}
	
	/* MFP */
	TimerD_EXIT();			/* Timer-Dの解除 */
	puts("App_exit Timer-D");
	MFP_EXIT();				/* MFP関連の解除 */
	puts("App_exit MFP");

	/* テキスト */
	T_EXIT();				/* テキスト終了処理 */
	puts("App_exit テキスト");

	MyMfree(0);				/* 全てのメモリを解放 */
	puts("App_exit メモリ");
	
	_dos_kflushio(0xFF);	/* キーバッファをクリア */
	puts("App_exit キーバッファクリア");

	/*スーパーバイザーモード終了*/
	_dos_super(g_nSuperchk);
	puts("App_exit スーパーバイザーモード終了");
	
	puts("App_exit 終了");
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
	uint8_t	bMode, bMode_rev;
	uint8_t	bNum;
	uint8_t	bNumCourse_Obj_Max;
	uint8_t	bNumEnemyCar_Max;
	uint8_t	bFlipStateOld;
	ST_TASK		stTask = {0}; 
	
	static uint8_t	bFlipState = Clear_G;
	
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
	bMode = g_mode;
	bMode_rev = g_mode_rev;
	
//	do
	{
		/* 背景の処理 */
		switch(bFlipState)
		{
			/* 描画のクリア処理 */
			case Clear_G:
			{
				G_CLR_ALL_OFFSC(bMode);	/* グラフィックを消去 */
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
			/* 自車の処理 */
			case MyCar_G:
			{
				MyCar_Interior(bMode);	/* 自車のインテリア処理 */
				S_Main_Score();			/* スコア表示 */
				PCG_Main();				/* スプライト出力 */
				T_Main();				/* テキスト画面の処理 */
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
		if(bFlipState > Flip_G)
		{
			bFlipState = 0;
		}
		BGprocces_ct++;

		GetNowTime(&time_now);	/* 現在時刻を取得 */
		
		if(ret != 0)	/* ループ脱出判定 */
		{
			ret = 1;	/* 画面をフリップする */
//			break;
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
//	while(1);
	
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
	int16_t	x, y;
	ST_CRT	stCRT = {0};
	ST_TASK		stTask = {0}; 
#ifdef DEBUG	/* デバッグコーナー */
	uint32_t time_now;
	static uint8_t	bFPS = 0u;
	static uint32_t	unTime_FPS = 0u;
#endif
	g_bFlip_old = g_bFlip;
	
	GetTaskInfo(&stTask);	/* タスクの情報を得る */
	if(stTask.bScene != SCENE_GAME)	/* SCENE_GAME以外ならフリップしない */
	{
		return ret;
	}
	
	if(g_bFlip == FALSE)	/* 描画中なのでフリップしない */
	{
		return ret;
	}

	SetFlip(FALSE);	/* フリップ禁止 */
	
	/* 画面をフリップする */
	GetCRT(&stCRT, g_mode);
	x = stCRT.hide_offset_x;
	y = stCRT.hide_offset_y;
	_iocs_home( 0b0001, x, y );	/* オフ・スクリーン側へ切替 */
	
	/* モードチェンジ */
	if(g_mode == 1u)		/* 上側判定 */
	{
		g_mode = 2u;	/* 下側へ */
		g_mode_rev = 1u;
	}
	else if(g_mode == 2u)	/* 下側判定 */
	{
		g_mode = 1u;	/* 上側へ */
		g_mode_rev = 2u;
	}
	else					/* その他 */
	{
		g_mode = 2u;	/* 下側へ */
		g_mode_rev = 1u;
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
	g_bFlip = bFlag;
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
void Debug_View(uint16_t uFreeRunCount)
{
	uint8_t bMode;
	
	/* モード引き渡し */
	bMode = g_mode;
	
	if(g_bDebugMode == TRUE)
	{
		uint8_t	str[256] = {0};
		static uint8_t ubDispNum = DEBUG_FREE;
		static uint8_t ubDispNum_flag = 0;

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
					i = Mmin(Mmax(g_uDebugNum, 0), COURSE_OBJ_MAX-1);
					GetCourseObj(&stCourse_Obj, i);	/* 障害物の情報 */
					sprintf(str, "C_Obj[%d](%4d,%3d,%d)(%6d,%d),Debug(%3d)", i, stCourse_Obj.x, stCourse_Obj.y, stCourse_Obj.z, stCourse_Obj.uTime, stCourse_Obj.ubAlive, g_uDebugNum);	/* 障害物の情報 */
#endif
				}
				break;
			case DEBUG_ENEMYCAR:
				{
#if 1	/* 敵車情報 */
					uint32_t	i = 0;
					ST_ENEMYCARDATA	stEnemyCar = {0};
					i = Mmin(Mmax(g_uDebugNum, 0), ENEMYCAR_MAX-1);
					GetEnemyCAR(&stEnemyCar, i);	/* ライバル車の情報 */
					sprintf(str, "Enemy[%d] (%d)(%4d,%4d,%4d),spd(%3d)", i,
						stEnemyCar.ubAlive,
						stEnemyCar.x,
						stEnemyCar.y,
						stEnemyCar.z,
						stEnemyCar.VehicleSpeed
					);	/* ライバル車の情報 */
#endif
				}
				break;
			case DEBUG_MYCAR:
				{
#if 1	/* 自車情報 */
					ST_CARDATA	stMyCar;
					GetMyCar(&stMyCar);	/* 自車 */
					sprintf(str, "Car[%d](%4d,%4d,%3d,%d,%d,%3d,%4d,%d)",
							stMyCar.ubCarType,			/* 車の種類 */
							stMyCar.uEngineRPM,			/* エンジン回転数 */
							stMyCar.Steering,			/* ステア */
							stMyCar.ubThrottle,			/* スロットル開度 */
							stMyCar.ubBrakeLights,		/* ブレーキライト */
							stMyCar.ubHeadLights,		/* ヘッドライト */
							stMyCar.ubWiper,			/* ワイパー */
							stMyCar.bTire,				/* タイヤの状態 */
							stMyCar.ubOBD				/* 故障の状態 */
					);	/* 自車の情報 */
#endif
				}
				break;
			case DEBUG_RASTER:
				{

#if 1	/* ラスター情報 */
					uint16_t x, y;
					int16_t pat;
					int16_t pos;
					ST_RAS_INFO	stRasInfo;
					GetRasterInfo(&stRasInfo);

					pos = Mmax(Mmin( g_uDebugNum, stRasInfo.size ), 0);
					GetRasterIntPos( &x, &y, &pat, pos );
					
					sprintf(str, "Ras(%d)s(%d,%d,%d,%d)i(%3d,%3d)(%3d,%3d)[%d]",
						pos,
						stRasInfo.st, stRasInfo.mid, stRasInfo.ed, stRasInfo.size,
						g_stRasterInt[(RASTER_H_MAX - 2)+0].y, g_stRasterInt[(RASTER_H_MAX - 2)+1].y,  
						g_stRasterInt[(RASTER_H_MAX - 4)+0].y, g_stRasterInt[(RASTER_H_MAX - 4)+1].y,
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
					sprintf(str, "%d T%2d(Max%2d),%2d,%2d,%2d,%2d,%2d FPS(%2d)", g_CpuTime, g_unTime_cal, g_unTime_cal_PH, 
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
#if 1	/* 何でもOK */
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
#if 1
			//			sprintf(str, "Ras %3d, %3d, %3d, V %3d", min, Raster_count, max);
					
			//			sprintf(str, "R=%3d,V=%3d,D=%3d,L=%3d,%3d,%3d,%3d,%3d",
				
					sprintf(str, "V=%3d,L=%3d,R=%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d",
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
