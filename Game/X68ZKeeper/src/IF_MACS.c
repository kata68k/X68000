#ifndef	IF_MACS_C
#define	IF_MACS_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usr_macro.h>
#include "IF_MACS.h"
#include "BIOS_CRTC.h"
#include "BIOS_Moon.h"
#include "BIOS_PCG.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"

#ifdef 	MACS_MOON

/* グローバル変数 */
int8_t		g_mov_on = FALSE;
MOV_LIST	g_mov_list[MACS_MAX];
uint32_t	g_mov_list_max	=	0u;
/* 構造体定義 */

#endif	/* MACS_MOON */

/* 関数のプロトタイプ宣言 */
void MOV_INIT(void);
int32_t MOV_Play(uint8_t);
int32_t MOV_Play2(uint8_t);
static uint32_t moon_check(void);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void MOV_INIT(void)
{
#if CNF_MACS

#ifdef 	MACS_MOON
	uint16_t	i;
	int32_t	moon_chk = 0;

	/* MACS再生するためのドライバ常駐チェック */
	if(moon_check() == 0)
	{
		puts("MACSDRV.XとMOON.X 常駐ください");
		exit(0);
	}
	
	/* リストからMoon.xでメモリ登録 */
	for(i = 0; i < g_mov_list_max; i++)
	{
		moon_chk = MoonRegst(&g_mov_list[i].bFileName[0]);	/* メモリへ登録 */
		if(moon_chk < 0)
		{
			printf("MoonRegst = %d\n", moon_chk);
		}
		else
		{
			puts("error:Moonに登録できませんでした。");
		}
	}
	g_mov_on = TRUE;
#else
	g_mov_on = FALSE;
#endif	/* MACS_MOON */

#endif	/* CNF_MACS */
}

/* 動画再生 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t MOV_Play(uint8_t bPlayNum)
{
	int32_t	ret=0;

	if(g_mov_on == FALSE)return ret;

#if CNF_MACS
	if(bPlayNum >= g_mov_list_max)return ret;

#ifdef 	MACS_MOON
	{
		int32_t	moon_stat = 0;

		moon_stat = MoonPlay(&g_mov_list[bPlayNum].bFileName[0]);	/* 再生 */
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
		int8_t	*pBuff = NULL;
		int32_t	FileSize = 0;

		if(GetFileLength(&g_mov_list[bPlayNum].bFileName[0], &FileSize) == -1)	/* ファイルのサイズ取得 */
		{

		}
		else
		{
			if(FileSize != 0)
			{
				printf("MacsData = %d\n", FileSize);
				pBuff = (int8_t*)MyMalloc(FileSize);	/* メモリ確保 */
				if(pBuff != NULL)
				{
					File_Load(g_mov_list[bPlayNum], pBuff, sizeof(uint8_t), FileSize );	/* ファイル読み込みからメモリへ保存 */
					MACS_Play(pBuff);	/* 再生 */
					MyMfree(pBuff);	/* メモリ解放 */
				}
			}
		}
	}
#endif	/* MACS_MOON */

#endif	/* CNF_MACS */
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t MOV_Play2(uint8_t bPlayNum)
{
	int32_t	ret=0;

	if(g_mov_on == FALSE)return ret;

#if CNF_MACS
	if(bPlayNum >= g_mov_list_max)return ret;

#ifdef 	MACS_MOON
	{
		int32_t	moon_stat = 0;

		moon_stat = MoonPlay2(&g_mov_list[bPlayNum].bFileName[0], 0x64 & (g_mov_list[bPlayNum].bKey << 2) | (g_mov_list[bPlayNum].bGR << 5) | (g_mov_list[bPlayNum].bSP << 6));	/* 再生 */
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
		int8_t	*pBuff = NULL;
		int32_t	FileSize = 0;

		if(GetFileLength(&g_mov_list[bPlayNum].bFileName[0], &FileSize) == -1)	/* ファイルのサイズ取得 */
		{

		}
		else
		{
			if(FileSize != 0)
			{
				printf("MacsData = %d\n", FileSize);
				pBuff = (int8_t*)MyMalloc(FileSize);	/* メモリ確保 */
				if(pBuff != NULL)
				{
					File_Load(&g_mov_list[bPlayNum].bFileName[0], pBuff, sizeof(uint8_t), FileSize );	/* ファイル読み込みからメモリへ保存 */
					MACS_Play(pBuff);	/* 再生 */
					MyMfree(pBuff);	/* メモリ解放 */
				}
			}
		}
	}
#endif	/* MACS_MOON */

#endif	/* CNF_MACS */
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static uint32_t moon_check(void)
{
	uint32_t	ret = 0;
#ifdef 	MACS_MOON
	uint64_t *v = (uint64_t *)( B_LPEEK((uint64_t *)(0x1d1*4)) + 2 );
	if( B_LPEEK( v ) == 'MOON' && B_LPEEK( v+1 ) == 'IOCS' )
	{
		ret = 1;
	}
#else	/* MACS_MOON */
	ret = 1;
#endif	/* MACS_MOON */

	return ret;
}

#endif	/* IF_MACS_C */

