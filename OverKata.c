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
#include "Draw.h"
#include "MFP.h"
#include "MyCar.h"
#include "Input.h"
#include "Output_Text.h"
#include "Task.h"
#include "Trap14.h"

/* グローバル変数 */
SI	nSuperchk = 0;
SI	nCrtmod = 0;
SS	moni = 0;
SS	moni_MAX = 0;
SS	speed = 0;
UC	g_mode = 0;

/* グローバル構造体 */
ST_ROADDATA	stRoadData[1024] = {0};
ST_CARDATA	stMyCar = {0};
ST_TASK		stTask = {0}; 
ST_TEXTINFO stTextInfo = {0};

/* 関数のプロトタイプ宣言 */
SS main(void);
void App_Init(void);
void App_exit(void);

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
	US	uRas_debug[256] = {0};
	US	uPal_tmp[256] = {0};
	US	uFreeRunCount=0;
	US	uCountNum;
	US	uRas_st, uRas_mid, uRas_ed, uRas_size;
	US	uDebugNum;
	US	uRoadAnime, uRoadAnime_old, uRoadAnimeBase, uRoadAnimeBase_old;
	US	uRoadAnimeCount;
	US	uCounter;
	
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
	SS	view_offset_x, view_offset_y;
	SS	BG_offset_x, BG_offset_y;
	SS	BG_under;
	SS	input;
	SS	rpm, Vs, speed_min;
	SS	Vibration;
	SS	cal_tan = 0;
	SS	cal_cos = 0;
	
	UC	bRoad_flag = FALSE;
	UC	bUpDown_flag = FALSE;
	UC	bMode_flag = FALSE;
	UC	bKeyUP_flag = FALSE;
	UC	bKeyDOWN_flag = FALSE;
	UC	bUpdate;
	UC	bDebugMode;
	UC	bDebugMode_flag;
	UC	bShiftPosFlag;
	UC	bRoad_Anime_flag;
	
	UI	unExplosion_time;
	UC	bExplosion;
	
	volatile US *CRTC_R21 = (US *)0xE8002Au;	/* テキスト・アクセス・セット、クリアーP.S */
	volatile US *CRTC_480 = (US *)0xE80480u;	/* CRTC動作ポート */

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
	uCountNum = 0;
	bUpdate = FALSE;
	uRoadAnimeBase = 0;
	uRoadAnimeBase_old = 0;
	uRoadAnime = 0;
	uRoadAnime_old = 0;
	uRoadAnimeCount = 0;
	bRoad_Anime_flag = FALSE;
	Vibration = 0;
	bDebugMode = FALSE;
	uDebugNum = 0x80;
	uCounter = 0;
	stTextInfo.uScoreMax = 10000;

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
	
	/* 音楽 */
	m_stop(0,0,0,0,0,0,0,0,0,0);	/* 音楽停止 */
