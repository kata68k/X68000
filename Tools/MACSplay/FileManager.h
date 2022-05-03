#ifndef	FILEMANAGER_H
#define	FILEMANAGER_H

#include <string.h>

#include "inc/usr_define.h"

extern int16_t File_Load(int8_t *, void *, size_t, size_t);
extern int16_t GetFileLength(int8_t *, int32_t *);
extern void *MyMalloc(int32_t);
extern void *MyMallocHi(int32_t);
extern int16_t MyMfree(void *);
extern int16_t MyMfreeHi(void *);
extern int32_t MaxMemSize(int8_t);

#endif	/* FILEMANAGER_H */
