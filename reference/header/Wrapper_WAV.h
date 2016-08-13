#ifndef WRAPPER_WAV_H
#define WRAPPER_WAV_H



namespace MySound
{
	struct SoundInfo;
}


#define WAV_BUFFER_SIZE 2048

#include <vector>
#include "../../Macros.h"
#include <cstring>

#include "./ISoundFileWrapper.h"

typedef struct wav_file
{
	FILE * f;
	char* curPtr;
	char* filePtr;
	size_t fileSize;
	size_t processedSize;

} wav_file;

typedef struct WAV_DESC
{
	uint8 riff[4];
	uint64 size;
	uint8 wave[4];

} WAV_DESC;

typedef struct WAV_FORMAT
{
	uint8 id[4];
	uint64 size;
	uint16 format;
	uint16 channels;
	uint64 sampleRate;
	uint64 byteRate;
	uint16 blockAlign;
	uint16 bitsPerSample;

} WAV_FORMAT;

typedef struct WAV_CHUNK
{
	uint8 id[4];
	uint64 size;

} WAV_CHUNK;

namespace MySound
{

	class WrapperWav : public ISoundFileWrapper
	{
		public:
			WrapperWav(int minDecompressLengthAtOnce = -1);
			~WrapperWav();

			virtual void LoadFromMemory(char * data, int dataSize, SoundInfo * soundInfo);
			virtual void LoadFromFile(FILE * f, SoundInfo * soundInfo);
			virtual void DecompressStream(std::vector<char> & decompressBuffer, bool inLoop = false);
			virtual void DecompressAll(std::vector<char> & decompressBuffer);
			virtual void ResetStream();

			virtual void Seek(size_t pos, SEEK_POS start);
			virtual size_t GetCurrentStreamPos() const;

			virtual float GetTime() const;
			virtual float GetTotalTime() const;

		private:
			wav_file t;
			char bufArray[WAV_BUFFER_SIZE];
			

			int minProcesssLengthAtOnce;

			WAV_DESC desc;
			WAV_FORMAT format;
			
			WAV_CHUNK curChunk;
			int curBufSize;
			
			void ReadData(void * dst, size_t size);
			

	};
}


#endif