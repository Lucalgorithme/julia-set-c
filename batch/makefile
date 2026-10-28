CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -g -fsanitize=address,undefined -Iinclude -I../julia/include
LDLIBS = -lm

all: batch tests_batch

batch: src/batch.c ../julia/src/julia.c src/main_batch.c include/batch.h ../julia/include/julia.h
	$(CC) $(CFLAGS) src/batch.c ../julia/src/julia.c src/main_batch.c -o ../bin/batch $(LDLIBS)

tests_batch: src/batch.c ../julia/src/julia.c tests/tests_batch.c include/batch.h ../julia/include/julia.h
	rm -rf t6_ppm t6_png t7_ppm t7_png
	$(CC) $(CFLAGS) src/batch.c ../julia/src/julia.c tests/tests_batch.c -o tests_batch $(LDLIBS)
	./tests_batch

clean:
	rm -f tests_batch t6_ppm t6_png t7_ppm t7_png