#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <cblas.h>

#include "bench.h"

long MATRIX_SIZE = 512;
int MATRIX_ITERATIONS = 400;

void run_dgemm(int rank, int size)
{

	MPI_Win_allocate(sizeof(int) * WINDOW_SIZE, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window_ptr, &win);

	MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
	window_ptr[0] = 1;
	MPI_Win_sync(win);
	MPI_Win_unlock(0, win);

	MPI_Barrier(MPI_COMM_WORLD);

	double *A = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
	double *B = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
	double *C = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));

	for (int i = 0; i < (MATRIX_SIZE * MATRIX_SIZE); i++)
	{
		A[i] = (double)(i + 1);
	}

	for (int i = 0; i < (MATRIX_SIZE * MATRIX_SIZE); i++)
	{
		B[i] = (double)(-i - 1);
	}

	for (int i = 0; i < (MATRIX_SIZE * MATRIX_SIZE); i++)
	{
		C[i] = 0.0;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	double start = MPI_Wtime();
	for (int i = 0; i < MATRIX_ITERATIONS; i++)
	{
		cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans,
					MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE,
					1.0, A, MATRIX_SIZE,
					B, MATRIX_SIZE, 0.0,
					C, MATRIX_SIZE);
	}

	double elapsed = MPI_Wtime() - start;

	int two = 2;

	MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);

	MPI_Accumulate(&two, 1, MPI_INT, 0, 0, 1, MPI_INT, MPI_REPLACE, win);

	MPI_Win_unlock(0, win);

	MPI_Barrier(MPI_COMM_WORLD);

	long ops = MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE*MATRIX_ITERATIONS;

	printf("___dgemm: dgemm,%d,%f,%f\n", rank, elapsed, (double)ops / elapsed);
}