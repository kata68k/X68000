#ifndef	IF_GRAPHIC_C
#define	IF_GRAPHIC_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>
#include <apicglib.h>
#include "BIOS_CRTC.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"
#include "IF_Draw.h"
#include "IF_Input.h"
#include "IF_Math.h"
#include "IF_Memory.h"

/* ��ʓ��o�ꐔ */
enum
{
	Clear_G=0,
	BackGround_G,
	Object0_G,
	Object1_G,
	Object2_G,
	Object3_G,
	Object4_G,
	Object5_G,
	Object6_G,
	Object7_G,
	Enemy1_G,
	Enemy2_G,
	StartPoint_G,
	GoalPoint_G,
	MyCar_G,
	Debug_View_G,
	Flip_G,
	MAX_G
};

/* �O���t�B�b�N�p�^�[����`�� 0-15 max16 */
#define DUMMY_CG		(0)		/* �_�~�[		(1) */
#define BG_CG			(1)		/* �w�i			(1) */
#define COURSE_OBJ_CG	(2)		/* ��Q��		(kind of 4) */
#define ENEMYCAR_CG		(6)		/* ���C�o����	(kind of 8) */
#define START_PT_CG		(14)	/* �X�^�[�g�ʒu	(1) */
#define GOAL_PT_CG		(15)	/* �S�[���ʒu	(1) */
#define PAL_MAX_CG		(16)	/* �ő�16�p�^�[���o�^ */
/* �O���t�B�b�N�e�L�X�g�p�^�[����`�� max 3 */
#define MYCAR_CG		(16)	/* ����			(kind of 3) */
/* �t���O���t�B�b�N */
#define TITLE_CG		(19)	/* �^�C�g�� */
#define DEMO_CG			(20)	/* �f�� */

/* define��` */
#define	PIC_FILE_BUF_SIZE	(524288ul)	/* 512*512*2 */
#define	PIC_WORK_BUF_SIZE	(524288ul)	/* 512*512*2 */
#define	PIC_R	(3)
#define	PIC_B	(3)
#define	PIC_G	(3)
#define	COLOR_R	(0)
#define	COLOR_G	(1)
#define	COLOR_B	(2)
#define	COLOR_MAX	(PIC_R * PIC_G * PIC_B)
#define	G_COLOR		(16)
#define	G_COLOR_SP	(1)	/* ����v���C�I���e�B�Ƃ� */
#define	G_PAL_MAX	(256)	/* 256�F���[�h���̃p���b�g�� */

/* �O���[�o���ϐ� */
volatile uint16_t *VIDEO_REG1 = (uint16_t *)0xE82400;
volatile uint16_t *VIDEO_REG2 = (uint16_t *)0xE82500;
volatile uint16_t *VIDEO_REG3 = (uint16_t *)0xE82600;

uint32_t	g_CG_List_Max						=	0u;
uint8_t		*g_pCG_FileBuf[CG_MAX]				=	{NULL};
uint16_t	g_CG_ColorCode[CG_MAX][G_PAL_MAX]	=	{0};
uint8_t		g_CG_MaxColor[CG_MAX][3]			=	{0};
uint8_t		g_CG_PalTbl[PAL_MAX_CG]				=	{0};

/* �O���[�o���\���� */
PICIMAGE	g_stPicImage[CG_MAX] = {0};
CG_LIST		g_stCG_LIST[CG_MAX] = {0};
struct APICGINFO	g_CGInfo[CG_MAX] = {0};

