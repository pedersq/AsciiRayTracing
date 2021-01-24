CCFLAGS=-g
CC=gcc

all: sim

sim: gray_ascii.c
	$(CC) $(CCFLAGS) -o sim gray_ascii.c vectors.c OBJ.c -l m

clean:
	rm -f sim
