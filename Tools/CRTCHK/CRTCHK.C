#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <iocslib.h>

#define FALSE	0
#define TRUE	!(FALSE)

#define SetRGB(R,G,B)	(( G << 11) + (R << 6) + (B << 1))

enum{
	CRT_MODE_0,
	CRT_MODE_1,
	CRT_MODE_2,
	CRT_MODE_3,
	CRT_MODE_4,
	CRT_MODE_5,
	CRT_MODE_6,
	CRT_MODE_7,
	CRT_MODE_8,
	CRT_MODE_9,
	CRT_MODE_10,
	CRT_MODE_11,
	CRT_MODE_12,
	CRT_MODE_13,
	CRT_MODE_14,
	CRT_MODE_15,
	CRT_MODE_16,
	CRT_MODE_17,
	CRT_MODE_18,
	CRT_MODE_19,
	CRT_MODE_20,
	CRT_MODE_21,
	CRT_MODE_22,
	CRT_MODE_23,
	CRT_MODE_24,
	CRT_MODE_25,
	CRT_MODE_26,
	CRT_MODE_27,
//	CRT_MODE_28_Ex,	/* 使わない */
//	CRT_MODE_29_Ex,	/* 使わない */
	CRT_MODE_MAX
};

struct CRT_MODE
{
	short Mode;
	short Horizontal;
	short Width;
	short Height;
	unsigned short Color;
	unsigned short VRAM;
}stCRT_MODE[CRT_MODE_MAX];

char sCRT_MODE_Mess[CRT_MODE_MAX][64] = {
	"31kHz 512x512  Col/Page:16/1    VRAM:1024",/*0*/
	"15kHz 512x512(480)  Col/Page:16/1    VRAM:1024",/*1*/
	"31kHz 256x256  Col/Page:16/1    VRAM:1024",/*2*/
	"15kHz 256x256(240)  Col/Page:16/1    VRAM:1024",/*3*/
	"31kHz 512x512  Col/Page:16/4    VRAM:512 ",/*4*/
	"15kHz 512x512(480)  Col/Page:16/4    VRAM:512 ",/*5*/
	"31kHz 256x256  Col/Page:16/4    VRAM:512 ",/*6*/
	"15kHz 256x256(240)  Col/Page:16/4    VRAM:512 ",/*7*/
	"31kHz 512x512  Col/Page:256/2   VRAM:512 ",/*8*/
	"15kHz 512x512(480)  Col/Page:256/2   VRAM:512 ",/*9*/
	"31kHz 256x256  Col/Page:256/2   VRAM:512 ",/*10*/
	"15kHz 256x256(240)  Col/Page:256/2   VRAM:512 ",/*11*/
	"31kHz 512x512  Col/Page:65536/1 VRAM:512 ",/*12*/
	"15kHz 512x512(480)  Col/Page:65536/1 VRAM:512 ",/*13*/
	"31kHz 256x256  Col/Page:65536/1 VRAM:512 ",/*14*/
	"15kHz 256x256(240)  Col/Page:65536/1 VRAM:512 ",/*15*/
	"31kHz 768x512  Col/Page:16/1    VRAM:1024",/*16*/
	"24kHz 1024x424 Col/Page:16/1    VRAM:1024",/*17*/
	"24kHz 1024x848 Col/Page:16/1    VRAM:1024",/*18*/
	"24kHz 640x480  Col/Page:16/1    VRAM:1024",/*19*/
	"31kHz 768x512  Col/Page:256/2   VRAM:1024",/*20*/
	"24kHz 1024x424 Col/Page:256/2   VRAM:1024",/*21*/
	"24kHz 1024x848 Col/Page:256/2   VRAM:1024",/*22*/
	"24kHz 640x480  Col/Page:256/2   VRAM:1024",/*23*/
	"31kHz 768x512  Col/Page:65536/1 VRAM:1024",/*24*/
	"24kHz 1024x424 Col/Page:65536/1 VRAM:1024",/*25*/
	"24kHz 1024x848 Col/Page:65536/1 VRAM:1024",/*26*/
	"24kHz 640x480  Col/Page:65536/1 VRAM:1024",/*27*/
//	"24kHz 680x424  Col/Page:16/1    VRAM:1024",/*28*/	/* 使わない */
//	"31kHz 384x256  Col/Page:65536/1 VRAM:1024",/*29*/	/* 使わない */
};

