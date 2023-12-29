#ifndef	APL_PCG_C
#define	APL_PCG_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>
#include "BIOS_PCG.h"
#include "IF_FileManager.h"
#include "IF_Math.h"
#include "APL_PCG.h"

/* define��` */

/* �O���[�o���ϐ� */

/* �֐��̃v���g�^�C�v�錾 */
void PCG_INIT_CHAR(void);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void PCG_INIT_CHAR(void)
{
	uint32_t	i, j;
	uint32_t	uPCG_num, uBufSize;
	uint8_t		ubOK;
	uint8_t		ubPri;
	uint8_t		ubPal;

	printf("SP load");
#if 0
	PCG_SP_dataload("data/sp/BG.SP");	/* �X�v���C�g�̃f�[�^�ǂݍ��� */
	PCG_PAL_dataload("data/sp/BG.PAL");	/* �X�v���C�g�̃p���b�g�f�[�^�ǂݍ��� */
	BG_TEXT_SET("data/map/map.csv");	/* �}�b�v�f�[�^�ɂ��a�f�̔z�u */
#endif
#if 0
	for(uPCG_num = 0; uPCG_num < PCG_MAX; uPCG_num++)
	{
		g_stPCG_DATA[uPCG_num].x				= (uPCG_num * 16) & 0xFF;	/* x���W */
		g_stPCG_DATA[uPCG_num].y				= (uPCG_num * 16) & 0xFF;	/* y���W */
		g_stPCG_DATA[uPCG_num].ratio			= 0;			/* �䗦 */
		g_stPCG_DATA[uPCG_num].rad			= 0;			/* ��] */
		g_stPCG_DATA[uPCG_num].Pat_w			= 1;			/* �������̃p�^�[���� */
		g_stPCG_DATA[uPCG_num].Pat_h			= 1;			/* �c�����̃p�^�[���� */
		g_stPCG_DATA[uPCG_num].pPatCodeTbl	= MyMalloc(sizeof(uint32_t));
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl)	= SetBGcode(0, 0, 0x0C, uPCG_num);	/* �p�^�[���R�[�h�e�[�u�� */
		g_stPCG_DATA[uPCG_num].update		= TRUE;			/* �X�V */
		g_stPCG_DATA[uPCG_num].validty		= TRUE;		/* �L�� */
	}
#endif
	
#if 0
	for(uPCG_num = 0; uPCG_num < 14; uPCG_num++)
	{
		g_stPCG_DATA[uPCG_num].x				= 16 * uPCG_num;	/* x���W */
		g_stPCG_DATA[uPCG_num].y				= 128;	/* y���W */
		g_stPCG_DATA[uPCG_num].ratio			= 0;	/* �䗦 */
		g_stPCG_DATA[uPCG_num].rad			= 0;	/* ��] */
		g_stPCG_DATA[uPCG_num].Pat_w			= 3;	/* �������̃p�^�[���� */
		g_stPCG_DATA[uPCG_num].Pat_h			= 3;	/* �c�����̃p�^�[���� */
		g_stPCG_DATA[uPCG_num].Pat_size		= 1;	/* �p�^�[���T�C�Y 0:8x8 1:16x16 */
		g_stPCG_DATA[uPCG_num].pPatCodeTbl	= MyMalloc(sizeof(uint32_t) * g_stPCG_DATA[uPCG_num].Pat_w * g_stPCG_DATA[uPCG_num].Pat_h);
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 0)	= SetBGcode(0, 0, 0x09, 0x40);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 1)	= SetBGcode(0, 0, 0x09, 0x41);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 2)	= SetBGcode(0, 0, 0x09, 0x42);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 3)	= SetBGcode(0, 0, 0x09, 0x50);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 4)	= SetBGcode(0, 0, 0x09, 0x51);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 5)	= SetBGcode(0, 0, 0x09, 0x52);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 6)	= SetBGcode(0, 0, 0x09, 0x60);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 7)	= SetBGcode(0, 0, 0x09, 0x61);	/* �p�^�[���R�[�h�e�[�u�� */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 8)	= SetBGcode(0, 0, 0x09, 0x62);	/* �p�^�[���R�[�h�e�[�u�� */
		g_stPCG_DATA[uPCG_num].update		= TRUE;		/* �X�V */
		g_stPCG_DATA[uPCG_num].validty		= TRUE;		/* �L�� */
	}
