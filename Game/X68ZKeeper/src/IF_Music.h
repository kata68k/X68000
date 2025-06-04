#ifndef	IF_MUSIC_H
#define	IF_MUSIC_H

#include <usr_define.h>

#define ZM_V2	1
#define ZM_V3	0
#define MC_DRV	0

#define	MUSIC_MAX	(32)
#define	SOUND_MAX	(32)
#define	ADPCM_MAX	(32)

enum{
    SE_OK,
    SE_NG,
    SE_CANSEL,
    SE_SELECT,
    SE_GAME_START,
    SE_GAME_END,
    SE_GAME_CLEAR,
    SE_GAME_OVER,
    SE_MISS,
    SE_NOMOREMOVE,
    SE_EXCELLENT,
    SE_EXCHANGE,
    SE_COMBO_1,
    SE_COMBO_2,
    SE_COMBO_3,
    SE_COMBO_4,
    SE_COMBO_5,
    SE_COMBO_6,
    SE_COMBO_7,
    SE_COMBO_8,
    SE_MAX
};

enum{
    BGM_INIT,
    BGM_STOP,
    BGM_LOAD,
    BGM_TITLE,
    BGM_DEMO,
    BGM_HISCORE,
    BGM_SCORE,
    BGM_GAME_CLEAR,
    BGM_GAME_OVER,
    BGM_STAGE_1,
    BGM_STAGE_2,
    BGM_STAGE_3,
    BGM_STAGE_4,
    BGM_STAGE_5,
    BGM_MAX
};

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
extern	int32_t SE_Play(uint8_t);
extern	int32_t SE_Stop(void);
extern	int32_t ADPCM_Play(uint8_t);
extern	int32_t ADPCM_Stop(void);
extern	int32_t ADPCM_SNS(void);
extern	int32_t M_SetMusic(uint32_t);
extern	int32_t M_Play(int16_t, int16_t);
extern	int32_t	M_TEMPO(int16_t);

#endif	/* IF_MUSIC_H */
