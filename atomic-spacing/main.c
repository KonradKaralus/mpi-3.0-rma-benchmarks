#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdint.h>

#ifdef USE_SCOREP
#include <SCOREP_User.h>
#include <SCOREP_User_Types.h>
SCOREP_USER_METRIC_LOCAL(my_local_metric)
#endif

#define BUFFER_SIZE 128 * 512 // = 128*4KiB | 8B uint64t * 512 = 4096
#define STRIDE_STEP 32
#define ITERATIONS 10000
#define WARMUP 2

int main(int argc, char *argv[])
{
#ifdef USE_SCOREP
    SCOREP_USER_METRIC_INIT(my_local_metric, "stride", "byte",
                            SCOREP_USER_METRIC_TYPE_INT64,
                            SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ntimes = 1;
    int num_observer = 1;

    MPI_Win *windows = malloc(size * sizeof(MPI_Win));
    MPI_Comm *pair_comms = malloc(size * sizeof(MPI_Comm));

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    if (argc > 2)
    {
        ntimes = atoi(argv[1]);
        num_observer = atoi(argv[2]);
    }

    uint64_t *window_ptr;
    if (rank == 0)
    {
        MPI_Alloc_mem(BUFFER_SIZE * sizeof(uint64_t), MPI_INFO_NULL, &window_ptr);
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            window_ptr[i] = 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 1; i < size; i++)
    {

        pair_comms[i] = MPI_COMM_NULL;
        windows[i] = MPI_WIN_NULL;

        // Nur rank 0 und rank i gehören zu diesem Paar
        if (rank == 0 || rank == i)
        {

            int ranks[2] = {0, i};

            MPI_Group pair_group;

            MPI_Group_incl(world_group, 2, ranks, &pair_group);

            MPI_Comm_create_group(
                MPI_COMM_WORLD,
                pair_group,
                i,
                &pair_comms[i]);

            MPI_Group_free(&pair_group);

            // Jeder Prozess stellt genau einen int bereit
            if (rank == 0)
            {
                MPI_Win_create(
                    window_ptr,
                    BUFFER_SIZE * sizeof(uint64_t),
                    sizeof(uint64_t),
                    MPI_INFO_NULL,
                    pair_comms[i],
                    &windows[i]);
            }
            else
            {
                MPI_Win_create(
                    window_ptr,
                    0,
                    sizeof(uint64_t),
                    MPI_INFO_NULL,
                    pair_comms[i],
                    &windows[i]);
            }

            int local_rank;
            MPI_Comm_rank(pair_comms[i], &local_rank);
        }
    }

    uint64_t one = 1;
    uint64_t result = 0;

    for (int rep = 0; rep < ntimes + WARMUP; rep++)
    {

        if (rank != 0)
        {
            MPI_Win_lock(MPI_LOCK_SHARED, 0, MPI_MODE_NOCHECK, windows[rank]);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        for (int stride = 0; stride <= BUFFER_SIZE / (size - 1); stride += STRIDE_STEP)
        {
#ifdef USE_SCOREP
            SCOREP_USER_METRIC_INT64(my_local_metric, (stride * sizeof(uint64_t)))
#endif

            if (rank == 0)
            {
                double start = MPI_Wtime();
                MPI_Barrier(MPI_COMM_WORLD);
                double elapsed = MPI_Wtime() - start;

                if (rep >= WARMUP)
                {
                    printf("%d,%lu,%f\n", num_observer, stride * sizeof(uint64_t), elapsed);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
            else
            {
                for (int i = 0; i < ITERATIONS; i++)
                {
                    MPI_Fetch_and_op(&one, &result, MPI_UINT64_T, 0, (rank - 1) * stride, MPI_SUM, windows[rank]);
                }
                MPI_Win_flush(0, windows[rank]);

                MPI_Barrier(MPI_COMM_WORLD);
                MPI_Barrier(MPI_COMM_WORLD);
            }
#ifdef USE_SCOREP
        SCOREP_USER_METRIC_INT64(my_local_metric, (stride * sizeof(uint64_t))
#endif
        }

        if (rank != 0)
        {
            MPI_Win_unlock(0, windows[rank]);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}