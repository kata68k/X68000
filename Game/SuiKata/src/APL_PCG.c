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
	uint32_t	uPCG_num;
	uint16_t	uPCG_SP_offset = 0x30;	/* 0~0x2FはBG */
	uint16_t	uPCG_SP_next = 0x00;	/* 0 */
	uint8_t		ubOK;
	uint8_t		ubPri;
	uint8_t		ubPal;

	printf("SP PCG_INIT_CHAR");
#if 0
	PCG_SP_dataload("data/sp/BG.SP");	/* スプライトのデータ読み込み */
	PCG_PAL_dataload("data/sp/BG.PAL");	/* スプライトのパレットデータ読み込み */
	BG_TEXT_SET("data/map/map.csv");	/* マップデータによるＢＧの配置 */
#endif
#if 0
	for(uPCG_num = 0; uPCG_num < PCG_MAX; uPCG_num++)
	{
		g_stPCG_DATA[uPCG_num].x				= (uPCG_num * 16) & 0xFF;	/* x座標 */
		g_stPCG_DATA[uPCG_num].y				= (uPCG_num * 16) & 0xFF;	/* y座標 */
		g_stPCG_DATA[uPCG_num].ratio			= 0;			/* 比率 */
		g_stPCG_DATA[uPCG_num].rad			= 0;			/* 回転 */
		g_stPCG_DATA[uPCG_num].Pat_w			= 1;			/* 横方向のパターン数 */
		g_stPCG_DATA[uPCG_num].Pat_h			= 1;			/* 縦方向のパターン数 */
		g_stPCG_DATA[uPCG_num].pPatCodeTbl	= MyMalloc(sizeof(uint32_t));
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl)	= SetBGcode(0, 0, 0x0C, uPCG_num);	/* パターンコードテーブル */
		g_stPCG_DATA[uPCG_num].update		= TRUE;			/* 更新 */
		g_stPCG_DATA[uPCG_num].validty		= TRUE;		/* 有効 */
	}
#endif
	
#if 0
	for(uPCG_num = 0; uPCG_num < 14; uPCG_num++)
	{
		g_stPCG_DATA[uPCG_num].x				= 16 * uPCG_num;	/* x座標 */
		g_stPCG_DATA[uPCG_num].y				= 128;	/* y座標 */
		g_stPCG_DATA[uPCG_num].ratio			= 0;	/* 比率 */
		g_stPCG_DATA[uPCG_num].rad			= 0;	/* 回転 */
		g_stPCG_DATA[uPCG_num].Pat_w			= 3;	/* 横方向のパターン数 */
		g_stPCG_DATA[uPCG_num].Pat_h			= 3;	/* 縦方向のパターン数 */
		g_stPCG_DATA[uPCG_num].Pat_size		= 1;	/* パターンサイズ 0:8x8 1:16x16 */
		g_stPCG_DATA[uPCG_num].pPatCodeTbl	= MyMalloc(sizeof(uint32_t) * g_stPCG_DATA[uPCG_num].Pat_w * g_stPCG_DATA[uPCG_num].Pat_h);
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 0)	= SetBGcode(0, 0, 0x09, 0x40);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 1)	= SetBGcode(0, 0, 0x09, 0x41);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 2)	= SetBGcode(0, 0, 0x09, 0x42);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 3)	= SetBGcode(0, 0, 0x09, 0x50);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 4)	= SetBGcode(0, 0, 0x09, 0x51);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 5)	= SetBGcode(0, 0, 0x09, 0x52);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 6)	= SetBGcode(0, 0, 0x09, 0x60);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 7)	= SetBGcode(0, 0, 0x09, 0x61);	/* パターンコードテーブル */
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + 8)	= SetBGcode(0, 0, 0x09, 0x62);	/* パターンコードテーブル */
		g_stPCG_DATA[uPCG_num].update		= TRUE;		/* 更新 */
		g_stPCG_DATA[uPCG_num].validty		= TRUE;		/* 有効 */
	}
#endif
	
