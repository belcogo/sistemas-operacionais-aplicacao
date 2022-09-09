#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ROW_S 2
#define COL_S 2
#define DEFAULT_LENGTH_ARRAY 2
// #define DEFAULT_LENGTH_MATRIX 512

void create_matrix(int random, int matrix[ROW_S][COL_S]);
void print_matrix(int matrix[ROW_S][COL_S]);
void multiply_matrix(int matrix_a[ROW_S][COL_S], int matrix_b[ROW_S][COL_S]);

int main() {
    int matrix_a[ROW_S][COL_S];
    int matrix_b[ROW_S][COL_S];
    int file_pipes[DEFAULT_LENGTH_ARRAY];
    pid_t fork_result;

    if (pipe(file_pipes) == 0) {
        fork_result = fork();
        
        if (fork_result == 0) {
            create_matrix((rand() % 10), &matrix_b); 
            close(file_pipes[1]);
            read(file_pipes[0], &matrix_a, sizeof(matrix_a));
            multiply_matrix(matrix_a, matrix_b);
            close(file_pipes[0]);
        } else {
            //parent 
            create_matrix((rand() % 10), &matrix_a);
            close(file_pipes[0]);
            write(file_pipes[1], matrix_a, sizeof(matrix_a));
            close(file_pipes[1]);
        }
    }

    return 0;
}

void create_matrix(int random, int matrix[ROW_S][COL_S]) {
    int i, j;

    for (i = 0; i < ROW_S; i++) {
        for (j = 0; j < COL_S; j++) {
            matrix[i][j] = ((rand() * random) % 10);
        }
    }

    // print_matrix(matrix);
}

void print_matrix(int matrix[ROW_S][COL_S]) {
    int i, j;

    for (i = 0; i < ROW_S; i++) {
        printf("| ");
        for (j = 0; j < COL_S; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf(" |\n");
    }
}

void multiply_matrix(int matrix_a[ROW_S][COL_S], int matrix_b[ROW_S][COL_S]) {
    print_matrix(matrix_a);
    print_matrix(matrix_b);

    int matrix_c[ROW_S][COL_S], i, j, k;

    for(i = 0; i < ROW_S; i++) {
        for (j = 0; j < COL_S; j++) {
            matrix_c[i][j] = 0;
            for(k = 0; k < COL_S; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }

    print_matrix(matrix_c);
}
