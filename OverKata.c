#ifndef	OVERKATA_C
#define	OVERKATA_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>

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
#include "Task.h"
#include "Text.h"
#include "Trap14.h"

/* グローバル変数 */
int32_t	g_nSuperchk = 0;
int32_t	g_nCrtmod = 0;
uint8_t	g_mode = 0;
uint8_t	g_mode_rev = 1;
uint16_t	g_uDebugNum = 0; 
uint8_t	g_bDebugMode = FALSE;
int16_t	g_CpuTime = 0;
uint32_t	g_unTime_cal = 0u;
uint32_t	g_unTime_cal_PH = 0u;
#ifdef DEBUG	/* デバッグコーナー */
uint32_t	g_unTime_Pass[6] = {0u};
#endif
int16_t	g_Input;

enum{
	DEBUG_NONE,
	DEBUG_COURSE_OBJ,
	DEBUG_ENEMYCAR,
	DEBUG_MYCAR,
	DEBUG_RASTER,
	DEBUG_CPUTIME,
	DEBUG_MAX,
};

/* グローバル構造体 */

/* 関数のプロトタイプ宣言 */
int16_t main(void);
static void App_Init(void);
static void App_exit(void);
int16_t	BG_main(uint8_t*);
int16_t	GetGameMode(uint8_t *);
int16_t	SetGameMode(uint8_t);
int16_t	GetDebugNum(uint16_t *);
int16_t	SetDebugNum(uint16_t);
int16_t	GetDebugMode(uint8_t *);
int16_t	SetDebugMode(uint8_t);

void (*usr_abort)(void);	/* ユーザのアボート処理関数 */

