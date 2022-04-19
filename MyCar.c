#ifndef	MyCar_C
#define	MyCar_C

#include <stdio.h>
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
#include "Score.h"
#include "Text.h"

/* define定義 */

/* グローバル変数 */
int16_t	g_speed;
static int16_t	g_Input;
static int16_t	g_SteeringDiff;
static int16_t	Torque_old = 0xFFFF;
static int16_t	Steering = 0;
static uint8_t	bShiftPosFlag[3] = {FALSE};
static int8_t	bThrottleON_Count = 0;
static int8_t	bThrottle_Flag = FALSE;
static uint8_t	ubShiftPos_N = FALSE;
static uint8_t	ubShiftPos_UP = FALSE;
static uint8_t	ubShiftPos_old = 0;
static uint32_t	unExplosion_time = 0xFFFFFFFFu;
static int16_t	Vibration = 0;
static int16_t	VibrationCT = 0;
static uint8_t	ubOBD_old = OBD_NORMAL;
static int16_t	CrashCount;
static uint16_t	rad = 180;
static uint8_t	ubRadFlag = TRUE;


/* グローバルデータ */
uint16_t	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
//	uint16_t	uTM_F[6] = { 0, 13390, 9140, 6768, 5137, 3567 };/* 総減速比 */
uint16_t	uRPM[11] = { 500, 1000, 1500, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000 };	/* Eng回転数 */
uint16_t	uTRQ[11] = { 120,  130,  150,  170,  210,  200,  180,  170,  140,  120,  100 };	/* トルク */


/* 構造体定義 */
ST_CARDATA	g_stMyCar = {0};

