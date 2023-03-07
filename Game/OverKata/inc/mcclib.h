/*	mcclib 1995 H.Ogasawara (COR.)		*/

extern int	MC_INIT();
extern void	MC_QUIT();

extern int	MC_PLAY( void* );
extern int	MC_SETPCM( void* );
extern void	MC_PLAYSE( void* );
extern void	MC_PLAYSE2( int, void* );
extern void	MC_STOPSE();
extern void	MC_PLAYSEPCM( int, int, void* );
extern void	MC_STOPSEPCM();

extern int	MC_PLAY_FN();
extern int	MC_FPLAY( char* );
extern int	MC_FPDX( char* );

extern void	MC_STOP();
extern void	MC_FADEOUT( int );
extern int	MC_SETFADE( int );
extern int	MC_TEMPOSNS( int );
extern void	MC_PAUSE_FN();
extern void	MC_PAUSE( int );

extern int	MC_ISPLAY();
extern int	MC_ISFADE();
extern int	MC_ISKEEP();
extern int	MC_LOOPCOUNT();
extern int	MC_NOWCLOCK();
extern int	MC_TOTALCLOCK();
extern char	*MC_TITLE();
extern char	*MC_COMMENT();

extern void	MC_SKIPPLAY( int );
extern void	MC_JUMPPLAY( int );
extern int	MC_RELEASE();
extern int	MC_UNREMOVE( int );
extern void	*MC_TRANSMDC( int, void* );
extern void	*MC_TRANSPCM( int, void* );

extern int	MC_MMCP_PLAY( char* );