/* 関数 */
int16_t main(void)
{
	int16_t	ret = 0;

	uint32_t	unTime_cal = 0u;
	uint32_t	unTime_cal_PH = 0u;
	
	uint16_t	uFreeRunCount=0u;
	
	int16_t	loop = 1;
	int16_t	RD[1024] = {0};
	uint8_t	bMode_flag = FALSE;
	uint8_t	bDebugMode = TRUE;
	uint8_t	bDebugMode_flag;
	uint8_t	bFlip = FALSE;
	
	ST_TASK		stTask = {0}; 
	
	usr_abort = App_exit;	/* 例外発生で終了処理を実施する */

	init_trap14();	/* デバッグ用致命的エラーハンドリング */

	/* 変数の初期化 */
	g_mode = 1;
	g_mode_rev = 2u;
	g_uDebugNum = 0x80;
	bDebugMode = TRUE;

	/* デバッグコーナー */
#if 0
	puts("デバッグコーナー 開始");
	/* ↓自由にコードを書いてね */
	{
		uint8_t bFlag = FALSE;
		uint8_t bHit = FALSE;
		int16_t	count = 0;
		
		Init_Music();	/* 初期化(スーパーバイザーモードより前)	*/
		
		/* スーパーバイザーモード開始 */
		g_nSuperchk = SUPER(0);

//		CRTC_INIT();	/* 特殊解像度 */
		
//		M_SetMusic(0);	/* 初期設定 */
//		Music_Play(1);	/* 初期化のみ */
		
		puts("ＥＳＣキーで終了");
		loop = 1;
		do
		{
			int16_t	input = 0;
			get_key(&input, 0, 1);	/* キーボード＆ジョイスティック入力 */
			if((input & KEY_b_ESC ) != 0u)		/* ＥＳＣキー */
			{
				loop = 0;	/* ループ終了 */
			}
			
			if(ChatCancelSW((input & KEY_A)!=0u, &bFlag) == TRUE)	/* Aボタン */
			{
				M_Play(count);
				bHit = TRUE;
			}
			else
			{
				bHit = FALSE;
			}
			
			if((input & KEY_B) != 0u)	/* Bボタン */
			{
				if(count < 9000)
				{
					count++;
				}
			}
			else
			{
				if(count > 0)
				{
					count--;
				}
			}
			printf("Count = %6d, %d\r", count, bHit);

			if(loop == 0)break;
		}
		while( loop );

		/*スーパーバイザーモード終了*/
		SUPER(g_nSuperchk);

		/* 音楽 */
		Exit_Music();			/* 音楽停止 */
		
		_dos_kflushio(0xFF);	/* キーバッファをクリア */
	}
	/* ↑自由にコードを書いてね */
	puts("デバッグコーナー 終了");
	
	exit(0);
#endif
	
#if 0
	{
		/* アドレスエラー発生 */
		char buf[10];

		int *A = (int *)&buf[1];
		int B = *A;
		return B;
	}
#endif
	
	App_Init();	/* 初期化 */

	SetTaskInfo(SCENE_INIT);	/* 初期化シーンへ設定 */
	
	/* 乱数 */
	{
		/* 乱数の初期化 */
		int16_t a,b,c,d;
		a = 0;
		b = 0;
		c = 0;

		srandom(TIMEGET());

		for(a=0; a < 16; a++)
		{
			d = a * 64;
			c = (rand() % 5) - 2;
			for(b = d; b < d + 64; b++)
			{
				RD[b] = c;
			}
		}
	}
	
	do	/* メインループ処理 */
	{
		uint32_t time_st, time_now;
		int16_t	input = 0;
#ifdef DEBUG	/* デバッグコーナー */
		uint8_t	bTimePass = 1;
#endif
		
		/* 時刻設定 */
		GetNowTime(&time_st);	/* メイン処理の開始時刻を取得 */
		SetStartTime(time_st);	/* メイン処理の開始時刻を記憶 */
#ifdef DEBUG	/* デバッグコーナー */
		g_unTime_Pass[0] = time_st;
#endif
		
		/* タスク処理 */
		TaskManage();			/* タスクを管理する */
		GetTaskInfo(&stTask);	/* タスクの情報を得る */

		/* 入力処理 */
		get_key(&input, 0, 1);	/* キーボード＆ジョイスティック入力 */
		g_Input = input;
		if((input & KEY_b_ESC ) != 0u)		/* ＥＳＣキー */
		{
			loop = 0;	/* ループ終了 */
		}
		if(loop == 0)	/* 終了処理 */
		{
			Music_Stop();	/* 音楽再生 停止 */
			
			/* 動画 */
			MOV_Play(2);	/* バイバイ */
			break;
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

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* 初期化シーン */
			{
				/* 変数の初期化 */
				g_mode = 1;
				g_mode_rev = 2u;
				g_uDebugNum = 0x80;
				uFreeRunCount = 0;

				/* グラフィック */
				G_HOME();			/* グラフィック標準位置 */
				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */
				/* テキスト */
				T_INIT();			/* テキストＶＲＡＭ初期化 */
				T_PALET();			/* テキストパレット設定 */

				SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
			}
			break;
			case SCENE_TITLE_S:	/* タイトルシーン(開始処理) */
			{
				Music_Play(2);	/* タイトル曲 */
				
				G_Load_Mem( 8, X_OFFSET, Y_OFFSET, 0 );	/* タイトル画像 */
			
				BG_TextPut("OVER KATA", 96, 128);		/* タイトル文字 */
				BG_TextPut("PUSH A BUTTON", 80, 160);	/* ボタン押してね */

				SetTaskInfo(SCENE_TITLE);	/* タイトルシーン(開始処理)へ設定 */
			}
			break;
			case SCENE_TITLE:	/* タイトルシーン */
			{
				if(input == KEY_A)	/* Aボタン */
				{
					Music_Stop();	/* 音楽再生 停止 */
					
					ADPCM_Play(10);	/* SE:決定 */
				
					SetTaskInfo(SCENE_TITLE_E);	/* タイトルシーン(開始処理)へ設定 */
				}
			}
			break;
			case SCENE_TITLE_E:	/* タイトルシーン(終了処理) */
			{
				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */

				/* テキスト表示 */
				T_Clear();		/* テキストクリア */

				/* テキスト表示 */
				G_CLR();		/* グラフィッククリア */

				/* MFP */
				MFP_INIT();	/* 初期化処理 */
				
				SetTaskInfo(SCENE_START_S);	/* タイトルシーン(開始処理)へ設定 */
			}
			break;
			case SCENE_DEMO:	/* デモシーン */
			{
				
			}
			break;
			case SCENE_START_S:	/* ゲーム開始シーン(開始処理) */
			{
				SetTaskInfo(SCENE_START);	/* ゲームスタートタスクへ設定 */
			}
			break;
			case SCENE_START:	/* ゲーム開始シーン */
			{
				/* 動画 */
				MOV_Play(0);	/* スタート */

				SetTaskInfo(SCENE_START_E);	/* ゲームスタートタスクへ設定 */
			}
			break;
			case SCENE_START_E:	/* ゲーム開始シーン(終了処理) */
			{
				Set_CRT_Contrast(0);	/* コントラスト暗 */
				
				/* スプライト＆ＢＧ表示 */
				PCG_INIT();		/* スプライト／ＢＧの初期化 */
				PCG_VIEW(TRUE);	/* スプライト＆ＢＧ表示 */

				/* テキスト表示 */
				T_Clear();			/* テキストクリア */
				MyCar_G_Load();		/* 自車の画像読み込み*/
				MyCarInfo_Init();
				
				/* ライバル車の初期化 */
				InitEnemyCAR();

				/* コースのオブジェクトの初期化 */
				InitCourseObj();

				/* コースデータの初期化 */
				Road_Init(1);
				
				/* ラスター情報の初期化 */
				Raster_Init();
				
				/* テキスト表示 */
				T_PALET();		/* テキストパレット設定 */
				T_TopScore();	/* TOP */
				T_Time();		/* TIME */
				T_Score();		/* SCORE */
				T_Speed();		/* SPEED */
				T_Gear();		/* GEAR */
				T_SetBG_to_Text();	/* テキスト用作業用データ展開 */
				
				SetTaskInfo(SCENE_GAME_S);	/* ゲーム(開始処理)タスクへ設定 */
			}
			case SCENE_GAME_S:	/* ゲームシーン開始処理 */
			{
//				Music_Play(3);	/* メインBGM */
//				Music_Stop();	/* 音楽再生 停止 */
//				M_SetMusic(0);	/* 効果音再生の設定 */

				Set_CRT_Contrast(-1);	/* コントラストdef */
				
				SetTaskInfo(SCENE_GAME);	/* ゲームタスクへ設定 */
			}
			break;
			case SCENE_GAME:	/* ゲームシーン */
			{
				if((input & KEY_b_Q) != 0u)	/* Ｑ */
				{
					Music_Stop();	/* 音楽再生 停止 */

					/* 動画 */
					MOV_Play(1);	/* うふふ */

					SetTaskInfo(SCENE_GAME_E);	/* ゲームシーン(終了処理)へ設定 */
				}
				
				/* 画面消去 */
				if(bFlip == TRUE)
				{
					/* グラフィックを消去 */
					G_CLR_ALL_OFFSC(g_mode);
				}
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = time_now - g_unTime_Pass[0];	/* 1 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
				/* 自車の情報を取得 */
				MyCarInfo_Update(input);	/* 自車の情報を更新 */
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = time_now - g_unTime_Pass[0];	/* 2 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
				
				/* ラスター処理 */
				Raster_Main(g_mode);
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = time_now - g_unTime_Pass[0];	/* 3 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
				
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = time_now - g_unTime_Pass[0];	/* 4 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
				/* 余った時間で処理 */
				BG_main(&bFlip);	/* バックグランド処理 */
#ifdef DEBUG	/* デバッグコーナー */
				if(g_bDebugMode == TRUE)
				{
					GetNowTime(&time_now);
					g_unTime_Pass[bTimePass] = time_now - g_unTime_Pass[0];	/* 5 */
					bTimePass++;
					g_unTime_Pass[0] = time_now;	/* 一時保存 */
				}
#endif
			}
			break;
			case SCENE_GAME_E:	/* ゲームシーン(終了処理) */
			{
				/* スプライト＆ＢＧ */
				PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */
				
				/* テキスト */
				T_Clear();		/* テキストクリア */
				
				/* MFP */
				MFP_EXIT();		/* MFP関連の解除 */
				
				SetTaskInfo(SCENE_INIT);	/* 初期化シーンへ設定 */
			}
			break;
			case SCENE_GAME_OVER:	/* ゲームオーバーシーン */
			{
			}
			break;
			case SCENE_HI_SCORE:	/* ハイスコアランキングシーン */
			{
				
			}
			break;
			case SCENE_OPTION:		/* オプションシーン */
			{
				
			}
			break;
			case SCENE_EXIT:		/* 終了シーン */
			{
				
			}
			break;
			default:	/* 異常シーン */
			{
				
			}
			break;
		}

		if( (ChatCancelSW((input & KEY_b_M)!=0u, &bMode_flag) == TRUE) || (bFlip == TRUE) )	/* Ｍでモード切替 */
		{
			int16_t	x, y;
			ST_CRT	stCRT = {0};
			/* モードチェンジ */
			if(g_mode == 1u)
			{
				g_mode = 2u;
				g_mode_rev = 1u;
			}
			else
			{
				g_mode = 1u;
				g_mode_rev = 2u;
			}
			/* 画面をフリップする */
			GetCRT(&stCRT, g_mode);
			x = stCRT.view_offset_x;
			y = stCRT.view_offset_y;
			/* 画面の位置 */
			HOME(0b01, x, y );	/* Screen 0(TPS/FPS) */
			T_Scroll( 0, y  );	/* テキスト画面 */
		}

		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */

		/* 処理時間計測 */
		GetNowTime(&time_now);
		if( stTask.bScene == SCENE_GAME )
		{
			unTime_cal = time_now - time_st;	/* LSB:1 UNIT:ms */
			unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);
			
			g_unTime_cal = unTime_cal;
			g_unTime_cal_PH = unTime_cal_PH;
		}

		/* 同期待ち */
		vwait(1);
	}
	while( loop );
	
	App_exit();	/* 終了処理 */

