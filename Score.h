#ifndef	SCORE_H
#define	SCORE_H

#include "inc/usr_define.h"

/* \‘¢‘Ì’è‹` */
typedef struct{
	uint64_t	ulScore;
	uint8_t		ubScoreID;
	uint64_t	ulScoreMax;
} ST_SCORE;

/* externéŒ¾ */
extern	int16_t S_Get_ScoreInfo(ST_SCORE *);
extern	int16_t S_Set_ScoreInfo(ST_SCORE);
extern	int16_t S_Add_Score(void);
extern	int16_t S_Reset_ScoreID(void);
extern	int16_t S_All_Init_Score(void);
extern	int16_t S_Init_Score(void);

#endif	/* SCORE_H */
