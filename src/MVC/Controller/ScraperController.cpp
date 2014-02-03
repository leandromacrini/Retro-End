#include "ScraperController.h"

#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#include <sstream>

#include <FreeImage.h>

#include "LogController.h"
#include "RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

#define BUFFERSIZE 1024*4 //4 kB

size_t callback(void *ptr, size_t size, size_t nmemb, void *vstring)
{
	std::string * pstring = (std::string*)vstring;
	pstring->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

size_t writetofilecallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

std::string DownloadUrlAsString(const std::string & url)
{
	std::string body;

	//CURLM* multi  = curl_multi_init( );

	CURL *curl_handle;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Retro-End Agent");
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &body);
	
	int* i = new int(1);

	//curl_multi_add_handle(multi, curl_handle);
	//curl_multi_perform(multi, i);

	CURLcode result = curl_easy_perform(curl_handle); 
	curl_easy_cleanup(curl_handle);

	if(result != 0)
	{
		LOG(LogLevel::Error, "ScraperController connection error. CURL error code: " + to_string(result));
		throw "Unable to retrieve data from TGDB.\nSee log for details.";
	}

	return body;
}

std::string DownloadUrlAsFile(const std::string & url, const std::string & path)
{

	FILE *fp;

	string file = path;
	file += strrchr( url.c_str(), '/' );

	fp = fopen(file.c_str(),"wb");

	CURL *curl_handle;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Retro-End Agent");
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writetofilecallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);
	CURLcode result = curl_easy_perform(curl_handle); 
	curl_easy_cleanup(curl_handle);

	if(result != 0)
	{
		LOG(LogLevel::Error, "ScraperController connection error. CURL error code: " + to_string(result));
		throw "Unable to retrieve data from TGDB. See log for details.";
	}

	fflush(fp);
	fclose(fp);

	//resize image
	FREE_IMAGE_FORMAT f = FreeImage_GetFileType(file.c_str());
	FIBITMAP* image = FreeImage_Load(f, file.c_str(), BMP_DEFAULT);

	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	//resize on width or height
	unsigned int Twidth = FreeImage_GetWidth(image);
	unsigned int Theight = FreeImage_GetHeight(image);

	int maxImageSide = (int)( H/3*2 );
	int Mheight = maxImageSide;
	int Mwidth  = maxImageSide;

	if( Twidth > Theight)
		Mheight = (int)(Theight * maxImageSide / Twidth);
	else
		Mwidth = (int)(Twidth * maxImageSide / Theight);

	FIBITMAP* resized = FreeImage_Rescale(image, Mwidth, Mheight, FILTER_BICUBIC);
	FreeImage_Save(f, resized, file.c_str(), BMP_DEFAULT);

	return file;
}

void ScraperController::ScrapeAllDevice()
{
	LOG(LogLevel::Info,  "Scraper is starting retrieving all the devices available ...\n");

	std::string data = DownloadUrlAsString("http://thegamesdb.net/api/GetPlatformsList.php");

	pugi::xml_document doc;

	doc.load(data.c_str());

	pugi::xml_node systemList = doc.child("Data").child("Platforms");

	for(pugi::xml_node system = systemList.child("Platform"); system; system = system.next_sibling("Platform"))
	{


		string tgdb_id = system.child("id").text().get();
		string name = system.child("name").text().get();

		LOG( LogLevel::Info, "Getting info for: " + name );

		std::string platData = DownloadUrlAsString("http://thegamesdb.net/api/GetPlatform.php?id=" + tgdb_id);

		pugi::xml_document platDoc;

		platDoc.load(platData.c_str());

		pugi::xml_node platform = platDoc.child("Data").child("Platform");

		Model::Device device;

		device.Name = platform.child("Platform").text().get();
		device.Description = platform.child("overview").text().get();
		device.Developer = platform.child("developer").text().get();
		device.Manufacturer = platform.child("manufacturer").text().get();
		device.Cpu = platform.child("cpu").text().get();
		device.Memory = platform.child("memory").text().get();
		device.Graphics = platform.child("graphics").text().get();
		device.Sound = platform.child("sound").text().get();
		device.Display = platform.child("display").text().get();
		device.MediaType = platform.child("media").text().get();
		device.MaxControllers = platform.child("maxcontrollers").text().get();
		device.TGDBId = platform.child("id").text().get();
		device.TGDBRating = platform.child("Rating").text().get();

		device.save();
		LOG(LogLevel::Info, name + " saved to db.");
	}
}

