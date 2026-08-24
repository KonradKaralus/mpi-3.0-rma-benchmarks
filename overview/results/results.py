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

plot.grid(True, which="both", linestyle="--", alpha=0.5)

df = pl.read_csv("output.csv").with_columns((pl.col("lat")*1000.0).alias("lat_ms"))

df= df.with_columns((pl.col("observers")*pl.col("size")*10000/pl.col("lat")).alias("tp"))


df = df.group_by("comp","observers","op","size").agg(pl.col("lat_ms").median().alias("median"),pl.col("tp").median().alias("tp_median") ,pl.col("lat_ms").max().alias("max"), pl.col("lat_ms").min().alias("min"))
df = df.with_columns((pl.col("max") - pl.col("median")).alias("ymax"),(pl.col("median") - pl.col("min")).alias("ymin"))

df = df.sort("observers","size")

for comp in ["ompi", "impi"]:
    for op in ["put","get"]:
        df_f = df.filter(pl.col("comp")==comp).filter(pl.col("op") == op)

        pivot = df_f.pivot(on="size", index="observers", values="median")

        x = np.array(pivot.columns[1:], dtype=int)
        y = pivot["observers"].to_list()
        z = pivot.select(pl.exclude("observers")).to_numpy()
        
        from matplotlib.colors import LogNorm

        plot.figure(figsize=(8,6))
        im = plot.imshow(
            z,
            origin="lower",
            aspect="auto",
            cmap="viridis",
            norm=LogNorm()
        )

        plot.xlabel("Message Size")
        plot.ylabel("Observers")
        plot.colorbar(im, label="Latency [ms]")

        tick_positions = np.where(x % 1024 == 0)[0]
        tick_labels = [f"{int(i/1024)}" for i in x[tick_positions]]
        plot.xlabel("Message size in KiB")

        plot.xticks(tick_positions, tick_labels)
        
        tick_labels_y = np.arange(start=1,stop=len(y),step=4)
        tick_positions_y = tick_labels_y - 1
        
        plot.yticks(ticks=tick_positions_y, labels=tick_labels_y*2-1)
        
        get = f"\\texttt{{MPI_Get}}" if tex else "MPI_Get"
        put = f"\\texttt{{MPI_Put}}" if tex else "MPI_Put"
        plot.title(f"Median latency of {get if op=='get' else put} for different message \nsizes and numbers of observers. ({'OpenMPI' if comp == 'ompi' else 'IntelMPI'})")
        

        plot.tight_layout()
        plot.savefig(f"{comp}_{op}_lat.pdf")

        plot.clf()

        fig, ax = plot.subplots()
        CS = ax.contour(x, y, z, levels=16)
        ax.clabel(CS, fontsize=10)
        ax.set_title(f"Contour lines for median latency [ms] of \n{get if op=='get' else put} for different message sizes and \n numbers of observers. ({'OpenMPI' if comp == 'ompi' else 'IntelMPI'})")


        plot.xlabel("Message size in KiB")
        ax.set_xticks(np.arange(start=1, stop=17)*1024)
        ax.set_xticklabels(tick_labels)

        plot.ylabel("Observers")
        ax.set_yticks(tick_positions_y*2+1)
        ax.set_yticklabels(tick_labels_y*2-1)

        plot.grid(True, which="both", linestyle="--", alpha=0.5)
        
        fig.tight_layout()

        plot.savefig(f"{comp}_{op}_contour.pdf")

        plot.clf()
        # throughput
        
        df_f = df.filter(pl.col("comp")==comp).filter(pl.col("op") == op)
        
        df_f = df_f.with_columns((pl.col("tp_median")/1000000).alias("tp_median"))

        pivot = df_f.pivot(on="size", index="observers", values="tp_median")

        x = np.array(pivot.columns[1:], dtype=int)
        y = pivot["observers"].to_list()
        z = pivot.select(pl.exclude("observers")).to_numpy()
        
        from matplotlib.colors import LogNorm

        plot.figure(figsize=(8,6))
        im = plot.imshow(
            z,
            origin="lower",
            aspect="auto",
            cmap="viridis",
            # norm=LogNorm()
        )

        plot.xlabel("Message Size")
        plot.ylabel("Observers")
        plot.colorbar(im, label="Throughput [MB/s]")

        tick_positions = np.where(x % 1024 == 0)[0]
        # tick_labels = x[tick_positions]
        tick_labels = [f"{int(i/1024)}" for i in x[tick_positions]]
        # tick_labels = [fr"$2^{{{int(math.log(i,2))}}}$" for i in x[tick_positions]]
        plot.xlabel("Message size in KiB")

        plot.xticks(tick_positions, tick_labels)
        
        
        plot.yticks(ticks=tick_positions_y, labels=tick_labels_y*2-1)
        
        plot.title(f"Median throughput of {get if op=='get' else put} for different message\n sizes and numbers of observers. ({'OpenMPI' if comp == 'ompi' else 'IntelMPI'})")
        

        plot.tight_layout()
        plot.savefig(f"{comp}_{op}_tp.pdf")