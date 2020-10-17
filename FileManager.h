#ifndef	FILEMANAGER_H
#define	FILEMANAGER_H

#include <string.h>

#include "inc/usr_define.h"

extern SS File_Load(SC *, void *, size_t, size_t);
extern SS File_Load_CSV(SC *, US *, US *, US *);
extern SS File_Load_Course_CSV(SC *, ST_ROADDATA *, US *, US *);
extern SS PCG_SP_dataload(SC *);
extern SS PCG_PAL_dataload(SC *);
extern SS APICG_DataLoad(SC *, US, US, US);
extern SS Load_Music_List(SC *, SC (*)[256], UI *);
extern SS Load_SE_List(SC *, SC (*)[256], UI *);
extern SS Load_CG_List(SC *, SC (*)[256], UI *);
extern SS Load_MACS_List(SC *, SC (*)[256], UI *);
extern SS File_To_Mem(SC *, SC *, SI);
extern SS GetFileLength(SC *, SI *);

#endif	/* FILEMANAGER_H */
