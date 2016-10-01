import numpy as np
import matplotlib.pyplot as plt

from filters import *

with open('./recording_20160910_223912.txt') as f:
    lines = f.readlines()

x = np.array([])
for line in lines:
    x = np.append(x, float(line.strip()))


T = 0.050
fs = 1/T
lowcut = 0.40
highcut = 4.00
order = 5

x_bp = butter_bandpass_filter(x, lowcut, highcut, fs, order)

plt.plot(x)
plt.show()

plt.plot(x_bp)
plt.show()





