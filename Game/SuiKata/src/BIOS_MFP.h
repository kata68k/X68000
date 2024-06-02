#ifndef	MFP_H
#define	MFP_H

#include <string.h>
#include <usr_define.h>

#define CNF_VDISP	(1)
#define CNF_HDISP	(0)
#define CNF_RASTER	(0)

#define PASSTIME_INIT	(0xFFFFFFFF)

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


extern volatile uint16_t Hsync_count;
extern volatile uint16_t Vsync_count;


extern int16_t MFP_INIT(void);
extern int16_t MFP_EXIT(void);
extern int16_t MFP_RESET(void);
extern int16_t TimerD_INIT(void);
extern int16_t TimerD_EXIT(void);
extern uint32_t Get_CPU_Time(void);
extern uint8_t GetNowTime(uint32_t *);		/* 現在の時間を取得する */
extern uint8_t SetNowTime(uint32_t);		/* 現在の時間を設定する */
extern uint8_t GetStartTime(uint32_t *);	/* 開始の時間を取得する */
extern uint8_t SetStartTime(uint32_t);		/* 開始の時間を設定する */
extern uint8_t GetPassTime(uint32_t, uint32_t *);	/* 経過タイマー */
extern int16_t StartRasterInt(void);
extern int16_t StopRasterInt(void);
extern uint8_t GetRasterCount(uint16_t *);
extern uint8_t	SetRasterCount(uint16_t, uint16_t);

#endif	/* MFP_H */
