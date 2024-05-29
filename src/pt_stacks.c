#include "pt_stacks.h"



void push_pt_stack(pt_stack_t* _stack, int entry_pos){
    check_pt_stack_size(_stack); 

    _stack->stack[_stack->ptr] = entry_pos; 
    _stack->ptr++;
}

int pop_pt_stack(pt_stack_t* _stack){
    int top_entry_pos = _stack->stack[_stack->ptr]; 
    _stack->ptr--; 

    return top_entry_pos; 
}