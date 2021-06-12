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
#include "Text.h"

/* define定義 */

/* グローバル変数 */
int16_t	g_speed = 0;
static int16_t g_Input;
static int16_t g_SteeringDiff;

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
int16_t	MyCarInfo_Update(int16_t);
static int16_t	MyCar_Steering(void);
static int16_t	MyCar_ShiftPos(void);
static int16_t	MyCar_Accel(void);
static int16_t	MyCar_Brake(void);
static int16_t	MyCar_EngineSpeed(int16_t);
static int16_t	MyCar_Crash(void);
static int16_t	MyCar_VehicleSpeed(void);
int16_t	MyCar_Interior(uint8_t);
int16_t	MyCar_CourseOut(void);
int16_t	GetMyCarSpeed(int16_t *);
void MyCar_Image(void);
static int16_t	MyCar_Vibration(void);
static int16_t	MyCar_Mascot(int16_t);
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
int16_t	MyCarInfo_Update(int16_t input)
{
	int16_t	ret = 0;

	int16_t	Torque = 0;

	g_Input = input;				/* 入力値更新 */
	
	Torque += MyCar_Steering();		/* ステアリング操作 */
	
	Torque += MyCar_ShiftPos();		/* シフト操作 */
	
	Torque += MyCar_Accel();		/* アクセル操作 */
	
	Torque += MyCar_Brake();		/* ブレーキ操作 */
	
	Torque += MyCar_Crash();		/* 衝突判定 */
	
	Torque += MyCar_CourseOut();	/* コースアウト時の処理 */
	
	MyCar_EngineSpeed(Torque);		/* エンジン回転数の算出 */
	
	MyCar_VehicleSpeed();			/* 車速の算出 */
	
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

	int16_t	Angle;
	int16_t	SteeringDiff = 0;
	int16_t	ExtFctDiff = 0;
	int16_t	Steering_old;
	int16_t	AnalogMode = 0;

	ST_ROAD_INFO	stRoadInfo;
	JOY_ANALOG_BUF stAnalog_Info;
	
	AnalogMode = GetAnalog_Info(&stAnalog_Info);	/* アナログ情報取得 */
	
	GetRoadInfo(&stRoadInfo);
	Angle = stRoadInfo.angle;
	Steering_old = g_stMyCar.Steering;
	
	if((g_Input & KEY_RIGHT) != 0u)
	{
		if((g_stMyCar.ubOBD & OBD_SPIN_R) != 0u)	/* 右スピン */
		{
			g_stMyCar.ubOBD = Mbclr(g_stMyCar.ubOBD, OBD_SPIN_R);
			ADPCM_Stop();	/* 効果音停止 */
			ADPCM_Play(4);	/* ブレーキ音 */
		}
		ret -= 1;	/* TorqueDW パワステ駆動減 */

		if(AnalogMode == 0)	/* アナログモード */
		{
			SteeringDiff += (stAnalog_Info.r_stk_lr - 0x80);
		}
		else
		{
			SteeringDiff += Mmul16(g_speed);	/* 右 */
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
		ret -= 1;	/* TorqueDW パワステ駆動減 */

		if(AnalogMode == 0)	/* アナログモード */
		{
			SteeringDiff -= (0x80 - stAnalog_Info.r_stk_lr);
		}
		else
		{
			SteeringDiff -= Mmul16(g_speed);	/* 左 */
		}
	}
	else
	{
		
	}
	g_SteeringDiff = SteeringDiff;
	
	/* スピン中の車両挙動 */
	/* スピン発生 */
	if((g_stMyCar.ubOBD & OBD_SPIN_R) != 0u)	/* 右スピン */
	{
		ret -= 10;	/* TorqueDW スピン */
		
		ExtFctDiff += Mmul8(g_speed);	/* 右 */
	}
	else if((g_stMyCar.ubOBD & OBD_SPIN_L) != 0u)	/* 左スピン */
	{
		ret -= 10;	/* TorqueDW スピン */

		ExtFctDiff -= Mmul8(g_speed);	/* 左 */
	}
	else
	{
		/* 保持 */
	}
	
	/* コーナリング */
#if 1
	if( g_speed != 0u )
	{
		if(Angle != 0)
		{
			ret -= g_speed;	/* TorqueDW ハーフスピン */
			
			ExtFctDiff += Angle * g_speed;	/* バランス調整要 */
		}
//		else if(Angle < 0)
//		{
//			ExtFctDiff -= Angle * g_speed;	/* バランス調整要 */
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
	
	/* 外部要因 */
	g_stMyCar.Steering = Steering_old + SteeringDiff + ExtFctDiff;
	/* ステアリングクリップ */
	g_stMyCar.Steering = Mmax(Mmin(g_stMyCar.Steering, 3800), -3800);
	
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

	static uint8_t bShiftPosFlag[3] = {FALSE};

	if( ((g_Input & KEY_A) != 0u) && ((g_Input & KEY_B) == 0u))	/* Aボタン(only) */
	{
		/* アクセルだけはシフト操作禁止 */
	}
	else{
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
	
	static int8_t	bThrottleON_Count = 0;
	static int8_t	bThrottle_Flag = FALSE;

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
static int16_t	MyCar_EngineSpeed(int16_t Input_Torque)
{
	int16_t	ret = 0;

	uint8_t	bAxis;
	uint32_t	i;
	uint32_t	time;
	uint16_t	uTorque_Cal;
	int16_t		rpm;
	int16_t		TargetRPM = 0;
	static uint8_t ubShiftPos_N = FALSE;
	static uint8_t ubShiftPos_UP = FALSE;
	static uint8_t ubShiftPos_old = 0;
	static	uint32_t	unExplosion_time = 0xFFFFFFFFu;

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

	ST_ENEMYCARDATA	stEnemyCar = {0};
	
#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	/* 当たり判定の生成 */
	myCarSx = ROAD_CT_POINT + (g_stMyCar.Steering >> 8) - 8;
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

	/* 敵車の車速 更新 */
	if( bHit == TRUE )
	{
		GetEnemyCAR(&stEnemyCar, uEnemyNum);
		stEnemyCar.VehicleSpeed = g_stMyCar.VehicleSpeed + 5;
		SetEnemyCAR(stEnemyCar, uEnemyNum);	/* 敵車の更新 */
	}
	
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)
	{
		uint16_t color;
		uint8_t	bMode;
		
		ST_CRT	stCRT;
		
		switch(g_stMyCar.ubOBD)
		{
			case OBD_NORMAL:
			{
				color = 0x0B;
				break;
			}
			case OBD_DAMAGE:
			{
				color = 0x0A;
				break;
			}
			case OBD_SPIN_L:
			case OBD_SPIN_R:
			{
				color = 0x0C;
				break;
			}
			case OBD_COURSEOUT:
			{
				color = 0x0E;
				break;
			}
			default:
			{
				color = 0x00;
				break;
			}
		}
		
		GetGameMode(&bMode);
		GetCRT(&stCRT, bMode);
		
		Draw_Box(
			stCRT.hide_offset_x + myCarSx,
			stCRT.hide_offset_y + myCarSy,
			stCRT.hide_offset_x + myCarEx,
			stCRT.hide_offset_y + myCarEy, color, 0xFFFF);
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
int16_t	MyCar_Interior(uint8_t bMode)
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
	int16_t	vx;
	vx	= g_stMyCar.Steering;

	if( Mabs(vx) > 2800 )	/* コース外 */
	{
		int16_t	rpm	= g_stMyCar.uEngineRPM;
		
		g_stMyCar.uEngineRPM -= (rpm>>3);	/* 減速処理 */

		g_stMyCar.ubOBD |= OBD_COURSEOUT;	/* コースアウト */

//		ADPCM_Play(11);	/* SE：コース外 */

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
	int32_t i;
	int32_t x = 0, y = 0;
	
	/* FPS */
	for(i=0; i < MYCAR_IMAGE_MAX; i++)
	{
		uint32_t	uWidth, uHeight, uFileSize;
		uint16_t	uCG_Num;
		
		uCG_Num = MYCAR_CG + i;
		CG_File_Load(uCG_Num);	/* グラフィックの読み込み */

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

	static	int16_t	Vibration = 0;
	static	int16_t	VibrationCT = 0;
	static	uint8_t	ubOBD_old = OBD_NORMAL;
	static	int16_t	CrashCount;

	/* 画面を揺らす */
	VibrationCT++;
	if(VibrationCT > 5)
	{
		VibrationCT = 0;
	}
	
	if(g_stMyCar.ubOBD == OBD_NORMAL)
	{
		Vibration = (VibrationCT == 0)?1:0;	/* 画面の振動 */
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
			
			Vibration = (VibrationCT == 0)?8:0;	/* 画面の振動 */
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
static int16_t	MyCar_Mascot(int16_t Vibration)
{
	int16_t ret = 0;

	uint16_t	nRatio = 0x80;
#if 0
	int16_t	x, y;
	uint8_t	palNum = 0;
	uint8_t	sp_num=0;
	
	volatile uint16_t *PCG_src  = (uint16_t *)0xEBA000;
	volatile uint16_t *PCG_dst  = (uint16_t *)0xEBA180;

	static	uint16_t	rad = 180;
	static	uint8_t	ubRadFlag = TRUE;
#endif

#ifdef	DEBUG
	GetDebugNum(&nRatio);
#endif

#if 0
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
		uint16_t	width;
		
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

	PCG_Rotation((uint16_t *)PCG_dst, (uint16_t *)PCG_src, 3, 3, x, y, &sp_num, palNum, (nRatio-0x80), 180-rad);

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
	
	int16_t	i;
	uint8_t	patNum = 0;
	uint8_t	palNum = 0;
	uint8_t	V=0, H=0;
	uint8_t	sp_num=0;
	int16_t	x, y;

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
	x = 82+16;
	y = 188+16 - Vibration;
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
	x = 82+16;
	y = 188+16 - Vibration;
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

	uint8_t	patNum = 0;
	uint8_t	palNum = 0;
	uint8_t	V=0, H=0;
	uint8_t	sp_num=5;
	int16_t	x, y;
	uint32_t	uWidth, uHeight, uFileSize;
	ST_TEXTINFO	stTextInfo;

	Get_PicImageInfo( MYCAR_CG, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	
	/* ステアリング位置 */
	x = 16 + Mdiv16(g_SteeringDiff) + 16 - 2;
	y = 96 + Mabs(Mdiv16(g_SteeringDiff)) + 16 + 2;
	V = 0;
	H = 0;
	palNum = 0x0C;
	patNum = 0x4A;
	SP_REGST( sp_num++, -1, x + 0, y + 0, SetBGcode(V, H, palNum, patNum), 3);
	
	/* ステアリング */
	x = 0+16;
	y = 96+16;
	V = 0;
	H = 0;
	palNum = 0x0B;
	patNum = 0x48;
	SP_REGST( sp_num++, -1, x + 0, y + 0, SetBGcode(V, H, palNum, patNum), 3);
	patNum = 0x49;
	SP_REGST( sp_num++, -1, x + 16, y + 0, SetBGcode(V, H, palNum, patNum), 3);
	patNum = 0x58;
	SP_REGST( sp_num++, -1, x + 0, y + 16, SetBGcode(V, H, palNum, patNum), 3);
	patNum = 0x59;
	SP_REGST( sp_num++, -1, x + 16, y + 16, SetBGcode(V, H, palNum, patNum), 3);

	/* 画面を切り替える */
	if(g_SteeringDiff == 0)
	{
		x = 1;
		y = 0;
	}
	else if(g_SteeringDiff < 0)
	{
		if(Mabs(g_SteeringDiff) > 0x40)
		{
			x = 0;
			y = 0;
		}
		else
		{
			x = 1;
			y = 0;
		}
	}
	else
	{
		if(Mabs(g_SteeringDiff) > 0x40)
		{
			x = 2;
			y = 0;
		}
		else
		{
			x = 1;
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

