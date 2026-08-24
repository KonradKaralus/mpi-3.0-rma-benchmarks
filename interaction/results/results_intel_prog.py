#!/usr/bin/python

import polars as pl
import matplotlib.pyplot as plot
import sys

import shutil, os

req = ["out_intel_prog.csv"]

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
    

lines:list[str]=[]

with open("out_intel_prog.csv","r") as f:
    for line in f.readlines():
        if line.startswith("___"):
            lines.append(line.removeprefix("___").replace("\n",""))


csv = "op,stride,compiler,size,metric,time,metric/t,total_metric/t,bench,rank,t_bench,bench_metric,every_n\n"

for i in range(0,len(lines),2):
    first = lines[i]
    second = lines[i+1]

    out = ""

    rev = False

    if first.startswith("dgemm") or first.startswith("stream"):
        rev = True

    if not rev:
        second = second.replace("stream: ","").replace("dgemm: ","")

        out += first + "," + second

    else:
        first = first.replace("stream: ","").replace("dgemm: ","")

        out += second + "," + first

    csv += out + "\n"

with open("parsed_intel.csv", "w") as f:
    f.write(csv)


df_intel = pl.read_csv("parsed_intel.csv")

df_intel = df_intel.with_columns((pl.col("total_metric/t") / 1000000.0).alias("mtotal_m/t"))
df_intel = df_intel.group_by("every_n").agg(pl.col("bench_metric").median().alias("bmedian"), pl.col("bench_metric").max().alias("bmax"), pl.col("bench_metric").min().alias("bmin"),pl.col("mtotal_m/t").median().alias("omedian"), pl.col("mtotal_m/t").max().alias("omax"), pl.col("mtotal_m/t").min().alias("omin"))
df_intel = df_intel.with_columns((pl.col("bmax") - pl.col("bmedian")).alias("bymax"),(pl.col("bmedian") - pl.col("bmin")).alias("bymin"))
df_intel = df_intel.with_columns((pl.col("omax") - pl.col("omedian")).alias("oymax"),(pl.col("omedian") - pl.col("omin")).alias("oymin"))

byerr = [df_intel["bymin"], df_intel["bymax"]]
oyerr = [df_intel["oymin"], df_intel["oymax"]]

test = "\\texttt{{MPI_Test}}" if tex else "MPI_Test"

plot.errorbar(x=df_intel["every_n"],y=df_intel["bmedian"],yerr=byerr,fmt="o", linestyle="none",ms=3.0,)
plot.ylim(bottom=0.0)
plot.xlabel(f"{test} called every nth iteration")
plot.ylabel("Throughput of stream (copy) [MB/s]")
plot.xscale("log", base=2)

plot.grid(True, which="both", linestyle="--", alpha=0.5)

plot.title(f"Performance of stream benchmark calling {test}")

plot.tight_layout()

plot.savefig(f"intel_prog_stream.pdf")
plot.savefig(f"intel_prog_stream.png", dpi=300)
plot.clf()

plot.errorbar(x=df_intel["every_n"],y=df_intel["omedian"],yerr=oyerr,fmt="o", linestyle="none",ms=3.0,)
plot.ylim(bottom=0.0,top=13000)
plot.xlabel(f"{test} called every nth iteration")
plot.ylabel("Accumulated throughput [MB/s]")
plot.xscale("log", base=2)

plot.grid(True, which="both", linestyle="--", alpha=0.5)

plot.title(f"Accumulated observer throughput with \n stream benchmark calling {test}")

plot.tight_layout()

plot.savefig(f"intel_prog_obs.pdf")
plot.savefig(f"intel_prog_obs.png", dpi=300)
plot.clf()