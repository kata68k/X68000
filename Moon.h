#ifndef	MOON_H
#define	MOON_H

#include "inc/usr_define.h"

#define	MACS_MOON	/* MACSDRV��MOON���g���Ƃ��͒�`���� */

/* �\���̒�` */
/* extern�錾 */
int32_t MoonPlay(void *);
int32_t MoonRegst(void *);
int32_t MACS_Play(void *);
int32_t MACS_Vsync(void *);
int32_t MACS_Vsync_R(void *);
int32_t MACS_Sleep(void);

#endif	/* MOON_H */