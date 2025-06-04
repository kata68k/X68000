#ifndef	USR_MACRO_H
#define	USR_MACRO_H

/*
        Mmax - 最大値取得 -
*/
/**
 * 指定された2つの引数のうち,最大のものを返す.
 * @param [in] a        最大値を選ぶ対象の変数1
 * @param [in] b        最大値を選ぶ対象の変数2
 * @retval 最大値
 * @attention 引数は複数回評価される.
 */
#define Mmax(a, b) ({ \
  typeof(a) A = (a); \
  typeof(b) B = (b); \
  A < B ? B : A; \
})
/*
        Mmin - 最小値取得 -
*/
/**
 * 指定された2つの引数のうち,最小のものを返す.
 * @param [in] a        最小値を選ぶ対象の変数1
 * @param [in] b        最小値を選ぶ対象の変数2
 * @retval 最小値
 * @attention 引数は複数回評価される.
 */
#define Mmin(a, b) ({ \
  typeof(a) A = (a); \
  typeof(b) B = (b); \
  A < B ? A : B; \
})

/*
        Mminmax - 最小最大値取得 -
*/
/**
 * 指定された2つの引数のうち,最小のものを返す.
 * @param [in] a        判定対象の変数1
 * @param [in] b        最小値を選ぶ対象の変数2
 * @param [in] c        最大値を選ぶ対象の変数3
 * @retval 最小最大値
 * @attention 引数は複数回評価される.
 */
#define Mminmax(a, b, c) ((b < c)?((a < b)?b:(a > c)?c:a):((a < c)?c:(a > b)?b:a))

/*
        Mabs - 絶対値取得 -
*/
/**
 * 指定された引数の絶対値を返す.
 * @param [in] x        絶対値にする対象の変数
 * @retval 絶対値
 * @attention 引数は複数回評価される.
 */
#define Mabs(x) ((x) < 0 ? -(x) : (x))

/*
        McmpSub - 比較して減算する -
*/
/**
 * 指定された引数の絶対値を返す.
 * @param [in] x        絶対値にする対象の変数
 * @retval 絶対値
 * @attention 引数は複数回評価される.
 */
#define McmpSub(x,y) ( (x < y) ? (y - x) : (x - y) )

/*
        Mbset - bitセット -
*/
/**
 * 指定された引数のビットをセット.
 * @param [in] x        元の値
 * @param [in] y        変更したい領域
 * @param [in] z        変更したい値
 * @retval ビットセットされた値
 * @attention 引数は複数回評価される.
 */
#define Mbset(x,y,z) (((x) & ~(y)) | (z))

/*
        Mbclr - bitクリア -
*/
/**
 * 指定された引数のビットをクリア.
 * @param [in] x        元の値
 * @param [in] y        0にしたい領域
 * @retval ビットセットされた値
 * @attention 引数は複数回評価される.
 */
#define Mbclr(x,y) ((x) & ~(y))

/*
        Mdec - デクリメント（０クリップ） -
*/
/**
 * カウンタ値を減算したい値で０までデクリメント
 * @param [in] x        カウンタ
 * @param [in] y        減算したい値
 * @retval 減算された値
 * @attention 引数は複数回評価される.
 */
/* @fujitanozomuさん, @kunichikoさん のご協力でできました */
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
        Minc - デクリメント（最大値クリップ） -
*/
/**
 * カウンタ値を加算したい値で最大値までインクリメント
 * @param [in] x        カウンタ
 * @param [in] y        加算したい値
 * @retval 加算された値
 * @attention 引数は複数回評価される.
 */
/* @fujitanozomuさん, @kunichikoさん のご協力でできました */
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

/* シフト割り算 */
#define Mdiv2(x)		(x>>1)
#define Mdiv4(x)		(x>>2)
#define Mdiv8(x)		(x>>3)
#define Mdiv10(x)		((x * 0xCCCDu)>>19)
#define Mdiv16(x)		(x>>4)
#define Mdiv32(x)		(x>>5)
#define Mdiv64(x)		(x>>6)
#define Mdiv128(x)		(x>>7)
#define Mdiv256(x)		(x>>8)
#define Mdiv256s(x)		(((x) < 0) ? ~((~(x)) >> (8)) : ((x) >> (8)))
#define Mdiv512(x)		(x>>9)
#define Mdiv1024(x)		(x>>10)
#define Mdiv2048(x)		(x>>11)
#define Mdiv4096(x)		(x>>12)
#define Mdiv8192(x)		(x>>13)
#define Mdiv16384(x)	(x>>14)
#define Mdiv32768(x)	(x>>15)
#define Mdiv65536(x)	(x>>16)
/* シフト掛け算 */
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
#define SetRGB_24BIT_TO_16BIT(val) ( \
                                        (((((val) >>  0) & 0xFF) << 5) >> 8) << 1 | \
                                        (((((val) >> 16) & 0xFF) << 5) >> 8) << 6 | \
                                        (((((val) >>  8) & 0xFF) << 5) >> 8) << 11 )

#define SetXSPinfo(V,H,PAL,PRI)	(0xCFFFU & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) | (PRI & 0x3FU)))
#define SetBGcode(V,H,PAL,PCG)	(0xCFFFU & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) | (PCG & 0xFFU)))
#define SetBGcode2(V,H,PAL)	(0xCF00U & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) ))
#define GetSPpal(x)             (x & 0xF00U)

#define ARRAY_LENGTH(array)	(sizeof(array)/sizeof(array[0]))

#endif	/* USR_MACRO_H */
