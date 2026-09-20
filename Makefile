CC := gcc
CFLAGS := -Wall -Wextra -g -Iinclude
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,obj/%.o,$(SRC))
BIN := bin/shell

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJ)

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj bin:
	mkdir -p $@

clean:
	rm -rf obj bin
