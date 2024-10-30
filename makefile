CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic \
		 -std=c99 \
		 -O2 \


###############################################################
### debugging flags ###
###############################################################

# DEBUG_GENERAL_TOGGLE has to always be set as '1' for other debugging flags to toggled 

DEBUG_GENERAL_TOGGLE = 1

DEBUG_INSERT_TOGGLE = 
DEBUG_DELETE_TOGGLE = 
DEBUG_MOVEMENT_TOGGLE = 1
DEBUG_PT_TOGGLE = 1
DEBUG_SCREEN_TOGGLE = 1

ifeq ($(DEBUG_GENERAL_TOGGLE), 1)
CFLAGS+=-g -D DEBUG_GEN
endif 

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_INSERT_TOGGLE), 11)
CFLAGS+= -D DEBUG_INSERT
endif

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_DELETE_TOGGLE), 11)
CFLAGS+= -D DEBUG_DELETE
endif

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_MOVEMENT_TOGGLE), 11)
CFLAGS+= -D DEBUG_MOVE
endif

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_PT_TOGGLE), 11)
CFLAGS+= -D DEBUG_PT
endif 

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_SCREEN_TOGGLE), 11)
CFLAGS += -D DEBUG_SCREEN
endif

###############################################################

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
	