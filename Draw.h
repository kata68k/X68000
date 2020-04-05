#ifndef	DRAW_H
#define	DRAW_H

extern int Draw_Pset(short x, short y, unsigned short color);
extern int Draw_Pget(short x, short y, unsigned short *color);
extern int Draw_Line(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style);
extern int Draw_Box(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style);
extern int Draw_Fill(short x1, short y1, short x2, short y2, unsigned short color);
extern int Draw_Circle(short x, short y, unsigned short rad, unsigned short color, short st, short ed, unsigned short rat);

#endif	/* DRAW_H */
