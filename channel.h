#ifndef CHANNEL_H
#define CHANNEL_H

#include "sound.h"

#include <atomic>

class Channel {
    public:
        Channel() : sound(nullptr) {}

        void Play(Sound* sound);
        void Stop();
        // void WriteSoundData(char* data, int count);

    private:
        Sound* sound;
        std::atomic<bool> shouldStop;

        // int position;
};

#endif
