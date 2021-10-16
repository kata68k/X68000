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

#include "APL_MACS.h"
#include "Graphic.h"
#include "MUSIC.h"
#include "PCG.h"

static uint8_t		*pcg_dat;		/* ＰＣＧデータファイル読み込みバッファ */
static 	uint16_t	pal_dat[ 128 ];	/* パレットデータファイル読み込みバッファ */


/* 関数のプロトタイプ宣言 */
int16_t Init_FileList_Load(void);
int16_t File_Load(int8_t *, void *, size_t, size_t);
int16_t File_Save(int8_t *, void *, size_t, size_t);
int16_t File_Load_CSV(int8_t *, uint16_t *, uint16_t *, uint16_t *);
int16_t PCG_SP_dataload(int8_t *);
int16_t PCG_PAL_dataload(int8_t *);
int16_t Load_Music_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_SE_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_CG_List(	int8_t *, int8_t *, CG_LIST *, uint32_t *);
int16_t Load_MACS_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t GetFileLength(int8_t *, int32_t *);
int16_t GetFilePICinfo(int8_t *, BITMAPINFOHEADER *);
int16_t GetRectangleSise(uint16_t *, uint16_t, uint16_t, uint16_t);
void *MyMalloc(int32_t);
int16_t MyMfree(void *);
int32_t	MaxMemSize(int8_t);

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Init_FileList_Load(void)
{
	int16_t ret = 0;
	uint8_t	sListFileName[255] = {0};
	uint32_t	i;
	
	Load_CG_List("data\\cg\\", "g_list.txt", g_stCG_LIST, &g_CG_List_Max);			/* グラフィックリストの読み込み */
	for(i = 0; i < g_CG_List_Max; i++)
	{
		printf("PIC   File %2d = %s\n", i, g_stCG_LIST[i].bFileName);
	}
	Load_MACS_List("data\\mov\\", "mov_list.txt", g_mov_list, &g_mov_list_max);	/* 動画(MACS)リストの読み込み */
	for(i = 0; i < g_mov_list_max; i++)
	{
		printf("MACS  File %2d = %s\n", i, g_mov_list[i]);
	}

	/* BGM */
#if		ZM_V2 == 1
	strcpy( sListFileName, "m_list.txt" );
#elif	ZM_V3 == 1
	strcpy( sListFileName, "m_list_V3.txt" );
#elif	MC_DRV == 1
	strcpy( sListFileName, "m_list_MC.txt" );
#else
	#error "No Music Lib"
#endif
	Load_Music_List("data\\music\\", sListFileName, music_list, &m_list_max);
	for(i = 0; i < m_list_max; i++)
	{
		printf("Music File %2d = %s\n", i, music_list[i]);
	}
	
	/* 効果音(FM) */
#if		ZM_V2 == 1
#elif	ZM_V3 == 1
	Load_SE_List("data\\seFM\\", "s_list_V3.txt", se_list, &s_list_max);
	for(i = 0; i < s_list_max; i++)
	{
		printf("SE FM File %2d = %s\n", i, se_list[i]);
	}
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif
	
	/* 効果音(ADPCM) */
	Load_SE_List("data\\se\\", "p_list.txt", adpcm_list, &p_list_max);
	for(i = 0; i < p_list_max; i++)
	{
		printf("ADPCM File %2d = %s\n", i, adpcm_list[i]);
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
/* ファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
int16_t File_Load(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int16_t ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)
	{
		/* ファイルが読み込めません */
		printf("ERR:%sファイルが見つかりません！\n", fname);
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* ファイル保存 */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
int16_t File_Save(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int16_t ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)	/* ファイルが無い */
	{
		/* ファイルを開ける */
		fp = fopen(fname, "wb");
		fwrite(ptr, size, n, fp);
	}
	else
	{
		/* ファイルが存在する場合は何もしない */
	}
	/* ファイルを閉じる */
	fclose (fp);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* CSVファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* *Col		データの行数のアドレス */
/* *Row		データの列数のアドレス */
int16_t File_Load_CSV(int8_t *fname, uint16_t *ptr, uint16_t *Col, uint16_t *Row)
{
	FILE *fp;
	int16_t ret = 0;
	uint16_t x, y, flag, cnv_flag;
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
				int64_t num = strtol(p, &end, 0);

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
					*ptr = (uint16_t)num;
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* CSV コースデータファイル読み込み */
/* *fname	ファイル名 */
/* *st_ptr	格納先の先頭アドレス */
/* *Col		データの行数のアドレス */
/* *Row		データの列数のアドレス */
int16_t File_Load_Course_CSV(int8_t *fname, ST_ROADDATA *st_ptr, uint16_t *Col, uint16_t *Row)
{
	FILE *fp;
	int16_t ret = 0;
	uint16_t x, y, flag, cnv_flag;
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
				int64_t num = strtol(p, &end, 0);

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
							st_ptr->bHeight = (uint8_t)num;
							break;
						}
						case 1:
						{
							st_ptr->bWidth = (uint8_t)num;
							break;
						}
						case 2:
						{
							st_ptr->bAngle = (uint8_t)num;
							break;
						}
						case 3:
						{
							st_ptr->bfriction = (uint8_t)num;
							break;
						}
						case 4:
						{
							st_ptr->bPat = (uint8_t)num;
							break;
						}
						case 5:
						{
							st_ptr->bObject = (uint8_t)num;
							break;
						}
						case 6:
						{
							st_ptr->bRepeatCount = (uint8_t)num;
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_SP_dataload(int8_t *fname)
{
	int16_t ret = 0;
	
	FILE *fp;

	/*-----------------[ ＰＣＧデータ読み込み ]-----------------*/

	fp = fopen( fname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		int32_t	pcg_size;
		pcg_size = filelength( fileno( fp ) );
		
		if(pcg_dat != NULL)
		{
//			MyMfree(pcg_dat);	/* メモリ解放 */
		}
		pcg_dat = (uint8_t*)MyMalloc( pcg_size );
		if(pcg_dat != NULL)
		{
			uint32_t i,j,pat;
			
			pat = pcg_size / SP_16_SIZE;
			
			j = fread( pcg_dat
				,  SP_16_SIZE				/* 1PCG = 128byte */
				,  pat	/* PCG */
				,  fp
				) ;
			fclose( fp ) ;
#if 1
			for( i = 0; i < pat; i++ )
			{
				_iocs_sp_defcg( i, 1, pcg_dat );
				pcg_dat += SP_16_SIZE;
			}
#endif
		}
//		MyMfree(pcg_dat);	/* メモリ解放 */
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
int16_t PCG_PAL_dataload(int8_t *fname)
{
	int16_t ret = 0;

	FILE *fp;
	uint32_t i;

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
			_iocs_spalet( (i&15) | (1<<0x1F) , Mdiv16(i)+1 , pal_dat[i] ) ;
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
int16_t Load_Music_List(int8_t *fpath, int8_t *fname, int8_t (*music_list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[1000], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_SE_List(int8_t *fpath, int8_t *fname, int8_t (*music_list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[1000], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_CG_List(int8_t *fpath, int8_t *fname, CG_LIST *cg_list, uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[1000], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0, bType = 0, bTransPal = 0;
	
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
#if 0
			FILE *fp_d;
			
			p = strtok(z_name, ".");
			sprintf(z_name, "%s.GRP", p, );
			fp_d = fopen(z_name, "r");
			if(fp_d == NULL)
			{
			}
#endif
			
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_MACS_List(int8_t *fpath, int8_t *fname, int8_t (*macs_list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[1000], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetFileLength(int8_t *pFname, int32_t *pSize)
{
	FILE *fp;
	int16_t ret = 0;
	int32_t	Tmp;

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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetFilePICinfo(int8_t *pFname, BITMAPINFOHEADER *info)
{
	FILE *fp;
	int16_t ret = 0;
	uint8_t	bFlag = FALSE;
	uint8_t	*pBuf;
	uint8_t	sBuf[128] = {0};
	uint16_t	uWord;
	uint32_t	i;

	fp = fopen( pFname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
		pBuf = &sBuf[0];	/* 先頭アドレス */

		/* PIC */
		fread( pBuf, sizeof(uint8_t), 3, fp );
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
		
		memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
		pBuf = &sBuf[0];	/* 先頭アドレス */

		/* 拡張ヘッダか？ */
		fread( pBuf, sizeof(uint8_t), 3, fp );
		if(strncmp( pBuf, "/MM", 3 ) == 0)
		{
			/* 一致 */
//			printf("拡張ファイルです\n", pBuf );

			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */

			for(i=0; i < 128; i++)
			{
				/* 拡張ヘッダ判定 */
				fread( pBuf, sizeof(uint8_t), 1, fp );
				if(strncmp( pBuf, "/", 1 ) == 0)
				{
					*pBuf = 0x00;
//					printf("%s\n", &sBuf[0]);
					
					memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
					pBuf = &sBuf[0];	/* 先頭アドレス */

					fread( pBuf, sizeof(uint8_t), 1, fp );
					/* 拡張ヘッダ終了判定 */
					if(strncmp( &sBuf[0], ":", 1 ) == 0)
					{
						break;
					}
					else
					{
						pBuf++;
					}
					
					fread( pBuf, sizeof(uint8_t), 1, fp );
					/* 画像データの作者判定 */
					if(strncmp( &sBuf[0], "AU", 2 ) == 0)
					{
//						printf("画像データの作者:");
						memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
						pBuf = &sBuf[0];	/* 先頭アドレス */
					}
					/* 画像のロード座標 */
					else if(strncmp( &sBuf[0], "XY", 2 ) == 0)
					{
//						printf("XY:");
						memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
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
			
			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */

			/* コメント終了まで読み込む */
			for(i=0; i < 128; i++)
			{
				fread( pBuf, sizeof(uint8_t), 1, fp);
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
				fread( pBuf, sizeof(uint8_t), 1, fp);
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

			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */
			
			while(1)
			{
				fread( pBuf, sizeof(uint8_t), 1, fp);
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
			fread( pBuf, sizeof(uint8_t), 1, fp);
			if(*pBuf == 0x00u)
			{
				bFlag = TRUE;
			}
		}

		/* タイプ・モード */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
			fread( pBuf, sizeof(uint8_t), 1, fp);
			if(*pBuf == 0x00u)
			{
//				printf("Type/Mode：%d\n", *pBuf );
				bFlag = TRUE;
			}
		}
		
		if(bFlag == TRUE)
		{
			/* 色のビット数 */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biBitCount = uWord;
//			printf("color bit：%d\n", uWord );

			/* Ｘ方向のサイズ */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biWidth	= (int64_t)uWord;
//			printf("X,Y：(%4d,", uWord );

			/* Ｙ方向のサイズ */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biHeight	= (int64_t)uWord;
//			printf("%4d)\n", uWord );
		}

		fclose( fp ) ;
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
int16_t	GetRectangleSise(uint16_t *uSize, uint16_t uWidth, uint16_t uHeight, uint16_t uMaxSize)
{
	int16_t ret = 0;
	
	*uSize = Mmax( ((((uWidth+ 7u) & 0xFFF8u) * uHeight) << uMaxSize),  (512u << uMaxSize) );
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void *MyMalloc(int32_t Size)
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
		else if((uint32_t)pPtr >= 0x81000000)
		{
			if((uint32_t)pPtr >= 0x82000000)
			{
				puts("メモリ不足です");
			}
			else
			{
				printf("メモリが確保できませんでした(%d)\n", (uint32_t)pPtr - 0x81000000 );
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyMfree(void *pPtr)
{
	int16_t ret = 0;
	uint32_t	result;
	
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t	MaxMemSize(int8_t SizeType)
{
	int32_t ret = 0;
	int32_t i, dummy;
	int32_t chk[2];
	int8_t *ptMem[2];
	
	ptMem[0] = (int8_t *)0x0FFFFF;
	ptMem[1] = (int8_t *)0x100000;
	
	do{
		for(i=0; i<2; i++)
		{
			if((int32_t)ptMem[i] >= 0xC00000)	/* 12MBの上限 */
			{
				chk[0] = 0;	/* 強制ループ脱出 */
				chk[1] = 2;	/* 強制ループ脱出 */
				break;
			}
			else
			{
				chk[i] = _dos_memcpy(ptMem[i], &dummy, 1);	/* バスエラーチェック */
			}
		}
		
		/* 実装メモリの境界 */
		if( (chk[0] == 0) &&	/* 読み書きできた */
			(chk[1] == 2) )		/* バスエラー */
		{
			break;	/* ループ脱出 */
		}
		
		ptMem[0] += 0x100000;	/* +1MB 加算 */
		ptMem[1] += 0x100000;	/* +1MB 加算 */
	}while(1);
	
//	printf("Memory Size = %d[MB](%d[Byte])(0x%p)\n", ((int)ptMem[1])>>20, ((int)ptMem[1]), ptMem[0]);

	switch(SizeType)
	{
	case 0:	/* Byte */
		ret = ((int)ptMem[1]);
		break;
	case 1:	/* KByte */
		ret = ((int)ptMem[1])>>10;
		break;
	case 2:	/* MByte */
	default:
		ret = ((int)ptMem[1])>>20;
		break;
	}
	
	return ret;
}

#endif	/* FILEMANAGER_C */

