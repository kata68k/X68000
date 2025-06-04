#ifndef	BIOS_PCG_H
#define	BIOS_PCG_H

#include <usr_define.h>
#include <XSP2lib.H>

#define CNF_XSP (1)
#define	CNF_PCG_AUTO_ACCELERATION	(1)
#define	CNF_PCG_AUTO_UPDATE_OFF		(0)

/* スプライトＰＣＧパターン最大数 */
#define	PCG_MAX		(512)
#define	PAL_MAX		(16)
#define PCG_LSB		(6)		/* bit 6 -512~511 */
#define BG_P0		(0)
#define BG_P1		(1)
#define	BG_WIDTH	(8)
#define	BG_HEIGHT	(8)
#define BG_0_W		(280)
#define BG_0_H		(63)
#define BG_1_W		(280)
#define BG_1_H		(63)
#define SP_W		(16)
#define SP_W2		(8)
#define SP_H		(16)
#define SP_H2		(8)
#define SP_X_OFFSET	(16)
#define SP_X_MAX	(256+SP_X_OFFSET)
#define SP_Y_OFFSET	(16)
#define SP_Y_MAX	(256+SP_Y_OFFSET)
#define SP_8_SIZE	(32)
#define SP_16_SIZE	(128)
#define SP_8x8		(0)
#define SP_16x16	(1)
#define	SP_PLN_MAX	(128)
#define	PCG_CODE_MASK	(0xFF)
#define	PCG_16x16_AREA	(0x30)
#define	PCG_LIST_MAX	(16)

/* スプライト１つごとのデータ */
typedef struct
{
	uint16_t	Plane;			/* プレーン番号の先頭 */
	int16_t		x;				/* x座標 */
	int16_t		y;				/* y座標 */
	int16_t		z;				/* y座標 */
	int16_t		dx;				/* 移動量x */
	int16_t		dy;				/* 移動量y */
	int16_t		dz;				/* 移動量z */
	int16_t 	speed;			/* 速度 */
	int16_t 	angle;			/* 角度 */
	uint8_t		Anime;			/* 現在のアニメーション */
	uint8_t		Anime_old;		/* 前回のアニメーション */
	uint8_t		status;			/* ステータス */
	uint8_t		update;			/* 更新 */
	uint8_t		validty;		/* 有効 */
	uint8_t		Pat_w;			/* 横方向のパターン数 */
	uint8_t		Pat_h;			/* 縦方向のパターン数 */
	uint8_t		Pat_AnimeMax;	/* アニメーションパターン数 */
	uint8_t		Pat_DataMax;	/* パターンデータ数 */
	uint16_t	*pPatData;		/* パターンデータ */
	uint16_t	*pPatCodeTbl;	/* パターンコードテーブル */
}	ST_PCG;

/* スプライトリストごとのデータ */
typedef struct
{
	uint8_t		Pal;			/* パレット番号 */
	uint8_t		Pat_w;			/* 横方向のパターン数 */
	uint8_t		Pat_h;			/* 縦方向のパターン数 */
	uint8_t		Pat_AnimeMax;	/* アニメーションパターン数 */
	int16_t		hit_x;			/* 当たり判定の始点x */
	int16_t		hit_y;			/* 当たり判定の始点y */
	int16_t		hit_width;		/* 当たり判定の幅 */
	int16_t		hit_height;		/* 当たり判定の高さ */
}	ST_PCG_LIST;

extern	int8_t		g_sp_list[ PCG_LIST_MAX ][256u];
extern	uint32_t	g_sp_list_max;
extern	int8_t		g_pal_list[ PAL_MAX ][256u];
extern	uint32_t	g_pal_list_max;
extern	int8_t 		g_pcg_alt[ PCG_MAX + 1u ];	/* XSP 用 PCG 配置管理テーブル	スプライト PCG パターン最大使用数 + 1 バイトのサイズが必要。*/
extern	int8_t 		g_pcg_dat[ PCG_MAX * 128u ];	/* ＰＣＧデータファイル読み込みバッファ */
extern	uint16_t 	g_pal_dat[ PAL_MAX * 16u ];	/* パレットデータファイル読み込みバッファ */

extern ST_PCG		g_stPCG_DATA[ PCG_MAX ];
extern ST_PCG_LIST	g_stST_PCG_LIST[ PCG_LIST_MAX ];

extern void PCG_VIEW(uint8_t);
extern void PCG_Rotation(uint16_t *, uint16_t *, uint8_t, uint8_t, int16_t, int16_t, uint8_t *, uint8_t, uint16_t, uint16_t);
extern int16_t PCG_Main(uint16_t);
extern ST_PCG *PCG_Get_Info(uint8_t);
extern int16_t PCG_Set_Info(ST_PCG, uint8_t);
extern void PCG_SP_PAL_DataLoad(void);
extern void PCG_Set_PAL_Data(int16_t, int16_t);
extern int16_t PCG_Load_Data(int8_t *, uint16_t , ST_PCG , uint16_t , uint8_t );
extern int16_t PCG_Load_PAL_Data(int8_t *, uint16_t, uint16_t);
extern int16_t PCG_PAL_Change(uint16_t , uint16_t , uint16_t);
#endif	/* BIOS_PCG_H */
