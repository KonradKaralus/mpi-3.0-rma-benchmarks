#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdint.h>

#define ITERATIONS 5000

MPI_Win win;
uint64_t *window_ptr;

#define WINDOW_SIZE 8192

int ntimes = 1;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint64_t sendbuf[WINDOW_SIZE];
    uint64_t targetbuf[WINDOW_SIZE];

    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        sendbuf[i] = 1;
        targetbuf[i] = 1;
    }

    if (argc > 1)
    {
        ntimes = atoi(argv[1]);
    }

    if (rank == 0)
    {
        // printf("direction,function,type,op,lat\n");

        MPI_Win_allocate(
            sizeof(uint64_t) * WINDOW_SIZE,
            sizeof(uint64_t),
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
            sizeof(uint64_t),
            MPI_INFO_NULL,
            MPI_COMM_WORLD,
            &window_ptr,
            &win);
    }

    MPI_Datatype stride_type;

    MPI_Type_vector(
        WINDOW_SIZE / 2, // #blocks
        1,               // #elements/block
        2,               // stride
        MPI_UINT64_T,
        &stride_type);

    MPI_Type_commit(&stride_type);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);

    for (int rep = 0; rep < ntimes; rep++)
    {
        if (rank == 0)
        {
            int ntimes = 8;
            for (int i = 0; i < ntimes; i++)
            {
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        else
        {
            double start = MPI_Wtime();
            // put
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Accumulate(sendbuf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, MPI_REPLACE, win);
            }
            MPI_Win_flush(0, win);
            double dur = MPI_Wtime() - start;
            printf("put,acc,uint64_t,MPI_REPLACE,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);

            start = MPI_Wtime();
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Put(sendbuf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, win);
            }
            MPI_Win_flush(0, win);
            dur = MPI_Wtime() - start;
            printf("put,put,uint64_t,,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);

            start = MPI_Wtime();
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Accumulate(sendbuf, WINDOW_SIZE / 2, MPI_UINT64_T, 0, 0, 1, stride_type, MPI_REPLACE, win);
            }
            MPI_Win_flush(0, win);
            dur = MPI_Wtime() - start;
            printf("put,acc,vec,MPI_REPLACE,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);

            start = MPI_Wtime();
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Put(sendbuf, WINDOW_SIZE / 2, MPI_UINT64_T, 0, 0, 1, stride_type, win);
            }
            MPI_Win_flush(0, win);
            dur = MPI_Wtime() - start;
            printf("put,put,vec,,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);

            // get

            start = MPI_Wtime();
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Get_accumulate(sendbuf, WINDOW_SIZE, MPI_UINT64_T, targetbuf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, MPI_NO_OP, win);
            }
            MPI_Win_flush(0, win);
            dur = MPI_Wtime() - start;
            printf("get,get_acc,uint64_t,MPI_NO_OP,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);

            start = MPI_Wtime();
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Get(sendbuf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, win);
            }
            MPI_Win_flush(0, win);
            dur = MPI_Wtime() - start;
            printf("get,get,uint64_t,,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);

            start = MPI_Wtime();
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Get_accumulate(sendbuf, WINDOW_SIZE / 2, MPI_UINT64_T, targetbuf, 1, stride_type, 0, 0, WINDOW_SIZE / 2, MPI_UINT64_T, MPI_NO_OP, win);
            }
            MPI_Win_flush(0, win);
            dur = MPI_Wtime() - start;
            printf("get,get_acc,vec,MPI_NO_OP,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);

            start = MPI_Wtime();
            for (int i = 0; i < ITERATIONS; i++)
            {
                MPI_Get(sendbuf, 1, stride_type, 0, 0, WINDOW_SIZE / 2, MPI_UINT64_T, win);
            }
            MPI_Win_flush(0, win);
            dur = MPI_Wtime() - start;
            printf("get,get,vec,,%f\n", dur);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    MPI_Win_unlock(0, win);

    MPI_Win_free(&win);

    MPI_Finalize();
    return 0;
}