#include "GamingController.h"

using namespace RetroEnd::Controller;
using namespace RetroEnd::Model;

void GamingController::start()
{
	LOG(LogLevel::Info, "\nGamingController is creating and updating all the tables.");
	LOG(LogLevel::Info, "Ignore any create or update error if new elements are already into the DB.");

	//init all the models so the tables are always updated
	Device::init();
	GameModel::init();

	LOG(LogLevel::Info, "GamingController updates end\n");
}