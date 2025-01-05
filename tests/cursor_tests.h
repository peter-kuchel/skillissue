#ifndef SKILLISSUE_TESTS_CURSOR_H
#define SKILLISSUE_TESTS_CURSOR_H

#include <stdio.h>
#include "../src/cursor.h"
#include "test_utils.h"


int sim_pressing_W(piece_table *pt, cursor_pos *pos, line_view *lv);
int sim_pressing_A(piece_table *pt, cursor_pos *pos, line_view *lv);
int sim_pressing_S(piece_table *pt, cursor_pos *pos, line_view *lv);
int sim_pressing_D(piece_table *pt, cursor_pos *pos, line_view *lv);

#endif
