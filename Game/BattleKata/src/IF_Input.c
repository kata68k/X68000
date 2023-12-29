#ifndef	IF_INPUT_C
#define	IF_INPUT_C

#include <stdio.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>

#include <usr_macro.h>
#include "IF_Input.h"
#include "BIOS_MPU.h"

#if CNF_JOYDRV360
#include <JOYLIB3.H>
#else
uint32_t	get_analog_data(uint32_t, JOY_ANALOG_BUF *);
#endif

int16_t		g_Input;
int16_t		g_Input_P[2];
int16_t		g_AnalogMode = 0xFFFF;
uint8_t		g_bAnalogStickMode = FALSE;
uint8_t		g_bAnalogStickMode_flag;
int16_t		g_Analog_Info[5];

/* �֐��̃v���g�^�C�v�錾 */
int16_t		Input_Init(void);
int16_t		Input_Main(void);
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

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Input_Init(void)
{
	int16_t ret = 0;

#if CNF_JOYDRV360
	/* �A�i���O�X�e�B�b�N���[�h���� */
	if(SetJoyDevMode(0, 0, 0) < 0)	/* JoyNo:0 DevID:0 PortNo:0 */
	{
		puts("JOYDRV3.X���풓���Ă܂���B");
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
	g_bAnalogStickMode = FALSE;	/* �f�W�^�� */
#endif
	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Input_Main(void)
{
	int16_t ret = 0;
	int16_t	input = 0;

	get_keyboard(&input, 0, 1);		/* �L�[�{�[�h����, JoyNo, mode=0:edge on 1:edge off */

	/* �A�i���O�X�e�B�b�N�^�f�W�^���X�e�B�b�N�ؑ� */
	if(ChatCancelSW((g_Input & KEY_b_TAB)!=0u, &g_bAnalogStickMode_flag) == TRUE)	/* TAB�ŃA�i���O�X�e�B�b�NON/OFF */
	{
#if CNF_JOYDRV360				
		if(g_bAnalogStickMode == TRUE)	/* ���݁F�A�i���O�X�e�B�b�N���[�h�̏ꍇ */
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

	if(g_bAnalogStickMode == TRUE)
	{
#if CNF_JOYDRV360				
		get_ajoy(&input, 1, 0, 0);	/* �A�i���O�W���C�X�e�B�b�N����, JoyNo, mode=0:edge on 1:edge off, Config 0:X680x0 1:rev */
#else
		get_djoy(&input, 0, 1);		/* �W���C�X�e�B�b�N����, JoyNo, mode=0:edge on 1:edge off */
#endif
	}
	else
	{
		get_djoy(&input, 1, 1);		/* �W���C�X�e�B�b�N����, JoyNo, mode=0:edge on 1:edge off */
		
		get_djoy(&input, 0, 1);		/* �W���C�X�e�B�b�N����, JoyNo, mode=0:edge on 1:edge off */
	
	}
	g_Input = input;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint16_t get_keyboard( uint16_t *key, uint8_t bPlayer, uint8_t mode )
{
	uint16_t ret = 0;
	uint32_t uKeyBoard[16] = {0u};
	static int16_t repeat_flag_a = KEY_TRUE;
	static int16_t repeat_flag_b = KEY_TRUE;
	
	uKeyBoard[0]	= BITSNS( 0 );
	uKeyBoard[2]	= BITSNS( 2 );
	uKeyBoard[3]	= BITSNS( 3 );
	uKeyBoard[4]	= BITSNS( 4 );
	uKeyBoard[5]	= BITSNS( 5 );
	uKeyBoard[6]	= BITSNS( 6 );
	uKeyBoard[7]	= BITSNS( 7 );
	uKeyBoard[8]	= BITSNS( 8 );
	uKeyBoard[9]	= BITSNS( 9 );
	uKeyBoard[10]	= BITSNS(10 );
	
	if( (uKeyBoard[2]  & Bit_1 ) != 0 ) *key |= KEY_b_Q;	/* �p */
	if( (uKeyBoard[0]  & Bit_1 ) != 0 ) *key |= KEY_b_ESC;	/* �d�r�b */
	if( (uKeyBoard[6]  & Bit_0 ) != 0 ) *key |= KEY_b_M;	/* �l */
	if( (uKeyBoard[6]  & Bit_5 ) != 0 ) *key |= KEY_b_SP;	/* �X�y�[�X�L�[ */
	if( (uKeyBoard[7]  & Bit_0 ) != 0 ) *key |= KEY_b_RLUP;	/* ���[���A�b�v */
	if( (uKeyBoard[7]  & Bit_1 ) != 0 ) *key |= KEY_b_RLDN;	/* ���[���_�E�� */
	if( (uKeyBoard[10] & Bit_4 ) != 0 ) *key |= KEY_b_HELP;	/* HELP */
	if( (uKeyBoard[2]  & Bit_0 ) != 0 ) *key |= KEY_b_TAB;	/* TAB */
	if( (uKeyBoard[4]  & Bit_2 ) != 0 ) *key |= KEY_b_G;	/* �f */
	
	if( ( uKeyBoard[7] & Bit_4 ) || ( uKeyBoard[8] & Bit_4 ) || ( uKeyBoard[2] & Bit_2 ) ) *key |= KEY_UPPER;	/* �� �� 8 w */
	if( ( uKeyBoard[7] & Bit_6 ) || ( uKeyBoard[9] & Bit_4 ) || ( uKeyBoard[3] & Bit_7 ) ) *key |= KEY_LOWER;	/* �� �� 2 s */
	if( ( uKeyBoard[7] & Bit_3 ) || ( uKeyBoard[8] & Bit_7 ) || ( uKeyBoard[3] & Bit_6 ) ) *key |= KEY_LEFT;	/* �� �� 4 a */
	if( ( uKeyBoard[7] & Bit_5 ) || ( uKeyBoard[9] & Bit_1 ) || ( uKeyBoard[4] & Bit_0 ) ) *key |= KEY_RIGHT;	/* �E �� 6 d */
	
	if( ( uKeyBoard[10]  & Bit_5 ) || ( uKeyBoard[5]   & Bit_2 ) )	/* �`�{�^�� or XF1 or z */
	{
		if( repeat_flag_a || (mode != 0u))
		{
			*key |= KEY_A;
			repeat_flag_a = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_a = KEY_TRUE;
	}
	
	if( ( uKeyBoard[10]  & 0x40 ) || ( uKeyBoard[5]   & 0x08 ) )	/* �a�{�^�� or XF2 or x  */
	{
		if( repeat_flag_b || (mode != 0u))
		{
			*key |= KEY_B;
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
	g_AnalogMode = 0xFFFF;	/* �A�i���O���[�h��OFF */
	
	if( !( uJoyStick & JOY_UP    ) ) *key |= KEY_UPPER;	/* �� */
	if( !( uJoyStick & JOY_DOWN  ) ) *key |= KEY_LOWER;	/* �� */
	if( !( uJoyStick & JOY_LEFT  ) ) *key |= KEY_LEFT;	/* �� */
	if( !( uJoyStick & JOY_RIGHT ) ) *key |= KEY_RIGHT;	/* �E */
	
	if( !( uJoyStick & JOY_A  ) )	/* �`�{�^�� */
	{
		if( repeat_flag_a || (mode != 0u))
		{
			*key |= KEY_A;
			repeat_flag_a = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_a = KEY_TRUE;
	}
	
	if( !( uJoyStick & JOY_B  )  )	/* �a�{�^�� */
	{
		if( repeat_flag_b || (mode != 0u))
		{
			*key |= KEY_B;
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint16_t get_ajoy( uint16_t *key, int32_t nJoyNo, uint8_t mode, uint8_t ubConfig )
{
	uint16_t ret = 0;
	int32_t AnalogJoyStick = 0;
	static int16_t repeat_flag_a = KEY_TRUE;
	static int16_t repeat_flag_b = KEY_TRUE;


	int16_t	analog_buf[5];
#if CNF_JOYDRV360
//	Set_DI();	/* ���荞�݋֎~ */
	AnalogJoyStick	= joydrv_ajoyget(nJoyNo, &analog_buf[0]);
//	Set_EI();	/* ���荞�݋֎~���� */
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
		//KeyHitESC();	/* �f�o�b�O�p */
#endif
	}
	
	if( (analog_buf[btn_data_4]  & AJOY_SELECT	) == 0 ) 	*key |= KEY_b_Q;	/* �p */
	if( (analog_buf[btn_data_4]  & AJOY_START	) == 0 ) 	*key |= KEY_b_ESC;	/* �d�r�b */
	if( (analog_buf[btn_data_4]  & AJOY_E2	 ) == 0 ) 		*key |= KEY_b_M;	/* �l */
	if( (analog_buf[btn_data_4]  & AJOY_D	 ) == 0 ) 		*key |= KEY_b_SP;	/* �X�y�[�X�L�[ */
	if( (analog_buf[btn_data_4]  & AJOY_E2	 ) == 0 )		*key |= KEY_b_RLUP;	/* ���[���A�b�v */
	if( (analog_buf[btn_data_4]  & AJOY_E1	 ) == 0 ) 		*key |= KEY_b_RLDN;	/* ���[���_�E�� */
	
	if(ubConfig == 0u)
	{
		/* X680x0 */
	}
	else
	{
		int16_t	analog_buf_tmp[5];
		/* Windows -> USB -> XM6 */
		
		memcpy(analog_buf, analog_buf_tmp, sizeof(analog_buf));
		
		/* L�X�e�B�b�N��R�X�e�B�b�N����ւ� */
		analog_buf[l_stk_ud_2] = analog_buf_tmp[r_stk_ud_0];
		analog_buf[l_stk_lr_3] = analog_buf_tmp[r_stk_lr_1];
		analog_buf[r_stk_ud_0] = analog_buf_tmp[l_stk_ud_2];
		analog_buf[r_stk_lr_1] = analog_buf_tmp[l_stk_lr_3];
	}
	if( analog_buf[l_stk_ud_2] > 0x90 ) *key |= KEY_UPPER;	/* �� */
	if( analog_buf[l_stk_ud_2] < 0x70 ) *key |= KEY_LOWER;	/* �� */
	if( analog_buf[r_stk_lr_1] < 0x70 ) *key |= KEY_LEFT;	/* �� */
	if( analog_buf[r_stk_lr_1] > 0x90 ) *key |= KEY_RIGHT;	/* �E */

	if( !( analog_buf[btn_data_4] & AJOY_A  ))	/* �`�{�^�� */
	{
		if( repeat_flag_a || (mode != 0u))
		{
			*key |= KEY_A;
			repeat_flag_a = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_a = KEY_TRUE;
	}
	
	if( !( analog_buf[btn_data_4] & AJOY_B  ))	/* �a�{�^�� */
	{
		if( repeat_flag_b || (mode != 0u))
		{
			*key |= KEY_B;
			repeat_flag_b = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_b = KEY_TRUE;
	}

	SetAnalog_Info(analog_buf);	/* �A�i���O���X�V */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	GetAnalog_Info(int16_t *p_Analog_Info)
{
	int16_t	ret = 0;

	memcpy(p_Analog_Info, &g_Analog_Info[0], sizeof(g_Analog_Info));

	ret = g_AnalogMode;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	SetAnalog_Info(int16_t *Analog_Info)
{
	int16_t	ret = 0;

	memcpy(&g_Analog_Info[0], Analog_Info, sizeof(g_Analog_Info));
	
	g_AnalogMode = 0;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	GetJoyDevice(int32_t nJoyNo, int32_t nDevNo, int32_t nPort)
{
	int16_t	ret = 0;
	int32_t mode = 0;

	if(nJoyNo < 0)
	{
#if CNF_JOYDRV360
		mode = joydrv_getadd(nDevNo, nPort);	/* �֐���(�W���C�X�e�B�b�N���[�h�擾(�f�o�C�XID�w��)) */
#endif
		switch(mode) {
			case 0:
				g_bAnalogStickMode = FALSE;	/* �f�W�^�� */
//				puts("joydrv_getadd:�f�o�C�X�̓f�W�^���ł�");
				break;
			case 1:
				g_bAnalogStickMode = TRUE;	/* �A�i���O */
//				puts("joydrv_getadd:�f�o�C�X�̓A�i���O�ł�");
				break;
			default:
//				puts("joydrv_getadd:�G���[�ł�");
				break;
		}
	}
	else
	{
#if CNF_JOYDRV360
		mode = joydrv_getadj(nJoyNo);			/* �֐���(�W���C�X�e�B�b�N���[�h�擾(JOY�ԍ��w��)) */
#endif
		switch(mode) {
			case 0:
				g_bAnalogStickMode = FALSE;	/* �f�W�^�� */
//				puts("joydrv_getadj:�f�o�C�X�̓f�W�^���ł�");
				break;
			case 1:
				g_bAnalogStickMode = TRUE;	/* �A�i���O */
//				puts("joydrv_getadj:�f�o�C�X�̓A�i���O�ł�");
				break;
			default:
//				puts("joydrv_getadj:�G���[�ł�");
				break;
		}
	}
	ret = mode;
	
	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
//		printf("joydrv_getmode:�G���[���������܂���[%d]\n", ret);
	}
#endif

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	SetJoyDevMode(int32_t nJoyNo, int32_t nDevId, int32_t nPortNo)
{
	int16_t	ret = 0;

#if CNF_JOYDRV360
	ret = joydrv_setmode(nJoyNo, nDevId, nPortNo);	/* �֐���(JOY�ԍ��w��A�f�o�C�XID�A�|�[�g�ݒ�) */

	if(ret == 0) {
//		printf("joydrv_setmode:JoyNo[%d]DevID[%d]PortNo[%d]��SET\n",nJoyNo, nDevId, nPortNo);
	}
	else {
//		printf("joydrv_setmode:�G���[���������܂���[%d]\n", ret);
	}
#endif

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
#if 0
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
#if 0
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* �`���^�����O�h�~SW */
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
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		/* �e�X�g�p���� */
		if(bDebugMode == TRUE)
		{
			/* �L�[�{�[�h���琔������� */
			DirectInputKeyNum(&g_uDebugNum, 3);
			/* �L�[���� */
#if 1
			/* �p�x�ύX */
			//if(ChatCancelSW((input & KEY_UPPER)!=0u, &bKeyUP_flag) == TRUE)	vy += 1;	/* �� */
			//if(ChatCancelSW((input & KEY_LOWER)!=0u, &bKeyDOWN_flag) == TRUE)	vy -= 1;	/* �� */
			if((input & KEY_UPPER)!=0u)	vy += 1;	/* �� */
			if((input & KEY_LOWER)!=0u)	vy -= 1;	/* �� */
#else
			/* �����v���� */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	KeyHitESC(void)
{
	int16_t	ret = 0;
	static uint8_t bFlag = 0;
	
	{
		uint32_t	loop = 1;
		do
		{
			if( ( BITSNS( 0 ) & 0x02 ) != 0 )	/* �d�r�b */
			{
				bFlag = 1u;
			}
			else if(bFlag == 1u)
			{
				_dos_kflushio(0xFF);	/* �L�[�o�b�t�@���N���A */
				bFlag = 0u;
				loop = 0;				/* ���[�v�E�o */
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
	
#endif	/* IF_INPUT_C */

