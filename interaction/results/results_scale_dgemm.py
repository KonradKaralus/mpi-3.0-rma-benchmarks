#!/usr/bin/python

import polars as pl
import matplotlib.pyplot as plot
import sys

import shutil, os

req = ["out_scale_dgemm.csv", "baseline_dgemm.csv"]

for file in req:
    if file not in os.listdir(".."):
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

dgemms = {"flops": [], "n_obs": []}

gets = []
lines:list[str]=[]
lines_base:list[str]=[]
base = {"observers":[],"flops":[]}

with open("out_scale_dgemm.csv","r") as f:
    for line in f.readlines():
        if line.startswith("___"):
            lines.append(line.removeprefix("___").replace("\n",""))

with open("baseline_dgemm.csv","r") as f:
    for line in f.readlines():
        if line.startswith("___"):
            lines_base.append(line.removeprefix("___dgemm: ").replace("\n",""))

current = 0
wanted = 1
for (idx, line) in enumerate(lines_base):
    flop = float(line.split(",")[-1])
    base["observers"].append(wanted)
    base["flops"].append(flop)
    
    current += 1
    if current == wanted:
        current = 0
        wanted += 1

ndgemms = 1
read_dgemms = 0

for line in lines:
    if "get" in line:
        gets.append(float(line.split(",")[-1]))
        continue

    if read_dgemms == ndgemms:
        ndgemms += 1
        read_dgemms = 0

    if read_dgemms < ndgemms:
        dgemms["flops"].append(float(line.split(",")[3]))
        dgemms["n_obs"].append(int(ndgemms))
        read_dgemms += 1

df_base = pl.DataFrame(base).with_columns((pl.col("flops")/1000000000.0).alias("gflops"))
df_base = df_base.group_by("observers").agg(pl.col("gflops").median().alias("median"), pl.col("gflops").max().alias("max"), pl.col("gflops").min().alias("min"))
df_base = df_base.with_columns((pl.col("max") - pl.col("median")).alias("ymax"),(pl.col("median") - pl.col("min")).alias("ymin"))

df_dgemms = pl.DataFrame(dgemms).with_columns((pl.col("flops")/1000000000.0).alias("gflops"))
df_dgemms = df_dgemms.group_by("n_obs").agg(pl.col("gflops").median().alias("median"), pl.col("gflops").max().alias("max"), pl.col("gflops").min().alias("min"))
df_dgemms = df_dgemms.with_columns((pl.col("max") - pl.col("median")).alias("ymax"),(pl.col("median") - pl.col("min")).alias("ymin"))

df_dgemms = df_dgemms.with_columns(pl.col("n_obs").map_elements(lambda x: gets[x-1], return_dtype=pl.Float64()).alias("total_metric/t"))
df_dgemms = df_dgemms.with_columns((pl.col("total_metric/t")/1000000.0).alias("mtotal_metric/t"))

yerr = [df_dgemms["ymin"], df_dgemms["ymax"]]
yerr_base = [df_base["ymin"], df_base["ymax"]]

x_pos=[]
n=13
while n<=df_dgemms["n_obs"].max():
    x_pos.append(n)
    n+=13

plot.errorbar(x=df_dgemms["n_obs"],y=df_dgemms["median"], yerr=yerr, fmt="o", linestyle="none",ms=3.0,label="DGEMM with RMA accesses")
plot.errorbar(x=df_base["observers"],y=df_base["median"], yerr=yerr_base, fmt="<", linestyle="none",ms=3.0,label="Baseline DGEMM")

plot.xlabel("Number compute ranks")

plot.ylim(bottom=0.0, top=35.0)
plot.ylabel("DGEMM performance [GFLOP/s]")

plot.title("Median DGEMM performance for different numbers\n of computing ranks with 32 observers")

plot.legend()

plot.xticks(x_pos)

plot.grid(True, which="both", linestyle="--", alpha=0.5)
plot.tight_layout()

plot.savefig(f"scale_dgemms_flops.pdf")
plot.savefig(f"scale_dgemms_flops.png", dpi=300)

plot.clf()

plot.scatter(x=df_dgemms["n_obs"],y=df_dgemms["mtotal_metric/t"],marker=",",label="Observer throughput", c="green")
plot.ylim(bottom=0.0,top=400)
plot.ylabel("Accumulated throughput [MB/s]")
plot.xlabel("Number compute ranks")

plot.xticks(x_pos)

plot.grid(True, which="both", linestyle="--", alpha=0.5)
get = "\\texttt{{MPI_Get}}" if tex else "MPI_Get"
plot.title(f"Accumulated throughput of 32 observers for \ndifferent numbers of computing ranks calling\n strided {get}, 4KiB message size.")

plot.tight_layout()

plot.savefig(f"scale_dgemms_tp.pdf")
plot.savefig(f"scale_dgemms_tp.png", dpi=300)
