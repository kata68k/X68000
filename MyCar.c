#ifndef	MyCar_C
#define	MyCar_C

#include <iocslib.h>
#include <sys/iocs.h>

#include "inc/usr_macro.h"
#include "inc/ZMUSIC.H"

#include "MyCar.h"
#include "Input.h"
#include "EnemyCAR.h"
#include "Output_Text.h"

/* 構造体定義 */

ST_CARDATA	stMyCar = {0};

/* 関数のプロトタイプ宣言 */
SS	GetMyCar(ST_CARDATA *stDat);
SS	SetMyCar(ST_CARDATA stDat);
SS	UpdateMyCarInfo(SS);
SS	MyCar_Interior(SS, SS);
SS	MyCar_CourseOut(SS);
void	SE_Play(void);

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
	static UC bShiftPosFlag[3] = {FALSE};

	US	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
	US	uTM_F[6] = { 0, 13390, 9140, 6768, 5137, 3567 };/* 総減速比 */
	US	uRPM[11] = { 500, 1000, 1500, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000 };	/* Eng回転数 */
	US	uTRQ[11] = { 120,  130,  150,  170,  210,  200,  180,  170,  140,  120,  100 };	/* トルク */
	UC	bUpDown = 0;
	
	/* ステアリング操作 */
	if((input & KEY_RIGHT) != 0u)	stMyCar.Steering += 1;	/* 右 */
	if((input & KEY_LEFT) != 0u)	stMyCar.Steering -= 1;	/* 左 */
	stMyCar.Steering = Mmax(Mmin(stMyCar.Steering, 31), -31);
	
	
	/* シフト操作 */
	if( ((input & KEY_A) != 0u) && ((input & KEY_B) == 0u))	/* Aボタン(only) */
	{
		/* アクセルだけはシフト操作禁止 */
	}
	else{
		if(ChatCancelSW((input & KEY_UPPER)!=0u, &bShiftPosFlag[0]) == TRUE)
		{
			if(stMyCar.ubShiftPos != 5)
			{
				bUpDown = 1;
				m_pcmplay(7,3,4);	/* SE:シフトアップ */
			}
			stMyCar.ubShiftPos = Mmin(stMyCar.ubShiftPos+1, 5);	/* 上 */
		}
		
		if(ChatCancelSW((input & KEY_LOWER)!=0u, &bShiftPosFlag[1]) == TRUE)
		{
			if(stMyCar.ubShiftPos != 0)
			{
				bUpDown = 2;
				m_pcmplay(7,3,4);	/* SE:シフトダウン */
			}
			stMyCar.ubShiftPos = Mmax(stMyCar.ubShiftPos-1, 0);	/* 下 */
		}
	}
#if 0
	stMyCar.ubShiftPos = 2;	/* テスト用変速固定 */
