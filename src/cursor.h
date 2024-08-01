#ifndef CURSOR_SKILLISSUE_H
#define CURSOR_SKILLISSUE_H

#include "piecetable.h"
#include "screen.h"
#include "utils.h"

int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir);
int handle_line_movement(piece_table* pt, cursor_pos* pos, int dir, line_view* lv);

int handle_jump_down(piece_table* pt, cursor_pos* pos, int prev_chr_ptr, int prev_ent, line_view* lv);
int handle_jump_up(piece_table* pt, cursor_pos* pos, line_view* lv);

#endif 