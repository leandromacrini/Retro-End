/* Gaming Controller
 *
 */

#pragma once

#ifdef WIN32
	#include <curl.h>
#else
	#include <curl/curl.h>
#endif

#include "../../globals.h"

#include "../Model/Device.h"
#include "../Model/Game.h"
#include "../Model/Observer.h"

#include "BaseController.h"
#include "RenderController.h"
#include "LogController.h"

using namespace std;

using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class GamingController : public BaseController
		{

		public:
			static GamingController& getInstance()
			{
				static GamingController instance;
				return instance;
			}
			void start();

			void launchSingleGame(Model::Device& device, Model::Game& game);
			void launchNetplayGameServer(Model::Device& device, Model::Game& game, int port);
			void launchNetplayGameClient(Model::Device& device, Model::Game& game, int port, string& server);
			
			void checkNewGamesAvailability(vector<Model::Game>& result);
			void checkOldGamesAvailability(vector<Model::Game>& result);

			const bool IsPlaying();

			//EVENTS

			Model::Observer<Model::Game&> onGameStart;
			Model::Observer<Model::Game&> onGameEnd;
			Model::Observer<const string&> onGameError;

		protected:

		private:
			void launchRetroarch(string& command, Model::Game);
			bool mIsPlaying;

			GamingController mInstance();
			GamingController() : mIsPlaying(false) { }; //private instance costructor for Singleton Controller
			GamingController(GamingController const&);// Don't Implement
			void operator=(GamingController const&); // Don't implement
		};
	}
}
