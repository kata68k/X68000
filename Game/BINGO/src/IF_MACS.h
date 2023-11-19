#ifndef	IF_MACS_H
#define	IF_MACS_H

#include <usr_define.h>
#include "BIOS_Moon.h"

#define	CNF_MACS	(1)

#ifdef 	MACS_MOON
/* define’è‹` */
#define	MACS_MAX	(10)

/* externéŒ¾ */
extern int8_t	g_mov_list[MACS_MAX][256];
extern uint32_t	g_mov_list_max;
#endif	/* MACS_MOON */

/* \‘¢‘Ì’è‹` */

/* externéŒ¾ */
extern void	MOV_INIT(void);
extern int32_t	MOV_Play(uint8_t);
extern int32_t MOV_Play2(uint8_t, uint8_t);

#endif	/* IF_MACS_H */
