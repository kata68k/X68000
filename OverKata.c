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
SI	g_nSuperchk = 0;
SI	g_nCrtmod = 0;
UC	g_mode = 0;
UC	g_mode_rev = 1;
US	g_uDebugNum = 0; 
UC	g_bDebugMode = FALSE;

/* グローバル構造体 */

/* 関数のプロトタイプ宣言 */
SS main(void);
static void App_Init(void);
static void App_exit(void);
SS	BG_main(UC*);
SS	GetDebugNum(US *);
SS	SetDebugNum(US);
SS	GetDebugMode(UC *);
SS	SetDebugMode(UC);

void (*usr_abort)(void);	/* ユーザのアボート処理関数 */

/* 関数 */
SS main(void)
{
	SS	ret = 0;

	UI	unTime_cal = 0u;
	UI	unTime_cal_PH = 0u;
	
	US	uFreeRunCount=0u;
	
	SS	loop = 1;
	SS	RD[1024] = {0};
	UC	bMode_flag = FALSE;
	UC	bDebugMode = TRUE;
	UC	bDebugMode_flag;
	UC	bFlip = TRUE;
	
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
		printf("MoonRegst = %d\n", MoonRegst("UFUFU.MCS"));
		printf("MoonPlay  = %d\n", MoonPlay("UFUFU.MCS"));
	}
	/* ↑自由にコードを書いてね */
	puts("ＥＳＣキーで終了");
	loop = 1;
	do
	{
		if( ( BITSNS( 0 ) & 0x02 ) != 0 ) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;
	}
	while( loop );
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
		SS a,b,c,d;
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
		UI time_st, time_now;
		SS	input = 0;
		
		/* 時刻設定 */
		GetNowTime(&time_st);	/* メイン処理の開始時刻を取得 */
		SetStartTime(time_st);	/* メイン処理の開始時刻を記憶 */

		/* タスク処理 */
		TaskManage();			/* タスクを管理する */
		GetTaskInfo(&stTask);	/* タスクの情報を得る */

		/* 入力処理 */
		get_key(&input, 0, 1);	/* キーボード＆ジョイスティック入力 */
		
		if((input & KEY_b_ESC ) != 0u)		/* ＥＳＣキー */
		{
			loop = 0;	/* ループ終了 */
		}
		if(loop == 0)	/* 終了処理 */
		{
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
				
				G_Load( 8, X_OFFSET, Y_OFFSET, 0 );	/* タイトル画像 */
			
				BG_TextPut("OVER KATA", 96, 128);		/* タイトル文字 */
				BG_TextPut("PUSH A BUTTON", 80, 160);	/* ボタン押してね */

				SetTaskInfo(SCENE_TITLE);	/* タイトルシーン(開始処理)へ設定 */
			}
			break;
			case SCENE_TITLE:	/* タイトルシーン */
			{
				if(input == KEY_A)	/* Aボタン */
				{
					ADPCM_Play(10);	/* SE:決定 */
				
					Music_Play(1);	/* ローディング中 */
					//Music_Play(3);	/* ゲーム曲 */
					
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
				/* コースデータの初期化 */
				Road_Init(1);
				
				/* ラスター情報の初期化 */
				Raster_Init();
				
				/* スプライト＆ＢＧ表示 */
				PCG_INIT();		/* スプライト／ＢＧの初期化 */
				PCG_VIEW(TRUE);	

				/* 自車の画像読み込み*/
				MyCar_G_Load();
				
				/* ライバル車の初期化 */
				InitEnemyCAR();

				/* コースのオブジェクトの初期化 */
				InitCourseObj();

				/* テキスト表示 */
				T_Clear();		/* テキストクリア */
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
				SetTaskInfo(SCENE_GAME);	/* ゲームタスクへ設定 */
			}
			break;
			case SCENE_GAME:	/* ゲームシーン */
			{
				if((input & KEY_b_Q) != 0u)	/* Ｑ */
				{
					/* 動画 */
					MOV_Play(1);	/* うふふ */

					SetTaskInfo(SCENE_GAME_E);	/* ゲームシーン(終了処理)へ設定 */
				}
				
				/* 自車の情報を取得 */
				UpdateMyCarInfo(input);		/* 自車の情報を更新 */
				
				/* ラスター処理 */
				Raster_Main(g_mode);
				
				if(stTask.b96ms == TRUE)
				{
					/* コースアウト時の処理 */
					MyCar_CourseOut();	/* コースアウト時のエフェクト */

					/* グラフィック画面の処理 */
					MyCar_Interior(g_mode);	/* 自車のインテリア処理 */
				}
				
				/* テキスト画面の処理 */
				if(stTask.b496ms == TRUE)
				{
					T_Main();
				}

				/* 余った時間で処理 */
				BG_main(&bFlip);	/* バックグランド処理 */
				
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
		}

#ifdef DEBUG	/* デバッグコーナー */
//		BG_TextPut("OverKata", 4, 10);
//		BG_TextPut("OverKata", 128, 128);
//		BG_TextPut("OVER KATA", 128, 128);

		if(bDebugMode == TRUE)
		{
			if(stTask.b496ms == TRUE)
			{
				/* モニタ */
//				Message_Num(&	,				 0,  9, 2, MONI_Type_SS, "%3d");
//				Message_Num(&, 				 6,  9, 2, MONI_Type_SS, "%2d");
//				Message_Num(&, 				12,  9, 2, MONI_Type_SS, "%2d");
//				Message_Num(&,		 			20,  9, 2, MONI_Type_SS, "%3d");
				
//				Message_Num(&uRas_st,			 0, 10, 2, MONI_Type_US, "%3d");
//				Message_Num(&uRas_mid,			 7, 10, 2, MONI_Type_US, "%3d");
//				Message_Num(&uRas_ed,			13, 10, 2, MONI_Type_US, "%3d");
//				Message_Num(&uRas_size,			20, 10, 2, MONI_Type_US, "%3d");
				
//				Message_Num(&uPal_tmp[uRas_st],	 0, 11, 2, MONI_Type_US, "%3d");
//				Message_Num(&uRas_tmp[uRas_st],	 6, 11, 2, MONI_Type_US, "%3d");
//				Message_Num(&cal_tan,			12, 11, 2, MONI_Type_SS, "%3d");
//				Message_Num(&rad,				12, 11, 2, MONI_Type_FL, "%f");
				
//				Message_Num(&road_height,		 0, 12, 2, MONI_Type_SS, "%3d");
//				Message_Num(&road_slope,	 	 6, 12, 2, MONI_Type_SS, "%3d");
//				Message_Num(&road_distance,		12, 12, 2, MONI_Type_SS, "%3d");
//				Message_Num(&road_angle,		20, 12, 2, MONI_Type_SS, "%3d");
				
				Message_Num(&unTime_cal,	 	 0, 13, 2, MONI_Type_UI, "%3d");
				Message_Num(&unTime_cal_PH,		 6, 13, 2, MONI_Type_UI, "%3d");
				Message_Num(&g_uDebugNum,		12, 13, 2, MONI_Type_US, "%3d");
			}
		}
#endif
		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */

		/* 処理時間計測 */
		GetNowTime(&time_now);
		if( stTask.bScene == SCENE_GAME )
		{
			unTime_cal = time_now - time_st;	/* LSB:1 UNIT:ms */
			unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);
		}

		/* 同期待ち */
		vwait(1);
	}
	while( loop );
	
	App_exit();	/* 終了処理 */

