import numpy as np
import matplotlib
import matplotlib.pyplot as plt

def read_data(filename):
    """
    Reads the serial data recorded in a txt file
    and returns a numpy array of the signal
    """
    data = []
    with open(filename) as f:
        for line in f:
            try:
                data.append(float(line.strip()))
            except ValueError:
                pass
    return np.asarray(data)


def find_peak_ixs(sample, m = 2, dt = 1):
    """
    Returns the indicies of detected peak values
    """
    ixs = []
    for i in range(1, len(sample) -1):
        peak_detected = (sample[i] - sample[i-dt] > m) & (sample[i] - sample[i+dt] > m)
        if peak_detected:
            ixs.append(i)
    return ixs

def calc_bpm(peak_ixs, N = 100, T= 0.050):
    """
    T is sampling interval, N is number of samples
    """
    t = np.linspace(0, N*T, N)
    tp = t[peak_ixs]

    tp_even = []
    for i in range(len(peak_ixs)):
        # Look only at peaks with even indicies
        if i % 2 == 0:
            tp_even.append(tp[i])

    ds = []
    for ix in range(len(tp_even)-1):
       d = tp_even[ix+1] - tp_even[ix]
       ds.append(d)

    # calculate average distance between peaks
    avg_d = sum(ds)/len(ds)

    # (1 beat/avg_interval sec)*(60 sec/1 min) = bpm
    bpm = (1/avg_d) * 60         # ~= 78.33

    return bpm




#from main import *
s = read_data('./aug3_IR_sample.txt')
#s = s[1400:2000]

s = s[1400:1500]
N = len(s)
T = 0.050
t = np.linspace(0.00, N*T, N)

plt.plot(t, s)
plt.show()

dt = 2
m = 3

ixs = []
for i in range(dt, N-dt):
    if (s[i] - s[i-dt] > m) and (s[i] - s[i+dt] > m):
        ixs.append(i)

tp = [t[i] for i in ixs]
sp = [s[i] for i in ixs]


plt.plot(t, s, 'b')
plt.plot(tp, sp, 'ro')
plt.show()



ixs = find_peak_ixs(s)

T = 0.050
bpm = calc_bpm(ixs, N = len(s), T = T)
print(bpm)


