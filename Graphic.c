#ifndef	GRAPHIC_C
#define	GRAPHIC_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include "inc/usr_macro.h"
#include "inc/apicglib.h"
#include "Graphic.h"

#include "CRTC.h"
#include "Draw.h"
#include "EnemyCAR.h"
#include "FileManager.h"
#include "Input.h"
#include "MyCar.h"

/* define定義 */
#define	PIC_FILE_BUF_SIZE	(512*1024)
#define	PIC_WORK_BUF_SIZE	(512*1024)
#define	PIC_R	(3)
#define	PIC_B	(3)
#define	PIC_G	(3)
#define	COLOR_R	(0)
#define	COLOR_B	(1)
#define	COLOR_G	(2)
#define	COLOR_MAX	(PIC_R * PIC_B * PIC_G)
#define	G_COLOR		(0x20)
#define	G_COLOR_SP	(1)	/* 特殊プライオリティとか */

/* グローバル変数 */
uint32_t	g_CG_List_Max	=	0u;
uint8_t	*g_pCG_FileBuf[CG_MAX];
uint16_t	g_CG_ColorCode[CG_MAX][256]	=	{0};
uint8_t	g_CG_MaxColor[CG_MAX][3]	=	{0};

/* グローバル構造体 */
PICIMAGE	g_stPicImage[CG_MAX];
CG_LIST		g_stCG_LIST[CG_MAX];
struct APICGINFO	g_CGInfo[CG_MAX];

/* 関数のプロトタイプ宣言 */
int16_t	Get_CG_FileList_MaxNum(uint32_t *);
uint8_t	*Get_CG_FileBuf(uint8_t);
int16_t	Get_PicImageInfo(uint8_t , uint32_t *, uint32_t *, uint32_t *);
int16_t	Get_PicImagePallet(uint8_t);
int16_t	Set_PicImagePallet(uint8_t);
void CG_File_Load(void);
void G_INIT(void);
void G_HOME(void);
void G_VIEW(uint8_t);
void G_Palette_INIT(void);
void G_Palette(void);
int16_t	G_Stretch_Pict( int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t, int16_t, uint16_t, uint8_t );
int16_t	G_BitBlt(int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t , int16_t , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
int32_t	G_CLR(void);
int16_t	G_CLR_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t);
int16_t	G_CLR_ALL_OFFSC(uint8_t);
int16_t	G_FILL_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t, uint8_t);
int16_t	G_Load(uint8_t, uint16_t, uint16_t, uint16_t);
int16_t	G_Load_Mem(uint8_t, uint16_t, uint16_t, uint16_t);
int16_t	APICG_DataLoad2G(int8_t *, uint64_t, uint64_t, uint16_t);
int16_t	APICG_DataLoad2M(uint8_t , uint64_t, uint64_t, uint16_t, uint16_t *);
int16_t	G_Subtractive_Color(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t, uint32_t);
int16_t	PutGraphic_To_Text(uint8_t , uint16_t , uint16_t );
int16_t	PutGraphic_To_Symbol(uint8_t *, uint16_t , uint16_t , uint16_t );
int16_t	G_Scroll(int16_t, int16_t, uint8_t);

/* 関数 */
int16_t Get_CG_FileList_MaxNum(uint32_t *uMaxNum)
{
	int16_t	ret = 0;
	
	*uMaxNum = g_CG_List_Max;
	
	return ret;
}

uint8_t *Get_CG_FileBuf(uint8_t bNum)
{
	uint8_t *ret = 0;
	
	ret = g_pCG_FileBuf[bNum];
//	printf("Get(%d,0x%p,0x%p)\n", bNum, g_pCG_FileBuf[bNum], ret );
	
	return ret;
}

int16_t Get_PicImageInfo(uint8_t bNum, uint32_t *pWidth, uint32_t *pHeight, uint32_t *pFileSize)
{
	int16_t	ret = 0;
	BITMAPFILEHEADER *pFile;
	BITMAPINFOHEADER *pInfo;
	
	if(g_CG_List_Max <= bNum)
	{
		ret = -1;
	}
	
	pFile = g_stPicImage[bNum].pBMf;
	*pFileSize	= pFile->bfSize;

	pInfo = g_stPicImage[bNum].pBMi;
	*pWidth		= pInfo->biWidth;
	*pHeight	= pInfo->biHeight;

#ifdef DEBUG
//	printf("st 0x%p\n", g_stPicImage );
//	printf("Get(%d,0x%p)\n", bNum, g_stPicImage[bNum].pImageData );
//	printf("Get(%d,0x%p, 0x%p)\n", bNum, g_stPicImage[bNum].pBMf, g_stPicImage[bNum].pBMi);
//	printf("Get(%d)=(x:%d,y:%d) %d\n", bNum, *pWidth, *pHeight, *pFileSize );
#endif
	
	return ret;
}

int16_t Get_PicImagePallet(uint8_t bNum)
{
	int16_t	ret = 0;
	uint32_t	i;

	/* パレットの取得 */
	for(i=0; i<256; i++)
	{
		g_CG_ColorCode[bNum][i] = GPALET( i, -1 );	/* 現在の設置を保存 */
	}
	
	return ret;
}

int16_t Set_PicImagePallet(uint8_t bNum)
{
	int16_t	ret = 0;
	uint32_t	i;
	uint32_t	offset_val;
	static uint32_t offset = 0;
	
	/* パレットの設定 */
	switch(g_stCG_LIST[bNum].ubType)
	{
		case 1:	/* スプライトライク */
		{
			offset_val = G_COLOR * offset * G_COLOR_SP;
			for(i=0; i< (COLOR_MAX * G_COLOR_SP) + 2; i++)
			{
				GPALET( i+offset_val, g_CG_ColorCode[bNum][i] );
			}
			offset++;
			break;
		}
		case 2:	/* テキスト描画 */
		case 3:	/* テキスト描画(グレイスケール) */
		{
			offset_val = 8;
			for(i=0; i< 8; i++)
			{
				TPALET2( i+offset_val, g_CG_ColorCode[bNum][i+1] );
			}
			break;
		}
		default:
		{
			offset_val = 0;
			for(i=0; i<256; i++)
			{
				GPALET( i, g_CG_ColorCode[bNum][i] );
			}
			offset = 0;
			break;
		}
	}
	
	if(offset >= 0x100 / (G_COLOR * G_COLOR_SP))
	{
		offset = 0;
	}
	ret = offset_val;
	
	return ret;
}

