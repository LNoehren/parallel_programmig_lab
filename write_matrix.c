#include "functions.h"
#include <stdio.h>

int write_matrix(int* data, char* filename, int N) {
	FILE *file;

	file = fopen(filename, "wb");
	fwrite (data, sizeof(int)*N*N, 1, file);
	fclose(file);
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

}


