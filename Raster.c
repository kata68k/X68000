#ifndef	RASTER_C
#define	RASTER_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>

#include "inc/usr_macro.h"
#include "OverKata.h"

#include "Raster.h"

#include "APL_Math.h"
#include "Course_Obj.h"
#include "CRTC.h"
#include "Draw.h"
#include "EnemyCAR.h"
#include "Graphic.h"
#include "MFP.h"
#include "MyCar.h"
#include "Output_Text.h"
#include "PCG.h"
#include "Score.h"
#include "Text.h"

#define MAPCOL_MAX	(5)

/* 変数 */
static uint16_t	g_uRoadAnime = 0u;
static uint16_t	g_uRoadAnime_old = 0u;
static uint16_t	g_uRoadAnimeBase = 0u;
static uint16_t	g_uRoadAnimeBase_old = 0u;
static uint16_t	g_uRoadAnimeCount = 0u;

static uint16_t	g_uRoadCycleCount = 0u;
static uint16_t	g_uCounter = 0u;
static uint8_t	g_bRoadInitFlag = TRUE;

static uint16_t	g_uMapCounter = 0u;

static int16_t	PositionSum = 0;
static int16_t	ViewAngle = 0;

static int16_t	g_RoadArray[RASTER_H_MAX] = {0u};

/* 構造体定義 */
ST_RASTER_INT g_stRasterInt[RASTER_H_MAX] = {0};
static ST_RAS_INFO	g_stRasInfo = {0};

ST_ROAD_INFO	g_stRoadInfo = {0};
static ST_ROADDATA	g_stRoadData[ROADDATA_MAX] = {0};

/* 関数のプロトタイプ宣言 */
int16_t	GetRasterInfo(ST_RAS_INFO *);
int16_t	SetRasterInfo(ST_RAS_INFO);
void	Raster_Init(void);
int16_t	Raster_Main(void);
static uint8_t Raster_Calc_H(int16_t *, int16_t, int16_t);
static int16_t Raster_SetCarLinePos(int16_t *, int16_t, int16_t);
static int16_t Raster_SetDriverView(int16_t *, int16_t, int16_t);
static int16_t Raster_SetBndngCnd(int16_t *, int16_t, int16_t);
static uint8_t Raster_Calc_V(int16_t *, int16_t, int16_t);

int16_t	GetRasterPos(uint16_t *, uint16_t *, uint16_t);	/* ラスター処理結果を取得 */

