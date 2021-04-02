#ifndef	MyCar_C
#define	MyCar_C

#include <limits.h>
#include <iocslib.h>

#include "inc/usr_macro.h"
#include "inc/ZMUSIC.H"

#include "MyCar.h"
#include "OverKata.h"
#include "APL_Math.h"
#include "APL_MACS.h"
#include "Course_Obj.h"
#include "CRTC.h"
#include "Draw.h"
#include "Input.h"
#include "EnemyCAR.h"
#include "FileManager.h"
#include "Graphic.h"
#include "MFP.h"
#include "Music.h"
#include "Output_Text.h"
#include "PCG.h"
#include "Raster.h"
#include "Text.h"

/* グローバル変数 */
SS	g_speed = 0;

/* 構造体定義 */
ST_CARDATA	g_stMyCar = {0};

/* 関数のプロトタイプ宣言 */
SS	MyCar_G_Load(void);
SS	GetMyCar(ST_CARDATA *stDat);
SS	SetMyCar(ST_CARDATA stDat);
SS	MyCarInfo_Init(void);
SS	MyCarInfo_Update(SS);
SS	MyCar_Interior(UC);
SS	MyCar_CourseOut(void);
SS	GetMyCarSpeed(SS *);
void MyCar_Image(void);
void MyCar_Background(void);
static SS	MyCar_Vibration(void);
static SS	MyCar_Mascot(SS);
static SS	MyCar_Tachometer(SS);

/* 関数 */
SS	MyCar_G_Load(void)
{
	SS	ret = 0;
	
	MyCar_Background();	/* 背景の表示 */

	MyCar_Image();		/* 自車の表示 */

	return ret;
}

SS	GetMyCar(ST_CARDATA *stDat)
{
	SS	ret = 0;
	*stDat = g_stMyCar;
	return ret;
}

SS	SetMyCar(ST_CARDATA stDat)
{
	SS	ret = 0;
	g_stMyCar = stDat;
	return ret;
}

SS	MyCarInfo_Init(void)
{
	SS	ret = 0;
	
	g_stMyCar.ubCarType		= 0u;	/* 車の種類 */
	g_stMyCar.uEngineRPM	= 0u;	/* エンジン回転数 */
	g_stMyCar.VehicleSpeed	= 0;	/* 車速 */
	g_stMyCar.Steering		= 0;	/* ステア */
	g_stMyCar.ubShiftPos	= 0u;	/* ギア段 */
	g_stMyCar.ubThrottle	= 0u;		/* スロットル開度 */
	g_stMyCar.ubBrakeLights	= FALSE;	/* ブレーキライト */
	g_stMyCar.ubHeadLights	= FALSE;	/* ヘッドライト */
	g_stMyCar.ubWiper		= FALSE;	/* ワイパー */
	g_stMyCar.bTire			= 0;	/* タイヤの状態 */
	g_stMyCar.ubOBD			= OBD_NORMAL;	/* 故障の状態 */
	
	return ret;
}

