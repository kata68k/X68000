#ifndef	OVERKATA_H
#define	OVERKATA_H

#include <usr_define.h>

extern int16_t	g_CpuTime;
extern volatile uint16_t	g_uGameStatus;

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
extern int16_t	GetDebugPos(int16_t *, int16_t *);
extern int16_t	SetDebugPos(int16_t, int16_t);
extern int16_t	GetDebugHis(int16_t *, int16_t);
extern int16_t	SetDebugHis(int16_t);

#endif	/* OVERKATA_H */
