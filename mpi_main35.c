#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "functions.h"

int main(int argc, char* argv){
	int rank;
	int worldSize;
	int N = 30000;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	
	int chunkSize = N*N/worldSize;
	int bigChunk = chunkSize;
	if(rank == worldSize - 1) bigChunk += (N*N)%worldSize;

	int* matA = malloc(sizeof(int)*N*N);
	int* matB = malloc(sizeof(int)*N*N);
	int* partRes = malloc(sizeof(int)*N*N);
	int* result;
	
	if(rank == 0){
		result = malloc(sizeof(int)*N*N);
	}

	read_part_matrix_bin(matA, "/bigwork/nhmqnoeh/A_30000x30000.bin", N);
	read_part_matrix_bin(matB, "/bigwork/nhmqnoeh/B_30000x30000.bin", N);
	
	for(int i = 0; i < worldSize; i++){
		if(i != rank){
			MPI_Send(&matA[rank*chunkSize], bigChunk, MPI_INT, i, 0 ,MPI_COMM_WORLD);
			MPI_Send(&matB[rank*chunkSize], bigChunk, MPI_INT, i, 0 ,MPI_COMM_WORLD);
			if(i == worldSize-1){
				MPI_Recv(&matA[i*chunkSize], chunkSize+((N*N)%worldSize), MPI_INT, i, 0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&matB[i*chunkSize], chunkSize+((N*N)%worldSize), MPI_INT, i, 0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			else {
				MPI_Recv(&matA[i*chunkSize], chunkSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                MPI_Recv(&matB[i*chunkSize], chunkSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
               	}

	}

	mul_matrix_mpi(matA, matB, partRes, N);
	
	char partMatPath[50];
	snprintf(partMatPath, sizeof(partMatPath), "/bigwork/nhmqnoeh/partialMat/result.%i", getpid());
	
	write_matrix(partRes, partMatPath, N);
	
	free(matA);
	free(matB);
	free(partRes);
	if(rank == 0) free(result);

	MPI_Finalize();	
}
