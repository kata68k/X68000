#ifndef	MFP_H
#define	MFP_H

#include <string.h>
#include "inc/usr_define.h"

enum{
	DST_none=0,
	DST_1us,
	DST_2p5us,
	DST_4us,
	DST_12p5us,
	DST_16us,
	DST_25us,
	DST_50us,
};

extern int16_t MFP_INIT(void);
extern int16_t MFP_EXIT(void);
extern int16_t MFP_RESET(void);
extern int16_t TimerD_INIT(void);
extern int16_t TimerD_EXIT(void);
extern uint8_t GetNowTime(uint32_t *);		/* 現在の時間を取得する */
extern uint8_t SetNowTime(uint32_t);		/* 現在の時間を設定する */
extern uint8_t GetStartTime(uint32_t *);	/* 開始の時間を取得する */
extern uint8_t SetStartTime(uint32_t);		/* 開始の時間を設定する */
extern int16_t SetRasterIntData(void *, size_t);
extern int16_t GetRasterIntPos(uint16_t *, uint16_t *, uint16_t *, uint16_t);
extern int16_t vwait(int16_t);


#endif	/* MFP_H */
