#ifndef	MUSIC_C
#define	MUSIC_C

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <sys/iocs.h>
#include <interrupt.h>

#include "inc/usr_macro.h"
#include "OverKata.h"
#include "MUSIC.h"
#include "FileManager.h"
#include "Output_Text.h"

#define FM_CH_MAX	(8)
#define FM_USE_CH	(1)
#define MML_BUF		(32)
#define MML_BUF_N	(2048)

#if		ZM_V2 == 1
#elif	ZM_V3 == 1
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

#if		ZM_V2 == 1
	#include "inc/ZMUSIC.H"
#elif	ZM_V3 == 1
	#include "inc/ZMSC3LIB.H"
#elif	MC_DRV == 1
	#include "inc/mcclib.h"
#else
	#error "No Music Lib"
#endif

#define ZM_NUM_V2	(0x02)
#define ZM_NUM_V3	(0x03)
#define MC_DRV_NUM	(0x00600000)

/* グローバル変数 */
int8_t		music_list[MUSIC_MAX][256]	=	{0};
uint32_t	m_list_max	=	0u;

#if		ZM_V2 == 1
#elif	ZM_V3 == 1
	int8_t	*music_dat[MUSIC_MAX];
	static int16_t	music_dat_size[MUSIC_MAX]	=	{0};
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

#if		ZM_V2 == 1
#elif	ZM_V3 == 1
	int8_t		se_list[SOUND_MAX][256]	=	{0};
	uint32_t	s_list_max	=	0u;
	int8_t		*se_dat[SOUND_MAX];
	static int16_t	se_dat_size[SOUND_MAX]	=	{0};
	static int32_t	se_dat_addr[SOUND_MAX]	=	{0};
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

int8_t		adpcm_list[ADPCM_MAX][256]	=	{0};
uint32_t	p_list_max	=	0u;
int8_t		*adpcm_dat[ADPCM_MAX];
static int32_t	adpcm_dat_size[ADPCM_MAX]	=	{0};

