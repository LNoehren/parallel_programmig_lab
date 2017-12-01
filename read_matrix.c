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

//read a matrix using mpi fileviews
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

        MPI_Datatype dataBlock;
        MPI_Type_contiguous(blockWidth, MPI_INT, &dataBlock);
        MPI_Type_commit(&dataBlock);
	
        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
        for(int i = 0; i < blockWidth; i++){
		int startPos = startPosX+startPosY+i*N;
		MPI_File_set_view(file, sizeof(int) * startPos, MPI_INT, dataBlock, "native", MPI_INFO_NULL);
                MPI_File_read(file, &data[startPos], blockWidth, MPI_INT, MPI_STATUS_IGNORE);
        }
        MPI_File_close(&file);
        return 0;
}

