#ifndef PT_REALLOC_SKILLISSUE_H
#define PT_REALLOC_SKILLISSUE_H

#include <stdio.h> 
#include "piecetable.h"
#include "utils.h"

#define PT_INC_CAP(cap) (cap << 1)

#define PT_REALLOC(type, ptr, new_cap) \
( (type*)handle_realloc(ptr, sizeof(type) * new_cap) )

void* handle_realloc(void* ptr, size_t new_cap);

void check_entries_size(piece_table* pt);
void check_additions_size(add_buffer_t* adds);
void check_pt_stack_size(pt_stack_t* stack);
int new_pt_entry(piece_table* pt);

#endif