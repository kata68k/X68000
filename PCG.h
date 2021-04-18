#ifndef	PCG_H
#define	PCG_H

#include "inc/usr_define.h"

extern void PCG_INIT(void);
extern void PCG_VIEW(uint8_t);
extern void PCG_Rotation(uint16_t *, uint16_t *, uint8_t, uint8_t, int16_t, int16_t, uint8_t *, uint8_t, uint16_t, uint16_t);
extern void BG_TEXT_SET(int8_t *);

#endif	/* PCG_H */
