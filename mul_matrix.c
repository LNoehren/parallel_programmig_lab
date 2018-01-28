#include <mpi.h>
#include "functions.h"
#include <math.h>
#include <stdio.h>

//multiply a matrix
int mul_matrix(int* first, int* second, int* result, int N){
	
	for(int i = 0; i < N; i++){
		for(int k = 0; k < N; k++){
			int sum = 0;
			for(int j = 0; j < N; j++){
				sum += first[j+k*N] * second[i+j*N];
			}
			result[i+k*N] = sum;
		}
	}
	return 0;
}

//multiply a matrix with mpi, split matrix in lines
int mul_matrix_mpi(int* first, int* second, int* result, int N){
	int world_size;
	int rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int chunkSize = N/world_size;
	int bigchunk = chunkSize;
	if(rank == world_size-1)bigchunk+=N%world_size;

        for(int i = 0; i < N; i++){
                for(int k = rank*chunkSize; k < (rank+1)*bigchunk && k < N; k++){
                        int sum = 0;
			for(int j = 0; j < N; j++){
                                sum += first[j+k*N] * second[i+j*N];
                        }
			result[i+k*N] = sum;
		}
        }
        return 0;
}

//multiply a matrix with mpi, split matrix in quadratic blocks
int mul_matrix_mpi_rect(int* first, int* second, int* result, int N){
        int world_size;
        int rank;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int chunkPerLine = sqrt(world_size);
	int yChunkPos = rank/chunkPerLine;
        int chunkSize = N/chunkPerLine;
        
	int bigchunkX = chunkSize;
	int bigchunkY = chunkSize;
        if((rank%chunkPerLine) == chunkPerLine-1)bigchunkX+=N%chunkPerLine;
	if((int)(rank/chunkPerLine) == chunkPerLine-1)bigchunkY+=N%chunkPerLine;


        for(int i = (rank%chunkPerLine)*chunkSize; i < ((rank%chunkPerLine)+1)*bigchunkX && i < N; i++){
                for(int k = yChunkPos*chunkSize; k < (yChunkPos+1)*bigchunkY && k < N; k++){
                        int sum = 0;
			for(int j = 0; j < N; j++){
                                sum += first[j+k*N] * second[i+j*N];
                        }
			result[i+k*N] = sum;
                }
        }
        return 0;
}

//multiply a matrix with mpi, split in quadratic blocs, input matrices are NxM and MxN write to NxN
int mul_matrix_mpi_rect2(int* first, int* second, int* result, int N, int M){
        int world_size;
        int rank;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int chunkPerLine = sqrt(world_size);
	int chunkSize = N/chunkPerLine;
        int yChunkPos =(int)(rank/chunkPerLine) * chunkSize;
        int xChunkPos = rank%chunkPerLine * chunkSize;

        for(int i = 0; i < M; i++){
                for(int k = 0; k < M; k++){
                        int sum = 0;
                        for(int j = 0; j < N; j++){
                                sum += first[j+k*N] * second[i+j*M];
                        }
                        result[(i+xChunkPos)+(k+yChunkPos)*N] = sum;
                }
        }
        return 0;
}

//multiply a matrix with mpi, split in quadratic blocs, input matrices are NxM and MxN. write to MxM
int mul_matrix_mpi_rect_small(int* first, int* second, int* result, int N, int M){
        int world_size;
        int rank;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int pos = 0;
        for(int k = 0; k < M; k++){
                for(int i = 0; i < M; i++){
                        int sum = 0;
                        for(int j = 0; j < N; j++){
                                sum += first[j+k*N] * second[i+j*M];
                        }
                        result[pos++] = sum;
                }
        }
        return 0;
}

//like previous function but writes directly after a line is completed
int mul_and_write_matrix(int* first, int* second, int N, int M, char* filename){
        MPI_File file;
	int world_size;
        int rank;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int chunkPerLine = sqrt(world_size);
        int blockWidth = N/chunkPerLine;
        int startPosX = (rank%chunkPerLine)*blockWidth;
        int startPosY = (int)(rank/chunkPerLine) * blockWidth * N;

        MPI_Datatype dataBlock, matRow;
        MPI_Type_contiguous(M, MPI_INT, &dataBlock);
        MPI_Type_create_resized(dataBlock, 0, N * sizeof(int), &matRow);
        MPI_Type_commit(&matRow);

        int stripe_count = world_size;
        if(stripe_count > 20) stripe_count = 20;
        int stripe_size = 4*N*N / stripe_count;

        char temp[100];
        sprintf(temp, "%d", stripe_count);

        MPI_Info info;
        MPI_Info_create(&info);
        MPI_Info_set(info, "striping_factor", temp);
        MPI_Info_set(info, "cb_nodes", temp);
        sprintf(temp, "%d", stripe_size);
        MPI_Info_set(info, "striping unit", temp);

        MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, info, &file);
        MPI_File_set_view(file, sizeof(int) * (startPosX+startPosY), MPI_INT, matRow, "native", info);

	int* result = malloc(sizeof(int)*M);

        for(int k = 0; k < M; k++){
                for(int i = 0; i < M; i++){
                        int sum = 0;
                        for(int j = 0; j < N; j++){
                                sum += first[j+k*N] * second[i+j*M];
                        }
                        result[i] = sum;
                }
		MPI_File_write(file, result, M, MPI_INT, MPI_STATUS_IGNORE);
        }
	MPI_File_close(&file);
	
	free(result);
        MPI_Info_free(&info);
        return 0;
}
