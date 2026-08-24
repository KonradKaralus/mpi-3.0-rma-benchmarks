# A suite of Benchmarks for MPI 3.0 RMA

This project contains benchmarks aimed at measuring performance metrics of MPI 3.0 RMA in non-standard cases. 
The experiments target e.g. communication between host as well as device (GPU) memory, strided data types, varying numbers of communicating ranks, atomic operations and the influence of RMA communication on tasks running
on the target rank and vice versa.
The benchmark suite has been developed as part of 'Großer Beleg' at TU Dresden.

## Provided Benchmarks

| Name | Description |
| -------- | ------- |
| atomic-spacing | Investigate atomic address collisions in InfiniBand NICs. |
| device-mem | Compare performance between RMA accesses to host and device memory using various data types. |
| overlapping-windows-performance | Measure performance of the overlapping windows technique. |
| overlapping-windows-semantics | Assert semantics of the overlapping windows technique. |
| overview | Basic test cases for different message sizes and numbers of accessors. |
| replace-performance | Compare performance of MPI_Put/MPI_Get and atomics. |
| interaction | Measure performance influence of tasks on target and RMA operations on each other. |
| osu-mbs | The [OSU Micro-Benchmarks](https://mvapich.cse.ohio-state.edu/benchmarks/). Used to obtain reference values for basic test cases. |

## Usage

Alternatively to building the benchmarks individually, a python script is provided with [helper.py](/helper.py).

### Build all benchmarks
```
python3 helper.py build all
```

### Build specific benchmarks
```
python3 helper.py build overview,interaction
```

### Exclude specific benchmarks from building
```
python3 helper.py build all --exclude device_mem,osu-mbs
```
This configuration builds all benchmarks that do not require device memory.

### Clean all benchmarks
```
python3 helper.py clean all
```
As with building, specific benchmarks and exclusions may be used.

### Plot results of all benchmarks
```
python3 helper.py results all
```
The script searches for ``results/results*.py`` files in the specified benchmarks' directories. 
These will attempt to copy required results files from their parent directory (the benchmark directory) and produce the results.

As with building, specific benchmarks and exclusions may be used.

```
python3 helper.py results all -t
```
Specifiying `-t` will plot the results using latex fonts and styles.

### Running benchmarks
Automation for running benchmarks is deliberately not included, as you will most likely won't want to run multiple benchmarks at once. Furthermore, the execution parameters specified will have to be changed for most test systems.

To run a benchmark using the slurm scheduling system, refer to the `run.sh` files in each directory.

### Striding benchmark
The [striding](/striding/) benchmark is intended for use with the [Score-P](https://www.vi-hps.org/projects/score-p/overview/overview.html) toolchain only.