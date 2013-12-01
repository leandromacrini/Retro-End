#include "GamingController.h"

#include "SettingsController.h"
#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;
namespace alg = boost::algorithm;

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
	
	fs::path prefGamePath = gamePath.make_preferred().native();

	string sp = prefGamePath.string();

	command = strreplace(command, "%ROM%", "\""+sp+"\"");

	if(command.empty())
	{
		LOG(LogLevel::Error, "Unable to start game: " + game.Title + " because Device: " + device.Name + " does't have a valid command!");
		onGameError("No valid command defined for this device!");
	} else if(game.GameFile.empty())
	{
		LOG(LogLevel::Error, "Unable to start game: " + game.Title + " because the game's rom is not available!");
		onGameError("No valid rom found for this game!");
	}
	else
	{
		LOG(LogLevel::Info, "Starting game: " + game.Title);

		onGameStart(game);

		//we must stop our SDL here
		RenderController::getInstance().quitSDL();

		int exitCode = system(command.c_str());

		if(exitCode != 0)
		{
			LOG(LogLevel::Warning, "Game terminated with this nonzero exit code: " + to_string(exitCode)) ;
		}

		//we must start SDL here so the reload of resources is done before all start to use those
		RenderController::getInstance().initSDL();

		LOG(LogLevel::Info, "Ended game: " + game.Title);
		onGameEnd(game);

		//update number of times the game has been launched and the time
		game.TimesPlayed++;
		game.save();
	}	
}

//this function search for game files that are no more present on the disk
void GamingController::checkOldGamesAvailability(vector<Game>& result)
{
	LOG(LogLevel::Info, "GamingController is checking if some game's rom has been removed");

	vector<Game> games = Game::getAllGames();

	for(auto it = games.begin(); it != games.end(); ++it)
	{


		if( ! it->GameFile.empty() && it->DeviceId != 0)
		{
			//get GameFile path
			string romPath = Device::getDeviceById(it->DeviceId).getRomsPath();
			//get Images path

			//delete rom file
			fs::path file = romPath / fs::path(it->GameFile);
			if( ! fs::exists(file)) result.push_back(*it);
		}
	}
}

//this function search for game files that are not associated with a Game in the DB
void GamingController::checkNewGamesAvailability(vector<Game>& result)
{
	LOG(LogLevel::Info, "GamingController is checking if new games was added");

	string setting = SettingsController::getInstance().getStringProperty("Supported Extensions", "");

	set<string> validExtensions;
	alg::split(validExtensions, setting, boost::is_any_of(" "));

	//for each device on DB
	vector<Device> devices = Device::getAllDevices();
	for (auto devIt = devices.begin() ; devIt != devices.end(); ++devIt)
	{
		vector<Game> games = Game::getGamesForDevice(devIt->id);

		fs::path dir = fs::path(devIt->getRomsPath());
		//for each file found in device's roms path
		for(fs::directory_iterator dirIt(dir); dirIt != fs::directory_iterator(); ++dirIt)
		{
			//if is a file and it's extensions is supported
			if( ! is_regular_file( *dirIt) ) continue;
			
			string fileExtension = alg::to_lower_copy(fs::extension(*dirIt));

			if(validExtensions.find( fileExtension ) != validExtensions.end())
			{
				bool found = false;

				for(auto gameIt = games.begin(); gameIt != games.end(); ++gameIt)
				{
					fs::path gamePath = dir / fs::path(gameIt->GameFile);

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

					result.push_back(game);
					
					LOG(LogLevel::Info, "GamingController new game found: " + game.GameFile);
				}
			}
		}
	}
}