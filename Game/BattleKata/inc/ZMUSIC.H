/*

	ＺＭＵＳＩＣ．Ｘ用ライブラリヘッダファイル

	   ZMLIB.H Version 2.00 (C) Z.Nishikawa

*/

int	m_alloc( int track_no, int buffer_size );
int	m_assign( int channel_no, int track_no );
int	m_vget( int tone_no, char *data_ptr );
int	m_vset( int tone_no, char *data_ptr );
int	m_tempo( int tempo );		/* tempo=-1でリクエスト */
int	m_trk( int track_no, char *MML_ptr );
int	m_free( int track_no );
int	m_play( int track_no1, int track_no2, int track_no3, int track_no4,
		int track_no5, int track_no6, int track_no7, int track_no8,
		int track_no9, int track_no10 );
		/* track_no1=0 で全トラック演奏開始 */
		/* track_no?=0 または 'NASI' 以降を無視する。 */
int	m_stat( int track_bit_pattern );	/* track_bit_pattern=0 で全チャンネル検査 */
int	m_stop( int track_no1, int track_no2, int track_no3, int track_no4,
		int track_no5, int track_no6, int track_no7, int track_no8,
		int track_no9, int track_no10 );
		/* track_no1=0 で全トラック演奏停止 */
		/* track_no?=0 または 'NASI' 以降を無視する。 */
int	m_cont( int track_no1, int track_no2, int track_no3, int track_no4,
		int track_no5, int track_no6, int track_no7, int track_no8,
		int track_no9, int track_no10 );
		/* track_no1=0 で全トラック演奏継続 */
		/* track_no?=0 または 'NASI' 以降を無視する。 */
void	m_init( void );
int	m_atoi( int track_no );
int	m_assign2( char *channel, int track_no );
void	m_ch( char *device );
int	m_pcmset( int note_no, char *filename, int pitch, int vol, int mix_note_no, int delay,
		  int cut, int reverse, int fade_in_out );
		/* int pitch以降のパラメータを省略する場合は 'NASI' とすること */
void	m_pcmplay( int note_no, int pan, int freq );
void	m_rec( void );
void	m_rstop( void );
int	m_save( char *filename );
int	m_trans( char *filename );
int	m_fmvset( int tone_no, char *data_ptr );
int	m_out( int  d1, int  d2, int  d3, int  d4, int  d5, int  d6, int  d7, int  d8, 
	       int  d9, int d10 );
		/* d?=-1 以降を無視する。 */
int	m_dirout( char *adrs, int size );
		/* sizeの省略不可 */
int	m_exc( char *adrs, int size );
		/* sizeの省略不可 */
int	m_roland( int devID, int modelID, char *adrs, int size );
		/* sizeの省略不可 */
int	m_total( void );
int	m_fadeout( int speed );
int	m_pcmcnf( char *filename );

int	sc55_v_reserve( char *adrs, int devID );
				/* size=16 */
int	sc55_reverb( char *adrs, int devID, int size );
				/* size=7 */
int	sc55_chorus( char *adrs, int devID, int size );
				/* size=8 */
int	sc55_part_setup( char part_no, char *adrs, int devID, int size );
				/* size=119 */
int	sc55_drum_setup( char map_no, char note_no, char *adrs, int devID, int size );
				/* size=8 */
int	sc55_print( char *message, int devID );
				/* size=32 */
int	sc55_display( int *pattern, int devID );

int	m_adpcm_block( char *filename );

int	mt32_p_reserve( char *adrs, int devID );
				/* size=9 */
int	mt32_reverb( char *adrs, int devID, int size );
				/* size=3 */
int	mt32_part_setup( char *adrs, int devID, int size );
				/* size=9 */
int	mt32_drum_setup( char note_no, char *adrs, int devID, int size );
				/* size=4 */
int	mt32_common( char timbre_no, char *timbre_name, char *adrs, int devID, int size );
				/* size=4 */
int	mt32_patch( char patch_no, char *adrs, int devID, int size );
				/* size=7 */
int	mt32_partial( char timbre_no, char partial_no, char *adrs, int devID, int size );
				/* size=58 */
int	mt32_print( char *message, int devID );
				/* size=20 */

