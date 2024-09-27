CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic \
		 -std=c99 \
		 -g \
		 -O2

### debugging flags ###

# this one has to always be enabled if debugging 
# if debugging is turned off then clean needs to be ran

DEBUG_GENERAL_TOGGLE = 

DEBUG_INSERT_TOGGLE = 
DEBUG_DELETE_TOGGLE = 
DEBUG_MOVEMENT_TOGGLE = 
DEBUG_PT_TOGGLE = 
DEBUG_SCREEN_TOGGLE = 


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

OBJ_FILES = $(SRC_FILES:$(SRC)/%.c=$(BIN)/%.o)

all: mk-bin $(TARGET) 

mk-bin:
	mkdir -p $(BIN)

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(NCURSES)


$(BIN)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $< -o $@  


clean:
	-rm -f $(OBJ_FILES) $(TARGET)
	