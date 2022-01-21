#ifndef	ENEMYCAR_C
#define	ENEMYCAR_C

#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>

#include "inc/usr_macro.h"
#include "EnemyCAR.h"
#include "OverKata.h"

#include "APL_Math.h"
#include "CRTC.h"
#include "Draw.h"
#include "FileManager.h"
#include "Graphic.h"
#include "Input.h"
#include "MFP.h"
#include "Music.h"
#include "MyCar.h"
#include "Raster.h"
#include "Score.h"

/* グローバル変数 */
uint8_t		*g_pCG_EnemyCARImageBuf[ENEMYCAR_TYP_MAX][ENEMYCAR_PAT_MAX];

/* 構造体定義 */
ST_ENEMYCARDATA	stEnemyCar[ENEMYCAR_MAX] = {0};
ST_ENEMYCARDATA	*g_pStEnemyCar[ENEMYCAR_MAX];

PICIMAGE	g_stPicEnemyCARImage[ENEMYCAR_TYP_MAX][ENEMYCAR_PAT_MAX];

/* 関数のプロトタイプ宣言 */
int16_t	InitEnemyCAR(void);
int16_t	GetEnemyCAR(ST_ENEMYCARDATA *, int16_t);
int16_t	SetEnemyCAR(ST_ENEMYCARDATA, int16_t);
int16_t	EnemyCAR_main(uint8_t, uint8_t, uint8_t);
int16_t	SetAlive_EnemyCAR(void);
int16_t	Put_EnemyCAR(uint16_t, uint16_t, uint16_t, uint8_t, uint8_t);
int16_t	Sort_EnemyCAR(void);
int16_t	Load_EnemyCAR(int16_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	InitEnemyCAR(void)
{
	int16_t ret = 0;
	int16_t i;
	
	for(i=0; i<ENEMYCAR_MAX; i++)
	{
		stEnemyCar[i].ubCarType = 0;
		stEnemyCar[i].VehicleSpeed = 60;
		stEnemyCar[i].x = 0;
		stEnemyCar[i].y = 0;
		stEnemyCar[i].z = 4;
		stEnemyCar[i].ubBrakeLights = 0;
		stEnemyCar[i].ubOBD = 0;
		stEnemyCar[i].ubAlive = FALSE;
		
		g_pStEnemyCar[i] = &stEnemyCar[i];
	}
	
	for(i=0; i<ENEMYCAR_TYP_MAX; i++)
	{
		Load_EnemyCAR(i);	/* 画像をVRAMに展開 */
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetEnemyCAR(ST_ENEMYCARDATA *stDat, int16_t Num)
{
	int16_t	ret = 0;
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetEnemyCAR(ST_ENEMYCARDATA stDat, int16_t Num)
{
	int16_t	ret = 0;
	
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	EnemyCAR_main(uint8_t bNum, uint8_t bMode, uint8_t bMode_rev)
{
	int16_t	ret = 0;
	
	if(bNum < ENEMYCAR_MAX)
	{
		if(g_pStEnemyCar[bNum]->ubAlive == TRUE)
		{
			int16_t	x, y, z;
			int16_t	cal, dis;
			int16_t	dx, dy, dz;
			int16_t	mx, my;
			uint16_t	ras_x, ras_y, ras_pat, ras_num;
			int16_t	Out_Of_Disp = 0;
			uint8_t	ubType;
			ST_CARDATA	stMyCar;
			ST_CRT		stCRT;
			ST_RAS_INFO	stRasInfo;
			ST_ROAD_INFO	stRoadInfo;

#ifdef DEBUG	/* デバッグコーナー */
			uint8_t	bDebugMode;
			GetDebugMode(&bDebugMode);
#endif
			ras_x = 0;
			ras_y = 0;
			
			x = g_pStEnemyCar[bNum]->x;
			y = g_pStEnemyCar[bNum]->y;
			z = g_pStEnemyCar[bNum]->z;
			ubType = g_pStEnemyCar[bNum]->ubCarType;
			
			GetCRT(&stCRT, bMode);
			GetMyCar(&stMyCar);
			GetRasterInfo(&stRasInfo);
			GetRoadInfo(&stRoadInfo);

#if 0
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
#endif
			/* ライバル車との距離 */
			dis = stMyCar.VehicleSpeed - g_pStEnemyCar[bNum]->VehicleSpeed;
			if(y <= 0)
			{
				y = stRasInfo.st;
			}
			cal = Mmin(Mdiv256(Mabs(dis) * y), 4);	/* 奥行の比率で移動量が変わる */
			
			if(dis >= 0)
			{
				y += cal;
			}
			else
			{
				y -= cal;
				
				if(y <= stRasInfo.st)
				{
					Out_Of_Disp = -1;	/* 先を越された */
				}
			}
			/* 位置 */
			my = Mmax(y - stRasInfo.st, 0);

			ras_num = Mmin( y - stRasInfo.st, stRasInfo.ed );	/* ラスター情報の配列番号を算出 */

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

			if( (my > 0) && (ret >= 0) && (y < Y_MAX_WINDOW) && (Out_Of_Disp == 0))
			{
				dx = mx;
				dy = y;
				cal = y - RASTER_MIN;
				/* 透視投影率＝焦点距離／（焦点距離＋Z位置）を256倍して16で割った pat 0-10 */
				z = Mmin( Mmax( 10 - ( Mdiv10( Mmul256(cal) / (cal + ROAD_ED_POINT) ) ) , 0), 10 );
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
											bMode_rev, ubType);
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
							stCRT.hide_offset_y + dy + ((ENEMY_CAR_1_H >> dz)>>1), 0x03, 0xFFFF);
					}
#endif
				}
			}
			else
			{
				if(y >= Y_MAX_WINDOW)	/* 追い抜かした */
				{
					ADPCM_Play(14);	/* SE：自動車通過 */
					S_Add_Score();	/* 加点 */
				}
				if(Out_Of_Disp < 0)
				{
//					ADPCM_Play(16);	/* SE：WAKAME */
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetAlive_EnemyCAR(void)
{
	int16_t	ret = 0;
	int16_t	i;
	ST_CARDATA	stMyCar;

	GetMyCar(&stMyCar);

	for(i=0; i<ENEMYCAR_MAX; i++)
	{
		if(g_pStEnemyCar[i]->ubAlive == FALSE)
		{
			uint32_t	rand = 0;
			/* 左右の挙動 */
			rand = random();
			rand &= 0x0Fu;
			
			g_pStEnemyCar[i]->ubCarType = rand & 0x07u;
			if(g_pStEnemyCar[i]->ubCarType < 4)
			{
				g_pStEnemyCar[i]->VehicleSpeed = Mmax(Mmin(stMyCar.VehicleSpeed-10, 240), 60);
			}
			else
			{
				g_pStEnemyCar[i]->VehicleSpeed = 0;
			}
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Put_EnemyCAR(uint16_t x, uint16_t y, uint16_t Size, uint8_t ubMode, uint8_t ubType)
{
	int16_t	ret = 0;

#if 1
	uint16_t	*pSrcBuf = NULL;
	uint32_t	uWidth=0, uHeight=0;
	BITMAPINFOHEADER *pInfo;
	
	if(ubType >= ENEMYCAR_TYP_MAX)return -1;
	if(Size >= ENEMYCAR_PAT_MAX)return -1;
	
	pSrcBuf = g_stPicEnemyCARImage[ubType][Size].pImageData;
	pInfo 	= g_stPicEnemyCARImage[ubType][Size].pBMi;

	uWidth	= pInfo->biWidth;
	uHeight	= pInfo->biHeight;
	
	ret = G_BitBlt_From_Mem( x, y, 0, pSrcBuf, uWidth, uHeight, ubMode, POS_MID, POS_CENTER);
#else
	int16_t	i;
	uint16_t	w, h;
	uint32_t	uWidth, uHeight, uFileSize;
	uint32_t	uWidth_o, uHeight_o;
	uint32_t	uW_tmp, uH_tmp;
	uint16_t	height_sum = 0u;
	uint16_t	height_sum_o = 0u;
	
	w = uWidth;
	h = uHeight;
	
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
	
	ret = G_BitBlt(	x,	w,	y,	h,	0,
					0,	0+w,	height_sum,	h,	0,
					ubMode, POS_MID, POS_CENTER);
#endif
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Sort_EnemyCAR(void)
{
	int16_t	ret = 0;
	int16_t	i;
	int16_t	count = 0;
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Load_EnemyCAR(int16_t Num)
{
	int16_t	ret = 0;
	
	int16_t PatNumber;
	uint16_t height_sum = 0u;
	uint16_t height_sum_o = 0u;
	uint32_t i=0u;
	uint32_t uWidth, uHeight, uFileSize;
	uint32_t uWidth_dst, uHeight_dst;
//	uint32_t uOffset_X = 0u;
	uint16_t *pSrcBuf = NULL;
#if 1
	int32_t	Size;
	uint32_t	uSize8x = 0;
	uint16_t	*pDstBuf = NULL;
	BITMAPFILEHEADER *pFile;
	BITMAPINFOHEADER *pInfo;
	
#endif
	if(Num >= ENEMYCAR_TYP_MAX)return -1;
	PatNumber = ENEMYCAR_CG + Num;

	CG_File_Load( PatNumber );	/* グラフィックの読み込み */
//	ret = G_Load_Mem( PatNumber, uOffset_X,	0,	0);	/* ライバル車 */
	pSrcBuf = Get_PicImageInfo( PatNumber, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	uWidth_dst = uWidth;
	uHeight_dst = uHeight;
#ifdef DEBUG
//	printf("debug1(0x%p)=(%d,%d)\n", pSrcBuf, uWidth, uHeight );
#endif

#if 1
	/* PICヘッダにメモリ割り当て */
	g_stPicEnemyCARImage[Num][i].pBMf = (BITMAPFILEHEADER*)MyMalloc( FILE_HEADER_SIZE );
	g_stPicEnemyCARImage[Num][i].pBMi = (BITMAPINFOHEADER*)MyMalloc( INFO_HEADER_SIZE );
	pFile = g_stPicEnemyCARImage[Num][i].pBMf;
	pInfo = g_stPicEnemyCARImage[Num][i].pBMi;
	pInfo->biWidth = uWidth;
	pInfo->biHeight = uHeight;
	/* メモリのサイズ演算 */
	uSize8x = ((((pInfo->biWidth)+7)/8) * 8);	/* 8の倍数 */
	Size = (pInfo->biHeight) * uSize8x * sizeof(uint16_t);
	pFile->bfSize = Size;		/* メモリサイズ設定 */
	/* メモリ確保 */
//	g_stPicEnemyCARImage[Num][i].pImageData = NULL;
	if(g_stPicEnemyCARImage[Num][i].pImageData != NULL)	/* Nullチェック */
	{
		MyMfree(g_stPicEnemyCARImage[Num][i].pImageData);	/* メモリ解放 */
	}
	g_stPicEnemyCARImage[Num][i].pImageData = (uint16_t*)MyMalloc( Size );	/* メモリの確保 */
	memset(g_stPicEnemyCARImage[Num][i].pImageData, 0, Size);				/* メモリクリア */
//	memcpy(g_stPicEnemyCARImage[Num][i].pImageData, pSrcBuf, Size);			/* マスターからライバル車用のバッファにコピー */
	ret = G_Copy_Pict_To_Mem(	g_stPicEnemyCARImage[Num][i].pImageData, uWidth, uHeight, pSrcBuf, uWidth, uHeight);	/* マスターからライバル車用のバッファにコピー */
#ifdef DEBUG
//	printf("debug2(0x%p)=(%d,%d)\n", g_stPicEnemyCARImage[Num][0].pImageData, uWidth, uHeight );
//	ret = G_BitBlt_From_Mem( 0, 0, 0, g_stPicEnemyCARImage[Num][0].pImageData, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);
//	KeyHitESC();	/* デバッグ用 */
#endif
	
#endif

	if(ret >= 0)
	{
		/* ENEMYCAR_PAT_MAXパターンを作る */
		uint32_t	uW_tmp, uH_tmp;
		
		height_sum = 0;
		height_sum_o = 0;

		for(i=1; i <= ENEMYCAR_PAT_MAX; i++)
		{
#if 1
			/* PICヘッダにメモリ割り当て */
			g_stPicEnemyCARImage[Num][i].pBMf = (BITMAPFILEHEADER*)MyMalloc( FILE_HEADER_SIZE );
			g_stPicEnemyCARImage[Num][i].pBMi = (BITMAPINFOHEADER*)MyMalloc( INFO_HEADER_SIZE );
			pFile = g_stPicEnemyCARImage[Num][i].pBMf;
						pInfo = g_stPicEnemyCARImage[Num][i].pBMi;
#else
			/* 縮小先のサイズ */
			height_sum_o += uHeight_dst;
#endif
			/* 縮小先のサイズ(W) */
			uW_tmp = uWidth_dst << 3;
			uWidth_dst = Mmul_p1(uW_tmp);
			uWidth_dst = Mmax(uWidth_dst, 8);
			
			/* 縮小先のサイズ(H) */
			uH_tmp = uHeight_dst << 3;
			uHeight_dst = Mmul_p1(uH_tmp);
			uHeight_dst = Mmax(uHeight_dst, 1);
#if 1
			pInfo->biWidth = uWidth_dst;
			pInfo->biHeight = uHeight_dst;
			/* メモリのサイズ演算 */
			uSize8x = ((((pInfo->biWidth)+7)/8) * 8);	/* 8の倍数 */
			Size = (pInfo->biHeight) * uSize8x * sizeof(uint16_t);
			pFile->bfSize = Size;		/* メモリサイズ設定 */
			/* メモリ確保 */
//			g_stPicEnemyCARImage[Num][i].pImageData = NULL;
			if(g_stPicEnemyCARImage[Num][i].pImageData != NULL)	/* Nullチェック */
			{
				MyMfree(g_stPicEnemyCARImage[Num][i].pImageData);	/* メモリ解放 */
			}
			g_stPicEnemyCARImage[Num][i].pImageData = (uint16_t*)MyMalloc( Size );	/* メモリの確保 */
			memset(g_stPicEnemyCARImage[Num][i].pImageData, 0, Size);				/* メモリクリア */
#ifdef DEBUG
//			printf("debug1(%d,0x%p)=(%d,%d)\n", i, g_stPicEnemyCARImage[Num][i].pBMi, pInfo->biWidth, pInfo->biHeight );
//			KeyHitESC();	/* デバッグ用 */
//			printf("debug2(%d,0x%p)=%d\n", i, g_stPicEnemyCARImage[Num][i].pBMf, pFile->bfSize);
//			KeyHitESC();	/* デバッグ用 */
#endif
			
			pDstBuf = g_stPicEnemyCARImage[Num][i].pImageData;		/* 作業用のポインタにセット */
			pSrcBuf = g_stPicEnemyCARImage[Num][i-1].pImageData;	/* 作業用のポインタにセット */
#ifdef DEBUG
//			printf("debug3(%d,0x%p)(%d,%d)\n", i, pDstBuf, uWidth_dst, uHeight_dst);
//			printf("debug4(%d,0x%p)(%d,%d)\n", i-1, pSrcBuf, uWidth, uHeight);
//			KeyHitESC();	/* デバッグ用 */
#endif
			/* 縮小コピー */
			G_Stretch_Pict_To_Mem(	pDstBuf,	uWidth_dst,	uHeight_dst,
									pSrcBuf,	uWidth,		uHeight);
#else
			/* 描画 */
			G_Stretch_Pict( 
							0 + uOffset_X,	uWidth_dst + uOffset_X,
							height_sum_o,	uHeight_dst,
							0,
							0 + uOffset_X,	uWidth + uOffset_X,
							height_sum,		uHeight,
							0);
#endif
			/* 次の縮小元 */
			height_sum += uHeight;
#ifdef DEBUG
//			ret = G_BitBlt_From_Mem( 0, height_sum, 0, g_stPicEnemyCARImage[Num][i].pImageData, uWidth_dst, uHeight_dst, 0xFF, POS_TOP, POS_LEFT);
//			KeyHitESC();	/* デバッグ用 */
#endif
			uWidth = uWidth_dst;
			uHeight = uHeight_dst;
		}
	}
	
	return ret;
}

#endif	/* ENEMYCAR_C */

