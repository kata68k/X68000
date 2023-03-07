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
	SCENE_NEXT_STAGE_S,
	SCENE_NEXT_STAGE,
	SCENE_NEXT_STAGE_E,
	SCENE_HI_SCORE_S,
	SCENE_HI_SCORE,
	SCENE_HI_SCORE_E,
	SCENE_OPTION_S,
	SCENE_OPTION,
	SCENE_OPTION_E,
	SCENE_DEBUG_S,
	SCENE_DEBUG,
	SCENE_DEBUG_E,
	SCENE_EXIT,
	SCENE_MAX,
};

/* 構造体 */

/* タスクの構造体 */
typedef struct
{
	uint8_t	b8ms;	/*   8ms Task */
	uint8_t	b16ms;	/*  16ms Task */
	uint8_t	b32ms;	/*  32ms Task */
	uint8_t	b96ms;	/*  96ms Task */
	uint8_t	b496ms;	/* 496ms Task */
	uint8_t	bScene;	/* シーン */
}	ST_TASK;

extern int16_t	TaskManage_Init(void);
extern int16_t	TaskManage(void);
extern int16_t	GetTaskInfo(ST_TASK *);
extern int16_t	UpdateTaskInfo(void);
extern int16_t	SetTaskInfo(uint8_t);

#endif	/* TASK_H */
