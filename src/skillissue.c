#include "skillissue.h" 

int save_file_writes(FILE* f, piece_table* pt){
    // put together the piece table 

    printf("%p %p", (void*)f, (void*)pt);


    return 0; 

    // use freopen to clear the contents 

}

int handle_insertion_mode(piece_table* pt, usermode* umode, cursor_pos* curs_pos, int user_in, line_view* lv){

    /* handle user finishing up insertion mode*/
    if (user_in == USR_ESC){
        pt->curr_ins_ent = NULL_ENT;
        pt->curr_del_ent = NULL_ENT; 
        umode->mode &= 0;
        
     /* handle deletion */
    } else if (user_in == KEY_BACKSPACE || user_in == KEY_DC) {

        #ifdef DEBUG_DELETE 
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Delete Pressed]: which: %s, char val: %d\n", 
                user_in == KEY_BACKSPACE ? "Backspace" : "Delete Key", user_in);
            log_to_file(&sk_logger, pbuf);
        #endif  

        // swap_ins_with_del(pt);
        if (pt->curr_ins_ent > 0) 
            pt->curr_ins_ent = NULL_ENT; 

        delete_manager(pt, curs_pos, user_in, lv);
    
    /* insert char into additions and update the entry*/
    } else {
        
        if (pt->curr_del_ent > 0) 
            pt->curr_del_ent = NULL_ENT;     

        char _usr_in = (char)user_in;
        insert_manager(pt, curs_pos, _usr_in, lv);
    }

    return 0; 
}

