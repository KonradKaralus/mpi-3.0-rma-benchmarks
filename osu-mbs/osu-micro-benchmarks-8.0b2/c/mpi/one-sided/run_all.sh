#!/bin/bash

#SBATCH --nodes=2
#SBATCH --time=01:00:00
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:2
#SBATCGH--exclusive
#SBATCH --ntasks=2


module load release/2026  GCC/14.3.0  OpenMPI/5.0.8
module load Score-P/9.4-CUDA-12.9.1

rm data.out
echo -#- osu_put_latency lock D D >> data.out
srun osu_put_latency -s lock -i 1000 D D >> data.out
echo -#- osu_put_latency lock D H >> data.out
srun osu_put_latency -s lock -i 1000 D H >> data.out
echo -#- osu_put_latency lock H D >> data.out
srun osu_put_latency -s lock -i 1000 H D >> data.out
echo -#- osu_put_latency lock H H >> data.out
srun osu_put_latency -s lock -i 1000 H H >> data.out
echo -#- osu_put_latency flush D D >> data.out
srun osu_put_latency -s flush -i 1000 D D >> data.out
echo -#- osu_put_latency flush D H >> data.out
srun osu_put_latency -s flush -i 1000 D H >> data.out
echo -#- osu_put_latency flush H D >> data.out
srun osu_put_latency -s flush -i 1000 H D >> data.out
echo -#- osu_put_latency flush H H >> data.out
srun osu_put_latency -s flush -i 1000 H H >> data.out
echo -#- osu_put_latency flush_local D D >> data.out
srun osu_put_latency -s flush_local -i 1000 D D >> data.out
echo -#- osu_put_latency flush_local D H >> data.out
srun osu_put_latency -s flush_local -i 1000 D H >> data.out
echo -#- osu_put_latency flush_local H D >> data.out
srun osu_put_latency -s flush_local -i 1000 H D >> data.out
echo -#- osu_put_latency flush_local H H >> data.out
srun osu_put_latency -s flush_local -i 1000 H H >> data.out
echo -#- osu_put_latency lock_all D D >> data.out
srun osu_put_latency -s lock_all -i 1000 D D >> data.out
echo -#- osu_put_latency lock_all D H >> data.out
srun osu_put_latency -s lock_all -i 1000 D H >> data.out
echo -#- osu_put_latency lock_all H D >> data.out
srun osu_put_latency -s lock_all -i 1000 H D >> data.out
echo -#- osu_put_latency lock_all H H >> data.out
srun osu_put_latency -s lock_all -i 1000 H H >> data.out
echo -#- osu_put_latency pscw D D >> data.out
srun osu_put_latency -s pscw -i 1000 D D >> data.out
echo -#- osu_put_latency pscw D H >> data.out
srun osu_put_latency -s pscw -i 1000 D H >> data.out
echo -#- osu_put_latency pscw H D >> data.out
srun osu_put_latency -s pscw -i 1000 H D >> data.out
echo -#- osu_put_latency pscw H H >> data.out
srun osu_put_latency -s pscw -i 1000 H H >> data.out
echo -#- osu_put_latency fence D D >> data.out
srun osu_put_latency -s fence -i 1000 D D >> data.out
echo -#- osu_put_latency fence D H >> data.out
srun osu_put_latency -s fence -i 1000 D H >> data.out
echo -#- osu_put_latency fence H D >> data.out
srun osu_put_latency -s fence -i 1000 H D >> data.out
echo -#- osu_put_latency fence H H >> data.out
srun osu_put_latency -s fence -i 1000 H H >> data.out
echo -#- osu_get_latency lock D D >> data.out
srun osu_get_latency -s lock -i 1000 D D >> data.out
echo -#- osu_get_latency lock D H >> data.out
srun osu_get_latency -s lock -i 1000 D H >> data.out
echo -#- osu_get_latency lock H D >> data.out
srun osu_get_latency -s lock -i 1000 H D >> data.out
echo -#- osu_get_latency lock H H >> data.out
srun osu_get_latency -s lock -i 1000 H H >> data.out
echo -#- osu_get_latency flush D D >> data.out
srun osu_get_latency -s flush -i 1000 D D >> data.out
echo -#- osu_get_latency flush D H >> data.out
srun osu_get_latency -s flush -i 1000 D H >> data.out
echo -#- osu_get_latency flush H D >> data.out
srun osu_get_latency -s flush -i 1000 H D >> data.out
echo -#- osu_get_latency flush H H >> data.out
srun osu_get_latency -s flush -i 1000 H H >> data.out
echo -#- osu_get_latency flush_local D D >> data.out
srun osu_get_latency -s flush_local -i 1000 D D >> data.out
echo -#- osu_get_latency flush_local D H >> data.out
srun osu_get_latency -s flush_local -i 1000 D H >> data.out
echo -#- osu_get_latency flush_local H D >> data.out
srun osu_get_latency -s flush_local -i 1000 H D >> data.out
echo -#- osu_get_latency flush_local H H >> data.out
srun osu_get_latency -s flush_local -i 1000 H H >> data.out
echo -#- osu_get_latency lock_all D D >> data.out
srun osu_get_latency -s lock_all -i 1000 D D >> data.out
echo -#- osu_get_latency lock_all D H >> data.out
srun osu_get_latency -s lock_all -i 1000 D H >> data.out
echo -#- osu_get_latency lock_all H D >> data.out
srun osu_get_latency -s lock_all -i 1000 H D >> data.out
echo -#- osu_get_latency lock_all H H >> data.out
srun osu_get_latency -s lock_all -i 1000 H H >> data.out
echo -#- osu_get_latency pscw D D >> data.out
srun osu_get_latency -s pscw -i 1000 D D >> data.out
echo -#- osu_get_latency pscw D H >> data.out
srun osu_get_latency -s pscw -i 1000 D H >> data.out
echo -#- osu_get_latency pscw H D >> data.out
srun osu_get_latency -s pscw -i 1000 H D >> data.out
echo -#- osu_get_latency pscw H H >> data.out
srun osu_get_latency -s pscw -i 1000 H H >> data.out
echo -#- osu_get_latency fence D D >> data.out
srun osu_get_latency -s fence -i 1000 D D >> data.out
echo -#- osu_get_latency fence D H >> data.out
srun osu_get_latency -s fence -i 1000 D H >> data.out
echo -#- osu_get_latency fence H D >> data.out
srun osu_get_latency -s fence -i 1000 H D >> data.out
echo -#- osu_get_latency fence H H >> data.out
srun osu_get_latency -s fence -i 1000 H H >> data.out
echo -#- osu_acc_latency lock H H >> data.out
srun osu_acc_latency -s lock -i 1000 H H >> data.out
echo -#- osu_acc_latency flush H H >> data.out
srun osu_acc_latency -s flush -i 1000 H H >> data.out
echo -#- osu_acc_latency flush_local H H >> data.out
srun osu_acc_latency -s flush_local -i 1000 H H >> data.out
echo -#- osu_acc_latency lock_all H H >> data.out
srun osu_acc_latency -s lock_all -i 1000 H H >> data.out
echo -#- osu_acc_latency pscw H H >> data.out
srun osu_acc_latency -s pscw -i 1000 H H >> data.out
echo -#- osu_acc_latency fence H H >> data.out
srun osu_acc_latency -s fence -i 1000 H H >> data.out
echo -#- osu_cas_latency lock H H >> data.out
srun osu_cas_latency -s lock -i 1000 H H >> data.out
echo -#- osu_cas_latency flush H H >> data.out
srun osu_cas_latency -s flush -i 1000 H H >> data.out
echo -#- osu_cas_latency flush_local H H >> data.out
srun osu_cas_latency -s flush_local -i 1000 H H >> data.out
echo -#- osu_cas_latency lock_all H H >> data.out
srun osu_cas_latency -s lock_all -i 1000 H H >> data.out
echo -#- osu_cas_latency pscw H H >> data.out
srun osu_cas_latency -s pscw -i 1000 H H >> data.out
echo -#- osu_cas_latency fence H H >> data.out
srun osu_cas_latency -s fence -i 1000 H H >> data.out
echo -#- osu_fop_latency lock H H >> data.out
srun osu_fop_latency -s lock -i 1000 H H >> data.out
echo -#- osu_fop_latency flush H H >> data.out
srun osu_fop_latency -s flush -i 1000 H H >> data.out
echo -#- osu_fop_latency flush_local H H >> data.out
srun osu_fop_latency -s flush_local -i 1000 H H >> data.out
echo -#- osu_fop_latency lock_all H H >> data.out
srun osu_fop_latency -s lock_all -i 1000 H H >> data.out
echo -#- osu_fop_latency pscw H H >> data.out
srun osu_fop_latency -s pscw -i 1000 H H >> data.out
echo -#- osu_fop_latency fence H H >> data.out
srun osu_fop_latency -s fence -i 1000 H H >> data.out
echo -#- osu_get_acc_latency lock D D >> data.out
srun osu_get_acc_latency -s lock -i 1000 D D >> data.out
echo -#- osu_get_acc_latency lock D H >> data.out
srun osu_get_acc_latency -s lock -i 1000 D H >> data.out
echo -#- osu_get_acc_latency lock H D >> data.out
srun osu_get_acc_latency -s lock -i 1000 H D >> data.out
echo -#- osu_get_acc_latency lock H H >> data.out
srun osu_get_acc_latency -s lock -i 1000 H H >> data.out
echo -#- osu_get_acc_latency flush D D >> data.out
srun osu_get_acc_latency -s flush -i 1000 D D >> data.out
echo -#- osu_get_acc_latency flush D H >> data.out
srun osu_get_acc_latency -s flush -i 1000 D H >> data.out
echo -#- osu_get_acc_latency flush H D >> data.out
srun osu_get_acc_latency -s flush -i 1000 H D >> data.out
echo -#- osu_get_acc_latency flush H H >> data.out
srun osu_get_acc_latency -s flush -i 1000 H H >> data.out
echo -#- osu_get_acc_latency flush_local D D >> data.out
srun osu_get_acc_latency -s flush_local -i 1000 D D >> data.out
echo -#- osu_get_acc_latency flush_local D H >> data.out
srun osu_get_acc_latency -s flush_local -i 1000 D H >> data.out
echo -#- osu_get_acc_latency flush_local H D >> data.out
srun osu_get_acc_latency -s flush_local -i 1000 H D >> data.out
echo -#- osu_get_acc_latency flush_local H H >> data.out
srun osu_get_acc_latency -s flush_local -i 1000 H H >> data.out
echo -#- osu_get_acc_latency lock_all D D >> data.out
srun osu_get_acc_latency -s lock_all -i 1000 D D >> data.out
echo -#- osu_get_acc_latency lock_all D H >> data.out
srun osu_get_acc_latency -s lock_all -i 1000 D H >> data.out
echo -#- osu_get_acc_latency lock_all H D >> data.out
srun osu_get_acc_latency -s lock_all -i 1000 H D >> data.out
echo -#- osu_get_acc_latency lock_all H H >> data.out
srun osu_get_acc_latency -s lock_all -i 1000 H H >> data.out
echo -#- osu_get_acc_latency pscw D D >> data.out
srun osu_get_acc_latency -s pscw -i 1000 D D >> data.out
echo -#- osu_get_acc_latency pscw D H >> data.out
srun osu_get_acc_latency -s pscw -i 1000 D H >> data.out
echo -#- osu_get_acc_latency pscw H D >> data.out
srun osu_get_acc_latency -s pscw -i 1000 H D >> data.out
echo -#- osu_get_acc_latency pscw H H >> data.out
srun osu_get_acc_latency -s pscw -i 1000 H H >> data.out
echo -#- osu_get_acc_latency fence D D >> data.out
srun osu_get_acc_latency -s fence -i 1000 D D >> data.out
echo -#- osu_get_acc_latency fence D H >> data.out
srun osu_get_acc_latency -s fence -i 1000 D H >> data.out
echo -#- osu_get_acc_latency fence H D >> data.out
srun osu_get_acc_latency -s fence -i 1000 H D >> data.out
echo -#- osu_get_acc_latency fence H H >> data.out
srun osu_get_acc_latency -s fence -i 1000 H H >> data.out
