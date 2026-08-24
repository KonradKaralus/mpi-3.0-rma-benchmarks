gpu_dims = ["D", "H"]
benches = ["osu_put_latency","osu_get_latency", "osu_acc_latency", "osu_cas_latency", "osu_fop_latency", "osu_get_acc_latency"]
not_working = ["osu_acc_latency", "osu_cas_latency", "osu_fop_latency"]
sync = ["lock", "flush", "flush_local", "lock_all", "pscw", "fence"]

out = """#!/bin/bash

#SBATCH --nodes=2
#SBATCH --time=01:00:00
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:2
#SBATCGH--exclusive
#SBATCH --ntasks=2


module load release/2026  GCC/14.3.0  OpenMPI/5.0.8
module load Score-P/9.4-CUDA-12.9.1

rm data.out
"""

for bench in benches:
    for sync_method in sync:
        for dim1 in gpu_dims:
            for dim2 in gpu_dims:
                if bench in not_working and (dim1 == "D" or dim2 == "D"):
                    continue
                
                
                out += f"echo -#- {bench} {sync_method} {dim1} {dim2} >> data.out\n"
                # print(f"srun --nodes 2 --gres=gpu:1 --time=00:00:30 osu_put_latency -s lock -i 10000 -m 16777216:16777217 D D")
                out += f"srun {bench} -s {sync_method} -i 1000 {dim1} {dim2} >> data.out\n"

with open("run_all.sh", "w") as f:
    f.write(out)
