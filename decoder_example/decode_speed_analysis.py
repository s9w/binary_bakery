import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import ticker

sizes = np.loadtxt("data_lz4.txt", dtype=int, delimiter=',', usecols=0)
sizes_kb = sizes / 1024

lz4_mean, _, stb_mean, _ = np.loadtxt("data_lz4.txt", dtype=float, delimiter=',', usecols=[1,2,3,4], unpack=True)
lz4_relative = lz4_mean / stb_mean * 100.0

zstd_mean, _, stb_mean, _ = np.loadtxt("data_zstd.txt", dtype=float, delimiter=',', usecols=[1,2,3,4], unpack=True)
zstd_relative = zstd_mean / stb_mean * 100.0

uncompressed_mean, _, stb_mean, _ = np.loadtxt("data_uncompressed.txt", dtype=float, delimiter=',', usecols=[1,2,3,4], unpack=True)
uncompressed_relative = uncompressed_mean / stb_mean * 100.0

def setup_ax(fig, position_param, x_label, y_label, x_log=False, y_log=False, disable_top_right = False):
   ax = fig.add_subplot(position_param)
   ax.set_xlabel(x_label)
   ax.set_ylabel(y_label)
   if x_log == True:
      ax.set_xscale('log')
      ax.xaxis.set_major_formatter(ticker.StrMethodFormatter("{x:8.0f}"))
   if y_log == True:
      ax.set_yscale('log')
      ax.yaxis.set_major_formatter(ticker.StrMethodFormatter("{x:8.0f}"))
   if disable_top_right:
      ax.spines["top"].set_visible(False)
      ax.spines["right"].set_visible(False)
   return ax

fig = plt.figure(figsize=(6, 3))
ax = setup_ax(fig, 111, x_label="Payload size [KB]", y_label="Relative decode time [%]", x_log=True, disable_top_right=True)

ax.plot(sizes_kb, zstd_relative, "o--", label="zstd")
ax.plot(sizes_kb, uncompressed_relative, "o--", label="uncompressed")
ax.plot(sizes_kb, lz4_relative, "o--", label="LZ4")

ax.set_ylim(0, 110)
ax.grid()
ax.legend(loc="upper left")

fig.tight_layout()
fig.savefig("../readme/decode_speed_analysis.png")