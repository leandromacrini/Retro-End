#pragma once

#include <string>
#include "SDL_audio.h"

namespace RetroEnd
{
	namespace Controller
	{
		class AudioController;
	}

	namespace Model
	{
		class Sound
		{
		public:
			~Sound();


			void play();
			bool isPlaying() const;
			void stop();

			friend class Controller::AudioController;
		private:
			Sound(const std::string & path = "");

			const Uint8 * getData() const;
			Uint32 getPosition() const;
			void setPosition(Uint32 newPosition);
			Uint32 getLength() const;
			Uint32 getLengthMillis() const;

			std::string mPath;
			SDL_AudioSpec mSampleFormat;
			Uint8 * mSampleData;
			Uint32 mSamplePos;
			Uint32 mSampleLength;
			bool playing;
			
	
		};
	}
}