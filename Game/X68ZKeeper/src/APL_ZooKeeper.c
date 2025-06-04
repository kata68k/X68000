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
#define TYPES (10)  // �����̎�ށi1?TYPES�j
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

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �Ֆʕ\��
void APL_ZKP_print_board() {
#if 0
    int32_t x;
    int32_t y;
    printf("�Ֆ�:\n");
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �A�j�}���J�E���^�[�̃N���A
void APL_ZKP_init_animal_counter() {
    memset(animal_counter, 0, sizeof(animal_counter));
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �Q�[�����x���ݒ�
void APL_ZKP_set_GameLv(int8_t bLevel) {
    s_GameLevel = Mmax(BASE_LEVEL, Mmin(MAX_LEVEL, BASE_LEVEL + bLevel));
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �Q�[���N���A�����m�F
int32_t APL_ZKP_stage_clear_chk() {
    int32_t i;
    int32_t ave = 0;
    int32_t ret = 0;

    if(S_Get_ScoreMode() == FALSE)return ret;  /* �_���v�Z��������� */

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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �����}�[�N�\��
void APL_ZKP_print_clear_map() {
#if 0
    int32_t x;
    int32_t y;
    printf("�����}�[�N:\n");
	printf("\033[1A");	/* $1B ESC[1A 1�s��Ɉړ� */
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            printf("%d ", to_clear[y][x]);
        }
        printf("\n");
    }
#endif
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �����_���������i�����ɍŏ�����������Ԃ������̂��{�������ȗ����j
void APL_ZKP_init_board() {
    int32_t x;
    int32_t y;
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            board[y][x] = rand() % s_GameLevel + 1;  // 1?TYPES�̃����_���l
        }
    }
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �}�b�`����i3�ȏ�̏c������j
void APL_ZKP_check_matches() {
    int32_t x;
    int32_t y;
    int32_t k, v;
    // �N���A�}�b�v������
    for (y = 0; y < SIZE; y++)
        for (x = 0; x < SIZE; x++)
            to_clear[y][x] = 0;

    // ���`�F�b�N
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

    // �c�`�F�b�N
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �`�F�b�N
int32_t APL_ZKP_check_matches_chk(){
    int32_t ret = 0;
    int32_t x;
    int32_t y;
    int32_t found = 0;

    // �}�b�`�����邩����
    found = 0;
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            if (to_clear[y][x]) {
                found = 1;
                break;   // for�I��
            }
        }
        if (found) break;   // for�I��
    }

    if (!found){
        ret = 0;  // �}�b�`�Ȃ� �� do while�I��
    }
    else{
        ret = 1;
    }

    return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// ���������ito_clear��1�̏ꏊ��0�Ɂj
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// ��������
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// ���������ito_clear��1�̏ꏊ��0�Ɂj(�X�e�b�v���s)
int32_t APL_ZKP_apply_clear_step() {
    int32_t x;
    int32_t y;
    int32_t ret = 0;

    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            if (to_clear[y][x]) {
                if(board[y][x] == 0)
                {
                    /* �b�� */
                }
                else
                {
                    if(animal_counter[board[y][x] - 1] < 100)
                    {
                        animal_counter[board[y][x] - 1]++;  /* �������i���o�[�����Z */
                    }
                    // 10�_ �~ �A���{�[�i�X�i�����ŋߎ��j
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// ���������F�ォ�珇�Ɍ��āA�󂫂�����΂��̏��1�}�X�����Ƃ�
void APL_ZKP_apply_gravity() {
    int32_t x;
    int32_t y;
    int32_t k;

    for (x = 0; x < SIZE; x++) {
        for (y = SIZE - 1; y > 0; y--) {
            if (board[y][x] == 0) {
                // ��̍s��T���āA��0�������Ă���
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// ���������F�ォ�珇�Ɍ��āA�󂫂�����΂��̏��1�}�X�����Ƃ�(�X�e�b�v���s)
int32_t APL_ZKP_apply_gravity_step() {
    int32_t x;
    int32_t y;
    int32_t k;
    int32_t ret = 0;

    for (x = 0; x < SIZE; x++) {
        for (y = SIZE - 1; y > 0; y--) {
            if (board[y][x] == 0) {
                // ��̍s��T���āA��0�������Ă���
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �V�����s�[�X���[�i0�ɂȂ��Ă���ꏊ�������_���Ȓl�Ŗ��߂�j
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �V�����s�[�X���[�i0�ɂȂ��Ă���ꏊ�������_���Ȓl�Ŗ��߂�j(�X�e�b�v���s)
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �אڂ��Ă��邩�𔻒�
int32_t APL_ZKP_is_adjacent(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
    return (abs(y1 - y2) + abs(x1 - x2)) == 1;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �ꎞ�I�ɃX���b�v
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �X���b�v�{�L�����`�F�b�N�i�����Ȃ�߂��j
int32_t APL_ZKP_swap_if_valid(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
    int32_t x;
    int32_t y;
    int32_t valid;
    int32_t ret = 0;

    if (!APL_ZKP_is_adjacent(y1, x1, y2, x2)) {
#if 0
        printf("�אڂ��Ă��܂���I\n");
   		printf("\033[1A");	/* $1B ESC[1A 1�s��Ɉړ� */
#endif
        return ret;
    }

    // �X���b�v���Ă݂�
    APL_ZKP_swap(y1, x1, y2, x2);
    APL_ZKP_check_matches();

    // �}�b�`������ΗL���A�Ȃ���Ζ߂�
    valid = 0;
    for (y = 0; y < SIZE; y++)
        for (x = 0; x < SIZE; x++)
            if (to_clear[y][x]) valid = 1;

    if (!valid) {
        // �}�b�`���Ȃ��̂Ō��ɖ߂�
        APL_ZKP_swap(y1, x1, y2, x2);
#if 0
        printf("�����ȃX���b�v�B�߂��܂����B\n");
   		printf("\033[1A");	/* $1B ESC[1A 1�s��Ɉړ� */
#endif
        ret = 0;
    }
    else{
        ret = 1;
    }
#if 0
    printf("�L���ȃX���b�v�I\n");
	printf("\033[1A");	/* $1B ESC[1A 1�s��Ɉړ� */
#endif

    return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �����X���b�v
int32_t APL_ZKP_swap_force(int32_t y1, int32_t x1, int32_t y2, int32_t x2) {
    int32_t ret = 0;

    if (!APL_ZKP_is_adjacent(y1, x1, y2, x2)) { /* �אڂ��Ă��܂���I */
        return ret;
    }

    // �X���b�v���Ă݂�
    APL_ZKP_swap(y1, x1, y2, x2);

    ret = 1;

    return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �A�������F�}�b�`����������J��Ԃ�
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

            APL_ZKP_apply_clear();  // �A������{���Ɏg��

            APL_ZKP_apply_gravity();

            APL_ZKP_refill_board();

            APL_ZKP_print_board();

            if(chain_count >= 2)
            {
                //�A��
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �J�[�\�����z�[���ʒu�Ɉړ�
void APL_ZKP_home_board() {
#if 0
    int32_t y;
    for (y = 0; y < SIZE; y++) {
        printf("\033[1A");	/* $1B ESC[1A 1�s��Ɉړ� */
    }
    printf("\033[1A");	/* $1B ESC[1A 1�s��Ɉړ� */
#endif
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �X���b�v �� �}�b�`�m�F �� ���ɖ߂�
int32_t APL_ZKP_has_valid_moves() {
    int32_t x, y;
    int32_t i, j;

    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            // �E�ׂƃX���b�v
            if (x + 1 < SIZE) {
                APL_ZKP_swap(y, x, y, x + 1);
                APL_ZKP_check_matches();

                for ( j = 0; j < SIZE; j++)
                    for ( i = 0; i < SIZE; i++)
                        if (to_clear[j][i]) {
                            APL_ZKP_swap(y, x, y, x + 1); // ���ɖ߂�
                            return 1; // �L���Ȏ肠��
                        }

                APL_ZKP_swap(y, x, y, x + 1); // ���ɖ߂�
            }

            // ���ׂƃX���b�v
            if (y + 1 < SIZE) {
                APL_ZKP_swap(y, x, y + 1, x);
                APL_ZKP_check_matches();

                for (j = 0; j < SIZE; j++)
                    for (i = 0; i < SIZE; i++)
                        if (to_clear[j][i]) {
                            APL_ZKP_swap(y, x, y + 1, x); // ���ɖ߂�
                            return 1; // �L���Ȏ肠��
                        }

                APL_ZKP_swap(y, x, y + 1, x); // ���ɖ߂�
            }
        }
    }

    return 0; // �L���Ȏ�Ȃ�
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �L����̕\��
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

            // �E�ׂƃX���b�v
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
                            //printf("? �q���g: [%d,%d] <-> [%d,%d]\n", y, x, y, x + 1);
                            found = 1;
                            break;
                        }
                    }
                    if (found) break;
                }

                APL_ZKP_swap(y, x, y, x + 1); // �߂�
            }

            // ���ׂƃX���b�v
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
                            //printf("? �q���g: [%d,%d] <-> [%d,%d]\n", y, x, y + 1, x);
                            found = 1;
                            break;
                        }
                    }
                    if (found) break;
                }

                APL_ZKP_swap(y, x, y + 1, x); // �߂�
            }

//          if (found) return; // �ŏ���1�����\���i�������������ꍇ��continue�j
            if (found) continue; // �ŏ���1�����\���i�������������ꍇ��continue�j
        }
    }

    if (!found) {
        //printf("?? �L���Ȏ肪������܂���B\n");
    }

}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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

			PCG_PRI_CHG(p_stPCG, PCG_PRI_HIGH);	/* �v���C�I���e�B */
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

			PCG_PRI_CHG(p_stPCG, PCG_PRI_MID);	/* �v���C�I���e�B */
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

			PCG_PRI_CHG(p_stPCG, PCG_PRI_MID);	/* �v���C�I���e�B */

		}
	}
    
#if 0   /* �e�X�g�p */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �p�l����\��
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
#if 0   /* �e�X�g�p */
            PCG_PAL_CHG( p_stPCG, 4 + board[y][x]);
#endif
        }
    }

    for(i = SP_Count_1; i <= SP_Count_10; i++)  /* �X�R�A�p�l�� */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �X���b�v�A�j����\��
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �J�[�\����\��
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �J�[�\����\��
void APL_ZKP_SP_cursor_view_off() {
   	ST_PCG	*p_stPCG = NULL;
    
    p_stPCG = PCG_Get_Info(SP_CURSOR1);	/* SP */
    p_stPCG->update	= FALSE;

    p_stPCG = PCG_Get_Info(SP_CURSOR2);	/* SP */
    p_stPCG->update	= FALSE;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
// �p�l����\��
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
        ((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
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
            s_bVertical = !(s_bVertical);	/* ���] */

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
        ((GetInput() & KEY_LEFT) != 0 )		)	/* �� */
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
            ((GetInput() & KEY_RIGHT) != 0 )			)	/* �E */
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
        ((GetInput() & KEY_UPPER) != 0 )	)	/* �� */
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
       		((GetInput() & KEY_LOWER) != 0 )	)	    /* �� */
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
