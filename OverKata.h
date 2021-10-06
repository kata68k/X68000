#ifndef	OVERKATA_H
#define	OVERKATA_H

#include "inc/usr_define.h"

extern int16_t	g_CpuTime;

extern int16_t	FlipProc(void);
extern int16_t	SetFlip(uint8_t);
extern void Set_DI(void);
extern void Set_EI(void);
extern int16_t	GetGameMode(uint8_t *);
extern int16_t	SetGameMode(uint8_t);
extern int16_t	GetDebugNum(uint16_t *);
extern int16_t	SetDebugNum(uint16_t);
extern int16_t	GetDebugMode(uint8_t *);
extern int16_t	SetDebugMode(uint8_t);

#endif	/* OVERKATA_H */
