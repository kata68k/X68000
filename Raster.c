#ifndef	RASTER_C
#define	RASTER_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>

#include "inc/usr_macro.h"
#include "OverKata.h"

#include "Raster.h"

#include "APL_Math.h"
#include "Course_Obj.h"
#include "CRTC.h"
#include "Draw.h"
#include "EnemyCAR.h"
#include "Graphic.h"
#include "MFP.h"
#include "MyCar.h"
#include "Output_Text.h"
#include "Text.h"

/* �ϐ� */
static uint16_t	g_uRoadAnime;
static uint16_t	g_uRoadAnime_old;
static uint16_t	g_uRoadAnimeBase;
static uint16_t	g_uRoadAnimeBase_old;
static uint16_t	g_uRoadAnimeCount;

static uint16_t	g_uRoadCycleCount;
static uint16_t	g_uCounter;
static uint8_t	g_bRoadInitFlag = TRUE;

/* �\���̒�` */
static ST_RAS_INFO	g_stRasInfo = {0};

static ST_ROAD_INFO	g_stRoadInfo = {0};
static ST_ROADDATA	g_stRoadData[ROADDATA_MAX] = {0};

/* �֐��̃v���g�^�C�v�錾 */
int16_t	GetRasterInfo(ST_RAS_INFO *);
int16_t	SetRasterInfo(ST_RAS_INFO);
void	Raster_Init(void);
void	Raster_Main(uint8_t);
static uint8_t Raster_Calc_H(int16_t *, int16_t, int16_t);
static uint8_t Raster_Calc_V(int16_t *, int16_t, int16_t);

int16_t	GetRasterPos(uint16_t *, uint16_t *, uint16_t);	/* ���X�^�[�������ʂ��擾 */

