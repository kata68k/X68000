#ifndef	IF_DRAW_C
#define	IF_DRAW_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <usr_define.h>
#include <usr_macro.h>

#include "BIOS_CRTC.h"
#include "IF_Draw.h"
#include "IF_Graphic.h"
#include "IF_Memory.h"

int32_t Draw_Pset(int16_t , int16_t, uint16_t);
int32_t Draw_Pget(int16_t, int16_t, uint16_t *);
int32_t Draw_Line(int16_t, int16_t, int16_t, int16_t, uint16_t, uint16_t );
int32_t Draw_Box(int16_t, int16_t, int16_t, int16_t, uint16_t, uint16_t );
int32_t Draw_Fill(int16_t, int16_t, int16_t, int16_t, uint16_t);
int32_t Draw_Circle(int16_t, int16_t, uint16_t , uint16_t, int16_t , int16_t, uint16_t );
int32_t Draw_FillCircle(int16_t, int16_t, uint16_t , uint16_t, int16_t , int16_t, uint16_t );
int16_t Draw_Message_To_Graphic(uint8_t *, uint16_t, uint16_t, int16_t, int16_t);

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t Draw_Pset(int16_t x, int16_t y, uint16_t color)
{
	struct _psetptr stPset;

	stPset.x = Mmax(Mmin(x, X_MAX_DRAW-1), 0);
	stPset.y = Mmax(Mmin(y, Y_MAX_DRAW-1), 0);
	stPset.color = color;
	
	return _iocs_pset(&stPset);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t Draw_Pget(int16_t x, int16_t y, uint16_t *color)
{
	struct _pointptr stPget;
	int32_t ret;
	
	stPget.x  = x;
	stPget.y  = y;
	
	ret = _iocs_point(&stPget);

	*color = stPget.color;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t Draw_Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, uint16_t style)
{
	struct _lineptr stLine;

	if((x1 == x2) && (y1 == y2))
	{
		return Draw_Pset(x1, y1, color);
	}
	
	stLine.x1 = Mmax(Mmin(x1, X_MAX_DRAW-1), 0);	/* 始点 */
	stLine.y1 = Mmax(Mmin(y1, Y_MAX_DRAW-1), 0);
	stLine.x2 = Mmax(Mmin(x2, X_MAX_DRAW-1), 0);	/* 終点 */
	stLine.y2 = Mmax(Mmin(y2, Y_MAX_DRAW-1), 0);
	stLine.color = color;
	stLine.linestyle = style;
	
	return _iocs_line(&stLine);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t Draw_Box(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, uint16_t style)
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t Draw_Fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t Draw_Circle(int16_t x, int16_t y, uint16_t rad, uint16_t color, int16_t st, int16_t ed, uint16_t rat)
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
int32_t Draw_FillCircle(int16_t x, int16_t y, uint16_t rad, uint16_t color, int16_t st, int16_t ed, uint16_t rat)
{
	struct _circleptr stCircle;
	int16_t *pBuf;
	size_t Size;

	stCircle.x = x;
	stCircle.y = y;
	stCircle.radius = rad;
	stCircle.color = color;
	stCircle.start = st;
	stCircle.end = ed;
	stCircle.ratio = rat;
	_iocs_circle(&stCircle);

	Size = (rad * 2) * (rad * 2) * sizeof(int16_t);
	pBuf = (int16_t*)MyMalloc(Size);
	if(pBuf != NULL)
	{
		struct _paintptr stPaint;

//		printf("pBuf(0x%p)=%d\n", pBuf, Size);
		
		stPaint.x = x;
		stPaint.y = y;
		stPaint.color = color;
		stPaint.buf_start = pBuf;
		stPaint.buf_end = pBuf + Size;
//		printf("area(0x%p)(0x%p)\n", stPaint.buf_start, stPaint.buf_end);

		_iocs_paint(&stPaint);

		MyMfree(pBuf);
	}
	
	return 0;
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
