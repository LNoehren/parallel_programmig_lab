#include "functions.h"
#include <mpi.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char* argv){
        int rank;
        int worldSize;
        int N = 30000;

        MPI_Init(NULL, NULL);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        int chunkPerLine = sqrt(worldSize);
        if(chunkPerLine * chunkPerLine != worldSize){
                if(rank==0) printf("Please only use a square number of Processors\n");
                return 1;
        }

        int* mat = malloc(sizeof(int)*N*N);

        char aPath[50];
        snprintf(aPath, sizeof(aPath), "/bigwork/nhmqnoeh/A_%ix%i.bin", N, N);
        if(rank==0){
		read_matrix_bin(mat, aPath, N);
        }
        MPI_Bcast(mat, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank==0)start_time();

	write_matrix_mpi(mat, aPath, N);
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==0){
		printf("writing took %llu ms\n", stop_time());
	}
	return 0;
}