uint8_t	SE_Data[] = {	/* 構造体にした方がよい？ */
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
int32_t Music_Play(uint8_t);
int32_t Music_Stop(void);
int32_t SE_Play(uint8_t);
int32_t SE_Play_Fast(uint8_t);
int32_t ADPCM_Play(uint8_t);
int32_t ADPCM_Stop(void);
int32_t Get_ZMD_Trak_Head(uint8_t *, int16_t);
int32_t M_SetMusic(uint32_t);
int32_t M_Play(int16_t);

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Init_Music(void)
{
	uint32_t	i;
#if		(ZM_V2 == 1) || (ZM_V3 == 1)
	uint32_t	unZmusicVer;
	uint32_t	unZmusicVerNum;
#endif

	/* サウンド常駐確認 */
#if		ZM_V2 == 1
	unZmusicVer = zm_ver();
	if(unZmusicVer == 0)		/* 0:常駐ナシ */
	{
		puts("Z-MUSICを常駐してください。");
		exit(0);
	}
	unZmusicVerNum = ZM_NUM_V2;	/* Ver2.0x判定 */
#elif	ZM_V3 == 1
	int16_t	ret;
//	int8_t	sTONE[33] = {0};
	
	unZmusicVer = zm_check_zmsc();
	printf("zm_check_zmsc = %d\n", unZmusicVer);
	if((unZmusicVer & 0xFFFF) == 0xFFFF)	/* 常駐ナシ */
	{
		puts("Z-MUSICを常駐してください。");
		exit(0);
	}
	unZmusicVerNum = ZM_NUM_V3;	/* Ver3.0x判定 */
#elif	MC_DRV == 1
	if( MC_INIT() < MC_DRV_NUM )
	{
		puts( "mcdrv が常駐してないかバージョンが古い！" );
		exit(0);
	}
#else
	#error "No Music Lib"
#endif
	
#if		(ZM_V2 == 1) || (ZM_V3 == 1)
	if((unZmusicVer&0xF000u)>>12 != unZmusicVerNum)	/* Ver判定 */
	{
		printf("Z-MUSIC Ver%dを常駐してください。\n", unZmusicVerNum);
		exit(0);
	}
#elif	MC_DRV == 1
	/* 何もしない */
#else
	#error "No Music Lib"
#endif


#if 0
	if((unZmusicVer&0xFFFF000u)>>16 == 0u)	/* PCM8判定 */
	{
		puts("PCM8Aを常駐してください。");
		exit(0);
	}
#endif
	
	/* サウンド初期化 */
#if		ZM_V2 == 1
	m_init();		/* 初期化 */
	m_ch("fm");		/* FM */
	for(i = 0; i < 8; i++)
	{
		int32_t	err = 0;
		uint16_t uCh, uTrk;

		uCh = i + 1;
		uTrk = i + 1;
		
		/* トラックバッファ設定 */
		err = m_alloc( uTrk, MML_BUF_N );	/* trk(1-80) */
		if(err != 0)
		{
			printf("m_alloc error %d-(%d,%d)\n", err, uTrk, MML_BUF_N);
		}
		/* チャンネルとトラックの割り付け */
		err = m_assign( uCh, uTrk );	/* ch(FM:1-8 ADPCM:9 MIDI:10-25 PCM8:26-32) trk(1-80) */
		if(err != 0)
		{
			printf("m_assign error %d\n", err);
		}
	}
#elif	ZM_V3 == 1
	ret = zm_init(0);		/* 初期化 */
	printf("zm_init = %d\n", ret);
#elif	MC_DRV == 1
	/* 何もしない */
#else
	#error "No Music Lib"
#endif

	Music_Stop();	/* 音楽停止 */

	/* BGM */
#if		ZM_V2 == 1
	/* 何もしない */
#elif	ZM_V3 == 1
	for(i = 0; i < m_list_max; i++)
	{
		int32_t	FileSize;
		
		GetFileLength(music_list[i], &FileSize);
		music_dat[i] = (int8_t*)MyMalloc(FileSize);
		memset(music_dat[i], 0, FileSize);
		/* メモリに登録 */
		music_dat_size[i] = File_Load(music_list[i], music_dat[i], sizeof(int8_t), 0);
		printf("Music File %2d = %s = size(%d[byte])\n", i, music_list[i], music_dat_size[i]);
	}
#elif	MC_DRV == 1
	/* 何もしない */
#else
	#error "No Music Lib"
#endif
	
	/* 効果音(FM) */
#if		ZM_V2 == 1
	/* 何もしない */
#elif	ZM_V3 == 1
	for(i = 0; i < s_list_max; i++)
	{
		int32_t	FileSize;
		
		GetFileLength(se_list[i], &FileSize);
		se_dat[i] = (int8_t*)MyMalloc(FileSize);
		memset(se_dat[i], 0, FileSize);
		/* メモリに登録 */
		se_dat_size[i] = File_Load(se_list[i], se_dat[i], sizeof(int8_t), 0);
		se_dat_addr[i] = Get_ZMD_Trak_Head(se_dat[i], se_dat_size[i]);
		printf("Sound Effect File %2d = %s = size(%d[byte](Head[0x%x]))\n", i, se_list[i], se_dat_size[i], se_dat_addr[i]);
	}
#elif	MC_DRV == 1
	/* 何もしない */
#else
	#error "No Music Lib"
#endif

#if		ZM_V2 == 1
	/* ZPD登録 */
#elif	ZM_V3 == 1
	/* ZPD登録 */
//	ret = zm_register_zpd("data\\se\\OverKata_V3.ZPD");
//	printf("zm_register_zpd = %d\n", ret);
#elif	MC_DRV == 1
	/* PDX登録 */
#else
	#error "No Music Lib"
#endif

	/* 効果音(ADPCM) */
	for(i = 0; i < p_list_max; i++)
	{
#if 1
		int32_t	FileSize;
		
		GetFileLength(adpcm_list[i], &FileSize);
		adpcm_dat[i] = (int8_t*)MyMalloc(FileSize);
		memset(adpcm_dat[i], 0, FileSize);
		/* メモリに登録 */
	 	adpcm_dat_size[i] = (int32_t)File_Load(adpcm_list[i], adpcm_dat[i], sizeof(int8_t), FileSize);
//		printf("ADPCM File %2d = %s = size(%d[byte]=%d)\n", i, adpcm_list[i], adpcm_dat_size[i], FileSize);
#else
	#if		ZM_V3 == 1
		/* ADPCM登録 */
		static int8_t	*adpcm_addr[ADPCM_MAX];
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
#endif
	}
	
#if 0
	exit(0);
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void Exit_Music(void)
{
	Music_Stop();	/* 音楽停止 */
	
#if		ZM_V2 == 1
	m_init();		/* 初期化 */
#elif	ZM_V3 == 1
	zm_init(0);		/* 初期化 */
#elif	MC_DRV == 1
	MC_QUIT();		/* 終了処理 */
#else
	#error "No Music Lib"
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* 音楽再生 */
int32_t Music_Play(uint8_t bPlayNum)
{
	int32_t	ret=0;
	if(bPlayNum > m_list_max)return ret;

	Music_Stop();	/* 音楽停止 */
#if		ZM_V2 == 1
	zmd_play(&music_list[bPlayNum][0]);	
#elif	ZM_V3 == 1
	ret = zm_play_zmd(music_dat_size[bPlayNum], &music_dat[bPlayNum][0]);
#elif	MC_DRV == 1
	MC_MMCP_PLAY(&music_list[bPlayNum][0]);
#else
	#error "No Music Lib"
#endif

	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* 音楽停止 */
int32_t Music_Stop(void)
{
	int32_t	ret=0;
	
#if		ZM_V2 == 1
	m_stop(0,0,0,0,0,0,0,0,0,0);
#elif	ZM_V3 == 1
	zm_stop_all();
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* 効果音ZMDの演奏(FM音源で効果音再生する) */
int32_t SE_Play(uint8_t bPlayNum)
{
	int32_t	ret=0;

#if		ZM_V2 == 1
#elif	ZM_V3 == 1
	int32_t	HeadNum = 0;
	
	if(bPlayNum > s_list_max)return ret;
	
	HeadNum = se_dat_addr[bPlayNum];
	
	ret = zm_play_zmd_se(&se_dat[bPlayNum][8]);	/* 低速 */
	/* 引数
		char *zmd:ZMDの格納バッファ(ヘッダを含まず)
	*/
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* 効果音ZMDの演奏(FM音源で効果音再生する) */
int32_t SE_Play_Fast(uint8_t bPlayNum)
{
	int32_t	ret=0;
	
#if		ZM_V2 == 1
	/* 事前のデータ調整や初期化で音色の初期化が別途必要 */
	struct	_regs	stInReg = {0}, stOutReg = {0};
	uint32_t	retReg;
//	uint32_t	TrkFreeSize;
	uint8_t	bCh, bTrk;
	int32_t	level;

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
	stInReg.a1 = (uint32_t)&SE_Data[0];	/* 演奏データ格納アドレス */
	
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
#elif	ZM_V3 == 1
	int32_t	HeadNum = 0;

	if(bPlayNum > s_list_max)return ret;
	
	HeadNum = se_dat_addr[bPlayNum];
	
	ret = zm_se_play(&se_dat[bPlayNum][HeadNum]);	/* 高速 */
	/* 引数
		char *zmd:ZMDの格納バッファ(トラック・チャンネル情報テーブルから)
	*/
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* (AD)PCM効果音の演奏 */
int32_t ADPCM_Play(uint8_t bPlayNum)
{
	int32_t	ret=0;
	
	if(bPlayNum > p_list_max)return ret;

	/* 色々試したけどIOCSライブラリの方を使う */
	_iocs_adpcmout(adpcm_dat[bPlayNum], Mmul256(4) + 3, adpcm_dat_size[bPlayNum]);	/* 再生 */
#if 0
	
#ifdef	ZM_V2
	int32_t	adpcm_sns;
	adpcm_sns = m_stat(9/*Mmin(Mmax(25, vx), 31)*/);	/* ADPCM ch1(9) ch2-8(25-31) */
//	Message_Num(&adpcm_sns,	 0, 13, 2, MONI_Type_SI, "%d");

	m_pcmplay(bPlayNum, 3, 4);
#elif	ZM_V3 == 1
//	int32_t	errnum;
//	int8_t	errtbl[64]={0};
//	uint8_t	**p;

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

#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif
	
#endif	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
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

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* 演奏トラックまでのオフセットを取得 */
int32_t	Get_ZMD_Trak_Head(uint8_t *dat, int16_t size)
{
	int32_t	ret = 0;
	
#if		ZM_V2 == 1
#elif	ZM_V3 == 1
	uint8_t	*p;
	p = dat + (4*2) + (4*1);
	ret = (*(p+0) << 12) + (*(p+1) << 8) + (*(p+2) << 4) + *(p+3);
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif
	
	return	ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int32_t	M_SetMusic(uint32_t uNum)
{
	int32_t	ret = 0;
#if		ZM_V2 == 1
	uint32_t	i = 0u;
	int32_t	err = 0;
	
	/* トラックの割り付け変更 */
	for(i = 0; i < FM_USE_CH; i++)
	{
		int32_t	ch, trk;
		
		ch = (FM_CH_MAX - FM_USE_CH + 1) + i;
		trk = 60 + ch;
		
		err = m_alloc( trk, MML_BUF );
		if(err != 0)
		{
			printf("m_alloc error %d-(%d,%d)\n", err, trk, MML_BUF);
		}
		
		err = m_assign( ch, trk );	/* ch(FM:1-8 ADPCM:9 MIDI:10-25 PCM8:26-32) trk(1-80) */
		if(err != 0)
		{
			printf("m_assign error %d\n", err);
		}
	}

#endif	
	return ret;
}
	
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* エンジン音 */
int32_t	M_Play(int16_t Key)
{
	int32_t	ret = 0;
	
#if		ZM_V2 == 1
	
	uint8_t	uMML[MML_BUF];
	int32_t	err = 0;
	int32_t	uCh = 0;
	int32_t	uTrk = 0;
	static uint8_t ubChanel = 0u;
	static uint8_t ubCount = 0u;

#ifdef DEBUG	/* デバッグコーナー */
	uint8_t	bDebugMode;
	uint16_t	uDebugNum;
	GetDebugMode(&bDebugMode);
	GetDebugNum(&uDebugNum);
#endif
	
	uCh = (FM_CH_MAX - FM_USE_CH + 1u) + ubChanel;
	uTrk = 60 + uCh;
	if(ubChanel < FM_USE_CH - 1u)
	{
		ubChanel++;
	}
	else
	{
		ubChanel = 0u;
	}

	err = m_alloc( uTrk, MML_BUF );	/* trk(1-80) */
	if(err != 0)
	{
		printf("m_alloc error %d-(%d,%d)\n", err, uTrk, MML_BUF);
	}
#ifdef DEBUG	/* デバッグコーナー */
	if(bDebugMode == TRUE)
	{
		if(Key > 0x80)
		{
			sprintf(uMML, "@%dv15o2l4q1@k%d d+&", uDebugNum, Mdiv8(Key) );	/* OK */
		}
		else
		{
			sprintf(uMML, "@137v15o2l4q1@k%d d+&", Mdiv8(Key) );	/* OK */
		}
	}
	else
#endif
	{
		sprintf(uMML, "@137v15o2l4q1@k%d d+&", Mdiv8(Key) );	/* OK */
	}
	err = m_trk( uTrk, uMML );	
	if(err != 0)
	{
		printf("m_trk error %d-(%d,%s,%d)\n", err, uTrk, uMML, ubCount);
	}
	/* 注意：X-BASICのm_playの引数は、チャンネル。Z-MUSICは、トラック番号 */
	/* 10個の引数はチャンネルを示すので再生したいトラックNoを設定する */
	err = m_play(uTrk,'NASI','NASI','NASI','NASI','NASI','NASI','NASI','NASI','NASI');	/* 1 */
	if(err != 0)
	{
		printf("m_play error %d %d %d\n", err, uCh, uTrk);
	}
	
	ubCount++;
	
#endif	
	return ret;
}
#endif	/* MUSIC_C */
