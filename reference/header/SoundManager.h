#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

namespace MySound 
{
	class SoundObject;
}

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

#include <pthread.h>

#include <unordered_map>
#include <vector>
#include <list>

#include "../../Macros.h"
#include "../../Utils/header/MyString.h"

namespace MySound 
{
	typedef struct SoundBuffer 
	{
		bool free;		
		uint32 refID;
	} SoundBuffer;

	typedef struct SoundSource 
	{
		bool free;	
		uint32 refID;
	} SoundSource;

	class SoundManager 
	{
		public:

			static std::vector<MyStringAnsi> GetAllDevices();
			static void Initialize(const MyStringAnsi & deviceName = "", bool useThreadUpdate = true);
			static void Destroy();			
			static SoundManager * GetInstance();			
			
			bool ExistSound(const MyStringAnsi & name) const;
			void ReleaseSound(const MyStringAnsi & name);
			void AddSound(const MyStringAnsi & fileName, const MyStringAnsi & name);
			void AddSound(SoundObject * sound);
	
			SoundObject * GetSound(const MyStringAnsi & name);

			void Update();
			
			void SetMasterVolume(float volume);
			void VolumeUp(float amount = 0.1f);
			void VolumeDown(float amount = 0.1f);

			bool IsEnabled();
			void Disable();
			void Enable();

			friend class SoundObject;			
	
		protected:	
			SoundManager(const MyStringAnsi & deviceName, bool useThreadUpdate);
			~SoundManager();		

			static SoundManager * instance;

			ALCdevice * deviceAL;
			ALCcontext * contextAL;
			MyStringAnsi deviceName;

			pthread_t updateThread;
			pthread_mutex_t fakeMutex;
			pthread_cond_t fakeCond;
			bool useThreadUpdate;
			bool ended;

			bool enabled;
			float lastVolume;

			float masterVolume;

			std::unordered_map<MyStringAnsi, SoundObject *> sounds;

			std::vector<SoundSource> sources;
			std::vector<SoundBuffer> buffers;

			std::list<SoundSource *> freeSources;
			std::list<SoundBuffer *> freeBuffers;

			void Init();
			
			SoundSource * GetFreeSource();
			SoundBuffer * GetFreeBuffer();
	
			void FreeSource(SoundSource * source);
			void FreeBuffer(SoundBuffer * buffer);

			static void * UpdateThread(void * c);
			void Wait(int timeInMS);
			void ThreadUpdate();
	};

} 


#endif