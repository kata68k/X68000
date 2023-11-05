#ifndef	APL_GRAPHIC_H
#define	APL_GRAPHIC_H

#include <usr_define.h>

/* 画面内登場数 */
enum
{
	Clear_G=0,
	BackGround_G,
	Object0_G,
	Object1_G,
	Object2_G,
	Object3_G,
	Object4_G,
	Object5_G,
	Object6_G,
	Object7_G,
	Enemy1_G,
	Enemy2_G,
	StartPoint_G,
	GoalPoint_G,
	MyCar_G,
	Debug_View_G,
	Flip_G,
	MAX_G
};

/* グラフィックパターン定義数 0-15 max16 */
#define DUMMY_CG		(0)		/* ダミー		(1) */
#define BG_CG			(1)		/* 背景			(1) */
#define COURSE_OBJ_CG	(2)		/* 障害物		(kind of 4) */
#define ENEMYCAR_CG		(6)		/* ライバル車	(kind of 8) */
#define START_PT_CG		(14)	/* スタート位置	(1) */
#define GOAL_PT_CG		(15)	/* ゴール位置	(1) */
#define PAL_MAX_CG		(16)	/* 最大16パターン登録 */
/* グラフィックテキストパターン定義数 max 3 */
#define MYCAR_CG		(16)	/* 自車			(kind of 3) */
/* フルグラフィック */
#define TITLE_CG		(19)	/* タイトル */
#define DEMO_CG			(20)	/* デモ */
	/* … */

/* 構造体定義 */

/* extern宣言 */
extern	int16_t APL_G_Main(void);

#endif	/* APL_GRAPHIC_H */