void CG_File_Load(void)
{
	uint16_t	i;
	int32_t	FileSize;

	/* グラフィックリスト */
	Load_CG_List("data\\cg\\", "g_list.txt", g_stCG_LIST, &g_CG_List_Max);
	
	for(i = 0; i < g_CG_List_Max; i++)
	{
		int32_t	Size;
		uint32_t	uSize8x = 0;
		BITMAPFILEHEADER *pFile;
		BITMAPINFOHEADER *pInfo;

		/* PICヘッダにメモリ割り当て */
		g_stPicImage[i].pBMf = (BITMAPFILEHEADER*)MyMalloc( FILE_HEADER_SIZE );
		g_stPicImage[i].pBMi = (BITMAPINFOHEADER*)MyMalloc( INFO_HEADER_SIZE );
		pFile = g_stPicImage[i].pBMf;
		pInfo = g_stPicImage[i].pBMi;

		/* PIC画像のファイルサイズを取得 */
		GetFileLength( g_stCG_LIST[i].bFileName, &FileSize );			/* ファイルサイズ取得 */
		if(FileSize <= 0)
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			continue;
		}
		pFile->bfSize = FileSize;		/* ファイルサイズ設定 */

		/* PIC画像の情報を取得 */
		GetFilePICinfo( g_stCG_LIST[i].bFileName, g_stPicImage[i].pBMi );	/* PICヘッダの読み込み */

#ifdef DEBUG
//		printf("Head1(%d,0x%p)=%d\n", i, g_stPicImage[i].pBMf, pFile->bfSize );
//		printf("Head2(%d,0x%p)=(%d,%d)\n", i, g_stPicImage[i].pBMi, pInfo->biWidth, pInfo->biHeight );
//		KeyHitESC();	/* デバッグ用 */
#endif
		/* メモリ展開用のサイズ演算 */
		uSize8x = ((((pInfo->biWidth)+7)/8) * 8);	/* 8の倍数 */
		Size = (pInfo->biHeight) * uSize8x * sizeof(uint16_t);
#ifdef DEBUG
//		printf("Head3(%d)=(%d)\n", i, Size );
//		KeyHitESC();	/* デバッグ用 */
#endif
		/* メモリ確保 */
		g_stPicImage[i].pImageData = NULL;
		g_stPicImage[i].pImageData = (uint16_t*)MyMalloc( Size );	/* メモリの確保 */
		if( g_stPicImage[i].pImageData == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			continue;
		}
		memset(g_stPicImage[i].pImageData, 0, Size);	/* メモリクリア */
#ifdef DEBUG
//		printf("Mem(%d,0x%p)\n", i, g_stPicImage[i].pImageData);
//		KeyHitESC();	/* デバッグ用 */
#endif
		
		/* ファイルをメモリに保存する */
		g_pCG_FileBuf[i] = NULL;
		g_pCG_FileBuf[i] = (uint8_t*)MyMalloc( FileSize );	/* メモリ確保 */
		if( g_pCG_FileBuf[i] == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			continue;
		}
		File_Load( g_stCG_LIST[i].bFileName, (uint8_t *)g_pCG_FileBuf[i], sizeof(uint8_t), FileSize);	/* メモリに読み込み */
#ifdef DEBUG
//		printf("File(%d,0x%p) = %d\n", i, g_pCG_FileBuf[i], FileSize);
//		puts("========================");
//		G_Load(i,0,0,0);
//		if(i==0)
//		{
//			KeyHitESC();	/* デバッグ用 */
//		}
#endif
	}

	for(i = 0; i < g_CG_List_Max; i++)
	{
		uint16_t	*pBuf = NULL;
		uint16_t	*pSrcBuf = NULL;
		uint16_t	*pDstBuf = NULL;
		int32_t	Res;
		uint32_t	uWidth, uHeight;
		uint32_t	uSize8x = 0;
		uint32_t	uAPICG_work_Size;

		BITMAPINFOHEADER *pInfo;
		
		/* メモリサイズ */
		pInfo = g_stPicImage[i].pBMi;
		uWidth	= pInfo->biWidth;
		uHeight	= pInfo->biHeight;
		uSize8x = Mmul8((Mdiv8(uWidth+7)));	/* 8の倍数 */
#ifdef DEBUG
//		printf("Load1(%d,0x%p)=(%d,%d)(%d)\n", i, g_stPicImage[i].pBMi, pInfo->biWidth, pInfo->biHeight, uSize8x );
#endif
		
		/* メモリ確保(画面を模擬) */
		uAPICG_work_Size = Mdiv2(PIC_WORK_BUF_SIZE);
		pSrcBuf = (uint16_t*)MyMalloc( uAPICG_work_Size );
		if( pSrcBuf == NULL )	/* メモリの確保 */
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			continue;
		}
		memset(pSrcBuf, 0, uAPICG_work_Size);	/* メモリクリア */
		pBuf = pSrcBuf;	/* 作業用にアドレスコピー */
#ifdef DEBUG
//		printf("Load2(%d,0x%p, 0x%p)=%d\n", i, pBuf, pSrcBuf, uAPICG_work_Size );
//		KeyHitESC();	/* デバッグ用 */
#endif
		/* メモリ上にPIC画像を展開する */
		Res = APICG_DataLoad2M( i, 0, 0, 0, pBuf);	/* 確保したメモリ上にロード */
		if( Res < 0 )	/* 展開失敗 */
		{
			printf("error(%d):CG File%2d=%s\n", Res, i, g_stCG_LIST[i].bFileName );
#ifdef DEBUG
//			KeyHitESC();	/* デバッグ用 */
#endif
			continue;
		}
		pBuf = pSrcBuf;
#ifdef DEBUG
//		printf("Load3(%d,0x%p)\n", i, pBuf);
//		KeyHitESC();	/* デバッグ用 */
#endif
		
#if 0	/* 画面に表示 */
		{
			uint16_t	*pDstGR = NULL;
			uint32_t	dx = 0, dy = 0;

//			memset((uint16_t*)0xC00000, 0x42, 0x80000);	/* デバッグ用 */
			
			for(y=0; y < uHeight; y++)
			{
				pDstGR = (uint16_t *)(0xC00000 + ((dy + y) << 10) + (dx << 1));
				pBuf = pSrcBuf + (y << 9);
				
				for(x=0; x < uWidth; x++)
				{
					*pDstGR = *pBuf & 0x00FF;
					pDstGR++;
					pBuf++;
				}
			}
		}
		pBuf = pSrcBuf;
#endif
		
		/* 書き込み先のメモリチェック */
		if( g_stPicImage[i].pImageData == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			continue;
		}
		/* 作業用ポインタ */
		pDstBuf = g_stPicImage[i].pImageData; 
		
#ifdef DEBUG
//		printf("Load5(%d,0x%p, 0x%p)\n", i, g_stPicImage[i].pImageData, pDstBuf);
//		KeyHitESC();	/* デバッグ用 */
#endif
		switch(g_stCG_LIST[i].ubType)
		{
			case 1:	/* スプライトライク */
			case 2:	/* テキスト描画 */
			case 3:	/* テキスト描画(グレイスケール) */
			{
				G_Subtractive_Color(pSrcBuf, pDstBuf, uWidth, uHeight, uSize8x, i);	/* 減色処理 */
				break;
			}
			default:	/* 通常の256色CG */	
			{
				uint32_t	x, y;
				
				/* 加工後をメモリに保存する */
				for(y=0; y < uHeight; y++)
				{
					pBuf = pSrcBuf + (y << 9);
					
					for(x=0; x < uSize8x; x++)
					{
						if(x < uWidth)
						{
							*pDstBuf = *pBuf & 0x00FF;
							pBuf++;
						}
						else
						{
							*pDstBuf = 0x00;
						}
						pDstBuf++;
					}
				}
				break;
			}
		}
		
