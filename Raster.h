#ifndef	RASTER_H
#define	RASTER_H

#include "inc/usr_define.h"

#define	ROADDATA_MAX	(1024u)

/* �\���̒�` */
/* ���X�^�[���̍\���� */
typedef struct
{
	uint16_t	st;			/* �J�n�ʒu */
	uint16_t	mid_H;		/* ���Ԉʒu(H) */
	uint16_t	mid;		/* ���Ԉʒu(L) */
	uint16_t	ed;			/* �I���ʒu */
	uint16_t	size;		/* �J�n�ƏI���̍��� */
}	ST_RAS_INFO;

/* ���X�^�[���荞�ݏ��̍\���� */
typedef struct
{
	uint16_t	x;
	uint16_t	y;
	uint16_t	pat;
}	ST_RASTER_INT;

/* ���[�h���̍\���� */
typedef struct
{
	int16_t	Horizon;		/* �����ʒu */
	int16_t	Horizon_Base;	/* ������ʒu */
	int16_t	offset_x;		/*  */
	int16_t	offset_y;		/*  */
	int16_t	offset_val;		/*  */
	int16_t	height;			/*  */
	int16_t	slope;			/*  */
	int16_t	angle;			/*  */
	int16_t	distance;		/*  */
	int16_t	object;			/*  */
	
}	ST_ROAD_INFO;

/* �R�[�X�̍\���� */
typedef struct
{
	uint8_t	bHeight;	/* ���̕W��	(0x80�Z���^�[) */
	uint8_t	bWidth;		/* ���̕�	(0x80�Z���^�[) */
	uint8_t	bAngle;		/* ���̊p�x	(0x80�Z���^�[) */
	uint8_t	bfriction;	/* ���̖��C	(0x80�Z���^�[) */
	uint8_t	bPat;		/* ���̎��	 */
	uint8_t	bObject;	/* �o���|�C���g�̃I�u�W�F�N�g�̎�� */
	uint8_t	bRepeatCount;	/* �J��Ԃ��� */
}	ST_ROADDATA;


/* extern�錾 */
extern void	Raster_Init(void);
extern void Raster_Main(uint8_t);
extern int16_t GetRasterInfo(ST_RAS_INFO *);
extern int16_t SetRasterInfo(ST_RAS_INFO);
extern int16_t GetRasterPos(uint16_t *, uint16_t *, uint16_t);

extern int16_t	GetRoadInfo(ST_ROAD_INFO *);
extern int16_t	SetRoadInfo(ST_ROAD_INFO);
extern void Road_Init(uint16_t);
extern uint64_t GetRoadDataAddr(void);
extern int16_t GetRoadCycleCount(uint16_t *);

#endif	/* RASTER_H */