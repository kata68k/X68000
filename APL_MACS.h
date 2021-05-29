#ifndef	APL_MACS_H
#define	APL_MACS_H

#include "inc/usr_define.h"
#include "Moon.h"

#ifdef 	MACS_MOON
/* define’è‹` */
#define	MACS_MAX	(16)

/* externéŒ¾ */
extern int8_t	g_mov_list[MACS_MAX][256];
extern uint32_t	g_mov_list_max;
#endif	/* MACS_MOON */

/* \‘¢‘Ì’è‹` */

/* externéŒ¾ */
extern void	MOV_INIT(void);
extern int32_t	MOV_Play(uint8_t);

#endif	/* APL_MACS_H */
