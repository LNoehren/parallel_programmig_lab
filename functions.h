#include <stdlib.h>
#include <sys/time.h>
//3.1
int read_matrix_bin(int* data, char* filename, int N);
//(3.5)wird nicht mehr genutzt
int read_part_matrix_bin(int* data, char* filename, int N);
//3.1
int read_matrix_data(int* data, char* filename, int N);
//4.5
int read_matrix_mpi_fw(int* data, char* filename, int N);
//5.1
int read_matrix_mpi_fw2(int* data, char* filename, int N, int M);
//6.1
int read_matrix_mpi_fw3(int* data, char* filename, int N, int M);
//7.3
int read_matrix_mpi_row(int* data, char* filename, int N, int M);
//3.2
int mul_matrix(int* first, int* second, int* result, int N);
//3.4
int mul_matrix_mpi(int* first, int* second, int* result, int N);
//4.1
int mul_matrix_mpi_rect(int* first, int* second, int* result, int N);
//5.1
int mul_matrix_mpi_rect2(int* first, int* second, int* result, int N, int M);
//6.1
int mul_matrix_mpi_rect_small(int* first, int* second, int* result, int N, int M);
//7.2
int mul_and_write_matrix(int* first, int* second, int N, int M, char* filename);
//3.3
int write_matrix(int*data, char* filename, int N);
//4.2
int write_matrix_mpi(int*data, char* filename, int N);
//4.3
int write_matrix_mpi_all(int*data, char* filename, int N);
//4.4
int write_matrix_mpi_fw(int*data, char* filename, int N);
//4.6
int write_matrix_mpi_fw_stripe(int*data, char* filename, int N);
//6.1
int write_matrix_mpi_fw_stripe_improved(int*data, char* filename, int N);

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

