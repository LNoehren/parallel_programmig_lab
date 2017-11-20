#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "functions.h"

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
}

