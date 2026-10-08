CC      = cc
CFLAGS  = -Wall -Wextra -Werror -std=c99 -Isrc
# LDFLAGS = -lm

SRC     = src/main.c
OBJ     = build/main.o
OUT     = bin/main
# HEADERS = src/main.h

all: dirs $(OUT)

dirs:
	mkdir -p build bin

build/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -rf build bin

.PHONY: all dirs run clean