#if 0
	for(uPCG_num = 0; uPCG_num < PCG_NUM_MAX; uPCG_num++)
	{
		FILE *fp;

		fp = fopen( "data/sp/BG.SP" , "rb" ) ;
		
		g_stPCG_DATA[uPCG_num].x			= SP_X_OFFSET + ((SP_W/2) * uPCG_num);	/* x座標 */
		g_stPCG_DATA[uPCG_num].y			= SP_Y_OFFSET + 128;	/* y座標 */
		g_stPCG_DATA[uPCG_num].Pat_w		= 1;	/* 横方向のパターン数 */
		g_stPCG_DATA[uPCG_num].Pat_h		= 1;	/* 縦方向のパターン数 */
		g_stPCG_DATA[uPCG_num].Pat_AnimeMax	= 1;
		/* メモリ確保 */
		uBufSize = g_stPCG_DATA[uPCG_num].Pat_w * g_stPCG_DATA[uPCG_num].Pat_h;
		/* スプライト定義設定 */
		g_stPCG_DATA[uPCG_num].pPatCodeTbl	= (uint32_t*)MyMalloc(sizeof(uint32_t) * uBufSize);
		for(j=0; j < uBufSize; j++)
		{
			*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x00, 0xFF);	/* パターンコードテーブル */
		}
		/* スプライト画像データ */
		uAnimeBuf = uBufSize * g_stPCG_DATA[uPCG_num].Pat_AnimeMax;
		g_stPCG_DATA[uPCG_num].pPatData	= (uint16_t*)MyMalloc(SP_16_SIZE * uAnimeBuf);
		memset(g_stPCG_DATA[uPCG_num].pPatData, 0, SP_16_SIZE * uAnimeBuf);
		
		fseek(fp, (0x20 * SP_16_SIZE) + (uPCG_num * (SP_16_SIZE / 2)), SEEK_SET);
		fread( g_stPCG_DATA[uPCG_num].pPatData
				,  (SP_16_SIZE / 2)		/* 1PCG = メモリサイズ */
				,  uAnimeBuf			/* 何パターン使うのか？ */
				,  fp
				);
		
		fclose( fp ) ;
		
		g_stPCG_DATA[uPCG_num].Plane		= 0xFFFF;	/* プレーンNo. */
		g_stPCG_DATA[uPCG_num].update		= TRUE;		/* 更新 */
		g_stPCG_DATA[uPCG_num].validty		= TRUE;		/* 有効 */
	}
#endif
	
#if 1
	srand(1);
	
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
			case	SP_SHIP_0	:	/* 自機 */
			{
				stPCG.Pat_w			= g_stST_PCG_LIST[5].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[5].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[5].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[5].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x20;													/* プライオリティ */
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

				/* スプライト定義設定 */
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case SP_GAL_0:				/* キサラ */
			{
				stPCG.Pat_w			= g_stST_PCG_LIST[6].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[6].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[6].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[6].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x20;
				if(uPCG_num == SP_GAL_0)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BG.SP", 0x90, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				ubPri = 0xFF;
#endif
				/* スプライト定義設定 */
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case	SP_ARROW_0:
			{
				stPCG.Pat_w			= g_stST_PCG_LIST[7].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[7].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[7].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[7].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x20;
				if(uPCG_num == SP_ARROW_0)
				{
					uPCG_SP_next += stPCG.Pat_DataMax;
				}
#else
				PCG_Load_Data("data/sp/BG.SP", 0x90, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				ubPri = 0xFF;
#endif
				/* スプライト定義設定 */
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
				}
				
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
			{
				stPCG.Pat_w			= g_stST_PCG_LIST[1].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[1].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[1].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[1].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x20;
				if(uPCG_num == SP_BALL_S_32)
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
				/* スプライト定義設定 */
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
				}
				
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
				stPCG.Pat_w			= g_stST_PCG_LIST[2].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[2].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[2].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[2].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x20;
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
				/* スプライト定義設定 */
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
				}
				
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
				stPCG.Pat_w			= g_stST_PCG_LIST[3].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[3].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[3].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[3].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x20;
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
				/* スプライト定義設定 */
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case	SP_BALL_XL_1:
			case	SP_BALL_XL_2:
			case	SP_BALL_XL_3:
			case	SP_BALL_XL_4:
			{
				stPCG.Pat_w			= g_stST_PCG_LIST[4].Pat_w;
				stPCG.Pat_h			= g_stST_PCG_LIST[4].Pat_h;
				stPCG.Pat_AnimeMax	= g_stST_PCG_LIST[4].Pat_AnimeMax;
				stPCG.Pat_DataMax	= stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				ubPal 				= g_stST_PCG_LIST[4].Pal;					/* パレット番号 */
#if  CNF_XSP
				PCG_Load_Data(NULL, 0x00, stPCG, uPCG_num, 3);
				g_stPCG_DATA[uPCG_num].Plane		= uPCG_SP_offset + uPCG_SP_next;		/* スプライトNo. */
				ubPri = 0x20;
				if(uPCG_num == SP_BALL_XL_4)
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
				/* スプライト定義設定 */
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, ubPal, ubPri);	/* パターンコードテーブル */
				}
				
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
#endif
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

