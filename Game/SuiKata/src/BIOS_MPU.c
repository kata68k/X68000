#ifndef	BIOS_MPU_C
#define	BIOS_MPU_C

/* system include */
#include <stdio.h>
#include <stdlib.h>
#include <sys/iocs.h>
#include <interrupt.h>

#include <usr_macro.h>
#include "BIOS_MPU.h"

/* define定義 */
#ifdef DEV_GCC
	/* Develop 版 GCC 拡張*/
	DOSCALL INTVCG (short);
	DOSCALL INTVCS (short, void *);
	DOSCALL PRINT (char *);
#else
	#include <doslib.h>
#endif

/*
stdin,stdout XC Ver 依存のため
sprintf でマク口にしておく
*/
#define Fprint(buf,fmt,mes) \
	do { \
		sprintf ((buf), (fmt), (mes)); \
		PRINT ((buf)); \
	} while (0)

static struct {
	int flag;				/* エラー種別フラグ*/
	unsigned short sr_reg;	/* エラー時の SR */
	int pc_reg;				/* エラー時の PC */
	char *mes;				/* メッセージ */
} trapbuf;

/* グローバル変数 */
int32_t	g_nIntLevel = 0;
int32_t	g_nIntCount = 0;

uint16_t *p_PhantomX_IO_REG = (uint16_t *)0xEA8000;
uint16_t *p_PhantomX_IO 	= (uint16_t *)0xEA8002;

/* スタティック変数 */
static void (*trap_12)();	/* 元の trap 12 ベクタを保存*/
static void (*trap_14)();	/* 元の trap 14 ベクタを保存*/

