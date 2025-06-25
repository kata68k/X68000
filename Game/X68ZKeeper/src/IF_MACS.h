#ifndef	IF_MACS_H
#define	IF_MACS_H

#include <usr_define.h>
#include "BIOS_Moon.h"

#ifdef 	MACS_MOON
/* define定義 */
#define	MACS_MAX	(10)

typedef struct tagMOV_LIST {
	int8_t	bFileName[256];		/* パス */
	int8_t	bGR;				/* 0:CG OFF、1:CG ON */
	int8_t	bSP;			    /* 0:スプライトOFF、1:スプライトON */
} MOV_LIST;

/* extern宣言 */
extern MOV_LIST	g_mov_list[MACS_MAX];
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

/* 構造体定義 */

/* extern宣言 */
extern void	MOV_INIT(void);
extern int32_t	MOV_Play(uint8_t);
extern int32_t MOV_Play2(uint8_t);

#endif	/* IF_MACS_H */
