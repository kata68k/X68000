#ifndef	TASK_C
#define	TASK_C

#include <usr_macro.h>
#include "IF_Task.h"
#include "BIOS_MFP.h"

/* グローバル変数 */
ST_TASK	g_stTask = {0}; 

/* スタティック変数 */
static uint32_t	un8ms_time		= 0;
static uint32_t	un16ms_time		= 0;
static uint32_t	un32ms_time		= 0;
static uint32_t	un96ms_time		= 0;
static uint32_t	un496ms_time	= 0;
	
/* 関数のプロトタイプ宣言 */
int16_t	TaskManage_Init(void);
int16_t	TaskManage(void);
int16_t	GetTaskInfo(ST_TASK *);
int16_t	UpdateTaskInfo(void);
int16_t	SetTaskInfo(uint8_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	TaskManage_Init(void)
{
	int16_t ret=0;
	uint32_t	time;
	
	GetStartTime(&time);	/* 開始時刻を取得 */
	
	un8ms_time		= time + 0;
	un16ms_time		= time + 1;
	un32ms_time		= time + 2;
	un96ms_time		= time + 3;
	un496ms_time	= time + 4;
	
	g_stTask.b8ms	=	TRUE;
	g_stTask.b16ms	=	TRUE;
	g_stTask.b32ms	=	TRUE;
	g_stTask.b96ms	=	TRUE;
	g_stTask.b496ms	=	TRUE;

	g_stTask.bScene = SCENE_INIT;

	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	TaskManage(void)
{
	int16_t ret=0;
	
	uint32_t	time;

	GetStartTime(&time);	/* 開始時刻を取得 */
	
	if( ((time - un8ms_time) >= 8) && (g_stTask.b8ms == FALSE) )	/* 8ms周期 */
	{
		ret = 1;
		un8ms_time = time;
		g_stTask.b8ms = TRUE;
	}
	
	if( ((time - un16ms_time) >= 16) && (g_stTask.b16ms == FALSE) )	/* 16ms周期 */
	{
		ret = 1;
		un16ms_time = time;
		g_stTask.b16ms = TRUE;
	}

	if( ((time - un32ms_time) >= 32) && (g_stTask.b32ms == FALSE) )	/* 32ms周期 */
	{
		ret = 1;
		un32ms_time = time;
		g_stTask.b32ms = TRUE;
	}

	if( ((time - un96ms_time) >= 96) && (g_stTask.b96ms == FALSE) )	/* 96ms周期 */
	{
		ret = 1;
		un96ms_time = time;
		g_stTask.b96ms = TRUE;
	}
	
	if( ((time - un496ms_time) >= 496) && (g_stTask.b496ms == FALSE) )	/* 496ms周期 */
	{
		ret = 1;
		un496ms_time = time;
		g_stTask.b496ms = TRUE;
	}

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetTaskInfo(ST_TASK *pst_Task)
{
	int16_t ret=0;

	*pst_Task = g_stTask;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	UpdateTaskInfo(void)
{
	int16_t ret=0;

	if(g_stTask.b8ms == TRUE)
	{
		g_stTask.b8ms	=	FALSE;
		ret = 1;
	}

	if(g_stTask.b16ms == TRUE)
	{
		g_stTask.b16ms	=	FALSE;
		ret = 1;
	}

	if(g_stTask.b32ms == TRUE)
	{
		g_stTask.b32ms	=	FALSE;
		ret = 1;
	}
	
	if(g_stTask.b96ms == TRUE)
	{
		g_stTask.b96ms	=	FALSE;
		ret = 1;
	}
	
	if(g_stTask.b496ms == TRUE)
	{
		g_stTask.b496ms	=	FALSE;
		ret = 1;
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	SetTaskInfo(uint8_t bScene)
{
	int16_t ret=0;

	g_stTask.bScene = bScene;

	return ret;
}

#endif	/* TASK_C */

