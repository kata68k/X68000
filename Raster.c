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
static US	uRoadAnime;
static US	uRoadAnime_old;
static US	uRoadAnimeBase;
static US	uRoadAnimeBase_old;
static US	uRoadAnimeCount;
static US	uRoadCycleCount;
static US	uRoadCycleCountLamp;
static US	uCounter;

static UC	bRoad_Anime_flag = FALSE;
static UC	bRoadCycleCountRst = FALSE;
static UC	bRoadCycleCountLamp = TRUE;

/* 構造体定義 */
ST_RAS_INFO		g_stRasInfo = {0};
ST_ROAD_INFO	g_stRoadInfo = {0};
ST_ROADDATA		g_stRoadData[1024] = {0};

/* 関数のプロトタイプ宣言 */
void	Raster_Main(UC);
void	Road_Pat(void);
SS	GetRasterInfo(ST_RAS_INFO *);
SS	SetRasterInfo(ST_RAS_INFO);
UL	GetRoadDataAddr(void);
SS	SetHorizon(UC );

/* 関数 */
void Raster_Main(UC bMode)
{
	SS	cal_tan = 0;
	SS	view_offset_x, view_offset_y;
	SS	hide_offset_x, hide_offset_y;
	SS	BG_offset_x, BG_offset_y;
	SS	BG_under;

	US	uRas_y;
	US	uRas_tmp[256] = {0};
	US	uRas_tmp_sub[256] = {0};
	US	uRas_debug[256] = {0};
	US	uRas_st, uRas_mid, uRas_ed, uRas_size;

	US	uPal_tmp[256] = {0};

	ST_CRT		stCRT = {0};
	ST_CARDATA	stMyCar = {0};
	
#ifdef DEBUG	/* デバッグコーナー */
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

	GetMyCar(&stMyCar);			/* 自車の情報を取得 */

	/* 初期化 */
	uRas_st = 0;
	uRas_mid = 0;
	uRas_ed = 0;
	uRas_size = 0xFFFF;

	/* 空のコントロール */
	uPal_tmp[1] = BG_offset_y;

	/* 水平線の設定 */
	SetHorizon(bMode);
	
	/* ラスター終了位置 */
	if(bMode == 0)		/* TPS */
	{
		uRas_ed = Mmin(V_SYNC_MAX, RASTER_MAX);	/* ラスター終了位置 */
	}
	else
	{
		uRas_ed = Mmin(Y_HORIZON_1 + RASTER_MIN + ROAD_SIZE, V_SYNC_MAX);	/* ラスター終了位置 */
	}
	
	uRas_st = Mmax(Mmin(g_stRoadInfo.Horizon + RASTER_MIN, RASTER_MAX), RASTER_MIN);	/* ラスター開始位置 */
	uRas_st += (uRas_st % 2);	/* 偶数にする */
	uRas_ed -= (uRas_ed % 2);	/* 偶数にする */
	uRas_size = uRas_ed - uRas_st;			/* ラスターの範囲 */
	uRas_mid = Mmin((uRas_st + g_stRoadInfo.distance), uRas_ed - 16);	/* 水平線の位置が変化しないポイント *//* ハイウェイスターなら 64 = ROAD_SIZE*2/3 */
	uPal_tmp[0] = uRas_st;				/* 割り込み位置の設定 */
	
	g_stRasInfo.st = uRas_st;
	g_stRasInfo.mid = uRas_mid;
	g_stRasInfo.ed = uRas_ed;
	g_stRasInfo.size = uRas_size;
	g_stRasInfo.horizon = g_stRoadInfo.Horizon;
	SetRasterInfo(g_stRasInfo);
	
	memset(&uPal_tmp[0], 0, sizeof(uPal_tmp));
	memset(&uRas_tmp[0], 0, sizeof(uRas_tmp));

	for(uRas_y=uRas_st; uRas_y < uRas_ed; uRas_y+=RASTER_NEXT)
	{
		SS	num, rev;
		SS	ras_cal_x=0, ras_cal_y=0, ras_pat, ras_result;
		SS	uBG_pat[4][4] = {288,   0,  96, 192, 
			 				 192, 288,   0,  96, 
			 				  96, 192, 288,   0, 
			 				   0,  96, 192, 288} ;
#ifdef DEBUG	/* デバッグコーナー */
		SS	col;
#endif
		
		num = uRas_y - uRas_st;			/* 0 -> uRas_size */
		rev = uRas_ed - uRas_y;			/* uRas_size -> 0 */
		
		/* X座標 */
		if(num == 0u)
		{
			ras_cal_x = 0;	/* 最初 */
		}
#if 0
		else if( uRas_y < uRas_mid )				/* 水平線の位置が変化 *//* 64 = ROAD_SIZE*2/3 */
		{
			ras_cal_x = ((num * stMyCar.Steering) >> 4) + (g_stRoadInfo.angle * (uRas_mid - uRas_st) / num);
		}
#endif
		else
		{
			ras_cal_x = Mmax( Mmin( (((num * stMyCar.Steering) >> 4) + ((g_stRoadInfo.angle * uRas_size) / num)), 256), -256);
		}
#ifdef DEBUG	/* デバッグコーナー */
		if(bDebugMode == TRUE)	/* デバッグモード */
		{
			Draw_Pset(	view_offset_x + (WIDTH >> 1) - ras_cal_x,
						view_offset_y + g_stRoadInfo.Horizon + num,
						0xC2);	/* デバッグ用グラフ*/
		}
#endif
		if(ras_cal_x < 0)	/* 負 */
		{
			ras_cal_x = 1024 + ras_cal_x;
		}
		else
		{
		}
		uRas_tmp[uRas_y] = (US)ras_cal_x;

		/* Y座標 */
		if(num == 0u)	/* 初回は0or512でないと空に道が出てくる */	/* ぷのひと さんのアドバイス箇所 */
		{
			ras_cal_y = 0;
			ras_pat = uBG_pat[3][0];
			g_stRoadInfo.offset_y = 0;
			g_stRoadInfo.offset_val = 0;
			uRoadAnime_old = uRoadAnime;
			uRoadAnimeBase_old = uRoadAnimeBase;

#ifdef DEBUG	/* デバッグコーナー */
			col = 0xBB;
#endif
		}
#if 1
		else if( uRas_y >= (uRas_ed - 16))		/* 下側のはみ出し部分を補完 */
		{
			ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
			ras_cal_y = BG_under;

#ifdef DEBUG	/* デバッグコーナー */
			col = 0xC2;
#endif
		}
		else if( uRas_y < uRas_mid )				/* 水平線の位置が変化 *//* 64 = ROAD_SIZE*2/3 */
		{
			SS	Road_strch;
			SS	offset_size;

			offset_size = uRas_mid - uRas_y;	/* uRas_size -> 0 */
			
			Road_strch = (offset_size * cal_tan) >> 8;	/* 高さ＝底辺×tanθ */
//			Road_strch = (offset_size * APL_Cos(num)) >> 8;
//			Road_strch = (offset_size * APL_Sin(num)) >> 8;
			ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
			ras_cal_y = g_stRoadInfo.Horizon_offset + Road_strch + ((SS)uDebugNum - 0x80);

#ifdef DEBUG	/* デバッグコーナー */
			col = 0xB7;
#endif
		}
		else				/* 平坦 */
		{
			ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
			ras_cal_y = g_stRoadInfo.Horizon_offset;

#ifdef DEBUG	/* デバッグコーナー */
			col = 0xFF;
#endif
		}
#else
		else
		{
			SS	Road_strch;
			SS	offset_size;

			offset_size = uRas_ed - uRas_y;	/* uRas_size -> 0 */
//			offset_size = uRas_mid - uRas_y;	/* uRas_size -> 0 */
			
//			Road_strch = (offset_size * cal_tan) >> 8;	/* 高さ＝底辺×tanθ */
//			Road_strch = (offset_size * APL_Cos(num<<1)) >> 8;
			Road_strch = (offset_size * APL_Sin(num<<1)) >> 8;
			ras_pat = uBG_pat[uRoadAnimeBase][uRoadAnime];
			ras_cal_y = g_stRoadInfo.Horizon_offset + Road_strch + ((SS)uDebugNum - 0x80);

#ifdef DEBUG	/* デバッグコーナー */
			col = 0xB7;
#endif
		}
#endif
		/* ロードパターン*/
		g_stRoadInfo.offset_y = (num * g_stRoadInfo.offset_val) / (g_stRoadInfo.offset_val + 96);
		uRas_tmp_sub[uRas_y] = g_stRoadInfo.offset_y;

		uRoadAnimeCount++;
		if(uRoadAnimeCount >= g_stRoadInfo.offset_y)
		{
			uRoadAnimeCount = 0;
			uRoadAnime++;
			g_stRoadInfo.offset_val++;
		}
		if(uRoadAnime >= 4u){uRoadAnime = 0;}

#ifdef DEBUG	/* デバッグコーナー */
		if(bDebugMode == TRUE)	/* デバッグモード */
		{
			Draw_Line(uRas_st + view_offset_x, (g_stRoadInfo.Horizon+ view_offset_y)+1, uRas_ed + view_offset_x, (g_stRoadInfo.Horizon+ view_offset_y)+1, 0x35, 0xFFFF);
			Draw_Pset(uRas_y + view_offset_x, (g_stRoadInfo.Horizon + RASTER_MIN + view_offset_y) - ras_cal_y, col);	/* デバッグ用グラフ*/
			uRas_debug[uRas_y] = ras_cal_y;
		}
#endif
		ras_result = ras_cal_y + ras_pat;
		if(ras_result <   0)ras_result = 0;
		if(ras_result > 511)ras_result = 0;
		
		uPal_tmp[uRas_y] = (US)ras_result;
		
	}
	SetRasterVal(uRas_tmp, sizeof(US)*RASTER_MAX);
	SetRasterPal(uPal_tmp, sizeof(US)*RASTER_MAX);
#if 0
#ifdef DEBUG	/* デバッグコーナー */
	printf("uRas_tmp[0]の中身 = %d(uRas_st=%d)\n", uRas_tmp[0], uRas_st );
	for(uRas_y=uRas_st; uRas_y < uRas_ed; uRas_y++)
	{
		printf("[%3d]=(%5d),", uRas_y, uRas_tmp[uRas_y] );
		//printf("[%3d]=(%5d),", uRas_y, uRas_debug[uRas_y] );
		if((uRas_y%5) == 0)printf("\n");
	}
	printf("\n");
#endif
#endif
}