#endif
	
#if 0
	for(uPCG_num = 0; uPCG_num < PCG_NUM_MAX; uPCG_num++)
	{
		FILE *fp;

		fp = fopen( "data/sp/BG.SP" , "rb" ) ;
		
		g_stPCG_DATA[uPCG_num].x			= SP_X_OFFSET + ((SP_W/2) * uPCG_num);	/* x���W */
		g_stPCG_DATA[uPCG_num].y			= SP_Y_OFFSET + 128;	/* y���W */
		g_stPCG_DATA[uPCG_num].Pat_w		= 1;	/* �������̃p�^�[���� */
		g_stPCG_DATA[uPCG_num].Pat_h		= 1;	/* �c�����̃p�^�[���� */
		g_stPCG_DATA[uPCG_num].Pat_AnimeMax	= 1;
		/* �������m�� */
		uBufSize = g_stPCG_DATA[uPCG_num].Pat_w * g_stPCG_DATA[uPCG_num].Pat_h;
		/* �X�v���C�g��`�ݒ� */
		g_stPCG_DATA[uPCG_num].pPatCodeTbl	= (uint32_t*)MyMalloc(sizeof(uint32_t) * uBufSize);
		for(j=0; j < uBufSize; j++)
		{
			*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x00, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
		}
		/* �X�v���C�g�摜�f�[�^ */
		uAnimeBuf = uBufSize * g_stPCG_DATA[uPCG_num].Pat_AnimeMax;
		g_stPCG_DATA[uPCG_num].pPatData	= (uint16_t*)MyMalloc(SP_16_SIZE * uAnimeBuf);
		memset(g_stPCG_DATA[uPCG_num].pPatData, 0, SP_16_SIZE * uAnimeBuf);
		
		fseek(fp, (0x20 * SP_16_SIZE) + (uPCG_num * (SP_16_SIZE / 2)), SEEK_SET);
		fread( g_stPCG_DATA[uPCG_num].pPatData
				,  (SP_16_SIZE / 2)		/* 1PCG = �������T�C�Y */
				,  uAnimeBuf			/* ���p�^�[���g���̂��H */
				,  fp
				);
		
		fclose( fp ) ;
		
		g_stPCG_DATA[uPCG_num].Plane		= 0xFF;		/* �v���[��No. */
		g_stPCG_DATA[uPCG_num].update		= TRUE;		/* �X�V */
		g_stPCG_DATA[uPCG_num].validty		= TRUE;		/* �L�� */
	}
#endif
	
