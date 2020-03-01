#ifndef	OVERKATA_H
#define	OVERKATA_H

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

extern unsigned short moni;
extern unsigned short moni_MAX;
extern int speed;

extern int GetSpeed(int *);
extern void Message_Num(int, int, int);

#endif	/* OVERKATA_H */
