#ifndef	APL_MATH_H
#define	APL_MATH_H

#include "inc/usr_define.h"

#define PI	(3.14159265f)

extern	const	int16_t	t_Sin256[91];

extern	FLOAT APL_Atan2(FLOAT , FLOAT );
extern	int16_t APL_Cos(int16_t);
extern	int16_t APL_Sin(int16_t);
extern	int16_t APL_Tan256(int16_t);
extern	uint32_t APL_uDiv(uint32_t, uint32_t);
extern	int32_t APL_sDiv(int32_t, int32_t);
#endif	/* APL_MATH_H */
