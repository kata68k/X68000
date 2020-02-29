#include <iocslib.h>
#include <interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>

#define WIDTH		(256)
#define HEIGHT		(256)

/* Ｘ，Ｙ座標の最大と最小 */
#define X_MIN_DRAW	(0)
#define X_MAX_DRAW	(511)
#define X_OFFSET	(128)
#define X_MIN		X_OFFSET
#define X_MAX		(WIDTH+X_OFFSET)
#define X_MAX_H		((WIDTH>>1)+X_OFFSET)

#define Y_MIN_DRAW	(0)
#define Y_MAX_DRAW	(511)
#define Y_OFFSET	(128)
#define Y_MIN		Y_OFFSET
#define Y_MAX		(HEIGHT+Y_OFFSET)
#define Y_MAX_H		((HEIGHT>>1)+Y_OFFSET)
#define Y_HORIZON	(Y_MAX_H)

/* ラスタ情報(256*256 31kHzの場合 ラインは2倍計算) */
#define RASTER_NEXT	(24)
#define RASTER_SIZE	(256)
#define RASTER_MIN	(40)
#define RASTER_MAX	(RASTER_MIN + 512)
#define RASTER_ST	(Y_MAX_H+RASTER_MIN-20-2)
#define RASTER_ED	(RASTER_ST+RASTER_SIZE)

/* ジョイスティック１のその１ */
#define UP		(0x01)
#define DOWN	(0x02)
#define LEFT	(0x04)
#define RIGHT	(0x08)
#define JOYA	(0x20)
#define JOYB	(0x40)
#define ESC_S	(27)

/* ジョイスティック１のその１ */
#define KEY_NULL		(0x00)
#define KEY_UPPER		(0x04)
#define KEY_LOWER		(0x08)
#define KEY_LEFT		(0x01)
#define KEY_UP_LEFT		(0x05)
#define KEY_DW_LEFT		(0x09)
#define KEY_RIGHT		(0x02)
#define KEY_UP_RIGHT	(0x06)
#define KEY_DW_RIGHT	(0x0A)
#define KEY_A			(0x10)
#define KEY_B			(0x20)

#define KEY_TRUE	1
#define KEY_FALSE	0

/* ボタンのデータ */
static short key;
static volatile unsigned short NowTime;
static volatile unsigned short moni;
static volatile unsigned short moni_MAX;
static volatile int speed;
static volatile unsigned short ras_count;
static volatile unsigned short Hsync_count;

enum{
	DST_none,
	DST_1us,
	DST_2p5us,
	DST_4us,
	DST_12p5us,
	DST_16us,
	DST_25us,
	DST_50us,
};
/*
        Mmax - 最大値取得 -
*/
/**
 * 指定された2つの引数のうち,最大のものを返す.
 * @param [in] a        最大値を選ぶ対象の変数1
 * @param [in] b        最大値を選ぶ対象の変数2
 * @retval 最大値
 * @attention 引数は複数回評価される.
 */
#define Mmax(a, b) ((a) > (b) ? (a) : (b))

/*
        Mmin - 最小値取得 -
*/
/**
 * 指定された2つの引数のうち,最小のものを返す.
 * @param [in] a        最小値を選ぶ対象の変数1
 * @param [in] b        最小値を選ぶ対象の変数2
 * @retval 最小値
 * @attention 引数は複数回評価される.
 */
#define Mmin(a, b) ((a) < (b) ? (a) : (b))

/* 関数のプロトタイプ宣言 */
void interrupt Timer_D_Func(void);
void interrupt Hsync_Func(void);
void interrupt Raster_Func(void);
void interrupt Vsync_Func(void);

unsigned short SetRGB(unsigned short R, unsigned short G, unsigned short B)
{
	return (( G << 11) + (R << 6) + (B << 1));
}

int Draw_Pset(short x, short y, unsigned short color)
{
	struct _psetptr stPset;

	stPset.x = Mmin(x, X_MAX_DRAW);
	stPset.y = Mmin(y, Y_MAX_DRAW);
	stPset.color = color;
	
	return PSET(&stPset);
}

