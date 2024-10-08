#include "piecetable.h"



int init_piece_table(FILE* f, char* fn, piece_table* pt){
   
   size_t f_size = file_size(fn);

   #ifdef DEBUG_PT
      memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Size of %s]: %ld\n", fn, f_size);
        log_to_file(&sk_logger, pbuf);
   #endif 

   char* org_buf = (char*)malloc( (f_size + 1) * sizeof(char) );
   char* add_buf = (char*)malloc(STARTING_ADD_BUF_SIZE * sizeof(char));

   size_t bytes_read = fread(org_buf, sizeof(char), f_size, f);

   if (bytes_read < f_size){
      perror("Something went wrong reading from the file"); 
      return -1;
   }

   if (ferror(f) != 0) return -1;

   memset((char*)pt, 0, sizeof(piece_table));

   pt->original.text = org_buf; 
   pt->original.size = f_size; 

   pt->addition.buf.text = add_buf; 
   pt->addition.buf.size = STARTING_ADD_BUF_SIZE; 

   pt_entry*  table_ents = (pt_entry*)malloc( sizeof(pt_entry) * DEFAULT_PT_ENT_SIZE);
   pt->entries = table_ents; 
   pt->ent_cap = DEFAULT_PT_ENT_SIZE;

   // line* file_lines = (line*)malloc( sizeof(line) *)

   line* _lines = (line*)malloc( sizeof(line) * DEFAULT_LINES_SIZE);
   pt->lh.lines = _lines; 
   pt->lh.cap = DEFAULT_LINES_SIZE; 

   int* _line_reclaim = (int*)malloc( sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->lh.line_reclaim.stack = _line_reclaim; 
   pt->lh.line_reclaim.cap = DEFAULT_PT_ENT_SIZE; 

   int* redo_ents = (int*)malloc( sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->redo.stack = redo_ents;
   pt->redo.cap = DEFAULT_PT_ENT_SIZE;

   int* undo_ents = (int*)malloc( sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->undo.stack = undo_ents; 
   pt->undo.cap = DEFAULT_PT_ENT_SIZE;

   int* reclaim_spots = (int*)malloc( sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->reclaim.stack = reclaim_spots;
   pt->reclaim.cap = DEFAULT_PT_ENT_SIZE;

   // add the first element - which is the original buffer 

   int first_entry = pt->ent_num;       // will be 0 because of memset
   pt->ent_num++;
   pt_entry* orgn_entry = &(pt->entries[first_entry]);

   orgn_entry->src = ORGN; 
   orgn_entry->start = 0; 
   orgn_entry->len = f_size;

   orgn_entry->left = NULL_ENT;
   orgn_entry->right = NULL_ENT; 

   pt->ent_head = first_entry; 
   pt->ent_tail = first_entry;

   pt->curr_ins_ent = NULL_ENT;
   pt->curr_del_ent = NULL_ENT;

   return 0; 

}

void empty_piece_table(piece_table* pt){

   safeFree(pt->original.text);
   safeFree(pt->addition.buf.text);
   
   safeFree(pt->entries); 
   safeFree(pt->lh.lines);

   safeFree(pt->redo.stack);
   safeFree(pt->undo.stack);
   safeFree(pt->reclaim.stack);

}

void log_piece_table_current(Logger* logger, piece_table* pt){
   
   int pbuf_i; 
   
   add_buffer_t* adds = &(pt->addition);
   log_to_file(logger, "----------------------\nCurrent additions state:\n[");

   pbuf_i = 0;
   memset(pbuf, 0, PBUF_SIZE);
   for (size_t j = 0; j < adds->curr_pos; j++){
      
      pbuf[pbuf_i] = adds->buf.text[j];
      pbuf_i++;

      if ( (j > 0 && j % PBUF_SIZE == 0) || j + 1 == adds->curr_pos){

         log_to_file(logger, pbuf);
         memset(pbuf, 0, PBUF_SIZE);
         pbuf_i = 0; 
      }
      
   }
   log_to_file(logger, "]\n");

   memset(pbuf, 0, PBUF_SIZE); 
   sprintf(pbuf, "\nhead: %d\ntail: %d\ncurr_ent_ptr: %d\ndel_ent: %d\nins_ent: %d\nchr_ptr: %ld\n",
               pt->ent_head, pt->ent_tail, pt->curr_ent_ptr, pt->curr_del_ent, pt->curr_ins_ent,
               pt->curr_chr_ptr
   );

   log_to_file(logger, pbuf);


   int i = 0; 
   int _ent = pt->ent_head; 
   // int _tail = tb->org_tail;
   log_to_file(logger, "\nCurrent piece table state:\n");

   pt_entry* ent; 

   int all_ent_ptrs[pt->ent_num];
   do {
      memset(pbuf, 0, PBUF_SIZE);

      ent = &(pt->entries[_ent]);
      sprintf(pbuf, "[%d] @ ( %d | %s, %ld, %ld | %d ) @ [%d]\n",
               _ent,
               ent->left, 
               ent->src == ORGN ? "ORG": "ADD",
               ent->start,
               ent->len, 
               ent->right, _ent  
      );
      log_to_file(logger, pbuf);

      all_ent_ptrs[i] = _ent; 
      i++; 
      _ent = ent->right;
      
   }while (_ent != NULL_ENT);

   int size = i; 

   log_to_file(logger, "\nCurrent organizer state:\n[");
   for (i = 0; i < size; i++){
      memset(pbuf, 0, PBUF_SIZE);
      if (all_ent_ptrs[i] == pt->ent_tail) 
         sprintf(pbuf, "%d", all_ent_ptrs[i]);
      else 
         sprintf(pbuf, "%d, ", all_ent_ptrs[i]);

      log_to_file(logger, pbuf);
   }

   log_to_file(logger, "]\n");

   log_to_file(logger, "Current undo stack state:\n[");
   for (i = 0; i < pt->undo.ptr; i++){
      memset(pbuf, 0, PBUF_SIZE);
      if (i == pt->undo.ptr - 1) 
         sprintf(pbuf, "%d", pt->undo.stack[i]);
      else 
         sprintf(pbuf, "%d, ", pt->undo.stack[i]);

      log_to_file(logger, pbuf);
   }

   log_to_file(logger, "]\n\n");

   log_to_file(logger, "Lines state: \n");

   line_handler* _lh = &(pt->lh);
   // line all_lines[_lh->size];

   memset(pbuf, 0, PBUF_SIZE);
   char _c = PTR_AT_CHR(pt, pt->curr_chr_ptr);
   sprintf(pbuf, "top: %d\nbottom: %d\ncurr: %d | chr_ptr: %c\n\n", _lh->top_line, _lh->bottom_line, _lh->curr_line, _c);
   log_to_file(logger, pbuf);

   int _l = _lh->top_line; 
   // int i = 0; 
   line* _line; 

   do {  
      _line = &(_lh->lines[_l]);

      memset(pbuf, 0, PBUF_SIZE);
      sprintf(pbuf, "[%d] @ ( p: %d | sz: %d| n: %d ) @ [%d]\n", _l, _line->prev_line, _line->line_size, _line->next_line, _l );
      log_to_file(logger, pbuf);

      _l = _line->next_line; 
   } while (_l != NULL_LINE);

   log_to_file(logger, "\n\n----------\n");

}