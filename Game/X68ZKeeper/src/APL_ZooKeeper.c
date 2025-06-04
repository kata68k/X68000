#ifndef	APL_ZOOKEEEPER_C
#define	APL_ZOOKEEEPER_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <usr_macro.h>

#include "APL_ZooKeeper.h"
#include "IF_Input.h"
#include "IF_Math.h"
#include "IF_PCG.h"
#include "APL_PCG.h"
#include "APL_Score.h"

#define SIZE (8)
#define TYPES (10)  // 動物の種類（1?TYPES）
#define BASE_LEVEL  (5)
#define MAX_LEVEL  (10)
#define CLEAR_JDG  (10)

int32_t board[SIZE][SIZE];
int32_t to_clear[SIZE][SIZE];
int32_t score = 0;
int32_t anime_x1, anime_y1, anime_x2, anime_y2; 
int32_t anime_count = 0;
int32_t anime_count2 = 0;
int8_t  s_bVertical = 0;
int8_t  animal_counter[TYPES];
int8_t  s_GameLevel = BASE_LEVEL;

/* 関数 */
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 盤面表示
void APL_ZKP_print_board() {
#if 0
    int32_t x;
    int32_t y;
    printf("盤面:\n");
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            printf("%d ", board[y][x]);
        }
        printf("\n");
    }
#else
    APL_ZKP_SP_view();
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// アニマルカウンターのクリア
void APL_ZKP_init_animal_counter() {
    memset(animal_counter, 0, sizeof(animal_counter));
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// ゲームレベル設定
void APL_ZKP_set_GameLv(int8_t bLevel) {
    s_GameLevel = Mmax(BASE_LEVEL, Mmin(MAX_LEVEL, BASE_LEVEL + bLevel));
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// ゲームクリア条件確認
int32_t APL_ZKP_stage_clear_chk() {
    int32_t i;
    int32_t ave = 0;
    int32_t ret = 0;

    if(S_Get_ScoreMode() == FALSE)return ret;  /* 点数計算無効化状態 */

    for(i=0; i < s_GameLevel; i++)
    {
        ave += animal_counter[i];
    }

    if((ave / s_GameLevel) >= CLEAR_JDG)
    {
        ret = 1;
    }
    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 消去マーク表示
void APL_ZKP_print_clear_map() {
#if 0
    int32_t x;
    int32_t y;
    printf("消去マーク:\n");
	printf("\033[1A");	/* $1B ESC[1A 1行上に移動 */
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            printf("%d ", to_clear[y][x]);
        }
        printf("\n");
    }
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// ランダム初期化（同時に最初から消える状態を避けるのが本来だが簡略化）
void APL_ZKP_init_board() {
    int32_t x;
    int32_t y;
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            board[y][x] = rand() % s_GameLevel + 1;  // 1?TYPESのランダム値
        }
    }
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// マッチ判定（3つ以上の縦横同一）
void APL_ZKP_check_matches() {
    int32_t x;
    int32_t y;
    int32_t k, v;
    // クリアマップ初期化
    for (y = 0; y < SIZE; y++)
        for (x = 0; x < SIZE; x++)
            to_clear[y][x] = 0;

    // 横チェック
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE - 2; x++) {
            v = board[y][x];
            if (v != 0 && v == board[y][x+1] && v == board[y][x+2]) {
                k = x;
                while (k < SIZE && board[y][k] == v) {
                    to_clear[y][k] = 1;
                    k++;
                }
            }
        }
    }

    // 縦チェック
    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE - 2; y++) {
            v = board[y][x];
            if (v != 0 && v == board[y+1][x] && v == board[y+2][x]) {
                k = y;
                while (k < SIZE && board[k][x] == v) {
                    to_clear[k][x] = 1;
                    k++;
                }
            }
        }
    }
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// チェック
int32_t APL_ZKP_check_matches_chk(){
    int32_t ret = 0;
    int32_t x;
    int32_t y;
    int32_t found = 0;

    // マッチがあるか判定
    found = 0;
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            if (to_clear[y][x]) {
                found = 1;
                break;   // for終了
            }
        }
        if (found) break;   // for終了
    }

    if (!found){
        ret = 0;  // マッチなし → do while終了
    }
    else{
        ret = 1;
    }

    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 消去処理（to_clearが1の場所を0に）