int16_t	GetRoadInfo(ST_ROAD_INFO *);
int16_t	SetRoadInfo(ST_ROAD_INFO);
void Road_Init(uint16_t);
void Road_BG_Init(uint16_t);
static void Get_Road_Pat(uint16_t *, uint16_t);
static void Set_Road_Pat_offset(int16_t);
static int16_t Road_Pat_Update(uint16_t);
static void Road_Pat_Base_Update(void);
static void Road_Pat_Start(void);
static void Road_Pat_Stop(void);
static int16_t	Set_Road_Height(void);
static int16_t	Set_Road_Slope(void);
static int16_t	Set_Road_Angle(void);
static int16_t	Set_Road_Distance(void);
static int16_t	Set_Road_Object(void);
static int16_t	Road_Pat_Main(void);
int16_t	GetRoadCycleCount(uint16_t *);
uint64_t	GetRoadDataAddr(void);
int16_t	SetHorizon(uint8_t);
static int16_t Load_Road_Background(int16_t);
static int16_t Set_Road_Background_Col(int16_t);
int16_t Road_Map_Draw(uint8_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t GetRasterInfo(ST_RAS_INFO *stDat)
{
	int16_t	ret = 0;
	
	*stDat = g_stRasInfo;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t SetRasterInfo(ST_RAS_INFO stDat)
{
	int16_t	ret = 0;
	
	g_stRasInfo = stDat;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Raster_Init(void)
{
	memset(&g_stRasInfo, 0, sizeof(ST_RAS_INFO));
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Raster_Main(void)
{
	int16_t	ret = 0;
	
	uint16_t	i;
	uint16_t	uRas_y;
	uint16_t	uRas_st, uRas_mid, uRas_ed, uRas_size;

	int16_t	RoadAngle = 0;
	int16_t	CarAngle = 0;
	int16_t	Diff = 0;
	uint16_t	uRev_uRas_y;
	uint16_t	ras_pat=0;
	int16_t		ras_cal_x=0;
	int16_t		ras_cal_y=0;

	uint8_t bMode;
	static uint8_t	bMode_old = 0xFF;
	static uint8_t	ubSide = 0;
	
	ST_RASTER_INT	stRasterInt;
	ST_CARDATA 		stMyCar = {0};

#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	GetGameMode(&bMode);

	if(bMode == bMode_old)
	{
		return ret;	/* 画面着替えが発生しなかった場合は、処理を行わない */
	}
	ret = 1;
	
	/* 一つのバッファに偶数/奇数で２枚確保する */
	if(bMode == 1u)		/* 上側判定 */
	{
		ubSide = 1;
	}
	else if(bMode == 2u)	/* 下側判定 */
	{
		ubSide = 0;
	}
	else					/* その他 */
	{
		ubSide = 1;
	}
	
	/* 自車の情報を取得 */
	GetMyCar(&stMyCar);
	CarAngle = stMyCar.Angle;	/* アングル */
	
	/* ロードパターン */
	ret = Road_Pat_Main();

	if(ret != 0)	/* コースデータの更新あり */
	{
		switch(ret)
		{
		case 1:
			;	/* コース情報更新 */
			break;
		case 2:
			return ret;	/* コースEND */
		default:
			break;
		}
	}
	RoadAngle = g_stRoadInfo.angle;	/* 道の角度 */
	
	/* 自車の位置を算出 */
	Diff = APL_AngleDiff(RoadAngle, CarAngle);	/* 車と道路の角度差分で車の位置が変わる */
	ViewAngle = - Mdiv4(Diff);	/* -128 ot 0 to 128 */
	if(Mabs(Diff) < 8)
	{
		PositionSum = Mdiv2(PositionSum);	/* 2分法 */
		if(PositionSum == -1)
		{
			PositionSum = 0;
		}
	}
	else
	{
		PositionSum += Mdiv8(Diff);
	}
	PositionSum = Mmax(Mmin((PositionSum), 128), -128);
	
	/* 初期化 */
	uRas_st = 0;
	uRas_mid = 0;
	uRas_ed = 0;
	uRas_size = 0xFFFF;

	/* ラスター位置 */
	g_stRasInfo.st		= SetHorizon(bMode);			/* ラスター開始位置(0-128) *//* 水平線の設定 */
	g_stRasInfo.mid		= g_stRoadInfo.Horizon_Base;	/* ラスター固定位置(64) */
	g_stRasInfo.ed		= Y_MAX_WINDOW;					/* ラスター終了位置(160) */
	g_stRasInfo.size	= g_stRasInfo.ed - Mmin(g_stRasInfo.st, g_stRasInfo.mid);	/* ラスターの範囲 */
	SetRasterInfo(g_stRasInfo);
	
	uRas_st		= g_stRasInfo.st;	/* 開始位置 */
	uRas_mid	= g_stRasInfo.mid;	/* 中間位置 */
	uRas_ed		= g_stRasInfo.ed;	/* 終了位置 */
	uRas_size	= g_stRasInfo.size;	/* ラスターの範囲 */
	
	Road_Pat_Start();	/* 初回は道のパレットの開始設定を行う */
	
	for(i=0; i<RASTER_H_MAX; i++)
	{
		if(i+1 == RASTER_H_MAX)
		{
			break;
		}
		g_RoadArray[i+1] = g_RoadArray[i];
	}
	g_RoadArray[0] = g_stRoadInfo.slope;
	
	for(uRas_y=0; uRas_y < uRas_size; uRas_y++ )
	{
		ras_pat=0;
		ras_cal_x=0;
		ras_cal_y=0;
		
		uRev_uRas_y	= uRas_size - uRas_y;	/* uRas_size -> 0 */
		
		/* X座標 */
		Raster_Calc_H( &ras_cal_x, uRas_y, uRev_uRas_y );
		
		/* Y座標 */
		Raster_Calc_V( &ras_cal_y, uRas_y, uRev_uRas_y );

		/* ロードパターン */
		Get_Road_Pat(&ras_pat, uRas_y);		/* ロードのパターン情報を取得 */
		Set_Road_Pat_offset(uRas_y);		/* ロードパターン閾値の更新 */

		stRasterInt.x = (uint16_t)(ras_cal_x & 0x1FFu);
		stRasterInt.y = (uint16_t)(ras_cal_y & 0x1FFu);
		stRasterInt.pat = ras_pat;

		g_stRasterInt[Mmul2(uRas_y) + ubSide] = stRasterInt;
	}
	
	/* BGのY座標を選ぶラスタの割り込み位置に対して表示したいBGのY座標から */
	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
	if(g_uCRT_Tmg != 0)	/* ラスタ２度読みの場合(31kHz) */
	{
		g_stRasterInt[(RASTER_H_MAX - 2) + ubSide].y = Mmul2(uRas_st);	/* 割り込み位置の設定 */
	}
	else	/* ノンインターレースモードの場合(15kHz) */
	{
		g_stRasterInt[(RASTER_H_MAX - 2) + ubSide].y = uRas_st;	/* 割り込み位置の設定 */
	}
	g_stRasterInt[(RASTER_H_MAX - 4) + ubSide].y = ROAD_ST_POINT - uRas_st;	/* 割り込みがかかるまでの表示位置(ラスタ割り込み位置によるずれの考慮は不要)	*/
	
	SetRasterIntData(ubSide);	/* ラスター割り込みデータ設定完了 */
	
	bMode_old = bMode;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static uint8_t Raster_Calc_H(int16_t *x, int16_t Num, int16_t RevNum)
{
	uint8_t bRet = 0;
	int16_t		Scroll_ofst = 0;
	int16_t		Scroll_w_clp=0;
	int16_t		Ras_x_offset = 0;
	uint16_t	uRas_y_p;
	uint16_t	uRas_size;

	uRas_y_p	= Num + 1;			/* Devide zero */
	uRas_size	= Num + RevNum;
	
	/* 車線ポジション */
	Raster_SetCarLinePos(&Scroll_ofst, uRas_y_p, uRas_size);

	/* 車が向いている方向 */
	Raster_SetDriverView(&Scroll_w_clp, RevNum, uRas_size);
	
	/* 道路の曲がり具合 */
	Raster_SetBndngCnd(&Ras_x_offset, uRas_y_p, uRas_size);
	
	*x = Scroll_ofst + Scroll_w_clp + Ras_x_offset;
	
	return bRet;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Raster_SetCarLinePos(int16_t *x, int16_t Count, int16_t Size)
{
	int16_t ret = 0;

	/* 車線ポジション */
	*x = PositionSum;	/* 車体の移動(高い位置ほど移動量が少ない) */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Raster_SetDriverView(int16_t *x, int16_t Count, int16_t Size)
{
	int16_t ret = 0;

	if(ViewAngle == 0)
	{
	}
	else
	{
		/* =直線の道路に対して ViewAngle=0で正面、127=左向き、-128=右向き */
		*x = (int16_t)APL_sDiv((int32_t)ViewAngle * Count, (int32_t)Size);	/* ワイパー的な線 */
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
static int16_t Raster_SetBndngCnd(int16_t *x, int16_t Count, int16_t Size)
{
	int16_t ret = 0;
	
	*x = APL_sDiv(-g_RoadArray[Count], Count);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static uint8_t Raster_Calc_V(int16_t *y, int16_t Num, int16_t RevNum)
{
	uint8_t bRet = 0;
	int16_t RoadPoint_y = 0;
	int16_t	Point_y, Point_y_def;
	int16_t	MinClip, MaxClip;
	int16_t	RasStart;
	int16_t	Road_strch = 0;
	int16_t	Size;
	int16_t	Offset = 0;

	if(	(g_stRasInfo.mid == g_stRoadInfo.Horizon)	/* 水平線の位置が通常より下側の処理 */
		|| (g_CpuTime < 300) )						/* 低速MPU */
	{
		*y = ROAD_ST_POINT - g_stRasInfo.mid;	/* 標準位置 */
		return bRet;
	}
	
	/* Y座標 */
	RasStart = Mmin(g_stRasInfo.st, g_stRasInfo.mid);
	MinClip = ROAD_ST_POINT - (RasStart + Num);	/* BGの道（先頭） */
	MaxClip = (ROAD_ST_POINT + 16) - (RasStart + Num);	/* BGの道（先頭） */
//	MaxClip = ROAD_ED_POINT - (RasStart + Num);	/* BGの道（最後尾） */
	Point_y_def = ROAD_ST_POINT - g_stRasInfo.mid;	/* 標準位置 */
#if 1
	if( g_stRasInfo.st <  g_stRasInfo.mid)	/* 水平線の位置が通常より上側の処理 */
	{
		Size = g_stRasInfo.mid - g_stRasInfo.st;
		Road_strch	= (-1 * Size * Num) / Mdiv2(Y_MAX_WINDOW);
		Offset = Mdiv8(APL_Sin(Num));
		Road_strch	+= Size - Offset;

		bRet = 0xFF;
	}
	else	/* 水平線の位置が通常より下側の処理 */
	{
//		Size = g_stRasInfo.st - g_stRasInfo.mid;
//		Road_strch	= (-1 * Size * Num) / Mdiv2(Y_MAX_WINDOW);
//		Offset = Mdiv8(APL_Sin(2*Num));
//		Road_strch	-= Size + Offset;
		Size = (g_stRasInfo.size + (g_stRasInfo.st - g_stRasInfo.mid));
		if(Size != 0)
		{
			Road_strch	= (g_stRasInfo.size / Size) * Num;
		}
		else
		{
			Road_strch	= 0;
		}
		bRet = 0xb7;
	}
	
	RoadPoint_y	= Point_y_def + Road_strch;
	Point_y = RoadPoint_y;
//	Point_y = Mmax(Mmin(RoadPoint_y, MaxClip), MinClip);	/* 上下限クリップ */
	if(RoadPoint_y == MaxClip)
	{
		bRet = TRUE;	/* MaxClip */
	}
#else
	Road_strch = Mdiv1024( (int16_t)g_stRasInfo.size * APL_Cos(2*(Num+90)) );
//	Road_strch = Num - 64;	/* 線形な登り坂 */
//	Road_strch = (Num * Num)/200 - 64;	/* 二次曲線 */
	RoadPoint_y = Mmin((Point_y_def + Road_strch), 96);
	Point_y = RoadPoint_y;	/* 上下限クリップ */
#endif
	*y = Point_y & 0x1FFu;	/* 9bitマスク */
	
	return bRet;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t GetRoadInfo(ST_ROAD_INFO *stDat)
{
	int16_t	ret = 0;
	
	*stDat = g_stRoadInfo;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t SetRoadInfo(ST_ROAD_INFO stDat)
{
	int16_t	ret = 0;
	
	g_stRoadInfo = stDat;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Road_Init(uint16_t uCourseNum)
{
	uint32_t	i;
	
	if(uCourseNum == 0u)uCourseNum = 1u;
	
	g_uRoadAnimeBase = 3;
	g_uRoadAnime = 0;
	g_bRoadInitFlag = TRUE;
	Road_Pat_Start();
	
	g_uCounter = 0u;
	g_uRoadCycleCount = 0u;

	g_stRoadInfo.Horizon		= 0x00;	/* 水平位置 */
	g_stRoadInfo.Horizon_Base	= 0x00;	/* 仮水平位置 */
	g_stRoadInfo.offset_x		= 0x00;	/*  */
	g_stRoadInfo.offset_y		= 0x00;	/*  */
	g_stRoadInfo.offset_val		= 0x00;	/*  */
	g_stRoadInfo.slope			= 0x00;	/*  */
	g_stRoadInfo.angle			= 0x00;	/*  */
	g_stRoadInfo.distance		= 0x00;	/*  */
	g_stRoadInfo.object			= 0x00;	/*  */
	
	for(i = 0u; i < ROADDATA_MAX; i++)
	{
		g_stRoadData[i].bHeight		= 0x80;	/* 道の標高	(0x80センター) */
		g_stRoadData[i].bWidth		= 0x80;	/* 道の幅	(0x80センター) */
		g_stRoadData[i].bAngle		= 0x80;	/* 道の角度	(0x80センター) */
		g_stRoadData[i].bfriction	= 0x80;	/* 道の摩擦	(0x80センター) */
		g_stRoadData[i].bPat		= 0x00;	/* 道の種類	 */
		g_stRoadData[i].bObject		= 0x00;	/* 出現ポイントのオブジェクトの種類 */
		g_stRoadData[i].bRepeatCount= 0x00;	/* 繰り返し回数 */
	}
	
	/* コースデータの読み込み */
	g_stRoadInfo.Courselength = Load_Course_Data(uCourseNum);	/* コースの全長 */

	Set_Road_Angle();		/* コースの曲がり具合 */
	Set_Road_Height();		/* コースの標高 */
	Set_Road_Slope();		/* コースの現在地から標高までの傾き */
	Set_Road_Distance();	/* 水平線から現在地までの距離 */
	Set_Road_Object();		/* ライバル車の出現 */
	
	/* 水平線の設定 */
	SetHorizon(1);
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Road_BG_Init(uint16_t uCourseNum)
{
	uint32_t	i;
	
	if(uCourseNum == 0u)uCourseNum = 1u;

	Load_Road_Background(0);	/* コース背景の展開 */
	
	/* コース障害物の展開 */
	for(i = 0u; i < COURSE_OBJ_TYP_MAX; i++)
	{
		Load_Course_Obj(i);		/* コース障害物読み込みの展開 */
	}
	
	/* 道路のパレットデータ */
	g_uMapCounter = 0;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Get_Road_Pat(uint16_t *p_uPat, uint16_t uNum)
{
#if 1
	uint16_t	uBG_pat[4][4] = {288,   0,  96, 192, 
		 						 192, 288,   0,  96, 
		 						  96, 192, 288,   0, 
		 						   0,  96, 192, 288} ;
	if(uNum == 0)
	{
		*p_uPat = uBG_pat[3][0];
	}
	else
	{
		*p_uPat = uBG_pat[g_uRoadAnimeBase][g_uRoadAnime];
	}
#else
	*p_uPat = 0;
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Set_Road_Pat_offset(int16_t Num)
{
	if( (Num == 0) || (g_stRoadInfo.offset_val == 0) )
	{
		if(g_stRasInfo.st <= g_stRasInfo.mid)
		{
			g_stRoadInfo.offset_val = 0;
			g_stRoadInfo.offset_y = 0;
		}
		else
		{
			g_stRoadInfo.offset_val = (g_stRasInfo.st - g_stRasInfo.mid);
			g_stRoadInfo.offset_y = 16;
		}
	}
	else
	{
		int16_t Size;
		if(g_stRasInfo.st <= g_stRasInfo.mid)
		{
			Size = Num;
		}
		else
		{
			Size = Num + (g_stRasInfo.st - g_stRasInfo.mid);
//			Size = g_stRasInfo.size + (g_stRasInfo.st - g_stRasInfo.mid);
		}
		//g_stRoadInfo.offset_y = (Num * g_stRoadInfo.offset_val) / Mmax((g_stRoadInfo.offset_val + 96), 1);
		g_stRoadInfo.offset_y = APL_uDiv((Size * g_stRoadInfo.offset_val), (g_stRoadInfo.offset_val + g_stRasInfo.size ));
	}
	if(Road_Pat_Update(g_stRoadInfo.offset_y) != 0u)
	{
		g_stRoadInfo.offset_val++;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Road_Pat_Update(uint16_t uThreshold)
{
	int16_t	ret = 0;
	
	g_uRoadAnimeCount++;
	if(g_uRoadAnimeCount >= uThreshold)
	{
		g_uRoadAnimeCount = 0;
		g_uRoadAnime++;
		ret = 1;
		
	}
	
	if(g_uRoadAnime >= 4u)
	{
		g_uRoadAnime = 0u;
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
static void Road_Pat_Base_Update(void)
{
	g_uRoadAnimeCount = 0xFFF;	/* 逆走アニメーション防止 */

	g_uRoadAnime = 0;

	g_uRoadAnimeBase++;
	if(g_uRoadAnimeBase >= 4)
	{	
		g_uRoadAnimeBase = 0;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Road_Pat_Start(void)
{
	g_uRoadAnime_old = g_uRoadAnime;
	g_uRoadAnimeBase_old = g_uRoadAnimeBase;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void Road_Pat_Stop(void)
{
	g_uRoadAnime = g_uRoadAnime_old;
	g_uRoadAnimeBase = g_uRoadAnimeBase_old;
	g_uRoadAnimeCount = 0;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	Set_Road_Height(void)
{
	int16_t	ret = 0;
	int16_t	road_height;
	static int16_t	road_height_old;
	
#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	road_height_old = g_stRoadInfo.height;	/* 前回値更新 */
		
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)
	{
		if(uDebugNum <= 0x80)
		{
			/* コースデータ読み込み */
			road_height = ((int16_t)g_stRoadData[g_uCounter].bHeight) - 0x80;	/* 道の標高	(0x80センター) */
		}
		else
		{
			road_height = g_stRoadInfo.height;
		}
	}
	else
#endif
	{
		/* コースデータ読み込み */
		road_height = ((int16_t)g_stRoadData[g_uCounter].bHeight) - 0x80;	/* 道の標高	(0x80センター) */
	}
	road_height = Mmax(Mmin(road_height, 127), -128);					/* +高い：-低い */

	g_stRoadInfo.height = road_height;		/* 更新 */

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	Set_Road_Slope(void)
{
	int16_t	ret = 0;
	static int8_t bSlope_Init = TRUE;
	static int8_t bAngleFlag = TRUE;
	static int16_t Angle_old = 0;
	int16_t AngleDiff = 0;
	int16_t	Angle;
	int16_t	Slope;
	int16_t	x, y, mv;
	
	ST_PCG	*p_stPCG = NULL;
	p_stPCG = PCG_Get_Info(ROAD_PCG_ARROW);	/* 矢印 */
	
	Angle = g_stRoadInfo.angle;
	Slope = g_stRoadInfo.slope;
	if(bSlope_Init == TRUE)
	{
		bSlope_Init = FALSE;
		Angle_old = Angle;

		x = 120 + SP_X_OFFSET;
		y = g_stRasInfo.mid + SP_Y_OFFSET;
		p_stPCG->x = x;
		p_stPCG->y = y;
	}
	
	if(Angle == Angle_old)		/* stop */
	{
		Slope = Mdiv2(Slope);	/* 2分法 */
		if(Slope == -1)	/* 算術シフトのマイナス側は０に収束しない対策 */
		{
			Slope = 0;
		}
	}
	else
	{
		AngleDiff = APL_AngleDiff(Angle_old, Angle);
	}
	
	Slope += AngleDiff;
	Slope = Mminmax(Slope, -384, 384);

	if(p_stPCG != NULL)
	{
		x = p_stPCG->x;
		y = p_stPCG->y;
		p_stPCG->dy = 0;
		
		if( (x < 80) || (x > 176) || (bAngleFlag == TRUE))
		{
			x = 120 + SP_X_OFFSET;
		}
		
		if(AngleDiff == 0)
		{
			x = 120 + SP_X_OFFSET;
			y = g_stRasInfo.mid + SP_Y_OFFSET;
			p_stPCG->dx = 0;

			p_stPCG->update	= FALSE;

			if(bAngleFlag == TRUE)
			{
				bAngleFlag = FALSE;
			}
			else
			{
				bAngleFlag = TRUE;
			}
		}
		else
		{
			mv = AngleDiff;
			
			if(AngleDiff < 0)
			{
				if(bAngleFlag == TRUE)
				{
					x = 120 + SP_X_OFFSET;
				}
				bAngleFlag = FALSE;
				
				if(mv == 0)
				{
					mv = 1;
				}
				p_stPCG->pPatCodeTbl[0] &= PCG_CODE_MASK;	/* パターン以外をクリア */
				p_stPCG->pPatCodeTbl[0] |= SetBGcode2(0, 1, 0x07);
			}
			else
			{
				if(bAngleFlag == FALSE)
				{
					x = 120 + SP_X_OFFSET;
				}
				bAngleFlag = TRUE;
				
				if(mv == 0)
				{
					mv = -1;
				}
				p_stPCG->pPatCodeTbl[0] &= PCG_CODE_MASK;	/* パターン以外をクリア */
				p_stPCG->pPatCodeTbl[0] |= SetBGcode2(0, 0, 0x07);
			}
			p_stPCG->dx = mv;
			
			p_stPCG->update	= TRUE;
		}
		p_stPCG->x = x;
		p_stPCG->y = y;
	}
	
	g_stRoadInfo.slope = Slope;
	
	Angle_old = g_stRoadInfo.angle;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	Set_Road_Angle(void)
{
	int16_t	ret = 0;
	int16_t	road_angle_diff = 0;
	int16_t	road_angle = 0;
	static int16_t	road_angle_old = 0;

	uint16_t	uRoad_data = 0;
	static uint16_t	uRoad_data_old = 0x80;
	
#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	/* コーナー */
	road_angle_old = g_stRoadInfo.angle;
	
	
	/* 一次ローパスフィルタ */
	uRoad_data = Mdiv256(((192 * g_stRoadData[g_uCounter].bAngle) + ((255 - 192) * uRoad_data_old)));
	uRoad_data_old = uRoad_data;
	
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)
	{
		if(uDebugNum <= 0x80)
		{
			road_angle_diff = 0x80 - (int16_t)uRoad_data;		/* 道の角度 */
		}
		else
		{
			road_angle_diff = 0;
			road_angle_old = g_stRoadInfo.angle;
		}
	}
	else
#endif
	{
		road_angle_diff = 0x80 - (int16_t)uRoad_data;		/* 道の角度 */
	}
	road_angle = road_angle_old + road_angle_diff;
	
	if(road_angle >= 360)
	{
		road_angle -= 360;
	}
	else if(road_angle < 0)
	{
		road_angle += 360;
	}

	g_stRoadInfo.angle = road_angle;	/* 0-360度の角度 */
	
	ret = road_angle_diff;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static	int16_t	Set_Road_Distance(void)
{
	int16_t	ret = 0;
	int16_t	road_distance;
	static int16_t road_distance_old;
	
	road_distance_old = g_stRoadInfo.distance;	/* 前回値更新 */
	road_distance = 0;
	g_stRoadInfo.distance	= road_distance;	/*  */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	Set_Road_Object(void)
{
	int16_t	ret = 0;
	int16_t	road_object = 0;
	
	road_object = g_stRoadData[g_uCounter].bObject;	/* 出現ポイントのオブジェクトの種類 */
	if(road_object != 0u)
	{
		SetAlive_EnemyCAR();	/* ライバル車を出現させる */
	}
	g_stRoadInfo.object		= road_object;		/*  */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Road_Pat_Main(void)
{
	int16_t	ret = 0;
	static uint32_t	unRoadAnimeTime = 0xFFFFFFFFu;
	uint32_t	time_st = 0u;
	int16_t	speed = 0;

	/* 初期化時のみの動作 */
	if(g_bRoadInitFlag == TRUE)
	{
		speed = 1;
		time_st = 0xFFFFu;
		unRoadAnimeTime = 0u;
	}
	else
	{
		GetStartTime(&time_st);		/* 開始時間を取得 */
		GetMyCarSpeed(&speed);		/* 車速を得る */
	}
	
	if(speed == 0u)	/* 車速0km/h */
	{
		unRoadAnimeTime = time_st;	/* 時刻更新 */

		Road_Pat_Stop();	/* 道のアニメ停止 */
	}
	else if( (time_st - unRoadAnimeTime) < (362 / speed) )	/* 更新周期未満 */
	{
		/* センターラインの長さと間隔は５ｍ */
		/* 60fps=17ms/f */
		/* 最大速度307km/h(85m/s,0.0853m/ms) 1f=1.45m進む */
		/* 最低速度6km/h(1.7m/s,0.0017m/ms) 1f=0.028m進む */
		/* 1LSB 10km/h(2.8m/s,0.0028m/ms) 1f=0.047m進む */
		/* 1times Cal 1m=4dot (17ms/f x 1/0.047 / speed) = (362/speed)*/

		Road_Pat_Stop();	/* 道のアニメ停止 */
	}
	else
	{
		unRoadAnimeTime = time_st;	/* 時刻更新 */
		
		Road_Pat_Base_Update();	/* 道のアニメ更新 */

		Set_Road_Angle();		/* コースの曲がり具合 */
		Set_Road_Height();		/* コースの標高 */
		Set_Road_Slope();		/* コースの現在地から標高までの傾き */
		Set_Road_Distance();	/* 水平線から現在地までの距離 */
		
		Set_Road_Object();		/* ライバル車の出現 */
		
		S_Add_Score_Point(speed * 10);	/* 車速で加点 */
		
		g_uCounter++;	/* コースデータのカウンタ更新 */
		
		if(g_uCounter >= g_stRoadInfo.Courselength)
		{
			g_uCounter = 0u;
			
			/* 背景を変更 */
			g_uMapCounter++;
			if(g_uMapCounter >= MAPCOL_MAX)
			{
				g_uMapCounter = 0;
			}
			Set_Road_Background_Col(g_uMapCounter);

			ret = 2;
		}
		else
		{
			ret = 1;
		}

		g_uRoadCycleCount++;	/* コース更新フリーランカウンタ */

	}
	
	/* 初期化時のみの動作 */
	if(g_bRoadInitFlag == TRUE)
	{
		g_bRoadInitFlag = FALSE;
		GetStartTime(&time_st);		/* 開始時間を取得 */
		unRoadAnimeTime = time_st;	/* 時刻更新 */
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
int16_t	GetRoadCycleCount(uint16_t *p_uCount)
{
	int16_t	ret = 0;
	
	*p_uCount = g_uRoadCycleCount;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint64_t GetRoadDataAddr(void)
{
	return (uint64_t)&g_stRoadData[0];
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetHorizon(uint8_t bMode)
{
	int16_t	ret = 0;
	int16_t	Height;

	Height = g_stRoadInfo.height;	/* 水平線高さ変更 */

	/* 水平線 */
	/* ラスター割り込み処理の開始・終了位置 */
	/* 角度と距離と標高から水平線の位置を算出する。 */
	/* 角度と距離で求めた高さと標高が一致していれば、リニアに伸縮 */
	/* 角度と距離で求めた高さより標高が大きい場合、途中でカットされる道 */
	/* 角度と距離で求めた高さより標高が小さい場合、短く表現する道 */
	switch(bMode)
	{
		case 0:		/* TPS */
		{
			g_stRoadInfo.Horizon_Base = Y_HORIZON_0;
			g_stRoadInfo.Horizon = Y_HORIZON_0 - Height;
			ret = Mmax(Mmin(Y_HORIZON_0 - Height, ROAD_0_MAX), ROAD_0_MIN);	/* 水平位置決定（クリップ） */
			break;
		}
		case 1:		/* FPS */
		case 2:		/* FPS */
		default:	/* FPS */
		{
			g_stRoadInfo.Horizon_Base = Y_HORIZON_1;
			g_stRoadInfo.Horizon = Y_HORIZON_1 - Height;
			ret = Mmax(Mmin(Y_HORIZON_1 - Height, ROAD_1_MAX), ROAD_1_MIN);	/* 水平位置決定（クリップ） */
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
static int16_t Load_Road_Background(int16_t Num)
{
	int16_t	ret = 0;
	int16_t	i, count, mod;
	uint16_t	*pImage = NULL;
	uint16_t	*pSrcBuf = NULL;
	uint32_t uWidth, uHeight, uFileSize;
	uint8_t ubPatNumber;
	uint16_t x, y;
	
	ubPatNumber = BG_CG + (uint8_t)Num;
	
//	CG_File_Load( ubPatNumber );	/* グラフィックの読み込み */
	G_Load_Mem( ubPatNumber, 0, 0, 0 );	/* MYCAR_CG */
	
	pImage = Get_PicImageInfo( ubPatNumber, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	
	if(pImage != NULL)
	{
		if(uWidth != 0u)
		{
			/* X */
			count = X_MAX_DRAW / uWidth;
			mod = X_MAX_DRAW % uWidth;
			
			if(mod != 0)	/* 余りがあるなら限界まで埋める */
			{
				count++;
			}
			
			/* 横方向に敷き詰める */
			for(i = 0; i < count; i++)
			{
				pSrcBuf = pImage;
				x = uWidth * i;
//				y = Y_MIN_DRAW +        0 + Y_HORIZON_1 - uHeight + RASTER_MIN;
				y = Y_MIN_DRAW;
				G_BitBlt_From_Mem( x, y, 1, pSrcBuf, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);	/* 背景(上側) */
//				G_Load_Mem( ubPatNumber, uWidth * i, Y_MIN_DRAW +        0 + Y_HORIZON_1 - uHeight + RASTER_MIN, 1);	/* 背景(上側) */
			}
		}
		else
		{
			ret = -1;	/* 横幅がない */
		}
	}
	else
	{
		ret = -1;		/* 画像がない */
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
static int16_t Set_Road_Background_Col(int16_t Num)
{
	int16_t	ret = 0;

	uint32_t	i, j;
	uint16_t uRoadCol[MAPCOL_MAX][4][5] ={
#if 1
		{/* 0 */
			0x9C62,0xEF76,0xEF76,0xEF7A,0xA252,
			0x9C62,0xEF76,0x8F40,0xEF7A,0xA210,
			0xACE6,0x8F40,0xEF76,0xACE6,0x918C,
			0xACE6,0x8F40,0x8F40,0xACE6,0x918C
		},
#endif
#if 1
		{/* 1 */
			0x9C62,0xEF76,0xEF76,0x9C62,0xBE4E,
			0x9C62,0xEF76,0x8F40,0x9C62,0xBE4E,
			0xACE6,0x8F40,0x8F40,0xACE6,0xCED2,
			0xACE6,0x8F40,0xEF76,0xACE6,0xCED2
		},
#endif
#if 1
		{/* 2 */
			0xAF5A,0x9DC0,0x9DC0,0xAF5A,0xDF66,
			0xAF5A,0x9DC0,0xEF72,0xAF5A,0xDF66,
			0xBF5E,0xEF72,0x9DC0,0xBF5E,0xCF62,
			0xBF5E,0xEF72,0xEF72,0xBF5E,0xCF62
		},
#endif
#if 1
		{/* 3 */
			0x739C,0x9CE6,0xA528,0xEF7A,0xAA52,
			0x739C,0x9CE6,0xDEF6,0xEF7A,0xAA52,
			0x6B5A,0xDEF6,0xA528,0x6B5A,0x99CE,
			0x6B5A,0xDEF6,0xDEF6,0x6B5A,0x99CE
		},
#endif
#if 1
		{/* 4 */
			0x8420,0x669A,0x669A,0xDEF6,0xBCD6,
			0x8420,0x669A,0xDF7A,0xDEF6,0xBCD6,
			0x7BDE,0xDF7A,0x669A,0x7BDE,0xCD5A,
			0x7BDE,0xDF7A,0xDF7A,0x7BDE,0xCD5A
		}/* end */
#endif
	};
	
	if(Num >= MAPCOL_MAX)Num = 0;
	
	for(i=0; i < 4; i++)
	{
		PCG_PAL_Change( 8,  i+1, uRoadCol[Num][i][0]);
		PCG_PAL_Change( 9,  i+1, uRoadCol[Num][i][1]);
		PCG_PAL_Change(10,  i+1, uRoadCol[Num][i][2]);
		PCG_PAL_Change(11,  i+1, uRoadCol[Num][i][3]);
		PCG_PAL_Change(12,  i+1, uRoadCol[Num][i][4]);
	}
	
	for(j=0; j < 16; j++)
	{
		PCG_PAL_Change( j,  14, 0xE81E);
	}
	
	for(j=0; j < 16; j++)
	{
		PCG_PAL_Change( j,  15, 0xE81E);
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
int16_t Road_Map_Draw(uint8_t bMode)
{
	int16_t	ret = 0;
	int16_t	col;
	int16_t			x =0, y = 0;
	uint16_t		pos_x, pos_y;
	static int32_t	xofst, yofst, rad;
	static uint16_t s_uCount = 0u;
	
	if(s_uCount >= g_stRoadInfo.Courselength)
	{
		s_uCount = 0;
		return  -1;
	}
	else if(s_uCount == 0)
	{
		_iocs_window( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1);	/* 全領域書き換えOK */
		
		xofst = Mmul256(256);
		yofst = Mmul256(0) + Mmul256((s_uCount/256) * 64);
		
		rad = 0;
	}
	else
	{
#if 0
		g_stRoadData[s_uCount].bHeight;		/* 道の標高	(0x80センター) */
		g_stRoadData[s_uCount].bWidth;			/* 道の幅	(0x80センター) */
		g_stRoadData[s_uCount].bAngle;			/* 道の角度	(0x80センター) */
		g_stRoadData[s_uCount].bfriction;		/* 道の摩擦	(0x80センター) */
		g_stRoadData[s_uCount].bPat;			/* 道の種類	 */
		g_stRoadData[s_uCount].bObject;		/* 出現ポイントのオブジェクトの種類 */
		g_stRoadData[s_uCount].bRepeatCount;	/* 繰り返し回数 */
#endif		
	}
	
	
	col = 0x3;	/* White */
	
	rad += 0x80 - g_stRoadData[s_uCount].bAngle;
	rad %= 360;
	
	x = APL_Cos(rad);
	y = APL_Sin(rad);
	
#if 1
	xofst += x;
	xofst += x;	/* bug */
	yofst += y;
#else
	if(xofst >= 0x20000)
	{
		xofst = Mmul256(0);
		yofst = Mmul256(0) + Mmul256((s_uCount/256) * 64);
	}
	else if(xofst <= -X_OFFSET)
	{
		xofst = Mmul256(256);
		yofst = Mmul256(0) + Mmul256((s_uCount/256) * 64);
	}
	else
	{
		xofst += x;
		yofst += y;
	}
#endif
	pos_x = Mdiv512(xofst);
	pos_y = Mdiv512(yofst);

	_iocs_home(0b0000, X_OFFSET + pos_x - 128, Y_OFFSET + pos_y - 128);

	Draw_Pset( X_OFFSET + pos_x, Y_OFFSET + pos_y,	col);
	
	s_uCount++;
	
	return ret;
}

#endif	/* RASTER_C */

