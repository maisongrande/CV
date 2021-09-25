/*
 * ============================================================================
 *
 *       Filename:  geometry.h
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/20/2016 12:59:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#ifndef  GEOMETRY_H
#define  GEOMETRY_H
#include <inttypes.h>
#include <math.h>
#include "point.h"


typedef struct vec2f32
{
	float x,y;
} vec2f32;

typedef struct vec2i32
{
	int32_t x,y;
} vec2i32;

typedef struct vec3f32
{
	float x,y,z;
} vec3f32;

typedef struct vec3i32
{
	int32_t x,y,z;
} vec3i32;

typedef struct triangle2d
{
	point2d A;
	point2d B;
	point2d C;
} triangle2d;

typedef struct triangle3d
{
	vec3f32 A;
	vec3f32 B;
	vec3f32 C;
} triangle3d;

vec3f32 crossProductF32(vec3f32 a, vec3f32 b);
vec3f32 subtractVec3f32(vec3f32 a, vec3f32 b);
float dotProductF32(vec3f32 a, vec3f32 b);
void normalizeF32(vec3f32 * v);
void swapVec3f32(vec3f32 * a, vec3f32 * b);
#endif   /*__GEOMETRY_H__*/
