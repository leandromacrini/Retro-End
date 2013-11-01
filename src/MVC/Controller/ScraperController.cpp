#include "ScraperController.h"

#include "../../LIB/pugiXML/pugixml.hpp"
#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#include <sstream>

#include <FreeImage.h>

#include "LogController.h"
#include "RenderController.h"

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
	curl_easy_perform(curl_handle); 
	curl_easy_cleanup(curl_handle);

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
		LOG(LogLevel::Info, name + " saved to db\n");
	}
}

void ScraperController::ScrapeAllGamesForDevice(Model::Device& device, bool downloadImages, int maxCount)
{
	LOG(LogLevel::Info, "Scraper is starting retrieving all the games info available for " + device.name + "\n");

	std::string data = DownloadUrlAsString("http://thegamesdb.net/api/GetPlatformGames.php?platform=" + device.tgbdId);

	pugi::xml_document doc;
	doc.load(data.c_str());
	pugi::xml_node systemList = doc.child("Data");

	int counter = 0;

	for(pugi::xml_node system = systemList.child("Game"); system && counter <= maxCount; system = system.next_sibling("Game"))
	{
		counter++;

		string tgdb_id = system.child("id").text().get();
		string title = system.child("GameTitle").text().get();

		std::cout << "Getting info for:" + title;

		std::string platData = DownloadUrlAsString("http://thegamesdb.net/api/GetGame.php?id=" + tgdb_id);

		pugi::xml_document platDoc;

		platDoc.load(platData.c_str());

		pugi::xml_node gameData = platDoc.child("Data").child("Game");

		Model::Game game;

		game.title = gameData.child("GameTitle").text().get();
		game.deviceId = device.id;
		game.description = gameData.child("Overview").text().get();
		game.ESRB = gameData.child("ESRB").text().get();
		game.players = gameData.child("Players").text().get();
		game.coOp = gameData.child("Co-op").text().get();
		game.publisher = gameData.child("Publisher").text().get();
		game.developer = gameData.child("Developer").text().get();
		game.tgbdRating = gameData.child("Rating").text().get();
		game.releaseDate = gameData.child("ReleaseDate").text().get();
		game.tgbdId = tgdb_id;
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
					game.imageFront = DownloadUrlAsFile( url, "data/game screens/front" );

				res = strcmp( image.attribute("side").value(), "back");
				if( res == 0)
					game.imageBack = DownloadUrlAsFile( url, "data/game screens/back" );

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

				game.imageScreen = DownloadUrlAsFile(url, "data/game screens/screens" );
				break;
			}
		}
		game.save();

		LOG(LogLevel::Info, title + " saved to db");
	}
}

void ScraperController::ScrapeGameByCRC(Model::Game& game, Model::Device& device)
{
	if(game.gameFile == "")
	{
		LOG(LogLevel::Error, "ScraperController ScrapeGameByCRC file not available for " + game.title);
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
		LOG(LogLevel::Error, "ScraperController ScrapeGameByCRC unable to open file: " + game.gameFile);
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
}
