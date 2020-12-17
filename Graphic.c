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

#include "EnemyCAR.h"
#include "Draw.h"
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
UI	g_CG_List_Max	=	0u;
UC	*g_pCG_FileBuf[CG_MAX];
US	g_CG_ColorCode[CG_MAX][256]	=	{0};
UC	g_CG_MaxColor[CG_MAX][3]	=	{0};

/* グローバル構造体 */
ST_CRT		g_stCRT[CRT_MAX] = {0};
PICIMAGE	g_stPicImage[CG_MAX];
CG_LIST		g_stCG_LIST[CG_MAX];
struct APICGINFO	g_CGInfo[CG_MAX];

/* 関数のプロトタイプ宣言 */
SS	GetCRT(ST_CRT *, SS);
SS	SetCRT(ST_CRT, SS);
SS	CRT_INIT(void);
SS	Get_CG_FileList_MaxNum(UI *);
UC	*Get_CG_FileBuf(UC);
SS	Get_PicImageInfo(UC , UI *, UI *, UI *);
SS	Get_PicImagePallet(UC);
SS	Set_PicImagePallet(UC);
void CG_File_Load(void);
void G_INIT(void);
void G_HOME(void);
void G_MyCar(void);
void G_Palette_INIT(void);
void G_Palette(void);
SS	G_Stretch_Pict( SS , US , SS , US , UC , SS , US, SS, US, UC );
SS	G_BitBlt(SS , US , SS , US , UC , SS , US , SS , US , UC , UC , UC , UC );
SI	G_CLR(void);
SS	G_CLR_AREA(SS, US, SS, US, UC);
SS	G_CLR_ALL_OFFSC(UC);
SS	G_Load(UC, US, US, US);
SS	G_Load_Mem(UC, US, US, US);
SS	APICG_DataLoad2G(SC *, UL, UL, US);
SS	APICG_DataLoad2M(UC , UL, UL, US, US *);
SS	G_Subtractive_Color(US *, US *, US, US, US, UI);

