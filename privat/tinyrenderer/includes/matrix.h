/*
 * ============================================================================
 *
 *       Filename:  matrix.h
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/22/2016 07:31:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */

#ifndef  MATRIX_H
#define  MATRIX_H
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct matrix
{
	uint32_t rows, cols;
	float ** data;
} matrixF32;


matrixF32 * newMatrixF32(uint32_t rows, uint32_t cols);
matrixF32 * identityMatrixF32(uint32_t dim);
void transposeMatrixF3(matrixF32 * m);
matrixF32 * makeTransposedMatrixF3(matrixF32 * m);
matrixF32 * matrixProduct(const matrixF32 * m1, const matrixF32 * m2);
#endif   /*__MATRIX_H__*/
