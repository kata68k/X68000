#ifndef	FILEMANAGER_C
#define	FILEMANAGER_C

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <doslib.h>
#include <iocslib.h>

#include "inc/usr_macro.h"
#include "inc/apicglib.h"

#include "FileManager.h"

/* ＰＣＧデータ */
static unsigned int pcg_size;
static unsigned char *pcg_dat; /* ＰＣＧデータファイル読み込みバッファ */
static unsigned short pal_dat[ 128 ]; /* パレットデータファイル読み込みバッファ */

/* 関数のプロトタイプ宣言 */
SI File_Load(SC *, void *, size_t, size_t);
SI File_Load_CSV(SC *, US *, UI *, UI *);
SI PCG_SP_dataload(SC *);
SI PCG_PAL_dataload(SC *);
SI APICG_DataLoad(SC *, US, US);

/* ファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
SI File_Load(SC *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	SI ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)
	{
		/* ファイルが読み込めません */
		ret = -1;
	}
	else
	{
		/* ファイル読み込み */
		ret = fread (ptr, size, n, fp);

		/* ファイルを閉じる */
		fclose (fp);
	}

	return ret;
}

/* CSVファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
SI File_Load_CSV(SC *fname, US *ptr, UI *Col, UI *Row)
{
	FILE *fp;
	SI ret = 0;
	UI x, y, flag, cnv_flag;
	char buf[1000], *s, *p, *end;
	
	x = 0;
	y = 0;
	flag = 0;
	cnv_flag = 0;
	
	fp = fopen(fname, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
	        p = buf;
			do
			{
				SL num = strtol(p, &end, 0);

//				printf("(%d,%d)=%d ->%s", x, y, num, end);

				if(*end == '\0')
				{
					/* 文字列の終わりだった */
					if(flag==0)
					{
						*Col = x;
					}
					flag = 1;
					break;
				}
				else if(p != end)
				{
					/* 変換できた */
					*ptr = (US)num;
					ptr++;

					p = end + 1;
					
					cnv_flag = 1;
					x++;
				}
				else
				{
					/* 変換できなかった */
					p = end + 1;
				}
			}
			while(1);
			
			if(cnv_flag != 0){
				cnv_flag = 0;
				y++;
			}
			x = 0;
		}
		fclose(fp);
	}
	
	*Row = y;
	
	return ret;
}

SI PCG_SP_dataload(SC *fname)
{
	FILE *fp;
	SI ret = 0;
	UI i,j;

	/*-----------------[ ＰＣＧデータ読み込み ]-----------------*/

	fp = fopen( fname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		pcg_size = filelength( fileno( fp ) );
		pcg_dat = ( char *) malloc( pcg_size );
		j = fread( pcg_dat
			,  128		/* 1PCG = 128byte */
			,  PCG_MAX	/* 256PCG */
			,  fp
			) ;
		fclose( fp ) ;

		for( i = 0 ; i < PCG_MAX ; i++ ){
			SP_DEFCG( i, 1, pcg_dat );
			pcg_dat += 128;
		}
	}
	
	return ret;
}

SI PCG_PAL_dataload(SC *fname)
{
	FILE *fp;
	SI ret = 0;
	UI i,j;

	/*--------[ スプライトパレットデータ読み込みと定義 ]--------*/

	fp = fopen( fname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		fread( pal_dat
			,  2		/* 1palet = 2byte */
			,  256		/* 16palet * 16block */
			,  fp
			) ;
		fclose( fp ) ;

		/* スプライトパレットに転送 */
		for( i = 0 ; i < 256 ; i++ )
		{
			SPALET( (i&15) | (1<<0x1F) , i/16+1 , pal_dat[i] ) ;
		}
	}
	
	return ret;
}

#define	PIC_FILE_BUF_SIZE	(16*1024)

/* PICファイルを読み込み */
SI APICG_DataLoad(SC *fname, US pos_x, US pos_y)
{
	US *GR = (US *)0xC00000;
	UC *file_buf, *work_buf;
	SI ret;
	file_buf = _dos_malloc (PIC_FILE_BUF_SIZE);
	work_buf = _dos_malloc (256 * 256);
	
	if (((int) file_buf < 0) || ((int) work_buf < 0)) {
		/* メモリエラー */
		ret = -1;
	} else {
		ret = APICLOAD(	GR, 
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
		_dos_mfree (work_buf);
		_dos_mfree (file_buf);
	}
	return ret;
}
#endif	/* FILEMANAGER_C */

