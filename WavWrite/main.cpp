//
//  main.cpp
//  WavWrite
//
//  Created by Karl Simu on 2022-02-19.
//

#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

const int SampleRate = 44100;
const int bitDepth = 16;

class SineOscillator {
    float frequency, amplitude, angle = 0.0f, offset = 0.0;
public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = (2 * M_PI * frequency / SampleRate);
    }
    float process() {
        auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
        // A*sin(2*pi*f/sr) division by sr puts the waveequation into the "perspective" of the samplerate
    }
};

void writeToFile(ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
}

int main(int argc, const char * argv[]) {
    int duration = 2;
    ofstream audioFile; //[datatype] write/create output file using fstream
    audioFile.open("waveform.wav", ios::binary);
    SineOscillator sineOscillator(440,0.5);
    
    //Header chunk
    audioFile << "RIFF";
    audioFile << "----"; //size
    audioFile << "WAVE";
    
    //Format chunk
    audioFile << "fmt ";
    writeToFile(audioFile, 16, 4); // size
    writeToFile(audioFile, 1, 2); // compression code
    writeToFile(audioFile, 1, 2); // num chan
    writeToFile(audioFile, SampleRate, 4); // sr
    writeToFile(audioFile, SampleRate * bitDepth * 1 / 8, 4); // byte rate
    writeToFile(audioFile, bitDepth/8, 2); // block align (2 bytes/sample)
    writeToFile(audioFile, bitDepth, 2); // bit depth
    
    //Data chunk
    audioFile << "data";
    audioFile << "----"; //size
    
    int preAudioPosition = audioFile.tellp();
    auto maxAmplitude = pow(2, bitDepth-1) - 1;
    for (int i = 0; i < SampleRate * duration; i++) {
        auto sample = sineOscillator.process(); //get sample
        int intSample = static_cast<int>(sample * maxAmplitude); //make sample 16-bit signed
        writeToFile(audioFile, intSample, 2); //Write as a character 2 bytes
    }
    
    //cout << "Float size " << sizeof(float) << endl;
    //cout << "Int size " << sizeof(int) << endl;
    int postAudioPosition = audioFile.tellp();
    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);
    
    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4);
    
    audioFile.close();
    return 0;
}
