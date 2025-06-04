#ifndef	IF_INPUT_C
#define	IF_INPUT_C

#include <stdio.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>

#include <usr_macro.h>
#include "IF_Input.h"
#include "BIOS_MPU.h"
#include "BIOS_MFP.h"
#include "IF_MUSIC.h"

#if CNF_JOYDRV360
#include <JOYLIB3.H>
#else
uint32_t	get_analog_data(uint32_t, JOY_ANALOG_BUF *);
#endif

#define		IPUT_INTERVAL_TIME	(1000)

uint32_t	g_Input;
uint16_t	g_Input_P[2];
int16_t		g_AnalogMode = 0xFFFF;
uint8_t		g_bAnalogStickMode = FALSE;
uint8_t		g_bAnalogStickMode_flag;
int16_t		g_Analog_Info[5];
static uint32_t s_PassTime;

/* 関数のプロトタイプ宣言 */
int16_t		Input_Init(void);
int16_t		Input_Main(uint8_t);
uint16_t	get_keyboard( uint16_t *, uint8_t , uint8_t );
uint16_t	get_djoy(uint16_t *, int32_t, uint8_t );
uint16_t	get_ajoy(uint16_t *, int32_t, uint8_t, uint8_t );
int16_t		GetAnalog_Info(int16_t *);
int16_t		SetAnalog_Info(int16_t *);
int16_t		GetJoyDevice(int32_t, int32_t, int32_t);
int16_t		GetJoyDevMode(int32_t);
int16_t		SetJoyDevMode(int32_t , int32_t , int32_t);
uint16_t	DirectInputKeyNum(uint16_t *, uint16_t );
uint8_t	ChatCancelSW(uint8_t , uint8_t *);
int16_t	KeyHitESC(void);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Input_Init(void)
{
	int16_t ret = 0;

#if CNF_JOYDRV360
	/* アナログスティックモード判定 */
	if(SetJoyDevMode(0, 0, 0) < 0)	/* JoyNo:0 DevID:0 PortNo:0 */
	{
		puts("JOYDRV3.Xが常駐してません。");
	}
	#if 0	
	switch(GetJoyDevice(1, 1, 0))
	{
		case 0:
		default:
			puts("App_init Digital");	
			SetJoyDevMode(0, 0, 0);	/* JoyNo:0 DevID:0 PortNo:0 */
			break;
		case 1:
			puts("App_init Analog");
			SetJoyDevMode(1, 1, 0);	/* JoyNo:1 DevID:1 PortNo:0 */
			break;
	}
	#endif
#else
	g_bAnalogStickMode = FALSE;	/* デジタル */
#endif
	GetNowTime(&s_PassTime);	/* タイムアウトカウンタリセット */

	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Input_Main(uint8_t ubAuto)
{
	int16_t ret = 0;
	int16_t	input = 0;

	get_keyboard(&input, 0, 1);		/* キーボード入力, JoyNo, mode=0:edge on 1:edge off */

	/* アナログスティック／デジタルスティック切替 */
	if(ChatCancelSW((input & KEY_b_TAB)!=0u, &g_bAnalogStickMode_flag) == TRUE)	/* TABでアナログスティックON/OFF */
	{
#if CNF_JOYDRV360				
		if(g_bAnalogStickMode == TRUE)	/* 現在：アナログスティックモードの場合 */
		{
			g_bAnalogStickMode = FALSE;
//					puts("A to D");
			SetJoyDevMode(0, 0, 0);	/* JoyNo:0 DevID:0 PortNo:0 */
			if(GetJoyDevMode(0) == 0)
			{
//						puts("A to D OK");
			}
		}
		else
		{
			g_bAnalogStickMode = TRUE;
//					puts("D to A");
			SetJoyDevMode(1, 1, 0);	/* JoyNo:1 DevID:1 PortNo:0 */
			if(GetJoyDevMode(1) == 0)
			{
//						puts("D to A OK");
			}
		}
#else
		g_bAnalogStickMode = FALSE;
#endif
	}


	if(ubAuto == FALSE)
	{
		if(g_bAnalogStickMode == TRUE)
		{
	#if CNF_JOYDRV360				
			get_ajoy(&input, 1, 0, 0);	/* アナログジョイスティック入力, JoyNo, mode=0:edge on 1:edge off, Config 0:X680x0 1:rev */
	#else
			get_djoy(&input, 0, 1);		/* ジョイスティック入力, JoyNo, mode=0:edge on 1:edge off */
	#endif
			g_Input = input;	/* 通常の入力 */
		}
		else
		{
			get_djoy(&input, 1, 1);		/* ジョイスティック入力, JoyNo, mode=0:edge on 1:edge off */
			
			g_Input = (g_Input & 0xFFFF) | (input << 16);	/* 通常の入力 */
			
			get_djoy(&input, 0, 1);		/* ジョイスティック入力, JoyNo, mode=0:edge on 1:edge off */

			g_Input = (g_Input & 0xFFFF0000) | (input);		/* 通常の入力 */
		}
	}
	else
	{
		if(input != 0u)	/* 何かしら入力あり */
		{
			ret = 1;
		}

		if(	GetPassTime(IPUT_INTERVAL_TIME, &s_PassTime) != 0u)	/* 1s経過 */
		{
			g_Input |= KEY_b_Z;
		}
		else
		{
			g_Input = 0;
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
uint16_t get_keyboard( uint16_t *key, uint8_t bPlayer, uint8_t mode )
{
	uint16_t ret = 0;
	uint32_t uKeyBoard[16] = {0u};
	static int16_t repeat_flag_a = KEY_TRUE;
	static int16_t repeat_flag_b = KEY_TRUE;
	
	uKeyBoard[0]	= BITSNS( 0 );
	uKeyBoard[1]	= BITSNS( 1 );
	uKeyBoard[2]	= BITSNS( 2 );
	uKeyBoard[3]	= BITSNS( 3 );
	uKeyBoard[4]	= BITSNS( 4 );
	uKeyBoard[5]	= BITSNS( 5 );
	uKeyBoard[6]	= BITSNS( 6 );
	uKeyBoard[7]	= BITSNS( 7 );
	uKeyBoard[8]	= BITSNS( 8 );
	uKeyBoard[9]	= BITSNS( 9 );
	uKeyBoard[10]	= BITSNS(10 );
	uKeyBoard[11]	= BITSNS(11 );
	uKeyBoard[12]	= BITSNS(12 );
	uKeyBoard[13]	= BITSNS(13 );
	uKeyBoard[14]	= BITSNS(14 );
	uKeyBoard[15]	= BITSNS(15 );
	/* 共通 */
	if( (uKeyBoard[2]  & Bit_1 ) != 0 ) *key |= KEY_b_Q;	/* Ｑ */
	if( (uKeyBoard[0]  & Bit_1 ) != 0 ) *key |= KEY_b_ESC;	/* ＥＳＣ */
#if 0
	if( (uKeyBoard[6]  & Bit_0 ) != 0 ) *key |= KEY_b_M;	/* Ｍ */
	if( (uKeyBoard[4]  & Bit_2 ) != 0 ) *key |= KEY_b_G;	/* Ｇ */
	if( (uKeyBoard[7]  & Bit_0 ) != 0 ) *key |= KEY_b_RLUP;	/* ロールアップ */
	if( (uKeyBoard[7]  & Bit_1 ) != 0 ) *key |= KEY_b_RLDN;	/* ロールダウン */
#endif
	if( (uKeyBoard[10] & Bit_4 ) != 0 ) *key |= KEY_b_HELP;	/* HELP */
	if( (uKeyBoard[2]  & Bit_0 ) != 0 ) *key |= KEY_b_TAB;	/* TAB */
	if( (uKeyBoard[6]  & Bit_5 ) != 0 ) *key |= KEY_b_SP;	/* スペースキー */
	if( (uKeyBoard[13] & Bit_0 ) != 0 ) *key |= KEY_b_F6;	/* F6 */

	/* P1-P2 */
	if( ( uKeyBoard[7] & Bit_4 ) || ( uKeyBoard[8] & Bit_4 ) ) *key |= KEY_UPPER;	/* 上 ↑ 8 */
	if( ( uKeyBoard[7] & Bit_6 ) || ( uKeyBoard[9] & Bit_4 ) ) *key |= KEY_LOWER;	/* 下 ↓ 2 */
	if( ( uKeyBoard[7] & Bit_3 ) || ( uKeyBoard[8] & Bit_7 ) ) *key |= KEY_LEFT;	/* 左 ← 4 */
	if( ( uKeyBoard[7] & Bit_5 ) || ( uKeyBoard[9] & Bit_1 ) ) *key |= KEY_RIGHT;	/* 右 → 6 */
#if 1
	if( ( uKeyBoard[2] & Bit_2 ) ) *key |= KEY_UPPER_W;	/* w */
	if( ( uKeyBoard[3] & Bit_7 ) ) *key |= KEY_LOWER_S;	/* s */
	if( ( uKeyBoard[3] & Bit_6 ) ) *key |= KEY_LEFT_A;	/* a */
	if( ( uKeyBoard[4] & Bit_0 ) ) *key |= KEY_RIGHT_D;	/* d */
#endif

	if( ( uKeyBoard[10]  & Bit_5 ) || ( uKeyBoard[5]   & Bit_2 ) || (uKeyBoard[3]  & Bit_5 ))	/* Ａボタン or XF1 or z or CR */
	{
		if( repeat_flag_a || (mode != 0u))
		{
			*key |= KEY_b_Z;
			repeat_flag_a = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_a = KEY_TRUE;
	}
	
	if( ( uKeyBoard[10]  & 0x40 ) || ( uKeyBoard[5]   & 0x08 ) )	/* Ｂボタン or XF2 or x  */
	{
		if( repeat_flag_b || (mode != 0u))
		{
			*key |= KEY_b_X;
			repeat_flag_b = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_b = KEY_TRUE;
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
uint16_t get_djoy( uint16_t *key, int32_t nJoyNo, uint8_t mode )
{
	uint16_t ret = 0;
	uint32_t uJoyStick = 0u;
	static int16_t repeat_flag_a = KEY_TRUE;
	static int16_t repeat_flag_b = KEY_TRUE;
#if CNF_JOYDRV360
	uJoyStick = joydrv_djoyget(nJoyNo);
#else
	uJoyStick = _iocs_joyget(nJoyNo);
#endif
	g_AnalogMode = 0xFFFF;	/* アナログモードはOFF */
	
	if( !( uJoyStick & JOY_UP    ) ) *key |= KEY_UPPER;	/* 上 */
	if( !( uJoyStick & JOY_DOWN  ) ) *key |= KEY_LOWER;	/* 下 */
	if( !( uJoyStick & JOY_LEFT  ) ) *key |= KEY_LEFT;	/* 左 */
	if( !( uJoyStick & JOY_RIGHT ) ) *key |= KEY_RIGHT;	/* 右 */
	
	if( !( uJoyStick & JOY_A  ) )	/* Ａボタン */
	{
		if( repeat_flag_a || (mode != 0u))
		{
			*key |= KEY_b_Z;
			repeat_flag_a = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_a = KEY_TRUE;
	}
	
	if( !( uJoyStick & JOY_B  )  )	/* Ｂボタン */
	{
		if( repeat_flag_b || (mode != 0u))
		{
			*key |= KEY_b_X;
			repeat_flag_b = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_b = KEY_TRUE;
	}

	g_Input_P[nJoyNo] = (~uJoyStick) & 0xFF;
//	printf("get_djoy[%d](0x%x)\n",  nJoyNo, g_Input_P[nJoyNo]);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint16_t get_ajoy( uint16_t *key, int32_t nJoyNo, uint8_t mode, uint8_t ubConfig )
{
	uint16_t ret = 0;
	int32_t AnalogJoyStick = 0;
	static int16_t repeat_flag_a = KEY_TRUE;
	static int16_t repeat_flag_b = KEY_TRUE;


	int16_t	analog_buf[5];
#if CNF_JOYDRV360
//	Set_DI();	/* 割り込み禁止 */
	AnalogJoyStick	= joydrv_ajoyget(nJoyNo, &analog_buf[0]);
//	Set_EI();	/* 割り込み禁止解除 */
#else
//	JOY_ANALOG_BUF	analog_buf_st;
//	AnalogJoyStick	= get_analog_data(nJoyNo, &analog_buf_st);
//	analog_buf[l_stk_ud_2] = analog_buf_st.l_stk_ud;
//	analog_buf[l_stk_lr_3] = analog_buf_st.l_stk_lr;
//	analog_buf[r_stk_ud_0] = analog_buf_st.r_stk_ud;
//	analog_buf[r_stk_lr_1] = analog_buf_st.r_stk_lr;
//	analog_buf[btn_data_4] = analog_buf_st.btn_data;
#endif

	if(AnalogJoyStick < 0)
	{
		return -1;
	}
	else
	{
#if 0
		printf("J(%d)=%2x,%2x,%2x,%2x,%b\n", nJoyNo, 
			analog_buf[l_stk_ud_2],
			analog_buf[l_stk_lr_3],
			analog_buf[r_stk_ud_0],
			analog_buf[r_stk_lr_1],
			analog_buf[btn_data_4]);
		//KeyHitESC();	/* デバッグ用 */
#endif
	}
	
	if( (analog_buf[btn_data_4]  & AJOY_SELECT	) == 0 ) 	*key |= KEY_b_Q;	/* Ｑ */
	if( (analog_buf[btn_data_4]  & AJOY_START	) == 0 ) 	*key |= KEY_b_ESC;	/* ＥＳＣ */
	if( (analog_buf[btn_data_4]  & AJOY_E2	 ) == 0 ) 		*key |= KEY_b_M;	/* Ｍ */
	if( (analog_buf[btn_data_4]  & AJOY_D	 ) == 0 ) 		*key |= KEY_b_SP;	/* スペースキー */
	if( (analog_buf[btn_data_4]  & AJOY_E2	 ) == 0 )		*key |= KEY_b_RLUP;	/* ロールアップ */
	if( (analog_buf[btn_data_4]  & AJOY_E1	 ) == 0 ) 		*key |= KEY_b_RLDN;	/* ロールダウン */
	
	if(ubConfig == 0u)
	{
		/* X680x0 */
	}
	else
	{
		int16_t	analog_buf_tmp[5];
		/* Windows -> USB -> XM6 */
		
		memcpy(analog_buf, analog_buf_tmp, sizeof(analog_buf));
		
		/* LスティックとRスティック入れ替え */
		analog_buf[l_stk_ud_2] = analog_buf_tmp[r_stk_ud_0];
		analog_buf[l_stk_lr_3] = analog_buf_tmp[r_stk_lr_1];
		analog_buf[r_stk_ud_0] = analog_buf_tmp[l_stk_ud_2];
		analog_buf[r_stk_lr_1] = analog_buf_tmp[l_stk_lr_3];
	}
	if( analog_buf[l_stk_ud_2] > 0x90 ) *key |= KEY_UPPER;	/* 上 */
	if( analog_buf[l_stk_ud_2] < 0x70 ) *key |= KEY_LOWER;	/* 下 */
	if( analog_buf[r_stk_lr_1] < 0x70 ) *key |= KEY_LEFT;	/* 左 */
	if( analog_buf[r_stk_lr_1] > 0x90 ) *key |= KEY_RIGHT;	/* 右 */

	if( !( analog_buf[btn_data_4] & AJOY_A  ))	/* Ａボタン */
	{
		if( repeat_flag_a || (mode != 0u))
		{
			*key |= KEY_b_Z;
			repeat_flag_a = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_a = KEY_TRUE;
	}
	
	if( !( analog_buf[btn_data_4] & AJOY_B  ))	/* Ｂボタン */
	{
		if( repeat_flag_b || (mode != 0u))
		{
			*key |= KEY_b_X;
			repeat_flag_b = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_b = KEY_TRUE;
	}

	SetAnalog_Info(analog_buf);	/* アナログ情報更新 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetAnalog_Info(int16_t *p_Analog_Info)
{
	int16_t	ret = 0;

	memcpy(p_Analog_Info, &g_Analog_Info[0], sizeof(g_Analog_Info));

	ret = g_AnalogMode;

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetAnalog_Info(int16_t *Analog_Info)
{
	int16_t	ret = 0;

	memcpy(&g_Analog_Info[0], Analog_Info, sizeof(g_Analog_Info));
	
	g_AnalogMode = 0;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetJoyDevice(int32_t nJoyNo, int32_t nDevNo, int32_t nPort)
{
	int16_t	ret = 0;
	int32_t mode = 0;

	if(nJoyNo < 0)
	{
#if CNF_JOYDRV360
		mode = joydrv_getadd(nDevNo, nPort);	/* 関数名(ジョイスティックモード取得(デバイスID指定)) */
#endif
		switch(mode) {
			case 0:
				g_bAnalogStickMode = FALSE;	/* デジタル */
//				puts("joydrv_getadd:デバイスはデジタルです");
				break;
			case 1:
				g_bAnalogStickMode = TRUE;	/* アナログ */
//				puts("joydrv_getadd:デバイスはアナログです");
				break;
			default:
//				puts("joydrv_getadd:エラーです");
				break;
		}
	}
	else
	{
#if CNF_JOYDRV360
		mode = joydrv_getadj(nJoyNo);			/* 関数名(ジョイスティックモード取得(JOY番号指定)) */
#endif
		switch(mode) {
			case 0:
				g_bAnalogStickMode = FALSE;	/* デジタル */
//				puts("joydrv_getadj:デバイスはデジタルです");
				break;
			case 1:
				g_bAnalogStickMode = TRUE;	/* アナログ */
//				puts("joydrv_getadj:デバイスはアナログです");
				break;
			default:
//				puts("joydrv_getadj:エラーです");
				break;
		}
	}
	ret = mode;
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetJoyDevMode(int32_t nJoyNo)
{
	int16_t	ret = 0;
	
#if CNF_JOYDRV360
	int32_t nDevId, nPortNo;

	ret = joydrv_getmode(nJoyNo, &nDevId, &nPortNo);
	if(ret == 0) {
//		printf("joydrv_getmode:JoyNo[%d]DevID[%d]PortNo[%d]\n",nJoyNo, nDevId, nPortNo);
	}
	else
	{
//		printf("joydrv_getmode:エラーが発生しました[%d]\n", ret);
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
int16_t	SetJoyDevMode(int32_t nJoyNo, int32_t nDevId, int32_t nPortNo)
{
	int16_t	ret = 0;

#if CNF_JOYDRV360
	ret = joydrv_setmode(nJoyNo, nDevId, nPortNo);	/* 関数名(JOY番号指定、デバイスID、ポート設定) */

	if(ret == 0) {
//		printf("joydrv_setmode:JoyNo[%d]DevID[%d]PortNo[%d]をSET\n",nJoyNo, nDevId, nPortNo);
	}
	else {
//		printf("joydrv_setmode:エラーが発生しました[%d]\n", ret);
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
uint16_t	DirectInputKeyNum(uint16_t *uVal, uint16_t uDigit)
{
	uint16_t ret = 0;
	static uint16_t uNum[10] = {0};
	static uint16_t uCount = 0;
	static uint8_t bKeyFlag1 = FALSE;
	static uint8_t bKeyFlag2 = FALSE;
	uint32_t i, kd_g0, kd_g1;
	
	if(uDigit > 10)return -1;
	
	kd_g0	= (BITSNS( 0 ) & 0xFCu);
	kd_g1	= (BITSNS( 1 ) & 0x0Fu);

	if(kd_g0 != 0)
	{
		if(bKeyFlag1 == FALSE)
		{
			bKeyFlag1 = TRUE;
			
			for(i = 0; i < 8; i++)
			{
				if( ((kd_g0 >> i) & 0x01) != 0u )
				{
					uNum[uCount] = i - 1;
					break;
				}
			}
			uCount++;
#if 1
			switch(uCount)	/* SE */
			{
			case 1:
				ADPCM_Play(1);
				break;
			case 2:
				ADPCM_Play(2);
				break;
			case 3:
				ADPCM_Play(3);
				break;
			default:
				ADPCM_Play(1);
				break;
			}
#endif
		}
	}
	else
	{
		bKeyFlag1 = FALSE;
	}
	
	if(kd_g1 != 0)
	{
		if(bKeyFlag2 == FALSE)
		{
			bKeyFlag2 = TRUE;

			for(i = 0; i < 8; i++)
			{
				if( ((kd_g1 >> i) & 0x01) != 0u )
				{
					uNum[uCount] = (i + 7) % 10;
					break;
				}
			}
			uCount++;
#if 1
			switch(uCount)	/* SE */
			{
			case 1:
				ADPCM_Play(1);
				break;
			case 2:
				ADPCM_Play(2);
				break;
			case 3:
				ADPCM_Play(3);
				break;
			default:
				ADPCM_Play(1);
				break;
			}
#endif
		}
	}
	else
	{
		bKeyFlag2 = FALSE;
	}

	if( ( (kd_g0 != 0) || (kd_g1 != 0) )
		&& (uCount == uDigit) )
	{
		uint16_t uCal = 0;
		uint16_t uD = 1;
		
		for(i = 0; i < uDigit; i++)
		{
			if(i == 0)
			{
				uCal += uNum[uDigit-1];
			}
			else
			{
				uCal += uNum[(uDigit-1) - i] * uD;
			}
			uD *= 10;
		}
		*uVal = uCal;
		uCount = 0;
		ret = 1;
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
/* チャタリング防止SW */
uint8_t	ChatCancelSW(uint8_t bJudge, uint8_t *bFlag)
{
	uint8_t	ret = FALSE;
	
	if(bJudge != 0u)
	{
		if(*bFlag == TRUE)
		{
			ret = TRUE;
			*bFlag = FALSE;
		}
	}
	else
	{
		*bFlag = TRUE;
	}
	return ret;
}

#if 0
#ifdef DEBUG	/* デバッグコーナー */
		/* テスト用入力 */
		if(bDebugMode == TRUE)
		{
			/* キーボードから数字を入力 */
			DirectInputKeyNum(&g_uDebugNum, 3);
			/* キー操作 */
#if 1
			/* 角度変更 */
			//if(ChatCancelSW((input & KEY_UPPER)!=0u, &bKeyUP_flag) == TRUE)	vy += 1;	/* 上 */
			//if(ChatCancelSW((input & KEY_LOWER)!=0u, &bKeyDOWN_flag) == TRUE)	vy -= 1;	/* 下 */
			if((input & KEY_UPPER)!=0u)	vy += 1;	/* 上 */
			if((input & KEY_LOWER)!=0u)	vy -= 1;	/* 下 */
#else
			/* ランプ操作 */
			if(bUpDown_flag == 0){
				vy += 1;
				if(vy > 45)
				{
					vy = 0;
					bUpDown_flag = 1;
				}
			}
			else{
				vy -= 1;
				if(vy < -45)
				{
					vy = 0;
					bUpDown_flag = 0;
				}
			}
#endif
			vy = Mmax(Mmin(vy, 45), -45);
		}
#endif
#endif

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	KeyHitESC(void)
{
	int16_t	ret = 0;
	static uint8_t bFlag = 0;
	
	{
		uint32_t	loop = 1;
		do
		{
			if( ( BITSNS( 0 ) & 0x02 ) != 0 )	/* ＥＳＣ */
			{
				bFlag = 1u;
			}
			else if(bFlag == 1u)
			{
				_dos_kflushio(0xFF);	/* キーバッファをクリア */
				bFlag = 0u;
				loop = 0;				/* ループ脱出 */
			}
			else
			{
				bFlag = 0u;
			}
			if(loop == 0)break;
		}
		while( loop );
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
uint32_t	SetInput(int32_t input)
{
	return g_Input |= input;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint32_t	GetInput(void)
{
	return g_Input;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint16_t	GetInput_P1(void)
{
	return g_Input_P[0];
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint16_t	GetInput_P2(void)
{
	return g_Input_P[1];
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int8_t GetJoyAnalogMode(void)
{
	return g_bAnalogStickMode;
}
#endif	/* IF_INPUT_C */

