#ifndef	IF_INPUT_H
#define	IF_INPUT_H

#include <usr_define.h>

/* ジョイスティック１のその１ */
#define UP		(0x01)
#define DOWN	(0x02)
#define LEFT	(0x04)
#define RIGHT	(0x08)
#define JOYA	(0x20)
#define JOYB	(0x40)
#define ESC_S	(27)

/* ジョイスティック１のその１ */
#define KEY_NULL		(0x0000)
#define KEY_LEFT		(0x0001)
#define KEY_RIGHT		(0x0002)
#define KEY_UPPER		(0x0004)
#define KEY_LOWER		(0x0008)
#define KEY_A			(0x0010)
#define KEY_B			(0x0020)
#define KEY_b_ESC		(0x0040)
#define KEY_b_Q			(0x0080)
#define KEY_b_M			(0x0100)
#define KEY_b_SP		(0x0200)
#define KEY_b_RLUP		(0x0400)
#define KEY_b_RLDN		(0x0800)
#define KEY_b_HELP		(0x1000)
#define KEY_b_TAB		(0x2000)
#define KEY_b_G			(0x4000)

#define KEY_TRUE	1
#define KEY_FALSE	0

/* アナログスティック */
#define AJOY_SELECT		(0x0001)
#define AJOY_START		(0x0002)
#define AJOY_E2			(0x0004)
#define AJOY_E1			(0x0008)
#define AJOY_D			(0x0010)
#define AJOY_C			(0x0020)
#define AJOY_B_B_DASH	(0x0040)
#define AJOY_A_A_DASH	(0x0080)
#define AJOY_B_DASH		(0x0100)
#define AJOY_A_DASH		(0x0200)
#define AJOY_B			(0x0400)
#define AJOY_A			(0x0800)

typedef struct tagANALOG {
	uint16_t	l_stk_ud;	/* 左スティックUP/DOWN */
	uint16_t	l_stk_lr;	/* 左スティックLEFT/RIGHT */
	uint16_t	r_stk_ud;	/* 右スティックUP/DOWN */
	uint16_t	r_stk_lr;	/* 右スティックLEFT/RIGHT */
	uint16_t	btn_data;	/* ボタンデータ */
} JOY_ANALOG_BUF;

extern	int16_t	g_Input;
extern	uint8_t	g_bAnalogStickMode;
extern	uint8_t	g_bAnalogStickMode_flag;

extern uint16_t	get_keyboard( uint16_t *, uint8_t , uint8_t );
extern uint16_t	get_djoy(uint16_t *, uint8_t, uint8_t );
extern uint16_t	get_ajoy(uint16_t *, uint8_t, uint8_t, uint8_t );
extern int16_t	GetAnalog_Info(JOY_ANALOG_BUF *);
extern int16_t	SetAnalog_Info(JOY_ANALOG_BUF);
extern uint16_t	DirectInputKeyNum(uint16_t *, uint16_t );
extern uint8_t	ChatCancelSW(uint8_t , uint8_t *);
extern int16_t	KeyHitESC(void);

#endif	/* IF_INPUT_H */