#endif	
	stMyCar.ubShiftPos = Mmax(Mmin(stMyCar.ubShiftPos, 5), 0);
	
	if(bUpDown != 0u) 
	{
		stMyCar.uEngineRPM = (stMyCar.VehicleSpeed * uTM[stMyCar.ubShiftPos]) / 26;
	}
	
	/* アクセル */
	if((input & KEY_A) != 0u){	/* Aボタン */
		switch(stMyCar.ubShiftPos)
		{
			case 0:
			{
				stMyCar.uEngineRPM += 200;
				SE_Play();
			}
			break;
			case 1:
			{
				stMyCar.uEngineRPM += 20;
			}
			break;
			case 2:
			{
				stMyCar.uEngineRPM += 15;
			}
			break;
			case 3:
			{
				stMyCar.uEngineRPM += 10;
			}
			break;
			case 4:
			{
				stMyCar.uEngineRPM += 6;
			}
			break;
			case 5:
			{
				stMyCar.uEngineRPM += 3;
			}
			break;
			default:
			{
				stMyCar.uEngineRPM += 100;
			}
			break;
		}
	}
	else{
		/* 回転数 */
		switch(stMyCar.ubShiftPos)
		{
			case 0:
			{
				stMyCar.uEngineRPM -= 200;
			}
			break;
			case 1:
			{
				stMyCar.uEngineRPM -= 40;
			}
			break;
			case 2:
			{
				stMyCar.uEngineRPM -= 30;
			}
			break;
			case 3:
			{
				stMyCar.uEngineRPM -= 20;
			}
			break;
			case 4:
			{
				stMyCar.uEngineRPM -= 12;
			}
			break;
			case 5:
			{
				stMyCar.uEngineRPM -= 6;
			}
			break;
			default:
			{
				stMyCar.uEngineRPM -= 200;
			}
			break;
		}
	}
	stMyCar.uEngineRPM = Mmax(Mmin(9000, stMyCar.uEngineRPM), 750);

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
	SS	i;
	UL	code = 0;
	UC	patNum = 0;
	UC	palNum = 0;
	UC	V=0, H=0;
	UC	bHit = FALSE;
	
	static	SS	Vibration = 0;
	static	SS	VibrationCT = 0;
	static	US	rad = 180;
	static	UC	ubRadFlag = TRUE;
	
	ST_ENEMYCARDATA	stEnemyCar;
	
	/* 画面を揺らす */
	VibrationCT++;
	if(VibrationCT > 5)
	{
		VibrationCT = 0;
	}
	
	if(stMyCar.VehicleSpeed != 0)
	{
		for(i = 0; i < ENEMYCAR_MAX; i++)
		{
			GetEnemyCAR(&stEnemyCar, i);

			if( (stEnemyCar.ubAlive == TRUE) && (stEnemyCar.z == 0) )
			{
				if( ((stEnemyCar.x - 8) <= stMyCar.Steering) && (stMyCar.Steering <= (stEnemyCar.x + 8)) )
				{
					bHit = TRUE;
				}
			}
		}
	}
	
	if(bHit == TRUE)
	{
		m_pcmplay(10,3,4);
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
	if(stMyCar.VehicleSpeed == 0)
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
		
		width = 15 + (stMyCar.VehicleSpeed / 10);
		
		if(ubRadFlag == TRUE)
		{
			rad+=Mmax(stMyCar.VehicleSpeed/30, 1);
			if(rad > 180 + width)ubRadFlag = FALSE;
		}
		else
		{
			rad-=Mmax(stMyCar.VehicleSpeed/30, 1);
			if(rad < 180 - width)ubRadFlag = TRUE;
		}
	}
	

	x = 16 + (( 32 * APL_Sin(rad)) >> 8);
	y =  0 + ((-32 * APL_Cos(rad)) >> 8);
	patNum = 0x40;
	palNum = 9;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( 0, -1, x + 0, y + 0, code, 3);
	patNum = 0x41;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( 1, -1, x + 16, y + 0, code, 3);
	patNum = 0x50;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( 2, -1, x + 0, y + 16, code, 3);
	patNum = 0x51;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( 3, -1, x + 16, y + 16, code, 3);
	patNum = 0x60;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( 4, -1, x + 0, y + 32, code, 3);
	patNum = 0x61;
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( 5, -1, x + 16, y + 32, code, 3);

	/* タコメーター */
	for( i = 0; i < 18; i++ )
	{
		if( ((i*500) <= stMyCar.uEngineRPM) && (stMyCar.uEngineRPM < ((i*500)+500)) )
		{
			break;
		}
	}
	palNum = 0x0C;
	if(i < 7)
	{
		x = 100;
		y = 140;
		patNum = 0x70+i;
		V = 1;
		H = 0;
	}
	else if(i < 13)
	{
		x = 100;
		y = 124;
		patNum = 0x70+13-i;
		V = 0;
		H = 0;
	}
	else
	{
		x = 116;
		y = 124;
		patNum = 0x70+i-13;
		V = 0;
		H = 1;
	}
	
	code = (UL)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
	SP_REGST( 6, -1, x, y, code, 3);
	
	Message_Num(&stMyCar.uEngineRPM, 0,  9, 2, MONI_Type_US, "%4d");
	
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

/* FM音源で効果音再生する */
void	SE_Play(void)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};

	UC	v[] = {	/* 自機ミサイル */
	/*	AF  OM  WF  SY  SP PMD AMD PMS AMS PAN DUMMY	*/
		60, 15,  0,  0,  0,  0,  0,  0,  0,  3,  0,
	/*	AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME	*/
		27, 15,  5,  2,  0,  0,  0,  0,  3,  1,  0,
		31, 18, 18,  6,  7,  0,  0,  0,  3,  2,  0,
		22, 31,  0, 10,  0, 42,  0,  7,  7,  0,  0,
		15, 31,  0,  8,  0,  0,  2,  1,  7,  0,  0};

	UC	SE_Data[] = {	/* 構造体にした方がよい？ */
		//0x01,										/* (+1)ZMDの構造 */
		//0x5A,0x6D,0x75,0x53,0x69,0x43,			/* (+6)ZmuSiC */
		//0x20,										/* (+7)バージョン */
		0x00,0x01,								/* 演奏総トラック数(.W) */
		0x00,0x00,0x00,0x02,					/* 演奏データまでのオフセット(.L) */
		0x00,0x05,								/* 演奏絶対チャンネル(.W) */
		0xa0,0x0c,0xb6,0x05,0xb3,0xd1,0x00,0x00,	/*	演奏データ */
		0x00,0x00,0xe6,0x00,0x3c,0xe8,0x00,0x18,0xff,0xff,
		0xd6,0x00,0x03,0x00,0x00,0xe0,0x39,0x00,0x0c,0xff,0xff,
		0x00,0x00,0x00,0x40,0x00,0x01,0x45,0x48,0xff,
		0xe0,0x45,0x00,0x0c,0x00,0x0c,0x00,0x00,0xff,
		0xc0,0x00,0xff,0xff
	};

	m_stop(1,2,3,4,5,6,7,8,9,10);
	
	m_vset(6, v);
	m_alloc(6,1000);
	m_assign(6, 6);

	m_play(1,2,3,4,5,6,7,8,9,10);
	
	stInReg.d0 = 0xF0;			/* ZMUSIC.XによるIOCSコール */
	stInReg.d1 = 0x12;			/* se_play $12（ZMUSIC内のファンクションコール） */
	stInReg.d2 = 0x05;			/* 演奏トラックNo */
	stInReg.a1 = (UI)&SE_Data[0];	/* 演奏データ格納アドレス */
	
	_iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
}

#endif	/* MyCar_C */

