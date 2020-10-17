#ifndef	MyCar_C
#define	MyCar_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "inc/ZMUSIC.H"

#include "MyCar.h"
#include "OverKata.h"
#include "APL_Math.h"
#include "Input.h"
#include "EnemyCAR.h"
#include "FileManager.h"
#include "Graphic.h"
#include "MFP.h"
#include "Music.h"
#include "Output_Text.h"
#include "PCG.h"

/* グローバル変数 */
SS	g_speed = 0;

/* 構造体定義 */
ST_CARDATA	g_stMyCar = {0};

/* 関数のプロトタイプ宣言 */
SS	MyCar_G_Load(void);
SS	GetMyCar(ST_CARDATA *stDat);
SS	SetMyCar(ST_CARDATA stDat);
SS	UpdateMyCarInfo(SS);
SS	MyCar_Interior(UC);
SS	MyCar_CourseOut(void);
SS	GetMyCarSpeed(SS *);


/* 関数 */
SS	MyCar_G_Load(void)
{
	SS	ret = 0;
	
	G_MyCar();		/* 自車の表示 */

	G_Background();	/* 背景の表示 */

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

SS	UpdateMyCarInfo(SS input)
{
	SS	ret = 0;
	SS	speed_min;
	static UC bShiftPosFlag[3] = {FALSE};

	US	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
//	US	uTM_F[6] = { 0, 13390, 9140, 6768, 5137, 3567 };/* 総減速比 */
//	US	uRPM[11] = { 500, 1000, 1500, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000 };	/* Eng回転数 */
//	US	uTRQ[11] = { 120,  130,  150,  170,  210,  200,  180,  170,  140,  120,  100 };	/* トルク */
	UC	bUpDown = 0;
	
	/* ステアリング操作 */
	if((input & KEY_RIGHT) != 0u)	g_stMyCar.Steering += 1;	/* 右 */
	if((input & KEY_LEFT) != 0u)	g_stMyCar.Steering -= 1;	/* 左 */
	g_stMyCar.Steering = Mmax(Mmin(g_stMyCar.Steering, 31), -31);
#if	0
	if( ((uFreeRunCount % 4) == 0)
		&& (g_speed != 0u) )
	{
		g_stMyCar.Steering = g_stMyCar.Steering + ((SS)(road_angle * g_speed) >> 6);	/* バランス調整要 */
		vx = g_stMyCar.Steering;
	}
	else{
		/* 前回値保持 */
	}
#endif
	
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
		switch(g_stMyCar.ubShiftPos)
		{
			case 0:
			{
				g_stMyCar.uEngineRPM += 200;
//				SE_Play(1);	/* FM効果音再生 */
			}
			break;
			case 1:
			{
				g_stMyCar.uEngineRPM += 20;
//				SE_Play(1);	/* FM効果音再生 */
			}
			break;
			case 2:
			{
				g_stMyCar.uEngineRPM += 15;
//				SE_Play(1);	/* FM効果音再生 */
			}
			break;
			case 3:
			{
				g_stMyCar.uEngineRPM += 10;
//				SE_Play(1);	/* FM効果音再生 */
			}
			break;
			case 4:
			{
				g_stMyCar.uEngineRPM += 6;
//				SE_Play(1);	/* FM効果音再生 */
			}
			break;
			case 5:
			{
				g_stMyCar.uEngineRPM += 3;
//				SE_Play(1);	/* FM効果音再生 */
			}
			break;
			default:
			{
				g_stMyCar.uEngineRPM += 100;
			}
			break;
		}
	}
	else{
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
				g_stMyCar.uEngineRPM -= 40;
			}
			break;
			case 2:
			{
				g_stMyCar.uEngineRPM -= 30;
			}
			break;
			case 3:
			{
				g_stMyCar.uEngineRPM -= 20;
			}
			break;
			case 4:
			{
				g_stMyCar.uEngineRPM -= 12;
			}
			break;
			case 5:
			{
				g_stMyCar.uEngineRPM -= 6;
			}
			break;
			default:
			{
				g_stMyCar.uEngineRPM -= 200;
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
	UL	code = 0;
	UC	patNum = 0;
	UC	palNum = 0;
	UC	V=0, H=0;
	UC	bHit = FALSE;
	UC	sp_num=0;
	SS	x, y;
#if 1
	US	nRatio;
#endif
	static	SS	Vibration = 0;
	static	SS	VibrationCT = 0;
	static	US	rad = 180;
	static	UC	ubRadFlag = TRUE;
	
#if 1
	volatile US *PCG_src  = (US *)0xEBA000;
	volatile US *PCG_dst  = (US *)0xEBA180;
#endif
	ST_ENEMYCARDATA	stEnemyCar = {0};
	ST_CRT			stCRT = {0};

	GetCRT(&stCRT, bMode);
	x = stCRT.view_offset_x;
	y = stCRT.view_offset_y;
	
	/* 画面を揺らす */
	VibrationCT++;
	if(VibrationCT > 5)
	{
		VibrationCT = 0;
	}
	
	if(g_stMyCar.VehicleSpeed != 0)
	{
		for(i = 0; i < ENEMYCAR_MAX; i++)
		{
			GetEnemyCAR(&stEnemyCar, i);

			if( (stEnemyCar.ubAlive == TRUE) && (stEnemyCar.z == 0) )
			{
				if( ((stEnemyCar.x - 8) <= g_stMyCar.Steering) && (g_stMyCar.Steering <= (stEnemyCar.x + 8)) )
				{
					bHit = TRUE;
				}
			}
		}
	}
	
	if(bHit == TRUE)
	{
		ADPCM_Play(10);
		Vibration = (VibrationCT - 3) * 5;	/* 画面の振動 */
	}
	else
	{
		Vibration = (VibrationCT == 0)?1:0;	/* 画面の振動 */
	}

	/* 画面の位置 */
	HOME(0b01, x, y + Vibration );	/* Screen 0(TPS/FPS) */
	HOME(0b10, x, y );				/* Screen 1 */

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
	GetDebugNum(&nRatio);
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
	
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( sp_num++, -1, x, y, code, 3);

	/* タコメーター */
	x = 90+16;
	y = 180+16 - Vibration;
	V = 0;
	H = 0;
	patNum = 0x46;
	palNum = 0x0D;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( sp_num++, -1, x + 0, y + 0, code, 3);

	patNum = 0x47;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( sp_num++, -1, x + 16, y + 0, code, 3);

	patNum = 0x56;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( sp_num++, -1, x + 0, y + 16, code, 3);

	patNum = 0x57;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( sp_num++, -1, x + 16, y + 16, code, 3);
	
	return ret;
}


SS	MyCar_CourseOut(void)
{
	static	UI	unExplosion_time = 0;
	SS	ret = 0;
	SS	vx;
	SS	rpm;
	
	vx	= g_stMyCar.Steering;
	rpm	= g_stMyCar.uEngineRPM;

	if( Mabs(vx) > 25 )	/* コース外 */
	{
		UI	time;

		GetStartTime(&time);	/* 開始時刻を取得 */
		
		if( (time - unExplosion_time) >  (60000 / rpm) )	/* 回転数のエンジン音 */
		{
			unExplosion_time = time;
			ADPCM_Play(11);	/* 回転数のエンジン音 */
		}
		else
		{
			/* 何もしない */
		}

		g_stMyCar.uEngineRPM = (g_stMyCar.uEngineRPM>>1) + (g_stMyCar.uEngineRPM>>2)  + (g_stMyCar.uEngineRPM>>3);	/* 減速処理 */
	}
	
	return ret;
}

SS	GetMyCarSpeed(SS *speed)
{
	SS	ret = 0;
	
	*speed = g_speed;

	return ret;
}


#endif	/* MyCar_C */

