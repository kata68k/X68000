#ifndef	OUTPUT_TEXT_H
#define	OUTPUT_TEXT_H

#include "inc/usr_define.h"

enum{
	MONI_Type_UL=0,
	MONI_Type_SL,
	MONI_Type_UI,
	MONI_Type_SI,
	MONI_Type_US,
	MONI_Type_SS,
	MONI_Type_UC,
	MONI_Type_SC,
	MONI_Type_FL,
	MONI_Type_PT
};

enum{
	BG_Normal=0,	/* ’Êí */
	BG_V_rev,		/* ‚’¼”½“] */
	BG_H_rev,		/* …•½”½“] */
	BG_VH_rev		/* ‚’¼…•½”½“] */
};

/* \‘¢‘Ì */

/* ‰æ–Êî•ño—Í‚Ì\‘¢‘Ì */
typedef struct
{
	uint16_t	uTimeCounter;
	uint16_t	uScore;
	uint16_t	uScoreMax;
	uint16_t	uVs;
	uint16_t	uShiftPos;
	uint16_t	uPosX;
	uint16_t	uPosY;
}	ST_TEXTINFO;

extern int16_t moni;
extern int16_t moni_MAX;

extern void Message_Num(void *, int16_t, int16_t, uint16_t, uint8_t, uint8_t *);
extern int16_t BG_TextPut(int8_t *, int16_t, int16_t);
extern int16_t BG_PutToText(int16_t, int16_t, int16_t, int16_t, uint8_t);
extern int16_t BG_TimeCounter(uint32_t, uint16_t, uint16_t);
extern int16_t BG_Number(uint32_t, uint16_t, uint16_t);
extern int16_t Text_To_Text(uint16_t, int16_t, int16_t, uint8_t, uint8_t *);
extern int16_t PutTextInfo(ST_TEXTINFO);
extern int16_t Put_Message_To_Graphic(uint8_t *, uint8_t);

#endif	/* OUTPUT_TEXT_H */
