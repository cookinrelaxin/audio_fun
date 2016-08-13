#include "../header/SoundManager.h"
#include "../header/SoundObject.h"

#include "../header/OpenAL.h"

#include "../../Utils/header/Logger.h"
#include "../../Utils/header/VFS/VFS.h"
#include "../../Utils/header/Utils.h"

#include "../../MyMath/header/MyMathUtils.h"

using namespace MySound;

SoundManager * SoundManager::instance = NULL;

//http://ffainelli.github.io/openal-example/
//http://stackoverflow.com/questions/3894044/maximum-number-of-openal-sound-buffers-on-iphone
//http://code.google.com/p/djinnengine/source/browse/trunk/CPP+Classes/Sound+Manager/SoundController.cpp

//http://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/

SoundManager::SoundManager(const MyStringAnsi & deviceName, bool useThreadUpdate)
{
	this->deviceAL = NULL;
	this->contextAL = NULL;
	this->deviceName = deviceName;
	this->ended = false;
	this->useThreadUpdate = useThreadUpdate;

	this->masterVolume = 1.0f;
    this->lastVolume = 1.0f;

	this->enabled = true;

	this->Init();
}

SoundManager::~SoundManager()
{
	
	
	this->ended = true;

	if (this->useThreadUpdate)
	{
		pthread_join(this->updateThread, NULL);
	}
	
	std::unordered_map<MyStringAnsi, SoundObject *>::iterator it;
	for (it = this->sounds.begin(); it != this->sounds.end(); it++)
	{		
		it->second->Stop();
		SAFE_DELETE(it->second);
	}

	this->sounds.clear();


	for (uint32 i = 0; i < this->buffers.size(); i++)
	{
		AL_CHECK( alDeleteBuffers(1, &this->buffers[i].refID) );
	}

	for (uint32 i = 0; i < this->sources.size(); i++)
	{
		AL_CHECK( alDeleteSources(1, &this->sources[i].refID) );
	}

	alcDestroyContext(this->contextAL);
	alcCloseDevice(this->deviceAL);
	
}


void SoundManager::Initialize(const MyStringAnsi & deviceName, bool useThreadUpdate)
{
	if (instance == NULL)
	{
		instance = new SoundManager(deviceName, useThreadUpdate);
	}
}

void SoundManager::Destroy()
{
	SAFE_DELETE(instance);
}

SoundManager * SoundManager::GetInstance()
{
	return instance;
}

std::vector<MyStringAnsi> SoundManager::GetAllDevices()
{
	const char * devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);	
	const char * next = devices + 1;

    size_t len = 0;
	std::vector<MyStringAnsi> devicesNames; 

    while (devices && *devices != '\0' && next && *next != '\0') 
	{
		MyStringAnsi d = devices;
		devicesNames.push_back(d);                
        len = strlen(devices);
        devices += (len + 1);
		next += (len + 2);
	}
    return devicesNames;
}

bool SoundManager::IsEnabled()
{
	return this->enabled;
}

void SoundManager::Disable()
{
	if (this->enabled == false)
	{
		return;
	}

	this->enabled = false;
	this->lastVolume = this->masterVolume; //store last volume

	this->SetMasterVolume(0.0f);
}

void SoundManager::Enable()
{
	if (this->enabled)
	{
		return;
	}
	this->enabled = true;

	this->SetMasterVolume(this->lastVolume); //restore volume
}

void SoundManager::Init()
{
	//reset error stack
	alGetError();

	this->deviceAL = alcOpenDevice(NULL);

	if (this->deviceAL == NULL)
	{
		MyUtils::Logger::LogError("Failed to init OpenAL device.");
		return;
	}
	

	this->contextAL = alcCreateContext(this->deviceAL, NULL);
	AL_CHECK( alcMakeContextCurrent(this->contextAL) );


	for (int i = 0; i < 512; i++)
	{
		SoundBuffer buffer;
		AL_CHECK( alGenBuffers((ALuint)1, &buffer.refID) );
		this->buffers.push_back(buffer);
	}

	for (int i = 0; i < 16; i++)
	{
		SoundSource source;
		AL_CHECK( alGenSources((ALuint)1, &source.refID)) ;
		this->sources.push_back(source);
	}


	for (uint32 i = 0; i < this->buffers.size(); i++)
	{
		this->freeBuffers.push_back(&this->buffers[i]);
	}

	for (uint32 i = 0; i < this->sources.size(); i++)
	{
		this->freeSources.push_back(&this->sources[i]);
	}

	if (this->useThreadUpdate)
	{
		this->fakeMutex = PTHREAD_MUTEX_INITIALIZER;
		this->fakeCond = PTHREAD_COND_INITIALIZER;

		if(pthread_create(&this->updateThread, NULL, &SoundManager::UpdateThread, this)) 
		{	
			MyUtils::Logger::LogError("Error creating thread");
			return;
		}
	}
	


}


