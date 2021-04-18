#ifndef	CRTC_C
#define	CRTC_C

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <iocslib.h>

#include "inc/usr_macro.h"
#include "CRTC.h"

/* グローバル変数 */

FLOAT dot_clock8[3][4] = {
	1.64678, 0.82339, 1.64678, 1.64678, 
	0.69013, 0.34507, 0.23004, 0.31778, 
	0.92017, 0.46009, 0.23004, 0.31778 
};

FLOAT freq_data[5][10] = {
	22.0842, 31.7460, 3.4507, 4.1408, 2.0704, 16.2540, 18.0317, 0.1905, 1.1111, 0.4762,
	25.4220, 31.7776, 3.8133, 1.9067, 0.6356, 15.2532, 17.2552, 0.0636, 1.0169, 0.9215,
	25.4220, 31.7776, 3.8133, 1.9464, 0.5958, 15.2532, 16.6832, 0.0636, 1.0169, 0.3496,
	29.4456, 40.4877, 3.6807, 4.6009, 2.7605, 17.1668, 18.8268, 0.3239, 1.0122, 0.3239,
	52.6971, 62.5778, 3.2936, 4.9404, 1.6468, 15.0187, 16.2702, 0.1877, 0.8761, 0.1877
};

uint32_t	hl, clk, freq, scanmode, colmode, size;
uint32_t	yesno;	
uint32_t	ir[8],ir20;
uint32_t	hres, vres;
FLOAT	fr[8];
uint8_t	keybuf[128];
int32_t	stack;
int8_t	g_CRT_Contrast = -1;

volatile uint16_t  *crtc = (uint16_t *)0xE80000u;
volatile uint16_t  *vcon = (uint16_t *)0xE82400u;
volatile uint16_t  *scon = (uint16_t *)0xEB080Au;
volatile uint16_t  *sysp = (uint16_t *)0xE8E007u;
volatile uint16_t  *CRTC_R0  = (uint16_t *)0xE80000u;
volatile uint16_t  *CRTC_R1  = (uint16_t *)0xE80002u;
volatile uint16_t  *CRTC_R2  = (uint16_t *)0xE80004u;
volatile uint16_t  *CRTC_R3  = (uint16_t *)0xE80006u;
volatile uint16_t  *CRTC_R4  = (uint16_t *)0xE80008u;
volatile uint16_t  *CRTC_R5  = (uint16_t *)0xE8000Au;
volatile uint16_t  *CRTC_R6  = (uint16_t *)0xE8000Cu;
volatile uint16_t  *CRTC_R7  = (uint16_t *)0xE8000Eu;
volatile uint16_t  *CRTC_R8  = (uint16_t *)0xE80010u;
volatile uint16_t  *CRTC_R20 = (uint16_t *)0xE8002Eu;

/* 構造体定義 */
ST_CRT		g_stCRT[CRT_MAX] = {0};


/* 関数のプロトタイプ宣言 */
void CRTC_INIT(void);
void CRTC_INIT_Manual(void);
void get_value(int8_t *, uint32_t *, uint32_t, uint32_t);
void cal_regs(void);
void dsp_regs(void);
void set_regs(void);
int16_t	GetCRT(ST_CRT *, int16_t);
int16_t	SetCRT(ST_CRT, int16_t);
int16_t	CRT_INIT(void);
int16_t	Get_CRT_Contrast(int8_t *);
int16_t	Set_CRT_Contrast(int8_t);


/* 関数 */
void CRTC_INIT(void)
{
	hl		= 3 - 1;
	
	clk		= 2 - 1;
	if( (hl == 0) && (clk == 2) )
	{
		clk = 0;
	}
	
	freq	= 1 - 1;
	
	do
	{
		scanmode= 3 - 1;
	}
	while( (hl == 0) && (scanmode == 2) );
	
	size	= 1 - 1;
	
	colmode	= 2 - 1;

	cal_regs();

	dsp_regs();

	set_regs();
}	


