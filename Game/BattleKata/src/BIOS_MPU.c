#ifndef	BIOS_MPU_C
#define	BIOS_MPU_C

/* system include */
#include <stdio.h>
#include <stdlib.h>
#include <sys/iocs.h>
#include <interrupt.h>

#include <usr_macro.h>
#include "BIOS_MPU.h"

#ifdef DEV_GCC
	/* Develop �� GCC �g��*/
	DOSCALL INTVCG (short);
	DOSCALL INTVCS (short, void *);
	DOSCALL PRINT (char *);
#else
	#include <doslib.h>
#endif

/*
stdin,stdout XC Ver �ˑ��̂���
sprintf �Ń}�N���ɂ��Ă���
*/
#define Fprint(buf,fmt,mes) \
	do { \
		sprintf ((buf), (fmt), (mes)); \
		PRINT ((buf)); \
	} while (0)

static struct {
	int flag;				/* �G���[��ʃt���O*/
	unsigned short sr_reg;	/* �G���[���� SR */
	int pc_reg;				/* �G���[���� PC */
	char *mes;				/* ���b�Z�[�W */
} trapbuf;

int32_t	g_nIntLevel = 0;
int32_t	g_nIntCount = 0;

static void (*trap_14)();	/* ���� trap 14 �x�N�^��ۑ�*/

extern int _main;			/* �X�^�[�g�A�b�v�̃_�~�[ */

static void interrupt trap14(void)
{
	PRAMREG (code, d7);	/* �p�����[�^���W�X�^ d7 */
	PRAMREG (add, a6);	/* �p�����[�^���W�X�^ a6 */
	SET_FRAME (a5);		/* �t���[���|�C���^�� a5 �Ɏw�� */
	char *p =(char *)add;

	/* code ����G���[�𕪐� */
	switch (code & 0xFFFF)
	{
		case 2:
			/* �o�X�G���[ */
			trapbuf.flag = code;
			trapbuf.mes = "�o�X�G���[���������܂���";
			break;
		case 3:
			/* �A�h���X�G���[ */
			trapbuf.flag = code;
			trapbuf.mes = "�A�h���X�G���[���������܂���";
			break;
		case 4:
			/* �������Ȗ��� */
			trapbuf.flag = code;
			trapbuf.mes = "�������Ȗ��߂����s���܂���";
			break;
	}
	
	if ((code & 0xFF00))
	{
		if ((code & 0xFF) == 2)
		{
			trapbuf.flag = -1;
			trapbuf.mes = "�h���C�u�̏������ł��Ă��܂���";
		}
	}
	
	if (trapbuf.flag || ((code & 0xFFFF) == 0x1F) || ((code & 0xFFFF) == 0x301F))
	{
		trapbuf.sr_reg = *(short *)p;
		p+=2;
		trapbuf.pc_reg = *(int *)p;
		asm ("\tmove.w #$ff,d0\n\ttrap #15\n");
		IJUMP_RTE();
	}
	else
		IJUMP(trap_14);
}

static void my_abort (void)
{
	char buf[64];
//	void (*usr_abort)(void);	/* ���[�U�̃A�{�[�g�����֐� */
	
	INTVCS(0x2E,trap_14);
	
	if (usr_abort)
		usr_abort();
	
	if (trapbuf.flag > 0)
	{
		extern int _SSTA;
		extern int _PSTA;
		trapbuf.flag = 0;
		Fprint (buf, "%s\r\n", trapbuf.mes);
		Fprint (buf, " pc=%X" , trapbuf.pc_reg);
		Fprint (buf, " offset=%X\r\n", trapbuf.pc_reg - (int) &_main);
		exit(512);
	}
	else
	{
		Fprint (buf, "%s\r\n", trapbuf.mes);
		exit(256);
	}
}

