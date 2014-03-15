/* Audio Controller
* 
* Allow access to SDL sound system
*/

#pragma once

#include "../../globals.h"

#ifdef _WINDOWS
#include "SDL_audio.h"
#else
#include "SDL2/SDL_audio.h"
#endif

#include "../Model/Sound.h"
#include "BaseController.h"
#include "LogController.h"
#include "GamingController.h"

using namespace std;

using namespace RetroEnd;
using namespace RetroEnd::Model;

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

			shared_ptr<Sound> createSound(string file);
			bool SoundsEnabled;

			void activate();
			void deactivate();

			friend class Sound;

		private:
			void init();
			void quit();
			void registerSound(shared_ptr<Sound> & sound);
			void unregisterSound(shared_ptr<Sound> & sound);

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
