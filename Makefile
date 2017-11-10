
CC=gcc
CFLAGS=-O3 -g -fopenmp

DEPS=global.h functions.h

OBJ=write_matrix.o read_matrix.o mul_matrix.o

matrix: $(OBJ) main.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(BINS)

