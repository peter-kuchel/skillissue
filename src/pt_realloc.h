#ifndef PT_REALLOC_SKILLISSUE_H
#define PT_REALLOC_SKILLISSUE_H

#include <stdio.h> 
#include "piecetable.h"
// #include "utils.h"

#define INC_CAP(cap) (cap << 1)

#define REALLOC(type, ptr, new_cap) \
    ( (type*)handle_realloc(ptr, sizeof(type) * new_cap) )

void* handle_realloc(void* ptr, size_t new_cap);

void check_entries_size(piece_table* pt);
void check_additions_size(add_buffer_t* adds);
void check_pt_stack_size(pt_stack_t* stack);
void check_line_handler_size(line_handler* lh);

int new_pt_entry(piece_table* pt);

// int get_new_line(line_handler* lh);
int add_new_line(line_handler* lh);
int init_line_handler(line_handler* lh, pt_buffer_t* original_buffer);

#endif