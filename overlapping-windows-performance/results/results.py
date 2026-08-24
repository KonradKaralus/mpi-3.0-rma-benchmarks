#!/usr/bin/python

import polars as pl
import matplotlib.pyplot as plot
import numpy as np
import sys

import shutil, os

req = ["output.csv"]

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

df = pl.read_csv("output.csv")

df = df.sort("type")

df_ompi = df.filter(pl.col("compiler") == "ompi")
df_impi = df.filter(pl.col("compiler") == "impi")
x = np.arange(2)

width = 0.15

fig, ax = plot.subplots()
fig.set_size_inches(6,5)


ax.bar(x - width/2, df_ompi["lat"], width, label="OpenMPI", color="blue")
ax.bar(x + width/2, df_impi["lat"], width, label="IntelMPI", color="green")

# plot.ylim(0)
plot.ylabel("Total Latency [s]")
plot.xlabel("Window type")
inner = "\\texttt{{MPI_Fetch_and_op}}" if tex else "MPI_Fetch_and_op"
plot.title(f"Total latency of 32 observers accessing different\nwindow types with {inner} 50000 times")
plot.legend()
plot.xticks(ticks=x,labels=["Overlapping" if l == "overlap" else "Single" for l in df_ompi["type"]])
plot.grid(True, which="both", linestyle="--", alpha=0.5)
plot.ylim(0,10)

plot.tight_layout()


plot.savefig("ow_perf.pdf")
plot.savefig(f"ow_perf.png", dpi=300)
