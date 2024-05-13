CFLAGS=-Werror -Wall -Wextra -std=c99 -pedantic
CLIBS=-lm
all:
	clang $(CFLAGS) main.c -o main $(CLIBS)
run: all
	./main
