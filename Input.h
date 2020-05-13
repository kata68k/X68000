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

#define KEY_TRUE	1
#define KEY_FALSE	0

extern US	get_key( UC );
extern US	DirectInputKeyNum( US );

#endif	/* INPUT_H */
