#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "piecetable.h"
#include "utils.h"

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

   pt_entry** table_ents = (pt_entry**)malloc( sizeof(pt_entry*) * DEFAULT_PT_ENT_SIZE);
   pt->table.entries = table_ents; 
   pt->table.cap = DEFAULT_PT_ENT_SIZE;

   pt_entry** redo_ents = (pt_entry**)malloc( sizeof(pt_entry*) * DEFAULT_PT_ENT_SIZE);
   pt->redo_stack.entries = redo_ents;
   pt->redo_stack.cap = DEFAULT_PT_ENT_SIZE;

   pt_entry** undo_ents = (pt_entry**)malloc( sizeof(pt_entry*) * DEFAULT_PT_ENT_SIZE);
   pt->undo_stack.entries = undo_ents; 
   pt->undo_stack.cap = DEFAULT_PT_ENT_SIZE;

   // add the first element - which is the original buffer 
   pt_entry* org_entry = (pt_entry*)malloc( sizeof(pt_entry) );
   org_entry->src = ORG; 
   org_entry->start = 0; 
   org_entry->len = f_size; 

   pt->table.entries[0] = org_entry;
   pt->table.curr_ent_num++;  

   return 0; 

}

void empty_piece_table(piece_table* pt){

   safeFree(pt->original.text);
   safeFree(pt->addition.text);

   int i; 
   for (i = 0; i < pt->table.cap; i++)
      safeFree(pt->table.entries[i]);
   
   for (i = 0; i < pt->redo_stack.cap; i++)
      safeFree(pt->redo_stack.entries[i]);

   for(i = 0; i < pt->undo_stack.cap; i++)
      safeFree(pt->undo_stack.entries[i]);

   safeFree(pt->table.entries);
   safeFree(pt->redo_stack.entries);
   safeFree(pt->undo_stack.entries);

}