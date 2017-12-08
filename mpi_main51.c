#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "functions.h"
#include <string.h>

int main(int argc, char* argv){
	int rank;
	int worldSize;
	int N = 10;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	
	int chunkPerLine = sqrt(worldSize);
        int blockWidth = N/chunkPerLine;

	if(chunkPerLine * chunkPerLine != worldSize){
		if(rank==0) printf("Please only use a square number of Processors\n");
		return 1;
	}

	//create row and column communicators
	int rowColor = rank / chunkPerLine;
	int colColor = rank % chunkPerLine;

	MPI_Comm rowComm;
	MPI_Comm colComm;
	MPI_Comm_split(MPI_COMM_WORLD, rowColor, rank, &rowComm);
	MPI_Comm_split(MPI_COMM_WORLD, colColor, rank, &colComm);

	int rowRank, colRank, rowSize, colSize;
	MPI_Comm_rank(rowComm, &rowRank);
	MPI_Comm_rank(colComm, &colRank);
	MPI_Comm_size(rowComm, &rowSize);
	MPI_Comm_size(colComm, &colSize);
	

	int* rowMatA = malloc(sizeof(int)*N*blockWidth);
	int* colMatA = malloc(sizeof(int)*N*blockWidth);
	int* rowMatB = malloc(sizeof(int)*N*blockWidth);
	int* colMatB = malloc(sizeof(int)*N*blockWidth);
	//int* partRes = malloc(sizeof(int)*N*N);
	
	if(rank==0)start_time();

	//read matA and matB
	char aPath[50];
	char bPath[50];
	snprintf(aPath, sizeof(aPath), "/bigwork/nhmqnoeh/A_%ix%i.bin", N, N);
	snprintf(bPath, sizeof(bPath), "/bigwork/nhmqnoeh/B_%ix%i.bin", N, N);
	
	read_matrix_mpi_fw(rowMatA, aPath, N);
	read_matrix_mpi_fw(rowMatB, bPath, N);

	memcpy(colMatA, rowMatA, sizeof(int)*N*blockWidth);
	memcpy(colMatB, rowMatB, sizeof(int)*N*blockWidth);

	//if(rank==0)print_matrix(matA, N);
	
        int startPosX = (rowRank%chunkPerLine)*blockWidth;
        int startPosY = colRank * blockWidth * blockWidth;

        int rowSendSize = blockWidth;
        int rowRecSize = blockWidth;
	int colSendSize = blockWidth * blockWidth;
	int colRecSize = blockWidth * blockWidth;

        if(rowRank == rowSize)rowSendSize += N%chunkPerLine;
	if(colRank == colSize)colSendSize += blockWidth*N%chunkPerLine;

	for(int i = 0; i < rowSize; i++){
		int istartPosX = (i%chunkPerLine)*blockWidth;
                int istartPosY = i * blockWidth * blockWidth;

                if(i == rowSize)rowRecSize += N%chunkPerLine;
		if(i == colSize)colRecSize += blockWidth*N%chunkPerLine;

                //int lineCount = blockWidth + (N%chunkPerLine);

		//send to/receive from row
		if(rowRank != i){
			for(int j = 0; j < blockWidth; j++){
				MPI_Request req[4];
				
				MPI_Isend(&rowMatA[startPosX+j*N], rowSendSize, MPI_INT, i, 0 ,rowComm, &req[0]);
                        	MPI_Isend(&rowMatB[startPosX+j*N], rowSendSize, MPI_INT, i, 0 ,rowComm, &req[1]);
				
				MPI_Irecv(&rowMatA[istartPosX+j*N], rowRecSize, MPI_INT, i, 0 ,rowComm, &req[2]);
                                MPI_Irecv(&rowMatB[istartPosX+j*N], rowRecSize, MPI_INT, i, 0 ,rowComm, &req[3]);
				
				MPI_Waitall(4, req, MPI_STATUS_IGNORE);
			}
               	}
		//send to/receive from col
		if(colRank != i){
                        MPI_Request req[4];

                        MPI_Isend(&colMatA[startPosY], colSendSize, MPI_INT, i, 0 ,colComm, &req[0]);
                        MPI_Isend(&colMatB[startPosY], colSendSize, MPI_INT, i, 0 ,colComm, &req[1]);

                        MPI_Irecv(&colMatA[istartPosY], colRecSize, MPI_INT, i, 0 ,colComm, &req[2]);
                        MPI_Irecv(&colMatB[istartPosY], colRecSize, MPI_INT, i, 0 ,colComm, &req[3]);

                        MPI_Waitall(4, req, MPI_STATUS_IGNORE);
                }
	}
	
	if(rank==0)print_matrix2(rowMatA, N, blockWidth);
        if(rank==0)print_matrix2(colMatA, blockWidth, N);

/*
	mul_matrix_mpi_rect(matA, matB, partRes, N);
	
	char resultPath[50];
	snprintf(resultPath, sizeof(resultPath), "/bigwork/nhmqnoeh/MY_C_%ix%i.bin", N, N);
	write_matrix_mpi_fw(partRes, resultPath, N);
		
	if(rank == 0){
		//read_matrix_bin(mat, resultPath, N);
		//print_matrix(mat, N);		
		printf("time taken: %llu ms\n", stop_time());
	}// */ 

	free(rowMatA);
	free(rowMatB);
	free(colMatA);
	free(colMatB);
	//free(partRes);
	
	MPI_Comm_free(&rowComm);
	MPI_Comm_free(&colComm);
	
	MPI_Finalize();	
}
