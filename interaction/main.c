#include <mpi.h>
#include "bench.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int WINDOW_SIZE = 8192;

int *window_ptr;
MPI_Win win;
int num_observers = 1;

#ifdef INTEL_PROG
int prog_every_n = 64;
#endif

// if the rank doesnt have to do anything, it still has to call all barriers etc.
void dummy_run(int size)
{
    MPI_Win_allocate(0, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window_ptr, &win);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    uint64_t res = 0;
    uint64_t zero = 0;
    MPI_Reduce(&zero, &res, 1, MPI_UINT64_T, MPI_SUM, size / 2, MPI_COMM_WORLD);
}

#ifdef INTEL_PROG
// if the rank doesnt have to do anything, but intel (no progress)
void dummy_run_intel(int size)
{
    MPI_Win_allocate(0, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window_ptr, &win);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    int sleep_time = 100 / prog_every_n;
    if (sleep_time < 10)
    {
        sleep_time = 10;
    }
    sleep(sleep_time); // this is a guess. if results show that a stream ran longer than this, this needs to be increased
    MPI_Barrier(MPI_COMM_WORLD);
    uint64_t res = 0;
    uint64_t zero = 0;
    MPI_Reduce(&zero, &res, 1, MPI_UINT64_T, MPI_SUM, size / 2, MPI_COMM_WORLD);
}
#endif

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int num_main = 1;
    if (argc > 2)
    {
        num_main = atoi(argv[1]);
        num_observers = atoi(argv[2]);

#ifdef INTEL_PROG
        prog_every_n = atoi(argv[3]);
#endif
    }

    if (size < 2)
    {
        MPI_Finalize();
        printf("MPI size < 2. Size: %d\n", size);
        return 1;
    }

    if (rank < num_main && rank < size / 2)
    {

#ifdef MAIN_STREAM
        run_stream(rank, size);
#endif
#ifdef MAIN_DGEMM
        run_dgemm(rank, size);
#endif
        uint64_t res = 0;
        uint64_t zero = 0;
        MPI_Reduce(&zero, &res, 1, MPI_UINT64_T, MPI_SUM, size / 2, MPI_COMM_WORLD);
    }
    else if ((rank >= size / 2) && ((rank - size / 2) < num_observers))
    {

        run_observer(rank, size);
    }
    else
    {
#ifdef INTEL_PROG
        dummy_run_intel(size);
#else
        dummy_run(size);
#endif
    }

    MPI_Win_free(&win);
    MPI_Finalize();

    return 0;
}
