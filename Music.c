#ifndef	MUSIC_C
#define	MUSIC_C

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <sys/iocs.h>
#include <interrupt.h>

#include "inc/usr_macro.h"
#include "MUSIC.h"
#include "FileManager.h"
#include "Output_Text.h"

#define ZM_V3

#ifdef	ZM_V2
		#include "inc/ZMUSIC.H"
#else
	#ifdef	ZM_V3
		#include "inc/ZMSC3LIB.H"
	#else
		#error "No Music Lib"
	#endif
#endif

#ifdef	ZM_V2
#else
	#ifdef	ZM_V3
	#else
		#error "No Music Lib"
	#endif
#endif

#define ZM_NUM_V2	(0x02)
#define ZM_NUM_V3	(0x03)

#define	MUSIC_MAX	(16)
#define	SOUND_MAX	(16)
#define	ADPCM_MAX	(16)

/* グローバル変数 */
static SC	music_list[MUSIC_MAX][256]	=	{0};
static SC	music_dat[MUSIC_MAX][4096]	=	{0};
static SS	music_dat_size[MUSIC_MAX]	=	{0};
static UI	m_list_max	=	0u;

static SC	se_list[SOUND_MAX][256]	=	{0};
static SC	se_dat[SOUND_MAX][4096]	=	{0};
static SS	se_dat_size[SOUND_MAX]	=	{0};
static SI	se_dat_addr[SOUND_MAX]	=	{0};
static UI	s_list_max	=	0u;

static SC	adpcm_list[ADPCM_MAX][256]	=	{0};
static SC	adpcm_dat[ADPCM_MAX][32768]	=	{0};
static SI	adpcm_dat_size[ADPCM_MAX]	=	{0};
static UI	p_list_max	=	0u;

UC	v[] = {
#if 1
/* 自機ミサイル */
/*	AF  OM  WF  SY  SP PMD AMD PMS AMS PAN DUMMY	*/
	60, 15,  0,  0,  0,  0,  0,  0,  0,  3,  0,
/*	AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME	*/
	27, 15,  5,  2,  0,  0,  0,  0,  3,  1,  0,
	31, 18, 18,  6,  7,  0,  0,  0,  3,  2,  0,
	22, 31,  0, 10,  0, 42,  0,  7,  7,  0,  0,
	15, 31,  0,  8,  0,  0,  2,  1,  7,  0,  0
#else
/*	AF  OM  WF  SY  SP PMD AMD PMS AMS PAN DUMMY	*/
	59, 15,  2,  1,200,127,  0,  0,  0,  3,  0,
/*	AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME */
	31,  8,  1,  8,  7, 20,  2,  1,  5,  3,  0,
	31,  8,  8,  7,  5, 24,  1,  2,  1,  1,  0,
	31,  3,  7,  8,  1, 21,  1,  1,  3,  0,  0,
	31,  0,  0,  9,  0,  0,  2,  8,  5,  2,  0
#endif
};

