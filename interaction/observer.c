#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdint.h>

#include "bench.h"

// int WINDOW_SIZE = 8192;
#define ITERATIONS 512

void print_metric(uint64_t metric, double t, int size, uint64_t total_metric)
{
	double metric_per_time = (double)metric / t;
	double total_metric_per_time = (double)total_metric / t;

	// header is op,uses_stride,compiler,size,metric,time,single_metric/t,total_metric/t

	printf("___");
#ifdef OP_GET
	printf("get,");
	metric_per_time = (double)(WINDOW_SIZE * sizeof(int)) * metric_per_time;
	total_metric_per_time = (double)(WINDOW_SIZE * sizeof(int)) * total_metric_per_time;
#endif
#ifdef OP_GET_ACC
	printf("get_acc,");
	metric_per_time = (double)(WINDOW_SIZE * sizeof(int)) * metric_per_time;
	total_metric_per_time = (double)(WINDOW_SIZE * sizeof(int)) * total_metric_per_time;
#endif
#ifdef OP_FOP
	printf("fop,");
#endif
#ifdef OP_CAS
	printf("cas,");
#endif
#ifdef USE_STRIDE
	printf("true,");
#endif
#ifndef USE_STRIDE
	printf("false,");
#endif

	printf("%s,", COMPILER);

	printf("%d,%lu,%f,%f,%f\n", num_observers, metric, t, metric_per_time,total_metric_per_time);
}

// int *window_ptr;
// MPI_Win win;

void run_observer(int rank, int size)
{

	MPI_Win_allocate(sizeof(int) * WINDOW_SIZE, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window_ptr, &win);

	int *recv = malloc(sizeof(int) * WINDOW_SIZE);
#ifdef OP_FOP
	int src = 0;
	int target = 0;
#endif
#ifdef OP_CAS
	int src = 2;
	int compare_buf = 2;
#endif
#ifdef OP_GET_ACC
	int *zeroes = malloc(sizeof(int) * WINDOW_SIZE);
	for (int i = 0; i < WINDOW_SIZE; i++)
	{
		zeroes[i] = 0;
	}

#endif

#ifdef USE_STRIDE
	MPI_Datatype stride_type;

	MPI_Type_vector(
		WINDOW_SIZE / 2, // #blocks
		1,				 // #elements/block
		2,				 // stride
		MPI_INT,
		&stride_type);

	MPI_Type_commit(&stride_type);
#endif

	MPI_Barrier(MPI_COMM_WORLD);

	uint64_t metric = 0;
	double t_start = MPI_Wtime();

	MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);

	MPI_Barrier(MPI_COMM_WORLD);

	while (1)
	{
#ifdef OP_GET
		for (int i = 0; i < ITERATIONS; i++)
		{
#ifndef USE_STRIDE
			MPI_Get(recv, WINDOW_SIZE, MPI_INT, 0, 0, WINDOW_SIZE, MPI_INT, win);
#endif
#ifdef USE_STRIDE
			MPI_Get(recv, 1, stride_type, 0, 0, WINDOW_SIZE / 2, MPI_INT, win);
#endif
		}

#endif
#ifdef OP_FOP
		MPI_Fetch_and_op(&src, recv, MPI_INT, 0, 0, MPI_SUM, win);
#endif
#ifdef OP_CAS
		MPI_Compare_and_swap(&src, &compare_buf, recv, MPI_INT, 0, 0, win);
#endif
#ifdef OP_GET_ACC
		for (int i = 0; i < ITERATIONS; i++)
		{
#ifndef USE_STRIDE
			MPI_Get_accumulate(
				zeroes, WINDOW_SIZE, MPI_INT,		 // from
				recv, WINDOW_SIZE, MPI_INT,			 // to
				0, 0, WINDOW_SIZE, MPI_INT, MPI_SUM, // origin
				win);
#endif
#ifdef USE_STRIDE
			MPI_Get_accumulate(
				zeroes, 1, stride_type,					 // from
				recv, 1, stride_type,					 // to
				0, 0, WINDOW_SIZE / 2, MPI_INT, MPI_SUM, // origin
				win);
#endif
		}
#endif

#if defined OP_FOP || defined OP_CAS || defined OP_GET_ACC
		MPI_Win_flush(0, win);
#endif
#ifdef OP_GET
		MPI_Win_flush_local(0, win);
#endif

		metric++;

		if (recv[0] == 2)
		{
			break;
		}

		if (recv[0] != 1)
		{
			return;
			printf("Validation of MPI Buffer failed, %d\n", recv[0]);
		}
	}
	MPI_Win_unlock(0, win);

	double dur = MPI_Wtime() - t_start;

	MPI_Barrier(MPI_COMM_WORLD);


#if defined OP_GET || defined OP_GET_ACC
	metric *= ITERATIONS;
#endif

	uint64_t res = 0;

	MPI_Reduce(&metric, &res, 1, MPI_UINT64_T, MPI_SUM, size / 2, MPI_COMM_WORLD);

	if (rank == (size / 2))
	{
		print_metric(metric, dur, size, res);
	}
}