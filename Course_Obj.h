#ifndef	OBJECT_H
#define	OBJECT_H

#include "inc/usr_define.h"

#define	COURSE_OBJ_MAX	(6)

/* ライバル車の構造体 */
typedef struct
{
	UC	ubType;			/* 種類 */
	SS	x;				/* X座標 */
	SS	y;				/* Y座標 */
	SS	z;				/* Z座標(倍率) */
	UC	ubAlive;		/* 表示の状態 */
}	ST_COURSE_OBJ;

extern	SS	InitCourseObj(void);
extern	SS	Course_Obj_main(UC, UC, UC);
extern	SS	Put_CouseObject(SS, SS, US, UC, UC);
extern	SS	GetRoadCycleCount(US *);
extern	SS	SetRoadCycleCount(US);
extern	SS	Sort_Course_Obj(void);
extern	SS	Load_Course_Data(UC);

#endif	/* OBJECT_H */
