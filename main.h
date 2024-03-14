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

typedef struct 
{
    int row, col;
    int (*matrix)[];
}Matrix_t;

typedef struct  
{
    int row, col, pad;
    int (*MatrixA)[];
    int (*MatrixB)[];
}MultiplicationMatrix_t;

typedef struct  
{
    int cur_row, cur_col;
    int (*MatrixC)[];
    MultiplicationMatrix_t *ms;
}FunctionSt_t;


typedef struct 
{
    Matrix_t *A;
    Matrix_t *B;
    Matrix_t *C[MATRICES_NUMBER];
    MultiplicationMatrix_t *D;
}heap_st;

int read_file(char *file_name, Matrix_t *matrix);
void write_file(char *file_name, char *extension, Matrix_t *matrix);
void *thread_matrix_multiplication(void *args);
void *thread_row_multiplication(void *args);
void *thread_element_multiplication(void *args);
void main_matrix_multiplication(MultiplicationMatrix_t *common_data, Matrix_t *output_matrix);
void main_row_multiplication(MultiplicationMatrix_t *common_data, Matrix_t *output_matrix);
void main_element_multiplication(MultiplicationMatrix_t *common_data, Matrix_t *output_matrix);
void free_heap(heap_st *heap);

#endif //__MAIN_H__