void CRTC_INIT_Manual(void)
{
	get_value("ドットクロック 1:Low 2:High 3:Middle =", &hl,	1,	3);
	
	get_value("ドットクロック 1:256 2:512 3:768 4:special =", &clk,	1,	4);
	
	if( (hl == 0) && (clk == 2) )
	{
		printf("Lowモードでは256と同じ\n");
		clk = 0;
	}
	
	get_value("水平周波数 1:31kHz 2:31kHz(V) 3:31kHz(VGA) 4:24kHz 5:15kHz =", &freq,	1,	5);
	
	do
	{
		get_value("走査モード 1:ノン・インターレース 2:インターレース 3:二度読み =", &scanmode,	1,	3);
	}
	while( (hl == 0) && (scanmode == 2) );
	
	get_value("実画面サイズ 1:512x512ドット 2:1024x1024ドット =", &size,	1,	2);
	
	get_value("色数 1:16色 2:256色 3:65535色 =", &colmode,	1,	3);
	
	cal_regs();
	
	dsp_regs();
	
	get_value("設定しますか？ 1:No 2:Yes =", &yesno,	1,	2);
	
	if(yesno != 0)
	{
		set_regs();
	}
}

void get_value(int8_t *s, uint32_t *i, uint32_t l, uint32_t h)
{
	do
	{
		printf("%s", s);
		if( !scanf("%d", i) )
		{
			printf("\n");
			scanf("%s", keybuf);
		}
	}
	while( (*i < l) || (*i > h ) );
	(*i)--;
}

void cal_regs(void)
{
	uint32_t	i, hilo;
	FLOAT	dot_clk, hsync;
	
	dot_clk = dot_clock8[hl][clk];
	
	hsync = freq_data[freq][1];
	
	fr[0] = ( freq_data[freq][1] / dot_clk) - 1.0;
	fr[1] = ( freq_data[freq][2] / dot_clk) - 1.0;
	fr[2] = ((freq_data[freq][2] + freq_data[freq][3]) / dot_clk) - 5.0;
	fr[3] = ((freq_data[freq][1] - freq_data[freq][4]) / dot_clk) - 5.0;
	fr[4] = ( freq_data[freq][6] * 1000.0 / hsync) - 1.0;
	fr[5] = ( freq_data[freq][7] * 1000.0 / hsync) - 1.0;
	fr[6] = ((freq_data[freq][7] + freq_data[freq][8]) * 1000.0 / hsync) - 1.0;
	fr[7] = ((freq_data[freq][6] - freq_data[freq][9]) * 1000.0 / hsync) - 1.0;
	
	for(i=0; i<8; i++)
	{
		if( fr[i] < 0.0 )
		{
			printf("ERROR:R%d (%f)\n", i, fr[i]);
			continue;
		}
		ir[i] = (uint32_t)(fr[i] + 0.5);
	}
	
//ir[1] = 0x06;
//ir[2] = 0x0B;
	
	ir[0] |= 1;
	
	if(colmode == 2)
	{
		colmode = 3;
	}
	
	if(hl >= 1)
	{
		hilo = 1;
	}
	else
	{
		hilo = 0;
	}
	
	ir20 = size * 0x400 + colmode * 0x100 + hilo * 0x10 + clk;
	
	if( ((hl == 0) && (scanmode == 1)) || ((hl >= 1) && (scanmode == 0)) )
	{
		ir20 += 4;
	}
	if((hl >= 1) && (scanmode == 1))
	{
		ir20 += 8;
	}
	
	hres = (ir[3] - ir[2]) * 8;
	vres = ir[7] - ir[6];
	
	if(scanmode == 1)
	{
		vres *= 2;
	}
	if(scanmode == 2)
	{
		vres /= 2;
	}
}

void dsp_regs()
{
	printf("R0   = %5d ($%04X)\n", ir[0], ir[0]);
	printf("R1   = %5d ($%04X)\n", ir[1], ir[1]);
	printf("R2   = %5d ($%04X)\n", ir[2], ir[2]);
	printf("R3   = %5d ($%04X)\n", ir[3], ir[3]);
	printf("R4   = %5d ($%04X)\n", ir[4], ir[4]);
	printf("R5   = %5d ($%04X)\n", ir[5], ir[5]);
	printf("R6   = %5d ($%04X)\n", ir[6], ir[6]);
	printf("R7   = %5d ($%04X)\n", ir[7], ir[7]);
	printf("R20  = %5d ($%04X)\n", ir20, ir20);
	printf("[%4d]x[%4d]\n", hres, vres);
}

