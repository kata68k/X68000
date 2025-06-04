#ifndef	IF_MUSIC_C
#define	IF_MUSIC_C

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <sys/iocs.h>
#include <interrupt.h>
#include <PCM8Afnc.H>

#include <usr_macro.h>
#include "BIOS_MFP.h"
#include "BIOS_MPU.h"
#include "IF_Memory.h"
#include "IF_MUSIC.h"
#include "IF_FileManager.h"

#define FM_USE_CH	(8)
#define MML_BUF		(128)
#define MML_BUF_N	(2048)

#if		ZM_V2 == 1
#elif	ZM_V3 == 1
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

#if		ZM_V2 == 1
	#include <ZMUSIC.H>
#elif	ZM_V3 == 1
	#include <ZMSC3LIB.H>
#elif	MC_DRV == 1
	#include <mcclib.h>
#else
	#error "No Music Lib"
#endif

#define ZM_NUM_V2	(0x02)
#define ZM_NUM_V3	(0x03)
#define MC_DRV_NUM	(0x00600000)

/* グローバル変数 */
int8_t		music_list[MUSIC_MAX][256]	=	{0};
uint32_t	m_list_max	=	0u;

#if		((ZM_V2 == 1) || (ZM_V3 == 1))
	int8_t	*music_dat[MUSIC_MAX];
	static size_t	music_dat_size[MUSIC_MAX]	=	{0};
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

#if		ZM_V2 == 1
#elif	ZM_V3 == 1
	int8_t		se_list[SOUND_MAX][256]	=	{0};
	uint32_t	s_list_max	=	0u;
	int8_t		*se_dat[SOUND_MAX];
	static size_t	se_dat_size[SOUND_MAX]	=	{0};
	static int32_t	se_dat_addr[SOUND_MAX]	=	{0};
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif

int8_t		adpcm_list[ADPCM_MAX][256]	=	{0};
uint32_t	p_list_max	=	0u;
int8_t		*adpcm_dat[ADPCM_MAX];
static size_t	adpcm_dat_size[ADPCM_MAX]	=	{0};

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
int32_t FM_SE_Play(uint8_t);
int32_t FM_SE_Play_Fast(uint8_t);
int32_t SE_Play(uint8_t);
int32_t SE_Stop(void);
int32_t ADPCM_Play(uint8_t);
int32_t ADPCM_Stop(void);
int32_t ADPCM_SNS(void);
int32_t Get_ZMD_Trak_Head(uint8_t *, int16_t);
int32_t M_SetMusic(uint32_t);
int32_t M_Play(int16_t, int16_t);
int32_t	M_TEMPO(int16_t);

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
	int8_t Ch, Trk;
	int32_t	nSysStat;

	puts("Music Init...");
	/* 機種判別 */
	nSysStat = mpu_stat_chk() & 0x0F;	/* MPUの種類 */
	if(nSysStat != 0)	/* 68000以外のMPU */
	{
		/* キャッシュクリア */
		mpu_cache_clr();
	}
	else
	{
		/* 68000 */
	}

	if(nSysStat == 0)	/* 68000 */
	{
		/* サウンド常駐確認 */
		puts("Music Ver chk...");
#if		ZM_V2 == 1
		unZmusicVer = zm_ver();
		printf("zm_ver = 0x%x\n", unZmusicVer);
		if(unZmusicVer == 0)		/* 0:常駐ナシ */
		{
			puts("Z-MUSICを常駐してください。");
			unZmusicVer = ZM_NUM_V2 << 12;
			exit(0);
		}
		unZmusicVerNum = ZM_NUM_V2;	/* Ver2.0x判定 */

		if((unZmusicVer&0xFFFF000u)>>16 == 0u)	/* PCM8判定 */
		{
			puts("PCM8Aを常駐してないです");
		}
		else
		{
			puts("PCM8Aを常駐確認");
			pcm8a_vsyncint_on();	/* PCM8A との衝突を回避 */
		}
#elif	ZM_V3 == 1
		int32_t	ret;
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
		
		puts("Music Ver num...");
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
	}	

	/* サウンド初期化 */
