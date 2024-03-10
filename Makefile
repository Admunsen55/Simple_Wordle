CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -lncurses

SRC = wordle.c
EXEC = wordle_game

all: $(EXEC)

$(EXEC): $(SRC)
    $(CC) $(CFLAGS) $^ -o $@

clean:
    rm -f $(EXEC)
