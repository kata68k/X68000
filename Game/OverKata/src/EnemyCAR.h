#ifndef	ENEMYCAR_H
#define	ENEMYCAR_H

#include "inc/usr_define.h"

#define	ENEMYCAR_MAX	(2)
#define ENEMY_CAR_0_W	(67)
#define ENEMY_CAR_0_H	(45)
#define ENEMY_CAR_1_W	(140)
#define ENEMY_CAR_1_H	(105)
#define	ENEMYCAR_PAT_MAX	(11)
#define	ENEMYCAR_TYP_MAX	(8)

/* 構造体定義 */

/* ライバル車の構造体 */
typedef struct
{
	uint8_t	ubCarType;			/* 車の種類 */
	int16_t	VehicleSpeed;		/* 車速 */
	int16_t	x;					/* X座標 */
	int16_t	y;					/* Y座標 */
	int16_t	z;					/* Z座標(倍率) */
	int16_t	sx;					/* X座標(当たり判定開始座標) */
	int16_t	ex;					/* X座標(当たり判定終了座標) */
	int16_t	sy;					/* Y座標(当たり判定開始座標) */
	int16_t	ey;					/* Y座標(当たり判定終了座標) */
	uint8_t	ubBrakeLights;		/* ブレーキライト */
	uint8_t	ubOBD;				/* 車両の状態 */
	uint8_t	ubAlive;			/* 表示の状態 */
}	ST_ENEMYCARDATA;

/* extern宣言 */
extern	int16_t	InitEnemyCAR(void);
extern	int16_t	GetEnemyCAR(ST_ENEMYCARDATA *, int16_t);
extern	int16_t	SetEnemyCAR(ST_ENEMYCARDATA, int16_t);
extern	int16_t	EnemyCAR_main(uint8_t, uint8_t, uint8_t);
extern	int16_t	SetAlive_EnemyCAR(void);
extern	int16_t	Put_EnemyCAR(uint16_t, uint16_t, uint16_t, uint8_t, uint8_t);
extern	int16_t	Sort_EnemyCAR(void);

#endif	/* ENEMYCAR_H */