int	m_print( char *message );
				/* size=96 */

int	u220_setup( char *adrs, int devID );
				/* size=7 */
int	u220_common( char *adrs, int devID );
				/* size=17 */
int	u220_drum_setup( char *adrs, int devID );
				/* size=7 */
int	u220_part_setup( char part_no, char *adrs, int devID );
				/* size=13 */
int	u220_timbre( char timbre_no, char *timbre_name, char *adrs, int devID );
				/* size=26 */
int	u220_drum_inst( char note_no, char *adrs, int devID, int size );
				/* size=20 */
int	u220_print( char *message, int devID );
				/* size=12 */

int	m1_midi_ch( char *midi_ch_list );
				/* size=8 */
int	m1_part_setup( char *track_param );
				/* size=40 */
int	m1_effect_setup( char *effect_param );
				/* size=25 */
int	m1_print( char *message );
				/* size=10 */
int	send_to_m1( int devID );

int	zmd_play( char *filename );
void	m_debug( char mode );
int	m_count( char count );
int	fm_master( char volume );
int	m_mute( int ch_no1, int ch_no2, int ch_no3, int ch_no4,
		int ch_no5, int ch_no6, int ch_no7, int ch_no8,
		int ch_no9, int ch_no10 );
		/* ch_no1=0 で全トラック演奏継続 */
		/* ch_no?=0 または 'NASI' 以降を無視する。 */
int	m_solo( int ch_no1, int ch_no2, int ch_no3, int ch_no4,
		int ch_no5, int ch_no6, int ch_no7, int ch_no8,
		int ch_no9, int ch_no10 );
		/* ch_no1=0 で全トラック演奏継続 */
		/* ch_no?=0 または 'NASI' 以降を無視する。 */
int	m_wave_form( char wave_no, char loop_type, int loop_point,
			int *wave_data, int size );
int	m_wave_form2( char wave_no, char loop_type, int loop_point,
			short int *wave_data, int size );

int	sc55_init( int devID );
int	mt32_init( int devID );

void	adpcm_to_pcm( char *source, int size, short int *destination );
		/* destinationの配列サイズはsourceの４倍必要
		   また、sizeはADPCMデータの個数 */
void	pcm_to_adpcm( short int *source, int size, char *destination );
		/* destinationの配列サイズはsourceの1/4倍必要
		   また、sizeはPCMデータの個数 */

void	exec_zms( char *zms_line );

int	m_inp( char inp_mode );	/* inp_mode≠0でループモード */

int	zm_ver( void );

int	m_trk2( char *MML_ptr,
		int track_no1, int track_no2, int track_no3 ,int track_no4,
		int track_no5, int track_no6, int track_no7 ,int track_no8 );
		/* track_no?=0 または 'NASI' 以降を無視する。 */

int	zm_work( char trk_num, int work_offset );
		/* 演奏トラックワークの値をバイト単位で返す */

/*
	　基本的にＸ－ＢＡＳＩＣ用外部関数であるＭＵＳＩＣＺ．ＦＮＣに
	コンパチに作られていますが、一部の言語仕様の違いから、互換でない
	関数もあります。
	　Ｘ－ＢＡＳＩＣでは配列の要素数を関数側で自動認知することが
	できますがＣ言語ではそれができません。よって、Ｘ－ＢＡＳＩＣでは
	パラメータの個数等を省略できた関数も、Ｃ言語では省略できないといった
	仕様変更がなされた関数があります。

		m_dirout()
		m_exc()
		m_roland()

	  また、さらに、Ｘ－ＢＡＳＩＣではパラメータ個数を書かなくても
	よかった関数もＣ言語では書かせるような仕様に変更されたものもあります。

		sc55_reverb()
		sc55_chorus()
		sc55_part_setup()
		sc55_drum_setup()
		mt32_reverb()
		mt32_part_setup()
		mt32_drum_setup()
		mt32_common()
		mt32_patch()
		mt32_partial()
		u220_drum_inst()

	  ＭＵＳＩＣＺ．ＦＮＣにはなかったコマンドが新設されています。

		m_wave_form2()
		波形メモリ登録コマンド#2 (short int専用版/配列データ加工なし)

*/
