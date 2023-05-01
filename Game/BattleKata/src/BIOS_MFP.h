#ifndef	MFP_H
#define	MFP_H

#include <string.h>
#include <usr_define.h>

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
extern uint8_t GetNowTime(uint32_t *);		/* ���݂̎��Ԃ��擾���� */
extern uint8_t SetNowTime(uint32_t);		/* ���݂̎��Ԃ�ݒ肷�� */
extern uint8_t GetStartTime(uint32_t *);	/* �J�n�̎��Ԃ��擾���� */
extern uint8_t SetStartTime(uint32_t);		/* �J�n�̎��Ԃ�ݒ肷�� */
extern uint8_t GetPassTime(uint32_t, uint32_t *);	/* �o�߃^�C�}�[ */
extern int16_t StartRaster(void);
extern int16_t StopRaster(void);
extern int16_t SetRasterIntData(uint16_t);
extern int16_t GetRasterIntPos(uint16_t *, uint16_t *, uint16_t *, uint16_t, uint8_t);
extern int16_t GetRasterCount(uint16_t *);

#endif	/* MFP_H */