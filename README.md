# SKILLISSUE

An easy to use cli text-editor for the vscode soydevs such as myself who don't know how to type properly on a keyboard and can't exit from Vim.

# How to use    

The editor has very limited commands and has some similarity to existing cli editors:

### Insert (i):
Insert characters at the current position of the cursor, exit with ESC 

### Save (z):
Save the changes made to the file

### Redo (j):
Redo the most recent change

### Undo (k):
Undo the most recent change

### Move (w,a,s,d):
Move around the currently opened document with these keys 

# TODO: 

* track position of cursor so that it moves correctly on the front end side of things 
    * handle finding new lines to increment y 
    * handle keeping the cursor in the same column when going up and down 
    * bounds checking 
    * rendering only text within the terminal box, based off of the size 
    * handling the cursor position on a line where it is just '\n'

* handling insertions and the organization of the piece table 
    * handle inserts by placing the possibly to be added chars into a buffer while insert is toggled
    * once insert is no longer toggled add them to the additions buffer 
    * handle creating the new piece table entry along with handling how the neighbouring entries might change 
    * handle re-organizing the entries so that everything is in order 
        * start by adding the first entry in the middle to make shifting easier by not having to shift all ptrs (?)

    * handle re-sizing when cap is met for entries and additions 
        * (general realloc function using macros to handle the types)

* handling redo and undo 
    * stack functions to handle the arrays
    * figuring out when a redo is no longer valid and removing it from the stack 

* line numbering and formating 
* something that displays the cmd that has been toggled (similar to vim?)