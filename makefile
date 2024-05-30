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
	$(CC) $(CFLAGS) \
	$(SRC)/utils.c \
	$(SRC)/piecetable.c \
	$(SRC)/screen.c \
	$(SRC)/skillissue.c \
	$(SRC)/logging.c \
	$(SRC)/cursor.c \
	$(SRC)/pt_insert.c \
	$(SRC)/pt_realloc.c \
	$(SRC)/pt_stacks.c \
	$(NCURSES) \
	-o $(BIN)/$(TARGET)