CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ansi
LDFLAGS =
TARGET = asmbler

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN := $(BUILD_DIR)/$(TARGET)

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean run

all: $(BIN)

run: $(BIN)
	./$(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile sources into build/obj
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)