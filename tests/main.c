#include "screen_tests.h"
#include "insertmode_tests.h"


void setup_test_objects(piece_table *pt, FILE* f, termw_info *tinfo, line_view *lv){

     if ( init_piece_table(f, TEST_FILE, pt) < 0 ){
          printf("failed to open test file\n");
	  exit(1); 
     }     

     init_line_handler(&(pt->lh), &(pt->original));
     init_line_view(pt, tinfo, lv, &(pt->lh));
     
     log_piece_table_current(&sk_logger, pt); 
}

void original_pos_assertion(piece_table *pt, line_view *lv, cursor_pos *pos){

     if ( lv->left_win != 0 || lv->right_win != 76 || pos->x != 0 || pt->curr_chr_ptr != 0){
          printf("\e[0;31m[ ORIGIN ASSERTION FAILED ]\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d, x = %d\n", 0, 0, 76, 0);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d, x = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win, pos->x);
          empty_piece_table(pt);
	  exit(1);
     }
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
    original_pos_assertion(&pt, &lv, &pos);
    
    all_insertionmode_tests(&pt, &lv, &pos);
    return 0;
}
