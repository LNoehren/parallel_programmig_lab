#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "functions.h"

int main(int argc, char* argv){
	int rank;
	int worldSize;
	int N = 10;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	
	int chunkPerLine = sqrt(worldSize);
	if(chunkPerLine * chunkPerLine != worldSize){
		if(rank==0) printf("Please only use a square number of Processors\n");
		return 1;
	}

	int* matA = malloc(sizeof(int)*N*N);
	int* matB = malloc(sizeof(int)*N*N);
	int* partRes = malloc(sizeof(int)*N*N);
	
	if(rank==0)start_time();

	//read matA and matB
	char aPath[50];
	char bPath[50];
	snprintf(aPath, sizeof(aPath), "/bigwork/nhmqnoeh/A_%ix%i.bin", N, N);
	snprintf(bPath, sizeof(bPath), "/bigwork/nhmqnoeh/B_%ix%i.bin", N, N);
	
	read_matrix_mpi_fw(matA, aPath, N);
	read_matrix_mpi_fw(matB, bPath, N);

	if(rank==0)print_matrix(matA, N);
	
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
					MPI_Isend(&matA[startPosX+startPosY+j*N], sendSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[0]);
                        		MPI_Isend(&matB[startPosX+startPosY+j*N], sendSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[1]);
				}
				
				//only receive more then blockWidth from the lowest row
				if((int)(i/chunkPerLine) == chunkPerLine-1 || (j < blockWidth)){
					MPI_Irecv(&matA[istartPosX+istartPosY+j*N], recSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[2]);
                                	MPI_Irecv(&matB[istartPosX+istartPosY+j*N], recSize, MPI_INT, i, 0 ,MPI_COMM_WORLD, &req[3]);
				}
				
				MPI_Waitall(4, req, MPI_STATUS_IGNORE);
			}
               	}
	}
	
	//if(rank==0)print_matrix(mat, N);	

	mul_matrix_mpi_rect(matA, matB, partRes, N);
	
	char resultPath[50];
	snprintf(resultPath, sizeof(resultPath), "/bigwork/nhmqnoeh/MY_C_%ix%i.bin", N, N);
	write_matrix_mpi_fw(partRes, resultPath, N);
		
	if(rank == 0){
		//read_matrix_bin(mat, resultPath, N);
		//print_matrix(mat, N);		
		printf("time taken: %llu ms\n", stop_time());
	}// */ 

	free(matA);
	free(matB);
	free(partRes);
	
	MPI_Finalize();	
}
