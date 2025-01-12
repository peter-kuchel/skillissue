#ifndef SKILLISSUE_TESTS_INSERTION_MODE_H
#define SKILLISSUE_TESTS_INSERTION_MODE_H

#include <stdio.h>
#include "../src/screen.h"
#include "../src/cursor.h"
#include "../src/pt_realloc.h"
#include "cursor_tests.h"
#include "test_utils.h"

int all_insertionmode_tests(piece_table *pt, line_view *lv, cursor_pos *pos);

#endif