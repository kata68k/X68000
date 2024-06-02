#ifndef	USR_DEFINE_H
#define	USR_DEFINE_H

#include "usr_style.h"

#define	DEBUG	/* �f�o�b�O�L����` */

#define FALSE	(0)
#define TRUE	(!FALSE)

#define Bit_0		(0x1)
#define Bit_1		(0x2)
#define Bit_2		(0x4)
#define Bit_3		(0x8)
#define Bit_4		(0x10)
#define Bit_5		(0x20)
#define Bit_6		(0x40)
#define Bit_7		(0x80)
#define Bit_8		(0x100)
#define Bit_9		(0x200)
#define Bit_10		(0x400)
#define Bit_11		(0x800)
#define Bit_12		(0x1000)
#define Bit_13		(0x2000)
#define Bit_14		(0x4000)
#define Bit_15		(0x8000)

#define Bit_Mask_8	(0x000000FF)
#define Bit_Mask_9	(0x000001FF)
#define Bit_Mask_10	(0x000003FF)

/* �w�C�x���W�i�\�����W�j */
#define V_SYNC_MAX		(240)
#define WIDTH			(256)
#define HEIGHT			(256)
#define Y_HORIZON_0		(128)
#define Y_HORIZON_1		(64)
#define Y_MAX_WINDOW	(256)	/* Y_HORIZON_1+ROAD_SIZE */

/* �w�C�x���W�i���z���W�j */
#define X_OFFSET	(128)
#define VX_OFFSET	(4096)
#define X_MIN		X_OFFSET
#define X_MAX		(WIDTH+X_OFFSET)
#define X_MAX_H		((WIDTH>>1)+X_OFFSET)
#define VX_MIN		(0)	/* (0 + 16)*256 */
#define VX_MAX		(65536)	/* (256 - 16)*256 */

#define Y_OFFSET	(256)
#define VY_OFFSET	(4096)
#define Y_MIN		Y_OFFSET
#define Y_MAX		(HEIGHT+Y_OFFSET)
#define Y_MAX_H		((HEIGHT>>1)+Y_OFFSET)
#define VY_MIN		(0)	/* (0 + 16)*256 */
#define VY_MAX		(65536)	/* (256 - 16)*256 */

/* ���X�^���(256*256 31kHz�̏ꍇ ���C����2�{�v�Z) */
#define RASTER_NEXT	(2)
#define RASTER_MIN	(16)			//�\���J�n�ʒu(16)	/* ���X�^�J�n�ʒu�́ACRTC��R6�Ɠ��� */
#define RASTER_L_MAX	(256)		//�\���I���ʒu(240)
#define RASTER_H_MAX	(512)		//�\���I���ʒu(480)

#define ROAD_ST_POINT	(128)	/* BG�̓��̊J�n�ʒu */
#define ROAD_MD_POINT	(176)	/* BG�̓��̒��Ԉʒu */
#define ROAD_ED_POINT	(224)	/* BG�̓��̏I���ʒu */
#define ROAD_CT_POINT	(124)	/* BG�̓��̒��S�ʒu */
#define ROAD_SIZE		(ROAD_ED_POINT-ROAD_ST_POINT)	/* BG�̓��̃T�C�Y(96) */
#define ROAD_0_MIN		(RASTER_MIN)	/* BG�̓��̍ŏ�(mode0)���� */
#define ROAD_0_MAX		(V_SYNC_MAX)	/* BG�̓��̍ő�(mode0)���� */
#define ROAD_1_MIN		(0)				/* BG�̓��̍ŏ�(mode1) */
#define ROAD_1_MAX		(128)			/* BG�̓��̍ő�(mode1) */

/* �\���� */

/* �f�[�^�e�[�u�� */

extern void (*usr_abort)(void);	/* ���[�U�̃A�{�[�g�����֐� */

#endif	/* USR_DEFINE_H */
