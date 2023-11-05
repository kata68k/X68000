#ifndef	TEXT_H
#define	TEXT_H

#include <usr_define.h>

/* ç\ë¢ëÃ */

/* âÊñ èÓïÒèoóÕÇÃç\ë¢ëÃ */
typedef struct
{
	uint16_t	uTimeCounter;
	uint64_t	ulScore;
	uint64_t	ulScoreMax;
	uint16_t	uVs;
	uint16_t	uShiftPos;
	uint16_t	uPosX;
	uint16_t	uPosY;
}	ST_TEXTINFO;

extern void T_DataInit(void);
extern void T_SetBG_to_Text(void);
extern void T_TopScore(void);
extern void T_Time(void);
extern void T_Score(void);
extern void T_Speed(void);
extern void T_Gear(void);
extern void T_TimerStop(void);
extern void T_TimerStart(void);
extern void T_TimerReset(void);
extern uint16_t T_Main(uint16_t *);
extern int16_t T_PutTextInfo(ST_TEXTINFO);
extern int16_t T_Get_TextInfo(ST_TEXTINFO *);
extern int16_t T_Set_TextInfo(ST_TEXTINFO);

#endif	/* TEXT_H */