void APL_ZKP_apply_clear() {
    int32_t x;
    int32_t y;

    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            if (to_clear[y][x]) {
                board[y][x] = 0;
                to_clear[y][x] = 0;
            }
        }
    }
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 消去処理
void APL_ZKP_ALL_clear() {
    int32_t x;
    int32_t y;

    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            board[y][x] = 0;
            to_clear[y][x] = 0;
        }
    }
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 消去処理（to_clearが1の場所を0に）(ステップ実行)
int32_t APL_ZKP_apply_clear_step() {
    int32_t x;
    int32_t y;
    int32_t ret = 0;

    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            if (to_clear[y][x]) {
                if(board[y][x] == 0)
                {
                    /* 暫定 */
                }
                else
                {
                    if(animal_counter[board[y][x] - 1] < 100)
                    {
                        animal_counter[board[y][x] - 1]++;  /* 消えたナンバーを加算 */
                    }
                    // 10点 × 連鎖ボーナス（整数で近似）
                    S_Add_Score(10);
                }
                board[y][x] = 0;
                to_clear[y][x] = 0;
                ret = 1;
                break;
            }
        }
    }
    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 落下処理：上から順に見て、空きがあればその上を1マスずつ落とす
void APL_ZKP_apply_gravity() {
    int32_t x;
    int32_t y;
    int32_t k;

    for (x = 0; x < SIZE; x++) {
        for (y = SIZE - 1; y > 0; y--) {
            if (board[y][x] == 0) {
                // 上の行を探して、非0を持ってくる
                k = y - 1;
                while (k >= 0 && board[k][x] == 0) {
                    k--;
                }
                if (k >= 0) {
                    board[y][x] = board[k][x];
                    board[k][x] = 0;
                }
            }
        }
    }
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 落下処理：上から順に見て、空きがあればその上を1マスずつ落とす(ステップ実行)
int32_t APL_ZKP_apply_gravity_step() {
    int32_t x;
    int32_t y;
    int32_t k;
    int32_t ret = 0;

    for (x = 0; x < SIZE; x++) {
        for (y = SIZE - 1; y > 0; y--) {
            if (board[y][x] == 0) {
                // 上の行を探して、非0を持ってくる
                k = y - 1;
                while (k >= 0 && board[k][x] == 0) {
                    k--;
                }
                if (k >= 0) {
                    board[y][x] = board[k][x];
                    board[k][x] = 0;
                    ret = 1;
                    break;
                }
            }
        }
    }
    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 新しいピースを補充（0になっている場所をランダムな値で埋める）
void APL_ZKP_refill_board() {
    int32_t x;
    int32_t y;
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            if (board[y][x] == 0) {
                board[y][x] = rand() % s_GameLevel + 1;
            }
        }
    }
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 新しいピースを補充（0になっている場所をランダムな値で埋める）(ステップ実行)
int32_t APL_ZKP_refill_board_step() {
    int32_t x;
    int32_t y;
    int32_t ret = 0;

    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            if (board[y][x] == 0) {
                board[y][x] = rand() % s_GameLevel + 1;
                ret = 1;
                break;
            }
        }
    }
    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 隣接しているかを判定