#ifdef DEBUG
//		printf("Load6(%d,0x%p, 0x%p, 0x%p)\n", i, g_stPicImage[i].pImageData, pBuf, pSrcBuf);
//		KeyHitESC();	/* デバッグ用 */
#endif
		/* パレット */
		G_Palette();	/* 0番パレット変更 */
		Get_PicImagePallet(i);	/* パレットを保存 */
		
		/* メモリ操作 */
		if(pSrcBuf != NULL)
		{
			MyMfree(pSrcBuf);	/* メモリ解放 */
		}
		printf("CG File %2d = %s(x:%d,y:%d)\n", i, g_stCG_LIST[i].bFileName, uWidth, uHeight );
#ifdef DEBUG
//		puts("========================");
//		KeyHitESC();	/* デバッグ用 */
#endif
	}
	printf("CG File Load 完了 = %d\n", g_CG_List_Max );
}

void G_INIT(void)
{
	volatile uint16_t *VIDEO_REG1 = (uint16_t *)0xE82400;
	volatile uint16_t *VIDEO_REG2 = (uint16_t *)0xE82500;
	volatile uint16_t *VIDEO_REG3 = (uint16_t *)0xE82600;
	volatile uint16_t *V_Sync_end = (uint16_t *)0xE8000E;

	G_CLR_ON();				/* グラフィックのクリア */
	VPAGE(0b1111);			/* pege(3:0n 2:0n 1:0n 0:0n) */
//											   210
	*VIDEO_REG1 = Mbset(*VIDEO_REG1,   0x07, 0b001);	/* 512x512 256color 2men */
//											   ||+--------------bit0 
//											   |+---------------bit1 
//											   |						00.    16 color 4 screen
//											   |						01.   256 color 2 screen
//											   |						11. 65535 color 1 screen
//											   +----------------bit2 
//																		0	VR.  512 x 512
//																		1	VR. 1024 x1024

//											   FEDCBA9876543210
	*VIDEO_REG2 = Mbset(*VIDEO_REG2, 0x3FFF, 0b0010000111100100);	/* 優先順位 TX>GR>SP GR0>GR1>GR2>GR3 */
//											   |||||||||||||||+-bit0 0	
//											   ||||||||||||||+--bit1 0	
//											   |||||||||||||+---bit2 1	
//											   ||||||||||||+----bit3 0	
//											   |||||||||||+-----bit4 0	
//											   ||||||||||+------bit5 1	
//											   |||||||||+-------bit6 1	
//											   ||||||||+--------bit7 1	
//											   ||||||||					優先順位 GR0>GR1>GR2>GR3
//											   |||||||+---------bit8 GR	1
//											   ||||||+----------bit9 GR	0
//											   |||||+-----------bitA TX	0
//											   ||||+------------bitB TX	0
//											   |||+-------------bitC SP	0
//											   ||+--------------bitD SP	1
//											   ||						優先順位 TX>GR>SP
//											   |+---------------bitE (Reserve)
//											   +----------------bitF (Reserve)

//											   FEDCBA9876543210
//	*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x3F, 0b0001010011101111);	/* 特殊モードあり 仮想画面512x512 */
	*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x3F, 0b0000000001101111);	/* 特殊モードなし 仮想画面512x512 */
//											   |||||||||||||||+-bit0 GS0	512x512 Pri0 <0:OFF 1:ON>
//											   ||||||||||||||+--bit1 GS1	512x512 Pri1 <0:OFF 1:ON>
//											   |||||||||||||+---bit2 GS2	512x512 Pri2 <0:OFF 1:ON>
//											   ||||||||||||+----bit3 GS3	512x512 Pri3 <0:OFF 1:ON>
//											   |||||||||||+-----bit4 GS4	1024x1024 <0:OFF 1:ON>
//											   ||||||||||+------bit5 TON	TEXT <0:OFF 1:ON>
//											   |||||||||+-------bit6 SON	SP <0:OFF 1:ON>
//											   ||||||||+--------bit7 (Reserve)
//											   |||||||+---------bit8 G/T	
//											   ||||||+----------bit9 G/G	
//											   |||||+-----------bitA B/P	1
//											   ||||+------------bitB H/P	
//											   |||+-------------bitC EXON	1
//											   ||+--------------bitD VHT	
//											   |+---------------bitE AH		
//											   +----------------bitF Ys		
	*V_Sync_end = V_SYNC_MAX;	/* 縦の表示範囲を決める(画面下のゴミ防止) */
}

void G_HOME(void)
{
	WINDOW( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1);
	HOME(0b0000, X_OFFSET, Y_OFFSET);
//	HOME(1, X_OFFSET, Y_OFFSET);
//	HOME(2, X_OFFSET, 416);
//	HOME(3, X_OFFSET, 416);
	WIPE();
}

void G_Palette_INIT(void)
{
	uint32_t	nPalette;

	/* グラフィックパレットの初期化 */
	for(nPalette=0; nPalette < 0xFF; nPalette++)
	{
		GPALET( nPalette, SetRGB(0, 0, 0));	/* Black */
	}
}

void G_VIEW(uint8_t bSW)
{
	volatile uint16_t *VIDEO_REG3 = (uint16_t *)0xE82600;
	
	if(bSW == TRUE)
	{
		/* グラフィック表示をＯＮ */
//												   FEDCBA9876543210
		*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x0F, 0b0000000000001111);	/* ON */
//												   |||||||||||||||+-bit0 GS0	512x512 Pri0 <0:OFF 1:ON>
//												   ||||||||||||||+--bit1 GS1	512x512 Pri1 <0:OFF 1:ON>
//												   |||||||||||||+---bit2 GS2	512x512 Pri2 <0:OFF 1:ON>
//												   ||||||||||||+----bit3 GS3	512x512 Pri3 <0:OFF 1:ON>
	}
	else
	{
		HOME(0b0000, 0, Y_OFFSET);
		/* グラフィック表示をＯＦＦ */
//												   FEDCBA9876543210
		*VIDEO_REG3 = Mbset(*VIDEO_REG3,   0x0F, 0b0000000000000000);	/* OFF */
//												   |||||||||||||||+-bit0 GS0	512x512 Pri0 <0:OFF 1:ON>
//												   ||||||||||||||+--bit1 GS1	512x512 Pri1 <0:OFF 1:ON>
//												   |||||||||||||+---bit2 GS2	512x512 Pri2 <0:OFF 1:ON>
//												   ||||||||||||+----bit3 GS3	512x512 Pri3 <0:OFF 1:ON>
	}
}