/* 関数 */
SS	GetCRT(ST_CRT *stDat, SS Num)
{
	SS	ret = 0;
	
	if(Num < CRT_MAX)
	{
		*stDat = g_stCRT[Num];
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

SS	SetCRT(ST_CRT stDat, SS Num)
{
	SS	ret = 0;
	
	if(Num < CRT_MAX)
	{
		g_stCRT[Num] = stDat;
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

SS CRT_INIT(void)
{
	SS	ret = 0;
	
	ret = CRTMOD(-1);	/* 現在のモードを返す */

	CRTMOD(11);			/* 偶数：31kHz、奇数：15kHz(17,18:24kHz) */

	/* CRTの設定 */
	g_stCRT[0].view_offset_x	= X_OFFSET;
	g_stCRT[0].view_offset_y	= Y_MIN_DRAW;
	g_stCRT[0].hide_offset_x	= X_OFFSET;
	g_stCRT[0].hide_offset_y	= Y_OFFSET;
	g_stCRT[0].BG_offset_x		= 0;
	g_stCRT[0].BG_offset_y		= 0;
	g_stCRT[0].BG_under			= BG_0_UNDER;
	
	g_stCRT[1].view_offset_x	= X_OFFSET;
	g_stCRT[1].view_offset_y	= Y_OFFSET;
	g_stCRT[1].hide_offset_x	= X_OFFSET;
	g_stCRT[1].hide_offset_y	= Y_MIN_DRAW;
	g_stCRT[1].BG_offset_x		= 0;
	g_stCRT[1].BG_offset_y		= 32;
	g_stCRT[1].BG_under			= BG_1_UNDER;

	g_stCRT[2].view_offset_x	= X_OFFSET;
	g_stCRT[2].view_offset_y	= Y_MIN_DRAW;
	g_stCRT[2].hide_offset_x	= X_OFFSET;
	g_stCRT[2].hide_offset_y	= Y_OFFSET;
	g_stCRT[2].BG_offset_x		= 0;
	g_stCRT[2].BG_offset_y		= 32;
	g_stCRT[2].BG_under			= BG_1_UNDER;
	
	return ret;
}

SS Get_CG_FileList_MaxNum(UI *uMaxNum)
{
	SS	ret = 0;
	
	*uMaxNum = g_CG_List_Max;
	
	return ret;
}

UC *Get_CG_FileBuf(UC bNum)
{
	UC *ret = 0;
	
	ret = g_pCG_FileBuf[bNum];
//	printf("Get(%d,0x%p,0x%p)\n", bNum, g_pCG_FileBuf[bNum], ret );
	
	return ret;
}

SS Get_PicImageInfo(UC bNum, UI *pWidth, UI *pHeight, UI *pFileSize)
{
	SS	ret = 0;
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

SS Get_PicImagePallet(UC bNum)
{
	SS	ret = 0;
	UI	i;

	/* パレットの取得 */
	for(i=0; i<256; i++)
	{
		g_CG_ColorCode[bNum][i] = GPALET( i, -1 );	/* 現在の設置を保存 */
	}
	
	return ret;
}

SS Set_PicImagePallet(UC bNum)
{
	SS	ret = 0;
	UI	i;
	UI	offset_val;
	static UI offset = 0;
	
	/* パレットの設定 */
	if(g_stCG_LIST[bNum].ubType != 0u)	/* スプライトライク */
	{
		offset_val = G_COLOR * offset * G_COLOR_SP;
		for(i=0; i< (COLOR_MAX * G_COLOR_SP) + 2; i++)
		{
			GPALET( i+offset_val, g_CG_ColorCode[bNum][i] );
		}
		offset++;
		if(offset >= 0x100 / (G_COLOR * G_COLOR_SP))
		{
			offset = 0;
		}
		ret = offset_val;
	}
	else
	{
		for(i=0; i<256; i++)
		{
			GPALET( i, g_CG_ColorCode[bNum][i] );
		}
		offset = 0;
	}
	
	return ret;
}

void CG_File_Load(void)
{
	US	i;
	SI	FileSize;

	/* グラフィックリスト */
	Load_CG_List("data\\cg\\g_list.txt", g_stCG_LIST, &g_CG_List_Max);
	
	for(i = 0; i < g_CG_List_Max; i++)
	{
		SI	Size;
		UI	uSize8x = 0;
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
		Size = (pInfo->biHeight) * uSize8x * sizeof(US);
#ifdef DEBUG
//		printf("Head3(%d)=(%d)\n", i, Size );
//		KeyHitESC();	/* デバッグ用 */
#endif
		/* メモリ確保 */
		g_stPicImage[i].pImageData = NULL;
		g_stPicImage[i].pImageData = (US*)MyMalloc( Size );	/* メモリの確保 */
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
		g_pCG_FileBuf[i] = (UC*)MyMalloc( FileSize );	/* メモリ確保 */
		if( g_pCG_FileBuf[i] == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			continue;
		}
		File_Load( g_stCG_LIST[i].bFileName, (UC *)g_pCG_FileBuf[i], sizeof(UC), FileSize);	/* メモリに読み込み */
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
		US	*pBuf = NULL;
		US	*pSrcBuf = NULL;
		US	*pDstBuf = NULL;
		SI	Res;
		UI	uWidth, uHeight;
		UI	uSize8x = 0;
		UI	uAPICG_work_Size;

		BITMAPINFOHEADER *pInfo;
		
		/* メモリサイズ */
		pInfo = g_stPicImage[i].pBMi;
		uWidth	= pInfo->biWidth;
		uHeight	= pInfo->biHeight;
		uSize8x = (((uWidth+7)/8) * 8);	/* 8の倍数 */
#ifdef DEBUG
//		printf("Load1(%d,0x%p)=(%d,%d)(%d)\n", i, g_stPicImage[i].pBMi, pInfo->biWidth, pInfo->biHeight, uSize8x );
#endif
		
		/* メモリ確保(画面を模擬) */
		uAPICG_work_Size = PIC_WORK_BUF_SIZE / 2;
		pSrcBuf = (US*)MyMalloc( uAPICG_work_Size );
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
			US	*pDstGR = NULL;
			UI	dx = 0, dy = 0;

//			memset((US*)0xC00000, 0x42, 0x80000);	/* デバッグ用 */
			
			for(y=0; y < uHeight; y++)
			{
				pDstGR = (US *)(0xC00000 + ((dy + y) << 10) + (dx << 1));
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
		if(g_stCG_LIST[i].ubType != 0u)	/* スプライトライク */
		{
			G_Subtractive_Color(pSrcBuf, pDstBuf, uWidth, uHeight, uSize8x, i);	/* 減色処理 */
		}
		else		/* 通常の256色CG */
		{
			UI	x, y;
			
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
	volatile US *VIDEO_REG1 = (US *)0xE82400;
	volatile US *VIDEO_REG2 = (US *)0xE82500;
	volatile US *VIDEO_REG3 = (US *)0xE82600;
	volatile US *V_Sync_end = (US *)0xE8000E;

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
	WINDOW( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW, Y_MAX_DRAW);
	HOME(0, X_OFFSET, Y_OFFSET);
	HOME(1, X_OFFSET, Y_OFFSET);
//	HOME(2, X_OFFSET, 416);
//	HOME(3, X_OFFSET, 416);
	WIPE();
}

void G_Palette_INIT(void)
{
	UI	nPalette;

	/* グラフィックパレットの初期化 */
	for(nPalette=0; nPalette < 0xFF; nPalette++)
	{
		GPALET( nPalette, SetRGB(0, 0, 0));	/* Black */
	}
}

void G_Palette(void)
{
	GPALET( 0, SetRGB( 0,  0,  0));	/* Black */
//	GPALET( 1, SetRGB( 1,  1,  1));	/* Black */
#if 0
	GPALET( 0, SetRGB( 0,  0,  0));	/* Black */
	GPALET( 1, SetRGB(16, 16, 16));	/* Glay1 */
	GPALET( 2, SetRGB(15, 15, 15));	/* D-Glay */
	GPALET( 3, SetRGB( 0,  0, 31));	/* Blue */
	GPALET( 5, SetRGB(31,  0,  0));	/* Red */
	GPALET( 8, SetRGB( 0, 31,  0));	/* Green */
	GPALET( 9, SetRGB( 0, 30,  0));	/* Green */
	GPALET(10, SetRGB( 0, 29,  0));	/* Green */
	GPALET(11, SetRGB( 0, 28,  0));	/* Green */
	GPALET(12, SetRGB( 0, 27,  0));	/* Green */
	GPALET(13, SetRGB( 0, 26,  0));	/* Green */
	GPALET(14, SetRGB(16, 16, 16));	/* Glay2 */
	GPALET(15, SetRGB(31, 31, 31));	/* White */
#endif

#if 0
	Draw_Line(  0, 0, 255, 255, 15, 0xFFFF);	
	Draw_Line(255, 0,   0, 255, 15, 0xFFFF);	
	Draw_Box(   0, 0, 255, 255, 15, 0xFFFF);	
	Draw_Box( 127, 0, 255, 255, 15, 0xFFFF);	
	Draw_Fill(  0, 0, 127, 127, 3);	
	Draw_Fill(255, 0, 255, 127, 5);	
	Draw_Fill(  0, 128, 255, 255, 9);	
	Draw_Fill(255, 128, 255, 255, 15);	
	Draw_Circle(768/2,512/2,512/2,15,0,360,256);
#endif

}

SS G_Stretch_Pict(	SS dst_x, US dst_w, SS dst_y, US dst_h, UC ubDstScrn,
					SS src_x, US src_w, SS src_y, US src_h, UC ubSrcScrn)
{
	SS	ret = 0;
	US	*pDstGR,	*pSrcGR;
	UI	DstGR_H,	SrcGR_H;
	SS	dst_ex,	dst_ey;
	SS	src_ex,	src_ey;
	SS	x, y;
	US	rate_x, rate_y;

	dst_ex	= dst_x + dst_w;
	dst_ey	= dst_y + dst_h;
	src_ex	= src_x + src_w;
	src_ey	= src_y + src_h;

	/* 倍率演算 */
	rate_x = src_w / dst_w;
	rate_y = src_h / dst_h;

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
		pDstGR = (US *)(DstGR_H + ((y << 10) + (dst_x << 1)));

		pSrcGR = (US *)(SrcGR_H + ((((src_y + (y - dst_y)) * rate_y) << 10) + (src_x << 1)));
	
		for(x = dst_x; x < dst_ex; x++)
		{
			*pDstGR = *pSrcGR;
			
			pDstGR++;
			pSrcGR+=rate_x;
		}
	}
#else

#endif
	return	ret;
}

/* 画像のコピー */
/* 同じサイズの画像を任意の位置に描画する */
/* 表示先はx,y座標を画像の中心とした位置 */
SS G_BitBlt(SS dst_x, US dst_w, SS dst_y, US dst_h, UC ubDstScrn,
			SS src_x, US src_w, SS src_y, US src_h, UC ubSrcScrn,
			UC ubMode, UC ubV, UC ubH)
{
	SS	ret = 0;
	US	*pDstGR,	*pSrcGR;
	UI	DstGR_H,	SrcGR_H;
	SS	dst_ex,	dst_ey;
	SS	src_ex,	src_ey;
	SS	x, y;
	SS	x_min, y_min;
	SS	x_max, y_max;
//	US	rate_x, rate_y;

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
			x_min = X_OFFSET;
			x_max = X_MAX_DRAW - X_MAX_DRAW_OF;
			y_min = Y_MIN_DRAW;
			y_max = V_SYNC_MAX;
			break;
		}
		case 1:
		default:
		{
			x_min = X_OFFSET;
			x_max = X_MAX_DRAW - X_MAX_DRAW_OF;
			y_min = Y_OFFSET;
			y_max = Y_OFFSET + V_SYNC_MAX;
			break;
		}
		case 2:
		{
			x_min = X_OFFSET;
			x_max = X_MAX_DRAW;
			y_min = Y_MIN_DRAW;
			y_max = Y_MIN_DRAW + V_SYNC_MAX;
			break;
		}
	}

	if(dst_ex < x_min)		/* 画面外 */
	{
		return -1;
	}
	else if((dst_x < x_min) && (dst_ex >= x_min))
	{
		src_x += x_min - dst_x;
		dst_x = x_min;
	}
	else if((dst_x >= x_min) && (dst_ex > x_max))
	{
		src_ex -= dst_ex - X_MAX_DRAW - X_MAX_DRAW_OF;
		dst_ex = X_MAX_DRAW - X_MAX_DRAW_OF;
	}
	else if(dst_x > x_max)	/* 画面外 */
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
	else if((dst_y < y_min) && (dst_ey >= y_min))
	{
		src_y += y_min - dst_y;
		dst_y = y_min;
	}
	else if((dst_y < y_max) && (dst_ey > y_max))
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
		pDstGR = (US *)(DstGR_H + ((y << 10) + (dst_x << 1)));

		pSrcGR = (US *)(SrcGR_H + (((src_y + (y - dst_y)) << 10) + (src_x << 1)));
	
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
SI G_CLR(void)
{
	return _iocs_wipe();
}

SS G_CLR_AREA(SS x, US w, SS y, US h, UC Screen)
{
	SS	ret = 0;
	SS	i=0;
	UL	ulGR_H;
	UL	ulPoint;
	UI	unSize;
#if 0
	US	data[512] = {0};
	SI	nMode;
	UC *DMA_DCR;
	UC *DMA_GCR;
	
	struct _chain stSrcImage;
	struct _chain stDstImage[256];

	if(DMAMODE() != 0u)return -1;
#endif
#if 0
	volatile US *CRTC_R21 = (US *)0xE8002Au;	/* テキスト・アクセス・セット、クリアーP.S */
	volatile US *CRTC_480 = (US *)0xE80480u;	/* CRTC動作ポート */
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
	
	DMA_DCR = (UC *)0xE84084u;
	*DMA_DCR = Mbset(*DMA_DCR, ((UC)0b11000000), ((UC)0b10000000));	/* XRM(bit7,6) 10: */
	DMA_GCR = (UC *)0xE840FFu;
	*DMA_GCR = Mbset(*DMA_GCR, ((UC)0b0011), ((UC)0b0011));/* BR(bit1,0) 11:6.25% */
	
	DMAMOV_A(&stDstImage[0],	/* addr1 */
			&data[0],			/* addr2 */
			nMode,				/* mode */
//		Mmax((SI)h/16, 1));
			(SI)h);				/* len */
#endif

#if 0
	for(i=0; i<h; i++)
	{
//		DMAMOVE(&data[0],									/* addr1 */
//				(void *)(ulGR_H + ulPoint + (i * 0x400u)),	/* addr2 */
//				nMode,										/* mode */
//				(SI)unSize);								/* len */
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

SS G_CLR_ALL_OFFSC(UC bMode)
{
	SS	ret = 0;
	
	/* 描画可能枠再設定 */
	WINDOW( g_stCRT[bMode].hide_offset_x, 
			g_stCRT[bMode].hide_offset_y,
			g_stCRT[bMode].hide_offset_x + WIDTH,
			g_stCRT[bMode].hide_offset_y + 152);	
	/* 消去 */
	ret = G_CLR_AREA(g_stCRT[bMode].hide_offset_x, WIDTH, g_stCRT[bMode].hide_offset_y, 152, 1);	/* Screen1 消去 */

	return	ret;
}

SS G_Load(UC bCGNum, US uX, US uY, US uArea)
{
	SS	ret = 0;

	ret = APICG_DataLoad2G( g_stCG_LIST[bCGNum].bFileName, uX, uY, uArea);

	return	ret;
}

SS G_Load_Mem(UC bCGNum, US uX, US uY, US uArea)
{
	SS	ret = 0;

	US	*pDstGR, *pSrcGR;
	UI	DstGR_H;
	UI	Addr_Max;
	UI	uMaxNum;
	SS	x, y;
	UI	uWidth, uHeight, uFileSize;
	US	uSize8x = 0;
	SS	Pal_offset;

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
		pDstGR = (US *)(DstGR_H + (((uY + y) << 10) + (uX << 1)));
		if(Addr_Max <= (UI)pDstGR)
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
				if(Addr_Max > (UI)pDstGR)
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
SS APICG_DataLoad2G(SC *fname, UL pos_x, UL pos_y, US uArea)
{
	SS ret;

	US *GR;
	UC *file_buf = NULL, *work_buf = NULL;
	
	file_buf = (UC*)MyMalloc(PIC_FILE_BUF_SIZE);
	work_buf = (UC*)MyMalloc(PIC_WORK_BUF_SIZE);
	
	if(uArea != 0)
	{
		GR = (US *)0xC80000;	/* Screen1 */
	}
	else{
		GR = (US *)0xC00000;	/* Screen0 */
	}
	
	if( (file_buf == NULL) || (work_buf == NULL) )
	{
		/* メモリエラー */
		ret = -1;
	}
	else
	{
		ret = APICLOAD(	(US*)GR, 
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

SS APICG_DataLoad2M(UC uNum, UL pos_x, UL pos_y, US uArea, US *pDst)
{
	US *GR;
	UC *work_buf = NULL;
	UI	uMaxNum;
	SS ret = 0;
	
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
			GR = (US *)0xC00000;	/* Screen0 */
			break;
		case 1:
			GR = (US *)0xC80000;	/* Screen1 */
			break;
		}
	}
	else{
		GR = pDst;					/* メモリ */
		pos_x = 0;					/* 強制0 */
		pos_y = 0;					/* 強制0 */
	}
	
	{
		UI uWidth, uHeight, uFileSize;
		UC *pFileBuf;

		pFileBuf = (UC*)Get_CG_FileBuf(uNum);	/* ファイルバッファのポインタ取得 */
		Get_PicImageInfo(uNum, &uWidth, &uHeight, &uFileSize);	/* イメージ情報の取得 */

#ifdef DEBUG
//		printf("MAPIC1(%d:0x%p)=(0x%p)\n", uNum, pDst, pFileBuf);
//		printf("MAPIC2(%d:%d)=(%d,%d)(%d)\n", uNum, uArea, uWidth, uHeight, uFileSize );
#endif

//		work_buf = (UC*)MyMalloc( uHeight * (((uWidth+7)/8) * 8) );
		work_buf = (UC*)MyMalloc( PIC_WORK_BUF_SIZE );	/* 画面と同じ */
		if( work_buf == NULL )
		{
			/* メモリエラー */
			ret = -1;
		}
		else
		{
			//int M_APICLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE * );
			/* メモリからロード */
			ret = M_APICLOAD(	(US*)GR,			/* Load Ptr */
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

SS G_Subtractive_Color(US *pSrcBuf, US *pDstBuf, US uWidth, US uHeight, US uWidthEx, UI uNum)
{
	SS	ret = 0;
	UI	i, j, k, m;
	UI	x, y, z;
	UI	uAPICG_work_Size;
	UI	uSize8x = 0;
	US	*pBuf = NULL;
	US	col;
	UC	ubR=0,ubG=0,ubB=0;
	UC	ubNotUsePal[256];
	US	uColTbl[256];
	UC	ubConvPal;
	
	/* 均等量子化テーブル */
	UC	ubGen8_R[PIC_R];
	UC	ubGen8_G[PIC_G];
	UC	ubGen8_B[PIC_B];

	pBuf = pSrcBuf;
	uAPICG_work_Size = PIC_WORK_BUF_SIZE / 2;
	uSize8x = uWidthEx;
	z = (PIC_R * PIC_G * PIC_B) + 2;
	ubConvPal = g_stCG_LIST[uNum].ubTransPal;
	
	/* 除外パレット抽出準備 */
	for(j=0; j<256; j++)
	{
		ubNotUsePal[j] = j;
		uColTbl[j] = 0;
	}

	/* 減色カラーの対象を抽出 */
	for(j=0; j < (uAPICG_work_Size / 2); j++)
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

	pBuf = pSrcBuf;
	
	/* 減色カラーの設定 */
	ubGen8_R[0] = 0x01;
	for(j=1; j<PIC_R; j++)
	{
		ubGen8_R[j] = g_CG_MaxColor[uNum][COLOR_R] / (PIC_R-j);
	}
	ubGen8_G[0] = 0x01;
	for(j=1; j<PIC_G; j++)
	{
		ubGen8_G[j] = g_CG_MaxColor[uNum][COLOR_G] / (PIC_G-j);
	}
	ubGen8_B[0] = 0x01;
	for(j=1; j<PIC_B; j++)
	{
		ubGen8_B[j] = g_CG_MaxColor[uNum][COLOR_B] / (PIC_B-j);
	}
	
	m = 1;
	uColTbl[0] = SetRGB(0, 0, 0);	/* 透過 */
//	uColTbl[1] = SetRGB(0, 0, 0);	/* 特殊プライオリティ用 */
	for(i=0; i<PIC_R; i++)
	{
		for(j=0; j<PIC_G; j++)
		{
			for(k=0; k<PIC_B; k++)
			{
				uColTbl[m] = SetRGB(ubGen8_R[i], ubGen8_G[j], ubGen8_B[k]);
				m++;
//				uColTbl[m] = SetRGB(0, 0, 0);	/* 特殊プライオリティ用 */
//				m++;
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
				UC pal;
				
				pal = (UC)(*pBuf & 0x00FF);	/* 画像のパレット番号を取得 */

				ubNotUsePal[pal] = 0;	/* 使用したパレット */

				if(pal == ubConvPal)	/* 変換対象色 */
				{
					pal = TRANS_PAL;	/* 透過色 */
				}
				else
				{
					UC ubTmp;
					
					col = GPALET(pal, -1);	/* パレット番号からカラーコードを取得 */
					ubR = GetR(col);
					ubG = GetG(col);
					ubB = GetB(col);
					
					/* 均等量子化 */
					k = 0;
					ubTmp = McmpSub(ubGen8_R[0], ubR);
					for(m=1; m<PIC_R; m++)
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
					for(m=1; m<PIC_G; m++)
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
					for(m=1; m<PIC_B; m++)
					{
						if( ubTmp > McmpSub(ubGen8_B[m], ubB) )
						{
							ubTmp = McmpSub(ubGen8_B[m], ubB);
							k = m;
						}
					}
					ubB = ubGen8_B[k];
					
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

#endif	/* GRAPHIC_C */
