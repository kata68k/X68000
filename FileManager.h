#ifndef	FILEMANAGER_H
#define	FILEMANAGER_H

#include <string.h>

#include "inc/usr_define.h"
#include "Graphic.h"

extern SS File_Load(SC *, void *, size_t, size_t);
extern SS File_Load_CSV(SC *, US *, US *, US *);
extern SS File_Load_Course_CSV(SC *, ST_ROADDATA *, US *, US *);
extern SS PCG_SP_dataload(SC *);
extern SS PCG_PAL_dataload(SC *);
extern SS Load_Music_List(SC *, SC (*)[256], UI *);
extern SS Load_SE_List(SC *, SC (*)[256], UI *);
extern SS Load_CG_List(SC *, CG_LIST *, UI *);
extern SS Load_MACS_List(SC *, SC (*)[256], UI *);
extern SS GetFileLength(SC *, SI *);
extern SS GetFilePICinfo(SC *, BITMAPINFOHEADER *);
extern SS GetRectangleSise(US *, US, US, US);
extern void *MyMalloc(SI);
extern SS MyMfree(void *);

#endif	/* FILEMANAGER_H */
