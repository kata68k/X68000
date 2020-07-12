#ifndef	MyCar_C
#define	MyCar_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "inc/ZMUSIC.H"

#include "MyCar.h"
#include "Input.h"

/* 構造体定義 */

ST_CARDATA	stMyCar = {0};

/* 関数のプロトタイプ宣言 */
SS	GetMyCar(ST_CARDATA *stDat);
SS	SetMyCar(ST_CARDATA stDat);
SS	UpdateMyCarInfo(SS);
SS	MyCar_Interior(SS, SS);
SS	MyCar_CourseOut(SS);

/* 関数 */

SS	GetMyCar(ST_CARDATA *stDat)
{
	SS	ret = 0;
	*stDat = stMyCar;
	return ret;
}

SS	SetMyCar(ST_CARDATA stDat)
{
	SS	ret = 0;
	stMyCar = stDat;
	return ret;
}

SS	UpdateMyCarInfo(SS input)
{
	static UC bShiftPosFlag[2] = {FALSE};

	US	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
	US	uRPM[11] = { 500, 1000, 1500, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000 };	/* Eng回転数 */
	US	uTRQ[11] = { 120,  130,  150,  170,  210,  200,  180,  170,  140,  120,  100 };	/* トルク */
	
	/* ステアリング操作 */
	if((input & KEY_RIGHT) != 0u)	stMyCar.Steering += 1;	/* 右 */
	if((input & KEY_LEFT) != 0u)	stMyCar.Steering -= 1;	/* 左 */
	stMyCar.Steering = Mmax(Mmin(stMyCar.Steering, 31), -31);
	
	/* アクセル */
	if((input & KEY_A) != 0u){	/* Aボタン */
		/* 回転数 */
//			if(b8ms_time == TRUE)
		{
			stMyCar.uEngineRPM += 125;
		}
	}
	else{
		/* 回転数 */
//			if(b8ms_time == TRUE)
		{
			stMyCar.uEngineRPM -= 100;
		}
	}
	stMyCar.uEngineRPM = Mmax(Mmin(9000, stMyCar.uEngineRPM), 750);

	/* シフト操作 */
	if(ChatCancelSW((input & KEY_UPPER)!=0u, &bShiftPosFlag[0]) == TRUE)
	{
		if(stMyCar.ubShiftPos != 5)	m_pcmplay(7,3,4);	/* SE:シフトアップ */
		stMyCar.ubShiftPos = Mmin(stMyCar.ubShiftPos+1, 5);	/* 上 */
	}
	
	if(ChatCancelSW((input & KEY_LOWER)!=0u, &bShiftPosFlag[1]) == TRUE)
	{
		if(stMyCar.ubShiftPos != 0)	m_pcmplay(7,3,4);	/* SE:シフトダウン */
		stMyCar.ubShiftPos = Mmax(stMyCar.ubShiftPos-1, 0);	/* 下 */
	}
#if 0
	stMyCar.ubShiftPos = 2;	/* テスト用変速固定 */
#endif	
	stMyCar.ubShiftPos = Mmax(Mmin(stMyCar.ubShiftPos, 5), 0);
	
	/* 車速 */
	if(stMyCar.ubShiftPos == 0u)		/* ニュートラル */
	{
		stMyCar.VehicleSpeed -= 1;
	}
	else
	{
		/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 減速比 4.687 タイヤ周長2052.1mm */
		/* タイヤ周長×６０×回転数／（１０００×変速比×減速比） */
		stMyCar.VehicleSpeed = (SS)(((UI)26 * stMyCar.uEngineRPM) / uTM[stMyCar.ubShiftPos]);	
	}
	
	/* ブレーキ */
	if( (input & KEY_B) != 0U )	/* Bボタン */
	{
		stMyCar.VehicleSpeed -= 30;	/* 変速比で変えたい */
		stMyCar.ubBrakeLights = TRUE;	/* ブレーキランプ ON */
		m_pcmplay(4,3,4);	/* ブレーキ音 */
	}
	else
	{
		stMyCar.ubBrakeLights = FALSE;	/* ブレーキランプ OFF */
	}
	stMyCar.VehicleSpeed = Mmax(Mmin(310, stMyCar.VehicleSpeed), 0);
}

SS	MyCar_Interior(SS x, SS y)
{
	SS	ret = 0;
	static	SS	Vibration = 0;
	static	SS	VibrationCT = 0;
	
	/* 画面を揺らす */
	VibrationCT++;
	if(VibrationCT > 5)
	{
		VibrationCT = 0;
	}
	Vibration = (VibrationCT == 0)?1:0;	/* 画面の振動 */

	/* 画面の位置 */
	HOME(0b01, x, y + Vibration );	/* Screen 0(TPS/FPS) */
	HOME(0b10, x, y );				/* Screen 1 */

	return ret;
}


SS	MyCar_CourseOut(SS	rpm)
{
	SS	ret = 0;
#if	1
	SI	adpcm_sns;
	UI	time;
	static	UI	unExplosion_time = FALSE;
	UC	bExplosion;

	GetStartTime(&time);	/* 開始時刻を取得 */
	
	adpcm_sns = m_stat(9/*Mmin(Mmax(25, vx), 31)*/);	/* ADPCM ch1(9) ch2-8(25-31) */
//			Message_Num(&adpcm_sns,	 0, 13, 2, MONI_Type_SI, "%d");
	
	if( (time - unExplosion_time) >  (60000 / rpm) )	/* 回転数のエンジン音 */
	{
		unExplosion_time = time;
		bExplosion = TRUE;
	}
	else
	{
		bExplosion = FALSE;
	}

	/* 回転数のエンジン音 */
	if( bExplosion == TRUE )
	{
		m_pcmplay(11,3,4);
	}
#endif
	return ret;
}

#endif	/* MyCar_C */

