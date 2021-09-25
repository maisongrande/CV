/*
 * ============================================================================
 *
 *       Filename:  linalg.c
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/25/2016 08:44:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#include "linalg.h"

inline v4sf addV4sf(v4sf a, v4sf b)
{
	return a + b;
}

inline v4sf subV4sf(v4sf a, v4sf b)
{
	return a - b;
}

float dotV4sf(v4sf a, v4sf b)
{
	v4sf result = a * b;
	return result[0] + result[1] + result[2] + result[3];
}

v4sf crossV4si(v4sf a, v4sf b)
{
	v4si mask1 = (v4si){1,2,0,0};
	v4si mask2 = (v4si){2,0,1,0};

	v4sf part1 = __builtin_shuffle(a, mask1);
	v4sf part2 = __builtin_shuffle(b, mask2);
	v4sf res1 = part1 * part2;

	part1 = __builtin_shuffle(a, mask2);
	part2 = __builtin_shuffle(b, mask1);
	v4sf res2 = part1 * part2;

	v4sf result = res1 - res2;
	return result;
}

v4sf normalizeV4sf(v4sf vec)
{
#if 0
	v4sf part = vec * vec;
	float fact = 1.0f/sqrt(part[0]+part[1]+part[2]);
	v4sf result = vec * fact;
#else
	v4sf result = vec * vec;
	result = result +  _mm_shuffle_ps(result, result, 0x4e);
	result = vec / _mm_sqrt_ps(result + _mm_shuffle_ps(result, result, 0x11));
#endif
	return result;
}

v4sf matrix4VectorProdV4sf(v4sf vec, v4sf * mat)
{
	v4si replicateX = {0,0,0,0};
	v4si replicateY = {1,1,1,1};
	v4si replicateZ = {2,2,2,2};
	v4si replicateW = {3,3,3,3};

	v4sf xxxx = __builtin_shuffle(vec, vec, replicateX);
	v4sf yyyy = __builtin_shuffle(vec, vec, replicateY);
	v4sf zzzz = __builtin_shuffle(vec, vec, replicateZ);
	v4sf wwww = __builtin_shuffle(vec, vec, replicateW);

	v4sf mRow0 = xxxx * mat[0];
	v4sf mRow1 = yyyy * mat[1];
	v4sf mRow2 = zzzz * mat[2];
	v4sf mRow3 = wwww * mat[3];

	v4sf result = mRow0 + mRow1 + mRow2 + mRow3;
	return result;
}

v4sf matrix4Matrix1ProdV4sf(const v4sf * mat1, const v4sf mat2)
{
	v4sf result = {0};
	for (int i = 0; i < 4; i++) {
		v4sf tmp = mat1[i] * mat2;
		result[i] = tmp[0] + tmp[1] + tmp[2] + tmp[3];
	}
	return result;
}

v4sf * matrix4Matrix4ProdV4sf(const v4sf * mat1, const v4sf * mat2, v4sf * result)
{
	v4si replicateX = {0,0,0,0};
	v4si replicateY = {1,1,1,1};
	v4si replicateZ = {2,2,2,2};
	v4si replicateW = {3,3,3,3};

	for (int32_t i = 0; i < 4; i++) {
		v4sf xxxx = __builtin_shuffle(mat1[i], mat1[i], replicateX);
		v4sf yyyy = __builtin_shuffle(mat1[i], mat1[i], replicateY);
		v4sf zzzz = __builtin_shuffle(mat1[i], mat1[i], replicateZ);
		v4sf wwww = __builtin_shuffle(mat1[i], mat1[i], replicateW);

		v4sf mRow0 = xxxx * mat2[0];
		v4sf mRow1 = yyyy * mat2[1];
		v4sf mRow2 = zzzz * mat2[2];
		v4sf mRow3 = wwww * mat2[3];

		result[i] = mRow0 + mRow1 + mRow2 + mRow3;
	}
	return result;
}

inline void swapV4sf(v4sf * a, v4sf * b)
{
	v4sf tmp = *(v4sf*)a;
	*a = *(v4sf*)b;
	*b = tmp;
}

inline void swapV4si(v4si * a, v4si * b)
{
	v4si tmp = *(v4si*)a;
	*a = *(v4si*)b;
	*b = tmp;
}
