#ifndef	BIOS_PCG_C
#define	BIOS_PCG_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>
#include "BIOS_PCG.h"
#include "IF_FileManager.h"
#include "APL_Math.h"

/* define定義 */
#define	PCG_16x16_AREA	(0x40)

/* グローバル変数 */
static ST_PCG	g_stPCG_DATA[PCG_MAX] = {0};

/* 関数のプロトタイプ宣言 */
void PCG_INIT(void);
void PCG_VIEW(uint8_t);
void PCG_Rotation(uint16_t *, uint16_t *, uint8_t, uint8_t, int16_t, int16_t, uint8_t *, uint8_t, uint16_t, uint16_t);
void BG_TEXT_SET(int8_t *);
int16_t PCG_Main(void);
ST_PCG *PCG_Get_Info(uint8_t);
int16_t PCG_Set_Info(ST_PCG, uint8_t);
int16_t PCG_Load_Data(int8_t *, uint16_t , ST_PCG , uint16_t , uint8_t );
int16_t PCG_Load_PAL_Data(int8_t *, uint16_t, uint16_t);
int16_t PCG_PAL_Change(uint16_t , uint16_t , uint16_t);
/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_INIT(void)
{
#if 0
	uint32_t	j;
	
	/* スプライトの初期化 */
	_iocs_sp_init();			/* スプライトの初期化 */
	
	/*スプライトレジスタ初期化*/
	for( j = 0x80000000; j < 0x80000000 + 128; j++ )
	{
		_iocs_sp_regst(j,-1,0,0,0,0);
	}
	for(uint32_t i = 0; i < 256; i++ )
	{
		_iocs_sp_cgclr(i);			/* スプライトクリア */
		_iocs_sp_defcg( i, 1,  );
	}
	
	//	_iocs_bgctrlgt(1);				/* BGコントロールレジスタ読み込み */
	_iocs_bgscrlst(0,0,0);				/* BGスクロールレジスタ設定 */
	_iocs_bgscrlst(1,0,0);				/* BGスクロールレジスタ設定 */
	_iocs_bgtextcl(0,SetBGcode(0,0,0,0));	/* BGテキストクリア */
	_iocs_bgtextcl(1,SetBGcode(0,0,0,0));	/* BGテキストクリア */
	//	BGTEXTGT(1,1,0);			/* BGテキスト読み込み */
#endif

	PCG_SP_dataload("data/sp/BK.SP");	/* スプライトのデータ読み込み */
	PCG_PAL_dataload("data/sp/BK.PAL");	/* スプライトのパレットデータ読み込み */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_VIEW(uint8_t bSW)
{
	if((bSW & Bit_0) != 0u)
	{
		_iocs_sp_on();				/* スプライト表示をＯＮ */
	}
	else
	{
		_iocs_sp_off();				/* スプライト表示をＯＦＦ */
	}
	
	if((bSW & Bit_1) != 0u)
	{
		_iocs_bgctrlst(0, 1, 1);	/* ＢＧ０表示ＯＮ */
	}
	else
	{
		_iocs_bgctrlst(0, 1, 0);	/* ＢＧ０表示ＯＦＦ */
	}

	if((bSW & Bit_2) != 0u)
	{
		_iocs_bgctrlst(1, 1, 1);	/* ＢＧ１表示ＯＮ */
	}
	else
	{
		_iocs_bgctrlst(1, 1, 0);	/* ＢＧ１表示ＯＦＦ */
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_Rotation(uint16_t *pDst, uint16_t *pSrc, uint8_t bX_num, uint8_t bY_num, int16_t pos_x, int16_t pos_y, uint8_t *sp_num, uint8_t palNum, uint16_t ratio, uint16_t rad)
{
	int16_t	x, y, vx, vy;
	int16_t	width, height;
	uint64_t	uPCG;
	uint64_t	uPCG_ARY[8];
	uint64_t	*pDstWork, *pSrcWork;
	uint8_t	*pBuf, *pDstBuf = NULL, *pSrcBuf = NULL;
	uint64_t	uADDR;
	uint64_t	uPointer_ADR, uPointer_ADR_X, uPointer_ADR_Y, uPointer_ADR_subX, uPointer_ADR_subY;
	uint16_t	uMem_size;
	uint8_t	bShift;
	uint64_t	code = 0;
	uint8_t	V=0, H=0;
	uint8_t	spNum;	
//	int8_t	bEx_num=0;
	
	/* src size */
	width =  16 * (int16_t)bX_num;
	height = 16 * (int16_t)bY_num;
	uMem_size = width * height * sizeof(uint8_t);
	
	if((ratio > 0) && (ratio <= 16))
	{
		bShift = ratio;	/* def:2048(=8) */
	}
	else
	{
		bShift = 8;
	}
//	Message_Num(&bShift,   0, 9, 2, MONI_Type_UC, "%2d");
//	Message_Num(&bEx_num, 11, 9, 2, MONI_Type_SC, "%2d");
	
	/* 作業エリア確保 */
	pSrcBuf = (uint8_t*)MyMalloc( (int32_t)uMem_size );
	if( pSrcBuf == NULL )
	{
		return;
	}
	pBuf = pSrcBuf;

	/* PCG -> CG */
	uPointer_ADR = (uint64_t)pSrc;
	uPointer_ADR_Y = 0ul;
	
	for(y = 0; y < bY_num; y++)	/* 一行分 */
	{
		uPointer_ADR_subY = 0ul;
		
		for(vy = 0; vy < 16; vy++)	/* 16bit分(8dot+8dot) */
		{
			uPointer_ADR_X = 0ul;
			
			for(x = 0; x < bX_num; x++)	/* 一列分 */
			{
				uPointer_ADR_subX = 0ul;
				
				for(vx = 0; vx < 2; vx++)	/* 16bit分 */
				{
					uADDR = uPointer_ADR + uPointer_ADR_Y + uPointer_ADR_subY + uPointer_ADR_X + uPointer_ADR_subX;	/* 原点 */
					pSrcWork = (uint64_t *)uADDR;	/* 0と2/1と3 */

					uPCG = (uint64_t)*pSrcWork;
					
					*pBuf = (uint8_t)((uPCG >> 28ul) & 0x0Fu);
					pBuf++;
					*pBuf = (uint8_t)((uPCG >> 24ul) & 0x0Fu);
					pBuf++;
					*pBuf = (uint8_t)((uPCG >> 20ul) & 0x0Fu);
					pBuf++;
					*pBuf = (uint8_t)((uPCG >> 16ul) & 0x0Fu);
					pBuf++;
					*pBuf = (uint8_t)((uPCG >> 12ul) & 0x0Fu);
					pBuf++;
					*pBuf = (uint8_t)((uPCG >>  8ul) & 0x0Fu);
					pBuf++;
					*pBuf = (uint8_t)((uPCG >>  4ul) & 0x0Fu);
					pBuf++;
					*pBuf = (uint8_t)((uPCG        ) & 0x0Fu);
					pBuf++;
					
					uPointer_ADR_subX += 0x40ul;
				}
				uPointer_ADR_X += 0x80ul;
			}
			uPointer_ADR_subY += 0x04ul;
		}
		uPointer_ADR_Y += 0x800ul;
	}

	/* 画像処理 */
#if 0
	/* 更に領域を拡張する機能を実装しようとしたが断念 */
	width =  16 * Mmax(((int16_t)bX_num + bEx_num), 1);
	height = 16 * Mmax(((int16_t)bY_num + bEx_num), 1);
	uMem_size = width * height * sizeof(uint8_t);
#endif
	
	pDstBuf = (uint8_t*)MyMalloc( (int32_t)uMem_size );
	if( pDstBuf == NULL )
	{
		MyMfree(pSrcBuf);	/* メモリ解放 */
		return;
	}
	pBuf = pDstBuf;
	memset(pBuf, 0, (size_t)uMem_size);
	
	if((rad == 0) && (bShift == 8))
	{
		memcpy( pDstBuf, pSrcBuf, (size_t)uMem_size );	/* 只のコピー */
	}
	else
	{
		int16_t	dx, dy;
		int16_t	width_h, height_h;
		int16_t	cos, sin;
		uint8_t	*pDstGR, *pSrcGR;
		
		cos = APL_Cos(rad);
		sin = APL_Sin(rad);
		width_h = width >> 1;
		height_h = height >> 1;

		pSrcGR = pSrcBuf;
		pDstGR = pBuf;
		
		for(y = 0; y < height; y++)
		{
			dy = y - height_h;
			
			for(x = 0; x < width; x++)
			{
				dx = x - width_h;
				
				vx = (((dx * cos) - (dy * sin)) >> bShift) + width_h;
				vy = (((dx * sin) + (dy * cos)) >> bShift) + height_h;
				
				if( (vx >= 0) && (vy >= 0) && (vx < width) && (vy < height) )
				{
					pSrcGR = pSrcBuf + ((vy * width) + vx);
					*pDstGR = *pSrcGR;
				}
				pDstGR++;
			}
		}
	}
	MyMfree(pSrcBuf);	/* メモリ解放 */

	/* CG -> PCG */
	pBuf = pDstBuf;

	uPointer_ADR = (uint64_t)pDst;
	uPointer_ADR_Y = 0ul;

	for(y = 0; y < bY_num; y++)	/* 一行分 */
	{
		uPointer_ADR_subY = 0ul;
		
		for(vy = 0; vy < 16; vy++)	/* 16bit分(8dot+8dot) */
		{
			uPointer_ADR_X = 0ul;
			
			for(x = 0; x < bX_num; x++)	/* 一列分 */
			{
				uPointer_ADR_subX = 0ul;
				
				for(vx = 0; vx < 2; vx++)	/* 16bit分 */
				{
					uADDR = uPointer_ADR + uPointer_ADR_Y + uPointer_ADR_subY + uPointer_ADR_X + uPointer_ADR_subX;	/* 原点 */
				
					uPCG_ARY[7] = (uint64_t)*pBuf;
					pBuf++;
					uPCG_ARY[6] = (uint64_t)*pBuf;
					pBuf++;
					uPCG_ARY[5] = (uint64_t)*pBuf;
					pBuf++;
					uPCG_ARY[4] = (uint64_t)*pBuf;
					pBuf++;
					uPCG_ARY[3] = (uint64_t)*pBuf;
					pBuf++;
					uPCG_ARY[2] = (uint64_t)*pBuf;
					pBuf++;
					uPCG_ARY[1] = (uint64_t)*pBuf;
					pBuf++;
					uPCG_ARY[0] = (uint64_t)*pBuf;
					pBuf++;
					
					uPCG = 	((uPCG_ARY[7] << 28ul) & 0xF0000000ul) |
							((uPCG_ARY[6] << 24ul) & 0x0F000000ul) |
							((uPCG_ARY[5] << 20ul) & 0x00F00000ul) |
							((uPCG_ARY[4] << 16ul) & 0x000F0000ul) |
							((uPCG_ARY[3] << 12ul) & 0x0000F000ul) |
							((uPCG_ARY[2] <<  8ul) & 0x00000F00ul) |
							((uPCG_ARY[1] <<  4ul) & 0x000000F0ul) |
							((uPCG_ARY[0]        ) & 0x0000000Ful);

					pDstWork = (uint64_t *) uADDR;
					*pDstWork = uPCG;

					uPointer_ADR_subX += 0x40ul;
				}
				uPointer_ADR_X += 0x80ul;
			}
			uPointer_ADR_subY += 0x04ul;
		}
		uPointer_ADR_Y += 0x800ul;
	}
	
	MyMfree(pDstBuf);	/* メモリ解放 */

	/* PCG -> SP */
	uPointer_ADR = (uint64_t)pDst;
	uPointer_ADR_Y = 0ul;
	spNum = *sp_num;
	
	for(y = 0; y < bY_num; y++)	/* 一行分 */
	{
		uPointer_ADR_X = 0ul;

		for(x = 0; x < bX_num; x++)	/* 一列分 */
		{
			uint8_t	patNum;
			patNum = (uint8_t)( (uint64_t) ( (uPointer_ADR - 0xEB8000ul) + uPointer_ADR_Y + uPointer_ADR_X ) / 0x80ul); /*0x43*/;
			code = (uint64_t)( 0xCFFFU & ( ((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((palNum & 0xFU)<<8U) | (patNum & 0xFFU) ) );
			SP_REGST( spNum++, -1, pos_x + (x * 16), pos_y + (y * 16), code, 3);
			*sp_num = spNum;
			uPointer_ADR_X += 0x80ul;
		}
		uPointer_ADR_Y += 0x800ul;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void BG_TEXT_SET(int8_t *fname)
{
	uint16_t	usV_pat;
	uint32_t	pal = 0;
	uint16_t	i,j;
	int32_t	x,y;
	uint16_t	map_data[64][64];

	/* マップデータ読み込み */
	File_Load_CSV( fname, (uint16_t *)&map_data[0][0], &i, &j);
	
	for(y=0; y<16; y++)
	{
		for(x=0; x<i; x++)
		{
			if(y < 12)		pal = 0x0E;
			else if(x > 31)	pal = 0x0F;
			else 			pal = 0x0E;
			_iocs_bgtextst(0,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
			_iocs_bgtextst(1,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
		}
	}

	usV_pat=0;
	pal = 1;
	for(y=16; y<j; y++)
	{
		for(x=0; x<i; x++)
		{
			if(x < 16 || x > 40){
				_iocs_bgtextst(0,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
				_iocs_bgtextst(1,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
			}
			else{	/* 垂直反転 */
				_iocs_bgtextst(0,x,y,SetBGcode(0,1,pal,map_data[y][x]));		/* BGテキスト設定 */
				_iocs_bgtextst(1,x,y,SetBGcode(0,1,pal,map_data[y][x]));		/* BGテキスト設定 */
			}
		}
		
		usV_pat++;
		if(usV_pat < 12){
		}
		else{
			pal++;
			usV_pat=0;
		}
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_Main(void)
{
	int16_t	ret = 0;
	int16_t	uWidth = 0, uHeight = 0;
	int16_t	x, y, count, pat_size, Anime_pat_size, pat_data;
	int16_t	uPCG_prw;
	int16_t	uPCG_num;
	uint8_t	Plane_num;
	uint8_t	Anime_num, Anime_num_old, DataMax;
	static uint16_t	s_uPCG_Num = PCG_16x16_AREA;

	for(uPCG_num = 0; uPCG_num < PCG_NUM_MAX; uPCG_num++)
	{
		if(g_stPCG_DATA[uPCG_num].validty == FALSE)
		{
			continue;
		}
		
		if( g_stPCG_DATA[uPCG_num].Pat_h == 0 )
		{
			continue;
		}
		
		if( g_stPCG_DATA[uPCG_num].Pat_w == 0 )
		{
			continue;
		}

		/* プレーン番号 */
		Plane_num = g_stPCG_DATA[uPCG_num].Plane;
		
		/* パターンサイズ */
		pat_size = g_stPCG_DATA[uPCG_num].Pat_w * g_stPCG_DATA[uPCG_num].Pat_h;
		
		/* アニメーション番号 */
		Anime_num_old = g_stPCG_DATA[uPCG_num].Anime_old;
		Anime_num = g_stPCG_DATA[uPCG_num].Anime;
		DataMax = g_stPCG_DATA[uPCG_num].Pat_DataMax;
		
		Anime_pat_size = Anime_num * pat_size;
		
		/* スプライト設定 */
		count = 0;
		uHeight = 0;
		for( y = 0; y < g_stPCG_DATA[uPCG_num].Pat_h; y++ )
		{
			uWidth = 0;
			for( x = 0; x < g_stPCG_DATA[uPCG_num].Pat_w; x++ )
			{
				pat_data = (count + (Anime_pat_size));
				
				/* PCGを定義、再定義する */
				if(	g_stPCG_DATA[uPCG_num].Plane == 0xFF ) /* 初回 */
				{
					if(count == 0)	/* 先頭のみ */
					{
						Plane_num = s_uPCG_Num;	/* プレーン番号決定 */
					}
					
					/* PCGを定義してパターンデータ書き換え */
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + pat_data) &= ~0xFFU;
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + pat_data) |= s_uPCG_Num;
					
					/* メモリからPCGエリアへコピー */
					_iocs_sp_defcg( (Plane_num + count) & (PCG_MAX-1),
									SP_16x16,
						g_stPCG_DATA[uPCG_num].pPatData + Mmul64(count + ((Anime_pat_size) % DataMax)) );	/* 64 = 128byte */
					
					s_uPCG_Num++;
					if(s_uPCG_Num >= PCG_MAX)
					{
						s_uPCG_Num = PCG_16x16_AREA;
					}
				}
				else if(Anime_num_old != Anime_num)	/* PCGを定義はそのままでアニメーション変更 */
				{
					/* メモリからPCGエリアへコピー */
					_iocs_sp_defcg( (Plane_num + count) & (PCG_MAX-1),
									SP_16x16,
						g_stPCG_DATA[uPCG_num].pPatData + Mmul64(count + ((Anime_pat_size) % DataMax)) );	/* 64 = 128byte */
				}
				
				/* 表示 */
				if(	g_stPCG_DATA[uPCG_num].update == TRUE )	/* 更新アリ */
				{
					/* パターンデータ書き換え */
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + pat_data) &= ~0xFFU;
					*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + pat_data) |= (Plane_num + count);
					uPCG_prw = 3;
				}
				else
				{
					uPCG_prw = 0;
				}
				
				/* 移動量 */
				g_stPCG_DATA[uPCG_num].x += g_stPCG_DATA[uPCG_num].dx;
				g_stPCG_DATA[uPCG_num].y += g_stPCG_DATA[uPCG_num].dy;
				
				/* スプライトレジスタの設定 */
				_iocs_sp_regst( (Plane_num + count) & (SP_PLN_MAX-1),
								-1,	/* mode:最上位ビット 1=垂直同期検出 0=垂直同期検出しない */
								g_stPCG_DATA[uPCG_num].x + uWidth,
								g_stPCG_DATA[uPCG_num].y + uHeight,
								*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + pat_data),
								uPCG_prw);
				
				uWidth += SP_W;
				count++;
			}
			uHeight += SP_H;
		}
		
		g_stPCG_DATA[uPCG_num].Plane = Plane_num;
		g_stPCG_DATA[uPCG_num].Anime_old = Anime_num;
		g_stPCG_DATA[uPCG_num].update = FALSE;	/* 更新済みにする */
	}

#ifdef DEBUG
//	printf("PCG(%d)=(%d,%d)(0x%4x)\n", uPCG_num, g_stPCG_DATA[uPCG_num].x + uWidth, g_stPCG_DATA[uPCG_num].y + uHeight, *(g_stPCG_DATA[uPCG_num].pPatCodeTbl) );
//	KeyHitESC();	/* デバッグ用 */
#endif
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
ST_PCG *PCG_Get_Info(uint8_t uNum)
{
	ST_PCG *ret;

	ret = (ST_PCG *)&g_stPCG_DATA[uNum];	/* 現情報を取得 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_Set_Info(ST_PCG stPCG, uint8_t uNum)
{
	int16_t	ret = 0;

	g_stPCG_DATA[uNum] = stPCG;	/* 更新 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_Load_Data(int8_t *sFileName, uint16_t uPCG_data_ofst,
						ST_PCG stPCG, uint16_t uPCG_num, uint8_t uPCG_Type)
{
	int16_t	ret = 0;
	uint32_t	j;
	uint32_t	uBufSize;

	/* コピー */
	g_stPCG_DATA[uPCG_num] = stPCG;
	
	/* メモリ確保 */
	uBufSize = stPCG.Pat_w * stPCG.Pat_h * stPCG.Pat_AnimeMax;
	
	/* スプライト定義設定 */
	g_stPCG_DATA[uPCG_num].pPatCodeTbl	= (uint32_t*)MyMalloc(sizeof(uint32_t) * uBufSize);	/* 4byte×サイズ分 */
	
	for(j=0; j < uBufSize; j++)
	{
		*(g_stPCG_DATA[uPCG_num].pPatCodeTbl + j)	= SetBGcode(0, 0, 0x00, 0xFF);	/* パターンコードテーブル */
	}
	
	/* スプライト画像データ */
	uBufSize = stPCG.Pat_DataMax;
	
	g_stPCG_DATA[uPCG_num].pPatData		= (uint16_t*)MyMalloc(SP_16_SIZE * uBufSize);		/* 128byte×アニメーション数 */
	
	memset(g_stPCG_DATA[uPCG_num].pPatData, 0, SP_16_SIZE * uBufSize);	/* 0クリア */
	
	/* スプライトデータ読み込み */
	{
		uint32_t uSP_MoveSize;
		FILE *fp;

		fp = fopen( sFileName, "rb" ) ;	/* ファイルオープン */
		if(fp == NULL)
		{
			return -1;
		}
		
		switch(uPCG_Type)
		{
			case 0:
			{
				fseek(fp, (uPCG_data_ofst * SP_16_SIZE), SEEK_SET);	/* 目的データの先頭位置 */
				
				fread( g_stPCG_DATA[uPCG_num].pPatData		/* パターンの格納先 */
					,  SP_16_SIZE							/* 1PCG = メモリサイズ 16x16 */
					,  uBufSize								/* 何パターン使うのか？ */
					,  fp	);								/* ファイルポインタ */
				break;
			}
			case 1:
			{
				uSP_MoveSize = Mdiv2(SP_16_SIZE);	/* 8dot */
				
				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					uint16_t *pPt = g_stPCG_DATA[uPCG_num].pPatData;
					
					fseek( fp, (uPCG_data_ofst * SP_16_SIZE) + (j * uSP_MoveSize), SEEK_SET);	/* 目的データの先頭位置 */
					
					fread( pPt + (uSP_MoveSize * j)
							,  uSP_MoveSize		/* 1PCG = メモリサイズ */
							,  1				/* 何パターン使うのか？ */
							,  fp
							);
				}
				break;
			}
			case 2:
			{
				uSP_MoveSize = Mdiv4(SP_16_SIZE);	/* 4dot */

				for(j=0; j < stPCG.Pat_DataMax; j++)
				{
					uint16_t *pPt = g_stPCG_DATA[uPCG_num].pPatData;
					
					fseek( fp, (uPCG_data_ofst * SP_16_SIZE) + (j * uSP_MoveSize), SEEK_SET);	/* 目的データの先頭位置 */
					
					fread( pPt + (uSP_MoveSize * j)
							,  uSP_MoveSize		/* 1PCG = メモリサイズ */
							,  1				/* 何パターン使うのか？ */
							,  fp
							);
				}
				break;
			}
			default:
			{
				uSP_MoveSize = SP_16_SIZE;
				break;
			}
		}
		fclose( fp ) ;		/* ファイルクローズ */
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_Load_PAL_Data(int8_t *sFileName, uint16_t uPAL_data_ofst, uint16_t uPAL_Set_block)
{
	int16_t	ret = 0;
	
	FILE *fp;
	uint32_t i;
	uint16_t	pal_dat[ 128 ]; /* パレットデータファイル読み込みバッファ */

	if(uPAL_data_ofst == 0)return -1;
	
	/*--------[ スプライトパレットデータ読み込みと定義 ]--------*/

	fp = fopen( sFileName , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		fseek( fp, 2 * 16 * (uPAL_data_ofst - 1), SEEK_SET);	/* 目的データの先頭位置 */
		
		fread( pal_dat
			,  2		/* 1palet = 2byte */
			,  16		/* 16palet * 16block */
			,  fp
			) ;
		fclose( fp ) ;

		/* スプライトパレットに転送 */
		for( i = 0 ; i < 16 ; i++ )
		{
			_iocs_spalet( (0x80 | (i & 0x0F)), (uPAL_Set_block & 0x0F), pal_dat[i] ) ;
		}
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_PAL_Change(uint16_t uPAL_Set_mode, uint16_t uPAL_Set_block, uint16_t uCol)
{
	int16_t	ret = 0;

#if 1
	volatile uint16_t *PALETTE_ADR = (uint16_t *)0xE82200;
	uint16_t *pPal;
	
	pPal = (uint16_t *)PALETTE_ADR + (Mmul16(uPAL_Set_block) + uPAL_Set_mode);
	*pPal = uCol;
#ifdef DEBUG
//	printf("PCG_PAL_Change[0x%p]=(%d,%d)0x%d\n", pPal, uPAL_Set_mode, uPAL_Set_block, uCol);
//	KeyHitESC();	/* デバッグ用 */
#endif
#else
	/* スプライトパレットに転送 */
	_iocs_spalet( (0x80 | (uPAL_Set_mode & 0x0F)), (uPAL_Set_block & 0x0F), uCol) ;
#endif
	
	return ret;
}

#endif	/* BIOS_PCG_C */

