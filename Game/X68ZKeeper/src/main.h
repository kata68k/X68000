#ifndef	MAIN_H
#define	MAIN_H

#include <usr_define.h>

#define MAJOR_VER   (0)
#define MINOR_VER   (0)
#define PATCH_VER   (6)

typedef struct {
	int16_t x, y;
	int16_t arm_r_x, arm_r_y;
	int16_t arm_l_x, arm_l_y;
	int16_t leg_r_x, leg_r_y;
	int16_t leg_l_x, leg_l_y;
	int16_t vx, vy;
	int16_t r_dx, r_dy;
	int16_t l_dx, l_dy;
	int16_t angle;
	int8_t	direction;
	int8_t	jump;
} PLAYER;

typedef struct {
	int16_t x, y;
	int16_t vx, vy;
	int16_t life;
	uint8_t status;
} ENEMY;

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
