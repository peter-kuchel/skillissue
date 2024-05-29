#ifndef PT_STACKS_SKILLISSUE_H
#define PT_STACKS_SKILLISSUE_H


#include "piecetable.h"
#include "pt_realloc.h"

void push_pt_stack(pt_stack_t* _stack, int entry);
int pop_pt_stack(pt_stack_t* _stack);

#endif 