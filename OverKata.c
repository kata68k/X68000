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
#include "inc/ZMUSIC.H"

#include "OverKata.h"
#include "APL_Math.h"
#include "Course_Obj.h"
#include "DMAC.h"
#include "Draw.h"
#include "EnemyCAR.h"
#include "FileManager.h"
#include "Graphic.h"
#include "Input.h"
#include "MFP.h"
#include "MyCar.h"
#include "Output_Text.h"
#include "PCG.h"
#include "Task.h"
#include "Text.h"
#include "Trap14.h"

/* グローバル変数 */
SI	nSuperchk = 0;
SI	nCrtmod = 0;
SS	moni = 0;
SS	moni_MAX = 0;
SS	speed = 0;
UC	g_mode = 0;
UC	g_mode_rev = 1;

/* グローバル構造体 */
ST_ROADDATA	stRoadData[1024] = {0};
ST_TASK		stTask = {0}; 
ST_TEXTINFO	stTextInfo = {0};

/* 関数のプロトタイプ宣言 */
SS main(void);
void App_Init(void);
void App_exit(void);
SS BG_main(UC*);

/* 関数 */
SS main(void)
{
	FLOAT	rad;

	UI	unStart_time;
	UI	unTime_cal = 0;
	UI	unTime_cal_PH = 0;
	UI	unRoadAnimeTime;
	
	US	x,y;
	US	uRas_tmp[256] = {0};
	US	uRas_tmp_sub[256] = {0};
	US	uRas_debug[256] = {0};
	US	uPal_tmp[256] = {0};
	US	uFreeRunCount=0;
	US	uRas_st, uRas_mid, uRas_ed, uRas_size;
	US	uDebugNum;
	US	uRoadAnime, uRoadAnime_old, uRoadAnimeBase, uRoadAnimeBase_old;
	US	uRoadAnimeCount;
	US	uRoadCycleCount;
	US	uRoadCycleCountLamp;
	US	uCounter;
	US	BGprocces_ct = 0;
	
	SS	i;
	SS	loop;
	SS	RD[1024] = {0};
	SS	Horizon, Horizon_tmp, Horizon_ras, Horizon_offset;
	SS	vx, vy;
	SS	r_height;
	SS	road_offset_x, road_offset_y, road_offset_val;
	SS	road_height, road_height_old;
	SS	road_slope, road_slope_old;
	SS	road_angle, road_angle_old, road_curve;
	SS	road_distance, road_distance_old;
	SS	road_object;
	SS	view_offset_x, view_offset_y;
	SS	hide_offset_x, hide_offset_y;
	SS	BG_offset_x, BG_offset_y;
	SS	BG_under;
	SS	input;
	SS	rpm, Vs, speed_min;
	SS	cal_tan = 0;
	SS	cal_cos = 0;
	
	UC	bRoad_flag = FALSE;
	UC	bUpDown_flag = FALSE;
	UC	bMode_flag = FALSE;
	UC	bKeyUP_flag = FALSE;
	UC	bKeyDOWN_flag = FALSE;
	UC	bDebugMode;
	UC	bDebugMode_flag;
	UC	bShiftPosFlag;
	UC	bRoad_Anime_flag;
	UC	bRoadCycleCountRst = FALSE;
	UC	bRoadCycleCountLamp = TRUE;
	UC	bFlip = TRUE;

	ST_CARDATA	stMyCar = {0};
	ST_CRT		stCRT = {0};
	ST_RAS_INFO	stRasInfo = {0};
	
	usr_abort = App_exit;	/* 例外発生で終了処理を実施する */
	
	init_trap14();	/* デバッグ用致命的エラーハンドリング */
	
	/* デバッグコーナー */
#if 0
	/* ↓自由にコードを書いてね */
	rad = APL_Atan2(1,1);
	printf("%f\n", rad);
	/* ↑自由にコードを書いてね */
	loop = 1;
	do
	{
		if( ( BITSNS( 0 ) & 0x02 ) != 0 ) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;
	}
	while( loop );
	
	exit(0);
#endif

	App_Init();	/* 初期化 */
	
	/* 変数の初期化 */
	speed = 0;
	road_offset_x = 0;
	road_offset_y = 0;
	road_offset_val = 0;
	road_angle = 0;
	road_height = 0;
	road_height_old = road_height;
	road_slope = 0;
	road_slope_old = road_slope;
	road_distance = 0;
	road_distance_old = road_distance;
	road_object = 0;
	r_height = 0;
	g_mode = 1;
	stMyCar.uEngineRPM = 750;	/* アイドル回転数 */
	stMyCar.ubShiftPos = 1;		/* 1速 */
	rpm = 0;
	Vs = 0;
	speed = 0;
	loop = 1;
	vx = vy =0;
	uFreeRunCount = 0;
	uRoadAnimeBase = 0;
	uRoadAnimeBase_old = 0;
	uRoadAnime = 0;
	uRoadAnime_old = 0;
	uRoadAnimeCount = 0;
	uRoadCycleCount = 0;
	uRoadCycleCountLamp = 0;
	bRoad_Anime_flag = FALSE;
	bDebugMode = TRUE;
	uDebugNum = 0x80;
	uCounter = 0;
	stTextInfo.uScoreMax = 10000;
	
	uRas_size = 0xFFFF;
	
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
	
	/* ライバル車の初期化 */
	InitEnemyCAR();
	/* コースのオブジェクトの初期化 */
	InitCourseObj();
	
	/* 音楽 */
	m_stop(0,0,0,0,0,0,0,0,0,0);	/* 音楽停止 */
//	zmd_play("data\\music\\PT034MK.ZMD");	/* BGM */
//	zmd_play("data\\music\\PT002MK.ZMD");	/* BGM */
//	zmd_play("data\\music\\kyaraban.zmd");	/* BGM */
//	zmd_play("data\\music\\KATAYS3X.zmd");	/* BGM */
//	zmd_play("data\\music\\STSL_PUP.zmd");	/* BGM */
//	zmd_play("data\\music\\1943_0020.zmd");	/* BGM */
//	zmd_play("data\\music\\FZ_BIG.zmd");	/* BGM */
	
	GetNowTime(&unStart_time);		/* Time Count用 */
	
#if 0
	{
		/* アドレスエラー発生 */
		char buf[10];

		int *A = (int *)&buf[1];
		int B = *A;
		return B;
	}
#endif

	do
	{
		UI time_st, time_now;
		
		GetNowTime(&time_st);	/* メイン処理の開始時刻を取得 */
		SetStartTime(time_st);	/* メイン処理の開始時刻を記憶 */
		
		/* タスクの情報を得る */
		GetTaskInfo(&stTask);

		/* 入力処理 */
		input = get_key(1);	/* キーボード＆ジョイスティック入力 */
		if((input & KEY_b_Q   ) != 0u) loop = 0;	/* Ｑで終了 */
		if((input & KEY_b_ESC ) != 0u) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;
		
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
		if(ChatCancelSW((input & KEY_b_SP)!=0u, &bDebugMode_flag) == TRUE)	/* スペースでデバッグON/OFF */
		{
			if(bDebugMode == FALSE)	bDebugMode = TRUE;
			else					bDebugMode = FALSE;
		}
#endif
		/* モード切替による設定値の変更 */
		GetCRT(&stCRT, g_mode);
		view_offset_x	= stCRT.view_offset_x;
		view_offset_y	= stCRT.view_offset_y;
		hide_offset_x	= stCRT.hide_offset_x;
		hide_offset_y	= stCRT.hide_offset_y;
		BG_offset_x		= stCRT.BG_offset_x;
		BG_offset_y		= stCRT.BG_offset_y;
		BG_under		= stCRT.BG_under;
		
#ifdef DEBUG	/* デバッグコーナー */
		/* テスト用入力 */
		if(bDebugMode == TRUE)
		{
			/* キーボードから数字を入力 */
			DirectInputKeyNum(&uDebugNum, 3);
			/* キー操作 */
#if 1
			/* 角度変更 */
			//if(ChatCancelSW((input & KEY_UPPER)!=0u, &bKeyUP_flag) == TRUE)	vy += 1;	/* 上 */
			//if(ChatCancelSW((input & KEY_LOWER)!=0u, &bKeyDOWN_flag) == TRUE)	vy -= 1;	/* 下 */
			if((input & KEY_UPPER)!=0u)	vy += 1;	/* 上 */
			if((input & KEY_LOWER)!=0u)	vy -= 1;	/* 下 */
#else
			/* ランプ操作 */
			if(bUpDown_flag == 0){
				vy += 1;
				if(vy > 45)
				{
					vy = 0;
					bUpDown_flag = 1;
				}
			}
			else{
				vy -= 1;
				if(vy < -45)
				{
					vy = 0;
					bUpDown_flag = 0;
				}
			}
#endif
			vy = Mmax(Mmin(vy, 45), -45);

			/* 高さ */
			if((input & KEY_b_RLUP)!=0u)	r_height += 1;	/* 上 */
			if((input & KEY_b_RLDN)!=0u)	r_height -= 1;	/* 下 */
			r_height = Mmax(Mmin(r_height, 31), -32);
		}
#endif
		/* 自車の情報を取得 */
		UpdateMyCarInfo(input);		/* 自車の情報を更新 */
		GetMyCar(&stMyCar);			/* 自車の情報を取得 */
		rpm			= stMyCar.uEngineRPM;
		Vs			= stMyCar.VehicleSpeed;
		
		if( (stMyCar.ubBrakeLights == TRUE)		/* ブレーキランプON */
		||  (stMyCar.ubShiftPos == 0)		)	/* ニュートラル */
		{
			speed_min = 0;
		}
		else
		{
			speed_min = 1;
		}

		/* 車速（ゲーム内） */
		speed = Vs >> 3;	/* 1LSB 10km/h */
		speed = Mmax(Mmin(speed, 31), speed_min);
		
		/* ロードパターン */
		if(speed == 0u)	/* 車速0km/h */
		{
			uRoadAnime = uRoadAnime_old;
			uRoadAnimeBase = uRoadAnimeBase_old;
			uRoadAnimeCount = 0;
			bRoad_Anime_flag = FALSE;
			
			GetStartTime(&unRoadAnimeTime);	/* 開始時刻を取得 */
		}
		else if( (time_st - unRoadAnimeTime) < (362 / speed) )	/* 更新周期未満 */
		{
			/* センターラインの長さと間隔は５ｍ */
			/* 60fps=17ms/f */
			/* 最大速度307km/h(85m/s,0.0853m/ms) 1f=1.45m進む */
			/* 最低速度6km/h(1.7m/s,0.0017m/ms) 1f=0.028m進む */
			/* 1LSB 10km/h(2.8m/s,0.0028m/ms) 1f=0.047m進む */
			/* 1times Cal 1m=4dot (17ms/f x 1/0.047 / speed) = (362/speed)*/
			uRoadAnime = uRoadAnime_old;
			uRoadAnimeBase = uRoadAnimeBase_old;
			uRoadAnimeCount = 0;
			bRoad_Anime_flag = FALSE;
		}
		else
		{
			GetStartTime(&unRoadAnimeTime);	/* 開始時刻を取得 */

			uRoadAnimeCount = 0xFFF;	/* 逆走アニメーション防止 */

			uRoadAnime = 0;
			bRoad_Anime_flag = TRUE;

			uRoadAnimeBase++;
			if(uRoadAnimeBase >= 4)
			{	
				uRoadAnimeBase = 0;
			}
			
			if(bRoadCycleCountRst == TRUE)
			{
				bRoadCycleCountRst = FALSE;
				/* 角度算出 */
				road_height_old = road_height;	/* 前回値更新 */
				/* コースデータ読み込み */
				road_height = (SS)(0x80 - stRoadData[uCounter].bHeight);	/* 道の標高	(0x80センター) */
				road_height = Mmax(Mmin(road_height, 31), -32);
				//road_height = r_height;	/* デバッグ入力 */
				road_angle = (SS)stRoadData[uCounter].bAngle - 0x80;		/* 道の角度	(0x80センター) */
				//road_angle = Mmax(Mmin(road_angle, 12), -12);
				//road_angle = 0;	/* デバッグ入力 */
				road_object = stRoadData[uCounter].bObject;					/* 出現ポイントのオブジェクトの種類 */
				if(road_object != 0u)
				{
					SetAlive_EnemyCAR();	/* ライバル車を出現させる */
				}
				
				if(road_height == road_height_old)
				{
				}
				else if(road_height > road_height_old)
				{
					/* 保持 */
				}
				else
				{
					/* 保持 */
				}
				
				uRoadCycleCount = 0;
				
				if(bRoadCycleCountLamp == TRUE)
				{
					bRoadCycleCountLamp = FALSE;
				}
				else
				{
					bRoadCycleCountLamp = TRUE;
				}

				uCounter++;			/* カウンタ更新 */
				if(uCounter >= 896)uCounter = 0;
			}
			else
			{
				uRoadCycleCount += 1;
				if(uRoadCycleCount > (uRas_size - 16))
				{
					bRoadCycleCountRst = TRUE;
				}
				
				if(bRoadCycleCountLamp == TRUE)
				{
					uRoadCycleCountLamp += 1;
				}
				else
				{
					uRoadCycleCountLamp -= 1;
				}
			}
		}
		SetRoadCycleCount(uRoadCycleCount);
		
		if(bRoad_Anime_flag == TRUE)
		{
			road_distance = uRoadCycleCountLamp;

			// 高低差から傾きを出す(-45～+45) rad = APL_Atan2( road_slope_old - road_slope,  );
			if(road_height != road_height_old)
			{
				road_slope_old = road_slope;	/* 前回値更新 */
				road_slope = road_height * -1;

				/* 角度からtanθ*/
#if 1
				if(road_slope == 0)
				{
					cal_tan = 0;
					cal_cos = 0;
				}
				else
				{
					if(road_slope != road_slope_old)
					{
						cal_tan = APL_Tan256(road_slope);	/* tanθ */
						cal_cos = APL_Cos(cal_tan);			/* cosθ */
					}
				}
#else
				/* 底辺と高さから角度を求めて、角度からY座標を演算していたが*/
				/* 角度演算は不要とのこと。単純に距離と角度から高さを算出できる @runshiwaさんより指摘 */
				if(vy == 0)
				{
					rad = 0.0;
				}
				else
				{
					rad = APL_Atan2( vy, uRas_mid );
				}
				cal_tan = APL_Tan256((SS)rad);
#endif
			}
			else{
				/* 前回値保持 */
			}
		}
		
		/* 空のコントロール */
		uPal_tmp[1] = BG_offset_y;
		
		/* コーナーの表現 */
#if 1
#ifdef DEBUG	/* デバッグコーナー */
		if(bDebugMode == TRUE)
		{
		}
		else
#endif
		{
		}
#else
		else
		{
			/* ランプ操作 */
			if( ((uFreeRunCount % (RD[uFreeRunCount & 0x03FFu])) == 0)
				&& ((uFreeRunCount % 3) == 0)
				&& (speed != 0u) )
			{
				if(bRoad_flag == 0){
					road_angle += 1;
					if(road_angle > 12)bRoad_flag = 1;
				}
				else{
					road_angle -= 1;
					if(road_angle < -12)bRoad_flag = 0;
				}
			}
		}
#endif

		/* ステアリング操作の更新 */
		if( ((uFreeRunCount % 4) == 0)
			&& (speed != 0u) )
		{
			stMyCar.Steering = stMyCar.Steering + ((SS)(road_angle * speed) >> 6);	/* バランス調整要 */
			vx = stMyCar.Steering;
		}
		else{
			/* 前回値保持 */
		}

		/* ラスター処理 */
#if 1
//		if(stTask.b8ms == TRUE)
		{
//			memset(&uPal_tmp[0], 0, sizeof(uPal_tmp));
//			memset(&uRas_tmp[0], 0, sizeof(uRas_tmp));

			/* 水平線 */
			/* ラスター割り込み処理の開始・終了位置 */
			/* 角度と距離と標高から水平線の位置を算出する。 */
			/* 角度と距離で求めた高さと標高が一致していれば、リニアに伸縮 */
			/* 角度と距離で求めた高さより標高が大きい場合、途中でカットされる道 */
			/* 角度と距離で求めた高さより標高が小さい場合、短く表現する道 */
			switch(g_mode)
			{
				case 0:		/* TPS */
				{
					Horizon_tmp = Y_HORIZON_0-16;		/* 仮水平位置 */
					Horizon = Mmax(Mmin(Horizon_tmp + road_height, Y_HORIZON_0-RASTER_NEXT), RASTER_MIN);	/* 水平位置 */
					uRas_ed = Mmin(V_SYNC_MAX, RASTER_MAX);	/* ラスター終了位置 */
					break;
				}
				case 1:		/* FPS */
				default:	/* FPS */
				{
					Horizon_tmp = Y_HORIZON_1;		/* 仮水平位置 */
					Horizon = Mmax(Mmin(Horizon_tmp + road_height, Y_HORIZON_1+32-RASTER_NEXT), RASTER_MIN);	/* 水平位置 *//* 30 = ROAD_SIZE*1/3-RASTER_NEXT */
					uRas_ed = Mmin(Y_HORIZON_1 + RASTER_MIN + ROAD_SIZE, V_SYNC_MAX);	/* ラスター終了位置 */
					break;
				}
			}
			Horizon_offset = ROAD_POINT - Horizon_tmp;	/* 実際のデータと表示位置との差異 */
			uRas_st = Mmax(Mmin(Horizon + RASTER_MIN, RASTER_MAX), RASTER_MIN);	/* ラスター開始位置 */
//			uRas_st += (uRas_st % 2);	/* 偶数にする */
//			uRas_ed -= (uRas_ed % 2);	/* 偶数にする */
			uRas_size = uRas_ed - uRas_st;			/* ラスターの範囲 */
			uRas_mid = Mmin((uRas_st + road_distance), uRas_ed - 16);	/* 水平線の位置が変化しないポイント *//* ハイウェイスターなら 64 = ROAD_SIZE*2/3 */
			uPal_tmp[0] = uRas_st;				/* 割り込み位置の設定 */
			
			stRasInfo.st = uRas_st;
			stRasInfo.mid = uRas_mid;
			stRasInfo.ed = uRas_ed;
			stRasInfo.size = uRas_size;
			stRasInfo.horizon = Horizon;
			SetRasterInfo(stRasInfo);
			
			road_curve = road_angle * uRas_size;
			
			for(y=uRas_st; y < uRas_ed; y++)
			{
				SS	num, rev;
				SS	ras_cal_x, ras_cal_y, ras_pat, ras_result;
				SS	col;
				SS	uBG_pat[4][4] = {288,   0,  96, 192, 
					 				 192, 288,   0,  96, 
					 				  96, 192, 288,   0, 
					 				   0,  96, 192, 288} ;
				
				num = y - uRas_st;			/* 0 -> uRas_size */
				rev = uRas_ed - y;			/* uRas_size -> 0 */
				
				/* X座標 */
				if(num == 0u)
				{
					ras_cal_x = 0;	/* 最初 */
				}
#if 0
				else if( y < uRas_mid )				/* 水平線の位置が変化 *//* 64 = ROAD_SIZE*2/3 */
				{
					ras_cal_x = ((num * vx) >> 4) + (road_angle * (uRas_mid - uRas_st) / num);
				}
#endif
				else
				{
					ras_cal_x = ((num * vx) >> 4) + (road_curve / num);
				}
#ifdef DEBUG	/* デバッグコーナー */
				if(bDebugMode == TRUE)	/* デバッグモード */
				{
					Draw_Pset(	view_offset_x + (WIDTH >> 1) - ras_cal_x,
								view_offset_y + Horizon + num,
								0xC2);	/* デバッグ用グラフ*/
				}
#endif
//				if(ras_cal_x <   0)ras_cal_x += 512;
//				if(ras_cal_x > 512)ras_cal_x -= 512;
				uRas_tmp[y] = (US)ras_cal_x;

				/* Y座標 */
				if(num == 0u)	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
				{
					ras_cal_y = 0;
					ras_pat = uBG_pat[3][0];
					road_offset_y = 0;
					road_offset_val = 0;
					uRoadAnime_old = uRoadAnime;
					uRoadAnimeBase_old = uRoadAnimeBase;

					col = 0xBB;
				}
#if 1
				else if( y >= (uRas_ed - 16))		/* 下側のはみ出し部分を補完 */
				{
					ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
					ras_cal_y = BG_under;

					col = 0xC2;
				}
				else if( y < uRas_mid )				/* 水平線の位置が変化 *//* 64 = ROAD_SIZE*2/3 */
				{
					SS	Road_strch;
					SS	offset_size;

					offset_size = uRas_mid - y;	/* uRas_size -> 0 */
					
					Road_strch = (offset_size * cal_tan) >> 8;	/* 高さ＝底辺×tanθ */
//					Road_strch = (offset_size * APL_Cos(num)) >> 8;
//					Road_strch = (offset_size * APL_Sin(num)) >> 8;
					ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
					ras_cal_y = Horizon_offset + Road_strch + ((SS)uDebugNum - 0x80);

					col = 0xB7;
				}
				else				/* 平坦 */
				{
					ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
					ras_cal_y = Horizon_offset;

					col = 0xFF;
				}
#else
				else
				{
					SS	Road_strch;
					SS	offset_size;

					offset_size = uRas_ed - y;	/* uRas_size -> 0 */
//					offset_size = uRas_mid - y;	/* uRas_size -> 0 */
					
//					Road_strch = (offset_size * cal_tan) >> 8;	/* 高さ＝底辺×tanθ */
//					Road_strch = (offset_size * APL_Cos(num<<1)) >> 8;
					Road_strch = (offset_size * APL_Sin(num<<1)) >> 8;
					ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
					ras_cal_y = Horizon_offset + Road_strch + ((SS)uDebugNum - 0x80);

					col = 0xB7;
				}
#endif
				/* ロードパターン*/
				road_offset_y = (num * road_offset_val) / (road_offset_val + 96);
				uRas_tmp_sub[y] = road_offset_y;

				uRoadAnimeCount++;
				if(uRoadAnimeCount >= road_offset_y)
				{
					uRoadAnimeCount = 0;
					uRoadAnime++;
					road_offset_val++;
				}
				if(uRoadAnime >= 4u){uRoadAnime = 0;}

#ifdef DEBUG	/* デバッグコーナー */
				if(bDebugMode == TRUE)	/* デバッグモード */
				{
					Draw_Line(uRas_st + view_offset_x, (Horizon+ view_offset_y)+1, uRas_ed + view_offset_x, (Horizon+ view_offset_y)+1, 0x35, 0xFFFF);
					Draw_Pset(y + view_offset_x, (Horizon + RASTER_MIN + view_offset_y) - ras_cal_y, col);	/* デバッグ用グラフ*/
					uRas_debug[y] = ras_cal_y;
				}
#endif
				ras_result = ras_cal_y + ras_pat;
				if(ras_result <   0)ras_result = 0;
				if(ras_result > 511)ras_result = 0;
				
				uPal_tmp[y] = (US)ras_result;
				
			}
			SetRasterVal(uRas_tmp, sizeof(US)*RASTER_MAX);
			SetRasterPal(uPal_tmp, sizeof(US)*RASTER_MAX);
		}
#endif
		/* テキスト画面の処理 */
		if(stTask.b496ms == TRUE)
		{
			UI unPassTime, unTimer;
			US uTimeCounter;
			/* Score */
			stTextInfo.uScore = uFreeRunCount;

			/* Top Score */
			stTextInfo.uScoreMax = Mmax(stTextInfo.uScore, stTextInfo.uScoreMax);
			
			/* Time Count */
			GetNowTime(&time_now);
			unPassTime = (time_now - unStart_time);
			unTimer = 120000 - unPassTime;
			if(120000 < unTimer)
			{
				uTimeCounter = 0;
			}
			else
			{
				uTimeCounter = (US)(unTimer / 1000);
			}
			stTextInfo.uTimeCounter = uTimeCounter;
			
			/* Speed */
			stTextInfo.uVs = Vs;
			
			/* Gear */
			stTextInfo.uShiftPos = (US)stMyCar.ubShiftPos;

			/* 描画 */
			PutTextInfo(stTextInfo);
		}
		
		if( Mabs(vx) > 25 )	/* コース外 */
		{
			/* コースアウト時の処理 */
			stMyCar.uEngineRPM = (stMyCar.uEngineRPM>>1) + (stMyCar.uEngineRPM>>2)  + (stMyCar.uEngineRPM>>3);	/* 減速処理 */
			MyCar_CourseOut(rpm);	/* コースアウト時のエフェクト */
		}
		
		/* グラフィック画面の処理 */
		MyCar_Interior(view_offset_x, view_offset_y);	/* 自車のインテリア処理 */

		/* 余った時間で処理 */
		BG_main(&bFlip);	/* バックグランド処理 */
		
		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */

#ifdef DEBUG	/* デバッグコーナー */
//		BG_TextPut("OverKata", 4, 10);
//		BG_TextPut("OverKata", 128, 128);
//		BG_TextPut("OVER KATA", 128, 128);

		if(bDebugMode == TRUE)
		{
			if(stTask.b496ms == TRUE)
			{
				/* モニタ */
				Message_Num(&uCounter,	 		12,  6, 2, MONI_Type_US, "%4d");
				
				Message_Num(&unTime_cal,	 	 0,  8, 2, MONI_Type_UI, "%3d");
				Message_Num(&unTime_cal_PH,		 6,  8, 2, MONI_Type_UI, "%3d");
//				Message_Num(&BGprocces_ct,		12,  8, 2, MONI_Type_US, "%3d");
				
//				Message_Num(&speed,				 0,  9, 2, MONI_Type_SS, "%3d");
//				Message_Num(&vx, 				 6,  9, 2, MONI_Type_SS, "%2d");
//				Message_Num(&vy, 				12,  9, 2, MONI_Type_SS, "%2d");
//				Message_Num(&r_height, 			20,  9, 2, MONI_Type_SS, "%3d");
				
				Message_Num(&uRas_st,			 0, 10, 2, MONI_Type_US, "%3d");
				Message_Num(&uRas_mid,			 7, 10, 2, MONI_Type_US, "%3d");
				Message_Num(&uRas_ed,			13, 10, 2, MONI_Type_US, "%3d");
				Message_Num(&uRas_size,			20, 10, 2, MONI_Type_US, "%3d");
				
//				Message_Num(&uPal_tmp[uRas_st],	 0, 11, 2, MONI_Type_US, "%3d");
//				Message_Num(&uRas_tmp[uRas_st],	 6, 11, 2, MONI_Type_US, "%3d");
				Message_Num(&cal_tan,			12, 11, 2, MONI_Type_SS, "%3d");
//				Message_Num(&rad,				12, 11, 2, MONI_Type_FL, "%f");
				
				Message_Num(&road_height,		 0, 12, 2, MONI_Type_SS, "%3d");
				Message_Num(&road_slope,	 	 6, 12, 2, MONI_Type_SS, "%3d");
				Message_Num(&road_distance,		12, 12, 2, MONI_Type_SS, "%3d");
				Message_Num(&road_angle,		20, 12, 2, MONI_Type_SS, "%3d");
				
				Message_Num(&uDebugNum,		 	 0, 13, 2, MONI_Type_US, "%3d");
			}
		}
#endif
		/* 処理時間計測 */
		GetNowTime(&time_now);
		unTime_cal = time_now - time_st;	/* LSB:1 UNIT:ms */
		unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);

		/* 同期待ち */
		vwait(1);
	}
	while( loop );
	
	App_exit();	/* 終了処理 */