void G_Palette(void)
{
	GPALET( 0, SetRGB( 0,  0,  0));	/* Black */
}

int16_t G_Stretch_Pict(	int16_t dst_x, uint16_t dst_w, int16_t dst_y, uint16_t dst_h, uint8_t ubDstScrn,
					int16_t src_x, uint16_t src_w, int16_t src_y, uint16_t src_h, uint8_t ubSrcScrn)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint16_t	*pSrcGR_tmp;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_ex,	src_ey;
	int16_t	x, y;

	dst_ex	= dst_x + dst_w;
	dst_ey	= dst_y + dst_h;
	src_ex	= src_x + src_w;
	src_ey	= src_y + src_h;

	/* アドレス算出 */
	if(ubDstScrn != 0)
	{
		DstGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		DstGR_H = 0xC00000;	/* Screen0 */
	}

	if(ubSrcScrn != 0)
	{
		SrcGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		SrcGR_H = 0xC00000;	/* Screen0 */
	}
#if 1
	for(y = dst_y; y < dst_ey; y++)
	{
		/* アドレス算出 */
		pDstGR = (uint16_t *)(DstGR_H + ((y << 10) + (dst_x << 1)));

		pSrcGR = (uint16_t *)(SrcGR_H + ((((src_y + Mmul_1p25(y - dst_y))) << 10) + (src_x << 1)));
		pSrcGR_tmp = pSrcGR;
	
		for(x = dst_x; x < dst_ex; x++)
		{
			pSrcGR = pSrcGR_tmp + Mmul_1p25(x-dst_x);
			*pDstGR = *pSrcGR;
			pDstGR++;
		}
	}
#else

#endif
	return	ret;
}

/* 画像のコピー */
/* 同じサイズの画像を任意の位置に描画する */
/* 表示先はx,y座標を画像の中心とした位置 */
int16_t G_BitBlt(int16_t dst_x, uint16_t dst_w, int16_t dst_y, uint16_t dst_h, uint8_t ubDstScrn,
			int16_t src_x, uint16_t src_w, int16_t src_y, uint16_t src_h, uint8_t ubSrcScrn,
			uint8_t ubMode, uint8_t ubV, uint8_t ubH)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_ex,	src_ey;
	int16_t	x, y;
	int16_t	x_min, y_min;
	int16_t	x_max, y_max;

	if(dst_x < X_MIN_DRAW)return -1;
	if(dst_x >= X_MAX_DRAW)return -1;
	if(dst_y < Y_MIN_DRAW)return -1;
	if(dst_y >= Y_MAX_DRAW)return -1;
	
	switch(ubV)
	{
		case POS_TOP:
		{
			break;
		}
		case POS_CENTER:
		default:
		{
			dst_y	= dst_y - (dst_h>>1);
			break;
		}
		case POS_BOTTOM:
		{
			dst_y	= dst_y - dst_h;
			break;
		}
	}

	switch(ubH)
	{
		case POS_LEFT:
		{
			break;
		}
		case POS_MID:
		default:
		{
			dst_x	= dst_x - (dst_w>>1);
			break;
		}
		case POS_RIGHT:
		{
			dst_x	= dst_x - dst_w;
			break;
		}
	}
	
	dst_ex	= dst_x + dst_w;
	dst_ey	= dst_y + dst_h;
	src_ex	= src_x + src_w;
	src_ey	= src_y + src_h;

	/* 倍率演算 */
	if(src_w != dst_w)return -1;
	if(src_h != dst_h)return -1;
	
	/* アドレス算出 */
	if(ubDstScrn != 0)
	{
		DstGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		DstGR_H = 0xC00000;	/* Screen0 */
	}

	if(ubSrcScrn != 0)
	{
		SrcGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		SrcGR_H = 0xC00000;	/* Screen0 */
	}
	
	/* 表示エリア内クリップ */
	switch(ubMode)
	{
		case 0:
		{
			x_min = X_MIN_DRAW + X_OFFSET;
			x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			y_min = Y_MIN_DRAW;
			y_max = Y_MIN_DRAW + Y_MAX_WINDOW;
			break;
		}
		case 1:
		default:
		{
			x_min = X_MIN_DRAW + X_OFFSET;
			x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			y_min = Y_MIN_DRAW + Y_OFFSET;
			y_max = Y_MIN_DRAW + Y_OFFSET + Y_MAX_WINDOW;
			break;
		}
		case 2:
		{
			x_min = X_MIN_DRAW + X_OFFSET;
			x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			y_min = Y_MIN_DRAW;
			y_max = Y_MIN_DRAW + Y_MAX_WINDOW;
			break;
		}
	}

	if(dst_ex < x_min)		/* 画像の右側が画面外 */
	{
		return -1;
	}
	else if((dst_x <= x_min) && (dst_ex >= x_min))		/* 画像の左側が画面外だが、画像の右側は画面内 */
	{
		src_x += x_min - dst_x;
		dst_x = x_min;
	}
	else if((dst_x < x_max) && (dst_ex >= x_max))		/* 画像の左側が画面内だが、画像の右側は画面外 */
	{
		src_ex -= dst_ex - x_max;
		dst_ex = x_max;
	}
	else if(dst_x >= x_max)	/*  画像の左側が画面外 */
	{
		return -1;
	}
	else						/* 範囲内 */
	{
		/* 何もしない */
	}
	
	if(dst_ey < y_min)		/* 画面外 */
	{
		return -1;
	}
	else if((dst_y <= y_min) && (dst_ey >= y_min))
	{
		src_y += y_min - dst_y;
		dst_y = y_min;
	}
	else if((dst_y < y_max) && (dst_ey >= y_max))
	{
		src_ey -= dst_ey - y_max;
		dst_ey = y_max;
	}
	else if(dst_y > y_max)	/* 画面外 */
	{
		return -1;
	}
	else						/* 範囲内 */
	{
		/* 何もしない */
	}

	for(y = dst_y; y < dst_ey; y++)
	{
		/* アドレス算出 */
		pDstGR = (uint16_t *)(DstGR_H + ((y << 10) + (dst_x << 1)));

		pSrcGR = (uint16_t *)(SrcGR_H + (((src_y + (y - dst_y)) << 10) + (src_x << 1)));
	
		for(x = dst_x; x < dst_ex; x++)
		{
			if(*pSrcGR != 0)
			{
				*pDstGR = *pSrcGR;
			}
			pDstGR++;
			pSrcGR++;
		}
	}
	return	ret;
}

/* 画面のクリア */
int32_t G_CLR(void)
{
	return _iocs_wipe();
}

