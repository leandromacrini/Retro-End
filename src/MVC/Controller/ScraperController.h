/* Base Model class
 *
 * Create the interface for access to any Model
 * Create the SQLite DB data layer
 */

#ifndef _SCRAPERCONTROLLER_H_
#define _SCRAPERCONTROLLER_H_

#include <curl.h>

#include "../../globals.h"

#include "BaseController.h"
#include "../Model/DeviceModel.h"
#include "../Model/GameModel.h"

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
			void ScrapeAllGamesForDevice(Model::DeviceModel device);

			void ScrapeGameByCRC(Model::GameModel game);

		protected:

		private:
			ScraperController mInstance();
			ScraperController() {}; //private instance costructor for Singleton Controller
			ScraperController(ScraperController const&);// Don't Implement
			void operator=(ScraperController const&); // Don't implement
		};
	}
}

#endif