#if		ZM_V2 == 1
	m_init();		/* 初期化 */
	m_ch("fm");		/* FM */
#if 1
	for(i = 0; i < 8; i++)	/* ch8 をのぞく */
	{
		int32_t	err = 0;

		Ch = i + 1;
		Trk = 60 + Ch;
		
		/* トラックバッファ設定 */
		err = m_alloc( Trk, MML_BUF_N );	/* trk(1-80) */
		if(err != 0)
		{
			printf("m_alloc error %d-(%hd,%d)\n", err, Trk, MML_BUF_N);
		}
		/* チャンネルとトラックの割り付け */
		err = m_assign( Ch, Trk );	/* ch(FM:1-8 ADPCM:9 MIDI:10-25 PCM8:26-32) trk(1-80) */
		if(err != 0)
		{
			printf("m_assign error %d\n", err);
		}
		err = m_free(Trk);
		if(err < 0)
		{
			printf("m_free size %d\n", err);
		}
	}
#endif

#elif	ZM_V3 == 1
	ret = zm_init(0);		/* 初期化 */
	printf("zm_init = %d\n", ret);
#elif	MC_DRV == 1
	/* 何もしない */
#else
	//#error "No Music Lib"
#endif

	Music_Stop();	/* 音楽停止 */
	
	/* BGM */
#if		((ZM_V2 == 1) || (ZM_V3 == 1))
	/* 何もしない */
	for(i = 0; i < m_list_max; i++)
	{
		int32_t	FileSize;
		
		if(GetFileLength(music_list[i], &FileSize) == -1)
		{

		}
		else
		{
			if(FileSize != 0)
			{
				music_dat[i] = (int8_t*)MyMalloc(FileSize);
				memset(music_dat[i], 0, FileSize);
				/* メモリに登録 */
				music_dat_size[i] = File_Load(music_list[i], music_dat[i], sizeof(int8_t), 0);
				printf("Music File %2d = %s = size(%d[byte])\n", i, music_list[i], music_dat_size[i]);
			}
		}
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
		
		if(GetFileLength(se_list[i], &FileSize) == -1)
		{

		}
		else
		{
			if(FileSize != 0)
			{
				se_dat[i] = (int8_t*)MyMalloc(FileSize);
				memset(se_dat[i], 0, FileSize);
				/* メモリに登録 */
				se_dat_size[i] = File_Load(se_list[i], se_dat[i], sizeof(int8_t), 0);
				se_dat_addr[i] = Get_ZMD_Trak_Head(se_dat[i], se_dat_size[i]);
				printf("Sound Effect File %2d = %s = size(%d[byte](Head[0x%x]))\n", i, se_list[i], se_dat_size[i], se_dat_addr[i]);
			}
		}
	}
#elif	MC_DRV == 1
	/* 何もしない */
#else
	#error "No Music Lib"
#endif

	/* 効果音(ADPCM) */
#if		ZM_V2 == 1
	/* CNF登録 */
//	m_pcmcnf("BK.CNF");
	/* ZPD登録 */
	/* バッチファイルにて常駐時に登録 */
#elif	ZM_V3 == 1
	/* ZPD登録 */
//	ret = zm_register_zpd("data\\se\\OverKata_V3.ZPD");
//	printf("zm_register_zpd = %d\n", ret);
#elif	MC_DRV == 1
	/* PDX登録 */
#else
	/* No Driver */
