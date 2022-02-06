import numpy as np
import matplotlib.pyplot as plt

activationPoint = -0.03

data = []

ach = "5"
filename = "../Code/Output/_0.400000_0_" + ach + "/test_cells_0.400000_0_" + ach + ".txt"
with open(filename, "r") as file:
    for line in file:
        p = line.split("\t")
        data.append(p)
data = np.array(data).astype(float)
data_T = data.T

plt.plot(data_T[0], data_T[1])
plt.title("N Cell spontaneity with ach concentration " + ach)
plt.xlabel("Time (s)")
plt.ylabel("Voltage (V)")
#plt.savefig("N_cells_ach/" + ach + ".png")
plt.show()


activated_time = []
temp = []
start = []
end = []
APD_array = []
for i in data:
    if i[1] >= activationPoint:
        activated_time.append(i[0])
activated_time = np.sort(activated_time)
for i in activated_time:
    difference = i - (i+1)
if np.abs(difference) > 0.005:
    temp.append(i)
for i in range(0, len(temp)):
    if i % 2 == 0:
        start.append(temp[i])
else:
    end.append(temp[i])
#while len(start) != len(end):
    #if len(end) + 1 == len(start):
       # del end[-1]
   # else:
      #  print("check the plot")
for i in range(0, len(start)):
    APD = end[i] - start [i]
    APD_array.append(APD)
