#ifndef	MFP_H
#define	MFP_H

extern void interrupt Timer_D_Func(void);
extern void interrupt Hsync_Func(void);
extern void interrupt Raster_Func(void);
extern void interrupt Vsync_Func(void);
extern int vwait(int);
extern int GetNowTime(unsigned short *);	/* Œ»İ‚ÌŠÔ‚ğæ“¾‚·‚é */
extern int SetNowTime(unsigned short);		/* Œ»İ‚ÌŠÔ‚ğİ’è‚·‚é */

#endif	/* MFP_H */
