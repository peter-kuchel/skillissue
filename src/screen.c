#include "screen.h"

void render_screen(piece_table* pt){

    /* clear screen first*/
    clear();

    size_t pt_elements = pt->table.curr_ent_num; 

    for (size_t i = 0 ; i < pt_elements; i++){
        pt_entry entry = *(pt->table.entries[i]); 

        int _start = entry.start; 
        int _len   = entry.len; 
        int _end = _start + _len;

        text_buffer* buff; 

        if (entry.src == ORG)
            buff = &(pt->original); 
        else 
            buff = &(pt->addition); 
        
        char to_print[_len + 1]; 
        int j = 0; 
        for (int i = _start; i < _end; i++){
            to_print[j] = buff->text[i];
            j++;
        }
             
        to_print[_len] = '\0';

        printw("%s", to_print);    
    }

}