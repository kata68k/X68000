#ifndef	MyCar_H
#define	MyCar_H

#include "inc/usr_define.h"

#define MY_CAR_0_W	(65)
#define MY_CAR_0_H	(52)
#define MY_CAR_1_W	(256)
#define MY_CAR_1_H	(224)

enum{
	OBD_NORMAL,
	OBD_DAMAGE,
	OBD_SPIN_L,
	OBD_SPIN_R,
	OBD_COURSEOUT,
	OBD_MAX
};

/* 構造体定義 */

/* 車の構造体 */
typedef struct
{
	uint8_t	ubCarType;			/* 車の種類 */
	uint16_t	uEngineRPM;			/* エンジン回転数 */
	int16_t	VehicleSpeed;		/* 車速 */
	int16_t	Steering;			/* ステア */
	uint8_t	ubShiftPos;			/* ギア段 */
	uint8_t	ubThrottle;			/* スロットル開度 */
	uint8_t	ubBrakeLights;		/* ブレーキライト */
	uint8_t	ubHeadLights;		/* ヘッドライト */
	uint8_t	ubWiper;			/* ワイパー */
	int8_t	bTire;				/* タイヤの状態 */
	uint8_t	ubOBD;				/* 車両の状態 */
}	ST_CARDATA;

/* extern宣言 */

extern	int16_t	MyCar_G_Load(void);
extern	int16_t	GetMyCar(ST_CARDATA *stDat);
extern	int16_t	SetMyCar(ST_CARDATA stDat);
extern	int16_t	MyCarInfo_Init(void);
extern	int16_t	MyCarInfo_Update(int16_t);
extern	int16_t	MyCar_Interior(uint8_t);
extern	int16_t	MyCar_CourseOut(void);
extern	int16_t	GetMyCarSpeed(int16_t *);

/* メモ */

/*
車体寸法	長4495×幅1810×高1480mm
室内寸法	長2030×幅1470×高1190mm
軸距＆輪距	2650mm
			前1545mm/後1545mm
最小半径	5.9m
最低高		140mm
タイヤ		前輪：245/40R18
			後輪：245/40R18
			タイヤ直径 65.3cm
			タイヤの有効半径(0.3265m)
ブレーキ	前：ベンチレーテッドディスク
			後：ベンチレーテッドディスク
車両重量	1530kg

エンジン諸元
原動機型式	4B11
気筒配列	直列4気筒
排気量		1998cc
圧縮比		9.0
吸気方式	ターボ
最高出力	313PS[230kW]/6500rpm
最大トルク	43.7kgm[429Nm]/3500rpm

変速比		1速:2.857
			2速:1.95
		 	3速:1.444
		 	4速:1.096
		 	5速:0.761
減速比			4.687

（１）質量ｍ＝1530kgの物体に作用する重力Wは
		W（Ｎ）＝質量ｍ（ｋｇ）×重力加速度9.81（m／sec2）
		 	＝	1530（kg）× 9.81（m／sec2）
			 ＝ 15162.3Ｎ
（２）摩擦力ｆ ＝μＮであるから、
		ｆ（Ｎ）＝1.0×15162.3（Ｎ）＝15162.3Ｎ
		アスファルト：0.5～1.0、砂利：0.4～0.5、雪：0.2～0.5
（３）物体を動かすには、摩擦力ｆ に等しい力が必要であるから
		Ｆ（Ｎ）＝ｆ	＝15162.3Ｎ

（４）駆動力＝トルク×最終ギヤ比（＝変速比×減速比）÷タイヤ半径

（５）加速度＝（トルク－摩擦抵抗－空気抵抗）÷車両重量

（６）空気抵抗 ＝ 1/2 x 空気密度 × 前面投影面積 × 速度の２乗 × CD値（空気抵抗係数）
*/

#endif	/* MyCar_H */
