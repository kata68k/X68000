#ifndef	IF_DRAW_H
#define	IF_DRAW_H

#define F_MOJI		(0xB0)
#define F_MOJI_BAK	(0xF6)

extern int Draw_Pset(short x, short y, unsigned short color);
extern int Draw_Pget(short x, short y, unsigned short *color);
extern int Draw_Line(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style);
extern int Draw_Box(short x1, short y1, short x2, short y2, unsigned short color, unsigned short style);
extern int Draw_Fill(short x1, short y1, short x2, short y2, unsigned short color);
extern int Draw_Circle(short x, short y, unsigned short rad, unsigned short color, short st, short ed, unsigned short rat);
extern int Draw_FillCircle(short x, short y, unsigned short rad, unsigned short color, short st, short ed, unsigned short rat);
extern int16_t Draw_Message_To_Graphic(uint8_t *, uint16_t, uint16_t, int16_t, int16_t);

#endif	/* IF_DRAW_H */
