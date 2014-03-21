/* Game Model class
*
*/

#pragma once

#include <boost/regex/v4/regex.hpp>
#include "../../LIB/pugiXML/pugixml.hpp"
#include "boost/filesystem.hpp"

#include "../../globals.h"

#include "../Controller/LogController.h"

#include "BaseModel.h"

namespace RetroEnd
{
	namespace Model
	{
		class Game : public BaseModel
		{
		public:
			//constructor
			Game();

			//constructor from sqlite record
			Game( sqlite3_stmt* record );

			//constructor from TGDB xml result
			Game ( pugi::xml_node gameNode, sqlite3_int64 deviceId = -1);

			~Game();

			//DB PROPERTIES
			string Title;
			sqlite3_int64 DeviceId;
			string ReleaseDate;
			string Description;
			string Developer;
			string Publisher;
			string ESRB;
			string MaxPlayers;
			string CoOp;
			
			string TGDBId;
			string TGDBRating;

			string ImageFront;
			string ImageBack;
			string ImageScreen;
			string ImageLogo;

			string GameFile;
			string FileCRC;

			int TimesPlayed;
			bool Favorite;

			//computed properties
			string getReleaseYear();
			string getCleanFileName();

			//table name
			static string table;

			//retrive all Game from DB
			static vector<Game> getAllGames();

			//retrive all Game from DB for a specified Device
			static vector<Game> getGamesForDevice(sqlite3_int64 deviceId);

			//retrive the Game identified by "id" or NULL if not present
			static Game getGameById(sqlite3_int64 id);

			//retrive the Game identified by "TGDBId" or NULL if not present
			static Game* getGameByTGDBId(string& TGDBId);

			//retrive the Game identified by "file" or NULL if not present
			static Game* getGameByFileName(string& file);

			//create the table, 
			static void init();

			//override remove to clear files too
			//Remove instance from DB and any game's file
			void remove() override;
		protected:
			string getUpdateQuery();
			string getDeleteQuery();
			string getInsertQuery();
		};
	}

}
