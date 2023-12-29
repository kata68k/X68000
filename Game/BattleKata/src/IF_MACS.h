#ifndef	IF_MACS_H
#define	IF_MACS_H

#include <usr_define.h>
#include "BIOS_Moon.h"

#define	CNF_MACS	(1)

#ifdef 	MACS_MOON
/* define��` */
#define	MACS_MAX	(1)

/* extern�錾 */
extern int8_t	g_mov_list[MACS_MAX][256];
extern uint32_t	g_mov_list_max;
#endif	/* MACS_MOON */

/* �\���̒�` */

/* extern�錾 */
extern void	MOV_INIT(void);
extern int32_t	MOV_Play(uint8_t);
extern int32_t MOV_Play2(uint8_t, uint8_t);

#endif	/* IF_MACS_H */
