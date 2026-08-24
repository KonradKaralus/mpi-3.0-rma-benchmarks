#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdint.h>

#define ITERATIONS 50000

int ntimes = 1;
int num_observer = 32;

MPI_Win main_win;
uint64_t *main_window_ptr;
uint64_t *pair_window_ptr;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc > 2)
    {
        ntimes = atoi(argv[1]);
        num_observer = atoi(argv[2]);
    }

    MPI_Win *windows = malloc(size * sizeof(MPI_Win));
    MPI_Comm *pair_comms = malloc(size * sizeof(MPI_Comm));

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    if (rank == 0)
    {
        MPI_Alloc_mem(sizeof(uint64_t) * size, MPI_INFO_NULL, &pair_window_ptr);
        MPI_Alloc_mem(sizeof(uint64_t) * size, MPI_INFO_NULL, &main_window_ptr);

        for (int i = 0; i < size; i++)
        {
            pair_window_ptr[i] = 0;
            main_window_ptr[i] = 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        MPI_Win_create(
            main_window_ptr,
            sizeof(uint64_t) * size,
            sizeof(uint64_t),
            MPI_INFO_NULL,
            MPI_COMM_WORLD,
            &main_win);
    }
    else
    {
        MPI_Win_create(
            main_window_ptr,
            0,
            sizeof(uint64_t),
            MPI_INFO_NULL,
            MPI_COMM_WORLD,
            &main_win);
    }

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
                    pair_window_ptr,
                    sizeof(uint64_t) * size,
                    sizeof(uint64_t),
                    MPI_INFO_NULL,
                    pair_comms[i],
                    &windows[i]);
            }
            else
            {
                MPI_Win_create(
                    pair_window_ptr,
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
        MPI_Win_lock(MPI_LOCK_SHARED, 0, MPI_MODE_NOCHECK, main_win);
    }

    for (int j = 0; j < ntimes; j++)
    {

        MPI_Barrier(MPI_COMM_WORLD);

        double start = MPI_Wtime();
        double elapsed_overlap = -1.0;
        double elapsed_one_win = -1.0;

        // overlapping wins
        if ((rank >= size / 2) && ((rank - size / 2) < num_observer))
        {
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Fetch_and_op(&one, &result, MPI_UINT64_T, 0, rank, MPI_SUM, windows[rank]);
            }
            MPI_Win_flush(0, windows[rank]);
            elapsed_overlap = MPI_Wtime() - start;

            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Barrier(MPI_COMM_WORLD);
        }
        else
        {
            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Barrier(MPI_COMM_WORLD);
        }

        start = MPI_Wtime();

        // one win

        if ((rank >= size / 2) && ((rank - size / 2) < num_observer))
        {
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Fetch_and_op(&one, &result, MPI_UINT64_T, 0, rank, MPI_SUM, main_win);
            }
            MPI_Win_flush(0, windows[rank]);
            elapsed_one_win = MPI_Wtime() - start;

            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Barrier(MPI_COMM_WORLD);
        }
        else
        {
            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Barrier(MPI_COMM_WORLD);
        }

        if (rank >= size / 2)
        {
            MPI_Win_unlock(0, windows[rank]);
            MPI_Win_unlock(0, main_win);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        double res_overlap = 0.0;
        double res_one_win = 0.0;

        MPI_Reduce(&elapsed_overlap, &res_overlap, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        MPI_Reduce(&elapsed_one_win, &res_one_win, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            printf("%d,overlap,%s,%f\n", num_observer, COMPILER, res_overlap);
            printf("%d,one,%s,%f\n", num_observer, COMPILER, res_one_win);
        }
    }

    if (rank == 0)
    {

        for (int i = 1; i < size; i++)
        {
            MPI_Win_free(&windows[i]);
        }
        MPI_Win_free(&main_win);
    }
    else
    {
        MPI_Win_free(&windows[rank]);
        MPI_Win_free(&main_win);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}