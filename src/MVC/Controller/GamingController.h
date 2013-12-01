/* Gaming Controller
 *
 */

#pragma once

#include <curl.h>

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

			void launchGame(Model::Device& device, Model::Game& game);
			
			void checkNewGamesAvailability(vector<Model::Game>& result);
			void checkOldGamesAvailability(vector<Model::Game>& result);

			//EVENTS

			Model::Observer<Model::Game&> onGameStart;
			Model::Observer<Model::Game&> onGameEnd;
			Model::Observer<const string&> onGameError;

		protected:

		private:
			GamingController mInstance();
			GamingController() {}; //private instance costructor for Singleton Controller
			GamingController(GamingController const&);// Don't Implement
			void operator=(GamingController const&); // Don't implement
		};
	}
}
