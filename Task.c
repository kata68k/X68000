#ifndef	TASK_C
#define	TASK_C

#include "inc/usr_macro.h"
#include "Task.h"
#include "MFP.h"

ST_TASK	g_stTask = {0}; 

int16_t	TaskManage(void);
int16_t	GetTaskInfo(ST_TASK *);
int16_t	SetTaskInfo(uint8_t);

int16_t	TaskManage(void)
{
	int16_t ret=0;
	
	static int8_t	bFirstTime		= TRUE;
	static uint32_t	un8ms_time		= 0;
	static uint32_t	un16ms_time		= 0;
	static uint32_t	un32ms_time		= 0;
	static uint32_t	un96ms_time		= 0;
	static uint32_t	un496ms_time	= 0;
	uint32_t	time;
	uint8_t	b8ms_time;
	uint8_t	b16ms_time;
	uint8_t	b32ms_time;
	uint8_t	b96ms_time;
	uint8_t	b496ms_time;

	GetStartTime(&time);	/* ŠJŽnŽž‚ðŽæ“¾ */
	
	if( (time - un8ms_time) > 8)	/* 8msŽüŠú */
	{
		un8ms_time = time;
		b8ms_time = TRUE;
	}
	else
	{
		b8ms_time = FALSE;
	}
	
	if( ((time+2) - un16ms_time) > 16)	/* 16msŽüŠú */
	{
		un16ms_time = (time+2);
		b16ms_time = TRUE;
	}
	else
	{
		b16ms_time = FALSE;
	}

	if( ((time+4) - un32ms_time) > 32)	/* 32msŽüŠú */
	{
		un32ms_time = (time+4);
		b32ms_time = TRUE;
	}
	else
	{
		b32ms_time = FALSE;
	}

	if( ((time+6) - un96ms_time) > 96)	/* 96msŽüŠú */
	{
		un96ms_time = (time+6);
		b96ms_time = TRUE;
	}
	else
	{
		b96ms_time = FALSE;
	}
	
	if( ((time+8)- un496ms_time) > 496)	/* 496msŽüŠú */
	{
		un496ms_time = (time+8);
		b496ms_time = TRUE;
	}
	else
	{
		b496ms_time = FALSE;
	}

	if(bFirstTime == TRUE)
	{
		bFirstTime = FALSE;
		g_stTask.b8ms	=	TRUE;
		g_stTask.b16ms	=	TRUE;
		g_stTask.b32ms	=	TRUE;
		g_stTask.b96ms	=	TRUE;
		g_stTask.b496ms	=	TRUE;
	}
	else
	{
		g_stTask.b8ms	=	b8ms_time;
		g_stTask.b16ms	=	b16ms_time;
		g_stTask.b32ms	=	b32ms_time;
		g_stTask.b96ms	=	b96ms_time;
		g_stTask.b496ms	=	b496ms_time;
	}

	return ret;
}

int16_t	GetTaskInfo(ST_TASK *pst_Task)
{
	int16_t ret=0;

	pst_Task->b8ms		=	g_stTask.b8ms;
	pst_Task->b16ms		=	g_stTask.b16ms;
	pst_Task->b32ms		=	g_stTask.b32ms;
	pst_Task->b96ms		=	g_stTask.b96ms;
	pst_Task->b496ms	=	g_stTask.b496ms;
	pst_Task->bScene	=	g_stTask.bScene;
	
	return ret;
}

int16_t	SetTaskInfo(uint8_t bScene)
{
	int16_t ret=0;

	g_stTask.bScene = bScene;

	return ret;
}

#endif	/* TASK_C */

