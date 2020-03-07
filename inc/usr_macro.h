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
        Mabs - 絶対値取得 -
*/
/**
 * 指定された引数の絶対値を返す.
 * @param [in] x        絶対値にする対象の変数
 * @retval 絶対値
 * @attention 引数は複数回評価される.
 */
#define Mabs(x) ((x) < 0 ? -(x) : (x))

//unsigned short SetRGB(unsigned short R, unsigned short G, unsigned short B)
#define SetRGB(R,G,B)	(( G << 11) + (R << 6) + (B << 1))

#endif	/* USR_MACRO_H */
