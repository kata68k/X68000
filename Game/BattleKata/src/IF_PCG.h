#ifndef	IF_PCG_H
#define	IF_PCG_H

#include <usr_define.h>

#define CNF_XSP (1)
/* ç\ë¢ëÃíËã` */
/* externêÈåæ */
extern void sp_dataload(void);
extern void PCG_INIT(void);
extern void PCG_ON(void);
extern void PCG_OFF(void);
extern void PCG_START_SYNC(void);
extern void PCG_END_SYNC(void);
extern void PCG_PUT_1x1(short, short, short, short);
extern void PCG_PUT_2x1(short, short, short, short);
extern void PCG_PUT_2x2(short, short, short, short);
#endif	/* IF_PCG_H */
