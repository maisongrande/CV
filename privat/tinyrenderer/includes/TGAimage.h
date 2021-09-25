#ifndef  TGAIMAGE_H
/*
 * ============================================================================
 *
 *       Filename:  TGAimage.h
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/18/2016 09:02:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#define  TGAIMAGE_H
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int32_t bool32;

#define Assert(expression) 				\
	if (!(expression)) { 				\
		fprintf(stderr, "Assertion fail!\n"); 	\
		*(int *)0 = 0; 				\
	}

enum {GRAYSCALE=1, RGB=3, RGBA=4};

//TGA FIELD 3, Image type
enum {
	NO_IMAGE_DATA=0,
	UNCOMP_COLORMAPPED=1,
	UNCOMP_TRUECOLOR=2,
	UNCOMP_GRAYSCALE=3,
	RLE_COLORMAPPED=9,
	RLE_TRUECOLOR=10,
	RLE_GRAYSCALE=11
};


typedef union tga_color
{
	struct
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};
	uint8_t raw[4];
	uint32_t value;
} tga_color;


struct tga_header
{
	int8_t idlength;
	int8_t colormaptype;
	int8_t datatypecode;
	int16_t colormaporigin;
	int16_t colormaplength;
	int8_t colormapdepth;
	int16_t x_origin;
	int16_t y_origin;
	int16_t width;
	int16_t height;
	int8_t  bitsperpixel;
	int8_t  imagedescriptor;
} __attribute__((packed));
typedef struct tga_header tga_header;


typedef struct tga_image
{
	int32_t width;
	int32_t height;
	int32_t bytesPerPixel;
	uint32_t pitch;
	uint32_t size;
	uint8_t * data;
} tga_image;

void makeTGAImage(tga_image * image, int32_t width, int32_t height, uint8_t format);
void readTGAImage(tga_image * image, const char * filename);
void writeTGAImage(tga_image * image, const char * filename, bool32 rle);
void setTGAImagePixel(int32_t x, int32_t y, tga_image * image, tga_color color);
tga_color getTGAImagePixel(tga_image * image, int32_t x, int32_t y);
void flipTGAImageVerticaly(tga_image * image);
void flipTGAImageHorizontally(tga_image * image);
void decompress_RLE(FILE * fp, tga_image * image);

#endif   /*__TGAIMAGE_H__*/
