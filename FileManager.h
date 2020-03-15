#ifndef	FILEMANAGER_H
#define	FILEMANAGER_H

#include <string.h>

#include "inc/usr_style.h"
#include "inc/usr_define.h"

extern SI File_Load(SC *, void *, size_t, size_t);
extern SI File_Load_CSV(SC *, US *, UI *, UI *);
extern void sp_dataload(void);

#endif	/* FILEMANAGER_H */
