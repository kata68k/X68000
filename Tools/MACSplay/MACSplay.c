#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>
#include "inc/usr_define.h"
#include "FileManager.h"

#define _MACS_CALL	(0xD0)
#define _HIMEM		(0xF8)

int32_t	MACS_Play(int8_t *, int32_t, int32_t, int32_t, int32_t);
int32_t	MACS_Load(int8_t *, int32_t);
int32_t	MACS_Load_Hi(int8_t *, int32_t);
int32_t	MACS_CHK(void);
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

/*	MACS_Play()
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
	
	stInReg.d0 = _MACS_CALL;	/* IOCS _MACS(macsdrv.x) */
	stInReg.d1 = nCommand;		/* コマンドコード(w) */
	stInReg.d2 = nBuffSize;		/* バッファサイズ(l) */
	stInReg.d3 = nAbort;		/* アボートフラグ(w) */
	stInReg.d4 = nEffect;		/* 特殊効果フラグ(l) */
	stInReg.a1 = (int32_t)pMacsBuff;	/* MACSデータの先頭アドレス */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	return retReg;
}

int32_t	MACS_Load(int8_t *sFileName, int32_t nFileSize)
{
	int32_t ret = 0;
	int8_t	*pBuff = NULL;
	int32_t nLoop;
	
	pBuff = (int8_t*)MyMalloc(nFileSize);	/* メモリ確保 */
	
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
		
		MyMfree(pBuff);		/* メモリ解放 */
	}
	
	return ret;
}

int32_t	MACS_Load_Hi(int8_t *sFileName, int32_t nFileSize)
{
	int32_t ret = 0;
	int8_t	*pBuff = NULL;
	int32_t nLoop;
	
	pBuff = (int8_t*)MyMallocHi(nFileSize);	/* メモリ確保 */
	
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

		MyMfreeHi(pBuff);	/* メモリ解放 */
	}
	
	return ret;
}

int32_t	MACS_CHK(void)
{
	int32_t ret = 0;	/* 0:常駐 !0:常駐していない */
	
	int32_t addr_IOCS;
	int32_t addr_MACSDRV;
	int32_t data;
	
	addr_IOCS = ((0x100 + _MACS_CALL) * 4);
	
	addr_MACSDRV = _iocs_b_lpeek((int32_t *)addr_IOCS);
	addr_MACSDRV += 2;
	
	data = _iocs_b_lpeek((int32_t *)addr_MACSDRV);
	if(data != 'MACS')ret |= 1;	/* アンマッチ */
	
	addr_MACSDRV += sizeof(int32_t);
	
	data = _iocs_b_lpeek((int32_t *)addr_MACSDRV);
	if(data != 'IOCS')ret |= 1;	/* アンマッチ */
	
	return ret;
}

int32_t	HIMEM_CHK(void)
{
	int32_t ret = 0;	/* 0:常駐 !0:常駐していない */
	
	int32_t addr_IOCS;
	int32_t addr_HIMEM;
	int32_t data;
	int8_t data_b;
	
	addr_IOCS = ((0x100 + _HIMEM) * 4);
	
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
	
	stInReg.d0 = 0x67;			/* IOCS _ADPCMMOD($67) */
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
	puts("MACS data Player「MACSplay.x」v1.00 (c)2022 カタ.");
	puts("MACSデータの再生を行います。");
	puts("  68030以降に限りローカルメモリが足りない場合はハイメモリを使って再生を行います。");
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
					puts("-SL MACSDRVがスリープしている時でも再生する");
					continue;
				}

				bFlag	= ((argv[i][1] == 'g') || (argv[i][1] == 'G')) ? TRUE : FALSE;
				bFlag2	= ((argv[i][2] == 'r') || (argv[i][2] == 'R')) ? TRUE : FALSE;
				if((bFlag == TRUE) && (bFlag2 == TRUE))
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_5);
					puts("-GR ｸﾞﾗﾌｨｯｸを表示したまま再生する");
					continue;
				}
				
				bFlag	= ((argv[i][1] == 's') || (argv[i][1] == 'S')) ? TRUE : FALSE;
				bFlag2	= ((argv[i][2] == 'p') || (argv[i][2] == 'P')) ? TRUE : FALSE;
				if((bFlag == TRUE) && (bFlag2 == TRUE))
				{
					g_nEffect = Mbset(g_nEffect, 0x7F, Bit_6);
					puts("-SP ｽﾌﾟﾗｲﾄを表示したまま再生する");
					continue;
				}

				bFlag	= ((argv[i][1] == 'a') || (argv[i][1] == 'A')) ? TRUE : FALSE;
				bFlag2	= ((argv[i][2] == 'd') || (argv[i][2] == 'D')) ? TRUE : FALSE;
				if((bFlag == TRUE) && (bFlag2 == TRUE))
				{
					g_nPCM8Achk = 1;
					puts("-AD PCM8A.Xの常駐をチェックしない");
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
					puts("-Rx リピート再生するx=1～255回、x=0(無限ループ)");
					//printf("Repeat = %d\n", g_nRepeat);
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
					printf("File Size = %d[MB](%d[KB], %d[byte])\n", nFileSize>>20, nFileSize>>10, nFileSize);
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
			int32_t	nMemSize;
			int32_t	nHiMemChk;
			
			nMemSize = (int32_t)_dos_malloc(-1) - 0x81000000UL;	/* ローカルメインメモリの空きチェック */
			nHiMemChk = HIMEM_CHK();	/* ハイメモリ実装チェック */
			
			if(nFileSize <= nMemSize)	/* ローカル */
			{
				printf("ローカルメモリで再生します(%d[kb] <= %d[kb])\n", nFileSize>>10, nMemSize>>10);
				nOut = MACS_Load(argv[nFilePos], nFileSize);		/* ローカル再生 */
			}
			else if(nHiMemChk == 0)		/* ハイメモリ実装 */
			{
				int32_t	nChk = PCM8A_CHK();
				
				if( (nChk >= 0) || (g_nPCM8Achk > 0))	/* PCM8A常駐チェック or -ADで無効化 */
				{
					puts("ハイメモリで再生します");
					nOut = MACS_Load_Hi(argv[nFilePos], nFileSize);	/* ハイメモリ再生 */
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
				puts("error：ローカルメモリの空きが不足です。");
				puts("error：HIMEMが見つかりませんでした。");
				ret = -1;
			}
		}
	}
	
	if(nOut < 0)
	{
		switch(nOut)
		{
		case -4:
			puts("再生を中断しました。");
			break;
		default:
			//printf("MACSplay.x = %d\n", nOut);
			break;
		}
	} 
	ADPCM_Stop();	/* 音を止める */
	
	return ret;
}
