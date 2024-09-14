# TODO (and possible future functionality) in no particular order: 

* high priority 
    * fix deleting first line from top of document
    * have the screen render down when inserting newlines 
    * fix small bugs still present when adding new lines

* medium priority 
    * implement using the line reclaim when a line is deleted 
        * if it has not already been implemented
        
* low priority 

    * re-factor delete exhaustion so that it doesn't make a new entry until
      there is a confirmed delete

    * fixed size buffer for printing to the screen(?)
        * calculate the buffer size using the max screen size 
            * dynamically alter this size when the screen is resized

    * handling redo and undo 
        * stack functions to handle the arrays
        * figuring out when a redo is no longer valid and removing it from the stack 

    * selecting text ( H? ) to do copy, cut, and paste ( C, X, P)

    * line numbering and formating 
    * text colouring - for stuff such as 'int', {}, [], etc.
    * jumping to a certain line number in the document

    * something that displays the command that has been toggled (similar to vim?)

    * searching the file for certain words

    * grammar correction 
