#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
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
	mul_matrix_mpi_rect(mat, matE, partRes, N);

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
	mul_matrix_mpi_rect(mat, matE, partRes, N);
	
	//write partialResults of B
	char partMatBPath[50];
        snprintf(partMatBPath, sizeof(partMatBPath), "/bigwork/nhmqnoeh/partialMatB/result.%i", getpid());
        write_matrix(partRes, partMatBPath, N);

	//if(rank==0)print_matrix(partRes, N);

	if (rank == 0) start_time();
	//read partial matrices and send them (store B in matE to save memory)
	read_matrix_bin(mat, partMatAPath, N);
	read_matrix_bin(matE, partMatBPath, N);
	
	int chunkPerLine = sqrt(worldSize);
        int blockWidth = N/chunkPerLine;
        int startPosX = (rank%chunkPerLine)*blockWidth;
        int startPosY = (int)(rank/chunkPerLine) * blockWidth * N;

        int sendSize = blockWidth;
        int recSize = blockWidth;

        if((rank%chunkPerLine) == chunkPerLine-1)sendSize += N%chunkPerLine;

	for(int i = 0; i < chunkPerLine*chunkPerLine; i++){
		if(rank != i){
			int istartPosX = (i%chunkPerLine)*blockWidth;
			int istartPosY = (int)(i/chunkPerLine) * blockWidth * N;
		
			if((i%chunkPerLine) == chunkPerLine-1)recSize += N%chunkPerLine;
		
			int lineCount = blockWidth + (N%chunkPerLine);
			for(int j = 0; j < lineCount; j++){
				MPI_Request req[4];
				
				//only the lowest row has to send more than blockWidth
				if((int)(rank/chunkPerLine) == chunkPerLine-1 || j < blockWidth){
					MPI_Isend(&mat[startPosX+startPosY+j*N], sendSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[0]);
                        		MPI_Isend(&matE[startPosX+startPosY+j*N], sendSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[1]);
				}
				
				//only receive more then blockWidth from the lowest row
				if((int)(i/chunkPerLine) == chunkPerLine-1 || (j < blockWidth)){
					MPI_Irecv(&mat[istartPosX+istartPosY+j*N], recSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[2]);
                                	MPI_Irecv(&matE[istartPosX+istartPosY+j*N], recSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[3]);
				}
				
				MPI_Waitall(4, req, MPI_STATUS_IGNORE);
			}
               	}
	}
	
	//if(rank==0)print_matrix(mat, N);	

	mul_matrix_mpi_rect(mat, matE, partRes, N);
	
	char resultPath[50];
	snprintf(resultPath, sizeof(resultPath), "/bigwork/nhmqnoeh/MY_C_%ix%i.bin", N, N);
	write_matrix_mpi(partRes, resultPath, N);
		
	if(rank == 0){
		//read_matrix_bin(mat, resultPath, N);
		//print_matrix(mat, N);		
		printf("time taken: %llu ms\n", stop_time());
	}// */ 

	free(mat);
	free(matE);
	free(partRes);
	
	MPI_Finalize();	
}
