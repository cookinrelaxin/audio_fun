#include "sound.h"

#include <iostream>
#include <fstream>

#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

Sound::Sound(const char* path) {
   std::ifstream file;
   file.open(path, std::ios::binary | std::ios::in);

   file.read(header.id, sizeof(header.id));
   if (std::memcmp(header.id, "RIFF", 4))
       throw std::runtime_error("Not a WAV file");

   file.read((char*)&header.totalLength, sizeof(header.totalLength));

   file.read(header.wavefmt, sizeof(header.wavefmt));
   if (std::memcmp(header.wavefmt, "WAVEfmt ", 8))
       throw std::runtime_error("Not the right format");

   file.read((char*)&header.format, sizeof(header.format));
   file.read((char*)&header.pcm, sizeof(header.pcm));
   file.read((char*)&header.channels, sizeof(header.channels));
   file.read((char*)&header.frequency, sizeof(header.frequency));
   file.read((char*)&header.bytesPerSecond, sizeof(header.bytesPerSecond));
   file.read((char*)&header.bytesByCapture, sizeof(header.bytesByCapture));
   file.read((char*)&header.bitsPerSample, sizeof(header.bitsPerSample));
   file.read(header.data, sizeof(header.data));
   file.read((char*)&header.bytesInData, sizeof(header.bytesInData));

   bufferData = (char*)malloc(header.bytesInData * sizeof(char));
   file.read(bufferData, header.bytesInData);

   printHeader();
   std::cout << "duration: " << durationInSeconds() << std::endl;
}

Sound::~Sound() {
    free(bufferData);
};
