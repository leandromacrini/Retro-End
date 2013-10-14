#include "GamingController.h"

using namespace RetroEnd::Controller;
using namespace RetroEnd::Model;

void GamingController::start()
{
	LOG(LogLevel::Info, "GamingController is creating and updating all the tables.");

	//init all the models so the tables are always updated
	Device::init();
	Game::init();

	LOG(LogLevel::Info, "GamingController started.");
}