#include "screen.h"

void render_screen(piece_table* pt){

    /* clear screen first*/
    clear();

    int ent_tail = pt->table.org_tail; 
    int ent_head = pt->table.org_head;

    for (int i = ent_head ; i <= ent_tail; i++){
        pt_entry entry = pt->table.entries[pt->table.organizer[i]]; 

        int _start = entry.start; 
        int _len   = entry.len; 
        int _end = _start + _len;

        pt_buffer_t* buff; 

        if (entry.src == ORGN)
            buff = &(pt->original); 
        else 
            buff = &(pt->addition.buf); 
        
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