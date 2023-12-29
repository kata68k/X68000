#ifndef	IF_PCG_C
#define	IF_PCG_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>
#include <usr_macro.h>

#include "IF_PCG.h"
#include "BIOS_PCG.h"
#include "BIOS_CRTC.h"
#include "IF_FileManager.h"

#if  CNF_XSP
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */

/* �o�b�f�f�[�^ */
/*
	XSP �p PCG �z�u�Ǘ��e�[�u��
	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B
*/
char pcg_alt[PCG_MAX + 1];		/* XSP �p PCG �z�u�Ǘ��e�[�u��	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B*/
char pcg_dat[PCG_MAX * 128];	/* �o�b�f�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
unsigned short pal_dat[ 256 ];	/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
char g_bSP = FALSE;

/* �\���̒�` */
/* �֐��̃v���g�^�C�v�錾 */
static void sp_dataload(void);
void PCG_INIT(void);
void PCG_ON(void);
void PCG_OFF(void);
void PCG_START_SYNC(int16_t);
void PCG_END_SYNC(int16_t);
void PCG_PUT_1x1(int16_t, int16_t, int16_t, int16_t);
void PCG_PUT_2x1(int16_t, int16_t, int16_t, int16_t);
void PCG_PUT_2x2(int16_t, int16_t, int16_t, int16_t);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void sp_dataload(void)
{
	FILE *fp;
	unsigned int i,j;

	/*�X�v���C�g���W�X�^������*/
	for(j = 0x80000000; j < 0x80000000 + 128; j++ ){
		SP_REGST(j,-1,0,0,0,0);
	}
	
	/*-----------------[ �o�b�f�f�[�^�ǂݍ��� ]-----------------*/

	fp = fopen( "data/sp/BALL.SP" , "rb" ) ;
//	fp = fopen( "SP_DATA/SAKANA.SP" , "rb" ) ;
	j = fread( pcg_dat
			,  128		/* 1PCG = 128byte */
			,  PCG_MAX	/* PCG�̐� */
			,  fp
	) ;
	fclose( fp ) ;

//	for( i = 0 ; i < PCG_MAX ; i++ ){
//		SP_DEFCG( i, 1, &pcg_dat[i * 128] );
//	}
	
#if 0
	/*-----------[ PCG �f�[�^���c��ʗp�� 90 �x��] ]-----------*/

	for (i = 0; i < 256; i++) {
		pcg_roll90(&pcg_dat[i * 128], 1);
	}
#endif
	
	/*--------[ �X�v���C�g�p���b�g�f�[�^�ǂݍ��݂ƒ�` ]--------*/

	fp = fopen( "data/sp/BALL.PAL" , "rb" ) ;
//	fp = fopen( "SP_DATA/SAKANA.PAL" , "rb" ) ;
	fread( pal_dat
		,  2		/* 1palet = 2byte */
		,  256		/* 16palet * 16block */
		,  fp
		) ;
	fclose( fp ) ;

	/* �X�v���C�g�p���b�g�ɓ]�� */
	for( i = 0 ; i < 256 ; i++ )
	{
		SPALET( (i&15) | (1<<0x1F) , i/16 + 1, pal_dat[i] ) ;
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_INIT(void)
{
#if  CNF_XSP
#else   /* CNF_XSP 0 */
	uint32_t	i, j, uPCG_num;
#endif  /*CNF_XSP*/

	/* �X�v���C�g�̏����� */
	g_bSP = FALSE;
#if  CNF_XSP
	printf("XSP mode");
    /*---------------------[ XSP �������� ]---------------------*/
	_iocs_sp_init();			/* �X�v���C�g�̏����� */

    sp_dataload();  	/* �X�v���C�g�̃f�[�^�ǂݍ��� */

	PCG_ON();			/* XSP ������ */

	xsp_mode(1);

	xsp_pcgmask_off(0, 127);

//	xsp_vertical(1);    /* �c��ʃ��[�h */
	/*---------------------[ XSP �������� ]---------------------*/
#else   /* CNF_XSP 0 */
	
	printf("SP/BG mode");

	/* �X�v���C�g�̏����� */
	_iocs_sp_init();			/* �X�v���C�g�̏����� */
	
	/*�X�v���C�g���W�X�^������*/
	for( j = 0x80000000; j < 0x80000000 + 128; j++ )
	{
		_iocs_sp_regst(j,-1,0,0,0,0);
	}
	for( i = 0; i < 256; i++ )
	{
		_iocs_sp_cgclr(i);			/* �X�v���C�g�N���A */
	}
	
	//	_iocs_bgctrlgt(1);				/* BG�R���g���[�����W�X�^�ǂݍ��� */
	_iocs_bgscrlst(0,0,0);				/* BG�X�N���[�����W�X�^�ݒ� */
	_iocs_bgscrlst(1,0,0);				/* BG�X�N���[�����W�X�^�ݒ� */
	_iocs_bgtextcl(0,SetBGcode(0,0,0,0));	/* BG�e�L�X�g�N���A */
	_iocs_bgtextcl(1,SetBGcode(0,0,0,0));	/* BG�e�L�X�g�N���A */
	//	BGTEXTGT(1,1,0);			/* BG�e�L�X�g�ǂݍ��� */
	
	/* �X�v���C�g�Ǘ��p�o�b�t�@�̃N���A */
	for(uPCG_num = 0; uPCG_num < PCG_MAX; uPCG_num++)
	{
		memset(&g_stPCG_DATA[uPCG_num], 0, sizeof(ST_PCG) );
	}
#endif  /*CNF_XSP*/
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_ON(void)
{
	if(g_bSP == FALSE)
	{
	    PCG_VIEW(1);        /* SP ON */
#if  CNF_XSP
    	xsp_on();			/* XSP ON */
		xsp_pcgdat_set(pcg_dat, pcg_alt, sizeof(pcg_alt));  /* PCG �f�[�^�� PCG �z�u�Ǘ����e�[�u�����w�� */
#else   /* CNF_XSP 0 */
#endif  /*CNF_XSP*/
		g_bSP = TRUE;
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_OFF(void)
{
	if(g_bSP == TRUE)
	{
		g_bSP = FALSE;
#if  CNF_XSP
		xsp_off();			/* XSP OFF */
#else   /* CNF_XSP 0 */
#endif  /*CNF_XSP*/
	    PCG_VIEW(0);        /* SP OFF */
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_START_SYNC(int16_t count)
{
#if  CNF_XSP
//	xsp_vsync(count);
	xsp_vsync2(count);  /* �������� */
#else   /* CNF_XSP 0 */
    /* �Ȃ� */
#endif  /*CNF_XSP*/
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_END_SYNC(int16_t count)
{
	PCG_Main();	/* �X�v���C�g�Ǘ� */

#if  CNF_XSP
    /* �X�v���C�g���ꊇ�\������ */
    xsp_out();
//	printf("xsp_out()\n");
#else   /* CNF_XSP 0 */
    /* �������� */
	wait_vdisp(count);	/* ��count�^60�b�҂�	*/
#endif  /*CNF_XSP*/
}

void PCG_PUT_1x1(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x, y, pt, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

void PCG_PUT_2x1(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x-8, y, pt+0, info);
    xsp_set( x+8, y, pt+1, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

void PCG_PUT_2x2(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x-8, y-8, pt+0, info);
    xsp_set( x+8, y-8, pt+1, info);
    xsp_set( x-8, y+8, pt+2, info);
    xsp_set( x+8, y+8, pt+3, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

void PCG_PUT_3x3(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x-24, y-24, pt+0, info);
    xsp_set( x- 8, y-24, pt+1, info);
    xsp_set( x+ 8, y-24, pt+2, info);

    xsp_set( x-24, y- 8, pt+3, info);
    xsp_set( x- 8, y- 8, pt+4, info);
    xsp_set( x+ 8, y- 8, pt+5, info);

    xsp_set( x-24, y+ 8, pt+6, info);
 	xsp_set( x- 8, y+ 8, pt+7, info);
 	xsp_set( x+ 8, y+ 8, pt+8, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

#endif	/* IF_PCG_C */