char	tCRT_15kHz[ 8] = {  1,  3,  5,  7,  9, 11, 13, 15 };
char	tCRT_24kHz[10] = { 17, 18, 19, 21, 22, 23, 25, 26, 27, 28};
char	tCRT_31kHz[11] = {  0,  2,  4,  8, 10, 12, 14, 16, 20, 24, 29};

unsigned short tPallet[16] = {
	SetRGB( 0,  0,  0),	/* Black */
	SetRGB( 7,  7,  7),	/* Glay */
	SetRGB(23, 23, 23),	/* D-White */
	SetRGB(31, 31, 31),	/* White */
	SetRGB(15,  0, 15),	/* D-Perple */
	SetRGB(31,  0, 31),	/* Perple */
	SetRGB(15,  0,  0),	/* D-Red */
	SetRGB(31,  0,  0),	/* Red */
	SetRGB(15, 15,  0),	/* D-Yellow */
	SetRGB(31, 31,  0),	/* Yellow */
	SetRGB( 0, 15,  0),	/* D-Green */
	SetRGB( 0, 31,  0),	/* Green */
	SetRGB( 0, 15, 15),	/* D-Water */
	SetRGB( 0, 31, 31),	/* Water */
	SetRGB( 0,  0, 15),	/* D-Blue */
	SetRGB( 0,  0, 31),	/* Blue */
};

int		g_ROMVer;

int Disp_Mode(int);
int Disp_Counter(unsigned int, unsigned char);
int Set_CRT_Mode(int , int);
int Set_CRT_ModeEx(int, int);
void View_Line(unsigned short, unsigned short);
void G_Palette(void);
void G_Palette16(void);
void T_PALET(void);

int Disp_Mode(int crtmod)
{
	_iocs_b_locate(0, 0);
	puts("CRTCHK.x Ver0.93a");
	printf("Mode[%2d] %s\n", crtmod, sCRT_MODE_Mess[crtmod]);
	puts("SPACE = next");
	puts("BS    = back");
	puts("ESC   = exit");
}

int Disp_Counter(unsigned int counter, unsigned char raw)
{
	static int y = 0;
	
	if(y >= raw)
	{
		int i;
		for(i=0; i<raw; i++)
		{
			_iocs_b_locate(0, 6 + i);
			printf("%10d\n", counter - raw + i);
		}
	}
	else
	{
		_iocs_b_locate(0, 6 + y);
		printf("%10d\n", counter);

		y++;
	}
	
	
}

int Set_CRT_Mode(int mode, int init_flag)
{
	int crtmode_ret;
	
	if( mode == -1 ) 
	{
		crtmode_ret = CRTMOD(-1);
	}
	else if(mode > CRT_MODE_27)
	{
		Set_CRT_ModeEx(mode, init_flag);

		G_CLR_ON();
		/*カーソルを消します。*/
		B_CUROFF();
	}
	else
	{
		if(init_flag != 0)
		{
			mode += 0x100;	/* 画面の初期化を行わない*/
		}
		CRTMOD(mode);
		G_CLR_ON();
		
		/*カーソルを消します。*/
		B_CUROFF();
	}

	return crtmode_ret;
}

