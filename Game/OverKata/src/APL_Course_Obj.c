#ifndef	APL_COURSE_OBJ_C
#define	APL_COURSE_OBJ_C

#include <stdio.h>
#include <limits.h>

#include <usr_macro.h>
#include "APL_Course_Obj.h"
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
#include "APL_Graphic.h"
#include "APL_MyCar.h"
#include "APL_Text.h"
#include "APL_Raster.h"

//const int16_t	g_nObjViewRateTbl[COURSE_OBJ_PAT_MAX] = { 198, 158, 126, 101, 79, 65, 51, 41, 33 };	/* X軸基準の場合 */
//const int16_t	g_nObjViewRateTbl[COURSE_OBJ_PAT_MAX] = { 99, 79, 63, 50, 39, 32, 25, 20, 16 };
//														{96, 77, 61, 49, 39, 31, 25, 20, 16, 13, 10};
const int16_t	g_nObjViewRateTbl[COURSE_OBJ_PAT_MAX] = { 99, 96, 93, 90, 88, 85, 82, 80, 77, 74, 72, 69, 66, 64, 61, 58, 56, 53, 50, 48, 45, 42, 40, 37, 34, 32, 29, 26, 24, 21, 18, 16 };

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
int16_t	Set_Course_TmpObject(uint16_t, uint8_t);

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
		g_stCourse_Obj[i].ubPos = i % 2;
		g_stCourse_Obj[i].y = Mmul4096(i * i);
		if(g_stCourse_Obj[i].ubPos == TRUE)
		{
			g_stCourse_Obj[i].x = ROAD_CT_POINT - Mmul4(g_stCourse_Obj[i].y);
		}
		else
		{
			g_stCourse_Obj[i].x = ROAD_CT_POINT + Mmul4(g_stCourse_Obj[i].y);
		}
		g_stCourse_Obj[i].z = Ms15b_To_s5b(SHRT_MAX - Mmul2(i));
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
#ifdef DEBUG	/* デバッグコーナー */
	{
//		SetDebugHis(uTimeDiff);
	}