SS	MyCarInfo_Update(SS input)
{
	SS	ret = 0;
	SS	speed_min;
	SS	Slope;
	SS	Angle;
	SS	myCarSx, myCarEx, myCarSy, myCarEy;

	US	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
//	US	uTM_F[6] = { 0, 13390, 9140, 6768, 5137, 3567 };/* 総減速比 */
	US	uRPM[11] = { 500, 1000, 1500, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000 };	/* Eng回転数 */
	US	uTRQ[11] = { 120,  130,  150,  170,  210,  200,  180,  170,  140,  120,  100 };	/* トルク */

	UC	bAxis;
	UC	bUpDown = 0;
	UC	bHit = FALSE;
	UI	time;
	UI	i;
	UI	uEnemyNum = 0;
	UC bMode;

	ST_CRT			stCRT;
	ST_ROAD_INFO	stRoadInfo;
	ST_ENEMYCARDATA	stEnemyCar = {0};

	static UC bShiftPosFlag[3] = {FALSE};
	static UC bSpin = 0;

#ifdef DEBUG	/* デバッグコーナー */
	UC	bDebugMode;
	US	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	GetGameMode(&bMode);
	GetCRT(&stCRT, bMode);
	GetRoadInfo(&stRoadInfo);
	Slope = stRoadInfo.slope;
	Angle = stRoadInfo.angle;

	GetStartTime(&time);	/* 開始時刻を取得 */
	
	/* ステアリング操作 */
	if((input & KEY_RIGHT) != 0u)
	{
		g_stMyCar.Steering += g_speed << 4;	/* 右 */
		g_stMyCar.uEngineRPM -= 3;	/* パワステ駆動減 */

		if(bSpin == 1)	/* 右スピン */
		{
			bSpin = 0;	/* スピンキャンセル */
			ADPCM_Play(4);	/* ブレーキ音 */
		}
	}
	else if((input & KEY_LEFT) != 0u)
	{
		g_stMyCar.Steering -= g_speed << 4;	/* 左 */
		g_stMyCar.uEngineRPM -= 3;	/* パワステ駆動減 */

		if(bSpin == 2)	/* 左スピン */
		{
			bSpin = 0;	/* スピンキャンセル */
			ADPCM_Play(4);	/* ブレーキ音 */
		}
	}
	else
	{
		
	}
	
	if(g_stMyCar.ubOBD == OBD_DAMAGE)	/* 故障中 */
	{
		/* スピン発生 */
		if(bSpin == 1)		
		{
			g_stMyCar.Steering += g_speed << 3;	/* 右 */
		}
		else if(bSpin == 2)
		{
			g_stMyCar.Steering -= g_speed << 3;	/* 左 */
		}
		else
		{
			/* 保持 */
		}
	}
	/* 当たり判定の生成 */
	myCarSx = ROAD_CT_POINT + (g_stMyCar.Steering >> 8) - 8;
	myCarEx = myCarSx + 16;
	myCarSy = Y_MAX_WINDOW - 32;
	myCarEy = myCarSy + 16;
	
	/* シフト操作 */
	if( ((input & KEY_A) != 0u) && ((input & KEY_B) == 0u))	/* Aボタン(only) */
	{
		/* アクセルだけはシフト操作禁止 */
	}
	else{
		if(ChatCancelSW((input & KEY_UPPER)!=0u, &bShiftPosFlag[0]) == TRUE)
		{
			if(g_stMyCar.ubShiftPos != 5)
			{
				bUpDown = 1;
				ADPCM_Play(7);	/* SE:シフトアップ */
			}
			g_stMyCar.ubShiftPos = Mmin(g_stMyCar.ubShiftPos+1, 5);	/* 上 */
		}
		
		if(ChatCancelSW((input & KEY_LOWER)!=0u, &bShiftPosFlag[1]) == TRUE)
		{
			if(g_stMyCar.ubShiftPos != 0)
			{
				bUpDown = 2;
				ADPCM_Play(7);	/* SE:シフトダウン */
			}
			g_stMyCar.ubShiftPos = Mmax(g_stMyCar.ubShiftPos-1, 0);	/* 下 */
		}
	}
#if 0
	g_stMyCar.ubShiftPos = 2;	/* テスト用変速固定 */
#endif	
	g_stMyCar.ubShiftPos = Mmax(Mmin(g_stMyCar.ubShiftPos, 5), 0);
	
	if(bUpDown != 0u) 
	{
		g_stMyCar.uEngineRPM = (g_stMyCar.VehicleSpeed * uTM[g_stMyCar.ubShiftPos]) / 26;
		g_stMyCar.uEngineRPM = Mmax(Mmin(9000, g_stMyCar.uEngineRPM), 750);
	}
	
	/* 回転数軸算出 */
	bAxis = 10;
	for( i=0; i<10; i++ )
	{
		if( g_stMyCar.uEngineRPM <= (uRPM[i] + ((uRPM[i+1] - uRPM[i]) >> 1)) )
		{
			bAxis = i;
			break;
		}
	}
	
	/* アクセル */
	if((input & KEY_A) != 0u)		/* Aボタン */
	{
		g_stMyCar.ubThrottle	= Minc(g_stMyCar.ubThrottle, 1u);	/* スロットル開度 */
		g_stMyCar.uEngineRPM += (uTM[g_stMyCar.ubShiftPos] + uTRQ[bAxis]) >> 7;		/* 回転数 */

		/* エンジンの音 */
		{
			static	UI	unExplosion_time = 0;
			SS	rpm;

			rpm	= Mmax(g_stMyCar.uEngineRPM, 1);
			
			if( (time - unExplosion_time) >  (150 - Mdiv64(rpm)) )	/* 回転数のエンジン音(60000 / rpm) */
			{
				unExplosion_time = time;
//				SE_Play_Fast(6);	/* FM効果音再生(高速) */
//				SE_Play(6);		/* FM効果音再生 */
			}
			else
			{
				/* 何もしない */
			}
		}
	}
	else{
		static	UI	unFuelCut_time = 0;
		SS	rpm;
		rpm	= Mmax(g_stMyCar.uEngineRPM, 1);
		
		g_stMyCar.ubThrottle = Mdec(g_stMyCar.ubThrottle, 1u);	/* スロットル開度 */
		g_stMyCar.uEngineRPM -= uTM[g_stMyCar.ubShiftPos] >> 8;		/* 回転数 */
		
		/* エンジンの音(燃料カット) */
		if( (time - unFuelCut_time) >  (150 - Mdiv64(rpm)) )	/* 回転数のエンジン音(60000 / rpm) */
		{
			unFuelCut_time = time;
			if(g_stMyCar.ubThrottle > 0)
			{
//				SE_Play(7);	/* FM効果音再生 */
			}
			else
			{
//				SE_Play(8);	/* FM効果音再生 */
			}
		}
	}
	
	/* ブレーキ */
	if( (input & KEY_B) != 0U )	/* Bボタン */
	{
		if(g_stMyCar.VehicleSpeed > 5)
		{
			ADPCM_Play(4);	/* ブレーキ音 */
		}
		g_stMyCar.VehicleSpeed = Mdec((US)g_stMyCar.VehicleSpeed, 1u);	/* 変速比で変えたい */
		g_stMyCar.ubBrakeLights = TRUE;	/* ブレーキランプ ON */
	}
	else
	{
		g_stMyCar.ubBrakeLights = FALSE;	/* ブレーキランプ OFF */
	}
	
	/* 衝突判定 */
	if(		(g_stMyCar.VehicleSpeed != 0) 		/* 車速あり */
		&&  (g_stMyCar.ubOBD == OBD_NORMAL) )	/* 故障していない */
	{
		for(i = 0; i < ENEMYCAR_MAX; i++)
		{
			GetEnemyCAR(&stEnemyCar, i);
			if( stEnemyCar.ubAlive == TRUE )
			{
				if(    (myCarSx > stEnemyCar.sx)
					&& (myCarEx < stEnemyCar.ex)
					&& (myCarSy > stEnemyCar.sy)
					&& (myCarEy < stEnemyCar.ey))
				{
					/* 衝突判定 */
					bHit = TRUE;
					uEnemyNum = i;

					/* ステアリングの状態でスピン */
					if((myCarSx - stEnemyCar.sx) >= (stEnemyCar.ex - myCarEx) )
					{
						bSpin = 1;	/* 右スピン */
					}
					else
					{
						bSpin = 2;	/* 左スピン */
					}
				}
			}
		}
	}
	
	/* 衝突判定 */
	if( bHit == TRUE )
	{
		g_stMyCar.ubOBD = OBD_DAMAGE;	/* 故障 */
		g_stMyCar.uEngineRPM = g_stMyCar.uEngineRPM >> 1;		/* 1/2 */
	}

	/* 回転数クリップ */
	g_stMyCar.uEngineRPM = Mmax(Mmin(9000, g_stMyCar.uEngineRPM), 750);

	/* 車速 */
	if(g_stMyCar.ubShiftPos == 0u)		/* ニュートラル */
	{
		g_stMyCar.VehicleSpeed = Mdec((US)g_stMyCar.VehicleSpeed, 1u);
	}
	else
	{
		if(g_stMyCar.ubBrakeLights == TRUE)
		{
			/* 車速からエンジン回転数を算出 */
			g_stMyCar.uEngineRPM = (g_stMyCar.VehicleSpeed * uTM[g_stMyCar.ubShiftPos] ) / 26u;	
			/* 回転数クリップ */
			g_stMyCar.uEngineRPM = Mmax(Mmin(9000, g_stMyCar.uEngineRPM), 750);
		}
		/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 減速比 4.687 タイヤ周長2052.1mm */
		/* タイヤ周長×６０×回転数／（１０００×変速比×減速比） */
		g_stMyCar.VehicleSpeed = (SS)(((UI)26 * g_stMyCar.uEngineRPM) / uTM[g_stMyCar.ubShiftPos]);	
	}
	/* 車速クリップ */
	g_stMyCar.VehicleSpeed = Mmax(Mmin(310, g_stMyCar.VehicleSpeed), 0);

	/* 車速（ゲーム内） */
	if( (g_stMyCar.ubBrakeLights == TRUE)		/* ブレーキランプON */
	||  (g_stMyCar.ubShiftPos == 0u)		)	/* ニュートラル */
	{
		speed_min = 0;
	}
	else
	{
		speed_min = 1;
	}

	/* 車速（ゲーム内） */
	g_speed = g_stMyCar.VehicleSpeed >> 3;	/* 1LSB 10km/h */
	g_speed = Mmax(Mmin(g_speed, 31), speed_min);

	/* コーナリング */
#if 1
	if( g_speed != 0u )
	{
		if(Angle != 0)
		{
			g_stMyCar.Steering += Angle * g_speed;	/* バランス調整要 */
		}
//		else if(Angle < 0)
//		{
//			g_stMyCar.Steering -= Angle * g_speed;	/* バランス調整要 */
//		}
		else
		{
			/* 前回値保持 */
		}
	}
	else{
		/* 前回値保持 */
	}
#endif
	
	/* ステアリングクリップ */
	g_stMyCar.Steering = Mmax(Mmin(g_stMyCar.Steering, 3800), -3800);

	/* 衝突判定 */
	if( bHit == TRUE )
	{
		GetEnemyCAR(&stEnemyCar, uEnemyNum);
		stEnemyCar.VehicleSpeed = g_stMyCar.VehicleSpeed + 5;
		SetEnemyCAR(stEnemyCar, uEnemyNum);	/* 敵車の更新 */
	}

	ret = (SS)bHit;
	
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)
	{
		US color;
		switch(g_stMyCar.ubOBD)
		{
			case OBD_NORMAL:
			{
				color = 0x03;
				break;
			}
			case OBD_DAMAGE:
			{
				color = 0x13;
				break;
			}
			case OBD_SPIN:
			{
				color = 0x19;
				break;
			}
			case OBD_COURSEOUT:
			{
				color = 0x08;
				break;
			}
			default:
			{
				color = 0x14;
				break;
			}
		}
		
		Draw_Box(
			stCRT.hide_offset_x + myCarSx,
			stCRT.hide_offset_y + myCarSy,
			stCRT.hide_offset_x + myCarEx,
			stCRT.hide_offset_y + myCarEy, color, 0xFFFF);
	}
