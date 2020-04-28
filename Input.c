#ifndef	INPUT_C
#define	INPUT_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "Input.h"


/* 関数のプロトタイプ宣言 */
SS get_key( UC );

/* 関数 */
SS get_key( UC mode )
{
	UI kd_k1,kd_k2_1,kd_k2_2,kd_b,kd_b2,kd_b3,kd_b4,kd_b5,kd_b6;
	static SS repeat_flag_a = KEY_TRUE;
	static SS repeat_flag_b = KEY_TRUE;
	SS ret = 0;
	
	kd_k1	= BITSNS(7);
	kd_k2_1	= BITSNS(8);
	kd_k2_2	= BITSNS(9);
	kd_b	= BITSNS(10);
	kd_b2	= BITSNS(5);
	kd_b3	= BITSNS( 2 );
	kd_b4	= BITSNS( 0 );
	kd_b5	= BITSNS( 3 );
	kd_b6	= BITSNS( 4 );
	
	if( (kd_b3  & 0x02 ) != 0 ) ret |= KEY_b_Q;		/* Ｑで終了 */
	if( (kd_b4  & 0x02 ) != 0 ) ret |= KEY_b_ESC;	/* ＥＳＣポーズ */

	if( !( JOYGET( 0 ) & UP    ) || ( kd_k1 & 0x10 ) || ( kd_k2_1 & 0x10 ) || ( kd_b3 & 0x04 ) ) ret |= KEY_UPPER;
	if( !( JOYGET( 0 ) & DOWN  ) || ( kd_k1 & 0x40 ) || ( kd_k2_2 & 0x10 ) || ( kd_b5 & 0x80 ) ) ret |= KEY_LOWER;
	if( !( JOYGET( 0 ) & LEFT  ) || ( kd_k1 & 0x08 ) || ( kd_k2_1 & 0x80 ) || ( kd_b5 & 0x40 ) ) ret |= KEY_LEFT;
	if( !( JOYGET( 0 ) & RIGHT ) || ( kd_k1 & 0x20 ) || ( kd_k2_2 & 0x02 ) || ( kd_b6 & 0x01 ) ) ret |= KEY_RIGHT;
	if( !( JOYGET( 0 ) & JOYA  ) || ( kd_b  & 0x20 ) || ( kd_b2   & 0x04 ) ) { /* Ａボタン or XF1 or z */
		if( repeat_flag_a || (mode != 0u)) {
			ret |= KEY_A;
			repeat_flag_a = KEY_FALSE;
			}
		} else {
			repeat_flag_a = KEY_TRUE;
		}
	if( !( JOYGET( 0 ) & JOYB  ) || ( kd_b  & 0x40 ) || ( kd_b2   & 0x08 ) ) { /* Ｂボタン or XF2 or x  */
		if( repeat_flag_b || (mode != 0u)) {
			ret |= KEY_B;
			repeat_flag_b = KEY_FALSE;
			}
		} else {
			repeat_flag_b = KEY_TRUE;
		}
	return ret;
}

#endif	/* INPUT_C */

