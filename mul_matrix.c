#include <mpi.h>
#include "functions.h"
#include <math.h>

int mul_matrix(int* first, int* second, int* result, int N){
	
	for(int i = 0; i < N; i++){
		for(int k = 0; k < N; k++){
			for(int j = 0; j < N; j++){
				result[i+k*N] += first[i+j*N] * second[j+k*N];
			}
		}
	}
	return 0;
}

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
                                sum += first[i+j*N] * second[j+k*N];
                        }
			result[i+k*N] = sum;
		}
        }
        return 0;
}

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
                        for(int j = 0; j < N; j++){
                                result[i+k*N] += first[i+j*N] * second[j+k*N];
                        }
                }
        }
        return 0;
}

/*int mul_matrix_omp(int* first, int* second, int* result, int N){
	#pragma omp parallel for collapes(3)
        for(int i = 0; i < N; i++){
                for(int k = 0; k < N; k++){
                        for(int j = 0; j < N; j++){
                                result[i+k*N] += first[i+j*N] * second[j+k*N];
                        }
                }
        }
	return 0;
}*/
