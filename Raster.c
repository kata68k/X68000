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
#include "Text.h"

/* 変数 */
static US	g_uRoadAnime;
static US	g_uRoadAnime_old;
static US	g_uRoadAnimeBase;
static US	g_uRoadAnimeBase_old;
static US	g_uRoadAnimeCount;

static US	g_uRoadCycleCount;
static US	g_uCounter;
static UC	g_bRoadInitFlag = TRUE;

/* 構造体定義 */
static ST_RAS_INFO	g_stRasInfo = {0};

static ST_ROAD_INFO	g_stRoadInfo = {0};
static ST_ROADDATA	g_stRoadData[ROADDATA_MAX] = {0};

/* 関数のプロトタイプ宣言 */
SS	GetRasterInfo(ST_RAS_INFO *);
SS	SetRasterInfo(ST_RAS_INFO);
void	Raster_Init(void);
void	Raster_Main(UC);
static UC Raster_Calc_H(SS *, SS, SS);
static UC Raster_Calc_V(SS *, SS, SS);

SS	GetRasterPos(US *, US *, US);	/* ラスター処理結果を取得 */

SS	GetRoadInfo(ST_ROAD_INFO *);
SS	SetRoadInfo(ST_ROAD_INFO);
void Road_Init(US);
static void Get_Road_Pat(US *, UC);
static void Set_Road_Pat_offset(SS);
static SS Road_Pat_Update(US);
static void Road_Pat_Base_Update(void);
static void Road_Pat_Start(void);
static void Road_Pat_Stop(void);
static SS	Set_Road_Height(void);
static SS	Set_Road_Slope(void);
static SS	Set_Road_Angle(void);
static SS	Set_Road_Distance(void);
static SS	Set_Road_Object(void);
static void	Road_Pat_Main(void);
SS	GetRoadCycleCount(US *);
UL	GetRoadDataAddr(void);
SS	SetHorizon(UC);

/* 関数 */

SS GetRasterInfo(ST_RAS_INFO *stDat)
{
	SS	ret = 0;
	
	*stDat = g_stRasInfo;
	
	return ret;
}

SS SetRasterInfo(ST_RAS_INFO stDat)
{
	SS	ret = 0;
	
	g_stRasInfo = stDat;
	
	return ret;
}

void Raster_Init(void)
{
	memset(&g_stRasInfo, 0, sizeof(ST_RAS_INFO));
}

void Raster_Main(UC bMode)
{
	SS	view_offset_x, view_offset_y;
	SS	hide_offset_x, hide_offset_y;
	SS	BG_offset_x, BG_offset_y;
	
	US	uRas_y;
	US	uRas_st, uRas_mid, uRas_ed, uRas_size;

	ST_CRT			stCRT = {0};
	ST_RASTER_INT	stRasterInt[RASTER_MAX] = {0};

#ifdef DEBUG	/* デバッグコーナー */
	UC	bY_area;
	UC	bDebugMode;
	US	uDebugNum;
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
	
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)	/* デバッグモード */
	{
		SS	col;
		
		APAGE(0);				/* グラフィックの書き込み */

		/* ラスター開始位置 */
		col = 0x38;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.st,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.st,
					col,
					0xFFFF);
		/* ラスター中間位置 */
		col = 0x3B;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.mid,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.mid,
					col,
					0xFFFF);
		/* ラスター終了位置 */
		col = 0x43;
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
		US	uRev_uRas_y;
		SS	ras_cal_x=0;
		SS	ras_cal_y=0;
		US	ras_pat=0;
		US	ras_offset;
		UC	bRoad_pat = FALSE;
		
		uRev_uRas_y = uRas_size - uRas_y;	/* uRas_size -> 0 */
		
		/* X座標 */
		Raster_Calc_H( &ras_cal_x, uRas_y, uRev_uRas_y );

		/* Y座標 */
		bY_area = Raster_Calc_V( &ras_cal_y, uRas_y, uRev_uRas_y );

		/* ロードパターン */
		if(uRas_y == 0)
		{
			bRoad_pat = TRUE;
		}
		Get_Road_Pat(&ras_pat, bRoad_pat);	/* ロードのパターン情報を取得 */
		Set_Road_Pat_offset(uRas_y);		/* ロードパターン閾値の更新 */

		ras_offset = uRas_st + uRas_y;

#ifdef DEBUG	/* デバッグコーナー */
		if(bDebugMode == TRUE)	/* デバッグモード */
		{
			SS	col;
			SS	point_x;
			
			/* x */
			col = 0xB4;
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
			col = (SS)bY_area;
			Draw_Pset(	hide_offset_x + ras_offset,
						hide_offset_y + ras_cal_y,
						col);
		}
