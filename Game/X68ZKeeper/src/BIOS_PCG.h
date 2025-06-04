#ifndef	BIOS_PCG_H
#define	BIOS_PCG_H

#include <usr_define.h>
#include <XSP2lib.H>

#define CNF_XSP (1)
#define	CNF_PCG_AUTO_ACCELERATION	(1)
#define	CNF_PCG_AUTO_UPDATE_OFF		(0)

/* �X�v���C�g�o�b�f�p�^�[���ő吔 */
#define	PCG_MAX		(512)
#define	PAL_MAX		(16)
#define PCG_LSB		(6)		/* bit 6 -512~511 */
#define BG_P0		(0)
#define BG_P1		(1)
#define	BG_WIDTH	(8)
#define	BG_HEIGHT	(8)
#define BG_0_W		(280)
#define BG_0_H		(63)
#define BG_1_W		(280)
#define BG_1_H		(63)
#define SP_W		(16)
#define SP_W2		(8)
#define SP_H		(16)
#define SP_H2		(8)
#define SP_X_OFFSET	(16)
#define SP_X_MAX	(256+SP_X_OFFSET)
#define SP_Y_OFFSET	(16)
#define SP_Y_MAX	(256+SP_Y_OFFSET)
#define SP_8_SIZE	(32)
#define SP_16_SIZE	(128)
#define SP_8x8		(0)
#define SP_16x16	(1)
#define	SP_PLN_MAX	(128)
#define	PCG_CODE_MASK	(0xFF)
#define	PCG_16x16_AREA	(0x30)
#define	PCG_LIST_MAX	(16)

/* �X�v���C�g�P���Ƃ̃f�[�^ */
typedef struct
{
	uint16_t	Plane;			/* �v���[���ԍ��̐擪 */
	int16_t		x;				/* x���W */
	int16_t		y;				/* y���W */
	int16_t		z;				/* y���W */
	int16_t		dx;				/* �ړ���x */
	int16_t		dy;				/* �ړ���y */
	int16_t		dz;				/* �ړ���z */
	int16_t 	speed;			/* ���x */
	int16_t 	angle;			/* �p�x */
	uint8_t		Anime;			/* ���݂̃A�j���[�V���� */
	uint8_t		Anime_old;		/* �O��̃A�j���[�V���� */
	uint8_t		status;			/* �X�e�[�^�X */
	uint8_t		update;			/* �X�V */
	uint8_t		validty;		/* �L�� */
	uint8_t		Pat_w;			/* �������̃p�^�[���� */
	uint8_t		Pat_h;			/* �c�����̃p�^�[���� */
	uint8_t		Pat_AnimeMax;	/* �A�j���[�V�����p�^�[���� */
	uint8_t		Pat_DataMax;	/* �p�^�[���f�[�^�� */
	uint16_t	*pPatData;		/* �p�^�[���f�[�^ */
	uint16_t	*pPatCodeTbl;	/* �p�^�[���R�[�h�e�[�u�� */
}	ST_PCG;

/* �X�v���C�g���X�g���Ƃ̃f�[�^ */
typedef struct
{
	uint8_t		Pal;			/* �p���b�g�ԍ� */
	uint8_t		Pat_w;			/* �������̃p�^�[���� */
	uint8_t		Pat_h;			/* �c�����̃p�^�[���� */
	uint8_t		Pat_AnimeMax;	/* �A�j���[�V�����p�^�[���� */
	int16_t		hit_x;			/* �����蔻��̎n�_x */
	int16_t		hit_y;			/* �����蔻��̎n�_y */
	int16_t		hit_width;		/* �����蔻��̕� */
	int16_t		hit_height;		/* �����蔻��̍��� */
}	ST_PCG_LIST;

extern	int8_t		g_sp_list[ PCG_LIST_MAX ][256u];
extern	uint32_t	g_sp_list_max;
extern	int8_t		g_pal_list[ PAL_MAX ][256u];
extern	uint32_t	g_pal_list_max;
extern	int8_t 		g_pcg_alt[ PCG_MAX + 1u ];	/* XSP �p PCG �z�u�Ǘ��e�[�u��	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B*/
extern	int8_t 		g_pcg_dat[ PCG_MAX * 128u ];	/* �o�b�f�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
extern	uint16_t 	g_pal_dat[ PAL_MAX * 16u ];	/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */

extern ST_PCG		g_stPCG_DATA[ PCG_MAX ];
extern ST_PCG_LIST	g_stST_PCG_LIST[ PCG_LIST_MAX ];

extern void PCG_VIEW(uint8_t);
extern void PCG_Rotation(uint16_t *, uint16_t *, uint8_t, uint8_t, int16_t, int16_t, uint8_t *, uint8_t, uint16_t, uint16_t);
extern int16_t PCG_Main(uint16_t);
extern ST_PCG *PCG_Get_Info(uint8_t);
extern int16_t PCG_Set_Info(ST_PCG, uint8_t);
extern void PCG_SP_PAL_DataLoad(void);
extern void PCG_Set_PAL_Data(int16_t, int16_t);
extern int16_t PCG_Load_Data(int8_t *, uint16_t , ST_PCG , uint16_t , uint8_t );
extern int16_t PCG_Load_PAL_Data(int8_t *, uint16_t, uint16_t);
extern int16_t PCG_PAL_Change(uint16_t , uint16_t , uint16_t);
#endif	/* BIOS_PCG_H */
