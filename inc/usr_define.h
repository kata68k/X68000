#ifndef	USR_DEFINE_H
#define	USR_DEFINE_H

#define WIDTH		(256)
#define HEIGHT		(256)

/* Ｘ，Ｙ座標の最大と最小 */
#define X_MIN_DRAW	(0)
#define X_MAX_DRAW	(511)
#define X_OFFSET	(128)
#define X_MIN		X_OFFSET
#define X_MAX		(WIDTH+X_OFFSET)
#define X_MAX_H		((WIDTH>>1)+X_OFFSET)

#define Y_MIN_DRAW	(0)
#define Y_MAX_DRAW	(511)
#define Y_OFFSET	(128)
#define Y_MIN		Y_OFFSET
#define Y_MAX		(HEIGHT+Y_OFFSET)
#define Y_MAX_H		((HEIGHT>>1)+Y_OFFSET)
#define Y_HORIZON	(Y_MAX_H)

/* ラスタ情報(256*256 31kHzの場合 ラインは2倍計算) */
#define RASTER_NEXT	(24)
#define RASTER_SIZE	(256)
#define RASTER_MIN	(40)
#define RASTER_MAX	(RASTER_MIN + 512)
#define RASTER_ST	(Y_MAX_H+RASTER_MIN-20-2)
#define RASTER_ED	(RASTER_ST+RASTER_SIZE)

/* ジョイスティック１のその１ */
#define UP		(0x01)
#define DOWN	(0x02)
#define LEFT	(0x04)
#define RIGHT	(0x08)
#define JOYA	(0x20)
#define JOYB	(0x40)
#define ESC_S	(27)

/* ジョイスティック１のその１ */
#define KEY_NULL		(0x00)
#define KEY_UPPER		(0x04)
#define KEY_LOWER		(0x08)
#define KEY_LEFT		(0x01)
#define KEY_UP_LEFT		(0x05)
#define KEY_DW_LEFT		(0x09)
#define KEY_RIGHT		(0x02)
#define KEY_UP_RIGHT	(0x06)
#define KEY_DW_RIGHT	(0x0A)
#define KEY_A			(0x10)
#define KEY_B			(0x20)

#define KEY_TRUE	1
#define KEY_FALSE	0

#endif	/* USR_DEFINE_H */
