#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdint.h>

#define WINDOW_SIZE 8192 * 2 // 8192 elements of uint32
#define ITERATIONS 10000

int ntimes = 1;
int num_observer = 1;

MPI_Win win;
uint8_t *window_ptr;

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

    uint8_t rcvbuf[WINDOW_SIZE];

    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        rcvbuf[i] = 1;
    }

    if (rank == 0)
    {
        MPI_Win_allocate(
            WINDOW_SIZE * sizeof(uint8_t),
            sizeof(uint8_t),
            MPI_INFO_NULL,
            MPI_COMM_WORLD,
            &window_ptr,
            &win);

        for (int i = 0; i < WINDOW_SIZE; i++)
        {
            window_ptr[i] = 1;
        }
    }
    else
    {
        MPI_Win_allocate(
            0,
            sizeof(uint8_t),
            MPI_INFO_NULL,
            MPI_COMM_WORLD,
            &window_ptr,
            &win);
    }

    double start;
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);

    for (int i = 0; i < ntimes; i++)
    {
        for (int observers = 1; observers <= num_observer; observers += 2)
        {
            for (int msg_size = 512; msg_size <= WINDOW_SIZE; msg_size += 512)
            {
                if (rank == 0)
                {
                    start = MPI_Wtime();
                    MPI_Barrier(MPI_COMM_WORLD);
                    double elapsed = MPI_Wtime() - start;
                    printf("%s,get,%d,%d,%f\n", COMPILER, observers, msg_size, elapsed);

                    start = MPI_Wtime();
                    MPI_Barrier(MPI_COMM_WORLD);
                    elapsed = MPI_Wtime() - start;
                    printf("%s,put,%d,%d,%f\n", COMPILER, observers, msg_size, elapsed);
                }
                else if ((rank >= size / 2) && ((rank - size / 2) < observers))
                {
                    for (int j = 0; j < ITERATIONS; j++)
                    {
                        MPI_Get(rcvbuf, msg_size, MPI_CHAR, 0, 0, msg_size, MPI_CHAR, win);
                    }
                    MPI_Win_flush(0, win);
                    MPI_Barrier(MPI_COMM_WORLD);

                    for (int j = 0; j < ITERATIONS; j++)
                    {
                        MPI_Put(rcvbuf, msg_size, MPI_CHAR, 0, 0, msg_size, MPI_CHAR, win);
                    }
                    MPI_Win_flush(0, win);
                    MPI_Barrier(MPI_COMM_WORLD);
                }
                else
                {
                    MPI_Barrier(MPI_COMM_WORLD);
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }
        }
    }

    MPI_Win_unlock(0, win);

    MPI_Win_free(&win);

    MPI_Finalize();
    return 0;
}