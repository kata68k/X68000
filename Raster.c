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

/* 構造体定義 */
ST_RASTER_INT g_stRasterInt[RASTER_MAX] = {0};
static ST_RAS_INFO	g_stRasInfo = {0};

static ST_ROAD_INFO	g_stRoadInfo = {0};
static ST_ROADDATA	g_stRoadData[ROADDATA_MAX] = {0};

/* 関数のプロトタイプ宣言 */
int16_t	GetRasterInfo(ST_RAS_INFO *);
int16_t	SetRasterInfo(ST_RAS_INFO);
void	Raster_Init(void);
void	Raster_Main(uint8_t);
static uint8_t Raster_Calc_H(int16_t *, int16_t, int16_t);
static uint8_t Raster_Calc_V(int16_t *, int16_t, int16_t);

int16_t	GetRasterPos(uint16_t *, uint16_t *, uint16_t);	/* ラスター処理結果を取得 */

int16_t	GetRoadInfo(ST_ROAD_INFO *);
int16_t	SetRoadInfo(ST_ROAD_INFO);
void Road_Init(uint16_t);
void Road_BG_Init(uint16_t);
static void Get_Road_Pat(uint16_t *, uint8_t);
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
static void	Road_Pat_Main(void);
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
void Raster_Main(uint8_t bMode)
{
	int16_t	view_offset_x, view_offset_y;
	int16_t	hide_offset_x, hide_offset_y;
	int16_t	BG_offset_x, BG_offset_y;
	
	uint16_t	uRas_y;
	uint16_t	uRas_st, uRas_mid, uRas_ed, uRas_size;

	int16_t	Steer = 0;
	int16_t	Scroll_ofst = 0;

	ST_CRT			stCRT = {0};
	ST_RASTER_INT	stRasterInt[RASTER_MAX] = {0};
	ST_CARDATA 		stMyCar = {0};

#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bY_area;
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	/* モード切替による設定値の変更 */
	GetCRT(&stCRT, bMode);
	view_offset_x	= stCRT.view_offset_x;
	view_offset_y	= stCRT.view_offset_y;
	hide_offset_x	= stCRT.hide_offset_x;
	hide_offset_y	= stCRT.hide_offset_y;
	BG_offset_x		= stCRT.BG_offset_x;
	BG_offset_y		= stCRT.BG_offset_y;

	/* 自車の情報を取得 */
	GetMyCar(&stMyCar);
	Steer = Mdiv16(stMyCar.Steering);	/* ステアリングポジション */

	/* ロードパターン */
	Road_Pat_Main();

	/* 水平線の設定 */
	SetHorizon(bMode);

	/* 初期化 */
	uRas_st = 0;
	uRas_mid = 0;
	uRas_ed = 0;
	uRas_size = 0xFFFF;

	/* ラスター位置 */
	g_stRasInfo.st	= g_stRoadInfo.Horizon;					/* ラスター開始位置 */
	g_stRasInfo.mid = g_stRoadInfo.Horizon_Base;			/* ラスター固定位置 *//* 水平線の位置が変化しないポイント *//* ハイウェイスターなら 64 = ROAD_SIZE*2/3 */
	if(bMode == 0)		/* TPS */
	{
		g_stRasInfo.ed = Mmin(RASTER_MAX, V_SYNC_MAX);		/* ラスター終了位置 */
	}
	else				/* FPS */
	{
		g_stRasInfo.ed = Mmin(Y_MAX_WINDOW, V_SYNC_MAX);	/* ラスター終了位置 */
	}

	uRas_st		= g_stRasInfo.st;	/* 開始位置 */
	uRas_mid	= g_stRasInfo.mid;	/* 中間位置 */
	uRas_ed		= g_stRasInfo.ed;	/* 終了位置 */
	uRas_size	= uRas_ed - uRas_st;				/* ラスターの範囲 */
	g_stRasInfo.size	= uRas_size;				/* ラスターの範囲 */
	SetRasterInfo(g_stRasInfo);
	
//#ifdef DEBUG	/* デバッグコーナー */
#if 0
	if(bDebugMode == TRUE)	/* デバッグモード */
	{
		int16_t	col;
		
		_iocs_apage(0);				/* グラフィックの書き込み(全ページ) */

		/* ラスター開始位置 */
		col = 0x3;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.st,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.st,
					col,
					0xFFFF);
		/* ラスター中間位置 */
		col = 0xF;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.mid,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.mid,
					col,
					0xFFFF);
		/* ラスター終了位置 */
		col = 0xB;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.ed,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.ed,
					col,
					0xFFFF);
	}
