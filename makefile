CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic \
		 -std=c99 \
		 -g \
		 -O2

# for debugging 
DEBUG_INSERT_TOGGLE = 1
DEBUG_DELETE_TOGGLE = 1
DEBUG_MOVEMENT_TOGGLE =
DEBUG_PT_TOGGLE = 1
DEBUG_GENERAL_TOGGLE = 1
DEBUG_SCREEN_TOGGLE = 1


ifeq ($(DEBUG_INSERT_TOGGLE), 1)
CFLAGS+= -D DEBUG_INSERT
endif

ifeq ($(DEBUG_DELETE_TOGGLE), 1)
CFLAGS+= -D DEBUG_DELETE
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

ifeq ($(DEBUG_SCREEN_TOGGLE), 1)
CFLAGS += -D DEBUG_SCREEN
endif

NCURSES = -lncurses

SRC = src
BIN = bin

TARGET = $(BIN)/skillissue
SRC_FILES = $(wildcard $(SRC)/*.c)
# SRC_FILES := $(shell find $(SRC) -name '*.c')

OBJ_FILES = $(SRC_FILES:$(SRC)/%.c=$(BIN)/%.o)

all: mk-bin $(TARGET) 

mk-bin:
	mkdir -p bin

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(NCURSES)


$(BIN)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $< -o $@  


clean:
	-rm -f $(BIN)/*.o $(BIN)/skillissue
	