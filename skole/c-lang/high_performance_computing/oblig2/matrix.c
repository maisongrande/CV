#include "matrix.h"

/*
 * ===  FUNCTION  =============================================================
 *         Name:  print_matrix
 *  Description:  Skriver ut matrisen.
 * ============================================================================
 */
void
print_matrix(double **mat, int rows, int cols)
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%.3lf ", mat[i][j]);
		}
		puts("");
	}
}

/*
 * ===  FUNCTION  =============================================================
 *         Name:  transpose
 *  Description:  Transponerer matrisen mat.
 * ============================================================================
 */
void
transpose(double ***mat, int *nRows, int *nCols)
{
	int rows = *nRows;
	int cols = *nCols;
	if (cols == rows) {
#pragma omp parallel for schedule(static)
		for (int i = 1; i < rows; i++)
			for (int j = 0; j < i; j++) {
				double tmp = (*mat)[i][j];
				(*mat)[i][j] =  (*mat)[j][i];
				(*mat)[j][i] = tmp;

			}
	} else {

		double **new_matrix = allocate_matrix2d(cols, rows);

#pragma omp parallel for schedule(static)
		for (int i = 0; i < cols; i++) {
			for (int j = 0; j < rows; j++) {
				new_matrix[i][j]= (*mat)[j][i];
			}
		}

		free(mat[0]);
		*mat = new_matrix;
		*nRows = cols;
		*nCols = rows;
	}
}

/*
 * ===  FUNCTION  =============================================================
 *         Name:  read_matrix_binaryformat
 *  Description:  Leser inn matrise-data fra fil og populerer allokert matrise
 *  med disse.
 * ============================================================================
 */
void
read_matrix_binaryformat(char *filename, double ***matrix,
		int *rows, int *cols)
{
	FILE *file_ptr = fopen(filename, "rb");

	if (!file_ptr) {
		fprintf(stderr,"couldn't open file '%s'; %s\n",
				filename,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	int read_sum = fread(rows, sizeof(int), 1, file_ptr);
	read_sum += fread(cols, sizeof(int), 1, file_ptr);
	if (read_sum < 2) {
		fprintf(stderr, "fread cols from %s failed", filename);
		exit(EXIT_FAILURE);
	}

	*matrix = malloc((*rows)*sizeof(double*));
	if (matrix == NULL) {
		fprintf(stderr, "Matrix->data allocation failed\n");
		exit(EXIT_FAILURE);
	}

	(*matrix)[0] = malloc(((*rows)*(*cols)) * sizeof(double));
	if (*matrix[0] == NULL) {
		fprintf(stderr, "Data allocation failed\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < *rows; i++) {
		(*matrix)[i] = (*matrix)[i-1]+(*cols);
	}

	read_sum += fread((*matrix)[0],
			sizeof(double),
			(*rows)*(*cols),
			file_ptr);
	if (read_sum < 2) {
		fprintf(stderr, "fread data from %s failed", filename);
		exit(EXIT_FAILURE);
	}

	if (fclose(file_ptr) == EOF) {
		fprintf (stderr,"couldn't close file '%s'; %s\n",
				filename,
				strerror(errno));
		exit(EXIT_FAILURE);
	}
}


/*
 * ===  FUNCTION  =============================================================
 *         Name:  write_matrix_binaryformat
 *  Description:  Skriver inneholdet av matrise matrix inn i fil "filnavn"
 *  paa binaer form.
 * ============================================================================
 */
void
write_matrix_binaryformat(char *filename,  double **matrix, int rows, int cols)
{
	FILE *file_ptr = fopen(filename, "wb");
	if (file_ptr == NULL) {
		fprintf(stderr,"couldn't open file '%s'; %s\n",
				filename,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	uint32_t res = fwrite(&rows, sizeof(int), 1, file_ptr);
	res += fwrite(&cols, sizeof(int), 1, file_ptr);
	res += fwrite(matrix[0], sizeof(double), rows * cols, file_ptr);

	if (res <= 2) {
		fprintf(stderr, "fread data from %s failed", filename);
		exit(EXIT_FAILURE);
	}

	if (fclose(file_ptr) == EOF) {
		fprintf (stderr,"couldn't close file '%s'; %s\n",
				filename,
				strerror(errno));
		exit(EXIT_FAILURE);
	}
}


/*
 * ===  FUNCTION  =============================================================
 *         Name:  allocate_matrix2d
 *  Description:  Returnerer en allokert matrise av oensket stoerrelse
 * ============================================================================
 */
double **
allocate_matrix2d(int rows, int cols)
{
	double **matrix = malloc(rows * sizeof(*matrix));
	if (!matrix) {
		fprintf(stderr,"Unable to allocate memory: %s\n", __FILE__);
		exit(EXIT_FAILURE);
	}
	double *data = calloc(rows * cols, sizeof(*data));
	if (!data) {
		fprintf(stderr,"Unable to allocate memory: %s\n", __FILE__);
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < rows; i++)
		matrix[i] = &data[i * cols];

	return matrix;
}


/*
 * ===  FUNCTION  =============================================================
 *         Name:  matrix_multiply
 *  Description:  Multipliserer matrise A med matrise B og legger resultatet
 *  i matrise "Result".
 *  (1) Transponer matrisen for bedre cache-lokalitet. For egen kuriositet.
 *  1) Fordel ytre loekke paa antallet traader prosessen har.
 *  2) Gjennomfoer sum av indre produkt og legg i resultat-matrisen.
 * ============================================================================
 */
void
matrix_multiply(double **matrixA, double **matrixB,
		int rowsA, int colsA, int rowsB, int colsB,
		double **result)
{
#if TRANSPOSE
	/*---------------------------------------------------------------------
	 *  Transponerer matrisen for mulig mer cache-vennlighet
	 *-------------------------------------------------------------------*/
	transpose(&matrixB, &rowsB, &colsB);
#pragma omp parallel for schedule(static)
	for (int r = 0; r < rowsA; r++) {
		for (int c = 0; c < rowsB; c++) {
			result[r][c] = 0.0f;
			for (int i = 0; i < colsA; i++) {
				result[r][c] += matrixA[r][i]*matrixB[c][i];
			}
		}
	}
#else
#pragma omp for schedule(static)
	for (int r = 0; r < rowsA; r++) {
		for (int c = 0; c < colsB; c++) {
			result[r][c] = 0.0f;
			for (int i = 0; i < colsA; i++) {
				result[r][c] += matrixA[r][i]*matrixB[i][c];
			}
		}
	}
#endif
}
