#include <mpi.h>
#include "functions.h"
#include <stdio.h>

int main(int argc, char* argv){
	int rank;
	int worldSize;
	int N = 30000;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	int* matA = malloc(sizeof(int)*N*N);
	int* matB = malloc(sizeof(int)*N*N);
	int* partRes = malloc(sizeof(int)*N*N);
	int* result;
	
	if(rank == 0){
		start_time();
		result = malloc(sizeof(int)*N*N);

		//read matA and matB
		char aPath[50];
		char bPath[50];
		snprintf(aPath, sizeof(aPath), "/bigwork/nhmqnoeh/A_%ix%i.bin", N, N);
		snprintf(bPath, sizeof(bPath), "/bigwork/nhmqnoeh/B_%ix%i.bin", N, N);
		
		read_matrix_bin(matA, aPath, N);
		read_matrix_bin(matB, bPath, N);
	}
	MPI_Bcast(matA, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(matB, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	mul_matrix_mpi(matA, matB, partRes, N);
	
	int startpos = rank*N*(N/worldSize);
	int bigSize = N*(N/worldSize + N%worldSize);
	int* recCount = malloc(worldSize*sizeof(int));
	int* dispels = malloc(worldSize*sizeof(int));
	for(int i=0; i<worldSize; i++){
		if(i==worldSize-1) recCount[i] = bigSize;
		else recCount[i] = N*(N/worldSize);
		dispels[i] = i*N*(N/worldSize);
	}

	if(rank==worldSize-1)
		MPI_Gatherv(&partRes[startpos], bigSize, MPI_INT, result, recCount, dispels, MPI_INT, 0, MPI_COMM_WORLD);
	else
		MPI_Gatherv(&partRes[startpos], N*(N/worldSize), MPI_INT, result, recCount, dispels, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank==0){
		//print_matrix(result, N);

		char cPath[50];
		snprintf(cPath, sizeof(cPath), "/bigwork/nhmqnoeh/C_%ix%i.bin", N, N);
		write_matrix(result, cPath, N);
		printf("3.4 took %llums\n", stop_time());
	}
	
	free(matA);
	free(matB);
	free(partRes);
	if(rank == 0) free(result);
		
	MPI_Finalize();

}
