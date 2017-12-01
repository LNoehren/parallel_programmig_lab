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
	int startPosY = (int)(rank/chunkPerLine) * blockWidth * N;
	
	if(rank >= chunkPerLine*chunkPerLine)return 0;

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	for(int i = 0; i < blockWidth; i++){
		int startPos = startPosX+startPosY+i*N;
		MPI_File_write_at(file, sizeof(int)*startPos, &data[startPos], blockWidth, MPI_INT, MPI_STATUS_IGNORE);
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
        int startPosY = (int)(rank/chunkPerLine) * blockWidth * N;

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
        for(int i = 0; i < blockWidth; i++){
                int startPos = startPosX+startPosY+i*N;
		MPI_File_write_at_all(file, sizeof(int) * startPos, &data[startPos], blockWidth, MPI_INT, MPI_STATUS_IGNORE);
        }
        MPI_File_close(&file);
        return 0;
}

int write_matrix_mpi_fw(int* data, char* filename, int N) {
        MPI_File file;
        int rank;
        int worldSize;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        int chunkPerLine = sqrt(worldSize);
        int blockWidth = N/chunkPerLine;
        int startPosX = (rank%chunkPerLine)*blockWidth;
        int startPosY = (int)(rank/chunkPerLine) * blockWidth * N;

	MPI_Datatype dataBlock;
	MPI_Type_contiguous(blockWidth, MPI_INT, &dataBlock);
	MPI_Type_commit(&dataBlock);

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
        for(int i = 0; i < blockWidth; i++){
                int startPos = startPosX+startPosY+i*N;
                MPI_File_set_view(file, sizeof(int) * startPos, MPI_INT, dataBlock, "native", MPI_INFO_NULL);
		MPI_File_write(file, &data[startPos], blockWidth, MPI_INT, MPI_STATUS_IGNORE);
        }
        MPI_File_close(&file);
        return 0;
}

int write_matrix_mpi_fw_stripe(int* data, char* filename, int N) {
        MPI_File file;
        int rank;
        int worldSize;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        int chunkPerLine = sqrt(worldSize);
        int blockWidth = N/chunkPerLine;
        int startPosX = (rank%chunkPerLine)*blockWidth;
        int startPosY = (int)(rank/chunkPerLine) * blockWidth * N;

        MPI_Datatype dataBlock;
        MPI_Type_contiguous(blockWidth, MPI_INT, &dataBlock);
        MPI_Type_commit(&dataBlock);

	int stripe_count = worldSize;
	if(stripe_count > 20) stripe_count = 20;
	int stripe_size = 10*N*N / stripe_count;
	
	char temp[100];
	sprintf(temp, "%d", stripe_count);
	
	MPI_Info info;
	MPI_Info_create(&info);
	MPI_Info_set(info, "striping_factor", temp);
	MPI_Info_set(info, "cb_nodes", temp);
	sprintf(temp, "%d", stripe_size);
	MPI_Info_set(info, "striping unit", temp);

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, info, &file);
        for(int i = 0; i < blockWidth; i++){
                int startPos = startPosX+startPosY+i*N;
                MPI_File_set_view(file, sizeof(int) * startPos, MPI_INT, dataBlock, "native", info);
                MPI_File_write(file, &data[startPos], blockWidth, MPI_INT, MPI_STATUS_IGNORE);
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
	printf("\n");

}

void print_matrix2(int* matrix, int N, int M){
        printf("\n");
	for(int i = 0; i< M; i++){
                for(int j=0; j < N; j++){
                        printf(" %d ", matrix[j + i*N]);
                }
                printf("\n");
        }
	printf("\n");

}


