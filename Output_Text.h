#ifndef	OUTPUT_TEXT_H
#define	OUTPUT_TEXT_H

#include "inc/usr_style.h"
#include "inc/usr_define.h"


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

extern void Message_Num(void *, SI, SI, US, UC);
extern SI BG_TextPut(SC *, SI, SI);
extern SI BG_PutToText(SI, SI, SI, SI, UC);
extern SI BG_TimeCounter(UI, US, US);
extern SI BG_Number(UI, US, US);

#endif	/* OUTPUT_TEXT_H */
