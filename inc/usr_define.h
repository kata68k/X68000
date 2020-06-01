#ifndef	USR_DEFINE_H
#define	USR_DEFINE_H

#include "usr_style.h"

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

/* Ｘ，Ｙ座標（表示座標） */
#define V_SYNC_MAX	(240)
#define WIDTH		(256)
#define HEIGHT		(256)
#define X_MIN_DRAW	(0)
#define X_MAX_DRAW	(511)
#define Y_MIN_DRAW	(0)
#define Y_MAX_DRAW	(511)
#define Y_HORIZON_0	(128)
#define Y_HORIZON_1	(64)

/* Ｘ，Ｙ座標（仮想座標） */
#define X_OFFSET	(224)
#define X_MIN		X_OFFSET
#define X_MAX		(WIDTH+X_OFFSET)
#define X_MAX_H		((WIDTH>>1)+X_OFFSET)

#define Y_OFFSET	(256)
#define Y_MIN		Y_OFFSET
#define Y_MAX		(HEIGHT+Y_OFFSET)
#define Y_MAX_H		((HEIGHT>>1)+Y_OFFSET)

/* ラスタ情報(256*256 31kHzの場合 ラインは2倍計算) */
#define RASTER_NEXT	(2)
#define RASTER_MIN	(16)//表示開始位置(40,16)
#define RASTER_MAX	(256)//表示終了位置(552,256)
#define RASTER_SIZE	(RASTER_MAX-RASTER_MIN)
#define ROAD_SIZE	(96)
#define ROAD_POINT	(128)

/* スプライトＰＣＧパターン最大数 */
#define	PCG_MAX	(256)

#define	BG_WIDTH	(8)
#define	BG_HEIGHT	(8)

/* 画像サイズ */
#define MY_CAR_0_W	(65)
#define MY_CAR_0_H	(52)
#define MY_CAR_1_W	(256)
#define MY_CAR_1_H	(256)
#define ENEMY_CAR_0_W	(67)
#define ENEMY_CAR_0_H	(45)
#define ENEMY_CAR_1_W	(140)
#define ENEMY_CAR_1_H	(105)
#define BG_0_W	(280)
#define BG_0_H	(63)
#define BG_1_W	(280)
#define BG_1_H	(63)
#define BG_0_UNDER	(280)	/* 未決 */
#define BG_1_UNDER	(64)

/* 構造体 */

/* コースの構造体 */
typedef struct
{
	UC	bHeight;	/* 道の標高	(0x80センター) */
	UC	bWidth;		/* 道の幅	(0x80センター) */
	UC	bAngle;		/* 道の角度	(0x80センター) */
	UC	bfriction;	/* 道の摩擦	(0x80センター) */
	UC	bPat;		/* 道の種類	 */
	UC	bObject;	/* 出現ポイントのオブジェクトの種類 */
	UC	bRepeatCount;	/* 繰り返し回数 */
}	ST_ROADDATA;

/* 車の構造体 */
typedef struct
{
	UC	ubCarType;			/* 車の種類 */
	US	uEngineRPM;			/* エンジン回転数 */
	SS	VehicleSpeed;		/* 車速 */
	SS	Steering;			/* ステア */
	UC	ubShiftPos;			/* ギア段 */
	SC	bThrottle;			/* スロットル開度 */
	UC	ubBrakeLights;		/* ブレーキライト */
	UC	ubHeadLights;		/* ヘッドライト */
	UC	ubWiper;			/* ワイパー */
	SC	bTire;				/* タイヤの状態 */
	UC	ubOBD;				/* 故障の状態 */
}	ST_CARDATA;



/* データテーブル */


#endif	/* USR_DEFINE_H */
