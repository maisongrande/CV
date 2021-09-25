#ifndef  TINYRENDERER_H
#define  TINYRENDERER_H
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <omp.h>
#include <float.h>
#include "point.h"
#include "geometry.h"
#include "TGAimage.h"
#include "model.h"
#include "matrix.h"
#include "linalg.h"


typedef int32_t bool32;

#define Abs(x) ((x) < 0 ? (-1 * (x)) : (x))
#define Max(x, y) (((x) > (y))? (x) : (y))
#define Min(x, y) (((x) < (y))? (x) : (y))


#define SwapI32(a, b) 			\
	do {				\
		int32_t tmp = *(a);	\
		*(a) = *(b);		\
		*(b) = tmp;		\
	} while(0);

#define true 1
#define false 0

#define VERT_A 0
#define VERT_B 1
#define VERT_C 2

#define X_COOR 0
#define Y_COOR 1
#define Z_COOR 2


tga_color white = {{.r = 255, .g = 255, .b = 255, .a = 255}};
tga_color red = {{.r = 255, .g = 0, .b = 0, .a = 255}};
tga_color green = {{.r = 0, .g = 255, .b = 0, .a = 255}};
tga_color blue = {{.r = 0, .g = 0, .b = 255, .a = 255}};
tga_color yellow = {{.r = 255, .g = 255, .b = 0, .a = 255}};

vec3f32 scaleWorldToScreenCoordinates(tga_image image, vec3f32 world);
void line2d(point2d start, point2d end, tga_image * image, tga_color color);
void drawTriangle(triangle2d t, tga_image * image, tga_color color);
void drawTriangleWithZaxis(triangle3d t, float * zBuffer,
		tga_image * image, tga_color color);
void drawAndFillPartOfTriangle(point2d X, point2d Y, triangle2d t,
		tga_image * image, tga_color color);
void drawTriangleTexture(v4si * triangle, v4si * texture, float intensity,
		float * zBuffer, tga_image *image, model * m);
void drawAndFillPartOfTriangleTexture(v4si X, v4si Y, v4si uvX, v4si uvY,
		v4si * triangle, v4si * texture, float intensity,
		float * zBuffer, tga_image * image, model * m);
int32_t roundFloatToInt32(float x);
v4si v4sfTOv4si(v4sf v);
v4sf v4siTOv4sf(v4si v);
void dumpZbufferToTGA(uint32_t width, uint32_t height, float * zBuffer,
		const char * filename);
#endif   /*__TINYRENDERER_H__*/

