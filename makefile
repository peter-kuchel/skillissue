CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic \
		 -std=c99 \
		 -O2 \

CFLAGS_DEBUG =
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
DEBUG_RENDER_TOGGLE = 

ifeq ($(DEBUG_GENERAL_TOGGLE), 1)
CFLAGS_DEBUG+=-g -D DEBUG_GEN
endif 

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_INSERT_TOGGLE), 11)
CFLAGS_DEBUG+= -D DEBUG_INSERT
endif

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_DELETE_TOGGLE), 11)
CFLAGS_DEBUG+= -D DEBUG_DELETE
endif

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_MOVEMENT_TOGGLE), 11)
CFLAGS_DEBUG+= -D DEBUG_MOVE
endif

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_PT_TOGGLE), 11)
CFLAGS_DEBUG+= -D DEBUG_PT
endif 

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_SCREEN_TOGGLE), 11)
CFLAGS_DEBUG += -D DEBUG_SCREEN
endif

ifeq ($(DEBUG_GENERAL_TOGGLE)$(DEBUG_RENDER_TOGGLE),11)
CFLAGS_DEBUG += -D DEBUG_RENDER
endif

###############################################################

NCURSES = -lncurses

SRC = src
BIN = bin
TEST = tests

TARGET = $(BIN)/skillissue
SRC_FILES = $(wildcard $(SRC)/*.c)

OBJ_FILES = $(SRC_FILES:$(SRC)/%.c=$(BIN)/%.o)

TEST_TARGET = $(TEST)/run_tests
TEST_FILES = $(wildcard $(TEST)/*.c)
FILTERED_OBJS = $(subst bin/skillissue.o ,, $(OBJ_FILES))
TEST_OBJS = $(TEST_FILES:$(TEST)/%.c=$(BIN)/%.o)


all: mk-bin $(TARGET) 

mk-bin:
	mkdir -p $(BIN)

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(NCURSES)

$(BIN)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) $< -o $@  

tests: all $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(FILTERED_OBJS) 
	$(CC) -o $@ $^ $(NCURSES)

$(BIN)/%.o: $(TEST)/%.c
	$(CC) -c -Wall -std=c99 -O2 $(CFLAGS_DEBUG) $< -o $@
clean:
	-rm -f $(OBJ_FILES) $(TARGET)
	-rm -f $(TEST_OBJS) $(TEST_TARGET)
	
