#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <complex>
#include <vector>
#include "..\libs\AudioFile\AudioFile.h"
using namespace std;

const string fileName = "test.wav";
const int sampleRate = 2048;//hertz
const int nyquistLimit = sampleRate/2;//hertz
const int duration = 1;//seconds
const int N = sampleRate * duration;//total samples
const int bitDepth = 16;

// const float a4 = 220;
const float c4 = 261.63;
const int c4int = (int)(c4 + 0.5);
const float amp = 0.20;
auto maxAmplitude = pow(2, bitDepth - 1) - 1;

const complex<double> i(0.0,1.0);    
vector<complex<double>> samples(N);

double bn(int n, int k) {
    return 2*3.14159*k*n;
} 
complex<double> wk(int k) {
    float inner = -2*3.14159*k/N;
    complex<double> leftEulers(cos(inner));
    complex<double> rightEulers(sin(inner));
    return leftEulers + i*rightEulers;
} 

int getBitReversal(int i, int bits) {
    if (i == 0) {return 0; } // edgecase
    int n = i;
    // int reverseBitsNum = 0;
    const int size = int(log2(i))+1;
    string bitsContainer = "";
    for(int k=0; k<size; k++) {
        // bitsContainer[k] = n%2;
        bitsContainer += to_string(n%2);
        n = n/2;
    }
    // append extended bits to end of string as bit
    // extension is append (not prepend) due to 
    // reversal
    for(int j=0; j<(bits-size); j++) {
        bitsContainer.append("0");
    }
    // cout<<"bits: "<<bitsContainer<<endl;
    /*
    for(int l=bits-1; l>=0; l--) {
        if(bitsContainer[l] == '1'){
            reverseBitsNum += int(pow(2,bits-l));
        } 
    }
    */
    const int reverseBitsNum = stoi(bitsContainer, 0, 2);
    return reverseBitsNum;
}

vector<complex<double>> bitReverseCopy(vector<complex<double>> samples) {
    const int n = samples.size();
    const int bits = int(log2(n));
    // const complex<double> neg1(-1);
    vector<complex<double>> bitReverseOrder(n);
    // cout<<"bits: "<<bits<<" "<<"samples: "<<samples.size()<<endl;
    for(int i=0; i<n; i++) {
        // cout<<getBitReversal(i,bits)<<" ";
        bitReverseOrder[getBitReversal(i,bits)]=samples[i];
    }
    return bitReverseOrder;
}

vector<complex<double>> fft(vector<complex<double>> samples) { 
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
    AudioFile<double> a;
    a.load ("plzfuckingwork.wav");
    // cout<<a.samples[0];
    int channel = 0;
    int numSamples = a.getNumSamplesPerChannel();
    vector<complex<double>> plzFuckingWork(numSamples);
    // cout<<"num samples: "<<numSamples<<endl;
    // samples[n] = complex<double>(sampleC);
    for (int i = 0; i < numSamples; i++)
    {
        plzFuckingWork[i]=complex<double>(a.samples[channel][i]);
    }

    vector<complex<double>> pain = fft(plzFuckingWork);
    for (int k=0; k<samples.size(); k++) {
        complex<double> fk = pain[k];
        cout<<k<<": "<<sqrt(pow(fk.real(),2)+pow(fk.imag(),2))<<'\n';
    }

    return 0;
}