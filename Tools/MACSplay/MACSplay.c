#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>
#include "inc/usr_define.h"
#include "FileManager.h"

#define IOCS_OFST		(0x100)
#define IOCS_MACS_CALL	(0xD0)
#define IOCS_HIMEM		(0xF8)
#define IOCS_SYS_STAT	(0xAC)
#define IOCS_ADPCMMOD	(0x67)

int32_t	MACS_Play(int8_t *, int32_t, int32_t, int32_t, int32_t);
int32_t	MACS_Load(int8_t *, int32_t, int8_t);
int32_t	MACS_Load_Hi(int8_t *, int32_t);
int32_t	MACS_CHK(void);
int32_t	SYS_STAT_CHK(void);
int32_t	HIMEM_CHK(void);
int32_t	PCM8A_CHK(void);
int32_t ADPCM_Stop(void);
void HelpMessage(void);
int16_t main(int16_t, int8_t **);

int32_t g_nCommand = 0;
int32_t g_nBuffSize = -1;
int32_t g_nAbort = 1;
int32_t g_nEffect = 0;
int32_t g_nPCM8Achk = 0;
int32_t g_nRepeat = 1;

/*	MACS_Play() MACS_Cal.docより
*	Input:
*		d1.w > コマンドコード
*		d2.l > バッファサイズ
*		d3.w > アボートフラグ
*		d4.l > 特殊効果フラグ
*		a1.l > 処理アドレス
*	------------------------------------------------------------------------------
*		d1.w = 0	アニメーション再生
*					(a1)から始まるアニメデータを再生します。
*	------------------------------------------------------------------------------
*		d2.lは通常-1($FFFFFFFF)にして下さい。
*				（アニメデータ再生中、d2.lのエリアを越えようとした時,再生を中止します）
*	------------------------------------------------------------------------------
*	Output:  d0.l > Status
*------------------------------------------------------------------------------
*	d1.w = 0	アニメーション再生
*				(a1)から始まるアニメデータを再生します。
*				d2.lは通常-1($FFFFFFFF)にして下さい。
*				（アニメデータ再生中、d2.lのエリアを越えようとした時
*				　再生を中止します）
*				d4.lについては下表を参照
*	
*	d1.w = 1	再生状況検査
*				リターン値が0以外ならばアニメーション再生中です
*	
*	d1.w = 2	中断リクエスト発行
*				アニメーション再生を中断するよう要求します。
*				d3.wの内容により中断後に処理を行なわせることが可能です
*	
*			d3.w = -1	中断後何もしない(通常)
*			d3.w = -2	中断後 d2,d4,a1の内容によりアニメーション開始
*			d3.w = -3	中断後 a1.lのアドレスにサブルーチンコール
*	
*	d1.w = 3	MACSバージョン検査
*				d0.wに常駐しているMACSのバージョン番号を返します。
*				(Version 1.16なら$01_16)
*	
*	d1.w = 4	MACSアプリケーションを登録します
*				a1.lにアプリケーション名へのポインタを設定します。
*				アプリケーション名は
*					'～～.x [アプリケーション名]',0
*				というフォーマットに従って下さい。このコールでアプリを
*				登録することで、MACSの常駐解除を禁止することができます
*	
*	d1.w = 5	MACSアプリケーションの登録を解除します
*				a1.lに(d1.w=4)で登録したアプリケーション名へのポインタ
*				を渡しコールすると登録アプリケーションを解除します。
*				登録可能アプリケーションは最大16個です。(Ver1.00)
*	
*	d1.w = 6	MACSデータのデータバージョンを得ます
*				a1.lにMACSデータの先頭アドレスを入れてコールします。
*				返り値としてd0.lにデータが作成されたMACSのバージョンが
*				入ります。d0.l=-1の時はMACSデータではありません。
*	
*	d1.w = 7	画面を退避し、使用可能にします
*				アニメーションスタートと同じ処理を行ないます。
*				このコールを利用する時は必ずアボートフラグに対応して
*				下さい。
*	
*	d1.w = 8	退避した画面を元に戻します
*				画面再初期化抑制フラグを1にしてアニメーション再生を行
*				った時は必ずあとでこのコールを発行して下さい。
*	
*	d1.w = 9	テキストVRAMにグラフィックを展開します
*				a1.lにMACS形式グラフィックデータのアドレス，a2.lに展開
*				先のテキストVRAMアドレスを指定してコールします。
*	
*	d1.w = 10	垂直同期割り込みを登録します
*				a1.lに割り込みルーチンのアドレス、d2.wにフラグ*256+カ
*				ウンタをセットしてコールします。
*				フラグは現在使用されていません。0にしておいて下さい。
*	
*	d1.w = 11	垂直同期割り込みを解除します
*				a1.lに登録した割り込みルーチンのアドレスをセットして
*				コールします。
*	
*	d1.w = 12	スリープ状況をチェックします
*				リターン値が0以外ならMACSDRVはスリープしています
*	
*	d1.w = 13	スリープ状態を設定します
*				d2.wが0なら再生可能状態，0以外ならスリープします。
*	
*	d1.w = -1	ワークエリアアドレス獲得
*				d0.lにMACS内部ワークエリアのアドレスを返します。
*------------------------------------------------------------------------------
*特殊効果フラグ (d4.l)
*
*bit 0	ハーフトーン			 		(1の時パレットを50%暗くする)
*bit 1	PCMカット				 		(1の時PCMの再生をしない)
*bit 2	キーセンスアボートOFF 			(1の時キー入力時終了しない)
*bit 3	画面再初期化抑制フラグ 			(1の時アニメ終了時画面を初期化しない)
*bit 4	スリープ状態無視 				(1の時MACSDRVがスリープしている時でも再生する)
*bit 5	グラフィックＶＲＡＭ表示フラグ 	(1の時ｸﾞﾗﾌｨｯｸを表示したまま再生する)
*bit 6	スプライト表示フラグ 			(1の時ｽﾌﾟﾗｲﾄを表示したまま再生する)
*
*bit 7
*  :		Reserved. (必ず0にすること)
*bit31
*
*------------------------------------------------------------------------------
* Status エラーコード一覧 (d0.l)
*
*-1	MACSデータではありません
*-2	MACSのバージョンが古すぎます
*-3	データがバッファに全部入力されていません
*-4	アボートされました
*-5	既に現在アニメーション再生中です
*-6	無効なファンクションを指定しました
*-7	ユーザーリストもしくは割り込みリストが一杯です
*-8	ユーザーリストもしくは割り込みリストに登録されていません
*==============================================================================
*/

