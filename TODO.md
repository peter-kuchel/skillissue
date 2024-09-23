# TODO (and possible future functionality) in no particular order: 

* high priority 


* medium priority 
    
    * moving left and right on a line when the line size is greater than the window length 
    
    * implement using the line reclaim when a line is deleted 
        * if it has not already been implemented

    * handling redo and undo 
        * stack functions to handle the arrays
        * figuring out when a redo is no longer valid and removing it from the stack 

    * add being able delete using the 'del' key

    * something that displays the command that has been toggled (similar to vim?)
        
* low priority 
    
    * add being able to use the arrow keys to move when in insertion mode 

    * re-factor delete exhaustion so that it doesn't make a new entry until
      there is a confirmed delete (?)

    * fixed size buffer for printing to the screen(?)
        * calculate the buffer size using the max screen size 
            * dynamically alter this size when the screen is resized

    * selecting text ( H? ) to do copy, cut, and paste ( C, X, P)

    * line numbering and formating 
    * text colouring - for stuff such as 'int', {}, [], etc.
    * jumping to a certain line number in the document

    * searching the file for certain words

    * grammar correction 