UC	SE_Data[] = {	/* 構造体にした方がよい？ */
//		0x01,										/* (+1)ZMDの構造 */
//		0x5A,0x6D,0x75,0x53,0x69,0x43,			/* (+6)ZmuSiC */
//		0x20,										/* (+7)バージョン */
#if 1
	0x00,0x01,									/* 演奏総トラック数(.W) */
	0x00,0x00,0x00,0x02,						/* 演奏データまでのオフセット(.L) */
	0x00,0x05,									/* 演奏絶対チャンネル(.W) -1 */
	0xa0,0x0c,0xb6,0x05,0xb3,0xd1,0x00,0x00,	/* 演奏データ */
	0x00,0x00,0xe6,0x00,0x3c,0xe8,0x00,0x18,
	0xff,0xff,0xd6,0x00,0x03,0x00,0x00,0xe0,
	0x39,0x00,0x0c,0xff,0xff,0x00,0x00,0x00,
	0x40,0x00,0x01,0x45,0x48,0xff,0xe0,0x45,
	0x00,0x0c,0x00,0x0c,0x00,0x00,0xff,0xc0,
	0x00,0xff,0xff,0xff
#else
	0x00, 0x01, 
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x05,
	0xA0, 0x80, 0xB6, 0x00, 0xB3, 0xD1, 0x00, 0x00,
	0x00, 0x00, 0xBB, 0x00, 0xE8, 0x00, 0x00, 0x00,
	0x00, 0xD6, 0x00, 0x06, 0x00, 0x06, 0x1A, 0x01,
	0xFF, 0x1C, 0x01, 0xFF, 0x1F, 0x01, 0xFF, 0x21,
	0x01, 0xFF, 0x25, 0x01, 0xFF, 0x21, 0x01, 0xFF,
	0x1F, 0x01, 0xFF, 0x1C, 0x01, 0xFF, 0xC1, 0xCF,
	0x06, 0x25, 0x01, 0xFF, 0x1A, 0x01, 0xFF, 0x1C,
	0x01, 0xFF, 0x1E, 0x01, 0xFF, 0x1F, 0x01, 0xFF,
	0xAB, 0x08, 0xC2, 0x00, 0x16, 0xFF
#endif	
};

/* 構造体定義 */

/* 関数のプロトタイプ宣言 */
void Init_Music(void);
void Exit_Music(void);
SI Music_Play(UC);
SI Music_Stop(void);
SI SE_Play(UC);
SI SE_Play_Fast(UC);
SI ADPCM_Play(UC);
SI Get_ZMD_Trak_Head(UC *, SS);

