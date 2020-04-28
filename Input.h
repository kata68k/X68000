#ifndef	INPUT_H
#define	INPUT_H

#include "inc/usr_style.h"
#include "inc/usr_define.h"

/* ジョイスティック１のその１ */
#define UP		(0x01)
#define DOWN	(0x02)
#define LEFT	(0x04)
#define RIGHT	(0x08)
#define JOYA	(0x20)
#define JOYB	(0x40)
#define ESC_S	(27)

/* ジョイスティック１のその１ */
#define KEY_NULL		(0x00)
#define KEY_UPPER		(0x04)
#define KEY_LOWER		(0x08)
#define KEY_LEFT		(0x01)
#define KEY_UP_LEFT		(0x05)
#define KEY_DW_LEFT		(0x09)
#define KEY_RIGHT		(0x02)
#define KEY_UP_RIGHT	(0x06)
#define KEY_DW_RIGHT	(0x0A)
#define KEY_A			(0x10)
#define KEY_B			(0x20)
#define KEY_b_ESC		(0x40)
#define KEY_b_Q			(0x80)

#define KEY_TRUE	1
#define KEY_FALSE	0

extern SS get_key( UC );

#endif	/* INPUT_H */
