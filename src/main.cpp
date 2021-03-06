//EmulationStation, a graphical front-end for ROM browsing. Created by Alec "Aloshi" Lofquist.
//http://www.aloshi.com

#ifdef _RPI_
	#include <bcm_host.h>
#endif

///////////////////////////////////////////
#include "globals.h"

#include "MVC/Controller/AudioController.h"
#include "MVC/Controller/GamingController.h"
#include "MVC/Controller/InputController.h"
#include "MVC/Controller/LogController.h"
#include "MVC/Controller/RenderController.h"
#include "MVC/Controller/ResourceController.h"
#include "MVC/Controller/ScraperController.h"
#include "MVC/Controller/SettingsController.h"
#include "MVC/Controller/SocialController.h"

///////////////////////////////////////////

using namespace RetroEnd;

bool parseArgs(int argc, char* argv[])
{
	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			//TODO update settings
		}
	}

	return true;
}

//called on start
void onStart()
{
	//start log controller
	Controller::LogController::getInstance().start(); //first

#ifdef _DEBUG
	Controller::LogController::getInstance().setReportingLevel(LogLevel::Info); //TODO by settings or command line
#else
	Controller::LogController::getInstance().setReportingLevel(LogLevel::Error);
#endif
	LOG(LogLevel::Info,  "***** WELCOME TO RETRO-END *****");

	//start any other controllers
	//the order is mandatory
	Controller::RenderController::getInstance().start();
	Controller::SettingsController::getInstance().start();
	Controller::GamingController::getInstance().start();
	Controller::ResourceController::getInstance().start();
	Controller::ScraperController::getInstance().start();
	Controller::InputController::getInstance().start();
	Controller::AudioController::getInstance().start();
	Controller::SocialController::getInstance().start();
}

//called on exit
void onExit()
{
	#ifdef _RPI_
		bcm_host_deinit();
	#endif

	//stop any other controllers
	Controller::SocialController::getInstance().stop();
	Controller::SettingsController::getInstance().stop();
	Controller::ScraperController::getInstance().stop();
	Controller::GamingController::getInstance().stop();
	Controller::AudioController::getInstance().stop();
	Controller::InputController::getInstance().stop();
	Controller::RenderController::getInstance().stop();
	
	LOG(LogLevel::Info,  "***** RETRO-END IS CLOSING *****");

	//stop log controller
	Controller::LogController::getInstance().stop(); //last
}

int main(int argc, char* argv[])
{
	//activate controllers
	onStart();

	//start MANTA
	Controller::RenderController::getInstance().manta();

	//loop until stop
	while( Controller::RenderController::getInstance().isRunning() )
	{
		//parse and handle inputs
		Controller::InputController::getInstance().update();

		//update and render current window
		Controller::RenderController::getInstance().update();

		//update logs
		Controller::LogController::getInstance().update();
	}

	//stops any controller
	onExit();

	return EXIT_SUCCESS;
}