void * SoundManager::UpdateThread(void * c)
{
	
	SoundManager * context = ((SoundManager *)c);
	while (true)
	{
		
		context->ThreadUpdate();
		context->Wait(400);
		
		if (context->ended) 
		{
			break;
		}
	}
	
	return NULL;
}

void SoundManager::Wait(int timeInMS)
{
	//http://stackoverflow.com/questions/1486833/pthread-cond-timedwait-help
	
	struct timespec timeToWait;	
	MyUtils::Utils::WaitTime(timeInMS, &timeToWait.tv_sec, &timeToWait.tv_nsec);


	pthread_mutex_lock(&this->fakeMutex);
	pthread_cond_timedwait(&this->fakeCond, &this->fakeMutex, &timeToWait);
	pthread_mutex_unlock(&this->fakeMutex);
	//printf("\nDone\n");
}


void SoundManager::SetMasterVolume(float volume)
{
	this->masterVolume = MyMath::MyMathUtils::Clamp(0.0f, 1.0f, volume);
	AL_CHECK( alListenerf(AL_GAIN, this->masterVolume) );
}

void SoundManager::VolumeUp(float amount)
{
	this->SetMasterVolume(this->masterVolume + amount);
}

void SoundManager::VolumeDown(float amount)
{
	this->SetMasterVolume(this->masterVolume - amount);
}


SoundObject * SoundManager::GetSound(const MyStringAnsi & name)
{
	if (!this->ExistSound(name))
	{
		return NULL;
	}

	return this->sounds[name];
}

bool SoundManager::ExistSound(const MyStringAnsi & name) const
{
	if (this->sounds.find(name) == this->sounds.end())
	{
		return false;
	}

	return true;
}

void SoundManager::ReleaseSound(const MyStringAnsi & name)
{
	if (!this->ExistSound(name))
	{
		return;
	}

	this->sounds[name]->Stop();
	SAFE_DELETE(this->sounds[name]);
	this->sounds.erase(this->sounds.find(name));
}

void SoundManager::AddSound(const MyStringAnsi & fileName, const MyStringAnsi & name)
{
	if (this->ExistSound(name))
	{
		MyUtils::Logger::LogError("Sound with name %s already exist.", name.GetConstString());
		return;
	}

	SoundObject * sound = new SoundObject(fileName, name);
	this->sounds.insert(std::make_pair(name, sound));

	//sound->Play();
}

void SoundManager::AddSound(SoundObject * sound)
{

	MyStringAnsi name = sound->GetSettings().name;
	if (this->ExistSound(name))
	{
		MyUtils::Logger::LogError("Sound with name %s already exist.", name.GetConstString());
		return;
	}

	this->sounds.insert(std::make_pair(name, sound));

	//sound->Play();
}


SoundSource * SoundManager::GetFreeSource()
{
	if (this->freeSources.size() == 0)
	{
		return NULL;
	}

	SoundSource * source = this->freeSources.front();
	this->freeSources.pop_front();
	source->free = false;

	return source;
}


SoundBuffer * SoundManager::GetFreeBuffer()
{
	if (this->freeBuffers.size() == 0)
	{
		return NULL;
	}

	SoundBuffer * buf = this->freeBuffers.front();
	this->freeBuffers.pop_front();
	buf->free = false;

	return buf;
}


void SoundManager::FreeSource(SoundSource * source)
{
	if (source == NULL)
	{
		return;
	}
	source->free = true;
	this->freeSources.push_back(source);
}

void SoundManager::FreeBuffer(SoundBuffer * buffer)
{
	if (buffer == NULL)
	{
		return;
	}
   
    AL_CHECK( alDeleteBuffers(1, &buffer->refID) );
    AL_CHECK( alGenBuffers(1, &buffer->refID) );
    
	buffer->free = true;
	this->freeBuffers.push_back(buffer);	
}


void SoundManager::Update()
{
	if (this->useThreadUpdate == false)
	{
		this->ThreadUpdate();
	}
}

void SoundManager::ThreadUpdate()
{
	if (this->enabled == false)
	{
		//return;
	}

	std::unordered_map<MyStringAnsi, SoundObject *>::iterator it;

	for (it = this->sounds.begin(); it != this->sounds.end(); it++)
	{
		if (it->second->IsPlaying())
		{
			it->second->Update();
		}
	}
}