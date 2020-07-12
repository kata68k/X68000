#ifndef	GRAPHIC_H
#define	GRAPHIC_H

#include "inc/usr_define.h"

#define	CRT_MAX		(3)

enum
{
	POS_TOP,
	POS_CENTER,
	POS_BOTTOM,
	POS_LEFT,
	POS_MID,
	POS_RIGHT,
};

enum
{
	Clear_G=0,
	Enemy1_G,
	Enemy2_G,
	Enemy3_G,
	Enemy4_G,
	Object1_G,
	Object2_G,
	Object3_G,
	Object4_G,
	Object5_G,
	Object6_G,
	Flip_G
};

extern	SS	GetCRTCAR(ST_CRT *, SS);
extern	SS	SetCRT(ST_CRT, SS);
extern	void	G_INIT(void);
extern	void	G_MyCar(void);
extern	void	G_Background(void);
extern	void	G_Palette(void);
extern	SS	G_Stretch_Pict( SS , US , SS , US , UC , SS , US, SS, US, UC );
extern	SS	G_BitBlt(SS , US , SS , US , UC , SS , US , SS , US , UC , UC , UC , UC );
extern	SS	G_CLR_AREA(SS, US, SS, US, UC);
extern	SS	G_CLR_ALL_OFFSC(UC);

#endif	/* GRAPHIC_H */
