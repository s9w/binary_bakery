import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import os
from os.path import isfile, join
from matplotlib import ticker
import re

def get_times_from_txt(filename):
    ms_regex = re.compile(r'TotalMilliseconds\s?:\s?(\d*[,.]\d*)')
    with open(filename) as file:
        times = np.zeros(shape=(0))
        for line in file.readlines():
            match = ms_regex.match(line.strip())
            if match is None:
                continue
            ms_str = match.group(1).replace(",", ".")
            ms = float(ms_str)
            seconds = ms / 1000.0
            times = np.append(times, seconds)
        return times[1:]

data = np.loadtxt("compile_sizes.txt", dtype=int, delimiter=',', usecols=[0, 1], unpack=False)
data = data[data[:, 0].argsort()] # Sort by size

source_sizes_kb = data[:,0] / 1024
binary_sizes_kb = data[:,1] / 1024
binary_size_overhead = binary_sizes_kb - binary_sizes_kb[0] - source_sizes_kb

compile_times = [np.mean(get_times_from_txt("compile_time_{}.txt".format(ss))) for ss in data[:,0]]
compile_times_std = [np.std(get_times_from_txt("compile_time_{}.txt".format(ss))) for ss in data[:,0]]
compile_times_increases = compile_times - compile_times[0]

fig = plt.figure(figsize=(6, 3))
ax = fig.add_subplot(111)
y = compile_times - compile_times[0]
ax.plot(source_sizes_kb, binary_size_overhead, "o--")
ax.set_xscale('log')
ax.xaxis.set_major_formatter(ticker.StrMethodFormatter("{x:8.0f}"))
ax.set_ylim(0, 2.5)
ax.set_xlabel("Memory size of include [KB]")
ax.set_ylabel("Binary size overhead [KB]")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
fig.tight_layout()
ax.grid(axis='x')
fig.savefig("binary_sizes.png")

fig = plt.figure(figsize=(6, 3))
ax = fig.add_subplot(111)
ax.errorbar(source_sizes_kb, compile_times_increases, fmt="o--", yerr=compile_times_std)
ax.set_xscale('log')
ax.xaxis.set_major_formatter(ticker.StrMethodFormatter("{x:8.0f}"))
ax.set_xlabel("Memory size of include [KB]")
ax.set_ylabel("Compile time increase [seconds]")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.set_ylim(0, )
ax.grid()
fig.tight_layout()
fig.savefig("compile_times.png")