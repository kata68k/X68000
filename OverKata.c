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

#include "OverKata.h"
#include "Draw.h"
#include "MFP.h"

/* グローバル変数 */
US moni;
US moni_MAX;
SI speed;

/* 変数 */
static SS key;

/* 関数のプロトタイプ宣言 */
SI GetSpeed(SI *);
void Message_Num(SI, SI, SI);


SI GetSpeed(SI *speed_val)
{
	*speed_val = speed;
	return 0;
}

void Message_Num(SI num, SI x, SI y)
{
	char str[64];

	sprintf(str, "%d", num);
	B_PUTMES( 3, x, y, 10-1, str);
}

SS get_key( void )
{
	UI kd_k1,kd_k2_1,kd_k2_2,kd_b;
	static SS repeat_flag_a = KEY_TRUE;
	static SS repeat_flag_b = KEY_TRUE;
	SS ret = 0;
	
	kd_k1 = BITSNS(7);
	kd_k2_1 = BITSNS(8);
	kd_k2_2 = BITSNS(9);
	kd_b = BITSNS(10);
	
	if( !( JOYGET( 0 ) & UP    ) || ( kd_k1 & 0x10 ) || ( kd_k2_1 & 0x10 ) ) ret |= KEY_UPPER;
	if( !( JOYGET( 0 ) & DOWN  ) || ( kd_k1 & 0x40 ) || ( kd_k2_2 & 0x10 ) ) ret |= KEY_LOWER;
	if( !( JOYGET( 0 ) & LEFT  ) || ( kd_k1 & 0x08 ) || ( kd_k2_1 & 0x80 ) ) ret |= KEY_LEFT;
	if( !( JOYGET( 0 ) & RIGHT ) || ( kd_k1 & 0x20 ) || ( kd_k2_2 & 0x02 ) ) ret |= KEY_RIGHT;
	if( !( JOYGET( 0 ) & JOYA  ) || kd_b & 0x20 ) { /* Ａボタン */
		if( repeat_flag_a ) {
			ret |= KEY_A;
			repeat_flag_a = KEY_FALSE;
			}
		} else {
			repeat_flag_a = KEY_TRUE;
		}
	if( !( JOYGET( 0 ) & JOYB  ) || kd_b & 0x40 ) { /* Ｂボタン */
		if( repeat_flag_b ) {
			ret |= KEY_B;
			repeat_flag_b = KEY_FALSE;
			}
		} else {
			repeat_flag_b = KEY_TRUE;
		}
	return ret;
}


