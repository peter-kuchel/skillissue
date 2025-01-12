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

int screen_test_scrolling_with_right_offset(piece_table *pt, line_view *lv, cursor_pos *pos){
     printf("screen_test_scrolling_with_right_offset -- ");
     int i;

     // move close to the very end of the line so that the right and left window have to be adjusted
     for (i = 0; i < 77; i++) sim_pressing_D(pt, pos, lv);


     // assert that window position is correct and the chr_ptr position is correct 
     if ( lv->left_win != 2 || lv->right_win != 78 || pt->curr_chr_ptr != 77){
	  printf("\e[0;31m[ TEST FAILED ]\n1st assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d\n", 77, 2, 78);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win);
	  return 0;
     }

     // move down 7 rows and see that the screen adjusted correctly and that the positions are correct
     for ( i = 0; i < 7; i ++) sim_pressing_S(pt, pos, lv);

     // assert that the position of everything is correct
     if (pt->curr_chr_ptr != 207){
          printf("\e[0;31m[ TEST FAILED ]\n2nd assertion failed\n\e[0m");
	  printf("Expected results :: current_chr_ptr = %d\n", 207);
	  printf("Actual results   :: current_chr_ptr = %ld\n", pt->curr_chr_ptr);

	  return 0;

     }


     // move down another 7 rows 
     for ( i = 0; i < 7; i ++) sim_pressing_S(pt, pos, lv);

     // move up another 7 rows and see that the screen adjusted correctly and that the positions are correct
     for ( i = 0; i < 7; i ++) sim_pressing_W(pt, pos, lv);
     
     // assert that the position of everything is correct
     if (pt->curr_chr_ptr != 207){
          printf("\e[0;31m[ TEST FAILED ]\n3rd assertion failed\n\e[0m");
	  printf("Expected results :: current_chr_ptr = %d\n", 207);
	  printf("Actual results   :: current_chr_ptr = %ld\n", pt->curr_chr_ptr);

	  return 0;

     }

     // move up another 7 rows and see that the screen adjusted correctly and that the positions are correct
     for ( i = 0; i < 7; i ++) sim_pressing_W(pt, pos, lv);

     // assert that the position of everything is correct
     if (pt->curr_chr_ptr != 77){
          printf("\e[0;31m[ TEST FAILED ]\n4th assertion failed\n\e[0m");
	  printf("Expected results :: current_chr_ptr = %d\n", 77);
	  printf("Actual results   :: current_chr_ptr = %ld\n", pt->curr_chr_ptr);

	  return 0;

     }
     // return back to original position and check that all is correct
     for (i = 0; i < 77; i++) sim_pressing_A(pt, pos, lv);

     if ( lv->left_win != 0 || lv->right_win != 76 || pos->x != 0 || pt->curr_chr_ptr != 0){
          printf("\e[0;31m[ TEST FAILED ]\n5th assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d, x = %d\n", 0, 0, 76, 0);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d, x = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win, pos->x);
	  return 0;

     }
     return 1;
     
}


