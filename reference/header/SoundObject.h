#ifndef SOUND_OBJECT_H
#define SOUND_OBJECT_H

namespace MySound 
{
	struct SoundSource;
	struct SoundBuffer;
	class ISoundFileWrapper;
}

struct VFS_FILE;

#define PRELOAD_BUFFERS_COUNT 3

#include "../../MyMath/header/Vector3.h"
#include "../../Utils/header/MyString.h"
#include "../../Macros.h"

namespace MySound 
{
	typedef struct SoundSettings
	{
			
		MyStringAnsi name;
		MyStringAnsi fileName;
		

		float pitch;
		float gain;
		bool loop;
		MyMath::Vector3 pos;
		MyMath::Vector3 velocity;


	} SoundSettings;

	typedef struct SoundInfo 
	{
		int freqency;
		int channels;
		int format;
		int bitsPerChannel;

		/* The below bitrate declarations are *hints*.
		 Combinations of the three values carry the following implications:

		 all three set to the same value:
		   implies a fixed rate bitstream
		 only nominal set:
		   implies a VBR stream that averages the nominal bitrate.  No hard
		   upper/lower limit
		 upper and or lower set:
		   implies a VBR bitstream that obeys the bitrate limits. nominal
		   may also be set to give a nominal rate.
		 none set:
		   the coder does not care to speculate.
	  */

	  long bitrate_upper;
	  long bitrate_nominal;
	  long bitrate_lower;
	  long bitrate_window;

      bool seekable;

	} SoundInfo;
	
	class SoundObject
	{
		public:

			typedef enum SOUND_STATE { PLAYING = 1, PAUSED = 2, STOPPED = 3} SOUND_STATE; 

			SoundObject(const MyStringAnsi & fileName, const MyStringAnsi & name);
			SoundObject(const SoundSettings & settings);
			SoundObject(char * rawData, uint32 dataSize, SoundInfo soundInfo, const MyStringAnsi & name);
			~SoundObject();

			const SoundSettings & GetSettings() const;
			const SoundInfo & GetInfo() const;

			void PlayInLoop(bool val);

			float GetTime() const;
			float GetMaxBufferedTime() const;
			int GetPlayedCount() const;

			bool IsPlaying() const;

			void Release();

			void Play();
			void Pause();
			void Stop();
	
            void Rewind();
        
			
			void GetRawData(std::vector<char> * rawData);

			template <typename T>
			void GetRawDataNormalized(std::vector<T> * rawData);
			

			friend class SoundManager;
	
		protected:

			SoundSource * source;
			SoundBuffer * buffers[PRELOAD_BUFFERS_COUNT];
			int activeBufferID;
			int SINGLE_BUFFER_SIZE;
			
			SoundSettings settings;
			SoundInfo soundInfo;
			
			VFS_FILE * vfsFile;
			char * soundData;
			int dataSize;
			ISoundFileWrapper * soundFileWrapper;

			SOUND_STATE state;

			int remainBuffers;
			

			int playedCount;

			void LoadData();
			void LoadRawData(char * rawData, uint32 dataSize);

			bool Preload();
			bool PreloadBuffer(int bufferID);
			void Update();

			bool spinLock;
			
			float GetBufferedTime(int buffersCount) const;

	};
}


#endif