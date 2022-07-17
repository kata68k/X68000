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
#include <interrupt.h>

#include "inc/usr_macro.h"

#include "FileManager.h"

asm("	.include	doscall.mac");
		
/* 関数のプロトタイプ宣言 */
int16_t File_Load(int8_t *, void *, size_t, size_t);
int16_t FileHeader_Load(int8_t *, void *, size_t, size_t);
int16_t File_strSearch(FILE *, char *, int, long);
int16_t File_Save(int8_t *, void *, size_t, size_t);
int16_t GetFileLength(int8_t *, int32_t *);
void *MyMalloc(int32_t);
void *MyMallocJ(int32_t);
void *MyMallocHi(int32_t);
int16_t MyMfree(void *);
int16_t	MyMfreeJ(void *);
int16_t	MyMfreeHi(void *);
int32_t	MaxMemSize(int8_t);

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
		printf("error:%sファイルが見つかりません！\n", fname);
		ret = -1;
	}
	else
	{
		int i, j;
		size_t ld, ld_mod, ld_t;
		
		/* データ個数を指定しない場合 */
		if(n == 0)
		{
			/* ファイルサイズを取得 */
			n = filelength(fileno(fp));
		}
		
		fprintf(stderr, "0%%       50%%       100%%\n");
		fprintf(stderr, "+---------+---------+   <cancel:ESC>\n");
		ld = n / 100;
		ld_mod = n % 100;
		ld_t = 0;
		
		for (i = 0; i <= 100; i++) {
			/* ファイル読み込み */
			if(i < 100)
			{
				fread (ptr, size, ld, fp);
				ptr += ld;
				ld_t += ld;
			}
			else
			{
				fread (ptr, size, ld_mod, fp);
				ld_t += ld_mod;
			}
			
			for (j = 0; j < i / 5 + 1; j++) {
				fprintf(stderr, "#");
			}
			fprintf(stderr, "\n");
			fprintf(stderr, "%3d%%(%d/%d)\n", i, ld_t, n);
			fprintf(stderr, "\033[2A");
			
			if((BITSNS( 0 ) & Bit_1) != 0u)
			{
				break;
			}
		}
		if(i >= 100)
		{
			fprintf(stderr, "\n\nfinish!\n");
		}
		else
		{
			fprintf(stderr, "\n\ncancel\n");
			ret = -1;
		}
		
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
/* ファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
int16_t FileHeader_Load(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int16_t ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)
	{
		/* ファイルが読み込めません */
		printf("error:%sファイルが見つかりません！\n", fname);
		ret = -1;
	}
	else
	{
		char sBuf[256];
		
		memset(sBuf, 0, sizeof(sBuf));
		fread (sBuf, sizeof(char), 8, fp);
		
		if(strcmp(sBuf, "MACSDATA") == 0)	/* MACSDATA判定 */
		{
			char toolver[2];
			long ld;
			
			fseek(fp, 8L, SEEK_SET);
			memset(sBuf, 0, sizeof(sBuf));
			fread (&toolver[0], sizeof(char), 1, fp);	/* バージョン H */
			fseek(fp, 9L, SEEK_SET);
			memset(sBuf, 0, sizeof(sBuf));
			fread (&toolver[1], sizeof(char), 1, fp);	/* バージョン L */
			
			fseek(fp, 10L, SEEK_SET);
			memset(sBuf, 0, sizeof(sBuf));
			fread (&ld, sizeof(long), 1, fp);	/* ファイルサイズ */
			
			if( (toolver[0] == 1) && (toolver[1] > 20))
			{
				printf("   ");
				if(File_strSearch( fp, "TITLE:", 6, 14L ) == 0)		/* TITLE: */
				{
					printf("\n");
				}
				printf(" ");
				if(File_strSearch( fp, "COMMENT:", 8, 14L ) == 0)		/* COMMENT: */
				{
					printf("\n");
				}
			}
			
			printf("FILEINFO:");
			printf("DataVer=%d.%d ", toolver[0], toolver[1]);
			printf("FileSize=%ld[MB] ", ld>>20);

			if( (toolver[0] == 1) && (toolver[1] > 20))
			{
				printf("PCM=");
				if(File_strSearch( fp, "DUALPCM/", 8, 14L ) == 0)		/* DUALPCM/ */
				{
					printf("\n");
				}
				else if(File_strSearch( fp, "PCM8PP", 6, 14L ) == 0)	/* PCM8PP */
				{
					printf("\n");
				}
				else if(File_strSearch( fp, "ADPCM", 5, 14L ) == 0)	/* ADPCM */
				{
					printf("\n");
				}
				else
				{
					/* 何もしない */
				}
			}
			printf("\n");
		}
		
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
int16_t File_strSearch(FILE *fp, char *str, int len, long file_ofst)
{
	int16_t ret = 0;
	
	int i;
	int cnt, sMeslen;
	char sBuf[256];
	
	sMeslen = 0;
	
	/* 探したキーワードからNULLまで表示する */
	for(i=0; i < 256; i++)
	{
		fseek(fp, file_ofst + i, SEEK_SET);
		
		memset(sBuf, 0, sizeof(sBuf));
		fread (sBuf, sizeof(char), len, fp);	/* ファイル読み込み */
		
		if(strncmp(sBuf, str, len) == 0)	/* 読み込んだデータとキーワードの一致確認 */
		{
			fseek(fp, file_ofst + i + len, SEEK_SET);
			memset(sBuf, 0, sizeof(sBuf));
			fgets(sBuf, 256, fp);
			sMeslen = strlen(sBuf);
			cnt = i;
			ret = 0;
			break;
		}
		memset(sBuf, 0, sizeof(sBuf));
		ret = -1;
	}
	
	if(sMeslen != 0)
	{
		printf("%s%s", str, sBuf);
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
void *MyMalloc(int32_t Size)
{
	void *pPtr = NULL;

	if(Size >= 0x1000000u)
	{
		printf("error:メモリ確保サイズが大きすぎます(0x%x)\n", Size );
	}
	else

	{
		pPtr = _dos_malloc(Size);	/* メモリ確保 */

//		pPtr = malloc(Size);	/* メモリ確保 */
		
		if(pPtr == NULL)
		{
			puts("error:メモリが確保できませんでした");
		}
		else if((uint32_t)pPtr >= 0x81000000)
		{
			if((uint32_t)pPtr >= 0x82000000)
			{
				printf("error:メモリが確保できませんでした(0x%x)\n", (uint32_t)pPtr);
			}
			else
			{
				printf("error:メモリが確保できませんでした(0x%x)\n", (uint32_t)pPtr - 0x81000000 );
			}
			pPtr = NULL;
		}
		else
		{
			//printf("Mem Address 0x%p Size = %d[byte]\n", pPtr, Size);
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
void *MyMallocJ(int32_t Size)
{
	void *pPtr = NULL;
	
	PRAMREG(d0_reg, d0);		/* d0を変数d0_regに割り当てる */
	
	asm("\tmove.l 4(sp),d3");	/* スタックに格納された引数Sizeを(d3)へ代入 */
	
	asm("\tmove.l d3,-(sp)");	/* MALLOC3の引数にd3を入れる */
	
 	asm("\tDOS _MALLOC3");		/* MALLOC3 */

	asm("\taddq.l #4, sp");		
	
	pPtr = (void *)d0_reg;
	
	if(pPtr == NULL)
	{
		puts("error:メモリが確保できませんでした");
	}
	else if((uint32_t)pPtr >= 0x81000000)
	{
		if((uint32_t)pPtr >= 0x82000000)
		{
			printf("error:メモリが確保できませんでした(0x%x)\n", (uint32_t)pPtr);
		}
		else
		{
			printf("error:メモリが確保できませんでした(0x%x)\n", (uint32_t)pPtr - 0x81000000 );
		}
		pPtr = NULL;
	}
	else
	{
		printf("JMem Address 0x%p Size = %d[byte]\n", pPtr, Size);
	}
	
	return (void*)pPtr;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void *MyMallocHi(int32_t Size)
{
	void *pPtr = NULL;

	struct	_regs	stInReg = {0}, stOutReg = {0};
	int32_t		retReg;	/* d0 */
	
	stInReg.d0 = 0xF8;					/* IOCS _HIMEM */
	stInReg.d1 = 0x03;					/* HIMEM_GETSIZE */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	if(stOutReg.d0 == 0)
	{
		printf("error:メモリサイズ%d[MB](%d[byte])/一度に確保できる最大のサイズ %d[byte]\n", stOutReg.d0 >> 20u, stOutReg.d0, stOutReg.d1);
		return pPtr;
	}
	
	
	stInReg.d0 = 0xF8;					/* IOCS _HIMEM */
	stInReg.d1 = 0x01;					/* HIMEM_MALLOC */
	stInReg.d2 = Size;					/* サイズ */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	if(stOutReg.d0 == 0)
	{
		pPtr = (void *)stOutReg.a1;	/* 確保したメモリのアドレス */
		//printf("HiMem Address 0x%p Size = %d[byte]\n", pPtr, Size);
	}
	else
	{
		puts("error:メモリを確保することが出来ませんでした");
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
		puts("error:メモリ解放に失敗");
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
int16_t	MyMfreeJ(void *pPtr)
{
	int16_t ret = 0;
	
	asm("\tmove.l 4(sp),d3");	/* スタックに格納された引数pPtrを(d3)へ代入 */
	
	asm("\tmove.l d3,-(sp)");	/* _MFREEの引数にd3を入れる */
	
 	asm("\tDOS _MFREE");		/* _MFREE */

	asm("\taddq.l #4, sp");		
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	MyMfreeHi(void *pPtr)
{
	int16_t ret = 0;
	struct	_regs	stInReg = {0}, stOutReg = {0};
	int32_t	retReg;	/* d0 */
	
	if(pPtr == NULL)
	{
		puts("自プロセス、子プロセスで確保したメモリをフルで解放します");
	}
	
	stInReg.d0 = 0xF8;					/* IOCS _HIMEM */
	stInReg.d1 = 0x02;					/* HIMEM_FREE */
	stInReg.d2 = (int32_t)pPtr;			/* 確保したメモリの先頭アドレス */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	if(stOutReg.d0 < 0)
	{
		puts("error:メモリ解放に失敗");
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

