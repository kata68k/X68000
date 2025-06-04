#ifndef	BIOS_MPU_H
#define	BIOS_MPU_H

#include <usr_define.h>

#define IOCS_SYS_STAT	(0xAC)

extern void init_trap14 (void);
extern void init_trap12 (void);
extern uint32_t mpu_cache_clr(void);
extern int32_t	mpu_stat_chk(void);
extern void Set_DI(void);
extern void Set_EI(void);
extern int32_t	Get_ROM_Ver(void);
extern void Set_SupervisorMode(void);
extern void Set_UserMode(void);
#endif	/* BIOS_MPU_H */