#endif
	
	return ret;
}

SS	MyCar_Interior(UC bMode)
{
	SS	ret = 0;

	SS	Vibration = 0;

	/* 車体を振動させる */
	Vibration = MyCar_Vibration();
	
	/* マスコット *//* 処理負荷 大 */
//	MyCar_Mascot(Vibration);
	
	/* タコメーター針 */
	MyCar_Tachometer(Vibration);
	
	/* ハンドル */
	/* 未実装 */
	
	return ret;
}


SS	MyCar_CourseOut(void)
{
	SS	ret = 0;
	SS	vx;
	vx	= g_stMyCar.Steering >> 8;

	if( Mabs(vx) > 255 )	/* コース外 */
	{
		SS	rpm	= g_stMyCar.uEngineRPM;
		
		g_stMyCar.uEngineRPM -= (rpm>>3);	/* 減速処理 */

		g_stMyCar.ubOBD = OBD_COURSEOUT;	/* コースアウト */
	}
	else
	{
		if(g_stMyCar.ubOBD == OBD_COURSEOUT)
		{
			g_stMyCar.ubOBD = OBD_NORMAL;	/* 通常 */
		}
	}
	
	return ret;
}

SS	GetMyCarSpeed(SS *speed)
{
	SS	ret = 0;
	
	*speed = g_speed;

	return ret;
}

