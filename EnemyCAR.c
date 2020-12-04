#ifndef	ENEMYCAR_C
#define	ENEMYCAR_C

#include "inc/usr_macro.h"
#include "EnemyCAR.h"

#include "Draw.h"
#include "Graphic.h"
#include "MFP.h"
#include "MyCar.h"
#include "Raster.h"

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
		stEnemyCar[i].VehicleSpeed = 80;
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
			SS	dx, dy, dz;
			SS	my;
			US	ras_x, ras_y, ras_pat, ras_num;
			SS	Out_Of_Disp;
			ST_CARDATA	stMyCar;
			ST_CRT		stCRT;
			ST_RAS_INFO	stRasInfo;
			ST_ROAD_INFO	stRoadInfo;

			ras_x = 0;
			ras_y = 0;
			
			x = pstEnemyCar[bNum]->x;
			y = pstEnemyCar[bNum]->y;
			z = pstEnemyCar[bNum]->z;

			GetCRT(&stCRT, bMode);
			GetMyCar(&stMyCar);
			GetRasterInfo(&stRasInfo);
			GetRoadInfo(&stRoadInfo);

			/* ライバル車との距離 */
			if(stMyCar.VehicleSpeed >= pstEnemyCar[bNum]->VehicleSpeed)
			{
				y += (stMyCar.VehicleSpeed - pstEnemyCar[bNum]->VehicleSpeed);
			}
			else
			{
				y -= (pstEnemyCar[bNum]->VehicleSpeed - stMyCar.VehicleSpeed);
			}
			y = Mmax(y, 2);
			my = y >> 3;
			
			if( (my > 0) && ((ROAD_SIZE-RASTER_NEXT)-48 > my) )
			{
				ras_num = (US)(stRasInfo.st + my);	/* ラスター情報の配列番号を算出 */
				GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num);	/* 配列番号のラスター情報取得 */
				
				if(ras_x < 256)	/* 道の左側 */
				{
					x = (SS)ras_x;
				}
				else	/* 道の右側 */
				{
					x = (SS)ras_x - 512;
				}
				z =  3 - (((my<<8) / (my + (ROAD_SIZE-RASTER_NEXT)))>>5);

				dx = stCRT.hide_offset_x + (WIDTH>>1) - x;
				dy = stCRT.hide_offset_y + stRoadInfo.Horizon + (((ras_y - RASTER_MIN) + ras_num) - ROAD_ST_POINT);
				dz = Mmin( Mmax(z, 0), 3 );
				
				pstEnemyCar[bNum]->x = x;
				pstEnemyCar[bNum]->y = y;
				pstEnemyCar[bNum]->z = z;
				
				Out_Of_Disp = Put_EnemyCAR(	dx,	dy, dz,	bMode_rev);
				if(Out_Of_Disp < 0)	/* 描画領域外 */
				{
					pstEnemyCar[bNum]->ubAlive = FALSE;
				}
#ifdef DEBUG	/* デバッグコーナー */
				{
					Draw_Pset(	dx, dy,	0xBE);	/* デバッグ用 */
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
	ST_CARDATA	stMyCar;

	GetMyCar(&stMyCar);

	for(i=0; i<ENEMYCAR_MAX; i++)
	{
		if(pstEnemyCar[i]->ubAlive == FALSE)
		{
			pstEnemyCar[i]->ubCarType = 0;
			pstEnemyCar[i]->VehicleSpeed = Mmax((stMyCar.VehicleSpeed - 10), 80);
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

	ret = G_BitBlt(	x,	w,	y,	h,	1,
					0,	0+w,	height_sum,	h,	1,
					ubMode, POS_MID, POS_CENTER);
	
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

