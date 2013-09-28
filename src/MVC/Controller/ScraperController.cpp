#include "ScraperController.h"

#include "../../LIB/pugiXML/pugixml.hpp"
#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#include <sstream>

using namespace RetroEnd::Controller;

#define BUFFERSIZE 1024*4 //4 kB
	
size_t callback(void *ptr, size_t size, size_t nmemb, void *vstring)
{
    std::string * pstring = (std::string*)vstring;
    pstring->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

std::string DownloadUrlAsString(const std::string & url)
{
    std::string body;

    CURL *curl_handle;
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Retro-End Agent");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &body);
    curl_easy_perform(curl_handle); 
    curl_easy_cleanup(curl_handle);

    return body;
}

void ScraperController::ScrapeAllDevice()
{
	std::cout << "Scraper is starting:\n";

	std::cout << "Get All supported platforms ...\n\n";
	std::string data = DownloadUrlAsString("http://thegamesdb.net/api/GetPlatformsList.php");

	pugi::xml_document doc;

	doc.load(data.c_str());

	pugi::xml_node systemList = doc.child("Data").child("Platforms");

	for(pugi::xml_node system = systemList.child("Platform"); system; system = system.next_sibling("Platform"))
	{
		

		string tgdb_id = system.child("id").text().get();
		string name = system.child("name").text().get();

		std::cout << "getting info for:" + name + "\n";

		std::string platData = DownloadUrlAsString("http://thegamesdb.net/api/GetPlatform.php?id=" + tgdb_id);

		pugi::xml_document platDoc;

		platDoc.load(platData.c_str());

		pugi::xml_node platform = platDoc.child("Data").child("Platform");

		Model::Device device;

		device.name = platform.child("Platform").text().get();
		device.description = platform.child("overview").text().get();
		device.developer = platform.child("developer").text().get();
		device.manufacturer = platform.child("manufacturer").text().get();
		device.cpu = platform.child("cpu").text().get();
		device.memory = platform.child("memory").text().get();
		device.graphics = platform.child("graphics").text().get();
		device.sound = platform.child("sound").text().get();
		device.display = platform.child("display").text().get();
		device.mediaType = platform.child("media").text().get();
		device.maxControllers = platform.child("maxcontrollers").text().get();
		device.tgbdId = platform.child("id").text().get();
		device.tgbdRating = platform.child("Rating").text().get();

		device.save();
		std::cout << name + " saved to db\n";
	}
}

void ScraperController::ScrapeAllGamesForDevice(Model::Device device)
{
	std::cout << "Scraper is starting:\n";

	std::cout << "Get All Games for "<<device.name<<"\n\n";
	std::string data = DownloadUrlAsString("http://thegamesdb.net/api/GetPlatformGames.php?platform=" + device.tgbdId);

	pugi::xml_document doc;

	doc.load(data.c_str());

	pugi::xml_node systemList = doc.child("Data");

	for(pugi::xml_node system = systemList.child("Game"); system; system = system.next_sibling("Game"))
	{
		string tgdb_id = system.child("id").text().get();
		string title = system.child("GameTitle").text().get();

		std::cout << "getting info for:" + title;

		std::string platData = DownloadUrlAsString("http://thegamesdb.net/api/GetGame.php?id=" + tgdb_id);

		pugi::xml_document platDoc;

		platDoc.load(platData.c_str());

		pugi::xml_node platform = platDoc.child("Data").child("Game");

		Model::GameModel game;

		game.title = platform.child("GameTitle").text().get();
		game.deviceId = device.id;
		game.description = platform.child("Overview").text().get();
		game.ESRB = platform.child("ESRB").text().get();
		game.players = platform.child("Players").text().get();
		game.coOp = platform.child("Co-op").text().get();
		game.publisher = platform.child("Publisher").text().get();
		game.developer = platform.child("Developer").text().get();
		game.tgbdRating = platform.child("Rating").text().get();

		game.save();

		std::cout << " -> saved to db\n";
	}
}

void ScraperController::ScrapeGameByCRC(Model::GameModel game)
{
	if(game.gameFile == "")
	{
		LOG(LogLevel::Error, "ScraperController ScrapeGameByCRC file not available");
	}

	//CRC is supported only for "the archive db" (where there is less informations)
	boost::crc_32_type crc;

	std::ifstream ifs(game.gameFile, std::ios_base::binary);

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
		LOG(LogLevel::Error, "ScraperController ScrapeGameByCRC failed to open file: " + game.gameFile);
	}

	std::stringstream stream;
	stream << hex << uppercase << crc.checksum();

	std::string data = DownloadUrlAsString("http://api.archive.vg/2.0/Game.getInfoByCRC/xml/7TTRM4MNTIKR2NNAGASURHJOZJ3QXQC5/"+stream.str());

	int a = 1;
}