#include "pt_realloc.h"


void* handle_realloc(void* ptr, size_t new_cap){
    void* result = realloc(ptr, new_cap); 

    // allocation can fail if there isn't enough memory to realloc 
    if (result == NULL) exit(-1);

    return result;
}

void check_organizer_size(piece_table* pt){
    pt_table_t* table = &(pt->table);
    int curr_org_ptr = pt->curr_org_ptr;

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

int new_pt_insert_entry(piece_table* pt){
    // check if entries need to be reallocd 
    check_entries_size(pt);

    pt->table.ent_num++; 
    int new_ent_pos = pt->table.ent_num; 
    
    pt_entry* new_single_entry = ENT_AT_POS_ENTRIES(pt, new_ent_pos);
    new_single_entry->src = ADD;
    new_single_entry->start = pt->addition.curr_pos; 
    new_single_entry->len = 0; 

    return new_ent_pos; 
}

int shift_organizer_right(piece_table* pt, int start, int end){

    #ifdef DEBUG_MOVEMENT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Shift Right Details]: start: %d, end: %d\n",start, end);
        log_to_file(&sk_logger, pbuf);
    #endif 

    // check that a shift does not go outside of the bounds of the organizer
    // by making sure the organizer is big enough 
    // check_organizer_size(pt);

    for (int i = start; i >= end; i--){
        int ent_i = pt->table.organizer[i];
        pt->table.organizer[i + 1] = ent_i;
    }

    pt->table.org_tail++; 

    return 0; 
 
}

int shift_organizer_left(piece_table* pt, int start, int end){
    
    #ifdef DEBUG_MOVEMENT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Shift Left Details]: start: %d, end: %d\n",start, end);
        log_to_file(&sk_logger, pbuf);
    #endif 

    // check that a shift does not go outside of the bounds of the organizer
    // check_organizer_size(pt);

    for (int i = start; i <= end; i++){
        int ent_i = pt->table.organizer[i];
        pt->table.organizer[i - 1] = ent_i;
    }

    pt->table.org_head--; 

    return 0; 
}