#ifndef	ENEMYCAR_C
#define	ENEMYCAR_C

#include "inc/usr_macro.h"
#include "EnemyCAR.h"

SS	Put_EnemyCAR(US, US, US, UC);
SS	Put_CouseObject(US , US , US , UC , UC );

SS	Put_EnemyCAR(US x, US y, US Size, UC ubMode)
{
	SS	ret;
	SS	i;
	US	w, h;
	US	height_sum = 0u;
	
	w = ENEMY_CAR_1_W >> Size;
	h = ENEMY_CAR_1_H >> Size;

	for(i = 1; i <= Size; i++)
	{
		height_sum += (ENEMY_CAR_1_H >> (i-1));
	}

	G_BitBlt(	x,	w,					y,	h,	1,
				0,	0+w,	height_sum,	h,	1,
				ubMode);
	
	return	ret;
}

SS	Put_CouseObject(US x, US y, US Size, UC ubMode, UC ubPos)
{
	SS	ret;
	SS	i;
	SS	dx, dy;
	US	w, h;
	US	height_sum = 0u;
	
	w = PINETREE_1_W >> Size;
	h = PINETREE_1_H >> Size;

	dy = y - (h >> 1);
	if(ubPos == 0)
	{
		dx = x + w;
	}
	else
	{
		dx = x - w;
	}
	
	for(i = 1; i <= Size; i++)
	{
		height_sum += (PINETREE_1_H >> (i-1));
	}
	
	G_BitBlt(	dx,		w,			dy,	h,	1,
				140,	w,	height_sum,	h,	1,
				ubMode);
	
	return	ret;
}

#endif	/* ENEMYCAR_C */

