/*
 * ============================================================================
 *
 *       Filename:  point.c
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/18/2016 11:11:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#include "point.h"

inline void swapPoint2d(point2d * a, point2d * b)
{
	point2d tmp = {.x = a->x, .y = a->y};
	a->x = b->x;
	a->y = b->y;
	b->x = tmp.x;
	b->y = tmp.y;
}

inline void swapCoordinates2d(point2d *p)
{
	point2d tmp = {.x = p->x, .y = p->y };
	p->x = tmp.y;
	p->y = tmp.x;
}
