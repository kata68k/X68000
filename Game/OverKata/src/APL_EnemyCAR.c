#ifndef	APL_ENEMYCAR_C
#define	APL_ENEMYCAR_C

#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>

#include <usr_macro.h>
#include "APL_EnemyCAR.h"
#include "OverKata.h"

#include "BIOS_CRTC.h"
#include "BIOS_MFP.h"
#include "BIOS_PCG.h"
#include "IF_Draw.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"
#include "IF_Input.h"
#include "IF_Math.h"
#include "IF_Memory.h"
#include "IF_Music.h"
#include "APL_Graphic.h"
#include "APL_MyCar.h"
#include "APL_Raster.h"
#include "APL_Score.h"

/* グローバル変数 */
uint8_t		*g_pCG_EnemyCARImageBuf[ENEMYCAR_TYP_MAX][ENEMYCAR_PAT_MAX];
//const int16_t	g_nViewRateTbl[ENEMYCAR_PAT_MAX] = {96, 77, 61, 49, 39, 31, 25, 20, 16, 13, 10};
//const int16_t	g_nViewRateTbl[ENEMYCAR_PAT_MAX] = { 198, 158, 126, 101, 79, 65, 51, 41, 33 };
const int16_t	g_nViewRateTbl[ENEMYCAR_PAT_MAX] = {96, 93, 90, 87, 84, 82, 79, 76, 73, 71, 68, 65, 62, 59, 57, 54, 51, 48, 46, 43, 40, 37, 34, 32, 29, 26, 23, 21, 18, 15, 12, 10};


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
		stEnemyCar[i].z = ENEMYCAR_PAT_MAX - 1;
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
	uint16_t	uCount;
	static uint16_t	uTime = 0xFFFF;
	static uint16_t	uTimeDiff = 0;
	
	GetRoadCycleCount(&uCount);
	
	if(bNum == 0)
	{
		if(uTime == 0xFFFF)
		{
			uTime = uCount;
		}
		uTimeDiff = uCount - uTime;
		uTime = uCount;
	}
	
	if(bNum >= ENEMYCAR_MAX)
	{
		ret = -1;
		return ret;
	}
	
	if(g_pStEnemyCar[bNum]->ubAlive == TRUE)
	{
		int16_t	i;
		int16_t	y_ofst, y_inv, y_old;
		int16_t	x, y, z;
		int16_t	dis;
		int16_t	dx, dy, dz;
		int16_t	mx, my;
		uint16_t	ras_x, ras_y, ras_pat, ras_num;
		uint16_t	ras_x_o, ras_y_o, ras_pat_o;
		int16_t	Out_Of_Disp = 0;
		uint8_t	ubType, ubPass;
		ST_CARDATA	stMyCar;
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
		
		GetMyCar(&stMyCar);
		GetRasterInfo(&stRasInfo);
		GetRoadInfo(&stRoadInfo);

		y_old = y;	/* 前回値 */
		ubPass = TRUE;
		
		/* ライバル車との距離 */
		if(g_pStEnemyCar[bNum]->VehicleSpeed == 0)
		{
			if(uTimeDiff == 0)	/* 前回との時間差 */
			{
				/* 変化なし */
				y_ofst = 0;
				dis = 0;
			}
			else
			{
				dis = 128;
			}
		}
		else
		{
			dis = stMyCar.VehicleSpeed - g_pStEnemyCar[bNum]->VehicleSpeed;
		}

		y_ofst = 0;
		/* 車幅(片側)は、4mと定義する。 */
		/* 4mで192dotであれば、1dotあたり0.021mと定義する。 */
		/* 画面奥が1dotの道幅しかないので画面奥は192m先にとなる。 */
		ras_num = Mmin( Mdiv256(Mdec(SHRT_MAX, y)), stRasInfo.ed );	/* ラスター情報の配列番号を算出 */

		if(ras_num == 0)
		{
			y_ofst++;
			y = Mdec(y, Mmul256(y_ofst));
		}
		else
		{
			if(dis >= 0)
			{
				if(dis == 128)
				{
					dis = 4;
				}
				else
				{
					dis = 1;
				}
				
				for(i = ras_num; i < stRasInfo.ed; i++)
				{
					y_ofst = Minc(y_ofst, dis);
					GetRasterIntPos(&ras_x_o, &ras_y_o, &ras_pat_o, i - 1, FALSE);	/* 配列番号のラスター情報取得 */
					GetRasterIntPos(&ras_x, &ras_y, &ras_pat, i, FALSE);			/* 配列番号のラスター情報取得 */
					if(ras_pat_o != ras_pat)	/* 境界で設定する */
					{
						break;
					}
				}
				y = Mdec(y, Mmul256(y_ofst));
			}
			else
			{
				dis = 1;

				for(i = ras_num; i > 0; i--)
				{
					y_ofst = Minc(y_ofst, Mabs(dis));
					GetRasterIntPos(&ras_x_o, &ras_y_o, &ras_pat_o, i + 1, FALSE);	/* 配列番号のラスター情報取得 */
					GetRasterIntPos(&ras_x, &ras_y, &ras_pat, i, FALSE);			/* 配列番号のラスター情報取得 */
					if(ras_pat_o != ras_pat)	/* 境界で設定する */
					{
						break;
					}
				}
				y = Minc(y, Mmul256(y_ofst));

				ubPass = FALSE;
			}
		}

		if(y >= 0)
		{
			y_inv = SHRT_MAX - y;	/* 逆数 */

			z = Ms15b_To_s5b(Mmin(y_inv + 21845, SHRT_MAX));
		
			my = Mdiv256(y_inv);

			ras_num = Mmin( my, stRasInfo.ed );	/* ラスター情報の配列番号を算出 */
			ret = GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num, FALSE);	/* 配列番号のラスター情報取得 */

			/* センター */
			if(ras_x < 256)
			{
				mx = x - ras_x;// + Mu9b_To_u8b((int16_t)ras_x);
			}
			else
			{
				mx = x - (ras_x - 512);// + Mu9b_To_u8b((int16_t)ras_x);
			}

			dx = mx;
			dy = Mmul2(my) + stRasInfo.st;
			dz = z;

			/* 当たり判定の設定 */
			g_pStEnemyCar[bNum]->sx = dx - (Mdiv2(ENEMY_CAR_1_W >> dz));
			g_pStEnemyCar[bNum]->ex = dx + (Mdiv2(ENEMY_CAR_1_W >> dz));
			g_pStEnemyCar[bNum]->sy = dy - (Mdiv2(ENEMY_CAR_1_H >> dz));
			g_pStEnemyCar[bNum]->ey = dy + (Mdiv2(ENEMY_CAR_1_H >> dz));
			
			/* 描画 */
			Out_Of_Disp = Put_EnemyCAR(	dx,
										dy,
										dz,
										bMode_rev, ubType);
			if(y_old < y)	/* 追い抜かした */
			{
				ADPCM_Play(14);	/* SE：自動車通過 */
				S_Add_Score();	/* 加点 */
				g_pStEnemyCar[bNum]->ubAlive = FALSE;
				x = ROAD_CT_POINT;
				y = SHRT_MAX;
				z = ENEMYCAR_PAT_MAX - 1;
			}
		}
		else	/* 画面外へ */
		{
			ADPCM_Play(16);	/* SE：WAKAME */
			g_pStEnemyCar[bNum]->ubAlive = FALSE;
			x = ROAD_CT_POINT;
			y = SHRT_MAX;
			z = ENEMYCAR_PAT_MAX - 1;
		}

		g_pStEnemyCar[bNum]->x = x;
		g_pStEnemyCar[bNum]->y = y;
		g_pStEnemyCar[bNum]->z = z;
	}
	else
	{
		/* nop */
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
			g_pStEnemyCar[i]->x = ROAD_CT_POINT + rand;
			g_pStEnemyCar[i]->y = SHRT_MAX;
			g_pStEnemyCar[i]->z = ENEMYCAR_PAT_MAX - 1;
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

	int16_t	sp_x, sp_y;
	ST_PCG	*p_stPCG = NULL;
#if 1
	int16_t PatNumber;
	uint16_t	*pSrcBuf = NULL;
	uint32_t	uWidth=0, uHeight=0;
	BITMAPINFOHEADER *pInfo;
	ST_CRT		stCRT;
	
	if(ubType >= ENEMYCAR_TYP_MAX)return -1;
	if(Size >= ENEMYCAR_PAT_MAX)return -1;
	
	pSrcBuf = g_stPicEnemyCARImage[ubType][Size].pImageData;
	pInfo 	= g_stPicEnemyCARImage[ubType][Size].pBMi;

	uWidth	= pInfo->biWidth;
	uHeight	= pInfo->biHeight;
	PatNumber = ENEMYCAR_CG + ubType;
	
	GetCRT(&stCRT, ubMode);
	
	ret = G_BitBlt_From_Mem(x, y, 0, pSrcBuf, uWidth, uHeight, ubMode, POS_MID, POS_BOTTOM, PatNumber);

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
		uWidth_o = Mmul_0p10(uW_tmp);
		uH_tmp = uHeight_o << 3;
		uHeight_o = Mmul_0p10(uH_tmp);
		
		/* 次の縮小元 */
		height_sum += uHeight;
		uWidth = uWidth_o;
		uHeight = uHeight_o;
	}
	
	ret = G_BitBlt(	x,	w,	y,	h,	0,
					0,	0+w,	height_sum,	h,	0,
					ubMode, POS_MID, POS_CENTER);
#endif
	sp_x = x + SP_X_OFFSET - 24;
	sp_y = y;
	
	p_stPCG = PCG_Get_Info(OBJ_SHADOW);	/* 影1*3 */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = sp_x;
		p_stPCG->y = sp_y;
		if(ret == 0)
		{
			p_stPCG->update	= TRUE;
		}
		else
		{
			p_stPCG->update	= FALSE;
		}
	}
	
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

	ret = G_Load_Mem( PatNumber, 0, 0, 0 );	/* ライバル車 */
	if(ret != 0)return ret;

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
			uWidth_dst = Mmul_0p10(uW_tmp);
			uWidth_dst = Mmax(uWidth_dst, 8);
			
			/* 縮小先のサイズ(H) */
			uH_tmp = uHeight_dst << 3;
			uHeight_dst = Mmul_0p10(uH_tmp);
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

#endif	/* APL_ENEMYCAR_C */

