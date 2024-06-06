#include "cursor.h"

int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir){
    
    pt_entry* ent = &(pt->entries[pt->curr_ent_ptr]);
    size_t chr_ptr = pt->curr_chr_ptr; 

    /* move to the right */  
    if (dir > 0){
        size_t upper_bound = ent->start + ent->len;

        // go forward to next entry 
        if (chr_ptr == upper_bound || chr_ptr + 1 == upper_bound){
            
            #ifdef DEBUG_MOVE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At (or past) upper bound]\n");
                log_to_file(&sk_logger, pbuf);
            #endif 

            if (pt->ent_tail == pt->curr_ent_ptr){
                
                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, "[At far left entry]: chr_ptr = %ld\n",chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 

                // if at the very end of the file 
                if (chr_ptr + 1 == upper_bound){
                    pos->x++;
                    pt->curr_chr_ptr++; 

                    #ifdef DEBUG_MOVE
                        memset(pbuf, 0, PBUF_SIZE);
                        sprintf(pbuf, "[currently at very end] before: %ld, after: %ld\n", 
                            chr_ptr, pt->curr_chr_ptr);
                        log_to_file(&sk_logger, pbuf);
                    #endif 
                }

                return 0;
            }

            if (chr_ptr + 1 == upper_bound){
                
                int right = (&(pt->entries[pt->curr_ent_ptr]))->right;

                ent = &(pt->entries[right]);

                pt->curr_chr_ptr = ent->start;
                pt->curr_ent_ptr = right;

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                    "[going into next entry + 1]: curr_org_ptr: %d, curr_chr_ptr before: %ld, after: %ld\n", 
                        pt->curr_ent_ptr, chr_ptr, pt->curr_chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 
            } 
        } else {
            pt->curr_chr_ptr++;
        }


        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt, pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(
                pbuf, 
                "[RIGHT MOVE] chr_ptr before: %ld, after: %ld | %c | ub: %ld| org_pos: %d, org_tail: %d\n", 
                chr_ptr, pt->curr_chr_ptr, _c, 
                upper_bound, pt->curr_ent_ptr, pt->ent_tail);
            log_to_file(&sk_logger, pbuf);
        #endif 

        pos->x++;
        

    /* move to the left */ 
    } else {
        size_t lower_bound = ent->start; 

        if (chr_ptr == lower_bound || chr_ptr - 1 == lower_bound){
            #ifdef DEBUG_MOVE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At (or past) lower bound]\n");
                log_to_file(&sk_logger, pbuf); 
            #endif 
            
            if (pt->ent_head == pt->curr_ent_ptr){

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, "[At far left entry]: chr_ptr = %ld\n",chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif  

                // at the very beginning of the file
                if (chr_ptr - 1 == lower_bound){
                    
                    pos->x--;
                    pt->curr_chr_ptr--; 

                    #ifdef DEBUG_MOVE
                        memset(pbuf, 0, PBUF_SIZE);
                        sprintf(pbuf, "[currently at very end] before: %ld, after: %ld\n",
                            chr_ptr, pt->curr_chr_ptr);
                        log_to_file(&sk_logger, pbuf);
                    #endif 
                }
                return 0;
            }
            
            if (chr_ptr == lower_bound){

                int left = (&(pt->entries[pt->curr_ent_ptr]))->left;

                ent = &(pt->entries[left]);

                pt->curr_chr_ptr = (ent->start + ent->len); 
                pt->curr_ent_ptr = left;

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                        "[going into next entry - 1]: curr_org_ptr: %d, curr_chr_ptr before: %ld, after: %ld\n", 
                        pt->curr_ent_ptr, chr_ptr, pt->curr_chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 
            }
        }

        pos->x--;
        pt->curr_chr_ptr--; 

        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt, pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(
                pbuf, 
                "[LEFT MOVE] chr_ptr before: %ld, after: %ld| %c | lb: %ld| org_pos: %d, org_head: %d\n", 
                chr_ptr, pt->curr_chr_ptr, _c, 
                lower_bound, pt->curr_ent_ptr, pt->ent_head);
            log_to_file(&sk_logger, pbuf);
        #endif 

    }

    return 0; 
}

int handle_jump_down(piece_table* pt, cursor_pos* pos){
    if ( PTR_AT_CHR(pt, pt->prev_chr_ptr) == '\n'){
                        // handle_side_movement(&pt, &pos, 1);
        pt->lh.curr_line++; 
        pos->y++;
        pos->x = 0;
        
        #ifdef DEBUG_MOVE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[going to next line from the right]");
            log_to_file(&sk_logger, pbuf);
        #endif 
    }

    return 0; 
}

int handle_jump_up(piece_table* pt, cursor_pos* pos){

    if (PTR_AT_CHR(pt, pt->curr_chr_ptr) == '\n'){
        
        pt->lh.curr_line--; 
        pos->y--; 
        pos->x = CURR_LINE_SIZE(pt);
        
    }
    return 0; 
}

int move_chr_ptr(piece_table* pt, int dist, int dir){

    int d = 0; 
    int curr_ent = pt->curr_ent_ptr; 
    
    pt_entry *curr = &(pt->entries[curr_ent]);
    size_t end = dir > 0 ? curr->start : curr->len - 1; 

    while (d < dist){

        if (dir > 0) 
            pt->curr_chr_ptr++;
        else         
            pt->curr_chr_ptr--; 

        d++; 

        if (pt->curr_chr_ptr == end){

            curr_ent = dir > 0 ? curr->left : curr->right; 
            curr = &(pt->entries[curr_ent]);
            
            pt->curr_chr_ptr = dir > 0 ? curr->start : curr->start + (curr->len - 1); 
            end = dir > 0 ? curr->len - 1: curr->start; 
        }
    }

    return 0; 
}

int handle_line_movement(piece_table* pt, cursor_pos* pos, int dir){
    line *curr, *jump_to;
    int dist;
    // check for going up the file

    int _curr_line = pt->lh.curr_line; 
    if (dir > 0){
        
        if (pt->lh.top_line == _curr_line) return 0; 

        pos->y--; 

        // handle x position 
        
        curr = &(pt->lh.lines[_curr_line]);

        jump_to = &(pt->lh.lines[curr->next_line]); 

        // calculate distance from current line to beginning of the new line 
        if (jump_to->line_size < curr->line_size)
            dist = pos->x; 
        // calculate how many steps to walk through to get at the same x
        else 
             dist = (jump_to->line_size - pos->x) + pos->x; 
        
        move_chr_ptr(pt, dist, -1);

    // check for going down the file
    } else {
        if (pt->lh.bottom_line == _curr_line) return 0;

        pos->y++; 

        curr = &(pt->lh.lines[_curr_line]);
        jump_to = &(pt->lh.lines[curr->prev_line]);

        if (jump_to->line_size < curr->line_size)
            dist = pos->x; 
        else 
            dist = (jump_to->line_size - pos->x) + pos->x; 
        

        move_chr_ptr(pt, dist, 1);
    } 
    return 0;
}