CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -fsanitize=address,undefined -Iinclude
LDLIBS = -lm

all: julia tests_julia

julia: src/julia.c src/main.c include/julia.h
	$(CC) $(CFLAGS) src/julia.c src/main.c -o ../bin/julia $(LDLIBS)

tests_julia: src/julia.c tests/tests.c include/julia.h
	$(CC) $(CFLAGS) src/julia.c tests/tests.c -o tests_julia $(LDLIBS)
	./tests_julia

clean:
	rm -f tests_julia *.ppm