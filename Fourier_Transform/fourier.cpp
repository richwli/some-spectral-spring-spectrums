#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <complex>
using namespace std;

const string fileName = "test.wav";
const int sampleRate = 3000;//hertz
const int nyquistLimit = sampleRate/2;//hertz
const int duration = 1;//seconds
const int N = sampleRate * duration;//total samples
const int bitDepth = 16;

// const float a4 = 220;
const float c4 = 440;
const int c4int = (int)(c4 + 0.5);
const float amp = 0.20;
auto maxAmplitude = pow(2, bitDepth - 1) - 1;

const complex<double> i(0.0,1.0);    
double samples[N] = {};
/*
class SineOscillator {
    float frequency, amplitude, angle = 0.0f, offset = 0.0;
public: 
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = 2 * 3.14159 * frequency / sampleRate; // one angle count from unit circ
    }
    float process() {
        auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
    }
    float getAngle() {
        return angle;
    }
};
*/

class SineOscillator {
    float frequency1, frequency2, frequency3, frequency4, amplitude, 
    angle1 = 0.0f, angle2 = 0.0f, angle3 = 0.0f, angle4 = 0.0f,
    offset1 = 0.0, offset2 = 0.0, offset3 = 0.0, offset4 = 0.0;
public: 
    SineOscillator(
        float freq1, float freq2, float freq3, float freq4,
        float amp) 
    : frequency1(freq1), frequency2(freq2), frequency3(freq3), frequency4(freq4),
    amplitude(amp) 
    {
        offset1 = 2 * 3.14159 * frequency1 / sampleRate; // one angle count from unit circ
        offset2 = 2 * 3.14159 * frequency2 / sampleRate; // one angle count from unit circ
        offset3 = 2 * 3.14159 * frequency3 / sampleRate; // one angle count from unit circ
        offset4 = 2 * 3.14159 * frequency4 / sampleRate; // one angle count from unit circ
    }
    float process() {
        auto sample = amplitude * (sin(angle1) + sin(angle2) + sin(angle3) + sin(angle4));
        angle1 += offset1;
        angle2 += offset2;
        angle3 += offset3;
        angle4 += offset4;
        return sample;
    }
};

void writeToFile(ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
}
double bn(int n, int k) {
    return 2*3.14159*k*n/N;
} 
complex<double> xn(int k,double sample) {
    complex<double> xk;
    for(int n=0; n < N; n++) {
        double bn_val = -1 * bn(n,k/1); // seems correct
        // double hanWindow = pow(sin(3.14159 * n / N),2);
        // complex<double> hanWindowComplex(hanWindow); // hann window
        // int intSample = static_cast<int> (sampleC * maxAmplitude);
        complex<double> complexSample(sample);
        complex<double> leftEulers(cos(bn_val));
        complex<double> rightEulers(sin(bn_val));
        //cout<<leftEulers<<" "<<rightEulers<<'='<<leftEulers + i*rightEulers<<endl;
        xk += complexSample * (leftEulers - i*rightEulers);
    }
    return xk;
}
complex<double> wn(int n, int k) {
    float inner = -2*3.14159*k/n;
    complex<double> leftEulers(cos(inner));
    complex<double> rightEulers(sin(inner));
    return leftEulers - i*rightEulers;
} 
int main() {
    const string fakeMap[12] = {
        "C",
        "C#_Db",
        "D",
        "D#_Eb",
        "E",
        "F",
        "F#_Gb",
        "G",
        "G#_Ab",
        "A",
        "A#_Bb",
        "B"
    };
    int nth;

    ofstream audioFile; 

    const float noteCFreq = pow(2,0/12.0) * c4;
    const float noteEFreq = pow(2,4/12.0) * c4;
    const float noteGFreq = pow(2,7/12.0) * c4;
    const float noteBFreq = pow(2,11/12.0) * c4;

    const int cFreqint = (int)(noteCFreq + 0.5);
    const int eFreqint = (int)(noteEFreq + 0.5);
    const int gFreqint = (int)(noteGFreq + 0.5);
    const int bFreqint = (int)(noteBFreq + 0.5);
    // cout<<cFreqint<<" "<<eFreqint<<" "<<gFreqint<<" "<<bFreqint<<" "<<endl;
    audioFile.open(fileName, ios::binary);
    // SineOscillator sineOscillator(noteCFreq, noteEFreq, noteGFreq, noteBFreq, amp);
    // SineOscillator sineOscillator(a4, amp);
    //Header chunk
    audioFile << "RIFF";
    audioFile << "----";
    audioFile << "WAVE";

    // Format chunk
    audioFile << "fmt ";
    writeToFile(audioFile, 16, 4); // Size
    writeToFile(audioFile, 1, 2); // Compression Code
    writeToFile(audioFile, 1, 2); // Number of channels
    writeToFile(audioFile, sampleRate, 4); // Sample rate
    writeToFile(audioFile, sampleRate * bitDepth / 8, 4); // Byte Rate
    writeToFile(audioFile, bitDepth / 8, 2); // Block align
    writeToFile(audioFile, bitDepth, 2); // bit depth

    // data chunk
    audioFile << "data";
    audioFile << "----";

    int preAudioPosition = audioFile.tellp();
    SineOscillator sineOscillator(cFreqint, eFreqint, gFreqint, bFreqint, amp);
    for(int n=0; n < N; n++) {
        auto sampleC = sineOscillator.process();
        double sample = sampleC * maxAmplitude;
        samples[n] = sample;
        cout<<sample<<endl;
    }
        // double magnitude = sqrt((xk.real*xk.real)+(xk.imag*xk.imag));
        // cout<<k<<": "<<sqrt(pow(xk.real(),2)+pow(xk.imag(),2))<<'\n';
    // const complex<double> fuck(9.0, 5.0);  
    // const complex<double> shit(11.0, 12.0);  
    // cout<<"test "<<(fuck*shit).real();
    /*
    int postAudioPosition = audioFile.tellp();

    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);

    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4);

    audioFile.close();
    */
    return 0;
}