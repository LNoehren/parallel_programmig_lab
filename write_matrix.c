#include "functions.h"
#include <stdio.h>

int write_matrix(int* data, char* filename, int N) {
	FILE *file;

	file = fopen(filename, "wb");
	fwrite (data, sizeof(int)*N*N, 1, file);
	fclose(file);
	return 0;
}
