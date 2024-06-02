#ifndef	IF_PCG_C
#define	IF_PCG_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>
#include <usr_macro.h>

#include "IF_PCG.h"
#include "BIOS_CRTC.h"
#include "BIOS_MFP.h"
#include "IF_FileManager.h"
#include "IF_Memory.h"
#include "main.h"

#if  CNF_XSP
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */

/* �o�b�f�f�[�^ */
int8_t		sp_list[PCG_MAX][256]	=	{0};
uint32_t	sp_list_max	=	0u;
static size_t	sp_dat_size[PCG_MAX]	=	{0};

int8_t		pal_list[PAL_MAX][256]	=	{0};
uint32_t	pal_list_max	=	0u;
static size_t	pal_dat_size[PAL_MAX]	=	{0};

/*
	XSP �p PCG �z�u�Ǘ��e�[�u��
	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B
*/
int8_t pcg_alt[PCG_MAX + 1];	/* XSP �p PCG �z�u�Ǘ��e�[�u��	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B*/
int8_t pcg_dat[PCG_MAX * 128];	/* �o�b�f�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
uint16_t pal_dat[ 256 ];		/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
int8_t g_bSP = FALSE;

/* �\���̒�` */
/* �֐��̃v���g�^�C�v�錾 */
void sp_dataload(void);
void PCG_INIT(void);
void PCG_ON(void);
void PCG_OFF(void);
int8_t Get_SP_Sns(void);
void BG_ON(int16_t);
void BG_OFF(int16_t);
void PCG_START_SYNC(int16_t);
void PCG_END_SYNC(int16_t);
void PCG_COL_SHIFT(int16_t, int16_t);
void PCG_PUT_1x1(int16_t, int16_t, int16_t, int16_t);
void PCG_PUT_2x1(int16_t, int16_t, int16_t, int16_t);
void PCG_PUT_2x2(int16_t, int16_t, int16_t, int16_t);
void BG_put_Clr(int16_t, int16_t, int16_t, int16_t, int16_t);
void BG_put_String(int16_t, int16_t, int16_t, int8_t *, int16_t);
void BG_put_Number10(int16_t, int16_t, int16_t, uint32_t);
void BG_Scroll(int16_t, int16_t, int16_t);
void BG_TEXT_SET(int8_t *, int16_t, int16_t, int16_t, int16_t);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void sp_dataload(void)
{
#if 0
	FILE *fp;
	int8_t sBuf[128];
#endif
	unsigned int i,j;

	/*-----------------[ �o�b�f�f�[�^�ǂݍ��� ]-----------------*/
	for(i = 0, j = 0; i < sp_list_max; i++)
	{
		int32_t	FileSize;
		
		if(GetFileLength(sp_list[i], &FileSize) == -1)
		{

		}
		else
		{
			if(FileSize != 0)
			{
				/* �������ɓo�^ */
				sp_dat_size[i] = (int32_t)File_Load(sp_list[i], &pcg_dat[j * 128], sizeof(int8_t), FileSize);
				printf("SP File %2d = %s = size(%d[byte]=%d)\n", i, sp_list[i], sp_dat_size[i], FileSize);
				j += (FileSize/128);
			}
		}
	}

#if 0
	fp = fopen( "data/sp/BALL.SP" , "rb" ) ;
//	fp = fopen( "SP_DATA/SAKANA.SP" , "rb" ) ;
	j = fread( pcg_dat
			,  128		/* 1PCG = 128byte */
			,  PCG_MAX	/* PCG�̐� */
			,  fp
	) ;
	fclose( fp ) ;
#endif
	for( i = 0; i < PCG_MAX; i++ )
	{
		_iocs_sp_defcg( i, 1, &pcg_dat[i * 128] );
	}
	
#if 0
	/*-----------[ PCG �f�[�^���c��ʗp�� 90 �x��] ]-----------*/

	for (i = 0; i < 256; i++) {
		pcg_roll90(&pcg_dat[i * 128], 1);
	}
#endif
	
	/*--------[ �X�v���C�g�p���b�g�f�[�^�ǂݍ��݂ƒ�` ]--------*/

#if 1
	for(i = 0; i < pal_list_max; i++)
	{
		int32_t	FileSize;
		
		if(GetFileLength(pal_list[i+1], &FileSize) == -1)
		{

		}
		else
		{
			if(FileSize != 0)
			{
				/* �������ɓo�^ */
				pal_dat_size[i] = (int32_t)File_Load(pal_list[i+1], &pal_dat[i*16], sizeof(int8_t), FileSize);
				printf("PAL File %2d = %s = size(%d[byte]=%d)\n", i+1, pal_list[i+1], pal_dat_size[i], FileSize);
			}
		}
	}
	/* �X�v���C�g�p���b�g�ɓ]�� */
	for( i = 0 ; i < 256 ; i++ )
	{
		_iocs_spalet( (i&15) | (1<<0x1F) , i/16 + 1, pal_dat[i] ) ;
	}
#else	
	sprintf(sBuf, "%s/sp/BALL.PAL", Get_DataList_Path());
	fp = fopen( sBuf, "rb" ) ;
//	fp = fopen( "data/sp/BALL.PAL" , "rb" ) ;
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
		_iocs_spalet( (i&15) | (1<<0x1F) , i/16 + 1, pal_dat[i] ) ;
	}
