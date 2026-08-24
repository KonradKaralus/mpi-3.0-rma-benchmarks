#!/usr/bin/python

import polars as pl
import matplotlib.pyplot as plot
import numpy as np
import sys

import shutil, os

req = ["data.out"]

for file in req:
    if file not in os.listdir("../osu-micro-benchmarks-8.0b2/c/mpi/one-sided/"):
        print(f"Required file {file} not found in parent directory.")
        exit(1)
    shutil.copyfile(f"../{file}", file)

tex = False
if len(sys.argv) > 1:
    if sys.argv[1] == "tex":
        plot.rcParams["text.usetex"] = True
        plot.rcParams["font.family"] = "serif"
        plot.rcParams["font.serif"] = ["Computer Modern Roman"]
        plot.rcParams["font.size"] = 14
        tex = True

out = ""

with open("data.out", "r") as f:
    for line in f.readlines():
        if not line.startswith("#") and not (line.strip() == "") and not line.startswith("src:") and not line.startswith("srun"):
            out += line
    
current_setting = ""
    
csv = "op,sync,t1,t2,size,lat\n"
            
for line in out.splitlines():
    if line.startswith("-#-"):
        current_setting = ",".join(line.removeprefix("-#- osu_").split()) + ","
        continue
    
    data = ",".join(line.split())
    
    csv += f"{current_setting}{data}\n"
    
with open("out.csv", "w") as f:
    f.write(csv)
    
df_lock = pl.read_csv("out.csv").filter(pl.col("sync") == "lock")

df_get_lat = df_lock.filter(pl.col("op") == "get_latency")
df_put_lat = df_lock.filter(pl.col("op") == "put_latency")
df_get_acc_lat = df_lock.filter(pl.col("op") == "put_latency")

titles = ["MPI_Get", "MPI_Put", "MPI_Get_acc"]


max_x = max(df_get_lat["size"])

x_pos = []
x_labels = []
n=1
i=0
while n<=max_x:
    x_pos.append(n)
    x_labels.append(fr"$2^{{{i}}}$")
    n*=2
    i+=1

i=0

for df in [df_get_lat, df_put_lat, df_get_acc_lat]:
    hh = df.filter((pl.col("t1") == "H") & (pl.col("t2") == "H"))
    hd = df.filter((pl.col("t1") == "H") & (pl.col("t2") == "D"))
    dh = df.filter((pl.col("t1") == "D") & (pl.col("t2") == "H"))
    dd = df.filter((pl.col("t1") == "D") & (pl.col("t2") == "D"))


    plot.scatter(x=hh["size"], y=hh["lat"], color="blue",marker="o",label="host-host")
    plot.scatter(x=hd["size"], y=hd["lat"], color="green",marker=",",label="host-device")
    plot.scatter(x=dh["size"], y=dh["lat"], color="orange",marker="v",label="device-host")
    plot.scatter(x=dd["size"], y=dd["lat"], color="black",marker="^",label="device-device")

    plot.yscale("log")

    plot.xscale("log")
    plot.legend()

    inner = f"\\texttt{{{titles[i]}}}" if tex else titles[i]
    
    title = f"Latency of {inner} for different message sizes"
    plot.xlabel("Message size [B]")
    plot.ylabel("Latency [us]")
    plot.title(title)
    plot.legend()
    
    plot.xticks(ticks=x_pos, labels=x_labels)
    plot.grid(True, which="both", linestyle="--", alpha=0.5)

    plot.tight_layout()

    plot.savefig(f"{titles[i]}.pdf")
    plot.savefig(f"{titles[i]}.png", dpi=300)
    plot.clf()

    i+=1


df_atomics = pl.read_csv("out.csv").filter(pl.col("size") == 1)

import numpy as np


df_cas = df_atomics.filter((pl.col("op") == "cas_latency"))
df_fop = df_atomics.filter((pl.col("op") == "fop_latency"))

x = np.arange(len(df_cas["sync"]))


width = 0.3

fig, ax = plot.subplots()

cas = f"\\texttt{{MPI_Compare_and_swap}}" if tex else "MPI_Compare_and_swap" 
fop = f"\\texttt{{MPI_Fetch_and_op}}" if tex else "MPI_Fetch_and_op" 

ax.bar(x - width/2, df_cas["lat"], width, label=cas, color="blue")
ax.bar(x + width/2, df_fop["lat"], width, label=fop, color="green")

plot.ylabel("Latency [us]")
plot.xlabel("Synchronization method")
plot.title("Latency of atomic operations for\n different synchronization methods")
plot.legend()
plot.tight_layout()
plot.xticks(ticks=x,labels=df_cas["sync"])
plot.grid(True, which="both", linestyle="--", alpha=0.5)

plot.savefig("atomics.pdf")
plot.savefig(f"atomics.png", dpi=300)
