#ifndef	OUTPUT_TEXT_C
#define	OUTPUT_TEXT_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "OutPut_Text.h"

#define	TAB_SIZE	(4)

/* 関数のプロトタイプ宣言 */
void Message_Num(void *, SI, SI, US, UC);
SI BG_TextPut(SC *, SI, SI);
SI BG_PutToText(SI, SI, SI, SI, UC);
SI BG_TimeCounter(UI, US, US);
SI BG_Number(UI, US, US);

/* 関数 */

void Message_Num(void *pNum, SI x, SI y, US nCol, UC mode)
{
	char str[64];

	switch(mode){
	case MONI_Type_UI:
		{
			UI *num;
			num = (UI *)pNum;
			sprintf(str, "%d", *num);
		}
		break;
	case MONI_Type_SI:
		{
			SI *num;
			num = (SI *)pNum;
			sprintf(str, "%d", *num);
		}
		break;
	case MONI_Type_US:
		{
			US *num;
			num = (US *)pNum;
			sprintf(str, "%d", *num);
		}
		break;
	case MONI_Type_SS:
		{
			SS *num;
			num = (SS *)pNum;
			sprintf(str, "%d", *num);
		}
		break;
	case MONI_Type_UC:
		{
			UC *num;
			num = (UC *)pNum;
			sprintf(str, "%d", *num);
		}
		break;
	case MONI_Type_SC:
		{
			SC *num;
			num = (SC *)pNum;
			sprintf(str, "%d", *num);
		}
		break;
	case MONI_Type_FL:
		{
			FL *num;
			num = (FL *)pNum;
			sprintf(str, "%f", *num);
		}
		break;
	case MONI_Type_PT:
		{
			sprintf(str, "0x%p", pNum);
		}
		break;
	default:
		{
			US *num;
			num = (US *)pNum;
			sprintf(str, "%d", *num);
		}
		break;
	}
	B_PUTMES( nCol, x, y, 10-1, str);
}

