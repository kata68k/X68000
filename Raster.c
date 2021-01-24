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
	SS	BG_under;
	
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
	BG_under		= stCRT.BG_under;

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
	if(bMode == 0)		/* TPS */
	{
		uRas_ed  = Mmin(RASTER_MAX, V_SYNC_MAX);	/* ラスター終了位置 */
		uRas_mid = Y_HORIZON_0 + RASTER_MIN;		/* 水平線の位置が変化しないポイント *//* ハイウェイスターなら 64 = ROAD_SIZE*2/3 */
	}
	else
	{
		uRas_ed  = Mmin(ROAD_ED_POINT - RASTER_MIN, V_SYNC_MAX);	/* ラスター終了位置 */
		uRas_mid = Y_HORIZON_1 + RASTER_MIN;		/* 水平線の位置が変化しないポイント *//* ハイウェイスターなら 64 = ROAD_SIZE*2/3 */
	}
//	uRas_st = RASTER_MIN;	/* ラスター開始位置 */	
	uRas_st = Mmax(Mmin(g_stRoadInfo.Horizon + RASTER_MIN, RASTER_MAX), RASTER_MIN);	/* ラスター開始位置 */
	if((uRas_st & 0x01) != 0)
	{
		uRas_st += 1;	/* 奇数なら無理矢理偶数にする */
	}
//	uRas_st -= (uRas_st % 2);	/* 偶数にする(for分がRASTER_NEXT分インクリメントするので) */
//	uRas_ed -= (uRas_ed % 2);	/* 偶数にする */
	uRas_size = uRas_ed - uRas_st;			/* ラスターの範囲 */
	
	g_stRasInfo.st			= uRas_st;
	g_stRasInfo.mid			= uRas_mid;
	g_stRasInfo.ed			= uRas_ed;
	g_stRasInfo.size		= uRas_size;
	SetRasterInfo(g_stRasInfo);
	
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)	/* デバッグモード */
	{
		SS	col;
		
		APAGE(0);				/* グラフィックの書き込み */

		/* 水平線 */
		col = 0x35;
		Draw_Line(	hide_offset_x + uRas_st,
					hide_offset_y + g_stRoadInfo.Horizon,
					hide_offset_x + uRas_ed, 
					hide_offset_y + g_stRoadInfo.Horizon,
					col,
					0xFFFF);
		/* ラスター開始位置 */
		col = 0x38;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + uRas_st,
					hide_offset_x + WIDTH, 
					hide_offset_y + uRas_st,
					col,
					0xFFFF);
		/* ラスター中間位置 */
		col = 0x3B;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + uRas_mid,
					hide_offset_x + WIDTH, 
					hide_offset_y + uRas_mid,
					col,
					0xFFFF);
		/* 表示終了位置 */
		col = 0x3D;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + Y_MAX_WINDOW,
					hide_offset_x + WIDTH, 
					hide_offset_y + Y_MAX_WINDOW,
					col,
					0xFFFF);
		/* ラスター終了位置 */
		col = 0x43;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + uRas_ed,
					hide_offset_x + WIDTH, 
					hide_offset_y + uRas_ed,
					col,
					0xFFFF);
	}
#endif
	
	for(uRas_y=uRas_st; uRas_y < uRas_ed; uRas_y+=RASTER_NEXT)
	{
		SS	num;
		SS	rev;
		SS	ras_cal_x=0;
		SS	ras_cal_y=0;
		US	ras_pat=0;
		UC	bRoad_pat = FALSE;
		
		num = uRas_y - uRas_st;			/* 0 -> uRas_size */
		rev = uRas_ed - uRas_y;			/* uRas_size -> 0 */
		
		/* X座標 */
		Raster_Calc_H( &ras_cal_x, num, rev );

		/* Y座標 */
		bY_area = Raster_Calc_V( &ras_cal_y, num, rev );
		
		/* ロードパターン */
		if(num == 0)
		{
			bRoad_pat = TRUE;
		}
		Get_Road_Pat(&ras_pat, bRoad_pat);	/* ロードのパターン情報を取得 */
		Set_Road_Pat_offset(num);			/* ロードパターン閾値の更新 */

#ifdef DEBUG	/* デバッグコーナー */
		if(bDebugMode == TRUE)	/* デバッグモード */
		{
			SS	col;
			SS	point_x;
			
			/* x */
			col = 0xB4;
			if(ras_cal_x < 256)
			{
				point_x = ras_cal_x;
			}
			else
			{
				point_x = ras_cal_x - 512;
			}
			Draw_Pset(	hide_offset_x + (WIDTH >> 1) - point_x,
						hide_offset_y + g_stRoadInfo.Horizon + num,
						col);

			/* y */
			col = (SS)bY_area;
			Draw_Pset(	hide_offset_x + uRas_y,
						hide_offset_y + ras_cal_y,
						col);
		}
#endif
		stRasterInt[uRas_y].x = (US)ras_cal_x;
		stRasterInt[uRas_y].y = (US)ras_cal_y;
		stRasterInt[uRas_y].pat = ras_pat;

		stRasterInt[uRas_y+1].x = (US)ras_cal_x;	/* 隙間の複製 */
		stRasterInt[uRas_y+1].y = (US)ras_cal_y;	/* 隙間の複製 */
		stRasterInt[uRas_y+1].pat = ras_pat;		/* 隙間の複製 */
	}
	
	stRasterInt[0].y = uRas_st;		/* 割り込み位置の設定 */
	stRasterInt[1].y = BG_offset_y;	/* 空のコントロール */
	
	SetRasterIntData(stRasterInt, sizeof(ST_RASTER_INT) * RASTER_MAX);
}

