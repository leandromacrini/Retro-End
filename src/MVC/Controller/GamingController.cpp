#include "GamingController.h"



using namespace boost::filesystem;

using namespace RetroEnd::Controller;
using namespace RetroEnd::Model;

void GamingController::start()
{
	LOG(LogLevel::Info, "GamingController is creating and updating all the tables");

	//init all the models so the tables are always updated
	Device::init();
	Game::init();

	LOG(LogLevel::Info, "GamingController started.");
}

void GamingController::launchGame(Device& device, Game& game)
{
	string command = device.LaunchCommand;

	boost::filesystem::path gamePath(device.getRomsPath());

	gamePath /= game.GameFile;
	
	path prefGamePath = gamePath.make_preferred().native();

	string sp = prefGamePath.string();

	command = strreplace(command, "%ROM%", "\""+sp+"\"");

	if(command.empty())
	{
		LOG(LogLevel::Error, "Unable to start game: " + game.Title + " because Device: " + device.Name + " does't have a valid command!");
		RenderController::getInstance().pushPopupMessage("No valid command defined for this device!");
	}
	else
	{
		LOG(LogLevel::Info, "Starting game: " + game.Title);

		onGameStart(game);

		int exitCode = system(command.c_str());

		if(exitCode != 0)
		{
			LOG(LogLevel::Warning, "Game terminated with this nonzero exit code: " + to_string(exitCode)) ;
		}

		LOG(LogLevel::Info, "Ended game: " + game.Title);
		onGameEnd(game);

		//update number of times the game has been launched and the time
		game.TimesPlayed++;
		game.save();
	}	
}

//this function search for game files that are not associated with a Game in the DB
void GamingController::checkNewGamesAvailability(vector<Game>* result)
{
	LOG(LogLevel::Info, "GamingController is checking if new games was added");

	//for each device on DB
	vector<Device> devices = Device::getAllDevices();
	for (auto devIt = devices.begin() ; devIt != devices.end(); ++devIt)
	{
		vector<Game> games = Game::getGamesForDevice(devIt->id);

		path dir = path(devIt->getRomsPath());
		//for each file found in device's roms path
		for(directory_iterator dirIt(dir); dirIt != directory_iterator(); ++dirIt)
		{
			if(is_regular_file( *dirIt))
			{
				bool found = false;

				for(auto gameIt = games.begin(); gameIt != games.end(); ++gameIt)
				{
					path gamePath = dir / path(gameIt->GameFile);

					if(gamePath == dirIt->path())
					{
						found = true;
						break;
					}
				}

				if( !found )
				{
					//insert the new Game
					Game game = Game();
					game.GameFile = dirIt->path().filename().string();
					game.DeviceId = devIt->id;

					result->push_back(game);
					
					LOG(LogLevel::Info, "GamingController new game found: " + game.GameFile);
				}
			}
		}
	}
}