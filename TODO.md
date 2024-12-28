# TODO (and possible future functionality): 
-- anything with (?) means might not be an issue if is a bug or not necessary if refactoring / feature

* __high priority__ 
    * moving left and right on a line when the line size is greater than the window length 
        * make sure cursor ends up in the correct position on the RHS when scrolling 
            * (such as when the line is cutoff on the right --> going to the correct col memory position)
        * make sure inserting & deleting works when document is offset to the right
        
    * fix small bug where cursor position on display doesn't match actual when moving the window view to the right
    * fix small bug when logger is toggled off then a segfault happens

* __medium priority__ 
    
    * sucessfully save the file with the [z] key

    * re-set terminal info when the screen gets resized
    
    * implement using the line reclaim when a line is deleted 
        * if it has not already been implemented -- it has not been

    * handling redo and undo 
        * stack functions to handle the arrays
        * figuring out when a redo is no longer valid and removing it from the stack 

    * add being able to delete using the 'del' key

    * handling tabs in general
        * moving over tabs to make the cursor jump (?)
            * maybe just have the tab create 4 spaces (will be an easier solution for the time being)
        * moving the cursor appropriately when inserting and deleting
        
* __low priority__ 
    
    * add being able to use the arrow keys to move when in insertion mode 
    * fix minor bug where after saving the mode isn't set after the next key pressed

    * re-factor delete exhaustion so that it doesn't make a new entry until
      there is a confirmed delete (?)

    * fix small bug with inserts not displaying (? maybe not an issue anymore ?) 

    * fixed size buffer for printing to the screen(?)
        * calculate the buffer size using the max screen size 
            * dynamically alter this size when the screen is resized

    * selecting text ( H? ) to do copy, cut, and paste ( C, X, P)

    * line numbering and formating 
    * text colouring - for stuff such as 'int', {}, [], etc.
        * own custom rules depending on the file extension being opened
        
    * jumping to a certain line number in the document

    * searching the file for certain words