#endif

	puts("Music ADPCM...");
	for(i = 0; i < p_list_max; i++)
	{
#if	1
		int32_t	FileSize;
		
		if(GetFileLength(adpcm_list[i], &FileSize) == -1)
		{

		}
		else
		{
			if(FileSize != 0)
			{
				adpcm_dat[i] = (int8_t*)MyMalloc(FileSize);
				memset(adpcm_dat[i], 0, FileSize);
				/* メモリに登録 */
				adpcm_dat_size[i] = (int32_t)File_Load(adpcm_list[i], adpcm_dat[i], sizeof(int8_t), FileSize);
				printf("ADPCM File %2d = %s = size(%d[byte]=%d)\n", i, adpcm_list[i], adpcm_dat_size[i], FileSize);
			}
		}
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
	uint32_t	unZmusicVer;

	Music_Stop();	/* 音楽停止 */

#if		ZM_V2 == 1
	unZmusicVer = zm_ver();
	if((unZmusicVer&0xFFFF000u)>>16 == 0u)	/* PCM8判定 */
	{
		puts("PCM8Aを常駐してないです");
	}
	else
	{
		puts("PCM8Aを常駐確認");
		pcm8a_vsyncint_off();	/* PCM8A との衝突を回避 */
	}
	
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
	int8_t	sZPD_File_Name[256];
	int8_t  *dot;
#if 0	
	struct	_regs	stInReg = {0}, stOutReg = {0};
	uint32_t	retReg;
#endif

	if(bPlayNum > m_list_max)return -1;

	Music_Stop();	/* 音楽停止 */

#if		ZM_V2 == 1
#if 0	
	stInReg.d0 = 0xF0;						/* ZMUSIC.XによるIOCSコール */
	stInReg.d1 = 0x11;						/* play_cnv_data $11（ZMUSIC内のファンクションコール） */
	stInReg.d2 = music_dat_size[bPlayNum];	/* d2.l＝データ総サイズ */
//	stInReg.d2 = 0;							/* d2.l＝0の場合はドライバ内にデータを転送せず即演奏(高速応答) */
	stInReg.a1 = (int32_t)&music_list[bPlayNum][7];	/* a1.l＝演奏データ格納アドレス(備考参照) */
//	stInReg.a1 = (int32_t)&SE_Data[0];		/* a1.l＝演奏データ格納アドレス(備考参照) */
/* 備考：  ＺＭＤの構造
	Offset  ＋0    :$10(.b)        ←偶数アドレス}
	＋1～＋6:'ZmuSiC'                           }メモリ上に無くても構わない
	＋7    :Version Number(.b)     ←a1.lで指し示すべきアドレス(奇数アドレス)
	なお、内部フォーマットについての詳しい解説はMEASURE12参照 */

	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */
#else
//	printf("Music File %2d = %s\n", bPlayNum, music_list[bPlayNum]);
	memset(sZPD_File_Name, 0, sizeof(sZPD_File_Name));

	/* リストから対象ファイルを指定して中身をサーチする */
	if(Get_ZPDFileName(&music_list[bPlayNum][0], &sZPD_File_Name[0]) == 0)
	{
		int8_t	sFilePath[256];
		memset(sFilePath, 0, sizeof(sFilePath));
		strcpy(&sFilePath[0], &music_list[bPlayNum][0]);
		dot = strrchr(sFilePath, '\\');	/* 拡張子の\のアドレスを出す */

		if(dot != NULL)
		{
			*dot = '\0';	/* .以降を終端で置き換え */
			strcat(sFilePath, "\\");
			strcat(sFilePath, sZPD_File_Name);
			
			if(Get_FileAlive(sFilePath) == 0)
			{
//				printf("file = %s\n", &sFilePath[0]);
				ret = (int32_t)m_adpcm_block(sFilePath);
//				ret = (int32_t)m_adpcm_block(sZPD_File_Name);	// ERROR 62
				if(ret != 0)
				{
					printf("Music ZPDFile %2d = %s(%d)\n", bPlayNum, sFilePath, ret);
					printf("Music File %2d = %s\n", bPlayNum, music_list[bPlayNum]);
					printf("zmd_play error %d\n", ret);
				}
			}
			else	/* ZPDがない場合 */
			{
//				printf("Music File %2d = %s\n", bPlayNum, music_list[bPlayNum]);
//				printf("zmd_play error ZPD lost %d\n", ret);
			}
		}
	}
	else	/* ZMDと同じ名前のZPDファイルを探す */
	{
		strcpy(&sZPD_File_Name[0], &music_list[bPlayNum][0]);
		
		dot = strrchr(sZPD_File_Name, '.');	/* 拡張子の.のアドレスを出す */

		if(dot != NULL)
		{
			*dot = '\0';	/* .以降を終端で置き換え */
			strcat(sZPD_File_Name, ".");
			strcat(sZPD_File_Name, "ZPD");	/* ZPDに変更 */
			if(Get_FileAlive(sZPD_File_Name) == 0)	/* ファイル名と同じZPDファイルがある */
			{
				ret = (int32_t)m_adpcm_block(sZPD_File_Name);
				if(ret != 0)
				{
					printf("Music ZPDFile %2d = %s(%d)\n", bPlayNum, sZPD_File_Name, ret);
					printf("Music File %2d = %s\n", bPlayNum, music_list[bPlayNum]);
					printf("zmd_play error %d\n", ret);
				}
			}
			else	/* ZPDがない場合 */
			{
//				printf("Music File %2d = %s\n", bPlayNum, music_list[bPlayNum]);
//				printf("zmd_play error ZPD lost %d\n", ret);
			}
		}
	}

	ret = zmd_play(&music_list[bPlayNum][0]);
	if(ret != 0)
	{
		printf("Music File %2d = %s\n", bPlayNum, music_list[bPlayNum]);
		printf("zmd_play error %d\n", ret);
	}

	if(Get_CPU_Time() < 400 )
	{
		m_mute(26,27,28,29,30,31,32,9,'NASI','NASI');	/* ADPCMパートをミュート */
//		printf("ADPCM stop %d\n", ret);
	}
#endif
	

#elif	ZM_V3 == 1
	ret = zm_play_zmd(music_dat_size[bPlayNum], &music_dat[bPlayNum][0]);
	printf("Music File %2d = %s = size(%d[byte])\n", bPlayNum, music_list[bPlayNum], music_dat_size[bPlayNum]);
#elif	MC_DRV == 1
	MC_MMCP_PLAY(&music_list[bPlayNum][0]);
#else
	#error "No Music Lib"
#endif

	_iocs_b_curoff();			/* カーソルを消します */

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
	ret = m_stop(0,0,0,0,0,0,0,0,0,0);
	if(ret != 0)
	{
		printf("m_stop %d\n", ret);
	}
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
int32_t FM_SE_Play(uint8_t bPlayNum)
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
int32_t FM_SE_Play_Fast(uint8_t bPlayNum)
{
	int32_t	ret=0;
	
#if		ZM_V2 == 1
	/* 事前のデータ調整や初期化で音色の初期化が別途必要 */
	struct	_regs	stInReg = {0}, stOutReg = {0};
	uint32_t	retReg;
//	uint32_t	TrkFreeSize;
	uint8_t	bCh, bTrk;
	int32_t	level;

	if(bPlayNum > m_list_max)return -1;

	bCh = FM_USE_CH;
	bTrk = 60 + bCh;
	
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
/* ドライバ経由(AD)PCM効果音の演奏 */
int32_t SE_Play(uint8_t bPlayNum)
{
	int32_t	ret=0;
	struct	_regs	stInReg = {0}, stOutReg = {0};
	uint32_t	retReg;
	
#if	ZM_V2 == 1
	stInReg.d0 = 0xF0;				/* ZMUSIC.XによるIOCSコール */
	stInReg.d1 = 0x14;				/* se_adpcm2 $14（ZMUSIC内のファンクションコール） */
	stInReg.d2 = bPlayNum;			/* ノート番号 */
	stInReg.d3 = 0xFF0403;			/* PAN,FRQ,LV */
//	引数:	d2.l         ＝ノート番号(0～511)
//			d3(bit00～07)＝PAN(0－3)
//			d3(bit08～15)＝FRQ(0－4)
//			d3(bit16～23)＝優先レベル(低0～255高)	
	retReg = _iocs_trap15(&stInReg, &stOutReg);	/* Trap 15 */

#elif	ZM_V3 == 1
//	int32_t	errnum;
//	int8_t	errtbl[64]={0};
//	uint8_t	**p;

	if(bPlayNum >= p_list_max)return ret;
	
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
	if(bPlayNum >= p_list_max)return ret;

	/* 色々試したけどIOCSライブラリの方を使う */
	_iocs_adpcmout(adpcm_dat[bPlayNum], Mmul256(4) + 3, adpcm_dat_size[bPlayNum]);	/* 再生 */
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
/* ドライバ経由(AD)PCM効果音の停止 */
int32_t SE_Stop(void)
{
	int32_t	ret=0;
	
#if		ZM_V2 == 1
	ADPCM_Stop();
#elif	ZM_V3 == 1
#elif	MC_DRV == 1
#else
	ADPCM_Stop();
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
	
	if(bPlayNum >= p_list_max)return ret;

	/* 色々試したけどIOCSライブラリの方を使う */
//	printf("ADPCM_Play st(%d)\n", bPlayNum);

//	printf("ADPCM_Play md(0x%p,%d,%d), \n", adpcm_dat[bPlayNum], Mmul256(4) + 3, adpcm_dat_size[bPlayNum]);
	_iocs_adpcmout(adpcm_dat[bPlayNum], Mmul256(4) + 3, adpcm_dat_size[bPlayNum]);	/* 再生 */

//	printf("ADPCM_Play st(%d)\n", bPlayNum);

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
/* (AD)PCM効果音の状態 */
int32_t ADPCM_SNS(void)
{
	int32_t	ret=0;
	
	if(_iocs_adpcmsns() != 0)	/* 何かしている */
	{
		ret = 1;
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
	int32_t	err = 0;
	
	/* トラックの割り付け変更 */
	int32_t	ch, trk;
		
	ch = FM_USE_CH;
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
int32_t	M_Play(int16_t Num, int16_t Key)
{
	int32_t	ret = 0;
	
#if		ZM_V2 == 1
	
	uint8_t	uMML[MML_BUF];
	int32_t	err = 0;
	int32_t	Ch = 0;
	int32_t	Trk = 0;
	static uint8_t ubCount = 0u;

	Ch = FM_USE_CH;
	Trk = 60 + Ch;

	err = m_alloc( Trk, MML_BUF );	/* trk(1-80) */
	if(err != 0)
	{
		printf("m_alloc error %d-(%d,%d)\n", err, Trk, MML_BUF);
	}

	switch(Num)
	{
		case 0:
		{
			sprintf(uMML, "@137v15o2l4q1@k%hd d+&", Mdiv8(Key) );	/* OK */
			break;
		}
		case 1:
		{
			strcpy(uMML, "@137v15o5(g>g2)");	/* OK */
			break;
		}
		case 2:
		{
			strcpy(uMML, "@137v15o4(g2<g)");	/* OK */
			break;
		}
		case 3:
		{
			sprintf(uMML, "@137v15o1k %d d+1", Key % 80);	/* OK */
			break;
		}
		case 4:
		{
			sprintf(uMML, "@137v15(o3d*96,e),48", Key % 80);	/* OK */
			break;
		}
		case 5:
		{
			//strcpy(uMML, "t100@200v15o0k0d+1");	/* OK */
			strcpy(uMML, "@137v15o4(g2<g)");	/* OK */
			break;
		}
		default:
		{
			sprintf(uMML, "@29v15o5l4q1@k%hd C+&", Key );	/* OK */
			break;
		}
	}
	err = m_trk( Trk, uMML );	
	if(err != 0)
	{
//		printf("m_trk error %d-(%d,%s,%d)\n", err, Trk, uMML, ubCount);
	}
	/* 注意：X-BASICのm_playの引数は、チャンネル。Z-MUSICは、トラック番号 */
	/* 10個の引数はチャンネルを示すので再生したいトラックNoを設定する */
	err = m_play(Trk,'NASI','NASI','NASI','NASI','NASI','NASI','NASI','NASI','NASI');	/* 1 */
	if(err != 0)
	{
		printf("m_play error %d %d %d\n", err, Ch, Trk);
	}
	
	ubCount++;
	
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
int32_t	M_TEMPO(int16_t Num)
{
	int32_t	ret = 0;

#if		ZM_V2 == 1
	ret = m_tempo(Num);
#endif	
	return ret;
}

#endif	/* IF_MUSIC_C */
