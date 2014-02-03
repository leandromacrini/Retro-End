/* Scraper Controller
 * 
 * Retrieve devices and games data from The Archive e The Games DB
 */

#pragma once

#ifdef WIN32
	#include <curl.h>
#else
	#include <curl/curl.h>
#endif

#include "../../LIB/pugiXML/pugixml.hpp"

#include "../../globals.h"

#include "BaseController.h"
#include "../Model/Device.h"
#include "../Model/Game.h"

using namespace std;

using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class ScraperController : public BaseController
		{

		public:
			static ScraperController& getInstance()
			{
				static ScraperController instance;
				return instance;
			}

			void ScrapeAllDevice();
			void ScrapeAllGamesForDevice(Model::Device& device, bool downloadImages = true, int maxCount = 999999999);

			void ScrapeGameByCRC(Model::Game& game, Model::Device& device);
			
			void SearchGameData(Model::Game& game, vector<string>& result);
			void StoreGamedata( vector<pair<Model::Game, string>>* gameData );

		protected:

		private:
			ScraperController mInstance();
			ScraperController() {}; //private instance costructor for Singleton Controller
			ScraperController(ScraperController const&);// Don't Implement
			void operator=(ScraperController const&); // Don't implement
		};
	}
}
