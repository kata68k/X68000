#ifndef	INPUT_C
#define	INPUT_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "Input.h"


/* 関数のプロトタイプ宣言 */
US	get_key( UC );
US	DirectInputKeyNum( US );

/* 関数 */
US get_key( UC mode )
{
	UI kd_k1,kd_k2_1,kd_k2_2,kd_b,kd_b2,kd_b3,kd_b4,kd_b5,kd_b6,kd_b7;
	static SS repeat_flag_a = KEY_TRUE;
	static SS repeat_flag_b = KEY_TRUE;
	US ret = 0;
	
	kd_b4	= BITSNS( 0 );
	kd_b3	= BITSNS( 2 );
	kd_b5	= BITSNS( 3 );
	kd_b6	= BITSNS( 4 );
	kd_b2	= BITSNS( 5 );
	kd_b7	= BITSNS( 6 );
	kd_k1	= BITSNS( 7 );
	kd_k2_1	= BITSNS( 8 );
	kd_k2_2	= BITSNS( 9 );
	kd_b	= BITSNS(10 );
	
	if( (kd_b3  & 0x02 ) != 0 ) ret |= KEY_b_Q;		/* Ｑ */
	if( (kd_b4  & 0x02 ) != 0 ) ret |= KEY_b_ESC;	/* ＥＳＣ */
	if( (kd_b7  & 0x01 ) != 0 ) ret |= KEY_b_M;		/* Ｍ */
	if( (kd_b7  & 0x20 ) != 0 ) ret |= KEY_b_SP;	/* スペースキー */

	if( !( JOYGET( 0 ) & UP    ) || ( kd_k1 & 0x10 ) || ( kd_k2_1 & 0x10 ) || ( kd_b3 & 0x04 ) ) ret |= KEY_UPPER;	/* 上 ↑ 8 w */
	if( !( JOYGET( 0 ) & DOWN  ) || ( kd_k1 & 0x40 ) || ( kd_k2_2 & 0x10 ) || ( kd_b5 & 0x80 ) ) ret |= KEY_LOWER;	/* 下 ↓ 2 s */
	if( !( JOYGET( 0 ) & LEFT  ) || ( kd_k1 & 0x08 ) || ( kd_k2_1 & 0x80 ) || ( kd_b5 & 0x40 ) ) ret |= KEY_LEFT;	/* 左 ← 4 a */
	if( !( JOYGET( 0 ) & RIGHT ) || ( kd_k1 & 0x20 ) || ( kd_k2_2 & 0x02 ) || ( kd_b6 & 0x01 ) ) ret |= KEY_RIGHT;	/* 右 → 6 d */
	
	if( !( JOYGET( 0 ) & JOYA  ) || ( kd_b  & 0x20 ) || ( kd_b2   & 0x04 ) )	/* Ａボタン or XF1 or z */
	{
		if( repeat_flag_a || (mode != 0u))
		{
			ret |= KEY_A;
			repeat_flag_a = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_a = KEY_TRUE;
	}
	
	if( !( JOYGET( 0 ) & JOYB  ) || ( kd_b  & 0x40 ) || ( kd_b2   & 0x08 ) )	/* Ｂボタン or XF2 or x  */
	{
		if( repeat_flag_b || (mode != 0u))
		{
			ret |= KEY_B;
			repeat_flag_b = KEY_FALSE;
		}
	}
	else
	{
		repeat_flag_b = KEY_TRUE;
	}
	return ret;
}

US	DirectInputKeyNum(US uDigit)
{
	US ret = 0;
	static US uNum[10] = {0};
	static US uResultNum = 0;
	static US uCount = 0;
	static UC bKeyFlag1 = FALSE;
	static UC bKeyFlag2 = FALSE;
	UI i, kd_g0, kd_g1;
	
	if(uDigit > 10)return -1;
	
	kd_g0	= (BITSNS( 0 ) & 0xFCu);
	kd_g1	= (BITSNS( 1 ) & 0x0Fu);

	if(kd_g0 != 0)
	{
		if(bKeyFlag1 == FALSE)
		{
			bKeyFlag1 = TRUE;
			
			for(i = 0; i < 8; i++)
			{
				if( ((kd_g0 >> i) & 0x01) != 0u )
				{
					uNum[uCount] = i - 1;
					break;
				}
			}
			uCount++;
		}
	}
	else
	{
		bKeyFlag1 = FALSE;
	}
	
	if(kd_g1 != 0)
	{
		if(bKeyFlag2 == FALSE)
		{
			bKeyFlag2 = TRUE;

			for(i = 0; i < 8; i++)
			{
				if( ((kd_g1 >> i) & 0x01) != 0u )
				{
					uNum[uCount] = (i + 7) % 10;
					break;
				}
			}
			uCount++;
		}
	}
	else
	{
		bKeyFlag2 = FALSE;
	}

	if(uCount == uDigit)
	{
		US uCal = 0;
		US uD = 1;
		
		for(i = 0; i < uDigit; i++)
		{
			if(i == 0)
			{
				uCal += uNum[uDigit-1];
			}
			else
			{
				uCal += uNum[(uDigit-1) - i] * uD;
			}
			uD *= 10;
		}
		uResultNum = uCal;
		uCount = 0;
	}
	
	ret = uResultNum;
	
	return ret;
}
#endif	/* INPUT_C */

