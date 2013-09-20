//EmulationStation, a graphical front-end for ROM browsing. Created by Alec "Aloshi" Lofquist.
//http://www.aloshi.com

#ifdef _RPI_
	#include <bcm_host.h>
#endif

///////////////////////////////////////////
#include "globals.h"

#include "MVC/Controller/SettingsController.h"
#include "MVC/Controller/GamingController.h"
#include "MVC/Controller/ScraperController.h"
#include "MVC/Controller/InputController.h"
#include "MVC/Controller/RenderController.h"

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
	Controller::LogController::getInstance().setReportingLevel(LogLevel::Debug);

	LOG(LogLevel::Info,  "***** WELCOME TO RETRO-END *****\n\n");

	//start any other controllers
	//the order is mandatory
	Controller::SettingsController::getInstance().start();
	Controller::GamingController::getInstance().start();
	Controller::ScraperController::getInstance().start();
	Controller::RenderController::getInstance().start();
	Controller::InputController::getInstance().start();

}

//called on exit
void onExit()
{
	#ifdef _RPI_
		bcm_host_deinit(); //TODO MOVE ???
	#endif

	//stop any other controllers
	Controller::SettingsController::getInstance().stop();
	Controller::InputController::getInstance().stop();
	Controller::ScraperController::getInstance().stop();
	Controller::GamingController::getInstance().stop();
	Controller::RenderController::getInstance().stop();
	
	LOG(LogLevel::Info,  "/n/n***** RETRO-END IS CLOSING *****");

	//stop log controller
	Controller::LogController::getInstance().stop(); //last
}

int main(int argc, char* argv[])
{
	//parse the arguments
	if( ! parseArgs( argc, argv) ) return 0;

	//starts any controller
	onStart();

	//loop until stop
	while( Controller::RenderController::getInstance().isRunning() )
	{
		//handle inputs
		//Controller::InputController::getInstance().update();

		//update the main window
		Controller::RenderController::getInstance().update();
	}

	//stops any controller
	onExit();

	return 0;
}
