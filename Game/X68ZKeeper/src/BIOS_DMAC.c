#ifndef	BIOS_DMAC_C
#define	BIOS_DMAC_C

#include <usr_macro.h>
#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>

#include "BIOS_DMAC.h"
#include "IF_Graphic.h"

int16_t DMAC_INIT(void);
int16_t DMAC_COPY(void *, void *, size_t);
int16_t DMAC_COPY_A(void *, struct _chain *, size_t);
int16_t DMAC_Image_To_VRAM(uint8_t, int16_t, int16_t);
int16_t DMAC_Image_To_VRAM_X(uint8_t, int16_t, int16_t);
/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t DMAC_INIT(void)
{
	int16_t ret = 0;

//	uint8_t *DMA_DCR;
//	uint8_t *DMA_GCR;

    ret = _iocs_dmamode();
	if(ret != 0u)
    {
        printf("DMAは実行中(0x%x)\n", ret);
        return -1;
    }

//	DMA_DCR = (uint8_t *)0xE84084u;
//	*DMA_DCR = Mbset(*DMA_DCR, ((uint8_t)0b11000000), ((uint8_t)0b10000000));	/* XRM(bit7,6) 10: */

//	DMA_GCR = (uint8_t *)0xE840FFu;
//	*DMA_GCR = Mbset(*DMA_GCR, ((uint8_t)0b0011), ((uint8_t)0b0011));           /* BR(bit1,0) 11:6.25% */

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t DMAC_COPY(void *pDst, void *pSrc, size_t nSize)
{
	int16_t ret = 0;

	int32_t 	nMode;

    ret = _iocs_dmamode();
	if(ret != 0u)
    {
        printf("DMAC_COPY:DMAは実行中(0x%x)\n", ret);
 		//KeyHitESC();	/* デバッグ用 */
        return -1;
    }
    else{
    }
    /* DMAモード設定 */
	nMode = 0b00000001;
	/*		  ||||||||      	*/
	/*		  ||||||++------	(bit1,0)	DAC(addr2)	00:none	01:inc	10:dec	*/
	/*		  ||||++--------	(bit3,2)	MAC(addr1)	00:none	01:inc	10:dec	*/
	/*		  |+++----------	(bit6,5,4)	(Reserved)	 0		        		*/
	/*		  +-------------	(bit7)		転送方向	  0:addr1->addr2	1:addr2->addr1	*/
	_iocs_dmamove(  pSrc,				/* addr1 */
		        	pDst,				/* addr2 */
			        nMode,				/* mode */
			        (int32_t)nSize);	/* len */

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t DMAC_COPY_A(void *pDst, struct _chain *pSrc, size_t nSize)
{
	int16_t ret = 0;
#if 0
	int32_t 	nMode;

    ret = _iocs_dmamode();
	if(ret != 0u)
    {
        printf("DMAC_COPY:DMAは実行中(0x%x)\n", ret);
 		//KeyHitESC();	/* デバッグ用 */
        return -1;
    }
    else{
    }
	
	/* DMAモード設定 */
	nMode = 0b00000001;
	/*		  ||||||||      	*/
	/*		  ||||||++------	(bit1,0)	DAC(addr)	00:none	01:inc	10:dec	*/
	/*		  ||||++--------	(bit3,2)	MAC(tbl)	00:none	01:inc	10:dec	*/
	/*		  |+++----------	(bit6,5,4)	(Reserved)	 0		        		*/
	/*		  +-------------	(bit7)		転送方向	  0:tbl->addr	1:addr->tbl	*/
	/* アレイチェイン */
	_iocs_dmamov_a(	pSrc,				/* tbl */
					pDst,				/* addr */
					nMode,				/* mode */
					(int32_t)nSize);	/* len */
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
int16_t DMAC_Image_To_VRAM(uint8_t bCGNum, int16_t Pos_y, int16_t Move)
{
	int16_t ret = 0;

	int32_t 	nMode;
	int32_t 	nCount;

	uint16_t	*pSrcGR;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	uSize8x;

	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	int32_t		nSize;
	struct _chain2 tblVRAM[32];

    ret = _iocs_dmamode();
	if(ret != 0u)
    {
#ifdef DEBUG
        printf("DMAC_Image_To_VRAM:DMAは実行中(0x%x)\n", ret);
 		//KeyHitESC();	/* デバッグ用 */
#endif
        return -1;
    }

	/* ソース */
	pSrcGR = (uint16_t *)Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8の倍数 */
	pSrcGR += (uSize8x * Pos_y);	/* 高さ分ポインタ移動 */

#ifdef DEBUG
//	printf("DMAC_Image_To_VRAM(%d,0x%p) = %d\n", bCGNum, pSrcGR, uSize8x);
//	KeyHitESC();	/* デバッグ用 */
#endif

	/* VRAM */
	ulGR_H	= 0xC00000;	/* Screen0 */
	nSize	= Mmul2(WIDTH);

	for(nCount = 0; nCount < Move; nCount++)
	{
		ulPoint	= Mmul1024(Pos_y + nCount) + Mmul2(0);
		tblVRAM[nCount].addr	= (void *)(ulGR_H + ulPoint);		/* アドレス */
		tblVRAM[nCount].len		= (uint16_t)nSize;					/* サイズ X軸方向の幅分 */
		if(nCount == (Move-1))
		{
			tblVRAM[Move-1].next	= 0;	/* 終了アドレス */
		}
		else{
			tblVRAM[nCount].next	= (void *)&tblVRAM[nCount+1];	/* 次のアドレス */
		}
	}

    /* DMAモード設定 */
	nMode = 0b10000101;
	/*		  ||||||||      	*/
	/*		  ||||||++------	(bit1,0)	DAC(addr)	00:none	01:inc	10:dec	*/
	/*		  ||||++--------	(bit3,2)	MAC(tbl)	00:none	01:inc	10:dec	*/
	/*		  |+++----------	(bit6,5,4)	(Reserved)	 0		        		*/
	/*		  +-------------	(bit7)		転送方向	  0:tbl->addr	1:addr->tbl	*/
	/* リンクアレイチェイン */
	_iocs_dmamov_l(	&tblVRAM[0],		/* tbl */
					pSrcGR,				/* addr */
					nMode);				/* mode */

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t DMAC_Image_To_VRAM_X(uint8_t bCGNum, int16_t Pos_X, int16_t Move)
{
	int16_t ret = 0;

	int32_t 	nMode;
	int32_t 	nCount;

	uint16_t	*pSrcGR;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	uSize8x;

	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	int32_t		nSize;
	struct _chain2 tblVRAM[256];

    ret = _iocs_dmamode();
	if(ret != 0u)
    {
#ifdef DEBUG
        printf("DMAC_Image_To_VRAM:DMAは実行中(0x%x)\n", ret);
 		//KeyHitESC();	/* デバッグ用 */
#endif
        return -1;
    }

	/* ソース */
	pSrcGR = (uint16_t *)Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8の倍数 */
	pSrcGR += ((Pos_X / Move) * Move) * 256;	/* ポインタ移動 */

#ifdef DEBUG
//	printf("DMAC_Image_To_VRAM()%d\n", Pos_X);
//	KeyHitESC();	/* デバッグ用 */
#endif

	/* VRAM */
	ulGR_H	= 0xC80000;	/* Screen1 */
	nSize	= Mmul2(Move);

	for(nCount = 0; nCount < 256; nCount++)
	{
		ulPoint	= Mmul1024(nCount) + Mmul2(Pos_X);
		tblVRAM[nCount].addr	= (void *)(ulGR_H + ulPoint);		/* アドレス */
		tblVRAM[nCount].len		= (uint16_t)nSize;					/* サイズ X軸方向の幅分 */
		if(nCount == 255)
		{
			tblVRAM[255].next	= 0;	/* 終了アドレス */
		}
		else{
			tblVRAM[nCount].next	= (void *)&tblVRAM[nCount+1];	/* 次のアドレス */
		}
	}

    /* DMAモード設定 */
	nMode = 0b10000101;
	/*		  ||||||||      	*/
	/*		  ||||||++------	(bit1,0)	DAC(addr)	00:none	01:inc	10:dec	*/
	/*		  ||||++--------	(bit3,2)	MAC(tbl)	00:none	01:inc	10:dec	*/
	/*		  |+++----------	(bit6,5,4)	(Reserved)	 0		        		*/
	/*		  +-------------	(bit7)		転送方向	  0:tbl->addr	1:addr->tbl	*/
	/* リンクアレイチェイン */
	_iocs_dmamov_l(	&tblVRAM[0],		/* tbl */
					pSrcGR,				/* addr */
					nMode);				/* mode */

	return ret;
}

#endif	/* BIOS_DMAC_C */