/* 関数のプロトタイプ宣言 */
int16_t	MyCar_G_Load(void);
int16_t	GetMyCar(ST_CARDATA *stDat);
int16_t	SetMyCar(ST_CARDATA stDat);
int16_t	MyCarInfo_Init(void);
int16_t	MyCarInfo_Update(int16_t, int16_t *);
int16_t	MyCarInfo_Update16ms(int16_t);
static int16_t	MyCar_Steering(void);
static int16_t	MyCar_ShiftPos(void);
static int16_t	MyCar_Accel(void);
static int16_t	MyCar_Brake(void);
static int16_t	MyCar_Angle(void);
static int16_t	MyCar_EngineSpeed(int16_t);
static int16_t	MyCar_Crash(void);
static int16_t	MyCar_VehicleSpeed(void);
int16_t	MyCar_Interior(void);
int16_t	MyCar_CourseOut(void);
int16_t	GetMyCarSpeed(int16_t *);
void MyCar_Image(void);
static int16_t	MyCar_Vibration(void);
int16_t	MyCar_Mascot(int16_t);
static int16_t	MyCar_Tachometer(int16_t);
static int16_t	MyCar_SteeringPos(int16_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyCar_G_Load(void)
{
	int16_t	ret = 0;
	
	MyCar_Image();		/* 自車の表示 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetMyCar(ST_CARDATA *stDat)
{
	int16_t	ret = 0;
	*stDat = g_stMyCar;
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetMyCar(ST_CARDATA stDat)
{
	int16_t	ret = 0;
	g_stMyCar = stDat;
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyCarInfo_Init(void)
{
	int16_t	ret = 0;
	
	g_speed = 0;
	g_Input = 0;
	g_SteeringDiff = 0;
	
	Torque_old = 0xFFFF;
	Steering = 0;
	bShiftPosFlag[0] = FALSE;
	bShiftPosFlag[1] = FALSE;
	bShiftPosFlag[2] = FALSE;
	bThrottleON_Count = 0;
	bThrottle_Flag = FALSE;
	ubShiftPos_N = FALSE;
	ubShiftPos_UP = FALSE;
	ubShiftPos_old = 0;
	unExplosion_time = 0xFFFFFFFFu;
	Vibration = 0;
	VibrationCT = 0;
	ubOBD_old = OBD_NORMAL;
	CrashCount = 0;
	rad = 180;
	ubRadFlag = TRUE;
	
	g_stMyCar.ubCarType		= 0u;	/* 車の種類 */
	g_stMyCar.uEngineRPM	= 0u;	/* エンジン回転数 */
	g_stMyCar.VehicleSpeed	= 0;	/* 車速 */
	g_stMyCar.Steering		= 0;	/* ステア */
	g_stMyCar.Angle			= 0;	/* 向き */
	g_stMyCar.ubShiftPos	= 0u;	/* ギア段 */
	g_stMyCar.ubThrottle	= 0u;		/* スロットル開度 */
	g_stMyCar.ubBrakeLights	= FALSE;	/* ブレーキライト */
	g_stMyCar.ubHeadLights	= FALSE;	/* ヘッドライト */
	g_stMyCar.ubWiper		= FALSE;	/* ワイパー */
	g_stMyCar.bTire			= 0;	/* タイヤの状態 */
	g_stMyCar.ubOBD			= OBD_NORMAL;	/* 車両の状態 */

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyCarInfo_Update(int16_t input, int16_t *pTorque)
{
	int16_t	ret = 0;

	int16_t	Torque = 0;

	g_Input = input;				/* 入力値更新 */
	
	Torque += MyCar_Steering();		/* ステアリング操作 */
	
	Torque += MyCar_ShiftPos();		/* シフト操作 */
	
	Torque += MyCar_Accel();		/* アクセル操作 */
	
	Torque += MyCar_Brake();		/* ブレーキ操作 */
	
	Torque += MyCar_Angle();		/* クルマの向き */
	
	Torque += MyCar_Crash();		/* 衝突判定 */
	
	Torque += MyCar_CourseOut();	/* コースアウト時の処理 */
	
	if(Torque_old != Torque)
	{
		Torque_old = Torque;
		ret = 1;
	}
	else
	{
		/* 何もしない */
	}
	
	*pTorque = Mmax(Torque, 0);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyCarInfo_Update16ms(int16_t Torque)
{
	int16_t	ret = 0;
	
	ret |= MyCar_EngineSpeed(Torque);	/* エンジン回転数の算出 */
	
	ret |= MyCar_VehicleSpeed();		/* 車速の算出 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	トルク	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	MyCar_Steering(void)
{
	int16_t	ret = 0;

	int8_t	bOn = 0;
	int16_t	Steering_old = 0;
	int16_t	SteeringDiff = 0;
	
	int16_t	AnalogMode = 0;
	JOY_ANALOG_BUF	stAnalog_Info;

	AnalogMode = GetAnalog_Info(&stAnalog_Info);	/* アナログスティック情報取得 */
	
	Steering_old = g_stMyCar.Steering;	/* 前回値 */
	
	if((g_Input & KEY_RIGHT) != 0u)
	{
		if(AnalogMode == 0)	/* アナログモード */
		{
			Steering = (stAnalog_Info.r_stk_lr - 0x80);
		}
		else
		{
			if(Steering < 0)
			{
				Steering = Mdiv2(Steering);	/* 2分法 */
				if(Steering == -1)	/* 算術シフトのマイナス側は０に収束しない対策 */
				{
					Steering = 0;
				}
			}
			Steering += g_speed;	/* 右 */
		}
		bOn = 1;
		ret -= 1;	/* TorqueDW パワステ駆動減 */
	}
	else if((g_Input & KEY_LEFT) != 0u)
	{
		if(AnalogMode == 0)	/* アナログモード */
		{
			Steering = (0x80 - stAnalog_Info.r_stk_lr);
		}
		else
		{
			if(Steering > 0)
			{
				Steering = Mdiv2(Steering);	/* 2分法 */
				if(Steering == -1)	/* 算術シフトのマイナス側は０に収束しない対策 */
				{
					Steering = 0;
				}
			}
			Steering -= g_speed;	/* 左 */
		}
		bOn = -1;
		ret -= 1;	/* TorqueDW パワステ駆動減 */
	}
	else
	{
#if 1
		if(g_speed != 0)	/* 車速アリ */
		{
			if(Steering == 0)
			{
				/* 何もしない */
			}
			else
			{
				Steering = Mdiv2(Steering);	/* 2分法 */
				if(Steering == -1)	/* 算術シフトのマイナス側は０に収束しない対策 */
				{
					Steering = 0;
				}
			}
		}
#endif
	}
	Steering = Mmax(Mmin(Steering, 120), -120);
	
	g_stMyCar.Steering = Steering;	/* ステアリングのキレ角(絶対値) */

	/* 外部出力 */
	SteeringDiff = APL_AngleDiff(Steering_old, g_stMyCar.Steering);	/* 変化量 */
	if(SteeringDiff == 0)
	{
		if(bOn == 0)
		{
		}
		else
		{
			if(bOn > 0)
			{
				SteeringDiff = g_speed;
			}
			else
			{
				SteeringDiff = -g_speed;
			}
		}
	}
	g_SteeringDiff = SteeringDiff;	/* 変化量 */

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	MyCar_ShiftPos(void)
{
	int16_t	ret = 0;

#if 0
	if( ((g_Input & KEY_A) != 0u) && ((g_Input & KEY_B) == 0u))	/* Aボタン(only) */
	{
		/* アクセルだけはシフト操作禁止 */
	}
	else
#endif
	{
		if(ChatCancelSW((g_Input & KEY_UPPER)!=0u, &bShiftPosFlag[0]) == TRUE)
		{
			if(g_stMyCar.ubShiftPos != 5)
			{
				ADPCM_Play(7);	/* SE:シフトアップ */
			}
			g_stMyCar.ubShiftPos = Mmin(g_stMyCar.ubShiftPos+1, 5);	/* 上 */
		}
		
		if(ChatCancelSW((g_Input & KEY_LOWER)!=0u, &bShiftPosFlag[1]) == TRUE)
		{
			if(g_stMyCar.ubShiftPos != 0)
			{
				ADPCM_Play(7);	/* SE:シフトダウン */
			}
			g_stMyCar.ubShiftPos = Mmax(g_stMyCar.ubShiftPos-1, 0);	/* 下 */
		}
	}
#if 0
	g_stMyCar.ubShiftPos = 2;	/* テスト用変速固定 */
#endif	
	g_stMyCar.ubShiftPos = Mmax(Mmin(g_stMyCar.ubShiftPos, 5), 0);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	MyCar_Accel(void)
{
	int16_t	ret = 0;
	
	/* アクセル */
	if(bThrottle_Flag == FALSE)
	{
		if((g_Input & KEY_A) != 0u)		/* Aボタン押す */
		{
			bThrottle_Flag = TRUE;
			bThrottleON_Count = 8;
		}
	}

	if(bThrottle_Flag == TRUE)
	{
		if((g_Input & KEY_A) == 0u)		/* Aボタン離す */
		{
			ret -= 1;	/* TorqueDW 燃料カット */
			
			if(bThrottleON_Count > -8)
			{
				bThrottleON_Count -= 1u;
			}
		}
		
		g_stMyCar.ubThrottle += bThrottleON_Count;

		if(g_stMyCar.ubThrottle >= 240)
		{
			g_stMyCar.ubThrottle = 240;
		}
		
		if(g_stMyCar.ubThrottle < 8)
		{
			g_stMyCar.ubThrottle = 0u;
			bThrottle_Flag = FALSE;
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
static int16_t	MyCar_Brake(void)
{
	int16_t	ret = 0;

	if( (g_Input & KEY_B) != 0U )	/* Bボタン */
	{
		if(g_stMyCar.VehicleSpeed > 5)
		{
			ADPCM_Play(4);	/* ブレーキ音 */
		}
		ret += 10;	/* TorqueUP マスターバックからの空気 */
		
		g_stMyCar.ubBrakeLights = TRUE;		/* ブレーキランプ ON */
	}
	else
	{
		g_stMyCar.ubBrakeLights = FALSE;	/* ブレーキランプ OFF */
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
static int16_t	MyCar_Angle(void)
{
	int16_t	ret = 0;

	int16_t	SteeringDiff = 0;
	
	ST_ROAD_INFO	stRoadInfo;
	int16_t	RoadAngle;
	int16_t	AngleDiff;

	GetRoadInfo(&stRoadInfo);
	RoadAngle = stRoadInfo.angle;
	
	/* ステアリング操作によるクルマの向き変化 */
	SteeringDiff = g_SteeringDiff;
	
	AngleDiff = APL_AngleDiff(RoadAngle, g_stMyCar.Angle);			/* 車と道路の角度差分で車の位置が変わる */
	
	/* クルマの向き */
	if( AngleDiff > 16 )
	{
		g_stMyCar.Angle = RoadAngle + 16;
	}
	else if( AngleDiff < -16 )
	{
		g_stMyCar.Angle = RoadAngle - 16;
	}
	else
	{
	}
	g_stMyCar.Angle += SteeringDiff;	/* 車の向き */
	
	/* 0or360度を超えた処理 */
	do
	{
		if(g_stMyCar.Angle >= 360)
		{
			g_stMyCar.Angle -= 360;
		}
		else if(g_stMyCar.Angle < 0)
		{
			g_stMyCar.Angle += 360;
		}
	}
	while((g_stMyCar.Angle < 0) || (g_stMyCar.Angle > 360) );
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	MyCar_EngineSpeed(int16_t Input_Torque)
{
	int16_t	ret = 0;

	uint8_t	bAxis;
	uint32_t	i;
	uint32_t	time;
	uint16_t	uTorque_Cal;
	int16_t		rpm;
	int16_t		TargetRPM = 0;

#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	GetStartTime(&time);	/* 開始時刻を取得 */

	if(g_stMyCar.ubBrakeLights == TRUE)
	{
		ubShiftPos_N = TRUE;	/* ニュートラルポジション */
	}
	else if(g_stMyCar.ubShiftPos != ubShiftPos_old) 	/* シフトチェンジによるエンジン回転数の更新 */
	{
		ubShiftPos_N = TRUE;	/* ニュートラルポジション */
	}
	else if(g_stMyCar.ubShiftPos == 0u)	/* ニュートラルポジション */
	{
		ubShiftPos_N = TRUE;	/* ニュートラルポジション */
	}
	else
	{
		/* 何もしない */
	}
	ubShiftPos_old = g_stMyCar.ubShiftPos;	/* 前回値更新 */
	
	if(ubShiftPos_N == TRUE)
	{
		TargetRPM = (g_stMyCar.VehicleSpeed * uTM[g_stMyCar.ubShiftPos]) / 26;	/* 車速からエンジン回転数を算出 */

		if(TargetRPM < g_stMyCar.uEngineRPM)
		{
			ubShiftPos_UP = TRUE;
		}
		else
		{
			ubShiftPos_UP = FALSE;
		}
	}
	else
	{
		ubShiftPos_UP = FALSE;
	}
	
	/* 回転数軸算出 */
	bAxis = 10;
	for( i=0; i<10; i++ )
	{
		if( g_stMyCar.uEngineRPM <= (uRPM[i] + Mdiv2(uRPM[i+1] - uRPM[i])) )
		{
			bAxis = i;
			break;
		}
	}
	/* 最終トルク */
	uTorque_Cal = (uint16_t)Mmax((int16_t)Mdiv256(uTRQ[bAxis] * g_stMyCar.ubThrottle) + Input_Torque, 1);
	g_stMyCar.ubWiper = (uint8_t)uTRQ[bAxis];	/* 仮 */
	g_stMyCar.bTire = (int8_t)uTorque_Cal;	/* 仮 */
	
	if(ubShiftPos_N == TRUE)	/* ニュートラルポジション */
	{
		if(g_stMyCar.ubShiftPos == 0u)	/* ニュートラルポジション */
		{
			if( g_stMyCar.ubThrottle > 0 )		/* アクセルON中 */
			{
				g_stMyCar.uEngineRPM += (uTM[g_stMyCar.ubShiftPos] + uTRQ[bAxis]);		/* 回転数 */
			}
			else
			{
				if(g_stMyCar.uEngineRPM > uTRQ[bAxis])
				{
					g_stMyCar.uEngineRPM -= Mmax(uTRQ[bAxis], 0);	/* 回転数 */
				}
			}
			ubShiftPos_N = FALSE;
		}
		else
		{
			if(ubShiftPos_UP == TRUE)	/* シフトアップによる回転数下降 */
			{
				if(g_stMyCar.uEngineRPM > uTRQ[bAxis])
				{
					g_stMyCar.uEngineRPM -= Mmax(uTRQ[bAxis], 0);	/* 回転数 */
				}
				if(TargetRPM > g_stMyCar.uEngineRPM)
				{
					ubShiftPos_N = FALSE;
				}
			}
			else	/* シフトダウンによる回転数上昇 */
			{
				g_stMyCar.uEngineRPM += (uTM[g_stMyCar.ubShiftPos] + uTRQ[bAxis]);		/* 回転数 */
				
				if(TargetRPM < g_stMyCar.uEngineRPM)
				{
					ubShiftPos_N = FALSE;
				}
			}
		}
	}
	else if( g_stMyCar.ubThrottle > 0 )		/* アクセルON中 */
	{
		g_stMyCar.uEngineRPM += (uTM[g_stMyCar.ubShiftPos] + uTorque_Cal) >> 7;	/* 回転数 */
	}
	else	/* アクセルOFF */
	{
		if(g_stMyCar.uEngineRPM > uTorque_Cal)
		{
			g_stMyCar.uEngineRPM -= Mmax(uTorque_Cal, 100);	/* 回転数 */
		}
	}
	
	/* 回転数クリップ */
	g_stMyCar.uEngineRPM = Mmax(Mmin(9000, g_stMyCar.uEngineRPM), 750);
	if(g_stMyCar.uEngineRPM >= 9000)
	{
		if(g_stMyCar.ubShiftPos == 0u)
		{
			g_stMyCar.uEngineRPM = 8200;
		}
		else
		{
			g_stMyCar.uEngineRPM = 8700;
		}
	}
	
	/* エンジンの音 */
	{
		uint32_t uInterval;
		rpm	= Mmax(g_stMyCar.uEngineRPM, 1);
		uInterval = Mmax(Mmin( (40 - Mdiv256(rpm)), 5), 50);
//		uInterval = 10000 - rpm;
		
		if(g_stMyCar.ubThrottle > 0)	/* アクセルON */
		{
			if( GetPassTime( uInterval, &unExplosion_time ) != 0u )	/* 回転数のエンジン音(60000 / rpm) */
	//		if( GetPassTime( 40, &unExplosion_time ) != 0u )	/* 回転数のエンジン音(60000 / rpm) */
			{
				M_Play(rpm);
			}
		}
		else	/* アクセルOFF */
		{
			if( GetPassTime( uInterval, &unExplosion_time ) != 0u )	/* 回転数のエンジン音(60000 / rpm) */
			{
				M_Play(rpm);
			}
		}
	}
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)
	{
#if 0
		uint8_t	bMode;
		uint8_t	str[80];
		int16_t	x, y;
		ST_CRT	stCRT = {0};
		GetGameMode(&bMode);
		GetCRT(&stCRT, bMode);	/* 画面情報を取得 */
		
		/* 座標設定 */
		x = stCRT.hide_offset_x;
		y = stCRT.hide_offset_y + 128;
		sprintf(str, "Base(%3d), Input(%3d), Cal(%d)", uTRQ[bAxis], Input_Torque, uTorque_Cal);
		PutGraphic_To_Symbol(str, x, y, 0x03);	/* メッセージエリア 描画 */
#endif
	}
#endif
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	MyCar_Crash(void)
{
	int16_t	ret = 0;

	uint8_t	bHit = FALSE;
	int16_t	myCarSx, myCarEx, myCarSy, myCarEy;
	uint32_t	i;
	uint32_t	uEnemyNum = 0;
	int16_t	ExtFctDiff = 0;
	int16_t	Pos = 0;

	ST_ENEMYCARDATA	stEnemyCar = {0};

	ST_ROAD_INFO	stRoadInfo;
	int16_t	RoadAngle;
	
#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif

	GetRoadInfo(&stRoadInfo);
	RoadAngle = stRoadInfo.angle;
	
	/* 当たり判定の生成 */
	Pos	= APL_AngleDiff(RoadAngle, g_stMyCar.Angle);
	myCarSx = ROAD_CT_POINT + Pos - 8;
	myCarEx = myCarSx + 16;
	myCarSy = Y_MAX_WINDOW - 32;
	myCarEy = myCarSy + 16;

	
	/* 衝突判定 */
	if(		(g_stMyCar.VehicleSpeed != 0) 		/* 車速あり */
		&&  (g_stMyCar.ubOBD == OBD_NORMAL) )	/* 正常 */
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
						ADPCM_Play(15);	/* スキール音 */
						g_stMyCar.ubOBD |= OBD_SPIN_R;	/* 右スピン */
					}
					else
					{
						ADPCM_Play(15);	/* スキール音 */
						g_stMyCar.ubOBD |= OBD_SPIN_L;	/* 左スピン */
					}
					S_Reset_ScoreID();	/* スコア リセット */
				}
			}
		}
	}
	
	/* 衝突後の車両状態更新 */
	if( bHit == TRUE )
	{
		ADPCM_Play(12);	/* SE：クラッシュ */
		g_stMyCar.ubOBD |= OBD_DAMAGE;	/* 故障 */
		g_stMyCar.uEngineRPM = g_stMyCar.uEngineRPM >> 1;		/* 1/2 */
	}
	else
	{
		if((g_Input & KEY_RIGHT) != 0u)
		{
			if((g_stMyCar.ubOBD & OBD_SPIN_R) != 0u)	/* 右スピン */
			{
				g_stMyCar.ubOBD = Mbclr(g_stMyCar.ubOBD, OBD_SPIN_R);
				ADPCM_Stop();	/* 効果音停止 */
				ADPCM_Play(4);	/* ブレーキ音 */
			}
		}
		else if((g_Input & KEY_LEFT) != 0u)
		{
			if((g_stMyCar.ubOBD & OBD_SPIN_L) != 0u)	/* 左スピン */
			{
				g_stMyCar.ubOBD = Mbclr(g_stMyCar.ubOBD, OBD_SPIN_L);
				ADPCM_Stop();	/* 効果音停止 */
				ADPCM_Play(4);	/* ブレーキ音 */
			}
		}
		else
		{
			
		}
	}

	/* スピン中の車両挙動 */
	/* スピン発生 */
	if((g_stMyCar.ubOBD & OBD_SPIN_R) != 0u)	/* 右スピン */
	{
		ExtFctDiff += g_speed;	/* 右 */
	}
	else if((g_stMyCar.ubOBD & OBD_SPIN_L) != 0u)	/* 左スピン */
	{
		ExtFctDiff -= g_speed;	/* 左 */
	}
	else
	{
		/* 保持 */
	}
	
	/* コーナリング */
#if 1
	if( g_speed != 0u )
	{
		if(RoadAngle != 0)
		{
			ret -= g_speed;	/* TorqueDW ハーフスピン */
			
			ExtFctDiff += Mdiv256(RoadAngle * g_speed);	/* バランス調整要 */
		}
//		else if(RoadAngle < 0)
//		{
//			ExtFctDiff -= RoadAngle * g_speed;	/* バランス調整要 */
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
	
	/* 敵車の車速 更新 */
	if( bHit == TRUE )
	{
		GetEnemyCAR(&stEnemyCar, uEnemyNum);
		if(stEnemyCar.VehicleSpeed == 0)
		{
			/* 何もしない */
		}
		else
		{
			stEnemyCar.VehicleSpeed = g_stMyCar.VehicleSpeed + 5;
		}
		SetEnemyCAR(stEnemyCar, uEnemyNum);	/* 敵車の更新 */
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
static int16_t	MyCar_VehicleSpeed(void)
{
	int16_t	ret = 0;

	int16_t	speed_min;
	int16_t VehicleSpeed_old, VS_sub;
	
	VehicleSpeed_old = g_stMyCar.VehicleSpeed;

	/* 車速 */
	if(g_stMyCar.ubShiftPos == 0u)		/* ニュートラル */
	{
		g_stMyCar.VehicleSpeed = Mdec((uint16_t)g_stMyCar.VehicleSpeed, 1u);	/* 走行抵抗で減速 */
	}
	else
	{
		/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 減速比 4.687 タイヤ周長2052.1mm */
		/* タイヤ周長×６０×回転数／（１０００×変速比×減速比） */
		g_stMyCar.VehicleSpeed = (int16_t)(((uint32_t)26 * g_stMyCar.uEngineRPM) / uTM[g_stMyCar.ubShiftPos]);
		/* ホイルスピン */
		if(VehicleSpeed_old <= g_stMyCar.VehicleSpeed)
		{
			VS_sub = g_stMyCar.VehicleSpeed - VehicleSpeed_old;
		}
		else
		{
			VS_sub = 0;
		}
		if(VS_sub > 20)
		{
			g_stMyCar.VehicleSpeed = VehicleSpeed_old + 1;
		}
		
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
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyCar_Interior(void)
{
	int16_t	ret = 0;

	int16_t	Vibration = 0;

	/* 車体を振動させる */
	Vibration = MyCar_Vibration();
	
	/* マスコット *//* 処理負荷 大 */
	MyCar_Mascot(Vibration);
	
	/* タコメーター針 */
	MyCar_Tachometer(Vibration);
	
	/* ハンドル */
	MyCar_SteeringPos(Vibration);
	
	return ret;
}


/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyCar_CourseOut(void)
{
	int16_t	ret = 0;
	int16_t	Pos;

	ST_ROAD_INFO	stRoadInfo;
	GetRoadInfo(&stRoadInfo);
	
	Pos	= APL_AngleDiff(stRoadInfo.angle, g_stMyCar.Angle);	/* 道路と車の向きの角度差 */

	if( Mabs(Pos) >= 120 )	/* コース外 */
	{
		g_stMyCar.uEngineRPM -= 50;	/* 減速処理 */

		g_stMyCar.ubOBD |= OBD_COURSEOUT;	/* コースアウト */

//		ADPCM_Play(11);	/* SE：コース外 */

		S_Reset_ScoreID();	/* スコア リセット */
	}
	else
	{
		if((g_stMyCar.ubOBD & OBD_COURSEOUT) != 0u)
		{
			g_stMyCar.ubOBD = Mbclr(g_stMyCar.ubOBD, OBD_COURSEOUT);	/* コースアウト解除 */
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
int16_t	GetMyCarSpeed(int16_t *speed)
{
	int16_t	ret = 0;
	
	*speed = g_speed;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void MyCar_Image(void)
{
	int16_t ret = 0;
	int32_t i;
	int32_t x = 0, y = 0;
	
	/* FPS */
	for(i=0; i < MYCAR_IMAGE_MAX; i++)
	{
		uint32_t	uWidth, uHeight, uFileSize;
		uint16_t	uCG_Num;
		
		uCG_Num = MYCAR_CG + i;
		ret = G_Load_Mem( uCG_Num, 0, 0, 0 );	/* MYCAR_CG */
		if(ret != 0)break;

		Get_PicImageInfo( uCG_Num, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
		
		/* テキスト */
		PutGraphic_To_Text( uCG_Num, uWidth * x, Y_OFFSET * y );			/* インテリア */

//		T_Fill( (uWidth * x) + 82, (Y_OFFSET * y) + 188, 32, 31, 0, 0);	/* メーター穴 */
		T_Circle( 82 + (uWidth * x), 188 + (Y_OFFSET * y), 32, 32, 0, 0);	/* メーター穴 */

#ifdef DEBUG
//		T_Fill( (uWidth * x) + (16 * i), (Y_OFFSET * y) + 164, 16, 16, 0, 0);	/* デバッグ用 */
#endif
		T_Fill( uWidth * x, (Y_OFFSET * y) + 224, 255, 2, 0, 0x0F);		/* 下帯 */
		
		x++;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	MyCar_Vibration(void)
{
	int16_t ret = 0;

	/* 画面を揺らす */
	VibrationCT++;
	if(VibrationCT > 360)
	{
		VibrationCT = 0;
	}
	
	if(g_stMyCar.ubOBD == OBD_NORMAL)
	{
		Vibration = Mdiv256( (1 * APL_Sin(VibrationCT)) );	/* 画面の振動 */
	}
	else
	{
		if(ubOBD_old == OBD_NORMAL)
		{
			if( (g_stMyCar.ubOBD & OBD_DAMAGE) != 0u )
			{
				CrashCount = 30;
			}
			else if( (g_stMyCar.ubOBD & OBD_COURSEOUT) != 0u )
			{
				CrashCount = 15;
			}
			else
			{
				/* 何もしない */
			}
		}

		if(CrashCount == 0)
		{
			g_stMyCar.ubOBD = Mbclr(g_stMyCar.ubOBD, (OBD_DAMAGE|OBD_COURSEOUT));
			Vibration = 0;
		}
		else
		{
			CrashCount = Mdec((int16_t)CrashCount, 1u);	/* カウンタデクリメント */
			
			Vibration = Mdiv256( APL_Sin(VibrationCT) );	/* 画面の振動 */
		}
	}
	ubOBD_old = g_stMyCar.ubOBD;	/* 前回値更新 */
	
	ret = Vibration;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyCar_Mascot(int16_t Vibration)
{
	int16_t ret = 0;

	int16_t	x, y;
	uint16_t	nRatio = 0x80;
	uint16_t	Speed;
#if 0
	uint8_t	palNum = 0;
	uint8_t	sp_num=0;
	
	volatile uint16_t *PCG_src  = (uint16_t *)0xEBA800;
	volatile uint16_t *PCG_dst  = (uint16_t *)0xEBA980;
#endif

	ST_PCG	*p_stPCG = NULL;

#ifdef	DEBUG
	GetDebugNum(&nRatio);
#endif

#if 0
	Speed = g_stMyCar.VehicleSpeed;
#else
	Speed = Mdiv256(Vibration) & 0xFF;
#endif

#if 1
	/* マスコットが揺れる */
	if(Speed == 0)
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
		uint16_t	width;
		
		width = 15 + (Speed / 10);
		
		if(ubRadFlag == TRUE)
		{
			rad+=Mmax(Speed/30, 1);
			if(rad > 180 + width)ubRadFlag = FALSE;
		}
		else
		{
			rad-=Mmax(Speed/30, 1);
			if(rad < 180 - width)ubRadFlag = TRUE;
		}
	}
	

	x = 16 + (( 32 * APL_Sin(rad)) >> 8);
#if 0
	y =  0 + ((-32 * APL_Cos(rad)) >> 8) - Vibration;
#else
	y =  0 + ((-32 * APL_Cos(rad)) >> 8);
#endif


#if 1
	p_stPCG = PCG_Get_Info(ETC_PCG_KISARA);	/* キサラ */
//	p_stPCG = PCG_Get_Info(ETC_PCG_SONIC);	/* ソニック */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = x;
		p_stPCG->y = y;
		p_stPCG->update	= TRUE;
	}
#else
	palNum = 9;
	PCG_Rotation((uint16_t *)PCG_dst, (uint16_t *)PCG_src, 3, 3, x, y, &sp_num, palNum, (nRatio-0x80), 180-rad);
#endif

#endif
	
	return ret;
}


/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t	MyCar_Tachometer(int16_t Vibration)
{
	int16_t ret = 0;
	
	int16_t	i = 0, dwRev = 0;
	int16_t	x, y;

	ST_PCG	*p_stPCG = NULL;

	/* タコメーター針 */
	do
	{
		if( (dwRev <= g_stMyCar.uEngineRPM) && (g_stMyCar.uEngineRPM < (dwRev + 384)) )
		{
			/* 回転数でスプライトのパターンを決める */
			break;
		}
		dwRev += 384;	/* 9216rpm / 24p = 384rpm/p */
		i++;
	}while(i<24);
	
	x =  81 + SP_X_OFFSET;
	y = 188 + SP_Y_OFFSET;

	if(i < 8)
	{
		y += SP_H;
		i = 7 - i;
	}
	else if(i < 16)
	{
		
	}
	else
	{
		x += SP_W;
		i = 16+23 - i;
	}
	
	p_stPCG = PCG_Get_Info(MYCAR_PCG_NEEDLE);	/* タコメーター針 */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = x;
		p_stPCG->y = y;
		p_stPCG->Anime = i;
		p_stPCG->update	= TRUE;
	}
	/* タコメーター */
	x =  81 + SP_X_OFFSET;
	y = 188 + SP_Y_OFFSET;
	p_stPCG = PCG_Get_Info(MYCAR_PCG_TACHO);	/* タコメーター */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = x;
		p_stPCG->y = y;
		p_stPCG->update	= TRUE;
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
static int16_t	MyCar_SteeringPos(int16_t Vibration)
{
	int16_t ret = 0;
	int16_t	x, y;
	uint32_t	uWidth, uHeight, uFileSize;
	ST_TEXTINFO	stTextInfo;
	ST_PCG	*p_stPCG = NULL;

	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	
	/* ステアリング位置 */
	x =  16 + Mdiv16(APL_Sin(g_stMyCar.Steering)) + SP_X_OFFSET - 4;
	y = 128 - Mdiv16(APL_Cos(g_stMyCar.Steering)) + SP_Y_OFFSET - 4;
	p_stPCG = PCG_Get_Info(MYCAR_PCG_STEERING_POS);	/* ステアリングポジション */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = x;
		p_stPCG->y = y;
		p_stPCG->update	= TRUE;
	}
	
	/* ステアリング */
	x =   0 + SP_X_OFFSET;
	y = 112 + SP_Y_OFFSET;
	p_stPCG = PCG_Get_Info(MYCAR_PCG_STEERING);	/* ステアリング */
	if(p_stPCG != NULL)
	{
		p_stPCG->x = x;
		p_stPCG->y = y;
		p_stPCG->update	= TRUE;
	}

	/* 画面を切り替える */
	if(g_stMyCar.Steering == 0)
	{
		x = 1;
		y = 0;
	}
	else
	{
		if(g_stMyCar.Steering < 0)
		{
			x = 0;
			y = 0;
		}
		else
		{
			x = 2;
			y = 0;
		}
	}

	/* テキスト画面のポジションを更新する */
	T_Get_TextInfo(&stTextInfo);
	stTextInfo.uPosX = x;	/* X座標 */
	stTextInfo.uPosY = y;	/* Y座標 */
	T_Set_TextInfo(stTextInfo);
	
	return ret;
}

#endif	/* MyCar_C */