void ScraperController::ScrapeAllGamesForDevice(Model::Device& device, bool downloadImages, int maxCount)
{

	LOG(LogLevel::Info, "ScraperController is starting retrieving all the games info available for " + device.Name + "\n");

	std::string data = DownloadUrlAsString("http://thegamesdb.net/api/GetPlatformGames.php?platform=" + device.TGDBId);

	pugi::xml_document doc;
	doc.load(data.c_str());
	pugi::xml_node systemList = doc.child("Data");

	int counter = 0;

	for(pugi::xml_node system = systemList.child("Game"); system && counter < maxCount; system = system.next_sibling("Game"))
	{
		counter++;

		string tgdb_id = system.child("id").text().get();
		string title = system.child("GameTitle").text().get();

		LOG(LogLevel::Info, "ScraperController is getting info for: " + title);

		std::string platData = DownloadUrlAsString("http://thegamesdb.net/api/GetGame.php?id=" + tgdb_id);

		pugi::xml_document platDoc;

		platDoc.load(platData.c_str());

		pugi::xml_node gameData = platDoc.child("Data").child("Game");

		Model::Game game = Game(gameData, device.id);

		if(downloadImages)
		{
			string baseurl = platDoc.child("Data").child("baseImgUrl").text().get();

			//download box arts
			for(pugi::xml_node image = gameData.child("Images").child("boxart"); image; image = image.next_sibling("boxart"))
			{
				string url = baseurl;
				url += image.text().get();

				int res = strcmp( image.attribute("side").value(), "front");
				if( res == 0)
					game.ImageFront = DownloadUrlAsFile( url, "data/game screens/front" );

				res = strcmp( image.attribute("side").value(), "back");
				if( res == 0)
					game.ImageBack = DownloadUrlAsFile( url, "data/game screens/back" );

			}
			/*
			for(pugi::xml_node image = gameData.child("Images").child("clearlogo"); image; image = image.next_sibling())
			{
				string url = baseurl;
				url += image.text().get();

				game.imageLogo = DownloadUrlAsFile(url, "data/game screens/logo" );
				break;
			}
			*/
			//download screenshots
			for(pugi::xml_node image = gameData.child("Images").child("screenshot"); image; image = image.next_sibling())
			{
				string url = baseurl;
				url += image.child("original").text().get();

				game.ImageScreen = DownloadUrlAsFile(url, "data/game screens/screens" );
				break;
			}
		}
		game.save();

		LOG(LogLevel::Info, "ScraperController " + title + " saved to DB");
	}
}

//TODO deprecated (Remove?)
void ScraperController::ScrapeGameByCRC(Model::Game& game, Model::Device& device)
{
	//TODO FIX THIS?
	/*
	if(game.GameFile == "")
	{
		LOG(LogLevel::Error, "ScraperController unable to calculate CRC! File is not available for " + game.Title);
	}

	//CRC is supported only for "the archive db" (where there is less informations)
	boost::crc_32_type crc;

	std::ifstream ifs(game.GameFile, std::ios_base::binary);

	if( ifs )
	{
		do
		{
			char buffer[BUFFERSIZE]; 

			ifs.read( buffer, BUFFERSIZE );
			crc.process_bytes( buffer, (size_t)ifs.gcount() );
		}while( ifs );
	}
	else
	{
		LOG(LogLevel::Error, "ScraperController ScrapeGameByCRC unable to open file: " + game.GameFile);
	}

	std::stringstream stream;
	stream << hex << uppercase << crc.checksum();

	std::string data = DownloadUrlAsString("http://api.archive.vg/2.0/Game.getInfoByCRC/xml/7TTRM4MNTIKR2NNAGASURHJOZJ3QXQC5/"+stream.str());

	pugi::xml_document doc;
	doc.load(data.c_str());
	pugi::xml_node systemList = doc.child("OpenSearchDescription").child("games");

	for(pugi::xml_node system = systemList.child("game"); system; system = system.next_sibling("game"))
	{
		string tgdb_id = system.child("id").text().get();
		string title = system.child("title").text().get();

		std::cout << "Getting info for:" + title;
		//TODO read data from archive.db 
	}
	*/
}