int Set_CRT_ModeEx(int mode, int init_flag)
{
	int ret = 0;
#if 0
	volatile unsigned short  *CRTC_R0  = (unsigned short *)0xE80000u;
	volatile unsigned short  *CRTC_R1  = (unsigned short *)0xE80002u;
	volatile unsigned short  *CRTC_R2  = (unsigned short *)0xE80004u;
	volatile unsigned short  *CRTC_R3  = (unsigned short *)0xE80006u;
	volatile unsigned short  *CRTC_R4  = (unsigned short *)0xE80008u;
	volatile unsigned short  *CRTC_R5  = (unsigned short *)0xE8000Au;
	volatile unsigned short  *CRTC_R6  = (unsigned short *)0xE8000Cu;
	volatile unsigned short  *CRTC_R7  = (unsigned short *)0xE8000Eu;
	volatile unsigned short  *CRTC_R8  = (unsigned short *)0xE80010u;
	volatile unsigned short  *CRTC_R20 = (unsigned short *)0xE8002Eu;
	
	switch(mode)
	{
	case CRT_MODE_28_Ex:
		{
			Set_CRT_Mode(19, FALSE);
			//"Frq:24kHz Rsl:680x424  Col/Page:16/1    VRAM:1024",/*28*/
			*CRTC_R0  =	0x0075;
			*CRTC_R1  =	0x000A;
			*CRTC_R2  =	0x0013;
			*CRTC_R3  =	0x0068;
			*CRTC_R4  =	0x01D0;
			*CRTC_R5  =	0x0007;
			*CRTC_R6  =	0x0020;
			*CRTC_R7  =	0x01C8;
			*CRTC_R20 =	0x0415;
		}
		break;
	case CRT_MODE_29_Ex:
		{
			Set_CRT_Mode(13, FALSE);
			//"Frq:31kHz Rsl:384x256  Col/Page:65536/1 VRAM:1024",/*29*/
			*CRTC_R0  =	0x0045;
			*CRTC_R1  =	0x0006;
			*CRTC_R2  =	0x000B;
			*CRTC_R3  =	0x003B;
			*CRTC_R4  =	0x0237;
			*CRTC_R5  =	0x0005;
			*CRTC_R6  =	0x0028;
			*CRTC_R7  =	0x0228;
			*CRTC_R20 =	0x0311;
		}
		break;
	default:
		{
			/* 何もなし */
		}
		break;
	}
#endif
	return ret;
}

void Set_Line(unsigned short x, unsigned short y)
{
	int i;
	volatile unsigned short  *GR  = (unsigned short *)0xC40000u;
	
	for(i=0; i<x; i++)
	{
		if(i == 0)
		{
			*GR = 0;
		}
		else
		{
			*GR = 65535 / i;
		}
		GR++;
	}
}
int Set_Apage(int mode)
{
	int apage_ret;
	
	if( (mode == -1) || (mode > 3) )
	{
		apage_ret = APAGE(-1);
	}
	else
	{
		apage_ret = APAGE(mode);
		
		if(apage_ret != 0){
			printf("apage error = %d\n", apage_ret);
		}
		else{
		}
	}
	/*カーソルを消します。*/
	B_CUROFF();
	
	return apage_ret;
}

int Set_Vpage(int mode)
{
	int vpage_ret;
	
	if( (mode < 0) || (mode > 3) )
	{
		vpage_ret = VPAGE(-1);
	}
	else
	{
		vpage_ret = VPAGE(mode);
		
		if(vpage_ret != 0){
			printf("vpage error = %d\n", vpage_ret);
		}
		else{
		}
	}
	/*カーソルを消します。*/
	B_CUROFF();
	
	return vpage_ret;
}

int Draw_Pset(short x, short y, unsigned short color)
{
	struct _psetptr stPset;

	stPset.x = x;
	stPset.y = y;
	stPset.color = color;
	
	return _iocs_pset(&stPset);
}

int Draw_Line(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style)
{
	struct _lineptr stLine;

	stLine.x1 = x1;
	stLine.y1 = y1;
	stLine.x2 = x2;
	stLine.y2 = y2;
	stLine.color = color;
	stLine.linestyle = style;
	
	return _iocs_line(&stLine);
}

int Draw_Box(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style)
{
	struct _boxptr stBox;

	stBox.x1 = x1;
	stBox.y1 = y1;
	stBox.x2 = x2;
	stBox.y2 = y2;
	stBox.color = color;
	stBox.linestyle = style;
	
	return _iocs_box(&stBox);
}

int Draw_Fill(short x1, short y1, short x2, short y2, unsigned short color)
{
	struct _fillptr stFill;

	stFill.x1 = x1;
	stFill.y1 = y1;
	stFill.x2 = x2;
	stFill.y2 = y2;
	stFill.color = color;
	
	return _iocs_fill(&stFill);
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
	
	return _iocs_circle(&stCircle);
}

