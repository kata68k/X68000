#ifndef	IF_PCG_H
#define	IF_PCG_H

#include <usr_define.h>
#include "BIOS_PCG.h"

#define PCG_PRI_HIGH    (0x3F)
#define PCG_PRI_MID     (0x2F)
#define PCG_PRI_LOW     (0x1F)

/* ç\ë¢ëÃíËã` */

/* externêÈåæ */
extern void PCG_INIT(int16_t);
extern void PCG_XSP_INIT(void);
extern void PCG_ON(void);
extern void PCG_OFF(void);
extern int8_t Get_SP_Sns(void);
extern void BG_ON(int16_t);
extern void BG_OFF(int16_t);
extern void PCG_START_SYNC(int16_t);
extern void PCG_END_SYNC(int16_t);
extern void PCG_COL_SHIFT(int16_t, int16_t);
extern void PCG_COL_SHIFT_ZERO(int16_t, int16_t);
extern void PCG_PUT_1x1(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_2x1(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_2x2(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_3x3(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PRI_CHG(ST_PCG	*, int16_t);
extern void PCG_PAL_CHG(ST_PCG	*, int16_t);
extern void PCG_PAL_CHG_SINGLE(ST_PCG *, int16_t, int16_t);
extern void PCG_PAL_GET(ST_PCG	*, int16_t *);
extern void PCG_INFO_CHG(ST_PCG *, int8_t, int8_t, int8_t, int8_t);
extern void PCG_SET_ANIME(ST_PCG *, int16_t);
extern void BG_put(int16_t, int16_t, int16_t, int16_t, int16_t);
extern void BG_put_Clr(int16_t, int16_t, int16_t, int16_t, int16_t);
extern void BG_put_String(int16_t, int16_t, int16_t, int8_t *, int16_t);
extern void BG_put_Number10(int16_t, int16_t, int16_t, uint32_t);
extern void BG_Scroll(int16_t, int16_t, int16_t);
extern void BG_TEXT_SET(int8_t *, int16_t, int16_t, int16_t, int16_t);
extern int16_t BG_TEXT_GET(int16_t, int16_t, int16_t);

#endif	/* IF_PCG_H */