#endif
	
	Road_Pat_Start();	/* 初回は道のパレットの開始設定を行う */
	
	for(uRas_y=0; uRas_y < uRas_size; uRas_y+=RASTER_NEXT)
	{
		uint16_t	uRev_uRas_y;
		int16_t	ras_cal_x=0;
		int16_t	ras_cal_y=0;
		uint16_t	ras_pat=0;
		uint16_t	ras_offset;
		uint8_t	bRoad_pat = FALSE;
		
		uRev_uRas_y = uRas_size - uRas_y;	/* uRas_size -> 0 */
		
		
		/* X座標 */
#if 1
		if(uRas_y == 0)
		{
			Scroll_ofst	= 0;
			ras_cal_x = 0;
			bRoad_pat = TRUE;
		}

		if(Steer == 0)
		{
			Scroll_ofst	= 0;
		}
		else
		{
			/* ステアリングポジション */
			Scroll_ofst	= APL_sDiv( Steer * uRas_y, g_stRasInfo.size );	/* 車体の移動(高い位置ほど移動量が少ない) */
		}
		/* スクロールする幅×計算する高さ÷ラスタースクロールさせる高さ */
		if(g_stRoadInfo.angle == 0)
		{
			ras_cal_x	= 0;
		}
		else
		{
			int16_t Scroll_w_clp;
			
			Scroll_w_clp = Mmax( Mmin( g_stRoadInfo.angle * g_stRasInfo.size, 255 ), -256 );
			
			ras_cal_x	= APL_sDiv( Scroll_w_clp, uRas_y);	/* カーブ(低い位置ほど移動量が少ない) */
		}
		ras_cal_x = (ras_cal_x + Scroll_ofst) & 0x1FFu;		/* 9bitマスク *//* Max224ぐらい */
#else
		Raster_Calc_H( &ras_cal_x, uRas_y, uRev_uRas_y );
#endif

		/* Y座標 */
		if(	(g_stRasInfo.mid == g_stRoadInfo.Horizon)	/* 水平線の位置が通常より下側の処理 */
			|| (g_CpuTime < 300) )						/* 低速MPU */
		{
			ras_cal_y = ROAD_ST_POINT - g_stRasInfo.mid;	/* 標準位置 */
#ifdef DEBUG	/* デバッグコーナー */
			bY_area = 0xFF;
#endif
		}
		else
		{
#ifdef DEBUG	/* デバッグコーナー */
			bY_area = Raster_Calc_V( &ras_cal_y, uRas_y, uRev_uRas_y );
#else
			Raster_Calc_V( &ras_cal_y, uRas_y, uRev_uRas_y );
#endif
		}

		/* ロードパターン */
		Get_Road_Pat(&ras_pat, bRoad_pat);	/* ロードのパターン情報を取得 */
		Set_Road_Pat_offset(uRas_y);		/* ロードパターン閾値の更新 */

		ras_offset = uRas_st + uRas_y;

//#ifdef DEBUG	/* デバッグコーナー */
#if 0
		if(bDebugMode == TRUE)	/* デバッグモード */
		{
			int16_t	col;
			int16_t	point_x;
			
			/* x */
			col = 0xE;
			/* センター */
			if( ras_cal_x < 256 )	/* 左カーブ */
			{
				point_x = ROAD_CT_POINT + (  0 - ras_cal_x);
			}
			else	/* 右カーブ */
			{
				point_x = ROAD_CT_POINT + (512 - ras_cal_x);
			}
			Draw_Pset(	hide_offset_x + point_x,
						hide_offset_y + g_stRoadInfo.Horizon + uRas_y,
						col);

			/* y */
			col = (int16_t)bY_area;
			Draw_Pset(	hide_offset_x + ras_offset,
						hide_offset_y + ras_cal_y,
						col);
		}
#endif
		stRasterInt[ras_offset].x = (uint16_t)ras_cal_x;
		stRasterInt[ras_offset].y = (uint16_t)ras_cal_y;
		stRasterInt[ras_offset].pat = ras_pat;

		g_stRasterInt[ras_offset] = stRasterInt[ras_offset];

		stRasterInt[ras_offset+1].x = (uint16_t)ras_cal_x;	/* 隙間の複製 */
		stRasterInt[ras_offset+1].y = (uint16_t)ras_cal_y;	/* 隙間の複製 */
		stRasterInt[ras_offset+1].pat = ras_pat;		/* 隙間の複製 */
		
		g_stRasterInt[ras_offset+1] = stRasterInt[ras_offset+1];
	}
	stRasterInt[0].y = g_stRasInfo.st;					/* 割り込み位置の設定 */
	g_stRasterInt[0] = stRasterInt[0];
	/* BGのY座標を選ぶラスタの割り込み位置に対して表示したいBGのY座標から */
	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
	stRasterInt[1].y = ROAD_ST_POINT - g_stRasInfo.st;	/* 割り込みがかかるまでの表示位置(ラスタ割り込み位置によるずれの考慮は不要)	*/
	g_stRasterInt[1] = stRasterInt[1];
	
	SetRasterIntData(NULL, 0);	/* ラスター割り込みデータ設定完了 */
}

