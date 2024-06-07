#include "pt_realloc.h"

void* handle_realloc(void* ptr, size_t new_cap){
    void* result = realloc(ptr, new_cap); 

    // allocation can fail if there isn't enough memory to realloc 
    if (result == NULL) exit(-1);

    return result;
}

void check_entries_size(piece_table* pt){
    if (pt->ent_num + 1 == pt->ent_cap){

        int new_cap = INC_CAP(pt->ent_cap); 
        pt_entry* new_ptr = REALLOC(pt_entry, pt->entries, new_cap); 

        pt->entries = new_ptr; 
        pt->ent_cap = new_cap;
        
    }
}

void check_additions_size(add_buffer_t* adds){
    if (adds->curr_pos + 1 == adds->buf.size){

        int new_size = INC_CAP(adds->buf.size); 
        char* new_text_buf = REALLOC(char, adds->buf.text, new_size); 

        adds->buf.text = new_text_buf; 
        adds->buf.size = new_size; 
    }
}

void check_pt_stack_size(pt_stack_t* pt_stack){
    if (pt_stack->ptr + 1 == pt_stack->cap){

        int new_cap = INC_CAP(pt_stack->cap); 
        int* new_stack = REALLOC(int, pt_stack->stack, new_cap);

        pt_stack->stack = new_stack;
        pt_stack->cap  = new_cap;
    }
}

void check_line_handler_size(line_handler* lh){
    if (lh->size + 1 == lh->cap){

        int new_cap = INC_CAP(lh->cap); 
        line* new_lines = REALLOC(line, lh->lines, new_cap);

        lh->lines = new_lines;
        lh->cap  = new_cap;
    }
}

static int get_new_line(line_handler* lh){
    check_line_handler_size(lh);
    
    int new_line = lh->size;
    lh->size++; 
     

    line* _line = &(lh->lines[new_line]);
    _line->line_size = 0;
    _line->prev_line = NULL_LINE; 
    _line->next_line = NULL_LINE; 

   return new_line; 
}

int add_new_line(line_handler* lh, int dir){

    // needs to handle cases for head and tail 
    // needs to handle cases where there is less than 3 lines already 
    line *new_line, *_curr_line, *neighbour; 

    int new_line_num = get_new_line(lh);

    _curr_line = &(lh->lines[lh->curr_line]);
    new_line = &(lh->lines[new_line_num]);

    if (dir > 0){
        new_line->next_line = _curr_line->next_line;
        new_line->prev_line = lh->curr_line;

        _curr_line->next_line = new_line_num;

        if (lh->curr_line == lh->bottom_line){
            lh->bottom = new_line_num;
        } else {
            neighbour = &(lh->lines[_curr_line->next_line]);
            neighbour->prev_line = new_line_num;
        }
          
    } else {
        new_line->prev_line = _curr_line->prev_line;
        new_line->next_line = lh->curr_line;

        _curr_line->prev_line = new_line_num;
        
        if (lh->curr_line == lh->top_line){
            lh->top_line = new_line_num;
        } else {
            neighbour = &(lh->lines[_curr_line->prev_line]);
            neighbour->next_line = new_line_num;
        }
        
    }

    return new_line_num;
}

int init_line_handler(line_handler* lh, pt_buffer_t* original_buffer){

    int s = 0, prev_line = NULL_LINE, new_line; 
    size_t i = 0; 

    if (original_buffer->size == 0){

        return 0; 
    }

    #ifdef DEBUG_PT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Creating Line Handler]\n");
        log_to_file(&sk_logger, pbuf);
    #endif 

    char c; 
    line *_prev_line = NULL, *_curr_line = NULL;
    while (i < original_buffer->size){

        c = original_buffer->text[i];

        int at_last = (i + 1 == original_buffer->size);

        if (c == '\n' || at_last){

            if (at_last) s++;

            new_line = get_new_line(lh);
            _curr_line = &(lh->lines[new_line]);

            _curr_line->line_size = s; 

            #ifdef DEBUG_PT
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, 
                    "[New line found in original at position]: %ld, new line is: %d\n", i, new_line);
                log_to_file(&sk_logger, pbuf);
            #endif 

            if (lh->size == 1){
               lh->top_line = new_line;
                
            } else {
                _prev_line->next_line = new_line;
                _curr_line->prev_line = prev_line;
            }

            _prev_line = _curr_line;
            prev_line = new_line;

            s = 0; 
        } else {
            s++; 
        }

        i++;
    
    }

    // add last line
    lh->bottom_line = prev_line;
    lh->curr_line = lh->top_line;

    return 0; 
}

int new_pt_entry(piece_table* pt){
     
    check_entries_size(pt);

    int new_ent_pos = pt->ent_num;
    pt->ent_num++;  
    
    pt_entry* new_single_entry = &(pt->entries[new_ent_pos]); 
    new_single_entry->src = ADD;
    new_single_entry->start = pt->addition.curr_pos; 
    new_single_entry->len = 0; 

    new_single_entry->left = NULL_ENT; 
    new_single_entry->right = NULL_ENT; 

    return new_ent_pos; 
}