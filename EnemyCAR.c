#ifndef	ENEMYCAR_C
#define	ENEMYCAR_C

#include <stdlib.h>
#include <iocslib.h>

#include "inc/usr_macro.h"
#include "EnemyCAR.h"

#include "Draw.h"
#include "Graphic.h"
#include "MFP.h"
#include "Music.h"
#include "MyCar.h"
#include "Raster.h"

/* 構造体定義 */
ST_ENEMYCARDATA	stEnemyCar[ENEMYCAR_MAX] = {0};
ST_ENEMYCARDATA	*g_pStEnemyCar[ENEMYCAR_MAX];

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
		
		g_pStEnemyCar[i] = &stEnemyCar[i];
	}
	
	return ret;
}

SS	GetEnemyCAR(ST_ENEMYCARDATA *stDat, SS Num)
{
	SS	ret = 0;
	if(Num < ENEMYCAR_MAX)
	{
		*stDat = *g_pStEnemyCar[Num];
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
		*g_pStEnemyCar[Num] = stDat;
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
		if(g_pStEnemyCar[bNum]->ubAlive == TRUE)
		{
			SS	x, y, z;
			SS	dx, dy, dz;
			SS	my, dist;
			US	ras_x, ras_y, ras_pat, ras_num;
			SS	Out_Of_Disp;
			UI	rand = 0;
			ST_CARDATA	stMyCar;
			ST_CRT		stCRT;
			ST_RAS_INFO	stRasInfo;
			ST_ROAD_INFO	stRoadInfo;

			ras_x = 0;
			ras_y = 0;
			
			x = g_pStEnemyCar[bNum]->x;
			y = g_pStEnemyCar[bNum]->y;
			z = g_pStEnemyCar[bNum]->z;

			GetCRT(&stCRT, bMode);
			GetMyCar(&stMyCar);
			GetRasterInfo(&stRasInfo);
			GetRoadInfo(&stRoadInfo);

			/* ライバル車との距離 */
			if(stMyCar.VehicleSpeed >= g_pStEnemyCar[bNum]->VehicleSpeed)
			{
				dist = (stMyCar.VehicleSpeed - g_pStEnemyCar[bNum]->VehicleSpeed);
				dist = Mmin(Mmax(dist, 0), 200);
				y += dist;
			}
			else
			{
				dist = (g_pStEnemyCar[bNum]->VehicleSpeed - stMyCar.VehicleSpeed);
				dist = Mmin(Mmax(dist, 0), 200);
				y -= dist;
			}
			my = Mmin( Mmax( y, 2 ), stRasInfo.size - 1 );
			
			ras_num = (US)(stRasInfo.st + my);	/* ラスター情報の配列番号を算出 */
			GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num);	/* 配列番号のラスター情報取得 */

			/* 左右の挙動 */
			rand = random();
			rand &= 0x0Fu;
			
			if(ras_x < 256)	/* 道の左側 */
			{
				x = (SS)ras_x + rand;
			}
			else	/* 道の右側 */
			{
				x = (SS)ras_x - 512 - rand;
			}
			y = my;
			z =  3 - (((my<<8) / (my + (ROAD_SIZE-RASTER_NEXT)))>>5);

			g_pStEnemyCar[bNum]->x = x;
			g_pStEnemyCar[bNum]->y = y;
			g_pStEnemyCar[bNum]->z = z;
			
			if( (2 < my) && ((stRasInfo.size-8) >= my) )
			{
				dx = (WIDTH>>1) - x;
				dy = stRoadInfo.Horizon + (((ras_y - RASTER_MIN) + ras_num) - ROAD_ST_POINT);
				dz = Mmin( Mmax(z, 0), 3 );
				
				/* 当たり判定の設定 */
				g_pStEnemyCar[bNum]->sx = dx - ((ENEMY_CAR_1_W >> dz)>>1);
				g_pStEnemyCar[bNum]->ex = dx + ((ENEMY_CAR_1_W >> dz)>>1);
				g_pStEnemyCar[bNum]->sy = dy - ((ENEMY_CAR_1_H >> dz)>>1);
				g_pStEnemyCar[bNum]->ey = dy + ((ENEMY_CAR_1_H >> dz)>>1);
				
				Out_Of_Disp = Put_EnemyCAR(	stCRT.hide_offset_x + dx, stCRT.hide_offset_y + dy, dz,	bMode_rev);
				if(Out_Of_Disp < 0)	/* 描画領域外 */
				{
					g_pStEnemyCar[bNum]->ubAlive = FALSE;
				}
#ifdef DEBUG	/* デバッグコーナー */
				{
					Draw_Box(
						stCRT.hide_offset_x + dx - ((ENEMY_CAR_1_W >> dz)>>1),
						stCRT.hide_offset_y + dy - ((ENEMY_CAR_1_H >> dz)>>1),
						stCRT.hide_offset_x + dx + ((ENEMY_CAR_1_W >> dz)>>1),
						stCRT.hide_offset_y + dy + ((ENEMY_CAR_1_H >> dz)>>1), 0x13, 0xFFFF);
				}
#endif
			}
			else
			{
				if((stRasInfo.size-8) < my)	/* 追い抜かした */
				{
					ADPCM_Play(14);	/* SE：自動車通過 */
				}
				g_pStEnemyCar[bNum]->ubAlive = FALSE;
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
		if(g_pStEnemyCar[i]->ubAlive == FALSE)
		{
			g_pStEnemyCar[i]->ubCarType = 0;
			g_pStEnemyCar[i]->VehicleSpeed = Mmax((stMyCar.VehicleSpeed - 10), 80);
			g_pStEnemyCar[i]->x = 0;
			g_pStEnemyCar[i]->y = 0;
			g_pStEnemyCar[i]->z = 4;
			g_pStEnemyCar[i]->sx = 0;
			g_pStEnemyCar[i]->ex = 0;
			g_pStEnemyCar[i]->sy = 0;
			g_pStEnemyCar[i]->ey = 0;
			g_pStEnemyCar[i]->ubBrakeLights = 0;
			g_pStEnemyCar[i]->ubOBD = 0;
			g_pStEnemyCar[i]->ubAlive = TRUE;
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

	ret = G_BitBlt(	x,	w,	y,	h,	0,
					0,	0+w,	height_sum,	h,	0,
					ubMode, POS_MID, POS_CENTER);
	
	return	ret;
}

SS	Sort_EnemyCAR(void)
{
	SS	ret = 0;
	SS	i;
	SS	count = 0;
	ST_ENEMYCARDATA	*pStEnemyCar_Tmp;
	
	while(1)
	{
		for(i=0; i<ENEMYCAR_MAX-1; i++)
		{
			if(g_pStEnemyCar[i]->y > g_pStEnemyCar[i+1]->y)
			{
				pStEnemyCar_Tmp = g_pStEnemyCar[i+1];
				g_pStEnemyCar[i+1] = g_pStEnemyCar[i];
				g_pStEnemyCar[i] = pStEnemyCar_Tmp;
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

