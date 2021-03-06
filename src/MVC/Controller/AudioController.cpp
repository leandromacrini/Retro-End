#include "AudioController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

std::vector<std::shared_ptr<Sound>> AudioController::sSoundVector;
SDL_AudioSpec AudioController::sAudioFormat;


void AudioController::mixAudio(void *unused, Uint8 *stream, int len)
{
	bool stillPlaying = false;

	//initialize the buffer to "silence"
	SDL_memset(stream, 0, len);

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


void AudioController::init()
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

	SoundsEnabled = true;
}

void AudioController::quit()
{
	//stop all playback
	deactivate();
	//completely tear down SDL audio. else SDL hogs audio resources and emulators might fail to start...
	SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioController::start()
{
	GamingController::getInstance().onGameStart += [this](Game& game)
	{
		quit();
	};

	GamingController::getInstance().onGameEnd += [this](Game& game)
	{
		init();
	};

	init();

	LOG(LogLevel::Info, "AudioController started.");
}

void AudioController::stop()
{
	quit();

	LOG(LogLevel::Info, "AudioController stopped.");
}

void AudioController::registerSound(std::shared_ptr<Sound> & sound)
{
	sSoundVector.push_back(sound);
}

void AudioController::unregisterSound(std::shared_ptr<Sound> & sound)
{
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

void AudioController::activate()
{
	BaseController::activate();

	//set internal audio volume. important after launching a game and returning here
	//VolumeControl::getInstance()->setVolume(50);

	//unpause audio, the mixer will figure out if samples need to be played...
	SDL_PauseAudio(0);
}

void AudioController::deactivate()
{
	BaseController::deactivate();

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

shared_ptr<Model::Sound> AudioController::createSound(string file)
{
	shared_ptr<Sound> sound(new Sound(file));

	registerSound(sound);

	return sound;
}