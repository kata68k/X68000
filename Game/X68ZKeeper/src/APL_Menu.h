#ifndef	APL_MENU_H
#define	APL_MENU_H

#include <usr_define.h>

typedef struct
{
    int16_t     (*MenuFunc)(int16_t);   /* 実行関数 */
    int16_t     nType;                  /* メニュー種類 */
    int8_t*     pbSelect;               /* メニュー選択内容 */
    int8_t*     sMenuName;              /* メニュー名 */
    int8_t*     sHelpMess;              /* メニュー選択時のヘルプ */
}   ST_MENU_DATA;

typedef struct
{
    ST_PCG      stPCG;          /* カーソル */
	int16_t	    nSelectMenu;		    /* メニュー番号 */
	int16_t	    nSelectMenu_Old;		/* メニュー番号(前回値) */
	int8_t		sDispMess[64];	/* ステータス */
	int16_t		nNum;			/* ナンバー */
	int16_t		nMenuX;			/* x座標 */
	int16_t		nMenuY;			/* y座標 */
}	ST_MENU;

extern uint8_t		g_ubMACS;
extern uint8_t		g_ubPractice;
extern int8_t		g_bStage;


extern void APL_Menu_Init(void);
extern void APL_Menu_Exit(void);
extern int16_t APL_Menu_Mess(int16_t);
extern int16_t APL_Menu_Proc(void);

#endif	/* APL_MENU_H */