int16_t G_CLR_AREA(int16_t x, uint16_t w, int16_t y, uint16_t h, uint8_t Screen)
{
	int16_t	ret = 0;
	int16_t	i=0;
	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	uint32_t	unSize;
#if 0
	uint16_t	data[512] = {0};
	int32_t	nMode;
	uint8_t *DMA_DCR;
	uint8_t *DMA_GCR;
	
	struct _chain stSrcImage;
	struct _chain stDstImage[256];

	if(DMAMODE() != 0u)return -1;
#endif
#if 0
	volatile uint16_t *CRTC_R21 = (uint16_t *)0xE8002Au;	/* テキスト・アクセス・セット、クリアーP.S */
	volatile uint16_t *CRTC_480 = (uint16_t *)0xE80480u;	/* CRTC動作ポート */
#endif

	switch(Screen)
	{
		case 0:
		{
			ulGR_H = 0xC00000;	/* Screen0 */
			break;
		}
		case 1:
		default:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
	}
	
	unSize = (w << 1);
	ulPoint = (y << 10u) + (x << 1);
#if 0
	nMode = 0b10000100;
	/*		  ||||||++------	(bit1,0)	DAC(addr2)	00:none	01:inc	10:dec	*/
	/*		  ||||++--------	(bit3,2)	MAC(addr1)	00:none	01:inc	10:dec	*/
	/*		  |+++----------	(bit6,5,4)	(Reserved)	0				*/
	/*		  +-------------	(bit7)		転送方向	0:addr1->addr2	1:addr2->addr1	*/

	stSrcImage.addr = &data[0];
	stSrcImage.len  = 512;
	for(i=0; i<h; i++)
	{
		stDstImage[i].addr =  (void *)(ulGR_H + ulPoint + (i << 10u));
		stDstImage[i].len  =  unSize;
	}
	
	DMA_DCR = (uint8_t *)0xE84084u;
	*DMA_DCR = Mbset(*DMA_DCR, ((uint8_t)0b11000000), ((uint8_t)0b10000000));	/* XRM(bit7,6) 10: */
	DMA_GCR = (uint8_t *)0xE840FFu;
	*DMA_GCR = Mbset(*DMA_GCR, ((uint8_t)0b0011), ((uint8_t)0b0011));/* BR(bit1,0) 11:6.25% */
	
	DMAMOV_A(&stDstImage[0],	/* addr1 */
			&data[0],			/* addr2 */
			nMode,				/* mode */
//		Mmax((int32_t)h/16, 1));
			(int32_t)h);				/* len */
#endif

#if 0
	for(i=0; i<h; i++)
	{
//		DMAMOVE(&data[0],									/* addr1 */
//				(void *)(ulGR_H + ulPoint + (i * 0x400u)),	/* addr2 */
//				nMode,										/* mode */
//				(int32_t)unSize);								/* len */
	}
#endif

#if 1
	for(i=0; i<h; i++)
	{
		memset((void *)(ulGR_H + ulPoint + (i << 10u)), 0x00u, unSize);
	}
#endif

#if 0
	/* 必ずテキスト表示処理の後に行うこと */
	/* クリアにかかる時間は1/55fpsとのこと@Tea_is_Appleさんより */
	if((*CRTC_480 & 0x02u) == 0u)	/* クリア実行中でない */
	{
		*CRTC_R21 = Mbset(*CRTC_R21, 0x0Fu, 0x0Cu);	/* SCREEN1 高速クリアON / SCREEN0 高速クリアOFF */
		*CRTC_480 = Mbset(*CRTC_480, 0x02u, 0x02u);	/* クリア実行 */
	}
#endif
	return	ret;
}

int16_t G_CLR_ALL_OFFSC(uint8_t bMode)
{
	int16_t	ret = 0;
	
	ST_CRT	stCRT = {0};
	GetCRT(&stCRT, bMode);
	
	/* 描画可能枠再設定 */
	WINDOW( stCRT.hide_offset_x, 
			stCRT.hide_offset_y,
			stCRT.hide_offset_x + WIDTH,
			stCRT.hide_offset_y + Y_MAX_WINDOW);	
	/* 消去 */
	ret = G_CLR_AREA(stCRT.hide_offset_x, WIDTH, stCRT.hide_offset_y, Y_MAX_WINDOW, 0);	/* Screen0 消去 */

	return	ret;
}

int16_t G_FILL_AREA(int16_t x, uint16_t w, int16_t y, uint16_t h, uint8_t Screen, uint8_t bPal)
{
	int16_t	ret = 0;
	int16_t	i=0;
	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	uint32_t	unSize;

	WINDOW( x, y, x + w, y + h);	/* 描画可能枠再設定 */

	switch(Screen)
	{
		case 0:
		{
			ulGR_H = 0xC00000;	/* Screen0 */
			break;
		}
		case 1:
		default:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
	}
	
	unSize = (w << 1);
	ulPoint = (y << 10u) + (x << 1);

	for(i=0; i<h; i++)
	{
		memset((void *)(ulGR_H + ulPoint + (i << 10u)), bPal, unSize);
	}

	return	ret;
}


int16_t G_Load(uint8_t bCGNum, uint16_t uX, uint16_t uY, uint16_t uArea)
{
	int16_t	ret = 0;

	ret = APICG_DataLoad2G( g_stCG_LIST[bCGNum].bFileName, uX, uY, uArea);

	return	ret;
}

int16_t G_Load_Mem(uint8_t bCGNum, uint16_t uX, uint16_t uY, uint16_t uArea)
{
	int16_t	ret = 0;

	uint16_t	*pDstGR, *pSrcGR;
	uint32_t	DstGR_H;
	uint32_t	Addr_Max;
	uint32_t	uMaxNum;
	int16_t	x, y;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	uSize8x = 0;
	int16_t	Pal_offset;

	Get_CG_FileList_MaxNum(&uMaxNum);
	if(uMaxNum <= bCGNum)
	{
		ret = -1;
		return	ret;
	}
	pSrcGR = g_stPicImage[bCGNum].pImageData;	/* Src PIC */
	
	Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8の倍数 */
	
	if(	(uWidth >= 512) || (uHeight >= 512) )
	{
		ret = -1;
		return	ret;
	}
	
	/* アドレス算出 */
	if(uArea == 0)
	{
		DstGR_H = 0xC00000;	/* Screen0 */
		Addr_Max = 0xC80000;
	}
	else{
		DstGR_H = 0xC80000;	/* Screen1 */
		Addr_Max = 0xD00000;
	}

	Pal_offset = Set_PicImagePallet(bCGNum);	/* パレットを設定 */
	
	for(y = 0; y < uHeight; y++)
	{
		/* アドレス算出 */
		pDstGR = (uint16_t *)(DstGR_H + (((uY + y) << 10) + (uX << 1)));
		if(Addr_Max <= (uint32_t)pDstGR)
		{
			continue;
		}
	
		for(x = 0; x < uSize8x; x++)
		{
			if(x < uWidth)
			{
				if(*pSrcGR != 0x00)
				{
					*pDstGR = *pSrcGR + Pal_offset;
				}
				if(Addr_Max > (uint32_t)pDstGR)
				{
					pDstGR++;
				}
			}
			pSrcGR++;
		}
	}
	
	return	ret;
}