int16_t	GetRoadInfo(ST_ROAD_INFO *);
int16_t	SetRoadInfo(ST_ROAD_INFO);
void Road_Init(uint16_t);
static void Get_Road_Pat(uint16_t *, uint8_t);
static void Set_Road_Pat_offset(int16_t);
static int16_t Road_Pat_Update(uint16_t);
static void Road_Pat_Base_Update(void);
static void Road_Pat_Start(void);
static void Road_Pat_Stop(void);
static int16_t	Set_Road_Height(void);
static int16_t	Set_Road_Slope(void);
static int16_t	Set_Road_Angle(void);
static int16_t	Set_Road_Distance(void);
static int16_t	Set_Road_Object(void);
static void	Road_Pat_Main(void);
int16_t	GetRoadCycleCount(uint16_t *);
uint64_t	GetRoadDataAddr(void);
int16_t	SetHorizon(uint8_t);
static int16_t Load_Road_Background(int16_t);

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t GetRasterInfo(ST_RAS_INFO *stDat)
{
	int16_t	ret = 0;
	
	*stDat = g_stRasInfo;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t SetRasterInfo(ST_RAS_INFO stDat)
{
	int16_t	ret = 0;
	
	g_stRasInfo = stDat;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void Raster_Init(void)
{
	memset(&g_stRasInfo, 0, sizeof(ST_RAS_INFO));
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void Raster_Main(uint8_t bMode)
{
	int16_t	view_offset_x, view_offset_y;
	int16_t	hide_offset_x, hide_offset_y;
	int16_t	BG_offset_x, BG_offset_y;
	
	uint16_t	uRas_y;
	uint16_t	uRas_st, uRas_mid, uRas_ed, uRas_size;

	ST_CRT			stCRT = {0};
	ST_RASTER_INT	stRasterInt[RASTER_MAX] = {0};

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	uint8_t	bY_area;
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	/* ���[�h�ؑւɂ��ݒ�l�̕ύX */
	GetCRT(&stCRT, bMode);
	view_offset_x	= stCRT.view_offset_x;
	view_offset_y	= stCRT.view_offset_y;
	hide_offset_x	= stCRT.hide_offset_x;
	hide_offset_y	= stCRT.hide_offset_y;
	BG_offset_x		= stCRT.BG_offset_x;
	BG_offset_y		= stCRT.BG_offset_y;

	/* ���[�h�p�^�[�� */
	Road_Pat_Main();

	/* �������̐ݒ� */
	SetHorizon(bMode);

	/* ������ */
	uRas_st = 0;
	uRas_mid = 0;
	uRas_ed = 0;
	uRas_size = 0xFFFF;

	/* ���X�^�[�ʒu */
	g_stRasInfo.st	= g_stRoadInfo.Horizon;					/* ���X�^�[�J�n�ʒu */
	g_stRasInfo.mid = g_stRoadInfo.Horizon_Base;			/* ���X�^�[�Œ�ʒu *//* �������̈ʒu���ω����Ȃ��|�C���g *//* �n�C�E�F�C�X�^�[�Ȃ� 64 = ROAD_SIZE*2/3 */
	if(bMode == 0)		/* TPS */
	{
		g_stRasInfo.ed = Mmin(RASTER_MAX, V_SYNC_MAX);		/* ���X�^�[�I���ʒu */
	}
	else				/* FPS */
	{
		g_stRasInfo.ed = Mmin(Y_MAX_WINDOW, V_SYNC_MAX);	/* ���X�^�[�I���ʒu */
	}

	uRas_st		= g_stRasInfo.st;	/* �J�n�ʒu */
	uRas_mid	= g_stRasInfo.mid;	/* ���Ԉʒu */
	uRas_ed		= g_stRasInfo.ed;	/* �I���ʒu */
	uRas_size	= uRas_ed - uRas_st;				/* ���X�^�[�͈̔� */
	g_stRasInfo.size	= uRas_size;				/* ���X�^�[�͈̔� */
	SetRasterInfo(g_stRasInfo);
	
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	if(bDebugMode == TRUE)	/* �f�o�b�O���[�h */
	{
		int16_t	col;
		
		APAGE(0);				/* �O���t�B�b�N�̏������� */

		/* ���X�^�[�J�n�ʒu */
		col = 0x3;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.st,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.st,
					col,
					0xFFFF);
		/* ���X�^�[���Ԉʒu */
		col = 0xF;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.mid,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.mid,
					col,
					0xFFFF);
		/* ���X�^�[�I���ʒu */
		col = 0xB;
		Draw_Line(	hide_offset_x + 0,
					hide_offset_y + g_stRasInfo.ed,
					hide_offset_x + WIDTH, 
					hide_offset_y + g_stRasInfo.ed,
					col,
					0xFFFF);
	}
#endif
	
	Road_Pat_Start();	/* ����͓��̃p���b�g�̊J�n�ݒ���s�� */
	
	for(uRas_y=0; uRas_y < uRas_size; uRas_y+=RASTER_NEXT)
	{
		uint16_t	uRev_uRas_y;
		int16_t	ras_cal_x=0;
		int16_t	ras_cal_y=0;
		uint16_t	ras_pat=0;
		uint16_t	ras_offset;
		uint8_t	bRoad_pat = FALSE;
		
		uRev_uRas_y = uRas_size - uRas_y;	/* uRas_size -> 0 */
		
		/* X���W */
		Raster_Calc_H( &ras_cal_x, uRas_y, uRev_uRas_y );

		/* Y���W */
		bY_area = Raster_Calc_V( &ras_cal_y, uRas_y, uRev_uRas_y );

		/* ���[�h�p�^�[�� */
		if(uRas_y == 0)
		{
			bRoad_pat = TRUE;
		}
		Get_Road_Pat(&ras_pat, bRoad_pat);	/* ���[�h�̃p�^�[�������擾 */
		Set_Road_Pat_offset(uRas_y);		/* ���[�h�p�^�[��臒l�̍X�V */

		ras_offset = uRas_st + uRas_y;

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		if(bDebugMode == TRUE)	/* �f�o�b�O���[�h */
		{
			int16_t	col;
			int16_t	point_x;
			
			/* x */
			col = 0xE;
			/* �Z���^�[ */
			if( ras_cal_x < 256 )	/* ���J�[�u */
			{
				point_x = ROAD_CT_POINT + (  0 - ras_cal_x);
			}
			else	/* �E�J�[�u */
			{
				point_x = ROAD_CT_POINT + (512 - ras_cal_x);
			}
			Draw_Pset(	hide_offset_x + point_x,
						hide_offset_y + g_stRoadInfo.Horizon + uRas_y,
						col);

			/* y */
			col = (int16_t)bY_area;
			Draw_Pset(	hide_offset_x + ras_offset,
						hide_offset_y + ras_cal_y,
						col);
		}
#endif
		stRasterInt[ras_offset].x = (uint16_t)ras_cal_x;
		stRasterInt[ras_offset].y = (uint16_t)ras_cal_y;
		stRasterInt[ras_offset].pat = ras_pat;

		stRasterInt[ras_offset+1].x = (uint16_t)ras_cal_x;	/* ���Ԃ̕��� */
		stRasterInt[ras_offset+1].y = (uint16_t)ras_cal_y;	/* ���Ԃ̕��� */
		stRasterInt[ras_offset+1].pat = ras_pat;		/* ���Ԃ̕��� */
	}
	
	stRasterInt[0].y = g_stRasInfo.st;		/* ���荞�݈ʒu�̐ݒ� */
	/* BG��Y���W��I�ԃ��X�^�̊��荞�݈ʒu�ɑ΂��ĕ\��������BG��Y���W���� */
	/* �����0or512�łȂ��Ƌ�ɓ����o�Ă��� */	/* �Ղ̂Ђ� ����̃A�h�o�C�X�ӏ� */
	stRasterInt[1].y = ROAD_ST_POINT - g_stRasInfo.st;	/* ���荞�݂�������܂ł̕\���ʒu(���X�^���荞�݈ʒu�ɂ�邸��̍l���͕s�v)	*/
	
	SetRasterIntData(stRasterInt, sizeof(ST_RASTER_INT) * RASTER_MAX);
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static uint8_t Raster_Calc_H(int16_t *x, int16_t Num, int16_t RevNum)
{
	uint8_t bRet = 0;

	if(Num == 0u)
	{
		bRet = 0;
	}
	/* X���W */
	{
		int16_t	Steer = 0;
		int16_t	Scroll_w = 0;
		int16_t	Scroll_ofst = 0;
		int16_t	Point_x = 0;
		ST_CARDATA stMyCar = {0};

		GetMyCar(&stMyCar);			/* ���Ԃ̏����擾 */

		Steer = (stMyCar.Steering >> 4);
		
		/* �X�N���[�����镝�~�v�Z���鍂�������X�^�[�X�N���[�������鍂�� */
		if((Steer == 0) || (Num == 0))
		{
			Scroll_ofst	= 0;
		}
		else
		{
			Scroll_ofst	= APL_sDiv( Steer * Num, g_stRasInfo.size );	/* �ԑ̂̈ړ�(�����ʒu�قǈړ��ʂ����Ȃ�) */
		}
		
		/* �X�N���[�����镝�~�v�Z���鍂�������X�^�[�X�N���[�������鍂�� */
		if(g_stRoadInfo.angle == 0)
		{
			Scroll_w	= 0;
		}
		else
		{
			int16_t Scroll_w_clp;
			Scroll_w_clp = Mmax( Mmin( g_stRoadInfo.angle * g_stRasInfo.size, 511 ), -512 );
			
			if(Num == 0)
			{
				Scroll_w	= Scroll_w_clp;	/* �J�[�u(�Ⴂ�ʒu�قǈړ��ʂ����Ȃ�) */
			}
			else
			{
				Scroll_w	= APL_sDiv( Scroll_w_clp, Num);	/* �J�[�u(�Ⴂ�ʒu�قǈړ��ʂ����Ȃ�) */
			}
		}
		Point_x		= Scroll_ofst + Scroll_w;	/* ���X�^�[�X�N���[���ʒu */

		*x = Point_x & 0x1FFu;	/* 9bit�}�X�N *//* Max224���炢 */
	}

	return bRet;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static uint8_t Raster_Calc_V(int16_t *y, int16_t Num, int16_t RevNum)
{
	uint8_t bRet = 0;
	int16_t RoadPoint_y = 0;

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif

	if(Num == 0u)	
	{
		bRet = 0x00;
	}
	/* Y���W */
	{
		int16_t	Point_y, Point_y_def;
		int16_t	MinClip, MaxClip;
		int16_t	uRasStart;
		
		uRasStart = Mmin(g_stRasInfo.st, g_stRasInfo.mid);
		MinClip = ROAD_ST_POINT - (uRasStart + Num);
		MaxClip = ROAD_ED_POINT - (uRasStart + Num);
		Point_y_def = ROAD_ST_POINT - g_stRasInfo.mid;	/* �W���ʒu */
		
		if(g_stRasInfo.mid == g_stRoadInfo.Horizon)	/* �������̈ʒu���ʏ��艺���̏��� */
		{
			RoadPoint_y = Point_y_def;

			bRet = 0xFF;
		}
		else if( g_stRasInfo.mid > g_stRoadInfo.Horizon )	/* �������̈ʒu���ʏ���㑤�̏��� */
		{
			int16_t	Road_strch = 0;
			
			uint16_t	uSize;

			uSize = g_stRasInfo.mid - g_stRoadInfo.Horizon;
			Road_strch = APL_uDiv( uSize * RevNum, g_stRasInfo.size );

			RoadPoint_y = Mmax(Point_y_def + Road_strch, Point_y_def);

			bRet = 0xFF;
		}
		else	/* �������̈ʒu���ʏ��艺���̏��� */
		{
			int16_t	Road_strch = 0;
			uint16_t	uSize;

			uSize = g_stRoadInfo.Horizon - g_stRasInfo.mid;
			Road_strch = APL_uDiv( uSize * RevNum, g_stRasInfo.size );

			RoadPoint_y = Mmax(Point_y_def - Road_strch, Point_y_def);

			bRet = 0xb7;
		}
		
		Point_y = Mmax(Mmin(RoadPoint_y, MaxClip), MinClip);	/* �㉺���N���b�v */
		if(RoadPoint_y == MaxClip)
		{
			bRet = TRUE;	/* MaxClip */
		}
		*y = Point_y & 0x1FFu;	/* 9bit�}�X�N */
	}
		
	return bRet;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t GetRoadInfo(ST_ROAD_INFO *stDat)
{
	int16_t	ret = 0;
	
	*stDat = g_stRoadInfo;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t SetRoadInfo(ST_ROAD_INFO stDat)
{
	int16_t	ret = 0;
	
	g_stRoadInfo = stDat;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void Road_Init(uint16_t uCourseNum)
{
	uint32_t	i;
	
	if(uCourseNum == 0u)uCourseNum = 1u;
	
	g_uRoadAnimeBase = 3;
	g_uRoadAnime = 0;
	g_bRoadInitFlag = TRUE;
	Road_Pat_Start();
	
	g_uCounter = 0u;
	g_uRoadCycleCount = 0u;

	g_stRoadInfo.Horizon		= 0x00;	/* �����ʒu */
	g_stRoadInfo.Horizon_Base	= 0x00;	/* �������ʒu */
	g_stRoadInfo.offset_x		= 0x00;	/*  */
	g_stRoadInfo.offset_y		= 0x00;	/*  */
	g_stRoadInfo.offset_val		= 0x00;	/*  */
	g_stRoadInfo.slope			= 0x00;	/*  */
	g_stRoadInfo.angle			= 0x00;	/*  */
	g_stRoadInfo.distance		= 0x00;	/*  */
	g_stRoadInfo.object			= 0x00;	/*  */
	
	for(i = 0u; i < ROADDATA_MAX; i++)
	{
		g_stRoadData[i].bHeight		= 0x80;	/* ���̕W��	(0x80�Z���^�[) */
		g_stRoadData[i].bWidth		= 0x80;	/* ���̕�	(0x80�Z���^�[) */
		g_stRoadData[i].bAngle		= 0x80;	/* ���̊p�x	(0x80�Z���^�[) */
		g_stRoadData[i].bfriction	= 0x80;	/* ���̖��C	(0x80�Z���^�[) */
		g_stRoadData[i].bPat		= 0x00;	/* ���̎��	 */
		g_stRoadData[i].bObject		= 0x00;	/* �o���|�C���g�̃I�u�W�F�N�g�̎�� */
		g_stRoadData[i].bRepeatCount= 0x00;	/* �J��Ԃ��� */
	}
	
	/* �R�[�X�f�[�^�̓ǂݍ��� */
	Load_Course_Data(uCourseNum);

	Set_Road_Angle();		/* �R�[�X�̋Ȃ��� */
	Set_Road_Height();		/* �R�[�X�̕W�� */
	Set_Road_Slope();		/* �R�[�X�̌��ݒn����W���܂ł̌X�� */
	Set_Road_Distance();	/* �������������ݒn�܂ł̋��� */
	Set_Road_Object();		/* ���C�o���Ԃ̏o�� */
	
	Load_Road_Background(0);	/* �R�[�X�w�i�̓W�J */
	
	/* �R�[�X��Q���̓W�J */
	for(i = 0u; i < COURSE_OBJ_TYP_MAX; i++)
	{
		Load_Course_Obj(i);		/* �R�[�X��Q���ǂݍ��݂̓W�J */
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Get_Road_Pat(uint16_t *p_uPat, uint8_t bFlag)
{
#if 1
	uint16_t	uBG_pat[4][4] = {288,   0,  96, 192, 
		 				 192, 288,   0,  96, 
		 				  96, 192, 288,   0, 
		 				   0,  96, 192, 288} ;
	if(bFlag == TRUE)
	{
		*p_uPat = uBG_pat[3][0];
	}
	else
	{
		*p_uPat = uBG_pat[g_uRoadAnimeBase][g_uRoadAnime];
	}
#else
	*p_uPat = 0;
#endif
}

static void Set_Road_Pat_offset(int16_t Num)
{
	if( (Num == 0) || (g_stRoadInfo.offset_val == 0) )
	{
		g_stRoadInfo.offset_y = 0;
		g_stRoadInfo.offset_val = 0;
	}
	else
	{
		//g_stRoadInfo.offset_y = (Num * g_stRoadInfo.offset_val) / Mmax((g_stRoadInfo.offset_val + 96), 1);
		g_stRoadInfo.offset_y = APL_uDiv((Num * g_stRoadInfo.offset_val), (g_stRoadInfo.offset_val + g_stRasInfo.size ));
	}
	if(Road_Pat_Update(g_stRoadInfo.offset_y) != 0u)
	{
		g_stRoadInfo.offset_val++;
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static int16_t Road_Pat_Update(uint16_t uThreshold)
{
	int16_t	ret = 0;
	
	g_uRoadAnimeCount++;
	if(g_uRoadAnimeCount >= uThreshold)
	{
		g_uRoadAnimeCount = 0;
		g_uRoadAnime++;
		ret = 1;
		
	}
	
	if(g_uRoadAnime >= 4u)
	{
		g_uRoadAnime = 0u;
	}
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Road_Pat_Base_Update(void)
{
	g_uRoadAnimeCount = 0xFFF;	/* �t���A�j���[�V�����h�~ */

	g_uRoadAnime = 0;

	g_uRoadAnimeBase++;
	if(g_uRoadAnimeBase >= 4)
	{	
		g_uRoadAnimeBase = 0;
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Road_Pat_Start(void)
{
	g_uRoadAnime_old = g_uRoadAnime;
	g_uRoadAnimeBase_old = g_uRoadAnimeBase;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Road_Pat_Stop(void)
{
	g_uRoadAnime = g_uRoadAnime_old;
	g_uRoadAnimeBase = g_uRoadAnimeBase_old;
	g_uRoadAnimeCount = 0;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static int16_t	Set_Road_Height(void)
{
	int16_t	ret = 0;
	int16_t	road_height;
	static int16_t	road_height_old;
	
	road_height_old = g_stRoadInfo.height;	/* �O��l�X�V */
	
	/* �R�[�X�f�[�^�ǂݍ��� */
	road_height = (int16_t)(0x80 - g_stRoadData[g_uCounter].bHeight);	/* ���̕W��	(0x80�Z���^�[) */
	road_height = Mmax(Mmin(road_height, 31), -32);					/* +�����F-�Ⴂ */

	g_stRoadInfo.height = road_height;		/*  */

	// ���፷����X�����o��(-45�`+45) rad = APL_Atan2( road_slope_old - road_slope,  );
	if(road_height != road_height_old)
	{
		if(road_height > road_height_old)
		{
			/* �ێ� */
		}
		else
		{
			/* �ێ� */
		}
		ret = 1;
	}
	else{
		/* �O��l�ێ� */
	}
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static int16_t	Set_Road_Slope(void)
{
	int16_t	ret = 0;
	int16_t	cal_tan = 0;
	int16_t	cal_cos = 0;
	int16_t	road_slope = 0;
	static int16_t	road_slope_old;

	road_slope_old = g_stRoadInfo.slope;		/* �O��l�X�V */
	road_slope = g_stRoadInfo.height;
	
#if 1
	if(road_slope == 0)
	{
		cal_tan = 0;
		cal_cos = 0;
	}
	else
	{
		if(road_slope != road_slope_old)
		{
			cal_tan = 0;
			cal_cos = 0;
//			cal_tan = APL_Tan256(road_slope);	/* tan�� */
//			cal_cos = APL_Cos(cal_tan);			/* cos�� */
		}
	}
#else
	/* ��ӂƍ�������p�x�����߂āA�p�x����Y���W�����Z���Ă�����*/
	/* �p�x���Z�͕s�v�Ƃ̂��ƁB�P���ɋ����Ɗp�x���獂�����Z�o�ł��� @runshiwa������w�E */
	if(vy == 0)
	{
		rad = 0.0;
	}
	else
	{
		rad = APL_Atan2( vy, uRas_mid );
	}
	cal_tan = APL_Tan256((int16_t)rad);
#endif
	g_stRoadInfo.slope		= road_slope;		/*  */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static int16_t	Set_Road_Angle(void)
{
	int16_t	ret = 0;
	int16_t	road_angle;
	static int16_t	road_angle_old;
	
	/* �R�[�i�[ */
	road_angle_old = g_stRoadInfo.angle;
	road_angle = (int16_t)g_stRoadData[g_uCounter].bAngle - 0x80;		/* ���̊p�x	(0x80�Z���^�[) */
	road_angle = Mmax(Mmin(road_angle, 12), -12);

	g_stRoadInfo.angle = road_angle;		/*  */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static	int16_t	Set_Road_Distance(void)
{
	int16_t	ret = 0;
	int16_t	road_distance;
	static int16_t road_distance_old;
	
	road_distance_old = g_stRoadInfo.distance;	/* �O��l�X�V */
	road_distance = 0;
	g_stRoadInfo.distance	= road_distance;	/*  */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static int16_t	Set_Road_Object(void)
{
	int16_t	ret = 0;
	int16_t	road_object = 0;
	
	road_object = g_stRoadData[g_uCounter].bObject;	/* �o���|�C���g�̃I�u�W�F�N�g�̎�� */
	if(road_object != 0u)
	{
		SetAlive_EnemyCAR();	/* ���C�o���Ԃ��o�������� */
	}
	g_stRoadInfo.object		= road_object;		/*  */
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void Road_Pat_Main(void)
{
	static uint32_t	unRoadAnimeTime = 0xFFFFFFFFu;
	uint32_t	time_st = 0u;
	int16_t	speed;

	/* ���������݂̂̓��� */
	if(g_bRoadInitFlag == TRUE)
	{
		speed = 1;
		time_st = 0xFFFFu;
		unRoadAnimeTime = 0u;
	}
	else
	{
		GetStartTime(&time_st);		/* �J�n���Ԃ��擾 */
		GetMyCarSpeed(&speed);		/* �ԑ��𓾂� */
	}
	
	if(speed == 0u)	/* �ԑ�0km/h */
	{
		unRoadAnimeTime = time_st;	/* �����X�V */

		Road_Pat_Stop();	/* ���̃A�j����~ */
	}
	else if( (time_st - unRoadAnimeTime) < (362 / speed) )	/* �X�V�������� */
	{
		/* �Z���^�[���C���̒����ƊԊu�͂T�� */
		/* 60fps=17ms/f */
		/* �ő呬�x307km/h(85m/s,0.0853m/ms) 1f=1.45m�i�� */
		/* �Œᑬ�x6km/h(1.7m/s,0.0017m/ms) 1f=0.028m�i�� */
		/* 1LSB 10km/h(2.8m/s,0.0028m/ms) 1f=0.047m�i�� */
		/* 1times Cal 1m=4dot (17ms/f x 1/0.047 / speed) = (362/speed)*/

		Road_Pat_Stop();	/* ���̃A�j����~ */
	}
	else
	{
		unRoadAnimeTime = time_st;	/* �����X�V */
		
		Road_Pat_Base_Update();	/* ���̃A�j���X�V */
		
		Set_Road_Angle();		/* �R�[�X�̋Ȃ��� */
		Set_Road_Height();		/* �R�[�X�̕W�� */
		Set_Road_Slope();		/* �R�[�X�̌��ݒn����W���܂ł̌X�� */
		Set_Road_Distance();	/* ���������猻�ݒn�܂ł̋��� */
		Set_Road_Object();		/* ���C�o���Ԃ̏o�� */
		
		g_uCounter++;			/* �R�[�X�f�[�^�̃J�E���^�X�V */
		if(g_uCounter >= ROADDATA_MAX)
		{
			g_uCounter = 0u;
		}

		g_uRoadCycleCount++;	/* �R�[�X�X�V�t���[�����J�E���^ */
	}
	
	/* ���������݂̂̓��� */
	if(g_bRoadInitFlag == TRUE)
	{
		g_bRoadInitFlag = FALSE;
		GetStartTime(&time_st);		/* �J�n���Ԃ��擾 */
		unRoadAnimeTime = time_st;	/* �����X�V */
	}
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	GetRoadCycleCount(uint16_t *p_uCount)
{
	int16_t	ret = 0;
	
	*p_uCount = g_uRoadCycleCount;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
uint64_t GetRoadDataAddr(void)
{
	return (uint64_t)&g_stRoadData[0];
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	SetHorizon(uint8_t bMode)
{
	int16_t	ret = 0;
	int16_t	Horizon=0;
	int16_t	Height;

	Height = g_stRoadInfo.height;	/* �����������ύX */

	/* ������ */
	/* ���X�^�[���荞�ݏ����̊J�n�E�I���ʒu */
	/* �p�x�Ƌ����ƕW�����琅�����̈ʒu���Z�o����B */
	/* �p�x�Ƌ����ŋ��߂������ƕW������v���Ă���΁A���j�A�ɐL�k */
	/* �p�x�Ƌ����ŋ��߂��������W�����傫���ꍇ�A�r���ŃJ�b�g����铹 */
	/* �p�x�Ƌ����ŋ��߂��������W�����������ꍇ�A�Z���\�����铹 */
	switch(bMode)
	{
		case 0:		/* TPS */
		{
			g_stRoadInfo.Horizon_Base = Y_HORIZON_0;
			Horizon = Mmax(Mmin(Y_HORIZON_0 - Height, ROAD_0_MAX), ROAD_0_MIN);	/* �����ʒu����i�N���b�v�j */
			break;
		}
		case 1:		/* FPS */
		case 2:		/* FPS */
		default:	/* FPS */
		{
			g_stRoadInfo.Horizon_Base = Y_HORIZON_1;
			Horizon = Mmax(Mmin(Y_HORIZON_1 - Height, ROAD_1_MAX), ROAD_1_MIN);	/* �����ʒu����i�N���b�v�j */
			break;
		}
	}
	g_stRoadInfo.Horizon = Horizon;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static int16_t Load_Road_Background(int16_t Num)
{
	int16_t	ret = 0;
	int16_t	i, count, mod;
	uint16_t	*pImage = NULL;
	uint16_t	*pSrcBuf = NULL;
	uint32_t uWidth, uHeight, uFileSize;
	uint8_t ubPatNumber;
	uint16_t x, y;
	
	ubPatNumber = BG_CG + (uint8_t)Num;
	
	CG_File_Load( ubPatNumber );	/* �O���t�B�b�N�̓ǂݍ��� */
	
	pImage = Get_PicImageInfo( ubPatNumber, &uWidth, &uHeight, &uFileSize);	/* �C���[�W���̎擾 */
	
	if(pImage != NULL)
	{
		if(uWidth != 0u)
		{
			/* X */
			count = X_MAX_DRAW / uWidth;
			mod = X_MAX_DRAW % uWidth;
			
			if(mod != 0)	/* �]�肪����Ȃ���E�܂Ŗ��߂� */
			{
				count++;
			}
			
			/* �������ɕ~���l�߂� */
			for(i = 0; i < count; i++)
			{
				pSrcBuf = pImage;
				x = uWidth * i;
				y = Y_MIN_DRAW +        0 + Y_HORIZON_1 - uHeight + RASTER_MIN;
				G_BitBlt_From_Mem( x, y, 1, pSrcBuf, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);	/* �w�i(�㑤) */

				pSrcBuf = pImage;
				y = Y_MIN_DRAW + Y_OFFSET + Y_HORIZON_1 - uHeight + RASTER_MIN;
				G_BitBlt_From_Mem( x, y, 1, pSrcBuf, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);	/* �w�i(����) */

//				G_Load_Mem( ubPatNumber, uWidth * i, Y_MIN_DRAW +        0 + Y_HORIZON_1 - uHeight + RASTER_MIN, 1);	/* �w�i(�㑤) */
//				G_Load_Mem( ubPatNumber, uWidth * i, Y_MIN_DRAW + Y_OFFSET + Y_HORIZON_1 - uHeight + RASTER_MIN, 1);	/* �w�i(����) */
			}
		}
		else
		{
			ret = -1;	/* �������Ȃ� */
		}
	}
	else
	{
		ret = -1;		/* �摜���Ȃ� */
	}
	
	return ret;
}


#endif	/* RASTER_C */
