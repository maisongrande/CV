/*
 * ============================================================================
 *
 *       Filename:  parallel_main.c
 *
 *    Description:  Parallellutgaven av denoising.
 *
 *        Created:  02/27/2016 01:22:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Christopher Storheim
 *
 * ============================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include "../oblig1.h"


/*
 * ===  FUNCTION  =============================================================
 *         Name:  main
 * ============================================================================
 */
int main(int argc, char **argv)
{
	int id;
	int seg_sz;
	int segment;

	image u;
	image u_bar;
	image whole_image;

	int rows;
	int cols;
	int iters;
	int comp;
	float kappa;

	int my_rows;
	int my_cols;
	int my_rank;
	int num_proc;
	MPI_Status status;

	unsigned char *image_chars;
	unsigned char *my_image_chars;
	char *input_jpeg_filename;
	char *output_jpeg_filename;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	iters = atoi(argv[1]);
	kappa = atof(argv[2]);
	input_jpeg_filename = argv[3];
	output_jpeg_filename = argv[4];


	if (my_rank == PARENT_PROC) {
		import_JPEG_file(input_jpeg_filename, &image_chars,
				&rows, &cols, &comp);
		allocate_image(&whole_image, rows, cols);
	}

	MPI_Bcast(&rows, 1, MPI_INT, PARENT_PROC, MPI_COMM_WORLD);
	MPI_Bcast(&cols, 1, MPI_INT, PARENT_PROC, MPI_COMM_WORLD);

	/* Hver prosess faar sine antall bytes av totalt antall bytes: */
	my_rows = my_rank == (num_proc-1) ?
		(rows-((num_proc - 1) * (rows / num_proc)))
		: (rows / num_proc);
	my_cols = cols;


	/* Alloker minne til my_rank sine bildesegmenter: */
	my_image_chars = malloc(((my_rows+2)*my_cols)*sizeof(*my_image_chars));
	if (my_image_chars == NULL) {
		fprintf(stderr,"Out of memory\n");
		exit(EXIT_FAILURE);
	}

	/* Fordel bildedata til de oevrige prosessene: */
	if (my_rank == PARENT_PROC) {
		segment = rows / num_proc;

		for (id = 1; id < num_proc; id++) {
			/* Sistemann ut tar resten */
			if (id == num_proc-1)
				seg_sz = rows - id * segment;
			else
				seg_sz = segment;

			MPI_Send(&image_chars[id*segment*my_cols],
					seg_sz*my_cols,
					MPI_UNSIGNED_CHAR,
					id,
					GEN_TAG,
					MPI_COMM_WORLD);
		}

		for (id = 0; id < my_rows*cols; id++) {
			my_image_chars[my_cols+id] = image_chars[id];
		}

	} else {
		/* Motta data fra prosess 0 (PARENT_PROC) */
		MPI_Recv(&my_image_chars[my_cols],
				my_rows * my_cols,
				MPI_UNSIGNED_CHAR,
				PARENT_PROC,
				GEN_TAG,
				MPI_COMM_WORLD,
				&status);
	}

	/* Alloker segment-bilder */
	allocate_image(&u,(my_rows+2), my_cols);
	allocate_image(&u_bar,(my_rows+2), my_cols);

	/* Populer segement-bildene med data */
	convert_jpeg_to_image(my_image_chars, &u);

	iso_diffusion_denoising_parallel(&u, &u_bar, kappa,
			iters, my_rank, num_proc);

	/*
	 * Reverser prosessen over og send resulatatet tilbake
	 * til PARENT_PROC som gjor om bildet til byte og skriver til fil.
	 * */
	if (my_rank != PARENT_PROC) {
		MPI_Send(u.image_data[1],
				my_rows * my_cols,
				MPI_FLOAT,
				PARENT_PROC,
				GEN_TAG,
				MPI_COMM_WORLD);

	} else {
		segment = rows / num_proc;
		memcpy(whole_image.image_data[0],
				u.image_data[1],
				(my_rows * my_cols) * sizeof(float));

		for (id = 1; id < num_proc; id++) {
			if (id == num_proc-1)
				seg_sz = rows - id * segment;
			else
				seg_sz = segment;

			MPI_Recv(&whole_image.image_data[0][id*segment*cols],
					seg_sz*cols,
					MPI_FLOAT,
					id,
					GEN_TAG,
					MPI_COMM_WORLD,
					&status);
		}

		convert_image_to_jpeg(&whole_image, image_chars);
		export_JPEG_file(output_jpeg_filename,
				image_chars,
				rows,
				cols,
				comp,
				QUALITY);

		deallocate_image(&whole_image);
		free(image_chars);
	}

	deallocate_image(&u);
	deallocate_image(&u_bar);

	free(my_image_chars);
	MPI_Finalize();

	return EXIT_SUCCESS;
}	/*-- end of function main --*/


/*
 * ===  FUNCTION  =============================================================
 *         Name:  iso_diffusion_denoising
 *  Description:  Foerst settes initial-rand-verdier i u_bar, saa kalkuleres
 *  ny pikselverdi for samtlige punkter i image_data. Gjentas iter ganger.
 * ============================================================================
 */
void iso_diffusion_denoising_parallel(image *u, image *u_bar,
		float kappa, int iters, int my_rank, int num_proc)
{
	int i;
	int j;
	int row;
	int column;
	float **U;
	float **U_BAR;
	float **tmp;
	MPI_Status status;

	row = u->m;
	column = u->n;
	U = u->image_data;
	U_BAR = u_bar->image_data;

	for (i = 0; i < row; i++) {
		U_BAR[i][0] = U[i][0];
		U_BAR[i][column-1] = U[i][column-1];
	}
	for (i = 0; i < column; i++) {
		U_BAR[0][i] = U[0][i];
		U_BAR[row-1][i] = U[row-1][i];
	}

	while (iters--) {
		if (my_rank != PARENT_PROC)
			MPI_Sendrecv(U[1], column, MPI_FLOAT, my_rank-1,
					GEN_TAG, U[0], column, MPI_FLOAT,
					my_rank-1, GEN_TAG, MPI_COMM_WORLD,
					&status);

		if (my_rank != num_proc-1)
			MPI_Sendrecv(U[row-2], column, MPI_FLOAT, my_rank+1,
					GEN_TAG, U[row-1], column, MPI_FLOAT,
					my_rank+1, GEN_TAG, MPI_COMM_WORLD,
					&status);

		for (i = 1; i < row-1; i++) {
			for (j = 1; j < column-1; j++) {
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
		fprintf(stderr,"Out of memory\n");
		exit(EXIT_FAILURE);
	}

	matrix = malloc(m*sizeof(*matrix));
	if (matrix==NULL) {
		free(data);
		fprintf(stderr,"Out of memory\n");
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
	printf("mpirun -np <ant traader> ");
	printf("%s <iters> <kappa> <jpeg-in-name> <jpeg-out-name>\n", name);
	exit(EXIT_FAILURE);
}
