/* Game Model class
*
*/

#pragma once

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
			Game();
			Game( sqlite3_stmt* record );

			~Game();

			//DB PROPERTIES
			string title;
			sqlite3_int64 deviceId;
			string releaseDate;
			string description;
			string developer;
			string publisher;
			string ESRB;
			string players;
			string coOp;
			
			string tgbdId;
			string tgbdRating;

			string imageFront;
			string imageBack;
			string imageScreen;
			string imageLogo;

			string gameFile;
			string fileCRC;

			//table name
			static string table;

			//retrive all Game from DB
			static vector<Game> getAllGames();

			//retrive all Game from DB for a specified Device
			static vector<Game> getGamesForDevice(int deviceId);

			//retrive the Game identified by "id" or NULL if not present
			static Game getGameById(int id);

			//create the table, 
			static void init();
		protected:
			string getUpdateQuery();
			string getDeleteQuery();
			string getInsertQuery();
		};
	}

}
