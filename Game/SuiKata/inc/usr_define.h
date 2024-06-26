#ifndef	USR_DEFINE_H
#define	USR_DEFINE_H

#include "usr_style.h"

#define	DEBUG	/* デバッグ有効定義 */

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

/* Ｘ，Ｙ座標（表示座標） */
#define V_SYNC_MAX		(240)
#define WIDTH			(256)
#define HEIGHT			(256)
#define Y_HORIZON_0		(128)
#define Y_HORIZON_1		(64)
#define Y_MAX_WINDOW	(256)	/* Y_HORIZON_1+ROAD_SIZE */

/* Ｘ，Ｙ座標（仮想座標） */
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

/* ラスタ情報(256*256 31kHzの場合 ラインは2倍計算) */
#define RASTER_NEXT	(2)
#define RASTER_MIN	(16)			//表示開始位置(16)	/* ラスタ開始位置は、CRTCのR6と同じ */
#define RASTER_L_MAX	(256)		//表示終了位置(240)
#define RASTER_H_MAX	(512)		//表示終了位置(480)

#define ROAD_ST_POINT	(128)	/* BGの道の開始位置 */
#define ROAD_MD_POINT	(176)	/* BGの道の中間位置 */
#define ROAD_ED_POINT	(224)	/* BGの道の終了位置 */
#define ROAD_CT_POINT	(124)	/* BGの道の中心位置 */
#define ROAD_SIZE		(ROAD_ED_POINT-ROAD_ST_POINT)	/* BGの道のサイズ(96) */
#define ROAD_0_MIN		(RASTER_MIN)	/* BGの道の最小(mode0)未定 */
#define ROAD_0_MAX		(V_SYNC_MAX)	/* BGの道の最大(mode0)未定 */
#define ROAD_1_MIN		(0)				/* BGの道の最小(mode1) */
#define ROAD_1_MAX		(128)			/* BGの道の最大(mode1) */

/* 構造体 */

/* データテーブル */

extern void (*usr_abort)(void);	/* ユーザのアボート処理関数 */

#endif	/* USR_DEFINE_H */
