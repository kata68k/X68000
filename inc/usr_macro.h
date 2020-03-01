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

//unsigned short SetRGB(unsigned short R, unsigned short G, unsigned short B)
#define SetRGB(R,G,B)	(( G << 11) + (R << 6) + (B << 1))

#endif	/* USR_MACRO_H */
