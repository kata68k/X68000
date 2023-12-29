#ifndef	BIOS_PCG_H
#define	BIOS_PCG_H

#include <usr_define.h>
#include <XSP2lib.H>

#define CNF_XSP (0)

/* �X�v���C�g�o�b�f�p�^�[���ő吔 */
#define	PCG_MAX		(128)
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
	SP_BALL_1,
	SP_BALL_2,
	SP_BALL_3,
	SP_BALL_4,
	SP_BALL_5,
	SP_BALL_6,
	SP_BALL_7,
	SP_BALL_8,
	SP_BALL_9,
	SP_BALL_10,
	SP_BALL_11,
	SP_BALL_12,
	SP_BALL_13,
	SP_BALL_14,
	SP_BALL_15,
	SP_BALL_16,
	SP_BALL_17,
	SP_BALL_18,
	SP_BALL_19,
	SP_BALL_20,
	SP_BALL_21,
	SP_BALL_22,
	SP_BALL_23,
	SP_BALL_24,
	SP_BALL_25,
	SP_BALL_26,
	SP_BALL_27,
	SP_BALL_28,
	SP_BALL_29,
	SP_BALL_30,
	SP_BALL_31,
	SP_BALL_32,
	SP_BALL_33,
	SP_BALL_34,
	SP_BALL_35,
	SP_BALL_36,
	SP_BALL_37,
	SP_BALL_38,
	SP_BALL_39,
	SP_BALL_40,
	SP_BALL_41,
	SP_BALL_42,
	SP_BALL_43,
	SP_BALL_44,
	SP_BALL_45,
	SP_BALL_46,
	SP_BALL_47,
	SP_BALL_48,
	SP_BALL_49,
	SP_BALL_50,
	SP_BALL_51,
	SP_BALL_52,
	SP_BALL_53,
	SP_BALL_54,
	SP_BALL_55,
	SP_BALL_56,
	SP_BALL_57,
	SP_BALL_58,
	SP_BALL_59,
	SP_BALL_60,
	SP_BALL_61,
	SP_BALL_62,
	SP_BALL_63,
	SP_BALL_64,
	SP_BALL_65,
	SP_BALL_66,
	SP_BALL_67,
	SP_BALL_68,
	SP_BALL_69,
	SP_BALL_70,
	SP_BALL_71,
	SP_BALL_72,
	SP_BALL_73,
	SP_BALL_74,
	SP_BALL_75,
	PCG_NUM_MAX,
};

typedef struct
{
	uint8_t		Plane;			/* �v���[���ԍ��̐擪 */
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

extern ST_PCG	g_stPCG_DATA[PCG_MAX];

extern void PCG_VIEW(uint8_t);
extern void PCG_Rotation(uint16_t *, uint16_t *, uint8_t, uint8_t, int16_t, int16_t, uint8_t *, uint8_t, uint16_t, uint16_t);
extern void BG_TEXT_SET(int8_t *);
extern int16_t PCG_Main(void);
extern ST_PCG *PCG_Get_Info(uint8_t);
extern int16_t PCG_Set_Info(ST_PCG, uint8_t);
extern int16_t PCG_Load_Data(int8_t *, uint16_t , ST_PCG , uint16_t , uint8_t );
extern int16_t PCG_Load_PAL_Data(int8_t *, uint16_t, uint16_t);
extern int16_t PCG_PAL_Change(uint16_t , uint16_t , uint16_t);
#endif	/* BIOS_PCG_H */
