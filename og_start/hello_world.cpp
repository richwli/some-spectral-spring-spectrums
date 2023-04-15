#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

const int sampleRate = 44100;
const int bitDepth = 16;
const int duration = 2;

class SineOscillator {
    float frequency, amplitude, angle = 0.0f, offset = 0.0;
public: 
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = 2 * 3.14159 * frequency / sampleRate;
    }
    float process() {
        auto sample = amplitude * sin(angle);
        angle += offset;
        // Asin(2pi*freq/sampleRate)
        return sample;
    }
};

void writeToFile(ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
}
int main() {
    ofstream audioFile; 
    audioFile.open("waveform.wav", ios::binary);
    SineOscillator sineOscillator(440, 0.5);

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
        auto sample = sineOscillator.process();
        int intSample = static_cast<int> (sample * maxAmplitude);
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