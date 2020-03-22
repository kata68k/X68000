#ifndef	OVERKATA_H
#define	OVERKATA_H

#include "inc/usr_style.h"

enum{
	DST_none=0,
	DST_1us,
	DST_2p5us,
	DST_4us,
	DST_12p5us,
	DST_16us,
	DST_25us,
	DST_50us,
};

enum{
	MONI_Type_UI=0,
	MONI_Type_SI,
	MONI_Type_US,
	MONI_Type_SS,
	MONI_Type_UC,
	MONI_Type_SC,
};

extern SS moni;
extern SS moni_MAX;
extern SS speed;

extern void Message_Num(void *, SI, SI, UC);

#endif	/* OVERKATA_H */