void MyCar_Image(void)
{
	/* FPS */
#if 1	/* テキスト */
	PutGraphic_To_Text( 0, 0, 0			);	/* インテリア */
	PutGraphic_To_Text( 0, 0, Y_OFFSET	);	/* インテリア */

	T_Fill( 90,        0 + 180, 32, 31, 0, 0);		/* メーター穴（上） */
	T_Fill( 90, Y_OFFSET + 180, 32, 31, 0, 0);		/* メーター穴（下） */

	T_Fill(  0,        0 + 224, 255, 2, 0, 0x0F);	/* 下帯（上） */
	T_Fill(  0, Y_OFFSET + 224, 255, 2, 0, 0x0F);	/* 下帯（下） */
#else
	SS x;

	G_Load_Mem( 0, X_OFFSET,	0,			0);	/* インテリア */
	G_Load_Mem( 0, X_OFFSET,	Y_OFFSET,	0);	/* インテリア */
	/* メーターの穴 */
	for(x = 0; x < 17; x++)
	{
		Draw_Circle(X_OFFSET + 106,				195,	x, TRANS_PAL, 0, 360, 255);	/* 穴をあける */
		Draw_Circle(X_OFFSET + 106, Y_OFFSET +	195,	x, TRANS_PAL, 0, 360, 255);	/* 穴をあける */
	}
	/* 余白の塗りつぶし */
	Draw_Fill(X_OFFSET, (       0 + MY_CAR_1_H), X_OFFSET + MY_CAR_1_W,        0 + V_SYNC_MAX, 0x01);
	Draw_Fill(X_OFFSET, (Y_OFFSET + MY_CAR_1_H), X_OFFSET + MY_CAR_1_W, Y_OFFSET + V_SYNC_MAX, 0x01);

	Draw_Fill(X_OFFSET + 90, 			180 + 1,	X_OFFSET + 90 + 31, 			180 + 31 - 1,	 TRANS_PAL);	/* 穴をあける */
	Draw_Fill(X_OFFSET + 90, Y_OFFSET + 180 + 1,	X_OFFSET + 90 + 31, Y_OFFSET +	180 + 31 - 1,	 TRANS_PAL);	/* 穴をあける */
#endif
	

#if 0	/* TPS */
#else
#endif
}

