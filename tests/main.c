#include "screen_tests.h"


void setup_test_objects(piece_table *pt, FILE* f, termw_info *tinfo, line_view *lv){

     if ( init_piece_table(f, TEST_FILE, pt) < 0 ){
          printf("failed to open test file\n");
	  exit(1); 
     }     

     init_line_handler(&(pt->lh), &(pt->original));
     init_line_view(pt, tinfo, lv, &(pt->lh));
     
     log_piece_table_current(&sk_logger, pt); 
}

int main(){

    piece_table pt; 
    line_view lv;
    cursor_pos pos = { 0, 0 };
    termw_info tinfo = { 8, 76 };
     
    printf("\e[0;32m--- [ TESTS STARTING ] ---\e[0m\n\n");
    FILE* f = fopen(TEST_FILE, "r+");

    setup_logger(&sk_logger, LOG_FILE);
    setup_test_objects(&pt, f, &tinfo, &lv);
    fclose(f); 

    all_screen_tests(&pt, &lv, &pos);

    return 0;
}
