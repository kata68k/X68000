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
void sp_dataload(void);

/* ファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
SI File_Load(SC *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	UI ret;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)
	{
		/* ファイルが読み込めません */
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
	UI ret, x, y, flag;
	char buf[1000], *s, *p, *end;
	
	x = 0;
	y = 0;
	flag = 0;
	
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
					
					x++;
				}
				else
				{
					/* 変換できなかった */
					p = end + 1;
				}
			}
			while(1);
			y++;
			x = 0;
		}
		fclose(fp);
	}
	
	*Row = y;
	
	return ret;
}

void sp_dataload(void)
{
	FILE *fp;
	UI i,j;

	/*-----------------[ ＰＣＧデータ読み込み ]-----------------*/

	fp = fopen( "data/BG.SP" , "rb" ) ;
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

	/*--------[ スプライトパレットデータ読み込みと定義 ]--------*/

	fp = fopen( "data/BG.PAL" , "rb" ) ;
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
#endif	/* FILEMANAGER_C */

