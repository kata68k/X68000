#ifndef	APL_MENU_C
#define	APL_MENU_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>

#include "IF_Draw.h"
#include "IF_Input.h"
#include "IF_Graphic.h"
#include "IF_Math.h"
#include "IF_PCG.h"
#include "APL_Menu.h"
#include "APL_PCG.h"
#include "APL_Score.h"

/* define定義 */
#define Menu_INDEX  (4)
#define Menu_POS_X  (14)
#define Menu_POS_Y  (64)


/* 関数のプロトタイプ宣言 */
void APL_Menu_Init(void);
void APL_Menu_Exit(void);
static int16_t Menu_Set_Dummy(int16_t);
static int16_t Menu_Set_CG_ON(int16_t);
static int16_t Menu_Set_CG_OFF(int16_t);
static int16_t Menu_Set_MACS_ON(int16_t);
static int16_t Menu_Set_MACS_OFF(int16_t);
int16_t APL_Menu_Mess(int16_t);
int16_t APL_Menu_HELP(void);
int16_t APL_Menu_CURSOR(void);
int16_t APL_Menu_Proc(void);

/* グローバル変数 */
static int8_t s_b_Updata = TRUE;

uint8_t		g_ubMACS = TRUE;
uint8_t		g_ubPractice = FALSE;
int8_t      g_bStage = 0;

int16_t     g_WindowMMax1 = 0;
int16_t     g_WindowMMax2 = 0;


ST_MENU  stMenu;

ST_MENU_DATA    stMenu_Data[] = {
    /* Menu1 */
    {
        Menu_Set_Dummy,
        1,
        &g_ubPractice,
        "GAME MODE(NORMAL/PRACTICE)",
        "ゲームモード(通常／練習)を切り替え"
    },
    /* Menu2 */
    {
        Menu_Set_Dummy,
        2,
        &g_bStage,
        "PRACTICE LEVEL(0-5)",
        "練習モードのレベル切り替え"
    },
    /* Menu3 */
    {
        Menu_Set_Dummy,
        0,
        &g_ubMACS,
        "MACS (ON/OFF)",
        "カットインを有効／無効に切り替え"
    },
    /* Menu4 */
    {
        NULL,
        0,
        NULL,
        "START",
        "ゲームを開始"
    }
};


