/*	ZMSC.X用 C言語ヘッダファイル
	ZMSC3LIB.H Version 0.01 (C) Z.Nishikawa
*/

/*Structure Declaration*/
typedef struct	MSTRCLK	{
	char	meter_n;	/*拍子分子(1-255)*/
	char	meter_d;	/*拍子分母(1-255)*/
	char	metronome;	/*音楽的音長(1-255)*/
	char	dummy;
	char	key1;		/*調号1(0:C,負値:♭の数,正値:♯の数)*/
	char	key2;		/*調号2(Major=0,Minor=1)*/
	short	master_clock;	/*全音符の絶対音長(1-65535)*/
} MSTRCLK;

/*Prototype Declaration*/
int	zm_check_zmsc( void );
int	zm_ver( void );
int	zm_get_zmlibver( void );
int	zm_init( int param );
int	zm_assign( int device,int track );
int	zm_compiler( int mode,int size,char *zms,char **result );
int	zm_vget( int timbre,int mode,char *buffer );
int	zm_vset( int timbre,int mode,char *buffer );
int	zm_tempo( int tempo,int mode);
int	zm_set_timer_value( int timer,int mode);
short	*zm_play( short *track );
void	zm_play_all( void );
void	zm_play2( void );
void	zm_play_again( void );
void	zm_play_status_all_ch( int *channel_list );
void	zm_play_status_all_tr( short *track_list );
int	zm_play_status_ch( int channel );
int	zm_play_status_tr( int track );
short	*zm_stop( short *track );
void	zm_stop_all( void );
short	*zm_cont( short *track );
void	zm_cont_all( void );
char	*zm_atoi( int track );
MSTRCLK	*zm_set_master_clock( int side,MSTRCLK *mstrclk);
int	zm_play_zmd( int size,char *zmd );
int	zm_play_zmd_se( char *zmd );
int	zm_se_play( char *zmd );
int	zm_se_adpcm1(char data_type,char volume,char frq,char pan,
			int size,short priority,short ch,char *adpcm);
int	zm_se_adpcm2(char data_type,char volume,char frq,char pan,
			int data_number,short priority,short ch);