int screen_test_scroll_right_offset_and_move_across_lines(piece_table *pt, line_view *lv, cursor_pos *pos){

     printf("screen_test_scroll_right_offset_and_move_across_lines -- ");
     int i;

     // move close to the very end of the line so that the right and left window have to be adjusted
     for (i = 0; i < 83; i++) sim_pressing_D(pt, pos, lv);


     // assert that window position is correct and the chr_ptr position is correct 
     if ( lv->left_win != 8 || lv->right_win != 84 || pt->curr_chr_ptr != 83){
	  printf("\e[0;31m[ TEST FAILED ]\n1st assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d\n", 83, 8, 84);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win);
	  return 0;
     }

     // move down 1 row and see that the screen adjusted correctly and that the positions are correct
     for ( i = 0; i < 1; i ++) sim_pressing_S(pt, pos, lv);
     
     // asssert that the position of everything is correct
     if ( lv->left_win != 8 || lv->right_win != 84 || pos->x != 13 || pt->curr_chr_ptr != 119){
          printf("\e[0;31m[ TEST FAILED ]\n2nd assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d, x = %d\n", 119, 8, 84, 13);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d, x = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win, pos->x);
	  return 0;

     }
     
     // move up two rows to return to where the column memory is and assert positioning is correct
     for (i = 0; i < 10; i++) sim_pressing_D(pt, pos, lv);

     // assert that the position of everything is correct
     if (pt->curr_chr_ptr != 129){
          printf("\e[0;31m[ TEST FAILED ]\n3rd assertion failed\n\e[0m");
	  printf("Expected results :: current_chr_ptr = %d\n", 129);
	  printf("Actual results   :: current_chr_ptr = %ld\n", pt->curr_chr_ptr);

	  return 0;

     }

     // move up 6 rows and see that the screen adjusted correctly and that the positions are correct
     for ( i = 0; i < 6; i ++) sim_pressing_W(pt, pos, lv);
     
     // asssert that the position of everything is correct
     if ( lv->left_win != 0 || lv->right_win != 76 || pos->x != 1 || pt->curr_chr_ptr != 1){
          printf("\e[0;31m[ TEST FAILED ]\n4th assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d, x = %d\n", 1, 0, 76, 1);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d, x = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win, pos->x);
	  return 0;

     }
     // move down 1 and see that the screen adjusted correctly and that the positions are correct
     for ( i = 0; i < 1; i ++) sim_pressing_A(pt, pos, lv);

     // assert that the position of everything is correct
     if (pt->curr_chr_ptr != 0){
          printf("\e[0;31m[ TEST FAILED ]\n5th assertion failed\n\e[0m");
	  printf("Expected results :: current_chr_ptr = %d\n", 0);
	  printf("Actual results   :: current_chr_ptr = %ld\n", pt->curr_chr_ptr);

	  return 0;

     }
     return 1;
}

int screen_test_moving_out_and_back_into_left_window(piece_table *pt, line_view *lv, cursor_pos *pos){
     int i;
     printf("screen_test_moving_out_and_back_into_left_window -- ");

     // move close to the very end of the line so that the right and left window have to be adjusted
     for (i = 0; i < 87; i++) sim_pressing_D(pt, pos, lv);


     // assert that window position is correct and the chr_ptr position is correct 
     if ( lv->left_win != 12 || lv->right_win != 88 || pt->curr_chr_ptr != 87){
	  printf("\e[0;31m[ TEST FAILED ]\n1st assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d\n", 77, 2, 78);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win);
	  return 0;
     }

     // move down 2 rows and see that the screen adjusted correctly and that the positions are correct
     for ( i = 0; i < 2; i ++) sim_pressing_S(pt, pos, lv);
     
     // asssert that the position of everything is correct
     if ( lv->left_win != 0 || lv->right_win != 76 || pos->x != 1 || pt->curr_chr_ptr != 121){
          printf("\e[0;31m[ TEST FAILED ]\n2nd assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d, x = %d\n", 121, 0, 76, 1);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d, x = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win, pos->x);
	  return 0;

     }
     
     // move up two rows to return to where the column memory is and assert positioning is correct
     for (i = 0; i < 2; i++) sim_pressing_W(pt, pos, lv);

     // assert that the position of everything is correct
     if (pt->curr_chr_ptr != 87){
          printf("\e[0;31m[ TEST FAILED ]\n3rd assertion failed\n\e[0m");
	  printf("Expected results :: current_chr_ptr = %d\n", 87);
	  printf("Actual results   :: current_chr_ptr = %ld\n", pt->curr_chr_ptr);

	  return 0;

     }

     // bring the cursor back to the very beginning of the file
     for(i = 0; i < 87; i++) sim_pressing_A(pt, pos, lv);

     // asssert that the position of everything is correct
     if ( lv->left_win != 0 || lv->right_win != 76 || pos->x != 0 || pt->curr_chr_ptr != 0){
          printf("\e[0;31m[ TEST FAILED ]\n4th assertion failed\n\e[0m");
	  printf("Expected results :: curr_chr_ptr = %d, left_win = %d, right_win = %d, x = %d\n", 0, 0, 76, 0);
	  printf("Actual results :: curr_chr_ptr = %ld,left_win = %d, right_win = %d, x = %d\n", 
			  pt->curr_chr_ptr, lv->left_win, lv->right_win, pos->x);
	  return 0;

     }

     // test passed
     return 1;

}

