#include <mpi.h>
#include "functions.h"
#include <math.h>

//multiply a matrix
int mul_matrix(int* first, int* second, int* result, int N){
	
	for(int i = 0; i < N; i++){
		for(int k = 0; k < N; k++){
			for(int j = 0; j < N; j++){
				result[i+k*N] += first[j+k*N] * second[i+j*N];
			}
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

//multiply a matrix with mpi, split in quadratic blocs, input matrices are NxM and MxN
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
