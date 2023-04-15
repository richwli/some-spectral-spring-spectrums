#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
using namespace std;

string floatToString(float f) {
    stringstream ss;
    ss << f;
    return ss.str();
}
const int sampleRate = 44100;
// const int sampleRate = 32768;
const int bitDepth = 16;
const int duration = 1;

const int totalOctaves = 7;
// const int nth = -2;
const float c4 = 261.63;
// const float noteFreq = pow(2,nth/12.0) * 440;

// const float freq = noteFreq;
const float amp = 0.20; // input 2

// const string freqString = floatToString(freq);
const string ampString = floatToString(amp);
const string fileName = "zero_padding_test.wav";

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

    // cout << "\n" << nameStringFile << '\n' << noteFreq <<"\n";
    // cout << fakeMap[key] <<'\n' << noteFreq <<"\n\n";
    audioFile.open(fileName, ios::binary);
    SineOscillator sineOscillatorChord(noteCFreq, noteEFreq, noteGFreq, noteBFreq, amp);
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

    auto maxAmplitude = pow(2, bitDepth - 1) - 1;
    for(int i=0; i < sampleRate * duration; i++) {
        auto sampleC = sineOscillatorChord.process();
        int intSample = static_cast<int> (sampleC * maxAmplitude);
        writeToFile(audioFile, intSample, 2);
    }
    int postAudioPosition = audioFile.tellp();

    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);

    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4);

    audioFile.close();

   return 0;
}