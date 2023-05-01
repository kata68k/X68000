#ifndef	BATTLEKATA_C
#define	BATTLEKATA_C

/* ���@�𓮂����v���O����*/

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>
#include <time.h>
#include <XSP2lib.H>
#include <PCG90.H>

#include "usr_style.h"
#include "usr_macro.h"
#include "BattleKata.h"

#include "BIOS_MFP.h"
#include "BIOS_MPU.h"
#include "BIOS_CRTC.h"
#include "BIOS_PCG.h"
#include "IF_FileManager.h"
#include "IF_Music.h"
#include "IF_Graphic.h"
#include "IF_Draw.h"
#include "IF_Input.h"
#include "IF_Text.h"
#include "IF_Task.h"
#include "APL_MACS.h"
#include "APL_Math.h"
#include "APL_Score.h"

/* �O���[�o���ϐ� */
SHIP *ship_data;	/* ���@�̃f�[�^ */
ENEMY *enemy_data;	/* �G�̃f�[�^ */
SHOT *shot_data;	/* ���e�̃f�[�^ */
AREA *area_data;	/* �ړ��͈̓f�[�^ */

int32_t g_nSuperchk;
int32_t g_nCrtmod;
uint8_t	g_bExit = TRUE;
uint32_t g_unFly_time;

/* �o�b�f�f�[�^ */
/*
	XSP �p PCG �z�u�Ǘ��e�[�u��
	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B
*/
char pcg_alt[PCG_MAX + 1];		/* XSP �p PCG �z�u�Ǘ��e�[�u��	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B*/
char pcg_dat[PCG_MAX * 128];	/* �o�b�f�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
unsigned short pal_dat[ 256 ];	/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */

short *seq_tbl;

/* �v���g�^�C�v�錾 */
void (*usr_abort)(void);	/* ���[�U�̃A�{�[�g�����֐� */
void sp_dataload(void);
void App_init( void );
static void App_exit(void);
void App_TimerProc( void );

void sp_dataload(void)
{
	FILE *fp;
	unsigned int i,j;

	/*�X�v���C�g���W�X�^������*/
	for(j = 0x80000000; j < 0x80000000 + 128; j++ ){
		SP_REGST(j,-1,0,0,0,0);
	}
	
	/*-----------------[ �o�b�f�f�[�^�ǂݍ��� ]-----------------*/

	fp = fopen( "data/sp/BK.SP" , "rb" ) ;
//	fp = fopen( "SP_DATA/SAKANA.SP" , "rb" ) ;
	j = fread( pcg_dat
			,  128		/* 1PCG = 128byte */
			,  PCG_MAX	/* 256PCG */
			,  fp
	) ;
	fclose( fp ) ;

//	for( i = 0 ; i < PCG_MAX ; i++ ){
//		SP_DEFCG( i, 1, &pcg_dat[i * 128] );
//	}
	
#if 0
	/*-----------[ PCG �f�[�^���c��ʗp�� 90 �x��] ]-----------*/

	for (i = 0; i < 256; i++) {
		pcg_roll90(&pcg_dat[i * 128], 1);
	}
#endif
	
	/*--------[ �X�v���C�g�p���b�g�f�[�^�ǂݍ��݂ƒ�` ]--------*/

	fp = fopen( "data/sp/BK.PAL" , "rb" ) ;
//	fp = fopen( "SP_DATA/SAKANA.PAL" , "rb" ) ;
	fread( pal_dat
		,  2		/* 1palet = 2byte */
		,  256		/* 16palet * 16block */
		,  fp
		) ;
	fclose( fp ) ;

	/* �X�v���C�g�p���b�g�ɓ]�� */
	for( i = 0 ; i < 256 ; i++ )
	{
		SPALET( (i&15) | (1<<0x1F) , i/16 + 1, pal_dat[i] ) ;
	}
}

void area_init( int32_t x, int32_t y )
{
	T_Clear();	/* �e�L�X�g���� */

	if(x < 0 && y < 0)
	{
		area_data->x = ship_data->x;
		area_data->y = ship_data->y;
	}
	else
	{
		area_data->x = x;
		area_data->y = y;
	}
	area_data->w = Mmul256(32);
	area_data->h = Mmul256(32);

	T_FillCircle(	Mdiv256(area_data->x),	/* �X�v���C�g�̍��W����Ȃ̂ŃI�t�Z�b�g���K�v */
					Mdiv256(area_data->y),
					Mdiv256(area_data->w) / 2, 1);

	area_data->flag = TP_ALIVE;
}

void game_init( void )
{
	ship_data	= (SHIP *) MyMalloc( sizeof(SHIP) );
	shot_data	= (SHOT *) MyMalloc( sizeof(SHOT) * SHOT_NUM );
	enemy_data	= (ENEMY *) MyMalloc( sizeof(ENEMY) * ENEMY_NUM );
	area_data	= (AREA *) MyMalloc( sizeof(AREA) );
}

void ship_init( void )
{
	ship_data->x = Mmul256(128);
	ship_data->y =  Mmul256(192);
	ship_data->angle = 0;
	ship_data->acc = 0;
	ship_data->flag = TP_ALIVE;
	ship_data->life = 4;

	M_SetMusic(0);	/* ���ʉ��Đ��̐ݒ� */
}

