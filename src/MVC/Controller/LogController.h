/* LogController class
 *
 * Create the interface for access the Log system
 */

#pragma once

#include "../../globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iostream>
#include <mutex>

#include "BaseController.h"

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

			void start();
			void stop();
			void update();

			LogLevel getReportingLevel();
			void setReportingLevel(LogLevel level);

			void log(LogLevel level, const string& message);

		protected:

		private:
			recursive_mutex mMutex;
			LogLevel mReportingLevel;
			FILE* mLogFile;

			//---Singleton---
			LogController mInstance();

			LogController() : mLogFile(NULL), mReportingLevel(LogLevel::Info) {}; //private instance costructor for Singleton Controller
			LogController(LogController const&);// Don't Implement
			void operator=(LogController const&); // Don't implement
			//---Singleton---
		};
	}
}
