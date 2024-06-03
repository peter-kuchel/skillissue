#include "screen.h"

void render_screen(piece_table* pt){

    /* clear screen first*/
    clear();

    pt_entry* entry;
    pt_buffer_t* buff;

    int _ent = pt->ent_head; 

    do {
        entry = &(pt->entries[_ent]);

        int _start = entry->start; 
        int _len   = entry->len; 
        int _end = _start + _len;

        if (entry->src == ORGN)
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

        _ent = entry->right;

    } while (_ent != NULL_ENT);

}