#endif
		stRasterInt[ras_offset].x = (US)ras_cal_x;
		stRasterInt[ras_offset].y = (US)ras_cal_y;
		stRasterInt[ras_offset].pat = ras_pat;

		stRasterInt[ras_offset+1].x = (US)ras_cal_x;	/* 隙間の複製 */
		stRasterInt[ras_offset+1].y = (US)ras_cal_y;	/* 隙間の複製 */
		stRasterInt[ras_offset+1].pat = ras_pat;		/* 隙間の複製 */
	}
	
	stRasterInt[0].y = g_stRasInfo.st;		/* 割り込み位置の設定 */
	/* BGのY座標を選ぶラスタの割り込み位置に対して表示したいBGのY座標から */
	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
	stRasterInt[1].y = ROAD_ST_POINT - g_stRasInfo.st;	/* 割り込みがかかるまでの表示位置(ラスタ割り込み位置によるずれの考慮は不要)	*/
	
	SetRasterIntData(stRasterInt, sizeof(ST_RASTER_INT) * RASTER_MAX);
}

static UC Raster_Calc_H(SS *x, SS Num, SS RevNum)
{
	UC bRet = 0;

	if(Num == 0u)
	{
		bRet = 0;
	}
	/* X座標 */
	{
		SS	Steer = 0;
		SS	Scroll_w = 0;
		SS	Scroll_ofst = 0;
		SS	Point_x = 0;
		ST_CARDATA stMyCar = {0};

		GetMyCar(&stMyCar);			/* 自車の情報を取得 */

		Steer = (stMyCar.Steering >> 4);
		
		/* スクロールする幅×計算する高さ÷ラスタースクロールさせる高さ */
		if((Steer == 0) || (Num == 0))
		{
			Scroll_ofst	= 0;
		}
		else
		{
			Scroll_ofst	= APL_sDiv( Steer * Num, g_stRasInfo.size );	/* 車体の移動(高い位置ほど移動量が少ない) */
		}
		
		/* スクロールする幅×計算する高さ÷ラスタースクロールさせる高さ */
		if(g_stRoadInfo.angle == 0)
		{
			Scroll_w	= 0;
		}
		else
		{
			SS Scroll_w_clp;
			Scroll_w_clp = Mmax( Mmin( g_stRoadInfo.angle * g_stRasInfo.size, 511 ), -512 );
			
			if(Num == 0)
			{
				Scroll_w	= Scroll_w_clp;	/* カーブ(低い位置ほど移動量が少ない) */
			}
			else
			{
				Scroll_w	= APL_sDiv( Scroll_w_clp, Num);	/* カーブ(低い位置ほど移動量が少ない) */
			}
		}
		Point_x		= Scroll_ofst + Scroll_w;	/* ラスタースクロール位置 */

		*x = Point_x & 0x1FFu;	/* 9bitマスク *//* Max224ぐらい */
	}

	return bRet;
}

