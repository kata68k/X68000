#ifndef	ENEMYCAR_H
#define	ENEMYCAR_H

#include "inc/usr_define.h"

#define	ENEMYCAR_MAX	(4)
#define ENEMY_CAR_0_W	(67)
#define ENEMY_CAR_0_H	(45)
#define ENEMY_CAR_1_W	(140)
#define ENEMY_CAR_1_H	(105)

/* 構造体定義 */

/* ライバル車の構造体 */
typedef struct
{
	UC	ubCarType;			/* 車の種類 */
	SS	VehicleSpeed;		/* 車速 */
	SS	x;					/* X座標 */
	SS	y;					/* Y座標 */
	SS	z;					/* Z座標(倍率) */
	UC	ubBrakeLights;		/* ブレーキライト */
	UC	ubOBD;				/* 故障の状態 */
	UC	ubAlive;			/* 表示の状態 */
}	ST_ENEMYCARDATA;

/* extern宣言 */
extern	SS	InitEnemyCAR(void);
extern	SS	GetEnemyCAR(ST_ENEMYCARDATA *, SS);
extern	SS	SetEnemyCAR(ST_ENEMYCARDATA, SS);
extern	SS	EnemyCAR_main(UC, UC, UC);
extern	SS	SetAlive_EnemyCAR(void);
extern	SS	Put_EnemyCAR(US, US, US, UC);
extern	SS	Sort_EnemyCAR(void);

#endif	/* ENEMYCAR_H */
