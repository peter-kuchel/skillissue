SRC = src
BIN = bin

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -g
NCURSES = -lncurses

TARGET = skillissue


all: mk-bin sk 

mk-bin:
	mkdir -p bin

sk: 
	$(CC) $(CFLAGS) $(SRC)/piecetable.c $(SRC)/skillissue.c -o $(BIN)/skillissue $(NCURSES)