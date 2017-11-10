#include "functions.h"

int main(){
	static int N = 30000;
        int *bin_mat = malloc(sizeof(int) * N*N);
        int *mul_result = malloc(sizeof(int) * N*N);
	
	read_matrix_bin(bin_mat, "/bigwork/nhmqnoeh/A_30000x30000.bin", N);
	mul_matrix(bin_mat, bin_mat, mul_result, N);
	write_matrix(mul_result, "/bigwork/nhmqnoeh/AA_30000x30000.bin", N);
	
	return 0;
}
