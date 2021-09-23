/*
 * ============================================================================
 *
 *       Filename:  serial_main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/15/2016 10:24:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Christopher Storheim
 *
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "../oblig1.h"


/*
 * ===  FUNCTION  =============================================================
 *         Name:  main
 * ============================================================================
 */
int main(int argc, char **argv)
{
	if (argc != 5)
		print_usage(argv[0]);

	unsigned char *image_bytes;
	int m, n;
	int c;
	image u, u_bar;

	/* Les fra kommandolinjen */
	unsigned int iters = strtoul(argv[1], NULL, 0);
	float kappa = atof(argv[2]);
	char *input_jpeg_filename = argv[3];
	char *output_jpeg_filename = argv[4];

	/* Les inn og alloker bildet */
	import_JPEG_file(input_jpeg_filename, &image_bytes, &m, &n, &c);

	if (c == BW_IMG) {
		allocate_image(&u, m, n);
		allocate_image(&u_bar, m, n);
		convert_jpeg_to_image(image_bytes, &u);
		iso_diffusion_denoising(&u, &u_bar, kappa, iters);
		convert_image_to_jpeg(&u, image_bytes);
	} else if (c == COLOR_IMG) {
		puts("Color image found");
	}
	export_JPEG_file(output_jpeg_filename, image_bytes,m, n, c, QUALITY);

	/* Free allokerte data */
	deallocate_image(&u);
	deallocate_image(&u_bar);
	free(image_bytes);

	return EXIT_SUCCESS;
}


/*
 * ===  FUNCTION  =============================================================
 *         Name:  iso_diffusion_denoising
 *  Description:  Foerst settes initial-rand-verdier i u_bar, saa kalkuleres
 *  ny pikselverdi for samtlige punkter i image_data. Gjentas iter ganger.
 * ============================================================================
 */
void iso_diffusion_denoising(image *u, image *u_bar, float kappa, int iters)
{
	int i;
	int j;
	int row;
	int column;
	float **U;
	float **U_BAR;
	float **tmp;

	row = u->m-1;
	column = u->n-1;
	U = u->image_data;
	U_BAR = u_bar->image_data;

	for (i = 0; i <= row; i++) {
		U_BAR[i][0] = U[i][0];
		U_BAR[i][column] = U[i][column];
	}
	for (i = 0; i <= column; i++) {
		U_BAR[0][i] = U[0][i];
		U_BAR[row][i] = U[row][i];
	}

	while (iters--) {
		for (i = 1; i < row; i++) {
			for (j = 1; j < column; j++) {
				U_BAR[i][j] = U[i][j]
					+ kappa * (U[i-1][j]+U[i][j-1]
							- 4*U[i][j]
							+ U[i][j+1]
							+ U[i+1][j]);
			}
		}
		tmp = U_BAR;
		U_BAR = U;
		U = tmp;
	}
}



/*
 * ===  FUNCTION  =============================================================
 *         Name:  allocate_image
 *  Description:  Allokerer et sammenhengende m*n 1-dim array for saa
 *  aa allokere et peker array som peker til hver n-te plass i det.
 * ============================================================================
 */
void allocate_image(image *u, int m, int n)
{
	int i;
	float *data;
	float **matrix;

	data = malloc((m*n)*sizeof(*data));
	if (data==NULL) {
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	matrix = malloc(m*sizeof(*matrix));
	if (matrix==NULL) {
		free(data);
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < m; i++)
		matrix[i] = &data[n*i];

	u->image_data = matrix;
	u->m = m;
	u->n = n;
}


/*
 * ===  FUNCTION  =============================================================
 *         Name:  convert_jpeg_to_image
 *  Description: Populerer 2d-matrisen image strukten peker paa med dataen
 *  image_chars peker paa, mottatt fra kall paa import _JPEG_file().
 * ============================================================================
 */
void convert_jpeg_to_image(const unsigned char *image_chars, image *u)
{
	int i;
	int j;
	int outer = u->m;
        int inner = u->n;
	int c = 0;

	for (i = 0; i < outer; i++)
		for (j = 0; j < inner; j++)
			u->image_data[i][j] = image_chars[c++];
}



/*
 * ===  FUNCTION  =============================================================
 *         Name:  convert_image_to_jpeg
 *  Description:  Konverterer den 2-dim matrisen tilbake til minnet som
 *  image_chars peker paa.
 * ============================================================================
 */
void convert_image_to_jpeg(const image *u, unsigned char *image_chars)
{
	int i;
	int j;
	int outer = u->m;
	int inner = u->n;
	int c = 0;

	for (i = 0; i < outer; i++)
		for (j = 0; j < inner; j++)
			image_chars[c++] = u->image_data[i][j];
}



/*
 * ===  FUNCTION  =============================================================
 *         Name:  deallocate_image
 *  Description:  Deallokerer 2-dim array.
 * ============================================================================
 */
void deallocate_image(image *u)
{
	free(u->image_data[0]);
	free(u->image_data);
}


/*
 * ===  FUNCTION  =============================================================
 *         Name:  print_usage
 *  Description:  Skriver info om bruk til skjerm.
 * ============================================================================
 */
void print_usage(const char* name) {
	printf("%s <iters> <kappa> <jpeg-in-name> <jpeg-out-name>\n", name);
	exit(EXIT_FAILURE);
}
