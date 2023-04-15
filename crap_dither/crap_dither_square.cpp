#include <iostream>
#include <random>
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
const int bitDepth = 16;
const int duration = 2;

const int totalOctaves = 7;
// const int nth = -2;
const float c4 = 261.63;
// const float noteFreq = pow(2,nth/12.0) * 440;

// const float freq = noteFreq;
const float amp = 0.25; // input 2

// const string freqString = floatToString(freq);
const string ampString = floatToString(amp);

const string fileName = "test.wav";

class SineOscillator {
    float frequency, amplitude, angle = 0.0f, offset = 0.0;
public: 
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = 2 * 3.14159 * frequency / sampleRate; // one angle count from unit circ
    }
    float process() {
        // auto sample = amplitude * sin(angle);
        int sign = (sin(angle) > 0) ? 1 : -1;
        auto sample = sign * amplitude;
        // auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
    }
};

void writeToFile(ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
}
int main() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
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

    // cout<<random1<<'\n'<<random2<<'\n'<<dither;
    ofstream audioFile; 
    const float noteFreq = pow(2,0/12.0) * c4;
    cout << noteFreq;
    audioFile.open(fileName, ios::binary);
    SineOscillator sineOscillator(noteFreq, amp);

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
        // triangular pdf vars
        double random1 = dist(mt);
        double random2 = -1 * dist(mt);
        double q = amp;

        auto sample = sineOscillator.process();
        // TODO: the scaling here is arbitrary, but it gets the job done
        double dither = sample*(random1 + random2)/4;
        int intSample = static_cast<int> ((sample+dither) * maxAmplitude);
        cout << intSample << '\n';
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