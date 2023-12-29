#ifndef	APL_SCORE_H
#define	APL_SCORE_H

#include <usr_define.h>

/* ç\ë¢ëÃíËã` */
typedef struct{
	uint64_t	ulScoreAdd;
	uint64_t	ulScore;
	uint64_t	ulScoreMax;
	uint16_t	uCombo;
	uint16_t	uComboMax;
} ST_SCORE;

typedef struct{
	uint16_t	x;
	uint16_t	y;
	uint16_t	x_old;
	uint16_t	y_old;
} ST_SCORE_POS;

/* externêÈåæ */
extern	int16_t S_Get_ScoreInfo(ST_SCORE *);
extern	int16_t S_Set_ScoreInfo(ST_SCORE);
extern	int16_t S_Add_Score(uint64_t);
extern	int16_t S_Add_Score_Point(uint64_t);
extern	int16_t S_Set_Combo(int16_t);
extern	int16_t S_All_Init_Score(void);
extern	int16_t S_Init_Score(void);
extern	int16_t S_GetPos(int16_t, int16_t);
extern	int16_t S_Clear_Score(void);
extern	int16_t S_Main_Score(void);
extern	int16_t S_Score_Board(void);
extern	int16_t S_Score_NameInput(int16_t, int8_t, int8_t);
extern	int16_t S_Score_Save(void);
extern	int16_t S_Score_Load(void);

#endif	/* APL_SCORE_H */
