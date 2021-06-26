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
#define Mmax(a, b) ((a) > (b) ? (a) : (b))

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
#define Mmin(a, b) ((a) < (b) ? (a) : (b))

/*
        Mminmax - 最小最大値取得 -
*/
/**
 * 指定された2つの引数のうち,最小のものを返す.
 * @param [in] a        最小値を選ぶ対象の変数1
 * @param [in] b        最小値を選ぶ対象の変数2
 * @param [in] c        最大値を選ぶ対象の変数3
 * @retval 最小最大値
 * @attention 引数は複数回評価される.
 */
#define Mminmax(a, b, c) ((a) > (c) ? (c) : ((a) < (b) ? (a) : (b)))

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
#define Mdec(x,y) ((x > (x - y)) ? (x - y) : 0)

/*
        Minc - デクリメント（最大値クリップ） -
*/
/**
 * カウンタ値を減算したい値で最大値までインクリメント
 * @param [in] x        カウンタ
 * @param [in] y        加算したい値
 * @retval 減算された値
 * @attention 引数は複数回評価される.
 */
#define Minc(x,y) ((0 < (x + y)) ? (x + y) : -1)

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
#define Mdiv512(x)		(x>>9)
#define Mdiv1024(x)		(x>>10)
#define Mdiv2048(x)		(x>>11)
#define Mdiv4096(x)		(x>>12)
#define Mdiv8192(x)		(x>>13)
#define Mdiv16384(x)	(x>>14)
#define Mdiv32768(x)	(x>>15)
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

#define Mmul_p1(x)		((x>>3)-(x>>5)+(x>>6)-(x>>7))	/* x / 10 */
#define Mmul_1p25(x)	(x+(x>>2))	/* x * 1.25 */

#define SetRGB(R,G,B)	(( G << 11) + (R << 6) + (B << 1))
#define GetR(color)	(( color >> 6) & 0x1Fu)
#define GetG(color)	(( color >> 11) & 0x1Fu)
#define GetB(color)	(( color >> 1) & 0x1Fu)

#define SetBGcode(V,H,PAL,PCG)	(0xCFFFU & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) | (PCG & 0xFFU)))
#define SetBGcode2(V,H,PAL)		(0xCF00U & (((V & 0x01U)<<15U) | ((H & 0x01U)<<14U) | ((PAL & 0xFU)<<8U) ))

#endif	/* USR_MACRO_H */
