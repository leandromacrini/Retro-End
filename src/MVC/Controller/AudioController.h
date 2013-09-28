/* Audio Controller
* 
* Allow access to SDL sound system
*/

#pragma once

#include "../../globals.h"

#include "SDL_audio.h"

#include "../Model/Sound.h"
#include "BaseController.h"

using namespace std;

using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class AudioController : public BaseController
		{

		public:
			static AudioController& getInstance()
			{
				static AudioController instance;
				return instance;
			}

			void start();
			void stop();

			void registerSound(shared_ptr<Model::Sound> & sound);
			void unregisterSound(shared_ptr<Model::Sound> & sound);

			void playAllSounds();
			void stopAllSounds();

		protected:

		private:
			static SDL_AudioSpec sAudioFormat;
			static vector<shared_ptr<Model::Sound>> sSoundVector;

			static void mixAudio(void *unused, Uint8 *stream, int len);

			AudioController mInstance();
			AudioController() {}; //private instance costructor for Singleton Controller
			AudioController(AudioController const&);// Don't Implement
			void operator=(AudioController const&); // Don't implement
		};
	}
}