void Road_Pat(void)
{
	static UI	unRoadAnimeTime;
	UI	time_st;

	SS	cal_tan = 0;
	SS	cal_cos = 0;
	SS	speed;
	
	SS	r_height;
	
	SS	road_height;
	static SS	road_height_old;
	SS	road_slope;
	static SS	road_slope_old;
	SS	road_angle;
	static SS	road_angle_old;
	SS	road_distance;
	static SS road_distance_old;
	SS	road_object;

	ST_RAS_INFO	stRasInfo = {0};

	GetStartTime(&time_st);		/* 開始時間を取得 */
	GetRasterInfo(&stRasInfo);	/* ラスター情報を取得 */
	GetMyCarSpeed(&speed);		/* 車速を得る */

	road_height		= g_stRoadInfo.height;
	road_slope		= g_stRoadInfo.slope;
	road_angle		= g_stRoadInfo.angle;
	road_distance	= g_stRoadInfo.distance;
	road_object		= g_stRoadInfo.object;

	/* コーナーの表現 */
#if 0
	/* デバッグコーナー */
	/* ランプ操作 */
	if( ((uFreeRunCount % (RD[uFreeRunCount & 0x03FFu])) == 0)
		&& ((uFreeRunCount % 3) == 0)
		&& (speed != 0u) )
	{
		if(bRoad_flag == 0){
			road_angle += 1;
			if(road_angle > 12)bRoad_flag = 1;
		}
		else{
			road_angle -= 1;
			if(road_angle < -12)bRoad_flag = 0;
		}
	}
#endif
	
	/* 高さ */
#if 1
	r_height = 0;
#else
	if((input & KEY_b_RLUP)!=0u)	r_height += 1;	/* 上 */
	if((input & KEY_b_RLDN)!=0u)	r_height -= 1;	/* 下 */
	r_height = Mmax(Mmin(r_height, 31), -32);
#endif
	
	
	if(speed == 0u)	/* 車速0km/h */
	{
		uRoadAnime = uRoadAnime_old;
		uRoadAnimeBase = uRoadAnimeBase_old;
		uRoadAnimeCount = 0;
		bRoad_Anime_flag = FALSE;
		
		GetStartTime(&unRoadAnimeTime);	/* 開始時刻を取得 */
	}
	else if( (time_st - unRoadAnimeTime) < (362 / speed) )	/* 更新周期未満 */
	{
		/* センターラインの長さと間隔は５ｍ */
		/* 60fps=17ms/f */
		/* 最大速度307km/h(85m/s,0.0853m/ms) 1f=1.45m進む */
		/* 最低速度6km/h(1.7m/s,0.0017m/ms) 1f=0.028m進む */
		/* 1LSB 10km/h(2.8m/s,0.0028m/ms) 1f=0.047m進む */
		/* 1times Cal 1m=4dot (17ms/f x 1/0.047 / speed) = (362/speed)*/
		uRoadAnime = uRoadAnime_old;
		uRoadAnimeBase = uRoadAnimeBase_old;
		uRoadAnimeCount = 0;
		bRoad_Anime_flag = FALSE;
	}
	else
	{
		GetStartTime(&unRoadAnimeTime);	/* 開始時刻を取得 */

		uRoadAnimeCount = 0xFFF;	/* 逆走アニメーション防止 */

		uRoadAnime = 0;
		bRoad_Anime_flag = TRUE;

		uRoadAnimeBase++;
		if(uRoadAnimeBase >= 4)
		{	
			uRoadAnimeBase = 0;
		}
		
		if(bRoadCycleCountRst == TRUE)
		{
			bRoadCycleCountRst = FALSE;
			/* 角度算出 */
			road_height_old = road_height;	/* 前回値更新 */
			/* コースデータ読み込み */
			road_height = (SS)(0x80 - g_stRoadData[uCounter].bHeight);	/* 道の標高	(0x80センター) */
			road_height = Mmax(Mmin(road_height, 31), -32);
			//road_height = r_height;	/* デバッグ入力 */
			road_angle_old = road_angle;
			road_angle = (SS)g_stRoadData[uCounter].bAngle - 0x80;		/* 道の角度	(0x80センター) */
			//road_angle = Mmax(Mmin(road_angle, 12), -12);
			//road_angle = 0;	/* デバッグ入力 */
			road_object = g_stRoadData[uCounter].bObject;					/* 出現ポイントのオブジェクトの種類 */
			if(road_object != 0u)
			{
				SetAlive_EnemyCAR();	/* ライバル車を出現させる */
			}
			
			if(road_height == road_height_old)
			{
			}
			else if(road_height > road_height_old)
			{
				/* 保持 */
			}
			else
			{
				/* 保持 */
			}
			
			uRoadCycleCount = 0;
			
			if(bRoadCycleCountLamp == TRUE)
			{
				bRoadCycleCountLamp = FALSE;
			}
			else
			{
				bRoadCycleCountLamp = TRUE;
			}

			uCounter++;			/* カウンタ更新 */
			if(uCounter >= 896)uCounter = 0;
		}
		else
		{
			uRoadCycleCount += ((8 + uRoadCycleCount) / 8);
			if(uRoadCycleCount > (stRasInfo.size - 16))
			{
				bRoadCycleCountRst = TRUE;
			}
			SetRoadCycleCount(uRoadCycleCount);
			
			if(bRoadCycleCountLamp == TRUE)
			{
				uRoadCycleCountLamp += 1;
			}
			else
			{
				uRoadCycleCountLamp -= 1;
			}
		}
	}
	
	if(bRoad_Anime_flag == TRUE)
	{
		road_distance_old = road_distance;	/* 前回値更新 */
		road_distance = uRoadCycleCountLamp;

		// 高低差から傾きを出す(-45～+45) rad = APL_Atan2( road_slope_old - road_slope,  );
		if(road_height != road_height_old)
		{
			road_slope_old = road_slope;	/* 前回値更新 */
			road_slope = road_height * -1;

			/* 角度からtanθ*/
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
		}
		else{
			/* 前回値保持 */
		}
	}
	g_stRoadInfo.height		= road_height;		/*  */
	g_stRoadInfo.slope		= road_slope;		/*  */
	g_stRoadInfo.angle		= road_angle;		/*  */
	g_stRoadInfo.distance	= road_distance;	/*  */
	g_stRoadInfo.object		= road_object;		/*  */
	
#if 0
#ifdef DEBUG	/* デバッグコーナー */
	Message_Num(&uCounter,			12,  8, 2, MONI_Type_US, "%4d");
#endif
#endif
}

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