int edit_file(char* fn, termw_info* tinfo){
     
    usermode umode; 
    memset((char*)&umode, 0, sizeof(usermode));

    int in_edit = 1;

    /* check that the file exists */
    if (file_exists(fn) < 0){
        printf("[ERROR]: file could not be located %s\n", fn);
        return -1;
    } 

    /* create appropriate items for reading file and info */
    piece_table pt; 
    FILE* f = fopen(fn, "r+");

    /* check that the file was opened correctly*/
    if (f == NULL){
        printf("[ERROR]: something went wrong with opening the file %s\n", fn);
        return -1;
    }
    
    int edit_status;

    /* open the file and init the piece table */
    if ((edit_status = init_piece_table(f, fn, &pt)) < 0 ){
        printf("Something went wrong trying to create piecetable for the file content\n");
        // goto handle_error; 
        exit(1);
    }

    piece_table* pt_ptr = &pt; 

    cursor_pos pos = { .x = 0, .y = 0 };
    line_view lv;

    init_line_handler(&(pt.lh), &(pt.original));
    init_line_view(pt_ptr, tinfo, &lv, &(pt.lh));
    
    render_screen(&pt, &lv);
    move(pos.y, pos.x);

    #ifdef DEBUG_PT
        log_piece_table_current(&sk_logger, &pt);
    #endif 

    int user_in;

    noecho();
    
    do {

        user_in = getch(); 

        #ifdef DEBUG_GEN

            if (umode.mode == 0){
                char* key_pressed;  
                char _key_pressed[10];
                switch(user_in){
                    case 'q':
                        key_pressed = "QUIT";
                        break;
                    case 'z':
                        key_pressed = "SAVE";
                        break;   
                    case 'w': 
                        key_pressed = "UP";
                        break;
                    case 's': 
                        key_pressed = "DOWN";
                        break;
                    case 'a': 
                        key_pressed = "LEFT";
                        break;
                    case 'd': 
                        key_pressed = "RIGHT";
                        break;
                    case 'i': 
                        key_pressed = "INSERTION";
                        break;
                    default:
                        // key_pressed = "N/A"; 
                        sprintf(_key_pressed, "%c", user_in);
                        key_pressed = _key_pressed; 
                        break;
                }
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[User In]: %s\n", key_pressed);
                log_to_file(&sk_logger, pbuf);
            }
            
        #endif  
        
        int insert_res; 

        /* determine the mode first if one is active */
        if (umode.mode > 0){
            
            switch(umode.mode){

                case MODE_INSERT:
                    insert_res = handle_insertion_mode(&pt, &umode, &pos, user_in, &lv);
                    if (insert_res < 0){}
                    break;

                case MODE_SAVE:
                    save_file_writes(f, &pt); 
                    umode.mode = 0; 
                    break; 

                default:
                    break; 
            }

        /* if not in a particular mode then see if we'll enter a mode */
        } else {
            int prev_ent, prev_chr_ptr; 
            switch(user_in){

                /* save progress and write to file */
                case 'z':
                    save_file_writes(f, &pt);
                    umode.mode = MODE_SAVE;
                    break; 

                /* quit the editor (if not saved then all progress is lost) */
                case 'q':
                    in_edit--; 
                    break;

                /* insert */
                case 'i':
                    umode.mode = MODE_INSERT;
                    break;  
                
                /* redo */
                case 'j':
                    break; 
                
                /* undo */
                case 'k':
                    break; 
                
                /* move around file*/
                case 'w':
                    handle_line_movement(&pt, &pos, -1, &lv);
                    break; 
                    
                case 'a':
                    handle_side_movement(&pt, &pos, -1, &lv);
                    handle_jump_up(&pt, &pos, &lv);
                    break;
                    
                case 's':
                    handle_line_movement(&pt, &pos, 1, &lv);
                    break;

                case 'd':
                    prev_chr_ptr = pt.curr_chr_ptr; 
                    prev_ent = pt.curr_ent_ptr;
                    handle_side_movement(&pt, &pos, 1, &lv);
                    handle_jump_down(&pt, &pos, prev_chr_ptr, prev_ent, &lv);
                    break; 
                
                default:
                    /* let the user know this was not valid - or maybe do nothing? */
                    break;
            }
        }

        // render only if a render is required
        if (lv.needs_render){
            render_screen(&pt, &lv);
            lv.needs_render--; 
        }

        // display info about current state
        display_screen_info(&pt, &lv, &pos, &umode, user_in); 

        move(pos.y , pos.x);



        #ifdef DEBUG_GEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[CURSOR POS]: x: %d, y: %d\n", pos.x, pos.y);
            log_to_file(&sk_logger, pbuf);
        #endif 
       
        
    } while (in_edit);

    #ifdef DEBUG_PT
        log_piece_table_current(&sk_logger, &pt);
    #endif 

    /* if no save was made then keep the contents as they were when the file was opened */
    if (!umode.made_save){
        
        fclose(f);
    } 

    empty_piece_table(&pt);

    return 0; 
}

void run_skillissue(int argc, char** argv){

    /* started with a file so load this file right away */
    if (argc > 1){
        termw_info tinfo;
        initscr(); 

        // get size of the terminal 
        getmaxyx(stdscr, tinfo.rows, tinfo.cols);

        // keep one row for displaying info
        tinfo.rows--;

        // enable getting int from getch
        keypad(stdscr, TRUE);   

        #ifdef DEBUG_GEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Screen Size] rows: %d, cols: %d\n", tinfo.rows, tinfo.cols);
            log_to_file(&sk_logger, pbuf);
        #endif 

        int res = edit_file(argv[1], &tinfo);

        #ifdef DEBUG_GEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Logger exiting]\n");
            log_to_file(&sk_logger, pbuf);
        #endif 

        #ifdef DEBUG_GEN
            teardown_logger(&sk_logger);
        #endif 

        endwin();

        if (res < 0) exit(1);

    } else {
        printf("[Error]: need to supply valid file path as arg\n");
        exit(1);
    }
}

int main(int argc, char** argv){

    #ifdef DEBUG_GEN
        setup_logger(&sk_logger, LOG_FILE);
        log_to_file(&sk_logger, "logger has been successfully set up!\n===================================\n\n");
    #endif 

    /* run */
    run_skillissue(argc, argv);

 
    return 0; 
}