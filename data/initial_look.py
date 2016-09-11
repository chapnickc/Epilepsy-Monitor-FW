import numpy as np
import matplotlib.pyplot as plt

with open('./recording_20160910_223912.txt') as f:
    lines = f.readlines()

rec1 = np.array([])
for line in lines:
    rec1 = np.append(rec1, float(line.strip()))


plt.plot(rec1)
plt.show()



