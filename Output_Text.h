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

/* âÊñ èÓïÒèoóÕÇÃç\ë¢ëÃ */
typedef struct
{
	US	uTimeCounter;
	US	uScore;
	US	uScoreMax;
	US	uVs;
	US	uShiftPos;
}	ST_TEXTINFO;

extern SS moni;
extern SS moni_MAX;

extern void Message_Num(void *, SS, SS, US, UC, UC *);
extern SS BG_TextPut(SC *, SS, SS);
extern SS BG_PutToText(SS, SS, SS, SS, UC);
extern SS BG_TimeCounter(UI, US, US);
extern SS BG_Number(UI, US, US);
extern SS Text_To_Text(US, SS, SS, UC, UC *);
extern SS PutTextInfo(ST_TEXTINFO);

#endif	/* OUTPUT_TEXT_H */