int T_Fill(short x1, short y1, short x2, short y2, unsigned short line_style, unsigned char color)
{
	struct _txfillptr stTxFill;
	
	stTxFill.x = x1;
	stTxFill.y = y1;
	stTxFill.x1= x2;
	stTxFill.y1= y2;
	stTxFill.fill_patn = line_style;
	
	if((color == 0) && (line_style == 0))
	{
		color = 0x0F;
	}
	
	if((color & 0x01) != 0u)
	{
		stTxFill.vram_page = 0;
		_iocs_txfill(&stTxFill);
	}
	if((color & 0x02) != 0u)
	{
		stTxFill.vram_page = 1;
		_iocs_txfill(&stTxFill);
	}
	if((color & 0x04) != 0u)
	{
		stTxFill.vram_page = 2;
		_iocs_txfill(&stTxFill);
	}
	if((color & 0x08) != 0u)
	{
		stTxFill.vram_page = 3;
		_iocs_txfill(&stTxFill);
	}
}

int T_Box(short x1, short y1, short x2, short y2, unsigned short line_style, unsigned char color)
{
	struct _tboxptr stTxBox;
	
	stTxBox.x = x1;
	stTxBox.y = y1;
	stTxBox.x1= x2;
	stTxBox.y1= y2;
	stTxBox.line_style = line_style;
	
	if((color == 0) && (line_style == 0))
	{
		color = 0x0F;
	}

	if((color & 0x01) != 0u)
	{
		stTxBox.vram_page = 0;
		_iocs_txbox(&stTxBox);
	}
	if((color & 0x02) != 0u)
	{
		stTxBox.vram_page = 1;
		_iocs_txbox(&stTxBox);
	}
	if((color & 0x04) != 0u)
	{
		stTxBox.vram_page = 2;
		_iocs_txbox(&stTxBox);
	}
	if((color & 0x08) != 0u)
	{
		stTxBox.vram_page = 3;
		_iocs_txbox(&stTxBox);
	}
}

int T_xLine(short x1, short y1, short w, unsigned short line_style, unsigned char color)
{
	struct _xlineptr stTxLine;
	
	stTxLine.x = x1;
	stTxLine.y = y1;
	stTxLine.x1= w;
	stTxLine.line_style = line_style;
	
	if((color == 0) && (line_style == 0))
	{
		color = 0x0F;
	}

	if((color & 0x01) != 0u)
	{
		stTxLine.vram_page = 0;
		_iocs_txxline(&stTxLine);
	}
	if((color & 0x02) != 0u)
	{
		stTxLine.vram_page = 1;
		_iocs_txxline(&stTxLine);
	}
	if((color & 0x04) != 0u)
	{
		stTxLine.vram_page = 2;
		_iocs_txxline(&stTxLine);
	}
	if((color & 0x08) != 0u)
	{
		stTxLine.vram_page = 3;
		_iocs_txxline(&stTxLine);
	}
}

int T_yLine(short x1, short y1, short h, unsigned short line_style, unsigned char color)
{
	struct _ylineptr stTyLine;
	
	stTyLine.x = x1;
	stTyLine.y = y1;
	stTyLine.y1= h;
	stTyLine.line_style = line_style;
	
	if((color == 0) && (line_style == 0))
	{
		color = 0x0F;
	}

	if((color & 0x01) != 0u)
	{
		stTyLine.vram_page = 0;
		_iocs_txyline(&stTyLine);
	}
	if((color & 0x02) != 0u)
	{
		stTyLine.vram_page = 1;
		_iocs_txyline(&stTyLine);
	}
	if((color & 0x04) != 0u)
	{
		stTyLine.vram_page = 2;
		_iocs_txyline(&stTyLine);
	}
	if((color & 0x08) != 0u)
	{
		stTyLine.vram_page = 3;
		_iocs_txyline(&stTyLine);
	}
}

