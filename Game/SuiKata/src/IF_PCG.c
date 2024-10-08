#ifndef	IF_PCG_C
#define	IF_PCG_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iocslib.h>
#include <doslib.h>
#include <usr_macro.h>

#include "IF_PCG.h"
#include "BIOS_CRTC.h"
#include "BIOS_MFP.h"
#include "IF_FileManager.h"
#include "IF_Memory.h"
#include "main.h"

#if  CNF_XSP
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */

/* ＰＣＧデータ */
int8_t		sp_list[PCG_MAX][256]	=	{0};
uint32_t	sp_list_max	=	0u;
static size_t	sp_dat_size[PCG_MAX]	=	{0};

int8_t		pal_list[PAL_MAX][256]	=	{0};
uint32_t	pal_list_max	=	0u;
static size_t	pal_dat_size[PAL_MAX]	=	{0};

/*
	XSP 用 PCG 配置管理テーブル
	スプライト PCG パターン最大使用数 + 1 バイトのサイズが必要。
*/
int8_t pcg_alt[PCG_MAX + 1];	/* XSP 用 PCG 配置管理テーブル	スプライト PCG パターン最大使用数 + 1 バイトのサイズが必要。*/
int8_t pcg_dat[PCG_MAX * 128];	/* ＰＣＧデータファイル読み込みバッファ */
uint16_t pal_dat[ 256 ];		/* パレットデータファイル読み込みバッファ */
int8_t g_bSP = FALSE;