#endif

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
	uint32_t	i, j;
#if  CNF_XSP
#else   /* CNF_XSP 0 */
	uint32_t	uPCG_num;
#endif  /*CNF_XSP*/

	/* �X�v���C�g�̏����� */
	g_bSP = FALSE;
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

	//	_iocs_bgctrlgt(1);					/* BG�R���g���[�����W�X�^�ǂݍ��� */
	_iocs_bgscrlst(0,0,0);					/* BG�X�N���[�����W�X�^�ݒ� */
	_iocs_bgscrlst(1,0,0);					/* BG�X�N���[�����W�X�^�ݒ� */
	_iocs_bgtextcl(0,SetBGcode(0,0,0,0));	/* BG�e�L�X�g�N���A */
	_iocs_bgtextcl(1,SetBGcode(0,0,0,0));	/* BG�e�L�X�g�N���A */
	//	BGTEXTGT(1,1,0);					/* BG�e�L�X�g�ǂݍ��� */

#if  CNF_XSP
#else   /* CNF_XSP 0 */

	puts("SP/BG mode");
	
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
void PCG_XSP_INIT(void)
{
#if  CNF_XSP
	puts("XSP mode");
    /*---------------------[ XSP ��������s ]---------------------*/
	PCG_ON();			/* SP ������ */

	xsp_mode(2);

//	xsp_vertical(1);    /* �c��ʃ��[�h */

	/*---------------------[ XSP ��������e ]---------------------*/
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
	if(Get_SP_Sns() == FALSE)
	{
	    PCG_VIEW(1);        /* SP ON */
#if  CNF_XSP
    	xsp_on();			/* XSP ON */

		xsp_pcgmask_on(0, 0x2F);		/* 0-47 */		
		xsp_pcgmask_on(0x80, 0xFF);		/* 128-255 */

		xsp_pcgdat_set(pcg_dat, pcg_alt, sizeof(pcg_alt));  /* PCG �f�[�^�� PCG �z�u�Ǘ����e�[�u�����w�� */

#if CNF_VDISP
		xsp_vsyncint_on(App_VsyncProc);		/* �A�����Ԋ��荞�݊J�n */
#endif

#if CNF_RASTER
		xsp_hsyncint_on(App_RasterProc);	/* ���X�^���荞�݊J�n */
#endif

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
	if(Get_SP_Sns() == TRUE)
	{
		g_bSP = FALSE;
#if  CNF_XSP
#if CNF_RASTER
		xsp_hsyncint_off();		/* ���X�^���荞�݊J�n */
#endif
#if CNF_VDISP
		xsp_vsyncint_off();		/* �A�����Ԋ��荞�݊J�n */
#endif
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
int8_t Get_SP_Sns(void)
{
	return g_bSP;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void BG_ON(int16_t nNum)
{
	if(Get_SP_Sns() == TRUE)
	{
		if(nNum == 0)
		{
			_iocs_bgctrlst(0, 0, 1);	/* �a�f�O�\���n�m */
		}
		else if(nNum == 1)
		{
			_iocs_bgctrlst(1, 1, 1);	/* �a�f�P�\���n�m */
		}
	    else
		{

		}
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void BG_OFF(int16_t nNum)
{
	if(Get_SP_Sns() == TRUE)
	{
		if(nNum == 0)
		{
			_iocs_bgctrlst(0, 0, 0);	/* �a�f�O�\���n�e�e */
		}
		else if(nNum == 1)
		{
			_iocs_bgctrlst(1, 1, 0);	/* �a�f�P�\���n�e�e */
		}
	    else
		{

		}
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

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_COL_SHIFT(int16_t Col, int16_t Count)
{
	int16_t *pSP_PAL;
	int16_t i;
	int16_t temp[16];

	pSP_PAL = (int16_t *)0xE82200;	/* �X�v���C�g�̃J���[�e�[�u���O��  */

	if(Col < 16)
	{
		pSP_PAL += (Col * 16);	/* �ړI�̃p���b�g�ړ� */

		// �I���W�i���̔z����ꎞ�I�Ȕz��ɃR�s�[
		for(i = 0; i < 16; i++)
		{
			temp[i] = *(pSP_PAL + i);
		}
		// �v�f�����炷
		for(i = 0; i < 16; i++)
		{
			*(pSP_PAL + ((i + Count) % 16)) = temp[i];
		}
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_PUT_1x1(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x, y, pt, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_PUT_2x1(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x-8, y, pt+0, info);
    xsp_set( x+8, y, pt+1, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
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

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
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

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void BG_put_Clr(int16_t nPage, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t i, j;
	int16_t ax, ay;
	int16_t bx, by;

	ax = Mdiv8(x1);
	ay = Mdiv8(y1);
	bx = Mdiv8(x2);
	by = Mdiv8(y2);

	for (j = ay; j < by; j++)
	{
		for (i = ax; i < bx; i++)
		{
			_iocs_bgtextst( nPage, i, j, SetBGcode( 0, 0, 1, 0x20));		/* BG�e�L�X�g�ݒ� */
		}
    }
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void BG_put_String(int16_t x, int16_t y, int16_t pal, int8_t *sStr, int16_t nLen)
{
	int16_t i = 0;
	int16_t bx, by;

	bx = Mdiv8(x);
	by = Mdiv8(y);

	for (i = 0; i < nLen; i++)
	{
		_iocs_bgtextst( 0, bx + i, by, SetBGcode( 0, 0, pal, *(sStr + i)));		/* BG�e�L�X�g�ݒ� */
    }
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void BG_put_Number10(int16_t x, int16_t y, int16_t pal, uint32_t uNum)
{
	int16_t i, pat;
	int16_t bx, by;
	uint32_t uDigit1, uDigit2, uMod;

	bx = Mdiv8(x);
	by = Mdiv8(y);

	uDigit1 = 10;
	uDigit2 = 1;

	for(i = 0; i < 8; i++)
	{
		uMod = uNum % uDigit1;

		if((uNum / uDigit2) != 0ul)
		{
			pat = (int16_t)(uMod / uDigit2) + 40;
		}
		else
		{
			pat = 0;
		}

		_iocs_bgtextst( 0, bx + 7-i, by, SetBGcode( 0, 0, pal, pat));		/* BG�e�L�X�g�ݒ� */

		uDigit1 *= 10;
		uDigit2 *= 10;
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void BG_Scroll(int16_t nPage, int16_t x, int16_t y)
{
	_iocs_bgscrlst(0x80000000 + nPage, x, y);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void BG_TEXT_SET(int8_t *fname, int16_t nPage, int16_t nPal, int16_t nX_ofst, int16_t nY_ofst)
{
#if 0
	uint16_t	usV_pat;
#endif
	uint16_t	x_max, y_max;
	uint16_t	x,y;
	uint16_t	x_bg_ofst, y_bg_ofst;
	uint16_t	map_data[64][64];

	/* �}�b�v�f�[�^�ǂݍ��� */
	File_Load_CSV( fname, (uint16_t *)&map_data[0][0], &x_max, &y_max);

	x_bg_ofst = Mdiv8(nX_ofst);
	y_bg_ofst = Mdiv8(nY_ofst);
	x_max = Mmin((x_max + x_bg_ofst), 32);
	y_max = Mmin((y_max + y_bg_ofst), 32);

	/* ��i */
	for(y=0; y<3; y++)
	{
		for(x=0; x<x_max; x++)
		{
			_iocs_bgtextst(nPage, x + x_bg_ofst, y + y_bg_ofst, SetBGcode(0,0,nPal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
		}
	}
	/* �������璆�� */
	for(y=3; y<y_max; y++)
	{
		for(x=0; x<16; x++)
		{
			_iocs_bgtextst(nPage, x + x_bg_ofst, y + y_bg_ofst, SetBGcode(0,0,nPal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
		}
	}
	/* ��������E�� */
	for(y=3; y<y_max; y++)
	{
		for(x=16; x<x_max; x++)
		{
			_iocs_bgtextst(nPage, x + x_bg_ofst, y + y_bg_ofst, SetBGcode(0,1,nPal,map_data[y][x]));		/* BG�e�L�X�g�ݒ�(����(���E)���]) */
		}
	}
#if 0
	for(y=0; y<16; y++)
	{
		for(x=0; x<x_max; x++)
		{
			if(y < 12)		pal = 0x0E;
			else if(x > 31)	pal = 0x0F;
			else 			pal = 0x0E;
			_iocs_bgtextst(0,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
			_iocs_bgtextst(1,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
		}
	}

	usV_pat=0;
	pal = 1;
	for(y=16; y<y_max; y++)
	{
		for(x=0; x<x_max; x++)
		{
			if(x < 16 || x > 40){
				_iocs_bgtextst(0,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
				_iocs_bgtextst(1,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
			}
			else{	/* �������] */
				_iocs_bgtextst(0,x,y,SetBGcode(0,1,pal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
				_iocs_bgtextst(1,x,y,SetBGcode(0,1,pal,map_data[y][x]));		/* BG�e�L�X�g�ݒ� */
			}
		}
		
		usV_pat++;
		if(usV_pat < 12){
		}
		else{
			pal++;
			usV_pat=0;
		}
	}
#endif
}

#endif	/* IF_PCG_C */

