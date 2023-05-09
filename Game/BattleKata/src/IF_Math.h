#ifndef	IF_MATH_H
#define	IF_MATH_H

#include <usr_define.h>

#define PI	(3.14159265f)

extern	const	int16_t	t_Sin256[91];

extern	FLOAT APL_Atan2(FLOAT , FLOAT );
extern	int16_t APL_Cos(int16_t);
extern	int16_t APL_Sin(int16_t);
extern	int16_t APL_Tan256(int16_t);
extern	uint32_t APL_uDiv(uint32_t, uint32_t);
extern	int32_t APL_sDiv(int32_t, int32_t);
extern	int16_t APL_AngleDiff(int16_t, int16_t);
extern	int16_t APL_mul_1p90(int16_t);
extern	int16_t APL_mul_1p80(int16_t);
extern	int16_t APL_mul_1p75(int16_t);
extern	int16_t APL_mul_1p70(int16_t);
extern	int16_t APL_mul_1p60(int16_t);
extern	int16_t APL_mul_1p50(int16_t);
extern	int16_t APL_mul_1p40(int16_t);
extern	int16_t APL_mul_1p30(int16_t);
extern	int16_t APL_mul_1p25(int16_t);
extern	int16_t APL_mul_1p20(int16_t);
extern	int16_t APL_mul_1p10(int16_t);
extern	int16_t APL_mul_1p00(int16_t);
extern	int16_t APL_mul_0p91(int16_t);
extern	int16_t APL_mul_0p83(int16_t);
extern	int16_t APL_mul_0p77(int16_t);
extern	int16_t APL_mul_0p71(int16_t);
extern	int16_t APL_mul_0p66(int16_t);
extern	int16_t APL_mul_0p62(int16_t);
extern	int16_t APL_mul_0p58(int16_t);
extern	int16_t APL_mul_0p54(int16_t);
extern	int16_t APL_mul_0p52(int16_t);
extern	int16_t APL_mul_0p50(int16_t);

#endif	/* IF_MATH_H */
