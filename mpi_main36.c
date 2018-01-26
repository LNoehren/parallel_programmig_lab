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
	
	int chunkSize = N/worldSize;
	int bigChunk = chunkSize;
	if(rank == worldSize - 1) bigChunk += N%worldSize;

	int* mat = malloc(sizeof(int)*N*N);
	int* matE = malloc(sizeof(int)*N*N);
	int* partRes = malloc(sizeof(int)*N*N);
	
	//read matA and matE
	if(rank == 0){
		char aPath[50];
		char ePath[50];
		snprintf(aPath, sizeof(aPath), "/bigwork/nhmqnoeh/A_%ix%i.bin", N, N);
		snprintf(ePath, sizeof(ePath), "/bigwork/nhmqnoeh/E_%ix%i.bin", N, N);
		read_matrix_bin(mat, aPath, N);
		read_matrix_bin(matE, ePath, N);
	}
	MPI_Bcast(mat, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(matE, N*N, MPI_INT, 0, MPI_COMM_WORLD);
	
	//mul A with E to split A
	mul_matrix_mpi(mat, matE, partRes, N);

	//write partial results of A
	char partMatAPath[50];
        snprintf(partMatAPath, sizeof(partMatAPath), "/bigwork/nhmqnoeh/partialMatA/result.%i", getpid());
        write_matrix(partRes, partMatAPath, N);
	
	//if(rank==0)print_matrix(partRes, N);

	//read matB
	if(rank == 0){
		char bPath[50];
		snprintf(bPath, sizeof(bPath), "/bigwork/nhmqnoeh/B_%ix%i.bin", N, N);
		read_matrix_bin(mat, bPath, N);
	}
	MPI_Bcast(mat, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	//mul B with E
	mul_matrix_mpi(mat, matE, partRes, N);
	
	//write partialResults of B
	char partMatBPath[50];
        snprintf(partMatBPath, sizeof(partMatBPath), "/bigwork/nhmqnoeh/partialMatB/result.%i", getpid());
        write_matrix(partRes, partMatBPath, N);

	//if(rank==0)print_matrix(partRes, N);

	if (rank == 0) start_time();
	//read partial matrices and send them (store B in matE to save memory)
	read_matrix_bin(mat, partMatAPath, N);
	read_matrix_bin(matE, partMatBPath, N);

	MPI_Request req[4*(worldSize-1)];
	int reqCount = 0;
	for(int i = 0; i < worldSize; i++){
		if(i != rank){
                  	MPI_Isend(&mat[rank*N*chunkSize], N*bigChunk, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[reqCount++]);
                        MPI_Isend(&matE[rank*N*chunkSize], N*bigChunk, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[reqCount++]);
                        
			int recSize = chunkSize;
			if(i == worldSize-1) recSize += N%worldSize;
                        
			MPI_Irecv(&mat[i*N*chunkSize], N*recSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[reqCount++]);
	                MPI_Irecv(&matE[i*N*chunkSize], N*recSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[reqCount++]);
               	}
	}
	MPI_Waitall(4*(worldSize-1), req, MPI_STATUS_IGNORE);

	mul_matrix_mpi(mat, matE, partRes, N);
	
	char partMatPath[50];
	snprintf(partMatPath, sizeof(partMatPath), "/bigwork/nhmqnoeh/partialMatC/result.%i", getpid());
	write_matrix(partRes, partMatPath, N);
		
	if(rank == 0){
		//print_matrix(partRes, N);
		printf("time taken: %llu ms\n", stop_time());
	}

	free(mat);
	free(matE);
	free(partRes);
	
	MPI_Finalize();	
}
