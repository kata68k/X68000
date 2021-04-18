#ifndef	CRTC_H
#define	CRTC_H

#include "inc/usr_define.h"

#define	CRT_MAX		(3)

/* 構造体定義 */

/* 画面情報の構造体 */
typedef struct
{
	int16_t	view_offset_x;		/* 表示画面のX座標オフセット */
	int16_t	view_offset_y;		/* 表示画面のY座標オフセット */
	int16_t	hide_offset_x;		/* 非表示画面のX座標オフセット */
	int16_t	hide_offset_y;		/* 非表示画面のY座標オフセット */
	int16_t	BG_offset_x;		/* BG画面のX座標オフセット */
	int16_t	BG_offset_y;		/* BG画面のY座標オフセット */
}	ST_CRT;

/* extern宣言 */
extern	void CRTC_INIT(void);
extern	void CRTC_INIT_Manual(void);
extern	int16_t	GetCRT(ST_CRT *, int16_t);
extern	int16_t	SetCRT(ST_CRT, int16_t);
extern	int16_t	CRT_INIT(void);
extern	int16_t	Get_CRT_Contrast(int8_t *);
extern	int16_t	Set_CRT_Contrast(int8_t);

#endif	/* CRTC_H */
