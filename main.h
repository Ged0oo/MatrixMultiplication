#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define MATRICES_NUMBER 3
#define INPUT1_NAME "a"
#define INPUT2_NAME "b"
#define OUTPUT_NAME "c"
#define MATRIX_NAME_EXT "_per_matrix"
#define ROW_NAME_EXT "_per_row"
#define ELEMENT_NAME_EXT "_per_element"

typedef struct {
    int row, col;
    int (*matrix)[];
}matrixStruct ;

typedef struct {
    int row, col, pad;
    int (*A)[];
    int (*B)[];
}multiplicationStruct ;

typedef struct {
    int cur_row, cur_col;
    int (*C)[];
    multiplicationStruct *ms;
}functionStruct ;

typedef struct {
    matrixStruct *A;
    matrixStruct *B;
    matrixStruct *C[MATRICES_NUMBER];
    multiplicationStruct *D;
}heapStruct ;

int read_file(char *file_name, matrixStruct *matrix);
void write_file(char *file_name, char *extension, matrixStruct *matrix);
void free_heap(heapStruct *heap);

void *ThreadMatrixMultiplication(void *args);
void *ThreadRowMultiplication(void *args);
void *ThreadElementMultiplication(void *args);

void MainMatrixMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix);
void MainRowMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix);
void MainElementMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix);

#endif //__MAIN_H__















