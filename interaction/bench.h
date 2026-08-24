#ifndef BENCH_H
#define BENCH_H

extern int WINDOW_SIZE;

extern int *window_ptr;
extern MPI_Win win;
extern int num_observers;

#ifdef INTEL_PROG
extern int prog_every_n;
#endif

void run_dgemm(int rank, int size);
void run_stream(int rank, int size);
void run_observer(int rank, int size);

#endif