/* 関数 */
void Init_Music(void)
{
	UI	unZmusicVer;
	UI	unZmusicVerNum;
	UI	i;
	SS	ret;

	/* サウンド常駐確認 */
#ifdef	ZM_V2
	unZmusicVer = zm_ver();
	if(unZmusicVer == 0)		/* 0:常駐ナシ */
	{
		puts("Z-MUSICを常駐してください。");
		exit(0);
	}
#else
	#ifdef	ZM_V3
//	SC	sTONE[33] = {0};
	
	unZmusicVer = zm_check_zmsc();
	printf("zm_check_zmsc = %d\n", unZmusicVer);
	if((unZmusicVer & 0xFFFF) == 0xFFFF)	/* 常駐ナシ */
	{
		puts("Z-MUSICを常駐してください。");
		exit(0);
	}
	#else
		#error "No Music Lib"
	#endif
#endif
	
#ifdef	ZM_V2
	unZmusicVerNum = ZM_NUM_V2;	/* Ver2.0x判定 */
#else
	#ifdef	ZM_V3
	unZmusicVerNum = ZM_NUM_V3;	/* Ver3.0x判定 */
	#else
		#error "No Music Lib"
	#endif
#endif
	if((unZmusicVer&0xF000u)>>12 != unZmusicVerNum)	/* Ver判定 */
	{
		printf("Z-MUSIC Ver%dを常駐してください。\n", unZmusicVerNum);
		exit(0);
	}

#if 0
	if((unZmusicVer&0xFFFF000u)>>16 == 0u)	/* PCM8判定 */
	{
		puts("PCM8Aを常駐してください。");
		exit(0);
	}
#endif
	
	/* サウンド初期化 */
#ifdef	ZM_V2
	m_init();		/* 初期化 */
	m_ch("fm");		/* FM */
#else
	#ifdef	ZM_V3
	ret = zm_init(0);		/* 初期化 */
	printf("zm_init = %d\n", ret);
	#else
		#error "No Music Lib"
	#endif
#endif

	Music_Stop();	/* 音楽停止 */

#ifdef	ZM_V2
	Load_Music_List("data\\music\\m_list.txt", music_list, &m_list_max);
	printf("%s\n", music_list[i]);
#else
	#ifdef	ZM_V3
	/* BGM */
	Load_Music_List("data\\music\\m_list_V3.txt", music_list, &m_list_max);
	for(i = 0; i < m_list_max; i++)
	{
		music_dat_size[i] = File_Load(music_list[i], music_dat[i], sizeof(UC), 0);
		printf("Music File %2d = %s = size(%d[byte])\n", i, music_list[i], music_dat_size[i]);
	}
	/* 効果音(FM) */
	Load_SE_List("data\\se\\s_list_V3.txt", se_list, &s_list_max);
	for(i = 0; i < s_list_max; i++)
	{
		se_dat_size[i] = File_Load(se_list[i], se_dat[i], sizeof(UC), 0);
		se_dat_addr[i] = Get_ZMD_Trak_Head(se_dat[i], se_dat_size[i]);
		printf("Sound Effect File %2d = %s = size(%d[byte](Head[0x%x]))\n", i, se_list[i], se_dat_size[i], se_dat_addr[i]);
	}
	/* 効果音(ADPCM) */
	Load_SE_List("data\\se\\p_list_V3.txt", adpcm_list, &p_list_max);
	for(i = 0; i < p_list_max; i++)
	{
#if 1
	 	adpcm_dat_size[i] = (SI)File_Load(adpcm_list[i], adpcm_dat[i], sizeof(UC), 0);
		printf("ADPCM File %2d = %s = size(%d[byte])\n", i, adpcm_list[i], adpcm_dat_size[i]);
#else
		/* ADPCM登録 */
		static SC	*adpcm_addr[ADPCM_MAX];
		sprintf( sTONE, "%s", "test" );
		sprintf( adpcm_dat[i], "%s%c%s%c", sTONE, 0, adpcm_list[i], 0 );
		printf("adpcm_dat %2d = %s\n", i, adpcm_dat[i]);
		adpcm_addr[i] = zm_pcm_read( 0x8000+i, 0, -1, 0, adpcm_dat[i] );
		/* 引数
			int num:登録番号 (トーン番号の場合:0～32767)
							 (音色番号の場合:$8000+(0～32767)
			int flag:(AD)PCM加工コマンド(PPC)で(AD)PCMデータ加工するか(-1:YES,0:NO)
			int type:登録次元(-1:ADPCM,1=16BitPCM,0=V2互換)
			int orig:オリジナルキー:0-127
			char *param:登録データ情報およびPPC列格納バッファ(備考参照)
			
			戻り値	0以外:char *param処理後のアドレス
					0:エラー
		*/
		printf("ADPCM File %2d = %s = addr(0x%x)\n", i, adpcm_list[i], adpcm_addr[i]);
#endif
	}
	/* ZPD登録 */
//	ret = zm_register_zpd("data\\se\\OverKata_V3.ZPD");
//	printf("zm_register_zpd = %d\n", ret);
	#else
		#error "No Music Lib"
	#endif
#endif
	
#if 0
	exit(0);
#endif
}

void Exit_Music(void)
{
	Music_Stop();	/* 音楽停止 */
	
#ifdef	ZM_V2
	m_init();		/* 初期化 */
#else
	#ifdef	ZM_V3
	zm_init(0);		/* 初期化 */
	#else
		#error "No Music Lib"
	#endif
#endif
}

/* 音楽再生 */
SI Music_Play(UC bPlayNum)
{
	SI	ret=0;
	if(bPlayNum > m_list_max)return ret;

	Music_Stop();	/* 音楽停止 */
#ifdef	ZM_V2
	zmd_play(&music_list[bPlayNum][0]);	
#else
	#ifdef	ZM_V3
	ret = zm_play_zmd(music_dat_size[bPlayNum], &music_dat[bPlayNum][0]);
	#else
		#error "No Music Lib"
	#endif
#endif
	return	ret;
}

/* 音楽停止 */
SI Music_Stop(void)
{
	SI	ret=0;
#ifdef	ZM_V2
	m_stop(0,0,0,0,0,0,0,0,0,0);
#else
	#ifdef	ZM_V3
	zm_stop_all();
	#else
		#error "No Music Lib"
	#endif
#endif
	return	ret;
}

