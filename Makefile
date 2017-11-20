
CC=mpicc
CFLAGS=-O3 -g

DEPS=global.h functions.h

OBJ=write_matrix.o read_matrix.o mul_matrix.o

mpi_main36: $(OBJ) mpi_main36.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) mpi_main36.o