#ifdef DEBUG	/* デバッグコーナー */
	{
		UI i=0, j=0;
		ST_RAS_INFO	stRasInfo;
		GetRasterInfo(&stRasInfo);
		
		printf("stRasInfo st,mid,ed,size=(%4d,%4d,%4d,%4d)\n", stRasInfo.st, stRasInfo.mid, stRasInfo.ed, stRasInfo.size);
		
		for(i=stRasInfo.st; i < stRasInfo.ed; i+=RASTER_NEXT)
		{
			US x, y;
			SS pat;
			
			GetRasterIntPos( &x, &y, &pat, i );
			
			printf("[%3d]=(%4d,%4d,%4d), ", i, x, y, pat);
			if((j%4) == 0)printf("\n");
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
	/* タイマーD初期化 */
	TimerD_INIT();
	
	/* 音楽の初期化 */
	Init_Music();

	Music_Play(1);	/* ローディング中 */

	/* スーパーバイザーモード開始 */
	/*ＤＯＳのスーパーバイザーモード開始*/
	g_nSuperchk = SUPER(0);
	if( g_nSuperchk < 0 ) {
		puts("すでにスーパーバイザーモード");
	} else {
		puts("スーパーバイザーモード開始");
	}
	
	g_nCrtmod = CRTMOD(-1);	/* 現在のモードを返す */
	
	/* 動画 */
	MOV_INIT();	/* 初期化処理 */
	
	/* グラフィック表示 */
	G_INIT();	/*画面の初期設定*/

	/* スプライト／ＢＧ表示 */
	PCG_INIT();	/* スプライト／ＢＧの初期化 */

	/* テキスト */
	T_INIT();	/* テキストＶＲＡＭ初期化 */
}

static void App_exit(void)
{
	/* MFP */
	MFP_EXIT();		/* MFP関連の解除 */
	TimerD_EXIT();	/* Timer-Dの解除 */
	
	/* グラフィック表示 */
	G_CLR_ON();

	/* スプライト＆ＢＧ */
	PCG_VIEW(FALSE);	/* スプライト＆ＢＧ非表示 */

	/* テキスト */
	T_EXIT();	/* テキスト初期化 */

	CRTMOD(g_nCrtmod);			/* モードをもとに戻す */
	
	/* 音楽停止 */
	Exit_Music();

	/* タイマーD終了 */
	TimerD_EXIT();
	
	_dos_kflushio(0xFF);	/* キーバッファをクリア */

	/*スーパーバイザーモード終了*/
	SUPER(g_nSuperchk);
}

SS BG_main(UC* bFlip)
{
	SS	ret = 0;
	UI	time_now;
	UI	time_st;
	US	BGprocces_ct = 0;
	UC	bNum;
	UC	bFlipStateOld;

	static UC	bFlipState = Clear_G;
	
	GetStartTime(&time_st);	/* 開始時刻を取得 */

	bFlipStateOld = bFlipState;

	/* 描画順をソートする */
	Sort_EnemyCAR();		/* ライバル車 */
	Sort_Course_Obj();		/* コースオブジェクト */
	
	do
	{
		GetNowTime(&time_now);	/* 現在時刻を取得 */
		
		if((time_now - time_st) >= 14)	/* 14ms以内なら余った時間で処理する */
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
				G_CLR_ALL_OFFSC(g_mode);
				bFlipState = Enemy1_G;
//				bFlipState = Object1_G;
				*bFlip = FALSE;
				break;
			}
			/* ライバル車 */
			case Enemy1_G:
			case Enemy2_G:
			case Enemy3_G:
			case Enemy4_G:
			{
				bNum = bFlipState - Enemy1_G;
				EnemyCAR_main(bNum, g_mode, g_mode_rev);
				bFlipState++;
				*bFlip = FALSE;
				break;
			}
			/* ヤシの木(E:右側 / O:左側) */
			case Object1_G:
			case Object2_G:
			case Object3_G:
			case Object4_G:
			case Object5_G:
			case Object6_G:
			{
				bNum = bFlipState - Object1_G;
				Course_Obj_main(bNum, g_mode, g_mode_rev);
				bFlipState++;
				*bFlip = FALSE;
				break;
			}
			case Flip_G:
			{
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
	}
	while(1);
	
	return	ret;
}

SS	GetDebugNum(US *uNum)
{
	SS	ret = 0;
	*uNum = g_uDebugNum;
	return ret;
}

SS	SetDebugNum(US uNum)
{
	SS	ret = 0;
	g_uDebugNum = uNum;
	return ret;
}

SS	GetDebugMode(UC *bMode)
{
	SS	ret = 0;
	*bMode = g_bDebugMode;
	return ret;
}

SS	SetDebugMode(UC bMode)
{
	SS	ret = 0;
	g_bDebugMode = bMode;
	return ret;
}


#endif	/* OVERKATA_C */
