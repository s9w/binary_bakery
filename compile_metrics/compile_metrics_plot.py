import matplotlib.pyplot as plt
import numpy as np
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

source_sizes = np.loadtxt("generated_data/compile_sizes_none.txt", dtype=int, delimiter=',', usecols=[0], unpack=False)
sort_indices = source_sizes.argsort()
source_sizes = source_sizes[sort_indices]
source_sizes_kb = source_sizes / 1024

fig = plt.figure(figsize=(6, 3))
ax = fig.add_subplot(111)

baseline_target_size_kb = np.loadtxt("generated_data/compile_sizes_zero.txt", dtype=int, delimiter=',', usecols=[1], unpack=False).item() / 1024

target_sizes_kb = {}
for compression in ["none", "LZ4", "zstd"]:
    loaded = np.loadtxt("generated_data/compile_sizes_{}.txt".format(compression), dtype=int, delimiter=',', usecols=[1], unpack=False)
    size_kb = loaded[sort_indices] / 1024
    ax.plot(source_sizes_kb, size_kb - baseline_target_size_kb, ".-", label=compression)

ax.set_xscale('log')
ax.set_yscale('log')
ax.xaxis.set_major_formatter(ticker.StrMethodFormatter("{x:8.0f}"))
ax.yaxis.set_major_formatter(ticker.StrMethodFormatter("{x:8.0f}"))
ax.set_xlabel("Payload size [KB]")
ax.set_ylabel("Binary size increase [KB]")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.grid()
ax.legend()
fig.tight_layout()
fig.savefig("../readme/binary_size_overhead.png")




fig = plt.figure(figsize=(6, 3))
ax = fig.add_subplot(111)

compile_time_baseline = np.mean(get_times_from_txt("generated_data/compile_times_zero.txt"))
for compression in ["none", "LZ4", "zstd"]:
    times = []
    for source_size in source_sizes:
        times.append(np.mean(get_times_from_txt("generated_data/compile_times_{}_{}.txt".format(compression, source_size))))
    ax.plot(source_sizes_kb, times - compile_time_baseline, ".-", label=compression)

ax.set_xscale('log')
ax.xaxis.set_major_formatter(ticker.StrMethodFormatter("{x:8.0f}"))
ax.set_xlabel("Payload size [KB]")
ax.set_ylabel("Compile time increase [seconds]")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.grid()
ax.legend()
fig.tight_layout()
fig.savefig("../readme/compile_times.png")
