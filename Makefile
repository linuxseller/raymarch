CFLAGS=-Werror -Wall -Wextra -std=c90 -pedantic
CLIBS=
all:
	clang $(CFLAGS) main.c -o main $(CLIBS)
run: all
	./main
