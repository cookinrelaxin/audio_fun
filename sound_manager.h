#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

namespace MySound {
    class SoundObject;
};

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

// #include <thread>
#include <cpthread>
#include <unordered_map>
#include <vector>
#include <list>

namespace MySound {
    typedef struct SoundBuffer {
        bool free;
        uint32 refID;
    } SoundBuffer;

    typedef struct SoundSource {
        bool free;
        uint32 refID;
    } SoundSource;

    class SoundManager {
        public:
           static std::vector<std::string> GetAllDevices();
           static void Initialize(const std::string& deviceName = "", bool useThreadUpdate = true);
           static void Destroy();
           static SoundManager* GetInstance();

           bool ExistSound(const std::string& name) const;
           void ReleaseSound(const std::string& name);
           void AddSound(
        private:
    };
};

#endif