void set_regs(void)
{
	uint32_t	i;
	uint16_t	r20;
//	stack = SUPER(0);
	r20 = *(crtc + 20) & 0x13;
	
	if(ir20 >= r20)
	{
		for(i=0; i<8; i++)
		{
			*(crtc + i) = ir[i];
		}
		*(crtc + 20) = ir20;
	}
	else
	{
		*(crtc + 20) = ir20;
		for(i=1; i<8; i++)
		{
			*(crtc + i) = ir[i];
		}
		*crtc = ir[0];
	}
	
	if(hl == 2)
	{
		*sysp |= 0x2;
	}
	else
	{
		*sysp &= ~0x2;
	}
	
	*vcon = (ir20 >> 8) & 0x07;
	
	*(scon + 1) = ir[2] + 4;

	for(i=0; i<0x200; i++)
	{
		/* 何もしない */
	}
	
	if( (ir20 & 0x1F) == 0 )
	{
		*scon = ir[0];
	}
	else
	{
		*scon = 0xFF;
	}
	
	*(scon + 2) = ir[6];
	*(scon + 3) = ir20 & 0xFF;
	
//	SUPER(stack);
}

int16_t	GetCRT(ST_CRT *stDat, int16_t Num)
{
	int16_t	ret = 0;
	
	if(Num < CRT_MAX)
	{
		*stDat = g_stCRT[Num];
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

int16_t	SetCRT(ST_CRT stDat, int16_t Num)
{
	int16_t	ret = 0;
	
	if(Num < CRT_MAX)
	{
		g_stCRT[Num] = stDat;
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

int16_t CRT_INIT(void)
{
	int16_t	ret = 0;
	volatile uint16_t *CRTC_R21 = (uint16_t *)0xE8002Au;
	
	ret = CRTMOD(-1);	/* 現在のモードを返す */
	CRTMOD(11);			/* 偶数：31kHz、奇数：15kHz(17,18:24kHz) */

//										   FEDCBA9876543210
	*CRTC_R21 = Mbset(*CRTC_R21, 0x03FF, 0b0000000000000000);	/* CRTC R21 */
//										   |||||||||||||||+-bit0 CP0	
//										   ||||||||||||||+--bit1 CP1	
//										   |||||||||||||+---bit2 CP2	
//										   ||||||||||||+----bit3 CP3	
//										   |||||||||||+-----bit4 AP0	同時アクセス
//										   ||||||||||+------bit5 AP1	同時アクセス
//										   |||||||||+-------bit6 AP2	同時アクセス
//										   ||||||||+--------bit7 AP3	同時アクセス
//										   |||||||+---------bit8 SA		同時アクセス
//										   ||||||+----------bit9 MEN	同時アクセスマスク

	/* CRTの設定 */
	g_stCRT[0].view_offset_x	= X_OFFSET;
	g_stCRT[0].view_offset_y	= Y_MIN_DRAW;
	g_stCRT[0].hide_offset_x	= X_OFFSET;
	g_stCRT[0].hide_offset_y	= Y_OFFSET;
	g_stCRT[0].BG_offset_x		= 0 + SP_X_OFFSET;
	g_stCRT[0].BG_offset_y		= Y_HORIZON_0 + SP_Y_OFFSET;
	
	g_stCRT[1].view_offset_x	= X_OFFSET;
	g_stCRT[1].view_offset_y	= Y_OFFSET;
	g_stCRT[1].hide_offset_x	= X_OFFSET;
	g_stCRT[1].hide_offset_y	= Y_MIN_DRAW;
	g_stCRT[1].BG_offset_x		= 0;
	g_stCRT[1].BG_offset_y		= Y_HORIZON_1;

	g_stCRT[2].view_offset_x	= X_OFFSET;
	g_stCRT[2].view_offset_y	= Y_MIN_DRAW;
	g_stCRT[2].hide_offset_x	= X_OFFSET;
	g_stCRT[2].hide_offset_y	= Y_OFFSET;
	g_stCRT[2].BG_offset_x		= 0;
	g_stCRT[2].BG_offset_y		= Y_HORIZON_1;
	
	Set_CRT_Contrast(-1);	/* コントラスト初期化 */
	
	return ret;
}

int16_t Get_CRT_Contrast(int8_t *pbContrast)
{
	int16_t	ret = 0;
	
	*pbContrast = _iocs_contrast(-1);
	
	return ret;
}

int16_t Set_CRT_Contrast(int8_t bContrast)
{
	int16_t ret = 0;
	
	if(g_CRT_Contrast < 0)
	{
		Get_CRT_Contrast(&g_CRT_Contrast);
	}

	if( (bContrast >= 0) && (bContrast <= 15) )
	{
		_iocs_contrast(bContrast);		/* 設定値 */
	}
	else
	{
		_iocs_contrast(g_CRT_Contrast);	/* 初期値 */
	}
	
	return ret;
}

#endif	/* CRTC_C */

