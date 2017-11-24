
CC=mpicc
CFLAGS=-O3 -g -lm

DEPS=global.h functions.h

OBJ=write_matrix.o read_matrix.o mul_matrix.o

test: $(OBJ) test.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) test.o

