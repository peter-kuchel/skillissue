#include "screen.h"

void intro_screen_no_file(){

}

void render_screen(piece_table* pt){

    /* clear screen first*/
    clear();

    size_t pt_elements = pt->pte_curr; 

    for (size_t i = 0 ; i < pt_elements; i++){
        piece_table_element elem = pt->table[i]; 

        int _start = elem.start; 
        int _len   = elem.len; 
        int _end = _start + _len;

        text_buffer* buff; 

        if (elem.src == ORG)
            buff = pt->original_buffer; 
        else 
            buff = pt->add_buffer; 
        
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