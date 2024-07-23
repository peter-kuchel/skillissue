#include "screen.h"


void init_line_view(piece_table* pt, termw_info* tinfo, line_view* lv, line_handler* lh){

    memset((char*)lv, 0, sizeof(line_view)); 

    lv->top_win = lh->top_line;

    // there are more rows than can be displayed
    if (tinfo->rows < lh->size){
        
        line* _l; 
        int curr_l = lh->top_line;

        for(int v = 0; v < tinfo->rows - 1; v++){
            _l = &(lh->lines[curr_l]);
            curr_l = _l->next_line;
        }

        lv->bot_win = curr_l;

    } else {
        lv->bot_win = lh->bottom_line; 
    }

    // lv->left_win is already 0 
    lv->right_win = tinfo->cols; 
    lv->tinfo_ptr = tinfo;

    lv->top_view_ent = pt->ent_head; 
    lv->top_view_chr = pt->curr_chr_ptr; 

    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "[line init]: top_win: %d, bot_win: %d, right_win: %d, left_win: %d\n", 
            lv->top_win, lv->bot_win, lv->right_win, lv->left_win);
        log_to_file(&sk_logger, pbuf);
    #endif
     
}

static void update_top_ent_up(piece_table* pt, line_handler* lh, line_view* lv){

    line* new_top = LINE_AT_POS(lh, lv->top_win); 
    pt_entry* ent = ENT_AT_POS(pt, lv->top_view_ent); 

    // avoid possible underflow when chr ptr is at the beginning of either text buffer 
    ssize_t chr_ptr = (ssize_t)lv->top_view_chr; 
    ssize_t ent_start = (ssize_t)ent->start;

    // line_size + 1 to account for the new line and skip over it
    for(int d = 0; d < new_top->line_size + 1; d++){

        chr_ptr--; 
        if (chr_ptr < ent_start){
            lv->top_view_ent = ent->left; 
            ent = ENT_AT_POS(pt, lv->top_view_ent);

            chr_ptr = (ssize_t)(ent->start + ent->len - 1);
            ent_start = (ssize_t)ent->start; 
        }
    }

    
    lv->top_view_chr = (size_t)chr_ptr;
}

static void update_top_ent_down(piece_table* pt, line_handler* lh, line_view* lv, int prev_top_win){
    
    line* prev_top = &(lh->lines[prev_top_win]);
    pt_entry* ent = ENT_AT_POS(pt, lv->top_view_ent);

    // line_size + 1 to account for the new line and skip over it
    for (int d = 0; d < prev_top->line_size + 1; d++){

        lv->top_view_chr++; 

        if (lv->top_view_chr == (ent->start + ent->len)){
            lv->top_view_ent = ent->right; 
            ent = ENT_AT_POS(pt, lv->top_view_ent);
            lv->top_view_chr = ent->start; 
        }
    }
}

void update_view_move_down(piece_table* pt, line_view* lv, cursor_pos* pos){
    
    line_handler* lh = &(pt->lh);

    int max_row = lv->tinfo_ptr->rows;

    // check if the screen lines will be shifted
    if (pos->y == max_row){

        if (lv->bot_win != lh->bottom_line){ 

            int prev_top_win = lv->top_win;
            lv->top_win = (lh->lines[lv->top_win]).next_line;  
            lv->bot_win = (lh->lines[lv->bot_win]).next_line;

            // update line views top_view_ent 
            update_top_ent_down(pt, lh, lv, prev_top_win);
            lv->needs_render++; 
        }

        pos->y--; 
        
    }


    // #ifdef DEBUG_SCREEN
    //     memset(pbuf, 0, PBUF_SIZE);
    //     sprintf(pbuf, "(curr) %d == %d (bot) | bot win: %d\n", lh->curr_line, lh->bottom_line, lv->bot_win);
    //     log_to_file(&sk_logger, pbuf);
    // #endif 
    
}

void update_view_move_up(piece_table* pt, line_view* lv, cursor_pos* pos){

    line_handler* lh = &(pt->lh);

    // check if the screen lines will be shifted
    if (pos->y < 0){

        if (lv->top_win != lh->top_line){

            lv->top_win = (lh->lines[lv->top_win]).prev_line; 
            lv->bot_win = (lh->lines[lv->bot_win]).prev_line; 

            update_top_ent_up(pt, lh, lv);    

            lv->needs_render++;
        }

        pos->y++;
        
    }
}

