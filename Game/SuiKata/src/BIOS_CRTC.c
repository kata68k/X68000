#ifndef	BIOS_CRTC_C
#define	BIOS_CRTC_C

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <iocslib.h>

#include <usr_macro.h>
#include "BIOS_PCG.h"
#include "BIOS_CRTC.h"
#include "BIOS_MFP.h"
#include "BIOS_MPU.h"
#include "IF_Input.h"

/* �O���[�o���ϐ� */

FLOAT dot_clock8[3][4] = {
	1.64678, 0.82339, 1.64678, 1.64678, 
	0.69013, 0.34507, 0.23004, 0.31778, 
	0.92017, 0.46009, 0.23004, 0.31778 
};

FLOAT freq_data[5][10] = {
	22.0842, 31.7460, 3.4507, 4.1408, 2.0704, 16.2540, 18.0317, 0.1905, 1.1111, 0.4762,
	25.4220, 31.7776, 3.8133, 1.9067, 0.6356, 15.2532, 17.2552, 0.0636, 1.0169, 0.9215,
	25.4220, 31.7776, 3.8133, 1.9464, 0.5958, 15.2532, 16.6832, 0.0636, 1.0169, 0.3496,
	29.4456, 40.4877, 3.6807, 4.6009, 2.7605, 17.1668, 18.8268, 0.3239, 1.0122, 0.3239,
	52.6971, 62.5778, 3.2936, 4.9404, 1.6468, 15.0187, 16.2702, 0.1877, 0.8761, 0.1877
};

uint32_t	hl, clk, freq, scanmode, colmode, size;
uint32_t	yesno;	
uint32_t	ir[8],ir20;
uint32_t	hres, vres;
FLOAT	fr[8];
uint8_t	keybuf[128];
int32_t	stack;
int8_t	g_CRT_Contrast = -1;

volatile uint16_t	*crtc = (uint16_t *)0xE80000u;
volatile uint16_t	*vcon = (uint16_t *)0xE82400u;
volatile uint16_t	*scon = (uint16_t *)0xEB080Au;
volatile uint8_t	*sysp =  (uint8_t *)0xE8E007u;

volatile uint16_t	*CRTC_R00 = (uint16_t *)0xE80000u;
volatile uint16_t	*CRTC_R01 = (uint16_t *)0xE80002u;
volatile uint16_t	*CRTC_R02 = (uint16_t *)0xE80004u;
volatile uint16_t	*CRTC_R03 = (uint16_t *)0xE80006u;
volatile uint16_t	*CRTC_R04 = (uint16_t *)0xE80008u;
volatile uint16_t	*CRTC_R05 = (uint16_t *)0xE8000Au;
volatile uint16_t	*CRTC_R06 = (uint16_t *)0xE8000Cu;
volatile uint16_t	*CRTC_R07 = (uint16_t *)0xE8000Eu;
volatile uint16_t	*CRTC_R08 = (uint16_t *)0xE80010u;
volatile uint16_t	*CRTC_R09 = (uint16_t *)0xE80012u;
volatile uint16_t	*CRTC_R10 = (uint16_t *)0xE80014u;
volatile uint16_t	*CRTC_R11 = (uint16_t *)0xE80016u;
volatile uint16_t	*CRTC_R12 = (uint16_t *)0xE80018u;
volatile uint16_t	*CRTC_R13 = (uint16_t *)0xE8001Au;
volatile uint16_t	*CRTC_R14 = (uint16_t *)0xE8001Cu;
volatile uint16_t	*CRTC_R15 = (uint16_t *)0xE8001Eu;
volatile uint16_t	*CRTC_R16 = (uint16_t *)0xE80020u;
volatile uint16_t	*CRTC_R17 = (uint16_t *)0xE80022u;
volatile uint16_t	*CRTC_R18 = (uint16_t *)0xE80024u;
volatile uint16_t	*CRTC_R19 = (uint16_t *)0xE80026u;
volatile uint16_t	*CRTC_R20 = (uint16_t *)0xE80028u;
volatile uint16_t	*CRTC_R21 = (uint16_t *)0xE8002Au;
volatile uint16_t	*CRTC_R22 = (uint16_t *)0xE8002Cu;
volatile uint16_t	*CRTC_R23 = (uint16_t *)0xE8002Eu;

