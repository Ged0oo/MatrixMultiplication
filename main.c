#include "main.h"

int main(int argc, char *argv[])
{
	char *mat_1  = INPUT1;
	char *mat_2  = INPUT2;
	char *outMat = OUTPUT;

	Matrix_t *matA = (Matrix_t *)malloc(sizeof(Matrix_t));
	Matrix_t *matB = (Matrix_t *)malloc(sizeof(Matrix_t));

	void (*fun[3])(MultiplicationMatrix_t *, Matrix_t *) = 
	{
		MainMatrixMultiplication,
		MainRowMultiplication,
		MainElementMultiplication
	};

	if(argc == MATRICES_NUMBER+1)
	{
		mat_1  = argv[1];
		mat_2  = argv[2];
		outMat = argv[3];
	}
	else
	{
		printf("Error in Passed Arguments\n");
		return 0;
	}

	int ret_1 = read_file(mat_1, matA);
	int ret_2 = read_file(mat_2, matB);
	if(ret_1 || ret_2)
	{
		printf("Error in reading input files\n");
		free(matA);
		free(matB);
		return 0;
	}

	Matrix_t *matC[MATRICES_NUMBER];
	MultiplicationMatrix_t *dataMat = (MultiplicationMatrix_t *)malloc(sizeof(MultiplicationMatrix_t));
	heap_st *heap = (heap_st *)malloc(sizeof(heap_st));
	heap->matrixA = matA;
	heap->matrixB = matB;

	for(int i=0 ; i<MATRICES_NUMBER ; i++)
	{
		matC[i] = (Matrix_t *)malloc(sizeof(Matrix_t));
		matC[i]->row = matA->row;
		matC[i]->col = matB->col;
		matC[i]->matrix = malloc(sizeof(int) * matC[i]->row * matC[i]->col);
		heap->matrixC[i] = matC[i];
	}

    if (matA->col != matB->row)
    {
        printf("Error Matrices Sizes are incompatible\n");
        free_heap(heap);
        return 0;
    }

    struct timeval stop, start;
    for (int i=0 ; i<MATRICES_NUMBER ; i++) 
    {
        gettimeofday(&start, NULL);
        fun[i](dataMat, matC[i]);
        gettimeofday(&stop, NULL);
        printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
        printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    }

    write_file(outMat, MATRIX_NAME_EXT, matC[0]);
    write_file(outMat, ROW_NAME_EXT, matC[1]);
    write_file(outMat, ELEMENT_NAME_EXT, matC[2]);

    free_heap(heap);
	return 0;
}

int read_file(char *file_name, Matrix_t *matrix)
{

}


void write_file(char *file_name, char *extension, Matrix_t *matrix)
{

}


void free_heap(heap_st *heap)
{

}

void *ThreadMatrixMultiplication(void *args)
{

}

void *ThreadRowMultiplication(void *args)
{

}

void *ThreadElementMultiplication(void *args)
{

}

void MainMatrixMultiplication(MultiplicationMatrix_t *common_data, Matrix_t *output_matrix)
{

}

void MainRowMultiplication(MultiplicationMatrix_t *common_data, Matrix_t *output_matrix)
{

}

void MainElementMultiplication(MultiplicationMatrix_t *common_data, Matrix_t *output_matrix)
{

}
