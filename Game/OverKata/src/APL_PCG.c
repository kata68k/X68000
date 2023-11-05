#ifndef	APL_PCG_C
#define	APL_PCG_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>
#include "BIOS_PCG.h"
#include "IF_FileManager.h"
#include "IF_Math.h"
#include "APL_PCG.h"

/* define定義 */
#define	PCG_16x16_AREA	(0x40)

/* グローバル変数 */

/* 関数のプロトタイプ宣言 */
void PCG_INIT_CHAR(void);

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
	uint32_t	j;
	uint32_t	uPCG_num, uBufSize;
	uint8_t		ubOK;

	PCG_SP_dataload("data/sp/BG.SP");	/* スプライトのデータ読み込み */
	PCG_PAL_dataload("data/sp/BG.PAL");	/* スプライトのパレットデータ読み込み */
	BG_TEXT_SET("data/map/map.csv");	/* マップデータによるＢＧの配置 */
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
		
		g_stPCG_DATA[uPCG_num].Plane		= 0xFF;		/* プレーンNo. */
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
			case MYCAR_PCG_NEEDLE:	/* タコメーター針 */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 32;
				stPCG.Pat_DataMax	= 8;
				
				PCG_Load_Data("data/sp/BG.SP", 0x40, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0D, 0x0D);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					if(j < 8)
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(1, 1, 0x0D, 0xFF);	/* パターンコードテーブル */
					else if(j < 16)
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 1, 0x0D, 0xFF);	/* パターンコードテーブル */
					else if(j < 24)
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0D, 0xFF);	/* パターンコードテーブル */
					else
						*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(1, 0, 0x0D, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case MYCAR_PCG_TACHO:	/* タコメーター */
			{
				stPCG.Pat_w		= 2;
				stPCG.Pat_h		= 2;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 4;
				
				PCG_Load_Data("data/sp/BG.SP", 0x48, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0D, 0x0D);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0D, 0xFF);	/* パターンコードテーブル */
				}

				ubOK = TRUE;
				break;
			}
			case MYCAR_PCG_STEERING_POS:	/* ステアリングポジション */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BG.SP", 0x3F, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0C, 0x0C);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0C, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case MYCAR_PCG_STEERING:	/* ステアリング */
			{
				stPCG.Pat_w			= 2;
				stPCG.Pat_h			= 2;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 4;
				
				PCG_Load_Data("data/sp/BG.SP", 0x4C, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0B, 0x0B);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0B, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case SCORE_PCG_1:	/* スコア１ */
			case SCORE_PCG_2:	/* スコア２ */
			case SCORE_PCG_3:	/* スコア３ */
			case SCORE_PCG_4:	/* スコア４ */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 10;
				stPCG.Pat_DataMax	= 10;
				
				PCG_Load_Data("data/sp/BG.SP", 0x20, stPCG, uPCG_num, 1);
				/* テキスト共用の０番パレットを使用 */
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x00, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case ROAD_PCG_ARROW:				/* 矢印 */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BG.SP", 0xB2, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x07, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case ROAD_PCG_SIGNAL_1:	/* シグナルランプ1 */
			case ROAD_PCG_SIGNAL_2:	/* シグナルランプ2 */
			case ROAD_PCG_SIGNAL_3:	/* シグナルランプ3 */
			{
				stPCG.Pat_w		= 1;
				stPCG.Pat_h		= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 1;
				
				PCG_Load_Data("data/sp/BG.SP", 0x3E, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x01, 0x01);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0A, 0x0A);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0A, 0xFF);	/* パターンコードテーブル */
				}

				ubOK = TRUE;
				break;
			}
			case OBJ_SHADOW:	/* 影1*3 */
			{
				stPCG.Pat_w			= 3;
				stPCG.Pat_h			= 1;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 3;	/* w x h x AnimeMax = DataMax */
				
				PCG_Load_Data("data/sp/BG.SP", 0x98, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x0B, 0x0B);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x0B, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case ETC_PCG_SONIC:				/* ソニック */
			{
				stPCG.Pat_w			= 3;
				stPCG.Pat_h			= 3;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 9;
				
				PCG_Load_Data("data/sp/BG.SP", 0xA0, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/BG.PAL", 0x09, 0x09);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x09, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case ETC_PCG_KISARA:				/* キサラ */
			{
				stPCG.Pat_w			= 2;
				stPCG.Pat_h			= 4;
				stPCG.Pat_AnimeMax	= 1;
				stPCG.Pat_DataMax	= 8;
				
				PCG_Load_Data("data/sp/BG.SP", 0x90, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x07, 0xFF);	/* パターンコードテーブル */
				}
				
				ubOK = TRUE;
				break;
			}
			case ETC_PCG_MAN:				/* ピクト君 */
			{
				stPCG.x			= SP_X_OFFSET + 128;	/* x座標 */
				stPCG.y			= SP_Y_OFFSET + 128;	/* y座標 */
				stPCG.Anime		= 0;	/* 現在のアニメ */
				stPCG.Anime_old	= 0;	/* 前回のアニメ */
				stPCG.Pat_w		= 2;
				stPCG.Pat_h		= 4;
				stPCG.Pat_AnimeMax	= 8;
				stPCG.Pat_DataMax	= 8*8;
				
				PCG_Load_Data("data/sp/BG.SP", 0x50, stPCG, uPCG_num, 0);
				PCG_Load_PAL_Data("data/sp/MASUO.PAL", 0x01, 0x07);
				
				uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;

				/* スプライト定義設定 */
				for(j=0; j < uBufSize; j++)
				{
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x07, 0xFF);	/* パターンコードテーブル */
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
		g_stPCG_DATA[uPCG_num].Plane		= 0xFF;		/* プレーンNo. */
#ifdef DEBUG
//		printf("g_stPCG_DATA[%d]=0x%p\n", uPCG_num, &g_stPCG_DATA[uPCG_num]);
//		KeyHitESC();	/* デバッグ用 */
#endif
	}
#endif

}

#endif	/* APL_PCG_C */

