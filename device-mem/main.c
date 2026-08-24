#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

MPI_Win device_win;
MPI_Win host_win;

uint64_t *device_buf;
uint64_t *host_buf;

int device_id;

int WINDOW_SIZE = 1024;
#define ITERATIONS 100

void set_buffers(uint64_t val)
{
    int host = omp_get_initial_device();

    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        host_buf[i] = val;
    }

    omp_target_memcpy(device_buf, host_buf,
                      WINDOW_SIZE * sizeof(uint64_t), 0, 0,
                      device_id, host);
}

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Datatype stride_type;

    MPI_Type_vector(
        WINDOW_SIZE / 2, // #blocks
        1,               // #elements/block
        2,               // stride
        MPI_UINT64_T,
        &stride_type);

    MPI_Type_commit(&stride_type);

    device_id = omp_get_default_device();

    device_buf = omp_target_alloc(WINDOW_SIZE * sizeof(uint64_t), device_id);

    host_buf = malloc(WINDOW_SIZE * sizeof(uint64_t));

    if (!device_buf)
    {
        printf("omp_target_alloc failed\n");
        exit(1);
    }

    set_buffers(1);

    MPI_Win_create(device_buf, WINDOW_SIZE * sizeof(uint64_t), sizeof(uint64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &device_win);
    MPI_Win_create(host_buf, WINDOW_SIZE * sizeof(uint64_t), sizeof(uint64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &host_win);

    if (rank == 0)
    {
        MPI_Barrier(MPI_COMM_WORLD);
    }
    else
    {
        MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, device_win);
        MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, host_win);
        double start;
        printf("source,target,op,stride,time\n");

        // H-H
        // Get
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Get(host_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, host_win);
        }
        MPI_Win_flush_local(0, host_win);
        printf("H,H,get,false,%f\n", MPI_Wtime() - start);

        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Get(host_buf, 1, stride_type, 0, 0, WINDOW_SIZE / 2, MPI_UINT64_T, host_win);
        }
        MPI_Win_flush_local(0, host_win);
        printf("H,H,get,true,%f\n", MPI_Wtime() - start);

        // Put
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(host_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, host_win);
        }
        MPI_Win_flush(0, host_win);
        printf("H,H,put,false,%f\n", MPI_Wtime() - start);

        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(host_buf, WINDOW_SIZE / 2, MPI_UINT64_T, 0, 0, 1, stride_type, host_win);
        }
        MPI_Win_flush(0, host_win);
        printf("H,H,put,true,%f\n", MPI_Wtime() - start);

        // H-D
        // Get
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Get(host_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, device_win);
        }
        MPI_Win_flush_local(0, device_win);
        printf("H,D,get,false,%f\n", MPI_Wtime() - start);

        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Get(host_buf, 1, stride_type, 0, 0, WINDOW_SIZE / 2, MPI_UINT64_T, device_win);
        }
        MPI_Win_flush_local(0, host_win);
        printf("H,D,get,true,%f\n", MPI_Wtime() - start);

        // Put
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(host_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, device_win);
        }
        MPI_Win_flush(0, device_win);
        printf("H,D,put,false,%f\n", MPI_Wtime() - start);

        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(host_buf, WINDOW_SIZE / 2, MPI_UINT64_T, 0, 0, 1, stride_type, device_win);
        }
        MPI_Win_flush(0, host_win);
        printf("H,D,put,true,%f\n", MPI_Wtime() - start);

        // D-H
        // Get
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Get(device_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, host_win);
        }
        MPI_Win_flush_local(0, host_win);
        printf("D,H,get,false,%f\n", MPI_Wtime() - start);

        // start = MPI_Wtime();
        // for (int i = 0; i < ITERATIONS; i++)
        // {
        //     MPI_Get(device_buf, 1, stride_type, 0, 0, WINDOW_SIZE / 2, MPI_UINT64_T, host_win);
        // }
        // MPI_Win_flush_local(0, host_win);
        printf("D,H,get,true,%f\n", 0.0);

        // Put
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(device_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, host_win);
        }
        MPI_Win_flush(0, host_win);
        printf("D,H,put,false,%f\n", MPI_Wtime() - start);

        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(device_buf, WINDOW_SIZE / 2, MPI_UINT64_T, 0, 0, 1, stride_type, host_win);
        }
        MPI_Win_flush(0, host_win);
        printf("D,H,put,true,%f\n", MPI_Wtime() - start);

        // D-D
        // Get
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Get(device_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, device_win);
        }
        MPI_Win_flush_local(0, device_win);
        printf("D,D,get,false,%f\n", MPI_Wtime() - start);

        // start = MPI_Wtime();
        // for (int i = 0; i < ITERATIONS; i++)
        // {
        //     MPI_Get(device_buf, 1, stride_type, 0, 0, WINDOW_SIZE / 2, MPI_UINT64_T, device_win);
        // }
        // MPI_Win_flush_local(0, host_win);
        printf("D,D,get,true,%f\n", 0.0);

        // Put
        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(device_buf, WINDOW_SIZE, MPI_UINT64_T, 0, 0, WINDOW_SIZE, MPI_UINT64_T, device_win);
        }
        MPI_Win_flush(0, device_win);
        printf("D,D,put,false,%f\n", MPI_Wtime() - start);

        start = MPI_Wtime();
        for (int i = 0; i < ITERATIONS; i++)
        {
            MPI_Put(device_buf, WINDOW_SIZE / 2, MPI_UINT64_T, 0, 0, 1, stride_type, device_win);
        }
        MPI_Win_flush(0, host_win);
        printf("D,D,put,true,%f\n", MPI_Wtime() - start);

        MPI_Win_unlock(0, host_win);
        MPI_Win_unlock(0, device_win);

        // printf("rank %d get: %d, fop: %d \n", rank, recv_get, recv_fop);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
}