void View_Line(unsigned short mode, unsigned short color)
{
	int w, h, sq;
	int x1, y1, x2, y2;
	int txtFlag = 0;
	int HiColorFlag = 0;
	
	_iocs_scroll(0, 0, 0);
	_iocs_scroll(1, 0, 0);
	_iocs_scroll(2, 0, 0);
	_iocs_scroll(3, 0, 0);
	Set_Vpage(0x0F);
	Set_Apage(0x00);
	
	_iocs_scroll(8, 0, 0);	/* テキスト座標 */
	T_PALET();
	T_Fill( 0, 0, 1023, 1023, 0, 0);	/* テキストクリア */
	
	switch(mode)
	{
	case 0:	/* 512x512 */
	case 4:	/* 512x512 */
	case 8:	/* 512x512 */
		{
			w = 512;
			h = 512;
			sq = h/8;

			G_Palette();
		}
		break;
	case 1:	/* 512x512(480) */
	case 5:	/* 512x512(480) */
	case 9:	/* 512x512(480) */
		{
			w = 512;
			h = 480;
			sq = h/8;

			G_Palette();
		}
		break;
	case 12:	/* 512x512 */
		{
			w = 512;
			h = 512;
			sq = h/8;
			
			G_Palette16();
			HiColorFlag = 1;
		}
		break;
	case 13:	/* 512x512(480) */
		{
			w = 512;
			h = 480;
			sq = h/8;
			
			G_Palette16();
			HiColorFlag = 1;
		}
		break;
	case  2:	/* 256x256 */
	case  6:	/* 256x256 */
	case 10:	/* 256x256 */
		{
			w = 256;
			h = 256;
			sq = h/8;

			G_Palette();
		}
		break;
	case  3:	/* 256x256(240) */
	case  7:	/* 256x256(240) */
	case 11:	/* 256x256(240) */
		{
			w = 256;
			h = 240;
			sq = h/8;

			G_Palette();
		}
		break;
	case 14:	/* 256x256 */
		{
			w = 256;
			h = 256;
			sq = h/8;

			G_Palette16();
			HiColorFlag = 1;
		}
		break;
	case 15:	/* 256x256(240) */
		{
			w = 256;
			h = 240;
			sq = h/8;

			G_Palette16();
			HiColorFlag = 1;
		}
		break;
	case 16:	/* 768x512 */
	default:
		{
			w = 768;
			h = 512;
			sq = h/8;

			G_Palette();
		}
		break;
	case 20:	/* 768x512 */
		{
			w = 768;
			h = 512;
			sq = h/8;
			
			G_Palette();
			txtFlag = 1;
		}
		break;
	case 24:	/* 768x512 */
		{
			w = 768;
			h = 512;
			sq = h/8;

			G_Palette();
//			G_Palette16();
			txtFlag = 1;
//			HiColorFlag = 1;
		}
		break;
	case 17:	/* 1024x424 */
		{
			w = 1024;
			h = 424;
			sq = h/8;

			G_Palette();
		}
		break;
	case 21:	/* 1024x424 */
		{
			w = 1024;
			h = 424;
			sq = h/8;

			G_Palette();
			txtFlag = 1;
		}
		break;
	case 25:	/* 1024x424 */
		{
			w = 1024;
			h = 424;
			sq = h/8;

			G_Palette();
//			G_Palette16();
			txtFlag = 1;
//			HiColorFlag = 1;
		}
		break;
	case 18:	/* 1024x848 */
		{
			w = 1024;
			h = 848;
			sq = h/8;

			G_Palette();
		}
		break;
	case 22:	/* 1024x848 */
		{
			w = 1024;
			h = 848;
			sq = h/8;

			G_Palette();
			txtFlag = 1;
		}
		break;
	case 26:	/* 1024x848 */
		{
			w = 1024;
			h = 848;
			sq = h/8;

			G_Palette();
//			G_Palette16();
			txtFlag = 1;
//			HiColorFlag = 1;
		}
		break;
	case 19:	/* 640x480 */
		{
			w = 640;
			h = 480;
			sq = h/8;

			G_Palette();
		}
		break;
	case 23:	/* 640x480 */
		{
			w = 640;
			h = 480;
			sq = h/8;

			G_Palette();
			txtFlag = 1;
		}
		break;
	case 27:	/* 640x480 */
		{
			w = 640;
			h = 480;
			sq = h/8;

			G_Palette();
//			G_Palette16();
			txtFlag = 1;
//			HiColorFlag = 1;
		}
		break;
#if 0
	case CRT_MODE_28_Ex:
		{
			w = 680;
			h = 424;
			sq = h/8;

			G_Palette();
			txtFlag = 1;
		}
		break;
	case CRT_MODE_29_Ex:
		{
			w = 384;
			h = 256;
			sq = h/8;

			G_Palette();
//			G_Palette16();
			txtFlag = 1;
//			HiColorFlag = 1;
		}
		break;
#endif
	}
	_iocs_window( 0, 0, w-1, h-1);
	_iocs_wipe();

	if(HiColorFlag == 1)
	{
		Draw_Line(     0,      0,    w-1,    h-1, 0x0303u, 0xFFFF);	/* 対角線右下がり（クロス） */
		Draw_Line(   w-1,      0,      0,    h-1, 0x0303u, 0xFFFF);	/* 対角線左下がり（クロス） */
		Draw_Box(      0,      0,    w-1,    h-1, 0x0303u, 0xFFFF);	/* 四角全体 */
		Draw_Box(   ((w-h)/2)+sq-1,   sq-1, w-((w-h)/2)-sq-1, h-sq-1, 0x0303u, 0xFFFF);	/* 四角正方形 */
		Draw_Fill(     0,      0,   sq-1,   sq-1, 0x0707u);	/* 左上 R */
		Draw_Fill(w-sq-1,      0,    w-1,   sq-1, 0x0B0Bu);	/* 右上 G */
		Draw_Fill(     0, h-sq-1,   sq-1,    h-1, 0x0F0Fu);	/* 左下 B */
		Draw_Fill(w-sq-1, h-sq-1,    w-1,    h-1, 0x0303u);	/* 右下 W */
		Draw_Circle(w/2,h/2,h/2,0x0303u,0,360,256);		/* 正円 */
	}
	else
	{
		Draw_Line(     0,      0,    w-1,    h-1, color, 0xFFFF);	/* 対角線右下がり（クロス） */
		Draw_Line(   w-1,      0,      0,    h-1, color, 0xFFFF);	/* 対角線左下がり（クロス） */
		Draw_Box(      0,      0,    w-1,    h-1, color, 0xFFFF);	/* 四角全体 */
		Draw_Box(   ((w-h)/2)+sq-1,   sq-1, w-((w-h)/2)-sq-1, h-sq-1, color, 0xFFFF);	/* 四角正方形 */
		Draw_Fill(     0,      0,   sq-1,   sq-1,  7);	/* 左上 R */
		Draw_Fill(w-sq-1,      0,    w-1,   sq-1, 11);	/* 右上 G */
		Draw_Fill(     0, h-sq-1,   sq-1,    h-1, 15);	/* 左下 B */
		Draw_Fill(w-sq-1, h-sq-1,    w-1,    h-1,  3);	/* 右下 W */
		Draw_Circle(w/2,h/2,h/2,color,0,360,256);		/* 正円 */
	}
	
	if(txtFlag == 1)
	{
		T_Box(      0,      0,    w-1,    h-1, 0xFFFF, color);	/* 四角全体 */
		T_Box( ((w-h)/2)+sq-1,   sq-1, (w-(((w-h)/2)+sq)*2), h-sq-sq, 0xFFFF, color);	/* 四角正方形 */
//		T_Fill(     0,      0,   sq,   sq, 0xFFFF,  7);	/* 左上 R */
		T_Fill(w-sq-1,      0,   sq,   sq, 0xFFFF, 11);	/* 右上 G */
//		T_Fill(     0, h-sq-1,   sq,   sq, 0xFFFF, 15);	/* 左下 B */
		T_Fill(w-sq-1, h-sq-1,   sq,   sq, 0xFFFF,  3);	/* 右下 W */
	}
}