SI BG_TextPut(SC *sString, SI x, SI y)
{
	US *BG_TEXT_HEAD = (US *)0xEB8800;
	US *BG_NUM_HEAD  = (US *)0xEB8600;
	UC *T0_HEAD = (UC *)0xE00000;
	UC *T1_HEAD = (UC *)0xE20000;
	UC *T2_HEAD = (UC *)0xE40000;
	UC *T3_HEAD = (UC *)0xE60000;
	US	*pStPAT;
	UC	*pDst0;
	UC	*pDst1;
	UC	*pDst2;
	UC	*pDst3;
	SI ret = 0;

	while(*sString != 0)
	{
		switch(*sString)
		{
			case 0x09:	/* HT(水平タブ) */
			{
				x += 32;		/* 4TAB分動かす */
				break;
			}
			case 0x0a:	/* LF(改行) */
			{
				break;
			}
			case 0x0d:	/* CR(復帰) */
			{
				break;
			}
			case 0x20:	/* SP(空白) */
			{
				x += 8;		/* 一文字分動かす */
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
			default:	/* 表示 */
			{
				SI i, j;
				
				j = (SI)((SC)*sString - '@');
				if(j >= 33)j-=32;	/* 小文字を大文字化 */
				pStPAT = BG_TEXT_HEAD + (US)(0x10 * j);

				pDst0   = T0_HEAD + (y * 0x80) + (x/8);
				pDst1   = T1_HEAD + (y * 0x80) + (x/8);
				pDst2   = T2_HEAD + (y * 0x80) + (x/8);
				pDst3   = T3_HEAD + (y * 0x80) + (x/8);
				
				for(i=0; i < 8; i++)
				{
					US nTmp[4];
					US nBuff[4];
					
					nTmp[0] = *pStPAT;	/* 16bit(0,1,2,3) -> 4dot分抽出 */
					pStPAT++;
					nTmp[1] = *pStPAT;	/* 16bit(4,5,6,7) -> 4dot分抽出 */
					pStPAT++;
					/* 8bit(PCG) to 1bit(Text-VRAM) */	/* がぶがぶさん、GIMONSさん、Shiroh Suzukiさんのアドバイス */
					nBuff[0] = (nTmp[0] & 0b0001000000000000) >> 9;
					nBuff[1] = (nTmp[0] & 0b0000000100000000) >> 6;
					nBuff[2] = (nTmp[0] & 0b0000000000010000) >> 3;
					nBuff[3] = (nTmp[0] & 0b0000000000000001);
					*pDst0 = (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0001000000000000) >> 9;
					nBuff[1] = (nTmp[1] & 0b0000000100000000) >> 6;
					nBuff[2] = (nTmp[1] & 0b0000000000010000) >> 3;
					nBuff[3] = (nTmp[1] & 0b0000000000000001);
					*pDst0 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);
					
					nBuff[0] = (nTmp[0] & 0b0010000000000000) >> 10;
					nBuff[1] = (nTmp[0] & 0b0000001000000000) >> 7;
					nBuff[2] = (nTmp[0] & 0b0000000000100000) >> 4;
					nBuff[3] = (nTmp[0] & 0b0000000000000010) >> 1;
					*pDst1 = (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0010000000000000) >> 10;
					nBuff[1] = (nTmp[1] & 0b0000001000000000) >> 7;
					nBuff[2] = (nTmp[1] & 0b0000000000100000) >> 4;
					nBuff[3] = (nTmp[1] & 0b0000000000000010) >> 1;
					*pDst1 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);

					nBuff[0] = (nTmp[0] & 0b0100000000000000) >> 11;
					nBuff[1] = (nTmp[0] & 0b0000010000000000) >> 8;
					nBuff[2] = (nTmp[0] & 0b0000000001000000) >> 5;
					nBuff[3] = (nTmp[0] & 0b0000000000000100) >> 2;
					*pDst2 = (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b0100000000000000) >> 11;
					nBuff[1] = (nTmp[1] & 0b0000010000000000) >> 8;
					nBuff[2] = (nTmp[1] & 0b0000000001000000) >> 5;
					nBuff[3] = (nTmp[1] & 0b0000000000000100) >> 2;
					*pDst2 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);

					nBuff[0] = (nTmp[0] & 0b1000000000000000) >> 12;
					nBuff[1] = (nTmp[0] & 0b0000100000000000) >> 9;
					nBuff[2] = (nTmp[0] & 0b0000000010000000) >> 6;
					nBuff[3] = (nTmp[0] & 0b0000000000001000) >> 3;
					*pDst3 = (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]) << 4;
					nBuff[0] = (nTmp[1] & 0b1000000000000000) >> 12;
					nBuff[1] = (nTmp[1] & 0b0000100000000000) >> 9;
					nBuff[2] = (nTmp[1] & 0b0000000010000000) >> 6;
					nBuff[3] = (nTmp[1] & 0b0000000000001000) >> 3;
					*pDst3 |= (nBuff[0] | nBuff[1] | nBuff[2] | nBuff[3]);

					pDst0 += 0x80;
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
				x += 8;		/* 一文字分動かす */
			}
		}
		sString++;
	}	
	return ret;
}

