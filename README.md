# SKILLISSUE

An easy to use cli text-editor for the vscode soy-devs like myself who don't know how to type properly on a keyboard, and can't exit from Vim.

*NOTE* This software is unfinished, please don't have any high expectations. See TODO.md for upcoming changes

# Requirements: 

Requires gcc 7.5.0 or greater, and ncurses plus associated headerfiles. 

# Installing: 

Git clone the repo, navigate into the directory and run ```make```. This will compile everything into a single binary which will be found in a created directory named ```bin```

# Functionality:    

The editor is very basic, with some surface-level similarity to existing cli editors. Using the following keys, you can:

### Quit [ q ]
Exit the currently opened file (we did it!)

### Move [ w, a, s, d ]:
Move around the currently opened document with these keys

### Insert [ i ]:
Toggle inserting / deleting characters, exit mode with ESC 

## Coming soon: 

### Save [ z ]:
Save the current changes made to the file

### Redo [ j ] & Undo [ k ]

### Select [ h ]

### Copy [ c ], Cut [ x ], & Paste [ p ]
