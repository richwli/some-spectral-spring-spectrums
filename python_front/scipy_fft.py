from scipy.fftpack import fft
from scipy.io import wavfile # get the api
from scipy import signal
import math
import numpy as np
import matplotlib.pyplot as plt
#0.4996518032746323
path = r"../STFT/freq440amp0d5_2048.wav"
fs, data = wavfile.read(path) # load the data
bitDepth = pow(2,15)
maxVolumeRefDbfs = bitDepth*2
hannWindow = signal.windows.hann(2048)
for i in range(len(data)):
    #hannWindow = 0.5 * (1-math.cos(2*3.14159*(i) / (2048-1)))
    #print(data[i]*hannWindow)
    data[i] = 2*data[i]*hannWindow[i]
    #print(hannWindow)
fftComplexArr = fft(data/bitDepth)
length = data.shape[0] / fs
#counter = 0
for index in range(len(fftComplexArr)):
    fk=fftComplexArr[index]
    magnitude = np.absolute(fk)
    amplitude = 2*magnitude/2048
    dbfs = 20*math.log10(magnitude);
    print(index,': ', dbfs)
    #counter+=1

time = np.linspace(0., length, data.shape[0])
plt.plot(time, data, label="Left channel")
plt.legend()
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")
plt.show()
