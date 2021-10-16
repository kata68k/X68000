#ifndef	DRAW_C
#define	DRAW_C

#include <iocslib.h>
#include "inc/usr_define.h"
#include "inc/usr_macro.h"

#include "Draw.h"

int Draw_Pset(short x, short y, unsigned short color)
{
	struct _psetptr stPset;

	stPset.x = Mmin(x, X_MAX_DRAW-1);
	stPset.y = Mmin(y, Y_MAX_DRAW-1);
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

	stLine.x1 = Mmin(x1, X_MAX_DRAW-1);
	stLine.y1 = Mmin(y1, Y_MAX_DRAW-1);
	stLine.x2 = Mmin(x2, X_MAX_DRAW-1);
	stLine.y2 = Mmin(y2, Y_MAX_DRAW-1);
	stLine.color = color;
	stLine.linestyle = style;
	
	return _iocs_line(&stLine);
}

int Draw_Box(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style)
{
	struct _boxptr stBox;

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

	stFill.x1 = Mmin(x1, X_MAX_DRAW-1);
	stFill.y1 = Mmin(y1, Y_MAX_DRAW-1);
	stFill.x2 = Mmin(x2, X_MAX_DRAW-1);
	stFill.y2 = Mmin(y2, Y_MAX_DRAW-1);
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


#endif	/* DRAW_C */
