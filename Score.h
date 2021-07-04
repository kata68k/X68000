#ifndef	SCORE_H
#define	SCORE_H

#include "inc/usr_define.h"

/* ç\ë¢ëÃíËã` */
typedef struct{
	uint64_t	ulScore;
	uint8_t		ubScoreID;
	uint64_t	ulScoreMax;
} ST_SCORE;

/* externêÈåæ */
extern	int16_t S_Get_ScoreInfo(ST_SCORE *);
extern	int16_t S_Set_ScoreInfo(ST_SCORE);
extern	int16_t S_Add_Score(void);
extern	int16_t S_Add_Score_Point(uint64_t);
extern	int16_t S_Reset_ScoreID(void);
extern	int16_t S_All_Init_Score(void);
extern	int16_t S_Init_Score(void);
extern	int16_t S_Main_Score(void);

#endif	/* SCORE_H */
