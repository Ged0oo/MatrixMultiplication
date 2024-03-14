#ifndef __MAIN_H__
#define __MAIN_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


#define MATRICES_NUMBER         3
#define INPUT1                  "a"
#define INPUT2                  "b"
#define OUTPUT                  "c"

#define MATRIX_NAME_EXT         "_per_matrix"
#define ROW_NAME_EXT            "_per_row"
#define ELEMENT_NAME_EXT        "_per_element"

#define THREAD_PER_MATRIX       "Method: A thread per matrix"
#define THREAD_PER_ROW          "Method: A thread per row"
#define THREAD_PER_ELEMENT      "Method: A thread per element"

/*
 * This is a struct that defines a matrix.
 * The row and col variables store the dimensions of the matrix.
 * matrix is a pointer to the matrix data it self.
 */
typedef struct 
{
    int row, col;
    int (*matrix)[];
}matrixStruct ;


/*
 * This is a struct that defines the data necessary for matrix multiplication.
 * row and col store the dimensions of the result matrix,
 * pad is the shared dimension of the two input matrices, 
 * A and B are pointers to the two input matrices.
 */
typedef struct 
{
    int row, col, pad;
    int (*matA)[];
    int (*matB)[];
}multiplicationStruct ;


/*
 * This is a struct that defines the necessary data for each thread of matrix multiplication.
 * cur_row and cur_col store the current row and column of the output matrix being computed, 
 * matC is a pointer to the output matrix.
 * ms is a pointer to the struct containing the data necessary for matrix multiplication.
 */
typedef struct 
{
    int cur_row, cur_col;
    int (*matC)[];
    multiplicationStruct *ms;
}functionStruct ;


/*
 * This is a struct that defines the heap of matrices used in the program.
 * matA and matB are pointers to the two input matrices, 
 * matC is an array of pointers to the output matrices,
 * matD is a pointer to the struct containing the data necessary for matrix multiplication.
 */
typedef struct 
{
    matrixStruct *matA;
    matrixStruct *matB;
    matrixStruct *matC[MATRICES_NUMBER];
    multiplicationStruct *matD;
}heapStruct ;


int read_file(char *file_name, matrixStruct *matrix);
void write_file(char *file_name, char *extension, char *entry, matrixStruct *matrix);
void free_heap(heapStruct *heap);


void *ThreadMatrixMultiplication(void *args);
void *ThreadRowMultiplication(void *args);
void *ThreadElementMultiplication(void *args);


void MainMatrixMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix);
void MainRowMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix);
void MainElementMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix);


#endif //__MAIN_H__