static UC Raster_Calc_V(SS *y, SS Num, SS RevNum)
{
	UC bRet = 0;
	SS RoadPoint_y = 0;

#ifdef DEBUG	/* デバッグコーナー */
	UC	bDebugMode;
	US	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif

	if(Num == 0u)	
	{
		bRet = 0x00;
	}
	/* Y座標 */
	{
		SS	Point_y, Point_y_def;
		SS	MinClip, MaxClip;
		SS	uRasStart;
		
		uRasStart = Mmin(g_stRasInfo.st, g_stRasInfo.mid);
		MinClip = ROAD_ST_POINT - (uRasStart + Num);
		MaxClip = ROAD_ED_POINT - (uRasStart + Num);
		Point_y_def = ROAD_ST_POINT - g_stRasInfo.mid;	/* 標準位置 */
		
		if(g_stRasInfo.mid == g_stRoadInfo.Horizon)	/* 水平線の位置が通常より下側の処理 */
		{
			RoadPoint_y = Point_y_def;

			bRet = 0xFF;
		}
		else if( g_stRasInfo.mid > g_stRoadInfo.Horizon )	/* 水平線の位置が通常より上側の処理 */
		{
			SS	Road_strch = 0;
			
			US	uSize;

			uSize = g_stRasInfo.mid - g_stRoadInfo.Horizon;
			Road_strch = APL_uDiv( uSize * RevNum, g_stRasInfo.size );

			RoadPoint_y = Mmax(Point_y_def + Road_strch, Point_y_def);

			bRet = 0xFF;
		}
		else	/* 水平線の位置が通常より下側の処理 */
		{
			SS	Road_strch = 0;
			US	uSize;

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

SS GetRoadInfo(ST_ROAD_INFO *stDat)
{
	SS	ret = 0;
	
	*stDat = g_stRoadInfo;
	
	return ret;
}

SS SetRoadInfo(ST_ROAD_INFO stDat)
{
	SS	ret = 0;
	
	g_stRoadInfo = stDat;
	
	return ret;
}

void Road_Init(US uCourseNum)
{
	UI	i;
	
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
	Load_Course_Data(uCourseNum);

	Set_Road_Angle();		/* コースの曲がり具合 */
	Set_Road_Height();		/* コースの標高 */
	Set_Road_Slope();		/* コースの現在地から標高までの傾き */
	Set_Road_Distance();	/* 水平線かｒ現在地までの距離 */
	Set_Road_Object();		/* ライバル車の出現 */
}

static void Get_Road_Pat(US *p_uPat, UC bFlag)
{
#if 1
	US	uBG_pat[4][4] = {288,   0,  96, 192, 
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

static void Set_Road_Pat_offset(SS Num)
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

static SS Road_Pat_Update(US uThreshold)
{
	SS	ret = 0;
	
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

static void Road_Pat_Start(void)
{
	g_uRoadAnime_old = g_uRoadAnime;
	g_uRoadAnimeBase_old = g_uRoadAnimeBase;
}

static void Road_Pat_Stop(void)
{
	g_uRoadAnime = g_uRoadAnime_old;
	g_uRoadAnimeBase = g_uRoadAnimeBase_old;
	g_uRoadAnimeCount = 0;
}

static SS	Set_Road_Height(void)
{
	SS	ret = 0;
	SS	road_height;
	static SS	road_height_old;
	
	road_height_old = g_stRoadInfo.height;	/* 前回値更新 */
	
	/* コースデータ読み込み */
	road_height = (SS)(0x80 - g_stRoadData[g_uCounter].bHeight);	/* 道の標高	(0x80センター) */
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

static SS	Set_Road_Slope(void)
{
	SS	ret = 0;
	SS	cal_tan = 0;
	SS	cal_cos = 0;
	SS	road_slope = 0;
	static SS	road_slope_old;

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
	cal_tan = APL_Tan256((SS)rad);
#endif
	g_stRoadInfo.slope		= road_slope;		/*  */
	
	return ret;
}

static SS	Set_Road_Angle(void)
{
	SS	ret = 0;
	SS	road_angle;
	static SS	road_angle_old;
	
	/* コーナー */
	road_angle_old = g_stRoadInfo.angle;
	road_angle = (SS)g_stRoadData[g_uCounter].bAngle - 0x80;		/* 道の角度	(0x80センター) */
	road_angle = Mmax(Mmin(road_angle, 12), -12);

	g_stRoadInfo.angle = road_angle;		/*  */
	
	return ret;
}

static	SS	Set_Road_Distance(void)
{
	SS	ret = 0;
	SS	road_distance;
	static SS road_distance_old;
	
	road_distance_old = g_stRoadInfo.distance;	/* 前回値更新 */
	road_distance = 0;
	g_stRoadInfo.distance	= road_distance;	/*  */
	
	return ret;
}

static SS	Set_Road_Object(void)
{
	SS	ret = 0;
	SS	road_object = 0;
	
	road_object = g_stRoadData[g_uCounter].bObject;	/* 出現ポイントのオブジェクトの種類 */
	if(road_object != 0u)
	{
		SetAlive_EnemyCAR();	/* ライバル車を出現させる */
	}
	g_stRoadInfo.object		= road_object;		/*  */
	
	return ret;
}

static void Road_Pat_Main(void)
{
	static UI	unRoadAnimeTime = 0xFFFFFFFFu;
	UI	time_st = 0u;
	SS	speed;

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
		
		g_uCounter++;			/* コースデータのカウンタ更新 */
		if(g_uCounter >= ROADDATA_MAX)
		{
			g_uCounter = 0u;
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

SS	GetRoadCycleCount(US *p_uCount)
{
	SS	ret = 0;
	
	*p_uCount = g_uRoadCycleCount;
	
	return ret;
}

UL GetRoadDataAddr(void)
{
	return (UL)&g_stRoadData[0];
}

SS	SetHorizon(UC bMode)
{
	SS	ret = 0;
	SS	Horizon=0;
	SS	Height;

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

#endif	/* RASTER_C */

