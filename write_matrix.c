#include "functions.h"
#include <stdio.h>
#include "mpi.h"
#include <math.h>

int write_matrix(int* data, char* filename, int N) {
	FILE *file;

	file = fopen(filename, "wb");
	fwrite (data, sizeof(int)*N*N, 1, file);
	fclose(file);
	return 0;
}

int write_matrix_mpi(int* data, char* filename, int N) {
        MPI_File file;
	int rank;
	int worldSize;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	
	int chunkPerLine = sqrt(worldSize);
	int blockWidth = N/chunkPerLine;
	int startPosX = (rank%chunkPerLine)*blockWidth;
	int startPosY = (int)(rank/chunkPerLine) * chunkPerLine * N;

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_SEQUENTIAL, MPI_INFO_NULL, &file);
	for(int i = 0; i < blockWidth; i++){
		MPI_File_write_at(file, startPosX+startPosY+i*N, &data[startPosX+startPosY+i*N], blockWidth, MPI_INT, MPI_STATUS_IGNORE);
	}
	MPI_File_close(&file);
        return 0;
}

int write_matrix_mpi_all(int* data, char* filename, int N) {
        MPI_File file;
        int rank;
        int worldSize;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        int chunkPerLine = sqrt(worldSize);
        int blockWidth = N/chunkPerLine;
        int startPosX = (rank%chunkPerLine)*blockWidth;
        int startPosY = (int)(rank/chunkPerLine) * chunkPerLine * N;

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_SEQUENTIAL, MPI_INFO_NULL, &file);
        for(int i = 0; i < blockWidth; i++){
                MPI_File_write_at_all(file, startPosX+startPosY+i*N, &data[startPosX+startPosY+i*N], blockWidth, MPI_INT, MPI_STATUS_IGNORE);
        }
        MPI_File_close(&file);
        return 0;
}


void print_matrix(int* matrix, int N){
        for(int i = 0; i< N; i++){
                for(int j=0; j < N; j++){
                        printf(" %d ", matrix[j + i*N]);
                }
                printf("\n");
        }

}

void print_matrix2(int* matrix, int N, int M){
        for(int i = 0; i< M; i++){
                for(int j=0; j < N; j++){
                        printf(" %d ", matrix[j + i*N]);
                }
                printf("\n");
        }
	printf("\n\n");

}


