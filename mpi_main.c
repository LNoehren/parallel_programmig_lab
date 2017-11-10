#include <mpi.h>
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
	//int* resultAll = malloc(sizeof(int)*N*N*worldSize);
	int* result = malloc(sizeof(int)*N*N);

	if(rank == 0){
		read_matrix_bin(matA, "/bigwork/nhmqnoeh/A_10x10.bin", N);
		read_matrix_bin(matB, "/bigwork/nhmqnoeh/B_10x10.bin", N);
	}
	MPI_Bcast(matA, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(matB, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	mul_matrix_mpi(matA, matB, partRes, N);
	
	//if(rank==0)print_matrix(matA, N);
	//if(rank==0)print_matrix(matB, N);
	//if(rank==0)print_matrix2(partRes, N, N);	
	double startpos = rank*N*N/worldSize;
	MPI_Gather(&partRes[(int)startpos], N*N/worldSize, MPI_INT, result, N*N/worldSize, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank==0){
		print_matrix(result, N);
	}
	
	MPI_Finalize();	
}
