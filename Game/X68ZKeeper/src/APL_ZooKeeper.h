#ifndef	APL_ZOOKEEEPER_H
#define	APL_ZOOKEEEPER_H

#include <usr_define.h>

extern void APL_ZKP_print_board();
extern void APL_ZKP_init_animal_counter();
extern void APL_ZKP_set_GameLv(int8_t);
extern int32_t APL_ZKP_stage_clear_chk();
extern void APL_ZKP_print_clear_map();
extern void APL_ZKP_init_board();
extern void APL_ZKP_check_matches();
extern int32_t APL_ZKP_check_matches_chk();
extern void APL_ZKP_apply_clear();
extern void APL_ZKP_ALL_clear();
extern int32_t APL_ZKP_apply_clear_step();
extern void APL_ZKP_apply_gravity();
extern int32_t APL_ZKP_apply_gravity_step();
extern void APL_ZKP_refill_board();
extern int32_t APL_ZKP_refill_board_step();
extern int32_t APL_ZKP_is_adjacent(int32_t, int32_t, int32_t, int32_t);
extern void APL_ZKP_swap(int32_t, int32_t, int32_t, int32_t);
extern int32_t APL_ZKP_swap_if_valid(int32_t, int32_t, int32_t, int32_t);
extern int32_t APL_ZKP_swap_force(int32_t, int32_t, int32_t, int32_t);
extern int32_t APL_ZKP_process_all_matches();
extern void APL_ZKP_home_board();
extern int32_t APL_ZKP_has_valid_moves();
extern void APL_ZKP_print_valid_moves();
extern void APL_ZKP_SP_init( void );
extern void APL_ZKP_SP_view();
extern int32_t APL_ZKP_SP_Swap_view(int8_t);
extern void APL_ZKP_SP_cursor_view(int32_t, int32_t, int32_t, int32_t);
extern void APL_ZKP_SP_cursor_view_off();
extern int32_t APL_ZKP_KEY_Input(int32_t *, int32_t *, int32_t *, int32_t *);

#endif	/* APL_ZOOKEEEPER_H */
