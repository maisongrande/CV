/*
 * ============================================================================
 *
 *       Filename:  TGAimage.c
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/18/2016 09:14:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#include "TGAimage.h"


void readTGAImage(tga_image * image, const char * filename)
{
	tga_header header = {0};

	FILE * fp = fopen(filename, "rb");
	if (fp) {
		uint32_t error = 0;
		if ((error = fread(&header, sizeof(tga_header), 1, fp)) < 1) {
			fprintf(stderr, "Unable to read into header %u", error);
		}

		int16_t width = header.width;
		int16_t height = header.height;
		int8_t  bytesPerPixel = header.bitsperpixel >> 3;
		if ((header.width <= 0 || header.height <= 0) ||
				(bytesPerPixel != RGB &&
				 bytesPerPixel != RGBA &&
				 bytesPerPixel != GRAYSCALE))
		{
			fprintf(stderr, "Unable to correctly read tga file %s",
					filename);
			exit(EXIT_FAILURE);
		}

		image->bytesPerPixel = bytesPerPixel;
		image->width = width;
		image->height = height;
		image->size = width * height * bytesPerPixel;
		image->pitch = bytesPerPixel * width;

		int32_t bytesToRead = height*width*bytesPerPixel;
		image->data = malloc(bytesToRead * sizeof(uint8_t));
		if (!image->data) {
			fclose(fp);
			fprintf(stderr, "unable to alloc tga_image data\n");
			exit(EXIT_FAILURE);
		}

		if (header.datatypecode == UNCOMP_GRAYSCALE ||
				header.datatypecode == UNCOMP_TRUECOLOR)
		{
			if (fread(image->data, sizeof(uint8_t), bytesToRead, fp)
					< (bytesToRead * sizeof(uint8_t)))
			{
				fprintf(stderr, "unable to read tga_image data\n");
				fclose(fp);
				exit(EXIT_FAILURE);
			}
		} else if (header.datatypecode == RLE_GRAYSCALE ||
				header.datatypecode == RLE_TRUECOLOR)
		{
			decompress_RLE(fp, image);
		} else {
			fprintf(stderr, "Unknown tga format. file: %s\n", filename);
			fclose(fp);
			exit(EXIT_FAILURE);
		}

		if (!(header.imagedescriptor & 0x20)) {
			//TODO: Her er det noe rart..
			//flipTGAImageVerticaly(image);
		}

		if (header.imagedescriptor & 0x10) {
			printf("flip horizontaly, not implemented\n");
		}

	} else {
		fprintf(stderr, "Unable to open and read %s\n", filename);
	}
	fclose(fp);
}

void writeTGAImage(tga_image * image, const char * filename, bool32 rle)
{
	uint8_t dev_area_ref[4] = {0};
	uint8_t ext_area_ref[4] = {0};
	const char * footer = "TRUEVISION-XFILE.";

	FILE * fp = fopen(filename, "wb");
	if (fp) {
		tga_header header = {0};
		header.bitsperpixel = image->bytesPerPixel << 3;
		header.width = image->width;
		header.height = image->height;
		header.datatypecode =
			((image->bytesPerPixel == GRAYSCALE)
			 ? (rle ? 11:3) : (rle ? 10:2));
		header.imagedescriptor = 0x20;

		if (fwrite((uint8_t *)&header, 1, sizeof(header), fp)
				< (sizeof(header)))
		{
			fprintf(stderr, "Unable to write to file");
			exit(EXIT_FAILURE);
		}

		if (!rle) {
			if (fwrite(image->data, sizeof(uint8_t), image->size, fp)
					!= image->size)
			{
				fprintf(stderr, "Unable to write image data\n");
				exit(EXIT_FAILURE);
			}

		} else {
			//TODO: implement rle compression
			printf("Not implemented rle comp: %s\n",__func__);
		}

		if (fwrite((uint8_t*)dev_area_ref, 1, sizeof(dev_area_ref), fp)
				< (sizeof(dev_area_ref)))
		{
			fprintf(stderr, "Unable to write dev_area_ref to file");
			exit(EXIT_FAILURE);
		}

		if (fwrite((uint8_t*)ext_area_ref, 1, sizeof(ext_area_ref), fp)
				< (sizeof(ext_area_ref)))
		{
			fprintf(stderr, "Unable to write ext_area_ref to file");
			exit(EXIT_FAILURE);
		}

		if (fwrite((uint8_t*)footer, 1, sizeof(footer), fp)
				< (sizeof(footer)))
		{
			fprintf(stderr, "Unable to write ext_area_ref to file");
			exit(EXIT_FAILURE);
		}
		fclose(fp);
	} else {
		fprintf(stderr, "Unable to open file");
		exit(EXIT_FAILURE);
	}
}

void setTGAImagePixel(int32_t x, int32_t y, tga_image * image, tga_color color)
{
	if (x < 0 || x >= image->width || y < 0 || y >= image->height) {
		return;
	}

	memcpy((image->data + ((x+y*image->width)*image->bytesPerPixel)),
			color.raw,
			image->bytesPerPixel);
}

tga_color getTGAImagePixel(tga_image * image, int32_t x, int32_t y)
{
	tga_color color = {{0}};
	if (x < image->width || y < image->height || y >= 0 || x >= 0) {
		for (int32_t i = 0; i < image->bytesPerPixel; i++) {
			color.raw[i] = image->data[(x + y * image->width)*image->bytesPerPixel+i];
		}
	}
	return color;
}

void makeTGAImage(tga_image * image, int32_t width, int32_t height, uint8_t format)
{
	Assert(format == RGB || format == RGBA || format == GRAYSCALE);
	image->width = width;
	image->height = height;
	image->bytesPerPixel = format;
	image->size = width * height * format;
	image->pitch = image->width * image->bytesPerPixel;
	image->data = calloc(image->size, sizeof(*image->data));
	Assert(image->data);
}

#if 0
void flipTGAImageHorizontally(tga_image * image)
{
	Assert(image->data);
	Assert(image->data);

	for (int32_t i = 0; i < image->height; i++) {
		uint8_t *left = (image->data + (image->pitch*i));
		uint8_t *right = (image->data +
				((image->pitch*i) +
				 (image->pitch - image->bytesPerPixel)));

		while (left < right) {
			uint32_t tmp = *(uint32_t *)right;
			*right = *(uint32_t *)left;
			*left = tmp;
			left += image->bytesPerPixel;
			right -= image->bytesPerPixel;
		}
	}
}
#else
void flipTGAImageHorizontally(tga_image * image)
{
	Assert(image->data);

	for (int32_t i = 0; i < image->height; i++) {
		uint8_t *left = (image->data + (image->pitch*i));
		uint8_t *right = (image->data +
				((image->pitch*i) + (image->pitch-1)));
		while (left < right) {
			uint8_t tmp = *left;
			*left = *right;
			*right = tmp;
			left++;
			right--;
		}
	}
}
#endif

void flipTGAImageVerticaly(tga_image * image)
{
	Assert(image->data);

	uint8_t * tmp = malloc(image->pitch * (sizeof(*tmp)));

	for (int32_t i = 0; i < image->height >> 1; i++) {
		if (tmp) {
			uintptr_t yMax = image->pitch * i;
			uintptr_t yMin = (image->height-i-1)*image->pitch;
			memmove(tmp, image->data+yMax, image->pitch);
			memmove(image->data+yMax, image->data+yMin, image->pitch);
			memmove(image->data+yMin, tmp, image->pitch);
		} else {
			fprintf(stderr, "Out of memory (%s: line %d)\n",
					__FILE__,__LINE__);
			exit(EXIT_FAILURE);
		}
	}
	free(tmp);
}

void decompress_RLE(FILE * fp, tga_image * image)
{
	uint32_t pixelCount = image->width * image->height;
	uint32_t currentPixel = 0;
	uint32_t currentByte = 0;
	//TODO: Enten tga_color..
	tga_color colorBuffer;
	uint32_t error = 0;

	do {
		uint8_t chunkHeader = 0;
		if (fread(&chunkHeader, sizeof(uint8_t), 1, fp) != 1) {
			fprintf(stderr, "Error decompressing tga file\n");
			exit(EXIT_FAILURE);
		}
		if (chunkHeader < 128) {
			chunkHeader++;
			for (uint32_t i = 0; i < chunkHeader; i++)
			{
				if ((error = fread(colorBuffer.raw, sizeof(uint8_t), image->bytesPerPixel, fp))
						< sizeof(uint8_t) * image->bytesPerPixel)
				{
					fclose(fp);
					fprintf(stderr, "Error decompressing tga file1 %u\n",error);
					exit(EXIT_FAILURE);

				}

				for (int32_t b = 0; b < image->bytesPerPixel; b++) {
					image->data[currentByte++] = colorBuffer.raw[b];
				}

				++currentPixel;
				if (currentPixel > pixelCount) {
					fclose(fp);
					fprintf(stderr, "Error currentPixel > pixelCount\n");
					exit(EXIT_FAILURE);
				}
			}

		} else {
			chunkHeader -= 127;
			if ((error = fread(colorBuffer.raw, sizeof(uint8_t), image->bytesPerPixel, fp))
					< sizeof(uint8_t) * image->bytesPerPixel)
			{
				fclose(fp);
				fprintf(stderr, "Error decompressing tga file: %u\n",
						error);
				exit(EXIT_FAILURE);

			}

			for (uint32_t i = 0; i < chunkHeader; i++) {
				for (int32_t b = 0; b < image->bytesPerPixel; b++) {
					image->data[currentByte++] = colorBuffer.raw[b];
				}

				++currentPixel;
				if (currentPixel > pixelCount) {
					fclose(fp);
					fprintf(stderr, "Error currentPixel > pixelCount\n");
					exit(EXIT_FAILURE);
				}
			}
		}
	} while(currentPixel < pixelCount);
}
