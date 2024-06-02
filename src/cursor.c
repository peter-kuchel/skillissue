#include "cursor.h"

int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir){
    
    pt_entry* ent = CURR_ORG_ENT_PTR(pt);
    size_t chr_ptr = pt->curr_chr_ptr; 

    // move to the right 
    if (dir > 0){
        size_t upper_bound = ent->start + ent->len;

        // go forward to next entry 
        if (chr_ptr == upper_bound || chr_ptr + 1 == upper_bound){
            
            #ifdef DEBUG_MOVE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At (or past) upper bound]\n");
                log_to_file(&sk_logger, pbuf);
            #endif 

            if (pt->table.org_tail == pt->curr_org_ptr){
                
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
                // pt->curr_org_ptr++; 
                // ent = CURR_ORG_ENT_PTR(pt);

                do {
                    pt->curr_org_ptr++; 
                    ent = CURR_ORG_ENT_PTR(pt);
                } while (ent->len == 0);

                pt->curr_chr_ptr = ent->start;

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                    "[going into next entry + 1]: curr_org_ptr: %d, curr_chr_ptr before: %ld, after: %ld\n", 
                        pt->curr_org_ptr, chr_ptr, pt->curr_chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 
            } 
        } else {
            pt->curr_chr_ptr++;
        }


        #ifdef DEBUG_MOVE
            char _c = CURR_PTR_AT_CHR(pt);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(
                pbuf, 
                "[LEFT MOVE] chr_ptr before: %ld, after: %ld | %c | ub: %ld| org_pos: %d, org_tail: %d\n", 
                chr_ptr, pt->curr_chr_ptr, _c, 
                upper_bound, pt->curr_org_ptr, pt->table.org_tail);
            log_to_file(&sk_logger, pbuf);
        #endif 

        pos->x++;
        
        // handle a newline
        char curr_chr = get_curr_char_by_entry(pt, ent, pt->curr_chr_ptr);

        if (curr_chr == '\n'){
            pos->x = 0; 
            pos->y++;
        }


    } else {
        size_t lower_bound = ent->start; 

        if (chr_ptr == lower_bound || chr_ptr - 1 == lower_bound){
            #ifdef DEBUG_MOVE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At (or past) lower bound]\n");
                log_to_file(&sk_logger, pbuf); 
            #endif 
            
            if (pt->table.org_head == pt->curr_org_ptr){

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
                // pt->curr_org_ptr--; 
                // ent = CURR_ORG_ENT_PTR(pt);

                do {
                    pt->curr_org_ptr--; 
                    ent = CURR_ORG_ENT_PTR(pt);
                } while (ent->len == 0);

                pt->curr_chr_ptr = (ent->start + ent->len); 

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                        "[going into next entry - 1]: curr_org_ptr: %d, curr_chr_ptr before: %ld, after: %ld\n", 
                        pt->curr_org_ptr, chr_ptr, pt->curr_chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 
            }
        }

        pos->x--;
        pt->curr_chr_ptr--; 

        #ifdef DEBUG_MOVE
            char _c = CURR_PTR_AT_CHR(pt);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(
                pbuf, 
                "[RIGHT MOVE] chr_ptr before: %ld, after: %ld| %c | lb: %ld| org_pos: %d, org_head: %d\n", 
                chr_ptr, pt->curr_chr_ptr, _c, 
                lower_bound, pt->curr_org_ptr, pt->table.org_head);
            log_to_file(&sk_logger, pbuf);
        #endif 

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