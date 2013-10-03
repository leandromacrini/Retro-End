/* Settings Controller class
 *
 * Contains all the application's settings
 */

#pragma once

#include <sqlite3.h>

#include "../../globals.h"

#include "BaseController.h"
#include "LogController.h"

using namespace std;
using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class SettingsController : public BaseController
		{
		public:
			static SettingsController& getInstance()
			{
				static SettingsController instance;
				return instance;
			}

			void start();
			void stop();

			bool	getProperty(const string& name, bool defaultValue);
			float	getProperty(const string& name, float defaultValue);
			string	getProperty(const string& name, const string& defaultValue);

			void	setProperty(const string& name, bool value);
			void	setProperty(const string& name, float value);
			void	setProperty(const string& name, const string& value);

		protected:

		private:
			//---Singleton---
			SettingsController mInstance();

			SettingsController() {}; //private instance costructor for Singleton Controller
			SettingsController(SettingsController const&);// Don't Implement
			void operator=(SettingsController const&); // Don't implement
			//---Singleton---
		};
	}
}
