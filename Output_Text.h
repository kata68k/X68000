#ifndef	OUTPUT_TEXT_H
#define	OUTPUT_TEXT_H

#include "inc/usr_style.h"
#include "inc/usr_define.h"

#define	DEBUG

enum{
	MONI_Type_UI=0,
	MONI_Type_SI,
	MONI_Type_US,
	MONI_Type_SS,
	MONI_Type_UC,
	MONI_Type_SC,
	MONI_Type_FL,
	MONI_Type_PT
};

enum{
	BG_Normal=0,
	BG_V_rev,
	BG_H_rev,
	BG_VH_rev
};

extern SS moni;
extern SS moni_MAX;

extern void Message_Num(void *, SS, SS, US, UC);
extern SS BG_TextPut(SC *, SS, SS);
extern SS BG_PutToText(SS, SS, SS, SS, UC);
extern SS BG_TimeCounter(UI, US, US);
extern SS BG_Number(UI, US, US);
extern SS Text_To_Text(US, SS, SS, UC, UC *);

#endif	/* OUTPUT_TEXT_H */