void main(void)
{
	SI superchk;
	SI crtmod;
	SI a,b,c,d,e,f,g,i,j,k,l,m;
	SI x,y,x1,x2,y1,y2;
	SI px1,px2;
	SI h,w;
	SI col_1, col_2, col_3;
	SI loop;
	SI RD[1024];
	SI time_cal = 0;
	SI time_cal_PH = 0;
	US	ras_tmp[512];
	
	/* スーパーバイザーモード開始 */
	/*ＤＯＳのスーパーバイザーモード開始*/
	superchk = SUPER(0);
	if( superchk < 0 ) {
		puts("すでに入ってる。");
	} else {
		puts("入った。");
	}
	
	/*画面の初期設定*/
	crtmod = CRTMOD(11);	/* 偶数：標準解像度、奇数：標準 */
	G_CLR_ON();
	VPAGE(1);
	APAGE(0);
	WINDOW( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW, Y_MAX_DRAW);
	HOME(0, X_OFFSET, Y_OFFSET);
	WIPE();
	
	/*カーソルを消します。*/
	B_CUROFF();

	/* MFP関連の初期化 */
	Init_MFP();

	/*Timer-Dセット*/
	SetNowTime(0);

	px1 = X_MAX_H;
	px2 = X_MAX_H;
	speed = 0;
	
	for(a=0; a < 1024; a++)
	{
		RD[a] = 0;
	}
	/* 乱数の初期化 */
	for(a = 0; a  < 100; a++)
	{
		b = (rand() % 768);
		c = (rand() % 5) - 2;
		for(d = b; d < b + 100; d++)
		{
			RD[d] = c;
		}
	}
	for(a=0; a < 1024; a++)
	{
		ras_tmp[a] = rand() % 512;
	}
	
	for(a=0; a < 0xFF; a++)
	{
		GPALET( a, SetRGB(31, 31, 31));	/* White */
	}
	GPALET( 0, SetRGB( 0,  0,  0));	/* Black */
	GPALET( 1, SetRGB(16, 16, 16));	/* Glay1 */
	GPALET( 2, SetRGB(15, 15, 15));	/* D-Glay */
	GPALET( 3, SetRGB( 0,  0, 31));	/* Blue */
	GPALET( 5, SetRGB(31,  0,  0));	/* Red */
	GPALET( 8, SetRGB( 0, 31,  0));	/* Green */
	GPALET( 9, SetRGB( 0, 30,  0));	/* Green */
	GPALET(10, SetRGB( 0, 29,  0));	/* Green */
	GPALET(11, SetRGB( 0, 28,  0));	/* Green */
	GPALET(12, SetRGB( 0, 27,  0));	/* Green */
	GPALET(13, SetRGB( 0, 26,  0));	/* Green */
	GPALET(14, SetRGB(16, 16, 16));	/* Glay2 */
	GPALET(15, SetRGB(31, 31, 31));	/* White */

	/* 空 */
	Draw_Fill( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW, Y_MAX_DRAW, 3);

	/* 建物とコースの間 */
	Draw_Fill( X_MIN_DRAW,  Y_HORIZON, X_MAX_DRAW,  Y_HORIZON, 1);

	/* 建物 */
	for(e = 0; e < 8; e++)
	{
		Draw_Fill((e << 6), 0/*Y_HORIZON-16-1*/, (e << 6) + 10, Y_HORIZON-1, 14);
	}

	a = 0;
	c = 1;
	e = 8;
	
	/* 画面の描画 */
	for(y=Y_HORIZON; y < Y_MAX; y++)
	{
		/* Y */
		h = y - Y_HORIZON;
		y1 = Y_MAX - (y - Y_HORIZON);	/* 下から描画 */

		/* X */
		w = y - Y_HORIZON;
		i = 32 * ((float)((Y_MAX - Y_HORIZON) - w) / (Y_MAX - Y_HORIZON));	/* 縁石幅を決める */
		x = ((float)(X_MAX_H - X_MIN) / (Y_MAX - Y_HORIZON)) * w;
		x1 = X_MIN + x;
		x2 = X_MAX - x;
		
		/* コース補間 */
		if(px1 > x2){
			x2 = px1;
		}
		else if(px2 < x1){
			x1 = px2;
		}
		else{
		}
		px1 = x1;
		px2 = x2;
		
		/* 縁石の奥行 */
		b = 64 * (2 /(float)(c*(c+1)));	/* 2mの視点からの奥行の比率 */
		if(h > b + e){
			if(a == 0){
				a=1;
			}
			else{
				a=0;
			}
			e = h;
			c++;
		}
		
		/* パレットアニメーション */
		if( a == 0 ){
			col_1 = 1;
			col_2 = 5;
			col_3 = 11;
		}
		else{
			col_1 = 2;
			col_2 = 15;
			col_3 = 8;
		}
#if 1
		/* コースアウト */
		Draw_Line( X_MIN_DRAW,     y1,         x1,   y1,  col_3, 0xFFFF);	/* 左側 */
		Draw_Line(         x2,     y1, X_MAX_DRAW,   y1,  col_3, 0xFFFF);	/* 右側 */
#endif			
		/* 道1 */
		Draw_Line(         x1,     y1,         x2,   y1,  col_1, 0xFFFF );
#if 1
		/* 側道 */
		Draw_Line(     x1 - i,     y1,         x1,   y1,  col_2, 0xFFFF );	/* 左側 */
		Draw_Line(         x2,     y1,     x2 + i,   y1,  col_2, 0xFFFF );	/* 右側 */
#endif
	}
	
	VDISPST(Vsync_Func, 0, 1);			/* V-Sync割り込み 帰線 */

	speed = 0;
	loop = 1;
	do
	{
#if 0		
		US time, time_old, time_now;
		GetNowTime(&time_old);
#endif

		if( ( BITSNS( 2 ) & 0x02 ) != 0 ) loop = 0;	/* Ｑで終了 */
		if( ( BITSNS( 0 ) & 0x02 ) != 0 ) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;

#if 1
		if(get_key() == KEY_RIGHT)	speed += 1; 
		if(get_key() == KEY_LEFT)	speed -= 1; 
//		if(speed >= RASTER_SIZE-1)speed =  RASTER_SIZE-1;
//		if(speed <= 0)speed = 0;
		if(speed >=  7)speed =  7;
		if(speed <= -8)speed = -8;
//		if(speed >=  32767)speed =  32767;
//		if(speed <= -32768)speed = -32768;
#else
		a++;
		if(a >= 1024)
		{
			a = 0;
			speed = 0;
		}
		c += RD[a];
		speed = c;
		if(speed >  10)speed =  10;
		if(speed < -10)speed = -10;
#endif
		Message_Num(speed, 0, 0);
		
		for(y=0; y < RASTER_MAX; y++)
		{
			ras_tmp[y] = X_MIN;
			if(y >= RASTER_ST){
				ras_tmp[y] += speed * ( (RASTER_ED - RASTER_ST) / (float)(Mmax(y-RASTER_ST, 1)) );
			}
		}
		SetRasterVal(ras_tmp, sizeof(US)*RASTER_MAX);
		
		moni = ras_tmp[Mabs(speed)];
		moni_MAX = sizeof(US)*RASTER_MAX;
		
		Message_Num(moni, 0, 1);
		Message_Num(moni_MAX, 0, 2);
		
#if 0		
		/* 処理時間計測 */
		GetNowTime(&time_now);
		time = time_now - time_old;
		time_cal = time << 4;	/* 物量変換 */
		time_cal_PH = Mmax(time_cal, time_cal_PH);
		Message_Num(time_cal, 0, 3);
		Message_Num(time_cal_PH, 0, 4);
#endif
		/* 同期待ち */
		vwait(1);
	}
	while( loop );

	CRTCRAS((void *)0, 0);		/* stop */
	HSYNCST((void *)0);			/* stop */
	VDISPST((void *)0, 0, 0);	/* stop */
	TIMERDST((void *)0, 0, 1);	/* Stop */

	B_CURON();

	CRTMOD(crtmod);
	
	/*スーパーバイザーモード終了*/
	SUPER(superchk);
}

#endif	/* OVERKATA_C */
