#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include "functions.h"

int main(int argc, char* argv){
	int rank;
	int worldSize;
	int N = 10;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	int* matA = malloc(sizeof(int)*N*N);
	int* matB = malloc(sizeof(int)*N*N);
	int* partRes = malloc(sizeof(int)*N*N);
	int* result;
	
	if(rank == 0){
		result = malloc(sizeof(int)*N*N);
		read_matrix_bin(matA, "/bigwork/nhmqnoeh/A_10x10.bin", N);
		read_matrix_bin(matB, "/bigwork/nhmqnoeh/B_10x10.bin", N);
	}
	MPI_Bcast(matA, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(matB, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	mul_matrix_mpi(matA, matB, partRes, N);
	
	char* partMatPaht[200];
	snprintf(partMatPath, sizeof(partMatPath), "partialMat/result.%i", getpid());
	
	write_matrix(partRes, partMatPath, N);
	
	MPI_Finalize();	
}
