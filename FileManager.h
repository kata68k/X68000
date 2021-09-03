#ifndef	FILEMANAGER_H
#define	FILEMANAGER_H

#include <string.h>

#include "inc/usr_define.h"
#include "Graphic.h"
#include "Raster.h"

extern int16_t Init_FileList_Load(void);
extern int16_t File_Load(int8_t *, void *, size_t, size_t);
extern int16_t File_Save(int8_t *, void *, size_t, size_t);
extern int16_t File_Load_CSV(int8_t *, uint16_t *, uint16_t *, uint16_t *);
extern int16_t File_Load_Course_CSV(int8_t *, ST_ROADDATA *, uint16_t *, uint16_t *);
extern int16_t PCG_SP_dataload(int8_t *);
extern int16_t PCG_PAL_dataload(int8_t *);
extern int16_t Load_Music_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
extern int16_t Load_SE_List(		int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
extern int16_t Load_CG_List(		int8_t *, int8_t *, CG_LIST *, uint32_t *);
extern int16_t Load_MACS_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
extern int16_t GetFileLength(int8_t *, int32_t *);
extern int16_t GetFilePICinfo(int8_t *, BITMAPINFOHEADER *);
extern int16_t GetRectangleSise(uint16_t *, uint16_t, uint16_t, uint16_t);
extern void *MyMalloc(int32_t);
extern int16_t MyMfree(void *);
extern int32_t MaxMemSize(int8_t);

#endif	/* FILEMANAGER_H */
