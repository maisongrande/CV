/*
 * ============================================================================
 *
 *       Filename:  model.h
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/20/2016 12:53:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#ifndef  MODEL_H
#define  MODEL_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <string.h>
#include "geometry.h"
#include "TGAimage.h"
#include "linalg.h"

#define LINE_MAX 1000

typedef struct model
{
	uint32_t numVerts;
	uint32_t numFaces;
	uint32_t numTextures;
	vec3f32 * vertex;
	int32_t ** faceIndex;
	vec3f32 * texture;
	int32_t ** textureIndex;
	tga_image * diffusionMap;
} model;

void makeModel(const char * filename, model * m);
uint32_t numVertecies(FILE * fp);
uint32_t numFaces(FILE * fp);
uint32_t numTextures(FILE * fp);
int32_t * processFaceAllocs(const char * line);
int32_t * processTextureAllocs(const char * line);
vec3f32 fillVec3f32FromLine(const char * line);
v4si getTextureCoordinatesV4si(int32_t xIndex, int32_t yIndex, const model m);

#endif   /*__MODEL_H__*/
