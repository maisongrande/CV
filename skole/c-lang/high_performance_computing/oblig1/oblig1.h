/*
 * ============================================================================
 *
 *       Filename:  oblig1.h
 *
 *    Description:  Diverse tilpasset oblig 1
 *
 *        Version:  1.0
 *        Created:  02/15/2016 12:13:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Christopher Storheim (cns), christopherstorheim@gmail.com
 *   Organization:  N/A
 *
 * ============================================================================
 */
#ifndef OBLIG1_H
#define OBLIG1_H

#include <stdlib.h>

#define QUALITY 75
#define COLOR_IMG 3
#define BW_IMG 1
#define GEN_TAG 1349
#define PARENT_PROC 0

typedef struct {
	float **image_data;
	int m;
	int n;
} image;

void import_JPEG_file(const char *filename,
		unsigned char **image_chars,
		int *image_height,
		int *image_width,
		int *num_components);

void export_JPEG_file(const char *filename,
		unsigned char *image_chars,
		int image_height,
		int image_width,
		int num_components, int quality);

void allocate_image(image *u, int m, int n);

void deallocate_image(image *u);

void convert_jpeg_to_image(const unsigned char *image_chars, image *u);

void convert_image_to_jpeg(const image *u, unsigned char *image_chars);

void iso_diffusion_denoising(image *u, image *u_bar, float kappa, int iters);

void iso_diffusion_denoising_parallel(image *u, image *u_bar,
		float kappa, int iters, int my_rank, int num_proc);
void print_usage(const char* name);

#endif /*__OBLIG1_H__*/