#if 1
	srand(1);
	
	/* ���ԃX�v���C�g */
	for(uPCG_num = 0; uPCG_num < PCG_NUM_MAX; uPCG_num++)
	{
		ST_PCG stPCG;
		
		/* �����܂��������͕s�v */
		stPCG.x			= 0;	/* x���W */
		stPCG.y			= 0;	/* y���W */
		stPCG.dx		= 0;	/* �ړ���x */
		stPCG.dy		= 0;	/* �ړ���y */
		stPCG.Anime		= 0;	/* ���݂̃A�j�� */
		stPCG.Anime_old	= 0;	/* �O��̃A�j�� */
		
		switch(uPCG_num)
		{
			case	SP_BALL_1	:
			case	SP_BALL_2	:
			case	SP_BALL_3	:
			case	SP_BALL_4	:
			case	SP_BALL_5	:
			case	SP_BALL_6	:
			case	SP_BALL_7	:
			case	SP_BALL_8	:
			case	SP_BALL_9	:
			case	SP_BALL_10	:
			case	SP_BALL_11	:
			case	SP_BALL_12	:
			case	SP_BALL_13	:
			case	SP_BALL_14	:
			case	SP_BALL_15	:
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_1)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0A, 0x0A);
				}
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0A, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_16	:
			case	SP_BALL_17	:
			case	SP_BALL_18	:
			case	SP_BALL_19	:
			case	SP_BALL_20	:
			case	SP_BALL_21	:
			case	SP_BALL_22	:
			case	SP_BALL_23	:
			case	SP_BALL_24	:
			case	SP_BALL_25	:
			case	SP_BALL_26	:
			case	SP_BALL_27	:
			case	SP_BALL_28	:
			case	SP_BALL_29	:
			case	SP_BALL_30	:
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_16)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0B, 0x0B);
				}				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0B, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_31	:
			case	SP_BALL_32	:
			case	SP_BALL_33	:
			case	SP_BALL_34	:
			case	SP_BALL_35	:
			case	SP_BALL_36	:
			case	SP_BALL_37	:
			case	SP_BALL_38	:
			case	SP_BALL_39	:
			case	SP_BALL_40	:
			case	SP_BALL_41	:
			case	SP_BALL_42	:
			case	SP_BALL_43	:
			case	SP_BALL_44	:
			case	SP_BALL_45	:
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_31)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0C, 0x0C);
				}				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0C, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_46	:
			case	SP_BALL_47	:
			case	SP_BALL_48	:
			case	SP_BALL_49	:
			case	SP_BALL_50	:
			case	SP_BALL_51	:
			case	SP_BALL_52	:
			case	SP_BALL_53	:
			case	SP_BALL_54	:
			case	SP_BALL_55	:
			case	SP_BALL_56	:
			case	SP_BALL_57	:
			case	SP_BALL_58	:
			case	SP_BALL_59	:
			case	SP_BALL_60	:
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_46)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0D, 0x0D);
				}
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0D, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_61	:
			case	SP_BALL_62	:
			case	SP_BALL_63	:
			case	SP_BALL_64	:
			case	SP_BALL_65	:
			case	SP_BALL_66	:
			case	SP_BALL_67	:
			case	SP_BALL_68	:
			case	SP_BALL_69	:
			case	SP_BALL_70	:
			case	SP_BALL_71	:
			case	SP_BALL_72	:
			case	SP_BALL_73	:
			case	SP_BALL_74	:
			case	SP_BALL_75	:
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_61)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0E, 0x0E);
				}				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0E, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