UL GetRoadDataAddr(void)
{
	return (UL)&g_stRoadData[0];
}

SS	SetHorizon(UC bMode)
{
	SS	ret = 0;
	SS	Horizon=0, Horizon_tmp, Horizon_offset;

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
			Horizon_tmp = Y_HORIZON_0-16;		/* 仮水平位置 */
			Horizon = Mmax(Mmin(Horizon_tmp + g_stRoadInfo.height, Y_HORIZON_0-RASTER_NEXT), RASTER_MIN);	/* 水平位置 */
			break;
		}
		case 1:		/* FPS */
		default:	/* FPS */
		{
			Horizon_tmp = Y_HORIZON_1;		/* 仮水平位置 */
			Horizon = Mmax(Mmin(Horizon_tmp + g_stRoadInfo.height, Y_HORIZON_1+32-RASTER_NEXT), RASTER_MIN);	/* 水平位置 *//* 30 = ROAD_SIZE*1/3-RASTER_NEXT */
			break;
		}
	}
	Horizon_offset = ROAD_POINT - Horizon_tmp;	/* 実際のデータと表示位置との差異 */
	
	g_stRoadInfo.Horizon		= Horizon;
	g_stRoadInfo.Horizon_tmp	= Horizon_tmp;
	g_stRoadInfo.Horizon_offset	= Horizon_offset;
	
	return ret;
}

#endif	/* RASTER_C */

