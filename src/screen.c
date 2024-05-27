#include "screen.h"

int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir){
    
    pt_entry* ent = CURR_ORG_ENT_PTR(pt);
    size_t chr_ptr = pt->curr_chr_ptr; 

    // move to the right 
    if (dir > 0){
        size_t upper_bound = ent->start + ent->len;

        // go forward to next entry 
        if (chr_ptr == upper_bound || chr_ptr + 1 == upper_bound){

            if (pt->table.org_tail == pt->curr_org_ptr){
                
                // if at the very end of the file 
                if (chr_ptr + 1 == upper_bound){
                    pos->x++;
                    pt->curr_chr_ptr++; 
                }

                return 0;
            }

            if (chr_ptr + 1 == upper_bound){
                pt->curr_org_ptr++; 
                ent = CURR_ORG_ENT_PTR(pt);

                pt->curr_chr_ptr = ent->start - 1;
            } 
        } 

        pos->x++;
        pt->curr_chr_ptr++;

        // handle a newline
        char curr_chr = get_curr_char_by_entry(pt, ent, pt->curr_chr_ptr);

        if (curr_chr == '\n'){
            pos->x = 0; 
            pos->y++;
        }


    } else {
        size_t lower_bound = ent->start; 

        // go back to prev entry 

        // memset(pbuf, 0, PBUF_SIZE);
        // sprintf(pbuf, "chr_ptr-- vs lowerbound: %ld <= %ld\n == %d\n", chr_ptr - 1, lower_bound, (chr_ptr - 1 <= lower_bound));
        // log_to_file(&sk_logger, pbuf); 

        if (chr_ptr == lower_bound || chr_ptr - 1 == lower_bound){
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[At (or past) lower bound]\n");
            log_to_file(&sk_logger, pbuf);  
            
            if (pt->table.org_head == pt->curr_org_ptr){

                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At far left entry]: chr_ptr = %ld\n",chr_ptr);
                log_to_file(&sk_logger, pbuf);  

                // at the very beginning of the file
                if (chr_ptr - 1 == lower_bound){
                    
                    pos->x--;
                    pt->curr_chr_ptr--; 

                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, "[currently at very end] before: %ld, after: %ld\n", chr_ptr, pt->curr_chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                }
                return 0;
            }
            
            if (chr_ptr == lower_bound){
                pt->curr_org_ptr--; 
                ent = CURR_ORG_ENT_PTR(pt);

                pt->curr_chr_ptr = (ent->start + ent->len); 

                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[going into next entry - 1]: curr_org_ptr: %d, curr_chr_ptr before: %ld, after: %ld\n", 
                    pt->curr_org_ptr, chr_ptr, pt->curr_chr_ptr);
                log_to_file(&sk_logger, pbuf);
            }
        }

        pos->x--;
        pt->curr_chr_ptr--; 

        char _c = CURR_PTR_AT_CHR(pt);
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(
            pbuf, 
            "chr_ptr before: %ld, after: %ld| %c | lb: %ld| org_pos: %d, org_head: %d\n", 
            chr_ptr, pt->curr_chr_ptr, _c, 
            lower_bound, pt->curr_org_ptr, pt->table.org_head);
        log_to_file(&sk_logger, pbuf);

        char curr_chr = get_curr_char_by_entry(pt, ent, pt->curr_chr_ptr);

        if (curr_chr == '\n'){

            // memset(pbuf, 0, PBUF_SIZE);
            // sprintf(pbuf, "chr_ptr %ld ? %ld lower bound: \n", chr_ptr, lower_bound);
            // log_to_file(&sk_logger, pbuf); 
            // need to find position for x at the end of the line 
            pos->x = 0; 
            pos->y--;
        }
    }

    

    return 0; 
}

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