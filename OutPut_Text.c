#ifndef	OUTPUT_TEXT_C
#define	OUTPUT_TEXT_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "OutPut_Text.h"

#define	TAB_SIZE	(4)

/* 関数のプロトタイプ宣言 */
void Message_Num(void *, SS, SS, US, UC, UC *);
SS BG_TextPut(SC *, SS, SS);
SS BG_PutToText(SS, SS, SS, SS, UC);
SS BG_TimeCounter(UI, US, US);
SS BG_Number(UI, US, US);
SS Text_To_Text(US, SS, SS, UC, UC *);
SS PutTextInfo(ST_TEXTINFO);

/* 関数 */

void Message_Num(void *pNum, SS x, SS y, US nCol, UC mode, UC *sFormat)
{
	char str[64];
	volatile US *CRTC_480 = (US *)0xE80480u;	/* CRTC動作ポート */

	memset(str, 0, sizeof(str));

	switch(mode){
	case MONI_Type_UI:
		{
			UI *num;
			num = (UI *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SI:
		{
			SI *num;
			num = (SI *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_US:
		{
			US *num;
			num = (US *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SS:
		{
			SS *num;
			num = (SS *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_UC:
		{
			UC *num;
			num = (UC *)pNum;
			sprintf(str, sFormat, *num);
		}
		break;
	case MONI_Type_SC:
		{
			SC *num;
			num = (SC *)pNum;
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
			US *num;
			num = (US *)pNum;
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

SS BG_TextPut(SC *sString, SS x, SS y)
{
	US *BG_HEAD 		= (US *)0xEB8000;
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
	SS ret = 0;
	US	BitMask[4][4] = { 
			0x1000, 0x2000, 0x4000, 0x8000,
			0x0100, 0x0200, 0x0400, 0x0800,
			0x0010, 0x0020, 0x0040, 0x0080,
			0x0001, 0x0002, 0x0004, 0x0008
	};

	while(*sString != 0)
	{
		switch(*sString)
		{
			case 0x09:	/* HT(水平タブ) */
			{
				x += 32;		/* 4TAB分動かす */
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
				SS i, j, k;
				
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
					j = (SS)((SC)*sString - '@');
					pStPAT = BG_TEXT_HEAD + (US)(0x10 * j);
				}

				k = (y * 0x80) + (x >> 3);
				if( (k < 0) || ((k+8) > 0x1FFFF) ) break;
				pDst0   = T0_HEAD + k;
				pDst1   = T1_HEAD + k;
				pDst2   = T2_HEAD + k;
				pDst3   = T3_HEAD + k;
				
				for(i=0; i < 8; i++)
				{
#if 1
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
#else
					US nTmp[2];

					/* 8bit(PCG) to 1bit(Text-VRAM) */
					for(j=0; j < 8; j++)
					{
						UI	bitshift;

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
					pDst0 += 0x80;
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

SS BG_PutToText(SS nPatNum, SS x, SS y, SS mode, UC bClr)
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
	SS	ret = 0;
	SS	i, j, k;
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

	k = (y * 0x80) + (x >> 3);
	pDst0   = T0_HEAD + k;
	pDst1   = T1_HEAD + k;
	pDst2   = T2_HEAD + k;
	pDst3   = T3_HEAD + k;
	
	for(i=0; i < 8; i++)
	{
		US nTmp[2];
		
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

SS BG_TimeCounter(UI unTime, US x, US y)
{
	SS ret = 0;
	UI un100=0, un10=0, un1=0;
	US u100_view[2], u10_view[2], u1_view[2];

	if(unTime >= 1000)unTime = 999;
#if 1
	un100 = ((unTime % 1000) / 100);
	un10 = ((unTime % 100) / 10);
	un1 = (unTime % 10);
#endif
	/* 表示 */
	u100_view[0] = 128 + (US)(un100 << 1);
	u100_view[1] = u100_view[0]+1;
	u10_view[0] = 128 + (US)(un10 << 1);
	u10_view[1] = u10_view[0] + 1;
	u1_view[0] = 128 + (US)(un1 << 1);
	u1_view[1] = u1_view[0] + 1;
	
	BG_PutToText( u100_view[0], x-(BG_WIDTH<<1),		y,			BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText( u100_view[1], x-(BG_WIDTH<<1),	y+BG_HEIGHT,	BG_Normal, TRUE);	/* 100桁目 */
	BG_PutToText(  u10_view[0], x-BG_WIDTH,				y,			BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(  u10_view[1], x-BG_WIDTH,			y+BG_HEIGHT,	BG_Normal, TRUE);	/*  10桁目 */
	BG_PutToText(   u1_view[0], x,						y,			BG_Normal, TRUE);	/*   1桁目 */
	BG_PutToText(   u1_view[1], x,					y+BG_HEIGHT,	BG_Normal, TRUE);	/*   1桁目 */
	
	return ret;
}

SS BG_Number(UI unNum, US x, US y)
{
	SS ret = 0;
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
	
	BG_TextPut(ucDigit, x, y);
	
	return ret;
}

/* TEXT-RAMに展開したデータから数字情報を作る */	/* 処理負荷改善@kunichikoさんのアドバイス */
SS Text_To_Text(US uNum, SS x, SS y, UC bLarge, UC *sFormat)
{
	UC	*T0_HEAD = (UC *)0xE00000;
	UC	*T1_HEAD = (UC *)0xE20000;
	UC	*T2_HEAD = (UC *)0xE40000;
	UC	*T3_HEAD = (UC *)0xE60000;
	UC	*pSrc0 = (UC *)0xE07400;
	UC	*pSrc1 = (UC *)0xE27400;
	UC	*pSrc2 = (UC *)0xE47400;
	UC	*pSrc3 = (UC *)0xE67400;
	UC	*T3_END = (UC *)0xE7FFFF;
	UC	*pDst0;
	UC	*pDst1;
	UC	*pDst2;
	UC	*pDst3;
	UC	*pData0;
	UC	*pData1;
	UC	*pData2;
	UC	*pData3;
	UC	data;
	UC	ucDigit[10] = {0};
	UC	*pString;
	SS	i, j, k, size;
	SS	ret = 0;
	
	if( (x < 0) || (y < 0) || (x > 1023) || (y > 1023) ) return -1;
	
	if(bLarge == TRUE)
	{
		pSrc0 += 0x400;
		pSrc1 += 0x400;
		pSrc2 += 0x400;
		pSrc3 += 0x400;
		size = 16;	/* 大 */
		sprintf(ucDigit, sFormat, uNum);
	}
	else
	{
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
				k = (y * 0x80) + (x >> 3);
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
				k = (y * 0x80) + (x >> 3);
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


SS PutTextInfo(ST_TEXTINFO stTextInfo)
{
	SS	ret = 0;

	/* Top Score */
	Text_To_Text(stTextInfo.uScoreMax,		 40,  8, FALSE, "%7d");
	/* Score */
	Text_To_Text(stTextInfo.uScore,			192,  8, FALSE, "%7d");
	/* Time Count */
	Text_To_Text(stTextInfo.uTimeCounter,	112, 24,  TRUE, "%3d");
	/* Speed */
	Text_To_Text(stTextInfo.uVs,			208, 24, FALSE, "%3d");
	/* Gear */
	Text_To_Text(stTextInfo.uShiftPos,		224, 32, FALSE,  "%d");

	return ret;
}

#endif	/* OUTPUT_TEXT_C */