#ifdef DEBUG	/* デバッグコーナー */
	printf("uRas_tmp[0]の中身 = %d(uRas_st=%d)\n", uRas_tmp[0], uRas_st );
	for(y=uRas_st; y < uRas_ed; y++)
	{
		printf("[%3d]=(%5d),", y, uRas_tmp[y] );
		//printf("[%3d]=(%5d),", y, uRas_debug[y] );
		if((y%5) == 0)printf("\n");
	}
	printf("\n");
#endif
	printf("処理時間:Real=%3d[ms] PH=%3d[ms]\n", unTime_cal, unTime_cal_PH);
}

void App_Init(void)
{
	UI	unZmusicVer;
	SS	i;
	US	x,y;

	/* サウンド常駐確認 */
	unZmusicVer = zm_ver();
	if(unZmusicVer == 0)	/* 0:常駐ナシ */
	{
		puts("Z-MUSIC Ver2を常駐してください。");
		exit(0);
	}
	if((unZmusicVer&0xF000u)>>12 != 2u)	/* Ver2.0x判定 */
	{
		puts("Z-MUSIC Ver2を常駐してください。");
		exit(0);
	}
#if 0
	if((unZmusicVer&0xFFFF000u)>>16 == 0u)	/* PCM8判定 */
	{
		puts("PCM8Aを常駐してください。");
		exit(0);
	}
#endif	
	/* スーパーバイザーモード開始 */
	/*ＤＯＳのスーパーバイザーモード開始*/
	nSuperchk = SUPER(0);
	if( nSuperchk < 0 ) {
		puts("すでにスーパーバイザーモード");
	} else {
		puts("スーパーバイザーモード開始");
	}
	
	nCrtmod = CRTMOD(-1);	/* 現在のモードを返す */
	
	/* サウンド初期化 */
	m_init();		/* 初期化 */
	m_ch("fm");		/* FM */
	m_stop(0,0,0,0,0,0,0,0,0,0);	/* 音楽停止 */
	zmd_play("data\\music\\X68K.ZMD");	/* ローディングBGM */

	/* コースデータの読み込み */
	File_Load_Course_CSV("data/map/course01.csv", &stRoadData[0], &x, &y);
	
	/* グラフィック表示 */
	G_INIT();		/*画面の初期設定*/
	APICG_DataLoad("data/cg/Over_A.pic"	, X_OFFSET, 	Y_OFFSET,	0);	/* FPS */
	APICG_DataLoad("data/cg/Over_A.pic"	, X_OFFSET,				0,	0);	/* FPS */
//	APICG_DataLoad("data/cg/Over_B.pic"	, X_OFFSET + ((WIDTH>>1) - (MY_CAR_0_W>>1)),  V_SYNC_MAX-RASTER_MIN-MY_CAR_0_H - 16,	0);	/* TPS */
	G_MyCar();		/* 自車の表示 */
	APICG_DataLoad("data/cg/Over_C.pic"	, 0,					0,	1);	/* ライバル車 */
//	APICG_DataLoad("data/cg/Over_D.pic"	, X_OFFSET,	Y_OFFSET +  4,	1);	/* 背景 */
	APICG_DataLoad("data/cg/Over_E.pic"	, 140,					0,	1);	/* ヤシの木 */
	G_Background();	/* 背景の表示 */

	/* スプライト／ＢＧ表示 */
	PCG_INIT();							/* スプライト／ＢＧの初期化 */
	PCG_SP_dataload("data/sp/BG.SP");	/* スプライトのデータ読み込み */
	PCG_PAL_dataload("data/sp/BG.PAL");	/* スプライトのパレットデータ読み込み */
	BG_TEXT_SET("data/map/map.csv");	/* マップデータによるＢＧの配置 */
	
	/* テキスト表示 */
	T_INIT();		/* テキストＶＲＡＭ初期化 */
	T_TopScore();	/* TOP */
	T_Time();		/* TIME */
	T_Score();		/* SCORE */
	T_Speed();		/* SPEED */
	T_Gear();		/* GEAR */
	BG_TextPut("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 232);
	for(i=0; i < 10; i++)
	{
		x = 0;
		y = 240;
		BG_PutToText(   0x80+ (i<<1) + 0, x + BG_WIDTH * i,	y,				BG_Normal, TRUE);	/* 数字大（上側）*/
		BG_PutToText(   0x80+ (i<<1) + 1, x + BG_WIDTH * i,	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 数字大（下側）*/
	}

	/* MFP */
	Init_MFP();						/* MFP関連の初期化 */
	TIMERDST(Timer_D_Func, 7, 20);	/* Timer-Dセット */	/* 50us(7) x 20cnt = 1ms */
	SetNowTime(0);					/* 時間の初期化 */
	VDISPST(Vsync_Func, 0, 1);		/* V-Sync割り込み 帰線 */
}

void App_exit(void)
{
	CRTCRAS((void *)0, 0);		/* stop */
	HSYNCST((void *)0);			/* stop */
	VDISPST((void *)0, 0, 0);	/* stop */
	TIMERDST((void *)0, 0, 1);	/* stop */
	
	BGCTRLST(0, 0, 0);	/* ＢＧ０表示ＯＮ */
	BGCTRLST(1, 1, 0);	/* ＢＧ１表示ＯＮ */
	G_CLR_ON();

	B_CURON();
	TextClear();

	m_stop(0,0,0,0,0,0,0,0,0,0);	/* 音楽停止 */
	m_init();		/* 初期化 */

	CRTMOD(nCrtmod);			/* モードをもとに戻す */
	
	_dos_kflushio(0xFF);	/* キーバッファをクリア */

	/*スーパーバイザーモード終了*/
	SUPER(nSuperchk);
}

SS BG_main(UC* bFlip)
{
	SS	ret = 0;
	UI	time_now;
	UI	time_st;
	US	BGprocces_ct = 0;
	UC	bNum;

	static UC	bFlipState = Clear_G;

	GetStartTime(&time_st);	/* 開始時刻を取得 */

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
	}
	while(1);
	
	return	ret;
}

#endif	/* OVERKATA_C */
