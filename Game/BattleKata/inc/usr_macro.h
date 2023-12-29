#ifndef	USR_MACRO_H
#define	USR_MACRO_H

/*
        Mmax - �ő�l�擾 -
*/
/**
 * �w�肳�ꂽ2�̈����̂���,�ő�̂��̂�Ԃ�.
 * @param [in] a        �ő�l��I�ԑΏۂ̕ϐ�1
 * @param [in] b        �ő�l��I�ԑΏۂ̕ϐ�2
 * @retval �ő�l
 * @attention �����͕�����]�������.
 */
#define Mmax(a, b) ({ \
  typeof(a) A = (a); \
  typeof(b) B = (b); \
  A < B ? B : A; \
})
/*
        Mmin - �ŏ��l�擾 -
*/
/**
 * �w�肳�ꂽ2�̈����̂���,�ŏ��̂��̂�Ԃ�.
 * @param [in] a        �ŏ��l��I�ԑΏۂ̕ϐ�1
 * @param [in] b        �ŏ��l��I�ԑΏۂ̕ϐ�2
 * @retval �ŏ��l
 * @attention �����͕�����]�������.
 */
#define Mmin(a, b) ({ \
  typeof(a) A = (a); \
  typeof(b) B = (b); \
  A < B ? A : B; \
})

/*
        Mminmax - �ŏ��ő�l�擾 -
*/
/**
 * �w�肳�ꂽ2�̈����̂���,�ŏ��̂��̂�Ԃ�.
 * @param [in] a        ����Ώۂ̕ϐ�1
 * @param [in] b        �ŏ��l��I�ԑΏۂ̕ϐ�2
 * @param [in] c        �ő�l��I�ԑΏۂ̕ϐ�3
 * @retval �ŏ��ő�l
 * @attention �����͕�����]�������.
 */
#define Mminmax(a, b, c) ((b < c)?((a < b)?b:(a > c)?c:a):((a < c)?c:(a > b)?b:a))

/*
        Mabs - ��Βl�擾 -
*/
/**
 * �w�肳�ꂽ�����̐�Βl��Ԃ�.
 * @param [in] x        ��Βl�ɂ���Ώۂ̕ϐ�
 * @retval ��Βl
 * @attention �����͕�����]�������.
 */
#define Mabs(x) ((x) < 0 ? -(x) : (x))

/*
        McmpSub - ��r���Č��Z���� -
*/
/**
 * �w�肳�ꂽ�����̐�Βl��Ԃ�.
 * @param [in] x        ��Βl�ɂ���Ώۂ̕ϐ�
 * @retval ��Βl
 * @attention �����͕�����]�������.
 */
#define McmpSub(x,y) ( (x < y) ? (y - x) : (x - y) )

/*
        Mbset - bit�Z�b�g -
*/
/**
 * �w�肳�ꂽ�����̃r�b�g���Z�b�g.
 * @param [in] x        ���̒l
 * @param [in] y        �ύX�������̈�
 * @param [in] z        �ύX�������l
 * @retval �r�b�g�Z�b�g���ꂽ�l
 * @attention �����͕�����]�������.
 */
#define Mbset(x,y,z) (((x) & ~(y)) | (z))

/*
        Mbclr - bit�N���A -
*/
/**
 * �w�肳�ꂽ�����̃r�b�g���N���A.
 * @param [in] x        ���̒l
 * @param [in] y        0�ɂ������̈�
 * @retval �r�b�g�Z�b�g���ꂽ�l
 * @attention �����͕�����]�������.
 */
#define Mbclr(x,y) ((x) & ~(y))

/*
        Mdec - �f�N�������g�i�O�N���b�v�j -
*/
/**
 * �J�E���^�l�����Z�������l�łO�܂Ńf�N�������g
 * @param [in] x        �J�E���^
 * @param [in] y        ���Z�������l
 * @retval ���Z���ꂽ�l
 * @attention �����͕�����]�������.
 */
/* @fujitanozomu����, @kunichiko���� �̂����͂łł��܂��� */
#define Mdec(x, y) \
( \
	((typeof(x))((unsigned)(x) - (y)) <= (x)) ? ( \
		(typeof(x))((unsigned)(x) - (y)) \
	) : (typeof(x))-1 > 0 ? ( \
		0 \
	) : ( \
		(typeof(x))((1U << (8 * sizeof(x) - 1))) \
	) \
)

