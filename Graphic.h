#ifndef	GRAPHIC_H
#define	GRAPHIC_H

#include "inc/usr_define.h"

extern void G_INIT(void);
extern void G_MyCar(void);
extern void G_Background(void);
extern void G_Palette(void);
extern SS G_Stretch_Pict(US, US, US, US, UC, US, US, US, US, UC);
extern SS G_BitBlt(US, US, US, US, UC, US, US, US, US, UC, UC);
#endif	/* GRAPHIC_H */
