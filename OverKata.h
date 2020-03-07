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

extern US moni;
extern US moni_MAX;
extern SI speed;

extern SI GetSpeed(SI *);
extern void Message_Num(SI, SI, SI);

#endif	/* OVERKATA_H */