volatile uint16_t	*SP_CRTC_0A = (uint16_t *)0xEB080Au;
volatile uint16_t	*SP_CRTC_0C = (uint16_t *)0xEB080Cu;
volatile uint16_t	*SP_CRTC_0E = (uint16_t *)0xEB080Eu;
volatile uint16_t	*SP_CRTC_10 = (uint16_t *)0xEB0810u;

volatile uint16_t	g_uCRT_Tmg;
volatile uint32_t	g_uCRT_LCG;

/* �\���̒�` */
ST_CRT		g_stCRT[CRT_MAX] = {0};


/* �֐��̃v���g�^�C�v�錾 */
int16_t CRTC_INIT(uint16_t);
int16_t CRTC_EXIT(uint16_t);
void CRTC_INIT_Manual(void);
void CRTC_INIT_SemiManual(void);
void get_value(int8_t *, uint32_t *, uint32_t, uint32_t);
void cal_regs(void);
void dsp_regs(void);
void set_regs(void);
int16_t	GetCRT(ST_CRT *, int16_t);
int16_t	SetCRT(ST_CRT, int16_t);
int16_t	CRT_INIT_MODE(void);
int16_t	Get_CRT_Contrast(int8_t *);
int16_t	Set_CRT_Contrast(int8_t);
int16_t	Get_CRT_Tmg(uint16_t *);
int16_t	Set_CRT_Tmg(uint16_t);
int16_t wait_vdisp(int16_t);
int16_t wait_h_sync(void);
int16_t wait_v_sync(void);
uint16_t CRTC_Rastar_FirstPos(void);
uint16_t CRTC_Rastar_EndPos(void);
int16_t CRTC_Rastar_SetPos(uint16_t);
int16_t CRTC_T_Scroll(int16_t, int16_t);
int16_t CRTC_G0_Scroll_4(int16_t, int16_t);
int16_t CRTC_G1_Scroll_4(int16_t, int16_t);
int16_t CRTC_G2_Scroll_4(int16_t, int16_t);
int16_t CRTC_G3_Scroll_4(int16_t, int16_t);
int16_t CRTC_G0_Scroll_8(int16_t, int16_t);
int16_t CRTC_G1_Scroll_8(int16_t, int16_t);
int16_t CRTC_G0_Scroll_16(int16_t, int16_t);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_INIT(uint16_t uNum)
{
	int16_t ret = 0;
	int32_t romver;
	uint32_t mode;
	uint32_t mode_ofst;
#if 1
	switch(uNum % 2)
	{
		case 0:	/* �����F31kHz */
		{
			Set_CRT_Tmg(0);
			break;
		}
		case 1:	/* ��F15kHz */
		default:
		{
			Set_CRT_Tmg(1);
			break;
		}
	}
#endif
	/* LCD���[�h���� */
	romver = Get_ROM_Ver();
	if(romver == 0x16)
	{
		mode = (_iocs_crtmod(0x16FF) >> 24) & 0xFF;		/* ���ݒ� */
		if((g_uCRT_LCG == 0x00) && (g_uCRT_LCG != mode))
		{
			g_uCRT_LCG = mode;
		}

		switch(mode)
		{
			case 0x16:
			{
//				printf("CRT()=0x%x\n", mode);
				break;
			}
			case 0x96:
			{
//				printf("LCD()=0x%x\n", mode);
//				mode = _iocs_crtmod(0x56FF);		/* ���ݒ� */
				break;
			}
			default:
			{
//				printf("CRT/LCD?()=0x%x\n", mode);
				break;
			}
		}
		mode_ofst = 0x100;
//		printf("CRTMOD()=0x%x\n", mode);
	}
	else
	{
		mode_ofst = 0x100;
	}
//	puts("CRTC_INIT tmg");
//	KeyHitESC();	/* �f�o�b�O�p */
	ret = _iocs_crtmod(-1);		/* ���ݒ� */
//	puts("CRTC_INIT crtmod old");
//	KeyHitESC();	/* �f�o�b�O�p */

	if(uNum <= 18u)
	{
		_iocs_crtmod(uNum);	/* uNum = �����F31kHz�A��F15kHz(17,18:24kHz) */
//	puts("CRTC_INIT num");
//	KeyHitESC();	/* �f�o�b�O�p */
	}
	else{
		if(uNum >= 0x200u)
		{
			CRTC_INIT_SemiManual();		/* 384 x 256 31kHz 256color */
		}
		else
		{
			_iocs_crtmod(mode_ofst + (0xFF & uNum));	/* �������t�� */
		}
//		KeyHitESC();	/* �f�o�b�O�p */
//		puts("CRTC_INIT manual");
	}

	CRT_INIT_MODE();	/* ��ʈʒu�ݒ� */
//	puts("CRTC_INIT Init");
//	KeyHitESC();	/* �f�o�b�O�p */

	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_EXIT(uint16_t uNum)
{
	int16_t ret = 0;

	CRTC_INIT(uNum);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void CRTC_INIT_Manual(void)
{
	get_value("�h�b�g�N���b�N 1:Low 2:High 3:Middle =", &hl,	1,	3);
	
	get_value("�h�b�g�N���b�N 1:256 2:512 3:768 4:special =", &clk,	1,	4);
	
	if( (hl == 0) && (clk == 2) )
	{
		printf("Low���[�h�ł�256�Ɠ���\n");
		clk = 0;
	}
	
	get_value("�������g�� 1:31kHz 2:31kHz(V) 3:31kHz(VGA) 4:24kHz 5:15kHz =", &freq,	1,	5);
	
	do
	{
		get_value("�������[�h 1:�m���E�C���^�[���[�X 2:�C���^�[���[�X 3:��x�ǂ� =", &scanmode,	1,	3);
	}
	while( (hl == 0) && (scanmode == 2) );
	
	get_value("����ʃT�C�Y 1:512x512�h�b�g 2:1024x1024�h�b�g =", &size,	1,	2);
	
	get_value("�F�� 1:16�F 2:256�F 3:65535�F =", &colmode,	1,	3);
	
	cal_regs();
	
	dsp_regs();
	
	get_value("�ݒ肵�܂����H 1:No 2:Yes =", &yesno,	1,	2);
	
	if(yesno != 0)
	{
		set_regs();
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void CRTC_INIT_SemiManual(void)
{
	hl = 3-1;	/* Middle */
	clk = 2-1;	/* 512 */
	
	if( (hl == 0) && (clk == 2) )
	{
		printf("Low���[�h�ł�256�Ɠ���\n");
		clk = 0;
	}
	
	freq = 1-1;	/* 31kHz */
	
	do
	{
		scanmode = 3-1;	/* ��x�ǂ� */
	}
	while( (hl == 0) && (scanmode == 2) );
	
	size = 1-1;	/* 512 x 512 */
	
	colmode = 2-1;	/* 256�F */
	
	cal_regs();
#if 0	
	dsp_regs();
#endif	
	yesno = 2-1;
	
	if(yesno != 0)
	{
		set_regs();
	}
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void get_value(int8_t *s, uint32_t *i, uint32_t l, uint32_t h)
{
	do
	{
		printf("%s", s);
		if( !scanf("%d", i) )
		{
			printf("\n");
			scanf("%s", keybuf);
		}
	}
	while( (*i < l) || (*i > h ) );
	(*i)--;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void cal_regs(void)
{
	uint32_t	i, hilo;
	FLOAT	dot_clk, hsync;
	
	dot_clk = dot_clock8[hl][clk];
	
	hsync = freq_data[freq][1];
	
	fr[0] = ( freq_data[freq][1] / dot_clk) - 1.0;
	fr[1] = ( freq_data[freq][2] / dot_clk) - 1.0;
	fr[2] = ((freq_data[freq][2] + freq_data[freq][3]) / dot_clk) - 5.0;
	fr[3] = ((freq_data[freq][1] - freq_data[freq][4]) / dot_clk) - 5.0;
	fr[4] = ( freq_data[freq][6] * 1000.0 / hsync) - 1.0;
	fr[5] = ( freq_data[freq][7] * 1000.0 / hsync) - 1.0;
	fr[6] = ((freq_data[freq][7] + freq_data[freq][8]) * 1000.0 / hsync) - 1.0;
	fr[7] = ((freq_data[freq][6] - freq_data[freq][9]) * 1000.0 / hsync) - 1.0;
	
	for(i=0; i<8; i++)
	{
		if( fr[i] < 0.0 )
		{
			printf("ERROR:R%d (%f)\n", i, fr[i]);
			continue;
		}
		ir[i] = (uint32_t)(fr[i] + 0.5);
	}
	
//ir[1] = 0x06;
//ir[2] = 0x0B;
	
	ir[0] |= 1;
	
	if(colmode == 2)
	{
		colmode = 3;
	}
	
	if(hl >= 1)
	{
		hilo = 1;
	}
	else
	{
		hilo = 0;
	}
	
	ir20 = size * 0x400 + colmode * 0x100 + hilo * 0x10 + clk;
	
	if( ((hl == 0) && (scanmode == 1)) || ((hl >= 1) && (scanmode == 0)) )
	{
		ir20 += 4;
	}
	if((hl >= 1) && (scanmode == 1))
	{
		ir20 += 8;
	}
	
	hres = (ir[3] - ir[2]) * 8;
	vres = ir[7] - ir[6];
	
	if(scanmode == 1)
	{
		vres *= 2;
	}
	if(scanmode == 2)
	{
		vres /= 2;
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void dsp_regs(void)
{
#ifdef DEBUG
	printf("R0   = %5d ($%04X)\n", ir[0], ir[0]);
	printf("R1   = %5d ($%04X)\n", ir[1], ir[1]);
	printf("R2   = %5d ($%04X)\n", ir[2], ir[2]);
	printf("R3   = %5d ($%04X)\n", ir[3], ir[3]);
	printf("R4   = %5d ($%04X)\n", ir[4], ir[4]);
	printf("R5   = %5d ($%04X)\n", ir[5], ir[5]);
	printf("R6   = %5d ($%04X)\n", ir[6], ir[6]);
	printf("R7   = %5d ($%04X)\n", ir[7], ir[7]);
	printf("R20  = %5d ($%04X)\n", ir20, ir20);
	printf("[%4d]x[%4d]\n", hres, vres);
//	KeyHitESC();	/* �f�o�b�O�p */
#endif
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void set_regs(void)
{
	uint32_t	i;
	uint16_t	r20;
	
//	stack = SUPER(0);

	r20 = *(crtc + 20) & 0x13;
	
	if(ir20 >= r20)
	{
		for(i=0; i<8; i++)
		{
			*(crtc + i) = ir[i];
		}
		*(crtc + 20) = ir20;
	}
	else
	{
		*(crtc + 20) = ir20;
		for(i=1; i<8; i++)
		{
			*(crtc + i) = ir[i];
		}
		*crtc = ir[0];
	}
	
	/* �V�X�e���|�[�g#4 */
	if(hl == 2)
	{
		*sysp |= 0x2;	/* �h�b�g�N���b�N HRL �r�b�g */
	}
	else
	{
		*sysp &= ~0x2;	/* �h�b�g�N���b�N HRL �r�b�g */
	}
	
	/* �r�f�I�R���g���[���[ */
	*vcon = (ir20 >> 8) & 0x07;	/* CRTC_R20�Ɠ��� */
	
	/* �X�v���C�g�E��ʃ��[�h�E���W�X�^ �����\���J�n�ʒu */
	*(scon + 1) = ir[2] + 4;	/* CRTC_R02��+4�������� */

	for(i=0; i<0x200; i++)
	{
		/* �������Ȃ� */
	}
	
	/* �X�v���C�g�E��ʃ��[�h�E���W�X�^ �����g�[�^�� */
	if( (ir20 & 0x1F) == 0 )
	{
		*scon = ir[0];	/* CRTC_R00�Ɠ��� */
	}
	else
	{
		*scon = 0xFF;
	}
	
	/* �X�v���C�g�E��ʃ��[�h�E���W�X�^ �����\���J�n�ʒu */
	*(scon + 2) = ir[6];		/* CRTC_R06�Ɠ��� */

	/* �X�v���C�g�E��ʃ��[�h�E���W�X�^ �𑜓x */
	*(scon + 3) = ir20 & 0xFF;	/* CRTC_R20�Ɠ���(�������H�v�����8x8�̂a�f���\���\) */
	
//	SUPER(stack);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	GetCRT(ST_CRT *stDat, int16_t Num)
{
	int16_t	ret = 0;
	
	if(Num < CRT_MAX)
	{
		*stDat = g_stCRT[Num];
	}
	else
	{
		ret = -1;
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
int16_t	SetCRT(ST_CRT stDat, int16_t Num)
{
	int16_t	ret = 0;
	
	if(Num < CRT_MAX)
	{
		g_stCRT[Num] = stDat;
	}
	else
	{
		ret = -1;
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
int16_t CRT_INIT_MODE(void)
{
	int16_t	ret = 0;

	ST_CRT stDat;
#ifdef DEBUG
	/* �f�o�b�O�E�B���h�E���� */
#else	/* �f�o�b�O�E�B���h�E�Ȃ� */
	uint16_t	uCRT_Tmg;
	Get_CRT_Tmg(&uCRT_Tmg);
	if(uCRT_Tmg == 0)	/* ���X�^�Q�x�ǂ݂̏ꍇ(31kHz) */
	{
		*CRTC_R07 = Mmul2(V_SYNC_MAX) + 8;	/* �c�̕\���͈͂����߂�(��ʉ��̃S�~�h�~) */
	}
	else				/* �m���C���^�[���[�X���[�h�̏ꍇ(15kHz) */
	{
		*CRTC_R07 = V_SYNC_MAX;			/* �c�̕\���͈͂����߂�(��ʉ��̃S�~�h�~) */
	}
#endif
//										   FEDCBA9876543210
//	*CRTC_R21 = Mbset(*CRTC_R21, 0x03FF, 0b1111111111111111);	/* CRTC R21 */
//	*CRTC_R21 = Mbset(*CRTC_R21, 0x03FF, 0b0000000000000000);	/* CRTC R21 */
//										   |||||||||||||||+-bit0 CP0	
//										   ||||||||||||||+--bit1 CP1	
//										   |||||||||||||+---bit2 CP2	
//										   ||||||||||||+----bit3 CP3	
//										   |||||||||||+-----bit4 AP0	�����A�N�Z�X
//										   ||||||||||+------bit5 AP1	�����A�N�Z�X
//										   |||||||||+-------bit6 AP2	�����A�N�Z�X
//										   ||||||||+--------bit7 AP3	�����A�N�Z�X
//										   |||||||+---------bit8 SA		�����A�N�Z�X
//										   ||||||+----------bit9 MEN	�����A�N�Z�X�}�X�N
	/* CRT�̐ݒ� */
	/* ��ʁi�I�t�Z�b�g�Ȃ��j */
	stDat.view_offset_x	= X_MIN_DRAW;
	stDat.view_offset_y	= Y_MIN_DRAW;
	stDat.hide_offset_x	= X_MIN_DRAW;
	stDat.hide_offset_y	= Y_MIN_DRAW + Y_OFFSET;
	stDat.BG_offset_x	= SP_X_OFFSET;
	stDat.BG_offset_y	= Y_HORIZON_0 + SP_Y_OFFSET;
	SetCRT(stDat , 0);
	
	/* ��ʁi�I�t�Z�b�g���� �\���F�㑤�j */
	stDat.view_offset_x	= X_MIN_DRAW + X_OFFSET;
	stDat.view_offset_y	= Y_MIN_DRAW;
	stDat.hide_offset_x	= X_MIN_DRAW + X_OFFSET;
	stDat.hide_offset_y	= Y_MIN_DRAW + Y_OFFSET;
	stDat.BG_offset_x	= 0;
	stDat.BG_offset_y	= Y_HORIZON_1;
	SetCRT(stDat , 1);
	
	/* ��ʁi�I�t�Z�b�g���� �\���F�����j */
	stDat.view_offset_x	= X_MIN_DRAW + X_OFFSET;
	stDat.view_offset_y	= Y_MIN_DRAW + Y_OFFSET;
	stDat.hide_offset_x	= X_MIN_DRAW + X_OFFSET;
	stDat.hide_offset_y	= Y_MIN_DRAW;
	stDat.BG_offset_x	= 0;
	stDat.BG_offset_y	= Y_HORIZON_1;
	SetCRT(stDat , 2);

	Set_CRT_Contrast(-1);	/* �R���g���X�g������ */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Get_CRT_Contrast(int8_t *pbContrast)
{
	int16_t	ret = 0;
	
	*pbContrast = _iocs_contrast(-1);
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Set_CRT_Contrast(int8_t bContrast)
{
	int16_t ret = 0;
	
	if(g_CRT_Contrast < 0)
	{
		Get_CRT_Contrast(&g_CRT_Contrast);
	}

	if( (bContrast >= 0) && (bContrast <= 15) )
	{
		_iocs_contrast(bContrast);		/* �ݒ�l */
	}
	else
	{
		_iocs_contrast(g_CRT_Contrast);	/* �����l */
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
int16_t	Get_CRT_Tmg(uint16_t *p_uNum)
{
	int16_t ret = 0;
	
	*p_uNum = g_uCRT_Tmg;
	
	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	Set_CRT_Tmg(uint16_t uNum)
{
	int16_t ret = 0;
	
	g_uCRT_Tmg = uNum;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t wait_vdisp(int16_t count)	/* ��count�^60�b�҂�	*/
{
	int16_t ret = 0;
	volatile uint8_t *pGPIP = (uint8_t *)0xe88001;
	
	/* GPIP4 VDISP @kamadox ���� */
	/* 0 �͐����u�����L���O���ԁB���Ȃ킿�A�����t�����g�|�[�` + ���������p���X + �����o�b�N�|�[�`�B*/
	/* 1 �͐����f�����ԁB*/
	/* ���[�J�[�R���̎����͐����A�����ԁ^�����\�����ԂƂ����������ɂȂ��Ă���A*/
	/* ���ꂪ���������M���ł��邩�̂悤�Ȍ���������Ă���B*/
	while(count--)
	{
		while(((*pGPIP) & GP_VDISP) == 0u);	/* �����u�����L���O���ԑ҂�	*/
		while(((*pGPIP) & GP_VDISP) != 0u);	/* �����f�����ԑ҂�	*/
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
int16_t wait_h_sync(void)
{
	int16_t ret = 0;
	volatile uint8_t *pGPIP = (uint8_t *)0xe88001;

	while((*pGPIP & GP_H_SYNC) == 0x00);
	while((*pGPIP & GP_H_SYNC) == GP_H_SYNC);
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t wait_v_sync(void)
{
	int16_t ret = 0;
	volatile uint8_t *pGPIP = (uint8_t *)0xe88001;
	
	while((*pGPIP) & GP_V_SYNC);
	while(!((*pGPIP) & GP_V_SYNC));
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint16_t CRTC_Rastar_FirstPos(void)
{
	return (uint16_t)*CRTC_R06;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint16_t CRTC_Rastar_EndPos(void)
{
	return (uint16_t)*CRTC_R07;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_Rastar_SetPos(uint16_t pos)
{
	int16_t ret = 0;
	uint16_t buf;
	buf = (pos & Bit_Mask_10);
	*CRTC_R09 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_T_Scroll(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;
	int16_t buf;
	buf = (pos_x & Bit_Mask_10);
	*CRTC_R10 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_10);
	*CRTC_R11 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_G0_Scroll_4(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;
	int16_t buf;
	buf = (pos_x & Bit_Mask_9);
	*CRTC_R12 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_9);
	*CRTC_R13 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_G1_Scroll_4(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;
	int16_t buf;
	buf = (pos_x & Bit_Mask_9);
	*CRTC_R14 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_9);
	*CRTC_R15 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_G2_Scroll_4(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;
	int16_t buf;
	buf = (pos_x & Bit_Mask_9);
	*CRTC_R16 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_9);
	*CRTC_R17 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_G3_Scroll_4(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;
	int16_t buf;
	buf = (pos_x & Bit_Mask_9);
	*CRTC_R18 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_9);
	*CRTC_R19 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_G0_Scroll_8(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;
	int16_t buf;
	buf = (pos_x & Bit_Mask_9);
	*CRTC_R12 = (uint16_t)buf;
	*CRTC_R14 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_9);
	*CRTC_R13 = (uint16_t)buf;
	*CRTC_R15 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_G1_Scroll_8(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;
	int16_t buf;
	buf = (pos_x & Bit_Mask_9);
	*CRTC_R16 = (uint16_t)buf;
	*CRTC_R18 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_9);
	*CRTC_R17 = (uint16_t)buf;
	*CRTC_R19 = (uint16_t)buf;
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t CRTC_G0_Scroll_16(int16_t pos_x, int16_t pos_y)
{
	int16_t ret = 0;

	int16_t buf;
	buf = (pos_x & Bit_Mask_9);
	*CRTC_R12 = (uint16_t)buf;
	*CRTC_R14 = (uint16_t)buf;
	*CRTC_R16 = (uint16_t)buf;
	*CRTC_R18 = (uint16_t)buf;

	buf = (pos_y & Bit_Mask_9);
	*CRTC_R13 = (uint16_t)buf;
	*CRTC_R15 = (uint16_t)buf;
	*CRTC_R17 = (uint16_t)buf;
	*CRTC_R19 = (uint16_t)buf;

	return ret;
}

#endif	/* BIOS_CRTC_C */

