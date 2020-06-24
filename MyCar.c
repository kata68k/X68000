#ifndef	MyCar_C
#define	MyCar_C

#include "inc/usr_macro.h"
#include "MyCar.h"
#include "Input.h"

SS	GetMyCarInfo(ST_CARDATA *, SS);

SS	GetMyCarInfo(ST_CARDATA *stCarData, SS input)
{
	static UC bShiftPosFlag[2] = {FALSE};

	US	uTM[6] = { 0, 2857, 1950, 1444, 1096, 761 };/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 */
	
	/* ステアリング操作 */
	if((input & KEY_RIGHT) != 0u)	stCarData->Steering += 1;	/* 右 */
	if((input & KEY_LEFT) != 0u)	stCarData->Steering -= 1;	/* 左 */
	stCarData->Steering = Mmax(Mmin(stCarData->Steering, 31), -31);
	
	/* アクセル */
	if((input & KEY_A) != 0u){	/* Aボタン */
		/* 回転数 */
//			if(b8ms_time == TRUE)
		{
			stCarData->uEngineRPM += 125;
		}
	}
	else{
		/* 回転数 */
//			if(b8ms_time == TRUE)
		{
			stCarData->uEngineRPM -= 100;
		}
	}
	stCarData->uEngineRPM = Mmax(Mmin(9000, stCarData->uEngineRPM), 750);

	/* シフト操作 */
	if(ChatCancelSW((input & KEY_UPPER)!=0u, &bShiftPosFlag[0]) == TRUE)
	{
		if(stCarData->ubShiftPos != 5)	m_pcmplay(7,3,4);	/* SE:シフトアップ */
		stCarData->ubShiftPos = Mmin(stCarData->ubShiftPos+1, 5);	/* 上 */
	}
	
	if(ChatCancelSW((input & KEY_LOWER)!=0u, &bShiftPosFlag[1]) == TRUE)
	{
		if(stCarData->ubShiftPos != 0)	m_pcmplay(7,3,4);	/* SE:シフトダウン */
		stCarData->ubShiftPos = Mmax(stCarData->ubShiftPos-1, 0);	/* 下 */
	}
#if 0
	stCarData->ubShiftPos = 2;	/* テスト用変速固定 */
#endif	
	stCarData->ubShiftPos = Mmax(Mmin(stCarData->ubShiftPos, 5), 0);
	
	/* 車速 */
	if(stCarData->ubShiftPos == 0u)		/* ニュートラル */
	{
		stCarData->VehicleSpeed -= 1;
	}
	else
	{
		/* 変速比  1:2.857 2:1.95 3:1.444 4:1.096 5:0.761 減速比 4.687 タイヤ周長2052.1mm */
		/* タイヤ周長×６０×回転数／（１０００×変速比×減速比） */
		stCarData->VehicleSpeed = (SS)(((UI)26 * stCarData->uEngineRPM) / uTM[stCarData->ubShiftPos]);	
	}
	
	/* ブレーキ */
	if( (input & KEY_B) != 0U )	/* Bボタン */
	{
		stCarData->VehicleSpeed -= 30;	/* 変速比で変えたい */
		stCarData->ubBrakeLights = TRUE;	/* ブレーキランプ ON */
		m_pcmplay(4,3,4);	/* ブレーキ音 */
	}
	else
	{
		stCarData->ubBrakeLights = FALSE;	/* ブレーキランプ OFF */
	}
	stCarData->VehicleSpeed = Mmax(Mmin(310, stCarData->VehicleSpeed), 0);
}

#endif	/* MyCar_C */