SI BG_PutToText(SI nPatNum, SI x, SI y, SI mode, UC bClr)
{
	US *BG_HEAD = (US *)0xEB8000;
	UC *T0_HEAD = (UC *)0xE00000;
	UC *T1_HEAD = (UC *)0xE20000;
	UC *T2_HEAD = (UC *)0xE40000;
	UC *T3_HEAD = (UC *)0xE60000;
	US	*pStPAT;
	UC	*pDst0;
	UC	*pDst1;
	UC	*pDst2;
	UC	*pDst3;
	SI	ret = 0;
	SI	i, j;
	US	BitMask[4][4] = { 
			0x1000, 0x2000, 0x4000, 0x8000,
			0x0100, 0x0200, 0x0400, 0x0800,
			0x0010, 0x0020, 0x0040, 0x0080,
			0x0001, 0x0002, 0x0004, 0x0008
	};
	
	switch(mode)
	{
	case BG_H_rev:
	case BG_VH_rev:
		pStPAT = BG_HEAD + (US)(0x10 * (nPatNum + 1));
		break;
	case BG_Normal:
	case BG_V_rev:
	default:
		pStPAT = BG_HEAD + (US)(0x10 * nPatNum);
		break;
	}

	pDst0   = T0_HEAD + (y * 0x80) + (x/8);
	pDst1   = T1_HEAD + (y * 0x80) + (x/8);
	pDst2   = T2_HEAD + (y * 0x80) + (x/8);
	pDst3   = T3_HEAD + (y * 0x80) + (x/8);
	
	for(i=0; i < 8; i++)
	{
		US nTmp[2];
		US nBuff[8];
		
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
			UI	bitshift;

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

			if(j < 4)	/* 上位4ドット */
			{
				/* P0 */
				*pDst0 |= (((nTmp[0] & BitMask[j%4][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P1 */
				*pDst1 |= (((nTmp[0] & BitMask[j%4][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P2 */
				*pDst2 |= (((nTmp[0] & BitMask[j%4][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P3 */
				*pDst3 |= (((nTmp[0] & BitMask[j%4][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
			}
			else		/* 下位4ドット */
			{
				/* P0 */
				*pDst0 |= (((nTmp[1] & BitMask[j%4][0]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P1 */
				*pDst1 |= (((nTmp[1] & BitMask[j%4][1]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P2 */
				*pDst2 |= (((nTmp[1] & BitMask[j%4][2]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
				/* P3 */
				*pDst3 |= (((nTmp[1] & BitMask[j%4][3]) != 0u) << bitshift);	/* 0bitシフトの結果が前回値となるマイコンあるので注意 */
			}
		}
		pDst0 += 0x80;
		pDst1 += 0x80;
		pDst2 += 0x80;
		pDst3 += 0x80;
	}
	return ret;
}

SI BG_TimeCounter(UI unTime, US x, US y)
{
	SI ret = 0;
	UI un100, un10, un1;
	US u100_view[2], u10_view[2], u1_view[2];

	if(unTime >= 1000)unTime = 999;
	
	un100 = ((unTime % 1000) / 100);
	un10 = ((unTime % 100) / 10);
	un1 = (unTime % 10);

	/* 表示 */
	u100_view[0] = 128 + (US)(un100 * 2);
	u100_view[1] = u100_view[0]+1;
	u10_view[0] = 128 + (US)(un10 * 2);
	u10_view[1] = u10_view[0] + 1;
	u1_view[0] = 128 + (US)(un1 * 2);
	u1_view[1] = u1_view[0] + 1;
	
	BG_PutToText( u100_view[0], x-(BG_WIDTH<<1),		y,			BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText( u100_view[1], x-(BG_WIDTH<<1),	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText(  u10_view[0], x-BG_WIDTH,				y,			BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(  u10_view[1], x-BG_WIDTH,			y+BG_HEIGHT,	BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(   u1_view[0], x,						y,			BG_Normal, TRUE);	/*   1桁目 */
	BG_PutToText(   u1_view[1], x,					y+BG_HEIGHT,	BG_Normal, TRUE);	/*   1桁目 */
	
	return ret;
}

SI BG_Number(UI unNum, US x, US y)
{
	SI ret = 0;
	UC ucDigit[10];
	UI unDigit, unDigitCal;
	static UC ucOverFlow = FALSE;
	
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
	
	BG_TextPut(ucDigit, (SI)x, (SI)y);
	
	return ret;
}

#endif	/* OUTPUT_TEXT_C */