extern int _main;			/* スタートアップのダミー */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void interrupt trap14(void)
{
	PRAMREG (code, d7);	/* パラメータレジスタ d7 */
	PRAMREG (add, a6);	/* パラメータレジスタ a6 */
	SET_FRAME (a5);		/* フレームポインタを a5 に指定 */
	char *p =(char *)add;

	/* code からエラーを分析 */
	switch (code & 0xFFFF)
	{
		case 2:
			/* バスエラー */
			trapbuf.flag = code;
			trapbuf.mes = "バスエラーが発生しました";
			break;
		case 3:
			/* アドレスエラー */
			trapbuf.flag = code;
			trapbuf.mes = "アドレスエラーが発生しました";
			break;
		case 4:
			/* おかしな命令 */
			trapbuf.flag = code;
			trapbuf.mes = "おかしな命令を実行しました";
			break;
	}
	
	if ((code & 0xFF00))
	{
		if ((code & 0xFF) == 2)
		{
			trapbuf.flag = -1;
			trapbuf.mes = "ドライブの準備ができていません";
		}
	}
	
	if (trapbuf.flag || ((code & 0xFFFF) == 0x1F) || ((code & 0xFFFF) == 0x301F))
	{
		trapbuf.sr_reg = *(short *)p;
		p+=2;
		trapbuf.pc_reg = *(int *)p;
		asm ("\tmove.w #$ff,d0\n\ttrap #15\n");
		IJUMP_RTE();
	}
	else
		IJUMP(trap_14);
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void my_abort (void)
{
	char buf[64];
//	void (*usr_abort)(void);	/* ユーザのアボート処理関数 */
	
	INTVCS(0x2E,trap_14);
	
	if (usr_abort)
		usr_abort();
	
	if (trapbuf.flag > 0)
	{
		extern int _SSTA;
		extern int _PSTA;
		trapbuf.flag = 0;
		Fprint (buf, "%s\r\n", trapbuf.mes);
		Fprint (buf, " pc=%X" , trapbuf.pc_reg);
		Fprint (buf, " offset=%X\r\n", trapbuf.pc_reg - (int) &_main);
		exit(512);
	}
	else
	{
		Fprint (buf, "%s\r\n", trapbuf.mes);
		exit(256);
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void init_trap14 (void)
{
	trap_14 = (void *)INTVCG (0x2E);	/* $2Eはエラー表示 */
	INTVCS (0x2E, trap14);
	INTVCS (0xFFF2, my_abort);
	INTVCS (0xFFF1, my_abort);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
static void interrupt trap12(void)
{
	IRTE();	/* 割り込み関数の最後で必ず実施 */
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void init_trap12 (void)
{
	trap_12 = (void *)INTVCG (0x2C);	/* $2EはCOPY処理 */
	INTVCS (0x2C, trap12);
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Set_DI(void)
{
	if(g_nIntCount == 0)
	{
#if 0
		/*スーパーバイザーモード終了*/
		if(g_nSuperchk > 0)
		{
			_dos_super(g_nSuperchk);
		}
#endif
		g_nIntLevel = intlevel(6);	/* 割禁設定 */
		g_nIntCount = Minc(g_nIntCount, 1u);
		
#if 0
		/* スーパーバイザーモード開始 */
		g_nSuperchk = _dos_super(0);
#endif
	}
	else
	{
		g_nIntCount = Minc(g_nIntCount, 1u);
	}
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Set_EI(void)
{
	if(g_nIntCount == 1)
	{
		g_nIntCount = Mdec(g_nIntCount, 1);
		
		/* スプリアス割り込みの中の人も(以下略)より */
		/* MFPでデバイス毎の割込み禁止設定をする際には必ずSR操作で割込みレベルを上げておく。*/
		asm ("\ttst.b $E9A001\n\tnop\n");
		/*
			*8255(ｼﾞｮｲｽﾃｨｯｸ)の空読み
			nop		*直前までの命令パイプラインの同期
					*早い話、この命令終了までには
					*それ以前のバスサイクルなどが
					*完了していることが保証される。
		*/
#if 0
		/*スーパーバイザーモード終了*/
		if(g_nSuperchk > 0)
		{
			_dos_super(g_nSuperchk);
		}
#endif
		intlevel(g_nIntLevel);	/* 割禁解除 */
#if 0
		/* スーパーバイザーモード開始 */
		g_nSuperchk = _dos_super(0);
#endif
	}
	else
	{
		g_nIntCount = Mdec(g_nIntCount, 1);
	}
}

/*===========================================================================================*/
/* 関数名	:	*/
/* 引数		:	*/
/* 戻り値	:	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		:	*/
/*===========================================================================================*/
uint32_t mpu_cache_clr(void)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};
	uint32_t	retReg;

	stInReg.d0 = IOCS_SYS_STAT;		/* システム状態の設定 */
	stInReg.d1 = 0x03;				/* $03 キャッシュクリア */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */

	return stOutReg.d1;
}
#endif	/* BIOS_MPU_C */

/*===========================================================================================*/
/* 関数名	:	*/
/* 引数		:	*/
/* 戻り値	:	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		:	*/
/*===========================================================================================*/
int32_t	mpu_stat_chk(void)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};
	int32_t	retReg;	/* d0 */
	
	stInReg.d0 = IOCS_SYS_STAT;	/* IOCS _SYS_STAT($AC) */
	stInReg.d1 = 0;				/* MPUステータスの取得 */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
//	printf("SYS_STAT 0x%x\n", (retReg & 0x0F));
    /* 060turbo.manより
		$0000   MPUステータスの取得
			>d0.l:MPUステータス
					bit0〜7         MPUの種類(6=68060)
					bit14           MMUの有無(0=なし,1=あり)
					bit15           FPUの有無(0=なし,1=あり)
					bit16〜31       クロックスピード*10
	*/
	
	return (retReg & 0x0F);
}

/*===========================================================================================*/
/* 関数名	:	*/
/* 引数		:	*/
/* 戻り値	:	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		:	*/
/*===========================================================================================*/
int32_t	Get_ROM_Ver(void)
{
	int32_t dat, ver, year, month, day;
	
	dat = _iocs_romver();
	
	ver		= (dat & 0xFF000000) >> 24;
	year	= (dat & 0x00FF0000) >> 16;
	month	= (dat & 0x0000FF00) >> 8;
	day		= (dat & 0x000000FF);
	
	if(year >= 0x80)	/* 0x80〜0x99 → 19xx年(1980〜1999年) */
	{
//		printf("ROM ver%x.%x(19%02x/%02x/%02x)(0x%x)\n", ((ver&0xF0)>>4), (ver&0x0F),year, month, day, dat);
	}
	else				/* 0x00〜0x79 → 20xx年(2000〜2079年) */
	{
//		printf("ROM ver%x.%x(20%02x/%02x/%02x)(0x%x)\n", ((ver&0xF0)>>4), (ver&0x0F),year, month, day, dat);
	}
	
	return ver;
}