#if 0
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

	if(Num == 0u)
	{
		bRet = 0;
	}
	/* X座標 */
	{
		int16_t	Scroll_w = 0;
		
		/* スクロールする幅×計算する高さ÷ラスタースクロールさせる高さ */
		if(g_stRoadInfo.angle == 0)
		{
			Scroll_w	= 0;
		}
		else
		{
			int16_t Scroll_w_clp;
			
			Scroll_w_clp = Mmax( Mmin( g_stRoadInfo.angle * g_stRasInfo.size, 255 ), -256 );
			
			if(Num == 0)
			{
				Scroll_w	= Scroll_w_clp;	/* カーブ(低い位置ほど移動量が少ない) */
			}
			else
			{
				Scroll_w	= APL_sDiv( Scroll_w_clp, Num);	/* カーブ(低い位置ほど移動量が少ない) */
			}
		}
		*x = Scroll_w;	/* ラスタースクロール位置 */
	}

	return bRet;
}
#endif

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

	if(Num == 0u)	
	{
		bRet = 0x00;
	}
	/* Y座標 */
	{
		int16_t	Point_y, Point_y_def;
		int16_t	MinClip, MaxClip;
		int16_t	uRasStart;
		
		uRasStart = Mmin(g_stRasInfo.st, g_stRasInfo.mid);
		MinClip = ROAD_ST_POINT - (uRasStart + Num);
		MaxClip = ROAD_ED_POINT - (uRasStart + Num);
		Point_y_def = ROAD_ST_POINT - g_stRasInfo.mid;	/* 標準位置 */
		
		if( g_stRasInfo.mid > g_stRoadInfo.Horizon )	/* 水平線の位置が通常より上側の処理 */
		{
			int16_t	Road_strch = 0;
			
			uint16_t	uSize;

			uSize = g_stRasInfo.mid - g_stRoadInfo.Horizon;
			Road_strch = APL_uDiv( uSize * RevNum, g_stRasInfo.size );

			RoadPoint_y = Mmax(Point_y_def + Road_strch, Point_y_def);

			bRet = 0xFF;
		}
		else	/* 水平線の位置が通常より下側の処理 */
		{
			int16_t	Road_strch = 0;
			uint16_t	uSize;

			uSize = g_stRoadInfo.Horizon - g_stRasInfo.mid;
			Road_strch = APL_uDiv( uSize * RevNum, g_stRasInfo.size );

			RoadPoint_y = Mmax(Point_y_def - Road_strch, Point_y_def);

			bRet = 0xb7;
		}
		
		Point_y = Mmax(Mmin(RoadPoint_y, MaxClip), MinClip);	/* 上下限クリップ */
		if(RoadPoint_y == MaxClip)
		{
			bRet = TRUE;	/* MaxClip */
		}
		*y = Point_y & 0x1FFu;	/* 9bitマスク */
	}
		
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
static void Get_Road_Pat(uint16_t *p_uPat, uint8_t bFlag)
{
#if 1
	uint16_t	uBG_pat[4][4] = {288,   0,  96, 192, 
		 						 192, 288,   0,  96, 
		 						  96, 192, 288,   0, 
		 						   0,  96, 192, 288} ;
	if(bFlag == TRUE)
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
		g_stRoadInfo.offset_y = 0;
		g_stRoadInfo.offset_val = 0;
	}
	else
	{
		//g_stRoadInfo.offset_y = (Num * g_stRoadInfo.offset_val) / Mmax((g_stRoadInfo.offset_val + 96), 1);
		g_stRoadInfo.offset_y = APL_uDiv((Num * g_stRoadInfo.offset_val), (g_stRoadInfo.offset_val + g_stRasInfo.size ));
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
	
	road_height_old = g_stRoadInfo.height;	/* 前回値更新 */
	
	/* コースデータ読み込み */
	road_height = ((int16_t)g_stRoadData[g_uCounter].bHeight) - 0x80;	/* 道の標高	(0x80センター) */
	road_height = Mmax(Mmin(road_height, 31), -32);					/* +高い：-低い */

	g_stRoadInfo.height = road_height;		/*  */

	// 高低差から傾きを出す(-45～+45) rad = APL_Atan2( road_slope_old - road_slope,  );
	if(road_height != road_height_old)
	{
		if(road_height > road_height_old)
		{
			/* 保持 */
		}
		else
		{
			/* 保持 */
		}
		ret = 1;
	}
	else{
		/* 前回値保持 */
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
static int16_t	Set_Road_Slope(void)
{
	int16_t	ret = 0;
	int16_t	cal_tan = 0;
	int16_t	cal_cos = 0;
	int16_t	road_slope = 0;
	static int16_t	road_slope_old;

	road_slope_old = g_stRoadInfo.slope;		/* 前回値更新 */
	road_slope = g_stRoadInfo.height;
	
#if 1
	if(road_slope == 0)
	{
		cal_tan = 0;
		cal_cos = 0;
	}
	else
	{
		if(road_slope != road_slope_old)
		{
			cal_tan = 0;
			cal_cos = 0;
//			cal_tan = APL_Tan256(road_slope);	/* tanθ */
//			cal_cos = APL_Cos(cal_tan);			/* cosθ */
		}
	}
#else
	/* 底辺と高さから角度を求めて、角度からY座標を演算していたが*/
	/* 角度演算は不要とのこと。単純に距離と角度から高さを算出できる @runshiwaさんより指摘 */
	if(vy == 0)
	{
		rad = 0.0;
	}
	else
	{
		rad = APL_Atan2( vy, uRas_mid );
	}
	cal_tan = APL_Tan256((int16_t)rad);
#endif
	g_stRoadInfo.slope		= road_slope;		/*  */
	
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
	int16_t	road_angle;
	static int16_t	road_angle_old;
	
	/* コーナー */
	road_angle_old = g_stRoadInfo.angle;
	road_angle = (int16_t)g_stRoadData[g_uCounter].bAngle - 0x80;		/* 道の角度	(0x80センター) */
	road_angle = Mmax(Mmin(road_angle, 12), -12);

	g_stRoadInfo.angle = road_angle;		/*  */
	
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
static void Road_Pat_Main(void)
{
	static uint32_t	unRoadAnimeTime = 0xFFFFFFFFu;
	uint32_t	time_st = 0u;
	int16_t	speed;

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
	int16_t	Horizon=0;
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
			Horizon = Mmax(Mmin(Y_HORIZON_0 - Height, ROAD_0_MAX), ROAD_0_MIN);	/* 水平位置決定（クリップ） */
			break;
		}
		case 1:		/* FPS */
		case 2:		/* FPS */
		default:	/* FPS */
		{
			g_stRoadInfo.Horizon_Base = Y_HORIZON_1;
			Horizon = Mmax(Mmin(Y_HORIZON_1 - Height, ROAD_1_MAX), ROAD_1_MIN);	/* 水平位置決定（クリップ） */
			break;
		}
	}
	g_stRoadInfo.Horizon = Horizon;
	
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
	
	CG_File_Load( ubPatNumber );	/* グラフィックの読み込み */
	
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
		return  -1;
	}
	else if(s_uCount == 0)
	{
		_iocs_apage(0);	/* グラフィックの書き込み(全ページ) */
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

