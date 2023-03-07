#ifndef	OUTPUT_TEXT_C
#define	OUTPUT_TEXT_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>

#include <usr_macro.h>
#include "Draw.h"
#include "CRTC.h"
#include "FileManager.h"
#include "Graphic.h"
#include "OutPut_Text.h"
#include "PCG.h"

#define	TAB_SIZE	(4)

/* 関数のプロトタイプ宣言 */
void Message_Num(void *, int16_t, int16_t, uint16_t, uint8_t, uint8_t *);
int16_t BG_TextPut(int8_t *, int16_t, int16_t);
int16_t BG_PutToText(int16_t, int16_t, int16_t, int16_t, uint8_t);
int16_t BG_TimeCounter(uint32_t, uint16_t, uint16_t);
int16_t BG_Number(uint32_t, uint16_t, uint16_t);
int16_t Text_To_Text(uint16_t, int16_t, int16_t, uint8_t, uint8_t *);
int16_t Text_To_Text2(uint64_t, int16_t, int16_t, uint8_t, uint8_t *);
int16_t Put_Message_To_Graphic(uint8_t *, uint8_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Message_Num(void *pNum, int16_t x, int16_t y, uint16_t nCol, uint8_t mode, uint8_t *sFormat)
{
	char str[64];
	volatile uint16_t *CRTC_480 = (uint16_t *)0xE80480u;	/* CRTC動作ポート */

	memset(str, 0, sizeof(str));

	switch(mode){
	case MONI_Type_UL:
		{
			uint64_t *num;
			num = (uint64_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SL:
		{
			int64_t *num;
			num = (int64_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_UI:
		{
			uint32_t *num;
			num = (uint32_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SI:
		{
			int32_t *num;
			num = (int32_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_US:
		{
			uint16_t *num;
			num = (uint16_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SS:
		{
			int16_t *num;
			num = (int16_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_UC:
		{
			uint8_t *num;
			num = (uint8_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SC:
		{
			int8_t *num;
			num = (int8_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_FL:
		{
			FL *num;
			num = (FL *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_PT:
		{
			sprintf(str, "0x%p", pNum);
		}
		break;
	default:
		{
			uint16_t *num;
			num = (uint16_t *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	}
	//BG_TextPut(str, x << 4, y  << 4);
	//Text_To_Text(atoi(str), x << 4, y << 4, FALSE);
	if((*CRTC_480 & 0x02u) == 0u)	/* クリア実行中でない */
	{
		B_PUTMES( nCol, x, y, 64, str);	/* 高速クリアの処理を阻害している */
		//B_LOCATE(x, y);
		//B_PRINT(str);
		//B_CUROFF();
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t BG_TextPut(int8_t *sString, int16_t x, int16_t y)
{
	uint16_t *BG_HEAD		= (uint16_t *)0xEB8000;
	uint16_t *BG_TEXT_HEAD	= (uint16_t *)0xEB8800;
//	uint16_t *BG_NUM_HEAD	= (uint16_t *)0xEB8600;
	uint8_t *T0_HEAD = (uint8_t *)0xE00000;
	uint8_t *T1_HEAD = (uint8_t *)0xE20000;
	uint8_t *T2_HEAD = (uint8_t *)0xE40000;
	uint8_t *T3_HEAD = (uint8_t *)0xE60000;
	uint16_t *pStPAT;
	uint8_t *pDst0;
	uint8_t *pDst1;
	uint8_t *pDst2;
	uint8_t *pDst3;
	int16_t ret = 0;
#if 1
#else
	uint16_t	BitMask[4][4] = { 
			0x1000, 0x2000, 0x4000, 0x8000,
			0x0100, 0x0200, 0x0400, 0x0800,
			0x0010, 0x0020, 0x0040, 0x0080,
			0x0001, 0x0002, 0x0004, 0x0008
	};
#endif
	
	while(*sString != 0)
	{
		switch(*sString)
		{
			case 0x09:	/* HT(水平タブ) */
			{
				x += 32;		/* TAB(4)分動かす(8dot x 4word) */
				break;
			}
			case 0x30:	/* 0 */
			case 0x31:	/* 1 */
			case 0x32:	/* 2 */
			case 0x33:	/* 3 */
			case 0x34:	/* 4 */
			case 0x35:	/* 5 */
			case 0x36:	/* 6 */
			case 0x37:	/* 7 */
			case 0x38:	/* 8 */
			case 0x39:	/* 9 */
			{
				BG_PutToText(*sString, x, y, BG_Normal, TRUE);
				x += 8;		/* 一文字分動かす */
				break;
			}
			case 0x20:	/* SP(空白) */
			case 0x2D:	/* -(マイナス) */
			case 0x40:	/* @ */
			case 0x41:	/* A */
			case 0x42:	/* B */
			case 0x43:	/* C */
			case 0x44:	/* D */
			case 0x45:	/* E */
			case 0x46:	/* F */
			case 0x47:	/* G */
			case 0x48:	/* H */
			case 0x49:	/* I */
			case 0x4A:	/* J */
			case 0x4B:	/* K */
			case 0x4C:	/* L */
			case 0x4D:	/* M */
			case 0x4E:	/* N */
			case 0x4F:	/* O */
			case 0x50:	/* P */
			case 0x51:	/* Q */
			case 0x52:	/* R */
			case 0x53:	/* S */
			case 0x54:	/* T */
			case 0x55:	/* U */
			case 0x56:	/* V */
			case 0x57:	/* W */
			case 0x58:	/* X */
			case 0x59:	/* Y */
			case 0x5A:	/* X */
			{
				uint32_t i, j, k;
				
				if(*sString == 0x20)		/* SP(空白) */
				{
					pStPAT = BG_HEAD;
					
				}
				else if(*sString == 0x2D)	/* -(マイナス) */
				{
					pStPAT = BG_TEXT_HEAD;
				}
				else	/* アルファベット */
				{
					j = (uint32_t)((int8_t)*sString - '@');
					pStPAT = BG_TEXT_HEAD + (uint16_t)(0x10 * j);
				}

				k = Mmul128(y) + Mdiv8(x);	/* 8bit */
				if( (k < 0) || ((k+8) > 0x1FFFF) ) break;
				pDst0   = T0_HEAD + k;
				pDst1   = T1_HEAD + k;
				pDst2   = T2_HEAD + k;
				pDst3   = T3_HEAD + k;
				
				for(i=0; i < 8; i++)
				{
#if 1
					uint16_t nTmp[4];
					uint16_t nBuff[4];
					
					nTmp[0] = *pStPAT;	/* 16bit(0,1,2,3) -> 4dot分抽出 */
					pStPAT++;
					nTmp[1] = *pStPAT;	/* 16bit(4,5,6,7) -> 4dot分抽出 */
					pStPAT++;
					/* 8bit(PCG) to 1bit(Text-VRAM) */	/* がぶがぶさん、GIMONSさん、Shiroh Suzukiさんのアドバイス */
					nBuff[0] = (nTmp[0] & 0b0001000000000000) >> 9;
					nBuff[1] = (nTmp[0] & 0b0000000100000000) >> 6;
					nBuff[2] = (nTmp[0] & 0b0000000000010000) >> 3;
					nBuff[3] = (nTmp[0] & 0b0000000000000001);
					*pDst0 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0001000000000000) >> 9;
					nBuff[1] = (nTmp[1] & 0b0000000100000000) >> 6;
					nBuff[2] = (nTmp[1] & 0b0000000000010000) >> 3;
					nBuff[3] = (nTmp[1] & 0b0000000000000001);
					*pDst0 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);
					
					nBuff[0] = (nTmp[0] & 0b0010000000000000) >> 10;
					nBuff[1] = (nTmp[0] & 0b0000001000000000) >> 7;
					nBuff[2] = (nTmp[0] & 0b0000000000100000) >> 4;
					nBuff[3] = (nTmp[0] & 0b0000000000000010) >> 1;
					*pDst1 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0010000000000000) >> 10;
					nBuff[1] = (nTmp[1] & 0b0000001000000000) >> 7;
					nBuff[2] = (nTmp[1] & 0b0000000000100000) >> 4;
					nBuff[3] = (nTmp[1] & 0b0000000000000010) >> 1;
					*pDst1 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);

					nBuff[0] = (nTmp[0] & 0b0100000000000000) >> 11;
					nBuff[1] = (nTmp[0] & 0b0000010000000000) >> 8;
					nBuff[2] = (nTmp[0] & 0b0000000001000000) >> 5;
					nBuff[3] = (nTmp[0] & 0b0000000000000100) >> 2;
					*pDst2 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0100000000000000) >> 11;
					nBuff[1] = (nTmp[1] & 0b0000010000000000) >> 8;
					nBuff[2] = (nTmp[1] & 0b0000000001000000) >> 5;
					nBuff[3] = (nTmp[1] & 0b0000000000000100) >> 2;
					*pDst2 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);

					nBuff[0] = (nTmp[0] & 0b1000000000000000) >> 12;
					nBuff[1] = (nTmp[0] & 0b0000100000000000) >> 9;
					nBuff[2] = (nTmp[0] & 0b0000000010000000) >> 6;
					nBuff[3] = (nTmp[0] & 0b0000000000001000) >> 3;
					*pDst3 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b1000000000000000) >> 12;
					nBuff[1] = (nTmp[1] & 0b0000100000000000) >> 9;
					nBuff[2] = (nTmp[1] & 0b0000000010000000) >> 6;
					nBuff[3] = (nTmp[1] & 0b0000000000001000) >> 3;
					*pDst3 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);
#else
					uint16_t nTmp[2];

					/* 8bit(PCG) to 1bit(Text-VRAM) */
					for(j=0; j < 8; j++)
					{
						uint32_t	bitshift;

						bitshift = j;

						k = j % 4;
						if(j < 4)	/* 上位4ドット */
						{
							/* P0 */
							*pDst0 |= (((nTmp[0] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P1 */
							*pDst1 |= (((nTmp[0] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P2 */
							*pDst2 |= (((nTmp[0] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P3 */
							*pDst3 |= (((nTmp[0] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
						}
						else		/* 下位4ドット */
						{
							/* P0 */
							*pDst0 |= (((nTmp[1] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P1 */
							*pDst1 |= (((nTmp[1] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P2 */
							*pDst2 |= (((nTmp[1] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
							/* P3 */
							*pDst3 |= (((nTmp[1] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
						}
					}
#endif
					pDst0 += 0x80;	/* byte x 64 x 2 = 1024 dot */
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
				x += 8;		/* 一文字分動かす */
			}
			case 0x0a:	/* LF(改行) */
			case 0x0d:	/* CR(復帰) */
			default:	/* 表示しない */
			{
				break;
			}
		}
		sString++;
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
int16_t BG_PutToText(int16_t nPatNum, int16_t x, int16_t y, int16_t mode, uint8_t bClr)
{
	uint16_t *BG_HEAD = (uint16_t *)0xEB8000;
	uint8_t *T0_HEAD = (uint8_t *)0xE00000;
	uint8_t *T1_HEAD = (uint8_t *)0xE20000;
	uint8_t *T2_HEAD = (uint8_t *)0xE40000;
	uint8_t *T3_HEAD = (uint8_t *)0xE60000;
	uint16_t	*pStPAT;
	uint8_t	*pDst0;
	uint8_t	*pDst1;
	uint8_t	*pDst2;
	uint8_t	*pDst3;
	int16_t	ret = 0;
	uint16_t	i, j, k;
	uint16_t	BitMask[4][4] = { 
			0x1000, 0x2000, 0x4000, 0x8000,
			0x0100, 0x0200, 0x0400, 0x0800,
			0x0010, 0x0020, 0x0040, 0x0080,
			0x0001, 0x0002, 0x0004, 0x0008
	};
	
	switch(mode)
	{
	case BG_H_rev:
	case BG_VH_rev:
		pStPAT = BG_HEAD + (uint16_t)(0x10 * (nPatNum + 1));
		break;
	case BG_Normal:
	case BG_V_rev:
	default:
		pStPAT = BG_HEAD + (uint16_t)(0x10 * nPatNum);
		break;
	}

	k = (y * 0x80) + (x >> 3);
	pDst0   = T0_HEAD + k;
	pDst1   = T1_HEAD + k;
	pDst2   = T2_HEAD + k;
	pDst3   = T3_HEAD + k;
	
	for(i=0; i < 8; i++)
	{
		uint16_t nTmp[2];
		
		switch(mode)
		{
		case BG_H_rev:
		case BG_VH_rev:
			pStPAT--;
			nTmp[1] = *pStPAT;	/* 16bit(4,5,6,7) -> 4dot分抽出 */
			pStPAT--;
			nTmp[0] = *pStPAT;	/* 16bit(0,1,2,3) -> 4dot分抽出 */
			break;
		case BG_Normal:
		case BG_V_rev:
		default:
			nTmp[0] = *pStPAT;	/* 16bit(0,1,2,3) -> 4dot分抽出 */
			pStPAT++;
			nTmp[1] = *pStPAT;	/* 16bit(4,5,6,7) -> 4dot分抽出 */
			pStPAT++;
			break;
		}

		/* クリア */
		if(bClr != 0)
		{
			*pDst0 = 0;
			*pDst1 = 0;
			*pDst2 = 0;
			*pDst3 = 0;
		}
		/* 8bit(PCG) to 1bit(Text-VRAM) */
		for(j=0; j < 8; j++)
		{
			uint32_t	bitshift;

			switch(mode)
			{
			case BG_V_rev:
			case BG_VH_rev:
				bitshift = j;
				break;
			case BG_Normal:
			case BG_H_rev:
			default:
				bitshift = (7-j);
				break;
			}
			k = j % 4;
			if(j < 4)	/* 上位4ドット */
			{
				/* P0 */
				*pDst0 |= (((nTmp[0] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P1 */
				*pDst1 |= (((nTmp[0] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P2 */
				*pDst2 |= (((nTmp[0] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P3 */
				*pDst3 |= (((nTmp[0] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
			}
			else		/* 下位4ドット */
			{
				/* P0 */
				*pDst0 |= (((nTmp[1] & BitMask[k][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P1 */
				*pDst1 |= (((nTmp[1] & BitMask[k][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P2 */
				*pDst2 |= (((nTmp[1] & BitMask[k][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P3 */
				*pDst3 |= (((nTmp[1] & BitMask[k][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
			}
		}
		pDst0 += 0x80;
		pDst1 += 0x80;
		pDst2 += 0x80;
		pDst3 += 0x80;
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
int16_t BG_TimeCounter(uint32_t unTime, uint16_t x, uint16_t y)
{
	int16_t ret = 0;
	uint32_t un100=0, un10=0, un1=0;
	uint16_t u100_view[2], u10_view[2], u1_view[2];

	if(unTime >= 1000)unTime = 999;
#if 1
	un100 = ((unTime % 1000) / 100);
	un10 = ((unTime % 100) / 10);
	un1 = (unTime % 10);
#endif
	/* 表示 */
	u100_view[0] = 128 + (uint16_t)(un100 << 1);
	u100_view[1] = u100_view[0]+1;
	u10_view[0] = 128 + (uint16_t)(un10 << 1);
	u10_view[1] = u10_view[0] + 1;
	u1_view[0] = 128 + (uint16_t)(un1 << 1);
	u1_view[1] = u1_view[0] + 1;
	
	BG_PutToText( u100_view[0], x-(BG_WIDTH<<1),		y,			BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText( u100_view[1], x-(BG_WIDTH<<1),	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText(  u10_view[0], x-BG_WIDTH,				y,			BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(  u10_view[1], x-BG_WIDTH,			y+BG_HEIGHT,	BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(   u1_view[0], x,						y,			BG_Normal, TRUE);	/*   1桁目 */
	BG_PutToText(   u1_view[1], x,					y+BG_HEIGHT,	BG_Normal, TRUE);	/*   1桁目 */
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t BG_Number(uint32_t unNum, uint16_t x, uint16_t y)
{
	int16_t ret = 0;
	uint8_t ucDigit[10];
#if 0
	uint32_t unDigit, unDigitCal;
#endif	
	static uint8_t ucOverFlow = FALSE;
	
	if(unNum >= 10000000)
	{
		ucOverFlow = TRUE;
	}
	
	if(ucOverFlow == TRUE)
	{
		unNum = 9999999;
	}
#if 0
	unDigitCal = unNum;
	/* 桁数を計算 */
	while(unDigitCal!= 0u)
	{
		unDigitCal = unDigitCal / 10;
		unDigit++;
	}
#endif	
	sprintf(ucDigit, "%7d", unNum);
	
	BG_TextPut(ucDigit, x, y);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* TEXT-RAMに展開したデータから数字情報を作る */	/* 処理負荷改善@kunichikoさんのアドバイス */
int16_t Text_To_Text(uint16_t uNum, int16_t x, int16_t y, uint8_t bLarge, uint8_t *sFormat)
{
	uint8_t	*T0_HEAD = (uint8_t *)0xE00000;
	uint8_t	*T1_HEAD = (uint8_t *)0xE20000;
	uint8_t	*T2_HEAD = (uint8_t *)0xE40000;
	uint8_t	*T3_HEAD = (uint8_t *)0xE60000;
	uint8_t	*pSrc0;
	uint8_t	*pSrc1;
	uint8_t	*pSrc2;
	uint8_t	*pSrc3;
//	uint8_t	*T3_END = (uint8_t *)0xE7FFFF;
	uint8_t	*pDst0;
	uint8_t	*pDst1;
	uint8_t	*pDst2;
	uint8_t	*pDst3;
	uint8_t	*pData0;
	uint8_t	*pData1;
	uint8_t	*pData2;
	uint8_t	*pData3;
	uint8_t	data;
	uint8_t	ucDigit[10] = {0};
	uint8_t	*pString;
	uint32_t	i, k, size;
	int16_t sx, sy, sAddr;
	int16_t	ret = 0;
	
	if( (x < 0) || (y < 0) || (x >= 1024) || (y >= 1024) ) return -1;

	/* ソースの置き場所 */
	sx = 256;
	sy = 0;
	
	if(bLarge == TRUE)
	{
		sy += 8;	/* 8dot下 */
		sAddr = Mmul128(sy) + Mdiv8(sx);
		
		pSrc0 = T0_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc1 = T1_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc2 = T2_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc3 = T3_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		size = 16;	/* 大 */
		sprintf(ucDigit, sFormat, uNum);
	}
	else
	{
		sAddr = Mmul128(sy) + Mdiv8(sx);
		
		pSrc0 = T0_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc1 = T1_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc2 = T2_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		pSrc3 = T3_HEAD + sAddr;	/* ソースの先頭座標(256,256) */
		size = 8;	/* 小 */
		sprintf(ucDigit, sFormat, uNum);
	}
	pString = &ucDigit[0];
	
	while(*pString != 0)
	{
		switch(*pString)
		{
			case 0x20:	/* SP */
			{
				/* コピー先 */
				k = Mmul128(y) + Mdiv8(x);
				if( (k < 0) || ((k+size) > 0x1FFFF) ) break;
				pDst0 = T0_HEAD + k;
				pDst1 = T1_HEAD + k;
				pDst2 = T2_HEAD + k;
				pDst3 = T3_HEAD + k;
				
				for(i=0; i < size; i++)
				{
					/* 更新 */
					*pDst0 = 0;
					*pDst1 = 0;
					*pDst2 = 0;
					*pDst3 = 0;

					pData0 += 0x80;
					pData1 += 0x80;
					pData2 += 0x80;
					pData3 += 0x80;

					pDst0 += 0x80;
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
				x+=8;
				break;
			}
			case 0x30:	/* 0 */
			case 0x31:	/* 1 */
			case 0x32:	/* 2 */
			case 0x33:	/* 3 */
			case 0x34:	/* 4 */
			case 0x35:	/* 5 */
			case 0x36:	/* 6 */
			case 0x37:	/* 7 */
			case 0x38:	/* 8 */
			case 0x39:	/* 9 */
			{
				/* コピー元 */
				data = *pString - '0';
				pData0 = pSrc0 + data;
				pData1 = pSrc1 + data;
				pData2 = pSrc2 + data;
				pData3 = pSrc3 + data;
				
				/* コピー先 */
				k = Mmul128(y) + Mdiv8(x);
				if( (k < 0) || ((k+size) > 0x1FFFF) ) break;
				pDst0 = T0_HEAD + k;
				pDst1 = T1_HEAD + k;
				pDst2 = T2_HEAD + k;
				pDst3 = T3_HEAD + k;
				
				for(i=0; i < size; i++)
				{
					/* 更新 */
					*pDst0 = *pData0;
					*pDst1 = *pData1;
					*pDst2 = *pData2;
					*pDst3 = *pData3;

					pData0 += 0x80;
					pData1 += 0x80;
					pData2 += 0x80;
					pData3 += 0x80;

					pDst0 += 0x80;
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
				x+=8;
				break;
			}
			default:	/* 表示対象外 */
			{
				x+=8;
				break;
			}
		}
		pString++;
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
/* TEXT-RAMに展開したデータから数字情報を作る */
int16_t Text_To_Text2(uint64_t ulNum, int16_t x, int16_t y, uint8_t bLarge, uint8_t *sFormat)
{
	int16_t	ret = 0;
	int16_t sx, sy;
	uint32_t	i, size, MemSize;
	uint8_t	data;
	uint8_t	ucDigit[20] = {0};
	uint8_t	ucBuffer[136] = {0};
	uint8_t	*pString;

	struct _fntbuf	*p_stTxbuf;

	if( (x < 0) || (y < 0) || (x >= 1024) || (y >= 1024) ) return -1;
	
	/* ソースの置き場所 */
	sx = 0;
	sy = 256;
	
#ifdef DEBUG
//	printf("Text_To_Text2(%ld) 1\n", ulNum);
//	KeyHitESC();	/* デバッグ用 */
#endif
	if(bLarge == TRUE)
	{
		sy += 8;	/* 8dot下 */
		size = Mmul2(BG_HEIGHT);	/* 大 */
		
		sprintf(ucDigit, sFormat, ulNum);
	}
	else
	{
		size = BG_HEIGHT;	/* 小 */
		sprintf(ucDigit, sFormat, ulNum);
	}
	pString = &ucDigit[0];

#ifdef DEBUG
//	printf("Text_To_Text2(%ld) 2\n", ulNum);
//	KeyHitESC();	/* デバッグ用 */
#endif
	
	MemSize = BG_WIDTH * size;
	p_stTxbuf = (struct _fntbuf	*)&ucBuffer[0];
	
	if(p_stTxbuf == NULL)
	{
		return -1;
	}
	p_stTxbuf->xl = BG_WIDTH;
	p_stTxbuf->yl = size;
	
	while(*pString != 0)
	{
		switch(*pString)
		{
			case 0x20:	/* SP */
			{
				/* コピー先 */
				memset(&(p_stTxbuf->buffer[0]), 0, MemSize);
				for(i=0; i < 4; i++)
				{
					/* テキストプレーン */
					_iocs_tcolor(1<<i);
					_iocs_textput(x, y, p_stTxbuf);
				}
				x+=BG_WIDTH;
				break;
			}
			case 0x30:	/* 0 */
			case 0x31:	/* 1 */
			case 0x32:	/* 2 */
			case 0x33:	/* 3 */
			case 0x34:	/* 4 */
			case 0x35:	/* 5 */
			case 0x36:	/* 6 */
			case 0x37:	/* 7 */
			case 0x38:	/* 8 */
			case 0x39:	/* 9 */
			{
				/* 対象データ */
				data = *pString - '0';
				
				for(i=0; i < 4; i++)
				{
					/* テキストプレーン */
					_iocs_tcolor(1<<i);
					/* コピー元 */
					_iocs_textget(sx + (data * BG_WIDTH), sy, p_stTxbuf);
					/* コピー先 */
					_iocs_textput(x, y, p_stTxbuf);
				}

				x+=BG_WIDTH;
				break;
			}
			default:	/* 表示対象外 */
			{
				x+=BG_WIDTH;
				break;
			}
		}
		pString++;
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
int16_t Put_Message_To_Graphic(uint8_t *str, uint8_t bMode)
{
	int16_t	ret = 0;
	
	int16_t	x, y;
	ST_CRT	stCRT = {0};
	
	GetCRT(&stCRT, bMode);	/* 画面情報を取得 */
	
	/* 座標設定 */
	x = stCRT.hide_offset_x;
	y = stCRT.hide_offset_y + 224;
	
	/* メッセージエリア クリア */
	_iocs_window( x, y, x + WIDTH, y + 16);	/* 描画可能枠再設定 */

	Draw_Fill( x, y, x + WIDTH, y + 16, 0x01);	/* Screen0 指定パレットで塗りつぶし */
	
	/* メッセージエリア 描画 */
	PutGraphic_To_Symbol(str, x, y, 0x03);
	
	return ret;
}

#endif	/* OUTPUT_TEXT_C */