void update_view_ins_nl(piece_table* pt, line_view* lv){
    line_handler* lh = &(pt->lh);

    // update bottom window depending on lines file relative to term window size
    if (lh->size < lv->tinfo_ptr->rows){

        lv->bot_win = lh->bottom_line;

    } else {

        lv->bot_win = (lh->lines[lv->bot_win]).prev_line;
    }

}


void update_view_del_nl(piece_table* pt, line_view* lv){

    // only the lv->bot_win gets changed?

    lv->bot_win = (pt->lh.lines[lv->bot_win]).prev_line; 
}
static void move_into_next_ent(piece_table* pt, pt_track* track){

    pt_entry* track_ent = &(track->ent); 
     
    if (track_ent->start == track->ent_end){
        int _next_ent = track->ent_end == 0 ? track->curr_ent : track_ent->right; 

        pt_entry* right_ent = &(pt->entries[_next_ent]);

        track_ent->start = track->ent_end == 0 ? track->curr_start_ptr : right_ent->start; 
        track_ent->len = right_ent->len; 
        track_ent->src = right_ent->src; 
        track_ent->right = right_ent->right; 

        track->buff = GET_PT_BUFF(pt, track_ent->src);
        track->ent_end = track_ent->start + track_ent->len;

        // #ifdef DEBUG_SCREEN
        //     memset(pbuf, 0, PBUF_SIZE);
        //     sprintf(pbuf, 
        //         "NEXT ENT: %d [start: %ld | end: %ld\n]\n", 
        //         _next_ent, track->ent.start, track->ent_end);
        //     log_to_file(&sk_logger, pbuf);
        // #endif
    }

    

}

// this can be re-written now 
static void move_right_offset(piece_table* pt, pt_track* track, line_print* lp){
    if (lp->right_cutoff){

        char curr_chr = CHR_IN_TRACK(track);
        while (curr_chr != '\n'){

            track->ent.start++; 
            move_into_next_ent(pt, track);
            curr_chr = CHR_IN_TRACK(track);
        }

        lp->right_cutoff--; 
    }
}

static void calc_line_size(piece_table* pt, pt_track* track, line_print* lp, line_view* lv, line* _l){
    lp->line_pos = 0;

    if (_l->line_size > lv->right_win){
        lp->right_cutoff++;
        lp->line_size = lv->right_win - lv->left_win;

    } else {
        lp->line_size = _l->line_size - lv->left_win;
    }

    // begin left offset
    for(int left_off = 0; left_off < lv->left_win; left_off++){
        track->ent.start++; 
        move_into_next_ent(pt, track);
    }

    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Line size: %d]\n", lp->line_size);
        log_to_file(&sk_logger, pbuf);
    #endif 
}

// TODO
// calc size of everything in advance so that only 1 sys call is made 

void render_screen(piece_table* pt, line_view* lv){

    clear();

    line_handler* lh = &(pt->lh);
    line* _l; 
    line_print lp = { 0, 0, 0 }; 
 
    int view_curr = lv->top_win;
    // int view_end = lh->curr_line == lh->bottom_line ? NULL_LINE : lv->bot_win;
    
    pt_track track;
    memset((char*)&track, 0, sizeof(pt_track)); 

    track.curr_start_ptr = lv->top_view_chr; 
    track.curr_ent = lv->top_view_ent; 

    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "\n--{Line start: %d | line end: %d | curr ent: %d}--\n\n", 
            view_curr, lv->bot_win, track.curr_ent);
        log_to_file(&sk_logger, pbuf);
    
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "top win: %d | bot win: %d\n", 
            lv->top_win, lv->bot_win);
        log_to_file(&sk_logger, pbuf);
    #endif 

    for (int i = 0; i < lv->tinfo_ptr->rows; i++){
        _l = &(lh->lines[view_curr]);

        if (lv->left_win > _l->line_size){

            // print newline here since this line is out of view
            printw("\n");

            #ifdef DEBUG_SCREEN
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[printed only newline]\n");
                log_to_file(&sk_logger, pbuf);
            #endif 

        } else {

            calc_line_size(pt, &track, &lp, lv, _l);

            lp.line_size++;
            
            char to_print[lp.line_size + 1];

            while (lp.line_pos < lp.line_size){

                // move into next ent if needed
                move_into_next_ent(pt, &track);

                to_print[lp.line_pos] = CHR_IN_TRACK( (&track));
                track.ent.start++; 
                lp.line_pos++; 
            }
            to_print[lp.line_size] = '\0';
            printw("%s", to_print);
        }

        // moving the chr ptr to the next top line
        move_right_offset(pt, &track, &lp);

        view_curr = _l->next_line; 
    }
    
}