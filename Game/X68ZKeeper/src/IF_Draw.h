#ifndef	IF_DRAW_H
#define	IF_DRAW_H

#include <usr_define.h>

extern int32_t Draw_Pset(int16_t , int16_t, uint16_t);
extern int32_t Draw_Pget(int16_t, int16_t, uint16_t *);
extern int32_t Draw_Line(int16_t, int16_t, int16_t, int16_t, uint16_t, uint16_t );
extern int32_t Draw_Box(int16_t, int16_t, int16_t, int16_t, uint16_t, uint16_t );
extern int32_t Draw_Fill(int16_t, int16_t, int16_t, int16_t, uint16_t);
extern int32_t Draw_Circle(int16_t, int16_t, uint16_t , uint16_t, int16_t , int16_t, uint16_t );
extern int32_t Draw_FillCircle(int16_t, int16_t, uint16_t , uint16_t, int16_t , int16_t, uint16_t );
extern int16_t Draw_Message_To_Graphic(uint8_t *, uint16_t, uint16_t, int16_t, int16_t);

#endif	/* IF_DRAW_H */
