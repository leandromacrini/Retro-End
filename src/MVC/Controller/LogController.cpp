#include "LogController.h"

#include "../../LIB/pugiXML/pugixml.hpp"
#include <boost/filesystem.hpp>
#include <ctime>
#include <iostream>

using namespace RetroEnd::Controller;

const string logLevelToString(LogLevel level)
{
	switch (level)
	{
	case Fatal:
		return "Fatal";
	case Error:
		return "Error";
	case Warning:
		return "Warning";
	case Info:
		return "Info";
	case Debug:
		return "Debug";
	default:
		return "Info";
	}
}

const string timeToString()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);

	return to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
}

LogLevel LogController::getReportingLevel()
{
	return mReportingLevel;
}

void LogController::setReportingLevel(LogLevel level)
{
	mReportingLevel = level;
}

void LogController::log(LogLevel level, const string& message)
{
	if(level <= mReportingLevel)
	{
		string output = "[ " + timeToString() + " - " + logLevelToString(level) + " ] - " + message + "\n";

		//log to file
		fprintf(mLogFile, "%s", output.c_str());

		//if debug print to cout too
		if(mReportingLevel >= LogLevel::Debug)
		{
			std::cout << output;
		}
	}
}

void LogController::start()
{
	mLogFile = fopen("data/log/Retro-End.log", "w");

	if(mLogFile == NULL)
	{
		LOG(LogLevel::Error,  "LogController start Error! Unable to open Retro-End.log file for write!");
		return;
	}
	
	LOG(LogLevel::Info,  "LogController started.");
}

void LogController::stop()
{
	fflush(mLogFile);
	fclose(mLogFile);

	LOG(LogLevel::Info,  "LogController stopped.");
}

void LogController::update()
{
	fflush(mLogFile);
}