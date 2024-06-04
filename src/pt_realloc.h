#ifndef PT_REALLOC_SKILLISSUE_H
#define PT_REALLOC_SKILLISSUE_H

#include <stdio.h> 
#include "piecetable.h"
// #include "utils.h"

void check_entries_size(piece_table* pt);
void check_additions_size(add_buffer_t* adds);
void check_pt_stack_size(pt_stack_t* stack);
void check_line_handler_size(line_handler* lh);
int get_new_line(line_handler* lh);
int new_pt_entry(piece_table* pt);

#endif