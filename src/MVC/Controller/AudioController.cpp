#include "AudioController.h"

#include <SDL.h>
#include "LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

std::vector<std::shared_ptr<Sound>> AudioController::sSoundVector;
SDL_AudioSpec AudioController::sAudioFormat;


void AudioController::mixAudio(void *unused, Uint8 *stream, int len)
{
	bool stillPlaying = false;

	//iterate through all our samples
	std::vector<std::shared_ptr<Sound>>::const_iterator soundIt = sSoundVector.cbegin();
	while (soundIt != sSoundVector.cend())
	{
		std::shared_ptr<Sound> sound = *soundIt;
		if(sound->isPlaying())
		{
			//calculate rest length of current sample
			Uint32 restLength = (sound->getLength() - sound->getPosition());
			if (restLength > (Uint32)len) {
				//if stream length is smaller than smaple lenght, clip it
				restLength = len;
			}
			//mix sample into stream
			SDL_MixAudio(stream, &(sound->getData()[sound->getPosition()]), restLength, SDL_MIX_MAXVOLUME);
			if (sound->getPosition() + restLength < sound->getLength())
			{
				//sample hasn't ended yet
				stillPlaying = true;
			}
			//set new sound position. if this is at or beyond the end of the sample, it will stop automatically
			sound->setPosition(sound->getPosition() + restLength);
		}
		//advance to next sound
		++soundIt;
	}
	//we have processed all samples. check if some will still be playing
	if (!stillPlaying) {
		//no. pause audio till a Sound::play() wakes us up
		SDL_PauseAudio(1);
	}
}

void AudioController::start()
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
	{
		LOG(LogLevel::Error, "Error initializing SDL audio! Error:" + string(SDL_GetError()));
		return;
	}

	//stop playing all Sounds
	for(unsigned int i = 0; i < sSoundVector.size(); i++)
	{
		if(sSoundVector.at(i)->isPlaying())
		{
			sSoundVector[i]->stop();
		}
	}

	//Set up format and callback. Play 16-bit stereo audio at 44.1Khz
	sAudioFormat.freq = 44100;
	sAudioFormat.format = AUDIO_S16;
	sAudioFormat.channels = 2;
	sAudioFormat.samples = 1024;
	sAudioFormat.callback = mixAudio;
	sAudioFormat.userdata = NULL;

	//Open the audio device and pause
	if (SDL_OpenAudio(&sAudioFormat, NULL) < 0) {
		LOG(LogLevel::Error, "AudioController Error - Unable to open SDL audio: " + string( SDL_GetError() ));
	}
}

void AudioController::stop()
{
	//stop all playback
	stopAllSounds();
	//completely tear down SDL audio. else SDL hogs audio resources and emulators might fail to start...
	SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioController::registerSound(std::shared_ptr<Sound> & sound)
{
	getInstance();
	sSoundVector.push_back(sound);
}

void AudioController::unregisterSound(std::shared_ptr<Sound> & sound)
{
	getInstance();
	for(unsigned int i = 0; i < sSoundVector.size(); i++)
	{
		if(sSoundVector.at(i) == sound)
		{
			sSoundVector[i]->stop();
			sSoundVector.erase(sSoundVector.begin() + i);
			return;
		}
	}
	LOG(LogLevel::Error, "AudioController Error - tried to unregister a sound that wasn't registered!");
}

void AudioController::playAllSounds()
{
	getInstance();

	//set internal audio volume. important after launching a game and returning here
	//VolumeControl::getInstance()->setVolume(50);

	//unpause audio, the mixer will figure out if samples need to be played...
	SDL_PauseAudio(0);
}

void AudioController::stopAllSounds()
{
	//stop playing all Sounds
	for(unsigned int i = 0; i < sSoundVector.size(); i++)
	{
		if(sSoundVector.at(i)->isPlaying())
		{
			sSoundVector[i]->stop();
		}
	}
	//pause audio
	SDL_PauseAudio(1);
}
