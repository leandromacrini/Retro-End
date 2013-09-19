/* Gaming Controller
 *
 */

#ifndef _GAMINGCONTROLLER_H_
#define _GAMINGCONTROLLER_H_

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
		class GamingController : public BaseController
		{

		public:
			static GamingController& getInstance()
			{
				static GamingController instance;
				return instance;
			}
			void start();

		protected:

		private:
			GamingController mInstance();
			GamingController() {}; //private instance costructor for Singleton Controller
			GamingController(GamingController const&);// Don't Implement
			void operator=(GamingController const&); // Don't implement
		};
	}
}

#endif _GAMINGCONTROLLER_H_