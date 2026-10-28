CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -fsanitize=address,undefined -Iinclude
LDLIBS = -lm

all: julia tests

julia: src/julia.c src/main.c include/julia.h
	$(CC) $(CFLAGS) src/julia.c src/main.c -o ../bin/julia $(LDLIBS)

tests: src/julia.c tests/tests.c include/julia.h
	$(CC) $(CFLAGS) src/julia.c tests/tests.c -o tests $(LDLIBS)
	./tests

clean:
	rm -f  *.ppm