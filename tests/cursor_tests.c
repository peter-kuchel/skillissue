#include "cursor_tests.h"


int sim_pressing_W(piece_table *pt, cursor_pos *pos, line_view *lv){
     handle_line_movement(pt, pos, -1, lv);
     return 0;
}

int sim_pressing_A(piece_table *pt, cursor_pos *pos, line_view *lv){
     handle_side_movement(pt, pos, -1, lv);
     handle_jump_up(pt, pos, lv);
     return 0;
}

int sim_pressing_S(piece_table *pt, cursor_pos *pos, line_view *lv){
     handle_line_movement(pt, pos, 1, lv);
     return 0;
}

int sim_pressing_D(piece_table *pt, cursor_pos *pos, line_view *lv){
     size_t prev_chr_ptr = pt->curr_chr_ptr;
     int prev_ent = pt->curr_ent_ptr;
     handle_side_movement(pt, pos, 1, lv);
     handle_jump_down(pt, pos, prev_chr_ptr, prev_ent, lv);
     return 0;

}