/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void APL_Menu_Init(void)
{
    int16_t loop;
    int16_t len, len_max, len_max2;
    int8_t str[128] = {0};
    ST_MENU  *p_stMenuWork = (ST_MENU *)&stMenu;

    p_stMenuWork->nSelectMenu = Menu_INDEX-1;
    p_stMenuWork->nNum = 0;
    p_stMenuWork->nMenuX = Menu_POS_X;
    p_stMenuWork->nMenuY = Menu_POS_Y;
    s_b_Updata = TRUE;

    G_PAGE_SET(0b0001);	/* GR0 */

    /* Windowの幅を決めるために文字の最大の長さを測る */
    len_max = 0;
    len = 0;
    for(loop=0; loop < Menu_INDEX; loop++)
    {
        if(stMenu_Data[loop].pbSelect != NULL)
        {
            switch(stMenu_Data[loop].nType)
            {
                case 0:
                default:
                {
                    int8_t sOnOff[4] = {0};
                    if(*stMenu_Data[loop].pbSelect == TRUE)
                    {
                        strcpy( sOnOff, "ON");
                    }
                    else
                    {
                        strcpy( sOnOff, "OFF");
                    }
                    sprintf( str, "%s = %s", stMenu_Data[loop].sMenuName, sOnOff );
                    break;
                }
                case 1:
                {
                    int8_t sMes[10] = {0};
                    if(*stMenu_Data[loop].pbSelect == TRUE)
                    {
                        strcpy( sMes, "PRACTICE");
                    }
                    else
                    {
                        strcpy( sMes, "NORMAL  ");
                    }
                    sprintf( str, "%s = %s", stMenu_Data[loop].sMenuName, sMes );
                    break;
                }
                case 2:
                {
                    sprintf( str, "%s = %d", stMenu_Data[loop].sMenuName, *stMenu_Data[loop].pbSelect );
                    break;
                }
            }
        }
        else
        {
            sprintf( str, "%s", stMenu_Data[loop].sMenuName );
        }
        len = strlen(str);
        if(len_max < len)len_max = len;
    }
    g_WindowMMax1 = len_max;
    Draw_Fill(Menu_POS_X - 12, Menu_POS_Y - 12, Menu_POS_X + (g_WindowMMax1 * 6) + 8 - 1, Menu_POS_Y + (loop * 12) + 12, G_BLACK);
    Draw_Box(Menu_POS_X - 12 - 1, Menu_POS_Y - 12, Menu_POS_X + (g_WindowMMax1 * 6) + 8, Menu_POS_Y + (loop * 12) + 12, G_WHITE, 0xFFFF);
    for(loop=0; loop < Menu_INDEX; loop++)
    {
        APL_Menu_Mess(loop);
    }
    APL_Menu_CURSOR();

    len_max2 = 0;
    len = 0;
    for(loop=0; loop < Menu_INDEX; loop++)
    {
        len = strlen(stMenu_Data[loop].sHelpMess);
        if(len_max2 < len)len_max2 = len;
    }
    g_WindowMMax2 = len_max2;
    Draw_Fill(Menu_POS_X - 12, Menu_POS_Y - 12 + 128, Menu_POS_X + (g_WindowMMax2 * 6) + 8, Menu_POS_Y + 12 + 12 + 128, G_BLACK);
    Draw_Box(Menu_POS_X - 12, Menu_POS_Y - 12 + 128, Menu_POS_X + (g_WindowMMax2 * 6) + 8, Menu_POS_Y + 12 + 12 + 128, G_WHITE, 0xFFFF);
    APL_Menu_HELP();
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void APL_Menu_Exit(void)
{
	G_CLR_AREA(0, 256, 0, 256, 0);	/* ページ 0 */
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Menu_Set_Dummy(int16_t nData)
{
    return 0;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Menu_Set_CG_ON(int16_t nData)
{
    return 0;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Menu_Set_CG_OFF(int16_t nData)
{
    return 0;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Menu_Set_MACS_ON(int16_t nData)
{
    return 0;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static int16_t Menu_Set_MACS_OFF(int16_t nData)
{
    return 0;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t APL_Menu_Mess(int16_t nPos)
{
    int16_t ret = 0;

    int8_t str[128] = {0};

    G_PAGE_SET(0b0001);	/* GR0 */

    /* 設定 */
    if(stMenu_Data[nPos].pbSelect != NULL)
    {
        switch(stMenu_Data[nPos].nType)
        {
            case 0:
            default:
            {
                int8_t sOnOff[4] = {0};
                if(*stMenu_Data[nPos].pbSelect == TRUE)
                {
                    strcpy( sOnOff, "ON");
                }
                else
                {
                    strcpy( sOnOff, "OFF");
                }
                sprintf( str, "%s = %s", stMenu_Data[nPos].sMenuName, sOnOff );
                break;
            }
            case 1:
            {
                int8_t sMes[10] = {0};
                if(*stMenu_Data[nPos].pbSelect == TRUE)
                {
                    strcpy( sMes, "PRACTICE");
                }
                else
                {
                    strcpy( sMes, "NORMAL  ");
                }
                sprintf( str, "%s = %s", stMenu_Data[nPos].sMenuName, sMes );
                break;
            }
            case 2:
            {
                sprintf( str, "%s = %d", stMenu_Data[nPos].sMenuName, *stMenu_Data[nPos].pbSelect );
                break;
            }
        }
    }
    else
    {
        sprintf( str, "%s", stMenu_Data[nPos].sMenuName );
    }

    Draw_Fill(Menu_POS_X, Menu_POS_Y + (nPos * 12), Menu_POS_X + (g_WindowMMax1 * 6) + 8 - 1, Menu_POS_Y + (nPos * 12) + 12, G_BLACK);
    PutGraphic_To_Symbol12(str, Menu_POS_X, Menu_POS_Y + (nPos * 12), G_WHITE);

    return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t APL_Menu_HELP(void)
{
    int16_t ret = 0; 
    ST_MENU  *p_stMenuWork = (ST_MENU *)&stMenu;

    G_PAGE_SET(0b0001);	/* GR0 */

    /* HELP */
    PutGraphic_To_Symbol12(stMenu_Data[p_stMenuWork->nSelectMenu_Old].sHelpMess, Menu_POS_X, Menu_POS_Y + 128, G_BLACK);
    PutGraphic_To_Symbol12(stMenu_Data[p_stMenuWork->nSelectMenu].sHelpMess, Menu_POS_X, Menu_POS_Y + 128, G_WHITE);

    return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t APL_Menu_CURSOR(void)
{
    int16_t ret = 0; 
    ST_MENU  *p_stMenuWork = (ST_MENU *)&stMenu;

    G_PAGE_SET(0b0001);	/* GR0 */

    /* カーソル */
    PutGraphic_To_Symbol12("→" , Menu_POS_X - 12, Menu_POS_Y + (p_stMenuWork->nSelectMenu_Old * 12) , G_BLACK);
    PutGraphic_To_Symbol12("→" , Menu_POS_X - 12, Menu_POS_Y + (p_stMenuWork->nSelectMenu * 12) , G_WHITE);

    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t APL_Menu_Proc(void)
{
    int16_t ret = 0; 
    static int8_t s_b_UP;
    static int8_t s_b_DOWN;
    static int8_t s_b_RIGHT;
    static int8_t s_b_LEFT;
    static int8_t s_b_A;
    static int8_t s_b_B;
    
    ST_MENU  *p_stMenuWork = (ST_MENU *)&stMenu;

    if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
        ((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
        ((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
    {
        if(s_b_A == FALSE)
        {
            s_b_A = TRUE;

            if(stMenu_Data[p_stMenuWork->nSelectMenu].MenuFunc == NULL )
            {

            }
            else{
                p_stMenuWork->nNum = stMenu_Data[p_stMenuWork->nSelectMenu].MenuFunc( p_stMenuWork->nNum );
            }
            ret = p_stMenuWork->nSelectMenu + 1;
        }
        else
        {
        }
    }
    else
    {
        s_b_A = FALSE;
    }

    if(	((GetInput_P1() & JOY_B ) != 0u)	||		/* B */
        ((GetInput() & KEY_b_X) != 0u)	)			/* B(z) */
    {
        if(s_b_B == FALSE)
        {
            s_b_B = TRUE;
        }
    }
    else
    {
        s_b_B = FALSE;
    }

    if(	((GetInput_P1() & JOY_LEFT ) != 0u )	||	/* LEFT */
        ((GetInput() & KEY_LEFT) != 0 )		)	/* 左 */
    {
        if(s_b_LEFT == FALSE)
        {
            s_b_LEFT = TRUE;

            if(stMenu_Data[p_stMenuWork->nSelectMenu].MenuFunc == NULL )
            {

            }
            else{
                p_stMenuWork->nNum = stMenu_Data[p_stMenuWork->nSelectMenu].MenuFunc( p_stMenuWork->nNum );
                ret = p_stMenuWork->nSelectMenu + 1;
            }
        }
    }
    else if( ((GetInput_P1() & JOY_RIGHT ) != 0u )	||	/* RIGHT */
            ((GetInput() & KEY_RIGHT) != 0 )			)	/* 右 */
    {
        if(s_b_RIGHT == FALSE)
        {
            s_b_RIGHT = TRUE;

            if(stMenu_Data[p_stMenuWork->nSelectMenu].MenuFunc == NULL )
            {

            }
            else{
                p_stMenuWork->nNum = stMenu_Data[p_stMenuWork->nSelectMenu].MenuFunc( p_stMenuWork->nNum );
                ret = p_stMenuWork->nSelectMenu + 1;
            }
        }
    }
    else
    {
        s_b_LEFT = FALSE;
        s_b_RIGHT = FALSE;
    }

    if(	((GetInput_P1() & JOY_UP ) != 0u )	||	/* UP */
        ((GetInput() & KEY_UPPER) != 0 )	)	/* 上 */
    {
        if(s_b_UP == FALSE)
        {
            s_b_UP = TRUE;

            p_stMenuWork->nSelectMenu_Old = p_stMenuWork->nSelectMenu;
            p_stMenuWork->nSelectMenu-=1;
            if(p_stMenuWork->nSelectMenu < 0)p_stMenuWork->nSelectMenu = Menu_INDEX - 1;

            APL_Menu_HELP();    /* ヘルプメッセージ更新 */
            APL_Menu_CURSOR();  /* カーソル更新 */
        }
    }
	else if(((GetInput_P1() & JOY_DOWN ) != 0u )	||	/* UP */
       		((GetInput() & KEY_LOWER) != 0 )	)	    /* 下 */
    {
        if(s_b_DOWN == FALSE)
        {
            s_b_DOWN = TRUE;

            p_stMenuWork->nSelectMenu_Old = p_stMenuWork->nSelectMenu;
            p_stMenuWork->nSelectMenu+=1;
            if(p_stMenuWork->nSelectMenu >= Menu_INDEX)p_stMenuWork->nSelectMenu = 0;
            
            APL_Menu_HELP();    /* ヘルプメッセージ更新 */
            APL_Menu_CURSOR();  /* カーソル更新 */
        }
    }
    else
    {
        s_b_UP = FALSE;
        s_b_DOWN = FALSE;
    }

    if( (s_b_UP == TRUE)    ||
        (s_b_DOWN == TRUE)  ||
        (s_b_RIGHT == TRUE) ||
        (s_b_LEFT == TRUE)  ||
        (s_b_A == TRUE)     ||
        (s_b_B == TRUE)     )
    {
        s_b_Updata = TRUE;
    }

    if( s_b_Updata == TRUE )
    {
        s_b_Updata = FALSE;
    }

    return ret;
}

#endif	/* APL_MENU_C */
