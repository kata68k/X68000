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

/* 関数のプロトタイプ宣言 */
void main(void);

/* 関数 */
void main(void)
{
	UC	road_flag = 0;
	UC	updown_flag = 0;
	US	x1,x2,y1,y2;
	US	x,y;
	US	ras_tmp[256];
	US	pal_tmp[256];
	US	*ptr;
	US	usFreeRunCount=0;
	US	uJdge=0;
	US	uCountNum;
	US	uSQ;
	US	ras_st, ras_ed, ras_size;
	SS	a,b,c,d,e,f,i;
	SS	loop;
	SS	RD[1024];
	SS	nHorizon, nHorizon_tmp, nHorizon_ras, nHorizon_offset;
	SS	vx, vy;
	SS	road;
	SS	road_offset_x, road_offset_y;
	SS	view_offset_x, view_offset_y;
	US	uRoad_rate, uRoad_strch;
	SS	input;
	UI	start_time;
	UI	time_cal = 0;
	UI	time_cal_PH = 0;
	SI	superchk;
	SI	crtmod;
	UC	mode_flag = 0;
	UC	bUpdate;
	
	volatile US *CRTC_R21 = (US *)0xE8002Au;	/* テキスト・アクセス・セット、クリアーP.S */
	volatile US *CRTC_480 = (US *)0xE80480u;	/* CRTC動作ポート */

	/* デバッグコーナー */
#if 0
	UI	vi, vj;
	/* マップデータ読み込み */
	File_Load_CSV("data/map.csv", map_data, &vi, &vj);
	
	for(y=0;y<vi;y++)
	{
		for(x=0;x<vj;x++)
		{
			printf("%d,", map_data[y][x]);
		}
		printf("->(%d,%d)\n",vj, vi);
	}
	
	loop = 1;
	do
	{
		if( ( BITSNS( 0 ) & 0x02 ) != 0 ) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;
	}
	while( loop );
	
	exit(0);
#endif
	
	/* スーパーバイザーモード開始 */
	/*ＤＯＳのスーパーバイザーモード開始*/
	superchk = SUPER(0);
	if( superchk < 0 ) {
		puts("すでに入ってる。");
	} else {
		puts("入った。");
	}
	
	crtmod = CRTMOD(-1);	/* 現在のモードを返す */
	
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
	
	for(a=0; a < 256; a++)
	{
		ras_tmp[a] = 0;
		pal_tmp[a] = 0;
	}

	for(a=0; a < 1024; a++)
	{
		RD[a] = 0;
	}
	
	/* 乱数の初期化 */
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
	
	Init_MFP();						/* MFP関連の初期化 */
	TIMERDST(Timer_D_Func, 7, 20);	/* Timer-Dセット */	/* 50us(7) x 20cnt = 1ms */
	SetNowTime(0);					/* 時間の初期化 */
	GetNowTime(&start_time);		/* Time Count用 */
	VDISPST(Vsync_Func, 0, 1);		/* V-Sync割り込み 帰線 */
	
	/* 音楽 */
	
	
	a = 0;
	b = 0;
	c = 0;
	d = 0;
	e = 0;
	g_mode = 1;
	speed = 0;
	road = 0;
	loop = 1;
	vx = vy =0;
	usFreeRunCount = 0;
	uCountNum = 0;
	bUpdate = FALSE;
	
	do
	{
		SI nHantei = 0;
		US uCount = 0;
		UI time, time_old, time_now;
		
		GetNowTime(&time_old);
		
		input = get_key(1);
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
		
#if 1
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
		/* 上 */
		if((input & KEY_UPPER) != 0u)	vy += 1;
		/* 下 */
		if((input & KEY_LOWER) != 0u)	vy -= 1;
#endif
		vy = Mmax(Mmin(vy, 24), -28);

		/* 右 */
		if((input & KEY_RIGHT) != 0u)	vx += 1;
		/* 左 */
		if((input & KEY_LEFT) != 0u)	vx -= 1;
		vx = Mmax(Mmin(vx, 32), -32);
		
		/* Aボタン */
		if((input & KEY_A) != 0u){
			if((usFreeRunCount % 5) == 0)speed += 1;
		}
		else{
			speed -= 1;
		}
		speed = Mmax(Mmin(speed, 32), 0);
		
		/* Bボタン */
		if( (input & KEY_B) != 0U )
		{
		}
		else
		{
		}

		/* コーナーの表現 */
#if 1
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
		if(speed == 0){
			d = e;
		}
		
		/* 水平線 */
		/* ラスター割り込み処理の開始・終了位置 */
		switch(g_mode)
		{
			case 0:		/* TPS */
			{
				nHorizon_tmp = Y_HORIZON_0;
				nHorizon = Mmax(Mmin(nHorizon_tmp + vy, Y_HORIZON_0), RASTER_MIN);
				break;
			}
			case 1:		/* FPS */
			{
				nHorizon_tmp = Y_HORIZON_1;
				nHorizon = Mmax(Mmin(nHorizon_tmp + vy, Y_HORIZON_1), RASTER_MIN);
				break;
			}
			default:	/* FPS */
			{
				nHorizon_tmp = Y_HORIZON_1;
				break;
			}
		}
		nHorizon_offset = ROAD_POINT - nHorizon;	/* 実際のデータと表示位置との差異 */
		ras_st = Mmax(Mmin(nHorizon + RASTER_MIN - RASTER_NEXT, RASTER_MAX - RASTER_NEXT), RASTER_MIN);	/* ラスター開始位置 */
//		ras_ed = Mmin(Mmax(ras_st + ROAD_SIZE, 154+RASTER_MIN), RASTER_MAX);				/* ラスター終了位置 */
		ras_ed = RASTER_MAX;			/* ラスター終了位置 */
		ras_size = ras_ed - ras_st;		/* ラスターの範囲 */
		pal_tmp[0] = ras_st;			/* 割り込み位置の設定 */
		
		uRoad_rate = ras_size / ROAD_SIZE;
		uRoad_strch = 0;
		uCount = 0;
		
		for(y=ras_st; y < ras_ed; y+=RASTER_NEXT)
		{
			SI num = y - ras_st;
			SI Anime;
			US uBG_pat[4] = {0, 96, 192, 288};
			US ras_cal;
			
			Anime = num;
			
			/* Y座標 */
			if(num == 0u)	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
			{
				ras_cal = 0 + uBG_pat[0];
				nHantei = 0;
				road_offset_y = 0;
				c = 0;
				e = d;
				uCount = 0;
			}
			else if(vy < 0)	/* 下り坂 */
			{
				if(uCount > uRoad_rate)
				{
					uRoad_strch ++;
					if(uRoad_strch >= ROAD_SIZE)uRoad_strch=ROAD_SIZE;
					uCount = 0;
				}
				if(nHorizon_offset - uRoad_strch < 0)
				{
					ras_cal = 512 + nHorizon_offset - uRoad_strch + uBG_pat[d];
				}
				else
				{
					ras_cal = nHorizon_offset - uRoad_strch + uBG_pat[d];
				}
			}
			else if(vy > 0)	/* 上り坂 */
			{
				if(uCount > uRoad_rate)
				{
					uRoad_strch ++;
					uCount = 0;
				}
				ras_cal = nHorizon_offset + vy - uRoad_strch + uBG_pat[d];
			}
			else							/* 平坦 */
			{
				ras_cal = nHorizon_offset + uBG_pat[d];
			}
			pal_tmp[y] = ras_cal;
			
			if( Anime > nHantei + f ){		/* 縁石の色の間隔 */
				switch(c)
				{
				case 0:
					road_offset_y++;
					if(num > (ras_size>>1))c++;
					break;
				case 1:
					road_offset_y+=2;
					c++;
					break;
				case 2:
					road_offset_y+=4;
					c++;
					break;
				case 3:
					road_offset_y+=8;
					c++;
					break;
				default:
					road_offset_y+=16;
					c++;
					break;
				}
				nHantei = Anime + road_offset_y;

				d++;
				if(d >= 4)d=0;
			}
			
			/* X座標 */
			//ras_tmp[y] = num * ((float)vx / 16);
			ras_tmp[y] = (num * vx) >> 4;										/* HUYEさんのアドバイス箇所 */
			//ras_tmp[y] += road * ( (RASTER_MAX - ras_st) / (float)(Mmax(y-ras_st, 1)) );
			ras_tmp[y] += (Mmin(Mmax((road * ras_size), -256), 512) / Mmax(y-ras_st,1 ));	/* HUYEさんのアドバイス箇所 */
			
			/* ラスター飛ばし分 */
			for(i = 0; i < RASTER_NEXT; i++){
				ras_tmp[y+i] = ras_tmp[y];
				pal_tmp[y+i] = pal_tmp[y];
			}
			uCount++;
		}
		SetRasterVal(ras_tmp, sizeof(US)*RASTER_MAX);
		SetRasterPal(pal_tmp, sizeof(US)*RASTER_MAX);
#endif
		if((usFreeRunCount % 10) == 0)
		{
			/* 処理負荷改善要 */
			UI nTimeCounter, nPassTime, nTimer;
			US uScore;
			static US uScoreMax = 3000;
			/* Score */
			uScore = usFreeRunCount;
#if 1
			Text_To_Text(uScore, 176, 8, FALSE);
#else
			BG_Number( uScore, 200, 8);
#endif
			/* Top Score */
			uScoreMax = Mmax(uScore, uScoreMax);
#if 1
			Text_To_Text(uScoreMax, 16, 8, FALSE);
#else
			BG_Number( uScoreMax, 40, 8);
#endif
			/* Time Count */
			GetNowTime(&time_now);
			nPassTime = (time_now - start_time);
			nTimer = 60000 - nPassTime;
			
			if(60000 < nTimer)
			{
				nTimeCounter = 0;
			}
			else
			{
				nTimeCounter = nTimer / 1000;
			}
#if 1
			Text_To_Text(nTimeCounter, 108, 24, TRUE);
#else
			BG_TimeCounter( nTimeCounter, (BG_WIDTH * 16), 24);
#endif

		}
#ifdef DEBUG
		/* デバッグコーナー */
//		BG_TextPut("OverKata", 4, 10);
//		BG_TextPut("OverKata", 128, 128);
//		BG_TextPut("OVER KATA", 128, 128);
#endif
#ifdef DEBUG
		if((usFreeRunCount % 10) == 0)
		{
			/* モニタ */
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

			if((*CRTC_480 & 0x02u) == 0u)	/* クリア実行中でない */
			{
				*CRTC_R21 = Mbset(*CRTC_R21, 0x0Fu, 0x0Cu);	/* SCREEN1 高速クリアON / SCREEN0 高速クリアOFF */
				*CRTC_480 = Mbset(*CRTC_480, 0x02u, 0x02u);	/* クリア実行 */
				bUpdate = FALSE;
			}
		}
#endif
		/* 描画処理 */
		/* 描画のクリア処理 */	/* 必ずテキスト表示処理の後に行うこと */
#if 0
		if( bUpdate == TRUE )	/* 前回描画更新だった */
		{
		}
		else
#endif
		{
			if(speed <= 8)								/* 抜かれる */
			{
				uCountNum = Mdec(uCountNum, 1);
				bUpdate = TRUE;
			}
			else if( (speed > 8) && (speed < 31) )		/* 保持*/
			{
			}
			else
			{
				uCountNum++;							/* 抜かす */
				bUpdate = TRUE;
			}
		}
		if(uCountNum >= 64)uCountNum = 0;
		if(uCountNum <= 0)uCountNum = 0;

		/* 画面を揺らす */
		if(speed == 0){	/* 停車 */
			f = 0;
		}
		else{			/* 走行中 */
			if((usFreeRunCount % 5) == 0)f = 1;	/* 画面の振動 */
			else f = 0;
		}
		

		/* 画面の位置 */
		HOME(0b01, view_offset_x, view_offset_y + f );	/* Screen 0(TPS/FPS) */
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

	CRTMOD(crtmod);			/* モードをもとに戻す */
	
	_dos_kflushio(0xFF);	/* キーバッファをクリア */

	/*スーパーバイザーモード終了*/
	SUPER(superchk);

#if 1
	printf("pal_tmp[0]の中身 = %d(vy=%d)\n", pal_tmp[0], vy );
	for(y=ras_st; y < RASTER_MAX; y+=RASTER_NEXT)
	{
		printf("[%3d]=(%5d),", y, pal_tmp[y] );
		if((y%5) == 0)printf("\n");
	}
	printf("\n");
#endif
	
}

#endif	/* OVERKATA_C */
