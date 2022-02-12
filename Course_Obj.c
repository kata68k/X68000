#ifndef	COURSE_OBJ_C
#define	COURSE_OBJ_C

#include <stdio.h>

#include "inc/usr_macro.h"
#include "Course_Obj.h"
#include "OverKata.h"

#include "APL_Math.h"
#include "CRTC.h"
#include "Draw.h"
#include "FileManager.h"
#include "Graphic.h"
#include "MFP.h"
#include "MyCar.h"
#include "Output_Text.h"
#include "Raster.h"

/* 変数定義 */
uint8_t		*g_pCG_Course_ObjImageBuf[COURSE_OBJ_TYP_MAX][COURSE_OBJ_PAT_MAX];

/* 構造体定義 */
static ST_COURSE_OBJ	g_stCourse_Obj[COURSE_OBJ_MAX] = {0};
PICIMAGE	g_stPicCourse_ObjImage[COURSE_OBJ_TYP_MAX][COURSE_OBJ_PAT_MAX];

/* 関数のプロトタイプ宣言 */
int16_t	InitCourseObj(void);
int16_t	GetCourseObj(ST_COURSE_OBJ *, int16_t);
int16_t	SetCourseObj(ST_COURSE_OBJ, int16_t);
int16_t	Course_Obj_main(uint8_t, uint8_t, uint8_t);
int16_t	Put_CouseObject(int16_t, int16_t, uint16_t, uint8_t, uint8_t, uint8_t);
int16_t	Sort_Course_Obj(void);
int16_t	Load_Course_Obj(int16_t);
int16_t	Load_Course_Data(uint8_t);
int16_t	Move_Course_BG(uint8_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	InitCourseObj(void)
{
	int16_t ret = 0;
	int16_t i;
	
	for(i=0; i<COURSE_OBJ_MAX; i++)
	{
		g_stCourse_Obj[i].ubType = 0;
		g_stCourse_Obj[i].x = 0;
		g_stCourse_Obj[i].y = (COURSE_OBJ_H_MAX * i);
		g_stCourse_Obj[i].z = 0;
		g_stCourse_Obj[i].uTime = 0xFFFF;
		g_stCourse_Obj[i].ubAlive = TRUE;
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
int16_t	GetCourseObj(ST_COURSE_OBJ *stDat, int16_t Num)
{
	int16_t	ret = 0;
	if(Num < COURSE_OBJ_MAX)
	{
		*stDat = g_stCourse_Obj[Num];
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
int16_t	SetCourseObj(ST_COURSE_OBJ stDat, int16_t Num)
{
	int16_t	ret = 0;
	
	if(Num < COURSE_OBJ_MAX)
	{
		g_stCourse_Obj[Num] = stDat;
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
int16_t Course_Obj_main(uint8_t bNum, uint8_t bMode, uint8_t bMode_rev)
{
	int16_t	ret = 0;
	uint16_t	uCount;

	GetRoadCycleCount(&uCount);

	if(g_stCourse_Obj[bNum].ubAlive == TRUE)
	{
		int16_t	x, y, z;
		int16_t	dx, dy, dz;
		int16_t	st, cal;
		int16_t	mx, my;
		uint16_t	ras_x, ras_y, ras_pat, ras_num;
		uint8_t	bEven;
		int16_t	Out_Of_Disp;
		uint8_t	ubType;
		uint16_t	uTime;

		ST_CRT	stCRT;
		ST_ROAD_INFO	stRoadInfo;
		ST_RAS_INFO		stRasInfo;
		
		GetCRT(&stCRT, bMode);
		GetRoadInfo(&stRoadInfo);
		GetRasterInfo(&stRasInfo);

		if((bNum & 0x01) == 0)
		{
			bEven = TRUE;
		}
		else
		{
			bEven = FALSE;
		}

		/* 前回値 */
		x = g_stCourse_Obj[bNum].x;
		y = g_stCourse_Obj[bNum].y;
		z = g_stCourse_Obj[bNum].z;
		ubType = g_stCourse_Obj[bNum].ubType;
		uTime = g_stCourse_Obj[bNum].uTime;	/* 共通カウンタの前回値 */
		if(uTime == 0xFFFF)
		{
			uTime = uCount;
		}

		if(uCount > uTime)
		{
			y += (uCount - uTime);	/* 変化量 */
		}
//		my = Mmin(y, Y_MAX_WINDOW);
		my = Mmin(Mdiv16(y*y), Y_MAX_WINDOW);
//		my = APL_uDiv( ((Y_MAX_WINDOW - y) + 128), (Y_MAX_WINDOW - y) );
		
		ras_num = Mmin(my, stRasInfo.ed);	/* ラスター情報の配列番号を算出 */
		ret = GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num);	/* 配列番号のラスター情報取得 */

		if(stRasInfo.st < stRasInfo.mid)
		{
			st = 0;
		}
		else
		{
			st = stRasInfo.st - stRasInfo.mid;
		}

		x = Mmul2(ras_num);	/* 96に対して200なのでおよそ2倍 */
		
		/* センター */
		if( ras_x < 256 )	/* 左カーブ */
		{
			mx = ROAD_CT_POINT + (  0 - ras_x);
		}
		else	/* 右カーブ */
		{
			mx = ROAD_CT_POINT + (512 - ras_x);
		}
		cal = x + 16 + Mmul2(st);	/* 16は道の最小幅 */
		
		/* 差分 */
		if(bEven == TRUE)	/* 左 */
		{
			dx = mx - cal;
		}
		else				/* 右 */
		{
			dx = mx + cal;
		}
#if 1
		if(dx > 256)
		{
			dx = 0x7FFF;
		}
		if(dx < 0)
		{
			dx = 0x7FFF;
		}
#endif
		z = x;
		
		if( (y > 0) && (y < 36) && (ret >= 0) && (dx < 256))
		{
			
			/* 水平線 */
			dy = stRasInfo.st;
			/* 透視投影率＝焦点距離／（焦点距離＋Z位置）を２５６倍して６４で割った(/4pat) */
			//dz = Mmin( Mmax( 3 - (((z<<8) / (z + ROAD_ED_POINT))>>5) , 0), 3 );
			/* 透視投影率＝焦点距離／（焦点距離＋Z位置）を２５６倍して16で割った(=7pat) */
			dz = Mmin( Mmax( 7 - (Mdiv16(Mmul256(z) / (z + ROAD_ED_POINT))) , 0), 7);
			/* 描画 */
			Out_Of_Disp = Put_CouseObject(	stCRT.hide_offset_x + dx,
											stCRT.hide_offset_y + dy,
											dz,
											bMode_rev,
											ubType,
											bEven);
			x = dx;
			z = dz;

			if(Out_Of_Disp < 0)	/* 描画領域外 */
			{
				/* 出現ポイントで描画領域外となるので要検討 */
//				x = 0;
//				y = 0;
//				z = 0;
//				g_stCourse_Obj[bNum].ubAlive = FALSE;
			}
		}
		else
		{
			if(y >= 36)	/* 画面から消える判定は横移動の要素がつよい */
			{
				x = 0;
				y = 0;
				z = 0;
				g_stCourse_Obj[bNum].ubAlive = FALSE;
			}
		}

		g_stCourse_Obj[bNum].x = x;
		g_stCourse_Obj[bNum].y = y;
		g_stCourse_Obj[bNum].z = z;
		g_stCourse_Obj[bNum].uTime = uCount;
	}
	else
	{
		g_stCourse_Obj[bNum].ubType++;
		if(g_stCourse_Obj[bNum].ubType >= COURSE_OBJ_TYP_MAX)
		{
			g_stCourse_Obj[bNum].ubType = 0u;
		}
		g_stCourse_Obj[bNum].x = 0;
		g_stCourse_Obj[bNum].y = 0;
		g_stCourse_Obj[bNum].z = 0;
		g_stCourse_Obj[bNum].uTime = uCount;
		g_stCourse_Obj[bNum].ubAlive = TRUE;
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
int16_t	Put_CouseObject(int16_t x, int16_t y, uint16_t Size, uint8_t ubMode, uint8_t ubType, uint8_t ubPos)
{
	int16_t	ret = 0;

	uint16_t	*pSrcBuf = NULL;
	uint32_t	uWidth=0, uHeight=0;
	uint8_t		ubPos_H;
	BITMAPINFOHEADER *pInfo;
	
	if(ubType >= COURSE_OBJ_TYP_MAX)return -1;
	if(Size >= COURSE_OBJ_PAT_MAX)return -1;
	
	pSrcBuf = g_stPicCourse_ObjImage[ubType][Size].pImageData;
	pInfo 	= g_stPicCourse_ObjImage[ubType][Size].pBMi;

	uWidth	= pInfo->biWidth;
	uHeight	= pInfo->biHeight;
	
	if(ubPos == TRUE)	/* 左 */
	{
		ubPos_H = POS_RIGHT;
	}
	else				/* 右 */
	{
		ubPos_H = POS_LEFT;
	}
	ret = G_BitBlt_From_Mem( x, y, 0, pSrcBuf, uWidth, uHeight, ubMode, ubPos_H, POS_CENTER);
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Sort_Course_Obj(void)
{
	int16_t	ret = 0;
	int16_t	i;
	int16_t	count = 0;
	ST_COURSE_OBJ	stTmp;
	
	while(1)
	{
		for(i=0; i<COURSE_OBJ_MAX-1; i++)
		{
			if(g_stCourse_Obj[i].y > g_stCourse_Obj[i+1].y)
			{
				stTmp = g_stCourse_Obj[i+1];
				g_stCourse_Obj[i+1] = g_stCourse_Obj[i];
				g_stCourse_Obj[i] = stTmp;
				count = 0;
			}
			else
			{
				count++;
			}
		}
		if(count >= (COURSE_OBJ_MAX - 1))
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
int16_t	Load_Course_Obj(int16_t Num)
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

	int32_t	Size;
	uint32_t	uSize8x = 0;
	uint16_t	*pDstBuf = NULL;
	BITMAPFILEHEADER *pFile;
	BITMAPINFOHEADER *pInfo;
	
	if(Num >= COURSE_OBJ_TYP_MAX)return -1;
	PatNumber = COURSE_OBJ_CG + Num;

	G_Load_Mem( PatNumber, 0, 0, 0 );	/* ヤシの木 */
//	ret = G_Load_Mem( PatNumber, uOffset_X,	0,	0);	/* ヤシの木 */
	pSrcBuf = Get_PicImageInfo( PatNumber, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	uWidth_dst = uWidth;
	uHeight_dst = uHeight;
#ifdef DEBUG
//	printf("debug1(0x%p)=(%d,%d)\n", pSrcBuf, uWidth, uHeight );
#endif

	/* PICヘッダにメモリ割り当て */
	g_stPicCourse_ObjImage[Num][i].pBMf = (BITMAPFILEHEADER*)MyMalloc( FILE_HEADER_SIZE );
	g_stPicCourse_ObjImage[Num][i].pBMi = (BITMAPINFOHEADER*)MyMalloc( INFO_HEADER_SIZE );
	pFile = g_stPicCourse_ObjImage[Num][i].pBMf;
	pInfo = g_stPicCourse_ObjImage[Num][i].pBMi;
	pInfo->biWidth = uWidth;
	pInfo->biHeight = uHeight;
	/* メモリのサイズ演算 */
	uSize8x = ((((pInfo->biWidth)+7)/8) * 8);	/* 8の倍数 */
	Size = (pInfo->biHeight) * uSize8x * sizeof(uint16_t);
	pFile->bfSize = Size;		/* メモリサイズ設定 */
	/* メモリ確保 */
//	g_stPicCourse_ObjImage[Num][i].pImageData = NULL;
	if(g_stPicCourse_ObjImage[Num][i].pImageData != NULL)	/* Nullチェック */
	{
		MyMfree(g_stPicCourse_ObjImage[Num][i].pImageData);	/* メモリ解放 */
	}
	g_stPicCourse_ObjImage[Num][i].pImageData = (uint16_t*)MyMalloc( Size );	/* メモリの確保 */
	memset(g_stPicCourse_ObjImage[Num][i].pImageData, 0, Size);					/* メモリクリア */
//	memcpy(g_stPicCourse_ObjImage[Num][i].pImageData, pSrcBuf, Size);			/* マスターからヤシの木車用のバッファにコピー */
	ret = G_Copy_Pict_To_Mem(	g_stPicCourse_ObjImage[Num][i].pImageData, uWidth, uHeight, pSrcBuf, uWidth, uHeight);	/* マスターからヤシの木用のバッファにコピー */
#ifdef DEBUG
//	printf("debug2(0x%p)=(%d,%d)\n", g_stPicCourse_ObjImage[Num][0].pImageData, uWidth, uHeight );
//	ret = G_BitBlt_From_Mem( 0, 0, 0, g_stPicCourse_ObjImage[Num][0].pImageData, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);
//	KeyHitESC();	/* デバッグ用 */
#endif

	if(ret >= 0)
	{
		/* COURSE_OBJ_PAT_MAXパターンを作る */
		uint32_t	uW_tmp, uH_tmp;
		
		height_sum = 0;
		height_sum_o = 0;

		for(i=1; i<COURSE_OBJ_PAT_MAX; i++)
		{
			/* PICヘッダにメモリ割り当て */
			g_stPicCourse_ObjImage[Num][i].pBMf = (BITMAPFILEHEADER*)MyMalloc( FILE_HEADER_SIZE );
			g_stPicCourse_ObjImage[Num][i].pBMi = (BITMAPINFOHEADER*)MyMalloc( INFO_HEADER_SIZE );
			pFile = g_stPicCourse_ObjImage[Num][i].pBMf;
			pInfo = g_stPicCourse_ObjImage[Num][i].pBMi;

			/* 縮小先のサイズ(W) */
			uW_tmp = uWidth_dst << 3;
			uWidth_dst = Mmul_p1(uW_tmp);
			uWidth_dst = Mmax(uWidth_dst, 8);

			/* 縮小先のサイズ(H) */
			uH_tmp = uHeight_dst << 3;
			uHeight_dst = Mmul_p1(uH_tmp);
			uHeight_dst = Mmax(uHeight_dst, 1);

			pInfo->biWidth = uWidth_dst;
			pInfo->biHeight = uHeight_dst;
			/* メモリのサイズ演算 */
			uSize8x = ((((pInfo->biWidth)+7)/8) * 8);	/* 8の倍数 */
			Size = (pInfo->biHeight) * uSize8x * sizeof(uint16_t);
			pFile->bfSize = Size;		/* メモリサイズ設定 */
			/* メモリ確保 */
//			g_stPicCourse_ObjImage[Num][i].pImageData = NULL;
			if(g_stPicCourse_ObjImage[Num][i].pImageData != NULL)	/* Nullチェック */
			{
				MyMfree(g_stPicCourse_ObjImage[Num][i].pImageData);	/* メモリ解放 */
			}
			g_stPicCourse_ObjImage[Num][i].pImageData = (uint16_t*)MyMalloc( Size );	/* メモリの確保 */
			memset(g_stPicCourse_ObjImage[Num][i].pImageData, 0, Size);				/* メモリクリア */
#ifdef DEBUG
//			printf("debug1(%d,0x%p)=(%d,%d)\n", i, g_stPicCourse_ObjImage[Num][i].pBMi, pInfo->biWidth, pInfo->biHeight );
//			KeyHitESC();	/* デバッグ用 */
//			printf("debug2(%d,0x%p)=%d\n", i, g_stPicCourse_ObjImage[Num][i].pBMf, pFile->bfSize);
//			KeyHitESC();	/* デバッグ用 */
#endif
			
			pDstBuf = g_stPicCourse_ObjImage[Num][i].pImageData;		/* 作業用のポインタにセット */
			pSrcBuf = g_stPicCourse_ObjImage[Num][i-1].pImageData;	/* 作業用のポインタにセット */
#ifdef DEBUG
//			printf("debug3(%d,0x%p)(%d,%d)\n", i, pDstBuf, uWidth_dst, uHeight_dst);
//			printf("debug4(%d,0x%p)(%d,%d)\n", i-1, pSrcBuf, uWidth, uHeight);
//			KeyHitESC();	/* デバッグ用 */
#endif
			/* 縮小コピー */
			G_Stretch_Pict_To_Mem(	pDstBuf,	uWidth_dst,	uHeight_dst,
									pSrcBuf,	uWidth,		uHeight);
			/* 次の縮小元 */
			height_sum += uHeight;
#ifdef DEBUG
//			ret = G_BitBlt_From_Mem( 0, height_sum, 0, g_stPicCourse_ObjImage[Num][i].pImageData, uWidth_dst, uHeight_dst, 0xFF, POS_TOP, POS_LEFT);
//			KeyHitESC();	/* デバッグ用 */
#endif
			uWidth = uWidth_dst;
			uHeight = uHeight_dst;
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
int16_t	Load_Course_Data(uint8_t bCourseNum)
{
	int16_t	ret = 0;
	int8_t	str[256];
	uint16_t	x, y;
	ST_ROADDATA *p_stRoadData;
	
	if(bCourseNum == 0u)bCourseNum = 1u;
	
	/* コースデータ読み込み */
	p_stRoadData = (ST_ROADDATA *)GetRoadDataAddr();
	sprintf(str, "data/map/course%02d.csv", bCourseNum);
	File_Load_Course_CSV(str, p_stRoadData, &x, &y);

	ret = y;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Move_Course_BG(uint8_t bMode)
{
	int16_t	ret = 0;
	
	int16_t	Angle;
	int16_t	Move;
	ST_RAS_INFO	stRasInfo;
	ST_ROAD_INFO	stRoadInfo;
	ST_CARDATA	stMyCar = {0};
	static int16_t	BG_x = X_OFFSET;
	
	GetRasterInfo(&stRasInfo);
	GetRoadInfo(&stRoadInfo);
	Angle = stRoadInfo.angle;

	GetMyCar(&stMyCar);			/* 自車の情報を取得 */
	
	if(stMyCar.VehicleSpeed != 0u)
	{
		if(Angle != 0)
		{
			BG_x = Angle;
		}
		else
		{
			/* 保持 */
		}
	}
	BG_x = BG_x & 0x3FF;
	
	/* 背景を動かす */
	Move = 65 + 8 - stRasInfo.st;	/* 背景の高さ + オフセット - 水平位置 */
	if(Move < 0)
	{
		Move = Move << 2;
	}
	G_Scroll(BG_x, Y_MIN_DRAW +        0 + Move, 1);	/* 背景スクロール */
	
	return ret;
}


#endif	/* COURSE_OBJ_C */

