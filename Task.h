#ifndef	TASK_H
#define	TASK_H

#include "inc/usr_define.h"

enum
{
	SCENE_INIT,
	SCENE_TITLE_S,
	SCENE_TITLE,
	SCENE_TITLE_E,
	SCENE_DEMO_S,
	SCENE_DEMO,
	SCENE_DEMO_E,
	SCENE_START_S,
	SCENE_START,
	SCENE_START_E,
	SCENE_GAME_S,
	SCENE_GAME,
	SCENE_GAME_E,
	SCENE_GAME_OVER_S,
	SCENE_GAME_OVER,
	SCENE_GAME_OVER_E,
	SCENE_HI_SCORE_S,
	SCENE_HI_SCORE,
	SCENE_HI_SCORE_E,
	SCENE_OPTION_S,
	SCENE_OPTION,
	SCENE_OPTION_E,
	SCENE_EXIT,
	SCENE_MAX,
};

/* 構造体 */

/* タスクの構造体 */
typedef struct
{
	UC	b8ms;	/*   8ms Task */
	UC	b16ms;	/*  16ms Task */
	UC	b32ms;	/*  32ms Task */
	UC	b96ms;	/*  96ms Task */
	UC	b496ms;	/* 496ms Task */
	UC	bScene;	/* シーン */
}	ST_TASK;

extern	SS	TaskManage(void);
extern	SS	GetTaskInfo(ST_TASK *);
extern	SS	SetTaskInfo(UC);

#endif	/* TASK_H */
