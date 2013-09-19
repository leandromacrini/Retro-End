/* Game Model class
*
*/
#ifndef _GAMEMODEL_H_
#define _GAMEMODEL_H_

#include "../../globals.h"
#include "../Controller/LogController.h"
#include "BaseModel.h"

namespace RetroEnd
{
	namespace Model
	{
		class GameModel : public BaseModel
		{
		public:
			GameModel();
			GameModel( sqlite3_stmt* record );

			~GameModel();

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

			//table name
			static string table;

			//retrive all Game from DB
			static vector<GameModel> getAllGames();

			//retrive all Game from DB for a specified Device
			static vector<GameModel> getGamesForDevice(int deviceId);

			//retrive the Game identified by "id" or NULL if not present
			static GameModel getGameById(int id);

			//create the table, 
			static void init();
		protected:
			string getUpdateQuery();
			string getDeleteQuery();
			string getInsertQuery();
		};
	}

}
#endif _GAMEMODEL_H_