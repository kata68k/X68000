#ifndef	USR_DEFINE_H
#define	USR_DEFINE_H

#include "usr_style.h"
#include "usr_macro.h"

//#define	DEBUG	/* デバッグ有効定義 */

#define FALSE	(0)
#define TRUE	(!FALSE)

#define Bit_0		(0x1)
#define Bit_1		(0x2)
#define Bit_2		(0x4)
#define Bit_3		(0x8)
#define Bit_4		(0x10)
#define Bit_5		(0x20)
#define Bit_6		(0x40)
#define Bit_7		(0x80)
#define Bit_8		(0x100)
#define Bit_9		(0x200)
#define Bit_10		(0x400)
#define Bit_11		(0x800)
#define Bit_12		(0x1000)
#define Bit_13		(0x2000)
#define Bit_14		(0x4000)
#define Bit_15		(0x8000)

/* 構造体 */

/* データテーブル */

extern void (*usr_abort)(void);	/* ユーザのアボート処理関数 */

#endif	/* USR_DEFINE_H */
