/*	*	*	*	*	*	*	*	*	*
 *	C-Library Header-File for APICGLIB.a
 *	written by GORRY.
 *	$Id: apicglib.h,v 2.0.0.5 1995/08/15 02:55:08 GORRY Exp GORRY $
 */

#ifndef	__APICGLIB_H
#define	__APICGLIB_H


#ifndef	__CLASS_H
#define	__CLASS_H

typedef	int		INT;
typedef	unsigned int	UINT;
typedef	short		WORD;
typedef	unsigned short	UWORD;
typedef	char		BYTE;
typedef	unsigned char	UBYTE;
typedef	void		VOID;
typedef	int		DEFAULT;
typedef	int		BOOLEAN;
typedef	long		LONG;
typedef	unsigned long	ULONG;

#endif	/* __CLASS_H */


/*
 *	APICG Flags
 */

#define	APF_NOINITCRT	(1<<0)
#define	APF_NOCLRBUF	(1<<1)
#define	APF_NOPRFC	(1<<2)
#define	APF_NOADJUST	(1<<3)
#define	APF_NOPRC	(1<<4)
#define	APF_LDFORCE256	(1<<8)
#define	APF_LD1024_256	(1<<9)
#define	APF_LDIMAGE	(1<<15)
#define	APF_CRT24	(1<<16)
#define	APF_NOLOAD	(1<<31)

#define	APF_NOEXHED	(1<<0)
#define	APF_SV65536	(1<<1)
#define	APF_SVEXFLG	(1<<2)
#define	APF_SVIMAGE	(1<<15)
#define	APF_SVEXLOW	(1<<16)
#define	APF_SVEXSQU	(1<<17)

/*
 *	APICG Return Information
 */

struct APICGINFO {
	long	STARTX;
	long	STARTY;
	long	SIZEX;
	long	SIZEY;
	long	COLOR;
};

/*
 *	Load from File
 */

int PICLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG );
int PIKLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG );
int APICLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE * );
int APICGLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int VAPICLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, UBYTE *, long );
int MAGLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int LMAGLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int MKILOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int PILOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long, UBYTE *, long );
int PI256LOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long, UBYTE *, long );
int TIFLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int LTIFLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int BMPLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int LBMPLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int JPGLOAD( UWORD *, char *, long, long, UBYTE *, long, ULONG, long );

int M_PICLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG );
int M_PIKLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG );
int M_APICLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE * );
int M_APICGLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_VAPICLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, UBYTE *, long );
int M_MAGLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_LMAGLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_MKILOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_PILOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long, UBYTE *, long );
int M_PI256LOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long, UBYTE *, long );
int M_TIFLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_LTIFLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_BMPLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_LBMPLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, UBYTE *, long );
int M_JPGLOAD( UWORD *, struct APICGINFO *, long, long, UBYTE *, long, ULONG, long );

/*
 *	Save to File
 */

int PICSAVE( UWORD *, char *, long, long, long, long, UBYTE *, UBYTE *, long, ULONG );
int APICSAVE( UWORD *, char *, long, long, long, long, UBYTE *, UBYTE *, long, ULONG, UBYTE * );

int M_PICSAVE( UWORD *, long *, long, long, long, long, UBYTE *, UBYTE *, long, ULONG );
int M_APICSAVE( UWORD *, long *, long, long, long, long, UBYTE *, UBYTE *, long, ULONG, UBYTE * );

/*
 *	Error Code
 */

#define	_ERRAPG_FORMAT	(-123)
#define	_ERRAPG_FILEBUF	(-122)
#define	_ERRAPG_RECT	(-121)
#define	_ERRAPG_MODE	(-119)
#define	_ERRAPG_WORKBUF	(-115)
#define	_ERRAPG_FUTURE	(-113)




/*
 *	*	*	*	*	*	*	*	*
 *	このファイルはパブリック・ドメインとなっています。	*
 *	改造・再配布・使用について必要な条件はありません。	*
 *	また、このファイルの使用による責務を作者である		*
 *	GORRYは負わないこととします。				*
 *	*	*	*	*	*	*	*	*
 */




#endif	/* __APICGLIB_H */
