#!/usr/bin/python

import argparse
import subprocess
import os

valid_actions = ["build","clean","results"]

parser = argparse.ArgumentParser(description="Build benchmarks.")

parser.add_argument("action", help="Either build, clean, results. Define what to do with the selected benchmarks.")
parser.add_argument("benchmarks", help="Comma separated list of benchmarks to build. Passing all builds all benchmarks.")
parser.add_argument("-e", "--exclude", help="Comma separated list of benchmarks to exclude.")
parser.add_argument("-t", "--tex", help="Use tex for results.",action="store_true")

args = parser.parse_args()

def build_benchmark(name:str):
    subprocess.call(f"cd {name} && bash build.sh", shell=True)
def clean_benchmark(name:str):
    subprocess.call(f"cd {name} && bash clean.sh", shell=True)
def results_benchmark(name:str):
    if "results" not in os.listdir(name):
        print(f"Benchmark {name} does not contain a results directory.")
        return
    res_scripts = [f for f in os.listdir(f"{name}/results/") if "results" in f]
    for script in res_scripts:
        print(f">>> Executing script {script}")
        subprocess.call(f"cd {name}/results && python3 {script} {'tex' if args.tex else ''}", shell=True)

def verify_benchmark(name:str) -> bool:
    clean = name.replace("/", "")
    if not os.path.isdir(clean):
        return False
    if clean not in os.listdir("."):
        return False
    if "build.sh" not in os.listdir(clean):
        return False

    return True


if args.action not in valid_actions:
    print(f"Specified action {args.action} does not exist.")
    exit(1)

benchmarks:str = args.benchmarks

if benchmarks == "all":
    selected = [dir for dir in os.listdir(".") if verify_benchmark(dir)]
else:
    selected = benchmarks.split(",")

excluded = []

if args.exclude is not None:
    excluded = [b.replace("/","") for b in args.exclude.split(",")]
    for benchmark in excluded:
        if not verify_benchmark(benchmark):
            print(f">>> Excluded benchmark {benchmark} does either not exist or the directory does not contain a build.sh file")
        

for benchmark in selected:
    if verify_benchmark(benchmark) == False:
        print(f">>> Skipping {benchmark}. It either does not exist or the directory does not contain a build.sh file")
        continue
    if benchmark.replace("/","") in excluded:
        print(f">>> Skipping {benchmark}, as it was excluded")
        continue

    if args.action == "build":
        print(f">>> Building {benchmark}")
        build_benchmark(benchmark)
    elif args.action == "clean":
        print(f">>> Cleaning {benchmark}")
        clean_benchmark(benchmark)
    elif args.action == "results":
        print(f">>> Building results of {benchmark}")
        results_benchmark(benchmark)


