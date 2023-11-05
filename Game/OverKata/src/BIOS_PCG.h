#ifndef	BIOS_PCG_H
#define	BIOS_PCG_H

#include <usr_define.h>

/* スプライトＰＣＧパターン最大数 */
#define	PCG_MAX		(128)
#define	BG_WIDTH	(8)
#define	BG_HEIGHT	(8)
#define BG_0_W		(280)
#define BG_0_H		(63)
#define BG_1_W		(280)
#define BG_1_H		(63)
#define SP_W		(16)
#define SP_H		(16)
#define SP_X_OFFSET	(16)
#define SP_Y_OFFSET	(16)
#define SP_8_SIZE	(32)
#define SP_16_SIZE	(128)
#define SP_8x8		(0)
#define SP_16x16	(1)
#define	SP_PLN_MAX	(128)
#define	PCG_CODE_MASK	(0xFF)

enum{
	MYCAR_PCG_NEEDLE,			/* タコメーター針 */
	MYCAR_PCG_TACHO,			/* タコメーター */
	MYCAR_PCG_STEERING_POS,		/* ステアリングポジション */
	MYCAR_PCG_STEERING,			/* ステアリング */
	SCORE_PCG_1,				/* スコア１ */
	SCORE_PCG_2,				/* スコア２ */
	SCORE_PCG_3,				/* スコア３ */
	SCORE_PCG_4,				/* スコア４ */
	ROAD_PCG_ARROW,				/* 矢印 */
	ROAD_PCG_SIGNAL_1,			/* シグナルランプ */
	ROAD_PCG_SIGNAL_2,			/* シグナルランプ */
	ROAD_PCG_SIGNAL_3,			/* シグナルランプ */
	OBJ_SHADOW,					/* 影1*3 */
	ETC_PCG_SONIC,				/* ソニック */
	ETC_PCG_KISARA,				/* キサラ */
	ETC_PCG_MAN,				/* ピクト君 */
	PCG_NUM_MAX,
};

typedef struct
{
	uint8_t		Plane;			/* プレーン番号の先頭 */
	uint16_t	x;				/* x座標 */
	uint16_t	y;				/* y座標 */
	int16_t		dx;				/* 移動量x */
	int16_t		dy;				/* 移動量y */
	uint8_t		Anime;			/* 現在のアニメーション */
	uint8_t		Anime_old;		/* 前回のアニメーション */
	uint8_t		update;			/* 更新 */
	uint8_t		validty;		/* 有効 */
	uint8_t		Pat_w;			/* 横方向のパターン数 */
	uint8_t		Pat_h;			/* 縦方向のパターン数 */
	uint8_t		Pat_AnimeMax;	/* アニメーションパターン数 */
	uint8_t		Pat_DataMax;	/* パターンデータ数 */
	uint16_t	*pPatData;		/* パターンデータ */
	uint32_t	*pPatCodeTbl;	/* パターンコードテーブル */
}	ST_PCG;

extern ST_PCG	g_stPCG_DATA[PCG_MAX];

extern void PCG_VIEW(uint8_t);
extern void PCG_Rotation(uint16_t *, uint16_t *, uint8_t, uint8_t, int16_t, int16_t, uint8_t *, uint8_t, uint16_t, uint16_t);
extern void BG_TEXT_SET(int8_t *);
extern int16_t PCG_Main(void);
extern ST_PCG *PCG_Get_Info(uint8_t);
extern int16_t PCG_Set_Info(ST_PCG, uint8_t);
extern int16_t PCG_Load_Data(int8_t *, uint16_t , ST_PCG , uint16_t , uint8_t );
extern int16_t PCG_Load_PAL_Data(int8_t *, uint16_t, uint16_t);
extern int16_t PCG_PAL_Change(uint16_t , uint16_t , uint16_t);
#endif	/* BIOS_PCG_H */
