#include "screen.h"


void init_line_view(termw_info* tinfo, line_view* lv, line_handler* lh){

    memset((char*)lv, 0, sizeof(line_view)); 

    lv->top_view = lh->top_line;

    // there are more rows than can be displayed
    if (tinfo->rows < lh->size){
        
        line* _l; 
        int curr_l = lh->top_line;

        for(int v = 0; v < tinfo->rows; v++){
            _l = &(lh->lines[curr_l]);
            curr_l = _l->next_line;
        }

        lv->bot_view = curr_l;

    } else {
        lv->bot_view = lh->bottom_line; 
    }

    // lv->left_win is already 0 
    lv->right_win = tinfo->cols; 
    lv->tinfo_ptr = tinfo;
    
     
}

void update_view_ins_nl(line_handler* lh, line_view* lv){
    if (lh->size < lv->tinfo_ptr->rows){
        lv->bot_view = lh->bottom_line;
    } else {
        lv->bot_view = (lh->lines[lv->bot_view]).prev_line;
    }
}


void update_view_del_nl(line_handler* lh, line_view* lv){

}
static void move_into_next_ent(piece_table* pt, pt_track* track){

    pt_entry* track_ent = &(track->ent); 
     
    if (track_ent->start == track->ent_end){
        int _next_ent = track->ent_end == 0 ? pt->ent_head : track_ent->right; 

        pt_entry* right_ent = &(pt->entries[_next_ent]);

        track_ent->start = right_ent->start; 
        track_ent->len = right_ent->len; 
        track_ent->src = right_ent->src; 
        track_ent->right = right_ent->right; 

        track->buff = GET_PT_BUFF(pt, track_ent->src);
        track->ent_end = track_ent->start + track_ent->len;
    }

    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "track.ent.start: %ld | track_end: %ld\n", track->ent.start, track->ent_end);
        log_to_file(&sk_logger, pbuf);
    #endif

}

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

    /* clear screen first*/
    clear();

    line_handler* lh = &(pt->lh);
    line* _l; 
    line_print lp = { 0, 0, 0 }; 

    // int curr_line = lh->top_line;  
    int view_curr = lv->top_view;
    int view_end = (lh->lines[lv->bot_view]).next_line;
    
    pt_track track;
    memset((char*)&track, 0, sizeof(pt_track)); 

    do {
        _l = &(lh->lines[view_curr]);

        #ifdef DEBUG_SCREEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "Line to print: %d | line end: %d\n", view_curr, view_end);
            log_to_file(&sk_logger, pbuf);
        #endif 

        if (lv->left_win > _l->line_size){

            // print newline here since this line is out of view
            printw("\n");

        } else {

            calc_line_size(pt, &track, &lp, lv, _l);

            // account for the \n at the very end when printing , if bottom line then don't inc the size
            if (view_curr != lh->bottom_line)
                lp.line_size++;
            
            char to_print[lp.line_size + 1];

            while (lp.line_pos < lp.line_size){
                move_into_next_ent(pt, &track);

                to_print[lp.line_pos] = CHR_IN_TRACK( (&track));
                track.ent.start++; 
                lp.line_pos++; 
            }
            to_print[lp.line_size] = '\0';
            printw("%s", to_print);
        }
        move_right_offset(pt, &track, &lp);

        view_curr = _l->next_line; 
    } while (view_curr != view_end);

}