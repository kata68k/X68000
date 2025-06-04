#ifndef	BIOS_TSRLIB_H
#define	BIOS_TSRLIB_H

#include <usr_define.h>

void		TSR_prepare(int32_t *p_SelfProcces_ADDR, int32_t *p_VectorADDR, int32_t nTSRMaker_HIGH, int32_t nTSRMaker_LOW);
int32_t		TSR_check(void);
int32_t		TSR_resident(int32_t *p_CallbackFunc_ADDR);
int32_t		TSR_release(void);

#endif	/* BIOS_TSRLIB_H */
