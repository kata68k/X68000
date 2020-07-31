#ifndef	COURSE_OBJ_C
#define	COURSE_OBJ_C

#include "inc/usr_macro.h"
#include "Course_Obj.h"
#include "Graphic.h"
#include "MyCar.h"


/* 変数定義 */
US	g_uRoadCycleCount = 0;

/* 構造体定義 */
ST_COURSE_OBJ	stCourse_Obj[COURSE_OBJ_MAX] = {0};
ST_COURSE_OBJ	*pstCourse_ObjList[COURSE_OBJ_MAX];

/* 関数のプロトタイプ宣言 */
SS	InitCourseObj(void);
SS	Course_Obj_main(UC, UC, UC);
SS	Put_CouseObject(SS, SS, US, UC, UC);
SS	GetRoadCycleCount(US *);
SS	SetRoadCycleCount(US);
SS	Sort_Course_Obj(void);

/* 関数 */
SS	InitCourseObj(void)
{
	SS ret = 0;
	SS i;
	
	for(i=0; i<COURSE_OBJ_MAX; i++)
	{
		stCourse_Obj[i].ubType = 0;
		stCourse_Obj[i].x = 16*(i/2);
		stCourse_Obj[i].y = 0;
		stCourse_Obj[i].z = 0;
		stCourse_Obj[i].ubAlive = TRUE;
		
		pstCourse_ObjList[i] = &stCourse_Obj[i];
	}
	
	return ret;
}

SS Course_Obj_main(UC bNum, UC bMode, UC bMode_rev)
{
	SS	ret = 0;

	if(pstCourse_ObjList[bNum]->ubAlive == TRUE)
	{
		SS	x, y, z;
		SS	dy;
		US	ras_x, ras_y;
		UC	bEven;
		US	uCount;
		ST_CRT		stCRT;
		ST_RAS_INFO	stRasInfo;

		if((bNum % 2) == 0)
		{
			bEven = TRUE;
		}
		else
		{
			bEven = FALSE;
		}

		x = pstCourse_ObjList[bNum]->x;
		y = pstCourse_ObjList[bNum]->y;
		z = pstCourse_ObjList[bNum]->z;
		
		GetRoadCycleCount(&uCount);
		if(uCount >= z)
		{
			y += (uCount - z) * 4;
		}
		else
		{
			y = 0;
		}
		z = uCount;
		
		dy = y - x;

		GetCRT(&stCRT, bMode);
		GetRasterInfo(&stRasInfo);

//		pstCourse_ObjList[bNum]->x = x;
		pstCourse_ObjList[bNum]->y = y;
		pstCourse_ObjList[bNum]->z = z;

		if(dy < 0)
		{
			/* nop */
		}
		else if((stRasInfo.st + dy) < stRasInfo.ed)
		{
			GetRasterPos(&ras_x, &ras_y, (US)(stRasInfo.st + dy));

			z = ((4 * 8) / (8 + dy));
			if(bEven == TRUE)
			{
				x = ras_x + (2 * dy);
			}
			else
			{
				x = ras_x - (2 * dy);
			}

			Put_CouseObject(stCRT.hide_offset_x + (WIDTH>>1) - x,
							stCRT.hide_offset_y + stRasInfo.horizon + dy,
							z,
							bMode_rev,
							bEven);
#ifdef DEBUG	/* デバッグコーナー */
//			if(bDebugMode == TRUE)	/* デバッグモード */
			{
				Draw_Pset(x, dy, 0xC2);	/* デバッグ用座標位置 */
			}
#endif
		}
		else
		{
			pstCourse_ObjList[bNum]->ubAlive = FALSE;
		}
	}
	else
	{
//		pstCourse_ObjList[bNum]->ubType = 0;
//		pstCourse_ObjList[bNum]->x = 0;
//		pstCourse_ObjList[bNum]->y = 0;
//		pstCourse_ObjList[bNum]->z = 0;
		pstCourse_ObjList[bNum]->ubAlive = TRUE;
	}
	
	return ret;
}

SS	Put_CouseObject(SS x, SS y, US Size, UC ubMode, UC ubPos)
{
	SS	ret;
	SS	i;
	SS	dx, dy;
	US	w, h;
	US	height_sum = 0u;
	
	w = PINETREE_1_W >> Size;
	h = PINETREE_1_H >> Size;

	dy = y - (h >> 1);
	if(ubPos == 0)
	{
		dx = x + w;
	}
	else
	{
		dx = x - w;
	}
	
	for(i = 1; i <= Size; i++)
	{
		height_sum += (PINETREE_1_H >> (i-1));
	}
	
	if(ubPos == 0)
	{
		G_BitBlt(	dx,		w,			dy,	h,	1,
					140,	w,	height_sum,	h,	1,
					ubMode, POS_LEFT, POS_BOTTOM);
	}
	else
	{
		G_BitBlt(	dx,		w,			dy,	h,	1,
					140,	w,	height_sum,	h,	1,
					ubMode, POS_RIGHT, POS_BOTTOM);
	}
	
	return	ret;
}

SS	GetRoadCycleCount(US *uCount)
{
	SS	ret = 0;
	
	*uCount = g_uRoadCycleCount;
	
	return ret;
}

SS	SetRoadCycleCount(US uCount)
{
	SS	ret = 0;
	
	g_uRoadCycleCount = uCount;
	
	return ret;
}

SS	Sort_Course_Obj(void)
{
	SS	ret = 0;
	SS	i;
	SS	count = 0;
	ST_COURSE_OBJ	*pstCourse_Obj_Tmp;
	
	while(1)
	{
		for(i=0; i<COURSE_OBJ_MAX-1; i++)
		{
			if(pstCourse_ObjList[i]->y > pstCourse_ObjList[i+1]->y)
			{
				pstCourse_Obj_Tmp = pstCourse_ObjList[i+1];
				pstCourse_ObjList[i+1] = pstCourse_ObjList[i];
				pstCourse_ObjList[i] = pstCourse_Obj_Tmp;
				count = 0;
			}
			else
			{
				count++;
			}
		}
		if(count >= (COURSE_OBJ_MAX - 1))
		{
			break;
		}
	}
	
	return ret;
}

#endif	/* COURSE_OBJ_C */

