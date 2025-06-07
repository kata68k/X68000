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

#include "APL_ZooKeeper.h"

//#define 	W_BUFFER	/* ダブルバッファリングモード */
//#define	FPS_MONI	/* FPS計測 */
//#define	CPU_MONI	/* CPU計測 */
#define	MEM_MONI	/* MEM計測 */

#define		GAMEPLAY_TIME				(90000)
#define		GAME_STOP_COUNT				(32)

#define		DEMO_LOGO_INTERVAL_TIME		(1000)
#define		DEMO_START_END_TIME			(30000)
#define		DEMO_GAMEOVER_TIME			(5000)

/* グローバル変数 */
uint32_t	g_unTime_cal = 0u;
uint32_t	g_unTime_cal_PH = 0u;
uint32_t	g_unTime_Pass = 0xFFFFFFFFul;
int32_t		g_nCrtmod = 0;
int32_t		g_nMaxUseMemSize;
int16_t		g_CpuTime = 0;
uint8_t		g_mode;
uint8_t		g_mode_rev;
uint8_t		g_Vwait = 1;
uint8_t		g_bFlip = FALSE;
uint8_t		g_bFlip_old = FALSE;
uint16_t	g_unFrameCount;
#ifdef FPS_MONI
uint8_t		g_bFPS_PH = 0u;
#endif
uint8_t		g_bExit = TRUE;
int16_t		g_GameSpeed;
static uint32_t s_PassTime;
static uint32_t s_DemoTime;
static uint16_t s_music_tempo;
static uint16_t s_music_tempo_old;
uint8_t		g_ubDemoPlay = FALSE;
uint8_t		g_ubPhantomX = FALSE;


