import numpy as np
import matplotlib.pyplot as plt

prefix = "_2.5_0_0"
fastfilename = "fast_cells" + prefix + ".txt"
slowfilename = "slow_cells" + prefix + ".txt"
pulse_start = 0.1
pulse_interval = 0.4
time_end = 5.5
activationPoint = -30e-3
pulses = int(np.floor((time_end - pulse_start) / pulse_interval))
time_step = 5e-6
fastcelldata = []
slowcelldata = []
activations = []

with open(fastfilename, "r") as data:
    for line in data:
        p = line.split("\t")
        p.pop()
        fastcelldata.append(p)
fastcelldata = np.array(fastcelldata).astype(float)

with open(slowfilename, "r") as data:
    for line in data:
        p = line.split("\t")
        p.pop()
        slowcelldata.append(p)
slowcelldata = np.array(fastcelldata).astype(float)
firstam = []
lastam = []
middlefast = []
middleslow = []
startbund = []
endbund = []

for i in fastcelldata:
    if i[1] >= activationPoint:
        firstam.append(i[0])
    if i[75] >= activationPoint:
        lastam.append(i[0])
    if i[175] >= activationPoint:
        middlefast.append(i[0])
    if i[250] >= activationPoint:
        startbund.append(i[0])
    if i[325] >= activationPoint:
        endbund.append(i[0])

for i in slowcelldata:
    if i[150] >= activationPoint:
        middleslow.append(i[0])


def remove_batch(array):
    array = np.sort(array)
    array = array.tolist()
    temp = []
    for i in range(1, len(array)):
        if array[i] <= array[i - 1] + 0.02:
            temp.append(array[i])
    for i in temp:
        if i in array:
            array.remove(i)
    return array

firstam = remove_batch(firstam)
lastam = remove_batch(lastam)
middlefast = remove_batch(middlefast)
middleslow = remove_batch(middleslow)
startbund = remove_batch(startbund)
endbund = remove_batch(endbund)

strings = ["Last AM", "Middle Slow", "Middle Fast", "1st Bundle", "Last Bundle"]

indexes = [1, 2, 3, 4, 5, 6]

fig, ax1 = plt.subplots(figsize=(12, 6))
#ax1.scatter(firstam, [6] * len(firstam), label="firstam")
ax1.scatter(lastam, [5] * len(lastam), label="lastam")
ax1.scatter(middlefast, [4] * len(middlefast), label="middlefast")
ax1.scatter(middleslow, [3] * len(middleslow), label="endfast")
ax1.scatter(startbund, [2] * len(startbund), label="startbund")
ax1.scatter(endbund, [1] * len(endbund), label="endbund")
ax1.set_xlim([2.5, 3.2])
plt.yticks(ticks=[5, 4, 3, 2, 1], labels=strings)
plt.grid(axis="y")
plt.show()
