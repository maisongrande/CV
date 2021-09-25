/*
 * ============================================================================
 *
 *       Filename:  matrix.c
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/22/2016 07:30:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#include "matrix.h"

matrixF32 * newMatrixF32(uint32_t rows, uint32_t cols)
{
	matrixF32 * result = malloc(sizeof(*result));
	float * data = calloc(cols * rows, sizeof(float));
	float ** mat = malloc(rows * sizeof(float *));
	if (result && data && mat) {
		for (uint32_t i = 0; i < rows; i++) {
			mat[i] = &data[i * cols];
		}
	} else {
		fprintf(stderr, "Matrix alloc failed\n");
		exit(EXIT_FAILURE);
	}
	result->rows = rows;
	result->cols = cols;
	result->data = mat;
	return result;
}

matrixF32 * identityMatrixF32(uint32_t dim)
{
	matrixF32 * result = newMatrixF32(dim, dim);
	for (uint32_t r = 0; r < dim; r++) {
		for (uint32_t c = 0; c < dim; c++) {
			result->data[r][c] = (r==c) ? 1 : 0;
		}
	}
	return result;
}


matrixF32 * makeTransposedMatrixF3(matrixF32 * m)
{
	if (m->rows == m->cols) {
		matrixF32 * result = newMatrixF32(m->rows, m->cols);
		for (uint32_t i = 0; i < m->rows; i++) {
			for (uint32_t j = 0; j < m->cols; j++) {
				result->data[j][i] = m->data[i][j];
			}
		}
		return result;
	} else {
		fprintf(stderr, "Transponering ikke implementert\n");
		exit(EXIT_FAILURE);
	}
}


void transposeMatrixF3(matrixF32 * m)
{
	if (m->rows == m->cols) {
		for (uint32_t i = 0; i < m->rows; i++) {
			for (uint32_t j = i; j < m->cols; j++) {
				float tmp = m->data[i][j];
				m->data[i][j] = m->data[j][i];
				m->data[j][i] = tmp;
			}
		}
	} else {
		fprintf(stderr, "Transponering ikke implementert\n");
		exit(EXIT_FAILURE);
	}
}

matrixF32 * matrixProduct(const matrixF32 * m1, const matrixF32 * m2)
{
	matrixF32 * result = result = newMatrixF32(m1->rows, m2->cols);

	for (uint32_t i = 0; i < m1->rows; i++) {
		for (uint32_t j = 0; j < m2->cols; j++) {
			for (uint32_t k = 0; k < m1->cols; k++) {
				result->data[i][j] +=
					m1->data[i][k] * m2->data[k][j];
			}
		}
	}
	return result;
}
