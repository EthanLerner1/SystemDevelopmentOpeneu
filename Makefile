CC = gcc
CFLAGS_COMMON = -Wall -Wextra -pedantic -ansi
CFLAGS_DEBUG  = -g -O0
CFLAGS_RELEASE = -O2

DEBUG ?= 0

ifeq ($(DEBUG),1)
    CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
else
    CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
endif

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

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