static UC Raster_Calc_H(SS *x, SS Num, SS RevNum)
{
	UC bRet = FALSE;

	if(Num == 0u)
	{
		*x = 0;	/* 最初 */
	}
#if 0
	else if( uRas_y < uRas_mid )				/* 水平線の位置が変化 *//* 64 = ROAD_SIZE*2/3 */
	{
		ras_cal_x = ((num * stMyCar.Steering) >> 4) + (g_stRoadInfo.angle * (uRas_mid - uRas_st) / num);
	}
#endif
	else
	{
		SS	Point_x = 0;
		SS	Point_x2 = 0;
		SS	Speed = 0;
		ST_CARDATA stMyCar = {0};

		GetMyCar(&stMyCar);			/* 自車の情報を取得 */
		GetMyCarSpeed(&Speed);

		/* スクロールする幅×計算する高さ÷ラスタースクロールさせる高さ */
		Point_x = stMyCar.Steering + ((SS)(g_stRoadInfo.angle * Speed) >> 6);	/* バランス調整要 */
		Point_x2 = ((Num * Point_x) >> 4) + ((g_stRoadInfo.angle * g_stRasInfo.size) / Num);	
		
		*x = Mmax( Mmin( (Point_x2), 256), -256);
	}
	
	/* 上下限クリップ */
	if(*x < 0)
	{
		*x = 512 + *x;
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

	if(Num == 0u)	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
	{
		*y = 0;
		Road_Pat_Start();
		bRet = 0x00;
	}
	else
	{
		SS	Point_y;
		SS	MinClip = 0;
		SS	MaxClip = Mmax( Mmin((RASTER_MAX - RASTER_MIN - RASTER_NEXT - g_stRasInfo.st - Num), (ROAD_SIZE-1)), 0);
		
		if( g_stRasInfo.st < g_stRasInfo.mid )	/* 水平線の位置が通常より上側の処理 */
		{
			SS	Road_strch = 0;
			UI	uCal;
			
			uCal = Mmax(APL_uDiv(Num, g_stRasInfo.size), 1);
			Road_strch = g_stRoadInfo.Horizon_tmp + APL_sDiv(g_stRoadInfo.height, uCal);

			RoadPoint_y = Mmax(g_stRoadInfo.Horizon_tmp, Road_strch);
			bRet = 0xFF;
		}
		else if( g_stRasInfo.st == g_stRasInfo.mid )	/* 水平線の位置 */
		{
			RoadPoint_y = g_stRoadInfo.Horizon;
			bRet = 0xFF;
		}
		else	/* 水平線の位置が通常より下側の処理 */
		{
			SS	Road_strch = 0;

			Road_strch = APL_uDiv(g_stRasInfo.size, Num);
			RoadPoint_y = g_stRoadInfo.Horizon - Road_strch;

			bRet = 0xb7;
		}
		
		Point_y = (RoadPoint_y - RASTER_MIN + (g_stRasInfo.st + Num) - ROAD_ST_POINT - RASTER_NEXT);
		
		if(Point_y < 0)
		{
			MinClip = Mmax( Mmin( (RoadPoint_y + (0 - Point_y)), (ROAD_SIZE-1) ), 0);
		}
		*y = Mmax(Mmin(RoadPoint_y, MaxClip), MinClip);	/* 上下限クリップ */
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
	g_stRoadInfo.Horizon_tmp	= 0x00;	/* 仮水平位置 */
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
		g_stRoadInfo.offset_y = (Num * g_stRoadInfo.offset_val) / Mmax((g_stRoadInfo.offset_val + 96), 1);
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
	road_slope = g_stRoadInfo.height * -1;
	
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
			cal_tan = APL_Tan256(road_slope);	/* tanθ */
			cal_cos = APL_Cos(cal_tan);			/* cosθ */
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
			Horizon = Mmax(Mmin(Y_HORIZON_0 - Height, ROAD_0_MAX), ROAD_0_MIN);	/* 水平位置決定（クリップ） */
			g_stRoadInfo.Horizon_tmp = Y_HORIZON_0;
			break;
		}
		case 1:		/* FPS */
		default:	/* FPS */
		{
			Horizon = Mmax(Mmin(Y_HORIZON_1 - Height, ROAD_1_MAX), ROAD_1_MIN);	/* 水平位置決定（クリップ） */
			g_stRoadInfo.Horizon_tmp = Y_HORIZON_1;
			break;
		}
	}
	g_stRoadInfo.Horizon = Horizon;
	
	return ret;
}

#endif	/* RASTER_C */

