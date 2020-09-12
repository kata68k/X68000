#ifndef	MUSIC_H
#define	MUSIC_H

#include "inc/usr_define.h"

/* \‘¢‘Ì’è‹` */
/* externéŒ¾ */
extern	void Init_Music(void);
extern	void Exit_Music(void);
extern	SI Music_Play(UC);
extern	SI Music_Stop(void);
extern	SI SE_Play(UC);
extern	SI ADPCM_Play(UC);

#endif	/* MUSIC_H */
