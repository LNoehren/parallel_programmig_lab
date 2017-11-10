#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

int read_matrix_bin(int* data, char* filename, int N){
        FILE *file;
        
        file = fopen(filename, "rb");
        if(!file) {
                printf("unable to open file\n");
                return 1;
        }
        fread(data, sizeof(int)*N*N, 1, file);
       	fclose(file);
	return 0;
}


int read_matrix_data(int* data, char* filename, int N){
        FILE *file;
        char buffer[N];
        
	file = fopen(filename, "r");
        if(!file) {
                printf("unable to open file\n");
                return 1;
        }
	size_t nread;
	int count = 0;
	
	//read data file and only put digits in matrix
	while((nread = fread(buffer, sizeof(buffer), 1, file)) > 0){
		char* numbers = strtok(buffer, ";");
		int i = 0;
		while(numbers){
			if(numbers == "\n") continue;
			data[count] = atoi(numbers);	
			i++;
			count++;
			numbers = strtok(NULL, ";");
		}
	}
	fclose(file);
	return 0;
}