int32_t APL_ZKP_is_adjacent(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
    return (abs(y1 - y2) + abs(x1 - x2)) == 1;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 一時的にスワップ
void APL_ZKP_swap(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
    int32_t temp = board[y1][x1];
    board[y1][x1] = board[y2][x2];
    board[y2][x2] = temp;

    anime_x1 = x1;
    anime_y1 = y1;
    anime_x2 = x2;
    anime_y2 = y2;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// スワップ＋有効かチェック（無効なら戻す）
int32_t APL_ZKP_swap_if_valid(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
    int32_t x;
    int32_t y;
    int32_t valid;
    int32_t ret = 0;

    if (!APL_ZKP_is_adjacent(y1, x1, y2, x2)) {
#if 0
        printf("隣接していません！\n");
   		printf("\033[1A");	/* $1B ESC[1A 1行上に移動 */
#endif
        return ret;
    }

    // スワップしてみる
    APL_ZKP_swap(y1, x1, y2, x2);
    APL_ZKP_check_matches();

    // マッチがあれば有効、なければ戻す
    valid = 0;
    for (y = 0; y < SIZE; y++)
        for (x = 0; x < SIZE; x++)
            if (to_clear[y][x]) valid = 1;

    if (!valid) {
        // マッチしないので元に戻す
        APL_ZKP_swap(y1, x1, y2, x2);
#if 0
        printf("無効なスワップ。戻しました。\n");
   		printf("\033[1A");	/* $1B ESC[1A 1行上に移動 */
#endif
        ret = 0;
    }
    else{
        ret = 1;
    }
#if 0
    printf("有効なスワップ！\n");
	printf("\033[1A");	/* $1B ESC[1A 1行上に移動 */
#endif

    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 強制スワップ
int32_t APL_ZKP_swap_force(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
    int32_t ret = 0;

    if (!APL_ZKP_is_adjacent(y1, x1, y2, x2)) { /* 隣接していません！ */
        return ret;
    }

    // スワップしてみる
    APL_ZKP_swap(y1, x1, y2, x2);

    ret = 1;

    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 連鎖処理：マッチが続く限り繰り返す
int32_t APL_ZKP_process_all_matches() {
    int32_t chain_count = 0;
    int32_t ret = 0;

    do{
        APL_ZKP_home_board();

        APL_ZKP_check_matches();

        if(APL_ZKP_check_matches_chk() != 0u)
        {
            chain_count++;
            
    //      APL_ZKP_print_clear_map();

            APL_ZKP_apply_clear();  // 連鎖数を倍率に使う

            APL_ZKP_apply_gravity();

            APL_ZKP_refill_board();

            APL_ZKP_print_board();

            if(chain_count >= 2)
            {
                //連鎖
            }
            ret = 1;
        }
        else{
            ret = 0;
            break;
        }
    }while (1);

    if (chain_count > 0) {
    } else {
    }

    return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// カーソルをホーム位置に移動
void APL_ZKP_home_board() {
#if 0
    int32_t y;
    for (y = 0; y < SIZE; y++) {
        printf("\033[1A");	/* $1B ESC[1A 1行上に移動 */
    }
    printf("\033[1A");	/* $1B ESC[1A 1行上に移動 */
#endif
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// スワップ → マッチ確認 → 元に戻す
int32_t APL_ZKP_has_valid_moves() {
    int32_t x, y;
    int32_t i, j;

    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            // 右隣とスワップ
            if (x + 1 < SIZE) {
                APL_ZKP_swap(y, x, y, x + 1);
                APL_ZKP_check_matches();

                for ( j = 0; j < SIZE; j++)
                    for ( i = 0; i < SIZE; i++)
                        if (to_clear[j][i]) {
                            APL_ZKP_swap(y, x, y, x + 1); // 元に戻す
                            return 1; // 有効な手あり
                        }

                APL_ZKP_swap(y, x, y, x + 1); // 元に戻す
            }

            // 下隣とスワップ
            if (y + 1 < SIZE) {
                APL_ZKP_swap(y, x, y + 1, x);
                APL_ZKP_check_matches();

                for (j = 0; j < SIZE; j++)
                    for (i = 0; i < SIZE; i++)
                        if (to_clear[j][i]) {
                            APL_ZKP_swap(y, x, y + 1, x); // 元に戻す
                            return 1; // 有効な手あり
                        }

                APL_ZKP_swap(y, x, y + 1, x); // 元に戻す
            }
        }
    }

    return 0; // 有効な手なし
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// 有効手の表示
void APL_ZKP_print_valid_moves() {
    int32_t x, y;
    int32_t i, j;
    int32_t found = 0;
   	ST_PCG	*p_stPCG = NULL;

    anime_count2+=45;
    if(anime_count2 >= 360)
    {
        anime_count2 = 0;
    }

    for ( y = 0; y < SIZE; y++) {
        for ( x = 0; x < SIZE; x++) {

            // 右隣とスワップ
            if (x + 1 < SIZE) {
                APL_ZKP_swap(y, x, y, x + 1);
                APL_ZKP_check_matches();

                for ( j = 0; j < SIZE; j++) {
                    for ( i = 0; i < SIZE; i++) {
                        if (to_clear[j][i]) {
                            p_stPCG = PCG_Get_Info(SP_Number_1 + (y * SIZE) + x);	/* SP */
                            p_stPCG->x = TO_FIXED_POINT((64 + (x * 16)) + Mdiv256(APL_Sin(anime_count2) * 1));
                            p_stPCG->y = TO_FIXED_POINT(64 + (y * 16));
                            p_stPCG = PCG_Get_Info(SP_Number_1 + (y * SIZE) + (x+1));	/* SP */
                            p_stPCG->x = TO_FIXED_POINT((64 + ((x+1) * 16)) + Mdiv256(APL_Sin(anime_count2) * 1));
                            p_stPCG->y = TO_FIXED_POINT(64 + (y * 16));
                            //printf("? ヒント: [%d,%d] <-> [%d,%d]\n", y, x, y, x + 1);
                            found = 1;
                            break;
                        }
                    }
                    if (found) break;
                }

                APL_ZKP_swap(y, x, y, x + 1); // 戻す
            }

            // 下隣とスワップ
            if (y + 1 < SIZE) {
                APL_ZKP_swap(y, x, y + 1, x);
                APL_ZKP_check_matches();

                for ( j = 0; j < SIZE; j++) {
                    for ( i = 0; i < SIZE; i++) {
                        if (to_clear[j][i]) {
                            p_stPCG = PCG_Get_Info(SP_Number_1 + (y * SIZE) + x);	/* SP */
                            p_stPCG->x = TO_FIXED_POINT(64 + (x * 16));
                            p_stPCG->y = TO_FIXED_POINT((64 + (y * 16)) + Mdiv256(APL_Sin(anime_count2) * 1));
                            p_stPCG = PCG_Get_Info(SP_Number_1 + ((y+1) * SIZE) + x);	/* SP */
                            p_stPCG->x = TO_FIXED_POINT(64 + (x * 16));
                            p_stPCG->y = TO_FIXED_POINT((64 + ((y+1) * 16)) + Mdiv256(APL_Sin(anime_count2) * 1));
                            //printf("? ヒント: [%d,%d] <-> [%d,%d]\n", y, x, y + 1, x);
                            found = 1;
                            break;
                        }
                    }
                    if (found) break;
                }

                APL_ZKP_swap(y, x, y + 1, x); // 戻す
            }

//          if (found) return; // 最初の1つだけ表示（複数見せたい場合はcontinue）
            if (found) continue; // 最初の1つだけ表示（複数見せたい場合はcontinue）
        }
    }

    if (!found) {
        //printf("?? 有効な手が見つかりません。\n");
    }

}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
void APL_ZKP_SP_init( void )
{
	int16_t	i;
	ST_PCG	*p_stPCG = NULL;
	for(i = SP_CURSOR1; i <= SP_CURSOR2; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* SP */

		if(p_stPCG != NULL)
		{
			p_stPCG->x = TO_FIXED_POINT(128);
			p_stPCG->y = TO_FIXED_POINT(128);
			p_stPCG->dx = 0;
			p_stPCG->dy = 0;
			p_stPCG->Anime = 0;
			p_stPCG->angle = 0;
			p_stPCG->validty = TRUE;
			p_stPCG->update	= FALSE;

			PCG_PRI_CHG(p_stPCG, PCG_PRI_HIGH);	/* プライオリティ */
		}
	}

	for(i = SP_Number_1; i <= SP_Number_64; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* SP */

		if(p_stPCG != NULL)
		{
			p_stPCG->x = TO_FIXED_POINT(64 + (i % 16) * 16);
			p_stPCG->y = TO_FIXED_POINT(64 + (i / 16) * 16);
			p_stPCG->dx = 0;
			p_stPCG->dy = 0;
			p_stPCG->Anime = 0;
			p_stPCG->angle = 0;
			p_stPCG->validty = TRUE;
			p_stPCG->update	= FALSE;

			PCG_PRI_CHG(p_stPCG, PCG_PRI_MID);	/* プライオリティ */
		}
	}

	for(i = SP_Count_1; i <= SP_Count_10; i++)
	{
		p_stPCG = PCG_Get_Info(i);	/* SP */

		if(p_stPCG != NULL)
		{
			p_stPCG->x = TO_FIXED_POINT(32 + (i * 16));
			p_stPCG->y = TO_FIXED_POINT(16);
			p_stPCG->dx = 0;
			p_stPCG->dy = 0;
            p_stPCG->Anime = i - SP_Count_1;
			p_stPCG->angle = 0;
			p_stPCG->validty = TRUE;
			p_stPCG->update	= TRUE;

			PCG_PRI_CHG(p_stPCG, PCG_PRI_MID);	/* プライオリティ */

		}
	}
    
#if 0   /* テスト用 */
    PCG_COL_SHIFT_ZERO(4, 8);
    PCG_COL_SHIFT_ZERO(5, 9);
    PCG_COL_SHIFT_ZERO(6, 10);
    PCG_COL_SHIFT_ZERO(7, 11);
    PCG_COL_SHIFT_ZERO(8, 12);
    PCG_COL_SHIFT_ZERO(9, 13);
    PCG_COL_SHIFT_ZERO(10, 14);
    PCG_COL_SHIFT_ZERO(11, 15);
    PCG_COL_SHIFT_ZERO(12, 8);
    PCG_COL_SHIFT_ZERO(13, 9);
#endif
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// パネルを表示
void APL_ZKP_SP_view() {
	int8_t	sBuf[8];
    int32_t x;
    int32_t y;
    int32_t i;
   	ST_PCG	*p_stPCG = NULL;
    
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
    		p_stPCG = PCG_Get_Info(SP_Number_1 + (y * SIZE) + x);	/* SP */
			p_stPCG->x = TO_FIXED_POINT(64 + (x * 16));
    		p_stPCG->y = TO_FIXED_POINT(64 + (y * 16));
            if(board[y][x] == 0)
            {
    			p_stPCG->update	= FALSE;
            }
            else
            {
                p_stPCG->Anime = board[y][x] - 1;
    			p_stPCG->update	= TRUE;
            }
#if 0   /* テスト用 */
            PCG_PAL_CHG( p_stPCG, 4 + board[y][x]);
#endif
        }
    }

    for(i = SP_Count_1; i <= SP_Count_10; i++)  /* スコアパネル */
	{
		p_stPCG = PCG_Get_Info(i);	/* SP */

		if(p_stPCG != NULL)
		{
            p_stPCG->Anime = i - SP_Count_1;
			p_stPCG->update	= TRUE;
		}
        memset(sBuf, 0, sizeof(sBuf));
        sprintf(sBuf, "%02d", animal_counter[i - SP_Count_1]);
        BG_put_String(32 + (i * 16), 32, BG_PAL, sBuf, strlen(sBuf));
	}

}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// スワップアニメを表示
int32_t APL_ZKP_SP_Swap_view(int8_t bHit) {
    int32_t ret = 0;
    int32_t x;
    int32_t y;
    int32_t nCountMax = 90;
   	ST_PCG	*p_stPCG = NULL;

    if(bHit == FALSE)
    {
        nCountMax = 180;
    }

    x = anime_x1;
    y = anime_y1;

    p_stPCG = PCG_Get_Info(SP_Number_1 + (y * SIZE) + x);	/* SP */

    if(bHit == FALSE)
    {
        if(s_bVertical == 0)
        {
            p_stPCG->x = TO_FIXED_POINT((64 + (x * 16)) + Mdiv256(APL_Sin(anime_count) * SP_W));
        }
        else
        {
            p_stPCG->y = TO_FIXED_POINT((64 + (y * 16)) + Mdiv256(APL_Sin(anime_count) * SP_H));
        }
    }
    else
    {
        x = anime_x2;
        y = anime_y2;
        if(s_bVertical == 0)
        {
            p_stPCG->x = TO_FIXED_POINT((64 + (x * 16)) - Mdiv256(APL_Sin(anime_count) * SP_W));
        }
        else
        {
            p_stPCG->y = TO_FIXED_POINT((64 + (y * 16)) - Mdiv256(APL_Sin(anime_count) * SP_H));
        }
    }

    p_stPCG->update	= TRUE;

    x = anime_x2;
    y = anime_y2;

    p_stPCG = PCG_Get_Info(SP_Number_1 + (y * SIZE) + x);	/* SP */

    if(bHit == FALSE)
    {
        if(s_bVertical == 0)
        {
            p_stPCG->x = TO_FIXED_POINT((64 + (x * 16)) - Mdiv256(APL_Sin(anime_count) * SP_W));
        }
        else
        {
            p_stPCG->y = TO_FIXED_POINT((64 + (y * 16)) - Mdiv256(APL_Sin(anime_count) * SP_H));
        }
    }
    else
    {
        x = anime_x1;
        y = anime_y1;
        if(s_bVertical == 0)
        {
            p_stPCG->x = TO_FIXED_POINT((64 + (x * 16)) + Mdiv256(APL_Sin(anime_count) * SP_W));
        }
        else
        {
            p_stPCG->y = TO_FIXED_POINT((64 + (y * 16)) + Mdiv256(APL_Sin(anime_count) * SP_H));
        }
    }

    p_stPCG->update	= TRUE;
    
    anime_count+=5;
    if(anime_count >= nCountMax)
    {
        anime_count = 0;
        ret = 1;
    }

    return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// カーソルを表示
void APL_ZKP_SP_cursor_view(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
   	ST_PCG	*p_stPCG = NULL;
    
    p_stPCG = PCG_Get_Info(SP_CURSOR1);	/* SP */
    p_stPCG->x = TO_FIXED_POINT(64 + (x1 * 16) );
    p_stPCG->y = TO_FIXED_POINT(64 + (y1 * 16) );
    p_stPCG->Anime = 0;
    p_stPCG->update	= TRUE;

    p_stPCG = PCG_Get_Info(SP_CURSOR2);	/* SP */
    p_stPCG->x = TO_FIXED_POINT(64 + (x2 * 16) );
    p_stPCG->y = TO_FIXED_POINT(64 + (y2 * 16) );
    p_stPCG->Anime = 0;
    p_stPCG->update	= TRUE;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// カーソルを表示
void APL_ZKP_SP_cursor_view_off() {
   	ST_PCG	*p_stPCG = NULL;
    
    p_stPCG = PCG_Get_Info(SP_CURSOR1);	/* SP */
    p_stPCG->update	= FALSE;

    p_stPCG = PCG_Get_Info(SP_CURSOR2);	/* SP */
    p_stPCG->update	= FALSE;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
// パネルを表示
int32_t APL_ZKP_KEY_Input(int32_t *y1, int32_t *x1, int32_t *y2, int32_t *x2) {
    static int8_t s_b_UP;
    static int8_t s_b_DOWN;
    static int8_t s_b_RIGHT;
    static int8_t s_b_LEFT;
    static int8_t s_b_A;
    static int8_t s_b_B;
    int32_t lx1,ly1,lx2,ly2;
    int32_t ret = 0;

    lx1 = *x1;
    ly1 = *y1;
    lx2 = *x2;
    ly2 = *y2;

    if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
        ((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
        ((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
    {
        if(s_b_A == FALSE)
        {
            s_b_A = TRUE;
            ret = 1;
        }
        else
        {
        }
    }
    else
    {
        s_b_A = FALSE;
    }

    if(	((GetInput_P1() & JOY_B ) != 0u)	||		/* B */
        ((GetInput() & KEY_b_X) != 0u)	)			/* B(z) */
    {
        if(s_b_B == FALSE)
        {
            s_b_B = TRUE;
            s_bVertical = !(s_bVertical);	/* 反転 */

            if(s_bVertical == 0)
            {
                lx2 = lx1 + 1;
                ly2 = ly1;
                if(lx1 == 7){
                    lx1 = 6;
                    lx2 = 7;
                }
            }
            else
            {
                lx2 = lx1;
                ly2 = ly1 + 1;
                if(ly1 == 7){
                    ly1 = 6;
                    ly2 = 7;
                }
            }
        }
    }
    else
    {
        s_b_B = FALSE;
    }

    if(	((GetInput_P1() & JOY_LEFT ) != 0u )	||	/* LEFT */
        ((GetInput() & KEY_LEFT) != 0 )		)	/* 左 */
    {
        if(s_b_LEFT == FALSE)
        {
            s_b_LEFT = TRUE;
            if(s_bVertical == 0)
            {
                if(lx1 == 0){
                }
                else{
                    lx1--;
                }
                if(lx2 == 1){
                }
                else{
                    lx2--;
                }
            }
            else
            {
                if(lx1 == 0){
                }
                else{
                    lx1--;
                }
                if(lx2 == 0){
                }
                else{
                    lx2--;
                }
            }
        }
    }
    else if( ((GetInput_P1() & JOY_RIGHT ) != 0u )	||	/* RIGHT */
            ((GetInput() & KEY_RIGHT) != 0 )			)	/* 右 */
    {
        if(s_b_RIGHT == FALSE)
        {
            s_b_RIGHT = TRUE;
            if(s_bVertical == 0)
            {
                if(lx1 >= 6){
                }
                else{
                    lx1++;
                }
                if(lx2 >= 7){
                }
                else{
                    lx2++;
                }
            }
            else
            {
                if(lx1 >= 7){
                }
                else{
                    lx1++;
                }
                if(lx2 >= 7){
                }
                else{
                    lx2++;
                }
            }
        }
    }
    else
    {
        s_b_LEFT = FALSE;
        s_b_RIGHT = FALSE;
    }

    if(	((GetInput_P1() & JOY_UP ) != 0u )	||	/* UP */
        ((GetInput() & KEY_UPPER) != 0 )	)	/* 上 */
    {
        if(s_b_UP == FALSE)
        {
            s_b_UP = TRUE;
            if(s_bVertical == 0)
            {
                if(ly1 == 0){
                }
                else{
                    ly1--;
                }
                if(ly2 == 0){
                }
                else{
                    ly2--;
                }
            }
            else
            {
                if(ly1 == 0){
                }
                else{
                    ly1--;
                }
                if(ly2 <= 1){
                }
                else{
                    ly2--;
                }
            }
        }
    }
	else if(((GetInput_P1() & JOY_DOWN ) != 0u )	||	/* UP */
       		((GetInput() & KEY_LOWER) != 0 )	)	    /* 下 */
    {
        if(s_b_DOWN == FALSE)
        {
            s_b_DOWN = TRUE;
            if(s_bVertical == 0)
            {
                if(ly1 >= 7){
                }
                else{
                    ly1++;
                }
                if(ly2 >= 7){
                }
                else{
                    ly2++;
                }
            }
            else
            {
                if(ly1 >= 6){
                }
                else{
                    ly1++;
                }
                if(ly2 >= 7){
                }
                else{
                    ly2++;
                }
            }
        }
    }
    else
    {
        s_b_UP = FALSE;
        s_b_DOWN = FALSE;
    }

    *x1 = lx1;
    *y1 = ly1;
    *x2 = lx2;
    *y2 = ly2;

    return ret;
}


#endif	/* APL_ZOOKEEEPER_C */
