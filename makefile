CC = gcc -g

CFLAGS = -Wall -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXECUTABLE = $(BIN_DIR)/mysh

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)

distclean: clean
	rm -f $(SRC_DIR)/command_log.txt $(INCLUDE_DIR)/command_log.txt

.PHONY: all clean distclean
