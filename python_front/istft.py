import numpy as np
import librosa
import matplotlib.pyplot as plt
import pandas as pd
import librosa.display
import soundfile as sf

filename = r'Spice and Wolf - You Are An Ass_complex_decimation_8_stft_w_1024_detail_1'
inputFile = f'fft_out/{filename}.csv'
# testWavFile = r'../STFT_Anti-Alias_Filter/OWOGH_mono.wav'
# data, sr = librosa.load(testWavFile, sr=None)
decimationFactor = int(inputFile.split("_decimation_")[1].split("_stft_w_")[0])
windowSize = int(inputFile.split("_stft_w_")[1].split("_detail_")[0])/decimationFactor
df = pd.read_csv(inputFile, header=None)

matrixOfComplex = []
# windowSize = 1024
hopsize = int(windowSize/2)
print('window/hop',windowSize,hopsize)
# print('gimme rows: ',df.T)
for index, row in df.iterrows():
    vectorOfComplex = []
    # print('row: ', row)
    for complexString in row:
        realString, imagString = complexString.split('+')
        complexNum = complex(float(realString))+complex(imagString)
        # print(complex(float(realString)),complex(imagString))
        vectorOfComplex.append(complexNum)
        # print('VEC:', vectorOfComplex)
        #print(vectorOfComplex)
    matrixOfComplex.append(vectorOfComplex)

frequencyToTimeMatrix = np.array(matrixOfComplex)
# print(matrixOfComplex)
transformedDataMatrix = np.flip(np.rot90(frequencyToTimeMatrix,k=3),1) 
# transformedDataMatrix = np.flip(df.transpose(),1) 
#pain = librosa.istft(transformedDataMatrix, n_fft=windowSize, hop_length=hopsize)
# print("why is this broken: ",frequencyToTimeMatrix[-1][-3])

# control = librosa.stft(data,n_fft=int(windowSize), hop_length=int(hopsize), center=False)
#iControl = librosa.istft(control, n_fft=int(windowSize), hop_length=int(hopsize))

idk = librosa.istft(transformedDataMatrix, n_fft=int(windowSize), hop_length=int(hopsize), center=None)
#print("control: ", len(control), len(control[0]))   
# print("data: ", transformedDataMatrix)
# sf.write('istft.wav', pain, 44100,'PCM_16')
sf.write(f'{filename}.wav', idk, samplerate=int(44100/decimationFactor),subtype='PCM_16')
print('**************')
"""
for i in range(len(control)):
    print("c:",control[0])
    print("d:",transformedDataMatrix[0])
"""
# print("data: ", frequencyToTimeMatrix)
#fig, ax = plt.subplots(nrows=3, sharex=True)
#librosa.display.waveshow(pain, sr=sr, ax=ax[0])
#plt.show()
#plt.plot(testWav)
#plt.show()
#plt.plot(idk)
#plt.show()

"""
    for complexString in row:
        for complexNumber in complexNumberStrings:
            pain = complexNumber.split('+')
            real, imag= float(pain[0]),pain[1]
            print(real,imag)
    """
"""
rng = np.random.default_rng()
fs = 1024
N = 10*fs
nperseg = 512
amp = 2 * np.sqrt(2)
noise_power = 0.001 * fs / 2
time = np.arange(N) / float(fs)
carrier = amp * np.sin(2*np.pi*50*time)
noise = rng.normal(scale=np.sqrt(noise_power),
                   size=time.shape)
x = carrier + noise
f, t, Zxx = signal.stft(x, fs=fs, nperseg=nperseg)
print("zxx: ", type(Zxx) ,Zxx)
"""
"""
plt.figure()
plt.pcolormesh(t, f, np.abs(Zxx), vmin=0, vmax=amp, shading='gouraud')
plt.ylim([f[1], f[-1]])
plt.title('STFT Magnitude')
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.yscale('log')
plt.show()
"""