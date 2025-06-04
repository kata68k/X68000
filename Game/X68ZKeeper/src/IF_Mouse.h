#ifndef	IF_MOUSE_H
#define	IF_MOUSE_H

#include <string.h>

#include <usr_define.h>

extern void Mouse_Init(void);
extern void Mouse_Exit(void);
extern void Mouse_Cur_Off(void);
extern void Mouse_GetPos(int32_t *, int32_t *);
extern void Mouse_SetPos(int32_t , int32_t);
extern void Mouse_GetDataPos(int8_t *, int8_t *, int8_t *, int8_t *);

#endif	/* IF_MOUSE_H */