int screen_test_moving_across_and_down_to_first_line(piece_table *pt, line_view *lv, cursor_pos *pos){
     int i;
     printf("screen_test_moving_across_and_down_to_first_line -- ");

     // move to the very endd of the 1st line and then it should automatically jump down to the next
     for (i = 0; i < 98; i++) sim_pressing_D(pt, pos, lv);

     // move up one space back to the beginning of the file
     for (i = 0; i < 1; i++) sim_pressing_W(pt, pos, lv);

     // assert that chr position and x pos are both 0
     if(pos->x != 0 || pt->curr_chr_ptr != 0){
          printf("\e[0;31m[ TEST FAILED ]\e[0m\n");
	  printf("Expected results :: curr_chr_ptr = %d, x = %d\n", 0, 0);
	  printf("Actual results :: curr_chr_ptr = %ld, x = %d\n", pt->curr_chr_ptr, pos->x);
	  return 0;
     }

     return 1;
}

int screen_test_scrolling_standard(piece_table *pt, line_view *lv, cursor_pos *pos){
     
     int i;
     printf("screen_test_scrolling_standard -- ");

     // move the cursor 8 places to the right
     for (i = 0; i < 8; i++) sim_pressing_D(pt, pos, lv);
     
     // move the cursor down to the bottom of the document 
     for (i = 0; i < 15; i++) sim_pressing_S(pt, pos, lv);

     // move the cursor back up to the top 
     for (i = 0; i < 15; i++) sim_pressing_W(pt, pos, lv);

     // move the cursor back to the beginning of the document
     for (i = 0; i < 8; i++) sim_pressing_A(pt, pos, lv);
     
     // assert that chr position and x pos are both 0
     if(pos->x != 0 || pt->curr_chr_ptr != 0){
          printf("\e[0;31m[ TEST FAILED ]\e[0m\n");
	  printf("Expected results :: curr_chr_ptr = %d, x = %d\n", 0, 0);
	  printf("Actual results   :: curr_chr_ptr = %ld, x = %d\n", pt->curr_chr_ptr, pos->x);
	  return 0;
     }

     return 1;
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
     if (res == 0)
	  goto teardown;
     else  
	  printf("\e[0;32m[ TEST PASSED ]\e[0m\n"); 

     res = screen_test_scrolling_standard(&pt, &lv, &pos);
     if (res == 0)
	  goto teardown;
      else 
	  printf("\e[0;32m[ TEST PASSED ]\e[0m\n"); 

     if (screen_test_moving_out_and_back_into_left_window(&pt, &lv, &pos) == 0) 
	goto teardown;
     else
     	printf("\e[0;32m[ TEST PASSED ]\e[0m\n");

     res = screen_test_scroll_right_offset_and_move_across_lines(&pt, &lv, &pos);
     if (res == 0) 
          goto teardown;
     else	     
     	  printf("\e[0;32m[ TEST PASSED ]\e[0m\n");
     res = screen_test_scrolling_with_right_offset(&pt, &lv, &pos);
     if (res == 0) 
          goto teardown;
     else	     
     	  printf("\e[0;32m[ TEST PASSED ]\e[0m\n");

     printf("-\e[0;32m-- [SCREEN TESTS FINISHED] ---\e[0m\n\n\n");
     return 0;

teardown:
     empty_piece_table(&pt);
     exit(1);
}