/* PICファイルを読み込み */
int16_t APICG_DataLoad2G(int8_t *fname, uint64_t pos_x, uint64_t pos_y, uint16_t uArea)
{
	int16_t ret;

	uint16_t *GR;
	uint8_t *file_buf = NULL, *work_buf = NULL;
	
	file_buf = (uint8_t*)MyMalloc(PIC_FILE_BUF_SIZE);
	work_buf = (uint8_t*)MyMalloc(PIC_WORK_BUF_SIZE);
	
	if(uArea != 0)
	{
		GR = (uint16_t *)0xC80000;	/* Screen1 */
	}
	else{
		GR = (uint16_t *)0xC00000;	/* Screen0 */
	}
	
	if( (file_buf == NULL) || (work_buf == NULL) )
	{
		/* メモリエラー */
		ret = -1;
	}
	else
	{
		ret = APICLOAD(	(uint16_t*)GR, 
						fname,				/* PICファイル名 */
						pos_x, pos_y,		/* 描画先のX座標とY座標 */
						file_buf,
						PIC_FILE_BUF_SIZE,	
						APF_NOINITCRT | 	/* 1で画面モードを初期化しません */
						APF_NOCLRBUF | 		/* 1で展開先バッファをクリアしません */
						APF_NOPRFC,			/* 1でファイル名とコメントを表示しません */
						work_buf);
		if (ret < 0) {
			/* メモリエラー */
			ret = -1;
		}
		if(work_buf != NULL)
		{
			MyMfree(work_buf);	/* メモリ解放 */
		}
		if(file_buf != NULL)
		{
			MyMfree(file_buf);	/* メモリ解放 */
		}
	}
	return ret;
}

