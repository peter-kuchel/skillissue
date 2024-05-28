#include "pt_realloc.h"


void* handle_realloc(void* ptr, size_t new_cap){
    void* result = realloc(ptr, new_cap); 

    // allocation can fail if there isn't enough memory to realloc 
    if (result == NULL) exit(-1);

    return result;
}

void check_organizer_size(piece_table* pt){
    pt_table_t* table = &(pt->table);
    int curr_org_ptr = pt->curr_chr_ptr;

    if (
        table->org_num + 1 == table->org_cap || 
        (curr_org_ptr == 0 || curr_org_ptr == table->org_cap - 1)
    ){

        int new_cap = PT_INC_CAP(table->org_cap);
        int* new_ptr = PT_REALLOC(int, table->organizer, new_cap);

        table->organizer = new_ptr;
        table->org_cap = new_cap;

        // recalc middle to potentially shift 
        int old_midd = (table->org_tail - table->org_head) >> 1;
        int midd_shift = (new_cap >> 1) - old_midd; 

        // shift everything right by midd_shift if too left skewed
        if (midd_shift > 0){

            for (int i = table->org_tail ; i  >= table->org_head ; i--){
                int ent_i = table->organizer[i];
                pt->table.organizer[i + midd_shift] = ent_i;
            }

            // update head and tail 
            table->org_head += midd_shift; 
            table->org_tail += midd_shift; 
        }
    }
}

void check_entries_size(piece_table* pt){
    if (pt->table.ent_num + 1 == pt->table.ent_cap){

        int new_cap = PT_INC_CAP(pt->table.ent_cap); 
        pt_entry* new_ptr = PT_REALLOC(pt_entry, pt->table.entries, new_cap); 

        pt->table.entries = new_ptr; 
        pt->table.ent_cap = new_cap;
        
    }
}

void check_additions_size(add_buffer_t* adds){
    if (adds->curr_pos + 1 == adds->buf.size){

        int new_size = PT_INC_CAP(adds->buf.size); 
        char* new_text_buf = PT_REALLOC(char, adds->buf.text, new_size); 

        adds->buf.text = new_text_buf; 
        adds->buf.size = new_size; 
    }
}

void check_pt_stack_size(pt_stack_t* pt_stack){
    if (pt_stack->ptr + 1 == pt_stack->cap){

        int new_cap = PT_INC_CAP(pt_stack->cap); 
        int* new_stack = PT_REALLOC(int, pt_stack->stack, new_cap);

        pt_stack->stack = new_stack;
        pt_stack->cap  = new_cap;
    }
}