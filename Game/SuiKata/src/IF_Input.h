#ifndef	IF_INPUT_H
#define	IF_INPUT_H

#include <usr_define.h>

#define	CNF_JOYDRV360	(0)

/* �W���C�X�e�B�b�N���̂P */
#define JOY_UP		(0x01)
#define JOY_DOWN	(0x02)
#define JOY_LEFT	(0x04)
#define JOY_RIGHT	(0x08)
#define JOY_A		(0x20)
#define JOY_B		(0x40)
#define ESC_S		(27)

/* �W���C�X�e�B�b�N���̂Q */
#define KEY_NULL		(0x0000)
#define KEY_LEFT		(0x0001)
#define KEY_RIGHT		(0x0002)
#define KEY_UPPER		(0x0004)
#define KEY_LOWER		(0x0008)
#define KEY_b_ESC		(0x0010)
#define KEY_b_Z			(0x0020)
#define KEY_b_X			(0x0040)
#define KEY_b_Q			(0x0080)
#define KEY_b_M			(0x0100)
#define KEY_b_SP		(0x0200)
#define KEY_b_RLUP		(0x0400)
#define KEY_b_RLDN		(0x0800)
#define KEY_b_HELP		(0x1000)
#define KEY_b_TAB		(0x2000)
#define KEY_b_G			(0x4000)
#define KEY_b_F6		(0x8000)

#define KEY_TRUE	1
#define KEY_FALSE	0

/* �A�i���O�X�e�B�b�N */
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

enum
{
	r_stk_ud_0,	/* �E�X�e�B�b�NUP/DOWN */
	r_stk_lr_1,	/* �E�X�e�B�b�NLEFT/RIGHT */
	l_stk_ud_2,	/* ���X�e�B�b�NUP/DOWN */
	l_stk_lr_3,	/* ���X�e�B�b�NLEFT/RIGHT */
	btn_data_4	/* �{�^���f�[�^ */
};
typedef struct tagANALOG {
	int16_t	l_stk_ud;	/* ���X�e�B�b�NUP/DOWN */
	int16_t	l_stk_lr;	/* ���X�e�B�b�NLEFT/RIGHT */
	int16_t	r_stk_ud;	/* �E�X�e�B�b�NUP/DOWN */
	int16_t	r_stk_lr;	/* �E�X�e�B�b�NLEFT/RIGHT */
	int16_t	btn_data;	/* �{�^���f�[�^ */
} JOY_ANALOG_BUF;

extern	int32_t	g_Input;
extern	int16_t	g_Input_P[2];
extern	uint8_t	g_bAnalogStickMode;
extern	uint8_t	g_bAnalogStickMode_flag;

extern	int16_t Input_Init(void);
extern	int16_t Input_Main(uint8_t);
extern uint16_t	get_keyboard( uint16_t *, uint8_t , uint8_t );
extern uint16_t	get_djoy(uint16_t *, int32_t, uint8_t );
extern uint16_t	get_ajoy(uint16_t *, int32_t, uint8_t, uint8_t );
extern int16_t	GetAnalog_Info(int16_t *);
extern int16_t	SetAnalog_Info(int16_t *);
extern int16_t	GetJoyDevice(int32_t, int32_t, int32_t);
extern int16_t	GetJoyDevMode(int32_t);
extern int16_t	SetJoyDevMode(int32_t , int32_t , int32_t);
extern uint16_t	DirectInputKeyNum(uint16_t *, uint16_t );
extern uint8_t	ChatCancelSW(uint8_t , uint8_t *);
extern int16_t	KeyHitESC(void);

#endif	/* IF_INPUT_H */