int16_t APICG_DataLoad2M(uint8_t uNum, uint64_t pos_x, uint64_t pos_y, uint16_t uArea, uint16_t *pDst)
{
	uint16_t *GR;
	uint8_t *work_buf = NULL;
	uint32_t	uMaxNum;
	int16_t ret = 0;
	
	Get_CG_FileList_MaxNum(&uMaxNum);
	if(uMaxNum <= uNum)
	{
		ret = -1;
		return	ret;
	}

	if(pDst == NULL)
	{
		switch(uArea)
		{
		case 0:
		default:
			GR = (uint16_t *)0xC00000;	/* Screen0 */
			break;
		case 1:
			GR = (uint16_t *)0xC80000;	/* Screen1 */
			break;
		}
	}
	else{
		GR = pDst;					/* メモリ */
		pos_x = 0;					/* 強制0 */
		pos_y = 0;					/* 強制0 */
	}
	
	{
		uint32_t uWidth, uHeight, uFileSize;
		uint8_t *pFileBuf;

		pFileBuf = (uint8_t*)Get_CG_FileBuf(uNum);	/* ファイルバッファのポインタ取得 */
		Get_PicImageInfo(uNum, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */

#ifdef DEBUG
//		printf("MAPIC1(%d:0x%p)=(0x%p)\n", uNum, pDst, pFileBuf);
//		printf("MAPIC2(%d:%d)=(%d,%d)(%d)\n", uNum, uArea, uWidth, uHeight, uFileSize );
#endif

//		work_buf = (uint8_t*)MyMalloc( uHeight * (((uWidth+7)/8) * 8) );
		work_buf = (uint8_t*)MyMalloc( PIC_WORK_BUF_SIZE );	/* 画面と同じ */
		if( work_buf == NULL )
		{
			/* メモリエラー */
			ret = -1;
		}
		else
		{
			//int M_APICLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE * );
			/* メモリからロード */
			ret = M_APICLOAD(	(uint16_t*)GR,			/* Load Ptr */
								&g_CGInfo[uNum],	/* Data Information */
								pos_x, pos_y,		/* Load Position */
								pFileBuf,			/* Load FileBuf */
								uFileSize,			/* Load FileSize */
								APF_NOINITCRT | 	/* 1で画面モードを初期化しません */
								APF_NOCLRBUF | 		/* 1で展開先バッファをクリアしません */
								APF_NOPRFC,			/* 1でファイル名とコメントを表示しません */
								work_buf			/* WorkBuf */
							);
		}
#ifdef DEBUG
//		printf("MAPIC3(%d)=(%d,%d)(%d,%d)(%d)\n", uNum,
//												g_CGInfo[uNum].STARTX,	g_CGInfo[uNum].STARTY,
//												g_CGInfo[uNum].SIZEX,	g_CGInfo[uNum].SIZEY,	g_CGInfo[uNum].COLOR );
#endif
		
		if(work_buf != NULL)
		{
			MyMfree(work_buf);	/* メモリ解放 */
		}
	}
	return ret;
}

#if 0
      +--------------------------------------------------------------+
      |0       |                |正常にロード・セーブが行なわれまし  |
      |        |                |た（エラーなし）。                  |
      |--------+----------------+------------------------------------|
      |-1～-96 |                |Human68Kが返すエラーコードです。    |
      |--------+----------------+------------------------------------|
      |-123    |_ERRAPG_FORMAT  |フォーマットが違う、あるいは壊れて  |
      |        |                |いるファイルです。                  |
      |--------+----------------+------------------------------------|
      |-122    |_ERRAPG_FILEBUF |ファイルバッファが小さすぎます。    |
      |--------+----------------+------------------------------------|
      |-121    |_ERRAPG_RECT    |座標が領域をはみ出しています。      |
      |--------+----------------+------------------------------------|
      |-119    |_ERRAPG_MODE    |セーブできない画面モードです。      |
      |--------+----------------+------------------------------------|
      |-115    |_ERRAPG_WORKBUF |ワークバッファが小さすぎます。      |
      |--------+----------------+------------------------------------|
      |-113    |_ERRAPG_FUTURE  |未対応のヘッダです。                |
      +--------------------------------------------------------------+
#endif

int16_t G_Subtractive_Color(uint16_t *pSrcBuf, uint16_t *pDstBuf, uint16_t uWidth, uint16_t uHeight, uint16_t uWidthEx, uint32_t uNum)
{
	int16_t	ret = 0;
	uint32_t	i, j, k, m;
	uint32_t	x, y, z;
	uint32_t	uAPICG_work_Size;
	uint32_t	uSize8x = 0;
	uint16_t	*pBuf = NULL;
	uint16_t	col;
	uint8_t	ubR=0,ubG=0,ubB=0;
	uint8_t	ubNotUsePal[256];
	uint16_t	uColTbl[256];
	uint8_t	ubConvPal;
	uint8_t	ubType;
	uint8_t	ubCow_R, ubCow_G, ubCow_B;
	uint8_t	ubTableOffset;
	
	/* 均等量子化テーブル */
	uint8_t	ubGen8_R[8];
	uint8_t	ubGen8_G[8];
	uint8_t	ubGen8_B[8];

	pBuf = pSrcBuf;
	uAPICG_work_Size = Mdiv2(PIC_WORK_BUF_SIZE);
	uSize8x = uWidthEx;
	ubConvPal = g_stCG_LIST[uNum].ubTransPal;
	ubType    = g_stCG_LIST[uNum].ubType;
	
	switch(ubType)		
	{
		case 1:	/* スプライトライク */
		{
			ubCow_R = PIC_R;
			ubCow_G = PIC_G;
			ubCow_B = PIC_B;
			ubTableOffset = 2u;
			z = (ubCow_R * ubCow_G * ubCow_B) + ubTableOffset;
			
			break;
		}
		case 2:	/* テキスト描画 */
		{
			ubCow_R = 2u;
			ubCow_G = 2u;
			ubCow_B = 2u;
			ubTableOffset = 0u;
			z = (ubCow_R * ubCow_G * ubCow_B) + ubTableOffset;

			break;
		}
		case 3:	/* テキスト描画(グレイスケール) */
		{
			ubCow_R = 8u;
			ubCow_G = 8u;
			ubCow_B = 8u;
			ubTableOffset = 0u;
			z = (8) + ubTableOffset;

			break;
		}
		default:
		{
			ubCow_R = PIC_R;
			ubCow_G = PIC_G;
			ubCow_B = PIC_B;
			ubTableOffset = 2u;
			z = (ubCow_R * ubCow_G * ubCow_B) + ubTableOffset;

			break;
		}
	}
	
	/* 除外パレット抽出準備 */
	for(j=0; j<256; j++)
	{
		ubNotUsePal[j] = j;
		uColTbl[j] = 0;
	}
	
	/* 色の設定 */
	if(ubType == 3u)	/* グレイスケール */
	{
		g_CG_MaxColor[uNum][COLOR_R] = 255;
		g_CG_MaxColor[uNum][COLOR_G] = 255;
		g_CG_MaxColor[uNum][COLOR_B] = 255;
		
		m = 0;
		for(i=0; i<z; i++)
		{
			ubGen8_R[i] = Mmul4(i);
			ubGen8_G[i] = ubGen8_R[i];
			ubGen8_B[i] = ubGen8_R[i];
			
			uColTbl[m] = SetRGB(ubGen8_R[i], ubGen8_G[i], ubGen8_B[i]);
			m++;
		}
	}
	else
	{
		uint32_t uHalfSize = Mdiv2(uAPICG_work_Size);
		/* 減色カラーの対象を抽出 */
		for(j=0; j < uHalfSize; j++)
		{
			col = GPALET( *pBuf, -1 );	/* 現在の設置を抽出 */
			
			if(*pBuf == ubConvPal)	/* 透明色 */
			{
				/* 何もしない */
			}
			else
			{
				/* 最大値を取得 */
				ubR = Mmax( ubR, GetR(col) );
				ubG = Mmax( ubG, GetG(col) );
				ubB = Mmax( ubB, GetB(col) );
			}
			pBuf++;
		}
		g_CG_MaxColor[uNum][COLOR_R] = ubR;
		g_CG_MaxColor[uNum][COLOR_G] = ubG;
		g_CG_MaxColor[uNum][COLOR_B] = ubB;

		pBuf = pSrcBuf;	/* ポインタをもとに戻す */

		/* 減色カラーの設定 */
		ubGen8_R[0] = 0x01;
		for(j=1; j<ubCow_R; j++)
		{
			ubGen8_R[j] = g_CG_MaxColor[uNum][COLOR_R] / (ubCow_R-j);
		}
		ubGen8_G[0] = 0x01;
		for(j=1; j<ubCow_G; j++)
		{
			ubGen8_G[j] = g_CG_MaxColor[uNum][COLOR_G] / (ubCow_G-j);
		}
		ubGen8_B[0] = 0x01;
		for(j=1; j<ubCow_B; j++)
		{
			ubGen8_B[j] = g_CG_MaxColor[uNum][COLOR_B] / (ubCow_B-j);
		}
		m = 1;
		uColTbl[0] = SetRGB(0, 0, 0);	/* 透過 */
		
		for(i=0; i<ubCow_R; i++)
		{
			for(j=0; j<ubCow_G; j++)
			{
				for(k=0; k<ubCow_B; k++)
				{
					uColTbl[m] = SetRGB(ubGen8_R[i], ubGen8_G[j], ubGen8_B[k]);
					m++;
				}
			}
		}
	}
	
	
	/* 加工後をメモリに保存する */
	for(y=0; y < uHeight; y++)
	{
		pBuf = pSrcBuf + (y << 9);
		
		for(x=0; x < uSize8x; x++)
		{
			if(x < uWidth)
			{
				uint8_t pal;
				
				pal = (uint8_t)(*pBuf & 0x00FF);	/* 画像のパレット番号を取得 */

				ubNotUsePal[pal] = 0;	/* 使用したパレット */

				if(pal == ubConvPal)	/* 変換対象色 */
				{
					pal = TRANS_PAL;	/* 透過色 */
				}
				else
				{
					uint8_t ubTmp;
					
					col = GPALET(pal, -1);	/* パレット番号からカラーコードを取得 */
					ubR = GetR(col);
					ubG = GetG(col);
					ubB = GetB(col);
					
					if(ubType == 3u)	/* グレイスケール */
					{
						uint32_t	uGlay;
						uGlay =  ((uint32_t)(Mmul2(ubR) + ubR));
						uGlay += ((uint32_t)(Mmul4(ubG) + Mmul2(ubG)));
						uGlay += ((uint32_t)ubB);
						uGlay = Mdiv10(uGlay);
						
						col = SetRGB(uGlay, uGlay, uGlay);	/* ３色を合成 */

						ubR = GetR(col);
						ubG = GetG(col);
						ubB = GetB(col);

						k = 0;
						ubTmp = McmpSub(ubGen8_R[0], ubR);
						for(m=1; m<ubCow_R; m++)
						{
							if( ubTmp > McmpSub(ubGen8_R[m], ubR) )
							{
								ubTmp = McmpSub(ubGen8_R[m], ubR);
								k = m;
							}
						}
						ubR = ubGen8_R[k];
						ubG = ubR;
						ubB = ubR;
					}
					else
					{
						/* 均等量子化 */
						k = 0;
						ubTmp = McmpSub(ubGen8_R[0], ubR);
						for(m=1; m<ubCow_R; m++)
						{
							if( ubTmp > McmpSub(ubGen8_R[m], ubR) )
							{
								ubTmp = McmpSub(ubGen8_R[m], ubR);
								k = m;
							}
						}
						ubR = ubGen8_R[k];
						
						k = 0;
						ubTmp = McmpSub(ubGen8_G[0], ubG);
						for(m=1; m<ubCow_G; m++)
						{
							if( ubTmp > McmpSub(ubGen8_G[m], ubG) )
							{
								ubTmp = McmpSub(ubGen8_G[m], ubG);
								k = m;
							}
						}
						ubG = ubGen8_G[k];

						k = 0;
						ubTmp = McmpSub(ubGen8_B[0], ubB);
						for(m=1; m<ubCow_B; m++)
						{
							if( ubTmp > McmpSub(ubGen8_B[m], ubB) )
							{
								ubTmp = McmpSub(ubGen8_B[m], ubB);
								k = m;
							}
						}
						ubB = ubGen8_B[k];
					}
					col = SetRGB(ubR, ubG, ubB);	/* ３色を合成 */
					
					for( i=0; i<z; i++ )
					{
						if(uColTbl[i] == col)
						{
							pal = i;
							break;
						}
					}
				}
				*pDstBuf = pal;		/* コピー先へ */
				
				pBuf++;
			}
			else
			{
				*pDstBuf = 0x00;
			}
			pDstBuf++;
		}
	}
	
	for(j=0; j<256; j++)
	{
		if(j <= z)
		{
			GPALET( j, uColTbl[j] );	/* 減色結果でパレット更新 */
		}
		else
		{
			GPALET( j, 0 );				/* 0 */
		}
	}
//	for(j=0; j<256; j++)
//	{
//		if(ubNotUsePal[j] != 0)
//		{
//			GPALET( j, SetRGB(0, 0, 0) );	/* 使ってないパレットは透過色設定 */
//		}
//	}
	
	return ret;
}

int16_t PutGraphic_To_Text(uint8_t bCGNum, uint16_t dx, uint16_t dy)
{
	int16_t	ret = 0;

	uint32_t	x, y, z;
	uint32_t	uMaxNum;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	uSize8x = 0;
	
	uint16_t	uDstTxAddr;
	uint16_t	*pSrcGR;
	
	uint8_t	*T0_HEAD = (uint8_t *)0xE00000;
	uint8_t	*T1_HEAD = (uint8_t *)0xE20000;
	uint8_t	*T2_HEAD = (uint8_t *)0xE40000;
	uint8_t	*T3_HEAD = (uint8_t *)0xE60000;
	uint8_t	*pDst0;
	uint8_t	*pDst1;
	uint8_t	*pDst2;
	uint8_t	*pDst3;
	uint16_t	unPal;
	uint16_t	unPalOffset;
	uint16_t	unTmp[4];
	
	Get_CG_FileList_MaxNum(&uMaxNum);
	if(uMaxNum <= bCGNum)
	{
		ret = -1;
		return	ret;
	}
	pSrcGR = g_stPicImage[bCGNum].pImageData;	/* Src PIC */
	
	Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8の倍数 */

	unPalOffset = Set_PicImagePallet(bCGNum);	/* パレットを設定 */
	
	for(y = dy; y < dy+uHeight; y++)
	{
		for(x = dx; x < dx+uWidth; x+=8)	/* 8bit */
		{
			/* アドレス算出 */
//			if( (k < 0) || ((k+size) > 0x1FFFF) ) break;
			uDstTxAddr = (y * 0x80) + (x>>3);
			pDst0 = T0_HEAD + uDstTxAddr;
			pDst1 = T1_HEAD + uDstTxAddr;
			pDst2 = T2_HEAD + uDstTxAddr;
			pDst3 = T3_HEAD + uDstTxAddr;
			
			unTmp[0] = 0;
			unTmp[1] = 0;
			unTmp[2] = 0;
			unTmp[3] = 0;
			for(z = 0; z < 8; z++)	/* 8dot分実施 */
			{
				if(*pSrcGR == 0)
				{
					unPal = *pSrcGR;
				}
				else if(*pSrcGR >= 16)
				{
					unPal = 15;
				}
				else
				{
					unPal = (*pSrcGR) + unPalOffset;
#ifdef DEBUG
//					printf("unPal(%3d,%3d)=%d\n", x, y, unPal);
					//KeyHitESC();	/* デバッグ用 */
#endif
				}
				
				unTmp[0] |= ((unPal & 0b0001)     ) << (7-z);
				unTmp[1] |= ((unPal & 0b0010) >> 1) << (7-z);
				unTmp[2] |= ((unPal & 0b0100) >> 2) << (7-z);
				unTmp[3] |= ((unPal & 0b1000) >> 3) << (7-z);

				pSrcGR++;
			}
			*pDst0 = unTmp[0];
			*pDst1 = unTmp[1];
			*pDst2 = unTmp[2];
			*pDst3 = unTmp[3];
		}
	}

	return ret;
}

int16_t PutGraphic_To_Symbol(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = 1;
	stSymbol.mag_y = 1;
	stSymbol.color = uPal;
	stSymbol.font_type = 0;
	stSymbol.angle = 0;
	
	_iocs_symbol(&stSymbol);

	return ret;
}

int16_t	G_Scroll(int16_t x, int16_t y, uint8_t bSCNum)
{
	int16_t	ret = 0;
	
	switch(bSCNum)
	{
	case 0:
	default:
		{
			volatile uint16_t *CRTC_R12 = (uint16_t *)0xE80018u;
			volatile uint16_t *CRTC_R13 = (uint16_t *)0xE8001Au;
			volatile uint16_t *CRTC_R14 = (uint16_t *)0xE8001Cu;
			volatile uint16_t *CRTC_R15 = (uint16_t *)0xE8001Eu;
			*CRTC_R12  = Mbset(*CRTC_R12 , 0x03FF, x);	/* Screen 0 x */
			*CRTC_R13  = Mbset(*CRTC_R13 , 0x03FF, y);	/* Screen 0 y */
			*CRTC_R14  = Mbset(*CRTC_R14 , 0x01FF, x);	/* Screen 1 x */
			*CRTC_R15  = Mbset(*CRTC_R15 , 0x01FF, y);	/* Screen 1 y */
			break;
		}
	case 1:
		{
			volatile uint16_t *CRTC_R16 = (uint16_t *)0xE80020u;
			volatile uint16_t *CRTC_R17 = (uint16_t *)0xE80022u;
			volatile uint16_t *CRTC_R18 = (uint16_t *)0xE80024u;
			volatile uint16_t *CRTC_R19 = (uint16_t *)0xE80026u;
			*CRTC_R16  = Mbset(*CRTC_R16 , 0x01FF, x);	/* Screen 2 x */
			*CRTC_R17  = Mbset(*CRTC_R17 , 0x01FF, y);	/* Screen 2 y */
			*CRTC_R18  = Mbset(*CRTC_R18 , 0x01FF, x);	/* Screen 3 x */
			*CRTC_R19  = Mbset(*CRTC_R19 , 0x01FF, y);	/* Screen 3 y */
			break;
		}
	case 2:
		{
			break;
		}
	case 3:
		{
			break;
		}
	}

	return ret;
}

#endif	/* GRAPHIC_C */
