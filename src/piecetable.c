#include "piecetable.h"


int init_piece_table(FILE* f, char* fn, piece_table* pt){
   
   size_t f_size = file_size(fn);

   char* org_buf = (char*)malloc(f_size * sizeof(char));
   char* add_buf = (char*)malloc(STARTING_ADD_BUF_SIZE * sizeof(char));

   fread(org_buf, sizeof(char), f_size, f);

   if (ferror(f) != 0) return -1;

   memset((char*)pt, 0, sizeof(piece_table));

   pt->original.text = org_buf; 
   pt->original.size = f_size; 

   pt->addition.text = add_buf; 
   pt->addition.size = STARTING_ADD_BUF_SIZE; 

   int* _orger = (int*)malloc(sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->table.organizer = _orger; 
   pt->table.ent_cap = DEFAULT_PT_ENT_SIZE;
   pt->table.middle = (pt->table.ent_cap >> 1) - 1; 

   pt_entry*  table_ents = (pt_entry*)malloc( sizeof(pt_entry) * DEFAULT_PT_ENT_SIZE);
   pt->table.entries = table_ents; 
   pt->table.ent_cap = DEFAULT_PT_ENT_SIZE;

   // pt_entry** redo_ents = (pt_entry**)malloc( sizeof(pt_entry*) * DEFAULT_PT_ENT_SIZE);
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

   int entry_pos = pt->table.ent_num;       // will be 0 because of memset

   pt_entry* org_entry = &(pt->table.entries[entry_pos]);

   org_entry->src = ORG; 
   org_entry->start = 0; 
   org_entry->len = f_size;

   pt->table.organizer[pt->table.middle] = entry_pos;
   pt->table.org_head = pt->table.middle; 
   pt->table.org_num++;  

   pt->curr_ent_ptr = entry_pos;

   pt->table.ent_num++; 

   return 0; 

}

void empty_piece_table(piece_table* pt){

   safeFree(pt->original.text);
   safeFree(pt->addition.text);

   safeFree(pt->table.organizer);
   safeFree(pt->table.entries); 

   safeFree(pt->redo.stack);
   safeFree(pt->undo.stack);
   safeFree(pt->reclaim.stack);

}