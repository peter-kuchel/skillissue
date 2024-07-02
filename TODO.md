# TODO (and possible future functionality) in not particular order: 

* re-factor delete exhaustion so that it doesn't make a new entry until
  there is a confirmed delete

* track position of cursor so that it moves correctly on the front end side of things 
    * rendering only text within the terminal box, based off of the size and position within the file
        * text view changing with terminal resizing

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
