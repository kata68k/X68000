#ifndef	OVERKATA_C
#define	OVERKATA_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>

#include "inc/usr_define.h"
#include "inc/usr_macro.h"
#include "inc/apicglib.h"
#include "inc/ZMUSIC.H"

#include "OverKata.h"
#include "Draw.h"
#include "MFP.h"
#include "Input.h"
#include "Output_Text.h"

/* グローバル変数 */
SS moni;
SS moni_MAX;
SS speed;
UC g_mode;

struct ROADDATA
{
	UC	bHeight;	/* 道の標高	(0x80センター) */
	UC	bWidth;		/* 道の幅	(0x80センター) */
	UC	bAngle;		/* 道の角度	(0x80センター) */
	UC	bfriction;	/* 道の摩擦	(0x80センター) */
	UC	bPat;		/* 道の種類	 */
	UC	bObject;	/* 出現ポイントのオブジェクトの種類 */
	UC	bRepeatCount;	/* 繰り返し回数 */
}stRoadData[256];

/* 関数のプロトタイプ宣言 */
void main(void);

/* 関数 */
void main(void)
{
	static US uScoreMax = 10000;
	UC	road_flag = 0;
	UC	updown_flag = 0;
	US	x,y;
	US	ras_tmp[256];
	US	pal_tmp[256];
	US	usFreeRunCount=0;
	US	uCountNum;
	US	ras_st, ras_ed, ras_size;
	SS	i;
	SS	loop;
	SS	RD[1024];
	SS	nHorizon, nHorizon_tmp, nHorizon_ras, nHorizon_offset;
	SS	vx, vy;
	SS	road;
	SS	road_offset_x, road_offset_y, road_offset_val;
	SS	view_offset_x, view_offset_y;
	US	uRoad_rate, uRoad_strch;
	SS	input;
	UI	start_time;
	UI	time_cal = 0;
	UI	time_cal_PH = 0;
	SI	superchk;
	SI	crtmod;
	UC	mode_flag = 0;
	UC	bKeyUP_flag = FALSE;
	UC	bKeyDOWN_flag = FALSE;
	UC	bUpdate;
	UC	bDebugMode;
	UC	bDebugMode_flag;
	US	uDebugNum;
	
	UI	unZmusicVer;
	
	UI	unExplosion_time;
	UC	bExplosion;
	
	UI	un8ms_time;
	UC	b8ms_time;
	UI	un16ms_time;
	UC	b16ms_time;
	UI	un32ms_time;
	UC	b32ms_time;
	UI	un96ms_time;
	UC	b96ms_time;
	
	SS	rpm, Vs, speed_min;
	SS	ShiftPos;
	UC	bShiftPosFlag;
	US	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
	US	uRoadAnime, uRoadAnime_old, uRoadAnimeBase, uRoadAnimeBase_old;
	US	uRoadAnimeCount;
	UI	unRoadAnimeTime;
	SS	nVibration;
	volatile US *CRTC_R21 = (US *)0xE8002Au;	/* テキスト・アクセス・セット、クリアーP.S */
	volatile US *CRTC_480 = (US *)0xE80480u;	/* CRTC動作ポート */

	/* デバッグコーナー */
#if 0
	/* ↓自由にコードを書いてね */
	
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
	superchk = SUPER(0);
	if( superchk < 0 ) {
		puts("すでにスーパーバイザーモード");
	} else {
		puts("スーパーバイザーモード開始");
	}
	
	crtmod = CRTMOD(-1);	/* 現在のモードを返す */
	
	/* サウンド初期化 */
	m_init();		/* 初期化 */
	m_ch("fm");		/* FM */
	m_stop(0,0,0,0,0,0,0,0,0,0);	/* 音楽停止 */
	zmd_play("data\\music\\X68K.ZMD");	/* ローディングBGM */

	/* グラフィック表示 */
	G_INIT();		/*画面の初期設定*/
	APICG_DataLoad("data/cg/Over_A.pic"	, X_OFFSET, 		Y_OFFSET - 32,	0);	/* FPS */
	APICG_DataLoad("data/cg/Over_B.pic"	, X_OFFSET + 96,			  165,	0);	/* TPS */
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
	
	/* 変数の初期化 */
	speed = 0;
	road_offset_x = 0;
	road_offset_y = 0;
	road_offset_val = 0;
	g_mode = 1;
	ShiftPos = 1;		/* 1速 */
	rpm = 0;
	Vs = 0;
	speed = 0;
	road = 0;
	loop = 1;
	vx = vy =0;
	usFreeRunCount = 0;
	uCountNum = 0;
	bUpdate = FALSE;
	uRoadAnimeBase = 0;
	uRoadAnimeBase_old = 0;
	uRoadAnime = 0;
	uRoadAnime_old = 0;
	uRoadAnimeCount = 0;
	nVibration = 0;
	bDebugMode = TRUE;
	uDebugNum = 0;
	
	for(i=0; i < 256; i++)
	{
		ras_tmp[i] = 0;
		pal_tmp[i] = 0;
	}

	/* 乱数 */
	{
		/* 乱数の初期化 */
		SS a,b,c,d;
		a = 0;
		b = 0;
		c = 0;
		for(a=0; a < 1024; a++)
		{
			RD[a] = 0;
		}

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
//	zmd_play("data\\music\\kyaraban.zmd");	/* BGM */

	/* MFP */
	Init_MFP();						/* MFP関連の初期化 */
	TIMERDST(Timer_D_Func, 7, 20);	/* Timer-Dセット */	/* 50us(7) x 20cnt = 1ms */
	SetNowTime(0);					/* 時間の初期化 */
	GetNowTime(&start_time);		/* Time Count用 */
	VDISPST(Vsync_Func, 0, 1);		/* V-Sync割り込み 帰線 */
	
	do
	{
		US uCount = 0;
		UI time, time_old, time_now;
		
		GetNowTime(&time_old);
		
		/* 時間判定 */
		if( (time_old - un8ms_time) > 8)	/* 8ms周期 */
		{
			un8ms_time = time_old;
			b8ms_time = TRUE;
		}
		else
		{
			b8ms_time = FALSE;
		}
		
		if( (time_old - un16ms_time) > 16)	/* 16ms周期 */
		{
			un16ms_time = time_old;
			b16ms_time = TRUE;
		}
		else
		{
			b16ms_time = FALSE;
		}

		if( (time_old - un32ms_time) > 32)	/* 32ms周期 */
		{
			un32ms_time = time_old;
			b32ms_time = TRUE;
		}
		else
		{
			b32ms_time = FALSE;
		}

		if( (time_old - un96ms_time) > 96)	/* 96ms周期 */
		{
			un96ms_time = time_old;
			b96ms_time = TRUE;
		}
		else
		{
			b96ms_time = FALSE;
		}
		/* 入力処理 */
		input = get_key(1);	/* キーボード＆ジョイスティック入力 */
		if((input & KEY_b_Q   ) != 0u) loop = 0;	/* Ｑで終了 */
		if((input & KEY_b_ESC ) != 0u) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;
		
		if((input & KEY_b_M   ) != 0u)	/* Ｍでモード切替 */
		{
			if( mode_flag == FALSE )
			{
				mode_flag = TRUE;
				if(g_mode == 0u)g_mode = 1u;
				else			g_mode = 0u;
			}
		}
		else
		{
			mode_flag = FALSE;
		}
		
		if((input & KEY_b_SP   ) != 0u)	/* スペースでデバッグON/OFF */
		{
			if( bDebugMode_flag == FALSE )
			{
				bDebugMode_flag = TRUE;
				if(bDebugMode == FALSE)	bDebugMode = TRUE;
				else					bDebugMode = FALSE;
			}
		}
		else
		{
			bDebugMode_flag = FALSE;
		}
		
		if(b96ms_time == TRUE)
		{
			/* モード切替による設定値の変更 */
			switch(g_mode)
			{
				case 0:		/* Screen 0(TPS) */
				{
					view_offset_x = X_OFFSET;
					view_offset_y = Y_MIN_DRAW;
					break;
				}
				case 1:		/* Screen 0(FPS) */
				{
					view_offset_x = X_OFFSET;
					view_offset_y = Y_OFFSET;
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
#if 0
		if( ((usFreeRunCount % (RD[usFreeRunCount & 0x03FFu])) == 0)
			&& ((usFreeRunCount % 5) == 0)
			&& (speed != 0u) )
		{
			if(updown_flag == 0){
				vy += 1;
				if(vy > 24)
				{
					updown_flag = 1;
				}
			}
			else{
				vy -= 1;
				if(vy < -28)
				{
					updown_flag = 0;
				}
			}
		}
#else
		if((input & KEY_UPPER) != 0u)
		{
			if(bKeyUP_flag == TRUE)
			{
				vy += 1;
				bKeyUP_flag = FALSE;
			}
		}
		else
		{
			bKeyUP_flag = TRUE;
		}
		if((input & KEY_LOWER) != 0u)
		{
			if(bKeyDOWN_flag == TRUE)
			{
				vy -= 1;
				bKeyDOWN_flag = FALSE;
			}
		}
		else
		{
			bKeyDOWN_flag = TRUE;
		}
#endif
		vy = Mmax(Mmin(vy, 24), -28);

		/* ステアリング操作 */
		if((input & KEY_RIGHT) != 0u)	vx += 1;	/* 右 */
		if((input & KEY_LEFT) != 0u)	vx -= 1;	/* 左 */
		vx = Mmax(Mmin(vx, 31), -31);
		
		/* アクセル */
		if((input & KEY_A) != 0u){	/* Aボタン */
			/* 回転数 */
			if(b8ms_time == TRUE)
			{
				rpm += 125;
			}
		}
		else{
			/* 回転数 */
			if(b8ms_time == TRUE)
			{
				rpm -= 100;
			}

			/* シフト操作 */
			if((input & KEY_UPPER) != 0u)
			{
				if(bShiftPosFlag == FALSE)
				{
					if(ShiftPos != 5)	m_pcmplay(5,3,4);	/* SE:シフトアップ */
					ShiftPos = Mmin(ShiftPos+1, 5);	/* 上 */
					bShiftPosFlag = TRUE;
				}
			}	
			else if((input & KEY_LOWER) != 0u)
			{
				if(bShiftPosFlag == FALSE)
				{
					if(ShiftPos != 0)	m_pcmplay(5,3,4);	/* SE:シフトダウン */
					ShiftPos = Mmax(ShiftPos-1, 0);	/* 下 */
					bShiftPosFlag = TRUE;
				}
			}
			else
			{
				bShiftPosFlag = FALSE;
			}
		}
		ShiftPos = Mmax(Mmin(ShiftPos, 5), 0);
		rpm = Mmax(Mmin(9000, rpm), 750);
		
		/* 車速 */
		if(ShiftPos == 0u)
		{
			Vs -= 10;	/* 変速比で変えたい */
			speed_min = 0;
		}
		else
		{
			/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 減速比 4.687 タイヤ周長2052.1mm */
			/* タイヤ周長×６０×回転数／（１０００×変速比×減速比） */
			Vs = (SS)(((UI)26 * rpm) / uTM[ShiftPos]);	
			speed_min = 1;
		}
		/* ブレーキ */
		if( (input & KEY_B) != 0U )	/* Bボタン */
		{
			Vs -= 30;	/* 変速比で変えたい */
			speed_min = 0;
		}
		else
		{
		}
		Vs = Mmax(Mmin(310, Vs), 0);
		/* 車速（ゲーム内） */
		speed = Vs / 10;
		speed = Mmax(Mmin(speed, 31), speed_min);
		

		/* コーナーの表現 */
#if 0
		if( ((usFreeRunCount % (RD[usFreeRunCount & 0x03FFu])) == 0)
			&& ((usFreeRunCount % 3) == 0)
			&& (speed != 0u) )
		{
			if(road_flag == 0){
				road += 1;
				if(road > 12)road_flag = 1;
			}
			else{
				road -= 1;
				if(road < -12)road_flag = 0;
			}
		}
#else
		road = 0;
#endif

#if 1
//		if(b8ms_time == TRUE)
		{
			/* 水平線 */
			/* ラスター割り込み処理の開始・終了位置 */
			switch(g_mode)
			{
				case 0:		/* TPS */
				{
					nHorizon_tmp = Y_HORIZON_0;		/* 仮水平位置 */
					nHorizon = Mmax(Mmin(nHorizon_tmp + vy, Y_HORIZON_0), RASTER_MIN);	/* 水平位置 */
					ras_ed = Mmin(V_SYNC_MAX, RASTER_MAX);	/* ラスター終了位置 */
					break;
				}
				case 1:		/* FPS */
				{
					nHorizon_tmp = Y_HORIZON_1;		/* 仮水平位置 */
					nHorizon = Mmax(Mmin(nHorizon_tmp + vy, Y_HORIZON_1), RASTER_MIN);	/* 水平位置 */
					ras_ed = Mmin(Y_HORIZON_1 + RASTER_MIN + ROAD_SIZE, V_SYNC_MAX);	/* ラスター終了位置 */
					break;
				}
				default:	/* FPS */
				{
					nHorizon_tmp = Y_HORIZON_1;
					ras_ed = Mmin(V_SYNC_MAX, RASTER_MAX);	/* ラスター終了位置 */
					break;
				}
			}
			nHorizon_offset = ROAD_POINT - nHorizon;	/* 実際のデータと表示位置との差異 */
			ras_st = Mmax(Mmin(nHorizon + RASTER_MIN, RASTER_MAX), RASTER_MIN);	/* ラスター開始位置 */
			ras_size = ras_ed - ras_st;		/* ラスターの範囲 */
			pal_tmp[0] = ras_st;			/* 割り込み位置の設定 */
			
			uRoad_rate = (ras_size << 12) / ROAD_SIZE;
			uRoad_strch = 0;
			uCount = 0;
			
			/* ロードパターン*/
			/* センターラインの長さと間隔は５ｍ */
			/* 60fps=17ms/f */
			/* 最大速度307km/h(85m/s,0.0853m/ms) 1f=1.45m進む */
			/* 最低速度6km/h(1.7m/s,0.0017m/ms) 1f=0.028m進む */
			/* 1LSB 10km/h(2.8m/s,0.0028m/ms) 1f=0.047m進む */
			/* 1times Cal 1m=4dot (17ms/f x 1/0.047 / speed) */
			if(speed == 0u)	/* 車速0km/h */
			{
				uRoadAnime = uRoadAnime_old;
				uRoadAnimeBase = uRoadAnimeBase_old;
				uRoadAnimeCount = 0;
				
				unRoadAnimeTime = time_old;	/* 前回値を更新 */
			}
			else if( (time_old - unRoadAnimeTime) < (362 / speed) )	/* 更新周期未満 */
			{
				uRoadAnime = uRoadAnime_old;
				uRoadAnimeBase = uRoadAnimeBase_old;
				uRoadAnimeCount = 0;
			}
			else
			{
				unRoadAnimeTime = time_old;	/* 前回値を更新 */

				uRoadAnime++;
				if(uRoadAnime >= 4u)	uRoadAnime = 0;
				uRoadAnimeBase++;
				if(uRoadAnimeBase >= 4)	uRoadAnimeBase = 0;
			}
			
			for(y=ras_st; y < ras_ed; y++)
			{
				SS num;
				US uBG_pat[4][4] = {288,   0,  96, 192,
					 				192, 288,   0,  96,
					 				 96, 192, 288,   0,
					 				  0,  96, 192, 288};
				US ras_cal;
				
				num = y - ras_st;
				
				/* Y座標 */
				if(num == 0u)	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
				{
					ras_cal = 0;
					road_offset_y = 3;
					road_offset_val = 1;
					uRoadAnime_old = uRoadAnime;
					uRoadAnimeBase_old = uRoadAnimeBase;
					uCount = 0;
				}
				else if(vy < 0)	/* 上り坂 */
				{
					if(uCount > (uRoad_rate>>12))
					{
						uRoad_strch++;
						if(uRoad_strch >= ROAD_SIZE)uRoad_strch=ROAD_SIZE;
						uCount = 0;
					}
					ras_cal = nHorizon_offset - uRoad_strch + uBG_pat[uRoadAnimeBase][uRoadAnime];
				}
				else if(vy > 0)	/* 下り坂 */
				{
					if(uCount > (uRoad_rate>>12))
					{
						uRoad_strch++;
						uCount = 0;
					}
					ras_cal = nHorizon_offset + vy - uRoad_strch + uBG_pat[uRoadAnimeBase][uRoadAnime];
				}
				else			/* 平坦 */
				{
					ras_cal = nHorizon_offset + uBG_pat[uRoadAnimeBase][uRoadAnime];
				}
				if(ras_cal <   0)ras_cal += 512;
				if(ras_cal > 512)ras_cal -= 512;
				pal_tmp[y] = ras_cal;
				
				/* ロードパターン*/
				uRoadAnimeCount++;
				if(uRoadAnimeCount >= road_offset_y)
				{
					if(
						((ras_size>>2) > num) 
					)
					{
						road_offset_y = 4;
					}
					else
					{
						road_offset_y += road_offset_val;
					}
					uRoadAnimeCount = 0;
					uRoadAnime++;
				}
				if(uRoadAnime >= 4u)
				{
					uRoadAnime = 0;
				}
				/* X座標 */
				//ras_tmp[y] = num * ((float)vx / 16);
				ras_tmp[y] = (num * vx) >> 4;										/* HUYEさんのアドバイス箇所 */
				//ras_tmp[y] += road * ( (RASTER_MAX - ras_st) / (float)(Mmax(y-ras_st, 1)) );
				ras_tmp[y] += (Mmin(Mmax((road * ras_size), -256), 512) / Mmax(y-ras_st,1 ));	/* HUYEさんのアドバイス箇所 */

				uCount++;
			}
			SetRasterVal(ras_tmp, sizeof(US)*RASTER_MAX);
			SetRasterPal(pal_tmp, sizeof(US)*RASTER_MAX);
		}
#endif
		if(b96ms_time == TRUE)
		{
			/* 処理負荷改善要 */
			UI nTimeCounter, nPassTime, nTimer;
			US uScore;
			/* Score */
			uScore = usFreeRunCount;
			Text_To_Text(uScore, 192, 8, FALSE, "%7d");
			/* Top Score */
			uScoreMax = Mmax(uScore, uScoreMax);
			Text_To_Text(uScoreMax, 40, 8, FALSE, "%7d");
			/* Time Count */
			GetNowTime(&time_now);
			nPassTime = (time_now - start_time);
			nTimer = 120000 - nPassTime;
			
			if(120000 < nTimer)
			{
				nTimeCounter = 0;
			}
			else
			{
				nTimeCounter = nTimer / 1000;
			}
			Text_To_Text(nTimeCounter, 112, 24, TRUE, "%3d");
			/* Speed */
			Text_To_Text(Vs, 208, 24, FALSE, "%3d");
			/* Gear */
			Text_To_Text(ShiftPos, 224, 32, FALSE, "%d");
		}
#ifdef DEBUG	/* デバッグコーナー */

#if	1
		if( (input & KEY_B) != 0U )	/* Bボタン */
		{
			SI	adpcm_sns;
			
			adpcm_sns = m_stat(9/*Mmin(Mmax(25, vx), 31)*/);	/* ADPCM ch1(9) ch2-8(25-31) */
//			Message_Num(&adpcm_sns,	 0, 13, 2, MONI_Type_SI);
			
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
			uDebugNum = DirectInputKeyNum(1);
			if(b96ms_time == TRUE)
			{
				/* モニタ */
				Message_Num(&rpm,	 			12,  6, 2, MONI_Type_SS);
				Message_Num(&time_cal,	 		 0,  8, 2, MONI_Type_UI);
				Message_Num(&time_cal_PH,		10,  8, 2, MONI_Type_UI);
				Message_Num(&speed,				 0,  9, 2, MONI_Type_SS);
				Message_Num(&vx, 				 6,  9, 2, MONI_Type_SS);
				Message_Num(&vy, 				12,  9, 2, MONI_Type_SS);
				Message_Num(&ras_st,			 0, 10, 2, MONI_Type_US);
				Message_Num(&ras_ed,			 7, 10, 2, MONI_Type_US);
				Message_Num(&ras_size,			13, 10, 2, MONI_Type_US);
				Message_Num(&pal_tmp[ras_st],	 0, 11, 2, MONI_Type_US);
				Message_Num(&uRoad_rate,		 6, 11, 2, MONI_Type_US);
				Message_Num(&uRoad_strch,		12, 11, 2, MONI_Type_US);
				Message_Num(&ras_tmp[ras_st],	 0, 12, 2, MONI_Type_SS);
				Message_Num(&road,			 	 6, 12, 2, MONI_Type_SS);
				Message_Num(&uDebugNum,		 	12, 12, 2, MONI_Type_US);
			}
		}
#endif
		/* 描画のクリア処理 */	/* 必ずテキスト表示処理の後に行うこと */
		if(b96ms_time == TRUE)
		{
			if((*CRTC_480 & 0x02u) == 0u)	/* クリア実行中でない */
			{
				*CRTC_R21 = Mbset(*CRTC_R21, 0x0Fu, 0x0Cu);	/* SCREEN1 高速クリアON / SCREEN0 高速クリアOFF */
				*CRTC_480 = Mbset(*CRTC_480, 0x02u, 0x02u);	/* クリア実行 */
			}
		}
		/* 描画処理 */
		if(b32ms_time == TRUE)
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
				nVibration = 0;
			}
			else{			/* 走行中 */
				if((usFreeRunCount % 5) == 0)nVibration = 1;	/* 画面の振動 */
				else nVibration = 0;
			}

			/* 画面の位置 */
			HOME(0b01, view_offset_x, view_offset_y + nVibration );	/* Screen 0(TPS/FPS) */
			HOME(0b10, view_offset_x, view_offset_y );		/* Screen 1 */

			if( bUpdate == TRUE )
			{
				/* ライバル車 */
				US	j,w,h;
				i = uCountNum;
				j = (64-uCountNum)>>4;
				w = ENEMY_CAR_1_W>>j;
				h = ENEMY_CAR_1_H>>j;
				x = view_offset_x + (WIDTH>>1) - ras_tmp[ras_st + (uCountNum << 1)];
				y = view_offset_y + nHorizon + i - 6;

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

		usFreeRunCount++;	/* フリーランカウンタ更新 */
#if 1
		/* 処理時間計測 */
		GetNowTime(&time_now);
		time = time_now - time_old;
		time_cal = time;	/* LSB:1 UNIT:ms */
		time_cal_PH = Mmax(time_cal, time_cal_PH);
#endif
		/* 同期待ち */
		vwait(1);
	}
	while( loop );

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

	CRTMOD(crtmod);			/* モードをもとに戻す */
	
	_dos_kflushio(0xFF);	/* キーバッファをクリア */

	/*スーパーバイザーモード終了*/
	SUPER(superchk);

#if 1
	printf("pal_tmp[0]の中身 = %d(vy=%d)\n", pal_tmp[0], vy );
	for(y=ras_st; y < RASTER_MAX; y+=1)
	{
		printf("[%3d]=(%5d),", y, pal_tmp[y] );
		if((y%5) == 0)printf("\n");
	}
	printf("\n");
#endif
	
}

#endif	/* OVERKATA_C */
