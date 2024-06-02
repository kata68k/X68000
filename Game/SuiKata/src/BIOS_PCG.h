#ifndef	BIOS_PCG_H
#define	BIOS_PCG_H

#include <usr_define.h>
#include <XSP2lib.H>

#define CNF_XSP (1)

/* �X�v���C�g�o�b�f�p�^�[���ő吔 */
#define	PCG_MAX		(256)
#define	PAL_MAX		(16)
#define BG_P0		(0)
#define BG_P1		(1)
#define	BG_WIDTH	(8)
#define	BG_HEIGHT	(8)
#define BG_0_W		(280)
#define BG_0_H		(63)
#define BG_1_W		(280)
#define BG_1_H		(63)
#define SP_W		(16)
#define SP_H		(16)
#define SP_X_OFFSET	(16)
#define SP_Y_OFFSET	(16)
#define SP_8_SIZE	(32)
#define SP_16_SIZE	(128)
#define SP_8x8		(0)
#define SP_16x16	(1)
#define	SP_PLN_MAX	(128)
#define	PCG_CODE_MASK	(0xFF)
#define	PCG_16x16_AREA	(0x00)

enum{
	SP_BALL_S_1,
	SP_BALL_S_2,
	SP_BALL_S_3,
	SP_BALL_S_4,
	SP_BALL_S_5,
	SP_BALL_S_6,
	SP_BALL_S_7,
	SP_BALL_S_8,
	SP_BALL_S_9,
	SP_BALL_S_10,
	SP_BALL_S_11,
	SP_BALL_S_12,
	SP_BALL_S_13,
	SP_BALL_S_14,
	SP_BALL_S_15,
	SP_BALL_S_16,
	SP_BALL_S_17,
	SP_BALL_S_18,
	SP_BALL_S_19,
	SP_BALL_S_20,
	SP_BALL_S_21,
	SP_BALL_S_22,
	SP_BALL_S_23,
	SP_BALL_S_24,
	SP_BALL_S_25,
	SP_BALL_S_26,
	SP_BALL_S_27,
	SP_BALL_S_28,
	SP_BALL_S_29,
	SP_BALL_S_30,
	SP_BALL_S_31,
	SP_BALL_S_32,
	SP_BALL_M_1,
	SP_BALL_M_2,
	SP_BALL_M_3,
	SP_BALL_M_4,
	SP_BALL_M_5,
	SP_BALL_M_6,
	SP_BALL_M_7,
	SP_BALL_M_8,
	SP_BALL_M_9,
	SP_BALL_M_10,
	SP_BALL_M_11,
	SP_BALL_M_12,
	SP_BALL_M_13,
	SP_BALL_M_14,
	SP_BALL_M_15,
	SP_BALL_M_16,
	SP_BALL_L_1,
	SP_BALL_L_2,
	SP_BALL_L_3,
	SP_BALL_L_4,
	SP_BALL_L_5,
	SP_BALL_L_6,
	SP_BALL_L_7,
	SP_BALL_L_8,
	SP_BALL_XL_1,
	SP_BALL_XL_2,
	SP_BALL_XL_3,
	SP_BALL_XL_4,
	SP_SHIP_0,
	SP_GAL_0,
	SP_ARROW_0,
	PCG_NUM_MAX,
};

typedef struct
{
	uint16_t	Plane;			/* �v���[���ԍ��̐擪 */
	uint16_t	x;				/* x���W */
	uint16_t	y;				/* y���W */
	int16_t		dx;				/* �ړ���x */
	int16_t		dy;				/* �ړ���y */
	uint8_t		Anime;			/* ���݂̃A�j���[�V���� */
	uint8_t		Anime_old;		/* �O��̃A�j���[�V���� */
	uint8_t		update;			/* �X�V */
	uint8_t		validty;		/* �L�� */
	uint8_t		Pat_w;			/* �������̃p�^�[���� */
	uint8_t		Pat_h;			/* �c�����̃p�^�[���� */
	uint8_t		Pat_AnimeMax;	/* �A�j���[�V�����p�^�[���� */
	uint8_t		Pat_DataMax;	/* �p�^�[���f�[�^�� */
	uint16_t	*pPatData;		/* �p�^�[���f�[�^ */
	uint32_t	*pPatCodeTbl;	/* �p�^�[���R�[�h�e�[�u�� */
}	ST_PCG;

typedef struct
{
	uint8_t		Pal;			/* �p���b�g�ԍ� */
	uint8_t		Pat_w;			/* �������̃p�^�[���� */
	uint8_t		Pat_h;			/* �c�����̃p�^�[���� */
	uint8_t		Pat_AnimeMax;	/* �A�j���[�V�����p�^�[���� */
}	ST_PCG_LIST;

extern ST_PCG	g_stPCG_DATA[PCG_MAX];
extern ST_PCG_LIST	g_stST_PCG_LIST[16];

extern void PCG_VIEW(uint8_t);
extern void PCG_Rotation(uint16_t *, uint16_t *, uint8_t, uint8_t, int16_t, int16_t, uint8_t *, uint8_t, uint16_t, uint16_t);
extern int16_t PCG_Main(void);
extern ST_PCG *PCG_Get_Info(uint8_t);
extern int16_t PCG_Set_Info(ST_PCG, uint8_t);
extern int16_t PCG_Load_Data(int8_t *, uint16_t , ST_PCG , uint16_t , uint8_t );
extern int16_t PCG_Load_PAL_Data(int8_t *, uint16_t, uint16_t);
extern int16_t PCG_PAL_Change(uint16_t , uint16_t , uint16_t);
#endif	/* BIOS_PCG_H */
