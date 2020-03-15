#ifndef	OVERKATA_H
#define	OVERKATA_H

#include "inc/usr_style.h"

enum{
	DST_none,
	DST_1us,
	DST_2p5us,
	DST_4us,
	DST_12p5us,
	DST_16us,
	DST_25us,
	DST_50us,
};

extern SS moni;
extern SS moni_MAX;
extern SS speed;

extern void Message_Num(SS, SI, SI);

#endif	/* OVERKATA_H */
