#ifndef	MAIN_H
#define	MAIN_H

#include <usr_define.h>

#define MAJOR_VER   (0)
#define MINOR_VER   (1)
#define PATCH_VER   (1)

extern int16_t	g_CpuTime;

extern void App_TimerProc( void );
extern int16_t App_RasterProc( uint16_t * );
extern void App_VsyncProc( void );
extern void App_HsyncProc( void );
extern int16_t	App_FlipProc(void);
extern int16_t	SetFlip(uint8_t);
extern int16_t	GetGameMode(uint8_t *);
extern int16_t	SetGameMode(uint8_t);

#endif	/* MAIN_H */