#endif
	
	if(g_stCourse_Obj[bNum].ubAlive == TRUE)
	{
		int16_t	i;
		int16_t	y_ofst, y_inv;
		int16_t	x, y, z;
		int16_t	dx, dy, dz;
		int16_t	mx, my;
		uint16_t	ras_x, ras_y, ras_pat, ras_num;
		uint8_t	bEven;
		int16_t	Out_Of_Disp = 0;
		int16_t	speed = 0;
		uint8_t	ubType;

		ST_ROAD_INFO	stRoadInfo;
		ST_RAS_INFO		stRasInfo;
		ST_CARDATA		stMyCar;
		
		GetRoadInfo(&stRoadInfo);
		GetRasterInfo(&stRasInfo);
		GetMyCar(&stMyCar);
		GetMyCarSpeed(&speed);		/* 車速を得る */

		if(g_stCourse_Obj[bNum].ubPos == TRUE)	/* 左 */
		{
			bEven = TRUE;	/* 左 */
		}
		else
		{
			bEven = FALSE;	/* 右 */
		}

		/* 前回値 */
		x = g_stCourse_Obj[bNum].x;
		y = g_stCourse_Obj[bNum].y;
		z = g_stCourse_Obj[bNum].z;
		ubType = g_stCourse_Obj[bNum].ubType;

//		y += stMyCar.VehicleSpeed;	/* 変化量 */
		if((speed == 0) || (uTimeDiff == 0) )
		{
			/* 変化なし */
		}
		else
		{
			uint16_t	ras_x_o, ras_y_o, ras_pat_o;

			y_ofst = 0;
			/* 車幅(片側)は、4mと定義する。 */
			/* 4mで192dotであれば、1dotあたり0.021mと定義する。 */
			/* 画面奥が1dotの道幅しかないので画面奥は192m先にとなる。 */
			ras_num = Mmin( Mdiv256(Mdec(SHRT_MAX, y)), stRasInfo.ed );	/* ラスター情報の配列番号を算出 */
			if(ras_num == 0)
			{
				y_ofst++;
			}
			else
			{
				for(i = ras_num; i < stRasInfo.ed; i++)
				{
					y_ofst = Minc(y_ofst, 4);
					GetRasterIntPos(&ras_x_o, &ras_y_o, &ras_pat_o, i - 1, FALSE);	/* 配列番号のラスター情報取得 */
					GetRasterIntPos(&ras_x, &ras_y, &ras_pat, i, FALSE);	/* 配列番号のラスター情報取得 */
					if(ras_pat_o != ras_pat)	/* 境界で設定する */
					{
						break;
					}
				}
			}
			y = Mdec(y, Mmul256(y_ofst));
		}

		if(y > 0)
		{
			y_inv = SHRT_MAX - y;	/* 逆数 */

			z = Ms15b_To_s5b(Mmin(y_inv + 21845, SHRT_MAX));

			my = Mdiv256(y_inv);

			ras_num = Mmin( my, stRasInfo.ed );	/* ラスター情報の配列番号を算出 */
			ret = GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num, FALSE);	/* 配列番号のラスター情報取得 */

			/* センター */
	//		mx = x + (int16_t)Mu9b_To_u8b(ras_x);	/* 0-1023 => (-512 < 0 < 512) */
			if(ras_x < 256)
			{
				mx = x - ras_x;// + Mu9b_To_u8b((int16_t)ras_x);
			}
			else
			{
				mx = x - (ras_x - 512);// + Mu9b_To_u8b((int16_t)ras_x);
			}

			dy = my + stRasInfo.st;

			/* 差分 */
			if(bEven == TRUE)	/* 左 */
			{
				dx = mx - Mmul4(my);
			}
			else				/* 右 */
			{
				dx = mx + Mmul4(my);
			}
			/* 透視投影率＝焦点距離／（焦点距離＋Z位置）を２５６倍して６４で割った(/4pat) */
			//dz = Mmin( Mmax( 3 - (((z<<8) / (z + ROAD_ED_POINT))>>5) , 0), 3 );
			/* 透視投影率＝焦点距離／（焦点距離＋Z位置）を２５６倍して16で割った(=7pat) */
			//dz = Mmin( Mmax( 7 - (Mdiv16(Mmul256(z) / (z + ROAD_ED_POINT))) , 0), 7);
			/* 透視投影率＝焦点距離／（焦点距離＋Z位置）を256倍して16で割った pat 0-10 */
			//g_nObjViewRateTbl[z]
			dz = z;
			/* 描画 */
			Out_Of_Disp = Put_CouseObject(	dx,
											dy,
											dz,
											bMode_rev,
											ubType,
											bEven);

	#ifdef DEBUG	/* デバッグコーナー */
			{
				ST_CRT	stCRT;
				GetCRT(&stCRT, bMode);
				Draw_Line(	stCRT.hide_offset_x + dx,
							stCRT.hide_offset_y + dy,
							stCRT.hide_offset_x + dx + dz, 
							stCRT.hide_offset_y + dy,
							10,
							0xFFFF);
			}
	#endif
		}
		else	/* 追い抜かした */
		{
//			printf("Out(%d,%d,%d)=(%d,0x%x)\n", dx, dy, dz, bMode_rev, Out_Of_Disp );
//			KeyHitESC();	/* デバッグ用 */
			g_stCourse_Obj[bNum].ubAlive = FALSE;
		}

		g_stCourse_Obj[bNum].x = x;
		g_stCourse_Obj[bNum].y = y;
		g_stCourse_Obj[bNum].z = z;
	}
	else
	{
		g_stCourse_Obj[bNum].x = ROAD_CT_POINT;
		g_stCourse_Obj[bNum].y = SHRT_MAX;
		g_stCourse_Obj[bNum].z = COURSE_OBJ_PAT_MAX - 1;
		g_stCourse_Obj[bNum].ubType++;
		if(g_stCourse_Obj[bNum].ubType >= COURSE_OBJ_TYP_MAX)
		{
			g_stCourse_Obj[bNum].ubType = 0u;
		}
		g_stCourse_Obj[bNum].ubAlive = TRUE;
		
#ifdef DEBUG	/* デバッグコーナー */
//		if((bEven == TRUE) && (bNum == 0) && (bMode_rev == 2))
		{
//			SetDebugHis(uTimeDiff);
		}
#endif
	}
#ifdef DEBUG	/* デバッグコーナー */
	{
//		SetDebugHis(g_stCourse_Obj[bNum].ubAlive);
	}
