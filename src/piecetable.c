#include <stdio.h>
#include <stdlib.h>

#include "piecetable.h"
#include "utils.h"

int init_piece_table(FILE* f, char* fn, piece_table* pt){

     size_t fsize = file_size(fn); 

     char* org_buf = (char*)malloc(sizeof(char) * fsize);
     char* add_buf = (char*)malloc(sizeof(char) * fsize);

     fread(org_buf, sizeof(char), fsize, f);

     // check that fread was successful 
     if (ferror(f) != 0) return -1; 

     text_buffer* org = (text_buffer*)malloc(sizeof(text_buffer));
     text_buffer* add = (text_buffer*)malloc(sizeof(text_buffer));

     org->text = org_buf; 
     org->size = fsize;
     org->curr_pos = -1;         /* -1 since original will be read only (though this doesn't actually matter) */

     add->text = add_buf;
     add->size = fsize; 
     add->curr_pos = 0; 

      pt->original_buffer = org; 
      pt->add_buffer = add; 
      pt->pte_size = DEFAULT_PT_ELEM_SIZE;
      pt->table = (piece_table_element*)malloc(sizeof(piece_table_element) * pt->pte_size); 
      pt->start_ins_chr = '\0';
      pt->start_ins_pos = 0;

      pt->pte_curr = 1; 

      /* first element in the piece table should be the original buffer */

      pt->table[0].src = ORG; 
      pt->table[0].start = 0;
      pt->table[0].len = (int)fsize;

     
   return 0;
}

void empty_piece_table(piece_table* pt){

   safeFree(pt->add_buffer->text);
   safeFree(pt->original_buffer->text);

   safeFree(pt->table);
   safeFree(pt->add_buffer);
   safeFree(pt->original_buffer);

}

int resize_pt_elements(piece_table_element* table){
   printf("%p", table);

   return 0; 
}

int resize_add_buffer(text_buffer* add_buff){
   char* curr_buff = add_buff->text; 

   size_t new_size = add_buff->size << 1; 

   char* new_loc = (char*)realloc(curr_buff, new_size);

   if (new_loc == NULL) 
      return -1; 

   add_buff->text = new_loc; 
   add_buff->size = new_size;

   return 0; 
}