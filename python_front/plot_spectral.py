import matplotlib.pyplot as plt
import numpy as np

inputFile = r'fft_out/spectral1_hann_1.txt'
xArr = []
yArr = []
with open(inputFile) as data:
    for line in data:
        x,y = line.split(',')
        xArr.append(float(x))
        yArr.append(float(y.strip('\n')))

plt.xlabel('Frequency') 
plt.ylabel('Amplitude') 
plt.xticks(np.arange(0, 23000, 1000))
plt.plot(xArr, yArr, color=('#ffb7c5'))
plt.show() 

"""
plt.xlabel('Frequency') 
plt.ylabel('Amplitude') 
plt.xticks(np.arange(0, 23000, 1000))
plt.plot(xArr, yArr, color=('#ffb7c5'))
plt.show() 
"""
"""
npTimeArr = df['Time'].to_numpy()
npFrequencyArr = df['Frequency'].to_numpy()
npAmplitudeColorArr = df['AmplitudeColor'].to_numpy()
plt.xticks(np.arange(0, 4, 0.1))
plt.yticks(np.arange(0, 22000, 500))
plt.scatter(npTimeArr, npFrequencyArr, s=1, c=npAmplitudeColorArr)
plt.show() 
"""