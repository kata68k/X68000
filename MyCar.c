#ifndef	MyCar_C
#define	MyCar_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "inc/ZMUSIC.H"

#include "MyCar.h"
#include "OverKata.h"
#include "APL_Math.h"
#include "APL_MACS.h"
#include "Course_Obj.h"
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
	g_stMyCar.ubOBD			= FALSE;	/* 故障の状態 */
	
	return ret;
}

SS	MyCarInfo_Update(SS input)
{
	SS	ret = 0;
	SS	speed_min;
	SS	Slope;
	SS	Angle;
	static UC bShiftPosFlag[3] = {FALSE};
	UI	time;

	US	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
//	US	uTM_F[6] = { 0, 13390, 9140, 6768, 5137, 3567 };/* 総減速比 */
//	US	uRPM[11] = { 500, 1000, 1500, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000 };	/* Eng回転数 */
//	US	uTRQ[11] = { 120,  130,  150,  170,  210,  200,  180,  170,  140,  120,  100 };	/* トルク */
	UC	bUpDown = 0;
	UI	i;
	ST_ENEMYCARDATA	stEnemyCar = {0};
	ST_ROAD_INFO	stRoadInfo;
	
	GetRoadInfo(&stRoadInfo);
	Slope = stRoadInfo.slope;
	Angle = stRoadInfo.angle;

	GetStartTime(&time);	/* 開始時刻を取得 */
	
	/* ステアリング操作 */
	if((input & KEY_RIGHT) != 0u)	g_stMyCar.Steering += 1;	/* 右 */
	if((input & KEY_LEFT) != 0u)	g_stMyCar.Steering -= 1;	/* 左 */
#if 0
	if( g_speed != 0u )
	{
		g_stMyCar.Steering += ((SS)(Angle + g_speed) >> 3);	/* バランス調整要 */
	}
	else{
		/* 前回値保持 */
	}
#endif
	g_stMyCar.Steering = Mmax(Mmin(g_stMyCar.Steering, 31), -31);
	
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
	
	/* アクセル */
	if((input & KEY_A) != 0u)		/* Aボタン */
	{
		g_stMyCar.ubThrottle	= Minc(g_stMyCar.ubThrottle, 1);	/* スロットル開度 */
		
		switch(g_stMyCar.ubShiftPos)
		{
			case 0:
			{
				g_stMyCar.uEngineRPM += 200;
			}
			break;
			case 1:
			{
				g_stMyCar.uEngineRPM += Mmax(20 - Slope,10);
			}
			break;
			case 2:
			{
				g_stMyCar.uEngineRPM += Mmax(15 - Slope,7);
			}
			break;
			case 3:
			{
				g_stMyCar.uEngineRPM += Mmax(10 - Slope,5);
			}
			break;
			case 4:
			{
				g_stMyCar.uEngineRPM += Mmax(6 - Slope,3);
			}
			break;
			case 5:
			{
				g_stMyCar.uEngineRPM += Mmax(3 - Slope,1);
			}
			break;
			default:
			{
				g_stMyCar.uEngineRPM += Mmax(100 - Slope, 50);
			}
			break;
		}

		/* エンジンの音 */
		{
			static	UI	unExplosion_time = 0;
			SS	rpm;

			rpm	= Mmax(g_stMyCar.uEngineRPM, 1);
			
			if( (time - unExplosion_time) >  (60000 / rpm) )	/* 回転数のエンジン音 */
			{
				unExplosion_time = time;
	//			ADPCM_Play(11);	/* 回転数のエンジン音 */
	//			SE_Play(1);			/* FM効果音再生 */
	//			SE_Play_Fast(1);	/* FM効果音再生 */
			}
			else
			{
				/* 何もしない */
			}
		}
	}
	else{
		g_stMyCar.ubThrottle	= Mdec(g_stMyCar.ubThrottle, 1);	/* スロットル開度 */

		/* 回転数 */
		switch(g_stMyCar.ubShiftPos)
		{
			case 0:
			{
				g_stMyCar.uEngineRPM -= 200;
			}
			break;
			case 1:
			{
				g_stMyCar.uEngineRPM -= Mmax(40 + Slope, 20);
			}
			break;
			case 2:
			{
				g_stMyCar.uEngineRPM -= Mmax(30 + Slope, 15);
			}
			break;
			case 3:
			{
				g_stMyCar.uEngineRPM -= Mmax(20 + Slope, 10);
			}
			break;
			case 4:
			{
				g_stMyCar.uEngineRPM -= Mmax(12 + Slope, 6);
			}
			break;
			case 5:
			{
				g_stMyCar.uEngineRPM -= Mmax(6 + Slope, 3);
			}
			break;
			default:
			{
				g_stMyCar.uEngineRPM -= Mmax(200 + Slope, 100);
			}
			break;
		}
	}

	g_stMyCar.uEngineRPM = Mmax(Mmin(9000, g_stMyCar.uEngineRPM), 750);

	/* 車速 */
	if(g_stMyCar.ubShiftPos == 0u)		/* ニュートラル */
	{
		g_stMyCar.VehicleSpeed -= 1;
	}
	else
	{
		/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 減速比 4.687 タイヤ周長2052.1mm */
		/* タイヤ周長×６０×回転数／（１０００×変速比×減速比） */
		g_stMyCar.VehicleSpeed = (SS)(((UI)26 * g_stMyCar.uEngineRPM) / uTM[g_stMyCar.ubShiftPos]);	
	}
	
	/* ブレーキ */
	if( (input & KEY_B) != 0U )	/* Bボタン */
	{
		if(g_stMyCar.VehicleSpeed > 5)
		{
			ADPCM_Play(4);	/* ブレーキ音 */
		}
		g_stMyCar.VehicleSpeed -= 30;	/* 変速比で変えたい */
		g_stMyCar.ubBrakeLights = TRUE;	/* ブレーキランプ ON */
	}
	else
	{
		g_stMyCar.ubBrakeLights = FALSE;	/* ブレーキランプ OFF */
	}
	
	/* 衝突判定 */
	if(g_stMyCar.VehicleSpeed != 0)
	{
		UC bMode;
		SS	myCarSx, myCarEx, myCarSy, myCarEy;
		ST_CRT		stCRT;
		
		GetGameMode(&bMode);
		GetCRT(&stCRT, bMode);

		myCarSx = ((WIDTH>>1) + g_stMyCar.Steering - 8);
		myCarEx = myCarSx + 16;
		myCarSy = Y_MAX_WINDOW - 16;
		myCarEy = myCarSy;

		for(i = 0; i < ENEMYCAR_MAX; i++)
		{
			GetEnemyCAR(&stEnemyCar, i);
			if( stEnemyCar.ubAlive == TRUE )
			{
				/* 当たり判定 */
				UC bHit = FALSE;
				
				if(    (myCarSx > stEnemyCar.sx)
					&& (myCarEx < stEnemyCar.ex)
					&& (myCarSy > stEnemyCar.sy)
					&& (myCarEy < stEnemyCar.ey))
				{
					bHit = TRUE;
				}
				
				if( bHit == TRUE )
				{
					g_stMyCar.ubOBD = TRUE;	/* 故障 */
					g_stMyCar.VehicleSpeed = g_stMyCar.VehicleSpeed >> 2;	/* 1/4 */
					g_stMyCar.uEngineRPM = g_stMyCar.uEngineRPM >> 2;		/* 1/4 */
					ret = (SS)bHit;
				}
			}
		}
		
#ifdef DEBUG	/* デバッグコーナー */
		{
			Draw_Box(
				stCRT.hide_offset_x + myCarSx,
				stCRT.hide_offset_y + myCarSy,
				stCRT.hide_offset_x + myCarEx,
				stCRT.hide_offset_y + myCarEy, 0x13, 0xFFFF);
		}
#endif
	}

	/* 車速クリップ */
	g_stMyCar.VehicleSpeed = Mmax(Mmin(310, g_stMyCar.VehicleSpeed), 0);
	
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

	return ret;
}