#ifdef DEBUG	/* デバッグコーナー */
	{
		uint32_t i=0, j=0;
		uint32_t st;
		ST_RAS_INFO	stRasInfo;
		GetRasterInfo(&stRasInfo);
		
		printf("stRasInfo st,mid,ed,size=(%4d,%4d,%4d,%4d)\n", stRasInfo.st, stRasInfo.mid, stRasInfo.ed, stRasInfo.size);
		printf("GetRasterIntPos[i]=(x,y,pat)=(H_pos)\n");
		
		st = stRasInfo.st;
		for(i=st; i < stRasInfo.ed; i+=RASTER_NEXT)
		{
			uint16_t x, y;
			int16_t pat;
			
			GetRasterIntPos( &x, &y, &pat, i );
			
			printf("[%3d]=(%4d,%4d,%4d)=(%4d), ", i, x, y, pat, i - st/*(y + i - ROAD_ST_POINT)*/ );
			if((j%3) == 0)printf("\n");
			j++;
		}
		printf("\n");
	}
#endif
	printf("処理時間:Real=%3d[ms] PH=%3d[ms]\n", unTime_cal, unTime_cal_PH);
	
	return ret;
}

static void App_Init(void)
{
	puts("App_Init 開始");

	/* デバッグ値初期化 */
	SetDebugNum(0x80);

	/* MFP */
	g_CpuTime = TimerD_INIT();	/* タイマーD初期化 */

	/* 音楽 */
	Init_Music();	/* 初期化(スーパーバイザーモードより前)	*/
	Music_Play(1);	/* ローディング中 */
	
	/* スーパーバイザーモード開始 */
	g_nSuperchk = SUPER(0);
	if( g_nSuperchk < 0 ) {
		puts("すでにスーパーバイザーモード");
	} else {
		puts("スーパーバイザーモード開始");
	}
	
	/* 画面 */
	g_nCrtmod = CRT_INIT();

	/* グラフィック */
	G_INIT();			/* 画面の初期設定 */
	G_Palette_INIT();	/* グラフィックパレットの初期化 */

	/* データの読み込み */
	CG_File_Load();		/* グラフィックデータ読み込み */

	/* 動画 */
	MOV_INIT();	/* 初期化処理 */

	/* スプライト／ＢＧ */
	PCG_INIT();	/* スプライト／ＢＧの初期化 */

	/* テキスト */
	T_INIT();	/* テキストＶＲＡＭ初期化 */

	puts("App_Init 終了");
}

