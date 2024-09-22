#ifndef	BIOS_MFP_C
#define	BIOS_MFP_C

#include <stdio.h>
#include <iocslib.h>
#include <interrupt.h>

#include <usr_macro.h>

#include "BIOS_MFP.h"
#include "BIOS_MPU.h"
#include "BIOS_Moon.h"
#include "BIOS_CRTC.h"
#include "BIOS_PCG.h"
#include "main.h"


/* define��` */

/* �O���[�o���ϐ� */
int8_t				g_bMFP = FALSE;
volatile uint16_t	g_uHsync_count;
volatile uint16_t	g_uVsync_count;
volatile uint16_t	g_uRaster_count;

/* �X�^�e�B�b�N�ϐ� */
static volatile uint8_t 	g_bTimer_D;
static volatile uint32_t	g_NowTime;
static volatile uint32_t	g_StartTime;
static volatile uint32_t	g_CPU_Time;
static volatile uint8_t		g_bRasterUse;
static volatile uint16_t	g_uRasterPos;
static volatile uint16_t	g_uRasterPos_first;
static volatile uint16_t	g_uRasterPos_clip;

#if 0
static uint32_t g_unTime_cal=0, g_unTime_cal_PH=0, g_unTime_min_PH=0;
#endif

/* �֐��̃v���g�^�C�v�錾 */
int16_t MFP_INIT(void);
int16_t MFP_EXIT(void);
int16_t MFP_RESET(void);
int16_t TimerD_INIT(void);
int16_t TimerD_EXIT(void);
uint8_t GetNowTime(uint32_t *);	/* ���݂̎��Ԃ��擾���� */
uint8_t SetNowTime(uint32_t);		/* ���݂̎��Ԃ�ݒ肷�� */
uint8_t GetStartTime(uint32_t *);	/* �J�n�̎��Ԃ��擾���� */
uint8_t SetStartTime(uint32_t);	/* �J�n�̎��Ԃ�ݒ肷�� */
uint8_t GetPassTime(uint32_t, uint32_t *);	/* �o�߃^�C�}�[ */
uint32_t Get_CPU_Time(void);
int16_t StartRasterInt(void);
int16_t StopRasterInt(void);
uint8_t GetRasterCount(uint16_t *);
uint8_t	SetRasterCount(uint16_t, uint16_t);

