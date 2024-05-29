#include "screen.h"

void render_screen(piece_table* pt){

    /* clear screen first*/
    clear();

    int _tail = pt->table.org_tail; 
    int _head = pt->table.org_head;

    pt_entry* entry;
    pt_buffer_t* buff;

    for (int i = _head ; i <= _tail; i++){
        
        entry = ENT_PTR_AT_POS(pt, i);

        int _start = entry->start; 
        int _len   = entry->len; 
        int _end = _start + _len;

        if (entry->src == ORGN)
            buff = &(pt->original); 
        else 
            buff = &(pt->addition.buf); 
        
        char to_print[_len]; 
        int j = 0; 
        for (int i = _start; i < _end; i++){
            to_print[j] = buff->text[i];
            j++;
        }
             
        to_print[_len] = '\0';

        printw("%s", to_print);    
    }

}