#ifndef	IF_MUSIC_H
#define	IF_MUSIC_H

#include <usr_define.h>

#define ZM_V2	1
#define ZM_V3	0
#define MC_DRV	0

#define	MUSIC_MAX	(32)
#define	SOUND_MAX	(32)
#define	ADPCM_MAX	(32)

/* ç\ë¢ëÃíËã` */

/* externêÈåæ */
extern	int8_t		music_list[MUSIC_MAX][256];
extern	uint32_t	m_list_max;
extern	int8_t		se_list[SOUND_MAX][256];
extern	uint32_t	s_list_max;
extern	int8_t		adpcm_list[ADPCM_MAX][256];
extern	uint32_t	p_list_max;

extern	void Init_Music(void);
extern	void Exit_Music(void);
extern	int32_t Music_Play(uint8_t);
extern	int32_t Music_Stop(void);
extern	int32_t FM_SE_Play(uint8_t);
extern	int32_t FM_SE_Play_Fast(uint8_t);
extern	int32_t ADPCM_Play(uint8_t);
extern	int32_t ADPCM_Stop(void);
extern	int32_t M_SetMusic(uint32_t);
extern	int32_t M_Play(int16_t, int16_t);
#endif	/* IF_MUSIC_H */