SS	MyCar_Interior(UC bMode)
{
	SS	ret = 0;
	SS	i;
	UC	patNum = 0;
	UC	palNum = 0;
	UC	V=0, H=0;
	UC	sp_num=0;
	SS	x, y;
#if 1
	US	nRatio;
#endif
	static	SS	Vibration = 0;
	static	SS	VibrationCT = 0;
	static	US	rad = 180;
	static	UC	ubRadFlag = TRUE;
	static	UC	ubOBD_old = FALSE;
	static	SS	CrashCount;
	
#if 1
	volatile US *PCG_src  = (US *)0xEBA000;
	volatile US *PCG_dst  = (US *)0xEBA180;
#endif
	
	/* 画面を揺らす */
	VibrationCT++;
	if(VibrationCT > 5)
	{
		VibrationCT = 0;
	}
	
	
	if(g_stMyCar.ubOBD == TRUE)
	{
		if(ubOBD_old == FALSE)
		{
			ADPCM_Play(12);	/* SE：クラッシュ */
			CrashCount = 5;
		}
		Vibration = (VibrationCT == 0)?1:-1;	/* 画面の振動 */
		CrashCount = Mdec(CrashCount, 1);	/* カウンタデクリメント */
		
		if(CrashCount == 0)
		{
			/* 動画 */
			//MOV_Play(3);	/* 嘘をつくな */
			g_stMyCar.ubOBD = FALSE;	/* 正常 */
		}
	}
	else
	{
		Vibration = (VibrationCT == 0)?1:0;	/* 画面の振動 */
	}
	ubOBD_old = g_stMyCar.ubOBD;	/* 前回値更新 */
	
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
#if 1
	GetDebugNum(&nRatio);
#endif	
	PCG_Rotation((US *)PCG_dst, (US *)PCG_src, 3, 3, x, y, &sp_num, palNum, (nRatio-0x80), 180-rad);

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
	
	/* ハンドル */
	
	return ret;
}