void ship_move( void )
{
	AREA *area;
	JOY_ANALOG_BUF	analog_data;

	area = area_data;

	if(g_bAnalogStickMode == TRUE)
	{
		GetAnalog_Info(&analog_data);	/* �A�i���O�f�[�^�ǂݍ��� */

		if( analog_data.r_stk_lr < 0x70 )	/* �� */
		{
			ship_data->angle += (0x80 - analog_data.r_stk_lr) >> 4;
			
			if(ship_data->angle >= 360)
			{
				ship_data->angle -= 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else if( analog_data.r_stk_lr > 0x90 )	/* �E */
		{
			ship_data->angle -= (analog_data.r_stk_lr - 0x80) >> 4;
			
			if(ship_data->angle < 0)
			{
				ship_data->angle += 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else /* �Ȃ� */
		{
			/* �������Ȃ� */
		}

		if( analog_data.l_stk_ud > 0x90 )	/* �� */
		{
			ship_data->acc += (analog_data.l_stk_ud - 0x80) >> 6;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else if( analog_data.l_stk_ud < 0x70 )	/* �� */
		{
			ship_data->acc += (0x80 - analog_data.l_stk_ud) >> 6;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else /* �Ȃ� */
		{
			ship_data->acc--;
			if(ship_data->acc <= 0)ship_data->acc = 0;
		}
	}
	else
	{
		if( (g_Input & KEY_LEFT) != 0 )	/* �� */
		{
			ship_data->angle += 4;
			
			if(ship_data->angle >= 360)
			{
				ship_data->angle -= 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else if( (g_Input & KEY_RIGHT) != 0 )	/* �E */
		{
			ship_data->angle -= 4;
			
			if(ship_data->angle < 0)
			{
				ship_data->angle += 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else /* �Ȃ� */
		{
			/* �������Ȃ� */
		}

		if( ( g_Input & KEY_UPPER ) != 0 )  /* �� */
		{
			ship_data->acc += 1;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else if( ( g_Input & KEY_LOWER ) != 0 )  /* �� */
		{
			ship_data->acc += 1;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else /* �Ȃ� */
		{
			ship_data->acc--;
			if(ship_data->acc <= 0)ship_data->acc = 0;
		}
	}
	ship_data->vx =  ship_data->acc * APL_Cos(ship_data->angle);
	ship_data->vy = -ship_data->acc * APL_Sin(ship_data->angle);
#if 0
	if( (ship_data->angle >= 0) && (ship_data->angle < 90) )
	{
	}
	else if( (ship_data->angle >= 90) && (ship_data->angle < 180) )
	{
	}
	else if( (ship_data->angle >= 180) && (ship_data->angle < 270) )
	{
	}
	else if(ship_data->angle >= 270)
	{
	}
	else /* �Ȃ� */
	{
		 /* �������Ȃ� */
	}
#endif

	if( ( g_Input & KEY_UPPER ) != 0) { /* �� */
		if(g_bAnalogStickMode == TRUE)
		{
			ship_data->x -= ship_data->vx;
			ship_data->y -= ship_data->vy;
		}
		else{
			ship_data->x += ship_data->vx;
			ship_data->y += ship_data->vy;
		}
//		printf("V=%3d,%3d AC=%3d\n", ship_data->vx, ship_data->vy, ship_data->acc);
		M_Play(3, 0);
	}
	else if( ( g_Input & KEY_LOWER ) != 0 ) { /* �� */
		if(g_bAnalogStickMode == TRUE)
		{
			ship_data->x += ship_data->vx;
			ship_data->y += ship_data->vy;
		}
		else{
			ship_data->x -= ship_data->vx;
			ship_data->y -= ship_data->vy;
		}
//		printf("V=%3d,%3d AC=%3d\n", ship_data->vx, ship_data->vy, ship_data->acc);
		M_Play(3, 1);
	}
	else /* �Ȃ� */
	{
		 /* �������Ȃ� */
	}
	
	/* �ړ��͈� */
	if(area->flag == TP_ALIVE)	/* ������ */
	{
		if(ship_data->x <  area->x - Mdiv2(area->w))	ship_data->x = Mmax( VX_MIN,	area->x - Mdiv2(area->w) );
		if(ship_data->x >= area->x + Mdiv2(area->w))	ship_data->x = Mmin( VX_MAX,	area->x + Mdiv2(area->w) );
		if(ship_data->y <  area->y - Mdiv2(area->h))	ship_data->y = Mmax( VY_MIN,	area->y - Mdiv2(area->h) );
		if(ship_data->y >= area->y + Mdiv2(area->h))	ship_data->y = Mmin( VY_MAX,	area->y + Mdiv2(area->h) );
	}
	else	/* �����Ȃ� */
	{
		if(ship_data->x <= VX_MIN + 8)ship_data->x = VX_MIN + 8;
		if(ship_data->x >= VX_MAX - 8)ship_data->x = VX_MAX - 8;
		if(ship_data->y <= VY_MIN + 8)ship_data->y = VY_MIN + 8;
		if(ship_data->y >= VY_MAX - 8)ship_data->y = VY_MAX - 8;
	}

}

void ship_sp( void )
{
	unsigned int pat = 0x18;
	unsigned int v = 0, h = 0;
	int32_t x, y;

	x = Mdiv256(ship_data->x);
	y = Mdiv256(ship_data->y);
#if 0
	unsigned int x_ofst = 0, y_ofst = 0;
	int32_t x1, y1;
	int32_t sin, cos;

	cos = APL_Cos(ship_data->angle);
	sin = APL_Sin(ship_data->angle);

	x1 = ((x * cos) - (y * sin)) >> 16;
	y1 = ((x * sin) + (y * cos)) >> 16;
	
	Draw_Line(x - 8, y - 8, (x - 8) + 4 * x1, (y - 8) + 4 * y1, F_MOJI, 0xFFFF);
	Draw_Line(x - 8, y - 8, (x - 8) + 4 * x1, (y - 8) + 4 * y1, F_MOJI_BAK, 0xFFFF);
#endif

	if( (ship_data->angle >= 0) && (ship_data->angle < 90) )
	{
		v = 0;
		h = 0;
		pat += (ship_data->angle / 12);
	}
	else if( (ship_data->angle >= 90) && (ship_data->angle < 180) )
	{
		v = 0;
		h = 1;
		pat += 7 -(ship_data->angle - 90) / 12;
	}
	else if( (ship_data->angle >= 180) && (ship_data->angle < 270) )
	{
		v = 1;
		h = 1;
		pat += (ship_data->angle - 180) / 12;
	}
	else if(ship_data->angle >= 270)
	{
		v = 1;
		h = 0;
		pat += 7 - (ship_data->angle - 270) / 12;
	}
	else /* �Ȃ� */
	{
		 /* �������Ȃ� */
	}

	/*���@�𓮂���*/
	xsp_set( X_OFFSET + x - 8, Y_OFFSET + y - 8, pat, SetXSPinfo(v, h, 0x0A, 0x20));
#if 0
	{
		int8_t	sBuf[128];
		memset(sBuf, 0, sizeof(sBuf));
		sprintf(sBuf, "%d (%d, %d)", ship_data->angle, x1, y1);
		Draw_Message_To_Graphic(sBuf, 0, 128, F_MOJI, F_MOJI_BAK);
	}
#endif
}

void shot_init( void )
{
	short i;
	SHOT *shot;
	
	for( i = 0, shot = shot_data; i < SHOT_NUM; i++, shot++ ){
		shot->flag = SHOT_ST_OUT;
		shot->pat = 0;
	}
}

void shot_move( void )
{
	int i;
	SHOT *shot;
	
	/* �e�����܂� */
	if( ((g_Input & KEY_A) != 0u) && (area_data->flag == TP_ALIVE)){ /* �`�{�^�� */
		
		for( i = 0, shot = shot_data; i < SHOT_NUM; i++, shot++ ) {
			if( shot->flag == SHOT_ST_OUT )
			{
				shot->flag = SHOT_ST_MOVE;
				shot->x = ship_data->x;
				shot->y = ship_data->y;
				shot->ex = shot->x;
				shot->ey = shot->y;
				shot->vx =  ( SHOT_VX * APL_Cos(ship_data->angle) );
				shot->vy =  ( SHOT_VY * APL_Sin(ship_data->angle) );
				shot->acc = - 1024;
			}
			else if(shot->flag == SHOT_ST_MOVE)
			{
				shot->acc = 0;
				shot->flag = SHOT_ST_DOWN;
			}
		}
	}

	/* �e�𓮂����܂� */
	for( i = 0, shot = shot_data; i < SHOT_NUM; i++, shot++ ) {
		if( shot->flag == SHOT_ST_OUT )		continue;

		if(shot->flag == SHOT_ST_MOVE)
		{
			shot->x += shot->vx;
			shot->y -= shot->vy;

			shot->ex += shot->vx;
			shot->acc += 16;
			shot->ey += shot->acc;

			shot->pat = Mmin(Mmax(Mdiv16384(shot->y - shot->ey), 0), 3);
			if(((shot->y - shot->ey) >> 8) <= 0)
			{
				shot->ex = shot->x;
				shot->ey = shot->y;
			}

			if(shot->acc > 0 )
			{
				shot->flag = SHOT_ST_DOWN;
			}
		}
		else if( (shot->flag == SHOT_ST_DOWN) || (shot->flag == SHOT_ST_TOUCH) || (shot->flag == SHOT_ST_RUN) )
		{
			shot->x += shot->vx;
			shot->y -= shot->vy;

			shot->ex += shot->vx;
			shot->acc += 64;
			shot->ey += shot->acc;

			shot->pat = Mmin(Mmax(((shot->y - shot->ey) >> 8) / 64, 0), 3);
			if(((shot->y - shot->ey) >> 8) <= 0)
			{
#if 0
				if(shot->flag == SHOT_ST_DOWN){
					int8_t	sBuf[128];
					memset(sBuf, 0, sizeof(sBuf));
					sprintf(sBuf, "n(%d, %d)v(%d, %d)e(%d, %d)a%d", shot->x, shot->y, shot->vx, shot->vy, shot->ex, shot->ey, shot->acc);
					Draw_Message_To_Graphic(sBuf, 0, 240, F_MOJI, F_MOJI_BAK);
				}
#endif
				shot->ex = shot->x;
				shot->ey = shot->y;
				if(shot->flag == SHOT_ST_DOWN)
				{
					shot->flag = SHOT_ST_TOUCH;
				}
				else
				{
					shot->flag = SHOT_ST_RUN;
				}

				if(shot->vx < 0)		shot->vx+=4;
				else if(shot->vx > 0)	shot->vx-=4;
				else ;

				if(shot->vy < 0)		shot->vy+=4;
				else if(shot->vy > 0)	shot->vy-=4;
				else ;

				if( (Mabs(shot->vx) <= 4) && (Mabs(shot->vy) <= 4) )	/* �]�����~ */
				{
					shot->flag = SHOT_ST_OUT;
					ship_data->life--;	
					ADPCM_Play(3);
					S_Set_Combo(0);	/* �R���{�J�E���^ ���Z�b�g */
				}
			}			
		}		
#ifdef DEBUG
		if(shot->pat >= 4)
		{				
			printf("shot_se=%x,%x,%x\n",  shot->pat, shot->y, shot->ey);
			KeyHitESC();	/* �f�o�b�O�p */
		}
#endif

		/* ��ʊO���� */
		if((( (shot->x <= VX_MIN) && (shot->ex <= VX_MIN) )
		||  ( (shot->y <= VY_MIN) && (shot->ey <= VY_MIN) )
		||  ( (shot->x >= VX_MAX) && (shot->ex >= VX_MAX) )
		||  ( (shot->y >= VY_MAX) && (shot->ey >= VY_MAX) ) )
		&& (shot->flag != SHOT_ST_MOVE) )
		{
			shot->flag = SHOT_ST_OUT;
			ship_data->life--;	
			ADPCM_Play(3);
			S_Set_Combo(0);	/* �R���{�J�E���^ ���Z�b�g */
		}
	}
}

void shot_se( void )
{
	static char key_f_A = 0;
	static char key_f_B = 0;
	static short i;
	short j;
	SHOT *shot;
	
	for( j = 0, shot = shot_data; j < SHOT_NUM; j++, shot++ ) {

		/* �`�{�^���̂r�d */
		if( (g_Input & KEY_A) != 0u ){ /* �`�{�^�� */

			if( key_f_A == 0 )
			{
				key_f_A = 1;

				if(area_data->flag == TP_ALIVE)
				{
					i = 0;

					ADPCM_Play(1);

					g_unFly_time = 0xFFFF;

					if(shot->acc <= 0 )
					{
						M_Play(0, i++);
					}
				}
			}
		}
		else
		{
			if( key_f_A == 1 )
			{
				if(shot->acc > 0 )
				{
					M_Play(1, (i++) % 128);
				}
			}
			if(shot->flag != SHOT_ST_MOVE)
			{
				key_f_A = 0;
			}
		}

		/* �a�{�^���̂r�d */
		if( ((g_Input & KEY_B) != 0u) && (key_f_B == 0)){ 

			key_f_B = 1;
			
			ADPCM_Play(2);
		}
		else
		{
			key_f_B = 0;
		}
	}
}

void enemy_init( void )
{
	ENEMY *enemy;
	short i;
	/* �G�̏����� */
	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ ) {
		enemy->flag = ENEMY_STDBY;
		enemy->x = Mmul256(16 + (rand()&1) * 16 + (i+1) * 12);
		enemy->y = Mmul256(16 + (rand() % 224));
		enemy->vx = 0 << 8;// (( rand()&1 ) * 2) - 1;
		enemy->vy = 0 << 8;// (( rand()&1 ) * 2) - 1;
		enemy->delay = 0;
	}
}
void enemy_set( int16_t nNum )
{
	short i;
	ENEMY *enemy;

	if(nNum <= 0)nNum = ENEMY_NUM;
	
	nNum = Mmin(nNum, ENEMY_NUM);

	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ )
	{
		if(i < nNum)
		{
			enemy->flag = ENEMY_ALIVE;
		}
		else
		{
			enemy->flag = ENEMY_DEAD;
		}
	}
}

void enemy_green_set( void )
{
	ENEMY *enemy;
	short i, j;
	
	_iocs_apage(1);		/* �O���t�B�b�N�̏�������(1) */

	/* �G�̏����� */
	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ ) 
	{
		if(enemy->flag == ENEMY_DEAD)continue;

		for( j = 0; j < 32; j++ ){
			Draw_Circle(	Mdiv256(enemy->x),
							Mdiv256(enemy->y) + 8,
							j, i+0x60, 0, 360, 64);
		}
	}
}
void enemy_move( void )
{
	short i;
	ENEMY *enemy;
	
	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ ) {
		if( enemy->flag == ENEMY_DEAD ) continue;
		
		/* �ړ����[�`�� */
		if( ((enemy->x + enemy->vx) < VX_MAX) && ((enemy->x + enemy->vx) >= VX_MIN) )
			enemy->x += enemy->vx;
		else
			enemy->vx *= -1;

		if( ((enemy->y + enemy->vy) < VY_MAX) && ((enemy->y + enemy->vy) >= VY_MIN) )
			enemy->y += enemy->vy;
		else
			enemy->vy *= -1;

		if( (enemy->x + enemy->vx) < 0 ) enemy->x = VX_MAX;
	}
}

void atari( void )
{
	ENEMY *enemy;
	AREA *area;
	short i;
	int32_t point;

	area = area_data;

	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ ) {
		SHOT *shot;
		int32_t j;
		int64_t dx, dy;
		
		if( enemy->flag == ENEMY_DEAD ) continue;

		if( ship_data->flag != TP_DEAD ) {
			
			dx = Mabs(enemy->x - ship_data->x);
			dy = Mabs(enemy->y - ship_data->y);
			
			if( (dx < ATARI_YOKO) && (dy < ATARI_TATE) ) {
				if( (enemy->flag != ENEMY_DEAD)  && (area->flag == TP_ALIVE) )
				{
					enemy->flag = ENEMY_DEAD;
					area_init(enemy->x, enemy->y);	/* �������X�V */

					/* �X�R�A */
					S_GetPos(Mdiv256(enemy->x) - 16, Mdiv256(enemy->y) - 36);	/* �X�R�A�\�����W�X�V */
					point = (i+1) * 100;
					S_Add_Score_Point(point);	/* �X�R�A�X�V */

					ship_data->life++;	/* �V���b�g��[ */

					ADPCM_Play(4);
				}
				else if( (enemy->flag == ENEMY_BOM) && (area->flag == TP_DEAD) )
				{
					enemy->flag = ENEMY_DEAD;
					area_init(enemy->x, enemy->y);	/* �������X�V */
					
					/* �X�R�A */
					S_GetPos(Mdiv256(enemy->x) - 16, Mdiv256(enemy->y) - 36);	/* �X�R�A�\�����W�X�V */
					point = 100 * (65 - enemy->delay);
					S_Add_Score_Point(point);	/* �X�R�A�X�V */
					
					ADPCM_Play(2);
				}
				continue;
			}
		}
		
		for( j = 0, shot = shot_data; j < SHOT_NUM; j++, shot++ ) {
			if( shot->flag == SHOT_ST_OUT )		continue;
			if( shot->flag == SHOT_ST_MOVE)		continue;
			if( (shot->x != shot->ex) || (shot->y != shot->ey) )continue;
//			if(shot->pat > 0)continue;

			dx = (shot->x - enemy->x);
			dx *= dx;
			dy = (shot->y - enemy->y);
			dy *= dy;
			
			if( ((dx + dy) >= 0) && ((dx + dy) < ATARI_XY) ) {
				enemy->flag = ENEMY_BOM;

//				area_init(enemy->x, enemy->y);	/* �������X�V */
				area->flag = TP_DEAD;

#if 0
				{
					int8_t	sBuf[128];
					memset(sBuf, 0, sizeof(sBuf));
					sprintf(sBuf, "%d, %d", shot->flag, shot->pat);
					Draw_Message_To_Graphic(sBuf, 0, 224, F_MOJI, F_MOJI_BAK);
				}
#endif
				if(shot->flag == SHOT_ST_TOUCH)
				{
					/* �X�R�A */
					point = (i+1) * 1000;
#if 0
					/* ���� */
					xsp_off();			/* XSP OFF */
					MOV_Play(0);		/* �J�b�g�C�� */
					/* ��ʃ��[�h�Đݒ� */
					CRTC_INIT(0x10A);	/* ��ʃ��[�h������ */
					PCG_VIEW(0x01);		/* �X�v���C�g������ */
					xsp_on();			/* XSP ON */
					/* PCG �f�[�^�� PCG �z�u�Ǘ����e�[�u�����w�� */
					xsp_pcgdat_set(pcg_dat, pcg_alt, sizeof(pcg_alt));
#endif
					/* ���ʉ� */
					ADPCM_Play(7);
				}
				else
				{
					/* �X�R�A */
					point = (i+1) * 100;
					/* ���ʉ� */
					ADPCM_Play(5);
				}
				S_GetPos(Mdiv256(enemy->x) - 16, Mdiv256(enemy->y) - 48);	/* �X�R�A�\�����W�X�V */
				S_Add_Score(point);	/* �X�R�A�X�V */
//				S_Add_Score((dx + dy));	/* �X�R�A�X�V */

				shot->flag = SHOT_ST_OUT;
#ifdef DEBUG
//				if(i==0)
//				{				
//					printf("atari=(%x,%x)(%x,%x)(%x,%x)\n",  shot->x, shot->y, enemy->x, enemy->y, dx, dy);
//					KeyHitESC();	/* �f�o�b�O�p */
//				}
#endif
			}
		}
	}
}

int16_t stage_clear(void)
{
	int16_t ret = 0;
	ENEMY *enemy;
	short i;

	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ ) {
		if( enemy->flag != ENEMY_DEAD )
		{
			ret = 1;
		}
	}
	return ret;
}

void trans_sp( void )
{
	SHOT *shot;
	ENEMY *enemy;
	
	int32_t i,j;
	int32_t ret;
	
	/* ���@�̕\�� */
	ship_sp();
	
	/*���e�̕\��*/
	for( i = 0, shot = shot_data; i < SHOT_NUM; i++, shot++ ) {
		switch( shot->flag )
		{
			case SHOT_ST_OUT:
				xsp_set( 0, 0, 0x25 + (shot->pat), SetXSPinfo(0, 0, 0x0B, 0x20) );
				break;
			default:
				xsp_set( X_OFFSET + Mdiv256(shot->x) - 8, 		Y_OFFSET + Mdiv256(shot->y) - 8, 		0x25 , 				SetXSPinfo(0, 0, 0x0B, 0x20) );	/* �e */
				xsp_set( X_OFFSET + Mdiv256(shot->ex) + 2 - 8, 	Y_OFFSET + Mdiv256(shot->ey) - 2 - 8,	0x26 + (shot->pat), SetXSPinfo(0, 0, 0x0B, 0x30) );	/* �{�[�� */
				break;
		}
//		Draw_Pset(Mdiv256(shot->x), Mdiv256(shot->y), 1);
	}
	
	/* �G�̕\�� */
	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ ) {
		switch( enemy->flag )
		{
			case ENEMY_DEAD:
				xsp_set( 0, 0, 0x30 + (i % 18), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20) );	/* ��ʊO */
				break;
			case ENEMY_BOM:
				j = enemy->delay;
				if(j < 64)
				{
					_iocs_apage(1);		/* �O���t�B�b�N�̏�������(1) */
					ret = Draw_Circle(	Mdiv256(enemy->x),
										Mdiv256(enemy->y) + 8,
										j>>1, j, 0, 360, 64);
					if(ret < 0)printf("error(%d):Draw_Circle\n", ret);

					/* ���� */
					xsp_set( X_OFFSET + Mdiv256(enemy->x) - 16, Y_OFFSET + Mdiv256(enemy->y) - 24, 0x50 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					xsp_set( X_OFFSET + Mdiv256(enemy->x) + 0, Y_OFFSET + Mdiv256(enemy->y) - 24, 0x51 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					xsp_set( X_OFFSET + Mdiv256(enemy->x) - 16, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x52 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					xsp_set( X_OFFSET + Mdiv256(enemy->x) + 0, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x53 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					enemy->delay++;
				}
				else
				{
					xsp_set( X_OFFSET + Mdiv256(enemy->x) - 8, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x49, SetXSPinfo(0, (enemy->vx < 0), 0x01, 0x20));	/* �G�l���M�[�� */
				}
//				ret = _iocs_apage(0);		/* �O���t�B�b�N�̏�������(0) */
//				if(ret < 0)printf("error(%d):_iocs_apage\n", ret);
//				ret = _iocs_vpage(0b0011);	/* �O���t�B�b�N�\��(page3:0n page2:0n page1:0n page0:0n) */
//				if(ret < 0)printf("error(%d):_iocs_vpage\n", ret);
//				ret = Draw_Fill( Mdiv256(enemy->x) - 32 - X_OFFSET/2,
//								 Mdiv256(enemy->y) - 32 - Y_OFFSET/2,
//								 Mdiv256(enemy->x) + 32 - X_OFFSET/2,
//								 Mdiv256(enemy->y) + 32 - Y_OFFSET/2, 0xFF);
//				if(ret < 0)printf("error(%d):Draw_Box\n", ret);
				break;
			default:
				xsp_set( X_OFFSET + Mdiv256(enemy->x) - 8, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x42, SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));	/* �� */
				xsp_set( X_OFFSET + Mdiv256(enemy->x) - 8, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x30 + (i % 18), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x30));	/* �ԍ� */
				break;
		}	
//		Draw_Circle(	Mdiv256(enemy->x),
//						Mdiv256(enemy->y),
//						8, i+1, 0, 360, 256);
	}
	/* �c�@�\�� */
	if(ship_data->life > 4)
	{
		ST_TASK stTask;
		int8_t	sBuf[8];

		GetTaskInfo(&stTask);	/* �^�X�N�擾 */
		if(stTask.b496ms == TRUE)
		{
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "x%d", ship_data->life);
			Draw_Message_To_Graphic(sBuf, 240, 0, F_MOJI, F_MOJI_BAK);
		}

		xsp_set( X_OFFSET + 224, Y_OFFSET, 0x29, SetXSPinfo(0, 0, 0x0B, 0x30) );
	}
	else
	{
		for(i = 0; i < ship_data->life; i++)
		{
			xsp_set( X_OFFSET + 240 - (i * 16), Y_OFFSET, 0x29, SetXSPinfo(0, 0, 0x0B, 0x30) );
		}
	}
}

void bg_pest( void )
{
	int i;
	/* BG0 > SP > BG1 */
	BGCTRLST(0,0,1);
	BGCTRLST(1,1,1);
	BGTEXTCL(0, SetBGcode(0,0,0x02, (((0x06 * 16) + 0x03) * 4) + 3) );	/* �� */
	BGTEXTCL(1, SetBGcode(0,0,0x0A, (((0x05 * 16) + 0x0C) * 4) + 0) );	/* �� */
	BGSCRLST(0,0,0);
	BGSCRLST(1,0,0);
//	BGTEXTCL(1,0x0a70);
//	BGCTRLGT(1);
//	BGTEXTGT(1,1,0);

	for( i = 0; i < 4096; i++ )
	{
		if((rand() % 2) == 0)
		{
	//		BGTEXTST( 0, i % 64, i / 64, SetBGcode( 0, 0, 0x00, (((0x0D * 16) + 0x00) * 4) + (i % 4)) );
		}
	}
}

void bg_move( void )
{
	static unsigned short i = 0x3FF;

	BGSCRLST(0x80000000, i, 0);
	BGSCRLST(0x80000001, 0, 0);
	i+=2;
	if(i >= 0x3FF)i = 0;

//	BGTEXTCL(1,0x0a70);
//	BGCTRLGT(1);
//	BGTEXTGT(1,1,0);
//	BGTEXTST(1,0,0,0x0a70);
}

void App_init( void )
{
	puts("App_init start");

	srand(time(NULL));	/* ���������� */

	/* ��O�n���h�����O���� */
	g_bExit = TRUE;
	usr_abort = App_exit;	/* ��O�����ŏI�����������{���� */
	init_trap14();			/* �f�o�b�O�p�v���I�G���[�n���h�����O */

	TimerD_INIT();			/* Timer-D������ */

	//puts("�������J�n");
	Init_FileList_Load();	/* ���X�g�t�@�C���̓ǂݍ��� */

	puts("Music ...");
	/* ���y */
	Init_Music();	/* ������(�X�[�p�[�o�C�U�[���[�h���O)	*/

	Music_Play(5);	/* ���[�f�B���O�� */

	puts("Super ...");
	/* �X�[�p�[�o�C�U�[���[�h�ւ̐؂�ւ��́A���y�̏�������ɂ�邱�� */
	g_nSuperchk = _dos_super(0);	/* �X�[�p�[�o�C�U�[���[�h�J�n */
	if( g_nSuperchk < 0 ) {
		puts("���łɓ����Ă�B");
		KeyHitESC();	/* �f�o�b�O�p */
	} else {
		puts("�������B");
	}
	
	puts("Movie ...");
	/* ���� */
	MOV_INIT();		/* ���������� */

	puts("CRT ...");
	/* ��� */
	g_nCrtmod = CRTC_INIT(0x100 + 0x0A);	/* ��ʂ̏����ݒ� 256x256 G256 */
	
	puts("Graphic ...");
	/* �Î~�� */
	G_INIT();		/* �O���t�B�b�N������ */
	
	puts("Text ...");
	T_INIT();		/* �e�L�X�g������ */
	
	puts("PCG ...");
	sp_dataload();	/* �X�v���C�g�f�[�^�̓ǂݍ��� */
	PCG_VIEW(0x01);	/* �X�v���C�g������ */
//	PCG_INIT();		/* �X�v���C�g������ */

	puts("XSP ...");
	/*---------------------[ XSP �������� ]---------------------*/
	/* XSP �̏����� */
	xsp_on();

	/* PCG �f�[�^�� PCG �z�u�Ǘ����e�[�u�����w�� */
	xsp_pcgdat_set(pcg_dat, pcg_alt, sizeof(pcg_alt));

	/* �c��ʃ��[�h */
//	xsp_vertical(1);
	/*---------------------[ XSP �������� ]---------------------*/

	/* �^�X�N���� */
	TaskManage_Init();

	/* �X�R�A������ */
	S_All_Init_Score();

	/* �Q�[���S�ʂ̏����� */
	game_init();

	/* �A�i���O�X�e�B�b�N���[�h */
	g_bAnalogStickMode = FALSE;

	Set_CRT_Contrast(0);	/* �R���g���X�g�� */
//	puts("App_init end");
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void App_exit(void)
{
	puts("App_exit start");

	if(g_bExit == TRUE)
	{
		puts("�G���[���L���b�`�I ESC to skip");
		KeyHitESC();	/* �f�o�b�O�p */
		exit(0);
	}
	g_bExit = TRUE;
	
	TaskManage_Init();		/* TimerTask ������ */
	TimerD_EXIT();			/* Timer-D �I�� */

	PCG_VIEW(0x00u);		/* �X�v���C�g���a�f��\�� */
	puts("App_exit PCG_VIEW");

	/* XSP �̏I������ */
	xsp_off();
	puts("App_exit xsp_off");

	/* ��� */
	g_nCrtmod = CRTC_INIT(g_nCrtmod);	/* ���[�h�����Ƃɖ߂� */
	puts("App_exit CRTC_INIT");

	/* ���y */
	Exit_Music();			/* ���y��~ */
	puts("App_exit Exit_Music");

	/* MFP */
//	MFP_EXIT();				/* MFP�֘A�̉��� */
//	puts("App_exit MFP_EXIT");

	/* �e�L�X�g */
	T_EXIT();				/* �e�L�X�g�I������ */
	puts("App_exit T_EXIT");

	MyMfree(0);				/* �S�Ẵ���������� */
	puts("App_exit MyMfree");

	_dos_kflushio(0xFF);	/* �L�[�o�b�t�@���N���A */
	puts("App_exit _dos_kflushio");

	_dos_super(g_nSuperchk);		/*�X�[�p�[�o�C�U�[���[�h�I��*/
	puts("App_exit _dos_super");

	puts("App_exit End");
}

void App_TimerProc( void )
{
	TaskManage();
	/* ������ ���̊Ԃɏ��������� ������ */


	/* ������ ���̊Ԃɏ��������� ������ */
}

int32_t main(void)
{
	int16_t	loop = 1;

	App_init();	/* �����ݒ� */

	do{
		uint32_t time_st;
		ST_TASK stTask;

		/* �������� */
		xsp_vsync2(1);

		/* �����ݒ� */
		GetNowTime(&time_st);	/* ���C�������̊J�n�������擾 */
		SetStartTime(time_st);	/* ���C�������̊J�n�������L�� */

		/* �^�X�N */
		TaskManage();
		GetTaskInfo(&stTask);	/* �^�X�N�擾 */

		/* ���͏��� */
		{
			int16_t	input = 0;
#if 1
			/* �A�i���O�X�e�B�b�N�^�f�W�^���X�e�B�b�N�ؑ� */
			if(ChatCancelSW((g_Input & KEY_b_TAB)!=0u, &g_bAnalogStickMode_flag) == TRUE)	/* TAB�ŃA�i���O�X�e�B�b�NON/OFF */
			{
				if(g_bAnalogStickMode == FALSE)	g_bAnalogStickMode = TRUE;
				else							g_bAnalogStickMode = FALSE;
			}
			get_keyboard(&input, 0, 0);		/* �L�[�{�[�h���� */
#else
			g_bAnalogStickMode = FALSE;
#endif
			if(g_bAnalogStickMode == TRUE)
			{
				get_ajoy(&input, 0, 0, 1);	/* �A�i���O�W���C�X�e�B�b�N���� 0:X680x0 1:etc */
			}
			else
			{
				get_djoy(&input, 0, 0);		/* �W���C�X�e�B�b�N���� */
			}
			g_Input = input;
		}
		
		/* �d�r�b�|�[�Y */
		if( ( BITSNS( 0 ) & 0x02 ) != 0 ) {
			puts("ESC");
			SetTaskInfo(SCENE_EXIT);	/* �I���V�[���֐ݒ� */
			break;
		}

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* �������V�[�� */
			{
				SetTaskInfo(SCENE_TITLE_S);	/* �^�C�g���V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_TITLE_S:	/* �^�C�g���V�[��(�J�n����) */
			{
				/* �O���t�B�b�N�\�� */
				G_HOME(0);			/* �z�[���ʒu�ݒ� */
				G_CLR();			/* �O���t�B�b�N�N���A */
				G_PaletteSetZero();	/* �O���t�B�b�N�p���b�g�̊�{�ݒ� */
				T_Clear();			/* �e�L�X�g�N���A */
	
				G_CLR_AREA(0,0,256,256,1);	/* �O���t�B�b�N�̈�N���A */
				G_Load_Mem(2, 0, 0, 1);	/* �O���t�B�b�N�̓ǂݍ��� */	/* �摜��\��������ƏI�����ɂ������Ȗ��߃G���[�ɂȂ� */
				
				S_Init_Score();		/* �X�R�A������ */
				PutGraphic_To_Symbol24("Ball und Panzer Golf", 9, 1, 0x12);
				PutGraphic_To_Symbol24("Ball und Panzer Golf", 8, 0, 0x86);
				PutGraphic_To_Symbol12("Ver 0.91", 176, 24, 0x86);
//				PutGraphic_To_Symbol12("Ver X68KBBS", 150, 24, 0x86);
				Draw_Message_To_Graphic("START", 160, 116, F_MOJI, 0x30);
				Draw_Message_To_Graphic("PUSH A BUTTON", 160, 128, F_MOJI, 0x30);

				Music_Stop();	/* ���y�Đ� ��~ */
				Music_Play(7);	/* �f����� */
//				Music_Play(4);	/* �^�C�g����� */

				Set_CRT_Contrast(-1);	/* �R���g���X�gdef */

				SetTaskInfo(SCENE_TITLE);	/* �^�C�g���V�[���֐ݒ� */
				break;
			}
			case SCENE_TITLE:	/* �^�C�g���V�[�� */
			{
				if(stTask.b496ms == TRUE)
				{
					PutGraphic_To_Symbol12("[TAB]Key", 8, 52, 0x86);
					if(g_bAnalogStickMode == FALSE)
					{
						PutGraphic_To_Symbol12("Analog", 8, 64, 0x71);
						PutGraphic_To_Symbol12("Digtal Input", 8, 64, 0x86);
					}
					else{
						PutGraphic_To_Symbol12("Digtal", 8, 64, 0x71);
						PutGraphic_To_Symbol12("Analog Input", 8, 64, 0x86);
					}
				}

				if(g_Input == KEY_A)	/* A�{�^�� */
				{
					SetTaskInfo(SCENE_TITLE_E);	/* �^�C�g���V�[��(�J�n����)�֐ݒ� */
				}
				break;
			}
			case SCENE_TITLE_E:	/* �^�C�g���V�[��(�I������) */
			{
				G_HOME(0);			/* �z�[���ʒu�ݒ� */
				G_CLR();			/* �O���t�B�b�N�N���A */
				G_PaletteSetZero();	/* �O���t�B�b�N�p���b�g�̊�{�ݒ� */

				SetTaskInfo(SCENE_START_S);	/* �Q�[���X�^�[�g(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_DEMO_S:	/* �f���V�[��(�J�n����) */
			{
				Music_Play(6);	/* �f����� */

				SetTaskInfo(SCENE_DEMO);	/* �f���V�[���֐ݒ� */
				break;
			}
			case SCENE_DEMO:	/* �f���V�[�� */
			{
				SetTaskInfo(SCENE_DEMO_E);	/* �f���V�[��(�I������)�֐ݒ� */
				break;
			}
			case SCENE_DEMO_E:	/* �f���V�[��(�I������) */
			{
				Music_Stop();	/* ���y�Đ� ��~ */

				SetTaskInfo(SCENE_TITLE_S);	/* �^�C�g���V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_START_S:	/* �Q�[���J�n�V�[��(�J�n����) */
			{
				Set_CRT_Contrast(0);	/* �R���g���X�g�� */

//				Music_Play(7);	/* �f����� */

				/* �O���t�B�b�N�\�� */
				G_HOME(0);			/* �z�[���ʒu�ݒ� */
				G_CLR();			/* �O���t�B�b�N�N���A */
				G_PaletteSetZero();	/* �O���t�B�b�N�p���b�g�̊�{�ݒ� */
				T_Clear();			/* �e�L�X�g�N���A */

				SetTaskInfo(SCENE_START);	/* �Q�[���J�n�V�[���֐ݒ� */
				break;
			}
			case SCENE_START:	/* �Q�[���J�n�V�[�� */
			{
				SetTaskInfo(SCENE_START_E);	/* �Q�[���J�n�V�[��(�I������)�֐ݒ� */
				break;
			}
			case SCENE_START_E:	/* �Q�[���J�n�V�[��(�I������) */
			{

				SetTaskInfo(SCENE_GAME_S);	/* �Q�[���V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_GAME_S:	/* �Q�[���V�[��(�J�n����) */
			{
				/*���@�̏����ݒ�*/
				ship_init();

				/*���e�̏����ݒ�*/
				shot_init();

				/* �G�̏����ݒ� */
				enemy_init();

				/* �s�������G���A�̏����ݒ� */
				area_init(-1, -1);

				G_CLR_AREA(0,0,256,256,1);	/* �O���t�B�b�N�̈�N���A */
				G_Load_Mem(1, 0, 0, 1);	/* �O���t�B�b�N�̓ǂݍ��� */	/* �摜��\��������ƏI�����ɂ������Ȗ��߃G���[�ɂȂ� */
				
				enemy_set(18);			/* �G�̃X�e�[�^�X�i������j */
				enemy_green_set();		/* �O���[���𐶐� */
				
				Music_Stop();	/* ���y�Đ� ��~ */

				Set_CRT_Contrast(-1);	/* �R���g���X�gdef */

				ADPCM_Play(6);

				Music_Play(8);	/* �Q�[���� */

				SetTaskInfo(SCENE_GAME);	/* �Q�[���V�[���֐ݒ� */
				break;
			}
			case SCENE_GAME:	/* �Q�[���V�[�� */
			{
				/* �L�����N�^�𑀍삵�܂��B*/
				ship_move();

				shot_move();
				shot_se();

				enemy_move();
				
				atari();
				
				trans_sp();
				
				bg_move();

				if(stTask.b496ms == TRUE)
				{
					S_Main_Score();
				}

				if(ship_data->life <= 0)	/* �Q�[���I�[�o�[ */
				{
					SetTaskInfo(SCENE_GAME_E);	/* �Q�[���V�[��(�I������)�^�X�N�֐ݒ� */
				}

				if(stage_clear() == 0)		/* �N���A */
				{
					SetTaskInfo(SCENE_GAME_E);	/* �Q�[���V�[��(�I������)�^�X�N�֐ݒ� */
				}

				break;
			}
			case SCENE_GAME_E:	/* �Q�[���V�[��(�I������) */
			{
				Music_Stop();
				T_Clear();			/* �e�L�X�g�N���A */

				if(ship_data->life <= 0)	/* �Q�[���I�[�o�[ */
				{
					SetTaskInfo(SCENE_GAME_OVER_S);		/* �Q�[���I�[�o�[�V�[��(�J�n����)�֐ݒ� */
				}
				else
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* NEXT�X�e�[�W�i�J�n�����j�֐ݒ� */
				}
				break;
			}
			case SCENE_NEXT_STAGE_S:	/* NEXT�X�e�[�W�i�J�n�����j */
			{
				int8_t	sBuf[128];
				
				S_Set_Combo(ship_data->life);	/* �R���{�J�E���^ �Z�b�g */
				S_GetPos(56, 96);	/* �X�R�A�\�����W�X�V */
				S_Add_Score(10000);	/* �X�R�A�X�V */
				S_Main_Score();		/* �X�R�A�X�V */
				S_Clear_Score();	/* �X�R�A�\�� */

				memset(sBuf, 0, sizeof(sBuf));
				strcpy(sBuf, "Push Trigger A button --> TITLE");
				Draw_Message_To_Graphic(sBuf, 24, 192, F_MOJI, F_MOJI_BAK);

				Music_Play(9);	/* ���̃X�e�[�W */

				SetTaskInfo(SCENE_NEXT_STAGE);	/* NEXT�X�e�[�W�i�����j�֐ݒ� */
				break;	
			}
			case SCENE_NEXT_STAGE:		/* NEXT�X�e�[�W�i�����j */
			{
				xsp_set( X_OFFSET + 192, X_OFFSET + 96, 0x29, SetXSPinfo(0, 0, 0x0B, 0x30) );	/* �X�R�A�{�[�� */

				if(g_Input == KEY_A)	/* A�{�^�� */
				{
					SetTaskInfo(SCENE_TITLE_S);	/* �^�C�g���V�[��(�J�n����)�֐ݒ� */
//					SetTaskInfo(SCENE_NEXT_STAGE_E);	/* NEXT�X�e�[�W�i�I�������j�֐ݒ� */
				}
				break;	
			}
			case SCENE_NEXT_STAGE_E:	/* NEXT�X�e�[�W�i�I�������j */
			{
				Music_Stop();
				
				S_Init_Score();			/* �X�R�A�N���A */
				
				Set_CRT_Contrast(0);	/* �R���g���X�g�� */

				SetTaskInfo(SCENE_START_S);	/* �Q�[���J�n�V�[��(�J�n����)�֐ݒ� */
				break;	
			}
			case SCENE_GAME_OVER_S:	/* �Q�[���I�[�o�[�V�[��(�J�n����) */
			{
				int8_t	sBuf[128];

//				PutGraphic_To_Symbol24("SCORE", 9, 1, F_MOJI_BAK);
//				PutGraphic_To_Symbol24("SCORE", 8, 0, F_MOJI);
//				memset(sBuf, 0, sizeof(sBuf));
//				sprintf(sBuf, "%d", );
//				Draw_Message_To_Graphic(sBuf, 96, 128, F_MOJI, F_MOJI_BAK);

				memset(sBuf, 0, sizeof(sBuf));
				strcpy(sBuf, "GAME OVER");
				Draw_Message_To_Graphic(sBuf, 96, 128, F_MOJI, F_MOJI_BAK);

				memset(sBuf, 0, sizeof(sBuf));
				strcpy(sBuf, "Push Trigger A button --> TITLE");
				Draw_Message_To_Graphic(sBuf, 32, 160, F_MOJI, F_MOJI_BAK);

				memset(sBuf, 0, sizeof(sBuf));
				strcpy(sBuf, "             B button --> END");
				Draw_Message_To_Graphic(sBuf, 32, 176, F_MOJI, F_MOJI_BAK);

				Music_Play(10);	/* GAME OVER */

				SetTaskInfo(SCENE_GAME_OVER);	/* �Q�[���I�[�o�[�V�[���^�X�N�֐ݒ� */
				break;
			}
			case SCENE_GAME_OVER:	/* �Q�[���I�[�o�[�V�[�� */
			{
				if(g_Input == KEY_A)	/* A�{�^�� */
				{
					SetTaskInfo(SCENE_TITLE_S);	/* �^�C�g���V�[��(�J�n����)�֐ݒ� */
				}
				if(g_Input == KEY_B)	/* B�{�^�� */
				{
					SetTaskInfo(SCENE_GAME_OVER_E);	/* �Q�[���I�[�o�[�V�[��(�I������)�֐ݒ� */
				}
				break;
			}
			case SCENE_GAME_OVER_E:	/* �Q�[���I�[�o�[�V�[��(�I������) */
			{
				Music_Stop();

//				SetTaskInfo(SCENE_HI_SCORE_S);	/* �n�C�X�R�A�����L���O�V�[��(�J�n����)�֐ݒ� */
				SetTaskInfo(SCENE_EXIT);	/* �I���V�[���֐ݒ� */
				break;
			}
			case SCENE_HI_SCORE_S:	/* �n�C�X�R�A�����L���O�V�[��(�J�n����) */
			case SCENE_HI_SCORE:	/* �n�C�X�R�A�����L���O�V�[�� */
			case SCENE_HI_SCORE_E:	/* �n�C�X�R�A�����L���O�V�[��(�I������) */
			{
				Music_Stop();
				
				SetTaskInfo(SCENE_TITLE_S);	/* �^�C�g���V�[��(�J�n����)�֐ݒ� */
				break;
			}
			case SCENE_OPTION_S:	/* �I�v�V�����V�[�� */
			case SCENE_OPTION:		/* �I�v�V�����V�[�� */
			case SCENE_OPTION_E:	/* �I�v�V�����V�[��(�I������) */
			{
				Music_Stop();

				break;
			}
			case SCENE_DEBUG_S:
			{
				puts("�f�o�b�O�R�[�i�[ �J�n");
				SetTaskInfo(SCENE_DEBUG);	/* �f�o�b�O(����)�֐ݒ� */
				break;
			}
			case SCENE_DEBUG:
			{
				break;
			}
			case SCENE_DEBUG_E:
			{
				Music_Stop();

				puts("�f�o�b�O�R�[�i�[ �I��");
				loop = 0;	/* ���[�v�I�� */
				break;
			}
			case SCENE_EXIT:		/* �I���V�[�� */
			{
				Music_Stop();	/* ���y�Đ� ��~ */

				loop = 0;	/* ���[�v�I�� */
				break;
			}
			default:	/* �ُ�V�[�� */
			{
				loop = 0;	/* ���[�v�I�� */
				break;
			}
		}
		
		if(UpdateTaskInfo() != 0)
		{
//			printf("%d, %d, %d, %d, %d\n", stTask.b8ms, stTask.b16ms, stTask.b32ms, stTask.b96ms, stTask.b496ms);
		}

		/* �X�v���C�g���ꊇ�\������ */
		xsp_out();
	}
	while( loop );

//	puts("���[�v�I��");
	
	g_bExit = FALSE;

	App_exit();

	return 0;
}

#endif	/* BATTLEKATA_C */
