#include <cmath>
#include <fstream>
#include <string>
#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include "../libs/AudioFile/AudioFile.h"
using namespace std;

const int windowSizeParam = 1024; // HAS TO BE POWER OF 2
const int hopSizeParam = windowSizeParam/2;
// optional vars
const int decimationFactor = 3; // no decimation = 0; force less resolution decimating values
const int zeroPaddingFactor = 0; // no padding = 0; force more resolution by end padding 0's

const int decimationResult = pow(2,decimationFactor); // ensure decimation maintains power of 2
const int zeroPaddingResult = pow(2,zeroPaddingFactor); // ensure zero padding maintains power of 2

// samples ***
const string inputFilePath = "Spice and Wolf - You Are An Ass.wav";
const string fileNamePrefix = "Spice and Wolf - You Are An Ass_complex";
// const string fileNamePrefix = "Spice and Wolf - You Are An Ass_dbfs";

//const string inputFilePath = "adrenal_vapor.wav";
//const string fileNamePrefix = "adrenal_vapor_dbfs";

// const string inputFilePath = "elvis_mic.wav";
// const string fileNamePrefix = "elvis_mic_complex";

// const string inputFilePath = "freq440amp0d5_2048.wav";
// const string fileNamePrefix = "freq440amp0d5_2048_complex";

//const string inputFilePath = "sin_c_major_7_chord.wav";
//const string fileNamePrefix = "sin_c_major_7_chord";

// const string inputFilePath = "ab_sample_2.wav";
// const string fileNamePrefix = "ab_sample_2_complex";

//const string inputFilePath = "ab_sample_1.wav";
//const string fileNamePrefix = "ab_sample_1";

//const string inputFilePath = "OWOGH.wav";
//const string fileNamePrefix = "OWOGH_complex";

// const string inputFilePath = "OWOGH_mono.wav";
// const string fileNamePrefix = "OWOGH_mono_complex_t";

const string outputFileDir = "../python_front/fft_out/";
const string outputFilePath = outputFileDir + fileNamePrefix + 
    "_decimation_" + to_string(decimationResult) +
    "_stft_w_" + to_string(windowSizeParam) +
    "_detail_" + to_string(zeroPaddingResult) +
    ".csv";

ofstream resultFile (outputFilePath);
const int hannAmplitudeCorrection = 2; // correction because window lowers power
const int hannEnergyCorrection = 1.63; // exclusive correction for energy under func

int getBitReversal(int i, int bits) {
    if (i == 0) {return 0; } // edgecase
    int n = i;
    const int size = int(log2(i))+1;
    string bitsContainer = "";
    for(int k=0; k<size; k++) {
        bitsContainer += to_string(n%2);
        n = n/2;
    }
    // append extended bits to end of string as bit extension is 
    // append (not prepend) due to reversal
    for(int j=0; j<(bits-size); j++) {
        bitsContainer.append("0");
    }
    const int reverseBitsNum = stoi(bitsContainer, 0, 2);
    return reverseBitsNum;
}

vector<complex<double>> bitReverseCopy(vector<double> samples) {
    const int n = samples.size();
    const int bits = int(log2(n));
    vector<complex<double>> bitReverseOrder(n);
    for(int i=0; i<n; i++) {
        bitReverseOrder[getBitReversal(i,bits)]=complex<double>(samples[i]);
    }
    return bitReverseOrder;
}

// Ref: CLRS FFT chapter
vector<complex<double>> fft(vector<double> samples) { 
    const int n = samples.size();
    vector<complex<double>> a = bitReverseCopy(samples);
    const int treeLevels = int(log2(n));
    for(int i=1; i<=treeLevels; i++) {
        const int m = pow(2,i);
        const complex<double> wm = polar(1.0,-2*3.14159/m);
        //const complex<double> complexWmInner(0,2*3.14159/m);
        //const complex<double> wm = exp(complexWmInner);
        for(int j=0; j<n; j+=m) {
            complex<double> w(1);
            for(int k=0; k<m/2; k++) {
                const complex<double> oddSide = w * a[k+j+m/2];
                const complex<double> evenSide = a[j+k];
                a[j+k] = evenSide + oddSide;
                a[j+k+m/2] = evenSide - oddSide;
                w = wm*w;
            }       
        }
    }
    return a;
}