void init_trap14 (void)
{
	trap_14 = (void *)INTVCG (0x2E);
	INTVCS (0x2E, trap14);
	INTVCS (0xFFF2, my_abort);
	INTVCS (0xFFF1, my_abort);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void Set_DI(void)
{
	if(g_nIntCount == 0)
	{
#if 0
		/*�X�[�p�[�o�C�U�[���[�h�I��*/
		if(g_nSuperchk > 0)
		{
			_dos_super(g_nSuperchk);
		}
#endif
		g_nIntLevel = intlevel(6);	/* ���֐ݒ� */
		g_nIntCount = Minc(g_nIntCount, 1u);
		
#if 0
		/* �X�[�p�[�o�C�U�[���[�h�J�n */
		g_nSuperchk = _dos_super(0);
#endif
	}
	else
	{
		g_nIntCount = Minc(g_nIntCount, 1u);
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void Set_EI(void)
{
	if(g_nIntCount == 1)
	{
		g_nIntCount = Mdec(g_nIntCount, 1);
		
		/* �X�v���A�X���荞�݂̒��̐l��(�ȉ���)��� */
		/* MFP�Ńf�o�C�X���̊����݋֎~�ݒ������ۂɂ͕K��SR����Ŋ����݃��x�����グ�Ă����B*/
		asm ("\ttst.b $E9A001\n\tnop\n");
		/*
			*8255(�ޮ��è��)�̋�ǂ�
			nop		*���O�܂ł̖��߃p�C�v���C���̓���
					*�����b�A���̖��ߏI���܂łɂ�
					*����ȑO�̃o�X�T�C�N���Ȃǂ�
					*�������Ă��邱�Ƃ��ۏ؂����B
		*/
#if 0
		/*�X�[�p�[�o�C�U�[���[�h�I��*/
		if(g_nSuperchk > 0)
		{
			_dos_super(g_nSuperchk);
		}
#endif
		intlevel(g_nIntLevel);	/* ���։��� */
#if 0
		/* �X�[�p�[�o�C�U�[���[�h�J�n */
		g_nSuperchk = _dos_super(0);
#endif
	}
	else
	{
		g_nIntCount = Mdec(g_nIntCount, 1);
	}
}

/*===========================================================================================*/
/* �֐���	:	*/
/* ����		:	*/
/* �߂�l	:	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		:	*/
/*===========================================================================================*/
uint32_t mpu_cache_clr(void)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};
	uint32_t	retReg;

	stInReg.d0 = IOCS_SYS_STAT;		/* �V�X�e����Ԃ̐ݒ� */
	stInReg.d1 = 0x03;				/* $03 �L���b�V���N���A */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */

	return stOutReg.d1;
}
#endif	/* BIOS_MPU_C */

/*===========================================================================================*/
/* �֐���	:	*/
/* ����		:	*/
/* �߂�l	:	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		:	*/
/*===========================================================================================*/
int32_t	mpu_stat_chk(void)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};
	int32_t	retReg;	/* d0 */
	
	stInReg.d0 = IOCS_SYS_STAT;	/* IOCS _SYS_STAT($AC) */
	stInReg.d1 = 0;				/* MPU�X�e�[�^�X�̎擾 */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
//	printf("SYS_STAT 0x%x\n", (retReg & 0x0F));
    /* 060turbo.man���
		$0000   MPU�X�e�[�^�X�̎擾
			>d0.l:MPU�X�e�[�^�X
					bit0�`7         MPU�̎��(6=68060)
					bit14           MMU�̗L��(0=�Ȃ�,1=����)
					bit15           FPU�̗L��(0=�Ȃ�,1=����)
					bit16�`31       �N���b�N�X�s�[�h*10
	*/
	
	return (retReg & 0x0F);
}

/*===========================================================================================*/
/* �֐���	:	*/
/* ����		:	*/
/* �߂�l	:	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		:	*/
/*===========================================================================================*/
int32_t	Get_ROM_Ver(void)
{
	int32_t dat, ver, year, month, day;
	
	dat = _iocs_romver();
	
	ver		= (dat & 0xFF000000) >> 24;
	year	= (dat & 0x00FF0000) >> 16;
	month	= (dat & 0x0000FF00) >> 8;
	day		= (dat & 0x000000FF);
	
	if(year >= 0x80)	/* 0x80�`0x99 �� 19xx�N(1980�`1999�N) */
	{
//		printf("ROM ver%x.%x(19%02x/%02x/%02x)(0x%x)\n", ((ver&0xF0)>>4), (ver&0x0F),year, month, day, dat);
	}
	else				/* 0x00�`0x79 �� 20xx�N(2000�`2079�N) */
	{
//		printf("ROM ver%x.%x(20%02x/%02x/%02x)(0x%x)\n", ((ver&0xF0)>>4), (ver&0x0F),year, month, day, dat);
	}
	
	return ver;
}
