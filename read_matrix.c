#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include "functions.h"

//read a matrix in binary format
int read_matrix_bin(int* data, char* filename, int N){
        FILE *file;
        
        file = fopen(filename, "rb");
        if(!file) {
                printf("unable to open file\n");
                return 1;
        }
        fread(data, sizeof(int)*N*N, 1, file);
       	fclose(file);
	return 0;
}

//read a matrix in data format
int read_matrix_data(int* data, char* filename, int N){
        FILE *file;
        char buffer[N];
        
	file = fopen(filename, "r");
        if(!file) {
                printf("unable to open file\n");
                return 1;
        }
	size_t nread;
	int count = 0;
	
	//read data file and only put digits in matrix
	while((nread = fread(buffer, sizeof(buffer), 1, file)) > 0){
		char* numbers = strtok(buffer, ";");
		int i = 0;
		while(numbers){
			if(numbers == "\n") continue;
			data[count] = atoi(numbers);	
			i++;
			count++;
			numbers = strtok(NULL, ";");
		}
	}
	fclose(file);
	return 0;
}
/*
int read_part_matrix_bin(int* data, char* filename, int N){
        int world_size;
        int rank;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        int chunkSize = N*N/world_size;
        int bigchunk = chunkSize;
        if(rank == world_size-1)bigchunk+=(N*N)%world_size;

	FILE *file;	
        file = fopen(filename, "rb");
        if(!file) {
                printf("unable to open file\n");
                return 1;
        }
	if(fseek(file, sizeof(int)*rank*chunkSize, SEEK_SET) != 0){
		printf("unable to move pointer\n");
		return 1;
	}
        fread(&data[rank*chunkSize], sizeof(int)*bigchunk, 1, file);
        fclose(file);
        return 0;
}*/

//read a NxN matrix using mpi fileviews
int read_matrix_mpi_fw(int* data, char* filename, int N) {
        MPI_File file;
        int rank;
        int worldSize;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        int chunkPerLine = sqrt(worldSize);
        int blockWidth = N/chunkPerLine;
        int startPosX = (rank%chunkPerLine) * blockWidth;
        int startPosY = (int)(rank/chunkPerLine) * blockWidth * N;

	int bigChunkX = blockWidth;
        int bigChunkY = blockWidth;
        if((rank%chunkPerLine) == chunkPerLine-1) bigChunkX += N%chunkPerLine;
        if((int)(rank/chunkPerLine) == chunkPerLine-1) bigChunkY += N%chunkPerLine;

	MPI_Datatype dataBlock, matRow;
        MPI_Type_contiguous(bigChunkX, MPI_INT, &dataBlock);
        MPI_Type_create_resized(dataBlock, 0, N * sizeof(int), &matRow);
        MPI_Type_commit(&matRow);

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
        MPI_File_set_view(file, sizeof(int) * (startPosX+startPosY), MPI_INT, matRow, "native", MPI_INFO_NULL);
	for(int i = 0; i < bigChunkY; i++){
		int startPos = startPosX+startPosY+i*N;
                MPI_File_read(file, &data[startPos], 1, matRow, MPI_STATUS_IGNORE);
        }
        MPI_File_close(&file);
        return 0;
}

//read NxM matrix using mpi fileviews
int read_matrix_mpi_fw2(int* data, char* filename, int N, int M) {
        MPI_File file;
        int rank;
        int worldSize;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	int matSize = N > M ? N : M;

	//startPos in file
        int chunkPerLine = sqrt(worldSize);
        int blockWidth = matSize/chunkPerLine;
        int startPosX = (rank%chunkPerLine) * blockWidth;
        int startPosY = (int)(rank/chunkPerLine) * blockWidth * matSize;
	
	int bigChunk = blockWidth + matSize%chunkPerLine;

	//startPos in data
	int startPosXData = N > M ? startPosX : 0;
	int startPosYData = N > M ? 0 : (int)(rank/chunkPerLine)*blockWidth*N;

        MPI_Datatype dataBlock, matRow;
        MPI_Type_contiguous(bigChunk, MPI_INT, &dataBlock);
	MPI_Type_create_resized(dataBlock, 0, matSize * sizeof(int), &matRow);
        MPI_Type_commit(&matRow);

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
        
	int startPos = startPosX+startPosY;
	MPI_File_set_view(file, sizeof(int) * startPos, MPI_INT, matRow, "native", MPI_INFO_NULL);

	for(int i = 0; i < bigChunk; i++){
		int startPosData = startPosXData+startPosYData+i*N;
                MPI_File_read(file, &data[startPosData], 1, matRow, MPI_STATUS_IGNORE);
        }
        MPI_File_close(&file);
        return 0;
}

