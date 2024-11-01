# TODO (and possible future functionality): 
-- anything with (?) means might not be an issue if is a bug or not necessary if refactoring / feature

* __high priority__ 

    * moving left and right on a line when the line size is greater than the window length 
        * fix moving down lines and keeping cursor position in the correct place
        * test to make sure addition entries work with scrolling to the sides
        * make sure inserting & deleting works when document is offset to the right
        
    * fix small bug where cursor position on display doesn't match actual when moving the window view to the right
    * fix small bug when logger is toggled off then a segfault happens


* __medium priority__ 

    * re-set terminal info when the screen gets resized
    
    * implement using the line reclaim when a line is deleted 
        * if it has not already been implemented

    * handling redo and undo 
        * stack functions to handle the arrays
        * figuring out when a redo is no longer valid and removing it from the stack 

    * add being able to delete using the 'del' key

    * fix small bug where the cursor position sometimes ends up in the wrong spot (non consistent bug) (?)
        * bug seems to happen when scrolling down to the very bottom of the document

    * handling tabs in general
        * moving over tabs to make the cursor jump (?)
            * maybe just have the tab create 4 spaces (will be an easier solution for the time being)
        * moving the cursor appropriately when inserting and deleting
        
* __low priority__ 
    
    * add being able to use the arrow keys to move when in insertion mode 

    * re-factor delete exhaustion so that it doesn't make a new entry until
      there is a confirmed delete (?)

    * fix small bug with inserts not displaying (? maybe not an issue anymore ?) 
    
    * fix small bug with writing info at the bottom (?)

    * fixed size buffer for printing to the screen(?)
        * calculate the buffer size using the max screen size 
            * dynamically alter this size when the screen is resized

    * selecting text ( H? ) to do copy, cut, and paste ( C, X, P)

    * line numbering and formating 
    * text colouring - for stuff such as 'int', {}, [], etc.
    * jumping to a certain line number in the document

    * searching the file for certain words

    * grammar correction 
