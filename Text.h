#ifndef	TEXT_H
#define	TEXT_H

#include "inc/usr_define.h"

extern void T_INIT(void);
extern void T_EXIT(void);
extern void T_Clear(void);
extern void T_PALET(void);
extern void T_SetBG_to_Text(void);
extern void T_TopScore(void);
extern void T_Time(void);
extern void T_Score(void);
extern void T_Speed(void);
extern void T_Gear(void);
extern void T_Main(UC);
extern SS T_Scroll(UI, UI);
extern SI T_Box(SS, SS, SS, SS, US, UC);
extern SI T_Fill(SS, SS, SS, SS, US, UC);
extern SI T_xLine(SS, SS, SS w, US, UC);
extern SI T_yLine(SS, SS, SS h, US, UC);

#endif	/* TEXT_H */
