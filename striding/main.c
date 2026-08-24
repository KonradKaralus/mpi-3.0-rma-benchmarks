#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdint.h>

#include <SCOREP_User.h>
#include <SCOREP_User_Types.h>
SCOREP_USER_METRIC_LOCAL(my_local_metric)

#define WINDOW_SIZE 8192 // 8192 elements of uint32
#define ITERATIONS 10000

int ntimes = 1;

MPI_Win win;
uint32_t *window_ptr;

int main(int argc, char *argv[])
{
    SCOREP_USER_METRIC_INIT(my_local_metric, "element_size", "byte",
                            SCOREP_USER_METRIC_TYPE_INT64,
                            SCOREP_USER_METRIC_CONTEXT_GLOBAL)

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc > 1)
    {
        ntimes = atoi(argv[1]);
    }

    uint32_t rcvbuf[WINDOW_SIZE];

    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        rcvbuf[i] = 1;
    }

    if (rank == 0)
    {
        MPI_Win_allocate(
            WINDOW_SIZE * sizeof(uint32_t),
            sizeof(uint32_t),
            MPI_INFO_NULL,
            MPI_COMM_WORLD,
            &window_ptr,
            &win);

        for (int i = 0; i < WINDOW_SIZE; i++)
        {
            window_ptr[i] = 1;
        }

        printf("compiler,block_bytes,time\n");
    }
    else
    {
        MPI_Win_allocate(
            0,
            sizeof(uint32_t),
            MPI_INFO_NULL,
            MPI_COMM_WORLD,
            &window_ptr,
            &win);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);

    for (int i = 0; i < ntimes; i++)
    {
        if (rank != 0)
        {
            for (int i = 1; i < 8; i++)
            {

                MPI_Datatype type = MPI_UINT32_T;
                int size = sizeof(uint32_t);

                int block_size = i;
                int num_blocks = WINDOW_SIZE / (block_size * 2); // 4096

                int block_bytes = block_size * sizeof(uint32_t); // 4

                MPI_Datatype stride_type;
                MPI_Type_vector(
                    num_blocks,     // #blocks
                    block_size,     // #elements/block
                    block_size * 2, // stride
                    type,
                    &stride_type);

                MPI_Type_commit(&stride_type);

                SCOREP_USER_METRIC_INT64(my_local_metric, block_bytes)

                double start = MPI_Wtime();
                for (int j = 0; j < ITERATIONS; j++)
                {
                    MPI_Get(rcvbuf, 1, stride_type, 0, 0, block_size * num_blocks, type, win);
                }
                MPI_Win_flush(0, win);
                double elapsed = MPI_Wtime() - start;
                printf("%s,%d,%f\n", COMPILER, block_bytes, elapsed);
                SCOREP_USER_METRIC_INT64(my_local_metric, block_bytes)
            }
        }
    }

    MPI_Win_unlock(0, win);

    MPI_Win_free(&win);

    MPI_Finalize();
    return 0;
}