/*
        Minc - �f�N�������g�i�ő�l�N���b�v�j -
*/
/**
 * �J�E���^�l�����Z�������l�ōő�l�܂ŃC���N�������g
 * @param [in] x        �J�E���^
 * @param [in] y        ���Z�������l
 * @retval ���Z���ꂽ�l
 * @attention �����͕�����]�������.
 */
/* @fujitanozomu����, @kunichiko���� �̂����͂łł��܂��� */
#define Minc(x, y) \
( \
	((typeof(x))((unsigned)(x) + (y)) >= (x)) ? ( \
		(typeof(x))((unsigned)(x) + (y)) \
	) : (typeof(x))-1 > 0 ? ( \
		(typeof(x))-1 \
	) : ( \
		(typeof(x))((1U << (8 * sizeof(x) - 1)) - 1) \
	) \
)

#define SWAP(a, b) do { typeof(a) tmp = a; a = b; b = tmp; } while (0)

#define M1024Roll(x)	(x & 0x3FF)
#define M512Roll(x)		(x & 0x1FF)
#define M256Roll(x)		(x & 0xFF)

#define MrasRoll(x)		(((x) < 0) ? (((x) % 224)*-1) : ((x) > 224) ? ((x) % 224) : (x))

#define Msinged_9b(x)	((x) % 256)
#define Mu10b_To_s8b(x)	(((x) & 0xFF) | (((x) & 0x200) ? 0xFFFFFF00 : 0))
#define Mu10b_To_s9b(x)	(((x) & 0x1FF) | (((x) & 0x200) ? 0xFE00 : 0))

/* �V�t�g����Z */
#define Mdiv2(x)		(x>>1)
#define Mdiv4(x)		(x>>2)
#define Mdiv8(x)		(x>>3)
#define Mdiv10(x)		((x * 0xCCCDu)>>19)
#define Mdiv16(x)		(x>>4)
#define Mdiv32(x)		(x>>5)
#define Mdiv64(x)		(x>>6)
#define Mdiv128(x)		(x>>7)
#define Mdiv256(x)		(x>>8)
#define Mdiv512(x)		(x>>9)
#define Mdiv1024(x)		(x>>10)
#define Mdiv2048(x)		(x>>11)
#define Mdiv4096(x)		(x>>12)
#define Mdiv8192(x)		(x>>13)
#define Mdiv16384(x)	(x>>14)
#define Mdiv32768(x)	(x>>15)
#define Mdiv65536(x)	(x>>16)
/* �V�t�g�|���Z */
#define Mmul2(x)		(x<<1)
#define Mmul4(x)		(x<<2)
#define Mmul8(x)		(x<<3)
#define Mmul16(x)		(x<<4)
#define Mmul32(x)		(x<<5)
#define Mmul64(x)		(x<<6)
#define Mmul128(x)		(x<<7)
#define Mmul256(x)		(x<<8)
#define Mmul512(x)		(x<<9)
#define Mmul1024(x)		(x<<10)
#define Mmul2048(x)		(x<<11)
#define Mmul4096(x)		(x<<12)
#define Mmul8192(x)		(x<<13)
#define Mmul16384(x)	(x<<14)
#define Mmul32768(x)	(x<<15)
#define Mmul65536(x)	(x<<16)

