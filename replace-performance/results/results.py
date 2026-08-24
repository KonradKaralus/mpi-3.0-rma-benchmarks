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

df = pl.read_csv("output.csv").with_columns((pl.col("lat")*1000.0).alias("lat_ms"))

df = df.group_by("direction","function","type","op").agg(pl.col("lat_ms").median().alias("median"),pl.col("lat_ms").max().alias("max"),pl.col("lat_ms").min().alias("min"))
df = df.with_columns((pl.col("max") - pl.col("median")).alias("ymax"),(pl.col("median") - pl.col("min")).alias("ymin"))

custom_order = {"get": 0, "get_acc": 1, "put": 2, "acc":3}

df_norm = df.filter(pl.col("type") == "uint64_t").sort(pl.col("function").replace_strict(custom_order))
df_str = df.filter(pl.col("type") == "vec").sort(pl.col("function").replace_strict(custom_order))

yerr_norm = [df_norm["ymin"], df_norm["ymax"]]
yerr_str = [df_str["ymin"], df_str["ymax"]]


x = np.arange(4)

width = 0.15

fig, ax = plot.subplots()

ax.bar(x - width/2, df_norm["median"], width, label="normal", color="blue")
ax.bar(x + width/2, df_str["median"], width, label="strided", color="green")


map = {
    "get": f"\\texttt{{MPI_Get}}" if tex else "MPI_Get",
    "put": f"\\texttt{{MPI_Put}}"if tex else "MPI_Put",
    "acc": f"\\texttt{{MPI_Accumulate}}" if tex else "MPI_Accumulate",
    "get_acc": f"\\texttt{{MPI_Get_accumulate}}" if tex else "MPI_Get_accumulate",
}

labels = []
for row in df_norm.iter_rows(named=True):
    labels.append(f'{map[row["function"]]}')


plot.ylabel("Latency [ms]")
plot.xlabel("Communication function")
plot.title("Median total latency of 5000 strided and non-strided \ntransfers (4/8Kib) for different communication functions")
plot.legend(loc="upper right",bbox_to_anchor=(1.01, 0.75))
plot.xticks(ticks=x,labels=labels)
plot.yscale("log")
plot.grid(True, which="both", linestyle="--", alpha=0.5)

plot.tight_layout()


plot.savefig("plot.pdf")
plot.savefig("plot.png", dpi=300)