//	zmd_play("data\\music\\PT034MK.ZMD");	/* BGM */
//	zmd_play("data\\music\\PT002MK.ZMD");	/* BGM */
//	zmd_play("data\\music\\kyaraban.zmd");	/* BGM */
//	zmd_play("data\\music\\KATAYS3X.zmd");	/* BGM */
//	zmd_play("data\\music\\STSL_PUP.zmd");	/* BGM */
	
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
		UI time, time_old, time_now;
		
		GetNowTime(&time_old);
		
		/* タスクの情報を得る */
		GetTaskInfo(&stTask, time_old);

		/* 入力処理 */
		input = get_key(1);	/* キーボード＆ジョイスティック入力 */
		if((input & KEY_b_Q   ) != 0u) loop = 0;	/* Ｑで終了 */
		if((input & KEY_b_ESC ) != 0u) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;
		
		if(ChatCancelSW((input & KEY_b_M)!=0u, &bMode_flag) == TRUE)	/* Ｍでモード切替 */
		{
			if(g_mode == 0u)g_mode = 1u;
			else			g_mode = 0u;
		}
		
		if(ChatCancelSW((input & KEY_b_SP)!=0u, &bDebugMode_flag) == TRUE)	/* スペースでデバッグON/OFF */
		{
			if(bDebugMode == FALSE)	bDebugMode = TRUE;
			else					bDebugMode = FALSE;
		}
		
		if(stTask.b496ms == TRUE)
		{
			/* モード切替による設定値の変更 */
			switch(g_mode)
			{
				case 0:		/* Screen 0(TPS) */
				{
					view_offset_x = X_OFFSET;
					view_offset_y = Y_MIN_DRAW;
					BG_offset_y = 0;
					BG_under = BG_0_UNDER;
					break;
				}
				case 1:		/* Screen 0(FPS) */
				{
					view_offset_x = X_OFFSET;
					view_offset_y = Y_OFFSET;
					BG_offset_y = 32;
					BG_under = BG_1_UNDER;
					break;
				}
				default:
				{
					view_offset_x = 0;
					view_offset_y = 0;
					break;
				}
			}
		}
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
		
		/* 自車の情報を取得 */
		GetMyCarInfo(&stMyCar, input);
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
			
			unRoadAnimeTime = time_old;	/* 前回値を更新 */
		}
		else if( (time_old - unRoadAnimeTime) < (362 / speed) )	/* 更新周期未満 */
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
			unRoadAnimeTime = time_old;	/* 前回値を更新 */

			uRoadAnimeCount = 0xFFF;	/* 逆走アニメーション防止 */

			uRoadAnime = 0;
			bRoad_Anime_flag = TRUE;
