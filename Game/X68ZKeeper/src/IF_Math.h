#ifndef	IF_MATH_H
#define	IF_MATH_H

#include <usr_define.h>
#include <stdio.h>

#define LENGTH 64  /* 8 * 8 */
#define FIXED_SCALE 16 // �Œ菬���_�̃X�P�[�����O
#define PI	(3.14159265f)
// �Œ菬���_���̒�`
#define FIXED_POINT_SCALE 256 // �Œ菬���_�̃X�P�[�����O�t�@�N�^�[ (2^8)
#define FIXED_POINT_SHIFT 6 // 10�r�b�g�𐮐����A6�r�b�g���������Ƃ���
// �Œ菬���_���̕ϊ��t�@�N�^�[
#define FIXED_POINT_FACTOR      (1 << FIXED_POINT_SHIFT)
#define FIXED_POINT_ONE 		(1 << FIXED_POINT_SHIFT)			// 1���Œ菬���_�ŕ\��
#define TO_FIXED_POINT(x)		((x) << FIXED_POINT_SHIFT)			// �������Œ菬���_�ɕϊ�
#define FROM_FIXED_POINT(x)		((x) >> FIXED_POINT_SHIFT)			// �Œ菬���_�𐮐��ɕϊ�
#define FIXED_POINT_MULT(a, b)	(((a) * (b)) >> FIXED_POINT_SHIFT)	// �Œ菬���_�̏�Z


// �Œ菭���_���̌^��`
typedef int16_t fixed_point_t;
// ��`�̍\����
typedef struct {
    int16_t x, y; // ������̍��W
    int16_t width, height; // ���ƍ���
} Rectangle;

// �_�̍\����
typedef struct {
    int32_t x; // ������̍��W
    int32_t y; // ������̍��W
} Point;

extern	const	int16_t	t_Sin256[91];

extern	FLOAT APL_Atan2(FLOAT , FLOAT );
extern	int16_t APL_Cos(int16_t);
extern	int16_t APL_Sin(int16_t);
extern	int16_t APL_Tan256(int16_t);
extern	uint16_t APL_uSub16(uint16_t, uint16_t);
extern	uint32_t APL_uDiv(uint32_t, uint32_t);
extern	int32_t APL_sDiv(int32_t, int32_t);
extern	int16_t APL_AngleDiff(int16_t, int16_t);
extern	int32_t APL_distance(Point, Point);
extern	float APL_distance_f(Point, Point);
extern	int32_t APL_fixed_point_atan(int32_t, int32_t);
extern	int32_t APL_calculate_angle(Point, Point);
extern	int16_t APL_calculate_b_coordinates(Point, Point, Point *, Point *);
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
extern	uint32_t APL_calculateCRC(uint8_t *, size_t);
extern	uint16_t BCD_to_decimal(uint16_t);
extern	uint16_t decimal_to_BCD(uint16_t);

#endif	/* IF_MATH_H */