int Draw_Line(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style)
{
	struct _lineptr stLine;

	stLine.x1 = Mmin(x1, X_MAX_DRAW);
	stLine.y1 = Mmin(y1, Y_MAX_DRAW);
	stLine.x2 = Mmin(x2, X_MAX_DRAW);
	stLine.y2 = Mmin(y2, Y_MAX_DRAW);
	stLine.color = color;
	stLine.linestyle = style;
	
	return LINE(&stLine);
}

int Draw_Box(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style)
{
	struct _boxptr stBox;

	stBox.x1 = Mmin(x1, X_MAX_DRAW);
	stBox.y1 = Mmin(y1, Y_MAX_DRAW);
	stBox.x2 = Mmin(x2, X_MAX_DRAW);
	stBox.y2 = Mmin(y2, Y_MAX_DRAW);
	stBox.color = color;
	stBox.linestyle = style;
	
	return BOX(&stBox);
}

int Draw_Fill(short x1, short y1, short x2, short y2, unsigned short color)
{
	struct _fillptr stFill;

	stFill.x1 = Mmin(x1, X_MAX_DRAW);
	stFill.y1 = Mmin(y1, Y_MAX_DRAW);
	stFill.x2 = Mmin(x2, X_MAX_DRAW);
	stFill.y2 = Mmin(y2, Y_MAX_DRAW);
	stFill.color = color;
	
	return FILL(&stFill);
}

int Draw_Circle(short x, short y, unsigned short rad, unsigned short color, short st, short ed, unsigned short rat)
{
	struct _circleptr stCircle;

	stCircle.x = x;
	stCircle.y = y;
	stCircle.radius = rad;
	stCircle.color = color;
	stCircle.start = st;
	stCircle.end = ed;
	stCircle.ratio = rat;
	
	return CIRCLE(&stCircle);
}

void Message_Num(int num, int x, int y)
{
	char str[64];

	sprintf(str, "%d", num);
	B_PUTMES( 3, x, y, 10-1, str);
}

