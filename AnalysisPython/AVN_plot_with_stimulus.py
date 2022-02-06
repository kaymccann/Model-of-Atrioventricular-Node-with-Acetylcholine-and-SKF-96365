# -*- coding: utf-8 -*-
"""
Created on Tue May  4 20:49:37 2021

@author: nhphi
"""

# -*- coding: utf-8 -*-
"""
Created on Tue Oct 27 20:21:19 2020

@author: nhphi
"""

# Taking in output data from the AVN Initial Code and plotting it, for a specified cell
import numpy as np
import matplotlib.pyplot as plt

#parameters that might need changing
prefix = "_2.5_0_0"
stim_time = 0.001
first_stim_time = 0.1
stim_interval = 0.25
step = 0.01
frequency = '2.5 Hz'

colour1 = '#1F25FF'  # N blue
colour2 = 'black'  # AM
colour16 = '#FF1818'  # AN red
colour19 = '#1C751F'  # NH green
width = 1
letter_size = 30
low_lim = 0
high_lim = 5000
same_colour = 'black'

filename = "fast_cells" + prefix + ".txt"
fast_data = []
slow_data = []

with open(filename, 'r') as data:
    for line in data:
        p = line.split("\t")
        p.pop()
        fast_data.append(p)
fast_data = np.array(fast_data).astype(float)
fast_data = fast_data.T

filename = "slow_cells" + prefix + ".txt"
with open(filename, 'r') as data:
    for line in data:
        p = line.split("\t")
        p.pop()
        slow_data.append(p)
slow_data = np.array(slow_data).astype(float)
slow_data = slow_data.T

r = 4
fig, (plot0, plot1, plot3, plot2, plot4) = plt.subplots(5, 1, figsize=(16, 16), gridspec_kw={'height_ratios': [1, r, r, r, r]}, sharex=True)
plt.rcParams['font.size'] = 30

# SLOW PATHWAY
for i in range(0, 15):
    plot2.plot(slow_data[0], slow_data[i*5 + 1] - 0.01*i, color=colour2, linewidth=width)
for i in range(15, 40):
    plot2.plot(slow_data[0], slow_data[i*5 + 1] - 0.01*i, color=colour1, linewidth=width)

#plot2.set_xlim(low_lim, high_lim)
plot2.set_title("Slow Pathway", loc="left", size=letter_size)  # Slow Pathway
plot2.axes.get_yaxis().set_visible(False)
# plot2.set_xlabel("Time (ms)")

# ATRIAL MUSCLE
for i in range(0, 15):
    plot1.plot(fast_data[0], fast_data[i*5 + 1] - 0.01*i, color=colour2, linewidth=width)
#plot1.set_xlim(low_lim, high_lim)
plot1.set_title("Atrial Muscle", loc="left", size=letter_size)  # Atrial Muscle
plot1.axes.get_yaxis().set_visible(False)
# plot1.set_xlabel("Time (ms)")
plt.box(on=None)

# FAST PATHWAY
for i in range(15, 25):
    plot3.plot(fast_data[0], fast_data[i*5 + 1] - 0.01*i, color=colour2, linewidth=width)
for i in range(25, 40):
    plot3.plot(fast_data[0], fast_data[i*5 + 1] - 0.01*i, color=colour16, linewidth=width)
for i in range(40, 50):
    plot3.plot(fast_data[0], fast_data[i*5 + 1] - 0.01*i, color=colour1, linewidth=width)
plot3.set_title("Fast Pathway", loc="left", size=letter_size)  # Fast Pathway
plot3.axes.get_yaxis().set_visible(False)
#plot3.set_xlim(low_lim, high_lim)
# plot3.set_xlabel("Time (ms)")

# BUNDLE OF HIS
for i in range(50, 65):
    plot4.plot(fast_data[0], fast_data[i*5 + 1] - 0.01*i, color=colour19, linewidth=width)
plot4.set_title("Bundle of His", loc="left", size=letter_size)  # Bundle of His
plot4.axes.get_yaxis().set_visible(False)
#plot4.set_xlim(low_lim, high_lim)
plot4.set_xlabel("Time (s)", size=letter_size)


#Stimulus input
stimulus = []
stim = first_stim_time
for i in range(len(fast_data[0])):
    if fast_data[0][i] <= stim + 0.005 and fast_data[0][i] >= stim - 0.005:
        stimulus.append(1)
        stim = stim + stim_interval
    else:
        stimulus.append(0)
    i=+1
    
title = frequency + " Stimulus"
plot0.plot(fast_data[0], stimulus, color = '#F45213', linewidth = width)
plot0.set_title(title, loc="left", size=letter_size)  # Fast Pathway
plot0.axes.get_yaxis().set_visible(False)
     
plt.box(on=None)
plt.tight_layout(pad=0)
#plt.xlim(2.5, 3.2)
#plt.savefig("AVN_plot_with_stimulus"+prefix)
plt.show()   