void G_Palette(void)
{
	GPALET( 0, SetRGB( 0,  0,  0));	/* Black */
	GPALET( 1, SetRGB( 7,  7,  7));	/* Glay */
	GPALET( 2, SetRGB(23, 23, 23));	/* D-White */
	GPALET( 3, SetRGB(31, 31, 31));	/* White */
	GPALET( 4, SetRGB(15,  0, 15));	/* D-Perple */
	GPALET( 5, SetRGB(31,  0, 31));	/* Perple */
	GPALET( 6, SetRGB(15,  0,  0));	/* D-Red */
	GPALET( 7, SetRGB(31,  0,  0));	/* Red */
	GPALET( 8, SetRGB(15, 15,  0));	/* D-Yellow */
	GPALET( 9, SetRGB(31, 31,  0));	/* Yellow */
	GPALET(10, SetRGB( 0, 15,  0));	/* D-Green */
	GPALET(11, SetRGB( 0, 31,  0));	/* Green */
	GPALET(12, SetRGB( 0, 15, 15));	/* D-Water */
	GPALET(13, SetRGB( 0, 31, 31));	/* Water */
	GPALET(14, SetRGB( 0,  0, 15));	/* D-Blue */
	GPALET(15, SetRGB( 0,  0, 31));	/* Blue */
}

