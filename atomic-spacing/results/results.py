#!/usr/bin/python

import polars as pl
import matplotlib.pyplot as plot
import numpy as np
import sys, os

import shutil

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

df = pl.read_csv("output.csv").with_columns((pl.col("time")*1000.0*1000.0/10000.0).alias("time_us"))

l = df.group_by("offset","observers").len()["len"].first()


df = df.group_by("offset","observers").agg(pl.col("time_us").median().alias("median"), pl.col("time_us").max().alias("max"), pl.col("time_us").min().alias("min"))
df = df.with_columns((pl.col("max") - pl.col("median")).alias("ymax"),(pl.col("median") - pl.col("min")).alias("ymin"))


# single

df_single = df.filter(pl.col("observers")==32)

plot.grid(True, which="both", linestyle="--", alpha=0.5)

yerr = [df_single["ymin"], df_single["ymax"]]

plot.errorbar(x=df_single["offset"],y=df_single["median"], yerr=yerr, fmt="o", linestyle="none",ms=2)

plot.ylim(bottom=0)

plot.xticks(np.arange(0, max(df_single["offset"])+1, 2048))

plot.ylabel("Latency [us]")
plot.xlabel("Access stride [B]")
op = "\\texttt{{MPI_Fetch_and_op}}" if tex else "MPI_Fetch_and_op"
plot.title(f"Median Latency of {op} for different \naccess strides over {l} runs (32 observers)")

plot.tight_layout()


plot.savefig("atomic_spacing.pdf")
plot.savefig("atomic_spacing.png", dpi=300)

plot.clf()

df=df.sort("observers","offset")

df = df.filter(pl.col("observers")>=32)

pivot = df.pivot(on="offset", index="observers", values="median")

# pivot = pivot.fill_null(float("nan"))

x = np.array(pivot.columns[1:], dtype=int)
y = pivot["observers"].to_list()
z = pivot.select(pl.exclude("observers")).to_numpy()

plot.figure()
# plot.figure(figsize=(24,12))
im = plot.imshow(
    z,
    origin="lower",
    aspect="auto",
    cmap="viridis"
)

min = df["observers"].min()
max = df["observers"].max()

yticks = np.arange(start=0, stop=(max-min)/2+1, step=4)
ylabels = np.arange(start=min, stop=max+1, step=8)

plot.yticks(yticks, ylabels)

# plot.xlabel("Message Size")
plot.ylabel("Observers")
plot.xlabel("Access stride [B]")

plot.colorbar(im, label="Latency [us]")

tick_positions = np.where(x % 4096 == 0)[0]
tick_labels = x[tick_positions]
# tick_labels = [f"{int(i/1024)}" for i in x[tick_positions]]
# tick_labels = [fr"$2^{{{int(math.log(i,2))}}}$" for i in x[tick_positions]]
# plot.xlabel("Message size in KiB")

plot.xticks(tick_positions, tick_labels)

plot.title(f"Median Latency of {op} for different \naccess strides and numbers of observers.")


plot.tight_layout()
plot.savefig(f"all.pdf")