void MyCar_Background(void)
{
	SS ret;
	US height_sum = 0u;
	US height_sum_o = 0u;
	UI i;
	UI uWidth, uHeight, uFileSize;
	UI uWidth_o, uHeight_o;
	UI uOffset_X = 0u;
	
	ret = G_Load_Mem( ENEMYCAR_CG, uOffset_X,	0,	0);	/* ライバル車 */
	Get_PicImageInfo( ENEMYCAR_CG, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	uWidth_o = uWidth;
	uHeight_o = uHeight;

	if(ret >= 0)
	{
		/* 11パターンを作る */
		UI	uW_tmp, uH_tmp;
		
		height_sum = 0;
		height_sum_o = 0;

		for(i=1; i<=11u; i++)
		{
			/* 縮小先のサイズ */
			height_sum_o += uHeight_o;
			uW_tmp = uWidth_o << 3;
			uWidth_o = Mmul_p1(uW_tmp);
			uH_tmp = uHeight_o << 3;
			uHeight_o = Mmul_p1(uH_tmp);
			
			/* 描画 */
			G_Stretch_Pict(
							0 + uOffset_X,	uWidth_o + uOffset_X,
							height_sum_o,	uHeight_o,
							0,
							0 + uOffset_X,	uWidth + uOffset_X,
							height_sum,		uHeight,
							0);
			/* 次の縮小元 */
			height_sum += uHeight;
			uWidth = uWidth_o;
			uHeight = uHeight_o;
		}
	}
	
	uOffset_X = 140;
	ret = G_Load_Mem( COURSE_OBJ_CG, uOffset_X,	0,	0);	/* ヤシの木 */
	Get_PicImageInfo( COURSE_OBJ_CG, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	uWidth_o = uWidth;
	uHeight_o = uHeight;
	
	if(ret >= 0)
	{
		/* 9パターンを作る */
		UI	uW_tmp, uH_tmp;
		
		height_sum = 0;
		height_sum_o = 0;

		for(i=1; i<9; i++)
		{
			/* 縮小先のサイズ */
			height_sum_o += uHeight_o;
			uW_tmp = uWidth_o << 3;
			uWidth_o = Mmul_p1(uW_tmp);
			uH_tmp = uHeight_o << 3;
			uHeight_o = Mmul_p1(uH_tmp);

			/* 描画 */
			G_Stretch_Pict(
							0 + uOffset_X,	uWidth_o + uOffset_X,
							height_sum_o,	uHeight_o,
							0,
							0 + uOffset_X,	uWidth + uOffset_X,
							height_sum,		uHeight,
							0);

			/* 次の縮小元 */
			height_sum += uHeight;
			uWidth = uWidth_o;
			uHeight = uHeight_o;
		}
	}

	Get_PicImageInfo( BG_CG, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	G_Load_Mem( BG_CG, 		0,		Y_MIN_DRAW +        0 + Y_HORIZON_1 - uHeight + 8,	1);	/* 背景 */
	G_Load_Mem( BG_CG, X_OFFSET+16,	Y_MIN_DRAW +        0 + Y_HORIZON_1 - uHeight + 8,	1);	/* 背景 */
	G_Load_Mem( BG_CG, 		0,		Y_MIN_DRAW + Y_OFFSET + Y_HORIZON_1 - uHeight + 8,	1);	/* 背景 */
	G_Load_Mem( BG_CG, X_OFFSET+16,	Y_MIN_DRAW + Y_OFFSET + Y_HORIZON_1 - uHeight + 8,	1);	/* 背景 */
}

static SS	MyCar_Vibration(void)
{
	SS ret = 0;

	SS	x, y;
	UC	bMode;
	ST_CRT	stCRT;

	static	SS	Vibration = 0;
	static	SS	VibrationCT = 0;
	static	UC	ubOBD_old = OBD_NORMAL;
	static	SS	CrashCount;
	
	GetGameMode(&bMode);
	GetCRT(&stCRT, bMode);

	/* 画面を揺らす */
	VibrationCT++;
	if(VibrationCT > 5)
	{
		VibrationCT = 0;
	}
	
	if((g_stMyCar.ubOBD & (OBD_DAMAGE|OBD_COURSEOUT)) != 0u)
	{
		if(ubOBD_old == OBD_NORMAL)
		{
			ADPCM_Play(12);	/* SE：クラッシュ */
			CrashCount = 30;
		}
		
		if((g_stMyCar.ubOBD & OBD_DAMAGE) != 0u)
		{
			if(CrashCount == 22)	/* 次のSEへ */
			{
				ADPCM_Play(15);	/* スキール音 */
			}
			CrashCount = Mdec(CrashCount, 1u);	/* カウンタデクリメント */

			if(CrashCount == 0)
			{
				/* 動画 */
				//MOV_Play(3);	/* 嘘をつくな */
				g_stMyCar.ubOBD = OBD_NORMAL;	/* 正常 */
			}
		}
		Vibration = (VibrationCT == 0)?8:0;	/* 画面の振動 */
	}
	else
	{
		Vibration = (VibrationCT == 0)?1:0;	/* 画面の振動 */
	}
	ubOBD_old = g_stMyCar.ubOBD;	/* 前回値更新 */

	/* 画面をフリップする */
	GetCRT(&stCRT, bMode);
	x = stCRT.view_offset_x;
	y = stCRT.view_offset_y;
	T_Scroll( 0, y + Vibration   );	/* テキスト画面 */
	
	ret = Vibration;
	
	return ret;
}

static SS	MyCar_Mascot(SS Vibration)
{
	SS ret = 0;
	
	SS	x, y;
	US	nRatio = 0x80;
	UC	palNum = 0;
	UC	sp_num=0;
	
	volatile US *PCG_src  = (US *)0xEBA000;
	volatile US *PCG_dst  = (US *)0xEBA180;

	static	US	rad = 180;
	static	UC	ubRadFlag = TRUE;

#ifdef	DEBUG
	GetDebugNum(&nRatio);
#endif

	/* マスコットが揺れる */
	if(g_stMyCar.VehicleSpeed == 0)
	{
		if(rad == 180)
		{
			rad = 180;
		}
		else if(rad < 180)
		{
			rad++;
		}
		else
		{
			rad--;
		}
	}
	else
	{
		US	width;
		
		width = 15 + (g_stMyCar.VehicleSpeed / 10);
		
		if(ubRadFlag == TRUE)
		{
			rad+=Mmax(g_stMyCar.VehicleSpeed/30, 1);
			if(rad > 180 + width)ubRadFlag = FALSE;
		}
		else
		{
			rad-=Mmax(g_stMyCar.VehicleSpeed/30, 1);
			if(rad < 180 - width)ubRadFlag = TRUE;
		}
	}
	

	x = 16 + (( 32 * APL_Sin(rad)) >> 8);
	y =  0 + ((-32 * APL_Cos(rad)) >> 8) - Vibration;

	palNum = 9;

	PCG_Rotation((US *)PCG_dst, (US *)PCG_src, 3, 3, x, y, &sp_num, palNum, (nRatio-0x80), 180-rad);

	return ret;
}


static SS	MyCar_Tachometer(SS Vibration)
{
	SS ret = 0;
	
	SS	i;
	UC	patNum = 0;
	UC	palNum = 0;
	UC	V=0, H=0;
	UC	sp_num=0;
	SS	x, y;

	/* タコメーター針 */
	for( i = 0; i < 18; i++ )
	{
		if( ((i*500) <= g_stMyCar.uEngineRPM) && (g_stMyCar.uEngineRPM < ((i*500)+500)) )
		{
			/* 回転数でスプライトのパターンを決める */
			break;
		}
	}
	palNum = 0x0D;
	x = 90+16;
	y = 180+16 - Vibration;
	if(i <= 6)
	{
		y += 16;
		patNum = 0x70+i;
		V = 1;
		H = 0;
	}
	else if(i <= 12)
	{
		patNum = 0x70+12-i;
		V = 0;
		H = 0;
	}
	else
	{
		x += 16;
		patNum = 0x70+i-12;
		V = 0;
		H = 1;
	}
	
	SP_REGST( sp_num++, -1, x, y, SetBGcode(V, H, palNum, patNum), 3);

	/* タコメーター */
	x = 90+16;
	y = 180+16 - Vibration;
	V = 0;
	H = 0;
	palNum = 0x0D;

	patNum = 0x46;
	SP_REGST( sp_num++, -1, x + 0, y + 0, SetBGcode(V, H, palNum, patNum), 3);
	patNum = 0x47;
	SP_REGST( sp_num++, -1, x + 16, y + 0, SetBGcode(V, H, palNum, patNum), 3);
	patNum = 0x56;
	SP_REGST( sp_num++, -1, x + 0, y + 16, SetBGcode(V, H, palNum, patNum), 3);
	patNum = 0x57;
	SP_REGST( sp_num++, -1, x + 16, y + 16, SetBGcode(V, H, palNum, patNum), 3);

	return ret;
}

#endif	/* MyCar_C */

