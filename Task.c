#ifndef	TASK_C
#define	TASK_C

#include "inc/usr_macro.h"
#include "Task.h"

SS	GetTaskInfo(ST_TASK *, UI);

SS	GetTaskInfo(ST_TASK *st_Task, UI time)
{
	static SC	bFirstTime		= TRUE;
	static UI	un8ms_time		= 0;
	static UI	un16ms_time		= 0;
	static UI	un32ms_time		= 0;
	static UI	un96ms_time		= 0;
	static UI	un496ms_time	= 0;

	SS	ret = 0;
	UC	b8ms_time;
	UC	b16ms_time;
	UC	b32ms_time;
	UC	b96ms_time;
	UC	b496ms_time;

	if( (time - un8ms_time) > 8)	/* 8msüŠú */
	{
		un8ms_time = time;
		b8ms_time = TRUE;
	}
	else
	{
		b8ms_time = FALSE;
	}
	
	if( (time - un16ms_time) > 16)	/* 16msüŠú */
	{
		un16ms_time = time;
		b16ms_time = TRUE;
	}
	else
	{
		b16ms_time = FALSE;
	}

	if( (time - un32ms_time) > 32)	/* 32msüŠú */
	{
		un32ms_time = time;
		b32ms_time = TRUE;
	}
	else
	{
		b32ms_time = FALSE;
	}

	if( (time - un96ms_time) > 96)	/* 96msüŠú */
	{
		un96ms_time = time;
		b96ms_time = TRUE;
	}
	else
	{
		b96ms_time = FALSE;
	}
	
	if( (time - un496ms_time) > 496)	/* 496msüŠú */
	{
		un496ms_time = time;
		b496ms_time = TRUE;
	}
	else
	{
		b496ms_time = FALSE;
	}

	if(bFirstTime == TRUE)
	{
		bFirstTime = FALSE;
		st_Task->b8ms	=	TRUE;
		st_Task->b16ms	=	TRUE;
		st_Task->b32ms	=	TRUE;
		st_Task->b96ms	=	TRUE;
		st_Task->b496ms	=	TRUE;
	}
	else
	{
		st_Task->b8ms	=	b8ms_time;
		st_Task->b16ms	=	b16ms_time;
		st_Task->b32ms	=	b32ms_time;
		st_Task->b96ms	=	b96ms_time;
		st_Task->b496ms	=	b496ms_time;
	}
	
	return ret;
}

#endif	/* TASK_C */