/*
			uRoadAnime++;
			if(uRoadAnime >= 4u)
			{
				uRoadAnime = 0;
			}
*/			
			uRoadAnimeBase++;
			if(uRoadAnimeBase >= 4)
			{	
				uRoadAnimeBase = 0;
				uCounter++;			/* カウンタ更新 */
				if(uCounter >= 1024)uCounter = 0;
			}
		}
		
		if(bRoad_Anime_flag == TRUE)
		{
			/* 角度算出 */
			road_height_old = road_height;	/* 前回値更新 */
			if(bDebugMode == FALSE)
			{
				/* コースデータ読み込み */
				road_height = (SS)(stRoadData[uCounter].bHeight - 0x80);	/* 道の標高	(0x80センター) */
			}
			else
			{
				/* デバッグ入力 */
				road_height = r_height;	/* デバッグ入力 */
			}
			road_height = Mmax(Mmin(road_height, 31), -32);
#if 1
			// 高低差から傾きを出す(-45～+45) rad = APL_Atan2( road_slope_old - road_slope,  );
			if(road_height != road_height_old)
			{
				road_slope_old = road_slope;	/* 前回値更新 */
				if(road_height > road_height_old)
				{
					road_slope++;
				}
				else
				{
					road_slope--;
				}
			}
			else{
				/* 前回値保持 */
			}
#else
			road_slope = vy;		/* デバッグ入力 */
#endif
			/* 角度からtanθ*/
#if 1
			if(road_slope == 0)
			{
				cal_tan = 0;
				road_distance = 0;
			}
			else
			{
				if(road_slope != road_slope_old)
				{
					cal_tan = APL_Tan256(road_slope);	/* tanθ */
					cal_cos = APL_Cos(road_slope);
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
			/* 高さと角度から最適なラスター開始位置の補正値を算出 */
			road_distance_old = road_distance;	/* 前回値更新 */
			if(cal_tan != 0u)
			{
				road_distance = ((0 - road_height) * cal_cos) >> 8;
			}
			else
			{
				road_distance = 0 - road_height;
			}
	//		road_distance = Mmax(Mmin(road_distance, 32), -40);
		}
		
		/* 空のコントロール */
		uPal_tmp[1] = BG_offset_y;
		
		/* コーナーの表現 */
#if 1
		if(bDebugMode == FALSE)
		{
			/* コースデータ読み込み */
			road_angle = (SS)stRoadData[uCounter].bAngle - 0x80;	/* 道の角度	(0x80センター) */
		}
		else
		{
			/* 固定 */
			road_angle = 0;
#else
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
#endif
		}

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

		road_angle = Mmax(Mmin(road_angle, 12), -12);

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
					Horizon = Mmax(Mmin(Horizon_tmp + road_distance, Y_HORIZON_0), RASTER_MIN);	/* 水平位置 */
					uRas_ed = Mmin(V_SYNC_MAX, RASTER_MAX);	/* ラスター終了位置 */
					break;
				}
				case 1:		/* FPS */
				{
					Horizon_tmp = Y_HORIZON_1;		/* 仮水平位置 */
					Horizon = Mmax(Mmin(Horizon_tmp + road_distance, Y_HORIZON_1+32), RASTER_MIN);	/* 水平位置 *//* 32 = ROAD_SIZE*1/3 */
					uRas_ed = Mmin(Y_HORIZON_1 + RASTER_MIN + ROAD_SIZE, V_SYNC_MAX);	/* ラスター終了位置 */
					break;
				}
				default:	/* FPS */
				{
					Horizon_tmp = Y_HORIZON_1;
					uRas_ed = Mmin(V_SYNC_MAX, RASTER_MAX);	/* ラスター終了位置 */
					break;
				}
			}
			Horizon_offset = ROAD_POINT - Horizon_tmp;	/* 実際のデータと表示位置との差異 */
			uRas_st = Mmax(Mmin(Horizon + RASTER_MIN, RASTER_MAX), RASTER_MIN);	/* ラスター開始位置 */
			uRas_st += (uRas_st % 2);	/* 偶数にする */
			uRas_ed -= (uRas_ed % 2);	/* 偶数にする */
			uRas_size = uRas_ed - uRas_st;			/* ラスターの範囲 */
			uRas_mid = (uRas_ed - 64);	/* 水平線の位置が変化しないポイント *//* 64 = ROAD_SIZE*2/3 */
			uPal_tmp[0] = uRas_st;				/* 割り込み位置の設定 */
			
			road_curve = road_angle * uRas_size;
			
			for(y=uRas_st; y < uRas_ed; y++)
			{
				SS	num;
				SS	ras_cal_x, ras_cal_y, ras_pat, ras_result;
				SS	col;
				SS	uBG_pat[4][4] = {288,   0,  96, 192, 
					 				 192, 288,   0,  96, 
					 				  96, 192, 288,   0, 
					 				   0,  96, 192, 288} ;
				
				num = y - uRas_st;			/* 0 -> uRas_size */
				
				/* X座標 */
				if(num == 0u)
				{
					ras_cal_x = vx;	/* 最初 */
				}
				else
				{
					SS curve_rate;
					curve_rate = road_curve / num;
					if(curve_rate < -256)	curve_rate = -256;
					if(curve_rate >  256)	curve_rate = 256;
					
					ras_cal_x = ((num * vx) >> 4) + curve_rate;	/* 第一項(自車の位置)＋第二項(曲がり具合) */	/* 固定小数点化はHUYEさんのアドバイス箇所 */
				}
				if(bDebugMode == TRUE)	/* デバッグモード */
				{
					Draw_Pset(view_offset_x + (WIDTH >> 1) + ras_cal_x, y + view_offset_y, 0xC2);	/* デバッグ用グラフ*/
				}
				if(ras_cal_x <   0)ras_cal_x += 512;
				if(ras_cal_x > 512)ras_cal_x -= 512;
				uRas_tmp[y] = (US)ras_cal_x;

				/* Y座標 */
				if(num == 0u)	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
				{
					ras_cal_y = 0;
					ras_pat = uBG_pat[3][0];
					road_offset_y = 4;
					road_offset_val = 4;
					uRoadAnime_old = uRoadAnime;
					uRoadAnimeBase_old = uRoadAnimeBase;
					col = 0xBB;
				}
#if 1
				else if( y < uRas_mid )	/* 水平線の位置が変化 *//* 64 = ROAD_SIZE*2/3 */
				{
					SS	Road_strch;
					SS	offset_size;

					offset_size = uRas_mid - y;	/* uRas_size -> 0 */
					
					Road_strch = (offset_size * cal_tan) >> 8;	/* 高さ＝底辺×tanθ */
//					Road_strch = (offset_size * APL_Cos(num)) >> 8;
//					Road_strch = (offset_size * APL_Sin(num)) >> 8;
					ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
					ras_cal_y = Horizon_offset + Road_strch + ((SS)uDebugNum - 0x80);

					/* ロードパターン*/
					uRoadAnimeCount++;
					if(uRoadAnimeCount >= road_offset_y)
					{
						road_offset_y = 4;
						
						uRoadAnimeCount = 0;
						uRoadAnime++;
					}
					if(uRoadAnime >= 4u){uRoadAnime = 0;}

					col = 0xB7;
				}
				else if( y >= (uRas_ed - 16))		/* 下側のはみ出し部分を補完 */
				{
					ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
					ras_cal_y = BG_under;

					/* ロードパターン*/
					uRoadAnimeCount++;
					if(uRoadAnimeCount >= road_offset_y)
					{
						road_offset_y = 4;
						
						uRoadAnimeCount = 0;
						uRoadAnime++;
					}
					if(uRoadAnime >= 4u){uRoadAnime = 0;}

					col = 0xC2;
				}
				else				/* 平坦 */
				{
					ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
					ras_cal_y = Horizon_offset;

					/* ロードパターン*/
					uRoadAnimeCount++;
					if(uRoadAnimeCount >= road_offset_y)
					{
						road_offset_y += road_offset_val;
						uRoadAnimeCount = 0;
						uRoadAnime++;
					}
					if(uRoadAnime >= 4u){uRoadAnime = 0;}

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

					/* ロードパターン*/
					uRoadAnimeCount++;
					if(uRoadAnimeCount >= road_offset_y)
					{
						road_offset_y = 4;
						
						uRoadAnimeCount = 0;
						uRoadAnime++;
					}
					if(uRoadAnime >= 4u){uRoadAnime = 0;}

					col = 0xB7;
				}
#endif
				
				if(bDebugMode == TRUE)	/* デバッグモード */
				{
					Draw_Line(uRas_st + view_offset_x, ((V_SYNC_MAX>>1)+ view_offset_y)+1, uRas_ed + view_offset_x, ((V_SYNC_MAX>>1)+ view_offset_y)+1, 0x35, 0xFFFF);
					Draw_Pset(y + view_offset_x, ((V_SYNC_MAX>>1)+ view_offset_y) - ras_cal_y , col);	/* デバッグ用グラフ*/
					uRas_debug[y] = ras_cal_y;
				}
				ras_result = ras_cal_y + ras_pat;
				if(ras_result <   0)ras_result += 512;
				if(ras_result > 512)ras_result -= 512;
				
				uPal_tmp[y] = (US)ras_result;
				
			}
			SetRasterVal(uRas_tmp, sizeof(US)*RASTER_MAX);
			SetRasterPal(uPal_tmp, sizeof(US)*RASTER_MAX);
		}
