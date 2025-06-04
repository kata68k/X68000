#ifndef	IF_TEXT_C
#define	IF_TEXT_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include "usr_macro.h"
#include "BIOS_CRTC.h"
#include "BIOS_PCG.h"
#include "IF_Draw.h"
#include "IF_Text.h"
#include "IF_Graphic.h"
#include "IF_Math.h"

/* define定義 */
#define	TAB_SIZE	(4)
#define	GP_VDISP	(0x10)

/* グローバル変数 */
int32_t g_TpalDef[16];

/* 関数のプロトタイプ宣言 */
void T_INIT(void);
void T_EXIT(void);
void T_Clear(void);
void T_PALET(void);
uint16_t T_Main(uint16_t *);
int16_t T_Scroll(uint32_t, uint32_t);
int32_t T_Box(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
int32_t T_Fill(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
int32_t T_xLine(int16_t, int16_t, int16_t w, uint16_t, uint8_t);
int32_t T_yLine(int16_t, int16_t, int16_t h, uint16_t, uint8_t);
int32_t _iocs_txline(struct _xylineptr *);
int32_t T_Line(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
int32_t T_Line2(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
int16_t T_Circle(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
void T_FillCircle(int16_t, int16_t, int16_t, uint8_t);
void Message_Num(void *, int16_t, int16_t, uint16_t, uint8_t, uint8_t *);
int16_t BG_TextPut(int8_t *, int16_t, int16_t);
int16_t BG_PutToText(int16_t, int16_t, int16_t, int16_t, uint8_t);
int16_t BG_TimeCounter(uint32_t, uint16_t, uint16_t);
int16_t BG_Number(uint32_t, uint16_t, uint16_t);
int16_t Text_To_Text(uint16_t, int16_t, int16_t, uint8_t, uint8_t *);
int16_t Text_To_Text2(uint64_t, int16_t, int16_t, uint8_t, uint8_t *);
int16_t Put_Message_To_Graphic(uint8_t *, uint8_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_INIT(void)
{
	uint16_t i;
	
	for(i=0; i < 16u; i++)
	{
		g_TpalDef[i] = _iocs_tpalet2( i, -1 );
//			printf("tpalet2[%d]=%d\n", i, g_TpalDef[i]);
//			KeyHitESC();	/* デバッグ用 */
	}
	_iocs_os_curof();			/* カーソルを消します */
	_iocs_b_curoff();			/* カーソルを消します */
	_iocs_skey_mod(0, 0, 0);	/* ソフトウェアキーボードを消します */
	_dos_c_fnkmod(3);			/* ファンクションキー行無効化 */
	_iocs_b_locate(0, 0);		/*  原点 */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_EXIT(void)
{
	uint16_t i;
	
	T_Clear();				/* テキストクリア */

	_iocs_os_curon();		/* カーソルを表示します */
	_iocs_b_curon();		/* カーソルを表示します */
	_dos_c_fnkmod(0);		/* ファンクションキー行の設定 */
	_dos_c_window(0,31);	/* スクロール範囲の設定 */
	_iocs_b_locate(0, 0);	/*  原点 */
	
	/* テキストパレットをデフォルトに戻す */
	for(i=0; i < 16u; i++)
	{
		_iocs_tpalet2( i, g_TpalDef[i]);	/* def */
//		printf("tpalet2[%d]=%d\n", i, g_TpalDef[i]);
//		KeyHitESC();	/* デバッグ用 */
	}
	_iocs_tpalet2( 0, 0 );	/* Black */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_Clear(void)
{
#if 0
	_iocs_b_clr_al();	/* ALL */
#else	
	struct _txfillptr stTxFill;

	stTxFill.vram_page = 0;
	stTxFill.x = 0;
	stTxFill.y = 0;
	stTxFill.x1= 1023;
	stTxFill.y1= 1023;
	stTxFill.fill_patn = 0;
	
	_iocs_scroll(8, 0, 0);	/* 8:テキスト画面 */
//	_iocs_txrascpy(256 / 4 * 256, 256 / 4, 0b1111);	/* テキスト画面クリア */
	_iocs_txfill(&stTxFill);
	stTxFill.vram_page = 1;
	_iocs_txfill(&stTxFill);
	stTxFill.vram_page = 2;
	_iocs_txfill(&stTxFill);
	stTxFill.vram_page = 3;
	_iocs_txfill(&stTxFill);
#endif
	_iocs_b_locate(0, 0);		/*  原点 */

	_iocs_b_curoff();			/* カーソルを消します */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void T_PALET(void)
{
	/* テキストパレットの初期化(Pal0はSPと共通) */
	_iocs_tpalet2( 0, SetRGB( 0,  0,  0));	/* Black */
	_iocs_tpalet2( 1, SetRGB( 1,  0,  0));	/* Black2 */
	_iocs_tpalet2( 2, SetRGB(31,  0,  0));	/* Red */
	_iocs_tpalet2( 3, SetRGB(31, 31, 31));	/* White */
	_iocs_tpalet2( 4, SetRGB(30, 26, 16));	/* Red2 */
	_iocs_tpalet2( 5, SetRGB(30,  8,  0));	/* Orenge */
	_iocs_tpalet2( 6, SetRGB(30, 30,  0));	/* Yellow */
	_iocs_tpalet2( 7, SetRGB( 0, 31,  0));	/* Green */
	_iocs_tpalet2( 8, SetRGB( 0,  0, 31));	/* Blue */
	/* 8～15グラフィックをテキスト化した画像で使用 */
	_iocs_tpalet2( 9, SetRGB(15,  0,  0));	/* Red */
	_iocs_tpalet2(10, SetRGB(15, 15, 15));	/* White */
	_iocs_tpalet2(11, SetRGB(15, 13, 8));	/* Red2 */
	_iocs_tpalet2(12, SetRGB(15,  4,  0));	/* Orenge */
	_iocs_tpalet2(13, SetRGB(15, 15,  0));	/* Yellow */
	_iocs_tpalet2(14, SetRGB( 0, 15,  0));	/* Green */
	_iocs_tpalet2(15, SetRGB( 0,  0, 15));	/* Blue */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t T_Scroll(uint32_t uPosX, uint32_t uPosY)
{
	int16_t	ret = 0;
	volatile uint16_t *CRTC_R10 = (uint16_t *)0xE80014u;	/* テキストX方向スクロール */
	volatile uint16_t *CRTC_R11 = (uint16_t *)0xE80016u;	/* テキストY方向スクロール */

	*CRTC_R10 = Mbset(*CRTC_R10, 0x03FF, uPosX);	/* CRTC R10 */
	*CRTC_R11 = Mbset(*CRTC_R11, 0x03FF, uPosY);	/* CRTC R11 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t T_Box(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t line_style, uint8_t color)
{
	int32_t	ret = 0;
	struct _tboxptr stTxBox;
	
	stTxBox.x = x1;
	stTxBox.y = y1;
	stTxBox.x1= x2;
	stTxBox.y1= y2;
	stTxBox.line_style = line_style;

	stTxBox.vram_page = 0;
	if((color & 0x01) == 0u)
	{
		stTxBox.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txbox(&stTxBox);

	stTxBox.line_style = line_style;
	stTxBox.vram_page = 1;
	if((color & 0x02) == 0u)
	{
		stTxBox.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txbox(&stTxBox);

	stTxBox.line_style = line_style;
	stTxBox.vram_page = 2;
	if((color & 0x04) == 0u)
	{
		stTxBox.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txbox(&stTxBox);

	stTxBox.line_style = line_style;
	stTxBox.vram_page = 3;
	if((color & 0x08) == 0u)
	{
		stTxBox.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txbox(&stTxBox);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t T_Fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t line_style, uint8_t color)
{
	int32_t	ret = 0;
	struct _txfillptr stTxFill;
	
	stTxFill.x = x1;
	stTxFill.y = y1;
	stTxFill.x1= x2;
	stTxFill.y1= y2;

	stTxFill.fill_patn = line_style;
	stTxFill.vram_page = 0;
	if((color & 0x01) == 0u)
	{
		stTxFill.fill_patn = line_style ^ 0xFFFF;
	}
	_iocs_txfill(&stTxFill);

	stTxFill.fill_patn = line_style;
	stTxFill.vram_page = 1;
	if((color & 0x02) == 0u)
	{
		stTxFill.fill_patn = line_style ^ 0xFFFF;
	}
	_iocs_txfill(&stTxFill);

	stTxFill.fill_patn = line_style;
	stTxFill.vram_page = 2;
	if((color & 0x04) == 0u)
	{
		stTxFill.fill_patn = line_style ^ 0xFFFF;
	}
	_iocs_txfill(&stTxFill);

	stTxFill.fill_patn = line_style;
	stTxFill.vram_page = 3;
	if((color & 0x08) == 0u)
	{
		stTxFill.fill_patn = line_style ^ 0xFFFF;
	}
	_iocs_txfill(&stTxFill);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t T_xLine(int16_t x1, int16_t y1, int16_t w, uint16_t line_style, uint8_t color)
{
	int32_t	ret = 0;
	struct _xlineptr stTxLine;
	
	stTxLine.x = x1;
	stTxLine.y = y1;
	stTxLine.x1= w;

	stTxLine.line_style = line_style;
	stTxLine.vram_page = 0;
	if((color & 0x01) == 0u)
	{
		stTxLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txxline(&stTxLine);

	stTxLine.line_style = line_style;
	stTxLine.vram_page = 1;
	if((color & 0x02) == 0u)
	{
		stTxLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txxline(&stTxLine);

	stTxLine.line_style = line_style;
	stTxLine.vram_page = 2;
	if((color & 0x04) == 0u)
	{
		stTxLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txxline(&stTxLine);

	stTxLine.line_style = line_style;
	stTxLine.vram_page = 3;
	if((color & 0x08) == 0u)
	{
		stTxLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txxline(&stTxLine);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t T_yLine(int16_t x1, int16_t y1, int16_t h, uint16_t line_style, uint8_t color)
{
	int32_t	ret = 0;
	struct _ylineptr stTyLine;
	
	stTyLine.x = x1;
	stTyLine.y = y1;
	stTyLine.y1= h;

	stTyLine.line_style = line_style;
	stTyLine.vram_page = 0;
	if((color & 0x01) == 0u)
	{
		stTyLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txyline(&stTyLine);

	stTyLine.line_style = line_style;
	stTyLine.vram_page = 1;
	if((color & 0x02) == 0u)
	{
		stTyLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txyline(&stTyLine);

	stTyLine.line_style = line_style;
	stTyLine.vram_page = 2;
	if((color & 0x04) == 0u)
	{
		stTyLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txyline(&stTyLine);

	stTyLine.line_style = line_style;
	stTyLine.vram_page = 3;
	if((color & 0x08) == 0u)
	{
		stTyLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txyline(&stTyLine);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t _iocs_txline(struct _xylineptr *pstline)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};
	int32_t	retReg;	/* d0 */
	
	stInReg.d0 = 0xD5;				/* IOCS TxLine */
	stInReg.a1 = (int32_t)pstline;	/* TxLine構造体の先頭アドレス */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	return retReg;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t T_Line(int16_t x1, int16_t y1, int16_t v, int16_t h, uint16_t line_style, uint8_t color)
{
	int32_t	ret = 0;
	struct _xylineptr stTLine;
	
	stTLine.x = x1;
	stTLine.y = y1;
	stTLine.x1= v;
	stTLine.y1= h;
	
	stTLine.line_style = line_style;
	stTLine.vram_page = 0;
	if((color & 0x01) == 0u)
	{
		stTLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txline(&stTLine);

	stTLine.line_style = line_style;
	stTLine.vram_page = 1;
	if((color & 0x02) == 0u)
	{
		stTLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txline(&stTLine);

	stTLine.line_style = line_style;
	stTLine.vram_page = 2;
	if((color & 0x04) == 0u)
	{
		stTLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txline(&stTLine);

	stTLine.line_style = line_style;
	stTLine.vram_page = 3;
	if((color & 0x08) == 0u)
	{
		stTLine.line_style = line_style ^ 0xFFFF;
	}
	_iocs_txline(&stTLine);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t T_Line2(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t line_style, uint8_t color)
{
	int32_t	ret = 0;
	int16_t v;
	int16_t h;
	
	v = x2 - x1;
	h = y2 - y1;
	
	ret = T_Line(x1, y1, v, h, line_style, color);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t T_Circle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t line_style, uint8_t color)
{
	int16_t	ret = 0;
	int16_t	i;
	int16_t	w, h;
	int16_t	cx, cy;
	int16_t	px, py;
	
	x2 += x1;
	y2 += y1;
	
	/* 縦線 */
	if(x1 == x2)
	{
		T_yLine(x1, y1, 1, line_style, color);
		return ret;
	}
	
	/* 横線 */
	if(y1 == y2)
	{
		T_xLine(x1, y1, 1, line_style, color);
		return ret;
	}
	
	w = Mdiv2(x2 - x1);
	h = Mdiv2(y2 - y1);
	cx = x1 + w;
	cy = y1 + h;
	
#ifdef DEBUG
//	printf("T_Circle=(%d,%d,%d,%d)(%d,%d)(%d,%d)\n",  x1, y1, x2, y2, cx, cy, w, h);
//	KeyHitESC();	/* デバッグ用 */
#endif
	
	for(i=0; i<90; i++)
	{
		/* 1 */
		px = cx + Mdiv256( (w * APL_Cos(i) ) );
		py = cy + Mdiv256( (h * APL_Sin(i) ) );
		if(line_style == 0xFFFF)
		{
			T_Fill( cx, py, px - cx, cy - py, line_style, color);
		}
		else
		{
			T_xLine( px, py, 1, 0xFFFF, color);
		}
#ifdef DEBUG
//		printf("T_Circle(%d)=(%d,%d,%d,%d)\n", i, cx, py, px - cx, cy - py);
//		KeyHitESC();	/* デバッグ用 */
#endif

		/* 2 */
		px = cx + Mdiv256( (w * APL_Cos(i+90)) );
		py = cy + Mdiv256( (h * APL_Sin(i+90)) );

		if(line_style == 0xFFFF)
		{
			T_Fill( px, py, cx - px, cy - py, line_style, color);
		}
		else
		{
			T_xLine( px, py, 1, 0xFFFF, color);
		}
#ifdef DEBUG
//		printf("T_Circle(%d)=(%d,%d,%d,%d)\n", i, px, py, cx - px, cy - py);
//		KeyHitESC();	/* デバッグ用 */
#endif

		/* 3 */
		px = cx + Mdiv256( (w * APL_Cos(i+180)) );
		py = cy + Mdiv256( (h * APL_Sin(i+180)) );

		if(line_style == 0xFFFF)
		{
			T_Fill( px, cy, cx - px, py - cy, line_style, color);
		}
		else
		{
			T_xLine( px, py, 1, 0xFFFF, color);
		}
#ifdef DEBUG
//		printf("T_Circle(%d)=(%d,%d,%d,%d)\n", i, px, cy, cx - px, py - cy);
//		KeyHitESC();	/* デバッグ用 */
#endif

		/* 4 */
		px = cx + Mdiv256( (w * APL_Cos(i+270)) );
		py = cy + Mdiv256( (h * APL_Sin(i+270)) );

		if(line_style == 0xFFFF)
		{
			T_Fill( cx, cy, px - cx, py - cy, line_style, color);
		}
		else
		{
			T_xLine( px, py, 1, 0xFFFF, color);
		}
#ifdef DEBUG
//		printf("T_Circle(%d)=(%d,%d,%d,%d)\n", i, cx, cy, px - cx, py - cy);
//		KeyHitESC();	/* デバッグ用 */
#endif
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
void T_FillCircle(int16_t x0, int16_t y0, int16_t radius, uint8_t color)
{
    int16_t x = radius;
    int16_t y = 0;
    int16_t xChange = 1 - (radius << 1);
    int16_t yChange = 0;
    int16_t radiusError = 0;
	int16_t i;

    while (x >= y)
    {
        for (i = x0 - x; i <= x0 + x; i++)
        {
            T_Line(i, y0 + y, 1, 1, 0xFFFF, color);
            T_Line(i, y0 - y, 1, 1, 0xFFFF, color);
        }
        for (i = x0 - y; i <= x0 + y; i++)
        {
            T_Line(i, y0 + x, 1, 1, 0xFFFF, color);
            T_Line(i, y0 - x, 1, 1, 0xFFFF, color);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

/*===========================================================================================*/
/* 関数名	:	*/
/* 引数		:	*/
/* 戻り値	:	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		:	*/
/*===========================================================================================*/
int16_t	T_SetQuarterFont(int8_t *sString, int8_t *sDst)
{
	int16_t ret = 0;
	int32_t length;
	int32_t i;
	
	length = strlen(sString);
	
	for(i = 0; i < length; i++ )
	{
		*sDst = 0xF3;
		sDst++;
		*sDst = *sString;
		sDst++;
		sString++;
	}
	ret = length;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Message_Num(void *pNum, int16_t x, int16_t y, uint16_t nCol, uint8_t mode, uint8_t *sFormat)
{
	char str[256];
#if 0	
	volatile uint16_t *CRTC_480 = (uint16_t *)0xE80480u;	/* CRTC動作ポート */
#endif	
	size_t strLength;
	memset(str, 0, sizeof(str));

	switch(mode){
	case MONI_Type_UL:
		{
			uint64_t *num;
			num = (uint64_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SL:
		{
			int64_t *num;
			num = (int64_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_UI:
		{
			uint32_t *num;
			num = (uint32_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SI:
		{
			int32_t *num;
			num = (int32_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_US:
		{
			uint16_t *num;
			num = (uint16_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SS:
		{
			int16_t *num;
			num = (int16_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_UC:
		{
			uint8_t *num;
			num = (uint8_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SC:
		{
			int8_t *num;
			num = (int8_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_FL:
		{
			FL *num;
			num = (FL *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_PT:
		{
			sprintf(str, "0x%p", pNum);
		}
		break;
	default:
		{
			uint16_t *num;
			num = (uint16_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	}
#if 0	
	//BG_TextPut(str, x << 4, y  << 4);
	//Text_To_Text(atoi(str), x << 4, y << 4, FALSE);
	if((*CRTC_480 & 0x02u) == 0u)	/* クリア実行中でない */
	{
		strLength = strlen(str);
//		printf("%s %d\n", str, strLength);

		x = _iocs_b_putmes( nCol, x, y, strLength + 1, str);	/* 高速クリアの処理を阻害している */
		printf("%d\n", x);
		//B_LOCATE(x, y);
		//B_PRINT(str);
		//B_CUROFF();
	}
#else
	strLength = strlen(str);
	_iocs_b_putmes( nCol, x, y, strLength + 1, str);	/* 高速クリアの処理を阻害している */
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t BG_TextPut(int8_t *sString, int16_t x, int16_t y)
{
//	uint16_t *BG_HEAD		= (uint16_t *)0xEB8000;
	uint16_t *BG_TEXT_HEAD	= (uint16_t *)0xEB8800;
//	uint16_t *BG_NUM_HEAD	= (uint16_t *)0xEB8600;
	uint8_t *T0_HEAD = (uint8_t *)0xE00000;
	uint8_t *T1_HEAD = (uint8_t *)0xE20000;
	uint8_t *T2_HEAD = (uint8_t *)0xE40000;
	uint8_t *T3_HEAD = (uint8_t *)0xE60000;
	uint16_t *pStPAT;
	uint8_t *pDst0;
	uint8_t *pDst1;
	uint8_t *pDst2;
	uint8_t *pDst3;
	int16_t ret = 0;
#if 1
#else
	uint16_t	BitMask[4][4] = { 
			0x1000, 0x2000, 0x4000, 0x8000,
			0x0100, 0x0200, 0x0400, 0x0800,
			0x0010, 0x0020, 0x0040, 0x0080,
			0x0001, 0x0002, 0x0004, 0x0008
	};
#endif
	
	while(*sString != 0)
	{
		switch(*sString)
		{
			case 0x09:	/* HT(水平タブ) */
			{
				x += 32;		/* TAB(4)分動かす(8dot x 4word) */
				break;
			}
			case 0x20:	/* SP(空白) */
			case 0x21:	/* ! */
			case 0x22:	/* " */
			case 0x23:	/* # */
			case 0x24:	/* $ */
			case 0x25:	/* % */
			case 0x26:	/* & */
			case 0x27:	/* ' */
			case 0x28:	/* [ */
			case 0x29:	/* ] */
			case 0x2A:	/* * */
			case 0x2B:	/* + */
			case 0x2C:	/* , */
			case 0x2D:	/* -(マイナス) */
			case 0x2E:	/* . */
			case 0x2F:	/* / */
			case 0x30:	/* 0 */
			case 0x31:	/* 1 */
			case 0x32:	/* 2 */
			case 0x33:	/* 3 */
			case 0x34:	/* 4 */
			case 0x35:	/* 5 */
			case 0x36:	/* 6 */
			case 0x37:	/* 7 */
			case 0x38:	/* 8 */
			case 0x39:	/* 9 */
			{
				BG_PutToText(*sString, x, y, BG_Normal, TRUE);
				x += 8;		/* 一文字分動かす */
				break;
			}
			case 0x40:	/* @ */
			case 0x41:	/* A */
			case 0x42:	/* B */
			case 0x43:	/* C */
			case 0x44:	/* D */
			case 0x45:	/* E */
			case 0x46:	/* F */
			case 0x47:	/* G */
			case 0x48:	/* H */
			case 0x49:	/* I */
			case 0x4A:	/* J */
			case 0x4B:	/* K */
			case 0x4C:	/* L */
			case 0x4D:	/* M */
			case 0x4E:	/* N */
			case 0x4F:	/* O */
			case 0x50:	/* P */
			case 0x51:	/* Q */
			case 0x52:	/* R */
			case 0x53:	/* S */
			case 0x54:	/* T */
			case 0x55:	/* U */
			case 0x56:	/* V */
			case 0x57:	/* W */
			case 0x58:	/* X */
			case 0x59:	/* Y */
			case 0x5A:	/* X */
			{
				uint32_t i, j, k;
				
				j = (uint32_t)((int8_t)*sString - '@');
				pStPAT = BG_TEXT_HEAD + (uint16_t)(0x10 * j);

				k = Mmul128(y) + Mdiv8(x);	/* 8bit */
				if( (k < 0) || ((k+8) > 0x1FFFF) ) break;
				pDst0   = T0_HEAD + k;
				pDst1   = T1_HEAD + k;
				pDst2   = T2_HEAD + k;
				pDst3   = T3_HEAD + k;
				
				for(i=0; i < 8; i++)
				{
#if 1
					uint16_t nTmp[4];
					uint16_t nBuff[4];
					
					nTmp[0] = *pStPAT;	/* 16bit(0,1,2,3) -> 4dot分抽出 */
					pStPAT++;
					nTmp[1] = *pStPAT;	/* 16bit(4,5,6,7) -> 4dot分抽出 */
					pStPAT++;
					/* 8bit(PCG) to 1bit(Text-VRAM) */	/* がぶがぶさん、GIMONSさん、Shiroh Suzukiさんのアドバイス */
					nBuff[0] = (nTmp[0] & 0b0001000000000000) >> 9;
					nBuff[1] = (nTmp[0] & 0b0000000100000000) >> 6;
					nBuff[2] = (nTmp[0] & 0b0000000000010000) >> 3;
					nBuff[3] = (nTmp[0] & 0b0000000000000001);
					*pDst0 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0001000000000000) >> 9;
					nBuff[1] = (nTmp[1] & 0b0000000100000000) >> 6;
					nBuff[2] = (nTmp[1] & 0b0000000000010000) >> 3;
					nBuff[3] = (nTmp[1] & 0b0000000000000001);
					*pDst0 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);
					
					nBuff[0] = (nTmp[0] & 0b0010000000000000) >> 10;
					nBuff[1] = (nTmp[0] & 0b0000001000000000) >> 7;
					nBuff[2] = (nTmp[0] & 0b0000000000100000) >> 4;
					nBuff[3] = (nTmp[0] & 0b0000000000000010) >> 1;
					*pDst1 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0010000000000000) >> 10;
					nBuff[1] = (nTmp[1] & 0b0000001000000000) >> 7;
					nBuff[2] = (nTmp[1] & 0b0000000000100000) >> 4;
					nBuff[3] = (nTmp[1] & 0b0000000000000010) >> 1;
					*pDst1 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);

					nBuff[0] = (nTmp[0] & 0b0100000000000000) >> 11;
					nBuff[1] = (nTmp[0] & 0b0000010000000000) >> 8;
					nBuff[2] = (nTmp[0] & 0b0000000001000000) >> 5;
					nBuff[3] = (nTmp[0] & 0b0000000000000100) >> 2;
					*pDst2 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0100000000000000) >> 11;
					nBuff[1] = (nTmp[1] & 0b0000010000000000) >> 8;
					nBuff[2] = (nTmp[1] & 0b0000000001000000) >> 5;
					nBuff[3] = (nTmp[1] & 0b0000000000000100) >> 2;
					*pDst2 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);

					nBuff[0] = (nTmp[0] & 0b1000000000000000) >> 12;
					nBuff[1] = (nTmp[0] & 0b0000100000000000) >> 9;
					nBuff[2] = (nTmp[0] & 0b0000000010000000) >> 6;
					nBuff[3] = (nTmp[0] & 0b0000000000001000) >> 3;
					*pDst3 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b1000000000000000) >> 12;
					nBuff[1] = (nTmp[1] & 0b0000100000000000) >> 9;
					nBuff[2] = (nTmp[1] & 0b0000000010000000) >> 6;
					nBuff[3] = (nTmp[1] & 0b0000000000001000) >> 3;
					*pDst3 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);
#else
					uint16_t nTmp[2];

					/* 8bit(PCG) to 1bit(Text-VRAM) */
					for(j=0; j < 8; j++)
					{
						uint32_t	bitshift;

						bitshift = j;

						k = j % 4;
						if(j < 4)	/* 上位4ドット */
						{
							/* P0 */
							*pDst0 |= (((nTmp[0] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P1 */
							*pDst1 |= (((nTmp[0] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P2 */
							*pDst2 |= (((nTmp[0] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P3 */
							*pDst3 |= (((nTmp[0] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
						}
						else		/* 下位4ドット */
						{
							/* P0 */
							*pDst0 |= (((nTmp[1] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P1 */
							*pDst1 |= (((nTmp[1] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P2 */
							*pDst2 |= (((nTmp[1] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P3 */
							*pDst3 |= (((nTmp[1] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
						}
					}
#endif
					pDst0 += 0x80;	/* byte x 64 x 2 = 1024 dot */
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
				x += 8;		/* 一文字分動かす */
			}
			case 0x0a:	/* LF(改行) */
			case 0x0d:	/* CR(復帰) */
			default:	/* 表示しない */
			{
				break;
			}
		}
		sString++;
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
int16_t BG_PutToText(int16_t nPatNum, int16_t x, int16_t y, int16_t mode, uint8_t bClr)
{
	uint16_t *BG_HEAD = (uint16_t *)0xEB8000;
	uint8_t *T0_HEAD = (uint8_t *)0xE00000;
	uint8_t *T1_HEAD = (uint8_t *)0xE20000;
	uint8_t *T2_HEAD = (uint8_t *)0xE40000;
	uint8_t *T3_HEAD = (uint8_t *)0xE60000;
	uint16_t	*pStPAT;
	uint8_t	*pDst0;
	uint8_t	*pDst1;
	uint8_t	*pDst2;
	uint8_t	*pDst3;
	int16_t	ret = 0;
	uint16_t	i, j, k;
	uint16_t	BitMask[4][4] = { 
			0x1000, 0x2000, 0x4000, 0x8000,
			0x0100, 0x0200, 0x0400, 0x0800,
			0x0010, 0x0020, 0x0040, 0x0080,
			0x0001, 0x0002, 0x0004, 0x0008
	};
	
	switch(mode)
	{
	case BG_H_rev:
	case BG_VH_rev:
		pStPAT = BG_HEAD + (uint16_t)(0x10 * (nPatNum + 1));
		break;
	case BG_Normal:
	case BG_V_rev:
	default:
		pStPAT = BG_HEAD + (uint16_t)(0x10 * nPatNum);
		break;
	}

	k = (y * 0x80) + (x >> 3);
	pDst0   = T0_HEAD + k;
	pDst1   = T1_HEAD + k;
	pDst2   = T2_HEAD + k;
	pDst3   = T3_HEAD + k;
	
	for(i=0; i < 8; i++)
	{
		uint16_t nTmp[2];
		
		switch(mode)
		{
		case BG_H_rev:
		case BG_VH_rev:
			pStPAT--;
			nTmp[1] = *pStPAT;	/* 16bit(4,5,6,7) -> 4dot分抽出 */
			pStPAT--;
			nTmp[0] = *pStPAT;	/* 16bit(0,1,2,3) -> 4dot分抽出 */
			break;
		case BG_Normal:
		case BG_V_rev:
		default:
			nTmp[0] = *pStPAT;	/* 16bit(0,1,2,3) -> 4dot分抽出 */
			pStPAT++;
			nTmp[1] = *pStPAT;	/* 16bit(4,5,6,7) -> 4dot分抽出 */
			pStPAT++;
			break;
		}

		/* クリア */
		if(bClr != 0)
		{
			*pDst0 = 0;
			*pDst1 = 0;
			*pDst2 = 0;
			*pDst3 = 0;
		}
		/* 8bit(PCG) to 1bit(Text-VRAM) */
		for(j=0; j < 8; j++)
		{
			uint32_t	bitshift;

			switch(mode)
			{
			case BG_V_rev:
			case BG_VH_rev:
				bitshift = j;
				break;
			case BG_Normal:
			case BG_H_rev:
			default:
				bitshift = (7-j);
				break;
			}
			k = j % 4;
			if(j < 4)	/* 上位4ドット */
			{
				/* P0 */
				*pDst0 |= (((nTmp[0] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P1 */
				*pDst1 |= (((nTmp[0] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P2 */
				*pDst2 |= (((nTmp[0] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P3 */
				*pDst3 |= (((nTmp[0] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
			}
			else		/* 下位4ドット */
			{
				/* P0 */
				*pDst0 |= (((nTmp[1] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P1 */
				*pDst1 |= (((nTmp[1] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P2 */
				*pDst2 |= (((nTmp[1] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P3 */
				*pDst3 |= (((nTmp[1] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
			}
		}
		pDst0 += 0x80;
		pDst1 += 0x80;
		pDst2 += 0x80;
		pDst3 += 0x80;
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
int16_t BG_TimeCounter(uint32_t unTime, uint16_t x, uint16_t y)
{
	int16_t ret = 0;
	uint32_t un100=0, un10=0, un1=0;
	uint16_t u100_view[2], u10_view[2], u1_view[2];

	if(unTime >= 1000)unTime = 999;
#if 1
	un100 = ((unTime % 1000) / 100);
	un10 = ((unTime % 100) / 10);
	un1 = (unTime % 10);
#endif
	/* 表示 */
	u100_view[0] = 128 + (uint16_t)(un100 << 1);
	u100_view[1] = u100_view[0]+1;
	u10_view[0] = 128 + (uint16_t)(un10 << 1);
	u10_view[1] = u10_view[0] + 1;
	u1_view[0] = 128 + (uint16_t)(un1 << 1);
	u1_view[1] = u1_view[0] + 1;
	
	BG_PutToText( u100_view[0], x-(BG_WIDTH<<1),		y,			BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText( u100_view[1], x-(BG_WIDTH<<1),	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText(  u10_view[0], x-BG_WIDTH,				y,			BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(  u10_view[1], x-BG_WIDTH,			y+BG_HEIGHT,	BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(   u1_view[0], x,						y,			BG_Normal, TRUE);	/*   1桁目 */
	BG_PutToText(   u1_view[1], x,					y+BG_HEIGHT,	BG_Normal, TRUE);	/*   1桁目 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t BG_Number(uint32_t unNum, uint16_t x, uint16_t y)
{
	int16_t ret = 0;
	uint8_t ucDigit[10];
#if 0
	uint32_t unDigit, unDigitCal;
#endif	
	static uint8_t ucOverFlow = FALSE;
	
	if(unNum >= 10000000)
	{
		ucOverFlow = TRUE;
	}
	
	if(ucOverFlow == TRUE)
	{
		unNum = 9999999;
	}
#if 0
	unDigitCal = unNum;
	/* 桁数を計算 */
	while(unDigitCal!= 0u)
	{
		unDigitCal = unDigitCal / 10;
		unDigit++;
	}
#endif	
	sprintf(ucDigit, "%7d", unNum);
	
	BG_TextPut(ucDigit, x, y);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* TEXT-RAMに展開したデータから数字情報を作る */	/* 処理負荷改善@kunichikoさんのアドバイス */
int16_t Text_To_Text(uint16_t uNum, int16_t x, int16_t y, uint8_t bLarge, uint8_t *sFormat)
{
	uint8_t	*T0_HEAD = (uint8_t *)0xE00000;
	uint8_t	*T1_HEAD = (uint8_t *)0xE20000;
	uint8_t	*T2_HEAD = (uint8_t *)0xE40000;
	uint8_t	*T3_HEAD = (uint8_t *)0xE60000;
	uint8_t	*pSrc0;
	uint8_t	*pSrc1;
	uint8_t	*pSrc2;
	uint8_t	*pSrc3;
//	uint8_t	*T3_END = (uint8_t *)0xE7FFFF;
	uint8_t	*pDst0;
	uint8_t	*pDst1;
	uint8_t	*pDst2;
	uint8_t	*pDst3;
	uint8_t	*pData0;
	uint8_t	*pData1;
	uint8_t	*pData2;
	uint8_t	*pData3;
	uint8_t	data;
	uint8_t	ucDigit[10] = {0};
	uint8_t	*pString;
	uint32_t	i, k, size;
	int16_t sx, sy, sAddr;
	int16_t	ret = 0;
	
	if( (x < 0) || (y < 0) || (x >= 1024) || (y >= 1024) ) return -1;

	/* ソースの置き場所 */
	sx = 256;
	sy = 0;
	
	if(bLarge == TRUE)
	{
		sy += 8;	/* 8dot下 */
		sAddr = Mmul128(sy) + Mdiv8(sx);
		
		pSrc0 = T0_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc1 = T1_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc2 = T2_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc3 = T3_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		size = 16;	/* 大 */
		sprintf(ucDigit, sFormat, uNum);
	}
	else
	{
		sAddr = Mmul128(sy) + Mdiv8(sx);
		
		pSrc0 = T0_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc1 = T1_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc2 = T2_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc3 = T3_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		size = 8;	/* 小 */
		sprintf(ucDigit, sFormat, uNum);
	}
	pString = &ucDigit[0];
	
	while(*pString != 0)
	{
		switch(*pString)
		{
			case 0x20:	/* SP */
			{
				/* コピー先 */
				k = Mmul128(y) + Mdiv8(x);
				if( (k < 0) || ((k+size) > 0x1FFFF) ) break;
				pDst0 = T0_HEAD + k;
				pDst1 = T1_HEAD + k;
				pDst2 = T2_HEAD + k;
				pDst3 = T3_HEAD + k;
				
				for(i=0; i < size; i++)
				{
					/* 更新 */
					*pDst0 = 0;
					*pDst1 = 0;
					*pDst2 = 0;
					*pDst3 = 0;

					pData0 += 0x80;
					pData1 += 0x80;
					pData2 += 0x80;
					pData3 += 0x80;

					pDst0 += 0x80;
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
				x+=8;
				break;
			}
			case 0x30:	/* 0 */
			case 0x31:	/* 1 */
			case 0x32:	/* 2 */
			case 0x33:	/* 3 */
			case 0x34:	/* 4 */
			case 0x35:	/* 5 */
			case 0x36:	/* 6 */
			case 0x37:	/* 7 */
			case 0x38:	/* 8 */
			case 0x39:	/* 9 */
			{
				/* コピー元 */
				data = *pString - '0';
				pData0 = pSrc0 + data;
				pData1 = pSrc1 + data;
				pData2 = pSrc2 + data;
				pData3 = pSrc3 + data;
				
				/* コピー先 */
				k = Mmul128(y) + Mdiv8(x);
				if( (k < 0) || ((k+size) > 0x1FFFF) ) break;
				pDst0 = T0_HEAD + k;
				pDst1 = T1_HEAD + k;
				pDst2 = T2_HEAD + k;
				pDst3 = T3_HEAD + k;
				
				for(i=0; i < size; i++)
				{
					/* 更新 */
					*pDst0 = *pData0;
					*pDst1 = *pData1;
					*pDst2 = *pData2;
					*pDst3 = *pData3;

					pData0 += 0x80;
					pData1 += 0x80;
					pData2 += 0x80;
					pData3 += 0x80;

					pDst0 += 0x80;
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
				x+=8;
				break;
			}
			default:	/* 表示対象外 */
			{
				x+=8;
				break;
			}
		}
		pString++;
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
/* TEXT-RAMに展開したデータから数字情報を作る */
int16_t Text_To_Text2(uint64_t ulNum, int16_t x, int16_t y, uint8_t bLarge, uint8_t *sFormat)
{
	int16_t	ret = 0;
	int16_t sx, sy;
	uint32_t	i, size, MemSize;
	uint8_t	data;
	uint8_t	ucDigit[20] = {0};
	uint8_t	ucBuffer[136] = {0};
	uint8_t	*pString;

	struct _fntbuf	*p_stTxbuf;

	if( (x < 0) || (y < 0) || (x >= 1024) || (y >= 1024) ) return -1;
	
	/* ソースの置き場所 */
	sx = 0;
	sy = 256;
	
#ifdef DEBUG
//	printf("Text_To_Text2(%ld) 1\n", ulNum);
//	KeyHitESC();	/* デバッグ用 */
#endif
	if(bLarge == TRUE)
	{
		sy += 8;	/* 8dot下 */
		size = Mmul2(BG_HEIGHT);	/* 大 */
		
		sprintf(ucDigit, sFormat, ulNum);
	}
	else
	{
		size = BG_HEIGHT;	/* 小 */
		sprintf(ucDigit, sFormat, ulNum);
	}
	pString = &ucDigit[0];

#ifdef DEBUG
//	printf("Text_To_Text2(%ld) 2\n", ulNum);
//	KeyHitESC();	/* デバッグ用 */
#endif
	
	MemSize = BG_WIDTH * size;
	p_stTxbuf = (struct _fntbuf	*)&ucBuffer[0];
	
	if(p_stTxbuf == NULL)
	{
		return -1;
	}
	p_stTxbuf->xl = BG_WIDTH;
	p_stTxbuf->yl = size;
	
	while(*pString != 0)
	{
		switch(*pString)
		{
			case 0x20:	/* SP */
			{
				/* コピー先 */
				memset(&(p_stTxbuf->buffer[0]), 0, MemSize);
				for(i=0; i < 4; i++)
				{
					/* テキストプレーン */
					_iocs_tcolor(1<<i);
					_iocs_textput(x, y, p_stTxbuf);
				}
				x+=BG_WIDTH;
				break;
			}
			case 0x30:	/* 0 */
			case 0x31:	/* 1 */
			case 0x32:	/* 2 */
			case 0x33:	/* 3 */
			case 0x34:	/* 4 */
			case 0x35:	/* 5 */
			case 0x36:	/* 6 */
			case 0x37:	/* 7 */
			case 0x38:	/* 8 */
			case 0x39:	/* 9 */
			{
				/* 対象データ */
				data = *pString - '0';
				
				for(i=0; i < 4; i++)
				{
					/* テキストプレーン */
					_iocs_tcolor(1<<i);
					/* コピー元 */
					_iocs_textget(sx + (data * BG_WIDTH), sy, p_stTxbuf);
					/* コピー先 */
					_iocs_textput(x, y, p_stTxbuf);
				}

				x+=BG_WIDTH;
				break;
			}
			default:	/* 表示対象外 */
			{
				x+=BG_WIDTH;
				break;
			}
		}
		pString++;
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
int16_t Put_Message_To_Graphic(uint8_t *str, uint8_t bMode )
{
	int16_t	ret = 0;
	
	int16_t	x, y;
	ST_CRT	stCRT = {0};
	
	GetCRT(&stCRT, bMode);	/* 画面情報を取得 */
	
	/* 座標設定 */
	x = stCRT.hide_offset_x;
	y = stCRT.hide_offset_y + ROAD_ED_POINT;
	
	/* メッセージエリア クリア */
	_iocs_window( x, y, x + WIDTH, y + 12);	/* 描画可能枠再設定 */

	Draw_Fill( x, y, x + WIDTH, y + 12, 0x01);	/* Screen0 指定パレットで塗りつぶし */
	
	/* メッセージエリア 描画 */
	PutGraphic_To_Symbol12(str, x, y, 0x03);
	
	return ret;
}

#endif	/* IF_TEXT_C */