int32_t	MACS_Play(int8_t *pMacsBuff, int32_t nCommand, int32_t nBuffSize, int32_t nAbort, int32_t nEffect)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};
	int32_t	retReg;	/* d0 */
	
	stInReg.d0 = IOCS_MACS_CALL;	/* IOCS _MACS(macsdrv.x) */
	stInReg.d1 = nCommand;			/* コマンドコード(w) */
	stInReg.d2 = nBuffSize;			/* バッファサイズ(l) */
	stInReg.d3 = nAbort;			/* アボートフラグ(w) */
	stInReg.d4 = nEffect;			/* 特殊効果フラグ(l) */
	stInReg.a1 = (int32_t)pMacsBuff;	/* MACSデータの先頭アドレス */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	return retReg;
}

int32_t	MACS_Load(int8_t *sFileName, int32_t nFileSize, int8_t bMode)
{
	int32_t ret = 0;
	int8_t	*pBuff = NULL;
	int32_t nLoop;
	
	switch(bMode)
	{
	case 0:
		pBuff = (int8_t*)MyMalloc(nFileSize);	/* メモリ確保 */
		break;
	case 1:
		pBuff = (int8_t*)MyMallocJ(nFileSize);	/* メモリ確保 */
		break;
	case 2:
		pBuff = (int8_t*)MyMallocHi(nFileSize);	/* メモリ確保 */
		break;
	default:
		pBuff = (int8_t*)MyMalloc(nFileSize);	/* メモリ確保 */
		break;
	}
	
	if(pBuff != NULL)
	{
		File_Load(sFileName, pBuff, sizeof(uint8_t), nFileSize );	/* ファイル読み込みからメモリへ保存 */
		
		nLoop = g_nRepeat;
		do
		{
			if(g_nRepeat == 0)
			{
				nLoop = 1;
			}
			else
			{
				nLoop--;
			}
			
			ret = MACS_Play(pBuff, g_nCommand, g_nBuffSize, g_nAbort, g_nEffect);	/* 再生 */
			if(ret < 0)
			{
				break;
			}
		}
		while(nLoop);
		
		switch(bMode)
		{
		case 0:
			MyMfree(pBuff);		/* メモリ解放 */
			break;
		case 1:
			MyMfreeJ(pBuff);	/* メモリ解放 */
			break;
		case 2:
			MyMfreeHi(pBuff);	/* メモリ解放 */
			break;
		default:
			MyMfree(pBuff);		/* メモリ解放 */
			break;
		}
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

int32_t	MACS_CHK(void)
{
	int32_t ret = 0;	/* 0:常駐 !0:常駐していない */
	
	int32_t addr_IOCS;
	int32_t addr_MACSDRV;
	int32_t data;
	
	addr_IOCS = ((IOCS_OFST + IOCS_MACS_CALL) * 4);
	
	addr_MACSDRV = _iocs_b_lpeek((int32_t *)addr_IOCS);
	addr_MACSDRV += 2;
	
	data = _iocs_b_lpeek((int32_t *)addr_MACSDRV);
	if(data != 'MACS')ret |= 1;	/* アンマッチ */
	
	addr_MACSDRV += sizeof(int32_t);
	
	data = _iocs_b_lpeek((int32_t *)addr_MACSDRV);
	if(data != 'IOCS')ret |= 1;	/* アンマッチ */
	
	return ret;
}

int32_t	SYS_STAT_CHK(void)
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
					bit0～7         MPUの種類(6=68060)
					bit14           MMUの有無(0=なし,1=あり)
					bit15           FPUの有無(0=なし,1=あり)
					bit16～31       クロックスピード*10
	*/
	
	return (retReg & 0x0F);
}

int32_t	HIMEM_CHK(void)
{
	int32_t ret = 0;	/* 0:常駐 !0:常駐していない */
	
	int32_t addr_IOCS;
	int32_t addr_HIMEM;
	int32_t data;
	int8_t data_b;
	
	addr_IOCS = ((IOCS_OFST + IOCS_HIMEM) * 4);
	
	addr_HIMEM = _iocs_b_lpeek((int32_t *)addr_IOCS);
	addr_HIMEM -= 6;
	
	data = _iocs_b_lpeek((int32_t *)addr_HIMEM);
	if(data != 'HIME')ret |= 1;	/* アンマッチ */
//	printf("data %x == %x\n", data, 'HIME');
	
	addr_HIMEM += 4;
	
	data_b = _iocs_b_bpeek((int32_t *)addr_HIMEM);
	if(data_b != 'M')ret |= 1;	/* アンマッチ */
//	printf("data %x == %x\n", data_b, 'M');
	
	return ret;
}

int32_t	PCM8A_CHK(void)
{
	struct	_regs	stInReg = {0}, stOutReg = {0};
	int32_t	retReg;	/* d0 */
	
	stInReg.d0 = IOCS_ADPCMMOD;	/* IOCS _ADPCMMOD($67) */
	stInReg.d1 = 'PCMA';		/* PCM8Aの常駐確認 */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	
	return retReg;
}

/* (AD)PCM効果音の停止 */
int32_t ADPCM_Stop(void)
{
	int32_t	ret=0;
	
	if(_iocs_adpcmsns() != 0)	/* 何かしている */
	{
		_iocs_adpcmmod(1);	/* 中止 */
		_iocs_adpcmmod(0);	/* 終了 */
	}
	
	return	ret;
}

void HelpMessage(void)
{
	puts("MACSデータの再生を行います。");
	puts("  68030以降に限りメインメモリが足りない場合はハイメモリを使って再生を行います。");
	puts("  ※ハイメモリでADPCMを再生する際は、PCM8A.X -w18 での常駐");
	puts("    もしくは、060turbo.sys -ad を設定ください。");
	puts("");
	puts("Usage:MACSplay.x [Option] file");
	puts("Option:-D  パレットを50%暗くする");
	puts("       -P  PCMの再生をしない");
	puts("       -K  キー入力時、終了しない");
	puts("       -C  アニメ終了時、画面を初期化しない");
	puts("       -SL MACSDRVがスリープしている時でも再生する");
	puts("       -GR ｸﾞﾗﾌｨｯｸを表示したまま再生する");
	puts("       -SP ｽﾌﾟﾗｲﾄを表示したまま再生する");
	puts("       -AD PCM8A.Xの常駐をチェックしない(060turbo.sys -adで再生する場合)");
	puts("       -Rx リピート再生するx=1～255回、x=0(無限ループ)");
}

int16_t main(int16_t argc, int8_t *argv[])
{
	int16_t ret = 0;
	int32_t nOut = 0;
	int32_t	nFileSize = 0;
	int32_t	nFilePos = 0;
	
	puts("MACS data Player「MACSplay.x」v1.02 (c)2022 カタ.");
	
	if(argc > 1)	/* オプションチェック */
	{
		int16_t i;
		
		for(i = 1; i < argc; i++)
		{
			int8_t	bOption;
			int8_t	bFlag;
			int8_t	bFlag2;
			
			bOption	= ((argv[i][0] == '-') || (argv[i][0] == '/')) ? TRUE : FALSE;

			if(bOption == TRUE)
			{
				bFlag	= ((argv[i][1] == '?') || (argv[i][1] == 'h') || (argv[i][1] == 'H')) ? TRUE : FALSE;
				
				if(bFlag == TRUE)
				{
					HelpMessage();	/* ヘルプ */
					ret = -1;
				}
				
				bFlag	= ((argv[i][1] == 'd') || (argv[i][1] == 'D')) ? TRUE : FALSE;
				if(bFlag == TRUE)
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_0);
					puts("パレットを50%暗くします");
					continue;
				}

				bFlag	= ((argv[i][1] == 'p') || (argv[i][1] == 'P')) ? TRUE : FALSE;
				if(bFlag == TRUE)
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_1);
					puts("PCMの再生をしません");
					continue;
				}

				bFlag	= ((argv[i][1] == 'k') || (argv[i][1] == 'K')) ? TRUE : FALSE;
				if(bFlag == TRUE)
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_2);
					puts("キー入力時、終了しません");
					continue;
				}

				bFlag	= ((argv[i][1] == 'c') || (argv[i][1] == 'C')) ? TRUE : FALSE;
				if(bFlag == TRUE)
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_3);
					puts("アニメ終了時、画面を初期化しない");
					continue;
				}
				
				bFlag	= ((argv[i][1] == 's') || (argv[i][1] == 'S')) ? TRUE : FALSE;
				bFlag2	= ((argv[i][2] == 'l') || (argv[i][2] == 'L')) ? TRUE : FALSE;
				if((bFlag == TRUE) && (bFlag2 == TRUE))
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_4);
					puts("MACSDRVがスリープしている時でも再生する");
					continue;
				}

				bFlag	= ((argv[i][1] == 'g') || (argv[i][1] == 'G')) ? TRUE : FALSE;
				bFlag2	= ((argv[i][2] == 'r') || (argv[i][2] == 'R')) ? TRUE : FALSE;
				if((bFlag == TRUE) && (bFlag2 == TRUE))
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_5);
					puts("ｸﾞﾗﾌｨｯｸを表示したまま再生する");
					continue;
				}
				
				bFlag	= ((argv[i][1] == 's') || (argv[i][1] == 'S')) ? TRUE : FALSE;
				bFlag2	= ((argv[i][2] == 'p') || (argv[i][2] == 'P')) ? TRUE : FALSE;
				if((bFlag == TRUE) && (bFlag2 == TRUE))
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_6);
					puts("ｽﾌﾟﾗｲﾄを表示したまま再生する");
					continue;
				}

				bFlag	= ((argv[i][1] == 'a') || (argv[i][1] == 'A')) ? TRUE : FALSE;
				bFlag2	= ((argv[i][2] == 'd') || (argv[i][2] == 'D')) ? TRUE : FALSE;
				if((bFlag == TRUE) && (bFlag2 == TRUE))
				{
					g_nPCM8Achk = 1;
					puts("PCM8A.Xの常駐をチェックしない");
					continue;
				}

				bFlag	= ((argv[i][1] == 'r') || (argv[i][1] == 'R')) ? TRUE : FALSE;
				bFlag2	= strlen(&argv[i][2]);
				if((bFlag == TRUE) && (bFlag2 > 0))
				{
					g_nRepeat = atoi(&argv[i][2]);
					if(g_nRepeat < 0)			g_nRepeat = 1;
					else if(g_nRepeat > 255)	g_nRepeat = 1;

					g_nEffect = Mbclr(g_nEffect, Bit_2);	/* -K キー入力終了を有効にする */
					g_nEffect = Mbclr(g_nEffect, Bit_3);	/* -C アニメ終了時画面を初期化しないを有効にする */
					if(g_nRepeat == 0)
					{
						puts("無限ループで再生する");
					}
					else
					{
						printf("%d回リピート再生する\n", g_nRepeat);
					}
					continue;
				}
				
				HelpMessage();	/* ヘルプ */
				ret = -1;
				break;
			}
			else
			{
				if(GetFileLength(argv[i], &nFileSize) < 0)	/* ファイルのサイズ取得 */
				{
					printf("ファイルが見つかりませんでした。%s\n", argv[i]);
				}
				else
				{
					nFilePos = i;
//					printf("File Size = %d[MB](%d[KB], %d[byte])\n", nFileSize>>20, nFileSize>>10, nFileSize);
				}
			}
		}
	}
	else
	{
		HelpMessage();	/* ヘルプ */
		ret = -1;
	}
	
	if(MACS_CHK() != 0)
	{
		puts("error：MACSDRVを常駐してください！");
		ret = -1;
	}
	
	if(ret == 0)
	{
		//printf("Option = 0x%x\n", g_nEffect);

		if(nFileSize != 0)
		{
			int32_t	nSysStat;
			int32_t	nMemSize;
			int32_t	nHiMemChk;
			int8_t	data_b;
			
			nMemSize = (int32_t)_dos_malloc(-1) - 0x81000000UL;	/* メインメモリの空きチェック */
			
			data_b = _iocs_b_bpeek((int32_t *)0xCBC);	/* 機種判別 */
//			printf("機種判別(MPU680%d0)\n", data_b);
			
			if(data_b != 0)	/* 68000以外のMPU */
			{
				nSysStat = SYS_STAT_CHK();	/* MPUの種類 */
			}
			else
			{
				nSysStat = (int32_t)data_b;	/* 0が入る */
			}
		
			nHiMemChk = HIMEM_CHK();	/* ハイメモリ実装チェック */

			if(nFileSize <= nMemSize)	/* メインメモリ */
			{
				printf("メインメモリで再生します(%d[kb] <= %d[kb])\n", nFileSize>>10, nMemSize>>10);
				nOut = MACS_Load(argv[nFilePos], nFileSize, 0);		/* メイン再生 */
			}
			else if((nHiMemChk != 0) && ((nSysStat == 4) || (nSysStat == 6)) )		/* ハイメモリ未実装 & 040/060EXCEL */
			{
				puts("拡張されたメモリで再生を試みます(040/060EXCEL)");
				nOut = MACS_Load(argv[nFilePos], nFileSize, 1);		/* 拡張されたメモリで再生 */
			}
			else if(nHiMemChk == 0)		/* ハイメモリ実装 */
			{
				int32_t	nChk = PCM8A_CHK();
				
				if( (nChk >= 0) || (g_nPCM8Achk > 0))	/* PCM8A常駐チェック or -ADで無効化 */
				{
					puts("ハイメモリで再生します");
					nOut = MACS_Load(argv[nFilePos], nFileSize, 2);	/* ハイメモリ再生 */
				}
				else
				{
					if(nChk < 0)	/* >=0:常駐 <0:常駐していない */
					{
						puts("error:PCM8Aが常駐されていません");
						puts("※ハイメモリを使う場合は、PCM8A -w18 で常駐ください。");
					}
				}
			}
			else						/* ハイメモリ or NG */
			{
				puts("error：メインメモリの空きが不足です。");
				puts("error：HIMEMが見つかりませんでした。");
				ret = -1;
			}
		}
	}
	
	if(nOut < 0)
	{
		switch(nOut)
		{
		case -1:
			puts("再生に失敗しました。");
			break;
		case -4:
			puts("再生を中断しました。");
			break;
		default:
			puts("再生を完了しました。");
			//printf("MACSplay.x = %d\n", nOut);
			break;
		}
	} 
	ADPCM_Stop();	/* 音を止める */
	
	return ret;
}
