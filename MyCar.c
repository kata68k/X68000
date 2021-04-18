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
int16_t	g_speed = 0;
static int16_t g_Input;
static 

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
void MyCar_Background(void);
static int16_t	MyCar_Vibration(void);
static int16_t	MyCar_Mascot(int16_t);
static int16_t	MyCar_Tachometer(int16_t);

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
	
	MyCar_Background();	/* 背景の表示 */

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
	ST_ROAD_INFO	stRoadInfo;
	
	GetRoadInfo(&stRoadInfo);
	Angle = stRoadInfo.angle;
	
	if((g_Input & KEY_RIGHT) != 0u)
	{
		if(g_stMyCar.ubOBD == OBD_SPIN_R)	/* 右スピン */
		{
			ADPCM_Play(4);	/* ブレーキ音 */
		}
		ret = -30;	/* TorqueDW パワステ駆動減 */

		g_stMyCar.Steering += g_speed << 4;	/* 右 */
	}
	else if((g_Input & KEY_LEFT) != 0u)
	{
		if(g_stMyCar.ubOBD == OBD_SPIN_L)	/* 左スピン */
		{
			ADPCM_Play(4);	/* ブレーキ音 */
		}
		ret = -30;	/* TorqueDW パワステ駆動減 */

		g_stMyCar.Steering -= g_speed << 4;	/* 左 */
	}
	else
	{
		
	}
	
	/* スピン中の車両挙動 */
	/* スピン発生 */
	if(g_stMyCar.ubOBD == OBD_SPIN_R)	/* 右スピン */
	{
		g_stMyCar.Steering += g_speed << 3;	/* 右 */
	}
	else if(g_stMyCar.ubOBD == OBD_SPIN_L)	/* 左スピン */
	{
		g_stMyCar.Steering -= g_speed << 3;	/* 左 */
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

	/* アクセル */
	if((g_Input & KEY_A) != 0u)		/* Aボタン */
	{
		if(g_stMyCar.ubThrottle < 255)
		{
			g_stMyCar.ubThrottle += 1;	/* スロットル開度 */
		}
	}
	else
	{
		if(g_stMyCar.ubThrottle > 16)
		{
			g_stMyCar.ubThrottle -= 16;	/* スロットル開度 */
		}
		else
		{
			g_stMyCar.ubThrottle = 0;	/* スロットル開度 */
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
		ret = 30;	/* TorqueUP マスターバックからの空気 */
		
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
	static uint8_t ubShiftPos_old = 0;
	static uint16_t uEngSound = 0u;

	GetStartTime(&time);	/* 開始時刻を取得 */

	/* シフトチェンジによるエンジン回転数の更新 */
	if(g_stMyCar.ubShiftPos != ubShiftPos_old) 
	{
		/* 車速からエンジン回転数を算出 */
		g_stMyCar.uEngineRPM = (g_stMyCar.VehicleSpeed * uTM[g_stMyCar.ubShiftPos]) / 26;
	}
	ubShiftPos_old = g_stMyCar.ubShiftPos;
	
	/* ブレーキ中エンジン回転数の更新 */
	if(g_stMyCar.ubBrakeLights == TRUE)
	{
		/* 車速からエンジン回転数を算出 */
		g_stMyCar.uEngineRPM = (g_stMyCar.VehicleSpeed * uTM[g_stMyCar.ubShiftPos] ) / 26u;	
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
	uTorque_Cal = (uint16_t)Mmax((int16_t)uTRQ[bAxis] + Input_Torque, 0);
	
	/* アクセル */
	if(g_stMyCar.ubThrottle > 0)
	{
		if(g_stMyCar.ubShiftPos == 0u)
		{
			g_stMyCar.uEngineRPM += (uTM[g_stMyCar.ubShiftPos] + uTRQ[bAxis]);		/* 回転数 */
		}
		else
		{
			g_stMyCar.uEngineRPM += (uTM[g_stMyCar.ubShiftPos] + uTorque_Cal) >> 7;		/* 回転数 */
		}

		/* エンジンの音 */
		{
			static	uint32_t	unExplosion_time = 0;
			int16_t	rpm;

			rpm	= Mmax(g_stMyCar.uEngineRPM, 1);
			
			if( (time - unExplosion_time) >  (150 - Mdiv64(rpm)) )	/* 回転数のエンジン音(60000 / rpm) */
			{
				unExplosion_time = time;
				if(uEngSound == 0u)
				{
					M_Play(rpm / 100);
	//				SE_Play_Fast(6);	/* FM効果音再生(高速) */
	//				SE_Play(7);	/* FM効果音再生 */
	//				SE_Play(6);	/* FM効果音再生 */
//					uEngSound = 8u;
				}
			}
			else
			{
				/* 何もしない */
			}
		}
	}
	else{
		static	uint32_t	unFuelCut_time = 0;
		int16_t	rpm;
		rpm	= Mmax(g_stMyCar.uEngineRPM, 1);
		
		if(g_stMyCar.ubShiftPos == 0u)
		{
			g_stMyCar.uEngineRPM -= uTRQ[bAxis];	/* 回転数 */
		}
		else
		{
			g_stMyCar.uEngineRPM -= uTorque_Cal;	/* 回転数 */
		}
		
		/* エンジンの音(燃料カット) */
		if( (time - unFuelCut_time) >  (150 - Mdiv64(rpm)) )	/* 回転数のエンジン音(60000 / rpm) */
		{
			unFuelCut_time = time;
			if(g_stMyCar.ubThrottle > 0)
			{
				if(uEngSound == 0u)
				{
					M_Play(rpm / 100);
	//				SE_Play(6);	/* FM効果音再生 */
	//				SE_Play(7);	/* FM効果音再生 */
//					uEngSound = 8u;
				}
			}
			else
			{
				if(uEngSound == 0u)
				{
					M_Play(rpm / 100);
	//				SE_Play(8);	/* FM効果音再生 */
//					uEngSound = 8u;
				}
			}
		}
	}
	uEngSound = Mdec((uint16_t)uEngSound, 1u);
	
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
						g_stMyCar.ubOBD = OBD_SPIN_R;	/* 右スピン */
					}
					else
					{
						g_stMyCar.ubOBD = OBD_SPIN_L;	/* 左スピン */
					}
				}
			}
		}
	}
	
	/* 衝突後の車両状態更新 */
	if( bHit == TRUE )
	{
		g_stMyCar.ubOBD = OBD_DAMAGE;	/* 故障 */
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
				color = 0x03;
				break;
			}
			case OBD_DAMAGE:
			{
				color = 0x13;
				break;
			}
			case OBD_SPIN_L:
			case OBD_SPIN_R:
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
	/* 未実装 */
	
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
	/* FPS */
