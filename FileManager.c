#ifndef	FILEMANAGER_C
#define	FILEMANAGER_C

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <io.h>
#include <time.h>
#include <doslib.h>
#include <iocslib.h>

#include "inc/usr_macro.h"

#include "FileManager.h"
#include "Graphic.h"

/* ＰＣＧデータ */
static UC	*pcg_dat; /* ＰＣＧデータファイル読み込みバッファ */
static US	pal_dat[ 128 ]; /* パレットデータファイル読み込みバッファ */

/* 関数のプロトタイプ宣言 */
SS File_Load(SC *, void *, size_t, size_t);
SS File_Load_CSV(SC *, US *, US *, US *);
SS PCG_SP_dataload(SC *);
SS PCG_PAL_dataload(SC *);
SS Load_Music_List(	SC *, SC *, SC (*)[256], UI *);
SS Load_SE_List(	SC *, SC *, SC (*)[256], UI *);
SS Load_CG_List(	SC *, SC *, CG_LIST *, UI *);
SS Load_MACS_List(	SC *, SC *, SC (*)[256], UI *);
SS GetFileLength(SC *, SI *);
SS GetFilePICinfo(SC *, BITMAPINFOHEADER *);
SS GetRectangleSise(US *, US, US, US);
void *MyMalloc(SI);
SS MyMfree(void *);

/* ファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
SS File_Load(SC *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	SS ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)
	{
		/* ファイルが読み込めません */
		ret = -1;
	}
	else
	{
		/* データ個数を指定しない場合 */
		if(n == 0)
		{
			/* ファイルサイズを取得 */
			n = filelength(fileno(fp));
		}
		
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
/* *Col		データの行数のアドレス */
/* *Row		データの列数のアドレス */
SS File_Load_CSV(SC *fname, US *ptr, US *Col, US *Row)
{
	FILE *fp;
	SS ret = 0;
	US x, y, flag, cnv_flag;
	char buf[1000], *p, *end;
	
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

/* CSV コースデータファイル読み込み */
/* *fname	ファイル名 */
/* *st_ptr	格納先の先頭アドレス */
/* *Col		データの行数のアドレス */
/* *Row		データの列数のアドレス */
SS File_Load_Course_CSV(SC *fname, ST_ROADDATA *st_ptr, US *Col, US *Row)
{
	FILE *fp;
	SS ret = 0;
	US x, y, flag, cnv_flag;
	char buf[1000], *p, *end;
	
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

//				printf("(%d,%d)=%d ->%s\n", x, y, num, end);

				if(*end == '\0')
				{
					/* 文字列の終わりだった */
					st_ptr++;
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
					switch(x)
					{
						case 0:
						{
							st_ptr->bHeight = (UC)num;
							break;
						}
						case 1:
						{
							st_ptr->bWidth = (UC)num;
							break;
						}
						case 2:
						{
							st_ptr->bAngle = (UC)num;
							break;
						}
						case 3:
						{
							st_ptr->bfriction = (UC)num;
							break;
						}
						case 4:
						{
							st_ptr->bPat = (UC)num;
							break;
						}
						case 5:
						{
							st_ptr->bObject = (UC)num;
							break;
						}
						case 6:
						{
							st_ptr->bRepeatCount = (UC)num;
							break;
						}
						default:
						{
							break;
						}
					}

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

SS PCG_SP_dataload(SC *fname)
{
	SS ret = 0;

	FILE *fp;

	/*-----------------[ ＰＣＧデータ読み込み ]-----------------*/

	fp = fopen( fname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		SI	pcg_size;
		pcg_size = filelength( fileno( fp ) );
		
		pcg_dat = NULL;
		pcg_dat = (UC*)MyMalloc( pcg_size );
		if(pcg_dat != NULL)
		{
			UI i,j;
			
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
	}
	
	return ret;
}

SS PCG_PAL_dataload(SC *fname)
{
	SS ret = 0;

	FILE *fp;
	UI i;

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
			SPALET( (i&15) | (1<<0x1F) , Mdiv16(i)+1 , pal_dat[i] ) ;
		}
	}
	
	return ret;
}

SS Load_Music_List(SC *fpath, SC *fname, SC (*music_list)[256], UI *list_max)
{
	FILE *fp;
	SS ret = 0;
	SC buf[1000], *p;
	SC z_name[256];
	UI i=0, num=0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
	        p = buf;
			sscanf(p,"%d = %s", &num, z_name);
			if(i == num)
			{
				sprintf(music_list[i], "%s%s", fpath, z_name);
			}
			i++;
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

SS Load_SE_List(SC *fpath, SC *fname, SC (*music_list)[256], UI *list_max)
{
	FILE *fp;
	SS ret = 0;
	SC buf[1000], *p;
	SC z_name[256];
	UI i=0, num=0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
	        p = buf;
			sscanf(p,"%d = %s", &num, z_name);
			if(i == num)
			{
				sprintf(music_list[i], "%s%s", fpath, z_name);
			}
			i++;
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

SS Load_CG_List(SC *fpath, SC *fname, CG_LIST *cg_list, UI *list_max)
{
	FILE *fp;
	SS ret = 0;
	SC buf[1000], *p;
	SC z_name[256];
	UI i=0, num=0, bType = 0, bTransPal = 0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
	        p = buf;
			sscanf(p,"%d= %s %d %d", &num, z_name, &bType, &bTransPal);
#ifdef DEBUG
//			printf("%d=%s,%d\n", num, z_name, bType);
//			KeyHitESC();	/* デバッグ用 */
#endif
			if(i == num)
			{
				sprintf( cg_list->bFileName, "%s%s", fpath, z_name );
				cg_list->ubType = bType;
				cg_list->ubTransPal = bTransPal;
			}
			cg_list++;
			i++;
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

SS Load_MACS_List(SC *fpath, SC *fname, SC (*macs_list)[256], UI *list_max)
{
	FILE *fp;
	SS ret = 0;
	SC buf[1000], *p;
	SC z_name[256];
	UI i=0, num=0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
	        p = buf;
			sscanf(p,"%d = %s", &num, z_name);
			if(i == num)
			{
				sprintf(macs_list[i], "%s%s", fpath, z_name);
			}
			i++;
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

SS	GetFileLength(SC *pFname, SI *pSize)
{
	FILE *fp;
	SS ret = 0;
	SI	Tmp;

	fp = fopen( pFname , "rb" ) ;
	if(fp == NULL)		/* Error */
	{
		ret = -1;
	}
	else
	{
		Tmp = fileno( fp );
		if(Tmp == -1)	/* Error */
		{
			*pSize = 0;
			ret = -1;
		}
		else
		{
			Tmp = filelength( Tmp );
			if(Tmp == -1)		/* Error */
			{
				*pSize = 0;
				ret = -1;
			}
			else
			{
				*pSize = Tmp;
//				printf("GetFileLength = (%4d, %4d)\n", *pSize, Tmp );
			}
		}

		fclose( fp ) ;
	}
	
	return ret;
}

SS	GetFilePICinfo(SC *pFname, BITMAPINFOHEADER *info)
{
	FILE *fp;
	SS ret = 0;
	UC	bFlag = FALSE;
	UC	*pBuf;
	UC	sBuf[128] = {0};
	US	uWord;
	UI	i;

	fp = fopen( pFname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
		pBuf = &sBuf[0];	/* 先頭アドレス */

		/* PIC */
		fread( pBuf, sizeof(UC), 3, fp );
		if(strncmp( pBuf, "PIC", 3 ) == 0)
		{
			/* 一致 */
//			printf("%sファイルです\n", pBuf );
			bFlag = TRUE;
		}
		else
		{
			bFlag = FALSE;
			printf("error:PICファイルではありません = %s\n", pBuf );
		}
		
		memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
		pBuf = &sBuf[0];	/* 先頭アドレス */

		/* 拡張ヘッダか？ */
		fread( pBuf, sizeof(UC), 3, fp );
		if(strncmp( pBuf, "/MM", 3 ) == 0)
		{
			/* 一致 */
//			printf("拡張ファイルです\n", pBuf );

			memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */

			for(i=0; i < 128; i++)
			{
				/* 拡張ヘッダ判定 */
				fread( pBuf, sizeof(UC), 1, fp );
				if(strncmp( pBuf, "/", 1 ) == 0)
				{
					*pBuf = 0x00;
//					printf("%s\n", &sBuf[0]);
					
					memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
					pBuf = &sBuf[0];	/* 先頭アドレス */

					fread( pBuf, sizeof(UC), 1, fp );
					/* 拡張ヘッダ終了判定 */
					if(strncmp( &sBuf[0], ":", 1 ) == 0)
					{
						break;
					}
					else
					{
						pBuf++;
					}
					
					fread( pBuf, sizeof(UC), 1, fp );
					/* 画像データの作者判定 */
					if(strncmp( &sBuf[0], "AU", 2 ) == 0)
					{
//						printf("画像データの作者:");
						memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
						pBuf = &sBuf[0];	/* 先頭アドレス */
					}
					/* 画像のロード座標 */
					else if(strncmp( &sBuf[0], "XY", 2 ) == 0)
					{
//						printf("XY:");
						memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
						pBuf = &sBuf[0];	/* 先頭アドレス */
					}
					else
					{
						pBuf++;
					}
				}
				else
				{
					pBuf++;
				}
			}
			
			memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */

			/* コメント終了まで読み込む */
			for(i=0; i < 128; i++)
			{
				fread( pBuf, sizeof(UC), 1, fp);
				if(*pBuf == 0x1Au)
				{
					if(i != 0u)
					{
//						printf("コメント(%d)：%s\n", i, sBuf[0] );
					}
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}
		else
		{
			/* 普通のヘッダファイルでした */
			pBuf += 3;
			bFlag = FALSE;

			for(i=0; i < 128; i++)
			{
				fread( pBuf, sizeof(UC), 1, fp);
				if(*pBuf == 0x1Au)
				{
//					printf("コメント(%d)：%s\n", i, sBuf[0] );
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}

		/* ダミー */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;

			memset(sBuf, 0, sizeof(UC) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */
			
			while(1)
			{
				fread( pBuf, sizeof(UC), 1, fp);
				if(*pBuf == 0x00u)
				{
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}

		/* 予約文字(0) */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
			fread( pBuf, sizeof(UC), 1, fp);
			if(*pBuf == 0x00u)
			{
				bFlag = TRUE;
			}
		}

		/* タイプ・モード */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
			fread( pBuf, sizeof(UC), 1, fp);
			if(*pBuf == 0x00u)
			{
//				printf("Type/Mode：%d\n", *pBuf );
				bFlag = TRUE;
			}
		}
		
		if(bFlag == TRUE)
		{
			/* 色のビット数 */
			fread( &uWord, sizeof(US), 1, fp);
			info->biBitCount = uWord;
//			printf("color bit：%d\n", uWord );

			/* Ｘ方向のサイズ */
			fread( &uWord, sizeof(US), 1, fp);
			info->biWidth	= (SL)uWord;
//			printf("X,Y：(%4d,", uWord );

			/* Ｙ方向のサイズ */
			fread( &uWord, sizeof(US), 1, fp);
			info->biHeight	= (SL)uWord;
//			printf("%4d)\n", uWord );
		}

		fclose( fp ) ;
	}
	
	return ret;
}

SS	GetRectangleSise(US *uSize, US uWidth, US uHeight, US uMaxSize)
{
	SS ret = 0;
	
	*uSize = Mmax( ((((uWidth+ 7u) & 0xFFF8u) * uHeight) << uMaxSize),  (512u << uMaxSize) );
	
	return ret;
}

void *MyMalloc(SI Size)
{
	void *pPtr = NULL;
	
	if(Size >= 0x1000000u)
	{
		printf("メモリ確保サイズが大きすぎるよ！(0x%x)\n", Size );
	}
	else
	{
		pPtr = _dos_malloc(Size);	/* メモリ確保 */
//		pPtr = malloc(Size);	/* メモリ確保 */
		
		if(pPtr == NULL)
		{
			puts("メモリが確保できませんでした");
		}
		else if((UI)pPtr >= 0x81000000)
		{
			if((UI)pPtr >= 0x82000000)
			{
				puts("メモリ不足です");
			}
			else
			{
				printf("メモリが確保できませんでした(%d)\n", (UI)pPtr - 0x81000000 );
			}
			pPtr = NULL;
		}
		else
		{
			//printf("メモリアドレス(0x%p)=%d\n", pPtr, Size);
		}
	}
	
	return pPtr;
}

SS	MyMfree(void *pPtr)
{
	SS ret = 0;
	UI	result;
	
	if(pPtr == 0)
	{
		puts("自プロセス、子プロセスで確保したメモリをフルで解放します");
	}
	
	result = _dos_mfree(pPtr);
	//free(pPtr);
	
	if(result < 0)
	{
		ret = -1;
	}
	
	return ret;
}
#endif	/* FILEMANAGER_C */