/* グローバル構造体 */
#ifdef DEBUG	/* デバッグコーナー */
uint16_t	g_uDebugNum = 0;
//uint16_t	g_uDebugNum = (Bit_7|Bit_4|Bit_0);
#endif

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
int16_t main_Task(void)
{
	int16_t	ret = 0;

	int16_t	rank = -1;
	int16_t	reset = -1;
	uint16_t	uFreeRunCount=0u;
#ifdef DEBUG	/* デバッグコーナー */
	uint32_t	unTime_cal = 0u;
	uint32_t	unTime_cal_PH = 0u;
#endif
	static int32_t y1 = 3, x1 = 3;
	static int32_t y2 = 3, x2 = 4;

	int16_t	loop;
	int16_t	pushCount = 0;

	int8_t	bStage = 0;
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
	g_unFrameCount = 0;
	
	s_music_tempo = 300;
	s_music_tempo_old = 300;

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

#if CNF_VDISP
		/* V-Syncで入力 */
#else
		if(Input_Main(g_ubDemoPlay) != 0u) 	/* 入力処理 */
		{
		}
#endif

#ifdef DEBUG	/* デバッグコーナー */
//		DirectInputKeyNum(&g_uDebugNum, 3);	/* キーボードから数字を入力 */
#endif

		if((GetInput() & KEY_b_ESC ) != 0u)	/* ＥＳＣキー */
		{
			/* 終了 */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 5)
			{
				SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
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
					ADPCM_Play(SE_OK);	/* 決定音 */

					Music_Stop();	/* Stop */

					SetTaskInfo(SCENE_INIT);	/* 終了シーンへ設定 */
				}
			}
		}		
		else if((GetInput() & KEY_b_Q ) != 0u)	/* Qキー */
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
					ADPCM_Play(SE_OK);	/* 決定音 */

					S_Init_Score();
					S_Set_Combo(0);	/* コンボカウンタ リセット */

					S_Set_ScoreMode(FALSE);	/* 点数計算無効 */
				}
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
				T_Clear();		/* テキストクリア */

				SetTaskInfo(SCENE_TITLE_S);	/* シーン(開始処理)へ設定 */
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_TITLE_S:
			{
				int8_t sStr[12] = {0};

				Music_Play(BGM_TITLE);

				Set_CRT_Contrast(0);	/* 真っ暗にする */
				G_VIDEO_PRI(3);	/* TX>SP>GR */
				G_PAGE_SET(0b0010);	/* GR1 */
			    PCG_VIEW(0x00);        /* SP OFF / BG0 OFF / BG1 OFF */

				if(G_Load(1, 0, 0, 1) < 0)	/* 256色 ページ1 */
				{
					T_Clear();		/* テキストクリア */
				}

				S_Init_Score();
				S_Set_Combo(0);	/* コンボカウンタ リセット */
				S_Set_ScoreMode(TRUE);	/* 点数計算有効 */
				
				BG_TextPut("PUSH A BUTTON TO START!", 40, 224);

				BG_TextPut("X68ZKEEPER 2025", 0, 248);
				sprintf(sStr, "VER%d.%d.%d", MAJOR_VER, MINOR_VER, PATCH_VER);
				BG_TextPut(sStr, 192, 248);

				Set_CRT_Contrast(-1);	/* もとに戻す */

				SetTaskInfo(SCENE_TITLE);	/* シーン(処理)へ設定 */
				break;
			}
			case SCENE_TITLE:
			{
				static int8_t s_bRelese = FALSE;
				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
				{
					s_bRelese = TRUE;
				}
				else
				{
					if(s_bRelese == TRUE)
					{
						ADPCM_Play(SE_OK);	/* 決定音 */

						s_bRelese = FALSE;
						SetTaskInfo(SCENE_TITLE_E);	/* シーン設定 */
					}
				}

				break;
			}
			case SCENE_TITLE_E:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					BG_TextPut("                       ", 40, 224);

					G_PAGE_SET(0b0010);	/* GR1 */
					if(G_Load(2, 0, 0, 1) < 0)	/* 256色 ページ1*/
					{
						T_Clear();		/* テキストクリア */
					}
					G_Palette_HALF();	/* パレットを設定値から半分にします */

					PCG_VIEW(0x07);        /* SP ON / BG0 ON / BG1 ON */
			
					APL_ZKP_SP_init();

					bStage = 0;
					reset = 0;
					APL_ZKP_set_GameLv(bStage);	/* ゲームレベルリセット */

					SetTaskInfo(SCENE_START_S);	/* シーン設定 */
				}
				break;
			}
			case SCENE_START_S:
			{
				Music_Play(BGM_STAGE_1);

				SetTaskInfo(SCENE_START);	/* シーン設定 */
				break;
			}
			case SCENE_START:
			{
				ADPCM_Play(SE_GAME_START);

#if CNF_MACS
				PCG_OFF();			/* SP OFF */
				/* 動画 */
				MOV_Play2(MOV_GAME_START, 0x64);		/* カットイン */
				/* 画面モード再設定 */
				CRTC_INIT(0x100 + 0x0A);	/* 画面モード初期化　グラフィック初期化なし */
				PCG_ON();			/* SP ON */
#else
#endif
				SetTaskInfo(SCENE_START_E);	/* シーン設定 */
				break;
			}
			case SCENE_START_E:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					SetTaskInfo(SCENE_GAME1_S);	/* シーン設定 */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME1_S:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					switch(reset)
					{
						case 0:	/* 配置、タイマーとカウンタごとリセット */
						{
							APL_ZKP_init_board();
							//if (APL_ZKP_swap_if_valid(0, 0, 0, 1))
							{
								// 成功した場合は処理を続行（消去?落下?補充）
								APL_ZKP_process_all_matches();
							}

							APL_ZKP_init_animal_counter();

							GetNowTime(&s_PassTime);	/* タイムアウトカウンタリセット */
							break;
						}
						case 1:	/* タイマーとカウンタリセット */
						{
							APL_ZKP_init_animal_counter();

							GetNowTime(&s_PassTime);	/* タイムアウトカウンタリセット */
							break;
						}
						case 2:	/* 配置リセット */
						{
							APL_ZKP_init_board();
							//if (APL_ZKP_swap_if_valid(0, 0, 0, 1))
							{
								// 成功した場合は処理を続行（消去?落下?補充）
								APL_ZKP_process_all_matches();
							}
							break;
						}
						default:
						{
							break;
						}
					}

					if(APL_ZKP_has_valid_moves() == 0){	/* 有効手を探す */
						reset = 2;
					}
					else
					{
						reset = 3;
						APL_ZKP_print_board();

						SetTaskInfo(SCENE_GAME1);	/* シーン設定 */
					}
				}
				break;
			}
			case SCENE_GAME1:
			{
				uint32_t cal;
				uint32_t time_now_pos;

				GetNowTime(&time_now_pos);
			    if(S_Get_ScoreMode() == FALSE)s_PassTime = time_now_pos;  /* 点数計算無効化状態 */
				cal = Mmax(0, (time_now_pos - s_PassTime));

				if(APL_ZKP_KEY_Input(&y1, &x1, &y2, &x2) != 0u)
				{
					ADPCM_Play(SE_EXCHANGE);

					if (APL_ZKP_swap_if_valid(y1, x1, y2, x2)) {
						// 成功
						SetTaskInfo(SCENE_GAME2_S);	/* シーン設定 */
					} else {
						// 失敗なら盤面そのままでロスタイム
						SetTaskInfo(SCENE_GAME3_S);	/* シーン設定 */
					}
				}

				APL_ZKP_print_board();
				APL_ZKP_SP_cursor_view(y1, x1, y2, x2);	/* カーソル表示 */

				if( GetPassTime(GAMEPLAY_TIME, &s_PassTime) != 0u)	/* タイムオーバー */
				{
					if(bStage > 5)	/* +5面クリア(+5) */
					{
						Music_Play(BGM_GAME_CLEAR);

						S_Clear_Score();	/* スコア：ステージクリアボーナス */
					}
					else
					{
					}
					SetTaskInfo(SCENE_GAME4_S);	/* シーン(実施処理)へ設定 */
				}
				else if(APL_ZKP_stage_clear_chk() != 0u)
				{
					reset = 0;
					if(bStage <= (BGM_STAGE_5 - BGM_STAGE_1))
					{
						bStage++;
					}
					APL_ZKP_set_GameLv(bStage);  /* 1,2,3,4*/
					
					ADPCM_Play(SE_GAME_CLEAR);

					Music_Play(BGM_STAGE_1 + bStage);  /* 1,2,3,4 */

					S_Add_Score_Point((GAMEPLAY_TIME - cal) / 10);	/* 残り時間[s]*100 */

#if CNF_MACS
					PCG_OFF();			/* SP OFF */
					/* 動画 */
					MOV_Play2(MOV_GAME_CLEAR, 0x64);		/* カットイン */
					/* 画面モード再設定 */
					CRTC_INIT(0x100 + 0x0A);	/* 画面モード初期化　グラフィック初期化なし */
					PCG_ON();			/* SP ON */
#else
#endif
					SetTaskInfo(SCENE_GAME1_S);	/* シーン(実施処理)へ設定 */
				}
				else
				{
					/* 何もしない */
				}

				break;
			}
			case SCENE_GAME1_E:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					SetTaskInfo(SCENE_GAME_OVER_S);	/* シーン設定 */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME2_S:
			{
				// 成功した場合は処理を続行（消去?落下?補充）
				if(APL_ZKP_SP_Swap_view(TRUE) != 0u)
				{
					SetTaskInfo(SCENE_GAME2);	/* シーン設定 */
				}

				break;
			}
			case SCENE_GAME2:
			{
				APL_ZKP_check_matches();

				if(APL_ZKP_check_matches_chk() != 0u)
				{
					uint32_t time_now_pos;
					GetNowTime(&time_now_pos);
					s_PassTime = Mmin(time_now_pos, s_PassTime + 2000);	/* +2s 加算 */

					ADPCM_Play(Mmin(SE_COMBO_1 + S_Get_Combo(), SE_COMBO_8));

					S_Set_Combo(S_Get_Combo() + 1);

					SetTaskInfo(SCENE_GAME2_0);	/* シーン設定 */
				}
				else
				{
					SetTaskInfo(SCENE_GAME2_E);	/* シーン設定 */
				}
				break;
			}
			case SCENE_GAME2_0:
			{
				static int32_t s_Clear_StepCount = 0;
		//      APL_ZKP_print_clear_map();
				/* 消して０にする */
				if(APL_ZKP_apply_clear_step() == 0u)// 連鎖数を倍率に使う
				{
					if(ADPCM_SNS() == 0){	/* 効果音停止中 */
						s_Clear_StepCount++;
					}

					if(s_Clear_StepCount >= 20)
					{
						s_Clear_StepCount = 0;
					
						SetTaskInfo(SCENE_GAME2_1);	/* シーン設定 */
					}
				}
				APL_ZKP_print_board();
				break;
			}
			case SCENE_GAME2_1:
			{
				/* 詰め込む */
				if(APL_ZKP_apply_gravity_step() == 0u)
				{
					SetTaskInfo(SCENE_GAME2_2);	/* シーン設定 */
				}
				APL_ZKP_print_board();
				break;
			}
			case SCENE_GAME2_2:
			{
				/* 空きを埋める */
				if(APL_ZKP_refill_board_step() == 0u)
				{
					SetTaskInfo(SCENE_GAME2_3);	/* シーン設定 */
				}
				APL_ZKP_print_board();
				break;
			}
			case SCENE_GAME2_3:
			{
#if 0				
				/* 強制入れ替え st */
				if(APL_ZKP_KEY_Input(&y1, &x1, &y2, &x2) != 0u)
				{
					ADPCM_Play(SE_EXCHANGE);
				}
				APL_ZKP_swap_force(y1, x1, y2, x2);
				APL_ZKP_SP_cursor_view(y1, x1, y2, x2);	/* カーソル表示 */
				/* 強制入れ替え ed */
#endif
				APL_ZKP_print_board();

				SetTaskInfo(SCENE_GAME2);	/* シーン設定 */
				break;
			}
			case SCENE_GAME2_E:
			{

				if(APL_ZKP_has_valid_moves() == 0){

					S_Set_Combo(0);	/* コンボカウンタ リセット */

					S_Add_Score_Point(1000);	/* 1000点 */

#if CNF_MACS
					PCG_OFF();			/* SP OFF */
					/* 動画 */
					MOV_Play2(MOV_NOMOREMOVE, 0x64);		/* カットイン */
					/* 画面モード再設定 */
					CRTC_INIT(0x100 + 0x0A);	/* 画面モード初期化　グラフィック初期化なし */
					PCG_ON();			/* SP ON */
#else
#endif
					ADPCM_Play(SE_NOMOREMOVE);

					reset = 2;
				}
				else
				{
					if(S_Get_Combo() >= 3)
					{
#if CNF_MACS
						PCG_OFF();			/* SP OFF */
						/* 動画 */
						MOV_Play2(MOV_EXCELLENT, 0x64);		/* カットイン */
						/* 画面モード再設定 */
						CRTC_INIT(0x100 + 0x0A);	/* 画面モード初期化　グラフィック初期化なし */
						PCG_ON();			/* SP ON */
#else
#endif
					}
					S_Set_Combo(0);	/* コンボカウンタ リセット */
					ADPCM_Play(SE_EXCELLENT);
					APL_ZKP_print_board();
				}
				SetTaskInfo(SCENE_GAME1_S);	/* シーン(実施処理)へ設定 */
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME3_S:
			{
				APL_ZKP_print_board();

				ADPCM_Play(SE_MISS);

				SetTaskInfo(SCENE_GAME3);	/* シーン設定 */
				break;
			}
			case SCENE_GAME3:
			{
				if(APL_ZKP_SP_Swap_view(FALSE) != 0u)
				{
					SetTaskInfo(SCENE_GAME3_E);	/* シーン設定 */
				}
				break;
			}
			case SCENE_GAME3_E:
			{
				APL_ZKP_print_board();
				
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					SetTaskInfo(SCENE_GAME1_S);	/* シーン設定 */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME4_S:
			{
				APL_ZKP_print_board();

				Music_Play(BGM_STOP);	/* Stop */

				ADPCM_Play(SE_MISS);

				SetTaskInfo(SCENE_GAME4);	/* シーン設定 */
				break;
			}
			case SCENE_GAME4:
			{
				static int16_t s_wait = GAME_STOP_COUNT;

				APL_ZKP_print_valid_moves();	/* 候補表示 */
				s_wait--;
				
				if(s_wait <= 0)	/* 効果音停止中 */
				{
					s_wait = GAME_STOP_COUNT;
					SetTaskInfo(SCENE_GAME4_E);	/* シーン設定 */
				}
				break;
			}
			case SCENE_GAME4_E:
			{
				if(ADPCM_SNS() == 0)	/* 効果音停止中 */
				{
					APL_ZKP_ALL_clear();
					APL_ZKP_print_board();
					APL_ZKP_SP_cursor_view_off();	/* カーソルOFF */
					
					T_Fill(48, 40, 208, 4, 0xFFFF, 0);	/* 消去 */

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
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_HI_SCORE_S:	/* ハイスコアランキングシーン(開始処理) */
			{
				T_Clear();	/* テキストクリア */

				G_VIDEO_PRI(0);	/* GR>TX>SP */

				G_PAGE_SET(0b0001);	/* GR0 */
				
				G_PaletteSetZero();
				G_CLR_AREA(0, 256, 0, 256, 0);	/* ページ 0 */
				G_CLR_AREA(0, 256, 0, 256, 1);	/* ページ 1 */

				rank = S_Score_Board();	/* ハイスコア表示 */
				
				if(rank >= 0)
				{
					Music_Play(BGM_HISCORE);	/* Hi-SCORE */
				}
				else
				{
					Music_Play(BGM_SCORE);	/* SCORE */
				}

				SetTaskInfo(SCENE_HI_SCORE);	/* ハイスコアランキングシーンへ設定 */
				break;
			}
			case SCENE_HI_SCORE:	/* ハイスコアランキングシーン */
			{
				if(rank >= 0)
				{
					if(S_Score_Name_Main(rank) >= 3)	/* スコアネーム入力 */
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* ハイスコアランキングシーン(終了処理)へ設定 */
					}
				}
				else
				{
					if(g_ubDemoPlay == FALSE)	/* デモ以外 */
					{
						if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
							((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
							((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
						{
							ADPCM_Play(SE_OK);	/* 決定音 */
							SetTaskInfo(SCENE_HI_SCORE_E);	/* ハイスコアランキングシーン(終了処理)へ設定 */
						}
					}
					else
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* ハイスコアランキングシーン(終了処理)へ設定 */
					}
				}
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
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME_OVER_S:
			{
				int16_t x, y;

				APL_ZKP_SP_cursor_view_off();	/* カーソルOFF */

				T_Clear();	/* テキストクリア */

				if(g_ubDemoPlay == FALSE)	/* デモ以外 */
				{
					Music_Play(BGM_GAME_OVER);	/* Game Over */
					ADPCM_Play(SE_GAME_OVER);
				}
				else
				{
					G_VIDEO_PRI(0);	/* GR>TX>SP */

					G_PAGE_SET(0b0001);	/* GR0 */

					G_CLR_AREA(0, 255, NAME_INP_Y, 255, 0);	/* ページ 0 */
					G_CLR_AREA(0, 256, 0, 256, 1);	/* ページ 1 */
					
					S_Score_Board();	/* ハイスコア表示 */

					GetNowTime(&s_DemoTime);	/* デモ開始カウンタリセット */
				}

				x = stCRT.view_offset_x + 76;
				y = stCRT.view_offset_y + 0;
				Draw_Fill( x, y + 8, x + 12*9, y + 24*1 + 6, G_BLACK);	/* 塗りつぶし */
				Draw_Box( x, y + 8, x + 12*9, y + 24*1 + 6, G_D_RED, 0xFFFF);	/* 枠 */
				PutGraphic_To_Symbol24("GAME OVER", x + 1, y + 1 + 8, G_RED);
				PutGraphic_To_Symbol24("GAME OVER", x, y + 8, G_L_PINK);

				SetTaskInfo(SCENE_GAME_OVER);	/* シーン(実施処理)へ設定 */
				break;
			}
			case SCENE_GAME_OVER:
			{
				/* 背景処理 */
				if(g_ubDemoPlay == FALSE)	/* デモ以外 */
				{
					if(	((GetInput_P1() & JOY_A ) != 0u)	||	/* Aボタン */
						((GetInput() & KEY_b_Z) != 0u)		||	/* KEY A(z) */
						((GetInput() & KEY_b_SP ) != 0u)	)	/* スペースキー */
					{
						ADPCM_Play(SE_OK);	/* 決定音 */
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
					G_CLR_AREA(0, 255, 0, 255, 0);	/* ページ 0 */
					G_CLR_AREA(0, 256, 0, 256, 1);	/* ページ 1 */
					SetTaskInfo(SCENE_TITLE_S);	/* シーン(初期化処理)へ設定 */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			default:	/* 異常シーン */
			{
				ADPCM_Play(SE_GAME_END);

				loop = 0;	/* ループ終了 */
				break;
			}
		}
		SetFlip(TRUE);	/* フリップ許可 */

		PCG_END_SYNC(g_Vwait);	/* スプライトの出力 */

		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */
		g_unFrameCount++;	/* 16bit フリーランカウンタ更新 */

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
	uint32_t nNum;
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init 開始");
#endif
#ifdef MEM_MONI	/* デバッグコーナー */
	g_nMaxUseMemSize = GetFreeMem();
	printf("FreeMem(%d[kb])\n", g_nMaxUseMemSize);	/* 空きメモリサイズ確認 */
	puts("App_Init メモリ");
#endif

	Set_SupervisorMode();	/* スーパーバイザーモード */
	/* MFP */
	MFP_INIT();	/* V-Sync割り込み等の初期化処理 */
	Set_UserMode();			/* ユーザーモード */
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

	Music_Play(BGM_INIT);	/* Init */
	Music_Play(BGM_STOP);	/* Stop */
	Music_Play(BGM_LOAD);	/* Loading */

	/* スーパーバイザーモード開始 */
	Set_SupervisorMode();

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
	G_PaletteSetZero();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init グラフィック");
#endif

	/* スプライト／ＢＧ */
	PCG_INIT(PCG_NUM_MAX);	/* スプライト／ＢＧの初期化 */
	PCG_INIT_CHAR();	/* スプライト＆ＢＧ定義セット */
    PCG_VIEW(1);        /* SP ON / BG0 OFF / BG1 OFF */
	BG_OFF(0);				
	BG_OFF(1);				
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

#if 1
	/* スコア初期化 */
	S_All_Init_Score();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_Init S_All_Init_Score");
#endif
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
	Set_UserMode();
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit スーパーバイザーモード終了");
#endif
	/* 音楽 */
	Exit_Music();			/* 音楽停止 */
#ifdef DEBUG	/* デバッグコーナー */
	puts("App_exit Music");
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
	if(stTask.b96ms == TRUE)	/* 96ms周期 */
	{
		if(GetJoyAnalogMode() == TRUE)	/* アナログ入力 */
		{
			if(Input_Main(g_ubDemoPlay) != 0u) 	/* 入力処理 */
			{
				g_ubDemoPlay = FALSE;	/* デモ解除 */

				SetTaskInfo(SCENE_INIT);	/* シーン(初期化処理)へ設定 */
			}
		}
		else	/* デジタル入力 */
		{
		}
#if 1
		switch(stTask.bScene)
		{
			case SCENE_GAME1_S:
			case SCENE_GAME1:
			case SCENE_GAME1_E:
			case SCENE_GAME2_S:
			case SCENE_GAME2:
			case SCENE_GAME2_0:
			case SCENE_GAME2_1:
			case SCENE_GAME2_2:
			case SCENE_GAME2_3:
			case SCENE_GAME2_4:
			case SCENE_GAME2_E:
			case SCENE_GAME3_S:
			case SCENE_GAME3:
			case SCENE_GAME3_E:
			{
				S_Main_Score();	/* スコア表示 */

				break;
			}
			default:
				break;
		}
#endif
	}

	if(stTask.b496ms == TRUE)	/* 496ms周期 */
	{
		switch(stTask.bScene)
		{
			case SCENE_GAME1_S:
			case SCENE_GAME1:
			case SCENE_GAME1_E:
			case SCENE_GAME2_S:
			case SCENE_GAME2:
			case SCENE_GAME2_0:
			case SCENE_GAME2_1:
			case SCENE_GAME2_2:
			case SCENE_GAME2_3:
			case SCENE_GAME2_4:
			case SCENE_GAME2_E:
			case SCENE_GAME3_S:
			case SCENE_GAME3:
			case SCENE_GAME3_E:
			{
#ifdef FPS_MONI	/* デバッグコーナー */
//				int8_t	sBuf[8];
#endif
				uint32_t cal;
				uint32_t time_now;
				GetNowTime(&time_now);
				cal = (time_now - s_PassTime);
				cal = Mmax(0, cal);
				T_Fill(48, 40, 160 - ((160 * cal) / GAMEPLAY_TIME), 4, 0xFFFF, T_YELLOW);
				T_Fill(Mmax(48,(208 - ((160 * cal) / GAMEPLAY_TIME))), 40, ((160 * cal) / GAMEPLAY_TIME), 4, 0xFFFF, T_RED);

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
	ST_TASK stTask;
//	puts("App_VsyncProc");
#if CNF_VDISP
	Timer_D_Less_NowTime();

	GetTaskInfo(&stTask);	/* タスク取得 */
	/* ↓↓↓ この間に処理を書く ↓↓↓ */

	if(GetJoyAnalogMode() == TRUE)	/* アナログ入力 */
	{
	}
	else	/* デジタル入力 */
	{
		if(Input_Main(g_ubDemoPlay) != 0u) 	/* 入力処理 */
		{
			g_ubDemoPlay = FALSE;	/* デモ解除 */

			SetTaskInfo(SCENE_INIT);	/* シーン(初期化処理)へ設定 */
		}
	}

	switch(stTask.bScene)
	{
		case SCENE_TITLE:
		{
			{
				int8_t x;
				int8_t y;
				int8_t left;
				int8_t right;
				
				Mouse_Cur_Off();
				Mouse_GetDataPos(&x, &y, &left, &right);

				if(x == 0)
				{

				}
				else if(x > 0)
				{
					SetInput(KEY_RIGHT);
				}
				else{
					SetInput(KEY_LEFT);
				}

				if(y == 0)
				{

				}
				else if(y > 0)
				{
					SetInput(KEY_LOWER);
				}
				else{
					SetInput(KEY_UPPER);
				}

				if(left != 0)
				{
					SetInput(KEY_b_X);
				}

				if(right != 0)
				{
					SetInput(KEY_b_Z);
				}


				//Mouse_SetPos(g_stPlayer.x, g_stPlayer.y);
			}
			break;
		}
		default:	/* 異常シーン */
		{
			break;
		}
	}
	/* ↑↑↑ この間に処理を書く ↑↑↑ */
	
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
