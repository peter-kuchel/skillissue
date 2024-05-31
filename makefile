CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic \
		 -std=c99 \
		 -g \
		 -O2

DEBUG_INSERT_TOGGLE = 0
DEBUG_MOVEMENT_TOGGLE = 0 
DEBUG_PT_TOGGLE = 0
DEBUG_GENERAL_TOGGLE = 1

ifeq ($(DEBUG_INSERT_TOGGLE), 1)
CFLAGS+= -D DEBUG_INSERT
endif

ifeq ($(DEBUG_MOVEMENT_TOGGLE), 1)
CFLAGS+= -D DEBUG_MOVE
endif

ifeq ($(DEBUG_PT_TOGGLE), 1)
CFLAGS+= -D DEBUG_PT
endif 

ifeq ($(DEBUG_GENERAL_TOGGLE), 1)
CFLAGS+= -D DEBUG_GEN
endif 

NCURSES = -lncurses

SRC = src
BIN = bin
TARGET = skillissue


all: mk-bin sk 

mk-bin:
	mkdir -p bin

sk: 
	$(CC) $(CFLAGS) \
	$(SRC)/cursor.c \
	$(SRC)/logging.c \
	$(SRC)/piecetable.c \
	$(SRC)/pt_delete.c \
	$(SRC)/pt_insert.c \
	$(SRC)/pt_realloc.c \
	$(SRC)/pt_stacks.c \
	$(SRC)/screen.c \
	$(SRC)/skillissue.c \
	$(SRC)/utils.c \
	$(NCURSES) \
	-o $(BIN)/$(TARGET)