/* 構造体定義 */
/* 関数のプロトタイプ宣言 */
void sp_dataload(void);
void PCG_INIT(void);
void PCG_ON(void);
void PCG_OFF(void);
int8_t Get_SP_Sns(void);
void BG_ON(int16_t);
void BG_OFF(int16_t);
void PCG_START_SYNC(int16_t);
void PCG_END_SYNC(int16_t);
void PCG_COL_SHIFT(int16_t, int16_t);
void PCG_PUT_1x1(int16_t, int16_t, int16_t, int16_t);
void PCG_PUT_2x1(int16_t, int16_t, int16_t, int16_t);
void PCG_PUT_2x2(int16_t, int16_t, int16_t, int16_t);
void BG_put_Clr(int16_t, int16_t, int16_t, int16_t, int16_t);
void BG_put_String(int16_t, int16_t, int16_t, int8_t *, int16_t);
void BG_put_Number10(int16_t, int16_t, int16_t, uint32_t);
void BG_Scroll(int16_t, int16_t, int16_t);
void BG_TEXT_SET(int8_t *, int16_t, int16_t, int16_t, int16_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void sp_dataload(void)
{
#if 0
	FILE *fp;
	int8_t sBuf[128];
#endif
	unsigned int i,j;

	/*-----------------[ ＰＣＧデータ読み込み ]-----------------*/
	for(i = 0, j = 0; i < sp_list_max; i++)
	{
		int32_t	FileSize;
		
		if(GetFileLength(sp_list[i], &FileSize) == -1)
		{

		}
		else
		{
			if(FileSize != 0)
			{
				/* メモリに登録 */
				sp_dat_size[i] = (int32_t)File_Load(sp_list[i], &pcg_dat[j * 128], sizeof(int8_t), FileSize);
				printf("SP File %2d = %s = size(%d[byte]=%d)\n", i, sp_list[i], sp_dat_size[i], FileSize);
				j += (FileSize/128);
			}
		}
	}

#if 0
	fp = fopen( "data/sp/BALL.SP" , "rb" ) ;
//	fp = fopen( "SP_DATA/SAKANA.SP" , "rb" ) ;
	j = fread( pcg_dat
			,  128		/* 1PCG = 128byte */
			,  PCG_MAX	/* PCGの数 */
			,  fp
	) ;
	fclose( fp ) ;
#endif
	for( i = 0; i < PCG_MAX; i++ )
	{
		_iocs_sp_defcg( i, 1, &pcg_dat[i * 128] );
	}
	
#if 0
	/*-----------[ PCG データを縦画面用に 90 度回転 ]-----------*/

	for (i = 0; i < 256; i++) {
		pcg_roll90(&pcg_dat[i * 128], 1);
	}
#endif
	
	/*--------[ スプライトパレットデータ読み込みと定義 ]--------*/

#if 1
	for(i = 0; i < pal_list_max; i++)
	{
		int32_t	FileSize;
		
		if(GetFileLength(pal_list[i+1], &FileSize) == -1)
		{

		}
		else
		{
			if(FileSize != 0)
			{
				/* メモリに登録 */
				pal_dat_size[i] = (int32_t)File_Load(pal_list[i+1], &pal_dat[i*16], sizeof(int8_t), FileSize);
				printf("PAL File %2d = %s = size(%d[byte]=%d)\n", i+1, pal_list[i+1], pal_dat_size[i], FileSize);
			}
		}
	}
	/* スプライトパレットに転送 */
	for( i = 0 ; i < 256 ; i++ )
	{
		_iocs_spalet( (i&15) | (1<<0x1F) , i/16 + 1, pal_dat[i] ) ;
	}
#else	
	sprintf(sBuf, "%s/sp/BALL.PAL", Get_DataList_Path());
	fp = fopen( sBuf, "rb" ) ;
//	fp = fopen( "data/sp/BALL.PAL" , "rb" ) ;
//	fp = fopen( "SP_DATA/SAKANA.PAL" , "rb" ) ;
	fread( pal_dat
		,  2		/* 1palet = 2byte */
		,  256		/* 16palet * 16block */
		,  fp
		) ;
	fclose( fp ) ;
	/* スプライトパレットに転送 */
	for( i = 0 ; i < 256 ; i++ )
	{
		_iocs_spalet( (i&15) | (1<<0x1F) , i/16 + 1, pal_dat[i] ) ;
	}
#endif

}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_INIT(void)
{
	uint32_t	i, j;
#if  CNF_XSP
#else   /* CNF_XSP 0 */
	uint32_t	uPCG_num;
#endif  /*CNF_XSP*/

	/* スプライトの初期化 */
	g_bSP = FALSE;
	/* スプライトの初期化 */
	_iocs_sp_init();			/* スプライトの初期化 */
	
	/*スプライトレジスタ初期化*/
	for( j = 0x80000000; j < 0x80000000 + 128; j++ )
	{
		_iocs_sp_regst(j,-1,0,0,0,0);
	}
	for( i = 0; i < 256; i++ )
	{
		_iocs_sp_cgclr(i);			/* スプライトクリア */
	}

	//	_iocs_bgctrlgt(1);					/* BGコントロールレジスタ読み込み */
	_iocs_bgscrlst(0,0,0);					/* BGスクロールレジスタ設定 */
	_iocs_bgscrlst(1,0,0);					/* BGスクロールレジスタ設定 */
	_iocs_bgtextcl(0,SetBGcode(0,0,0,0));	/* BGテキストクリア */
	_iocs_bgtextcl(1,SetBGcode(0,0,0,0));	/* BGテキストクリア */
	//	BGTEXTGT(1,1,0);					/* BGテキスト読み込み */

#if  CNF_XSP
	PCG_XSP_INIT();		/* XSP初期化 */
#else   /* CNF_XSP 0 */

	puts("SP/BG mode");
	
	/* スプライト管理用バッファのクリア */
	for(uPCG_num = 0; uPCG_num < PCG_MAX; uPCG_num++)
	{
		memset(&g_stPCG_DATA[uPCG_num], 0, sizeof(ST_PCG) );
	}

#endif  /*CNF_XSP*/
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_XSP_INIT(void)
{
#if  CNF_XSP
	puts("XSP mode");
    /*---------------------[ XSP を初期化s ]---------------------*/
	PCG_ON();			/* SP 初期化 */

	xsp_mode(2);

//	xsp_vertical(1);    /* 縦画面モード */

	/*---------------------[ XSP を初期化e ]---------------------*/
#endif  /*CNF_XSP*/
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_ON(void)
{
	if(Get_SP_Sns() == FALSE)
	{
#if  CNF_XSP
    	xsp_on();			/* XSP ON */

		xsp_pcgmask_on(0, Mmax(PCG_16x16_AREA - 1, 0));		/* 0-PCG_16x16_AREA-1 */		
		xsp_pcgmask_on(0x80, 0xFF);							/* 128-255 */

		xsp_pcgdat_set(pcg_dat, pcg_alt, sizeof(pcg_alt));  /* PCG データと PCG 配置管理をテーブルを指定 */

#if CNF_VDISP
		xsp_vsyncint_on(App_VsyncProc);		/* 帰線期間割り込み開始 */
#endif

#if CNF_RASTER
		xsp_hsyncint_on(App_RasterProc);	/* ラスタ割り込み開始 */
#endif

#else   /* CNF_XSP 0 */
#endif  /*CNF_XSP*/
		g_bSP = TRUE;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_OFF(void)
{
	if(Get_SP_Sns() == TRUE)
	{
		g_bSP = FALSE;
#if  CNF_XSP
#if CNF_RASTER
		xsp_hsyncint_off();		/* ラスタ割り込み開始 */
#endif
#if CNF_VDISP
		xsp_vsyncint_off();		/* 帰線期間割り込み開始 */
#endif
		xsp_off();			/* XSP OFF */
#else   /* CNF_XSP 0 */

#endif  /*CNF_XSP*/
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int8_t Get_SP_Sns(void)
{
	return g_bSP;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void BG_ON(int16_t nNum)
{
	if(Get_SP_Sns() == TRUE)
	{
		if(nNum == 0)
		{
			_iocs_bgctrlst(0, 0, 1);	/* ＢＧ０表示ＯＮ */
		}
		else if(nNum == 1)
		{
			_iocs_bgctrlst(1, 1, 1);	/* ＢＧ１表示ＯＮ */
		}
	    else
		{

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
void BG_OFF(int16_t nNum)
{
	if(Get_SP_Sns() == TRUE)
	{
		if(nNum == 0)
		{
			_iocs_bgctrlst(0, 0, 0);	/* ＢＧ０表示ＯＦＦ */
		}
		else if(nNum == 1)
		{
			_iocs_bgctrlst(1, 1, 0);	/* ＢＧ１表示ＯＦＦ */
		}
	    else
		{

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
void PCG_START_SYNC(int16_t count)
{
#if  CNF_XSP
//	xsp_vsync(count);
	xsp_vsync2(count);  /* 垂直同期 */
#else   /* CNF_XSP 0 */
    /* なし */
#endif  /*CNF_XSP*/
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_END_SYNC(int16_t count)
{
	PCG_Main();	/* スプライト管理 */

#if  CNF_XSP
    /* スプライトを一括表示する */
    xsp_out();
//	printf("xsp_out()\n");
#else   /* CNF_XSP 0 */
    /* 垂直同期 */
	wait_vdisp(count);	/* 約count／60秒待つ	*/
#if CNF_VDISP
#else
	Timer_D_Less_NowTime();
#endif

#endif  /*CNF_XSP*/
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_COL_SHIFT(int16_t Col, int16_t Count)
{
	int16_t *pSP_PAL;
	int16_t i;
	int16_t temp[16];

	pSP_PAL = (int16_t *)0xE82200;	/* スプライトのカラーテーブル０番  */

	if(Col < 16)
	{
		pSP_PAL += (Col * 16);	/* 目的のパレット移動 */

		// オリジナルの配列を一時的な配列にコピー
		for(i = 0; i < 16; i++)
		{
			temp[i] = *(pSP_PAL + i);
		}
		// 要素をずらす
		for(i = 0; i < 16; i++)
		{
			*(pSP_PAL + ((i + Count) % 16)) = temp[i];
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
void PCG_PUT_1x1(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x, y, pt, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_PUT_2x1(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x-8, y, pt+0, info);
    xsp_set( x+8, y, pt+1, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_PUT_2x2(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x-8, y-8, pt+0, info);
    xsp_set( x+8, y-8, pt+1, info);
    xsp_set( x-8, y+8, pt+2, info);
    xsp_set( x+8, y+8, pt+3, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void PCG_PUT_3x3(int16_t x, int16_t y, int16_t pt, int16_t info)
{
#if  CNF_XSP
    xsp_set( x-24, y-24, pt+0, info);
    xsp_set( x- 8, y-24, pt+1, info);
    xsp_set( x+ 8, y-24, pt+2, info);

    xsp_set( x-24, y- 8, pt+3, info);
    xsp_set( x- 8, y- 8, pt+4, info);
    xsp_set( x+ 8, y- 8, pt+5, info);

    xsp_set( x-24, y+ 8, pt+6, info);
 	xsp_set( x- 8, y+ 8, pt+7, info);
 	xsp_set( x+ 8, y+ 8, pt+8, info);
#else   /* CNF_XSP 0 */
#endif  /* CNF_XSP */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void BG_put_Clr(int16_t nPage, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t i, j;
	int16_t ax, ay;
	int16_t bx, by;

	ax = Mdiv8(x1);
	ay = Mdiv8(y1);
	bx = Mdiv8(x2);
	by = Mdiv8(y2);

	for (j = ay; j < by; j++)
	{
		for (i = ax; i < bx; i++)
		{
			_iocs_bgtextst( nPage, i, j, SetBGcode( 0, 0, 1, 0x20));		/* BGテキスト設定 */
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
void BG_put_String(int16_t x, int16_t y, int16_t pal, int8_t *sStr, int16_t nLen)
{
	int16_t i = 0;
	int16_t bx, by;

	bx = Mdiv8(x);
	by = Mdiv8(y);

	for (i = 0; i < nLen; i++)
	{
		_iocs_bgtextst( 0, bx + i, by, SetBGcode( 0, 0, pal, *(sStr + i)));		/* BGテキスト設定 */
    }
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void BG_put_Number10(int16_t x, int16_t y, int16_t pal, uint32_t uNum)
{
	int16_t i, pat;
	int16_t bx, by;
	uint32_t uDigit1, uDigit2, uMod;

	bx = Mdiv8(x);
	by = Mdiv8(y);

	uDigit1 = 10;
	uDigit2 = 1;

	for(i = 0; i < 8; i++)
	{
		uMod = uNum % uDigit1;

		if((uNum / uDigit2) != 0ul)
		{
			pat = (int16_t)(uMod / uDigit2) + 40;
		}
		else
		{
			pat = 0;
		}

		_iocs_bgtextst( 0, bx + 7-i, by, SetBGcode( 0, 0, pal, pat));		/* BGテキスト設定 */

		uDigit1 *= 10;
		uDigit2 *= 10;
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void BG_Scroll(int16_t nPage, int16_t x, int16_t y)
{
	_iocs_bgscrlst(0x80000000 + nPage, x, y);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void BG_TEXT_SET(int8_t *fname, int16_t nPage, int16_t nPal, int16_t nX_ofst, int16_t nY_ofst)
{
#if 0
	uint16_t	usV_pat;
#endif
	uint16_t	x_max, y_max;
	uint16_t	x,y;
	uint16_t	x_bg_ofst, y_bg_ofst;
	uint16_t	map_data[64][64];

	/* マップデータ読み込み */
	File_Load_CSV( fname, (uint16_t *)&map_data[0][0], &x_max, &y_max);

	x_bg_ofst = Mdiv8(nX_ofst);
	y_bg_ofst = Mdiv8(nY_ofst);
	x_max = Mmin((x_max + x_bg_ofst), 32);
	y_max = Mmin((y_max + y_bg_ofst), 32);

	/* 上段 */
	for(y=0; y<3; y++)
	{
		for(x=0; x<x_max; x++)
		{
			_iocs_bgtextst(nPage, x + x_bg_ofst, y + y_bg_ofst, SetBGcode(0,0,nPal,map_data[y][x]));		/* BGテキスト設定 */
		}
	}
	/* 左側から中央 */
	for(y=3; y<y_max; y++)
	{
		for(x=0; x<16; x++)
		{
			_iocs_bgtextst(nPage, x + x_bg_ofst, y + y_bg_ofst, SetBGcode(0,0,nPal,map_data[y][x]));		/* BGテキスト設定 */
		}
	}
	/* 中央から右側 */
	for(y=3; y<y_max; y++)
	{
		for(x=16; x<x_max; x++)
		{
			_iocs_bgtextst(nPage, x + x_bg_ofst, y + y_bg_ofst, SetBGcode(0,1,nPal,map_data[y][x]));		/* BGテキスト設定(垂直(左右)反転) */
		}
	}
#if 0
	for(y=0; y<16; y++)
	{
		for(x=0; x<x_max; x++)
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
	for(y=16; y<y_max; y++)
	{
		for(x=0; x<x_max; x++)
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
#endif
}

#endif	/* IF_PCG_C */

