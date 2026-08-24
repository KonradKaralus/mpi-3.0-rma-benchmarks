#!/usr/bin/python

import polars as pl
import matplotlib.pyplot as plot
import sys

import shutil, os

req = ["out.csv", "baseline_stream.csv"]

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

with open("out.csv","r") as f:
    for line in f.readlines():
        if line.startswith("___"):
            lines.append(line.removeprefix("___").replace("\n",""))


csv = "op,stride,compiler,size,metric,time,metric/t,total_metric/t,task,rank,t_bench,flops\n"

for i in range(0,len(lines),2):
    first = lines[i]
    second = lines[i+1]

    out = ""


    rev = False

    if first.startswith("dgemm") or first.startswith("stream"):
        rev = True

    if not rev:
        second = second.replace("stream: ","").replace("dgemm: ","")

        out += first + "," + second + "\n"

    else:
        first = first.replace("stream: ","").replace("dgemm: ","")

        out += second + "," + first + "\n"

    csv += out


with open("parsed.csv", "w") as f:
    f.write(csv)
    
nl = "\n"

for op in ["cas", "fop"]:
    df_dgemm = pl.read_csv("parsed.csv").filter((pl.col("op") == op)  & (pl.col("task") == "dgemm")).filter(pl.col("compiler") == "ompi")
    df_stream = pl.read_csv("parsed.csv").filter((pl.col("op") == op)  & (pl.col("task") == "stream")).filter(pl.col("compiler") == "ompi")
    
    df_dgemm=df_dgemm.with_columns((pl.col("total_metric/t")/1000.0).alias("tkops"))
    df_stream=df_stream.with_columns((pl.col("total_metric/t")/1000.0).alias("tkops"))

    plot.scatter(x=df_dgemm["size"],y=df_dgemm["tkops"], color="blue", marker="o", label="dgemm")
    plot.scatter(x=df_stream["size"],y=df_stream["tkops"], color="green", marker=",", label="stream")
    
    cas = f"\\texttt{{MPI_Compare_and_swap}}" if tex else "MPI_Compare_and_swap"
    fop = f"\\texttt{{MPI_Fetch_and_op}}" if tex else "MPI_Fetch_and_op"
    
    title = f'Accumulated operation throughput \nof {cas if op == "cas" else fop} \nfor different numbers of observers'

    plot.title(title)

    plot.xlabel("Number of Observers")

    plot.yscale("log")

    plot.ylabel("Accumulated operations [KOps/s]")
    plot.legend()
    
    plot.grid(True, which="both", linestyle="--", alpha=0.5)


    plot.tight_layout()
    
    plot.savefig(f"{op}.pdf")
    plot.savefig(f"{op}.png", dpi=300)

    plot.clf()

for op in ["get", "get_acc"]:
    for stride in [True, False]:
        if stride:
            df_dgemm = pl.read_csv("parsed.csv").filter((pl.col("op") == op)  & (pl.col("task") == "dgemm") & pl.col("stride")).filter(pl.col("compiler") == "ompi")
            df_stream = pl.read_csv("parsed.csv").filter((pl.col("op") == op)  & (pl.col("task") == "stream") & pl.col("stride")).filter(pl.col("compiler") == "ompi")

        else:
            df_dgemm = pl.read_csv("parsed.csv").filter((pl.col("op") == op)  & (pl.col("task") == "dgemm") & ~pl.col("stride")).filter(pl.col("compiler") == "ompi")
            df_stream = pl.read_csv("parsed.csv").filter((pl.col("op") == op)  & (pl.col("task") == "stream") & ~pl.col("stride")).filter(pl.col("compiler") == "ompi")

        df_dgemm = df_dgemm.with_columns((pl.col("total_metric/t") / 1000000.0).alias("mtotal_m/t"))
        df_stream = df_stream.with_columns((pl.col("total_metric/t") / 1000000.0).alias("mtotal_m/t"))

        plot.scatter(x=df_dgemm["size"],y=df_dgemm["mtotal_m/t"], color="blue", label="dgemm", marker="o")
        plot.scatter(x=df_stream["size"],y=df_stream["mtotal_m/t"], color="green",label="stream",marker=",")
        
        get = f"\\texttt{{MPI_Get}}" if tex else "MPI_Get"
        gac = f"\\texttt{{MPI_Get_accumulate}}" if tex else "MPI_Get_accumulate"

        title = f"Accumulated observer throughput of \n {get if op == 'get' else gac} {'with' if stride else 'without'} stride \n for different numbers of observers"
        plot.xlabel("Number of Observers")
        plot.ylabel("Accumulated throughput [MB/s]")
        plot.title(title)
        plot.legend()
        
        plot.grid(True, which="both", linestyle="--", alpha=0.5)
        
        plot.ylim(bottom=0)

        plot.tight_layout()

        # l[i].ylim(bottom=0)

        plot.savefig(f"{op}_{stride}.pdf")
        plot.savefig(f"{op}_{stride}.png", dpi=300)

        plot.clf()

        if op == "get" and stride == False:
            plot.scatter(x=df_stream["size"],y=df_stream["mtotal_m/t"], color="green",label="total observer throughput",marker=",")
            plot.scatter(x=df_stream["size"],y=df_stream["flops"], color="blue",label="stream throughput",marker="o")

            get = "\\texttt{{MPI_Get}}" if tex else "MPI_Get"
            
            title = f"Accumulated observer throughput of {get} \n without stride and stream benchmark \n for different numbers of observers"
            plot.xlabel("Number of Observers")
            plot.ylabel("Accumulated throughput [MB/s]")
            plot.title(title)
                        
            plot.grid(True, which="both", linestyle="--", alpha=0.5)
            
            plot.ylim(bottom=0)

            avg = 0
            num = 0
            with open("baseline_stream.csv","r") as f:
                for line in f.readlines():
                    avg += float(line.replace("\n","").split(",")[-1])
                    num += 1
            avg /= num

            plot.axhline(y=avg, color="red", label="base stream throughput")

            plot.legend()

            plot.tight_layout()

            plot.savefig(f"get_false_stream_annotated.pdf")
            plot.savefig(f"get_false_stream_annotated.png", dpi=300)

            plot.clf()