#endif
	
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

	int16_t	sp_x, sp_y;
	int16_t PatNumber;
	uint16_t	*pSrcBuf = NULL;
	uint32_t	uWidth=0, uHeight=0;
	uint8_t		ubPos_H;
	BITMAPINFOHEADER *pInfo;
	ST_PCG	*p_stPCG = NULL;
	ST_CRT			stCRT;

	GetCRT(&stCRT, ubMode);
	
	if(ubType >= COURSE_OBJ_TYP_MAX)return -1;
	if(Size >= COURSE_OBJ_PAT_MAX)return -1;
	
	PatNumber = COURSE_OBJ_CG + ubType;
	
	pSrcBuf = g_stPicCourse_ObjImage[ubType][Size].pImageData;
	pInfo 	= g_stPicCourse_ObjImage[ubType][Size].pBMi;

	uWidth	= pInfo->biWidth;
	uHeight	= pInfo->biHeight;
	
	if(ubPos == TRUE)	/* 左 */
	{
		ubPos_H = POS_RIGHT;
		sp_x = x + SP_X_OFFSET - Mdiv2(uWidth) - 24;
	}
	else				/* 右 */
	{
		ubPos_H = POS_LEFT;
		sp_x = x + SP_X_OFFSET + Mdiv2(uWidth) - 24;
	}
	sp_y = y;
	
	ret = G_BitBlt_From_Mem(x, y, 0, pSrcBuf, uWidth, uHeight, ubMode, ubPos_H, POS_BOTTOM, PatNumber);

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
				count++;
			}
		}
		if(count == 0)	/* ソート完了 */
		{
			break;
		}
		else{			/* 再ソート */
			count = 0;
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

	ret = G_Load_Mem( PatNumber, 0, 0, 0 );	/* ヤシの木 */
	if(ret != 0)return ret;
	
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
			uWidth_dst = Mmul_0p10(uW_tmp);
			uWidth_dst = Mmax(uWidth_dst, 8);

			/* 縮小先のサイズ(H) */
			uH_tmp = uHeight_dst << 3;
			uHeight_dst = Mmul_0p10(uH_tmp);
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
	
	if(bCourseNum == 0u)
	{
		/* デバッグモード */
	}
	else
	{
		/* コースデータ読み込み */
		p_stRoadData = (ST_ROADDATA *)GetRoadDataAddr(0);
		sprintf(str, "data/map/course%02d.csv", bCourseNum);
		File_Load_Course_CSV(str, p_stRoadData, &x, &y);
	}

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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Set_Course_TmpObject(uint16_t uCount, uint8_t ubPictNum)
{
	int16_t	ret = 0;

	uint8_t	ubMode, ubModeRev;
	int8_t	bZoom;
	int16_t	i, y_ofst;
	int16_t	x, y;
	uint16_t	ras_x, ras_y, ras_pat, ras_num;
	uint16_t	*pSrcBuf = NULL;
	uint32_t	uWidth, uHeight, uFileSize;
	ST_CRT			stCRT;
	ST_RAS_INFO		stRasInfo;

#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	GetRasterInfo(&stRasInfo);
	GetGameMode(&ubMode);	/* 現在のモード取得 */
	GetCRT(&stCRT, ubMode);	/* 現在の画面座標を取得 */

	y = uCount;
	
	ras_num = Mmax(Mmin(uCount, stRasInfo.ed), 2);		/* ラスター情報の配列番号を算出 */
	
	if(ras_num > 2)
	{
		y_ofst = 0;
		for(i = ras_num; i < stRasInfo.ed; i++)
		{
			uint16_t	ras_x_o, ras_y_o, ras_pat_o;
			ret = GetRasterIntPos(&ras_x_o, &ras_y_o, &ras_pat_o, i - 1, FALSE);	/* 配列番号のラスター情報取得 */
			ret = GetRasterIntPos(&ras_x, &ras_y, &ras_pat, i, FALSE);	/* 配列番号のラスター情報取得 */
			if((ras_pat_o != 0) && (ras_pat == 0))	/* 最初の0の境界で設定する */
			{
				break;
			}
			y_ofst++;
		}
	}
	else
	{
		ret = GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num, FALSE);	/* 配列番号のラスター情報取得 */
		y_ofst = 0;
	}
	y += y_ofst;
	
	ret = GetRasterIntPos(&ras_x, &ras_y, &ras_pat, ras_num, FALSE);	/* 配列番号のラスター情報取得 */
	
	//x = (int16_t)ROAD_CT_POINT + Mu9b_To_u8b((int16_t)ras_x);
	if(ras_x < 256)
	{
		x = (int16_t)ROAD_CT_POINT + ras_x;// + Mu9b_To_u8b((int16_t)ras_x);
	}
	else
	{
		x = (int16_t)ROAD_CT_POINT + ras_x - 512;// + Mu9b_To_u8b((int16_t)ras_x);
	}
	y = stRasInfo.st + y;
	
	bZoom = (int8_t)uCount - 9;

	/* 描画対象をOFFスクリーンへ */
	if(ubMode == 1)
	{
		ubModeRev = 2;
	}
	else
	{
		ubModeRev = 1;
	}

	pSrcBuf = Get_PicImageInfo( ubPictNum, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	G_Stretch_Pict_toVRAM(x, y, 0, ubModeRev, POS_MID, POS_CENTER, pSrcBuf, uWidth, uHeight, bZoom, ubPictNum);

#ifdef DEBUG
//	printf("H(%d)=(%d, %d, %d)(%d, %d)(%d, %d)\n", ubMode, uCount, ubPictNum, bZoom, x, y, uWidth, uHeight);
//	KeyHitESC();	/* デバッグ用 */
#endif

	return ret;
}

#endif	/* APL_COURSE_OBJ_C */

