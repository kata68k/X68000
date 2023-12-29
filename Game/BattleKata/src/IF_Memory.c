#ifndef	IF_MEMORY_C
#define	IF_MEMORY_C

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <io.h>
#include <time.h>
#include <doslib.h>
#include <iocslib.h>

#include <usr_macro.h>

#include "IF_Memory.h"

static int32_t g_nMaxFreeMem = 0x7FFFFFFF;

/* �֐��̃v���g�^�C�v�錾 */
void *MyMalloc(int32_t);
int16_t MyMfree(void *);
int32_t	MaxMemSize(int8_t);
int32_t GetFreeMem(void);
int32_t	GetMaxFreeMem(void);

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void *MyMalloc(int32_t Size)
{
	void *pPtr = NULL;
	
	if(Size >= 0x1000000u)
	{
		printf("�������m�ۃT�C�Y���傫�������I(0x%x)\n", Size );
	}
	else
	{
#if 0
		pPtr = _dos_malloc(Size);	/* �������m�� */
		pPtr = malloc(sizeof(uint8_t) * Size);	/* �������m�� */
		
		if(pPtr == NULL)
		{
			puts("���������m�ۂł��܂���ł���");
		}
		else if((uint32_t)pPtr >= 0x81000000)
		{
			if((uint32_t)pPtr >= 0x82000000)
			{
				puts("�������s���ł�");
			}
			else
			{
				printf("���������m�ۂł��܂���ł���(%d)\n", (uint32_t)pPtr - 0x81000000 );
			}
			pPtr = NULL;
		}
		else
		{
			//printf("MyMalloc(0x%p)=%d\n", pPtr, Size);
		}
#else
		pPtr = malloc(sizeof(uint8_t) * Size);	/* �������m�� */
		
		if(pPtr == NULL)
		{
			puts("���������m�ۂł��܂���ł���");
		}

#endif
	}
	
	return pPtr;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	MyMfree(void *pPtr)
{
	int16_t ret = 0;
	
	if(pPtr == 0)
	{
		puts("���v���Z�X�A�q�v���Z�X�Ŋm�ۂ������������t���ŉ�����܂�");
	}
#if 0
	{
		uint32_t	result;
		result = _dos_mfree(pPtr);
		if(result < 0)
		{
			ret = -1;
		}
	}
#else
	free(pPtr);
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
int32_t	MaxMemSize(int8_t SizeType)
{
	int32_t ret = 0;
	int32_t i, dummy;
	int32_t chk[2];
	int8_t *ptMem[2];
	
	ptMem[0] = (int8_t *)0x0FFFFF;
	ptMem[1] = (int8_t *)0x100000;
	
	do{
		for(i=0; i<2; i++)
		{
			if((int32_t)ptMem[i] >= 0xC00000)	/* 12MB�̏�� */
			{
				chk[0] = 0;	/* �������[�v�E�o */
				chk[1] = 2;	/* �������[�v�E�o */
				break;
			}
			else
			{
				chk[i] = _dos_memcpy(ptMem[i], &dummy, 1);	/* �o�X�G���[�`�F�b�N */
			}
		}
		
		/* �����������̋��E */
		if( (chk[0] == 0) &&	/* �ǂݏ����ł��� */
			(chk[1] == 2) )		/* �o�X�G���[ */
		{
			break;	/* ���[�v�E�o */
		}
		
		ptMem[0] += 0x100000;	/* +1MB ���Z */
		ptMem[1] += 0x100000;	/* +1MB ���Z */
	}while(1);
	
//	printf("Memory Size = %d[MB](%d[Byte])(0x%p)\n", ((int)ptMem[1])>>20, ((int)ptMem[1]), ptMem[0]);

	switch(SizeType)
	{
	case 0:	/* Byte */
		ret = ((int)ptMem[1]);
		break;
	case 1:	/* KByte */
		ret = ((int)ptMem[1])>>10;
		break;
	case 2:	/* MByte */
	default:
		ret = ((int)ptMem[1])>>20;
		break;
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
int32_t	GetFreeMem(void)
{
	int32_t ret = 0;
	int32_t mem;

	mem = (int)_dos_malloc(-1);
	ret	= Mdiv1024(mem-0x81000000);

	if(g_nMaxFreeMem >= ret)
	{
		g_nMaxFreeMem = ret;
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
int32_t	GetMaxFreeMem(void)
{
	int32_t ret = 0;

	ret = g_nMaxFreeMem;

	return ret;
}

#endif	/* IF_MEMORY_C */

