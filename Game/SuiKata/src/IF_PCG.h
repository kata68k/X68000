#ifndef	IF_PCG_H
#define	IF_PCG_H

#include <usr_define.h>
#include "BIOS_PCG.h"

extern	int8_t		sp_list[PCG_MAX][256];
extern	uint32_t	sp_list_max;
extern	int8_t		pal_list[PAL_MAX][256];
extern	uint32_t	pal_list_max;

/* ç\ë¢ëÃíËã` */

/* externêÈåæ */
extern void sp_dataload(void);
extern void PCG_INIT(void);
extern void PCG_XSP_INIT(void);
extern void PCG_ON(void);
extern void PCG_OFF(void);
extern int8_t Get_SP_Sns(void);
extern void BG_ON(int16_t);
extern void BG_OFF(int16_t);
extern void PCG_START_SYNC(int16_t);
extern void PCG_END_SYNC(int16_t);
extern void PCG_COL_SHIFT(int16_t, int16_t);
extern void PCG_PUT_1x1(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_2x1(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_2x2(int16_t, int16_t, int16_t, int16_t);
extern void PCG_PUT_3x3(int16_t, int16_t, int16_t, int16_t);
extern void BG_put_Clr(int16_t, int16_t, int16_t, int16_t, int16_t);
extern void BG_put_String(int16_t, int16_t, int16_t, int8_t *, int16_t);
extern void BG_put_Number10(int16_t, int16_t, int16_t, uint32_t);
extern void BG_Scroll(int16_t, int16_t, int16_t);
extern void BG_TEXT_SET(int8_t *, int16_t, int16_t, int16_t, int16_t);

#endif	/* IF_PCG_H */
