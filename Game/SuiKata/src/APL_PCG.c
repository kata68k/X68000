#ifndef	APL_PCG_C
#define	APL_PCG_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>
#include "BIOS_PCG.h"
#include "IF_PCG.h"
#include "IF_FileManager.h"
#include "IF_Math.h"
#include "APL_PCG.h"

/* define定義 */

/* グローバル変数 */

/* 関数のプロトタイプ宣言 */
void PCG_INIT_CHAR(void);
void PCG_DATA_LOAD(void);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_INIT_CHAR(void)
{
	uint32_t	i, j;
	uint32_t	uPCG_list;
	uint32_t	uPCG_num;
	uint16_t	uPCG_SP_offset = PCG_16x16_AREA;	/* 0~0x2FはBG */
	uint16_t	uPCG_SP_next = 0x00;	/* 0はBG */
	uint8_t		ubOK;
	uint8_t		ubPri = 0x00;
	uint8_t		ubPal = 0x00;

	printf("SP PCG_INIT_CHAR");
	
	/* 自車スプライト */
	for(uPCG_num = 0; uPCG_num < PCG_NUM_MAX; uPCG_num++)
	{
		ST_PCG stPCG;
		
		/* たちまち初期化は不要 */
		stPCG.x			= 0;	/* x座標 */
		stPCG.y			= 0;	/* y座標 */
		stPCG.dx		= 0;	/* 移動量x */
		stPCG.dy		= 0;	/* 移動量y */
		stPCG.Anime		= 0;	/* 現在のアニメ */
		stPCG.Anime_old	= 0;	/* 前回のアニメ */
		
		switch(uPCG_num)
		{
			case	BG_DATA:
			{
				uPCG_list = 0;
				ubOK = FALSE;
				continue;
			}
			case	SP_SHIP_0	:	/* 自機 */
			{
				uPCG_list = 5;
				stPCG.Pat_w			= g_stST_PCG_LIST[uPCG_list].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[uPCG_list].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[uPCG_list].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[uPCG_list].Pal;				/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x2F;													/* プライオリティ */
				if(uPCG_num == SP_SHIP_0)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_1)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0A, 0x0A);
				}
				ubPri = 0xFF;
#endif
				ubOK = TRUE;
				break;
			}
			case SP_GAL_0:				/* キサラ */
			{
				uPCG_list = 6;
				stPCG.Pat_w			= g_stST_PCG_LIST[uPCG_list].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[uPCG_list].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[uPCG_list].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[uPCG_list].Pal;				/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x2E;
				if(uPCG_num == SP_GAL_0)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BG.SP", 0x90, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				ubPri = 0xFF;
#endif
				ubOK = TRUE;
				break;
			}
			case	SP_ARROW_0:
			{
				uPCG_list = 7;
				stPCG.Pat_w			= g_stST_PCG_LIST[uPCG_list].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[uPCG_list].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[uPCG_list].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[uPCG_list].Pal;				/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x2E;
				if(uPCG_num == SP_ARROW_0)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BG.SP", 0x90, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				ubPri = 0xFF;
#endif
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_S_1	:
			case	SP_BALL_S_2	:
			case	SP_BALL_S_3	:
			case	SP_BALL_S_4	:
			case	SP_BALL_S_5	:
			case	SP_BALL_S_6	:
			case	SP_BALL_S_7	:
			case	SP_BALL_S_8	:
			case	SP_BALL_S_9	:
			case	SP_BALL_S_10:
			case	SP_BALL_S_11:
			case	SP_BALL_S_12:
			case	SP_BALL_S_13:
			case	SP_BALL_S_14:
			case	SP_BALL_S_15:
			case	SP_BALL_S_16:
			case	SP_BALL_S_17:
			case	SP_BALL_S_18:
			case	SP_BALL_S_19:
			case	SP_BALL_S_20:
			case	SP_BALL_S_21:
			case	SP_BALL_S_22:
			case	SP_BALL_S_23:
			case	SP_BALL_S_24:
			case	SP_BALL_S_25:
			case	SP_BALL_S_26:
			case	SP_BALL_S_27:
			case	SP_BALL_S_28:
			case	SP_BALL_S_29:
			case	SP_BALL_S_30:
			case	SP_BALL_S_31:
			case	SP_BALL_S_32:
			case	SP_BALL_S_33:
			case	SP_BALL_S_34:
			case	SP_BALL_S_35:
			case	SP_BALL_S_36:
			case	SP_BALL_S_37:
			case	SP_BALL_S_38:
			case	SP_BALL_S_39:
			case	SP_BALL_S_40:
			case	SP_BALL_S_41:
			case	SP_BALL_S_42:
			case	SP_BALL_S_43:
			case	SP_BALL_S_44:
			case	SP_BALL_S_45:
			case	SP_BALL_S_46:
			case	SP_BALL_S_47:
			case	SP_BALL_S_48:
			{
				uPCG_list = 1;
				stPCG.Pat_w			= g_stST_PCG_LIST[uPCG_list].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[uPCG_list].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[uPCG_list].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[uPCG_list].Pal;				/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x2D;
				if(uPCG_num == SP_BALL_S_48)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_1)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0A, 0x0A);
				}
				ubPri = 0xFF;
