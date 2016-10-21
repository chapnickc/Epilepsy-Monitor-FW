import numpy as np
import matplotlib.pyplot as plt

from filters import *

with open('./recording_20161014_191716.txt') as f:
    lines = f.readlines()

x = np.array([])
for line in lines:
    x = np.append(x, float(line.strip()))


x = x - np.mean(x)
x = x/max(abs(x))



Fs = 125
T = 1/Fs
n = len(x)
t = np.linspace(0, n*T, n)

# x_o = x
# x = x_o


lowcut = 0.50
highcut = 5.00
order = 5


x_bp = butter_bandpass_filter(x, lowcut, highcut, Fs, order)


Xf = np.fft.fft(x_bp)
f = np.fft.fftfreq(x_bp.size, T)
idx = np.argsort(f)

ix = abs(Xf).argmax()
abs(Xf[ix])
f[ix]

plt.plot(f[idx], abs(Xf[idx]))
plt.show()


plt.plot(t, x_bp)
plt.show()





x_bp = x_bp[(t > 20) & (t < 100)]
t = t[(20 < t) & (t < 100)]


plt.plot(t, x, 'r')
plt.plot(t, x_bp, 'b')
plt.show()



plt.plot(peak_sign)
plt.show()



offset = 10
peaks = np.zeros(len(x_bp))
peak_sign = np.sign(x_bp)
for i in range(int(offset), int(len(x_bp) - offset)):
    if peak_sign[i] == 1 and peak_sign[i + 1] < 1:
        peaks[i] = 1

plt.plot(peaks)
plt.show()
peak_values = x_bp[peaks != 0]

t_pk = t[peaks != 0]
t_pk


plt.plot(peak_values)
plt.show()


        for j in range(1, int(offset)):
            if peak_sign[i - j] < 1 or peak_sign[i + j] > -1:
            #if peak_sign[i-j]==-1 or peak_sign[i+j]==1:
                peaks[i] = 0
                break


