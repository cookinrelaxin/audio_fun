#include "channel.h"

#include "sound.h"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif __linux
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

void TEST_ERROR(std::string msg) {
    if (alGetError() != AL_NO_ERROR)
        throw std::runtime_error(msg + " failed");
}

void Channel::Play(Sound* s) {
    sound = s;
 
    ALboolean enumeration;
    const ALCchar* devices;
    const ALCchar* defaultDeviceName;

    ALCdevice* device;
    ALvoid* data;
    ALCcontext* context;
    ALsizei size, freq;
    ALenum format;
    ALuint buffer, source;
    ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    ALboolean loop = AL_FALSE;
    ALCenum error;
    ALint source_state;

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE)
        std::cout << "enumeration extension is not available" << std::endl;

    defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    device = alcOpenDevice(defaultDeviceName);
    if (!device)
        throw std::runtime_error("unable to open default device");

    std::cout << "Device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;

    alGetError();

    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context))
        throw std::runtime_error("failed to make default context");

    TEST_ERROR("make default context");

    alListener3f(AL_POSITION, 0, 0, 1.0f);
    TEST_ERROR("listener position");

    alListener3f(AL_VELOCITY, 0, 0, 0);
    TEST_ERROR("listener velocity");

    alListenerfv(AL_ORIENTATION, listenerOri);
    TEST_ERROR("listener orientation");

    alGenSources((ALuint)1, &source);
    TEST_ERROR("source generation");

    alSourcef(source, AL_PITCH, 1);
    TEST_ERROR("source pitch");

    alSourcef(source, AL_GAIN, 1);
    TEST_ERROR("source gain");

    alSource3f(source, AL_POSITION, 0, 0, 0);
    TEST_ERROR("source position");

    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    TEST_ERROR("source velocity");

    alSourcei(source, AL_LOOPING, AL_FALSE);
    TEST_ERROR("source looping");

    alGenBuffers(1, &buffer);
    TEST_ERROR("buffer generation");

    if (sound->header.format == 16
        and sound->header.channels == 2
        and sound->header.frequency == 44100) {
        alBufferData(buffer,
                     AL_FORMAT_STEREO16,
                     sound->bufferData,
                     sound->header.bytesInData * sizeof(char),
                     44100);
        TEST_ERROR("load buffer data");
    }
    else
        throw std::runtime_error("Expected 16 bit, stereo 44100 Hz");

    alSourcei(source, AL_BUFFER, buffer);
    TEST_ERROR("bind buffer");

    alSourcePlay(source);
    TEST_ERROR("source play");

    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    TEST_ERROR("get source state");

    while (AL_SOURCE_STATE != AL_STOPPED) {
        std::cout << "AL_SOURCE_STATE: " << AL_SOURCE_STATE << std::endl;
        // if (AL_SOURCE_STATE == AL_STOPPED) break;
        // if (AL_SOURCE_STATE == AL_PLAYING) std::cout << "is playing" << std::endl;
        // if (AL_SOURCE_STATE == AL_STOPPED) std::cout << "is stopped" << std::endl;
        // if (AL_SOURCE_STATE == AL_PAUSED) std::cout << "is paused" << std::endl;
        // if (AL_SOURCE_STATE == AL_INITIAL) std::cout << "is initial" << std::endl;
        if (shouldStop) {
            alSourceStop(source);
            shouldStop = false;
        }
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        TEST_ERROR("get source state loop");
    }

    std::cout << "stopped" << std::endl;

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void Channel::Stop() {
    std::cout << "stop" << std::endl;
    shouldStop = true;
}
