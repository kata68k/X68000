#ifndef	MFP_H
#define	MFP_H

#include <string.h>
#include "inc/usr_style.h"

extern UI Init_MFP(void);
extern void interrupt Timer_D_Func(void);
extern void interrupt Hsync_Func(void);
extern void interrupt Raster_Func(void);
extern void interrupt Vsync_Func(void);
extern SI vwait(SI);
extern SI GetNowTime(US *);	/* Œ»İ‚ÌŠÔ‚ğæ“¾‚·‚é */
extern SI SetNowTime(US);		/* Œ»İ‚ÌŠÔ‚ğİ’è‚·‚é */
extern SI SetRasterVal(void *, size_t);	

#endif	/* MFP_H */