/* �֐��̃v���g�^�C�v�錾 */
int16_t	Get_CG_FileList_MaxNum(uint32_t *);
uint8_t	*Get_CG_FileBuf(uint8_t);
uint16_t	*Get_PicImageInfo(uint8_t , uint32_t *, uint32_t *, uint32_t *);
int16_t	Get_PicImagePallet(uint8_t);
int16_t Get_PicImagePalletTmp(uint16_t *);
int16_t Set_PicImagePalletTmp(uint16_t *);
int16_t	Set_PicImagePallet(uint8_t);
int16_t Set_PicImagePalletALL(void);
int16_t CG_File_Load(uint16_t);
int16_t CG_File_Load_to_Mem(uint16_t);
int16_t CG_Mem_Convert_Type(uint16_t);
void G_INIT(void);
void G_VIDEO_INIT(void);
void G_HOME(uint8_t);
void G_Palette_INIT(void);
void G_PaletteSetZero(void);
int16_t	G_Stretch_Pict( int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t, int16_t, uint16_t, uint8_t );
int16_t G_Stretch_Pict_toVRAM(int16_t, int16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t *, uint16_t, uint16_t, int8_t, uint8_t);
int16_t G_Stretch_Pict_To_Mem( uint16_t	*, uint16_t, uint16_t, uint16_t	*, uint16_t, uint16_t);
int16_t G_Copy_Pict_To_Mem(	uint16_t *, uint16_t , uint16_t , uint16_t *, uint16_t , uint16_t);
int16_t	G_BitBlt(int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t , int16_t , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
int16_t G_BitBlt_From_Mem(	int16_t, int16_t , uint8_t , uint16_t *, uint16_t , uint16_t , uint8_t , uint8_t , uint8_t, uint8_t);
int32_t G_PAGE_SET(uint16_t);
int32_t	G_CLR(void);
int16_t G_CLR_HS(void);
int16_t	G_CLR_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t);
int16_t	G_CLR_ALL_OFFSC(uint8_t);
int16_t	G_FILL_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t, uint8_t);
int16_t	G_Load(uint8_t, uint16_t, uint16_t, uint16_t);
int16_t	G_Load_Mem(uint8_t, int16_t, int16_t, uint16_t);
int16_t	APICG_DataLoad2G(int8_t *, uint64_t, uint64_t, uint16_t);
int16_t	APICG_DataLoad2M(uint8_t , uint64_t, uint64_t, uint16_t, uint16_t *);
int16_t	G_Subtractive_Color(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t, uint32_t);
int16_t	PutGraphic_To_Text(uint8_t , uint16_t , uint16_t, uint16_t);
int16_t	PutGraphic_To_Symbol12(uint8_t *, uint16_t , uint16_t , uint16_t );
int16_t	PutGraphic_To_Symbol16(uint8_t *, uint16_t , uint16_t , uint16_t );
int16_t	PutGraphic_To_Symbol24(uint8_t *, uint16_t , uint16_t , uint16_t );
int16_t PutGraphic_To_Symbol24_Xn(uint8_t *, uint16_t, uint16_t, uint16_t, uint16_t);
int16_t	G_Scroll(int16_t, int16_t, uint8_t);
int16_t	G_ClipAREA_Chk_Width(	int16_t *, int16_t *, int16_t *, uint16_t , uint8_t , uint8_t);
int16_t	G_ClipAREA_Chk_Height(	int16_t *, int16_t *, int16_t *, uint16_t , uint8_t , uint8_t);
int16_t G_MedianCut(uint16_t *, uint16_t *, uint16_t , uint16_t , uint16_t, uint8_t, int16_t);
void G_MedianCut_Gen(YUV* , int32_t , int32_t , int32_t );
void G_APIC_to_Mem(uint16_t *, uint16_t *, uint16_t , uint16_t );
void G_Img_V_H_Cnv(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t);
void G_Mem_Cnv_Pal(uint16_t *, uint16_t , uint16_t , int16_t);
int16_t G_Pal_Set(uint8_t);
int16_t G_Pal_Get(uint8_t);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Get_CG_FileList_MaxNum(uint32_t *uMaxNum)
{
	int16_t	ret = 0;
	
	*uMaxNum = g_CG_List_Max;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint8_t *Get_CG_FileBuf(uint8_t bNum)
{
	uint8_t *ret = 0;
	
	ret = g_pCG_FileBuf[bNum];
//	printf("Get(%d,0x%p,0x%p)\n", bNum, g_pCG_FileBuf[bNum], ret );
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint16_t *Get_PicImageInfo(uint8_t bNum, uint32_t *pWidth, uint32_t *pHeight, uint32_t *pFileSize)
{
	uint16_t	*pRet = NULL;
	BITMAPFILEHEADER *pFile;
	BITMAPINFOHEADER *pInfo;
	
	if(g_CG_List_Max > bNum)
	{
		pRet = g_stPicImage[bNum].pImageData;
	}
	
	pFile = g_stPicImage[bNum].pBMf;
	*pFileSize	= pFile->bfSize;

	pInfo = g_stPicImage[bNum].pBMi;
	*pWidth		= pInfo->biWidth;
	*pHeight	= pInfo->biHeight;

#ifdef DEBUG
//	printf("st 0x%p\n", g_stPicImage );
//	printf("Get(%d,0x%p)\n", bNum, g_stPicImage[bNum].pImageData );
//	printf("Get(%d,0x%p, 0x%p)\n", bNum, g_stPicImage[bNum].pBMf, g_stPicImage[bNum].pBMi);
//	printf("Get(%d)=(x:%d,y:%d) %d\n", bNum, *pWidth, *pHeight, *pFileSize );
#endif
	
	return pRet;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Get_PicImagePallet(uint8_t bNum)
{
	int16_t	ret = 0;
	uint32_t	i;

	/* �p���b�g�̎擾 */
	for(i=0; i<G_PAL_MAX; i++)
	{
		g_CG_ColorCode[bNum][i] = GPALET( i, -1 );	/* ���݂̐ݒu��ۑ� */
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
int16_t Get_PicImagePalletTmp(uint16_t *G_Pal_Tmp)
{
	int16_t	ret = 0;
	uint32_t	i;

	/* �p���b�g�̎擾 */
	for(i=0; i<G_PAL_MAX; i++)
	{
		*G_Pal_Tmp = GPALET( i, -1 );	/* ���݂̐ݒu��ۑ� */
		G_Pal_Tmp++;
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
int16_t Set_PicImagePalletTmp(uint16_t *G_Pal_Tmp)
{
	int16_t	ret = 0;
	uint32_t	i;

	/* �p���b�g�̎擾 */
	for(i=0; i<G_PAL_MAX; i++)
	{
		GPALET( i, *G_Pal_Tmp );	/* �o�b�N�A�b�v�̏����߂� */
		G_Pal_Tmp++;
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
int16_t Set_PicImagePallet(uint8_t bNum)
{
	int16_t	ret = 0;
	uint32_t	i;
	uint32_t	offset_val;
	
	/* �p���b�g�̐ݒ� */
	switch(g_stCG_LIST[bNum].bType)
	{
		case 1:	/* �X�v���C�g���C�N */
		case 4:	/* �X�v���C�g���C�N */
		{
			/* �O���t�B�b�N�p���b�g 0�`15�́ACG_LIST�ƂP�΂P�̊֌W */
			offset_val = G_Pal_Get(bNum) * G_COLOR * G_COLOR_SP;	/* G_COLOR�F���̃I�t�Z�b�g�l���o�� */
			for(i=0; i< G_COLOR; i++)
			{
				GPALET( offset_val + i, g_CG_ColorCode[bNum][i] );
			}
			break;
		}
		case 2:	/* �e�L�X�g�`�� */
		case 3:	/* �e�L�X�g�`��(�O���C�X�P�[��) */
		{
			offset_val = 8;	/* �e�L�X�g�p���b�g8�p�^�[���ڂ���15�p�^�[���ڂ܂Ŏg���̂�(0�`7�͗\��̈�) */
			for(i=0; i< 8; i++)	/* 8�`15�Ԗڂ��g�� */
			{
				TPALET2( offset_val + i, g_CG_ColorCode[bNum][i+1] );
			}
			break;
		}
		default:
		{
			/* �S�p���b�g���g���摜 */
			offset_val = 0;
#if 0			
			for(i=0; i<G_PAL_MAX; i++)
			{
				GPALET( i, g_CG_ColorCode[bNum][i] );
			}
#else
			memcpy((int16_t *)0xE82000, &g_CG_ColorCode[bNum][0], sizeof(int16_t) * G_PAL_MAX);
#endif
			break;
		}
	}
	
	ret = offset_val;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Set_PicImagePalletALL(void)
{
	int16_t	ret = 0;
	int16_t	i = 0;
	
	for(i=0; i<CG_MAX; i++)
	{
		Set_PicImagePallet(i);
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
int16_t CG_File_Load(uint16_t uListNum)
{
	int16_t	ret = 0;
	
	uint16_t	uPalTmp[G_PAL_MAX];
	
	if(uListNum >= g_CG_List_Max)
	{
		return -1;
	}
	
	Get_PicImagePalletTmp(&uPalTmp[0]);	/* ���݂̃p���b�g��ޔ� */
#ifdef DEBUG
//	printf("Get_PicImagePalletTmp = %d\n", uListNum );
#endif

	ret = CG_File_Load_to_Mem(uListNum);		/* �t�@�C�����X�g���烁������PIC��W�J���� */
#ifdef DEBUG
//	printf("CG_File_Load_to_Mem = %d\n", uListNum );
#endif
	if(ret != 0)return ret;
	
	CG_Mem_Convert_Type(uListNum);		/* �������ɓW�J���ꂽ�摜��Type�ʂɕϊ����� */
#ifdef DEBUG
//	printf("CG_Mem_Convert_Type = %d\n", uListNum );
#endif
	
	Set_PicImagePalletTmp(&uPalTmp[0]);	/* �ޔ������p���b�g��߂� */
#ifdef DEBUG
//	printf("Set_PicImagePalletTmp = %d\n", uListNum );
#endif
	Set_PicImagePallet(uListNum);		/* �p���b�g���Z�b�g���� */
#ifdef DEBUG
//	printf("Set_PicImagePallet = %d\n", uListNum );
#endif
	//G_PaletteSetZero();					/* �O�̂��� 0�ԃp���b�g�ύX */
#ifdef DEBUG
//	printf("G_PaletteSetZero = %d\n", uListNum );
#endif
	
#ifdef DEBUG
//	printf("CG File Load ���� = %d(%d)\n", uListNum, ret);
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
int16_t CG_File_Load_to_Mem(uint16_t uListNum)
{
	int16_t	ret = 0;
	uint16_t	i;
	int32_t	FileSize;
	
	i = uListNum;
	
	if(uListNum >= g_CG_List_Max)
	{
		return -1;
	}

	{
		int32_t	Size;
		uint32_t	uSize8x = 0;
		BITMAPFILEHEADER *pFile;
		BITMAPINFOHEADER *pInfo;

		/* PIC�w�b�_�Ƀ��������蓖�� */
		g_stPicImage[i].pBMf = (BITMAPFILEHEADER*)MyMalloc( FILE_HEADER_SIZE );
		g_stPicImage[i].pBMi = (BITMAPINFOHEADER*)MyMalloc( INFO_HEADER_SIZE );
		pFile = g_stPicImage[i].pBMf;
		pInfo = g_stPicImage[i].pBMi;

		/* PIC�摜�̃t�@�C���T�C�Y���擾 */
		GetFileLength( g_stCG_LIST[i].bFileName, &FileSize );			/* �t�@�C���T�C�Y�擾 */
		if(FileSize <= 0)
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			return -1;
		}
		pFile->bfSize = FileSize;		/* �t�@�C���T�C�Y�ݒ� */

		/* PIC�摜�̏����擾 */
		GetFilePICinfo( g_stCG_LIST[i].bFileName, g_stPicImage[i].pBMi );	/* PIC�w�b�_�̓ǂݍ��� */

#ifdef DEBUG
//		printf("Head1(%d,0x%p)=%d\n", i, g_stPicImage[i].pBMf, pFile->bfSize );
//		printf("Head2(%d,0x%p)=(%d,%d)\n", i, g_stPicImage[i].pBMi, pInfo->biWidth, pInfo->biHeight );
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		/* �������W�J�p�̃T�C�Y���Z */
		uSize8x = ((((pInfo->biWidth)+7)/8) * 8);	/* 8�̔{�� */
		Size = (pInfo->biHeight) * uSize8x * sizeof(uint16_t);
#ifdef DEBUG
//		printf("Head3(%d)=(%d)\n", i, Size );
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		if( g_stPicImage[i].pImageData != NULL )
		{
			return -1;	/* ���Ɋm�ۍς� */
		}
		/* �������m�� */
		g_stPicImage[i].pImageData = (uint16_t*)MyMalloc( Size );	/* �������̊m�� */
		if( g_stPicImage[i].pImageData == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			return -1;
		}
		memset(g_stPicImage[i].pImageData, 0xA5, Size);	/* �������N���A */
#ifdef DEBUG
//		printf("Mem(%d,0x%p,0x%p,%d)\n", i, g_stPicImage[i].pImageData, g_stPicImage[i].pImageData + (Size/sizeof(uint16_t)), Size);
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		
		/* �t�@�C�����������ɕۑ����� */
		g_pCG_FileBuf[i] = NULL;
		g_pCG_FileBuf[i] = (uint8_t*)MyMalloc( FileSize );	/* �������m�� */
		if( g_pCG_FileBuf[i] == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			return -1;
		}
		File_Load( g_stCG_LIST[i].bFileName, (uint8_t *)g_pCG_FileBuf[i], sizeof(uint8_t), FileSize);	/* �������ɓǂݍ��� */
#ifdef DEBUG
//		printf("File(%d,0x%p) = %d\n", i, g_pCG_FileBuf[i], FileSize);
//		puts("========================");
//		G_Load(i,0,0,0);
//		if(i==0)
//		{
//			KeyHitESC();	/* �f�o�b�O�p */
//		}
#endif
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
int16_t CG_Mem_Convert_Type(uint16_t uListNum)
{
	int16_t	ret = 0;
	uint32_t	i;
	
	i = uListNum;
	
	if(uListNum >= g_CG_List_Max)
	{
		return -1;
	}
	
	{
		uint16_t	*pSrcBuf = NULL;
		uint16_t	*pDstBuf = NULL;
		uint32_t	uWidth, uHeight, uFileSize;
		uint32_t	uSize8x = 0;
		int32_t		Res;
		size_t		Size;
		uint8_t		ubGpal = 0u;

		uint16_t *work_buf = NULL;

		pSrcBuf = g_stPicImage[i].pImageData;	/* Src PIC */
		if(pSrcBuf == NULL)
		{
			/* �t�@�C������ǂݍ��� */
			CG_File_Load_to_Mem(i);						/* �t�@�C�����������ɓǂݍ��݁��C���[�W�o�b�t�@�m�� */
		}		
		pSrcBuf = Get_PicImageInfo( i, &uWidth, &uHeight, &uFileSize );	/* �摜�̏����擾 */
		uSize8x = Mmul8((Mdiv8(uWidth+7)));	/* 8�̔{�� */
#ifdef DEBUG
//		printf("Load1(%d,0x%p)=(%d,%d)(%d)\n", i, g_stPicImage[i].pBMi, uWidth, uHeight, uSize8x );
//		printf("Load2(%d,0x%p)\n", i, pSrcBuf );
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		work_buf = (uint16_t*)MyMalloc( sizeof(uint16_t) * 512 * 512 );	/* ��ʂƓ��� */

		/* ���������PIC�摜��W�J���� */
		Res = APICG_DataLoad2M( i, 0, 0, 0, work_buf);	/* �m�ۂ�����������Ƀ��[�h */

		G_APIC_to_Mem(work_buf, pSrcBuf, uSize8x, uHeight);	/* APIC�摜���g���₷��BITMAP���̃������ɓW�J */

		/* ���������� */
		if(work_buf != NULL)
		{
			MyMfree(work_buf);	/* ��������� */
		}

#ifdef DEBUG
//		printf("Load3(%d,0x%p)=%d\n", i, pSrcBuf, Res);
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		if( Res < 0 )	/* �W�J���s */
		{
			printf("error(%d):CG File%2d=%s\n", Res, i, g_stCG_LIST[i].bFileName );
#ifdef DEBUG
//			KeyHitESC();	/* �f�o�b�O�p */
#endif
			return -1;
		}
		
#if 0	/* ��ʂɕ\�� */
		{
			uint16_t	*pDstGR = NULL;
			uint32_t	dx = 0, dy = 0;

//			memset((uint16_t*)0xC00000, 0x42, 0x80000);	/* �f�o�b�O�p */
			
			for(y=0; y < uHeight; y++)
			{
				pDstGR = (uint16_t *)(0xC00000 + ((dy + y) << 10) + (dx << 1));
				pBuf = pSrcBuf + (y << 9);
				
				for(x=0; x < uWidth; x++)
				{
					*pDstGR = *pBuf & 0x00FF;
					pDstGR++;
					pBuf++;
				}
			}
		}
		pBuf = pSrcBuf;
#endif
		
		/* �������ݐ�̃������`�F�b�N */
		if( g_stPicImage[i].pImageData == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			return -1;
		}
		/* ��Ɨp�|�C���^ */
		pDstBuf = g_stPicImage[i].pImageData; 
#ifdef DEBUG
//		printf("Load5(%d,0x%p, 0x%p)\n", i, g_stPicImage[i].pImageData, pDstBuf);
//		KeyHitESC();	/* �f�o�b�O�p */
#endif

#if 0
		/* ��Ɨp�|�C���^ */
		pDstBuf = g_stPicImage[i].pImageData; 
		
		G_APIC_to_Mem(pSrcBuf, pDstBuf, uWidth, uHeight);	/* APIC�摜���g���₷��BITMAP���̃������ɓW�J */
		
		/* ���������� */
		if(pSrcBuf != NULL)
		{
			MyMfree(pSrcBuf);	/* ��������� */
		}
		pSrcBuf = (uint16_t*)MyMalloc( uSize8x * uHeight * sizeof(uint16_t));	/* �������̊m�� */
#endif
		
#ifdef DEBUG
//		printf("debug2(0x%p)=(%d,%d)\n", pDstBuf, uWidth, uHeight );
//		printf("debug3(0x%p)=(%d,%d)\n", pSrcBuf, uWidth, uHeight );
//		KeyHitESC();	/* �f�o�b�O�p */
#endif

#if 0
		G_BitBlt_From_Mem(0, 0, 0, pDstBuf, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);
		G_MedianCut(pSrcBuf, pDstBuf, uWidth, uHeight, 16);
		G_BitBlt_From_Mem(0, 66, 0, pSrcBuf, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);
#endif
		switch(g_stCG_LIST[i].bType)
		{
			case 1:	/* �X�v���C�g���C�N */
			{
				uint8_t str[256] = {0};
				memset(str, 0, sizeof(uint8_t) * 256);
				sprintf(str, "data\\cg\\OK_tmp_G%02d.G16", i );
				
				G_Pal_Set(i);	/* �Ώۉ摜�̃p���b�g��ݒ� */
				Size = (size_t)uSize8x * uHeight;
				if(Size != File_Load( str, g_stPicImage[i].pImageData, sizeof(uint16_t), Size))	/* �t�@�C�����Ȃ��ꍇ */
				{
					printf("%s���쐬���܂��c3�����X���҂���������(%d)\n", str, Size);
//					printf("%s��(%d=%d)\n", str, Size, Res);
					
					G_MedianCut(pSrcBuf, pDstBuf, uWidth, uHeight, 16, i, g_stCG_LIST[i].TransPal);	/* 256��16�F ���F���� */
					File_Save( str, g_stPicImage[i].pImageData, sizeof(uint16_t), uSize8x * uHeight );

					ubGpal = 1u;	/* �p���b�g�ۑ��t���O�Z�b�g */
				}
				else	/* �t�@�C��������ꍇ */
				{
					ubGpal = 2u;	/* �p���b�g�ۑ��t���O�Z�b�g */
				}
				
				break;
			}
			case 2:	/* �e�L�X�g�`�� */
			case 3:	/* �e�L�X�g�`��(�O���C�X�P�[��) */
			{
				G_Subtractive_Color(pSrcBuf, pDstBuf, uWidth, uHeight, uSize8x, i);	/* ���F���� */
//				G_APIC_to_Mem(pSrcBuf, pDstBuf, uWidth, uHeight);	/* APIC�摜���g���₷��BITMAP���̃������ɓW�J */
//				G_MedianCut(pDstBuf, pDstBuf, uWidth, uHeight, 4);	/* 256��4�F ���F���� */
#ifdef DEBUG
//				printf("debug4(0x%p)=(%d,%d)\n", pDstBuf, uWidth, uHeight );
//				printf("debug5(0x%p)=(%d,%d)\n", pSrcBuf, uWidth, uHeight );
//				KeyHitESC();	/* �f�o�b�O�p */
#endif
				break;
			}
			case 4:	/* �X�v���C�g���C�N(��) */
			{
				uint8_t str[256] = {0};
				memset(str, 0, sizeof(uint8_t) * 256);
				sprintf(str, "data\\cg\\OK_tmp_G%02d.G16", i );
				
				G_Pal_Set(i);	/* �Ώۉ摜�̃p���b�g��ݒ� */
				
				Size = (size_t)uSize8x * uHeight;
				if(Size != File_Load( str, g_stPicImage[i].pImageData, sizeof(uint16_t), Size))	/* �t�@�C�����Ȃ��ꍇ */
				{
					work_buf = (uint16_t*)MyMalloc( sizeof(uint16_t) * uSize8x * uHeight );	/* ��ʂƓ��� */
					
					printf("%s���쐬���܂��c3�����X���҂���������(%d)\n", str, Size);
//					printf("%s��(%d=%d)\n", str, Size, Res);
					
					G_MedianCut(pSrcBuf, pDstBuf, uWidth, uHeight, 16, i, g_stCG_LIST[i].TransPal);	/* 256��16�F ���F���� */
					G_Mem_Cnv_Pal(pDstBuf, uWidth, uHeight, i);	/* ��������V�����p���b�g�ɃR���o�[�g���� */
					memcpy(work_buf, pDstBuf, sizeof(uint16_t) * uSize8x * uHeight );
					G_Img_V_H_Cnv(work_buf, pDstBuf, uWidth, uHeight, 32);	/* ���X�N���[���p�ɉ摜���R���o�[�g */
					File_Save( str, g_stPicImage[i].pImageData, sizeof(uint16_t), uSize8x * uHeight );

					/* ���������� */
					if(work_buf != NULL)
					{
						MyMfree(work_buf);	/* ��������� */
					}
					ubGpal = 1u;	/* �p���b�g�ۑ��t���O�Z�b�g */
				}
				else	/* �t�@�C��������ꍇ */
				{
					ubGpal = 2u;	/* �p���b�g�ۑ��t���O�Z�b�g */
				}
				
				break;
			}
			default:	/* �ʏ��256�FCG */	
			{
				break;
			}
		}
		
#ifdef DEBUG
//		printf("Load6(%d,%d,0x%p, 0x%p)\n", i, g_stCG_LIST[i].ubType, g_stPicImage[i].pImageData, pSrcBuf);
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		/* �p���b�g */
		switch(ubGpal)
		{
			case 0:
			default:
			{
		//		G_PaletteSetZero();	/* 0�ԃp���b�g�ύX */
				Get_PicImagePallet(i);	/* �p���b�g��ۑ� */
				break;
			}
			case 1:	/* �X�v���C�g���C�N(�t�@�C���Ȃ�) */
			{
				uint8_t str[256] = {0};
				sprintf(str, "data\\cg\\OK_tmp_G%02d.P16", i );
				
				Get_PicImagePallet(i);	/* �p���b�g��ۑ� */
				
				File_Save( str, &g_CG_ColorCode[i][0], sizeof(uint16_t), 16 );	/* �p���b�g�Z�[�u */
				break;
			}
			case 2:	/* �X�v���C�g���C�N(�t�@�C������) */
			{
				uint8_t str[256] = {0};
				sprintf(str, "data\\cg\\OK_tmp_G%02d.P16", i );
				
				File_Load( str, &g_CG_ColorCode[i][0], sizeof(uint16_t), 16 );	/* �p���b�g���[�h */
				break;
			}
		}
#ifdef DEBUG
//		printf("CG File %2d = %s(x:%d,y:%d)\n", i, g_stCG_LIST[i].bFileName, uWidth, uHeight );
//		puts("========================");
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
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
void G_INIT(void)
{
	int32_t	ret;
	int16_t	i;
	
	G_CLR();	/* �O���t�B�b�N�N���A */

	ret = _iocs_vpage(0b0011);	/* �O���t�B�b�N�\��(page3:0n page2:0n page1:0n page0:0n) */
	if(ret < 0)printf("error(%d):_iocs_vpage\n", ret);
	
	for(i=0; i < PAL_MAX_CG; i++)
	{
		g_CG_PalTbl[i] = 0xFF;
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void G_VIDEO_INIT(void)
{
//											   210
//	*VIDEO_REG1 = Mbset(*VIDEO_REG1,   0x07, 0b001);	/* 512x512 256color 2men */
//											   ||+--------------bit0 
//											   |+---------------bit1 
//											   |						00.    16 color 4 screen
//											   |						01.   256 color 2 screen
//											   |						11. 65535 color 1 screen
//											   +----------------bit2 
//																		0	VR.  512 x 512
//																		1	VR. 1024 x1024

//											   FEDCBA9876543210
	*VIDEO_REG2 = Mbset(*VIDEO_REG2, 0x3FFF, 0b0000011011100100);	/* �D�揇�� SP>TX>GR GR0>GR1>GR2>GR3 */
//											   |||||||||||||||+-bit0 0	
//											   ||||||||||||||+--bit1 0	
//											   |||||||||||||+---bit2 1	
//											   ||||||||||||+----bit3 0	
//											   |||||||||||+-----bit4 0	
//											   ||||||||||+------bit5 1	
//											   |||||||||+-------bit6 1	
//											   ||||||||+--------bit7 1	
//											   ||||||||					�D�揇�� GR0>GR1>GR2>GR3
//											   |||||||+---------bit8 GR	0
//											   ||||||+----------bit9 GR	1
//											   |||||+-----------bitA TX	1
//											   ||||+------------bitB TX	0
//											   |||+-------------bitC SP	0
//											   ||+--------------bitD SP	0
//											   ||						�D�揇�� SP>TX>GR
//											   |+---------------bitE (Reserve)
//											   +----------------bitF (Reserve)

//											   FEDCBA9876543210
//	*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x3F, 0b0001010011101111);	/* ���ꃂ�[�h���� ���z���512x512 */
	*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x3F, 0b0000000001101111);	/* ���ꃂ�[�h�Ȃ� ���z���512x512 */
//											   |||||||||||||||+-bit0 GS0	512x512 Pri0 <0:OFF 1:ON>
//											   ||||||||||||||+--bit1 GS1	512x512 Pri1 <0:OFF 1:ON>
//											   |||||||||||||+---bit2 GS2	512x512 Pri2 <0:OFF 1:ON>
//											   ||||||||||||+----bit3 GS3	512x512 Pri3 <0:OFF 1:ON>
//											   |||||||||||+-----bit4 GS4	1024x1024 <0:OFF 1:ON>
//											   ||||||||||+------bit5 TON	TEXT <0:OFF 1:ON>
//											   |||||||||+-------bit6 SON	SP <0:OFF 1:ON>
//											   ||||||||+--------bit7 (Reserve)
//											   |||||||+---------bit8 G/T	
//											   ||||||+----------bit9 G/G	
//											   |||||+-----------bitA B/P	1
//											   ||||+------------bitB H/P	
//											   |||+-------------bitC EXON	1
//											   ||+--------------bitD VHT	
//											   |+---------------bitE AH		
//											   +----------------bitF Ys		
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void G_HOME(uint8_t bMode)
{
	int16_t	x, y;
	ST_CRT	stCRT = {0};

	GetCRT(&stCRT, bMode);
	switch(bMode)
	{
		case 0:	/* �\�����W */
		default:/* �\�����W */
		{
			x = stCRT.view_offset_x;
			y = stCRT.view_offset_y;
			_iocs_home( 0b0001, x, y );	/* �z�[���ʒu�ݒ� */
			_iocs_window( x, y, x + WIDTH, y + HEIGHT);	/* �N���b�s���O */
			break;
		}
		case 1:	/* �I�t�X�N���[�����W */
		case 2:
		{
			x = stCRT.view_offset_x;
			y = stCRT.view_offset_y;
			_iocs_home( 0b0001, x, y );	/* �z�[���ʒu�ݒ� */
//			_iocs_window( x, y, x + WIDTH, y + HEIGHT);	/* �N���b�s���O */
			_iocs_window( x, 0, x + WIDTH, 511);	/* �N���b�s���O */
			x = stCRT.hide_offset_x;
			y = stCRT.hide_offset_y;
			break;
		}
	}
//	_iocs_home( 0b0001, x, y );	/* �z�[���ʒu�ݒ� */
//	_iocs_home(0b0000, X_OFFSET, Y_OFFSET);	/* �z�[���ʒu */
//	_iocs_home(1, X_OFFSET, Y_OFFSET);
//	_iocs_home(2, X_OFFSET, 416);
//	_iocs_home(3, X_OFFSET, 416);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void G_Palette_INIT(void)
{
	uint32_t	nPalette;

	/* �O���t�B�b�N�p���b�g�̏����� */
	for(nPalette=0; nPalette < 0xFF; nPalette++)
	{
		GPALET( nPalette, SetRGB(0, 0, 0));	/* Black */
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void G_PaletteSetZero(void)
{
	int16_t i=0;
	
	/* ���ߐF */
	GPALET( i++, SetRGB( 0,  0,  0));	/* 0:Black */
	/* �P�`�P�T�����ʃJ���[�Ƃ��� */
	GPALET( i++, SetRGB(  1,  1,  1));	/* 1 */
	GPALET( i++, SetRGB( 15, 15, 15));	/* 2 */
	GPALET( i++, SetRGB( 31, 31, 31));	/* 3 */
	GPALET( i++, SetRGB( 15,  0,  0));	/* 4 */
	GPALET( i++, SetRGB(  0, 15,  0));	/* 5 */
	GPALET( i++, SetRGB( 15, 15,  0));	/* 6 */
	GPALET( i++, SetRGB(  0,  0, 15));	/* 7 */
	GPALET( i++, SetRGB( 15,  0, 15));	/* 8 */
	GPALET( i++, SetRGB(  0, 15, 15));	/* 9 */
	GPALET( i++, SetRGB( 31,  0,  0));	/* A */
	GPALET( i++, SetRGB(  0, 31,  0));	/* B */
	GPALET( i++, SetRGB( 31, 31,  0));	/* C */
	GPALET( i++, SetRGB(  0,  0, 31));	/* D */
	GPALET( i++, SetRGB( 31,  0, 31));	/* E */
	GPALET( i++, SetRGB(  0, 31, 31));	/* F */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* VRAM����VRAM�ɓW�J */
int16_t G_Stretch_Pict(
			int16_t dst_x, uint16_t dst_w, int16_t dst_y, uint16_t dst_h, uint8_t ubDstScrn,
			int16_t src_x, uint16_t src_w, int16_t src_y, uint16_t src_h, uint8_t ubSrcScrn)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint16_t	*pSrcGR_tmp;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_ex,	src_ey;
	int16_t	x, y;

	dst_ex	= dst_x + dst_w;
	if(dst_ex >= X_MAX_DRAW)
	{
		dst_ex = X_MAX_DRAW;
	}
	dst_ey	= dst_y + dst_h;
	if(dst_ey >= Y_MAX_DRAW)
	{
		dst_ey = Y_MAX_DRAW;
	}
	src_ex	= src_x + src_w;
	src_ey	= src_y + src_h;

	/* �A�h���X�Z�o */
	if(ubDstScrn != 0)
	{
		DstGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		DstGR_H = 0xC00000;	/* Screen0 */
	}

	if(ubSrcScrn != 0)
	{
		SrcGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		SrcGR_H = 0xC00000;	/* Screen0 */
	}
#if 1
	for(y = dst_y; y < dst_ey; y++)
	{
		/* �A�h���X�Z�o */
		pDstGR = (uint16_t *)(DstGR_H + (Mmul1024(y) + Mmul2(dst_x)));

		pSrcGR = (uint16_t *)(SrcGR_H + (Mmul1024((src_y + Mmul_1p25(y - dst_y))) + Mmul2(src_x)));
		pSrcGR_tmp = pSrcGR;
	
		for(x = dst_x; x < dst_ex; x++)
		{
			pSrcGR = pSrcGR_tmp + Mmul_1p25(x-dst_x);
			if(*pSrcGR != 0)
			{
				*pDstGR = *pSrcGR;
			}
			pDstGR++;
		}
	}
#else

#endif
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* ���������璼��VRAM�ɓW�J */
int16_t G_Stretch_Pict_toVRAM(	int16_t dst_x, int16_t dst_y, uint8_t ubDstScrn,
								uint8_t ubMode, uint8_t ubH, uint8_t ubV,
								uint16_t *pSrcBuf, uint16_t src_w, uint16_t src_h, int8_t bZoom, uint8_t bCGNum)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint16_t	*pSrcGR_tmp;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t dst_w, dst_h;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_x,	src_y;
	int16_t	src_ex,	src_ey;
	int16_t	x_min,	x_max;
	int16_t	y_min,	y_max;
	int16_t	x, y;
	int16_t	cal_x, cal_y;
	int16_t	dst_z;
	int16_t	src_z;
	int16_t	Pal_offset;
	int16_t	ConvPal;
	int8_t	bType;

	int16_t (*pFunc[])(int16_t) = {
		APL_mul_0p52,	/* 0 -9 */
		APL_mul_0p54,	/* 1 -8 */
		APL_mul_0p58,	/* 2 -7 */
		APL_mul_0p62,	/* 3 -6 */
		APL_mul_0p66,	/* 4 -5 */
		APL_mul_0p71,	/* 5 -4 */
		APL_mul_0p77,	/* 6 -3 */
		APL_mul_0p83,	/* 7 -2 */
		APL_mul_0p91,	/* 8 -1 */
		APL_mul_1p00,	/* 9  0 */
		APL_mul_1p10,	/* 10 1 */
		APL_mul_1p20,	/* 11 2 */
		APL_mul_1p30,	/* 12 3 */
		APL_mul_1p40,	/* 13 4 */
		APL_mul_1p50,	/* 14 5 */
		APL_mul_1p60,	/* 15 6 */
		APL_mul_1p70,	/* 16 7 */
		APL_mul_1p80,	/* 17 8 */
		APL_mul_1p90	/* 18 9 */
	};

	if(pSrcBuf == NULL)	return -1;
	
	if(bZoom < -9) bZoom = -9;
	if(bZoom > 9) bZoom = 9;
	dst_z = bZoom + 9;
	src_z = 9 - bZoom;
	
	dst_w = (*pFunc[dst_z])(src_w);
	dst_w = (((dst_w+7)/8) * 8);	/* 8�̔{�� */
	dst_h = (*pFunc[dst_z])(src_h);
	
	/* �`�悵�����G���A���犮�S�ɂ͂ݏo�Ă邩�ǂ����`�F�b�N */
	ret = G_ClipAREA_Chk_Width(&dst_x, &x_min, &x_max, dst_w, ubMode, ubH);
	if(ret != 0)return ret;
	
	ret = G_ClipAREA_Chk_Height(&dst_y, &y_min, &y_max, dst_h, ubMode, ubV);
	if(ret != 0)return ret;
	
	dst_ex	= dst_x + dst_w;
	dst_ey	= dst_y + dst_h;
	
	src_x = 0;
	src_y = 0;
	src_w = (((src_w+7)/8) * 8);	/* 8�̔{�� */
	src_ex	= src_x + src_w;
	src_ey	= src_y + src_h;
	
	/* �A�h���X�Z�o */
	if(ubDstScrn != 0)
	{
		DstGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		DstGR_H = 0xC00000;	/* Screen0 */
	}
	
	SrcGR_H = (uint32_t)pSrcBuf;
	Pal_offset = Set_PicImagePallet(bCGNum);	/* �p���b�g��ݒ� */
	ConvPal = g_stCG_LIST[bCGNum].TransPal;	
	bType   = g_stCG_LIST[bCGNum].bType;

#ifdef DEBUG
//	printf("d(%d, %d)(%d, %d)(%d, %d)\n", dst_x, dst_y, dst_w, dst_h, dst_ex, dst_ey);
//	printf("s(%d, %d)(%d, %d)(%d, %d)\n", src_x, src_y, src_w, src_h, src_ex, src_ey);
//	KeyHitESC();	/* �f�o�b�O�p */
#endif
	
#if 1
	for(y = dst_y; y < dst_ey; y++)
	{
		if(y < y_min)continue;
		if(y >= y_max)break;
		
		/* �A�h���X�Z�o */
		pDstGR = (uint16_t *)(DstGR_H + Mmul2(Mmul512(y) + dst_x));

		cal_y = src_y + (*pFunc[src_z])(y - dst_y);
		if(cal_y >= src_ey)break;
		
		pSrcGR = (uint16_t *)(SrcGR_H + Mmul2(cal_y * src_w)) ;
		pSrcGR_tmp = pSrcGR;
	
		for(x = dst_x; x < dst_ex; x++)
		{
			if(x < x_min)
			{
				pDstGR++;
				continue;
			}
			if(x >= x_max)break;
			
			cal_x = (*pFunc[src_z])(x-dst_x);
			if(cal_x >= src_ex)break;
			
			pSrcGR = pSrcGR_tmp + cal_x;
			if((*pSrcGR != 0) || (ConvPal == -1))	/* ���ߐF�ȊO */
			{
				*pDstGR = *pSrcGR + Pal_offset;
			}
			pDstGR++;
		}
	}
#else

#endif
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* ���������烁�����ɓW�J */
int16_t G_Stretch_Pict_To_Mem(	uint16_t *pDstBuf, uint16_t dst_w, uint16_t dst_h,
								uint16_t *pSrcBuf, uint16_t src_w, uint16_t src_h)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint16_t	*pSrcGR_tmp;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	x, y;
	int16_t	sx, sy;
	uint32_t	uSize8x_dst = 0;
	uint32_t	uSize8x_src = 0;
	
	if(pDstBuf == NULL)return -1;
	if(pSrcBuf == NULL)return -1;
	if(src_w == 0u)return -1;
	if(dst_w == 0u)return -1;
	if(dst_h == 0u)return -1;
	if(src_h == 0u)return -1;

	DstGR_H = (uint32_t)pDstBuf;
	SrcGR_H = (uint32_t)pSrcBuf;

	uSize8x_dst = (((dst_w+7)/8) * 8);	/* 8�̔{�� */
	uSize8x_src = (((src_w+7)/8) * 8);	/* 8�̔{�� */

	for(y = 0; y < dst_h; y++)
	{
		sy = Mmul_1p25(y);
		if(sy >= src_h)
		{
			break;
		}
		/* �A�h���X�Z�o */
		pDstGR = (uint16_t *)( DstGR_H + Mmul2( y * uSize8x_dst) );
		pSrcGR = (uint16_t *)( SrcGR_H + Mmul2(sy * uSize8x_src) );
		pSrcGR_tmp = pSrcGR;
	
		for(x = 0; x < dst_w; x++)
		{
			sx = Mmul_1p25(x);
			if(sx < src_w)
			{
				pSrcGR = pSrcGR_tmp + sx;
				if(*pSrcGR != 0)
				{
					*pDstGR = *pSrcGR;
				}
			}
			pDstGR++;
		}
	}
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* ���������烁�����ɓW�J */
int16_t G_Copy_Pict_To_Mem(	uint16_t *pDstBuf, uint16_t dst_w, uint16_t dst_h,
							uint16_t *pSrcBuf, uint16_t src_w, uint16_t src_h)
{
	int16_t	ret = 0;
	int16_t	y;
	uint16_t	*pDstGR,	*pSrcGR;
	uint32_t	uSize8x_dst = 0;
	
	if(pDstBuf == NULL)return -1;
	if(pSrcBuf == NULL)return -1;
	if(src_w == 0u)return -1;
	if(dst_w == 0u)return -1;
	if(dst_h == 0u)return -1;
	if(src_h == 0u)return -1;
	if(src_w != dst_w)return -1;
	if(src_h != dst_h)return -1;

	uSize8x_dst = (((dst_w+7)/8) * 8) * dst_h;	/* 8�̔{�� */

	/* �A�h���X�Z�o */
	pDstGR = pDstBuf;
	pSrcGR = pSrcBuf;
	
	for(y = 0; y < uSize8x_dst; y++)
	{
		/* �A�h���X�Z�o */
		*pDstGR = *pSrcGR;
		pDstGR++;
		pSrcGR++;
	}
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* �摜�̃R�s�[ */
/* �����T�C�Y�̉摜��C�ӂ̈ʒu�ɕ`�悷�� */
/* �\�����x,y���W���摜�̒��S�Ƃ����ʒu */
int16_t G_BitBlt(int16_t dst_x, uint16_t dst_w, int16_t dst_y, uint16_t dst_h, uint8_t ubDstScrn,
			int16_t src_x, uint16_t src_w, int16_t src_y, uint16_t src_h, uint8_t ubSrcScrn,
			uint8_t ubMode, uint8_t ubH, uint8_t ubV)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_ex,	src_ey;
	int16_t	x, y;
	int16_t	x_min, y_min;
	int16_t	x_max, y_max;

	if(dst_x < X_MIN_DRAW)return -1;
	if(dst_x >= X_MAX_DRAW)return -1;
	if(dst_y < Y_MIN_DRAW)return -1;
	if(dst_y >= Y_MAX_DRAW)return -1;
	
	switch(ubV)
	{
		case POS_TOP:
		{
			break;
		}
		case POS_CENTER:
		{
			dst_y	= dst_y - (dst_h>>1);
			break;
		}
		case POS_BOTTOM:
		{
			dst_y	= dst_y - dst_h;
			break;
		}
		default:
		{
			break;
		}
	}

	switch(ubH)
	{
		case POS_LEFT:
		{
			break;
		}
		case POS_MID:
		{
			dst_x	= dst_x - (dst_w>>1);
			break;
		}
		case POS_RIGHT:
		{
			dst_x	= dst_x - dst_w;
			break;
		}
		default:
		{
			break;
		}
	}
	
	dst_ex	= dst_x + dst_w;
	dst_ey	= dst_y + dst_h;
	src_ex	= src_x + src_w;
	src_ey	= src_y + src_h;

	/* �{�����Z */
	if(src_w != dst_w)return -1;
	if(src_h != dst_h)return -1;
	
	/* �A�h���X�Z�o */
	if(ubDstScrn != 0)
	{
		DstGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		DstGR_H = 0xC00000;	/* Screen0 */
	}

	if(ubSrcScrn != 0)
	{
		SrcGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		SrcGR_H = 0xC00000;	/* Screen0 */
	}
	
	/* �\���G���A���N���b�v */
	switch(ubMode)
	{
		case 0:
		{
			x_min = X_MIN_DRAW;
			x_max = X_MAX_DRAW;
			y_min = Y_MIN_DRAW;
			y_max = Y_MAX_DRAW;
			break;
		}
		case 1:
		{
			x_min = X_MIN_DRAW + X_OFFSET;
			x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			y_min = Y_MIN_DRAW;
			y_max = Y_MIN_DRAW + Y_MAX_WINDOW;
			break;
		}
		case 2:
		{
			x_min = X_MIN_DRAW + X_OFFSET;
			x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			y_min = Y_MIN_DRAW + Y_OFFSET;
			y_max = Y_MIN_DRAW + Y_OFFSET + Y_MAX_WINDOW;
			break;
		}
		default:
		{
			x_min = X_MIN_DRAW;
			x_max = X_MAX_DRAW;
			y_min = Y_MIN_DRAW;
			y_max = Y_MAX_DRAW;
			break;
		}
	}

	if(dst_ex < x_min)		/* �摜�̉E������ʊO */
	{
		return -1;
	}
	else if((dst_x <= x_min) && (dst_ex >= x_min))		/* �摜�̍�������ʊO�����A�摜�̉E���͉�ʓ� */
	{
		src_x += x_min - dst_x;
		dst_x = x_min;
	}
	else if((dst_x < x_max) && (dst_ex >= x_max))		/* �摜�̍�������ʓ������A�摜�̉E���͉�ʊO */
	{
		src_ex -= dst_ex - x_max;
		dst_ex = x_max;
	}
	else if(dst_x >= x_max)	/*  �摜�̍�������ʊO */
	{
		return -1;
	}
	else						/* �͈͓� */
	{
		/* �������Ȃ� */
	}
	
	if(dst_ey < y_min)		/* ��ʊO */
	{
		return -1;
	}
	else if((dst_y <= y_min) && (dst_ey >= y_min))
	{
		src_y += y_min - dst_y;
		dst_y = y_min;
	}
	else if((dst_y < y_max) && (dst_ey >= y_max))
	{
		src_ey -= dst_ey - y_max;
		dst_ey = y_max;
	}
	else if(dst_y > y_max)	/* ��ʊO */
	{
		return -1;
	}
	else						/* �͈͓� */
	{
		/* �������Ȃ� */
	}

	for(y = dst_y; y < dst_ey; y++)
	{
		/* �A�h���X�Z�o */
		pDstGR = (uint16_t *)(DstGR_H + ((y << 10) + (dst_x << 1)));

		pSrcGR = (uint16_t *)(SrcGR_H + (((src_y + (y - dst_y)) << 10) + (src_x << 1)));
	
		for(x = dst_x; x < dst_ex; x++)
		{
			if(*pSrcGR != 0)
			{
				*pDstGR = *pSrcGR;
			}
			pDstGR++;
			pSrcGR++;
		}
	}
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* ����������摜�� */
/* �����T�C�Y�̉摜��C�ӂ̈ʒu�ɕ`�悷�� */
/* �\�����x,y���W���摜�̒��S�Ƃ����ʒu */
int16_t G_BitBlt_From_Mem(	int16_t dst_x, int16_t dst_y, uint8_t ubDstScrn,
							uint16_t *pSrcImage, uint16_t src_w, uint16_t src_h,
							uint8_t ubMode, uint8_t ubH, uint8_t ubV, uint8_t bCGNum)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_ex,	src_ey;
	int16_t	x, y;
	int16_t	x_min, y_min;
	int16_t	x_max, y_max;
	int16_t	Pal_offset;
	int16_t	ConvPal;
	int8_t	bType;
	
	/* �`�悵�����G���A���犮�S�ɂ͂ݏo�Ă邩�ǂ����`�F�b�N */
	ret = G_ClipAREA_Chk_Width(&dst_x, &x_min, &x_max, src_w, ubMode, ubH);
	if(ret != 0)
	{
//		puts("G_BitBlt_From_Mem w");
		return ret;
	}
	ret = G_ClipAREA_Chk_Height(&dst_y, &y_min, &y_max, src_h, ubMode, ubV);
	if(ret != 0)
	{
//		puts("G_BitBlt_From_Mem h");
		return ret;
	}

	dst_ex	= dst_x + src_w;
	dst_ey	= dst_y + src_h;
	src_ex	= (((src_w+7)/8) * 8);	/* 8�̔{�� */
	src_ey	= src_h;
	
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	Draw_Box( dst_x, dst_y, dst_ex, dst_ey, 8, 0xFFFF);
#endif

	/* �A�h���X�Z�o */
	if(ubDstScrn != 0)
	{
		DstGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		DstGR_H = 0xC00000;	/* Screen0 */
	}

	SrcGR_H = (uint32_t)pSrcImage;	/* Mem */
	
	Pal_offset = Set_PicImagePallet(bCGNum);	/* �p���b�g��ݒ� */
	ConvPal = g_stCG_LIST[bCGNum].TransPal;	
	bType   = g_stCG_LIST[bCGNum].bType;
	
	for(y = dst_y; y < dst_ey; y++)
	{
		if(y < y_min)continue;	/* �`��G���A�O�i��j */
		if(y >= y_max)break;	/* �`��G���A�O�i���j */
		
		/* �A�h���X�Z�o */
		pDstGR = (uint16_t *)(DstGR_H + Mmul2(Mmul512(y) + dst_x));
		pSrcGR = (uint16_t *)(SrcGR_H + Mmul2((y - dst_y) * src_ex));
#ifdef DEBUG
//		printf("debug3(0x%p)=(%d,%d)\n", pSrcGR, (y - dst_y), ((y - dst_y) * src_ex) );
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		for(x = dst_x; x < dst_ex; x++)
		{
			if( (x >= x_min) && (x < x_max) )
			{
				if((*pSrcGR != 0) || (ConvPal == -1))	/* ���ߐF�ȊO */
				{
					*pDstGR = *pSrcGR + Pal_offset;
				}
			}
			pDstGR++;
			pSrcGR++;
#ifdef DEBUG
//			printf("debug4(0x%p)=(%d,%d)\n", pSrcGR, x, y );
//			KeyHitESC();	/* �f�o�b�O�p */
#endif
		}
	}

	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int32_t G_PAGE_SET(uint16_t uState)
{
	int32_t	ret = 0;

	if((uState & Bit_3) != 0u)
	{
		ret = _iocs_apage(3);		/* �O���t�B�b�N�̏�������(4) */
		if(ret < 0)printf("error(%d):_iocs_apage3\n", ret);
	}
	if((uState & Bit_2) != 0u)
	{
		ret = _iocs_apage(2);		/* �O���t�B�b�N�̏�������(3) */
		if(ret < 0)printf("error(%d):_iocs_apage2\n", ret);
	}
	if((uState & Bit_1) != 0u)
	{
		ret = _iocs_apage(1);		/* �O���t�B�b�N�̏�������(2) */
		if(ret < 0)printf("error(%d):_iocs_apage1\n", ret);
	}
	if((uState & Bit_0) != 0u)
	{
		ret = _iocs_apage(0);		/* �O���t�B�b�N�̏�������(1) */
		if(ret < 0)printf("error(%d):_iocs_apage0\n", ret);
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
/* ��ʂ̃N���A */
int32_t G_CLR(void)
{
	int32_t	ret = 0;
	
	_iocs_window( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1);

	_iocs_g_clr_on();			/* �O���t�B�b�N�̃N���A */

	ret = _iocs_wipe();
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_CLR_HS(void)	/* ���ݕ\�����Ă���̈���N���A���� */
{
	int16_t	ret = 0;
	
	volatile uint16_t *CRTC_R21  = (uint16_t *)0xE8002Au;	/* �e�L�X�g�E�A�N�Z�X�E�Z�b�g�A�N���A�[P.S */
	volatile uint16_t *CRTC_mode = (uint16_t *)0xE80480u;	/* CRTC����|�[�g */

	/* �K���e�L�X�g�\�������̌�ɍs������ */
	/* �N���A�ɂ����鎞�Ԃ�1/55fps�Ƃ̂���@Tea_is_Apple������ */
//	wait_v_sync();
	*CRTC_R21  = 0x03u;	/* SCREEN1 �����N���AOFF(bit3:0 bit2:0) / SCREEN0 �����N���AON(bit1:1 bit0:1) */
	*CRTC_mode = 0x02u;	/* �N���A���s */
//	*CRTC_R21  = Mbset(*CRTC_R21,  0x0Fu, 0x0Cu);	/* SCREEN1 �����N���AON / SCREEN0 �����N���AOFF */
//	*CRTC_mode = Mbset(*CRTC_mode, 0x02u, 0x02u);	/* �N���A���s */
//	wait_v_sync();
		
	while(!(*CRTC_mode) & 0x02);	/* �����O�E�F�C�g */
	while((*CRTC_mode) & 0x02);		/* �������E�F�C�g */
	
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_CLR_AREA(int16_t x, uint16_t w, int16_t y, uint16_t h, uint8_t Screen)
{
	int16_t	ret = 0;
#if 1	/* memset */
	int16_t	i=0;
	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	uint32_t	unSize;
#endif

#if 1	/* memset */
	switch(Screen)
	{
		case 0:
		{
			ulGR_H = 0xC00000;	/* Screen0 */
			break;
		}
		case 1:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
		default:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
	}
	
	unSize = (w << 1);
	ulPoint = (y << 10u) + (x << 1);
#endif

#if 1	/* memset */
	for(i=0; i<h; i++)
	{
		memset((void *)(ulGR_H + ulPoint + (i << 10u)), 0x00u, unSize);
	}
#endif

	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_CLR_ALL_OFFSC(uint8_t bMode)
{
	int16_t	ret = 0;
	
	ST_CRT	stCRT = {0};
	
	GetCRT(&stCRT, bMode);
	
	/* �`��\�g�Đݒ� */
	_iocs_window(	stCRT.hide_offset_x, 
					stCRT.hide_offset_y,
					stCRT.hide_offset_x + WIDTH,
					stCRT.hide_offset_y + Y_MAX_WINDOW);	
	/* ���� */
#if	1	/* IOCS */
	ret = _iocs_wipe();	/* �N���A */
#else
	ret = G_CLR_AREA(stCRT.hide_offset_x, WIDTH, stCRT.hide_offset_y, Y_MAX_WINDOW, 0);	/* Screen0 ���� */
#endif
	
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_FILL_AREA(int16_t x, uint16_t w, int16_t y, uint16_t h, uint8_t Screen, uint8_t bPal)
{
	int16_t	ret = 0;
	int16_t	i=0;
	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	uint32_t	unSize;

	_iocs_window( x, y, x + w, y + h);	/* �`��\�g�Đݒ� */

	switch(Screen)
	{
		case 0:
		{
			ulGR_H = 0xC00000;	/* Screen0 */
			break;
		}
		case 1:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
		default:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
	}
	
	unSize = (w << 1);
	ulPoint = (y << 10u) + (x << 1);

	for(i=0; i<h; i++)
	{
		memset((void *)(ulGR_H + ulPoint + (i << 10u)), bPal, unSize);
	}

	return	ret;
}


/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_Load(uint8_t bCGNum, uint16_t uX, uint16_t uY, uint16_t uArea)
{
	int16_t	ret = 0;

	ret = APICG_DataLoad2G( g_stCG_LIST[bCGNum].bFileName, uX, uY, uArea);

	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_Load_Mem(uint8_t bCGNum, int16_t posX, int16_t posY, uint16_t uArea)
{
	int16_t	ret = 0;

	uint16_t	*pDstGR, *pSrcGR;
	uint32_t	DstGR_H;
	uint32_t	Addr_Max;
	uint32_t	uMaxNum;
	int16_t		x, y, sx, sy, ex, ey;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	uSize8x = 0;
	int16_t		Pal_offset;
	int16_t		ConvPal;
	int8_t		bType;

	Get_CG_FileList_MaxNum(&uMaxNum);
	if(uMaxNum <= bCGNum)
	{
		ret = -1;
	}
//	printf("G_Load_Mem:(%d/%d)\n", bCGNum, uMaxNum);	
//	KeyHitESC();	/* �f�o�b�O�p */

	pSrcGR = g_stPicImage[bCGNum].pImageData;	/* Src PIC */
	if(pSrcGR == NULL)
	{
#if 0		
		uint16_t *work_buf = NULL;
		work_buf = (uint16_t*)MyMalloc( PIC_WORK_BUF_SIZE );	/* ��ʂƓ��� */

		/* �t�@�C������ǂݍ��� */
		CG_File_Load_to_Mem(bCGNum);						/* �t�@�C�����������ɓǂݍ��݁��C���[�W�o�b�t�@�m�� */
		ret = APICG_DataLoad2M(bCGNum, posX, posY, uArea, work_buf);	/* PIC�f�[�^���������ɓW�J */
		pSrcGR = Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* �摜�̏����擾 */
		G_APIC_to_Mem(work_buf, pSrcGR, uWidth, uHeight);	/* APIC�摜���g���₷��BITMAP���̃������ɓW�J */
		if(work_buf != NULL)
		{
			MyMfree(work_buf);	/* ��������� */
		}
#else
#ifdef DEBUG
//		printf("G_Load_Mem(%d,0x%p) = %d\n", bCGNum, pSrcGR, uSize8x);
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		ret = CG_File_Load(bCGNum);	/* ���̃��[�`���Ƀo�O���� */
//		printf("G_Load_Mem:(%d, %d)\n", ret, bCGNum);	
//		KeyHitESC();	/* �f�o�b�O�p */
		if(ret < 0)
		{
			return	ret;
		}
#endif
	}
	/* ��ʕ\�� */
	pSrcGR = Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* �摜�̏����擾 */
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8�̔{�� */
	
	if( uWidth >= X_MAX_DRAW )
	{
		uWidth = X_MAX_DRAW;
	}
	if( uHeight >= Y_MAX_DRAW )
	{
		uHeight = Y_MAX_DRAW;
	}
	
	/* �A�h���X�Z�o */
	if(uArea == 0)
	{
		DstGR_H		= 0xC00000;	/* Screen0 */
		Addr_Max	= 0xC80000;
	}
	else{
		DstGR_H		= 0xC80000;	/* Screen1 */
		Addr_Max	= 0xD00000;
	}

	Pal_offset = Set_PicImagePallet(bCGNum);	/* �p���b�g��ݒ� */
	ConvPal = g_stCG_LIST[bCGNum].TransPal;	
	bType   = g_stCG_LIST[bCGNum].bType;

	if((posX + uWidth) < X_MAX_DRAW)
	{
		ex = posX + uWidth;
	}
	else	/* �͂ݏo�����͏������Ȃ��悤�ɃT�C�Y������������*/
	{
		ex = X_MAX_DRAW;
	}
	
	if((posY + uHeight) < Y_MAX_DRAW)
	{
		ey = posY + uHeight;
	}
	else	/* �͂ݏo�����͏������Ȃ��悤�ɃT�C�Y������������*/
	{
		ey = Y_MAX_DRAW;
	}
	
	if(posX >= X_MIN_DRAW)
	{
		sx = posX;
	}
	else
	{
		sx = X_MIN_DRAW;
	}
	
	if(posY >= Y_MIN_DRAW)
	{
		sy = posY;
	}
	else
	{
		sy = Y_MIN_DRAW;
	}
	
	
	for(y = 0; y < uHeight; y++)
	{
		if( ((posY + y) >= Y_MIN_DRAW) && ((posY + y) < Y_MAX_DRAW) )
		{
			/* �A�h���X�Z�o */
			pDstGR = (uint16_t *)( DstGR_H + ( Mmul1024(posY + y) + Mmul2(posX) ) );
		}
		else
		{
			pSrcGR += uSize8x;
			continue;
		}
		
		for(x = 0; x < uSize8x; x++)
		{
			if( ((posX + x) >= X_MIN_DRAW) && ((posX + x) < X_MAX_DRAW) )
			{
				/* �A�h���X�Z�o */
//				pDstGR = (uint16_t *)( DstGR_H + ( Mmul1024(posY + y) + Mmul2(posX + x) ) );
				
				if((*pSrcGR != 0x00) || (ConvPal == -1))	/* ���ߐF�ȊO */
				{
#ifdef DEBUG
//					if(bCGNum == 1)
//					{
//						printf("DImg(0x%p)SImg(0x%p)=%d\n", pDstGR, pSrcGR, (*pSrcGR) + Pal_offset);
//						KeyHitESC();	/* �f�o�b�O�p */
//					}
#endif
					*pDstGR = (*pSrcGR) + Pal_offset;
				}
			}
			pDstGR++;
			pSrcGR++;
		}
	}
	
	return	ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* PIC�t�@�C����ǂݍ��� */
int16_t APICG_DataLoad2G(int8_t *fname, uint64_t pos_x, uint64_t pos_y, uint16_t uArea)
{
	int16_t ret;

	uint16_t *GR;
	uint8_t *file_buf = NULL, *work_buf = NULL;
	
	file_buf = (uint8_t*)MyMalloc(PIC_FILE_BUF_SIZE);
	work_buf = (uint8_t*)MyMalloc(PIC_WORK_BUF_SIZE);
	
	if(uArea != 0)
	{
		GR = (uint16_t *)0xC80000;	/* Screen1 */
	}
	else{
		GR = (uint16_t *)0xC00000;	/* Screen0 */
	}
	
	if( (file_buf == NULL) || (work_buf == NULL) )
	{
		/* �������G���[ */
		ret = -1;
	}
	else
	{
		ret = APICLOAD(	(uint16_t*)GR, 
						fname,				/* PIC�t�@�C���� */
						pos_x, pos_y,		/* �`����X���W��Y���W */
						file_buf,
						PIC_FILE_BUF_SIZE,	
						APF_NOINITCRT | 	/* 1�ŉ�ʃ��[�h�����������܂��� */
						APF_NOCLRBUF | 		/* 1�œW�J��o�b�t�@���N���A���܂��� */
						APF_NOPRFC,			/* 1�Ńt�@�C�����ƃR�����g��\�����܂��� */
						work_buf);
		if (ret < 0) {
			/* �������G���[ */
			ret = -1;
		}
	}
	
	if(work_buf != NULL)
	{
		MyMfree(work_buf);	/* ��������� */
	}
	if(file_buf != NULL)
	{
		MyMfree(file_buf);	/* ��������� */
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
int16_t APICG_DataLoad2M(uint8_t uNum, uint64_t pos_x, uint64_t pos_y, uint16_t uArea, uint16_t *pDst)
{
	uint16_t *GR;
	uint8_t *work_buf = NULL;
	uint32_t	uMaxNum;
	int16_t ret = 0;
	
	Get_CG_FileList_MaxNum(&uMaxNum);
	if(uMaxNum <= uNum)
	{
		ret = -1;
		return	ret;
	}

	if(pDst == NULL)
	{
		switch(uArea)
		{
		case 0:
			GR = (uint16_t *)0xC00000;	/* Screen0 */
			break;
		case 1:
			GR = (uint16_t *)0xC80000;	/* Screen1 */
			break;
		default:
			GR = (uint16_t *)0xC00000;	/* Screen0 */
			break;
		}
	}
	else{
		GR = pDst;					/* ������ */
		pos_x = 0;					/* ����0 */
		pos_y = 0;					/* ����0 */
	}
	
	{
		uint32_t uWidth, uHeight, uFileSize;
		uint8_t *pFileBuf;

		pFileBuf = (uint8_t*)Get_CG_FileBuf(uNum);	/* �t�@�C���o�b�t�@�̃|�C���^�擾 */
		Get_PicImageInfo(uNum, &uWidth, &uHeight, &uFileSize);	/* �C���[�W���̎擾 */

#ifdef DEBUG
//		printf("MAPIC1(%d:0x%p)=(0x%p)\n", uNum, pDst, pFileBuf);
//		printf("MAPIC2(%d:%d)=(%d,%d)(%d)\n", uNum, uArea, uWidth, uHeight, uFileSize );
#endif

//		work_buf = (uint8_t*)MyMalloc( uHeight * (((uWidth+7)/8) * 8) );
//		work_buf = (uint8_t*)MyMalloc( Mmul2(uHeight * (((uWidth+7)/8) * 8)) );
		work_buf = (uint8_t*)MyMalloc( PIC_WORK_BUF_SIZE );	/* ��ʂƓ��� */
		memset(work_buf, 0, PIC_WORK_BUF_SIZE);
//		KeyHitESC();	/* �f�o�b�O�p */
		if( work_buf == NULL )
		{
			/* �������G���[ */
			ret = -1;
		}
		else
		{
			//int M_APICLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE * );
//			KeyHitESC();	/* �f�o�b�O�p */
			/* ���������烍�[�h */
			ret = M_APICLOAD(	GR,					/* Load Ptr */
								&g_CGInfo[uNum],	/* Data Information */
								pos_x, pos_y,		/* Load Position */
								pFileBuf,			/* Load FileBuf */
								uFileSize,			/* Load FileSize */
								APF_NOINITCRT | 	/* 1�ŉ�ʃ��[�h�����������܂��� */
								APF_NOCLRBUF | 		/* 1�œW�J��o�b�t�@���N���A���܂��� */
								APF_NOPRFC,			/* 1�Ńt�@�C�����ƃR�����g��\�����܂��� */
								work_buf			/* WorkBuf */
							);
		}
#ifdef DEBUG
//		printf("MAPIC3(%d)=(%d,%d)(%d,%d)(%d)\n", uNum,
//												g_CGInfo[uNum].STARTX,	g_CGInfo[uNum].STARTY,
//												g_CGInfo[uNum].SIZEX,	g_CGInfo[uNum].SIZEY,	g_CGInfo[uNum].COLOR );
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
		if(work_buf != NULL)
		{
			MyMfree(work_buf);	/* ��������� */
		}
#ifdef DEBUG
//		printf("MAPIC4(%d:0x%p)\n", uNum, pDst);
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
	}
	return ret;
}

#if 0
      +--------------------------------------------------------------+
      |0       |                |����Ƀ��[�h�E�Z�[�u���s�Ȃ��܂�  |
      |        |                |���i�G���[�Ȃ��j�B                  |
      |--------+----------------+------------------------------------|
      |-1�`-96 |                |Human68K���Ԃ��G���[�R�[�h�ł��B    |
      |--------+----------------+------------------------------------|
      |-123    |_ERRAPG_FORMAT  |�t�H�[�}�b�g���Ⴄ�A���邢�͉���  |
      |        |                |����t�@�C���ł��B                  |
      |--------+----------------+------------------------------------|
      |-122    |_ERRAPG_FILEBUF |�t�@�C���o�b�t�@�����������܂��B    |
      |--------+----------------+------------------------------------|
      |-121    |_ERRAPG_RECT    |���W���̈���͂ݏo���Ă��܂��B      |
      |--------+----------------+------------------------------------|
      |-119    |_ERRAPG_MODE    |�Z�[�u�ł��Ȃ���ʃ��[�h�ł��B      |
      |--------+----------------+------------------------------------|
      |-115    |_ERRAPG_WORKBUF |���[�N�o�b�t�@�����������܂��B      |
      |--------+----------------+------------------------------------|
      |-113    |_ERRAPG_FUTURE  |���Ή��̃w�b�_�ł��B                |
      +--------------------------------------------------------------+
#endif

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_Subtractive_Color(uint16_t *pSrcBuf, uint16_t *pDstBuf, uint16_t uWidth, uint16_t uHeight, uint16_t uWidthEx, uint32_t uNum)
{
	int16_t	ret = 0;
	uint32_t	i, j, k, m;
	uint32_t	x, y, z;
	uint32_t	uAPICG_work_Size;
	uint32_t	uSize8x = 0;
	uint16_t	*pBuf = NULL;
	uint16_t	col;
	uint8_t	ubR=0,ubG=0,ubB=0;
	uint8_t	ubNotUsePal[256];
	uint16_t	uColTbl[256];
	int16_t	ConvPal;
	int8_t	bType;
	uint8_t	ubCow_R, ubCow_G, ubCow_B;
	uint8_t	ubTableOffset;
	
	/* �ϓ��ʎq���e�[�u�� */
	uint8_t	ubGen8_R[8];
	uint8_t	ubGen8_G[8];
	uint8_t	ubGen8_B[8];

	pBuf = pSrcBuf;
	uAPICG_work_Size = Mdiv2(PIC_WORK_BUF_SIZE);
	uSize8x = uWidthEx;
	ConvPal = g_stCG_LIST[uNum].TransPal;
	bType   = g_stCG_LIST[uNum].bType;
	
	switch(bType)		
	{
		case 1:	/* �X�v���C�g���C�N */
		case 4:	/* �X�v���C�g���C�N */
		{
			ubCow_R = PIC_R;
			ubCow_G = PIC_G;
			ubCow_B = PIC_B;
			ubTableOffset = 2u;
			z = (ubCow_R * ubCow_G * ubCow_B) + ubTableOffset;
			
			break;
		}
		case 2:	/* �e�L�X�g�`�� */
		{
			ubCow_R = 2u;
			ubCow_G = 2u;
			ubCow_B = 2u;
			ubTableOffset = 0u;
			z = (ubCow_R * ubCow_G * ubCow_B) + ubTableOffset;

			break;
		}
		case 3:	/* �e�L�X�g�`��(�O���C�X�P�[��) */
		{
			ubCow_R = 8u;
			ubCow_G = 8u;
			ubCow_B = 8u;
			ubTableOffset = 0u;
			z = (8) + ubTableOffset;

			break;
		}
		default:
		{
			ubCow_R = PIC_R;
			ubCow_G = PIC_G;
			ubCow_B = PIC_B;
			ubTableOffset = 2u;
			z = (ubCow_R * ubCow_G * ubCow_B) + ubTableOffset;

			break;
		}
	}
	
	/* ���O�p���b�g���o���� */
	for(j=0; j<G_PAL_MAX; j++)
	{
		ubNotUsePal[j] = j;
		uColTbl[j] = 0;
	}
	
	/* �F�̐ݒ� */
	if(bType == 3u)	/* �O���C�X�P�[�� */
	{
		g_CG_MaxColor[uNum][COLOR_R] = 255;
		g_CG_MaxColor[uNum][COLOR_G] = 255;
		g_CG_MaxColor[uNum][COLOR_B] = 255;
		
		m = 0;
		for(i=0; i<z; i++)
		{
			ubGen8_R[i] = Mmul4(i);
			ubGen8_G[i] = ubGen8_R[i];
			ubGen8_B[i] = ubGen8_R[i];
			
			uColTbl[m] = SetRGB(ubGen8_R[i], ubGen8_G[i], ubGen8_B[i]);
			m++;
		}
	}
	else
	{
		uint32_t uHalfSize = Mdiv2(uAPICG_work_Size);
		/* ���F�J���[�̑Ώۂ𒊏o */
		for(j=0; j < uHalfSize; j++)
		{
			col = GPALET( *pBuf, -1 );	/* ���݂̐ݒu�𒊏o */
			
			if(*pBuf == ConvPal)	/* �����F */
			{
				/* �������Ȃ� */
			}
			else
			{
				/* �ő�l���擾 */
				ubR = Mmax( ubR, GetR(col) );
				ubG = Mmax( ubG, GetG(col) );
				ubB = Mmax( ubB, GetB(col) );
			}
			pBuf++;
		}
		g_CG_MaxColor[uNum][COLOR_R] = ubR;
		g_CG_MaxColor[uNum][COLOR_G] = ubG;
		g_CG_MaxColor[uNum][COLOR_B] = ubB;

		pBuf = pSrcBuf;	/* �|�C���^�����Ƃɖ߂� */

		/* ���F�J���[�̐ݒ� */
		ubGen8_R[0] = 0x01;
		for(j=1; j<ubCow_R; j++)
		{
			ubGen8_R[j] = g_CG_MaxColor[uNum][COLOR_R] / (ubCow_R-j);
		}
		ubGen8_G[0] = 0x01;
		for(j=1; j<ubCow_G; j++)
		{
			ubGen8_G[j] = g_CG_MaxColor[uNum][COLOR_G] / (ubCow_G-j);
		}
		ubGen8_B[0] = 0x01;
		for(j=1; j<ubCow_B; j++)
		{
			ubGen8_B[j] = g_CG_MaxColor[uNum][COLOR_B] / (ubCow_B-j);
		}
		m = 1;
		uColTbl[0] = SetRGB(0, 0, 0);	/* ���� */
		
		for(i=0; i<ubCow_R; i++)
		{
			for(j=0; j<ubCow_G; j++)
			{
				for(k=0; k<ubCow_B; k++)
				{
					uColTbl[m] = SetRGB(ubGen8_R[i], ubGen8_G[j], ubGen8_B[k]);
					m++;
				}
			}
		}
	}
	
	
	/* ���H����������ɕۑ����� */
	for(y=0; y < uHeight; y++)
	{
//		pBuf = pSrcBuf + (y << 9);
		pBuf = pSrcBuf + (y << 8);

		for(x=0; x < uSize8x; x++)
		{
			if(x < uWidth)
			{
				uint8_t pal;
				
				pal = (uint8_t)(*pBuf & 0x00FF);	/* �摜�̃p���b�g�ԍ����擾 */

				ubNotUsePal[pal] = 0;	/* �g�p�����p���b�g */

				if(pal == ConvPal)	/* �ϊ��ΏېF */
				{
					pal = TRANS_PAL;	/* ���ߐF */
				}
				else
				{
					uint8_t ubTmp;
					
					col = GPALET(pal, -1);	/* �p���b�g�ԍ�����J���[�R�[�h���擾 */
					ubR = GetR(col);
					ubG = GetG(col);
					ubB = GetB(col);
					
					if(bType == 3u)	/* �O���C�X�P�[�� */
					{
						uint32_t	uGlay;
						uGlay =  ((uint32_t)(Mmul2(ubR) + ubR));
						uGlay += ((uint32_t)(Mmul4(ubG) + Mmul2(ubG)));
						uGlay += ((uint32_t)ubB);
						//uGlay = Mdiv10(uGlay);
						uGlay = Mdiv4(uGlay);
						
						col = SetRGB(uGlay, uGlay, uGlay);	/* �R�F������ */

						ubR = GetR(col);
						ubG = GetG(col);
						ubB = GetB(col);

						k = 1;
						ubTmp = McmpSub(ubGen8_R[0], ubR);
						for(m=1; m<ubCow_R; m++)
						{
							if( ubTmp > McmpSub(ubGen8_R[m], ubR) )
							{
								ubTmp = McmpSub(ubGen8_R[m], ubR);
								k = m;
							}
						}
						ubR = ubGen8_R[k];
						ubG = ubR;
						ubB = ubR;
					}
					else
					{
						/* �ϓ��ʎq�� */
						k = 0;
						ubTmp = McmpSub(ubGen8_R[0], ubR);
						for(m=1; m<ubCow_R; m++)
						{
							if( ubTmp > McmpSub(ubGen8_R[m], ubR) )
							{
								ubTmp = McmpSub(ubGen8_R[m], ubR);
								k = m;
							}
						}
						ubR = ubGen8_R[k];
						
						k = 0;
						ubTmp = McmpSub(ubGen8_G[0], ubG);
						for(m=1; m<ubCow_G; m++)
						{
							if( ubTmp > McmpSub(ubGen8_G[m], ubG) )
							{
								ubTmp = McmpSub(ubGen8_G[m], ubG);
								k = m;
							}
						}
						ubG = ubGen8_G[k];

						k = 0;
						ubTmp = McmpSub(ubGen8_B[0], ubB);
						for(m=1; m<ubCow_B; m++)
						{
							if( ubTmp > McmpSub(ubGen8_B[m], ubB) )
							{
								ubTmp = McmpSub(ubGen8_B[m], ubB);
								k = m;
							}
						}
						ubB = ubGen8_B[k];
					}
					col = SetRGB(ubR, ubG, ubB);	/* �R�F������ */
					
					for( i=0; i<z; i++ )
					{
						if(uColTbl[i] == col)
						{
							pal = i;
							break;
						}
					}
				}
				*pDstBuf = pal;		/* �R�s�[��� */
				
				pBuf++;
			}
			else
			{
				*pDstBuf = 0x00;
			}
			pDstBuf++;
		}
	}
	
	for(j=0; j<G_PAL_MAX; j++)
	{
		if(j <= z)
		{
			GPALET( j, uColTbl[j] );	/* ���F���ʂŃp���b�g�X�V */
		}
		else
		{
			GPALET( j, 0 );				/* 0 */
		}
	}
//	for(j=0; j<G_PAL_MAX; j++)
//	{
//		if(ubNotUsePal[j] != 0)
//		{
//			GPALET( j, SetRGB(0, 0, 0) );	/* �g���ĂȂ��p���b�g�͓��ߐF�ݒ� */
//		}
//	}
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t PutGraphic_To_Text(uint8_t bCGNum, uint16_t dx, uint16_t dy, uint16_t uCRT)
{
	int16_t	ret = 0;

	uint32_t	x, y, z;
	uint32_t	y_max;
	uint32_t	y_inc;
	uint32_t	uMaxNum;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	uSize8x = 0;
	
	uint16_t	uDstTxAddr;
	uint16_t	*pSrcGR;
	uint16_t	*pSrcGR_Tmp;
	
	uint16_t	*T0_HEAD = (uint16_t *)0xE00000;
	uint16_t	*T1_HEAD = (uint16_t *)0xE20000;
	uint16_t	*T2_HEAD = (uint16_t *)0xE40000;
	uint16_t	*T3_HEAD = (uint16_t *)0xE60000;
	uint16_t	*pDst0;
	uint16_t	*pDst1;
	uint16_t	*pDst2;
	uint16_t	*pDst3;
	uint16_t	unPal;
	uint16_t	unPalOffset;
	uint16_t	unTmp[4];
	
	Get_CG_FileList_MaxNum(&uMaxNum);
	if(uMaxNum <= bCGNum)
	{
		ret = -1;
		return	ret;
	}
	pSrcGR = g_stPicImage[bCGNum].pImageData;	/* Src PIC */
	
	Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* �摜�̏����擾 */
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8�̔{�� */
#ifdef DEBUG
//	printf("%d, %d, %d, %d, %d\n", dx, dy, uWidth, uHeight, uSize8x);
//	KeyHitESC();	/* �f�o�b�O�p */
#endif

	unPalOffset = Set_PicImagePallet(bCGNum);	/* �p���b�g��ݒ� */
	
	y_inc = 1;
	y_max = (dy + uHeight);

	for(y = dy; y < y_max; y++)
	{
		pSrcGR_Tmp = pSrcGR + (y * uSize8x);
		
		uDstTxAddr = (y * 0x80)>>y_inc;

		for(x = dx; x < dx+uWidth; x+=16)	/* 16dot */
		{
			/* �A�h���X�Z�o */
//			if( (k < 0) || ((k+size) > 0x1FFFF) ) break;
			unTmp[0] = 0;
			unTmp[1] = 0;
			unTmp[2] = 0;
			unTmp[3] = 0;
			for(z = 0; z < 16; z++)	/* 16dot�����{ */
			{
				if(*pSrcGR_Tmp == 0)
				{
					unPal = *pSrcGR_Tmp;
				}
				else if(*pSrcGR_Tmp >= 16)
				{
					unPal = 15;
				}
				else
				{
					unPal = (*pSrcGR_Tmp) + unPalOffset;
#ifdef DEBUG
//					printf("unPal(%3d,%3d)=%d\n", x, y, unPal);
					//KeyHitESC();	/* �f�o�b�O�p */
#endif
				}
				
				unTmp[0] |= ((unPal & 0b0001)     ) << (15-z);
				unTmp[1] |= ((unPal & 0b0010) >> 1) << (15-z);
				unTmp[2] |= ((unPal & 0b0100) >> 2) << (15-z);
				unTmp[3] |= ((unPal & 0b1000) >> 3) << (15-z);

				pSrcGR_Tmp++;
			}

			pDst0 = T0_HEAD + uDstTxAddr + Mdiv16(x);
			pDst1 = T1_HEAD + uDstTxAddr + Mdiv16(x);
			pDst2 = T2_HEAD + uDstTxAddr + Mdiv16(x);
			pDst3 = T3_HEAD + uDstTxAddr + Mdiv16(x);
			
			*pDst0 = unTmp[0];
			*pDst1 = unTmp[1];
			*pDst2 = unTmp[2];
			*pDst3 = unTmp[3];
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
int16_t PutGraphic_To_Symbol12(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = 1;
	stSymbol.mag_y = 1;
	stSymbol.color = uPal;
	stSymbol.font_type = 0;	/* 0:12x12 1:16x16 2:24x24 */
	stSymbol.angle = 0;		/* 0:Normal 1:90 2:180 3:270 */
	
	_iocs_symbol(&stSymbol);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t PutGraphic_To_Symbol16(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = 1;
	stSymbol.mag_y = 1;
	stSymbol.color = uPal;
	stSymbol.font_type = 1;	/* 0:12x12 1:16x16 2:24x24 */
	stSymbol.angle = 0;		/* 0:Normal 1:90 2:180 3:270 */
	
	_iocs_symbol(&stSymbol);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t PutGraphic_To_Symbol24(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = 1;
	stSymbol.mag_y = 1;
	stSymbol.color = uPal;
	stSymbol.font_type = 2;	/* 0:12x12 1:16x16 2:24x24 */
	stSymbol.angle = 0;		/* 0:Normal 1:90 2:180 3:270 */
	
	_iocs_symbol(&stSymbol);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t PutGraphic_To_Symbol24_Xn(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal, uint16_t uMag)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = uMag;
	stSymbol.mag_y = uMag;
	stSymbol.color = uPal;
	stSymbol.font_type = 2;	/* 0:12x12 1:16x16 2:24x24 */
	stSymbol.angle = 0;		/* 0:Normal 1:90 2:180 3:270 */
	
	_iocs_symbol(&stSymbol);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	G_Scroll(int16_t x, int16_t y, uint8_t bSCNum)
{
	int16_t	ret = 0;
	
	switch(bSCNum)
	{
	case 0:	/* �y�[�W�O */
		{
			volatile uint16_t *CRTC_R12 = (uint16_t *)0xE80018u;
			volatile uint16_t *CRTC_R13 = (uint16_t *)0xE8001Au;
			volatile uint16_t *CRTC_R14 = (uint16_t *)0xE8001Cu;
			volatile uint16_t *CRTC_R15 = (uint16_t *)0xE8001Eu;
			*CRTC_R12  = Mbset(*CRTC_R12 , 0x03FF, x);	/* Screen 0 x */
			*CRTC_R13  = Mbset(*CRTC_R13 , 0x03FF, y);	/* Screen 0 y */
			*CRTC_R14  = Mbset(*CRTC_R14 , 0x01FF, x);	/* Screen 1 x */
			*CRTC_R15  = Mbset(*CRTC_R15 , 0x01FF, y);	/* Screen 1 y */
			break;
		}
	case 1:	/* �y�[�W�P */
		{
			volatile uint16_t *CRTC_R16 = (uint16_t *)0xE80020u;
			volatile uint16_t *CRTC_R17 = (uint16_t *)0xE80022u;
			volatile uint16_t *CRTC_R18 = (uint16_t *)0xE80024u;
			volatile uint16_t *CRTC_R19 = (uint16_t *)0xE80026u;
			*CRTC_R16  = Mbset(*CRTC_R16 , 0x01FF, x);	/* Screen 2 x */
			*CRTC_R17  = Mbset(*CRTC_R17 , 0x01FF, y);	/* Screen 2 y */
			*CRTC_R18  = Mbset(*CRTC_R18 , 0x01FF, x);	/* Screen 3 x */
			*CRTC_R19  = Mbset(*CRTC_R19 , 0x01FF, y);	/* Screen 3 y */
			break;
		}
	case 2:
		{
			break;
		}
	case 3:
		{
			break;
		}
	default:
		{
			break;
		}
	}

	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	�z�u�������ʒu�ƃ|�W�V��������`�悵�����摜����̍��W�ʒu���Z�o����		*/
/*===========================================================================================*/
int16_t G_ClipAREA_Chk_Width(	int16_t *pPos_x, int16_t *x_min, int16_t *x_max, uint16_t uSrc_w, 
								uint8_t ubMode, uint8_t ubH)
{
	int16_t	ret = 0;
	int16_t	x, w;
	
	x = *pPos_x;
	
	/* ���� */
	switch(ubMode)
	{
		case 0:
		{
			x += X_MIN_DRAW;
			*x_min = X_MIN_DRAW;
			*x_max = X_MIN_DRAW + WIDTH;
			break;
		}
		case 1:
		{
			x += X_MIN_DRAW + X_OFFSET;
			*x_min = X_MIN_DRAW + X_OFFSET;
			*x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			break;
		}
		case 2:
		{
			x += X_MIN_DRAW + X_OFFSET;
			*x_min = X_MIN_DRAW + X_OFFSET;
			*x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			break;
		}
		default:
		{
			x += X_MIN_DRAW;
			*x_min = X_MIN_DRAW;
			*x_max = X_MAX_DRAW;
			break;
		}
	}

	/*�\���G���A�͈͓����� */
	switch(ubH)	/* �摜�̊�ʒu���Z�o */
	{
		case POS_LEFT:
		default:
		{
			/*�\���G���A�͈͓����� */
			w = (int16_t)uSrc_w;
			if((x + w) < *x_min)
			{
				ret = x << 8;
			}
			else if(x >= *x_max)
			{
				ret = x << 8;
			}
			else
			{
				/* OK */
			}
			break;
		}
		case POS_MID:
		{
			w = (int16_t)Mdiv2(uSrc_w);
			/*�\���G���A�͈͓����� */
			if((x + w) < *x_min)
			{
				ret = x << 8;
			}
			else if((x - w) >= *x_max)
			{
				ret = x << 8;
			}
			else
			{
				/* OK */
			}
			x -= w;
			break;
		}
		case POS_RIGHT:
		{
			w = (int16_t)uSrc_w;
			/*�\���G���A�͈͓����� */
			if(x < *x_min)
			{
				ret = x << 8;
			}
			else if((x - w) >= *x_max)
			{
				ret = x << 8;
			}
			else
			{
				/* OK */
			}
			x -= w;
			break;
		}
	}
	
	*pPos_x = x;
	
#if 0
	if(ret != 0)
	{
		printf("W(%hd)(%hd,%hd)(%hd)(%d)(%d)\n", x, *x_min, *x_max, uSrc_w, ubMode, ubH);
		KeyHitESC();	/* �f�o�b�O�p */
	}
#endif
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	�z�u�������ʒu�ƃ|�W�V��������`�悵�����摜����̍��W�ʒu���Z�o����		*/
/*===========================================================================================*/
int16_t G_ClipAREA_Chk_Height(	int16_t *pPos_y, int16_t *y_min, int16_t *y_max, uint16_t uSrc_h,
								uint8_t ubMode, uint8_t ubV)
{
	int16_t	ret = 0;
	int16_t	y, h;
	
	y = *pPos_y;
	
	/* �c�� */
	switch(ubMode)
	{
		case 0:
		{
			y += Y_MIN_DRAW;
			*y_min = Y_MIN_DRAW;
			*y_max = Y_MAX_DRAW;
			break;
		}
		case 1:
		{
			y += Y_MIN_DRAW;
			*y_min = Y_MIN_DRAW;
			*y_max = Y_MIN_DRAW + Y_MAX_WINDOW;
			break;
		}
		case 2:
		{
			y += Y_MIN_DRAW + Y_OFFSET;
			*y_min = Y_MIN_DRAW + Y_OFFSET;
			*y_max = Y_MIN_DRAW + Y_OFFSET + Y_MAX_WINDOW;
			break;
		}
		default:
		{
			y += Y_MIN_DRAW;
			*y_min = Y_MIN_DRAW;
			*y_max = Y_MAX_DRAW;
			break;
		}
	}
	
	/*�\���G���A�͈͓����� */
	switch(ubV)
	{
		case POS_TOP:
		default:
		{
			h = (int16_t)uSrc_h;
			/*�\���G���A�͈͓����� */
			if((y + h) < *y_min)
			{
				ret = y;
			}
			else if(y >= *y_max)
			{
				ret = y;
			}
			else
			{
				/* OK */
			}
			break;
		}
		case POS_CENTER:
		{
			h = (int16_t)Mdiv2(uSrc_h);
			/*�\���G���A�͈͓����� */
			if((y + h) < *y_min)
			{
				ret = y;
			}
			else if((y - h) >= *y_max)
			{
				ret = y;
			}
			else
			{
				/* OK */
			}
			y -= h;
			break;
		}
		case POS_BOTTOM:
		{
			h = (int16_t)uSrc_h;
			/*�\���G���A�͈͓����� */
			if(y < *y_min)
			{
				ret = y;
			}
			else if((y - h) >= *y_max)
			{
				ret = y;
			}
			else
			{
				/* OK */
			}
			y -= h;
			break;
		}
	}
	
	*pPos_y = y;
	
#if 0
	if(ret != 0)
	{
		printf("H(%hd)(%hd,%hd)(%hd)(%d)(%d)\n", y, *y_min, *y_max, uSrc_h, ubMode, ubV);
		KeyHitESC();	/* �f�o�b�O�p */
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
/* ���f�B�A���J�b�g�@�ɂ��摜�̌��F */
/* http://hiroshi0945.seesaa.net/article/159985352.html */
/* ���f�B�A���J�b�g���FDIB�̍쐬 */
int16_t G_MedianCut(uint16_t *pDstImg, uint16_t *pSrcImg, uint16_t uSrcWidth, uint16_t uSrcHeight,
					uint16_t cEntries, uint8_t ubImgNum, int16_t ConvCol)
{	
	int16_t		ret = 0;
	int32_t		iYUV = 0;
	int32_t		nShift, divBit;
	int32_t		nColors;
	int32_t		i;
	int32_t		index;
	int32_t		pos;
	uint16_t	x, y;
	uint16_t	width, height, Size8x;
	uint32_t	dwSizeDimension;
	uint16_t	*pDst, *pSrc;
	uint16_t	col;
	uint16_t	pal_no;
	uint16_t	nBpp;
	uint8_t		ubOffsetCol;
	RGBQUAD*	pColorTable;
	RGBQUAD		rgb;
	YUV			*pYUV;
	AVGYUV		*pAvgYUV;
	
	/* ���F����F����512�ȏ�2�ȉ��Ȃ�FALSE��Ԃ��ďI������B*/
	if( (cEntries > 512) || (cEntries < 2) )return -1;	/* cEntries�͌��F����F�� */

	/* �p���b�g�F������r�b�g�J�E���g���擾���� */
	if 	(cEntries <= 2u)		nBpp = 1u;	/* cEntries�͌��F����F�� */
	else if (cEntries <= 16u)	nBpp = 4u;	/* 4bit(16�F) */
	else if (cEntries <= 256u)	nBpp = 8u;	/* 8bit(256�F) */
	else						nBpp = 16u;	/* 16bit(65535�F) */
#ifdef DEBUG
//	printf("�F[%d]��(%d bit)\n", cEntries, nBpp);
//	KeyHitESC();	/* �f�o�b�O�p */
#endif
	
	width	= uSrcWidth;	/* �摜�� */
	height	= uSrcHeight;	/* �摜���� */
	Size8x	= (((width+7)/8) * 8);	/* 8�̔{�� */
#ifdef DEBUG
//	printf("SImg(0x%p)%d,%d,%d)\n", pSrcImg, width, height, Size8x );
//	KeyHitESC();	/* �f�o�b�O�p */
#endif

	/* YUV�e�[�u���쐬 */
	dwSizeDimension = Size8x * height;					/* YUV�e�[�u���̃T�C�Y */
	pYUV = (YUV*)MyMalloc( sizeof(YUV) * dwSizeDimension );	/* YUV�e�[�u���̈�m�� */
	if(pYUV == NULL)
	{
		return -1;
	}

	if((ConvCol < 0) || (ConvCol >= 256))
	{
		ConvCol = -1;	/* ���ߐF�Ȃ��ݒ� */
	}
    
	for(y=0; y < height; y++)
	{
        /* y�s�̈�ԍ��[�̃s�N�Z���ւ̃|�C���^�̎Z�o */
		pSrc = pSrcImg + (y * Size8x);
		
        for(x=0; x < width; x++)
		{
			pal_no = *pSrc;
			col = GPALET( *pSrc, -1 );	/* ���݂̐ݒu�𒊏o */
			pSrc++;

			if(ConvCol == -1)		/* ���ߐF�Ȃ����� */
			{
				/* �s�N�Z���f�[�^�����o�� */
				rgb.rgbRed   = Mmul8( GetR(col) );
				rgb.rgbGreen = Mmul8( GetG(col) );
				rgb.rgbBlue  = Mmul8( GetB(col) );
			}
			else
			{
				if(ConvCol == pal_no)	/* ���ߐF���� */
				{
					/* ���ߐF�i���j�ɂ��� */
					rgb.rgbRed   = 0x00u;
					rgb.rgbGreen = 0x00u;
					rgb.rgbBlue  = 0x00u;
				}
				else
				{
					/* �s�N�Z���f�[�^�����o�� */
					rgb.rgbRed   = Mmul8( GetR(col) );
					rgb.rgbGreen = Mmul8( GetG(col) );
					rgb.rgbBlue  = Mmul8( GetB(col) );
				}
			}
#ifdef DEBUG
//			printf("rgb(%d,%d,%d)\n", rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue, GetR(col), GetG(col), GetB(col) );
//			KeyHitESC();	/* �f�o�b�O�p */
#endif
			
			/* RGB����YUV�֕ϊ����� */
			pYUV[iYUV].y = ( 0.29891f * (float)(rgb.rgbRed)) + (0.58661f * (float)(rgb.rgbGreen)) + (0.11448f * (float)(rgb.rgbBlue));
			pYUV[iYUV].u = (-0.16874f * (float)(rgb.rgbRed)) - (0.33126f * (float)(rgb.rgbGreen)) + (0.50000f * (float)(rgb.rgbBlue));
			pYUV[iYUV].v = ( 0.50000f * (float)(rgb.rgbRed)) - (0.41869f * (float)(rgb.rgbGreen)) - (0.08131f * (float)(rgb.rgbBlue));
			pYUV[iYUV].no = 0;
#ifdef DEBUG
//			printf("pYUV(%3.1f,%3.1f,%3.1f)\n", pYUV[iYUV].y, pYUV[iYUV].u, pYUV[iYUV].v );
//			KeyHitESC();	/* �f�o�b�O�p */
#endif
			iYUV++;
		}
	}

	/* ���͐F������2�̏搔���Z�o���܂��B*/
	nShift = cEntries >> 1;
	divBit = 0;
	while(nShift > 0)
	{
		nShift >>= 1;
		divBit++;
	}

	/* �F�𕪂��郁�f�B�A���J�b�g���F�֐����Ăяo���܂��B*/
	G_MedianCut_Gen(pYUV, dwSizeDimension, 0, divBit);

	/* ���F�J���[�e�[�u�����쐬����B*/
	nColors = (nBpp > 8) ? cEntries : (cEntries / 2) * 2 + (cEntries % 2) * 2;
	pColorTable = (RGBQUAD*)MyMalloc( sizeof(RGBQUAD) * nColors );
	memset(pColorTable, 0, sizeof(RGBQUAD) * nColors);
	
	pAvgYUV = (AVGYUV*)MyMalloc( sizeof(AVGYUV) * nColors );
	if(pAvgYUV == NULL)
	{
		return -1;
	}
	
	memset(pAvgYUV, 0, sizeof(AVGYUV) * nColors);

	for(y = 0; y < height; y++)
	{
		iYUV = y * Size8x;
		for(x = 0; x < width; x++)
		{
			int32_t num;
			pos = iYUV + x;
			num = pYUV[pos].no;
			
			pAvgYUV[num].y += (double)pYUV[pos].y;
			pAvgYUV[num].u += (double)pYUV[pos].u;
			pAvgYUV[num].v += (double)pYUV[pos].v;
			pAvgYUV[num].cnt++;
		}
	}

	/* �J���[�e�[�u����YUV�l��RGB�ɕϊ�����B*/
	for(i = 0; i < nColors; i++)
	{
		if(pAvgYUV[i].cnt > 0)
		{
			double red, green, blue;
			
			pAvgYUV[i].y = pAvgYUV[i].y / (double)pAvgYUV[i].cnt;
			pAvgYUV[i].u = pAvgYUV[i].u / (double)pAvgYUV[i].cnt;
			pAvgYUV[i].v = pAvgYUV[i].v / (double)pAvgYUV[i].cnt;

			/* �Ԑ�����YUV->RGB�ϊ� */
			red = pAvgYUV[i].y + 1.40200 * pAvgYUV[i].v;
			red += 0.49;
			if(red < 0.0)	red = 0.0;
			if(red > 255.0)	red = 255.0;
			pColorTable[i].rgbRed = (uint8_t)red;

			//�ΐ�����YUV->RGB�ϊ�
			green = pAvgYUV[i].y - 0.34414 * pAvgYUV[i].u - 0.71414 * pAvgYUV[i].v;
			green += 0.49;
			if(green < 0.0)		green = 0.0;
			if(green > 255.0)	green = 255.0;
			pColorTable[i].rgbGreen = (uint8_t)green;

			//������YUV->RGB�ϊ�
			blue = pAvgYUV[i].y + 1.77200 * pAvgYUV[i].u;
			blue += 0.49;
			if(blue < 0.0)		blue = 0.0;
			if(blue > 255.0)	blue = 255.0;
			pColorTable[i].rgbBlue = (uint8_t)blue;
#ifdef DEBUG
//			printf("RGB[%d](%d,%d,%d)\n", i, pColorTable[i].rgbRed, pColorTable[i].rgbGreen, pColorTable[i].rgbBlue );
//			KeyHitESC();	/* �f�o�b�O�p */
#endif
		}
    }

   	if(pAvgYUV != NULL)
	{
		MyMfree(pAvgYUV);
	}
	
#ifdef DEBUG
	//printf("G_MedianCut[%d]\n", ubImgNum);
	//KeyHitESC();	/* �f�o�b�O�p */
#endif
	ubOffsetCol = 0;
	/* ���F����F����256�ȉ��Ȃ�p���b�gDIB�Ȃ̂ŁA*/
	/* �J���[�e�[�u����BITMAPINFO�\���̂̃J���[�e�[�u���ɃR�s�[���܂��B */
	if(nColors<=256)
	{
		for(i = 0; i < nColors; i++)
		{
			uint16_t uCol;
			uCol = SetRGB( Mdiv8(pColorTable[i].rgbRed), Mdiv8(pColorTable[i].rgbGreen), Mdiv8(pColorTable[i].rgbBlue) );
			GPALET( i, uCol );	/* ���F���ʂŃp���b�g�X�V */
		}
	}
	
#ifdef DEBUG
//	printf("DImg(0x%p)%d,%d,%d)\n", pDstImg, width, height, Size8x );
//	KeyHitESC();	/* �f�o�b�O�p */
#endif
#ifdef DEBUG
	printf("Bpp(%d)\n", nBpp);
#endif
	/* �s�N�Z���f�[�^��DIB�Ɋi�[���܂��B*/
	switch(nBpp)
	{
		case 16:	/* 16�r�b�gDIB */
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);

				iYUV = y * Size8x;
				
				for(x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
					*pDst = SetRGB( Mdiv8(pColorTable[index].rgbRed), Mdiv8(pColorTable[index].rgbGreen), Mdiv8(pColorTable[index].rgbBlue) );
					pDst++;
			    }
			}
			break;

		case 8:	/* 8�r�b�gDIB */
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);
				iYUV = y * Size8x;
				
				for( x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
#ifdef DEBUG
//					printf("pYUV[%d]=(%d)(%d,%d)\n", iYUV, index, x, y);
//					KeyHitESC();	/* �f�o�b�O�p */
#endif
					if(ConvCol == -1)		/* ���ߐF�Ȃ����� */
					{
						*pDst = index + ubOffsetCol;
					}
					else
					{
						if( (pColorTable[index].rgbRed   == 0) &&
							(pColorTable[index].rgbGreen == 0) &&
							(pColorTable[index].rgbBlue  == 0) )
						{
							*pDst = 0;	/* ���F�͋������ߐF */
						}
						else
						{
							*pDst = index + ubOffsetCol;
						}
					}
					pDst++;
				}
			}
			break;

		case 4:	/* 4�r�b�gDIB */
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);
				iYUV = y * Size8x;
				
				for(x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
#ifdef DEBUG
//					printf("pYUV[%5d]=(%3d)(%3d,%3d)\n", iYUV, index, x, y);
//					KeyHitESC();	/* �f�o�b�O�p */
#endif
					if(ConvCol == -1)		/* ���ߐF�Ȃ����� */
					{
						*pDst = index + ubOffsetCol;
					}
					else
					{
						if( (pColorTable[index].rgbRed   == 0) &&
							(pColorTable[index].rgbGreen == 0) &&
							(pColorTable[index].rgbBlue  == 0) )
						{
							*pDst = 0;	/* ���F�͋������ߐF */
						}
						else
						{
							*pDst = index + ubOffsetCol;
						}
					}
					pDst++;
			    }
			}
			break;

		case 1:     //���m�N��DIB
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);
				iYUV = y * Size8x;
				
				for(x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
#ifdef DEBUG
//					printf("pYUV[%d]=(%d)(%d,%d)\n", iYUV, index, x, y);
//					KeyHitESC();	/* �f�o�b�O�p */
#endif
					if(ConvCol == -1)		/* ���ߐF�Ȃ����� */
					{
						*pDst = index + ubOffsetCol;
					}
					else
					{
						if( (pColorTable[index].rgbRed   == 0) &&
							(pColorTable[index].rgbGreen == 0) &&
							(pColorTable[index].rgbBlue  == 0) )
						{
							*pDst = 0;	/* ���F�͋������ߐF */
						}
						else
						{
							*pDst = index + ubOffsetCol;
						}
					}
					pDst++;
			    }
			}
			break;
    }
	if(pYUV != NULL)
	{
		MyMfree(pYUV);
	}
	if(pColorTable != NULL)
	{
		MyMfree(pColorTable);
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
/* ���f�B�A���J�b�g���F */
void G_MedianCut_Gen(YUV* tblYUV, int32_t maxTbl, int32_t base, int32_t divBit)
{
	
	/* ��Ƃ���ϐ��̐��𐔂��� */
	int32_t	i;
	int32_t	a, b;
	int32_t	idx = 0;
	int32_t	swData;
	float minY = 0,minU = 0,minV = 0;
	float maxY = 0,maxU = 0,maxV = 0;
	float avgY = 0,avgU = 0,avgV = 0;
	double allY = 0,allU = 0,allV = 0;

	if(tblYUV == NULL)return;
	if(divBit < 1)return;

	for(i=0; i < maxTbl; i++)
	{
		if(tblYUV[i].no == base)
		{
			if(idx==0)	/* ���߂Ă̒l�̓��͂Ȃ�� */
			{
				allY = minY = maxY = avgY = tblYUV[i].y;
				allU = minU = maxU = avgU = tblYUV[i].u;
				allV = minV = maxV = avgV = tblYUV[i].v;
			}
			else
			{
				if(tblYUV[i].y <= minY)minY = tblYUV[i].y;
				if(tblYUV[i].y >= maxY)maxY = tblYUV[i].y;
				if(tblYUV[i].u <= minU)minU = tblYUV[i].u;
				if(tblYUV[i].u >= maxU)maxU = tblYUV[i].u;
				if(tblYUV[i].v <= minV)minV = tblYUV[i].v;
				if(tblYUV[i].v >= maxV)maxV = tblYUV[i].v;
				allY += (double)tblYUV[i].y;
				allU += (double)tblYUV[i].u;
				allV += (double)tblYUV[i].v;
			}
			idx++;
		}
	}
	if(idx == 0) return;	 /* ��Ƃ���ϐ����Ȃ��̂ŏI�� */

	a = base;
	b = base + (1 << (divBit - 1));

	avgY = (float)(allY / idx);
	avgU = (float)(allU / idx);
	avgV = (float)(allV / idx);

	/* ���f�B�A���J�b�g�@���s���̂ŁA�����ő�ɂȂ�v�f��T���B*/
	if((maxY - minY) > (maxU - minU))
	{
		swData = ((maxY - minY) > (maxV - minV)) ? 0 : 2;
	}
	else
	{
		swData = ((maxU - minU) > (maxV - minV)) ? 1 : 2;
	}
	/* ���ϒl�ŐF�̐؂蕪�� */
	switch(swData)
	{
		case 0:	/* Y */
			for(i = 0; i < maxTbl; i++)
			{
				if(tblYUV[i].no == base)
				{
					tblYUV[i].no = (tblYUV[i].y < avgY) ? a : b;
				}
			}
			break;
		case 1:	/* U */
			for(i = 0; i < maxTbl; i++)
			{
				if(tblYUV[i].no == base)
				{
					tblYUV[i].no = (tblYUV[i].u < avgU) ? a : b;
				}
			}
			break;
		case 2:	/* V */
			for(i = 0; i < maxTbl; i++)
			{
				if(tblYUV[i].no == base)
				{
					tblYUV[i].no = (tblYUV[i].v < avgV) ? a : b;
				}
			}
			break;
	}

	/* �ċN���� */
	G_MedianCut_Gen(tblYUV, maxTbl, a, divBit-1);
	G_MedianCut_Gen(tblYUV, maxTbl, b, divBit-1);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void G_APIC_to_Mem(uint16_t *pSrcBuf, uint16_t *pDstBuf, uint16_t uWidth, uint16_t uHeight)
{
	uint32_t	x, y;
	uint16_t	*pBuf;
	uint16_t	uSize8x;
	
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8�̔{�� */
	
	/* ���H����������ɕۑ����� */
	for(y=0u; y < uHeight; y++)
	{
		pBuf = pSrcBuf + (y << 9u);
		
		for(x=0u; x < uSize8x; x++)
		{
			if(x < uWidth)
			{
				*pDstBuf = *pBuf & 0xFFu;
				pBuf++;
			}
			else
			{
				*pDstBuf = 0x00u;
			}
			pDstBuf++;
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
void G_Img_V_H_Cnv(uint16_t *pSrcBuf, uint16_t *pDstBuf, uint16_t uWidth, uint16_t uHeight, uint16_t uScrlw)
{
	uint32_t	x, y, z;
	uint16_t	*pBuf;
	uint16_t	uSize8x;
	uint16_t	uScrlwCnt;
	
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8�̔{�� */
	
	uScrlwCnt = uSize8x / uScrlw;	/* �u���b�N�� */

	for(z=0u; z < uScrlwCnt; z++)
	{
		for(y=0u; y < uHeight; y++)
		{
			pBuf = (uint16_t *)(pSrcBuf + (y * uSize8x) + (z * uScrlw));
			
			for(x=0u; x < uScrlw; x++)
			{
				*pDstBuf = *pBuf & 0xFFu;
				pDstBuf++;
				pBuf++;
			}
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
void G_Mem_Cnv_Pal(uint16_t *pDstBuf, uint16_t uWidth, uint16_t uHeight, int16_t pal)
{
	uint32_t	x, y;
	uint16_t	*pBuf;
	uint16_t	uSize8x;
	uint16_t	Pal_offset;
	int16_t		ConvPal;
	int8_t		bType;
	
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8�̔{�� */
	
	Pal_offset = Set_PicImagePallet(pal);	/* �p���b�g��ݒ� */
	ConvPal = g_stCG_LIST[pal].TransPal;	
	bType   = g_stCG_LIST[pal].bType;

	/* 512x512 --> uWidth x uHeight */

	/* ���H����������ɕۑ����� */
	for(y=0u; y < uHeight; y++)
	{
		pBuf = (uint16_t *)(pDstBuf + (y * uSize8x));
		
		for(x=0u; x < uSize8x; x++)
		{
			if(x < uWidth)
			{
				*pBuf += Pal_offset;
				pBuf++;
			}
			else
			{
				*pBuf = 0x00u;
				pBuf++;
			}
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
int16_t G_Pal_Set(uint8_t ubImgNum)
{
	int16_t	ret = 0;
	
	if(ubImgNum >= PAL_MAX_CG)return -1;
	
	g_CG_PalTbl[ubImgNum] = ubImgNum;			/* �p���b�g�e�[�u���ɃZ�b�g */
	
	ret = g_CG_PalTbl[ubImgNum];
	
    return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t G_Pal_Get(uint8_t ubImgNum)
{
	int16_t	ret = 0;

	if(ubImgNum >= PAL_MAX_CG)return 0;

	ret = g_CG_PalTbl[ubImgNum];
	
    return ret;
}
#endif	/* IF_GRAPHIC_C */