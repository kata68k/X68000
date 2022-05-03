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


/* 関数のプロトタイプ宣言 */
int16_t File_Load(int8_t *, void *, size_t, size_t);
int16_t File_Save(int8_t *, void *, size_t, size_t);
int16_t GetFileLength(int8_t *, int32_t *);
void *MyMalloc(int32_t);
void *MyMallocHi(int32_t);
int16_t MyMfree(void *);
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
				puts("error:メモリ不足です");
			}
			else
			{
				printf("error:メモリが確保できませんでした(%d)\n", (uint32_t)pPtr - 0x81000000 );
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