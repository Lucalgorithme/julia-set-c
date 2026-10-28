CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -fsanitize=address,undefined
LDLIBS = -lm

.PHONY: all julia batch clean

all: julia batch

julia:
	@mkdir -p bin
	$(MAKE) -C julia

batch: julia
	@mkdir -p bin
	$(MAKE) -C batch

clean:
	rm -rf bin
	$(MAKE) -C julia clean
	$(MAKE) -C batch clean