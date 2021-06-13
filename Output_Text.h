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
	BG_Normal=0,	/* í èÌ */
	BG_V_rev,		/* êÇíºîΩì] */
	BG_H_rev,		/* êÖïΩîΩì] */
	BG_VH_rev		/* êÇíºêÖïΩîΩì] */
};

/* ç\ë¢ëÃ */

extern int16_t moni;
extern int16_t moni_MAX;

extern void Message_Num(void *, int16_t, int16_t, uint16_t, uint8_t, uint8_t *);
extern int16_t BG_TextPut(int8_t *, int16_t, int16_t);
extern int16_t BG_PutToText(int16_t, int16_t, int16_t, int16_t, uint8_t);
extern int16_t BG_TimeCounter(uint32_t, uint16_t, uint16_t);
extern int16_t BG_Number(uint32_t, uint16_t, uint16_t);
extern int16_t Text_To_Text(uint16_t, int16_t, int16_t, uint8_t, uint8_t *);
extern int16_t Text_To_Text2(uint64_t, int16_t, int16_t, uint8_t, uint8_t *);
extern int16_t Put_Message_To_Graphic(uint8_t *, uint8_t);

#endif	/* OUTPUT_TEXT_H */
