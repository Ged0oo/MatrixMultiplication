#include "main.h"


int main(int argc, char *argv[]) 
{
    char *file1 = INPUT1, *file2 = INPUT2, *output = OUTPUT;

    matrixStruct *matA = malloc(sizeof(matrixStruct));
    matrixStruct *matB = malloc(sizeof(matrixStruct));

    void (*MulFucntions[3])(multiplicationStruct *, matrixStruct *) = 
    {
    	MainMatrixMultiplication, 
    	MainRowMultiplication, 
    	MainElementMultiplication
    };

    if (argc == MATRICES_NUMBER+1)
    {
        file1 = argv[1];
        file2 = argv[2];
        output = argv[3];
    }
    else
    {
    	printf("Error in the Passed Parameters.\n");
        free(matA);
        free(matB);
        return 0;
    }

    if (read_file(file1, matA) || read_file(file2, matB))
    {
        printf("Error while Reading File.\n");
        free(matA);
        free(matB);
        return 0;
    }

    matrixStruct *matC[MATRICES_NUMBER];
    multiplicationStruct *data = malloc(sizeof(multiplicationStruct));
    heapStruct *heap = malloc(sizeof(heapStruct));

    heap->matA = matA;
    heap->matB = matB;

    for (int i = 0; i < MATRICES_NUMBER; ++i) 
    {
        matC[i] = malloc(sizeof(matrixStruct));
        matC[i]->row = matA->row;
        matC[i]->col = matB->col;
        matC[i]->matrix = malloc(matC[i]->row * matC[i]->col * sizeof(int));
        heap->matC[i] = matC[i];
    }

    data->row = matA->row;
    data->col = matB->col;
    data->pad = matA->col;

    data->matA = matA->matrix;
    data->matB = matB->matrix;
    heap->matD = data;

    if (matA->col != matB->row)
    {
        printf("Error Array Sizes are incompatible.\n");
        free_heap(heap);
        return 0;
    }

    printf("\n");
    struct timeval stop, start;
    for (int i = 0; i < MATRICES_NUMBER; ++i) 
    {
        gettimeofday(&start, NULL);
        MulFucntions[i](data, matC[i]);
        gettimeofday(&stop, NULL);
        printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
        printf("Microseconds taken: %lu\n\n", stop.tv_usec - start.tv_usec);
    }

    write_file(output, MATRIX_NAME_EXT, THREAD_PER_MATRIX, matC[0]);
    write_file(output, ROW_NAME_EXT, THREAD_PER_ROW, matC[1]);
    write_file(output, ELEMENT_NAME_EXT, THREAD_PER_ELEMENT, matC[2]);

    free_heap(heap);
    return 0;
}


int read_file(char *file_name, matrixStruct *matrix)
{
    char * line = NULL, fullName[256];
    strcpy(fullName, file_name);

    int row, col;
    size_t length = 0;


    FILE *inputFile;
    inputFile = fopen(fullName, "r");
    if (inputFile == NULL)
        return 0;

    getline(&line, &length, inputFile);
    strtok(line, "=");
    row = atoi(strtok(NULL, " "));
    strtok(NULL, "=");
    col = atoi(strtok(NULL, "\0"));

    matrix->row = row;
    matrix->col = col;

    int (*array)[col] = malloc(row * col * sizeof(int));
    int row_c = 0, col_c = 0;

    while (getline(&line, &length, inputFile) != -1) 
    {
        char *num = strtok(line, "\t");

        while (col_c < col && num != NULL) 
        {
            array[row_c][col_c] = atoi(num);
            num = strtok(NULL, "\t");
            col_c++;
        }

        row_c++;
        col_c = 0;
    }

    fclose(inputFile);
    if (line)
    {
        free(line);
    }

    matrix->matrix = array;
    return 0;
}


void write_file(char *file_name, char *extension, char *entry, matrixStruct *matrix)
{
    FILE *output_file;
    char fullName[256];

    int row = matrix->row;
    int col = matrix->col;
    int val;

    strcpy(fullName, file_name);
    strcat(fullName, extension);
    strcat(fullName, ".txt");

    output_file = fopen(fullName, "w");

    if (output_file == NULL)
        return;

    fprintf(output_file, "%s\n", entry);
    fprintf(output_file, "row=%d col=%d\n", row, col);

    for (int i=0 ; i < row ; ++i) 
    {
        for (int j = 0; j < col; ++j) 
        {
        	val = *((*(matrix->matrix) + i) + j);
            fprintf(output_file, "%d ", val);
        }
        fprintf(output_file, "\n");
    }

    fclose(output_file);
}