//search a Game by name and device
void ScraperController::SearchGameData(Game& game, vector<string>& result)
{
	LOG(LogLevel::Info, "ScraperController is searching info for: " + game.GameFile);
	pugi::xml_document doc;

	Device device = Device::getDeviceById((int)game.DeviceId);

	string gameName = curl_escape(game.getCleanFileName().c_str(), 0);
	string platformName = curl_escape(device.Name.c_str(), device.Name.length());

	std::string gameData = DownloadUrlAsString("http://thegamesdb.net/api/GetGame.php?name=" + gameName + "&platform="+ platformName);
	doc.load(gameData.c_str());

	for(pugi::xml_node gameNode = doc.child("Data").child("Game"); gameNode; gameNode = gameNode.next_sibling("Game"))
	{
		stringstream ss;
		gameNode.print(ss);

		result.push_back(ss.str());
	}

	//TODO try to make this better
	//this is an hack for megadrive and genesis that are separated on TGDB
	//I really don't know why they did that!
	if(device.Name == "Sega Mega Drive")
	{
		gameData = DownloadUrlAsString("http://thegamesdb.net/api/GetGame.php?name=" + gameName + "&platform=Sega%20Genesis");
		pugi::xml_document genesisDoc;
		genesisDoc.load(gameData.c_str());

		for(pugi::xml_node gameNode = genesisDoc.child("Data").child("Game"); gameNode; gameNode = gameNode.next_sibling("Game"))
		{
			//check if already added to list
			bool found = false;
			for(pugi::xml_node alreadyPushedGN = doc.child("Data").child("Game"); alreadyPushedGN; alreadyPushedGN = alreadyPushedGN.next_sibling("Game"))
			{
				if( strcmp( gameNode.child("GameTitle").text().get() , alreadyPushedGN.child("GameTitle").text().get())  == 0)
				{
					found = true;
					break;
				}
			}

			if(! found)
			{
				stringstream ss;
				gameNode.print(ss);

				result.push_back(ss.str());
			}
		}
	}
}

void ScraperController::StoreGamedata( vector<pair<Model::Game, string>>* gameData )
{
	LOG(LogLevel::Info, "ScraperController is downloading images and saving games to DB");

	//for each new game founded
	for(auto it = gameData->begin(); it != gameData->end(); ++it)
	{
		Game tempGame = it->first;
		string xml= it->second;

		//if the user has choose a GameTitle we try to download image and create a complete Game record
		if( ! xml.empty() )
		{
			pugi::xml_document gameDoc;
			gameDoc.load(xml.c_str());

			Game newGame = Game(gameDoc.child("Game"), tempGame.DeviceId);

			//before go on we check if there is already the same game on db to avoid duplicates
			Game* duplicate = Game::getGameByTGDBId(newGame.TGDBId);
			if(duplicate != NULL)
			{
				newGame.id = duplicate->id;
				newGame.Favorite = duplicate->Favorite;
				newGame.TimesPlayed = duplicate->TimesPlayed;
			}

			newGame.GameFile = tempGame.GameFile;

			string baseurl = "http://thegamesdb.net/banners/";

			//download box arts
			for(pugi::xml_node image = gameDoc.child("Game").child("Images").child("boxart"); image; image = image.next_sibling("boxart"))
			{
				string url = baseurl;
				url += image.text().get();

				int res = strcmp( image.attribute("side").value(), "front");
				if( res == 0)
					newGame.ImageFront = DownloadUrlAsFile( url, "data/game screens/front" );

				res = strcmp( image.attribute("side").value(), "back");
				if( res == 0)
					newGame.ImageBack = DownloadUrlAsFile( url, "data/game screens/back" );

			}
			//download screenshots
			for(pugi::xml_node image = gameDoc.child("Game").child("Images").child("screenshot"); image; image = image.next_sibling())
			{
				string url = baseurl;
				url += image.child("original").text().get();

				newGame.ImageScreen = DownloadUrlAsFile(url, "data/game screens/screens" );
				break;
			}

			newGame.save();
			LOG(LogLevel::Info, "ScraperController " + newGame.Title + " saved to DB");
		}
		//else we create a "GameTitle = GameFile" Game record
		else
		{
			tempGame.Title = tempGame.GameFile;

			tempGame.save();
			LOG(LogLevel::Info, "ScraperController " + tempGame.Title + " saved to DB");
		}
	}
}