void G_Palette16(void)
{
	int i;
	volatile unsigned char  *GR_PALET = (unsigned char *)0xE82000u;

	for(i=1; i<16; i+=2)
	{
		*GR_PALET = (unsigned char)(tPallet[i-1] & 0x00FFu);
		GR_PALET++;
		*GR_PALET = (unsigned char)(tPallet[i] & 0x00FFu);
		GR_PALET++;
		*GR_PALET = (unsigned char)((tPallet[i-1] & 0xFF00u) >> 8);
		GR_PALET++;
		*GR_PALET = (unsigned char)((tPallet[i] & 0xFF00u) >> 8);
		GR_PALET++;
	}
}

void T_PALET(void)
{
	/* テキストパレットの初期化(Pal0はSPと共通) */
	TPALET2( 0, SetRGB( 0,  0,  0));	/* Black */
	TPALET2( 1, SetRGB( 7,  7,  7));	/* Glay */
	TPALET2( 2, SetRGB(23, 23, 23));	/* D-White */
	TPALET2( 3, SetRGB(31, 31, 31));	/* White */
	TPALET2( 4, SetRGB(15,  0, 15));	/* D-Perple */
	TPALET2( 5, SetRGB(31,  0, 31));	/* Perple */
	TPALET2( 6, SetRGB(15,  0,  0));	/* D-Red */
	TPALET2( 7, SetRGB(31,  0,  0));	/* Red */
	TPALET2( 8, SetRGB(15, 15,  0));	/* D-Yellow */
	TPALET2( 9, SetRGB(31, 31,  0));	/* Yellow */
	TPALET2(10, SetRGB( 0, 15,  0));	/* D-Green */
	TPALET2(11, SetRGB( 0, 31,  0));	/* Green */
	TPALET2(12, SetRGB( 0, 15, 15));	/* D-Water */
	TPALET2(13, SetRGB( 0, 31, 31));	/* Water */
	TPALET2(14, SetRGB( 0,  0, 15));	/* D-Blue */
	TPALET2(15, SetRGB( 0,  0, 31));	/* Blue */
}


int	Get_ROM_Ver(void)
{
	int dat, ver, year, month, day;
	
	dat = _iocs_romver();
	
	ver		= (dat & 0xFF000000) >> 24;
	year	= (dat & 0x00FF0000) >> 16;
	month	= (dat & 0x0000FF00) >> 8;
	day		= (dat & 0x000000FF);
	
	printf("ROM ver%x.%x(19%02x/%02x/%02x)\n", ((ver&0xF0)>>4), (ver&0x0F),year, month, day);
	
	return ver;
}

