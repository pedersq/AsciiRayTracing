CC=gcc
CFLAGS=-I.
DEPS = vectors.h OBJ.h
OBJ = vectors.o OBJ.o gray_ascii.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sim: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

clean:
	rm -f OBJ.o vectors.o sim gray_ascii.o