/* 効果音ZMDの演奏(FM音源で効果音再生する) */
SI SE_Play(UC bPlayNum)
{
	SI	ret=0;
#ifdef	ZM_V2
	#error "No Music Lib"
#else
	#ifdef	ZM_V3
	SI	HeadNum = 0;
	
	if(bPlayNum > s_list_max)return ret;
	
	HeadNum = se_dat_addr[bPlayNum];
	
	ret = zm_play_zmd_se(&se_dat[bPlayNum][8]);	/* 低速 */
	/* 引数
		char *zmd:ZMDの格納バッファ(ヘッダを含まず)
	*/
	#else
		#error "No Music Lib"
	#endif
#endif
	return	ret;
}

/* 効果音ZMDの演奏(FM音源で効果音再生する) */
SI SE_Play_Fast(UC bPlayNum)
{
	SI	ret=0;
#ifdef	ZM_V2
	/* 事前のデータ調整や初期化で音色の初期化が別途必要 */
	struct	_regs	stInReg = {0}, stOutReg = {0};
	UI	retReg;
//	UI	TrkFreeSize;
	UC	bCh, bTrk;
	SI	level;

	if(bPlayNum > m_list_max)return ret;

	bCh		= 6;
	bTrk	= 6;
	
	if(m_stat(bCh) != 0u)	/* bChが演奏中かどうか判定 */
	{
		Music_Stop();
	}

//	TrkFreeSize = m_free(bTrk);
//	Message_Num(&TrkFreeSize,	6,	10,	2, MONI_Type_UI, "%4d");
//	m_init();
//	m_stop(bTrk,0,0,0,0,0,0,0,0,0);
//	m_mute(6);
//	m_alloc(bTrk, 1000);
//	m_assign(bCh, bTrk);
//	m_vset(bTrk, v);

	m_play(bTrk,0,0,0,0,0,0,0,0,0);

	level = intlevel(7);	/* 割り込み禁止 */
	
	stInReg.d0 = 0xF0;				/* ZMUSIC.XによるIOCSコール */
	stInReg.d1 = 0x12;				/* se_play $12（ZMUSIC内のファンクションコール） */
	stInReg.d2 = bTrk;				/* 演奏トラックNo */
	stInReg.a1 = (UI)&SE_Data[0];	/* 演奏データ格納アドレス */
	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
	{
//			Message_Num(&retReg,		22,	3,	2, MONI_Type_PT, "0x%x");
//			Message_Num(&stOutReg.a1,	22,	4,	2, MONI_Type_SI, "0x%x");
//			Message_Num(&stOutReg.a2,	22,	5,	2, MONI_Type_SI, "0x%x");
//			Message_Num(&stOutReg.a3,	22,	6,	2, MONI_Type_SI, "0x%x");
//			Message_Num(&stOutReg.a4,	22,	7,	2, MONI_Type_SI, "0x%x");
//			Message_Num(&stOutReg.a5,	22,	8,	2, MONI_Type_SI, "0x%x");
//			Message_Num(&stOutReg.a6,	22,	9,	2, MONI_Type_SI, "0x%x");
	}
	intlevel(level);		/* 割り込み解除 */
#else
	#ifdef	ZM_V3
	SI	HeadNum = 0;

	if(bPlayNum > s_list_max)return ret;
	
	HeadNum = se_dat_addr[bPlayNum];
	
	ret = zm_se_play(&se_dat[bPlayNum][HeadNum]);	/* 高速 */
	/* 引数
		char *zmd:ZMDの格納バッファ(トラック・チャンネル情報テーブルから)
	*/
	#else
		#error "No Music Lib"
	#endif
#endif
	return	ret;
}