int main(int argc, char *argv[])
{
	int superchk;
	int Clip15;
	int Clip24;
	int Clip31;
	int Clip;
	unsigned int loop = 1;
	unsigned int counter = 0;

	int crtmod_old;
	int crtmod=16;
	unsigned short color=3;

	static short KeyEdge = 0;
	
	/* スーパーバイザーモード開始 */
	/*ＤＯＳのスーパーバイザーモード開始*/
	superchk = SUPER(0);

	if( superchk < 0 )
	{
		puts("すでに入ってる。");
	}
	else
	{
		puts("入った。");
	}
	g_ROMVer = Get_ROM_Ver();
	
	/*画面の初期設定*/
	crtmod_old = Set_CRT_Mode(-1, FALSE);
	Set_CRT_Mode(crtmod, FALSE);
	View_Line(crtmod, color);
	
	switch(g_ROMVer)
	{
	case 0x11:
	default:
		{
			Clip15	= 8;
			Clip24	= 2;
			Clip31	= 8;
			Clip 	= 19;
		}
		break;
	case 0x12:
		{
			Clip15	= 8;
			Clip24	= 3;
			Clip31	= 8;
			Clip 	= 20;
		}
		break;
	case 0x13:
		{
			Clip15	= 8;
			Clip24	= 9;
			Clip31	= 10;
			Clip 	= CRT_MODE_MAX;
		}
		break;
	}
	
	do {
		unsigned int bitsns;
		
		bitsns = BITSNS( 0 );
		
		if( (bitsns & 0x02 ) != 0 )	/* ＥＳＣ */
		{
			//printf("bitsns = %d\n", bitsns);
			loop =0;	/* 終了 */
		}
		else if((bitsns & 0x04) != 0 )	/* 1 */
		{
			if(KeyEdge == 0){
				KeyEdge = 1;
				
				crtmod++;
				if(crtmod >= Clip15)crtmod = 0;

				Set_CRT_Mode(tCRT_15kHz[crtmod], FALSE);
				View_Line(tCRT_15kHz[crtmod], color);
			}
		}
		else if((bitsns & 0x08) != 0 )	/* 2 */
		{
			if(KeyEdge == 0){
				KeyEdge = 1;
				
				crtmod++;
				if(crtmod >= Clip24)crtmod = 0;

				Set_CRT_Mode(tCRT_24kHz[crtmod], FALSE);
				View_Line(tCRT_24kHz[crtmod], color);
			}
		}
		else if((bitsns & 0x10) != 0 )	/* 3 */
		{
			if(KeyEdge == 0){
				KeyEdge = 1;
				
				crtmod++;
				if(crtmod >= Clip31)crtmod = 0;

				Set_CRT_Mode(tCRT_31kHz[crtmod], FALSE);
				View_Line(tCRT_31kHz[crtmod], color);
			}
		}
		else if( ( BITSNS( 6 ) & (1<<5) ) != 0 )	/* SPC */
		{
			if(KeyEdge == 0){
				KeyEdge = 1;
				
				crtmod++;
				if(crtmod >= Clip)crtmod = 0;

				Set_CRT_Mode(crtmod, FALSE);
				View_Line(crtmod, color);
			}
		}
		else if( ( BITSNS( 1 ) & (1<<7) ) != 0 )	/* BS */
		{
			if(KeyEdge == 0){
				KeyEdge = 1;
				
				crtmod--;
				if(crtmod < 0)crtmod = Clip - 1;

				Set_CRT_Mode(crtmod, FALSE);
				View_Line(crtmod, color);
			}
		}
		else
		{
			KeyEdge = 0;
		}
		
		Disp_Mode(crtmod);
		if(crtmod == 16)
		{
			Disp_Counter(counter++, 16);
		}
		else if(crtmod < 16)
		{
			Disp_Counter(counter++, 8);
		}
		else
		{
			Disp_Counter(counter++, 12);
		}
		
		/*カーソルを消します。*/
		B_CUROFF();
		
		_dos_kflushio(0xFF);	/* キーバッファをクリア */
	}
	while(loop);

	/* 元の画面に戻る */
	Set_CRT_Mode(crtmod_old, FALSE);

	_iocs_scroll(8, 0, 0);	/* テキスト座標 */
	T_Fill( 0, 0, 1023, 1023, 0, 0);	/* テキストクリア */
	
	_dos_kflushio(0xFF);	/* キーバッファをクリア */

	/*スーパーバイザーモード終了*/
	SUPER(superchk);
	
	return 0;
}
