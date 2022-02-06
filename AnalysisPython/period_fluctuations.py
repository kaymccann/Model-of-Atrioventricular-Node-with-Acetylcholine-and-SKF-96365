#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This code will record how period of time when cell reaches threshold potential
fluctuates with time.

lines 11-83 are copied from 'conduction velocity.py'

@author: egle
"""
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

#calculating periods
def period_array (array):   
    p_array = []
    for i in range(len(array)-1):
        P = array[i+1] - array[i]
        P = np.round(P, 4)
        p_array.append(P)
    return p_array

p_firstam = period_array(firstam)
p_lastam = period_array(lastam)
p_middlefast = period_array(middlefast)
p_middleslow = period_array(middleslow)
p_startbund = period_array(startbund)
p_endbund = period_array(endbund)

fig, (ax1, ax2, ax3, ax4, ax5, ax6) = plt.subplots(6, 1, figsize=(12, 16), sharex = True)
ax1.scatter(firstam[1:],p_firstam, label="firstam")
ax2.scatter(lastam[1:],p_lastam, label="lastam")
ax3.scatter(middlefast[1:],p_middlefast, label="middlefast")
ax4.scatter(middleslow[1:],p_middleslow, label="middleslow")
ax5.scatter(startbund[1:],p_startbund, label="startbund")
ax6.scatter(endbund[1:],p_endbund, label="endbund")

#plt.yticks(ticks=[5, 4, 3, 2, 1], labels=strings)
#plt.grid(axis="y")
plt.show()
        
        