/* (AD)PCM効果音の演奏 */
SI ADPCM_Play(UC bPlayNum)
{
	SI	ret=0;
	
	if(bPlayNum > p_list_max)return ret;

	/* 色々試したけどIOCSライブラリの方を使う */
	if(_iocs_adpcmsns() != 0)	/* 何かしている */
	{
		_iocs_adpcmmod(0);	/* 停止 */
	}
	_iocs_adpcmout(adpcm_dat[bPlayNum], 0x403, adpcm_dat_size[bPlayNum]);	/* 再生 */
#if 0
#ifdef	ZM_V2
	SI	adpcm_sns;
	adpcm_sns = m_stat(9/*Mmin(Mmax(25, vx), 31)*/);	/* ADPCM ch1(9) ch2-8(25-31) */
//	Message_Num(&adpcm_sns,	 0, 13, 2, MONI_Type_SI, "%d");

	m_pcmplay(bPlayNum, 3, 4);
#else
	#ifdef	ZM_V3
//	SI	errnum;
//	SC	errtbl[64]={0};
//	UC	**p;

	if(bPlayNum > p_list_max)return ret;
	
	//ret = zm_se_adpcm1( -1, 0x40, 4, 3, adpcm_dat_size[bPlayNum], 0, 7, adpcm_dat[bPlayNum]);
	/* 引数
		char data_type:データタイプ(-1:ADPCM,1:16bitPCM,2:8bitPCM)
		char volume:音量(0-127,64:標準)
		char frq:再生周波数(0:3.9kHz 1:5.2kHz 2:7.8kHz 3:10.4kHz 4:15.6kHz)
		char pan:パンポット(0-3 / $80+0-127)
		int size:再生するADPCMのデータサイズ
		short priority:効果音優先度(0-255/0が最も低い)
		short ch:再生チャンネル(0-15)
		char *adpcm:(AD)PCMデータアドレス
	*/
	//ret = zm_se_adpcm2( -1, 0x40, 4, 3, bPlayNum, 0, 0);
	/*	引数
		char data_type:データタイプ(-1:ADPCM,1:16bitPCM,2:8bitPCM)
		char volume:音量(0-127,64:標準)
		char frq:再生周波数(0:3.9kHz 1:5.2kHz 2:7.8kHz 3:10.4kHz 4:15.6kHz)
		char pan:パンポット(0-3 / $80+0-127)
		int data_number:PCMデータ番号(d15が1ならば音色番号($8000+0-32767),
				      0ならばノート番号(0-32767)指定)
		short priority:効果音優先度(0-255/0が最も低い)
		short ch:再生チャンネル(0-15)
	*/
	if(ret != 0)
	{
	//	errnum = zm_store_error(ret, 10, &p);
		/* 引数
			int err (上位16):エラーの発生したファンクション番号(0-32767)
					(下位16):エラーコード(0-65535)
			int err=-1とした場合は問い合わせのみでストアは行わない
			int noferr:エラーの合計個数値に幾つ加算するか
			char **addr:エラーストックバッファの先頭アドレスを格納する変数

			戻り値	現在ストックしているエラーの個数
		*/
	//	zm_print_error(0, 1, errnum, 0, 0, 0, &errtbl[0], &p);
		
		/* 引数
			int mode:メッセージの出力先(0:画面,1:バッファ)
			int lang:メッセージの言語(0:英語,1:日本語)
			int noferr:エラーの数
			char *zmd:ZMDのファイル名(無い場合は0)
			char *srcaddr:ソース(ZMS)のアドレス(無い場合は0)
			char *srcname:ソース(ZMS)ファイル名(無い場合は0)
			char *errtbl:エラーストックバッファの先頭アドレス
			char **buff:出力バッファの先頭アドレスを格納する変数
		    (int mode=1の時のみ有効,バッファの終端には0)
		*/
	}
//	Message_Num(&ret,	 11, 12, 2, MONI_Type_SI, "0x%x");

	#else
		#error "No Music Lib"
	#endif
#endif
#endif	
	return	ret;
}

/* 演奏トラックまでのオフセットを取得 */
SI	Get_ZMD_Trak_Head(UC *dat, SS size)
{
	UI	ret = 0;
	
#ifdef	ZM_V3
	UC	*p;
	p = dat + (4*2) + (4*1);
	ret = (*(p+0) << 12) + (*(p+1) << 8) + (*(p+2) << 4) + *(p+3);
#endif	
	
	return	ret;
}

#endif	/* MUSIC_C */
