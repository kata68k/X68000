#ifndef	APL_COURSE_OBJ_H
#define	APL_COURSE_OBJ_H

#include <usr_define.h>

#define	COURSE_OBJ_MAX		(8)
#define	COURSE_OBJ_H_MAX	(4)	/* COURSE_OBJ_MAXの半分を定義する */
#define PINETREE_0_W	(31)
#define PINETREE_0_H	(56)
#define PINETREE_1_W	(62)
#define PINETREE_1_H	(112)
#define	COURSE_OBJ_PAT_MAX	(32)
#define	COURSE_OBJ_TYP_MAX	(4)

/* ライバル車の構造体 */
typedef struct
{
	uint8_t	ubType;			/* 種類 */
	uint8_t	ubPos;			/* 位置(0：左、1：右) */
	int16_t	x;				/* X座標 */
	int16_t	y;				/* Y座標 */
	int16_t	z;				/* Z座標(倍率) */
	uint8_t	ubAlive;		/* 表示の状態 */
}	ST_COURSE_OBJ;

extern	int16_t	InitCourseObj(void);
extern	int16_t	GetCourseObj(ST_COURSE_OBJ *, int16_t);
extern	int16_t	SetCourseObj(ST_COURSE_OBJ, int16_t);
extern	int16_t	Course_Obj_main(uint8_t, uint8_t, uint8_t);
extern	int16_t	Put_CouseObject(int16_t, int16_t, uint16_t, uint8_t, uint8_t, uint8_t);
extern	int16_t	Sort_Course_Obj(void);
extern	int16_t	Load_Course_Obj(int16_t);
extern	int16_t	Load_Course_Data(uint8_t);
extern	int16_t	Move_Course_BG(uint8_t);
extern	int16_t	Set_Course_TmpObject(uint16_t, uint8_t);

#endif	/* APL_COURSE_OBJ_H */
