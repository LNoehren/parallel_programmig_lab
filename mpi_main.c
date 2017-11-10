#include "functions.h"
#include "mpi.h"

int main(int argc, char* argv){
	int rank;
	int N = 10;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int* matA = malloc(sizeof(int)*N*N);
	int* matB = malloc(sizeof(int)*N*N);
	if(rank == 0){
		read_mat_bin(matA, "/bigwork/nhmqnoeh/A_10x10.bin", N);
		read_mat_bin(matB, "/bigwork/nhmqnoeh/B_10x10.bin", N);
	}
	MPI_Bcast(matA, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank == 1){
		print_mat(matA, N);
	}
	
}
