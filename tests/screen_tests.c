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

int screen_test_moving_across_and_down_to_first_line(piece_table *pt, line_view *lv, cursor_pos *pos){
     int i;

     // move to the very endd of the 1st line and then it should automatically jump down to the next
     for (i = 0; i < 79; i++) sim_pressing_D(pt, pos, lv);

     // move up one space back to the beginning of the file
     for (i = 0; i < 1; i++) sim_pressing_W(pt, pos, lv);

     return pos->x == 0 && pt->curr_chr_ptr == 0;
}

int screen_test_scrolling_standard(piece_table *pt, line_view *lv, cursor_pos *pos){
     
     int i;

     // move the cursor 8 places to the right
     for (i = 0; i < 8; i++) sim_pressing_D(pt, pos, lv);
     
     // move the cursor down to the bottom of the document 
     for (i = 0; i < 15; i++) sim_pressing_S(pt, pos, lv);

     // move the cursor back up to the top 
     for (i = 0; i < 15; i++) sim_pressing_W(pt, pos, lv);

     // move the cursor back to the beginning of the document
     for (i = 0; i < 8; i++) sim_pressing_A(pt, pos, lv);
     
     // assert that chr position and x pos are both 0
     return pos->x == 0 && pt->curr_chr_ptr == 0;
}
int main(){

     piece_table pt; 
     line_view lv;
     cursor_pos pos = { 0, 0 };
     termw_info tinfo = { 8, 76 };
     
     printf("\e[0;32m--- [SCREEN TESTS STARTING] ---\e[0m\n");
     FILE* f = fopen(TEST_FILE, "r+");

     setup_logger(&sk_logger, LOG_FILE);
     setup_test_objects(&pt, f, &tinfo, &lv);
     fclose(f); 
     
     int res;
     res = screen_test_moving_across_and_down_to_first_line(&pt, &lv, &pos);
     if (res == 0){
          printf("\e[0;31m[TEST FAILED -- screen_test_moving_across_and_down_to_first_line]\e[0m\n");
	  printf("Expected results :: curr_chr_ptr = %d, x = %d\n", 0, 0);
	  printf("Actual results :: curr_chr_ptr = %ld, x = %d\n", pt.curr_chr_ptr, pos.x);
	  exit(1);
     }

     res = screen_test_scrolling_standard(&pt, &lv, &pos);
     if (res == 0){
          printf("\e[0;31m[TEST FAILED -- screen_test_scrolling_standard]\e[0m\n");
	  printf("Expected results :: curr_chr_ptr = %d, x = %d\n", 0, 0);
	  printf("Actual results :: curr_chr_ptr = %ld, x = %d\n", pt.curr_chr_ptr, pos.x);
	  exit(1);
     }
		     
     printf("--- [SCREEN TESTS FINISHED] ---\n");
     return 0;
}
