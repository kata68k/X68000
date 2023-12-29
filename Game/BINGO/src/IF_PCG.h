#ifndef	IF_PCG_H
#define	IF_PCG_H

#include <usr_define.h>

#define CNF_XSP (0)
/* �\���̒�` */
/* extern�錾 */
extern void PCG_INIT(void);
extern void PCG_ON(void);
extern void PCG_OFF(void);
extern void PCG_START_SYNC(int16_t);
extern void PCG_END_SYNC(int16_t);
extern void PCG_PUT_1x1(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_2x1(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_2x2(int16_t, int16_t, int16_t, int16_t);
#endif	/* IF_PCG_H */