#if 1	/* テキスト */
	PutGraphic_To_Text( 0, 0, 0			);	/* インテリア */
	PutGraphic_To_Text( 0, 0, Y_OFFSET	);	/* インテリア */

	T_Fill( 90,        0 + 180, 32, 31, 0, 0);		/* メーター穴（上） */
	T_Fill( 90, Y_OFFSET + 180, 32, 31, 0, 0);		/* メーター穴（下） */

	T_Fill(  0,        0 + 224, 255, 2, 0, 0x0F);	/* 下帯（上） */
	T_Fill(  0, Y_OFFSET + 224, 255, 2, 0, 0x0F);	/* 下帯（下） */
#else
	int16_t x;

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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void MyCar_Background(void)
{
	int16_t ret;
	uint16_t height_sum = 0u;
	uint16_t height_sum_o = 0u;
	uint32_t i;
	uint32_t uWidth, uHeight, uFileSize;
	uint32_t uWidth_o, uHeight_o;
	uint32_t uOffset_X = 0u;
	
	ret = G_Load_Mem( ENEMYCAR_CG, uOffset_X,	0,	0);	/* ライバル車 */
	Get_PicImageInfo( ENEMYCAR_CG, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */
	uWidth_o = uWidth;
	uHeight_o = uHeight;

	if(ret >= 0)
	{
		/* 11パターンを作る */
		uint32_t	uW_tmp, uH_tmp;
		
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
		uint32_t	uW_tmp, uH_tmp;
		
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

	int16_t	x, y;
	uint8_t	bMode;
	ST_CRT	stCRT;

	static	int16_t	Vibration = 0;
	static	int16_t	VibrationCT = 0;
	static	uint8_t	ubOBD_old = OBD_NORMAL;
	static	int16_t	CrashCount;
	
	GetGameMode(&bMode);
	GetCRT(&stCRT, bMode);

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
			if( g_stMyCar.ubOBD == OBD_DAMAGE )
			{
				ADPCM_Play(12);	/* SE：クラッシュ */
				CrashCount = 30;
			}
			else if( g_stMyCar.ubOBD == OBD_COURSEOUT )
			{
				ADPCM_Play(11);	/* SE：コース外 */
				CrashCount = 15;
			}
			else
			{
				/* 何もしない */
			}
		}
		
		if( g_stMyCar.ubOBD == OBD_DAMAGE )
		{
			if(CrashCount == 22)	/* 次のSEへ */
			{
				ADPCM_Play(15);	/* スキール音 */
			}
			CrashCount = Mdec((int16_t)CrashCount, 1u);	/* カウンタデクリメント */

		}
		else if( g_stMyCar.ubOBD == OBD_COURSEOUT )
		{
			if((CrashCount % 2) == 0)	/* 次のSEへ */
			{
				ADPCM_Play(11);	/* SE：コース外 */
			}
			CrashCount = Mdec((int16_t)CrashCount, 1u);	/* カウンタデクリメント */
		}
		else
		{
			CrashCount = Mdec((int16_t)CrashCount, 1u);	/* カウンタデクリメント */
		}

		if(CrashCount == 0)
		{
			/* 動画 */
			//MOV_Play(3);	/* 嘘をつくな */
			Vibration = 0;
		}
		else
		{
			Vibration = (VibrationCT == 0)?8:0;	/* 画面の振動 */
		}
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
	
	int16_t	x, y;
	uint16_t	nRatio = 0x80;
	uint8_t	palNum = 0;
	uint8_t	sp_num=0;
	
	volatile uint16_t *PCG_src  = (uint16_t *)0xEBA000;
	volatile uint16_t *PCG_dst  = (uint16_t *)0xEBA180;

	static	uint16_t	rad = 180;
	static	uint8_t	ubRadFlag = TRUE;

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

