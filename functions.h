#include <stdlib.h>
#include <sys/time.h>

int read_matrix_bin(int* data, char* filename, int N);

int read_matrix_data(int* data, char* filename, int N);

int mul_matrix(int* first, int* second, int* result, int N);

int mul_matrix_mpi(int* first, int* second, int* result, int N);

//int mul_matrix_omp(int* first, int* second, int* result, int N);

int write_matrix(int*data, char* filename, int N);

static struct timeval tm1;

static inline void start_time() {
    gettimeofday(&tm1, NULL);
}

static inline unsigned long long stop_time() {
    struct timeval tm2;
    gettimeofday(&tm2, NULL);

    unsigned long long t = 1000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec) / 1000;
    return t;
}

void print_matrix(int* matrix, int N);

void print_matrix2(int* matrix, int N, int M);

