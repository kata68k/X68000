#ifndef	IF_TEXT_H
#define	IF_TEXT_H

#include "usr_define.h"


#define T_BLACK1	(0)
#define T_BLACK2	(1)
#define T_RED		(2)
#define T_WHITE		(3)
#define T_SKIN		(4)
#define T_RED2		(5)
#define T_YELLOW	(6)
#define T_GREEN		(7)
#define T_BLUE		(8)
#define T_D_RED		(9)
#define T_D_WHITE	(10)
#define T_D_SKIN	(11)
#define T_D_RED2	(12)
#define T_D_YELLOW	(13)
#define T_D_GREEN	(14)
#define T_D_BLUE	(15)

#define T_WIDTH		(512)
#define T_HEIGHT	(512)

/* 構造体 */

struct _xylineptr
{
	uint16_t vram_page;		/* テキストページ */
	int16_t x;				/* xの座標 */
	int16_t y;				/* yの座標 */
	int16_t x1;				/* 終点までのxの長さ */
	int16_t y1;				/* 終点までのyの長さ */
	uint16_t line_style;	/* ラインスタイル */
};

enum{
	MONI_Type_UL=0,
	MONI_Type_SL,
	MONI_Type_UI,
	MONI_Type_SI,
	MONI_Type_US,
	MONI_Type_SS,
	MONI_Type_UC,
	MONI_Type_SC,
	MONI_Type_FL,
	MONI_Type_PT
};

enum{
	BG_Normal=0,	/* 通常 */
	BG_V_rev,		/* 垂直反転 */
	BG_H_rev,		/* 水平反転 */
	BG_VH_rev		/* 垂直水平反転 */
};

extern int16_t moni;
extern int16_t moni_MAX;

extern void T_INIT(void);
extern void T_EXIT(void);
extern void T_Clear(void);
extern void T_PALET(void);
extern uint16_t T_Main(uint16_t *);
extern int16_t T_Scroll(uint32_t, uint32_t);
extern int32_t T_Box(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
extern int32_t T_Fill(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
extern int32_t T_xLine(int16_t, int16_t, int16_t w, uint16_t, uint8_t);
extern int32_t T_yLine(int16_t, int16_t, int16_t h, uint16_t, uint8_t);
extern int32_t T_Line(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
extern int32_t T_Line2(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
extern int16_t T_Circle(int16_t, int16_t, int16_t, int16_t, uint16_t, uint8_t);
extern void T_FillCircle(int16_t, int16_t, int16_t, uint8_t);
extern int16_t T_SetQuarterFont(int8_t *, int8_t *);
extern void Message_Num(void *, int16_t, int16_t, uint16_t, uint8_t, uint8_t *);
extern int16_t BG_TextPut(int8_t *, int16_t, int16_t);
extern int16_t BG_PutToText(int16_t, int16_t, int16_t, int16_t, uint8_t);
extern int16_t BG_TimeCounter(uint32_t, uint16_t, uint16_t);
extern int16_t BG_Number(uint32_t, uint16_t, uint16_t);
extern int16_t Text_To_Text(uint16_t, int16_t, int16_t, uint8_t, uint8_t *);
extern int16_t Text_To_Text2(uint64_t, int16_t, int16_t, uint8_t, uint8_t *);
extern int16_t Put_Message_To_Graphic(uint8_t *, uint8_t);

#endif	/* IF_TEXT_H */
