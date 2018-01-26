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
	int N = 1000;

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
	
	int bigBlock = blockSize + N%chunkPerLine;

	int write_matrix_mpi_fw_stripe(int*data, char* filename, int N);int* rowMatA = malloc(sizeof(int)*N*bigBlock);
	int* colMatB = malloc(sizeof(int)*N*bigBlock);
	int* partRes = malloc(sizeof(int)*bigBlock*bigBlock);
	
	if(rank==0)start_time();

	//read matA and matB
	char aPath[50];
	char bPath[50];
	snprintf(aPath, sizeof(aPath), "/bigwork/nhmqnoeh/A_%ix%i.bin", N, N);
	snprintf(bPath, sizeof(bPath), "/bigwork/nhmqnoeh/B_%ix%i.bin", N, N);
	
	read_matrix_mpi_fw2(rowMatA, aPath, N, bigBlock);
	read_matrix_mpi_fw3(colMatB, bPath, bigBlock, N);

	//if(rank==0)print_matrix2(rowMatA, N, bigBlock);
	//if(rank==0)print_matrix2(colMatB, bigBlock, N);
	
        int startPosX = rowRank * blockSize;
        int startPosY = colRank*bigBlock*blockSize;
	
	int numberOfOperations = 2*(rowSize-1)+2*bigBlock*(rowSize-1);
	MPI_Request req[numberOfOperations];
	int reqPos=0;
	for(int i = 0; i < rowSize; i++){
                int istartPosX = i * blockSize;
		int istartPosY = i * blockSize * bigBlock;

	_Allgather(MPI_IN_PLACE, bigBlock*bigBlock, MPI_INT, colMatB, bigBlock*bigBlock, MPI_INT, colComm);
		//send to/receive from row partners
		if(rowRank != i){
			for(int j = 0; j < bigBlock; j++){
				MPI_Isend(&rowMatA[startPosX+j*N], bigBlock, MPI_INT, i, 0 ,rowComm, &req[reqPos++]);
                        	MPI_Irecv(&rowMatA[istartPosX+j*N], bigBlock, MPI_INT, i, 0 ,rowComm, &req[reqPos++]);
			}
               	}
		//send to/receive from col partners
		if(colRank != i){
                        MPI_Isend(&colMatB[startPosY], bigBlock*bigBlock, MPI_INT, i, 0 ,colComm, &req[reqPos++]);
                        MPI_Irecv(&colMatB[istartPosY], bigBlock*bigBlock, MPI_INT, i, 0 ,colComm, &req[reqPos++]);
                }
	}
	MPI_Waitall(numberOfOperations, req, MPI_STATUS_IGNORE);
	
	//MPI_Allgather(MPI_IN_PLACE, bigBlock*bigBlock, MPI_INT, rowMatA, bigBlock*bigBlock, MPI_INT, rowComm);
	//MPI_Allgather(MPI_IN_PLACE, bigBlock*bigBlock, MPI_INT, colMatB, bigBlock*bigBlock, MPI_INT, colComm);
	
	//if(rank==0)print_matrix2(rowMatA, N, bigBlock);
        //if(rank==0)print_matrix2(colMatB, bigBlock, N);

	mul_matrix_mpi_rect_small(rowMatA, colMatB, partRes, N, bigBlock);
	
	//if(rank==0)print_matrix(partRes, bigBlock);

	char resultPath[50];
	snprintf(resultPath, sizeof(resultPath), "/bigwork/nhmqnoeh/C_striped_%ix%i.bin", N, N);
	write_matrix_mpi_fw_stripe_improved(partRes, resultPath, N);
		
	if(rank == 0){
		/*int* result = malloc(sizeof(int)*N*N);
		read_matrix_bin(result, resultPath, N);
		print_matrix(result, N);
		free(result);// */		
		printf("time taken: %llu ms\n", stop_time());
	} 

	free(rowMatA);
	free(colMatB);
	free(partRes);
	
	MPI_Comm_free(&rowComm);
	MPI_Comm_free(&colComm);
	
	MPI_Finalize();	
}
