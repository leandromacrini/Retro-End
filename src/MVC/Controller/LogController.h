/* LogController class
 *
 * Create the interface for access the Log system
 */

#ifndef _LOGCONTROLLER_H_
#define _LOGCONTROLLER_H_

#include "../../globals.h"

using namespace std;
using namespace RetroEnd;

#define LOG(level, message) Controller::LogController::getInstance().log(level, message);

enum LogLevel
{
	Fatal,
	Error,
	Warning,
	Info,
	Debug
};

namespace RetroEnd
{
	namespace Controller
	{
		class LogController : public BaseController
		{

		public:
			static LogController& getInstance()
			{
				static LogController instance;
				return instance;
			}

			LogLevel getReportingLevel();
			void setReportingLevel(LogLevel level);

			void log(LogLevel level, const string& message);

		protected:

		private:
			LogLevel mReportingLevel;

			//---Singleton---
			LogController mInstance();

			LogController() {}; //private instance costructor for Singleton Controller
			LogController(LogController const&);// Don't Implement
			void operator=(LogController const&); // Don't implement
			//---Singleton---
		};
	}
}

#endif _LOGCONTROLLER_H_