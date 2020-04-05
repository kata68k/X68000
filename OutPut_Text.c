#ifndef	OUTPUT_TEXT_C
#define	OUTPUT_TEXT_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "OutPut_Text.h"

#define	TAB_SIZE	(4)

/* 関数のプロトタイプ宣言 */
void Message_Num(void *, SI, SI, UC);
SI BG_TextPut(SC *, SI, SI);

/* 関数 */
void Message_Num(void *pNum, SI x, SI y, UC mode)
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
	B_PUTMES( 1, x, y, 10-1, str);
}

SI BG_TextPut(SC *sString, SI x, SI y)
{
	struct _fntbuf	stFont;
	US *BG_TEXT_HEAD = (US *)0xEB8800;
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

	stFont.xl = 8;
	stFont.yl = 8;

#if 1
#else
	B_LOCATE(x, y);
#endif
	while(*sString != 0)
	{
		switch(*sString)
		{
			case 0x09:	/* HT(水平タブ) */
			{
#if 1
#else
				B_LOCATE(x + TAB_SIZE, y);
#endif
				break;
			}
			case 0x0a:	/* LF(改行) */
			{
#if 1
#else
				x = 0;
				B_LOCATE(x, y);
#endif
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
			default:	/* 表示 */
			{
#if 1
//				pStPAT = BG_TEXT_HEAD + (0x20 * (*sString - '@'));
//				memcpy(stFont.buffer, pStPAT, stFont.xl * stFont.yl);
//				TEXTPUT( x, y, &stFont);
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
//					for(j=0; j < 8; j++)
//					{
						US nTmp[4];
						US nBuff[4];
						
						nTmp[0] = *pStPAT;
						pStPAT++;
						nTmp[1] = *pStPAT;
						pStPAT++;
//						nTmp[2] = *pStPAT;
//						pStPAT++;
//						nTmp[4] = *pStPAT;
//						pStPAT++;
						
	
						/* T0 8bit */
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

//					}
					pDst0 += 0x80;
					pDst1 += 0x80;
					pDst2 += 0x80;
					pDst3 += 0x80;
				}
#else
				B_PUTC(*sString);
#endif
				x += 8;		/* 一文字分動かす */
			}
		}
		sString++;
	}	
	
#if 1
#else
	B_CUROFF();	/*カーソルを消します。*/
#endif

	return ret;
}
#endif	/* OUTPUT_TEXT_C */

