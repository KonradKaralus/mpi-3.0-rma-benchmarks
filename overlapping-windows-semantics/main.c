#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdint.h>

#define ITERATIONS 50000

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Win *windows = malloc(size * sizeof(MPI_Win));
    MPI_Comm *pair_comms = malloc(size * sizeof(MPI_Comm));

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    uint64_t *window_ptr;
    if (rank == 0)
    {
        MPI_Alloc_mem(sizeof(uint64_t), MPI_INFO_NULL, &window_ptr);
        *window_ptr = 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 1; i < size; i++)
    {

        pair_comms[i] = MPI_COMM_NULL;
        windows[i] = MPI_WIN_NULL;

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

            if (rank == 0)
            {
                MPI_Win_create(
                    window_ptr,
                    sizeof(uint64_t),
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
    if (rank >= size / 2)
    {
        MPI_Win_lock(MPI_LOCK_SHARED, 0, MPI_MODE_NOCHECK, windows[rank]);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    uint64_t zero_accesses = 0;

    if (rank < size / 2)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
    }
    else
    {
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Fetch_and_op(&one, &result, MPI_UINT64_T, 0, 0, MPI_SUM, windows[rank]);
        }
        MPI_Win_flush(0, windows[rank]);

        MPI_Barrier(MPI_COMM_WORLD);

        zero_accesses += ITERATIONS;

        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank >= size / 2)
    {
        MPI_Win_unlock(0, windows[rank]);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    uint64_t res = 0;

    MPI_Reduce(&zero_accesses, &res, 1, MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("zero should be: %lu, is after: %lu\n", res, window_ptr[0]);

        for (int i = 1; i < size; i++)
        {
            MPI_Win_free(&windows[i]);
        }
    }
    else
    {
        MPI_Win_free(&windows[rank]);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}