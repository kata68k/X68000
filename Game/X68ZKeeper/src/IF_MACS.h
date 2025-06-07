#ifndef	IF_MACS_H
#define	IF_MACS_H

#include <usr_define.h>
#include "BIOS_Moon.h"

#ifdef 	MACS_MOON
/* defineíËã` */
#define	MACS_MAX	(10)

/* externêÈåæ */
extern int8_t	g_mov_list[MACS_MAX][256];
extern uint32_t	g_mov_list_max;
#endif	/* MACS_MOON */

enum{
    MOV_GAME_START,
    MOV_GAME_END,
    MOV_GAME_CLEAR,
    MOV_GAME_OVER,
    MOV_NOMOREMOVE,
    MOV_EXCELLENT,
    MOV_MAX
};

/* ç\ë¢ëÃíËã` */

/* externêÈåæ */
extern void	MOV_INIT(void);
extern int32_t	MOV_Play(uint8_t);
extern int32_t MOV_Play2(uint8_t, uint8_t);

#endif	/* IF_MACS_H */
