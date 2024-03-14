#include "main.h"


int main(int argc, char *argv[]) 
{

    struct timeval stop, start;
    char *input1 = INPUT1_NAME;
    char *input2 = INPUT2_NAME;
    char *output = OUTPUT_NAME;

    matrixStruct *A = malloc(sizeof(matrixStruct));
    matrixStruct *B = malloc(sizeof(matrixStruct));
    void (*funcs[3])(multiplicationStruct *, matrixStruct *) = {MainMatrixMultiplication, MainRowMultiplication, MainElementMultiplication};

    if (argc == MATRICES_NUMBER+1){
        input1 = argv[1];
        input2 = argv[2];
        output = argv[3];
    }

    if (read_file(input1, A) || read_file(input2, B)) {
        printf("Error reading files\n");
        free(A);
        free(B);
        return 0;
    }

    matrixStruct *C[MATRICES_NUMBER];
    multiplicationStruct *data = malloc(sizeof(multiplicationStruct));
    heapStruct *heap = malloc(sizeof(heapStruct));

    heap->A = A;
    heap->B = B;

    for (int i = 0; i < MATRICES_NUMBER; ++i) {
        C[i] = malloc(sizeof(matrixStruct));
        C[i]->row = A->row;
        C[i]->col = B->col;
        C[i]->matrix = malloc(C[i]->row * C[i]->col * sizeof(int));
        heap->C[i] = C[i];
    }

    data->row = A->row;
    data->col = B->col;
    data->pad = A->col;
    data->A = A->matrix;
    data->B = B->matrix;
    heap->D = data;

    if (A->col != B->row){
        printf("Sizes of matrices are incompatible");
        free_heap(heap);
        return 0;
    }

    for (int i = 0; i < MATRICES_NUMBER; ++i) {
        gettimeofday(&start, NULL);
        funcs[i](data, C[i]);
        gettimeofday(&stop, NULL);
        printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
        printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    }

    write_file(output, MATRIX_NAME_EXT, C[0]);
    write_file(output, ROW_NAME_EXT, C[1]);
    write_file(output, ELEMENT_NAME_EXT, C[2]);

    free_heap(heap);
    return 0;
}


int read_file(char *file_name, matrixStruct *matrix)
{
    FILE *input_file;
    char * line = NULL, full_name[256];
    size_t len = 0;
    int row, col;

    strcpy(full_name, file_name);
    input_file = fopen(full_name, "r");
    if (input_file == NULL)
        return -1;

    getline(&line, &len, input_file);
    strtok(line, "=");
    row = atoi(strtok(NULL, " "));
    strtok(NULL, "=");
    col = atoi(strtok(NULL, "\0"));

    matrix->row = row;
    matrix->col = col;

    int (*array)[col] = malloc(row * col * sizeof(int));
    int row_c = 0, col_c = 0;
    while (getline(&line, &len, input_file) != -1) {
        char *num = strtok(line, "\t");

        while (col_c < col && num != NULL) {
            array[row_c][col_c] = atoi(num);
            num = strtok(NULL, "\t");
            col_c++;
        }

        row_c++;
        col_c = 0;
    }

    fclose(input_file);
    if (line)
        free(line);

    matrix->matrix = array;
    return 0;
}


void write_file(char *file_name, char *extension, matrixStruct *matrix)
{
    FILE *output_file;
    char full_name[256];

    strcpy(full_name, file_name);
    strcat(full_name, extension);
    strcat(full_name, ".txt");
    output_file = fopen(full_name, "w");

    if (output_file == NULL)
        return;

    fprintf(output_file, "row=%d col=%d\n", matrix->row, matrix->col);
    for (int i = 0; i < matrix->row; ++i) {
        for (int j = 0; j < matrix->col; ++j) {
            fprintf(output_file, "%d ", ((int (*)[matrix->col])matrix->matrix)[i][j]);
        }
        fprintf(output_file, "\n");
    }

    fclose(output_file);
}


void *ThreadMatrixMultiplication(void *args)
{
    functionStruct *data = args;
    for (int i = 0; i < data->ms->row; ++i) {
        for (int j = 0; j < data->ms->col; ++j) {
            ((int (*)[data->ms->col]) data->C)[i][j] = 0;
            for (int k = 0; k < data->ms->pad; ++k) {
                ((int (*)[data->ms->col]) data->C)[i][j] += ((int (*)[data->ms->pad]) data->ms->A)[i][k] * ((int (*)[data->ms->col]) data->ms->B)[k][j];
            }
        }
    }
    free(data);
    pthread_exit(0);
}


void *ThreadRowMultiplication(void *args)
{
    functionStruct *data = args;
    for (int i = 0; i < data->ms->col; ++i) {
        ((int (*)[data->ms->col])data->C)[data->cur_row][i] = 0;
        for (int j = 0; j < data->ms->pad; ++j) {
            ((int (*)[data->ms->col])data->C)[data->cur_row][i] += ((int (*)[data->ms->pad])data->ms->A)[data->cur_row][j] * ((int (*)[data->ms->col])data->ms->B)[j][i];
        }
    }
    free(data);
    pthread_exit(0);
}


void *ThreadElementMultiplication(void *args)
{
    functionStruct *data = args;
    ((int (*)[data->ms->col])data->C)[data->cur_row][data->cur_col] = 0;
    for (int i = 0; i < data->ms->pad; ++i) {
        ((int (*)[data->ms->col])data->C)[data->cur_row][data->cur_col] += ((int (*)[data->ms->pad])data->ms->A)[data->cur_row][i] * ((int (*)[data->ms->col])data->ms->B)[i][data->cur_col];
    }
    free(data);
    pthread_exit(0);
}


void MainMatrixMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix)
{
    pthread_t thread;
    functionStruct *special_data = malloc(sizeof(functionStruct));
    special_data->C = output_matrix->matrix;
    special_data->ms = common_data;
    pthread_create(&thread, NULL, ThreadMatrixMultiplication, special_data);
    pthread_join(thread, NULL);
}


void MainRowMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix)
{
    int threads_num = common_data->row;
    pthread_t threads[threads_num];

    for (int i = 0; i < common_data->row; ++i) {
        functionStruct *special_data = malloc(sizeof(functionStruct));
        special_data->cur_row = i;
        special_data->C = output_matrix->matrix;
        special_data->ms = common_data;
        pthread_create(&threads[i], NULL, ThreadRowMultiplication, special_data);
    }

    for (int i = 0; i < threads_num; ++i) {
        pthread_join(threads[i], NULL);
    }
}


void MainElementMultiplication(multiplicationStruct *common_data, matrixStruct *output_matrix)
{
    int threads_num = common_data->row * common_data->col;
    pthread_t threads[threads_num];

    for (int i = 0; i < common_data->row; ++i) {
        for (int j = 0; j < common_data->col; ++j) {
            functionStruct *special_data = malloc(sizeof(functionStruct));
            special_data->cur_row = i;
            special_data->cur_col = j;
            special_data->C = output_matrix->matrix;
            special_data->ms = common_data;
            pthread_create(&threads[i*common_data->col+j], NULL, ThreadElementMultiplication, special_data);
        }
    }

    for (int i = 0; i < threads_num; ++i) {
        pthread_join(threads[i], NULL);
    }
}


void free_heap(heapStruct *heap)
{
    free(heap->A->matrix);
    free(heap->A);
    free(heap->B->matrix);
    free(heap->B);
    for (int i = 0; i < MATRICES_NUMBER; ++i) {
        free(heap->C[i]->matrix);
        free(heap->C[i]);
    }
    free(heap->D);
    free(heap);
}