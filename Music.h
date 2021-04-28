#ifndef	MUSIC_H
#define	MUSIC_H

#include "inc/usr_define.h"

/* \‘¢‘Ì’è‹` */
/* externéŒ¾ */
extern	void Init_Music(void);
extern	void Exit_Music(void);
extern	int32_t Music_Play(uint8_t);
extern	int32_t Music_Stop(void);
extern	int32_t SE_Play(uint8_t);
extern	int32_t SE_Play_Fast(uint8_t);
extern	int32_t ADPCM_Play(uint8_t);
extern	int32_t ADPCM_Stop(void);
extern	int32_t M_SetMusic(uint32_t);
extern	int32_t M_Play(int16_t);
#endif	/* MUSIC_H */
