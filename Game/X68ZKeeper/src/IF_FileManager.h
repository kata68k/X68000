#ifndef	IF_FILEMANAGER_H
#define	IF_FILEMANAGER_H

#include <string.h>

#include <usr_define.h>
#include "IF_Graphic.h"
#include "IF_MACS.h"

#define	DATA_MAX	(128)

extern int16_t Init_FileList_Load(void);
extern size_t File_Load(int8_t *, void *, size_t, size_t);
extern int16_t File_Save(int8_t *, void *, size_t, size_t);
extern int16_t File_Save_OverWrite(int8_t *, void *, size_t, size_t);
extern int16_t File_Load_CSV(int8_t *, uint16_t *, uint16_t *, uint16_t *);
//extern int16_t File_Load_Course_CSV(int8_t *fname, ST_ROADDATA *st_ptr, uint16_t *Col, uint16_t *Row);
extern int16_t PCG_SP_dataload(int8_t *);
extern int16_t PCG_PAL_dataload(int8_t *);
extern int16_t PCG_MAP_dataload(int8_t *);
extern int16_t Load_Music_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
extern int16_t Load_SE_List(		int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
extern int16_t Load_CG_List(		int8_t *, int8_t *, CG_LIST *, uint32_t *);
extern int16_t Load_MACS_List(	int8_t *, int8_t *, MOV_LIST *, uint32_t *);
extern int16_t Load_DATA_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
extern int16_t GetFileLength(int8_t *, int32_t *);
extern int16_t GetFilePICinfo(int8_t *, BITMAPINFOHEADER *);
extern int16_t GetRectangleSise(uint16_t *, uint16_t, uint16_t, uint16_t);
extern int16_t Get_DataList_Num(uint16_t *);
extern int16_t Set_DataList_Num(uint16_t);
extern int8_t *Get_DataList_Path(void);
extern int16_t Get_FileAlive(const int8_t *);
extern int16_t Get_ZPDFileName(const int8_t *, int8_t *);

#endif	/* IF_FILEMANAGER_H */