#endif
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
#ifdef DEBUG	/* デバッグコーナー */

#if	1
		if( Mabs(vx) > 25 )	/* コース外 */
		{
			SI	adpcm_sns;
			
			adpcm_sns = m_stat(9/*Mmin(Mmax(25, vx), 31)*/);	/* ADPCM ch1(9) ch2-8(25-31) */
//			Message_Num(&adpcm_sns,	 0, 13, 2, MONI_Type_SI, "%d");
			
			stMyCar.uEngineRPM = (stMyCar.uEngineRPM>>1) + (stMyCar.uEngineRPM>>2)  + (stMyCar.uEngineRPM>>3);
			
			if( (time_old - unExplosion_time) >  (60000 / rpm) )	/* 回転数のエンジン音 */
			{
				unExplosion_time = time_old;
				bExplosion = TRUE;
			}
			else
			{
				bExplosion = FALSE;
			}

			/* 回転数のエンジン音 */
			if( bExplosion == TRUE )
			{
				m_pcmplay(11,3,4);
			}
		}
		else
		{
		}
#endif
//		BG_TextPut("OverKata", 4, 10);
//		BG_TextPut("OverKata", 128, 128);
//		BG_TextPut("OVER KATA", 128, 128);
#endif
#ifdef DEBUG
		if(bDebugMode == TRUE)
		{
			if(stTask.b496ms == TRUE)
			{
				/* モニタ */
				Message_Num(&rpm,	 			12,  6, 2, MONI_Type_SS, "%4d");
				
				Message_Num(&unTime_cal,	 	 0,  8, 2, MONI_Type_UI, "%3d");
				Message_Num(&unTime_cal_PH,		 6,  8, 2, MONI_Type_UI, "%3d");
				
				Message_Num(&speed,				 0,  9, 2, MONI_Type_SS, "%3d");
				Message_Num(&vx, 				 6,  9, 2, MONI_Type_SS, "%2d");
				Message_Num(&vy, 				12,  9, 2, MONI_Type_SS, "%2d");
				Message_Num(&r_height, 			20,  9, 2, MONI_Type_SS, "%3d");
				
				Message_Num(&uRas_st,			 0, 10, 2, MONI_Type_US, "%3d");
				Message_Num(&uRas_mid,			 7, 10, 2, MONI_Type_US, "%3d");
				Message_Num(&uRas_ed,			13, 10, 2, MONI_Type_US, "%3d");
				Message_Num(&uRas_size,			20, 10, 2, MONI_Type_US, "%3d");
				
				Message_Num(&uPal_tmp[uRas_st],	 0, 11, 2, MONI_Type_US, "%3d");
				Message_Num(&uRas_tmp[uRas_st],	 6, 11, 2, MONI_Type_US, "%3d");
				Message_Num(&cal_tan,			12, 11, 2, MONI_Type_SS, "%3d");
//				Message_Num(&rad,				12, 11, 2, MONI_Type_FL, "%f");
				
				Message_Num(&road_angle,		 0, 12, 2, MONI_Type_SS, "%3d");
				Message_Num(&road_distance,	 	 6, 12, 2, MONI_Type_SS, "%3d");
				Message_Num(&road_slope,		12, 12, 2, MONI_Type_SS, "%3d");
				Message_Num(&road_height,		20, 12, 2, MONI_Type_SS, "%3d");
				
				Message_Num(&uDebugNum,		 	 0, 13, 2, MONI_Type_US, "%3d");
			}
		}