#endif
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_M_1:
			case	SP_BALL_M_2:
			case	SP_BALL_M_3:
			case	SP_BALL_M_4:
			case	SP_BALL_M_5:
			case	SP_BALL_M_6:
			case	SP_BALL_M_7:
			case	SP_BALL_M_8:
			case	SP_BALL_M_9:
			case	SP_BALL_M_10:
			case	SP_BALL_M_11:
			case	SP_BALL_M_12:
			case	SP_BALL_M_13:
			case	SP_BALL_M_14:
			case	SP_BALL_M_15:
			case	SP_BALL_M_16:
			{
				uPCG_list = 2;
				stPCG.Pat_w			= g_stST_PCG_LIST[uPCG_list].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[uPCG_list].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[uPCG_list].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[uPCG_list].Pal;				/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x2C;
				if(uPCG_num == SP_BALL_M_16)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_16)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0B, 0x0B);
				}
				ubPri = 0xFF;
#endif
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_L_1:
			case	SP_BALL_L_2:
			case	SP_BALL_L_3:
			case	SP_BALL_L_4:
			case	SP_BALL_L_5:
			case	SP_BALL_L_6:
			case	SP_BALL_L_7:
			case	SP_BALL_L_8:
			{
				uPCG_list = 3;
				stPCG.Pat_w			= g_stST_PCG_LIST[uPCG_list].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[uPCG_list].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[uPCG_list].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[uPCG_list].Pal;				/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x2B;
				if(uPCG_num == SP_BALL_L_8)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_31)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0C, 0x0C);
				}				
				ubPri = 0xFF;
#endif				
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_XL_1:
			case	SP_BALL_XL_2:
			{
				uPCG_list = 4;
				stPCG.Pat_w			= g_stST_PCG_LIST[uPCG_list].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[uPCG_list].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[uPCG_list].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[uPCG_list].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x2A;
				if(uPCG_num == SP_BALL_XL_2)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BALL.SP", 0x00, stPCG, uPCG_num, 0);
				if(uPCG_num == SP_BALL_31)
				{
					PCG_Load_PAL_Data("data/sp/BALL.PAL", 0x0C, 0x0C);
				}				
				ubPri = 0xFF;
#endif				
				ubOK = TRUE;
				break;
			}
			default:
			{

				ubOK = FALSE;
				break;
			}
		}
				
		if(ubOK == TRUE)
		{
			/* スプライト定義設定 */
			for(j=0; j < stPCG.Pat_DataMax; j++)
			{
#if  CNF_XSP
				*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetXSPinfo(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
#else
				*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode2(0, 0, ubPal);	/* パターンコードテーブル */
#endif
			}
			g_stPCG_DATA[uPCG_num].update	= FALSE;
			g_stPCG_DATA[uPCG_num].validty	= TRUE;
		}
		else
		{
			g_stPCG_DATA[uPCG_num].Pat_w		= 0;
			g_stPCG_DATA[uPCG_num].Pat_h		= 0;
			g_stPCG_DATA[uPCG_num].update	= FALSE;
			g_stPCG_DATA[uPCG_num].validty	= FALSE;
		}
#if  CNF_XSP
		/* なし */
#else
		g_stPCG_DATA[uPCG_num].Plane		= 0xFFFF;		/* プレーンNo. */
#endif
//		printf("%d, %d, %d, %d, %d\n", stPCG.Pat_w, stPCG.Pat_h, stPCG.Pat_AnimeMax, stPCG.Pat_DataMax, ubPal);
#if 1
		if((uPCG_num % 10) == 0)
		{
			printf("\n");
		}
		printf("..........");
		printf("\033[10D");	/* $1B ESC[10D 左に10文字分移動 */
		for(i=0; i<(uPCG_num % 10)+1; i++)
		{
			printf("*");
		}
		printf("\n");
		printf("\033[1A");	/* $1B ESC[1A 1行上に移動 */
#endif

#ifdef DEBUG
//		printf("g_stPCG_DATA[%d]=0x%p\n", uPCG_num, &g_stPCG_DATA[uPCG_num]);
//		KeyHitESC();	/* デバッグ用 */
#endif
	}

	if((uPCG_SP_offset + uPCG_SP_next) >= PCG_MAX)
	{
		printf("\nERROR:PCG定義OVER(%d >= %d)\n", (uPCG_SP_offset + uPCG_SP_next), PCG_MAX);
	}
	else
	{
		printf("\n(%d)...Ok!\n", (uPCG_SP_offset + uPCG_SP_next));
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_DATA_LOAD(void)
{
	sp_dataload();
}

#endif	/* APL_PCG_C */

