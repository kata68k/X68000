#ifndef	IF_DRAW_C
#define	IF_DRAW_C

#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <usr_define.h>
#include <usr_macro.h>

#include "BIOS_CRTC.h"
#include "IF_Draw.h"
#include "IF_Graphic.h"

int Draw_Pset(short x, short y, unsigned short color);
int Draw_Pget(short x, short y, unsigned short *color);
int Draw_Line(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style);
int Draw_Box(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style);
int Draw_Fill(short x1, short y1, short x2, short y2, unsigned short color);
int Draw_Circle(short x, short y, unsigned short rad, unsigned short color, short st, short ed, unsigned short rat);
int16_t Draw_Message_To_Graphic(uint8_t *, uint16_t, uint16_t, int16_t, int16_t);

int Draw_Pset(short x, short y, unsigned short color)
{
	struct _psetptr stPset;

	stPset.x = Mmax(Mmin(x, X_MAX_DRAW-1), 0);
	stPset.y = Mmax(Mmin(y, Y_MAX_DRAW-1), 0);
	stPset.color = color;
	
	return _iocs_pset(&stPset);
}

int Draw_Pget(short x, short y, unsigned short *color)
{
	struct _pointptr stPget;
	int ret;
	
	stPget.x  = x;
	stPget.y  = y;
	
	ret = _iocs_point(&stPget);

	*color = stPget.color;
	
	return ret;
}

int Draw_Line(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style)
{
	struct _lineptr stLine;

	if((x1 == x2) && (y1 == y2))
	{
		return Draw_Pset(x1, y1, color);
	}
	
	stLine.x1 = Mmax(Mmin(x1, X_MAX_DRAW-1), 0);
	stLine.y1 = Mmax(Mmin(y1, Y_MAX_DRAW-1), 0);
	stLine.x2 = Mmax(Mmin(x2, X_MAX_DRAW-1), 0);
	stLine.y2 = Mmax(Mmin(y2, Y_MAX_DRAW-1), 0);
	stLine.color = color;
	stLine.linestyle = style;
	
	return _iocs_line(&stLine);
}

int Draw_Box(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style)
{
	struct _boxptr stBox;

	if((x1 == x2) && (y1 == y2))
	{
		return Draw_Pset(x1, y1, color);
	}
	
	stBox.x1 = Mmin(x1, X_MAX_DRAW-1);
	stBox.y1 = Mmin(y1, Y_MAX_DRAW-1);
	stBox.x2 = Mmin(x2, X_MAX_DRAW-1);
	stBox.y2 = Mmin(y2, Y_MAX_DRAW-1);
	stBox.color = color;
	stBox.linestyle = style;
	
	return _iocs_box(&stBox);
}

int Draw_Fill(short x1, short y1, short x2, short y2, unsigned short color)
{
	struct _fillptr stFill;

	if((x1 == x2) && (y1 == y2))
	{
		return Draw_Pset(x1, y1, color);
	}
	
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Draw_Message_To_Graphic(uint8_t *str, uint16_t x, uint16_t y, int16_t col, int16_t bk_col)
{
	int16_t	ret = 0;
	int16_t	len = 0;

	len = strlen(str);
	if(len > 0)
	{
		ret = _iocs_apage(0);		/* グラフィックの書き込み(表) */
		/* 塗りつぶし */
		Draw_Fill(x, y, x + (len * 8), y + 16, 0);
		/* メッセージ描画(影) */
		PutGraphic_To_Symbol12(str, x+1, y+1, bk_col);
		/* メッセージ描画 */
		PutGraphic_To_Symbol12(str, x, y, col);
	}
	
	return ret;
}

#endif	/* IF_DRAW_C */