static void App_exit(void)
{
	puts("App_exit 開始");
	
	/* MFP */
	MFP_EXIT();				/* MFP関連の解除 */
	TimerD_EXIT();			/* Timer-Dの解除 */

	/* 音楽 */
	Exit_Music();			/* 音楽停止 */
	
	/* テキスト */
	T_EXIT();				/* テキスト終了処理 */

	/* スプライト＆ＢＧ */
	PCG_VIEW(FALSE);		/* スプライト＆ＢＧ非表示 */

	/* 画面 */
	CRTMOD(g_nCrtmod);		/* モードをもとに戻す */

	MyMfree(0);				/* 全てのメモリを解放 */
	
	_dos_kflushio(0xFF);	/* キーバッファをクリア */

	/*スーパーバイザーモード終了*/
	SUPER(g_nSuperchk);
	
	puts("App_exit 終了");
}

int16_t BG_main(uint8_t* bFlip)
{
	int16_t	ret = 0;
	uint32_t	time_now;
	uint32_t	time_st;
	uint16_t	BGprocces_ct = 0;
	uint8_t	bNum;
	uint8_t	bFlipStateOld;
	ST_TASK		stTask = {0}; 

	static uint8_t	bFlipState = Clear_G;
	
	GetStartTime(&time_st);	/* 開始時刻を取得 */
	GetTaskInfo(&stTask);	/* タスクの情報を得る */

	bFlipStateOld = bFlipState;

	do
	{
		GetNowTime(&time_now);	/* 現在時刻を取得 */
		
		if((time_now - time_st) >= 500)	/* 28ms以内なら余った時間で処理する */
		{
			break;
		}
		
		/* 背景の処理 */
		/* 描画のクリア処理 */
		switch(bFlipState)
		{
			/* 描画のクリア処理 */
			case Clear_G:
			{
				bFlipState++;
				*bFlip = FALSE;
				break;
			}
			/* 背景 */
			case BackGround_G:
			{
				if((time_now - time_st) >= 28)	/* 28ms以内なら余った時間で処理する */
				{
					bFlipState++;
				}
				else
				{
					Move_Course_BG(g_mode);	/* コースの動きにあわせて背景を動かす */
				}
				
				bFlipState++;
				*bFlip = FALSE;
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
			case Object8_G:
			case Object9_G:
			case ObjectA_G:
			case ObjectB_G:
			case ObjectC_G:
			case ObjectD_G:
			case ObjectE_G:
			case ObjectF_G:
			{
				if((time_now - time_st) >= 28)	/* 28ms以内なら余った時間で処理する */
				{
					bFlipState++;
				}
				else
				{
					bNum = bFlipState - Object0_G;
					if(bNum < COURSE_OBJ_MAX)
					{
						/* 描画順をソートする */
						if(bNum == 0)
						{
							Sort_Course_Obj();		/* コースオブジェクト */
						}
						Course_Obj_main(bNum, g_mode, g_mode_rev);
					}
				}
				
				bFlipState++;
				*bFlip = FALSE;
				break;
			}
			/* ライバル車 */
			case Enemy1_G:
			case Enemy2_G:
			case Enemy3_G:
			case Enemy4_G:
			{
				if((time_now - time_st) >= 28)	/* 28ms以内なら余った時間で処理する */
				{
					bFlipState++;
				}
				else
				{
					bNum = bFlipState - Enemy1_G;
					if(bNum < ENEMYCAR_MAX)
					{
						/* 描画順をソートする */
						if(bNum == 0)
						{
							Sort_EnemyCAR();		/* ライバル車 */
						}
						
						EnemyCAR_main(bNum, g_mode, g_mode_rev);
					}
				}
				
				bFlipState++;
				*bFlip = FALSE;
				break;
			}
			case MyCar_G:
			{
				if((time_now - time_st) >= 28)	/* 28ms以内なら余った時間で処理する */
				{
					bFlipState++;
				}
				else
				{
					T_Main(g_mode);	/* テキスト画面の処理 */

					MyCar_Interior(g_mode);	/* 自車のインテリア処理 */
				}
				
				bFlipState++;
				*bFlip = FALSE;
				break;
			}
#ifdef DEBUG	/* デバッグコーナー */
			case Debug_View_G:
			{
				if(g_bDebugMode == TRUE)
				{
					uint8_t	str[256] = {0};
					static uint8_t ubDispNum = DEBUG_MYCAR;
					static uint8_t ubDispNum_flag = 0;

					if(ChatCancelSW((g_Input & KEY_b_RLUP)!=0u, &ubDispNum_flag) == TRUE)	/* ロールアップで表示切替 */
					{
						ubDispNum++;
						if(DEBUG_MAX <= ubDispNum)
						{
							ubDispNum = DEBUG_NONE;
						}
					}

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
							GetRasterIntPos( &x, &y, &pat, stRasInfo.st + pos );
							
							sprintf(str, "Ras[st+(%d)]s(%d,%d,%d,%d)i(%3d,%3d,%3d)",
								pos,
								stRasInfo.st, stRasInfo.mid, stRasInfo.ed, stRasInfo.size,
								x, y, pat
							);	/* ラスター情報 */
#endif
						}
						break;
					case DEBUG_CPUTIME:
						{
#if 1	/* CPU情報 */
//							sprintf(str, "CPU Time%2d[ms](MAX%2d[ms]),Debug(%3d)", g_unTime_cal, g_unTime_cal_PH, g_uDebugNum);	/* 処理負荷 */
							sprintf(str, "%d Time[ms]%2d(Max%2d),%2d,%2d,%2d,%2d,%2d", g_CpuTime, g_unTime_cal, g_unTime_cal_PH, 
									g_unTime_Pass[1],
									g_unTime_Pass[2],
									g_unTime_Pass[3],
									g_unTime_Pass[4],
									g_unTime_Pass[5]
							);
#endif
						}
						break;
					default:
						break;
					}
					/* 表示 */
					Put_Message_To_Graphic(str, g_mode);	/* グラフィックのデバッグエリアにメッセージ描画 */
				}
				
				bFlipState++;
				*bFlip = FALSE;
				break;
			}
#endif
			case Flip_G:
			{
				/* 画面をフリップする */
				bFlipState = Clear_G;
				*bFlip = TRUE;
				break;
			}
			default:
			{
				bFlipState = Clear_G;
				*bFlip = FALSE;
				break;
			}
		}
		
		BGprocces_ct++;
		
		if(*bFlip == TRUE)	/* 切替判定時は即ループ終了 */
		{
			break;
		}
		
		if(bFlipStateOld == bFlipState)	/* ステートが一周したらループ終了 */
		{
			break;
		}

		if((time_now - time_st) >= 14)	/* 14ms以内なら余った時間で処理する */
		{
			break;
		}
	}
	while(1);
	
	return	ret;
}

int16_t	GetGameMode(uint8_t *bMode)
{
	int16_t	ret = 0;
	*bMode = g_mode;
	return ret;
}

int16_t	SetGameMode(uint8_t bMode)
{
	int16_t	ret = 0;
	g_mode = bMode;
	return ret;
}

int16_t	GetDebugNum(uint16_t *uNum)
{
	int16_t	ret = 0;
	*uNum = g_uDebugNum;
	return ret;
}

int16_t	SetDebugNum(uint16_t uNum)
{
	int16_t	ret = 0;
	g_uDebugNum = uNum;
	return ret;
}

int16_t	GetDebugMode(uint8_t *bMode)
{
	int16_t	ret = 0;
	*bMode = g_bDebugMode;
	return ret;
}

int16_t	SetDebugMode(uint8_t bMode)
{
	int16_t	ret = 0;
	g_bDebugMode = bMode;
	return ret;
}

#endif	/* OVERKATA_C */
