#ifndef	ENEMYCAR_C
#define	ENEMYCAR_C

#include <stdlib.h>
#include <iocslib.h>

#include "inc/usr_macro.h"
#include "EnemyCAR.h"
#include "OverKata.h"

#include "APL_Math.h"
#include "CRTC.h"
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
			SS	cal;
			SS	dx, dy, dz;
			SS	mx, my, dist;
			US	ras_x, ras_y, ras_pat, ras_num;
			SS	Out_Of_Disp = 0;
			ST_CARDATA	stMyCar;
			ST_CRT		stCRT;
			ST_RAS_INFO	stRasInfo;
			ST_ROAD_INFO	stRoadInfo;

#ifdef DEBUG	/* デバッグコーナー */
			UC	bDebugMode;
			GetDebugMode(&bDebugMode);
#endif
			ras_x = 0;
			ras_y = 0;
			
			x = g_pStEnemyCar[bNum]->x;
			y = g_pStEnemyCar[bNum]->y;
			z = g_pStEnemyCar[bNum]->z;

			GetCRT(&stCRT, bMode);
			GetMyCar(&stMyCar);
			GetRasterInfo(&stRasInfo);
			GetRoadInfo(&stRoadInfo);

			dist = stMyCar.VehicleSpeed - g_pStEnemyCar[bNum]->VehicleSpeed;

			cal = Mdiv256(dist * y);	/* 奥行の比率で移動量が変わる */
			if(cal == 0)
			{
				if(dist == 0)
				{
					/* 何もしない */
				}
				else if(dist > 0)
				{
					dist = 1;
				}
				else
				{
					dist = -1;
				}
				
			}
			else
			{
				dist = cal;
			}
			
			/* ライバル車との距離 */
			if(dist >= 0)
			{
				y += Mmax(dist, 1);
			}
			else
			{
				dist = Mabs(dist);
				y -= dist;
				y = Mmax( y, stRasInfo.st );
				if(y == stRasInfo.st)	/* 先を越された */
				{
					Out_Of_Disp = -1;
				}
			}

			ras_num = Mmax( Mmin( y, stRasInfo.ed ), stRasInfo.st + RASTER_NEXT);	/* ラスター情報の配列番号を算出 */
			/* 位置 */
			y = Mmax(y, ras_num);
			my = Mmax(y - 32, 0);

			ret = GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num);	/* 配列番号のラスター情報取得 */
			
			/* センター */
			if( ras_x < 256 )	/* 左カーブ */
			{
				mx = ROAD_CT_POINT + (  0 - ras_x);
			}
			else	/* 右カーブ */
			{
				mx = ROAD_CT_POINT + (512 - ras_x);
			}
			
			if(x < 8)
			{
				cal = Mmul16(x) * my;
				mx -= Mdiv128(cal);
			}
			else
			{
				cal = x-8;
				cal = Mmul16(cal) * my;
				mx += Mdiv128(cal);
			}

			if( (y > 0) && (ret >= 0) && (y < Y_MAX_WINDOW) && (Out_Of_Disp == 0))
			{
				dx = mx;
				dy = y;
				/* 透視投影率＝焦点距離／（焦点距離＋Z位置）を256倍して16で割った pat 0-10 */
				z = Mmin( Mmax( 10 - ( Mdiv10( Mmul256(my) / (my + ROAD_ED_POINT) ) ) , 0), 10 );
				dz = z;
				
				/* 当たり判定の設定 */
				g_pStEnemyCar[bNum]->sx = dx - (Mdiv2(ENEMY_CAR_1_W >> dz));
				g_pStEnemyCar[bNum]->ex = dx + (Mdiv2(ENEMY_CAR_1_W >> dz));
				g_pStEnemyCar[bNum]->sy = dy - (Mdiv2(ENEMY_CAR_1_H >> dz));
				g_pStEnemyCar[bNum]->ey = dy + (Mdiv2(ENEMY_CAR_1_H >> dz));
				
				/* 描画 */
				Out_Of_Disp = Put_EnemyCAR(	stCRT.hide_offset_x + dx,
											stCRT.hide_offset_y + dy,
											dz,
											bMode_rev);
				if(Out_Of_Disp < 0)	/* 描画領域外 */
				{
					/* 出現ポイントで描画領域外となるので要検討 */
//					g_pStEnemyCar[bNum]->ubAlive = FALSE;
				}
				else
				{
#ifdef DEBUG	/* デバッグコーナー */
					if(bDebugMode == TRUE)
					{
						Draw_Box(
							stCRT.hide_offset_x + dx - ((ENEMY_CAR_1_W >> dz)>>2),
							stCRT.hide_offset_y + dy - ((ENEMY_CAR_1_H >> dz)>>1),
							stCRT.hide_offset_x + dx + ((ENEMY_CAR_1_W >> dz)>>2),
							stCRT.hide_offset_y + dy + ((ENEMY_CAR_1_H >> dz)>>1), 0x13, 0xFFFF);
					}
#endif
				}
			}
			else
			{
				if(y >= Y_MAX_WINDOW)	/* 追い抜かした */
				{
					ADPCM_Play(14);	/* SE：自動車通過 */
				}
				if(Out_Of_Disp < 0)
				{
					ADPCM_Play(16);	/* SE：WAKAME */
				}
				g_pStEnemyCar[bNum]->ubAlive = FALSE;
			}

			g_pStEnemyCar[bNum]->x = x;
			g_pStEnemyCar[bNum]->y = y;
			g_pStEnemyCar[bNum]->z = z;
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
			UI	rand = 0;
			/* 左右の挙動 */
			rand = random();
			rand &= 0x0Fu;
			
			g_pStEnemyCar[i]->ubCarType = 0;
			g_pStEnemyCar[i]->VehicleSpeed = Mmax(Mmin(stMyCar.VehicleSpeed, 110), 80);
			g_pStEnemyCar[i]->x = rand;
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
	UI	uWidth, uHeight, uFileSize;
	UI	uWidth_o, uHeight_o;
	UI	uW_tmp, uH_tmp;
	US	height_sum = 0u;
	US	height_sum_o = 0u;
	
	Get_PicImageInfo( ENEMYCAR_CG, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	uWidth_o = uWidth;
	uHeight_o = uHeight;
	
	for(i=1; i<=Size; i++)
	{
		/* 縮小先のサイズ */
		height_sum_o += uHeight_o;
		uW_tmp = uWidth_o << 3;
		uWidth_o = Mmul_p1(uW_tmp);
		uH_tmp = uHeight_o << 3;
		uHeight_o = Mmul_p1(uH_tmp);
		
		/* 次の縮小元 */
		height_sum += uHeight;
		uWidth = uWidth_o;
		uHeight = uHeight_o;
	}
	
	w = uWidth;
	h = uHeight;

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

