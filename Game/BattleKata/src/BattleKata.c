#ifndef	BATTLEKATA_C
#define	BATTLEKATA_C

/* 自機を動かすプログラム*/

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>
#include <time.h>
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
#include "IF_PCG.h"
#include "IF_Text.h"
#include "IF_Task.h"
#include "IF_Memory.h"
#include "IF_MACS.h"
#include "IF_Math.h"
#include "APL_Score.h"

/* グローバル変数 */
SHIP *ship_data;	/* 自機のデータ */
ENEMY *enemy_data;	/* 敵のデータ */
SHOT *shot_data;	/* 自弾のデータ */
AREA *area_data;	/* 移動範囲データ */

int32_t g_nSuperchk;
int32_t g_nCrtmod;
uint8_t	g_bExit = TRUE;
uint32_t g_unFly_time;


int32_t g_nBaseTempo;
int32_t g_nDebugNum;

short *seq_tbl;

/* プロトタイプ宣言 */
void (*usr_abort)(void);	/* ユーザのアボート処理関数 */
void App_init( void );
static void App_exit(void);
void App_TimerProc( void );


void area_init( int32_t x, int32_t y )
{
	T_Clear();	/* テキスト消去 */

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

	T_FillCircle(	Mdiv256(area_data->x),	/* スプライトの座標が基準なのでオフセットが必要 */
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

	M_SetMusic(0);	/* 効果音再生の設定 */
}

void ship_move( void )
{
	AREA *area;
//	JOY_ANALOG_BUF	analog_data;
	int16_t	analog_data[5];

	area = area_data;

	if(g_bAnalogStickMode == TRUE)
	{
		GetAnalog_Info(&analog_data[0]);	/* アナログデータ読み込み */
#if 0
		{
			int8_t	sBuf[128];
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "(%x,%x,%x,%x)(%x)",
						analog_data[0], analog_data[1],
						analog_data[2], analog_data[3],
						analog_data[4] );
			Draw_Message_To_Graphic(sBuf, 0, 240, F_MOJI, F_MOJI_BAK);
//			KeyHitESC();	/* デバッグ用 */
		}
#endif

		if( analog_data[r_stk_lr_1] < 0x70 )	/* 左 */
		{
			ship_data->angle += (0x80 - analog_data[r_stk_lr_1]) >> 4;
			
			if(ship_data->angle >= 360)
			{
				ship_data->angle -= 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else if( analog_data[r_stk_lr_1] > 0x90 )	/* 右 */
		{
			ship_data->angle -= (analog_data[r_stk_lr_1] - 0x80) >> 4;
			
			if(ship_data->angle < 0)
			{
				ship_data->angle += 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else /* なし */
		{
			/* 何もしない */
		}

		if( analog_data[l_stk_ud_2] > 0x90 )	/* 上 */
		{
			ship_data->acc += (analog_data[l_stk_ud_2] - 0x80) >> 6;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else if( analog_data[l_stk_ud_2] < 0x70 )	/* 下 */
		{
			ship_data->acc += (0x80 - analog_data[l_stk_ud_2]) >> 6;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else /* なし */
		{
			ship_data->acc--;
			if(ship_data->acc <= 0)ship_data->acc = 0;
		}
	}
	else
	{
		if( (g_Input & KEY_LEFT) != 0 )	/* 左 */
		{
			ship_data->angle += 4;
			
			if(ship_data->angle >= 360)
			{
				ship_data->angle -= 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else if( (g_Input & KEY_RIGHT) != 0 )	/* 右 */
		{
			ship_data->angle -= 4;
			
			if(ship_data->angle < 0)
			{
				ship_data->angle += 360;
			}
	//		printf("A=%3d\n", ship_data->angle);
		}
		else /* なし */
		{
			/* 何もしない */
		}

		if( ( g_Input & KEY_UPPER ) != 0 )  /* 上 */
		{
			ship_data->acc += 1;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else if( ( g_Input & KEY_LOWER ) != 0 )  /* 下 */
		{
			ship_data->acc += 1;
			if(ship_data->acc >= 3)ship_data->acc = 3;
		}
		else /* なし */
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
	else /* なし */
	{
		 /* 何もしない */
	}
#endif

	if( ( g_Input & KEY_UPPER ) != 0) { /* 上 */
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
	else if( ( g_Input & KEY_LOWER ) != 0 ) { /* 下 */
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
	else /* なし */
	{
		 /* 何もしない */
	}
	
	/* 移動範囲 */
	if(area->flag == TP_ALIVE)	/* 制限中 */
	{
		if(ship_data->x <  area->x - Mdiv2(area->w))	ship_data->x = Mmax( VX_MIN,	area->x - Mdiv2(area->w) );
		if(ship_data->x >= area->x + Mdiv2(area->w))	ship_data->x = Mmin( VX_MAX,	area->x + Mdiv2(area->w) );
		if(ship_data->y <  area->y - Mdiv2(area->h))	ship_data->y = Mmax( VY_MIN,	area->y - Mdiv2(area->h) );
		if(ship_data->y >= area->y + Mdiv2(area->h))	ship_data->y = Mmin( VY_MAX,	area->y + Mdiv2(area->h) );
	}
	else	/* 制限なし */
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
	else /* なし */
	{
		 /* 何もしない */
	}

	/*自機を動かす*/
	PCG_PUT_1x1( X_OFFSET + x - 8, Y_OFFSET + y - 8, pat, SetXSPinfo(v, h, 0x0A, 0x20));
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
	static int32_t s_shot_angle;
	SHOT *shot;
	
	/* 弾を作ります */
	if( ((g_Input & KEY_A) != 0u) && (area_data->flag == TP_ALIVE)){ /* Ａボタン */
		
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
				s_shot_angle = ship_data->angle;
			}
			else if(shot->flag == SHOT_ST_MOVE)
			{
				shot->acc = 0;
				shot->flag = SHOT_ST_DOWN;
			}
		}
	}

	/* 弾を動かします */
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
				int32_t clp_x, clp_y;
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

				clp_x =  Mdiv64( SHOT_VX * APL_Cos(s_shot_angle) );

				if(Mabs(shot->vx) > Mabs(clp_x)){
					if(shot->vx > 0){
						shot->vx -= Mmax(Mabs(clp_x), 1);
					}
					else if(shot->vx < 0){
						shot->vx += Mmax(Mabs(clp_x), 1);
					}
					else{
					}
				}
				else
				{
					shot->vx = 0;
				}

				clp_y =  Mdiv64( SHOT_VY * APL_Sin(s_shot_angle) );

				if(Mabs(shot->vy) > Mabs(clp_y)){
					if(shot->vy > 0){
						shot->vy -= Mmax(Mabs(clp_y), 1);
					}
					else if(shot->vy < 0){
						shot->vy += Mmax(Mabs(clp_y), 1);
					}
					else{
					}
				}
				else{
					shot->vy = 0;
				}

				if( (Mabs(shot->vx) <= Mabs(clp_x)) && (Mabs(shot->vy) <= Mabs(clp_y)) )	/* 転がり停止 */
				{
					shot->flag = SHOT_ST_OUT;
					ship_data->life--;
					if(ship_data->life == 1){
						M_TEMPO(180);	/* 高速テンポ */
					}
					ADPCM_Play(3);
					S_Set_Combo(0);	/* コンボカウンタ リセット */
				}
			}			
		}		
#ifdef DEBUG
		if(shot->pat >= 4)
		{				
			printf("shot_se=%x,%x,%x\n",  shot->pat, shot->y, shot->ey);
			KeyHitESC();	/* デバッグ用 */
		}
#endif

		/* 画面外判定 */
		if((( (shot->x <= VX_MIN) && (shot->ex <= VX_MIN) )
		||  ( (shot->y <= VY_MIN) && (shot->ey <= VY_MIN) )
		||  ( (shot->x >= VX_MAX) && (shot->ex >= VX_MAX) )
		||  ( (shot->y >= VY_MAX) && (shot->ey >= VY_MAX) ) )
		&& (shot->flag != SHOT_ST_MOVE) )
		{
			shot->flag = SHOT_ST_OUT;
			ship_data->life--;	
			if(ship_data->life == 1){
				M_TEMPO(180);	/* 高速テンポ */
			}
			ADPCM_Play(3);
			S_Set_Combo(0);	/* コンボカウンタ リセット */
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

		/* ＡボタンのＳＥ */
		if( (g_Input & KEY_A) != 0u ){ /* Ａボタン */

			if( key_f_A == 0 )
			{
				key_f_A = 1;

				i = 0;

				g_unFly_time = 0xFFFF;

				if(area_data->flag == TP_ALIVE)
				{
					ADPCM_Play(1);

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

		/* ＢボタンのＳＥ */
		if( ((g_Input & KEY_B) != 0u) && (key_f_B == 0)){ 

			key_f_B = 1;
			
			if(area_data->flag == TP_ALIVE)
			{
				ADPCM_Play(2);
			}
		}
		else
		{
			key_f_B = 0;
		}

		switch(shot->flag)
		{
			case SHOT_ST_MOVE:
			{
				break;
			}
			case SHOT_ST_DOWN:
			{
				break;
			}
			case SHOT_ST_TOUCH:
			{
//				M_Play(1, (i++) % 128);
				break;
			}
			case SHOT_ST_RUN:
			{
//				M_Play(1, (i++) % 128);
				break;
			}
			default:
				break;
		}
	}
}

void enemy_init( void )
{
	ENEMY *enemy;
	short i;
	/* 敵の初期化 */
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
	
	_iocs_apage(1);		/* グラフィックの書き込み(1) */

	/* 敵の初期化 */
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
		
		/* 移動ルーチン */
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
		int32_t dx, dy;
		int64_t distX, distY;
		
		if( enemy->flag == ENEMY_DEAD ) continue;

		if( ship_data->flag != TP_DEAD ) {
			
			dx = Mabs(enemy->x - ship_data->x);
			dy = Mabs(enemy->y - ship_data->y);
			
			if( (dx < ATARI_YOKO) && (dy < ATARI_TATE) ) {
				if( (enemy->flag != ENEMY_DEAD)  && (area->flag == TP_ALIVE) )
				{
					enemy->flag = ENEMY_DEAD;
					area_init(enemy->x, enemy->y);	/* 制限を更新 */

					/* スコア */
					S_GetPos(Mdiv256(enemy->x) - 16, Mdiv256(enemy->y) - 36);	/* スコア表示座標更新 */
					point = (i+1) * 100;
					S_Add_Score_Point(point);	/* スコア更新 */

					ship_data->life++;	/* ショット補充 */
					if(ship_data->life > 1)	/* 残り１機よりおおきい */
					{
						M_TEMPO(g_nBaseTempo);	/* 通常のテンポ */
					}

					ADPCM_Play(4);
				}
				else if( (enemy->flag == ENEMY_BOM) && (area->flag == TP_DEAD) )
				{
					enemy->flag = ENEMY_DEAD;
					area_init(enemy->x, enemy->y);	/* 制限を更新 */
					
					/* スコア */
					S_GetPos(Mdiv256(enemy->x) - 16, Mdiv256(enemy->y) - 36);	/* スコア表示座標更新 */
					point = 100 * (65 - enemy->delay);
					S_Add_Score_Point(point);	/* スコア更新 */
					
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

			dx = shot->x - enemy->x;
			distX = Mdiv65536((uint64_t)(dx * dx));
			
			dy = shot->y - enemy->y;
			distY = Mdiv65536((uint64_t)(dy * dy));
			
			if( ((distX + distY) >= 0) && ((distX + distY) < Mdiv65536(ATARI_XY) )) {
				enemy->flag = ENEMY_BOM;

//				area_init(enemy->x, enemy->y);	/* 制限を更新 */
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
					/* スコア */
					point = (i+1) * 1000;
#if CNF_MACS
					PCG_OFF();			/* SP OFF */
					/* 動画 */
					MOV_Play(0);		/* カットイン */
					/* 画面モード再設定 */
					CRTC_INIT(0x10A);	/* 画面モード初期化 */
					PCG_ON();			/* SP ON */
#else
#endif
					/* 効果音 */
					ADPCM_Play(7);
				}
				else
				{
					/* スコア */
					point = (i+1) * 100;
					/* 効果音 */
					ADPCM_Play(5);
				}
				S_GetPos(Mdiv256(enemy->x) - 16, Mdiv256(enemy->y) - 48);	/* スコア表示座標更新 */
				S_Add_Score(point);	/* スコア更新 */
#if 0
				{
					int8_t	sBuf[128];
					memset(sBuf, 0, sizeof(sBuf));
					sprintf(sBuf, "s(%d, %d)e(%d, %d)d(%x, %x)", shot->x, shot->y, enemy->x, enemy->y, distX, distY);
					Draw_Message_To_Graphic(sBuf, 0, 240, F_MOJI, F_MOJI_BAK);
					KeyHitESC();	/* デバッグ用 */
				}
#endif

				shot->flag = SHOT_ST_OUT;
#ifdef DEBUG
//				if(i==0)
//				{				
//					printf("atari=(%x,%x)(%x,%x)(%x,%x)\n",  shot->x, shot->y, enemy->x, enemy->y, dx, dy);
//					KeyHitESC();	/* デバッグ用 */
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
	
	/* 自機の表示 */
	ship_sp();
	
	/*自弾の表示*/
	for( i = 0, shot = shot_data; i < SHOT_NUM; i++, shot++ ) {
		switch( shot->flag )
		{
			case SHOT_ST_OUT:
				PCG_PUT_1x1( 0, 0, 0x25 + (shot->pat), SetXSPinfo(0, 0, 0x0B, 0x20) );
				break;
			default:
				PCG_PUT_1x1( X_OFFSET + Mdiv256(shot->x) - 8, 		Y_OFFSET + Mdiv256(shot->y) - 8, 		0x25 , 				SetXSPinfo(0, 0, 0x0B, 0x20) );	/* 影 */
				PCG_PUT_1x1( X_OFFSET + Mdiv256(shot->ex) + 2 - 8, 	Y_OFFSET + Mdiv256(shot->ey) - 2 - 8,	0x26 + (shot->pat), SetXSPinfo(0, 0, 0x0B, 0x30) );	/* ボール */
				break;
		}
//		Draw_Pset(Mdiv256(shot->x), Mdiv256(shot->y), 1);
	}
	
	/* 敵の表示 */
	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i++, enemy++ ) {
		switch( enemy->flag )
		{
			case ENEMY_DEAD:
				PCG_PUT_1x1( 0, 0, 0x30 + (i % 18), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20) );	/* 画面外 */
				break;
			case ENEMY_BOM:
				j = enemy->delay;
				if(j < 64)
				{
					_iocs_apage(1);		/* グラフィックの書き込み(1) */
					ret = Draw_Circle(	Mdiv256(enemy->x),
										Mdiv256(enemy->y) + 8,
										j>>1, j, 0, 360, 64);
					if(ret < 0)printf("error(%d):Draw_Circle\n", ret);

					/* 爆風 */
					PCG_PUT_1x1( X_OFFSET + Mdiv256(enemy->x) - 16, Y_OFFSET + Mdiv256(enemy->y) - 24, 0x50 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					PCG_PUT_1x1( X_OFFSET + Mdiv256(enemy->x) + 0, Y_OFFSET + Mdiv256(enemy->y) - 24, 0x51 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					PCG_PUT_1x1( X_OFFSET + Mdiv256(enemy->x) - 16, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x52 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					PCG_PUT_1x1( X_OFFSET + Mdiv256(enemy->x) + 0, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x53 + Mmul4(j/16), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));
					enemy->delay++;
				}
				else
				{
					PCG_PUT_1x1( X_OFFSET + Mdiv256(enemy->x) - 8, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x49, SetXSPinfo(0, (enemy->vx < 0), 0x01, 0x20));	/* エネルギー缶 */
				}
//				ret = _iocs_apage(0);		/* グラフィックの書き込み(0) */
//				if(ret < 0)printf("error(%d):_iocs_apage\n", ret);
//				ret = _iocs_vpage(0b0011);	/* グラフィック表示(page3:0n page2:0n page1:0n page0:0n) */
//				if(ret < 0)printf("error(%d):_iocs_vpage\n", ret);
//				ret = Draw_Fill( Mdiv256(enemy->x) - 32 - X_OFFSET/2,
//								 Mdiv256(enemy->y) - 32 - Y_OFFSET/2,
//								 Mdiv256(enemy->x) + 32 - X_OFFSET/2,
//								 Mdiv256(enemy->y) + 32 - Y_OFFSET/2, 0xFF);
//				if(ret < 0)printf("error(%d):Draw_Box\n", ret);
				break;
			default:
				PCG_PUT_1x1( X_OFFSET + Mdiv256(enemy->x) - 8, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x42, SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x20));	/* 旗 */
				PCG_PUT_1x1( X_OFFSET + Mdiv256(enemy->x) - 8, Y_OFFSET + Mdiv256(enemy->y) - 8, 0x30 + (i % 18), SetXSPinfo(0, (enemy->vx < 0), 0x06, 0x30));	/* 番号 */
				break;
		}	
//		Draw_Circle(	Mdiv256(enemy->x),
//						Mdiv256(enemy->y),
//						8, i+1, 0, 360, 256);
	}
	/* 残機表示 */
	if(ship_data->life > 4)
	{
		ST_TASK stTask;
		int8_t	sBuf[8];

		GetTaskInfo(&stTask);	/* タスク取得 */
		if(stTask.b496ms == TRUE)
		{
			memset(sBuf, 0, sizeof(sBuf));
			sprintf(sBuf, "x%d", ship_data->life);
			Draw_Message_To_Graphic(sBuf, 240, 0, F_MOJI, F_MOJI_BAK);
		}

		PCG_PUT_1x1( X_OFFSET + 224, Y_OFFSET, 0x29, SetXSPinfo(0, 0, 0x0B, 0x30) );
	}
	else
	{
		for(i = 0; i < ship_data->life; i++)
		{
			PCG_PUT_1x1( X_OFFSET + 240 - (i * 16), Y_OFFSET, 0x29, SetXSPinfo(0, 0, 0x0B, 0x30) );
		}
	}
}

void bg_pest( void )
{
	int i;
	/* BG0 > SP > BG1 */
	BGCTRLST(0,0,1);
	BGCTRLST(1,1,1);
	BGTEXTCL(0, SetBGcode(0,0,0x02, (((0x06 * 16) + 0x03) * 4) + 3) );	/* 空白 */
	BGTEXTCL(1, SetBGcode(0,0,0x0A, (((0x05 * 16) + 0x0C) * 4) + 0) );	/* 空 */
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

	GetFreeMem();		/* 起動前のメモリ状態 */

	srand(time(NULL));	/* 乱数初期化 */

	/* 例外ハンドリング処理 */
	g_bExit = TRUE;
	usr_abort = App_exit;	/* 例外発生で終了処理を実施する */
	init_trap14();			/* デバッグ用致命的エラーハンドリング */

	TimerD_INIT();			/* Timer-D初期化 */

	//puts("初期化開始");
	Init_FileList_Load();	/* リストファイルの読み込み */

	puts("Music ...");
	/* 音楽 */
	Init_Music();	/* 初期化(スーパーバイザーモードより前)	*/

	Music_Play(5);	/* ローディング中 */

	puts("Super ...");
	/* スーパーバイザーモードへの切り替えは、音楽の初期化後にやること */
	g_nSuperchk = _dos_super(0);	/* スーパーバイザーモード開始 */
	if( g_nSuperchk < 0 ) {
		puts("すでに入ってる。");
		KeyHitESC();	/* デバッグ用 */
	} else {
		puts("入った。");
	}
	
#if CNF_MACS
	puts("Movie ...");
	/* 動画 */
	MOV_INIT();		/* 初期化処理 */
#endif

	puts("CRT ...");
	/* 画面 */
	g_nCrtmod = CRTC_INIT(0x100 + 0x0A);	/* 画面の初期設定 256x256 G256 */
	
	puts("Graphic ...");
	/* 静止画 */
	G_INIT();		/* グラフィック初期化 */
	
	puts("Text ...");
	T_INIT();		/* テキスト初期化 */
	
	puts("PCG ...");
	PCG_INIT();		/* スプライト初期化 */

	puts("Controller ...");	
#if CNF_JOYDRV360
	/* アナログスティックモード判定 */
	if(SetJoyDevMode(0, 0, 0) < 0)	/* JoyNo:0 DevID:0 PortNo:0 */
	{
		puts("JOYDRV3.Xが常駐してません。");
	}
#if 0	
	switch(GetJoyDevice(1, 1, 0))
	{
		case 0:
		default:
			puts("App_init Digital");	
			SetJoyDevMode(0, 0, 0);	/* JoyNo:0 DevID:0 PortNo:0 */
			break;
		case 1:
			puts("App_init Analog");
			SetJoyDevMode(1, 1, 0);	/* JoyNo:1 DevID:1 PortNo:0 */
			break;
	}
#endif

#else
	g_bAnalogStickMode = FALSE;	/* デジタル */
#endif
	/* タスク初期 */
	TaskManage_Init();

	/* スコア初期化 */
	S_All_Init_Score();

	/* ゲーム全般の初期化 */
	game_init();

	Set_CRT_Contrast(0);	/* コントラスト暗 */
//	puts("App_init end");
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void App_exit(void)
{
	puts("App_exit start");

	if(g_bExit == TRUE)
	{
		puts("エラーをキャッチ！ ESC to skip");
		KeyHitESC();	/* デバッグ用 */
		exit(0);
	}
	g_bExit = TRUE;
	
	TaskManage_Init();		/* TimerTask 初期化 */
	TimerD_EXIT();			/* Timer-D 終了 */

	PCG_OFF();				/* スプライトOFF */

	/* 画面 */
	CRTC_INIT(16);			/* モードをもとに戻す */
	puts("App_exit CRTC_INIT");

	/* 音楽 */
	Exit_Music();			/* 音楽停止 */
	puts("App_exit Exit_Music");

	/* MFP */
//	MFP_EXIT();				/* MFP関連の解除 */
//	puts("App_exit MFP_EXIT");

	/* テキスト */
	T_EXIT();				/* テキスト終了処理 */
	puts("App_exit T_EXIT");

	MyMfree(0);				/* 全てのメモリを解放 */
	puts("App_exit MyMfree");

	_dos_kflushio(0xFF);	/* キーバッファをクリア */
	puts("App_exit _dos_kflushio");

	_dos_super(g_nSuperchk);		/*スーパーバイザーモード終了*/
	puts("App_exit _dos_super");

	printf("メモリ最大空き容量(%dkB)\n", GetMaxFreeMem());

	puts("App_exit End");
}

void App_TimerProc( void )
{
	TaskManage();
	/* ↓↓↓ この間に処理を書く ↓↓↓ */


	/* ↑↑↑ この間に処理を書く ↑↑↑ */
}

int32_t main(void)
{
	int16_t	loop = 1;

	App_init();	/* 初期設定 */

	do{
		uint32_t time_st;
		ST_TASK stTask;

		PCG_START_SYNC();	/* スプライト開始処理 */

		/* 時刻設定 */
		GetNowTime(&time_st);	/* メイン処理の開始時刻を取得 */
		SetStartTime(time_st);	/* メイン処理の開始時刻を記憶 */

		/* タスク */
		TaskManage();
		GetTaskInfo(&stTask);	/* タスク取得 */

		/* 入力処理 */
		{
			int16_t	input = 0;
			/* アナログスティック／デジタルスティック切替 */
			if(ChatCancelSW((g_Input & KEY_b_TAB)!=0u, &g_bAnalogStickMode_flag) == TRUE)	/* TABでアナログスティックON/OFF */
			{
#if CNF_JOYDRV360				
				if(g_bAnalogStickMode == TRUE)	/* 現在：アナログスティックモードの場合 */
				{
					g_bAnalogStickMode = FALSE;
//					puts("A to D");
					SetJoyDevMode(0, 0, 0);	/* JoyNo:0 DevID:0 PortNo:0 */
					if(GetJoyDevMode(0) == 0)
					{
//						puts("A to D OK");
					}
				}
				else
				{
					g_bAnalogStickMode = TRUE;
//					puts("D to A");
					SetJoyDevMode(1, 1, 0);	/* JoyNo:1 DevID:1 PortNo:0 */
					if(GetJoyDevMode(1) == 0)
					{
//						puts("D to A OK");
					}
				}
#else
				if(g_bAnalogStickMode == TRUE)
				{
					g_bAnalogStickMode = FALSE;
				}
				else
				{
					g_bAnalogStickMode = TRUE;
				}
#endif
			}
			get_keyboard(&input, 0, 0);		/* キーボード入力 */

			if(g_bAnalogStickMode == TRUE)
			{
				get_ajoy(&input, 1, 0, 0);	/* アナログジョイスティック入力, JoyNo, mode=0:edge on 1:edge off, Config 0:X680x0 1:rev */
				
			}
			else
			{
				get_djoy(&input, 0, 0);		/* ジョイスティック入力, JoyNo, mode=0:edge on 1:edge off */
			}
			g_Input = input;
		}
		
		/* ＥＳＣポーズ */
		if( ( BITSNS( 0 ) & 0x02 ) != 0 ) {
			puts("ESC");
			SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
			break;
		}

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* 初期化シーン */
			{
				SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_TITLE_S:	/* タイトルシーン(開始処理) */
			{
				/* グラフィック表示 */
				G_HOME(0);			/* ホーム位置設定 */
				G_CLR();			/* グラフィッククリア */
				G_PaletteSetZero();	/* グラフィックパレットの基本設定 */
				T_Clear();			/* テキストクリア */
	
				G_CLR_AREA(0,0,256,256,1);	/* グラフィック領域クリア */
				G_Load(2, 0, 0, 1);	/* グラフィックの読み込み */	/* 画像を表示させると終了時におかしな命令エラーになる */
				
				S_Init_Score();		/* スコア初期化 */
				PutGraphic_To_Symbol24("Ball und Panzer Golf", 9, 1, 0x12);
				PutGraphic_To_Symbol24("Ball und Panzer Golf", 8, 0, 0x86);
				PutGraphic_To_Symbol12("2023 Ver 0.95", 144, 24, 0x86);
				PutGraphic_To_Symbol12("For X680x0", 144, 36, 0x86);
//				PutGraphic_To_Symbol12("Ver X68KBBS", 150, 24, 0x86);
				Draw_Message_To_Graphic("START", 160, 116, F_MOJI, 0x30);
				Draw_Message_To_Graphic("PUSH A BUTTON", 160, 128, F_MOJI, 0x30);

				Music_Play(7);	/* デモ画面 */
//				Music_Play(4);	/* タイトル画面 */

				Set_CRT_Contrast(-1);	/* コントラストdef */

				SetTaskInfo(SCENE_TITLE);	/* タイトルシーンへ設定 */
				break;
			}
			case SCENE_TITLE:	/* タイトルシーン */
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

				if(g_Input == KEY_A)	/* Aボタン */
				{
					SetTaskInfo(SCENE_TITLE_E);	/* タイトルシーン(開始処理)へ設定 */
				}
				break;
			}
			case SCENE_TITLE_E:	/* タイトルシーン(終了処理) */
			{
				G_HOME(0);			/* ホーム位置設定 */
				G_CLR();			/* グラフィッククリア */
				G_PaletteSetZero();	/* グラフィックパレットの基本設定 */

				SetTaskInfo(SCENE_START_S);	/* ゲームスタート(開始処理)へ設定 */
				break;
			}
			case SCENE_DEMO_S:	/* デモシーン(開始処理) */
			{
				Music_Play(6);	/* デモ画面 */

				SetTaskInfo(SCENE_DEMO);	/* デモシーンへ設定 */
				break;
			}
			case SCENE_DEMO:	/* デモシーン */
			{
				SetTaskInfo(SCENE_DEMO_E);	/* デモシーン(終了処理)へ設定 */
				break;
			}
			case SCENE_DEMO_E:	/* デモシーン(終了処理) */
			{
				Music_Stop();	/* 音楽再生 停止 */

				SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_START_S:	/* ゲーム開始シーン(開始処理) */
			{
				Set_CRT_Contrast(0);	/* コントラスト暗 */

//				Music_Play(7);	/* デモ画面 */

				/* グラフィック表示 */
				G_HOME(0);			/* ホーム位置設定 */
				G_CLR();			/* グラフィッククリア */
				G_PaletteSetZero();	/* グラフィックパレットの基本設定 */
				T_Clear();			/* テキストクリア */

				SetTaskInfo(SCENE_START);	/* ゲーム開始シーンへ設定 */
				break;
			}
			case SCENE_START:	/* ゲーム開始シーン */
			{
				SetTaskInfo(SCENE_START_E);	/* ゲーム開始シーン(終了処理)へ設定 */
				break;
			}
			case SCENE_START_E:	/* ゲーム開始シーン(終了処理) */
			{

				SetTaskInfo(SCENE_GAME_S);	/* ゲームシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_GAME_S:	/* ゲームシーン(開始処理) */
			{
				/*自機の初期設定*/
				ship_init();

				/*自弾の初期設定*/
				shot_init();

				/* 敵の初期設定 */
				enemy_init();

				/* 行動制限エリアの初期設定 */
				area_init(-1, -1);

				G_CLR_AREA(0,0,256,256,1);	/* グラフィック領域クリア */
				G_Load(1, 0, 0, 1);	/* グラフィックの読み込み */	/* 画像を表示させると終了時におかしな命令エラーになる */
				
				enemy_set(18);			/* 敵のステータス（生きる） */
				enemy_green_set();		/* グリーンを生成 */
				
				Set_CRT_Contrast(-1);	/* コントラストdef */

				ADPCM_Play(6);

//				Music_Play(4);	/* ゲーム中 */
				Music_Play(8);	/* ゲーム中 */
				g_nBaseTempo = M_TEMPO(-1);	/* 現在のテンポを取得 */
//				printf("tempo=%d\n",  g_nBaseTempo);
//				KeyHitESC();	/* デバッグ用 */
				Music_Play(8);	/* ゲーム中 */

				SetTaskInfo(SCENE_GAME);	/* ゲームシーンへ設定 */
				break;
			}
			case SCENE_GAME:	/* ゲームシーン */
			{
				/* キャラクタを操作します。*/
				ship_move();

				shot_move();
				shot_se();

				enemy_move();
				
				atari();
				
				trans_sp();
				
//				bg_move();

				if(stTask.b496ms == TRUE)
				{
					S_Main_Score();
				}

				if(ship_data->life <= 0)	/* ゲームオーバー */
				{
					SetTaskInfo(SCENE_GAME_E);	/* ゲームシーン(終了処理)タスクへ設定 */
				}

				if(stage_clear() == 0)		/* クリア */
				{
					SetTaskInfo(SCENE_GAME_E);	/* ゲームシーン(終了処理)タスクへ設定 */
				}

				break;
			}
			case SCENE_GAME_E:	/* ゲームシーン(終了処理) */
			{
				M_TEMPO(g_nBaseTempo);	/* 通常のテンポ */
				T_Clear();			/* テキストクリア */

				if(ship_data->life <= 0)	/* ゲームオーバー */
				{
					SetTaskInfo(SCENE_GAME_OVER_S);		/* ゲームオーバーシーン(開始処理)へ設定 */
				}
				else
				{
					SetTaskInfo(SCENE_NEXT_STAGE_S);	/* NEXTステージ（開始処理）へ設定 */
				}
				break;
			}
			case SCENE_NEXT_STAGE_S:	/* NEXTステージ（開始処理） */
			{
				int8_t	sBuf[128];
				
				S_Set_Combo(ship_data->life);	/* コンボカウンタ セット */
				S_GetPos(56, 96);	/* スコア表示座標更新 */
				S_Add_Score(10000);	/* スコア更新 */
				S_Main_Score();		/* スコア更新 */
				S_Clear_Score();	/* スコア表示 */

				memset(sBuf, 0, sizeof(sBuf));
				strcpy(sBuf, "Push Trigger A button --> TITLE");
				Draw_Message_To_Graphic(sBuf, 24, 192, F_MOJI, F_MOJI_BAK);

				Music_Play(9);	/* 次のステージ */

				SetTaskInfo(SCENE_NEXT_STAGE);	/* NEXTステージ（処理）へ設定 */
				break;	
			}
			case SCENE_NEXT_STAGE:		/* NEXTステージ（処理） */
			{
				PCG_PUT_1x1( X_OFFSET + 192, X_OFFSET + 96, 0x29, SetXSPinfo(0, 0, 0x0B, 0x30) );	/* スコアボール */

				if(g_Input == KEY_A)	/* Aボタン */
				{
					SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
//					SetTaskInfo(SCENE_NEXT_STAGE_E);	/* NEXTステージ（終了処理）へ設定 */
				}
				break;	
			}
			case SCENE_NEXT_STAGE_E:	/* NEXTステージ（終了処理） */
			{
				Music_Stop();
				
				S_Init_Score();			/* スコアクリア */
				
				Set_CRT_Contrast(0);	/* コントラスト暗 */

				SetTaskInfo(SCENE_START_S);	/* ゲーム開始シーン(開始処理)へ設定 */
				break;	
			}
			case SCENE_GAME_OVER_S:	/* ゲームオーバーシーン(開始処理) */
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
				
				SetTaskInfo(SCENE_GAME_OVER);	/* ゲームオーバーシーンタスクへ設定 */
				break;
			}
			case SCENE_GAME_OVER:	/* ゲームオーバーシーン */
			{
				if(g_Input == KEY_A)	/* Aボタン */
				{
					SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
				}
				if(g_Input == KEY_B)	/* Bボタン */
				{
					SetTaskInfo(SCENE_GAME_OVER_E);	/* ゲームオーバーシーン(終了処理)へ設定 */
				}
				break;
			}
			case SCENE_GAME_OVER_E:	/* ゲームオーバーシーン(終了処理) */
			{
				Music_Stop();

//				SetTaskInfo(SCENE_HI_SCORE_S);	/* ハイスコアランキングシーン(開始処理)へ設定 */
				SetTaskInfo(SCENE_EXIT);	/* 終了シーンへ設定 */
				break;
			}
			case SCENE_HI_SCORE_S:	/* ハイスコアランキングシーン(開始処理) */
			case SCENE_HI_SCORE:	/* ハイスコアランキングシーン */
			case SCENE_HI_SCORE_E:	/* ハイスコアランキングシーン(終了処理) */
			{
				Music_Stop();
				
				SetTaskInfo(SCENE_TITLE_S);	/* タイトルシーン(開始処理)へ設定 */
				break;
			}
			case SCENE_OPTION_S:	/* オプションシーン */
			case SCENE_OPTION:		/* オプションシーン */
			case SCENE_OPTION_E:	/* オプションシーン(終了処理) */
			{
				Music_Stop();

				break;
			}
			case SCENE_DEBUG_S:
			{
				puts("デバッグコーナー 開始");
				SetTaskInfo(SCENE_DEBUG);	/* デバッグ(処理)へ設定 */
				break;
			}
			case SCENE_DEBUG:
			{
				break;
			}
			case SCENE_DEBUG_E:
			{
				Music_Stop();

				puts("デバッグコーナー 終了");
				loop = 0;	/* ループ終了 */
				break;
			}
			case SCENE_EXIT:		/* 終了シーン */
			{
				Music_Stop();	/* 音楽再生 停止 */

				loop = 0;	/* ループ終了 */
				break;
			}
			default:	/* 異常シーン */
			{
				loop = 0;	/* ループ終了 */
				break;
			}
		}
		
		GetFreeMem();	/* メモリ使用量測定 */

		if(UpdateTaskInfo() != 0)
		{
//			printf("%d, %d, %d, %d, %d\n", stTask.b8ms, stTask.b16ms, stTask.b32ms, stTask.b96ms, stTask.b496ms);
		}
		PCG_END_SYNC();
	}
	while( loop );

//	puts("ループ終了");
	
	g_bExit = FALSE;

	App_exit();

	return 0;
}

#endif	/* BATTLEKATA_C */
