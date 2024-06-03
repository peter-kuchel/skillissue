#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


#include "skillissue.h"


static termw_info tinfo; 


void save_file_writes(FILE* f, piece_table* pt){
    // put together the piece table 

    printf("%p %p", (void*)f, (void*)pt);

    // use freopen to clear the contents 

}

void exit_insertion_mode(piece_table* pt, usermode* umode){
    pt->curr_ins_ent = NULL_ENT;
    pt->curr_del_ent = NULL_ENT; 
    umode->mode &= 0;
}

int handle_insertion_mode(piece_table* pt, usermode* umode, cursor_pos* curs_pos, int user_in){

    /* handle user finishing up insertion mode*/
    if (user_in == USR_MODE_ESC){
        exit_insertion_mode(pt, umode);
        
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

        delete_manager(pt, curs_pos, user_in);
    
    /* insert char into additions and update the entry*/
    } else {
        
        if (pt->curr_del_ent > 0) 
            pt->curr_del_ent = NULL_ENT;     

        char _usr_in = (char)user_in;
        insert_manager(pt, curs_pos, _usr_in);
    }

    return 0; 
}

int edit_file(char* fn){
     
    usermode umode; 
    umode.mode &= 0; 

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
    
    render_screen(&pt);

    /* set cursor to very beginning of the file */
    cursor_pos pos = { .x = 0, .y = 0 };
    move(pos.y, pos.x);

    log_piece_table_current(&sk_logger, &pt);

    int user_in;
    
    do {

        /* render piece table to output */
        
        user_in = getch(); 
        #ifdef DEBUG_GEN 
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[User In]: %d\n", user_in);
            log_to_file(&sk_logger, pbuf);
        #endif  
        
        int insert_res; 

        /* determine the mode first if one is active */
        if (umode.mode > 0){
            
            switch(umode.mode){

                case USR_MODE_INS:
                    insert_res = handle_insertion_mode(&pt, &umode, &pos, user_in);
                    if (insert_res < 0){}
                    break;

                default:
                    break; 
            }

        /* if not in a particular mode then see if we'll enter a mode */
        } else {
            switch(user_in){

                /* save progress and write to file */
                case 'z':
                    save_file_writes(f, &pt);
                    umode.made_save = 1;
                    break; 

                /* quit the editor (if not saved then all progress is lost) */
                case 'q':
                    in_edit--; 
                    break;

                /* insert */
                case 'i':
                    umode.mode |= USR_MODE_INS;
                    break;  
                
                /* redo */
                case 'j':
                    break; 
                
                /* undo */
                case 'k':
                    break; 
                
                /* move around file*/
                case 'w':
                    break; 
                    
                case 'a':
                    handle_side_movement(&pt, &pos, -1);
                    break;
                case 's':

                    break;
                case 'd':
                    handle_side_movement(&pt, &pos, 1);
                    break; 
                
                default:
                    /* let the user know this was not valid - or maybe do nothing? */
                    break;
            }
        }

        /* re-render the screen */
        // need to be in this order 
        render_screen(&pt);
        move(pos.y , pos.x);
       

        if (edit_status < 0) in_edit--;
        
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

void run_sk(int argc, char** argv){

    /* started with a file so load this file right away */
    if (argc > 1){
        initscr(); 

        // get size of the terminal 
        getmaxyx(stdscr, tinfo.rows, tinfo.cols);

        // enable getting int from getch 
        keypad(stdscr, TRUE);   

        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Screen Size] rows: %d, cols: %d\n", tinfo.rows, tinfo.cols);
        log_to_file(&sk_logger, pbuf);

        int res = edit_file(argv[1]);

        teardown_logger(&sk_logger);
        endwin();

        if (res < 0) exit(1);

    } else {
        printf("[Error]: need to supply valid file path as arg\n");
        exit(1);
    }
}

int main(int argc, char** argv){

    
    setup_logger(&sk_logger, LOG_FILE);
    log_to_file(&sk_logger, "logger has been successfully set up!\n===================================\n\n");

    /* run */
    run_sk(argc, argv);

 
    return 0; 
}
