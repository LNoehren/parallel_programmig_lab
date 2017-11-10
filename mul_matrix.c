#include "omp.h"
#include "functions.h"

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

int mul_matrix_omp(int* first, int* second, int* result, int N){
	#pragma omp parallel for collapes(3)
        for(int i = 0; i < N; i++){
                for(int k = 0; k < N; k++){
                        for(int j = 0; j < N; j++){
                                result[i+k*N] += first[i+j*N] * second[j+k*N];
                        }
                }
        }
	return 0;
}