#if 0
			case MYCAR_PCG_NEEDLE:	/* �^�R���[�^�[�j */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 32;
				stPCG.Pat_DataMax	= 8;
				
				PCG_Load_Data("data/sp/BG.SP", 0x40, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0D, 0x0D);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					if(j < 8)
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(1, 1, 0x0D, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
					else if(j < 16)
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 1, 0x0D, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
					else if(j < 24)
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0D, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
					else
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(1, 0, 0x0D, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case MYCAR_PCG_TACHO:	/* �^�R���[�^�[ */
			{
				stPCG.Pat_w		= 2;
				stPCG.Pat_h		= 2;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 4;
				
				PCG_Load_Data("data/sp/BG.SP", 0x48, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0D, 0x0D);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0D, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}

				ubOK = TRUE;
				break;
			}
			case MYCAR_PCG_STEERING_POS:	/* �X�e�A�����O�|�W�V���� */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BG.SP", 0x3F, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0C, 0x0C);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0C, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case MYCAR_PCG_STEERING:	/* �X�e�A�����O */
			{
				stPCG.Pat_w			= 2;
				stPCG.Pat_h			= 2;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 4;
				
				PCG_Load_Data("data/sp/BG.SP", 0x4C, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0B, 0x0B);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0B, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case SCORE_PCG_1:	/* �X�R�A�P */
			case SCORE_PCG_2:	/* �X�R�A�Q */
			case SCORE_PCG_3:	/* �X�R�A�R */
			case SCORE_PCG_4:	/* �X�R�A�S */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 10;
				stPCG.Pat_DataMax	= 10;
				
				PCG_Load_Data("data/sp/BG.SP", 0x20, stPCG, uPCG_num, 1);
				/* �e�L�X�g���p�̂O�ԃp���b�g���g�p */
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x00, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case ROAD_PCG_ARROW:				/* ��� */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BG.SP", 0xB2, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x07, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case ROAD_PCG_SIGNAL_1:	/* �V�O�i�������v1 */
			case ROAD_PCG_SIGNAL_2:	/* �V�O�i�������v2 */
			case ROAD_PCG_SIGNAL_3:	/* �V�O�i�������v3 */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BG.SP", 0x3E, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x01, 0x01);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0A, 0x0A);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0A, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}

				ubOK = TRUE;
				break;
			}
			case OBJ_SHADOW:	/* �e1*3 */
			{
				stPCG.Pat_w			= 3;
				stPCG.Pat_h			= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 3;	/* w x h x AnimeMax = DataMax */
				
				PCG_Load_Data("data/sp/BG.SP", 0x98, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0B, 0x0B);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0B, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case ETC_PCG_SONIC:				/* �\�j�b�N */
			{
				stPCG.Pat_w			= 3;
				stPCG.Pat_h			= 3;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 9;
				
				PCG_Load_Data("data/sp/BG.SP", 0xA0, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x09, 0x09);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x09, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case ETC_PCG_KISARA:				/* �L�T�� */
			{
				stPCG.Pat_w			= 2;
				stPCG.Pat_h			= 4;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 8;
				
				PCG_Load_Data("data/sp/BG.SP", 0x90, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x07, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
			case ETC_PCG_MAN:				/* �s�N�g�N */
			{
				stPCG.x			= SP_X_OFFSET + 128;	/* x���W */
				stPCG.y			= SP_Y_OFFSET + 128;	/* y���W */
				stPCG.Anime		= 0;	/* ���݂̃A�j�� */
				stPCG.Anime_old	= 0;	/* �O��̃A�j�� */
				stPCG.Pat_w		= 2;
				stPCG.Pat_h		= 4;
				stPCG.Pat_AnimeMax	= 8;
				stPCG.Pat_DataMax	= 8*8;
				
				PCG_Load_Data("data/sp/BG.SP", 0x50, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* �X�v���C�g��`�ݒ� */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x07, 0xFF);	/* �p�^�[���R�[�h�e�[�u�� */
				}
				
				ubOK = TRUE;
				break;
			}
#endif
			default:
			{

				ubOK = FALSE;
				break;
			}
		}
		
		if(ubOK == TRUE)
		{
			g_stPCG_DATA[uPCG_num].update	= FALSE;
			g_stPCG_DATA[uPCG_num].validty	= TRUE;
		}
		else
		{
			g_stPCG_DATA[uPCG_num].Pat_w		= 0;
			g_stPCG_DATA[uPCG_num].Pat_h		= 0;
			g_stPCG_DATA[uPCG_num].update	= FALSE;
			g_stPCG_DATA[uPCG_num].validty	= FALSE;
		}
		g_stPCG_DATA[uPCG_num].Plane		= 0xFF;		/* �v���[��No. */

		printf(".");
#ifdef DEBUG
//		printf("g_stPCG_DATA[%d]=0x%p\n", uPCG_num, &g_stPCG_DATA[uPCG_num]);
//		KeyHitESC();	/* �f�o�b�O�p */
#endif
	}
#endif
	printf("Ok!\n", uPCG_num, &g_stPCG_DATA[uPCG_num]);
}

#endif	/* APL_PCG_C */