void	zm_intercept_play( int mode );
int	zm_current_midi_in( int port);
int	zm_current_midi_out( int port);
int	zm_midi_transmission( int port,int size,char *midi);
void	zm_exclusive( int port,int size,int id,char *midi);
int	zm_set_eox_wait( int port,int wait );
int	zm_midi_inp1(int port,int mode );
int	zm_midi_out1(int port,char data );
int	zm_midi_rec( int port );
int	zm_midi_rec_end( int port );
int	zm_gs_reset( int port,int id );
int	zm_gs_partial_reserve( int port,int id,char *rsv);
int	zm_gs_reverb( int port,int size,int id,char *rvb);
int	zm_gs_chorus( int port,int size,int id,char *cho);
int	zm_gs_part_parameter(int port,int size,int pt_id,char *param);
int	zm_gs_part_setup(int port,int size,int pt_id,char *param);
int	zm_gs_drum_parameter(int port,int size,int dr_id,char *param);
int	zm_gs_drum_setup(int port,int size,int dr_id,char *param);
int	zm_gs_drum_name(int port,int size,int mp_id,char *name);
int	zm_gs_print( int port,int size,int id,char *mes );
int	zm_gs_display( int port,int id,short *pattern );
int	zm_sc55_reset( int port,int id );
int	zm_sc55_partial_reserve( int port,int id,char *rsv);
int	zm_sc55_reverb( int port,int size,int id,char *rvb);
int	zm_sc55_chorus( int port,int size,int id,char *cho);
int	zm_sc55_part_parameter(int port,int size,int pt_id,char *param);
int	zm_sc55_part_setup(int port,int size,int pt_id,char *param);
int	zm_sc55_drum_parameter(int port,int size,int dr_id,char *param);
int	zm_sc55_drum_setup(int port,int size,int dr_id,char *param);
int	zm_sc55_drum_name(int port,int size,int mp_id,char *name);
int	zm_sc55_print( int port,int size,int id,char *mes );
int	zm_sc55_display( int port,int id,short *pattern );
int	zm_gm_system_on( int port);
int	zm_sc88_mode_set( int port,int md_id);
int	zm_sc88_reverb( int port,int size,int id,char *rvb);
int	zm_sc88_chorus( int port,int size,int id,char *cho);
int	zm_sc88_equalizer( int port,int size,int id,char *eql);
int	zm_sc88_part_parameter(int port,int size,int pt_id,char *param);
int	zm_sc88_part_setup(int port,int size,int pt_id,char *param);
int	zm_sc88_drum_parameter(int port,int size,int dr_id,char *param);
int	zm_sc88_drum_setup(int port,int size,int dr_id,char *param);
int	zm_sc88_drum_name(int port,int size,int mp_id,char *name);
int	zm_sc88_user_inst(int port,int size,int ui_id,char *param);
int	zm_sc88_user_drum(int port,int size,int ud_id,char *param);
int	zm_mt32_reset( int port,int id );
int	zm_mt32_partial_reserve( int port,int id,char *rsv);
int	zm_mt32_reverb( int port,int size,int id,char *rvb );
int	zm_mt32_part_setup( int port,int size,int id,char *param );
int	zm_mt32_drum( int port,int size,int dr_id,char *drum );
int	zm_mt32_common( int port,int size,int cm_id,char *common );
int	zm_mt32_partial( int port,int size,int pl_id,char *partial );
int	zm_mt32_patch( int port,int size,int pt_id,char *patch );
int	zm_mt32_print( int port,int size,int id,char *mes );
int	zm_u220_setup( int port,int id,char *setup );
int	zm_u220_part_setup( int port,int pt_id,char *setup );
int	zm_u220_common( int port,int id,char *common );
int	zm_u220_timbre( int port,int tm_id,char *timbre );
int	zm_u220_drum_setup( int port,int id,char *setup );
int	zm_u220_drum_inst( int port,int dr_id,char *inst );
int	zm_u220_print( int port,int size,int id,char *mes );
int	zm_m1_setup( char *setup );
int	zm_m1_part_setup( char *ptstup );
int	zm_m1_effect_setup( char *effect );
int	zm_m1_print( int size,char *mes );
int	zm_send_to_m1( int port,int id );
char	*zm_pcm_read( int num,int flag,int type,int orig,char *param);
int	zm_pcm_erase( int num );
int	zm_register_zpd( char *zpd );
int	zm_set_zpd_table( int ver,char *zpd );
int	zm_exec_subfile( char *filename );
int	zm_transmit_midi_dump( int port,char *filename );
int	zm_set_wave_form1( int wave,char *data );
int	zm_set_wave_form2( int wave,char *data );
int	*zm_obtain_events( int omt,int *event );
int	zm_loop_control( int mode );
short	*zm_mask_tracks( short *track );
int	zm_mask_all_tracks( int mode );
int	zm_solo_track( int track );
int	*zm_mask_channels( short *channel );
int	zm_mask_all_channels( int mode );
int	zm_solo_channel( int channel );
int	zm_set_ch_output_level( char *out );
int	zm_set_tr_output_level( char *out );
int	zm_master_fader( char *fdr );
int	zm_get_fader_status( void );
int	zm_get_play_time( void );
char	*zm_get_1st_comment( void );
int	zm_get_timer_mode( void );
char	*zm_get_track_table( void );
char	*zm_get_track_table_se( void );
char	*zm_get_play_work( int track );
char	*zm_get_play_work_se( int track );
char	*zm_get_buffer_information( void );
char	*zm_get_zmsc_status( void );
int	zm_calc_total( char *zmd,char **errtbl);
int	zm_register_application( char *addr,int *result);
int	zm_unregister_application( int result );
int	zm_release_driver( char **zmsc );
int	zm_occupy_zmusic( int mode );
char	*zm_hook_fnc_service( int fnc,char *job );
char	*zm_hook_zmd_service( int zmd,char *job );
int	zm_occupy_compiler( int mode );
int	zm_store_error( int err,int noferr,char **addr );
int	zm_print_error( int mode,int lang,int noferr,
			char *ZMD,char *srcaddr,char *srcname,
			char *errtbl,
			char **buff);
char	*zm_get_mem( int size,int emply );
char	*zm_enlarge_mem( int size,char *addr );
int	zm_free_mem( char *addr );
int	zm_free_mem2( int emply );
int	zm_exchange_memid( int mode,int s_emply,int d_emply,char *addr );
int	zm_init_all( void );
void	zm_int_start( int tmtype );
void	zm_int_stop( int tmtype );
int	zm_set_int_service( int clock,int tempo,char *entry );
char	*zm_check_int_service( void );
int	zm_release_int_service( char *entry );
int	zm_control_tempo( int mode );
void	zm_convert_pcm( int mode, int size, char *source, char *destination );
int	zm_work( int track,int offset );
int	zm_exec_zmd( int track,int size,char *zmd);
