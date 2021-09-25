/*
 * ============================================================================
 *
 *       Filename:  geometry.c
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/21/2016 04:36:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#include "geometry.h"

vec3f32 crossProductF32(vec3f32 a, vec3f32 b)
{
	vec3f32 result = {
		.x = ((a.y * b.z) - (a.z * b.y)),
		.y = ((a.z * b.x) - (a.x * b.z)),
		.z = ((a.x * b.y) - (a.y * b.x))
	};
	return result;
}

float dotProductF32(vec3f32 a, vec3f32 b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

vec3f32 subtractVec3f32(vec3f32 a, vec3f32 b)
{
	vec3f32 result = {
		.x = (a.x - b.x),
		.y = (a.y - b.y),
		.z = (a.z - b.z)
	};
	return result;
}


void normalizeF32(vec3f32 * v)
{
	float scale = 1.0f / sqrt((v->x*v->x) + (v->y*v->y) + (v->z*v->z));
	v->x = v->x * scale;
	v->y = v->y * scale;
	v->z = v->z * scale;
}


void swapVec3f32(vec3f32 * a, vec3f32 * b)
{
	vec3f32 tmp = {.x = a->x, .y = a->y, .z = a->z};
	*a = (struct vec3f32){.x = b->x, .y = b->y, .z = b->z};
	*b = (struct vec3f32){.x = tmp.x, .y = tmp.y, .z = tmp.z};
}
