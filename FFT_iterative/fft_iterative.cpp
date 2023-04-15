#include <cmath>
#include <fstream>
#include <string>
#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include "../libs/AudioFile/AudioFile.h"
using namespace std;

const string inputFilePath = "ab_sample_2.wav";
const string outputFilePath = "../python_front/fft_out/spectral1.txt";

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

vector<complex<double>> fft(vector<double> samples) { 
    const int n = samples.size();
    vector<complex<double>> a = bitReverseCopy(samples);
    const int treeLevels = int(log2(n));
    for(int i=1; i<=treeLevels; i++) {
        const int m = pow(2,i);
        const complex<double> wm = polar(1.0,-2*3.14159/m);
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

int main() {
    ofstream resultFile (outputFilePath);
    AudioFile<double> a;
    a.load (inputFilePath);
    int channel = 0;
    int numSamples = a.getNumSamplesPerChannel();
    int newVectorSizePadded = 1;
    int sampleRate = a.getSampleRate();
    float nyquistLimit = sampleRate/2.0;
    // find smallest power of 2 that is larger than samples length
    while(newVectorSizePadded < numSamples){
        newVectorSizePadded *= 2;
    }
    cout<<"newVectorSizePadded: "<<newVectorSizePadded<<endl;
    vector<double> samplesWithZeroPadding(newVectorSizePadded, 0);
    for (int i = 0; i < newVectorSizePadded; i++)
    {
        samplesWithZeroPadding[i]=a.samples[channel][i];
    }

    vector<complex<double>> pain = fft(samplesWithZeroPadding);
    int iterationCounter = 0;
    float frequencyBin = iterationCounter*(numSamples/float(newVectorSizePadded));
    // have all frequency bins under nyquist cutoff as anything post-Nyquist is useless
    while(frequencyBin <= nyquistLimit) {
        complex<double> fk = pain[iterationCounter];
        frequencyBin = iterationCounter*(numSamples/float(newVectorSizePadded));
        const double magnitude = sqrt(pow(fk.real(),2)+pow(fk.imag(),2));
        resultFile<<frequencyBin<<","<<magnitude<<endl;
        iterationCounter++;
    }
    resultFile.close();
    return 0;
}