#endif
		/* 描画のクリア処理 */	/* 必ずテキスト表示処理の後に行うこと */
		if(stTask.b96ms == TRUE)
		{
			if((*CRTC_480 & 0x02u) == 0u)	/* クリア実行中でない */
			{
				*CRTC_R21 = Mbset(*CRTC_R21, 0x0Fu, 0x0Cu);	/* SCREEN1 高速クリアON / SCREEN0 高速クリアOFF */
				*CRTC_480 = Mbset(*CRTC_480, 0x02u, 0x02u);	/* クリア実行 */
			}
		}
		/* 描画処理 */
		if(stTask.b32ms == TRUE)
		{
			if(speed <= 8)								/* 抜かれる */
			{
				uCountNum = Mdec(uCountNum, 1);
				bUpdate = TRUE;
			}
			else if( (speed > 8) && (speed < 16) )		/* 保持*/
			{
			}
			else
			{
				uCountNum++;							/* 抜かす */
				bUpdate = TRUE;
			}
			if(uCountNum >= 64)uCountNum = 0;
			if(uCountNum <= 0)uCountNum = 0;
			
			/* 画面を揺らす */
			if((speed == 0) && (rpm == 0)){	/* 停車 */
				Vibration = 0;
			}
			else{			/* 走行中 */
				if((uFreeRunCount % 5) == 0)Vibration = 1;	/* 画面の振動 */
				else Vibration = 0;
			}

			/* 画面の位置 */
			HOME(0b01, view_offset_x, view_offset_y + Vibration );	/* Screen 0(TPS/FPS) */
			HOME(0b10, view_offset_x, view_offset_y );		/* Screen 1 */

			if( bUpdate == TRUE )
			{
				/* ライバル車 */
				US	j,w,h;
				i = uCountNum;
				j = (64-uCountNum)>>4;	/* 等間隔倍率になっているので改善要 */
				w = ENEMY_CAR_1_W>>j;
				h = ENEMY_CAR_1_H>>j;
				/* xもyも、倍率変化時に座標がセンター調整のため違和感あり */
				x = view_offset_x + (WIDTH>>1) - uRas_tmp[Mmin(uRas_st + uCountNum, uRas_ed)];
				y = view_offset_y + Horizon + i;

				G_BitBlt(	x,	w,
							y,	h,
							1,
							0,					w,
							ENEMY_CAR_1_H * j,	h,
							1,
							g_mode);
#if 0
				/* 障害物 */
				w = MY_CAR_0_W>>j;
				h = MY_CAR_0_W>>j;
				for(i=0; i<2; i++)
				{
					US obj_x, obj_y;
					obj_x = x-32;
					obj_y = y;
					Draw_Fill(obj_x,obj_y,obj_x-w,obj_y-6,0xC2u);
					obj_x = x+32;
					obj_y = y;
					Draw_Fill(obj_x,obj_y,obj_x+w,obj_y-6,0xC2u);
				}
#endif
			}
		}

		uFreeRunCount++;	/* 16bit フリーランカウンタ更新 */
