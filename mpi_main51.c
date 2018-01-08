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
        int blockSize = N/chunkPerLine;

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
	
	int blockHeight = blockSize;
	int blockWidth = blockSize;
        if(rowRank == rowSize-1)blockWidth += N%chunkPerLine;
        if(colRank == colSize-1)blockHeight += N%chunkPerLine;

	int* rowMatA = malloc(sizeof(int)*N*blockHeight);
	int* colMatB = malloc(sizeof(int)*N*blockWidth);
	int* partRes = malloc(sizeof(int)*N*N);
	
	if(rank==0)start_time();

	//read matA and matB
	char aPath[50];
	char bPath[50];
	snprintf(aPath, sizeof(aPath), "/bigwork/nhmqnoeh/A_%ix%i.bin", N, N);
	snprintf(bPath, sizeof(bPath), "/bigwork/nhmqnoeh/B_%ix%i.bin", N, N);
	
	read_matrix_mpi_fw2(rowMatA, aPath, N, blockHeight);
	read_matrix_mpi_fw2(colMatB, bPath, blockWidth, N);

	if(rank==6)print_matrix2(rowMatA, N, blockHeight);
	if(rank==6)print_matrix2(colMatB, blockWidth, N);
	
        int startPosX = (rowRank%chunkPerLine)*blockSize;
        int startPosY = colRank * blockWidth * blockSize;

        int rowSendSize = blockWidth;
        int rowRecSize = blockSize;
	int colSendSize = blockWidth*blockHeight;
	int colRecSize = blockWidth*blockSize;

	for(int i = 0; i < rowSize; i++){
		int istartPosX = (i%chunkPerLine)*blockSize;
                int istartPosY = i * blockWidth * blockSize;

                if(i == rowSize-1)rowRecSize += N%chunkPerLine;
		if(i == colSize-1)colRecSize = blockWidth*(blockSize+N%chunkPerLine);

		//send to/receive from row partners
		if(rowRank != i){
			for(int j = 0; j < blockHeight; j++){
				MPI_Request req[2];
                        	
				MPI_Isend(&rowMatA[startPosX+j*N], rowSendSize, MPI_INT, i, 0 ,rowComm, &req[0]);
                                MPI_Irecv(&rowMatA[istartPosX+j*N], rowRecSize, MPI_INT, i, 0 ,rowComm, &req[1]);
				
				MPI_Waitall(2, req, MPI_STATUS_IGNORE);
			}
               	}
		//send to/receive from col partners
		if(colRank != i){
			MPI_Request req[2];

                        MPI_Isend(&colMatB[startPosY], colSendSize, MPI_INT, i, 0 ,colComm, &req[0]);
                        MPI_Irecv(&colMatB[istartPosY], colRecSize, MPI_INT, i, 0 ,colComm, &req[1]);

                        MPI_Waitall(2, req, MPI_STATUS_IGNORE);
                }
	}
	
	if(rank==6)print_matrix2(rowMatA, N, blockHeight);
        if(rank==6)print_matrix2(colMatB, blockWidth, N);

	mul_matrix_mpi_rect2(rowMatA, colMatB, partRes, N, blockWidth);
	
	if(rank==6)print_matrix(partRes, N);

	char resultPath[50];
	snprintf(resultPath, sizeof(resultPath), "/bigwork/nhmqnoeh/C_%ix%i.bin", N, N);
	write_matrix_mpi_fw_stripe(partRes, resultPath, N);
		
	if(rank == 0){
		read_matrix_bin(partRes, resultPath, N);
		print_matrix(partRes, N);		
		printf("time taken: %llu ms\n", stop_time());
	}// */ 

	free(rowMatA);
	free(colMatB);
	free(partRes);
	
	MPI_Comm_free(&rowComm);
	MPI_Comm_free(&colComm);
	
	MPI_Finalize();	
}
