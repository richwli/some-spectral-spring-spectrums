import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np
import ast
from PyQt5 import QtGui
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtGui import QPainter, QBrush, QPen, QColor
from PyQt5.QtCore import Qt
import sys
import pandas as pd
import math
EPSILON = sys.float_info.epsilon  # Smallest possible difference.

inputFile = r'fft_out/Spice and Wolf - You Are An Ass_dbfs_decimation_4_stft_w_1024_detail_4.csv'
windowSize = int(inputFile.split("_stft_w_")[1].split("_detail_")[0])
decimationFactor = int(inputFile.split("_decimation_")[1].split("_stft_w_")[0])
df = pd.read_csv(inputFile)

plotWidth = 750
plotHeight = 500
sampleRate = 44100
# sampleRate = 44100/decimationFactor
# maxFrequency = df.iloc[-1]['Frequency'] # last frequency output is largest
frequencyCap = sampleRate/2 # nyquist limit; var can be lowered

columnSizeInSeconds = windowSize/sampleRate
# I ceiling'd last frame to be whole number so all window frames are same size instead having last cutoff.
# Consequently data will be at most window_size - 1 samples larger than original audio (worst case)
# and at least same size as original (best case when totalSamples is factor of window_size + h hops)
totalTimeInSeconds = df.iloc[-1]['Time']+columnSizeInSeconds

#ratioColumnToTotal = columnSizeInSeconds/totalTimeInSeconds
#columnSizeInPixels = math.ceil(plotWidth*ratioColumnToTotal)

pixelsInSecond = plotWidth/totalTimeInSeconds
pixelsInFrequency = plotHeight/frequencyCap
print("ratioSecondToPixel: ", pixelsInSecond)
rgbColors = [(0, 63, 92),(140, 118, 160),(255, 183, 197)]
# filter out amplitude 0 freqs and get min max
min_dbfs = np.amin(df[df['dBFS']>float('-inf')])['dBFS']
max_dbfs = np.amax(df['dBFS'])
print('some min: ', min_dbfs)
print('some max: ', max_dbfs)
class Window(QMainWindow):
    def __init__(self):
        super().__init__()
        self.title = "PyQt5 Plz F00king Work"
        self.top= 0
        self.left= 0
        self.width = plotWidth
        self.height = plotHeight
        self.InitWindow()

    def InitWindow(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.top, self.left, self.width, self.height)
        self.setStyleSheet("background-color: black")
        self.show()
    def paintEvent(self, e):
        qp = QPainter()
        qp.begin(self)
        self.drawPoints(qp)
        qp.end()

    def drawPoints(self, qp):
        transformColumnDataToWindow(qp)

def roundFloat(fl):
    decimal=fl-math.floor(fl) 
    return int(math.floor(fl)) if decimal<=0.5 else int(math.ceil(fl))

def convert_to_rgb(val, colors, minval=0, maxval=1):
    # `colors` is a series of RGB colors delineating a series of
    # adjacent linear color gradients between each pair.

    # Determine where the given value falls proportionality within
    # the range from minval->maxval and scale that fractional value
    # by the total number in the `colors` palette.
    i_f = float(val-minval) / float(maxval-minval) * (len(colors)-1)

    # Determine the lower index of the pair of color indices this
    # value corresponds and its fractional distance between the lower
    # and the upper colors.
    i, f = int(i_f // 1), i_f % 1  # Split into whole & fractional parts.

    # Does it fall exactly on one of the color points?
    if f < EPSILON:
        return colors[i]
    else: # Return a color linearly interpolated in the range between it and 
          # the following one.
        (r1, g1, b1), (r2, g2, b2) = colors[i], colors[i+1]
        return int(r1 + f*(r2-r1)), int(g1 + f*(g2-g1)), int(b1 + f*(b2-b1))

def makeColumnLine(qp, startTime, frequency, dBFS):
    endTime = startTime+columnSizeInSeconds
    startXPixel = roundFloat(pixelsInSecond*startTime)
    endXPixel = roundFloat(pixelsInSecond*endTime)
    frequencyYPixel = plotHeight-roundFloat(pixelsInFrequency*frequency)
    normalizedVolume = (dBFS-min_dbfs)/(max_dbfs-min_dbfs)
    # print(normalizedVolume)
    # heatMapRgb = getValueBetweenTwoFixedColors(normalizedVolume)
    # color = round(min(600*amplitude, 255)) # amplitudeColor cannot exceed rgb 255 limit
    heatMapRgb = convert_to_rgb(normalizedVolume, rgbColors)
    drawLineData(qp, startXPixel,endXPixel,frequencyYPixel,*heatMapRgb)


def transformColumnDataToWindow(qp):
    for _, row in df.iterrows():
        if(row['dBFS']>float('-inf')):
            makeColumnLine(qp, row['Time'], row['Frequency'],row['dBFS'])

def drawLineData(qp,startXPixel,endXPixel,frequencyYPixel,r,g,b):
    qp.setPen(QColor(r, g, b))
    qp.drawLine(startXPixel,frequencyYPixel,endXPixel,frequencyYPixel)

def main():
    #transformColumnDataToCanvas()
    App = QApplication(sys.argv)
    window = Window()
    print("fuck")
    sys.exit(App.exec())
    
if __name__ == "__main__":
    main()