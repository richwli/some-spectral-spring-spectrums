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
const int bitDepth = 16;
const int duration = 2;

const int totalOctaves = 7;
// const int nth = -2;
const float c1 = 65.41;
// const float noteFreq = pow(2,nth/12.0) * 440;

// const float freq = noteFreq;
const float amp = 0.65; // input 2

// const string freqString = floatToString(freq);
const string ampString = floatToString(amp);
/*
string cleanedString(string input) {
    for (int i = 0; i < input.length(); i++) {
        if (input[i] == '.') {
            const string newString = input.replace(i,1,"d");
            return newString;
        }
    }
    return input;
}
*/
const string fileName = "test.wav";

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
    /*
    unordered_map<int, string> m =
    {
        {1, "one"},
        {2, "two"},
        {3, "three"}
    };
    I shit u not, I fucking cannot make an unordered map.
    Why.
    > the procedure entry point _zst28__throw_bad_array_new_lengthv
    */
    /* unordered_map<int, string> keyMap;
    keyMap[0] = "C";
    keyMap[1] = "C#_Db";
    keyMap[2] = "D";
    keyMap[3] = "D#_Eb";
    keyMap[4] = "E";
    keyMap[5] = "F";
    keyMap[6] = "F#_Gb";
    keyMap[7] = "G";
    keyMap[8] = "G#_Ab";
    keyMap[9] = "A";
    keyMap[10] = "A#_Bb";
    keyMap[11] = "B";
    */
    int nth;

    ofstream audioFile; 
    for (int i = 0; i < totalOctaves; i++) {
        for (int key = 0; key < 12; key++) {
            nth = i*12+key;
            const float noteFreq = pow(2,nth/12.0) * c1;
            const string nameStringFile = fakeMap[key] + "_" + to_string(i+2) + ".wav";
            cout << "\n" << nameStringFile << '\n' << noteFreq <<"\n";
            // cout << fakeMap[key] <<'\n' << noteFreq <<"\n\n";
            /***********************Big Paste**************************/
            audioFile.open(nameStringFile, ios::binary);
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
            /**********************************************************/
            
        }
    }
   return 0;
}