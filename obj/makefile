
CC = gcc


CFLAGS = -Wall -Wextra -Iinclude


SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXECUTABLE = $(BIN_DIR)/mysh

# Default target
all: $(EXECUTABLE)

# Link the executable
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@

# Compile the source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)

# Clean up build artifacts and temporary files
distclean: clean
	rm -f $(SRC_DIR)/command_log.txt $(INCLUDE_DIR)/command_log.txt

# Phony targets
.PHONY: all clean distclean
