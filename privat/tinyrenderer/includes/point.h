/*
 * ============================================================================
 *
 *       Filename:  point.h
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/18/2016 11:12:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#ifndef  POINT_H
#define  POINT_H
#include <inttypes.h>

typedef struct point2d
{
	int32_t x;
	int32_t y;
} point2d;

void swapPoint2d(point2d * a, point2d * b);
void swapCoordinates2d(point2d * p);
#endif   /*__POINT_H__*/
