import numpy as np
import matplotlib.pyplot as plt
data = []
ach = "3"
skf = "0"
stimulus = "0.400000"
float_stimulus = float(stimulus)
first_stim_time = 0.1
if float_stimulus == 999:    
   frequency = 0
else:
    frequency = 1/float_stimulus
filename = "../Code/Output/_" + stimulus + "_" + skf + "_" + ach + "/test_cells_" + stimulus + "_0_" + ach + ".txt"
with open(filename, "r") as file:
    for line in file:
        p = line.split("\t")
        data.append(p)
data = np.array(data).astype(float)
data = data.T



#Stimulus input
stimulus_array = []
stim = first_stim_time
for i in range(len(data[0])):
    if data[0][i] <= stim + 0.0005 and data[0][i] >= stim - 0.0005:
        stimulus_array.append(1)
        stim = stim + float_stimulus
    else:
        stimulus_array.append(0)
    i=+1

fig, (plot1, plot2) = plt.subplots(2, 1, figsize=(16, 10), gridspec_kw={'height_ratios': [1, 5]}, sharex = True)
plt.rcParams['font.size'] = 25

title = "{:.1f} Hz Stimulus".format(frequency)
plot1.plot(data[0], stimulus_array, color = 'black', linewidth = 1)
plot1.set_title(title, loc="left", size = 25)
plot1.axes.get_yaxis().set_visible(False)
plot2.plot(data[0], data[1])
plot2.set_title("N Cell with ach concentration " + ach +" and {:.1f} Hz stimulus".format(frequency), size = 25, loc = 'left')
plot2.set_xlabel("Time (s)", size = 20)
plot2.set_ylabel("Voltage (V)", size = 15)
#plot2.set_xticks(size = 10)
#plot2.set_yticks(size = 15)
plot2.axes.get_yaxis().set_visible(False)


plt.savefig("N_cells_ach/"+stimulus+"_" +skf+"_" + ach + ".png", bbox_inches='tight')
plt.show()