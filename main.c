#include "functions.h"
#include <stdio.h>

int main(){
	static int N = 10;
	int *bin_mat = malloc(sizeof(int) * N*N);
	int *data_mat = malloc(sizeof(int) * N*N);
	int *mul_result = malloc(sizeof(int) * N*N);
	//int *mul_result_omp = malloc(sizeof(int) * N*N);
     		
	//read bin
	start_time();
	read_matrix_bin(bin_mat, "/bigwork/nhmqnoeh/A_10x10.bin", N);	
	printf("reading bin matrix of size %ix%i took %llu milliseconds\n", N, N, stop_time());	       
	print_matrix(bin_mat, N);

	//read data
        start_time();
	read_matrix_data(data_mat, "/bigwork/nhmqnoeh/A_10x10.data", N);
        printf("reading data matrix of size %ix%i took %llu milliseconds\n", N, N, stop_time());
	print_matrix(data_mat, N);

	//multiply serial
        start_time();
	mul_matrix(data_mat, data_mat, mul_result, N);
        printf("serial: multiplying matrix of size %ix%i took %llu milliseconds\n", N, N, stop_time());
	print_matrix(mul_result, N);
	
	//multiply with openMP
	/*start_time();
	mul_matrix_omp(data_mat, data_mat, mul_result_omp, N);
	printf("openmp: multiplying matrix of size %ix%i took %llu milliseconds\n", N, N, stop_time());
	print_matrix(mul_result_omp, N);
	*/
	start_time();
	write_matrix(mul_result, "/bigwork/nhmqnoeh/myAA_1000x1000.bin", N);
	printf("writing bin matrix of size %ix%i took %llu milliseconds\n", N, N, stop_time());
	
	free(bin_mat);
	free(data_mat);
	free(mul_result);	

	return 0;
}
