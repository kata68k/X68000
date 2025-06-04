#ifndef	BIOS_MOON_H
#define	BIOS_MOON_H

#include <usr_define.h>

#define	CNF_MACS	(0)
//#define	MACS_MOON	/* MACSDRVとMOONを使うときは定義する */

/* 構造体定義 */
/* extern宣言 */
int32_t MoonPlay(void *);
int32_t MoonPlay2(void *, int16_t);
int32_t MoonRegst(void *);
int32_t MACS_Play(void *);
int32_t MACS_Vsync(void *);
int32_t MACS_Vsync_R(void *);
int32_t MACS_Sleep(void);

#endif	/* BIOS_MOON_H */