#define Mmul_1p25(x)	(x+(x>>2))										/* x * 1.25 */
#define Mmul_1p90(x)	(x+(x>>1)+(x>>2)+(x>>3)+(x>>4)-(x>>5)-(x>>7))	/* x * 0.8984375 */
#define Mmul_1p80(x)	(x+(x>>1)+(x>>2)+(x>>5)+(x>>6)+(x>>8))			/* x * 0.8007813 */
#define Mmul_1p75(x)	(x+(x>>1)+(x>>2))								/* x * 1.75 */
#define Mmul_1p70(x)	(x+(x>>1)+(x>>3)+(x>>4)+(x>>6)-(x>>8))			/* x * 1.6992188 */
#define Mmul_1p60(x)	(x+(x>>1)+(x>>3)-(x>>6)-(x>>7))					/* x * 1.6015625 */
#define Mmul_1p50(x)	(x+(x>>1))										/* x * 1.5 */
#define Mmul_1p40(x)	(x+(x>>2)+(x>>3)+(x>>6)+(x>>7))					/* x * 1.3984375 */
#define Mmul_1p30(x)	(x+(x>>2)+(x>>4)-(x>>7)-(x>>8))					/* x * 1.3007813 */
#define Mmul_1p20(x)	(x+(x>>3)+(x>>4)+(x>>6)-(x>>8))					/* x * 1.1992188 */
#define Mmul_1p10(x)	(x+(x>>3)-(x>>5)+(x>>6)-(x>>7))					/* x * 1.1015625 */
#define Mmul_1p00(x)	(x)												/* x * 1 */
#define Mmul_0p91(x)	((x>>1)+(x>>2)+(x>>3)+(x>>5)+(x>>8))			/* x * 0.9101563 */
#define Mmul_0p90(x)	((x>>1)+(x>>2)+(x>>3)+(x>>4)-(x>>5)-(x>>7))		/* x * 0.8984375 */
#define Mmul_0p83(x)	((x>>1)+(x>>2)+(x>>3)-(x>>5)+(x>>6)+(x>>8))		/* x * 0.8320313 */
#define Mmul_0p80(x)	((x>>1)+(x>>2)+(x>>5)+(x>>6)+(x>>8))			/* x * 0.8007813 */
#define Mmul_0p77(x)	((x>>1)+(x>>2)+(x>>6)+(x>>8))					/* x * 0.7695313 */
#define Mmul_0p75(x)	((x>>1)+(x>>2))									/* x * 0.75 */
#define Mmul_0p71(x)	((x>>1)+(x>>3)+(x>>4)+(x>>5))					/* x * 0.7187500 */
#define Mmul_0p70(x)	((x>>1)+(x>>3)+(x>>4)+(x>>6)-(x>>8))			/* x * 0.6992188 */
#define Mmul_0p66(x)	((x>>1)+(x>>3)+(x>>5)+(x>>7))					/* x * 0.6640625 */
#define Mmul_0p62(x)	((x>>1)+(x>>3))									/* x * 0.625 */
#define Mmul_0p60(x)	((x>>1)+(x>>3)-(x>>6)-(x>>7))					/* x * 0.6015625 */
#define Mmul_0p58(x)	((x>>1)+(x>>4)+(x>>5)-(x>>7))					/* x * 0.5859375 */
#define Mmul_0p54(x)	((x>>1)+(x>>4)+(x>>6)+(x>>7))					/* x * 0.5859375 */
#define Mmul_0p52(x)	((x>>1)+(x>>5)-(x>>6)+(x>>7))					/* x * 0.5234375 */
#define Mmul_0p50(x)	((x>>1))										/* x * 0.5 */
#define Mmul_0p40(x)	((x>>2)+(x>>3)+(x>>6)+(x>>7))					/* x * 0.3984375 */
#define Mmul_0p30(x)	((x>>2)+(x>>4)-(x>>7)-(x>>8))					/* x * 0.3007813 */
#define Mmul_0p20(x)	((x>>3)+(x>>4)+(x>>6)-(x>>8))					/* x * 0.1992188 */
#define Mmul_0p10(x)	((x>>3)-(x>>5)+(x>>6)-(x>>7))					/* x * 0.1015625 */

#define SetRGB(R,G,B)	(( G << 11) + (R << 6) + (B << 1))
#define GetR(color)	(( color >> 6) & 0x1Fu)
#define GetG(color)	(( color >> 11) & 0x1Fu)
#define GetB(color)	(( color >> 1) & 0x1Fu)

#define SetBGcode(V,H,PAL,PCG)	(0xCFFFU & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) | (PCG & 0xFFU)))
#define SetBGcode2(V,H,PAL)		(0xCF00U & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) ))

#define ARRAY_LENGTH(array)	(sizeof(array)/sizeof(array[0]))

#endif	/* USR_MACRO_H */
