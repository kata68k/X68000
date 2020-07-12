#ifndef	MyCar_H
#define	MyCar_H

#include "inc/usr_define.h"

/* 構造体定義 */

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

/* extern宣言 */

extern	SS	GetMyCar(ST_CARDATA *stDat);
extern	SS	SetMyCar(ST_CARDATA stDat);
extern	SS	UpdateMyCarInfo(SS);
extern	SS	MyCar_Interior(SS, SS);
extern	SS	MyCar_CourseOut(SS);

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

（１）質量ｍ＝1530kgの物体に作用する重力Wは
		W（Ｎ）＝質量ｍ（ｋｇ）×重力加速度9.81（m／sec2）
		 	＝	1530（kg）× 9.81（m／sec2）
			 ＝ 15162.3Ｎ
（２）摩擦力ｆ ＝μＮであるから、
		ｆ（Ｎ）＝1.0×15162.3（Ｎ）＝15162.3Ｎ
		アスファルト：0.5～1.0、砂利：0.4～0.5、雪：0.2～0.5
（３）物体を動かすには、摩擦力ｆ に等しい力が必要であるから
		Ｆ（Ｎ）＝ｆ	＝15162.3Ｎ
*/

#endif	/* MyCar_H */
