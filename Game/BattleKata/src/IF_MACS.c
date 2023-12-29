#ifndef	IF_MACS_C
#define	IF_MACS_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usr_macro.h>
#include "IF_MACS.h"
#include "BIOS_CRTC.h"
#include "BIOS_Moon.h"
#include "BIOS_PCG.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"

#ifdef 	MACS_MOON

/* �O���[�o���ϐ� */
int8_t		g_mov_list[MACS_MAX][256]	=	{0};
uint32_t	g_mov_list_max	=	0u;
/* �\���̒�` */

#endif	/* MACS_MOON */

/* �֐��̃v���g�^�C�v�錾 */
void MOV_INIT(void);
int32_t MOV_Play(uint8_t);
int32_t MOV_Play2(uint8_t, uint8_t);
static uint32_t moon_check(void);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void MOV_INIT(void)
{
#if CNF_MACS

#ifdef 	MACS_MOON
	uint16_t	i;
	int32_t	moon_chk = 0;

	/* MACS�Đ����邽�߂̃h���C�o�풓�`�F�b�N */
	if(moon_check() == 0)
	{
		puts("MACSDRV.X��MOON.X �풓��������");
		exit(0);
	}
	
	/* ���X�g����Moon.x�Ń������o�^ */
	for(i = 0; i < g_mov_list_max; i++)
	{
		moon_chk = MoonRegst(g_mov_list[i]);	/* �������֓o�^ */
		if(moon_chk < 0)
		{
			printf("MoonRegst = %d\n", moon_chk);
		}
		else
		{
			puts("error:Moon�ɓo�^�ł��܂���ł����B");
		}
	}
#endif	/* MACS_MOON */

#endif	/* CNF_MACS */
}

/* ����Đ� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int32_t MOV_Play(uint8_t bPlayNum)
{
	int32_t	ret=0;

	if(bPlayNum >= g_mov_list_max)return ret;

#ifdef 	MACS_MOON
	{
		int32_t	moon_stat = 0;

		moon_stat = MoonPlay(g_mov_list[bPlayNum]);	/* �Đ� */
		if(moon_stat != 0)
		{
			//printf("MoonPlay  = %d\n", moon_stat);
			/* �A�{�[�g(-4)�����ꍇ�́A���������@���K�v�������悤�ȁc */
		}
		else
		{
		}
	//	MACS_Sleep();	/* �X���[�v */
	}
#else	/* MACSDRV�P�Ƃ̏ꍇ */
	{
		int8_t	*pBuff = NULL;
		int32_t	FileSize = 0;

		GetFileLength(g_mov_list[bPlayNum], &FileSize);	/* �t�@�C���̃T�C�Y�擾 */
		printf("MacsData = %d\n", FileSize);
		pBuff = (int8_t*)MyMalloc(FileSize);	/* �������m�� */
		if(pBuff != NULL)
		{
			File_Load(g_mov_list[bPlayNum], pBuff, sizeof(uint8_t), FileSize );	/* �t�@�C���ǂݍ��݂��烁�����֕ۑ� */
			MACS_Play(pBuff);	/* �Đ� */
			MyMfree(pBuff);	/* ��������� */
		}
	}
#endif	/* MACS_MOON */

	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int32_t MOV_Play2(uint8_t bPlayNum, uint8_t bOption)
{
	int32_t	ret=0;

	if(bPlayNum >= g_mov_list_max)return ret;

#ifdef 	MACS_MOON
	{
		int32_t	moon_stat = 0;

		moon_stat = MoonPlay2(g_mov_list[bPlayNum], bOption);	/* �Đ� */
		if(moon_stat != 0)
		{
			//printf("MoonPlay  = %d\n", moon_stat);
			/* �A�{�[�g(-4)�����ꍇ�́A���������@���K�v�������悤�ȁc */
		}
		else
		{
		}
	//	MACS_Sleep();	/* �X���[�v */
	}
#else	/* MACSDRV�P�Ƃ̏ꍇ */
	{
		int8_t	*pBuff = NULL;
		int32_t	FileSize = 0;

		GetFileLength(g_mov_list[bPlayNum], &FileSize);	/* �t�@�C���̃T�C�Y�擾 */
		printf("MacsData = %d\n", FileSize);
		pBuff = (int8_t*)MyMalloc(FileSize);	/* �������m�� */
		if(pBuff != NULL)
		{
			File_Load(g_mov_list[bPlayNum], pBuff, sizeof(uint8_t), FileSize );	/* �t�@�C���ǂݍ��݂��烁�����֕ۑ� */
			MACS_Play(pBuff);	/* �Đ� */
			MyMfree(pBuff);	/* ��������� */
		}
	}
#endif	/* MACS_MOON */

	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static uint32_t moon_check(void)
{
	uint32_t	ret = 0;
#ifdef 	MACS_MOON
	uint64_t *v = (uint64_t *)( B_LPEEK((uint64_t *)(0x1d1*4)) + 2 );
	if( B_LPEEK( v ) == 'MOON' && B_LPEEK( v+1 ) == 'IOCS' )
	{
		ret = 1;
	}
#else	/* MACS_MOON */
	ret = 1;
#endif	/* MACS_MOON */

	return ret;
}

#endif	/* IF_MACS_C */