SS	MyCar_CourseOut(void)
{
	SS	ret = 0;

	SS	vx;
	vx	= g_stMyCar.Steering;

	if( Mabs(vx) > 25 )	/* コース外 */
	{
		SS	rpm	= g_stMyCar.uEngineRPM;
		g_stMyCar.uEngineRPM = (rpm>>1) + (rpm>>2)  + (rpm>>3);	/* 減速処理 */
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
	SS x;
	struct _txfillptr stTxFill;

	/* FPS */
//	G_Load_Mem( 0, X_OFFSET,	0,			0);	/* インテリア */
//	G_Load_Mem( 0, X_OFFSET,	Y_OFFSET,	0);	/* インテリア */
	PutGraphic_To_Text( 0, 0, 0			);	/* インテリア */
	PutGraphic_To_Text( 0, 0, Y_OFFSET	);	/* インテリア */
	/* メーターの穴 */
	for(x = 0; x < 17; x++)
	{
		Draw_Circle(X_OFFSET + 106,				195,	x, TRANS_PAL, 0, 360, 255);	/* 穴をあける */
		Draw_Circle(X_OFFSET + 106, Y_OFFSET +	195,	x, TRANS_PAL, 0, 360, 255);	/* 穴をあける */
	}
//	Draw_Fill(X_OFFSET + 90, 			180 + 1,	X_OFFSET + 90 + 31, 			180 + 31 - 1,	 TRANS_PAL);	/* 穴をあける */
//	Draw_Fill(X_OFFSET + 90, Y_OFFSET + 180 + 1,	X_OFFSET + 90 + 31, Y_OFFSET +	180 + 31 - 1,	 TRANS_PAL);	/* 穴をあける */

	/* 余白の塗りつぶし */
	Draw_Fill(X_OFFSET, (       0 + MY_CAR_1_H), X_OFFSET + MY_CAR_1_W,        0 + V_SYNC_MAX, 0x01);
	Draw_Fill(X_OFFSET, (Y_OFFSET + MY_CAR_1_H), X_OFFSET + MY_CAR_1_W, Y_OFFSET + V_SYNC_MAX, 0x01);
	
	/* テキスト塗りつぶし */
	for(x = 0; x < 4; x++)
	{
		stTxFill.vram_page = x;
		stTxFill.fill_patn = 0;	/* パレット設定 0 */

//		stTxFill.x = 0;
//		stTxFill.x1= MY_CAR_1_W;
//		stTxFill.y = MY_CAR_1_H;
//		stTxFill.y1= V_SYNC_MAX;
//		_iocs_txfill(&stTxFill);

		stTxFill.x = 90;
		stTxFill.x1= 32;	/* xから増加分 */
		stTxFill.y = 180;	
		stTxFill.y1= 31;	/* yから増加分 */
		_iocs_txfill(&stTxFill);

		stTxFill.x = 90;
		stTxFill.x1= 32;	/* xから増加分 */
		stTxFill.y = Y_OFFSET +180;	
		stTxFill.y1= 31;	/* yから増加分 */
		_iocs_txfill(&stTxFill);

		/* パレット設定 1 */
		if(x == 0)
		{
			stTxFill.fill_patn = 0xFFFF;	/* T0 */
		}
		else
		{
			stTxFill.fill_patn = 0x00;		/* T1,T2,T3 */
		}
		stTxFill.x = 0;
		stTxFill.x1= 255;	/* xから増加分 */
		stTxFill.y = 224;
		stTxFill.y1= 2;		/* yから増加分 */
		_iocs_txfill(&stTxFill);

		stTxFill.x = 0;
		stTxFill.x1= 255;	/* xから増加分 */
		stTxFill.y = Y_OFFSET + 224;
		stTxFill.y1= 2;		/* yから増加分 */
		_iocs_txfill(&stTxFill);
	}
	
//	Set_PicImagePallet(0);	/* パレット設定 */

#if 0	/* TPS */
//	SS x_offset, y_offset;
//	APICG_DataLoad2G("data/cg/Over_B.pic"	, X_OFFSET + ((WIDTH>>1) - (MY_CAR_0_W>>1)),  V_SYNC_MAX-RASTER_MIN-MY_CAR_0_H - 16,	0);	/* TPS */
	x_offset = X_OFFSET + ((WIDTH>>1) - (MY_CAR_0_W>>1));
	y_offset = V_SYNC_MAX - RASTER_MIN - MY_CAR_0_H - 16;
	
	Draw_Fill(X_OFFSET, (y_offset + MY_CAR_0_H), X_OFFSET + WIDTH, V_SYNC_MAX, 0xFF);
#else
#endif
}

void MyCar_Background(void)
{
	SS ret;
	US height_sum = 0u;
	UI i;
//	SS x,y;
	
	ret = G_Load_Mem( 2, 0,	0,	0);	/* ライバル車 */
//	Set_PicImagePallet(2);	/* パレット設定 */
//	APICG_DataLoad2G("data/cg/Over_D.pic"	, X_OFFSET,	Y_OFFSET +  4,	1);	/* 背景 */
	if(ret >= 0)
	{
		/* パターンを */
		height_sum = 0u;
		for(i=1; i<4; i++)
		{
			height_sum += (ENEMY_CAR_1_H >> (i-1));
			
			G_Stretch_Pict(
							0,				ENEMY_CAR_1_W>>i,
							height_sum,		ENEMY_CAR_1_H>>i,
							0,
							0,	ENEMY_CAR_1_W,
							0,	ENEMY_CAR_1_H,
							0);
		}
	}
	
	ret = G_Load_Mem( 4, 140,	0,	0);	/* ヤシの木 */
//	Set_PicImagePallet(4);	/* パレット設定 */
	if(ret >= 0)
	{
		/* パターンを */
		height_sum = 0u;
		for(i=1; i<4; i++)
		{
			height_sum += (PINETREE_1_H >> (i-1));

			G_Stretch_Pict(
							140,		140+PINETREE_1_W>>i,
							height_sum,		PINETREE_1_H>>i,
							0,
							140,	140+PINETREE_1_W,
							0,		PINETREE_1_H,
							0);
		}
	}

	G_Load_Mem( 6, 		0,		Y_OFFSET+8,	1);	/* 背景 */
	G_Load_Mem( 6, X_OFFSET+16,	Y_OFFSET+8,	1);	/* 背景 */
	G_Load_Mem( 6, 		0,		Y_MIN_DRAW+8,	1);	/* 背景 */
	G_Load_Mem( 6, X_OFFSET+16,	Y_MIN_DRAW+8,	1);	/* 背景 */
#if 0
	SS e;
	
	APAGE(2);				/* グラフィックの書き込み */

	/* 建物とコースの間 */
	Draw_Fill( X_MIN_DRAW,  Y_HORIZON + Y_OFFSET, X_MAX_DRAW,  Y_HORIZON + Y_OFFSET, 1);

	/* 建物 */
	for(e = 0; e < 8; e++)
	{
		Draw_Fill((e << 6), Y_HORIZON-16-1 + Y_OFFSET, (e << 6) + 10, Y_HORIZON-1 + Y_OFFSET, 0xFF);
	}
#endif
}

#endif	/* MyCar_C */

