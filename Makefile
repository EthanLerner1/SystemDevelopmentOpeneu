CC = gcc
CFLAGS_COMMON  = -Wall -Wextra -pedantic -ansi
CFLAGS_DEBUG   = -g -O0
CFLAGS_RELEASE = -O2

DEBUG ?= 0
ARGS  ?=

ifeq ($(DEBUG),1)
CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
else
CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
endif

LDFLAGS =
TARGET = asmbler

BUILD_DIR := build
SRC_DIR   := src
OBJ_DIR   := $(BUILD_DIR)/obj
BIN       := $(BUILD_DIR)/$(TARGET)

# Recursive source discovery (works for src/ and src/utils/, etc.)
SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')

# Map src/path/file.c -> build/obj/path/file.o   (strip "src/")
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(BIN)

run: $(BIN)
	./$(BIN) $(ARGS)

$(BIN): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Build rule that supports subdirectories (e.g. utils/string_utils.c)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
