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

iterations = 100

tex = False
if len(sys.argv) > 1:
    if sys.argv[1] == "tex":
        plot.rcParams["text.usetex"] = True
        plot.rcParams["font.family"] = "serif"
        plot.rcParams["font.serif"] = ["Computer Modern Roman"]
        plot.rcParams["font.size"] = 14
        tex = True

df = pl.read_csv("output.csv").with_columns((pl.col("time")*1000.0*1000.0/iterations).alias("time_us"))
df_get_false = df.filter((pl.col("op") == "get") & ~(pl.col("stride")))
df_get_true = df.filter((pl.col("op") == "get") & (pl.col("stride")))
df_put_false = df.filter((pl.col("op") == "put") & ~(pl.col("stride")))
df_put_true = df.filter((pl.col("op") == "put") & (pl.col("stride")))

x = np.arange(4)

width = 0.1

fig, ax = plot.subplots()

ax.bar(x - 6*width/4, df_get_false["time_us"], width, label="unstrided get", color="blue")
ax.bar(x - 2*width/4, df_get_true["time_us"], width, label="strided get", color="green")
ax.bar(x + 2*width/4, df_put_false["time_us"], width, label="unstrided put", color="orange")
ax.bar(x + 6*width/4, df_put_true["time_us"], width, label="strided put", color="black")

labels = []
for row in df_put_true.iter_rows(named=True):
    labels.append(f'{row["source"]} - {row["target"]}')

# plot.ylim(bottom=10)
plot.ylabel("Latency [us]")
plot.xlabel("Memory type (H: Host, D: Device)")
plot.title("Latency of strided and unstrided \ntransfers (4/8Kib) for different memory types")
plot.xticks(ticks=x,labels=labels)
plot.yscale("log")
plot.grid(True, which="both", linestyle="--", alpha=0.5)

fig.legend(loc="upper right",bbox_to_anchor=(0.965, 0.75))

plot.tight_layout()


plot.savefig("plot.pdf")
plot.savefig("plot.png", dpi=300)