#if 1
		/* 処理時間計測 */
		GetNowTime(&time_now);
		time = time_now - time_old;
		unTime_cal = time;	/* LSB:1 UNIT:ms */
		unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);
#endif
		/* 同期待ち */
		vwait(1);
	}
	while( loop );
	
	App_exit();	/* 終了処理 */

#if 1
	printf("uPal_tmp[0]の中身 = %d(vy=%d)\n", uRas_debug[0], vy );
	for(y=uRas_st; y < uRas_ed; y++)
	{
		printf("[%3d]=(%5d),", y, uRas_debug[y] );
		//printf("[%3d]=(%5d),", y, uRas_debug[y] );
		if((y%5) == 0)printf("\n");
	}
	printf("\n");
#endif
	
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
	if((unZmusicVer&0xFFFF000u)>>16 == 0u)	/* PCM8判定 */
	{
		puts("PCM8Aを常駐してください。");
		exit(0);
	}
	
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
	APICG_DataLoad("data/cg/Over_A.pic"	, X_OFFSET, 				Y_OFFSET - 32,	0);	/* FPS */
	APICG_DataLoad("data/cg/Over_B.pic"	, X_OFFSET + ((WIDTH>>1) - (MY_CAR_0_W>>1)),  V_SYNC_MAX-RASTER_MIN-MY_CAR_0_H - 16,	0);	/* TPS */
	G_MyCar();		/* 自車の表示 */
	APICG_DataLoad("data/cg/Over_C.pic"	, 0,							0,	1);	/* ライバル車 */
	APICG_DataLoad("data/cg/Over_D.pic"	, X_OFFSET - 32,	Y_OFFSET +  4,	1);	/* 背景 */
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
#endif	/* OVERKATA_C */