void free_heap(heapStruct *heap)
{
    free(heap->matA->matrix);
    free(heap->matA);

    free(heap->matB->matrix);
    free(heap->matB);
    
    for (int i=0 ; i < MATRICES_NUMBER ; ++i) 
    {
        free(heap->matC[i]->matrix);
        free(heap->matC[i]);
    }
    
    free(heap->matD);
    free(heap);
}


void MainMatrixMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix)
{
    functionStruct *_data = malloc(sizeof(functionStruct));
    _data->matC = output_matrix->matrix;
    _data->ms = common_data;

 	pthread_t thread;
    pthread_create(&thread, NULL, ThreadMatrixMultiplication, _data);
    pthread_join(thread, NULL);
}


void *ThreadMatrixMultiplication(void *args)
{
    functionStruct *data = args;

    int row = data->ms->row;
    int col = data->ms->col;
    int pad = data->ms->pad;

    for (int i = 0; i < row; ++i) 
    {
        for (int j = 0; j < col; ++j) 
        {
            ((int (*)[col]) data->matC)[i][j] = 0;
            for (int k = 0; k < data->ms->pad; ++k) 
            {
                ((int (*)[col]) data->matC)[i][j] += ((int (*)[data->ms->pad]) data->ms->matA)[i][k] * ((int (*)[data->ms->col]) data->ms->matB)[k][j];
            }
        }
    }

    free(data);
    pthread_exit(0);
}


void MainRowMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix)
{
    int threads_num = common_data->row;
    pthread_t threads[threads_num];

    for (int i=0 ; i < common_data->row ; ++i) 
    {
        functionStruct *_data = malloc(sizeof(functionStruct));
        _data->cur_row = i;
        _data->matC = output_matrix->matrix;
        _data->ms = common_data;
        pthread_create(&threads[i], NULL, ThreadRowMultiplication, _data);
    }

    for (int i=0 ; i < threads_num ; ++i) 
    {
        pthread_join(threads[i], NULL);
    }
}


void *ThreadRowMultiplication(void *args)
{
	int row = data->ms->row;
    int col = data->ms->col;
    int pad = data->ms->pad;

    functionStruct *data = args;
    for (int i = 0; i < col; ++i) 
    {
        ((int (*)[col]) data->matC)[data->cur_row][i] = 0;
        for (int j=0 ; j < pad ; ++j) 
        {
            ((int (*)[data->ms->col])data->matC)[data->cur_row][i] += ((int (*)[pad]) data->ms->matA)[data->cur_row][j] * ((int (*)[col]) data->ms->matB)[j][i];
        }
    }
    free(data);
    pthread_exit(0);
}


void MainElementMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix)
{
    int threads_num = common_data->row * common_data->col;
    pthread_t threads[threads_num];

    for (int i = 0 ; i < common_data->row ; ++i) 
    {
        for (int j = 0 ; j < common_data->col ; ++j) 
        {
            functionStruct *_data = malloc(sizeof(functionStruct));
            _data->cur_row = i;
            _data->cur_col = j;
            _data->matC = output_matrix->matrix;
            _data->ms = common_data;
            pthread_create(&threads[i * common_data->col+j], NULL, ThreadElementMultiplication, _data);
        }
    }

    for (int i = 0; i < threads_num; ++i) 
    {
        pthread_join(threads[i], NULL);
    }
}


void *ThreadElementMultiplication(void *args)
{
	int row = data->ms->row;
    int col = data->ms->col;
    int pad = data->ms->pad;

    functionStruct *data = args;
    ((int (*)[col]) data->matC)[data->cur_row][data->cur_col] = 0;
    for (int i = 0 ; i < pad ; ++i) 
    {
        ((int (*)[col]) data->matC)[data->cur_row][data->cur_col] += ((int (*)[pad]) data->ms->matA)[data->cur_row][i] * ((int (*)[col]) data->ms->matB)[i][data->cur_col];
    }
    free(data);
    pthread_exit(0);
}
