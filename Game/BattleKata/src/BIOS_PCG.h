#ifndef	BIOS_PCG_H
#define	BIOS_PCG_H

#include <usr_define.h>

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

enum{
	MYCAR_PCG_NEEDLE,			/* �^�R���[�^�[�j */
	MYCAR_PCG_TACHO,			/* �^�R���[�^�[ */
	MYCAR_PCG_STEERING_POS,		/* �X�e�A�����O�|�W�V���� */
	MYCAR_PCG_STEERING,			/* �X�e�A�����O */
	SCORE_PCG_1,				/* �X�R�A�P */
	SCORE_PCG_2,				/* �X�R�A�Q */
	SCORE_PCG_3,				/* �X�R�A�R */
	SCORE_PCG_4,				/* �X�R�A�S */
	ROAD_PCG_ARROW,				/* ��� */
	ROAD_PCG_SIGNAL_1,			/* �V�O�i�������v */
	ROAD_PCG_SIGNAL_2,			/* �V�O�i�������v */
	ROAD_PCG_SIGNAL_3,			/* �V�O�i�������v */
	OBJ_SHADOW,					/* �e1*3 */
	ETC_PCG_SONIC,				/* �\�j�b�N */
	ETC_PCG_KISARA,				/* �L�T�� */
	ETC_PCG_MAN,				/* �s�N�g�N */
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
