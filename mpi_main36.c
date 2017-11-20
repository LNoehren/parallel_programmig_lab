#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "functions.h"

int main(int argc, char* argv){
	int rank;
	int worldSize;
	int N = 10;

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

	read_part_matrix_bin(matA, "/bigwork/nhmqnoeh/A_10x10.bin", N);
	read_part_matrix_bin(matB, "/bigwork/nhmqnoeh/B_10x10.bin", N);
	
	for(int i = 0; i < worldSize; i++){
		if(i != rank){
			MPI_Request req[4];			

			MPI_Isend(&matA[rank*chunkSize], bigChunk, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[0]);
			MPI_Isend(&matB[rank*chunkSize], bigChunk, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[1]);
			if(i == worldSize-1){
				MPI_Irecv(&matA[i*chunkSize], chunkSize+((N*N)%worldSize), MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[2]);
				MPI_Irecv(&matB[i*chunkSize], chunkSize+((N*N)%worldSize), MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[3]);
			}
			else {
				MPI_Irecv(&matA[i*chunkSize], chunkSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[2]);
                                MPI_Irecv(&matB[i*chunkSize], chunkSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[3]);
			}

			MPI_Waitall(4, req, MPI_STATUS_IGNORE);
			//MPI_Request_free(req);
               	}

	}

	if(rank==0)print_matrix(matB, N);
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
