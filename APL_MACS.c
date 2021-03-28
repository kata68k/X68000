#ifndef	APL_MACS_C
#define	APL_MACS_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inc/usr_macro.h"
#include "APL_MACS.h"

#include "Moon.h"
#include "OverKata.h"
#include "FileManager.h"
#include "Graphic.h"
#include "PCG.h"

#ifdef 	MACS_MOON

/* define定義 */
#define	MACS_MAX	(16)

/* グローバル変数 */
static SC	mov_list[MACS_MAX][256]	=	{0};
static UI	mov_list_max	=	0u;
/* 構造体定義 */

#endif	/* MACS_MOON */

/* 関数のプロトタイプ宣言 */
void MOV_INIT(void);
SI MOV_Play(UC);
static UI moon_check(void);

/* 関数 */
void MOV_INIT(void)
{
#ifdef 	MACS_MOON
	US	i;
	SI	moon_chk = 0;

	/* MACS再生するためのドライバ常駐チェック */
	if(moon_check() == 0)
	{
		puts("MACSDRV.XとMOON.X 常駐ください");
		exit(0);
	}
	
	/* グラフィックリスト */
	Load_MACS_List("data\\mov\\", "mov_list.txt", mov_list, &mov_list_max);
	for(i = 0; i < mov_list_max; i++)
	{
		moon_chk = MoonRegst(mov_list[i]);	/* メモリへ登録 */
		if(moon_chk != 0)
		{
			printf("MoonRegst = %d\n", moon_chk);
		}
		printf("MACS File %2d = %s\n", i, mov_list[i] );
	}
#endif	/* MACS_MOON */
}

/* 動画再生 */
SI MOV_Play(UC bPlayNum)
{
	SI	ret=0;

	if(bPlayNum >= mov_list_max)return ret;

#ifdef 	MACS_MOON
#if 1	/* MOONを使う場合 */
	{
		SI	moon_stat = 0;

		moon_stat = MoonPlay(mov_list[bPlayNum]);	/* 再生 */
		if(moon_stat != 0)
		{
			//printf("MoonPlay  = %d\n", moon_stat);
			/* アボート(-4)した場合は、何かしら作法が必要だったような… */
		}
		else
		{
		}
	//	MACS_Sleep();	/* スリープ */
	}
#else	/* MACSDRV単独の場合 */
	{
		SC	*pBuff = NULL;
		SI	FileSize = 0;

		GetFileLength(mov_list[bPlayNum], &FileSize);	/* ファイルのサイズ取得 */
		printf("MacsData = %d\n", FileSize);
		pBuff = (SC*)MyMalloc(FileSize);	/* メモリ確保 */
		if(pBuff != NULL)
		{
			File_Load(mov_list[bPlayNum], pBuff, sizeof(UC), FileSize );	/* ファイル読み込みからメモリへ保存 */
			MACS_Play(pBuff);	/* 再生 */
			MyMfree(pBuff);	/* メモリ解放 */
		}
	}
#endif
#endif	/* MACS_MOON */

	return	ret;
}

static UI moon_check(void)
{
	UI	ret = 0;
#ifdef 	MACS_MOON
	UL *v = (UL *)( B_LPEEK((UL *)(0x1d1*4)) + 2 );
	if( B_LPEEK( v ) == 'MOON' && B_LPEEK( v+1 ) == 'IOCS' )
	{
		ret = 1;
	}
#else	/* MACS_MOON */
	ret = 1;
#endif	/* MACS_MOON */

	return ret;
}

#endif	/* APL_MACS_C */

