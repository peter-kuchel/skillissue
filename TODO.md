# TODO (and possible future functionality): 

* handle deletes

* handling redo and undo 
    * stack functions to handle the arrays
    * figuring out when a redo is no longer valid and removing it from the stack 

* track position of cursor so that it moves correctly on the front end side of things 
    * handle finding new lines to increment y 
    * handle keeping the cursor in the same column when going up and down 
    * bounds checking 
    * rendering only text within the terminal box, based off of the size and position within the file
    * handling the cursor position on a line where it is just '\n'
        * keep a record of lines using indexes into the buffers

* line numbering and formating 
* selecting text ( H? ) to do copy, cut, and paste ( C, X, P)
* something that displays the command that has been toggled (similar to vim?)
* jumping to a certain line number in the document
* during a save, re-building the piece table (?) (so to stop future allocations and potential performance degration?)
* searching the file for certain words
* text colouring - for stuff such as int, {}, [], etc.