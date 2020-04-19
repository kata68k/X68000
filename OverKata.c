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
	SS	nHorizon;
	SS	vx, vy;
	SS	road;
	SS	road_offset_x, road_offset_y;
	SS	nRoad_strch;
	SS	input;
	UI	start_time;
	UI	time_cal = 0;
	UI	time_cal_PH = 0;
	SI	superchk;
	SI	crtmod;
	
	volatile US *text_addr  = (US *)0xE00000;

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
	G_Palette();	/* グラフィックパレットの設定 */
	G_Background();	/* 背景の表示 */
	G_MyCar();		/* 自車の表示 */
	APICG_DataLoad("data/enemy.pic", X_OFFSET + 64, Y_OFFSET + 88, 1);	/* ライバル車 */
	
	/* スプライト／ＢＧ表示 */
	PCG_INIT();							/* スプライト／ＢＧの初期化 */
	PCG_SP_dataload("data/BG.SP");		/* スプライトのデータ読み込み */
	PCG_PAL_dataload("data/BG.PAL");	/* スプライトのパレットデータ読み込み */
	BG_TEXT_SET();						/* マップデータによるＢＧの配置 */
	
	/* テキスト表示 */
	T_INIT();		/* テキストＶＲＡＭ初期化 */
	T_TopScore();	/* TOP */
	T_Time();		/* TIME */
	T_Score();		/* SCORE */

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
	
	a = 0;
	b = 0;
	c = 0;
	d = 0;
	e = 0;
	speed = 0;
	road = 0;
	loop = 1;
	vx = vy =0;
	usFreeRunCount = 0;
	do
	{
		SI nHantei = 0;
		UI time, time_old, time_now;
		
		GetNowTime(&time_old);
		
		input = get_key(1);
		if(input & KEY_b_Q   ) loop = 0;	/* Ｑで終了 */
		if(input & KEY_b_ESC ) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;
		
#if 1
		if( ((usFreeRunCount % (RD[usFreeRunCount & 0x03FFu])) == 0)
			&& ((usFreeRunCount % 5) == 0) )
		{
			if(updown_flag == 0){
				vy += 1;
				if(vy > 48)updown_flag = 1;
			}
			else{
				vy -= 1;
				if(vy < -5)updown_flag = 0;
			}
		}
#else
		if(input & KEY_UPPER)	vy += 1;
		if(input & KEY_LOWER)	vy -= 1;
#endif
		vy = Mmax(Mmin(vy, 48), -5);

		if(input & KEY_RIGHT)	vx += 1;
		if(input & KEY_LEFT)	vx -= 1;
		vx = Mmax(Mmin(vx, 32), -32);
		
		if(input & KEY_A){
			if((usFreeRunCount % 5) == 0)speed += 1;
		}
		else{
			speed -= 1;
		}
		speed = Mmax(Mmin(speed, 32), 0);
#if 1
		if( ((usFreeRunCount % (RD[usFreeRunCount & 0x03FFu])) == 0)
			&& ((usFreeRunCount % 3) == 0) )
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
		HOME(0b01, X_OFFSET, Y_OFFSET + f );		/* Screen 0 */
		HOME(0b10, X_OFFSET + vx + road * 2, Y_OFFSET + f );	/* Screen 1 */
		
		nHorizon = Mmax(Y_HORIZON + vy, RASTER_MIN);										/* 水平線 */
		ras_st = Mmax(Mmin(nHorizon + RASTER_MIN - RASTER_NEXT, RASTER_MAX - RASTER_NEXT), RASTER_MIN);	/* ラスター開始位置 */
		ras_ed = Mmin(Mmax(ras_st + ROAD_SIZE, 154+RASTER_MIN), RASTER_MAX);				/* ラスター終了位置 */
		pal_tmp[0] = ras_st;
		
		if(speed == 0){
			d = e;
			f = 0;
		}
		else{
			if((usFreeRunCount % 5) == 0)f = 1;
			else f = 0;
		}
#if 1
		ras_size = ras_ed - ras_st;
		if(ROAD_SIZE < ras_size)
		{
			nRoad_strch = ras_size - ROAD_SIZE;
		}
		else
		{
			nRoad_strch = 0;
		}
		
		for(y=ras_st; y < ras_ed; y+=RASTER_NEXT)
		{
			SI num = y - ras_st;
			SI Anime;
			US uBG_pat[4] = {0, 96, 192, 288};
//			US uBG_pat[4] = {288,192, 96, 0};
			
			Anime = num;
			
			/* Y座標 */
			if(num == 0u)	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
			{
				pal_tmp[y] = 0 + uBG_pat[0];
				nHantei = 0;
				road_offset_y = 0;
				c = 0;
				e = d;
			}				/* 下り坂 */
			else if(ras_size > ROAD_SIZE)
			{
				if(nRoad_strch > 0){
					nRoad_strch--;
				}
				pal_tmp[y] = Mmax(Mmin(nHorizon - (vy<<1) - nRoad_strch*2, 128), 32) + uBG_pat[d];
			}
#if 0
			else if(ras_size < ROAD_SIZE)
			{
				pal_tmp[y] = Mmax(Mmin(nHorizon + (vy<<1) - nRoad_strch, 128), 32) + uBG_pat[d];
			}
#endif
			else			/* 平坦or上り坂 */
			{
				pal_tmp[y] = Mmax(Mmin(nHorizon - (vy<<1) - nRoad_strch, 128), 32) + uBG_pat[d];
//				pal_tmp[y] = Mmax(Mmin(nHorizon, 128), 32) + uBG_pat[d];
//				pal_tmp[y] = Mmin(nHorizon + uBG_pat[d] + vy, 1024);
			}
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
			ras_tmp[y] += ((road * ras_size) / Mmax(y-ras_st,1 ));	/* HUYEさんのアドバイス箇所 */
			
			/* ラスター飛ばし分 */
			for(i = 0; i < RASTER_NEXT; i++){
				ras_tmp[y+i] = ras_tmp[y];
				pal_tmp[y+i] = pal_tmp[y];
			}
		}
		SetRasterVal(ras_tmp, sizeof(US)*RASTER_MAX);
		SetRasterPal(pal_tmp, sizeof(US)*RASTER_MAX);
#endif
		
		/* デバッグコーナー */
#if 1
//		BG_TextPut("OverKata", 4, 10);
//		BG_TextPut("OverKata", 128, 128);
//		BG_TextPut("OVER KATA", 128, 128);
#endif
		
		if((usFreeRunCount % 10) == 0)
		{
#if 1		/* 処理負荷改善要 */
			UI nTimeCounter, nPassTime, nTimer;
			UI nScore;
			static UI nScoreMax = 3000;
			
			nScore = (UI)usFreeRunCount;
			
			/* Score */
			BG_Number( nScore, 200, 8);
			
			/* Top Score */
			nScoreMax = Mmax(nScore, nScoreMax);
			BG_Number( nScoreMax, 40, 8);
			
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
			BG_TimeCounter( nTimeCounter, (BG_WIDTH * 16), 24);
#endif
#if 1
			/* モニタ */
			Message_Num(&time_cal,	 		 0,  8, 2, MONI_Type_UI);
			Message_Num(&time_cal_PH,		 4,  8, 2, MONI_Type_UI);
			Message_Num(&speed,				 0,  9, 2, MONI_Type_SS);
			Message_Num(&vx, 				 4,  9, 2, MONI_Type_SS);
			Message_Num(&vy, 				 8,  9, 2, MONI_Type_SS);
			Message_Num(&ras_st,			 0, 10, 2, MONI_Type_US);
			Message_Num(&ras_ed,			 4, 10, 2, MONI_Type_US);
			Message_Num(&ras_size,			 8, 10, 2, MONI_Type_US);
			Message_Num(&pal_tmp[ras_st],	 0, 11, 2, MONI_Type_US);
			Message_Num(&nHorizon,			 4, 11, 2, MONI_Type_SS);
			
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
	printf("pal_tmp[0]の中身 = %d(vy=%d)\n", (SS)pal_tmp[0], vy );
	for(y=ras_st; y < RASTER_MAX; y+=RASTER_NEXT)
	{
		printf("[%3d]=(%5d),", y, (SS)pal_tmp[y] );
		if((y%5) == 0)printf("\n");
	}
	printf("\n");
#endif
	
}

#endif	/* OVERKATA_C */
