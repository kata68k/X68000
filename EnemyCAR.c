#ifndef	ENEMYCAR_C
#define	ENEMYCAR_C

#include "inc/usr_macro.h"
#include "EnemyCAR.h"

#include "Draw.h"
#include "Graphic.h"
#include "MFP.h"
#include "MyCar.h"

/* 構造体定義 */
ST_ENEMYCARDATA	stEnemyCar[ENEMYCAR_MAX] = {0};
ST_ENEMYCARDATA	*pstEnemyCar[ENEMYCAR_MAX];

/* 関数のプロトタイプ宣言 */
SS	InitEnemyCAR(void);
SS	GetEnemyCAR(ST_ENEMYCARDATA *, SS);
SS	SetEnemyCAR(ST_ENEMYCARDATA, SS);
SS	EnemyCAR_main(UC, UC, UC);
SS	SetAlive_EnemyCAR(void);
SS	Put_EnemyCAR(US, US, US, UC);
SS	Sort_EnemyCAR(void);

/* 関数 */
SS	InitEnemyCAR(void)
{
	SS ret = 0;
	SS i;
	
	for(i=0; i<ENEMYCAR_MAX; i++)
	{
		stEnemyCar[i].ubCarType = 0;
		stEnemyCar[i].VehicleSpeed = 120;
		stEnemyCar[i].x = 0;
		stEnemyCar[i].y = 0;
		stEnemyCar[i].z = 4;
		stEnemyCar[i].ubBrakeLights = 0;
		stEnemyCar[i].ubOBD = 0;
		stEnemyCar[i].ubAlive = FALSE;
		
		pstEnemyCar[i] = &stEnemyCar[i];
	}
	
	return ret;
}

SS	GetEnemyCAR(ST_ENEMYCARDATA *stDat, SS Num)
{
	SS	ret = 0;
	if(Num < ENEMYCAR_MAX)
	{
		stDat = pstEnemyCar[Num];
	}
	else
	{
		ret = -1;
	}
	return ret;
}

SS	SetEnemyCAR(ST_ENEMYCARDATA stDat, SS Num)
{
	SS	ret = 0;
	
	if(Num < ENEMYCAR_MAX)
	{
		*pstEnemyCar[Num] = stDat;
	}
	else
	{
		ret = -1;
	}
	return ret;
}

SS	EnemyCAR_main(UC bNum, UC bMode, UC bMode_rev)
{
	SS	ret = 0;
	
	if(bNum < ENEMYCAR_MAX)
	{
		if(pstEnemyCar[bNum]->ubAlive == TRUE)
		{
			SS	x, y, z;
			US	ras_x, ras_y;
			ST_CARDATA	stMyCar;
			ST_CRT		stCRT;
			ST_RAS_INFO	stRasInfo;

			ras_x = 0;
			ras_y = 0;
			
			x = pstEnemyCar[bNum]->x;
			y = pstEnemyCar[bNum]->y;
			z = pstEnemyCar[bNum]->z;

			GetCRT(&stCRT, bMode);
			GetMyCar(&stMyCar);

			//y += ((stMyCar.VehicleSpeed - pstEnemyCar[bNum]->VehicleSpeed) / 10);
			y += RASTER_NEXT;

			GetRasterInfo(&stRasInfo);
			GetRasterPos(&ras_x, &ras_y, (US)(stRasInfo.st + y));
			x = ras_x;
			z = ((4 * 8) / (8 + y));
			
			pstEnemyCar[bNum]->x = x;
			pstEnemyCar[bNum]->y = y;
			pstEnemyCar[bNum]->z = z;
			
			if((stRasInfo.st + y) < (stRasInfo.ed - 32))
			{
				Put_EnemyCAR(	stCRT.hide_offset_x + (WIDTH>>1) - x,
								stCRT.hide_offset_y + stRasInfo.horizon + y,
								z,
								bMode_rev);
#ifdef DEBUG	/* デバッグコーナー */
				{
					Draw_Pset(	stCRT.hide_offset_x + (WIDTH>>1) - x,
								stCRT.hide_offset_y + stRasInfo.horizon + y,
								0xBE);	/* デバッグ用 */
				}
#endif
			}
			else
			{
				pstEnemyCar[bNum]->ubAlive = FALSE;
			}
		}
		else
		{
			/* nop */
		}
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

SS	SetAlive_EnemyCAR(void)
{
	SS	ret = 0;
	SS	i;

	for(i=0; i<ENEMYCAR_MAX; i++)
	{
		if(pstEnemyCar[i]->ubAlive == FALSE)
		{
			pstEnemyCar[i]->ubCarType = 0;
			pstEnemyCar[i]->VehicleSpeed = 120;
			pstEnemyCar[i]->x = 0;
			pstEnemyCar[i]->y = 0;
			pstEnemyCar[i]->z = 4;
			pstEnemyCar[i]->ubBrakeLights = 0;
			pstEnemyCar[i]->ubOBD = 0;
			pstEnemyCar[i]->ubAlive = TRUE;
			break;
		}
	}
	ret = i;
	
	return	ret;
}

SS	Put_EnemyCAR(US x, US y, US Size, UC ubMode)
{
	SS	ret = 0;
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
				ubMode, POS_MID, POS_BOTTOM);
	
	return	ret;
}

SS	Sort_EnemyCAR(void)
{
	SS	ret = 0;
	SS	i;
	SS	count = 0;
	ST_ENEMYCARDATA	*pstEnemyCar_Tmp;
	
	while(1)
	{
		for(i=0; i<ENEMYCAR_MAX-1; i++)
		{
			if(pstEnemyCar[i]->y > pstEnemyCar[i+1]->y)
			{
				pstEnemyCar_Tmp = pstEnemyCar[i+1];
				pstEnemyCar[i+1] = pstEnemyCar[i];
				pstEnemyCar[i] = pstEnemyCar_Tmp;
				count = 0;
			}
			else
			{
				count++;
			}
		}
		if(count >= (ENEMYCAR_MAX - 1))
		{
			break;
		}
	}
	
	return ret;
}

#endif	/* ENEMYCAR_C */