static void interrupt Timer_D_Func(void);
static void interrupt Hsync_Func(void);
static void interrupt Raster_Func(void);
static void interrupt Vsync_Func(void);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t MFP_INIT(void)
{
	int32_t	vdispst = -1;
#if CNF_VDISP
#if CNF_XSP
#else
	volatile uint8_t *MFP_TACR = (uint8_t *)0xe88019;
	volatile uint8_t *MFP_TADR = (uint8_t *)0xe8801f;
#endif	/* CNF_XSP */
#endif	/* CNF_VDISP */

	g_bMFP = FALSE;

#if CNF_HDISP	
	/* H-Sync���荞�� */
	g_uHsync_count = 0;
#endif /* CNF_HDISP */

	g_CPU_Time = TimerD_INIT();		/* Timer-D������ */
	
#if CNF_RASTER
	/* ���X�^�[���荞�� */
 #if CNF_XSP
	/* ���X�^���荞�݊J�n(IF_PCG.c�Ŏ��{) */
 #else
	g_uRaster_count = 0;
	StopRasterInt();	/* ���X�^�[���荞�ݏ������~ */
	g_uRasterPos = 0;
	g_uRasterPos_first = 0;
	g_uRasterPos_clip = 0xFFFF;
 #endif
#endif /* CNF_RASTER */

#if CNF_VDISP
	/* V-Sync���荞�� */
	g_uVsync_count = 0;
#if CNF_XSP
#else
	//VDISPST ���񂾂��o�O����iEXCEED.����j
	*MFP_TACR = 0x00;	/* �^�C�}�[�`���~�߂� */
	*MFP_TADR = 0x01;	/* �J�E���^��ݒ�(0=256��) */
	*MFP_TACR = 0x08;	/* �C�x���g�J�E���g���[�h�ɐݒ肷�� */
#endif	/* CNF_XSP */

#if CNF_XSP
	/* �A�����Ԋ��荞�݊J�n(IF_PCG.c�Ŏ��{) */
#elif CNF_MACS	
 #ifdef 	MACS_MOON
	vdispst = MACS_Vsync(Vsync_Func);		/* V-Sync���荞�� */
 #else	/* MACS_MOON */
	vdispst = VDISPST(Vsync_Func, 0, 1);	/* V-Sync���荞�� �A�� */
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
	vdispst = VDISPST(Vsync_Func, 0, 1);	/* V-Sync���荞�� �A�� */
#endif	/* CNF_MACS */
//	Message_Num(&vdispst,  0, 10, 2, MONI_Type_SI, "%4d");
#endif	/* CNF_VDISP */
	g_bMFP = TRUE;

	return vdispst;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t MFP_EXIT(void)
{
	int32_t	vdispst = -1;

	if(g_bMFP == TRUE)
	{
		g_bMFP = FALSE;

		TimerD_EXIT();			/* Timer-D �I�� */
#if CNF_RASTER
		StopRasterInt();		/* ���X�^�[���荞�ݏ������~ */
		puts("MFP_EXIT CRTCRAS");
#endif /* CNF_RASTER */

#if CNF_HDISP
		_iocs_hsyncst((void *)0);		/* stop */
		puts("MFP_EXIT HSYNCST");
#endif	/* CNF_HDISP */

#if CNF_VDISP
#if CNF_XSP

#elif CNF_MACS
 #ifdef 	MACS_MOON
		vdispst = MACS_Vsync_R(Vsync_Func);	/* stop */
		puts("MFP_EXIT MACS_Vsync_R");
 #else	/* MACS_MOON */
		vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
		puts("MFP_EXIT VDISPST");
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
		vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
		puts("MFP_EXIT VDISPST");
#endif	/* CNF_MACS */
//		Message_Num(&vdispst,  6, 10, 2, MONI_Type_SI, "%4d");
#endif	/* CNF_VDISP */
	}

	return vdispst;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t MFP_RESET(void)
{
	int16_t	ret = 0;

	volatile uint8_t *MFP_INTCTRL_A = (uint8_t *)0xE88007;	/* ���荞�݃C�l�[�u�����W�X�^A */
	volatile uint8_t *MFP_TADR  	= (uint8_t *)0xE8801F;	/* �^�C�}�[A�f�[�^���W�X�^ */
	
	*MFP_TADR = 1;					/* �J�E���^�ݒ� */
	*MFP_INTCTRL_A |= 0b01000000;	/* CRTC Raster���荞�݋��� */
//	*MFP_INTCTRL_A |= 0b00100000;	/* Timer-A���荞�݋��� */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	TimerD_INIT()
{
	uint16_t CpuCount=0;
	int32_t nUse;
	/* Timer-D���荞�� */
	g_NowTime = 0;
	g_StartTime = 0;

	nUse = _iocs_timerdst(Timer_D_Func, 7, 20);	/* Timer-D�Z�b�g */	/* 50us(7) x 20cnt = 1ms */
//	TIMERDST(Timer_D_Func, 3, 5);	/* Timer-D�Z�b�g */	/* 4us(3) x 5cnt = 20us */
	if(nUse == 0)
	{
		g_bTimer_D =TRUE;
		SetNowTime(g_NowTime);	/* ���Ԃ̏����� */
		/* �}�C�R���N���b�N���v�� */
		do
		{
			CpuCount++;
		}
		while(g_NowTime==0);
	}
	else
	{
		puts("TimerD���g���܂���ł���");		
	}

	return CpuCount;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	TimerD_EXIT()
{
	if(g_bTimer_D == TRUE)
	{
		_iocs_timerdst((void *)0, 0, 1);	/* stop */
		g_bTimer_D = FALSE;
	}

	return 0;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint8_t GetNowTime(uint32_t *time)	/* ���݂̎��Ԃ��擾���� */
{
	*time = g_NowTime;
	return g_bTimer_D;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint8_t SetNowTime(uint32_t time)	/* ���݂̎��Ԃ�ݒ肷�� */
{
	g_NowTime = time;
	
	return g_bTimer_D;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint8_t GetStartTime(uint32_t *time)	/* �J�n�̎��Ԃ��擾���� */
{
	*time = g_StartTime;
	return g_bTimer_D;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint8_t SetStartTime(uint32_t time)	/* �J�n�̎��Ԃ�ݒ肷�� */
{
	g_StartTime = time;
	return g_bTimer_D;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	����\1ms */
/*===========================================================================================*/
uint8_t GetPassTime(uint32_t uSetTimer, uint32_t *pOldTimer)	/* �o�߃^�C�}�[ */
{
	uint8_t ret = FALSE;
	uint32_t uTime, uTimeDiff, uTimeMem;
	
	uTime = g_NowTime;
	uTimeMem = (*pOldTimer);

	if(*pOldTimer == PASSTIME_INIT)
	{
		*pOldTimer = uTime;
		ret = TRUE;			/* �o�߂��� */
	}
	else
	{
		uTimeDiff = uTime - uTimeMem;
		
		if(uSetTimer <= uTimeDiff)
		{
			*pOldTimer = uTime;
			ret = TRUE;			/* �o�߂��� */
		}
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
int16_t StartRasterInt(void)
{
	int16_t ret = 0;

	g_bRasterUse = TRUE;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t StopRasterInt(void)
{
	int16_t ret = 0;
	
	if(g_bRasterUse == TRUE)
	{
		g_bRasterUse = FALSE;
		_iocs_crtcras((void *)0, 0);	/* stop */
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
uint8_t	GetRasterCount(uint16_t *uCount)
{
	uint8_t	ret = 0;

	*uCount = g_uRasterPos;
	ret = g_bRasterUse;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint8_t	SetRasterCount(uint16_t uCount, uint16_t uClip)
{
	uint8_t	ret = 0;

	g_uRasterPos = uCount;
	g_uRasterPos_first = uCount;
	g_uRasterPos_clip = uClip;
	ret = g_bRasterUse;
	
	return ret;
}
/* ���荞�݊֐��͏C���q��interrupt�����邱�� */	/* ���ɂ����iKunihiko Ohnaka�j����̃A�h�o�C�X */

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void interrupt Timer_D_Func(void)
{
	g_NowTime++;

	App_TimerProc();	/* �^�C�}�[���荞�ݏ��� */

	IRTE();	/* ���荞�݊֐��̍Ō�ŕK�����{ */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint8_t Timer_D_Less_NowTime(void)
{
	if(g_bTimer_D == FALSE)
	{
		 g_NowTime += 15;	/* 15ms(1ms����\) */
	}

	return	g_bTimer_D;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint32_t Get_CPU_Time(void)
{
	return g_CPU_Time;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void interrupt Hsync_Func(void)
{
#if CNF_HDISP	/* H-Sync���荞�ݏ��� */
	_iocs_hsyncst((void *)0);			/* stop */

	g_uHsync_count++;

#endif	/* CNF_HDISP */
	IRTE();	/* ���荞�݊֐��̍Ō�ŕK�����{ */
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* ���X�^�[���荞�݂̏������͕K�v�Œ������ */	/* EXCEED.����̃A�h�o�C�X */
/* �Q���C����΂����炢�����x�̌��E */	/* EXCEED.����̃A�h�o�C�X */
static void interrupt Raster_Func(void)
{
#if CNF_RASTER	/* ���X�^�[���荞�ݏ��� */
	uint16_t uRaster_pos;

	g_uRaster_count++;		/* ���X�^�[���荞�ݏ����� */

	uRaster_pos = g_uRasterPos - g_uRasterPos_first;

	/* �A�v���P�[�V�������� */
	App_RasterProc( &uRaster_pos );
	
	g_uRasterPos = Mmin((g_uRasterPos_first + uRaster_pos), g_uRasterPos_clip);	/* �N���b�v���� */
	CRTC_Rastar_SetPos(g_uRasterPos);

#endif	/* CNF_RASTER */

	IRTE();	/* ���荞�݊֐��̍Ō�ŕK�����{ */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void interrupt Vsync_Func(void)
{
#if CNF_VDISP	/* V-Sync���荞�ݏ��� */
	int32_t	vdispst = -1;	
	
	g_uVsync_count++;

#if CNF_XSP
	vdispst = 0;
#elif CNF_MACS
 #ifdef 	MACS_MOON
	/* �������Ȃ� */
	/* MACS_Vsync�o�R�ł���Ί��荞�݋֎~��ԂŎ��s����Ă��邽�߁A���荞�݋֎~���u�s�v */
 #else	/* MACS_MOON */
	Set_DI();	/* ���荞�݋֎~�ݒ� */
	vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
	Set_EI();	/* ���荞�݋֎~���� */
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
	Set_DI();	/* ���荞�݋֎~�ݒ� */
	vdispst = _iocs_vdispst((void *)0, 0, 0);	/* stop */
	Set_EI();	/* ���荞�݋֎~���� */
#endif	/* CNF_MACS */
	
	App_VsyncProc();	/* �A�v�� V-Sync���荞�ݏ��� */

#if CNF_RASTER	/* ���X�^�[���荞�ݏ��� */
	/* ���X�^���荞�� */
	if(g_bRasterUse == TRUE)
	{
#if CNF_XSP
#else	/* CNF_XSP */
		_iocs_crtcras((void *)0, 0);	/* stop */
		_iocs_crtcras( Raster_Func, g_uRasterPos );	/* ���X�^�[���荞�� */
#endif
	}
#endif	/* CNF_RASTER */
	
#if CNF_HDISP	/* H-Sync���荞�ݏ��� */
	Hsync_count = 0;
	_iocs_hsyncst((void *)0);		/* stop */
	_iocs_hsyncst(Hsync_Func);	/* H-Sync���荞�� */
#endif

#if CNF_XSP
	vdispst = 0;
#elif CNF_MACS
 #ifdef 	MACS_MOON
	/* �������Ȃ� */
	IRTS();	/* MACSDRV�ł�rts�ň����K�v���� */
 #else	/* MACS_MOON */
	vdispst = _iocs_vdispst(Vsync_Func, 0, 1);	/* V-Sync���荞�� �A�� */
	IRTE();	/* ���荞�݊֐��̍Ō�ŕK�����{ */
 #endif	/* MACS_MOON */
#else	/* CNF_MACS */
	vdispst = _iocs_vdispst(Vsync_Func, 0, 1);	/* V-Sync���荞�� �A�� */
	IRTE();	/* ���荞�݊֐��̍Ō�ŕK�����{ */
#endif	/* CNF_MACS */
#else	/* CNF_VDISP */
	IRTE();	/* ���荞�݊֐��̍Ō�ŕK�����{ */
#endif	/* CNF_VDISP */
}

#endif	/* BIOS_MFP_H */