short get_key( void )
{
	unsigned int kd_k1,kd_k2_1,kd_k2_2,kd_b;
	static short repeat_flag_a = KEY_TRUE;
	static short repeat_flag_b = KEY_TRUE;
	short ret = 0;
	
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

void interrupt Timer_D_Func(void)
{
	NowTime++;

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

void interrupt Hsync_Func(void)
{
	HSYNCST((void *)0);		/* stop */
	HSYNCST(Hsync_Func);	/* H-Sync割り込み */

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

void interrupt Raster_Func(void)
{
	static volatile unsigned short ras_cal = 0;
	static volatile unsigned short ras_val = 0;
	volatile unsigned short *scroll_x = (unsigned short *)0xE80018;
	volatile unsigned short *raster_add = (unsigned short *)0xE80012;
	
//	CRTCRAS((void *)0, 0);	/* stop */
	
	ras_count += RASTER_NEXT;
	ras_val = X_MIN;

	/* ラスタ領域 */
	if((ras_count >= RASTER_ST) && (ras_count <= (RASTER_ST + 20)))
	{
		ras_val += speed;
	}
	else if(ras_count < RASTER_MAX)
	{
		ras_val += speed * ( RASTER_SIZE  / (float)(Mmax((ras_count - RASTER_ST), 1)) );
	}
	else
	{
		ras_count = RASTER_ST;
	}

	*(scroll_x) = ras_val;

	moni = ras_count;
	
//	CRTCRAS(Raster_Func, ras_count);	/* ラスター割り込み */
	*raster_add = ras_count;
	
	if(Hsync_count != 0U)
	{
		ras_cal++;
	}
	else{
		ras_cal = 0;
	}
	
	if(ras_cal > 32)
	{
		ras_cal = 0;
	}
	moni_MAX = ras_cal;
	
	Hsync_count = 1;

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

void interrupt Vsync_Func(void)
{
	static volatile unsigned short PalAnime = 0;

	VDISPST((void *)0, 0, 0);	/* stop */
	
	if((PalAnime % 10) < 5)
	{
		GPALET( 1, SetRGB(16, 16, 16));	/* Glay */
		GPALET( 2, SetRGB(15, 15, 15));	/* D-Glay */
		
		GPALET( 5, SetRGB(31,  0,  0));	/* Red */
		GPALET(15, SetRGB(31, 31, 31));	/* White */
		
		GPALET( 8, SetRGB( 0, 31,  0));	/* Green */
		GPALET(11, SetRGB( 0, 28,  0));	/* Green */
	}
	else
	{
		GPALET( 2, SetRGB(16, 16, 16));	/* Glay */
		GPALET( 1, SetRGB(15, 15, 15));	/* D-Glay */
		
		GPALET(15, SetRGB(31,  0,  0));	/* Red */
		GPALET( 5, SetRGB(31, 31, 31));	/* White */
		
		GPALET(11, SetRGB( 0, 31,  0));	/* Green */
		GPALET( 8, SetRGB( 0, 28,  0));	/* Green */
	}
	PalAnime++;

	//moni_MAX = Mmax(moni_MAX, moni);

	Hsync_count = 0;
//	HSYNCST(Hsync_Func);				/* H-Sync割り込み */
	VDISPST(Vsync_Func, 0, 1);

	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

int vwait(int count)				/* 約count／60秒待つ	*/
{
	volatile char *mfp = (char *)0xe88001;
	
	while(count--){
		while(!((*mfp) & 0b00010000));	/* 垂直表示期間待ち	*/
		while((*mfp) & 0b00010000);	/* 垂直帰線期間待ち	*/
	}
}

void main(void)
{
	int superchk;
	int crtmod;
	int a,b,c,d,e,f,g,i,j,k,l,m;
	int x,y,x1,x2,y1,y2;
	int px1,px2;
	int h,w;
	int col_1, col_2, col_3;
	int loop;
	int RD[1024];
	int time_cal = 0;
	int time_cal_PH = 0;
	
	/* スーパーバイザーモード開始 */
	/*ＤＯＳのスーパーバイザーモード開始*/
	superchk = SUPER(0);
	if( superchk < 0 ) {
		puts("すでに入ってる。");
	} else {
		puts("入った。");
	}
	
	/*画面の初期設定*/
	crtmod = CRTMOD(10);	/* 偶数：標準解像度、奇数：標準 */
	G_CLR_ON();
	VPAGE(1);
	APAGE(0);
	WINDOW( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW, Y_MAX_DRAW);
	HOME(0, X_OFFSET, Y_OFFSET);
	WIPE();
	
	/*カーソルを消します。*/
	B_CUROFF();

	/*Timer-Dセット*/
	NowTime = 0;

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
		
		/* パレットアニメーション */
		if( (i % 2) == 0 ){
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
	
	CRTCRAS(Raster_Func, (RASTER_MAX - RASTER_MIN)/2 );	/* ラスター割り込み */
	VDISPST(Vsync_Func, 0, 1);
	
	loop = 1;
	do
	{
#if 0		
		unsigned short time;
		time = NowTime;
#endif

		if( ( BITSNS( 2 ) & 0x02 ) != 0 ) loop = 0;	/* Ｑで終了 */
		if( ( BITSNS( 0 ) & 0x02 ) != 0 ) loop = 0;	/* ＥＳＣポーズ */
		if(loop == 0)break;

#if 1
		if(get_key() == KEY_RIGHT)	speed += 1; 
		if(get_key() == KEY_LEFT)	speed -= 1; 
		if(speed >=  32767)speed =  32767;
		if(speed <= -32768)speed = -32768;
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
		
		Message_Num(moni, 0, 1);
		Message_Num(moni_MAX, 0, 2);
		
#if 0		
		/* 処理時間計測 */
		time = NowTime - time;
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
