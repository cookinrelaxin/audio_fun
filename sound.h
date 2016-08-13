#ifndef SOUND_H
#define SOUND_H

#include <iostream>

struct WavHeader {
    char id[4];
    int32_t totalLength;
    char wavefmt[8];
    int32_t format;
    int16_t pcm;
    int16_t channels;
    int32_t frequency;
    int32_t bytesPerSecond;
    int16_t bytesByCapture;
    int16_t bitsPerSample;
    char data[4];
    int32_t bytesInData;
};

class Sound {
    public:
        Sound(const char* path);
        ~Sound();

        void printHeader() {
            std::cout << "id: " << header.id << std::endl;
            std::cout << "totalLength: " << header.totalLength << std::endl;
            std::cout << "wavefmt: " << header.wavefmt << std::endl;
            std::cout << "format: " << header.format << std::endl;
            std::cout << "pcm: " << header.pcm << std::endl;
            std::cout << "channels: " << header.channels << std::endl;
            std::cout << "frequency: " << header.frequency << std::endl;
            std::cout << "bytesPerSecond: " << header.bytesPerSecond << std::endl;
            std::cout << "bytesByCapture: " << header.bytesByCapture << std::endl;
            std::cout << "bitsPerSample: " << header.bitsPerSample << std::endl;
            std::cout << "data: " << header.data << std::endl;
            std::cout << "bytesInData: " << header.bytesInData << std::endl;
        }

        float durationInSeconds() {
            return header.bytesInData / header.bytesPerSecond;
        }

        char* bufferData;
        WavHeader header;

    private:


};

#endif
