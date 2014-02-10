#include "LogController.h"

#include "../../LIB/pugiXML/pugixml.hpp"
#include <boost/filesystem.hpp>
#include <ctime>
#include <iostream>

using namespace RetroEnd::Controller;

#define LOG_PATH "data/log"
#define LOG_FILE "data/log/Retro-End.log"

const string logLevelToString(LogLevel level)
{
	switch (level)
	{
	case Fatal:
		return " Fatal ";
	case Error:
		return " Error ";
	case Warning:
		return "Warning";
	case Debug:
		return " Debug ";
	case Info:
	default:
		return " Info  ";
	}
}

//padding time to 2 char
const string p2z(int input)
{
	string result = to_string(input);
	if(input < 10) result = "0" + result;
	return result;
}

const string timeToString()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);

	return p2z(ltm->tm_hour) + ":" + p2z(ltm->tm_min) + ":" + p2z(ltm->tm_sec);
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
		std::cout << output;
	}
}

void LogController::start()
{
	//the log folder must always exists
	boost::filesystem::create_directory(LOG_PATH);

	mLogFile = fopen(LOG_FILE, "w");

	if(mLogFile == NULL)
	{
		LOG(LogLevel::Error,  "LogController start Error! Unable to open Retro-End.log file for write!");
		throw "LogController start Error! Unable to open Retro-End.log file for write!";
	}

	LOG(LogLevel::Info,  "LogController started");
}

void LogController::stop()
{
	fflush(mLogFile);
	fclose(mLogFile);
}

void LogController::update()
{
	fflush(mLogFile);
}