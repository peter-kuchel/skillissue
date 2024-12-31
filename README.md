# SKILLISSUE

An easy to use cli text-editor for the vscode soy-devs like myself who don't know how to type properly on a keyboard, and can't exit from Vim.

_**`NOTE: This software is unfinished, please don't have any high expectations.`**_ 
See TODO.md for upcoming changes and bug fixes.

# Requirements: 

Requires gcc 7.5.0 or greater, and ncurses plus it's associated headerfiles. To install ncurses see [here](https://www.cyberciti.biz/faq/linux-install-ncurses-library-headers-on-debian-ubuntu-centos-fedora/)

# Installing: 

Git clone the repo, navigate into the directory and run ```make```. This will compile everything into a single binary which will be found in a created directory named ```bin```

# Functionality:    

The editor is very basic, with some surface-level similarity to existing cli editors. Using the following keys, you can:

### Quit [ q ]
Exit the currently opened file (we did it!)

### Move [ w, a, s, d ]:
Move around the currently opened document with these keys

### Insert [ i ]:
Toggle inserting / deleting characters. Currently only deleting with the 'backspace' key is supported at the moment. Exit insert mode with ESC. 


### Save [ z ]:
Save the current changes made to the file

## Coming soon: 

### Redo [ j ] & Undo [ k ]

### Select [ h ]

### Copy [ c ], Cut [ x ], & Paste [ p ]