void writeFrame(float startingTime, int originalWindowSize, vector<double> samples, int sampleRate) { 
    const int antiAliasedSampleRate = (int)floor(sampleRate/decimationResult);
    const float antiAliasedNyquist = antiAliasedSampleRate/2.0;
    const float decimatedWindowSize = originalWindowSize/decimationResult;
    const float nyquist = sampleRate/2.0;
    // float currentTime = startingTime;
    vector<complex<double>> pain = fft(samples);
    int iterationCounter = 0;
    float frequencyBin = 0; // bin begins at 0hz
    // have all frequency bins under low-pass nyquist cutoff as anything post-Nyquist is repeated
    // and to low-pass limit to prevent aliasing post decimation
    while(frequencyBin <= antiAliasedNyquist) {
        complex<double> fk = pain[iterationCounter];
        const double magnitude = sqrt(pow(fk.real(),2)+pow(fk.imag(),2));
        // window correction, values under nyquist doubling, and magnitude normalization over total samples
        const double amplitude = hannAmplitudeCorrection * 2 * magnitude/decimatedWindowSize;
        // max amplitude for 16 bit is 1 so sample/reference = sample/1
        const double dbfs = 20*log10(amplitude);

        // Normalize values by decimation factor else results will be louder than original
        // const float real = fk.real()/decimationResult;
        // const float imag = fk.imag()/decimationResult;
        // resultFile<<real<<"+"<<imag<<"j"<<",";
        // allows downsampling by skipping samples that are to be decimated
        iterationCounter = iterationCounter + decimationResult;
        // cout<<"frequencyBin: "<<frequencyBin<<" dbfs: "<<dbfs<<endl;
        frequencyBin = iterationCounter*(sampleRate/float(samples.size()));
    }
    /*
    // delete final comma
    long pos = resultFile.tellp();
    resultFile.seekp(pos - 1);
    resultFile<<"\n";
    */
}
int main() {
    AudioFile<double> a;
    a.load (inputFilePath);
    const int channel = 0;
    const int totalSamples = a.getNumSamplesPerChannel();
    const int sampleRate = a.getSampleRate();
    int numChannels = a.getNumChannels();
    cout<<"numChannels: "<<numChannels<<endl;
    // post decimation sample rate; default decimationFactor 1 keeps original SR
    const int bitDepth = a.getBitDepth();
    // amplitude is max displacement of sinousoid past 0 so bitDepth-1
    // double maxVolumeRefDbfs = pow(2,bitDepth-1);
    // cout<<"bitDepth: "<<bitDepth<<" maxVolume: "<<maxVolumeRefDbfs<<endl;
    // int nyquist = antiAliasingSampleRate/2;
    int startingIndex = 0;
    int endingIndex = 0;

    // const int decimatedSampleRate = sampleRate / decimationFactor;

    // zero pad up window so when indexes are added with 0 -> windowSizeParam-1
    // right end is padded with 0's
    // I have N O idea whyyy tf decimationFactor needs to be zeropadded for decimation to work
    // TODO: consult an expert (I deadass don't know who) because I am lost here.
    vector<double> currentWindow(zeroPaddingResult*decimationResult*windowSizeParam,0);
    float startingTime;
    double hannWindow;

    // Time: seconds ; Frequency: hz ; dBFS: Decibels relative to full scale
    // resultFile<<"Time,Frequency,dBFS"<<""<<endl;
    cout<<"Starting STFT."<<endl;
    while(endingIndex < totalSamples-1) {
        startingTime = float(startingIndex)/sampleRate;
        // last window most likely is smaller than windowSizeParam, but since init vector
        // is always windowSizeParam and is replaced with sample per index, zero-padding on end
        // should be implicitly applied if needed

        // Used to prevent index overflow from audio channel since last window size will likely
        // exceed remaining samples left in the samplesChannel
        int currentWindowSize = ((totalSamples-startingIndex) < windowSizeParam) 
            ? totalSamples-startingIndex
            : windowSizeParam;
        // read from sample channel and apply window function, then toss val into currentWindow
        for (int i=startingIndex; i<startingIndex+currentWindowSize; i++) {
            hannWindow = 0.5 * (1-cos(2*3.14159*(i-startingIndex) / (currentWindowSize-1)));
            double windowedSample = a.samples[channel][i] * hannWindow;
            // double windowedSample = a.samples[channel][i];
            currentWindow[i-startingIndex] = windowedSample;
        }
        writeFrame(startingTime, currentWindowSize, currentWindow, sampleRate);
        // cout<<"ran."<<endl;
        endingIndex = startingIndex + windowSizeParam - 1;
        startingIndex += hopSizeParam;
        // I have N O idea whyyy tf decimationFactor needs to be zeropadded for decimation to work
        // TODO: consult an expert (I deadass don't know who) because I am lost here.
        currentWindow.assign(zeroPaddingResult*decimationResult*windowSizeParam,0); // restore vector to og size and 0 for next iter
    }
    cout<<"i think ...??? i know what i just did."<<endl;
    resultFile.close();
    return 0;
}