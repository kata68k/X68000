#ifndef	TASK_H
#define	TASK_H

#include "inc/usr_define.h"

/* 構造体 */

/* タスクの構造体 */
typedef struct
{
	UC	b8ms;	/*   8ms Task */
	UC	b16ms;	/*  16ms Task */
	UC	b32ms;	/*  32ms Task */
	UC	b96ms;	/*  96ms Task */
	UC	b496ms;	/* 496ms Task */
}	ST_TASK;

extern	SS	GetTaskInfo(ST_TASK *, UI);

#endif	/* TASK_H */
