#ifndef  MATRIX_H
#define  MATRIX_H
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <omp.h>

#define SWAP(a, b) 						\
	do {							\
		double __tmp = *a;				\
		*a = *b;					\
		*b = __tmp;					\
	} while (0);

void
read_matrix_binaryformat(char *filename, double ***matrix, int *rows, int *cols);

void
write_matrix_binaryformat(char *filename, double **matrix, int rows, int cols);

void
transpose(double ***mat, int *rows, int *cols);

void
print_matrix(double **mat, int rows, int cols);

double **
allocate_matrix2d(int rows, int cols);

void
matrix_multiply(double **matrixA, double **matrixB,
		int rowsA, int colsA, int rowsB, int colsB,
		double **result);
#endif   /*__MATRIX_H__*/
