#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <mpi.h>
#include <omp.h>
#include "matrix.h"

#define MAX_MSG 1024
#define PARENT 0
#define TAG 0




/*
 * ===  FUNCTION  =============================================================
 *         Name:  main
 *  Description:
 * ============================================================================
 */
int main(int argc, char **argv)
{
	char *out_file_name = "resultMatrix.bin";
	double **matrixA;
	int my_rowsA = 0;
	int rowsA;
	int colsA;

	double **matrixB;
	int my_rowsB = 0;
	int rowsB;
	int colsB;

	double **result = NULL;
	int my_rowsC = 0;
	int rowsC;
	int colsC;

	double *columnB = NULL;

	char fileA[FILENAME_MAX];
	char fileB[FILENAME_MAX];

	int id;
	int num_proc;

	double tic = 0.0f;
	double toc = 0.0f;

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	/*---------------------------------------------------------------------
	 *  PARENT Tar imot filnavn og leser inn matrisene fra bruker:
	 *-------------------------------------------------------------------*/
	if (id == PARENT) {
		printf("AxB:\n");
		printf("Provide file name A-matrix: ");
		fflush(stdout);
		if (scanf("%s", fileA) != 1) {
			fprintf(stderr, "Failed reading input\n");
		}
		printf("Provide file name B-matrix: ");
		fflush(stdout);
		if (scanf("%s", fileB) != 1) {
			fprintf(stderr, "Failed reading input\n");
		}

		/*-------------------------------------------------------------
		 *  Les og alloker samtlige matriser:
		 *-----------------------------------------------------------*/
		read_matrix_binaryformat(fileA, &matrixA, &rowsA, &colsA);
		read_matrix_binaryformat(fileB, &matrixB, &rowsB, &colsB);
		result = allocate_matrix2d(rowsA, colsB);
		rowsC = rowsA;
		colsC = colsB;

		// Lokal "glidende"-kolonne
		columnB = malloc(rowsB * sizeof(double));
		if (!columnB) {
			fprintf(stderr, "Memory allocation failed!\n");
			exit(EXIT_SUCCESS);
		}

		my_rowsA = rowsA / num_proc;
		my_rowsB = rowsB / num_proc;
		my_rowsC = rowsC / num_proc;
	}

#if PROFILE // Tidtagning startes vha f.eks make-fil
	if (id == PARENT) {
		tic = MPI_Wtime();
	}
#endif

	/*---------------------------------------------------------------------
	 *  Broadcast matrisestorrelser til samtlige prosesser:
	 *-------------------------------------------------------------------*/
	MPI_Bcast(&rowsB, 1, MPI_INT, PARENT, MPI_COMM_WORLD);
	MPI_Bcast(&colsB, 1, MPI_INT, PARENT, MPI_COMM_WORLD);
	MPI_Bcast(&rowsA, 1, MPI_INT, PARENT, MPI_COMM_WORLD);
	MPI_Bcast(&colsA, 1, MPI_INT, PARENT, MPI_COMM_WORLD);
	MPI_Bcast(&rowsC, 1, MPI_INT, PARENT, MPI_COMM_WORLD);
	MPI_Bcast(&colsC, 1, MPI_INT, PARENT, MPI_COMM_WORLD);

	/*---------------------------------------------------------------------
	 * Alloker plass til lokale matriser hos hver prosess:
	 *-------------------------------------------------------------------*/
	if (id != PARENT) {

		// Stoerrelsen kan variere hos hver prosess:
		my_rowsA = rowsA / num_proc;
		my_rowsB = rowsB / num_proc;
		my_rowsC = rowsC / num_proc;

		if (id==num_proc-1) {
			my_rowsA = rowsA - id * my_rowsA;
			my_rowsB = rowsB - id * my_rowsB;
			my_rowsC = rowsC - id * my_rowsC;
		}

		// Alloker ihht prosessenes stoerrelser:
		matrixA = allocate_matrix2d(my_rowsA, colsA);
		matrixB = allocate_matrix2d(my_rowsB, colsB);
		result = allocate_matrix2d(my_rowsC, colsC);

		// Lokal "glidende"-kolonne:
		columnB = malloc(rowsB * sizeof(double));
		if (!columnB) {
			fprintf(stderr, "Memory allocation failed!\n");
			exit(EXIT_SUCCESS);
		}
	}


	/*---------------------------------------------------------------------
	 *  Prosess 0 (PARENT) fordeler inneholdet i matrixA til subprosessene.
	 *-------------------------------------------------------------------*/
	if (id == PARENT) {
		for (int proc_id = 1; proc_id < num_proc; proc_id++) {
			//Send rader fra A
			int start_index_to_transportA = proc_id * (rowsA/num_proc);

			int rows_to_transportA = (proc_id==num_proc-1)
				? rowsA-(start_index_to_transportA)
				: (rowsA/num_proc);

			MPI_Send(matrixA[start_index_to_transportA],
					rows_to_transportA * colsA,
					MPI_DOUBLE,
					proc_id,
					TAG,
					MPI_COMM_WORLD);

			//Send rader fra B
			int start_index_to_transportB = proc_id * (rowsB/num_proc);

			int rows_to_transportB = (proc_id==num_proc-1)
				? rowsB-(start_index_to_transportB)
				: (rowsB/num_proc);

			MPI_Send(matrixB[start_index_to_transportB],
					rows_to_transportB * colsB,
					MPI_DOUBLE,
					proc_id,
					TAG,
					MPI_COMM_WORLD);
		}
	} else {
		/*-------------------------------------------------------------
		 *  subprosessene mottar data inn i eget allokert minne
		 *-----------------------------------------------------------*/
		// Motta i A-Matrisen
		MPI_Recv(matrixA[0], my_rowsA * colsA, MPI_DOUBLE, PARENT, TAG,
				MPI_COMM_WORLD, &status);
		// Motta i B-Matrisen
		MPI_Recv(matrixB[0], my_rowsB * colsB, MPI_DOUBLE, PARENT, TAG,
				MPI_COMM_WORLD, &status);
	}



	/*---------------------------------------------------------------------
	 * Alle Matrise-multipliserer:
	 *-------------------------------------------------------------------*/
	int startIndexToSend = id * (rowsB / num_proc);
	int segmentToSend = id == (num_proc-1)
		? (rowsB-startIndexToSend)
		: (rowsB/num_proc);

	for (int row = 0; row < my_rowsA; row++) {
		for (int col = 0; col < colsC; col++) {
			// Populer egen kolonne med verdier fra matriseB
			for (int i = 0; i < my_rowsB; i++) {
				columnB[startIndexToSend + i] = matrixB[i][col];
			}
			// Fordel egene og motta andres lokale verdier:
			for (int tID = 0; tID < num_proc; tID++) {
				if (tID != id) {
					int startIndexToRecv = tID * (rowsB / num_proc);
					int segmentToRecv = (tID != (num_proc-1) ?
							(rowsB / num_proc)
							: (rowsB - tID * (rowsB / num_proc)));

					MPI_Sendrecv(&columnB[startIndexToSend],
							segmentToSend,
							MPI_DOUBLE,
							tID,
							TAG,
							&columnB[startIndexToRecv],
							segmentToRecv,
							MPI_DOUBLE,
							tID,
							TAG,
							MPI_COMM_WORLD,
							&status);
				}
			}
			// Kalkulerer resultat:
			for (int k = 0; k < rowsB; k++) {
				result[row][col] += matrixA[row][k] * columnB[k];
			}
		}
	}
#if 0
	if ((id != num_proc -1)) {
		for (int i = 0; i < rowsA % num_proc; i++) {
			for (int j = 0; j < colsB; j++) {
				MPI_Sendrecv(&columnB[startIndex],
						segment,
						MPI_DOUBLE,
						num_proc-1,
						TAG,
						&columnB[(num_proc-1) * (rowsB / num_proc)],
						(rowsB - (num_proc-1) * (rowsB / num_proc)),
						MPI_DOUBLE,
						num_proc - 1,
						TAG,
						MPI_COMM_WORLD,
						&status);
			}
		}
	}
#endif

	/*---------------------------------------------------------------------
	 *  Alle sender sine resultater til Parent-prosessen
	 *-------------------------------------------------------------------*/
	if (id != PARENT) {
		MPI_Send(*result, my_rowsA * colsB, MPI_DOUBLE, PARENT,
				TAG,
				MPI_COMM_WORLD);
	} else {
	/*---------------------------------------------------------------------
	 *  PARENT tar imot resultat og skriver det til fil.
	 *-------------------------------------------------------------------*/
		for (int proc_id = 1; proc_id < num_proc; proc_id++) {
			int start_index_to_transport = proc_id * (rowsA / num_proc);

			int rows_to_transport = (proc_id == num_proc-1)
				? rowsA - (start_index_to_transport)
				: (rowsA / num_proc);

			MPI_Recv(result[start_index_to_transport],
					rows_to_transport * colsB,
					MPI_DOUBLE,
					proc_id,
					TAG,
					MPI_COMM_WORLD,
					&status);
		}
#if PROFILE
		toc = MPI_Wtime();
		printf("Totaltid: %lf\n", toc-tic);
#endif
		write_matrix_binaryformat(out_file_name, result, rowsA, colsB);
	}

	MPI_Finalize();
	return EXIT_SUCCESS;
}


