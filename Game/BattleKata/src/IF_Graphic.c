#ifndef	IF_GRAPHIC_C
#define	IF_GRAPHIC_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>

#include <usr_macro.h>
#include <apicglib.h>
#include "BIOS_CRTC.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"
#include "IF_Draw.h"
#include "IF_Math.h"

/* define定義 */
#define	PIC_FILE_BUF_SIZE	(524288ul)	/* 512*512*2 */
#define	PIC_WORK_BUF_SIZE	(524288ul)	/* 512*512*2 */
#define	PIC_R	(3)
#define	PIC_B	(3)
#define	PIC_G	(3)
#define	COLOR_R	(0)
#define	COLOR_G	(1)
#define	COLOR_B	(2)
#define	COLOR_MAX	(PIC_R * PIC_G * PIC_B)
#define	G_COLOR		(16)
#define	G_COLOR_SP	(1)	/* 特殊プライオリティとか */
#define	G_PAL_MAX	(256)	/* 256色モード中のパレット数 */

/* グローバル変数 */
volatile uint16_t *VIDEO_REG1 = (uint16_t *)0xE82400;
volatile uint16_t *VIDEO_REG2 = (uint16_t *)0xE82500;
volatile uint16_t *VIDEO_REG3 = (uint16_t *)0xE82600;

uint32_t	g_CG_List_Max						=	0u;
uint8_t		*g_pCG_FileBuf[CG_MAX]				=	{NULL};
uint16_t	g_CG_ColorCode[CG_MAX][G_PAL_MAX]	=	{0};
uint8_t		g_CG_MaxColor[CG_MAX][3]			=	{0};
uint8_t		g_CG_PalTbl[PAL_MAX_CG]				=	{0};

/* グローバル構造体 */
PICIMAGE	g_stPicImage[CG_MAX] = {0};
CG_LIST		g_stCG_LIST[CG_MAX] = {0};
struct APICGINFO	g_CGInfo[CG_MAX] = {0};

