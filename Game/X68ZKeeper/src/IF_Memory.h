#ifndef	IF_MEMORY_H
#define	IF_MEMORY_H

#include <string.h>

#include <usr_define.h>

extern void *MyMalloc(int32_t);
extern int16_t MyMfree(void *);
extern int32_t MaxMemSize(int8_t);
extern int32_t GetFreeMem(void);
extern int32_t	GetMaxFreeMem(void);

#endif	/* IF_MEMORY_H */
