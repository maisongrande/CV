/*
 * ============================================================================
 *
 *       Filename:  linalg.h
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/25/2016 10:58:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#ifndef  LINALG_H
#define  LINALG_H
#include <inttypes.h>
#include <xmmintrin.h>
#include <math.h>

typedef float v4sf __attribute__((vector_size(16)));
typedef int32_t v4si __attribute__((vector_size(16)));


v4sf addV4sf(v4sf a, v4sf b);
v4sf subV4sf(v4sf a, v4sf b);
float dotV4sf(v4sf a, v4sf b);
v4sf crossV4si(v4sf a, v4sf b);
v4sf normalizeV4sf(v4sf vec);
v4sf matrix4VectorProdV4sf(v4sf vec, v4sf * mat);
v4sf * matrix4Matrix4ProdV4sf(const v4sf * mat1, const v4sf * mat2, v4sf * result);
v4sf matrix4Matrix1ProdV4sf(const v4sf * mat1, const v4sf mat2);
void swapV4sf(v4sf * a, v4sf * b);
void swapV4si(v4si * a, v4si * b);

#endif   /*__LINALG_H__*/