/* 関数のプロトタイプ宣言 */
int16_t	Get_CG_FileList_MaxNum(uint32_t *);
uint8_t	*Get_CG_FileBuf(uint8_t);
uint16_t	*Get_PicImageInfo(uint8_t , uint32_t *, uint32_t *, uint32_t *);
int16_t	Get_PicImagePallet(uint8_t);
int16_t Get_PicImagePalletTmp(uint16_t *);
int16_t Set_PicImagePalletTmp(uint16_t *);
int16_t	Set_PicImagePallet(uint8_t);
int16_t Set_PicImagePalletALL(void);
int16_t CG_File_Load(uint16_t);
int16_t CG_File_Load_to_Mem(uint16_t);
int16_t CG_Mem_Convert_Type(uint16_t);
void G_INIT(void);
void G_VIDEO_INIT(void);
void G_HOME(uint8_t);
void G_Palette_INIT(void);
void G_PaletteSetZero(void);
int16_t	G_Stretch_Pict( int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t, int16_t, uint16_t, uint8_t );
int16_t G_Stretch_Pict_toVRAM(int16_t, int16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t *, uint16_t, uint16_t, int8_t, uint8_t);
int16_t G_Stretch_Pict_To_Mem( uint16_t	*, uint16_t, uint16_t, uint16_t	*, uint16_t, uint16_t);
int16_t G_Copy_Pict_To_Mem(	uint16_t *, uint16_t , uint16_t , uint16_t *, uint16_t , uint16_t);
int16_t	G_BitBlt(int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t , int16_t , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
int16_t G_BitBlt_From_Mem(	int16_t, int16_t , uint8_t , uint16_t *, uint16_t , uint16_t , uint8_t , uint8_t , uint8_t, uint8_t);
int32_t	G_CLR(void);
int16_t G_CLR_HS(void);
int16_t	G_CLR_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t);
int16_t	G_CLR_ALL_OFFSC(void);
int16_t	G_FILL_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t, uint8_t);
int16_t	G_Load(uint8_t, uint16_t, uint16_t, uint16_t);
int16_t	G_Load_Mem(uint8_t, int16_t, int16_t, uint16_t);
int16_t	APICG_DataLoad2G(int8_t *, uint64_t, uint64_t, uint16_t);
int16_t	APICG_DataLoad2M(uint8_t , uint64_t, uint64_t, uint16_t, uint16_t *);
int16_t	G_Subtractive_Color(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t, uint32_t);
int16_t	PutGraphic_To_Text(uint8_t , uint16_t , uint16_t );
int16_t	PutGraphic_To_Symbol12(uint8_t *, uint16_t , uint16_t , uint16_t );
int16_t	PutGraphic_To_Symbol16(uint8_t *, uint16_t , uint16_t , uint16_t );
int16_t	PutGraphic_To_Symbol24(uint8_t *, uint16_t , uint16_t , uint16_t );
int16_t	G_Scroll(int16_t, int16_t, uint8_t);
int16_t	G_ClipAREA_Chk_Width(	int16_t *, int16_t *, int16_t *, uint16_t , uint8_t , uint8_t);
int16_t	G_ClipAREA_Chk_Height(	int16_t *, int16_t *, int16_t *, uint16_t , uint8_t , uint8_t);
int16_t G_MedianCut(uint16_t *, uint16_t *, uint16_t , uint16_t , uint16_t, uint8_t, uint8_t);
void G_MedianCut_Gen(YUV* , int32_t , int32_t , int32_t );
void G_APIC_to_Mem(uint16_t *, uint16_t *, uint16_t uWidth, uint16_t uHeight);
int16_t G_Pal_Set(uint8_t);
int16_t G_Pal_Get(uint8_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Get_CG_FileList_MaxNum(uint32_t *uMaxNum)
{
	int16_t	ret = 0;
	
	*uMaxNum = g_CG_List_Max;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint8_t *Get_CG_FileBuf(uint8_t bNum)
{
	uint8_t *ret = 0;
	
	ret = g_pCG_FileBuf[bNum];
//	printf("Get(%d,0x%p,0x%p)\n", bNum, g_pCG_FileBuf[bNum], ret );
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
uint16_t *Get_PicImageInfo(uint8_t bNum, uint32_t *pWidth, uint32_t *pHeight, uint32_t *pFileSize)
{
	uint16_t	*pRet = NULL;
	BITMAPFILEHEADER *pFile;
	BITMAPINFOHEADER *pInfo;
	
	if(g_CG_List_Max > bNum)
	{
		pRet = g_stPicImage[bNum].pImageData;
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
	
	return pRet;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Get_PicImagePallet(uint8_t bNum)
{
	int16_t	ret = 0;
	uint32_t	i;

	/* パレットの取得 */
	for(i=0; i<G_PAL_MAX; i++)
	{
		g_CG_ColorCode[bNum][i] = GPALET( i, -1 );	/* 現在の設置を保存 */
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
int16_t Get_PicImagePalletTmp(uint16_t *G_Pal_Tmp)
{
	int16_t	ret = 0;
	uint32_t	i;

	/* パレットの取得 */
	for(i=0; i<G_PAL_MAX; i++)
	{
		*G_Pal_Tmp = GPALET( i, -1 );	/* 現在の設置を保存 */
		G_Pal_Tmp++;
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
int16_t Set_PicImagePalletTmp(uint16_t *G_Pal_Tmp)
{
	int16_t	ret = 0;
	uint32_t	i;

	/* パレットの取得 */
	for(i=0; i<G_PAL_MAX; i++)
	{
		GPALET( i, *G_Pal_Tmp );	/* バックアップの書き戻し */
		G_Pal_Tmp++;
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
int16_t Set_PicImagePallet(uint8_t bNum)
{
	int16_t	ret = 0;
	uint32_t	i;
	uint32_t	offset_val;
	
	/* パレットの設定 */
	switch(g_stCG_LIST[bNum].ubType)
	{
		case 1:	/* スプライトライク */
		{
			/* グラフィックパレット 0～15は、CG_LISTと１対１の関係 */
			offset_val = G_Pal_Get(bNum) * G_COLOR * G_COLOR_SP;	/* G_COLOR色分のオフセット値を出す */
			for(i=0; i< G_COLOR; i++)
			{
				GPALET( offset_val + i, g_CG_ColorCode[bNum][i] );
			}
			break;
		}
		case 2:	/* テキスト描画 */
		case 3:	/* テキスト描画(グレイスケール) */
		{
			offset_val = 8;	/* テキストパレット8パターン目から15パターン目まで使うので(0～7は予約領域) */
			for(i=0; i< 8; i++)	/* 8～15番目を使う */
			{
				TPALET2( offset_val + i, g_CG_ColorCode[bNum][i+1] );
			}
			break;
		}
		default:
		{
			/* 全パレットを使う画像 */
			offset_val = 0;
			for(i=0; i<G_PAL_MAX; i++)
			{
				GPALET( i, g_CG_ColorCode[bNum][i] );
			}
			break;
		}
	}
	
	ret = offset_val;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Set_PicImagePalletALL(void)
{
	int16_t	ret = 0;
	int16_t	i = 0;
	
	for(i=0; i<CG_MAX; i++)
	{
		Set_PicImagePallet(i);
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
int16_t CG_File_Load(uint16_t uListNum)
{
	int16_t	ret = 0;
	
	uint16_t	uPalTmp[G_PAL_MAX];
	
	if(uListNum >= g_CG_List_Max)
	{
		return -1;
	}
	
	Get_PicImagePalletTmp(&uPalTmp[0]);	/* 現在のパレットを退避 */
#ifdef DEBUG
//	printf("Get_PicImagePalletTmp = %d\n", uListNum );
#endif

	ret = CG_File_Load_to_Mem(uListNum);		/* ファイルリストからメモリにPICを展開する */
#ifdef DEBUG
//	printf("CG_File_Load_to_Mem = %d\n", uListNum );
#endif
	if(ret != 0)return ret;
	
	CG_Mem_Convert_Type(uListNum);		/* メモリに展開された画像をType別に変換する */
#ifdef DEBUG
//	printf("CG_Mem_Convert_Type = %d\n", uListNum );
#endif
	
	Set_PicImagePalletTmp(&uPalTmp[0]);	/* 退避したパレットを戻す */
#ifdef DEBUG
//	printf("Set_PicImagePalletTmp = %d\n", uListNum );
#endif
	Set_PicImagePallet(uListNum);		/* パレットをセットする */
#ifdef DEBUG
//	printf("Set_PicImagePallet = %d\n", uListNum );
#endif

	//G_PaletteSetZero();					/* 念のため 0番パレット変更 */
#ifdef DEBUG
//	printf("G_PaletteSetZero = %d\n", uListNum );
#endif
	
#ifdef DEBUG
//	printf("CG File Load 完了 = %d\n", uListNum );
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
int16_t CG_File_Load_to_Mem(uint16_t uListNum)
{
	int16_t	ret = 0;
	uint16_t	i;
	int32_t	FileSize;
	
	i = uListNum;
	
	if(uListNum >= g_CG_List_Max)
	{
		return -1;
	}

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
			return -1;
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
		if( g_stPicImage[i].pImageData != NULL )
		{
			return -1;	/* 既に確保済み */
		}
		/* メモリ確保 */
		g_stPicImage[i].pImageData = (uint16_t*)MyMalloc( Size );	/* メモリの確保 */
		if( g_stPicImage[i].pImageData == NULL )
		{
			printf("error:CG File %2d = %s\n", i, g_stCG_LIST[i].bFileName );
			return -1;
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
			return -1;
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
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t CG_Mem_Convert_Type(uint16_t uListNum)
{
	int16_t	ret = 0;
	uint32_t	i;
	
	i = uListNum;
	
	if(uListNum >= g_CG_List_Max)
	{
		return -1;
	}
	
	{
		uint16_t	*pBuf = NULL;
		uint16_t	*pSrcBuf = NULL;
		uint16_t	*pDstBuf = NULL;
		uint32_t	uWidth, uHeight;
		uint32_t	uSize8x = 0;
		uint32_t	uAPICG_work_Size;
		int32_t		Res;
		uint8_t		ubGpal = 0u;

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
			return -1;
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
			return -1;
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
			return -1;
		}
		/* 作業用ポインタ */
		pDstBuf = g_stPicImage[i].pImageData; 
#ifdef DEBUG
//		printf("Load5(%d,0x%p, 0x%p)\n", i, g_stPicImage[i].pImageData, pDstBuf);
//		KeyHitESC();	/* デバッグ用 */
#endif

#if 0
		/* 作業用ポインタ */
		pDstBuf = g_stPicImage[i].pImageData; 
		
		G_APIC_to_Mem(pSrcBuf, pDstBuf, uWidth, uHeight);	/* APIC画像を使いやすいBITMAP似のメモリに展開 */
		
		/* メモリ操作 */
		if(pSrcBuf != NULL)
		{
			MyMfree(pSrcBuf);	/* メモリ解放 */
		}
		pSrcBuf = (uint16_t*)MyMalloc( uSize8x * uHeight * sizeof(uint16_t));	/* メモリの確保 */
#endif
		
#ifdef DEBUG
//		printf("debug2(0x%p)=(%d,%d)\n", pDstBuf, uWidth, uHeight );
//		printf("debug3(0x%p)=(%d,%d)\n", pSrcBuf, uWidth, uHeight );
//		KeyHitESC();	/* デバッグ用 */
#endif

#if 0
		G_BitBlt_From_Mem(0, 0, 0, pDstBuf, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);
		G_MedianCut(pSrcBuf, pDstBuf, uWidth, uHeight, 16);
		G_BitBlt_From_Mem(0, 66, 0, pSrcBuf, uWidth, uHeight, 0xFF, POS_LEFT, POS_TOP);
#endif
		switch(g_stCG_LIST[i].ubType)
		{
			case 1:	/* スプライトライク */
			{
				uint8_t str[256] = {0};
				sprintf(str, "data\\cg\\OK_tmp_G%02d.G16", i );
				
				G_Pal_Set(i);	/* 対象画像のパレットを設定 */
				
				if(File_Load( str, g_stPicImage[i].pImageData, sizeof(uint16_t), uSize8x * uHeight) < 0)
				{
					/* ファイルがない場合 */
					G_APIC_to_Mem(pSrcBuf, pDstBuf, uWidth, uHeight);	/* APIC画像を使いやすいBITMAP似のメモリに展開 */
					G_MedianCut(pDstBuf, pDstBuf, uWidth, uHeight, 16, i, g_stCG_LIST[i].ubTransPal);	/* 256→16色 減色処理 */
					File_Save( str, g_stPicImage[i].pImageData, sizeof(uint16_t), uSize8x * uHeight );
					ubGpal = 1u;	/* パレット保存フラグセット */
				}
				else
				{
					/* ファイルがある場合 */
					ubGpal = 2u;	/* パレット保存フラグセット */
				}
				
				break;
			}
			case 2:	/* テキスト描画 */
			case 3:	/* テキスト描画(グレイスケール) */
			{
				G_Subtractive_Color(pSrcBuf, pDstBuf, uWidth, uHeight, uSize8x, i);	/* 減色処理 */
//				G_APIC_to_Mem(pSrcBuf, pDstBuf, uWidth, uHeight);	/* APIC画像を使いやすいBITMAP似のメモリに展開 */
//				G_MedianCut(pDstBuf, pDstBuf, uWidth, uHeight, 4);	/* 256→4色 減色処理 */
				break;
			}
			default:	/* 通常の256色CG */	
			{
				G_APIC_to_Mem(pSrcBuf, pDstBuf, uWidth, uHeight);	/* APIC画像を使いやすいBITMAP似のメモリに展開 */
				break;
			}
		}
		
#ifdef DEBUG
//		printf("Load6(%d,0x%p, 0x%p, 0x%p)\n", i, g_stPicImage[i].pImageData, pBuf, pSrcBuf);
//		KeyHitESC();	/* デバッグ用 */
#endif
		/* パレット */
		switch(ubGpal)
		{
			case 0:
			default:
			{
		//		G_PaletteSetZero();	/* 0番パレット変更 */
				Get_PicImagePallet(i);	/* パレットを保存 */
				break;
			}
			case 1:
			{
				uint8_t str[256] = {0};
				sprintf(str, "data\\cg\\OK_tmp_G%02d.P16", i );
				
				Get_PicImagePallet(i);	/* パレットを保存 */
				
				File_Save( str, &g_CG_ColorCode[i][0], sizeof(uint16_t), 16 );	/* パレットセーブ */
				break;
			}
			case 2:
			{
				uint8_t str[256] = {0};
				sprintf(str, "data\\cg\\OK_tmp_G%02d.P16", i );
				
				File_Load( str, &g_CG_ColorCode[i][0], sizeof(uint16_t), 16 );	/* パレットロード */
				break;
			}
		}
		
		/* メモリ操作 */
		if(pSrcBuf != NULL)
		{
			MyMfree(pSrcBuf);	/* メモリ解放 */
		}
#ifdef DEBUG
//		printf("CG File %2d = %s(x:%d,y:%d)\n", i, g_stCG_LIST[i].bFileName, uWidth, uHeight );
//		puts("========================");
//		KeyHitESC();	/* デバッグ用 */
#endif
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
void G_INIT(void)
{
	int32_t	ret;
	int16_t	i;
	
	_iocs_g_clr_on();			/* グラフィックのクリア */
	G_CLR();					/* クリッピングエリア全開＆消す */
	ret = _iocs_apage(1);		/* グラフィックの書き込み(裏) */
	if(ret < 0)printf("error(%d):_iocs_apage\n", ret);
	ret = _iocs_apage(0);		/* グラフィックの書き込み(表) */
	if(ret < 0)printf("error(%d):_iocs_apage\n", ret);
	ret = _iocs_vpage(0b0011);	/* グラフィック表示(page3:0ff page2:0ff page1:0n page0:0n) */
	if(ret < 0)printf("error(%d):_iocs_vpage\n", ret);
//	G_HOME(0);				/* ホーム位置設定 */
	G_VIDEO_INIT();			/* ビデオコントローラーの初期化 */
	
	for(i=0; i < PAL_MAX_CG; i++)
	{
		g_CG_PalTbl[i] = 0xFF;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void G_VIDEO_INIT(void)
{
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
	*VIDEO_REG2 = Mbset(*VIDEO_REG2, 0x3FFF, 0b0000011011100100);	/* 優先順位 TX>GR>SP GR0>GR1>GR2>GR3 */
//											   |||||||||||||||+-bit0 0	
//											   ||||||||||||||+--bit1 0	
//											   |||||||||||||+---bit2 1	
//											   ||||||||||||+----bit3 0	
//											   |||||||||||+-----bit4 0	
//											   ||||||||||+------bit5 1	
//											   |||||||||+-------bit6 1	
//											   ||||||||+--------bit7 1	
//											   ||||||||					優先順位 GR0>GR1>GR2>GR3
//											   |||||||+---------bit8 GR	0
//											   ||||||+----------bit9 GR	1
//											   |||||+-----------bitA TX	1
//											   ||||+------------bitB TX	0
//											   |||+-------------bitC SP	0
//											   ||+--------------bitD SP	0
//											   ||						優先順位 SP>TX>GR
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
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void G_HOME(uint8_t bSW)
{
	int16_t	x, y;
	uint8_t bMode = 0;
	ST_CRT	stCRT = {0};

	GetCRT(&stCRT, bMode);
	if(bSW == 0)	/* 画面をフリップする */
	{
		/* オフスクリーン座標 */
		x = stCRT.hide_offset_x;
		y = stCRT.hide_offset_y;
	}
	else
	{
		/* 表示座標 */
		x = stCRT.view_offset_x;
		y = stCRT.view_offset_y;
	}
	_iocs_home( 0b0001, x, y );	/* オフ・スクリーン側へ切替 */
	
//	_iocs_home(0b0000, X_OFFSET, Y_OFFSET);	/* ホーム位置 */
//	_iocs_home(1, X_OFFSET, Y_OFFSET);
//	_iocs_home(2, X_OFFSET, 416);
//	_iocs_home(3, X_OFFSET, 416);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void G_Palette_INIT(void)
{
	uint32_t	nPalette;

	/* グラフィックパレットの初期化 */
	for(nPalette=0; nPalette < 0xFF; nPalette++)
	{
		GPALET( nPalette, SetRGB(0, 0, 0));	/* Black */
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void G_PaletteSetZero(void)
{
	int16_t i=0;
	
	/* 透過色 */
	GPALET( i++, SetRGB( 0,  0,  0));	/* 0:Black */
	/* １～１５を共通カラーとする */
	GPALET( i++, SetRGB(  1,  1,  1));	/* 1 */
	GPALET( i++, SetRGB( 15, 15, 15));	/* 2 */
	GPALET( i++, SetRGB( 31, 31, 31));	/* 3 */
	GPALET( i++, SetRGB( 15,  0,  0));	/* 4 */
	GPALET( i++, SetRGB(  0, 15,  0));	/* 5 */
	GPALET( i++, SetRGB( 15, 15,  0));	/* 6 */
	GPALET( i++, SetRGB(  0,  0, 15));	/* 7 */
	GPALET( i++, SetRGB( 15,  0, 15));	/* 8 */
	GPALET( i++, SetRGB(  0, 15, 15));	/* 9 */
	GPALET( i++, SetRGB( 31,  0,  0));	/* A */
	GPALET( i++, SetRGB(  0, 31,  0));	/* B */
	GPALET( i++, SetRGB( 31, 31,  0));	/* C */
	GPALET( i++, SetRGB(  0,  0, 31));	/* D */
	GPALET( i++, SetRGB( 31,  0, 31));	/* E */
	GPALET( i++, SetRGB(  0, 31, 31));	/* F */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* VRAMからVRAMに展開 */
int16_t G_Stretch_Pict(
			int16_t dst_x, uint16_t dst_w, int16_t dst_y, uint16_t dst_h, uint8_t ubDstScrn,
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
	if(dst_ex >= X_MAX_DRAW)
	{
		dst_ex = X_MAX_DRAW;
	}
	dst_ey	= dst_y + dst_h;
	if(dst_ey >= Y_MAX_DRAW)
	{
		dst_ey = Y_MAX_DRAW;
	}
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
		pDstGR = (uint16_t *)(DstGR_H + (Mmul1024(y) + Mmul2(dst_x)));

		pSrcGR = (uint16_t *)(SrcGR_H + (Mmul1024((src_y + Mmul_1p25(y - dst_y))) + Mmul2(src_x)));
		pSrcGR_tmp = pSrcGR;
	
		for(x = dst_x; x < dst_ex; x++)
		{
			pSrcGR = pSrcGR_tmp + Mmul_1p25(x-dst_x);
			if(*pSrcGR != 0)
			{
				*pDstGR = *pSrcGR;
			}
			pDstGR++;
		}
	}
#else

#endif
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* メモリから直接VRAMに展開 */
int16_t G_Stretch_Pict_toVRAM(	int16_t dst_x, int16_t dst_y, uint8_t ubDstScrn,
								uint8_t ubMode, uint8_t ubH, uint8_t ubV,
								uint16_t *pSrcBuf, uint16_t src_w, uint16_t src_h, int8_t bZoom, uint8_t bCGNum)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint16_t	*pSrcGR_tmp;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t dst_w, dst_h;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_x,	src_y;
	int16_t	src_ex,	src_ey;
	int16_t	x_min,	x_max;
	int16_t	y_min,	y_max;
	int16_t	x, y;
	int16_t	cal_x, cal_y;
	int16_t	dst_z;
	int16_t	src_z;
	int16_t	Pal_offset;

	int16_t (*pFunc[])(int16_t) = {
		APL_mul_0p52,	/* 0 -9 */
		APL_mul_0p54,	/* 1 -8 */
		APL_mul_0p58,	/* 2 -7 */
		APL_mul_0p62,	/* 3 -6 */
		APL_mul_0p66,	/* 4 -5 */
		APL_mul_0p71,	/* 5 -4 */
		APL_mul_0p77,	/* 6 -3 */
		APL_mul_0p83,	/* 7 -2 */
		APL_mul_0p91,	/* 8 -1 */
		APL_mul_1p00,	/* 9  0 */
		APL_mul_1p10,	/* 10 1 */
		APL_mul_1p20,	/* 11 2 */
		APL_mul_1p30,	/* 12 3 */
		APL_mul_1p40,	/* 13 4 */
		APL_mul_1p50,	/* 14 5 */
		APL_mul_1p60,	/* 15 6 */
		APL_mul_1p70,	/* 16 7 */
		APL_mul_1p80,	/* 17 8 */
		APL_mul_1p90	/* 18 9 */
	};

	if(pSrcBuf == NULL)	return -1;
	
	if(bZoom < -9) bZoom = -9;
	if(bZoom > 9) bZoom = 9;
	dst_z = bZoom + 9;
	src_z = 9 - bZoom;
	
	dst_w = (*pFunc[dst_z])(src_w);
	dst_w = (((dst_w+7)/8) * 8);	/* 8の倍数 */
	dst_h = (*pFunc[dst_z])(src_h);
	
	/* 描画したいエリアから完全にはみ出てるかどうかチェック */
	ret = G_ClipAREA_Chk_Width(&dst_x, &x_min, &x_max, dst_w, ubMode, ubH);
	if(ret != 0)return ret;
	
	ret = G_ClipAREA_Chk_Height(&dst_y, &y_min, &y_max, dst_h, ubMode, ubV);
	if(ret != 0)return ret;
	
	dst_ex	= dst_x + dst_w;
	dst_ey	= dst_y + dst_h;
	
	src_x = 0;
	src_y = 0;
	src_w = (((src_w+7)/8) * 8);	/* 8の倍数 */
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
	
	SrcGR_H = (uint32_t)pSrcBuf;
	Pal_offset = Set_PicImagePallet(bCGNum);	/* パレットを設定 */
#ifdef DEBUG
//	printf("d(%d, %d)(%d, %d)(%d, %d)\n", dst_x, dst_y, dst_w, dst_h, dst_ex, dst_ey);
//	printf("s(%d, %d)(%d, %d)(%d, %d)\n", src_x, src_y, src_w, src_h, src_ex, src_ey);
//	KeyHitESC();	/* デバッグ用 */
#endif
	
#if 1
	for(y = dst_y; y < dst_ey; y++)
	{
		if(y < y_min)continue;
		if(y >= y_max)break;
		
		/* アドレス算出 */
		pDstGR = (uint16_t *)(DstGR_H + Mmul2(Mmul512(y) + dst_x));

		cal_y = src_y + (*pFunc[src_z])(y - dst_y);
		if(cal_y >= src_ey)break;
		
		pSrcGR = (uint16_t *)(SrcGR_H + Mmul2(cal_y * src_w)) ;
		pSrcGR_tmp = pSrcGR;
	
		for(x = dst_x; x < dst_ex; x++)
		{
			if(x < x_min)
			{
				pDstGR++;
				continue;
			}
			if(x >= x_max)break;
			
			cal_x = (*pFunc[src_z])(x-dst_x);
			if(cal_x >= src_ex)break;
			
			pSrcGR = pSrcGR_tmp + cal_x;
			if(*pSrcGR != 0)	/* 透過色以外 */
			{
				*pDstGR = *pSrcGR + Pal_offset;
			}
			pDstGR++;
		}
	}
#else

#endif
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* メモリからメモリに展開 */
int16_t G_Stretch_Pict_To_Mem(	uint16_t *pDstBuf, uint16_t dst_w, uint16_t dst_h,
								uint16_t *pSrcBuf, uint16_t src_w, uint16_t src_h)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint16_t	*pSrcGR_tmp;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	x, y;
	int16_t	sx, sy;
	uint32_t	uSize8x_dst = 0;
	uint32_t	uSize8x_src = 0;
	
	if(pDstBuf == NULL)return -1;
	if(pSrcBuf == NULL)return -1;
	if(src_w == 0u)return -1;
	if(dst_w == 0u)return -1;
	if(dst_h == 0u)return -1;
	if(src_h == 0u)return -1;

	DstGR_H = (uint32_t)pDstBuf;
	SrcGR_H = (uint32_t)pSrcBuf;

	uSize8x_dst = (((dst_w+7)/8) * 8);	/* 8の倍数 */
	uSize8x_src = (((src_w+7)/8) * 8);	/* 8の倍数 */

	for(y = 0; y < dst_h; y++)
	{
		sy = Mmul_1p25(y);
		if(sy >= src_h)
		{
			break;
		}
		/* アドレス算出 */
		pDstGR = (uint16_t *)( DstGR_H + Mmul2( y * uSize8x_dst) );
		pSrcGR = (uint16_t *)( SrcGR_H + Mmul2(sy * uSize8x_src) );
		pSrcGR_tmp = pSrcGR;
	
		for(x = 0; x < dst_w; x++)
		{
			sx = Mmul_1p25(x);
			if(sx < src_w)
			{
				pSrcGR = pSrcGR_tmp + sx;
				if(*pSrcGR != 0)
				{
					*pDstGR = *pSrcGR;
				}
			}
			pDstGR++;
		}
	}
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* メモリからメモリに展開 */
int16_t G_Copy_Pict_To_Mem(	uint16_t *pDstBuf, uint16_t dst_w, uint16_t dst_h,
							uint16_t *pSrcBuf, uint16_t src_w, uint16_t src_h)
{
	int16_t	ret = 0;
	int16_t	y;
	uint16_t	*pDstGR,	*pSrcGR;
	uint32_t	uSize8x_dst = 0;
	
	if(pDstBuf == NULL)return -1;
	if(pSrcBuf == NULL)return -1;
	if(src_w == 0u)return -1;
	if(dst_w == 0u)return -1;
	if(dst_h == 0u)return -1;
	if(src_h == 0u)return -1;
	if(src_w != dst_w)return -1;
	if(src_h != dst_h)return -1;

	uSize8x_dst = (((dst_w+7)/8) * 8) * dst_h;	/* 8の倍数 */

	/* アドレス算出 */
	pDstGR = pDstBuf;
	pSrcGR = pSrcBuf;
	
	for(y = 0; y < uSize8x_dst; y++)
	{
		/* アドレス算出 */
		*pDstGR = *pSrcGR;
		pDstGR++;
		pSrcGR++;
	}
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* 画像のコピー */
/* 同じサイズの画像を任意の位置に描画する */
/* 表示先はx,y座標を画像の中心とした位置 */
int16_t G_BitBlt(int16_t dst_x, uint16_t dst_w, int16_t dst_y, uint16_t dst_h, uint8_t ubDstScrn,
			int16_t src_x, uint16_t src_w, int16_t src_y, uint16_t src_h, uint8_t ubSrcScrn,
			uint8_t ubMode, uint8_t ubH, uint8_t ubV)
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
		{
			dst_y	= dst_y - (dst_h>>1);
			break;
		}
		case POS_BOTTOM:
		{
			dst_y	= dst_y - dst_h;
			break;
		}
		default:
		{
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
		{
			dst_x	= dst_x - (dst_w>>1);
			break;
		}
		case POS_RIGHT:
		{
			dst_x	= dst_x - dst_w;
			break;
		}
		default:
		{
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
		default:
		{
			x_min = X_MIN_DRAW;
			x_max = X_MAX_DRAW;
			y_min = Y_MIN_DRAW;
			y_max = Y_MAX_DRAW;
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* メモリから画像へ */
/* 同じサイズの画像を任意の位置に描画する */
/* 表示先はx,y座標を画像の中心とした位置 */
int16_t G_BitBlt_From_Mem(	int16_t dst_x, int16_t dst_y, uint8_t ubDstScrn,
							uint16_t *pSrcImage, uint16_t src_w, uint16_t src_h,
							uint8_t ubMode, uint8_t ubH, uint8_t ubV, uint8_t bCGNum)
{
	int16_t	ret = 0;
	uint16_t	*pDstGR,	*pSrcGR;
	uint32_t	DstGR_H,	SrcGR_H;
	int16_t	dst_ex,	dst_ey;
	int16_t	src_ex,	src_ey;
	int16_t	x, y;
	int16_t	x_min, y_min;
	int16_t	x_max, y_max;
	int16_t	Pal_offset;
	
	/* 描画したいエリアから完全にはみ出てるかどうかチェック */
	ret = G_ClipAREA_Chk_Width(&dst_x, &x_min, &x_max, src_w, ubMode, ubH);
	if(ret != 0)return ret;
	
	ret = G_ClipAREA_Chk_Height(&dst_y, &y_min, &y_max, src_h, ubMode, ubV);
	if(ret != 0)return ret;
	
	dst_ex	= dst_x + src_w;
	dst_ey	= dst_y + src_h;
	src_ex	= (((src_w+7)/8) * 8);	/* 8の倍数 */
	src_ey	= src_h;
	
	/* アドレス算出 */
	if(ubDstScrn != 0)
	{
		DstGR_H = 0xC80000;	/* Screen1 */
	}
	else{
		DstGR_H = 0xC00000;	/* Screen0 */
	}

	SrcGR_H = (uint32_t)pSrcImage;	/* Mem */
	
	Pal_offset = Set_PicImagePallet(bCGNum);	/* パレットを設定 */
	
	for(y = dst_y; y < dst_ey; y++)
	{
		if(y < y_min)continue;	/* 描画エリア外（上） */
		if(y >= y_max)break;	/* 描画エリア外（下） */
		
		/* アドレス算出 */
		pDstGR = (uint16_t *)(DstGR_H + Mmul2(Mmul512(y) + dst_x));
		pSrcGR = (uint16_t *)(SrcGR_H + Mmul2((y - dst_y) * src_ex));
#ifdef DEBUG
//		printf("debug3(0x%p)=(%d,%d)\n", pSrcGR, (y - dst_y), ((y - dst_y) * src_ex) );
//		KeyHitESC();	/* デバッグ用 */
#endif
		for(x = dst_x; x < dst_ex; x++)
		{
			if( (x >= x_min) && (x < x_max) )
			{
				if(*pSrcGR != 0)
				{
					*pDstGR = *pSrcGR + Pal_offset;
				}
			}
			pDstGR++;
			pSrcGR++;
#ifdef DEBUG
//			printf("debug4(0x%p)=(%d,%d)\n", pSrcGR, x, y );
//			KeyHitESC();	/* デバッグ用 */
#endif
		}
	}

#ifdef DEBUG
	Draw_Box( dst_x, dst_y, dst_ex, dst_ey, 8, 0xFFFF);
#endif
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* 画面のクリア */
int32_t G_CLR(void)
{
	_iocs_window( X_MIN_DRAW, Y_MIN_DRAW, X_MAX_DRAW-1, Y_MAX_DRAW-1);
	return _iocs_wipe();
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t G_CLR_HS(void)	/* 現在表示している領域をクリアする */
{
	int16_t	ret = 0;
	
	volatile uint16_t *CRTC_R21  = (uint16_t *)0xE8002Au;	/* テキスト・アクセス・セット、クリアーP.S */
	volatile uint16_t *CRTC_mode = (uint16_t *)0xE80480u;	/* CRTC動作ポート */

	/* 必ずテキスト表示処理の後に行うこと */
	/* クリアにかかる時間は1/55fpsとのこと@Tea_is_Appleさんより */
//	wait_v_sync();
	*CRTC_R21  = 0x03u;	/* SCREEN1 高速クリアOFF(bit3:0 bit2:0) / SCREEN0 高速クリアON(bit1:1 bit0:1) */
	*CRTC_mode = 0x02u;	/* クリア実行 */
//	*CRTC_R21  = Mbset(*CRTC_R21,  0x0Fu, 0x0Cu);	/* SCREEN1 高速クリアON / SCREEN0 高速クリアOFF */
//	*CRTC_mode = Mbset(*CRTC_mode, 0x02u, 0x02u);	/* クリア実行 */
//	wait_v_sync();
		
	while(!(*CRTC_mode) & 0x02);	/* 消去前ウェイト */
	while((*CRTC_mode) & 0x02);		/* 消去中ウェイト */
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t G_CLR_AREA(int16_t x, uint16_t w, int16_t y, uint16_t h, uint8_t Screen)
{
	int16_t	ret = 0;
#if 1	/* memset */
	int16_t	i=0;
	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	uint32_t	unSize;
#endif
	
#if 0	/* DMA */
	uint16_t	data[512] = {0};
	int32_t	nMode;
	uint8_t *DMA_DCR;
	uint8_t *DMA_GCR;
	
	struct _chain stSrcImage;
	struct _chain stDstImage[256];

	if(DMAMODE() != 0u)return -1;
#endif

#if 1	/* memset */
	switch(Screen)
	{
		case 0:
		{
			ulGR_H = 0xC00000;	/* Screen0 */
			break;
		}
		case 1:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
		default:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
	}
	
	unSize = (w << 1);
	ulPoint = (y << 10u) + (x << 1);
#endif

#if 0	/* DMA */
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

#if 0	/* DMA */
	for(i=0; i<h; i++)
	{
//		DMAMOVE(&data[0],									/* addr1 */
//				(void *)(ulGR_H + ulPoint + (i * 0x400u)),	/* addr2 */
//				nMode,										/* mode */
//				(int32_t)unSize);								/* len */
	}
#endif

#if 1	/* memset */
	for(i=0; i<h; i++)
	{
		memset((void *)(ulGR_H + ulPoint + (i << 10u)), 0x00u, unSize);
	}
#endif

	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t G_CLR_ALL_OFFSC(void)
{
	int16_t	ret = 0;
	uint8_t bMode = 0;
	
	ST_CRT	stCRT = {0};
	
	GetCRT(&stCRT, bMode);
	
	/* 描画可能枠再設定 */
	_iocs_window(	stCRT.hide_offset_x, 
					stCRT.hide_offset_y,
					stCRT.hide_offset_x + WIDTH,
					stCRT.hide_offset_y + Y_MAX_WINDOW);	
	/* 消去 */
#if	1	/* IOCS */
	ret = _iocs_wipe();	/* クリア */
#else
	ret = G_CLR_AREA(stCRT.hide_offset_x, WIDTH, stCRT.hide_offset_y, Y_MAX_WINDOW, 0);	/* Screen0 消去 */
#endif
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t G_FILL_AREA(int16_t x, uint16_t w, int16_t y, uint16_t h, uint8_t Screen, uint8_t bPal)
{
	int16_t	ret = 0;
	int16_t	i=0;
	uint64_t	ulGR_H;
	uint64_t	ulPoint;
	uint32_t	unSize;

	_iocs_window( x, y, x + w, y + h);	/* 描画可能枠再設定 */

	switch(Screen)
	{
		case 0:
		{
			ulGR_H = 0xC00000;	/* Screen0 */
			break;
		}
		case 1:
		{
			ulGR_H = 0xC80000;	/* Screen1 */
			break;
		}
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


/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t G_Load(uint8_t bCGNum, uint16_t uX, uint16_t uY, uint16_t uArea)
{
	int16_t	ret = 0;

	ret = APICG_DataLoad2G( g_stCG_LIST[bCGNum].bFileName, uX, uY, uArea);

	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t G_Load_Mem(uint8_t bCGNum, int16_t posX, int16_t posY, uint16_t uArea)
{
	int16_t	ret = 0;

	uint16_t	*pDstGR, *pSrcGR;
	uint32_t	DstGR_H;
	uint32_t	Addr_Max;
	uint32_t	uMaxNum;
	int16_t		x, y, sx, sy, ex, ey;
	uint32_t	uWidth, uHeight, uFileSize;
	uint16_t	uSize8x = 0;
	int16_t		Pal_offset;

	Get_CG_FileList_MaxNum(&uMaxNum);
	if(uMaxNum <= bCGNum)
	{
		ret = -1;
	}

	pSrcGR = g_stPicImage[bCGNum].pImageData;	/* Src PIC */
	if(pSrcGR == NULL)
	{
		/* ファイルから読み込み */
		ret = CG_File_Load(bCGNum);
		if(ret < 0)
		{
			return	ret;
		}
	}
	
	pSrcGR = Get_PicImageInfo( bCGNum, &uWidth, &uHeight, &uFileSize );	/* 画像の情報を取得 */
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8の倍数 */
	
	if( uWidth >= X_MAX_DRAW )
	{
		uWidth = X_MAX_DRAW;
	}
	if( uHeight >= Y_MAX_DRAW )
	{
		uHeight = Y_MAX_DRAW;
	}
	
	/* アドレス算出 */
	if(uArea == 0)
	{
		DstGR_H		= 0xC00000;	/* Screen0 */
		Addr_Max	= 0xC80000;
	}
	else{
		DstGR_H		= 0xC80000;	/* Screen1 */
		Addr_Max	= 0xD00000;
	}

	Pal_offset = Set_PicImagePallet(bCGNum);	/* パレットを設定 */
	
	if((posX + uWidth) < X_MAX_DRAW)
	{
		ex = posX + uWidth;
	}
	else	/* はみ出た分は処理しないようにサイズを小さくする*/
	{
		ex = X_MAX_DRAW;
	}
	
	if((posY + uHeight) < Y_MAX_DRAW)
	{
		ey = posY + uHeight;
	}
	else	/* はみ出た分は処理しないようにサイズを小さくする*/
	{
		ey = Y_MAX_DRAW;
	}
	
	if(posX >= X_MIN_DRAW)
	{
		sx = posX;
	}
	else
	{
		sx = X_MIN_DRAW;
	}
	
	if(posY >= Y_MIN_DRAW)
	{
		sy = posY;
	}
	else
	{
		sy = Y_MIN_DRAW;
	}
	
	
	for(y = 0; y < uHeight; y++)
	{
		if( ((posY + y) >= Y_MIN_DRAW) && ((posY + y) < Y_MAX_DRAW) )
		{
			/* アドレス算出 */
			pDstGR = (uint16_t *)( DstGR_H + ( Mmul1024(posY + y) + Mmul2(posX) ) );
		}
		else
		{
			pSrcGR += uSize8x;
			continue;
		}
		
		for(x = 0; x < uSize8x; x++)
		{
			if( ((posX + x) >= X_MIN_DRAW) && ((posX + x) < X_MAX_DRAW) )
			{
				/* アドレス算出 */
//				pDstGR = (uint16_t *)( DstGR_H + ( Mmul1024(posY + y) + Mmul2(posX + x) ) );
				
				if(*pSrcGR != 0x00)
				{
#ifdef DEBUG
//					if(bCGNum == 1)
//					{
//						printf("DImg(0x%p)SImg(0x%p)=%d\n", pDstGR, pSrcGR, (*pSrcGR) + Pal_offset);
//						KeyHitESC();	/* デバッグ用 */
//					}
#endif
					*pDstGR = (*pSrcGR) + Pal_offset;
				}
			}
			pDstGR++;
			pSrcGR++;
		}
	}
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
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
	}
	
	if(work_buf != NULL)
	{
		MyMfree(work_buf);	/* メモリ解放 */
	}
	if(file_buf != NULL)
	{
		MyMfree(file_buf);	/* メモリ解放 */
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
			GR = (uint16_t *)0xC00000;	/* Screen0 */
			break;
		case 1:
			GR = (uint16_t *)0xC80000;	/* Screen1 */
			break;
		default:
			GR = (uint16_t *)0xC00000;	/* Screen0 */
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
		
	}
	
	if(work_buf != NULL)
	{
		MyMfree(work_buf);	/* メモリ解放 */
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
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
	for(j=0; j<G_PAL_MAX; j++)
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
						//uGlay = Mdiv10(uGlay);
						uGlay = Mdiv4(uGlay);
						
						col = SetRGB(uGlay, uGlay, uGlay);	/* ３色を合成 */

						ubR = GetR(col);
						ubG = GetG(col);
						ubB = GetB(col);

						k = 1;
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
	
	for(j=0; j<G_PAL_MAX; j++)
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
//	for(j=0; j<G_PAL_MAX; j++)
//	{
//		if(ubNotUsePal[j] != 0)
//		{
//			GPALET( j, SetRGB(0, 0, 0) );	/* 使ってないパレットは透過色設定 */
//		}
//	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PutGraphic_To_Symbol12(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = 1;
	stSymbol.mag_y = 1;
	stSymbol.color = uPal;
	stSymbol.font_type = 0;	/* 0:12x12 1:16x16 2:24x24 */
	stSymbol.angle = 0;		/* 0:Normal 1:90 2:180 3:270 */
	
	_iocs_symbol(&stSymbol);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PutGraphic_To_Symbol16(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = 1;
	stSymbol.mag_y = 1;
	stSymbol.color = uPal;
	stSymbol.font_type = 1;	/* 0:12x12 1:16x16 2:24x24 */
	stSymbol.angle = 0;		/* 0:Normal 1:90 2:180 3:270 */
	
	_iocs_symbol(&stSymbol);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PutGraphic_To_Symbol24(uint8_t *sString, uint16_t dx, uint16_t dy, uint16_t uPal)
{
	int16_t	ret = 0;
	
	struct _symbolptr stSymbol;
	
	stSymbol.x1 = dx;
	stSymbol.y1 = dy;
	stSymbol.string_address = sString;
	stSymbol.mag_x = 1;
	stSymbol.mag_y = 1;
	stSymbol.color = uPal;
	stSymbol.font_type = 2;	/* 0:12x12 1:16x16 2:24x24 */
	stSymbol.angle = 0;		/* 0:Normal 1:90 2:180 3:270 */
	
	_iocs_symbol(&stSymbol);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	G_Scroll(int16_t x, int16_t y, uint8_t bSCNum)
{
	int16_t	ret = 0;
	
	switch(bSCNum)
	{
	case 0:	/* ページ０ */
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
	case 1:	/* ページ１ */
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
	default:
		{
			break;
		}
	}

	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	配置したい位置とポジションから描画したい画像左上の座標位置を算出する		*/
/*===========================================================================================*/
int16_t G_ClipAREA_Chk_Width(	int16_t *pPos_x, int16_t *x_min, int16_t *x_max, uint16_t uSrc_w, 
								uint8_t ubMode, uint8_t ubH)
{
	int16_t	ret = 0;
	int16_t	x;
	
	x = *pPos_x;
	
	/* 横軸 */
	switch(ubH)
	{
		case POS_LEFT:
		{
			break;
		}
		case POS_MID:
		{
			x -= (int16_t)Mdiv2(uSrc_w);
			break;
		}
		case POS_RIGHT:
		{
			x -= (int16_t)uSrc_w;
			break;
		}
		default:
		{
			break;
		}
	}
	
	/* 表示エリア内クリップ */
	switch(ubMode)
	{
		case 0:
		{
			*x_min = X_MIN_DRAW;
			*x_max = X_MIN_DRAW + WIDTH;
			break;
		}
		case 1:
		{
			*x_min = X_MIN_DRAW + X_OFFSET;
			*x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			break;
		}
		case 2:
		{
			*x_min = X_MIN_DRAW + X_OFFSET;
			*x_max = X_MIN_DRAW + X_OFFSET + WIDTH;
			break;
		}
		default:
		{
			*x_min = X_MIN_DRAW;
			*x_max = X_MAX_DRAW;
			break;
		}
	}
	
	/*表示エリア範囲内判定 */
	if((x + uSrc_w) < *x_min)
	{
		ret = x;
	}
	else if(x >= *x_max)
	{
		ret = x;
	}
	else
	{
		/* OK */
	}
	
	*pPos_x = x;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	配置したい位置とポジションから描画したい画像左上の座標位置を算出する		*/
/*===========================================================================================*/
int16_t G_ClipAREA_Chk_Height(	int16_t *pPos_y, int16_t *y_min, int16_t *y_max, uint16_t uSrc_h,
								uint8_t ubMode, uint8_t ubV)
{
	int16_t	ret = 0;
	int16_t	y;
	
	y = *pPos_y;
	
	/* 縦軸 */
	switch(ubV)
	{
		case POS_TOP:
		{
			break;
		}
		case POS_CENTER:
		{
			y -= (int16_t)Mdiv2(uSrc_h);
			break;
		}
		case POS_BOTTOM:
		{
			y -= (int16_t)uSrc_h;
			break;
		}
		default:
		{
			break;
		}
	}
	
	/* 表示エリア内クリップ */
	switch(ubMode)
	{
		case 0:
		{
			*y_min = Y_MIN_DRAW;
			*y_max = Y_MIN_DRAW + Y_MAX_WINDOW;
			break;
		}
		case 1:
		{
			*y_min = Y_MIN_DRAW + Y_OFFSET;
			*y_max = Y_MIN_DRAW + Y_OFFSET + Y_MAX_WINDOW;
			break;
		}
		case 2:
		{
			*y_min = Y_MIN_DRAW;
			*y_max = Y_MIN_DRAW + Y_MAX_WINDOW;
			break;
		}
		default:
		{
			*y_min = Y_MIN_DRAW;
			*y_max = Y_MAX_DRAW;
			break;
		}
	}
	
	/*表示エリア範囲内判定 */
	if((y + uSrc_h) < *y_min)
	{
		ret = y;
	}
	else if(y >= *y_max)
	{
		ret = y;
	}
	else
	{
		/* OK */
	}
	
	*pPos_y = y;
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* メディアンカット法による画像の減色 */
/* http://hiroshi0945.seesaa.net/article/159985352.html */
/* メディアンカット減色DIBの作成 */
int16_t G_MedianCut(uint16_t *pDstImg, uint16_t *pSrcImg, uint16_t uSrcWidth, uint16_t uSrcHeight,
					uint16_t cEntries, uint8_t ubImgNum, uint8_t ubConvCol)
{	
	int16_t		ret = 0;
	int32_t		iYUV = 0;
	int32_t		nShift, divBit;
	int32_t		nColors;
	int32_t		i;
	int32_t		index;
	int32_t		pos;
	uint16_t	x, y;
	uint16_t	width, height, Size8x;
	uint32_t	dwSizeDimension;
	uint16_t	*pDst, *pSrc;
	uint16_t	col;
	uint16_t	pal_no;
	uint16_t	nBpp;
	uint8_t		ubOffsetCol;
	RGBQUAD*	pColorTable;
	RGBQUAD		rgb;
	YUV			*pYUV;
	AVGYUV		*pAvgYUV;
	
	/* 減色する色数が512以上2以下ならFALSEを返して終了する。*/
	if( (cEntries > 512) || (cEntries < 2) )return -1;	/* cEntriesは減色する色数 */

	/* パレット色数からビットカウントを取得する */
	if 	(cEntries <= 2u)		nBpp = 1u;	/* cEntriesは減色する色数 */
	else if (cEntries <= 16u)	nBpp = 4u;	/* 4bit(16色) */
	else if (cEntries <= 256u)	nBpp = 8u;	/* 8bit(256色) */
	else						nBpp = 16u;	/* 16bit(65535色) */
#ifdef DEBUG
//	printf("色[%d]＝(%d bit)\n", cEntries, nBpp);
//	KeyHitESC();	/* デバッグ用 */
#endif
	
	width	= uSrcWidth;	/* 画像幅 */
	height	= uSrcHeight;	/* 画像高さ */
	Size8x	= (((width+7)/8) * 8);	/* 8の倍数 */
#ifdef DEBUG
//	printf("SImg(0x%p)%d,%d,%d)\n", pSrcImg, width, height, Size8x );
//	KeyHitESC();	/* デバッグ用 */
#endif

	/* YUVテーブル作成 */
	dwSizeDimension = width * height;					/* YUVテーブルのサイズ */
	pYUV = MyMalloc( sizeof(YUV) * dwSizeDimension );	/* YUVテーブル領域確保 */
	if(pYUV == NULL)
	{
		return -1;
	}
    
	for(y=0; y < height; y++)
	{
        /* y行の一番左端のピクセルへのポインタの算出 */
		pSrc = pSrcImg + (y * Size8x);
		
        for(x=0; x < width; x++)
		{
			pal_no = *pSrc;
			col = GPALET( *pSrc, -1 );	/* 現在の設置を抽出 */
			pSrc++;

			if(ubConvCol == pal_no)	/* 透過色判定 */
			{
				/* 透過色（黒）にする */
				rgb.rgbRed   = 0x00u;
				rgb.rgbGreen = 0x00u;
				rgb.rgbBlue  = 0x00u;
			}
			else
			{
				/* ピクセルデータを取り出す */
				rgb.rgbRed   = Mmul8( GetR(col) );
				rgb.rgbGreen = Mmul8( GetG(col) );
				rgb.rgbBlue  = Mmul8( GetB(col) );
			}
#ifdef DEBUG
//			printf("rgb(%d,%d,%d)\n", rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue, GetR(col), GetG(col), GetB(col) );
//			KeyHitESC();	/* デバッグ用 */
#endif
			
			/* RGBからYUVへ変換する */
			pYUV[iYUV].y = ( 0.29891f * (float)(rgb.rgbRed)) + (0.58661f * (float)(rgb.rgbGreen)) + (0.11448f * (float)(rgb.rgbBlue));
			pYUV[iYUV].u = (-0.16874f * (float)(rgb.rgbRed)) - (0.33126f * (float)(rgb.rgbGreen)) + (0.50000f * (float)(rgb.rgbBlue));
			pYUV[iYUV].v = ( 0.50000f * (float)(rgb.rgbRed)) - (0.41869f * (float)(rgb.rgbGreen)) - (0.08131f * (float)(rgb.rgbBlue));
			pYUV[iYUV].no = 0;
#ifdef DEBUG
//			printf("pYUV(%3.1f,%3.1f,%3.1f)\n", pYUV[iYUV].y, pYUV[iYUV].u, pYUV[iYUV].v );
//			KeyHitESC();	/* デバッグ用 */
#endif
			iYUV++;
		}
	}

	/* 入力色数から2の乗数を算出します。*/
	nShift = cEntries >> 1;
	divBit = 0;
	while(nShift > 0)
	{
		nShift >>= 1;
		divBit++;
	}

	/* 色を分けるメディアンカット減色関数を呼び出します。*/
	G_MedianCut_Gen(pYUV, dwSizeDimension, 0, divBit);

	/* 減色カラーテーブルを作成する。*/
	nColors = (nBpp > 8) ? cEntries : (cEntries / 2) * 2 + (cEntries % 2) * 2;
	pColorTable = (RGBQUAD*)MyMalloc( sizeof(RGBQUAD) * nColors );
	memset(pColorTable, 0, sizeof(RGBQUAD) * nColors);
	
	pAvgYUV = (AVGYUV*)MyMalloc( sizeof(AVGYUV) * nColors );
	if(pAvgYUV == NULL)
	{
		return -1;
	}
	
	memset(pAvgYUV, 0, sizeof(AVGYUV) * nColors);

	for(y = 0; y < height; y++)
	{
		iYUV = y * width;
		for(x = 0; x < width; x++)
		{
			int32_t num;
			pos = iYUV + x;
			num = pYUV[pos].no;
			
			pAvgYUV[num].y += (double)pYUV[pos].y;
			pAvgYUV[num].u += (double)pYUV[pos].u;
			pAvgYUV[num].v += (double)pYUV[pos].v;
			pAvgYUV[num].cnt++;
		}
	}

	/* カラーテーブルのYUV値をRGBに変換する。*/
	for(i = 0; i < nColors; i++)
	{
		if(pAvgYUV[i].cnt > 0)
		{
			double red, green, blue;
			
			pAvgYUV[i].y = pAvgYUV[i].y / (double)pAvgYUV[i].cnt;
			pAvgYUV[i].u = pAvgYUV[i].u / (double)pAvgYUV[i].cnt;
			pAvgYUV[i].v = pAvgYUV[i].v / (double)pAvgYUV[i].cnt;

			/* 赤成分のYUV->RGB変換 */
			red = pAvgYUV[i].y + 1.40200 * pAvgYUV[i].v;
			red += 0.49;
			if(red < 0.0)	red = 0.0;
			if(red > 255.0)	red = 255.0;
			pColorTable[i].rgbRed = (uint8_t)red;

			//緑成分のYUV->RGB変換
			green = pAvgYUV[i].y - 0.34414 * pAvgYUV[i].u - 0.71414 * pAvgYUV[i].v;
			green += 0.49;
			if(green < 0.0)		green = 0.0;
			if(green > 255.0)	green = 255.0;
			pColorTable[i].rgbGreen = (uint8_t)green;

			//青成分のYUV->RGB変換
			blue = pAvgYUV[i].y + 1.77200 * pAvgYUV[i].u;
			blue += 0.49;
			if(blue < 0.0)		blue = 0.0;
			if(blue > 255.0)	blue = 255.0;
			pColorTable[i].rgbBlue = (uint8_t)blue;
#ifdef DEBUG
//			printf("RGB[%d](%d,%d,%d)\n", i, pColorTable[i].rgbRed, pColorTable[i].rgbGreen, pColorTable[i].rgbBlue );
//			KeyHitESC();	/* デバッグ用 */
#endif
		}
    }

   	if(pAvgYUV != NULL)
	{
		MyMfree(pAvgYUV);
	}
	
#ifdef DEBUG
	//printf("G_MedianCut[%d]\n", ubImgNum);
	//KeyHitESC();	/* デバッグ用 */
#endif
	ubOffsetCol = 0;
	/* 減色する色数が256以下ならパレットDIBなので、*/
	/* カラーテーブルをBITMAPINFO構造体のカラーテーブルにコピーします。 */
	if(nColors<=256)
	{
		for(i = 0; i < nColors; i++)
		{
			uint16_t uCol;
			uCol = SetRGB( Mdiv8(pColorTable[i].rgbRed), Mdiv8(pColorTable[i].rgbGreen), Mdiv8(pColorTable[i].rgbBlue) );
			GPALET( i, uCol );	/* 減色結果でパレット更新 */
		}
	}
	
#ifdef DEBUG
//	printf("DImg(0x%p)%d,%d,%d)\n", pDstImg, width, height, Size8x );
//	KeyHitESC();	/* デバッグ用 */
#endif
	/* ピクセルデータをDIBに格納します。*/
	switch(nBpp)
	{
		case 16:	/* 16ビットDIB */
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);

				iYUV = y * width;
				
				for(x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
					*pDst = SetRGB( Mdiv8(pColorTable[index].rgbRed), Mdiv8(pColorTable[index].rgbGreen), Mdiv8(pColorTable[index].rgbBlue) );
					pDst++;
			    }
			}
			break;

		case 8:	/* 8ビットDIB */
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);
				iYUV = y * width;
				
				for( x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
#ifdef DEBUG
//					printf("pYUV[%d]=(%d)(%d,%d)\n", iYUV, index, x, y);
//					KeyHitESC();	/* デバッグ用 */
#endif
					if( (pColorTable[index].rgbRed   == 0) &&
						(pColorTable[index].rgbGreen == 0) &&
						(pColorTable[index].rgbBlue  == 0) )
					{
						*pDst = 0;	/* 黒色は強制透過色 */
					}
					else
					{
						*pDst = index + ubOffsetCol;
					}
					pDst++;
				}
			}
			break;

		case 4:	/* 4ビットDIB */
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);
				iYUV = y * width;
				
				for(x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
#ifdef DEBUG
//					printf("pYUV[%5d]=(%3d)(%3d,%3d)\n", iYUV, index, x, y);
//					KeyHitESC();	/* デバッグ用 */
#endif
					if( (pColorTable[index].rgbRed   == 0) &&
						(pColorTable[index].rgbGreen == 0) &&
						(pColorTable[index].rgbBlue  == 0) )
					{
						*pDst = 0;	/* 黒色は強制透過色 */
					}
					else
					{
						*pDst = index + ubOffsetCol;
					}
					pDst++;
			    }
			}
			break;

		case 1:     //モノクロDIB
			for(y = 0; y < height; y++)
			{
				pDst = pDstImg + (y * Size8x);
				iYUV = y * width;
				
				for(x = 0; x < width; x++)
				{
					index = pYUV[iYUV + x].no;
#ifdef DEBUG
//					printf("pYUV[%d]=(%d)(%d,%d)\n", iYUV, index, x, y);
//					KeyHitESC();	/* デバッグ用 */
#endif
					if( (pColorTable[index].rgbRed   == 0) &&
						(pColorTable[index].rgbGreen == 0) &&
						(pColorTable[index].rgbBlue  == 0) )
					{
						*pDst = 0;	/* 黒色は強制透過色 */
					}
					else
					{
						*pDst = index + ubOffsetCol;
					}
					pDst++;
			    }
			}
			break;
    }
	if(pYUV != NULL)
	{
		MyMfree(pYUV);
	}
	if(pColorTable != NULL)
	{
		MyMfree(pColorTable);
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
/* メディアンカット減色 */
void G_MedianCut_Gen(YUV* tblYUV, int32_t maxTbl, int32_t base, int32_t divBit)
{
	
	/* 作業する変数の数を数える */
	int32_t	i;
	int32_t	a, b;
	int32_t	idx = 0;
	int32_t	swData;
	float minY = 0,minU = 0,minV = 0;
	float maxY = 0,maxU = 0,maxV = 0;
	float avgY = 0,avgU = 0,avgV = 0;
	double allY = 0,allU = 0,allV = 0;

	if(tblYUV == NULL)return;
	if(divBit < 1)return;

	for(i=0; i < maxTbl; i++)
	{
		if(tblYUV[i].no == base)
		{
			if(idx==0)	/* 初めての値の入力ならば */
			{
				allY = minY = maxY = avgY = tblYUV[i].y;
				allU = minU = maxU = avgU = tblYUV[i].u;
				allV = minV = maxV = avgV = tblYUV[i].v;
			}
			else
			{
				if(tblYUV[i].y <= minY)minY = tblYUV[i].y;
				if(tblYUV[i].y >= maxY)maxY = tblYUV[i].y;
				if(tblYUV[i].u <= minU)minU = tblYUV[i].u;
				if(tblYUV[i].u >= maxU)maxU = tblYUV[i].u;
				if(tblYUV[i].v <= minV)minV = tblYUV[i].v;
				if(tblYUV[i].v >= maxV)maxV = tblYUV[i].v;
				allY += (double)tblYUV[i].y;
				allU += (double)tblYUV[i].u;
				allV += (double)tblYUV[i].v;
			}
			idx++;
		}
	}
	if(idx == 0) return;	 /* 作業する変数がないので終了 */

	a = base;
	b = base + (1 << (divBit - 1));

	avgY = (float)(allY / idx);
	avgU = (float)(allU / idx);
	avgV = (float)(allV / idx);

	/* メディアンカット法を行うので、幅が最大になる要素を探す。*/
	if((maxY - minY) > (maxU - minU))
	{
		swData = ((maxY - minY) > (maxV - minV)) ? 0 : 2;
	}
	else
	{
		swData = ((maxU - minU) > (maxV - minV)) ? 1 : 2;
	}
	/* 平均値で色の切り分け */
	switch(swData)
	{
		case 0:	/* Y */
			for(i = 0; i < maxTbl; i++)
			{
				if(tblYUV[i].no == base)
				{
					tblYUV[i].no = (tblYUV[i].y < avgY) ? a : b;
				}
			}
			break;
		case 1:	/* U */
			for(i = 0; i < maxTbl; i++)
			{
				if(tblYUV[i].no == base)
				{
					tblYUV[i].no = (tblYUV[i].u < avgU) ? a : b;
				}
			}
			break;
		case 2:	/* V */
			for(i = 0; i < maxTbl; i++)
			{
				if(tblYUV[i].no == base)
				{
					tblYUV[i].no = (tblYUV[i].v < avgV) ? a : b;
				}
			}
			break;
	}

	/* 再起する */
	G_MedianCut_Gen(tblYUV, maxTbl, a, divBit-1);
	G_MedianCut_Gen(tblYUV, maxTbl, b, divBit-1);
}


/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void G_APIC_to_Mem(uint16_t *pSrcBuf, uint16_t *pDstBuf, uint16_t uWidth, uint16_t uHeight)
{
	uint32_t	x, y;
	uint16_t	*pBuf;
	uint16_t	uSize8x;
	
	uSize8x	= (((uWidth+7)/8) * 8);	/* 8の倍数 */
	
	/* 加工後をメモリに保存する */
	for(y=0u; y < uHeight; y++)
	{
		pBuf = pSrcBuf + (y << 9u);
		
		for(x=0u; x < uSize8x; x++)
		{
			if(x < uWidth)
			{
				*pDstBuf = *pBuf & 0xFFu;
				pBuf++;
			}
			else
			{
				*pDstBuf = 0x00u;
			}
			pDstBuf++;
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
int16_t G_Pal_Set(uint8_t ubImgNum)
{
	int16_t	ret = 0;
	
	if(ubImgNum >= PAL_MAX_CG)return -1;
	
	g_CG_PalTbl[ubImgNum] = ubImgNum;			/* パレットテーブルにセット */
	
	ret = g_CG_PalTbl[ubImgNum];
	
    return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t G_Pal_Get(uint8_t ubImgNum)
{
	int16_t	ret = 0;

	if(ubImgNum >= PAL_MAX_CG)return 0;

	ret = g_CG_PalTbl[ubImgNum];
	
    return ret;
}
#endif	/* IF_GRAPHIC_C */