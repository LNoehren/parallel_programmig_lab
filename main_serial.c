#include "functions.h"
#include <stdio.h>

int main(){
	start_time();
	static int N = 30000;
        int *bin_matA = malloc(sizeof(int) * N*N);
	int *bin_matB = malloc(sizeof(int) * N*N);
        int *mul_result = malloc(sizeof(int) * N*N);
	
	read_matrix_bin(bin_matA, "/bigwork/nhmqnoeh/A_30000x30000.bin", N);
	read_matrix_bin(bin_matB, "/bigwork/nhmqnoeh/B_30000x30000.bin", N);
	
	mul_matrix(bin_matA, bin_matB, mul_result, N);

	//print_matrix(mul_result, N);

	write_matrix(mul_result, "/bigwork/nhmqnoeh/C_30000x30000.bin", N);
	
	free(bin_matA);
	free(bin_matB);
	free(mul_result);

	printf("serial multiplication took %llums\n", stop_time());

	return 0;
}
