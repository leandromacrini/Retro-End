#include "Game.h"

#include "Device.h"

namespace fs = boost::filesystem;

using namespace RetroEnd::Model;
using namespace std;

// The Name of the table into the DB for this Model
string Game::table = "games";

// Model Class Constructor
Game::Game() : BaseModel()
{
	//default values
	DeviceId = 0; 
	TimesPlayed = 0;
	Favorite = false;
}

// Model Class Constructor from DB record
Game::Game( sqlite3_stmt* record ) : BaseModel()
{
	id			= sqlite3_column_int(record, 0);
	Title		= (char*)sqlite3_column_text(record,1);
	DeviceId	= sqlite3_column_int(record,2);
	Description = (char*)sqlite3_column_text(record,3);
	Developer	= (char*)sqlite3_column_text(record,4);
	Publisher	= (char*)sqlite3_column_text(record,5);
	ESRB		= (char*)sqlite3_column_text(record,6);
	MaxPlayers	= (char*)sqlite3_column_text(record,7);
	CoOp		= (char*)sqlite3_column_text(record,8);
	ReleaseDate = (char*)sqlite3_column_text(record,9);
	TGDBId		= (char*)sqlite3_column_text(record,10);
	TGDBRating	= (char*)sqlite3_column_text(record,11);
	ImageFront	= (char*)sqlite3_column_text(record,12);
	ImageBack	= (char*)sqlite3_column_text(record,13);
	ImageScreen	= (char*)sqlite3_column_text(record,14);
	ImageLogo	= (char*)sqlite3_column_text(record,15);
	GameFile	= (char*)sqlite3_column_text(record,16);
	FileCRC		= (char*)sqlite3_column_text(record,17);
	TimesPlayed	= sqlite3_column_int(record, 18);
	Favorite	= sqlite3_column_int(record, 19) == 1;
}

// Model Class Constructor from TGDB result
Game::Game ( pugi::xml_node gameNode, sqlite3_int64 deviceId) : BaseModel()
{
	//default values
	TimesPlayed = 0;
	Favorite = false;

	DeviceId = deviceId;

	Title = gameNode.child("GameTitle").text().get();
	Description = gameNode.child("Overview").text().get();
	ESRB = gameNode.child("ESRB").text().get();
	MaxPlayers = gameNode.child("Players").text().get();
	CoOp = gameNode.child("Co-op").text().get();
	Publisher = gameNode.child("Publisher").text().get();
	Developer = gameNode.child("Developer").text().get();
	TGDBRating = gameNode.child("Rating").text().get();
	ReleaseDate = gameNode.child("ReleaseDate").text().get();
	TGDBId = gameNode.child("id").text().get();
}

// Model Class Distructor
Game::~Game()
{

}

// Build the query for update the DB record
string Game::getUpdateQuery()
{
	string query = "UPDATE " + table + " SET";

	query += " DEVICE_ID = "	+	to_string( DeviceId )+",";
	query += " TITLE = '"		+	sqlEscape( Title )+"',";
	query += " DESCRIPTION = '"	+	sqlEscape( Description )+"',";
	query += " DEVELOPER = '"	+	sqlEscape( Developer )+"',";
	query += " PUBLISHER = '"	+	sqlEscape( Publisher )+"',";
	query += " ESRB = '"		+	sqlEscape( ESRB )+"',";
	query += " PLAYERS = '"		+	sqlEscape( MaxPlayers )+"',";
	query += " CO_OP = '"		+	sqlEscape( CoOp )+"',";
	query += " RELEASE_DATE = '"+	sqlEscape( ReleaseDate )+"',";
	query += " TGDB_ID = '"		+	sqlEscape( TGDBId )+"',";
	query += " TGDB_RATING = '"	+	sqlEscape( TGDBRating )+"',";
	query += " IMAGE_FRONT = '"	+	sqlEscape( ImageFront )+"',";
	query += " IMAGE_BACK = '"	+	sqlEscape( ImageBack )+"',";
	query += " IMAGE_SCREEN = '"+	sqlEscape( ImageScreen )+"',";
	query += " IMAGE_LOGO = '"	+	sqlEscape( ImageLogo )+"',";
	query += " GAME_FILE = '"	+	sqlEscape( GameFile )+"' ,";
	query += " FILE_CRC = '"	+	sqlEscape( FileCRC )+"' ,";
	query += " TIMES_PLAYED = "	+	to_string( TimesPlayed )+" ,";
	query += " FAVORITE = "		+	to_string( Favorite? 1 : 0 ) ;

	query += " WHERE id="+to_string( id )+";";

	return query;
}

// Build the query for delete the DB record
string Game::getDeleteQuery()
{
	return "DELETE FROM " + table + " WHERE id="+to_string( id )+";";
}

// Build the query for create the DB record
string Game::getInsertQuery()
{
	string query = "INSERT into " + table + " (TITLE,DEVICE_ID,DESCRIPTION,DEVELOPER,PUBLISHER,ESRB,PLAYERS,CO_OP,RELEASE_DATE,TGDB_ID,TGDB_RATING,IMAGE_FRONT,IMAGE_BACK,IMAGE_SCREEN,IMAGE_LOGO, GAME_FILE, FILE_CRC, TIMES_PLAYED, FAVORITE)";
	query += "values ('" + sqlEscape( Title ) + "','" + to_string(DeviceId) + "','" + sqlEscape( Description ) + "','" + sqlEscape( Developer ) + "','" + sqlEscape( Publisher ) + "','" + sqlEscape( ESRB ) + "','" + sqlEscape( MaxPlayers ) + "','" + sqlEscape( CoOp ) + "','" + sqlEscape( ReleaseDate ) + "','" + sqlEscape( TGDBId ) + "','" + sqlEscape( TGDBRating ) + "','" + sqlEscape( ImageFront ) + "','" + sqlEscape( ImageBack ) + "','" + sqlEscape( ImageScreen ) + "','" + sqlEscape( ImageLogo ) + "','" + sqlEscape( GameFile ) + "','" + sqlEscape( FileCRC ) + "', "+to_string(TimesPlayed)+", " + to_string(Favorite? 1 : 0) + ")";
	return query;
}

// Build the query for create the table into the SQLite DB
// Sould be called only on first run of the app
void Game::init()
{
	//open db connection
	sqlite3* db;
	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;
	vector<string> queries;

	//THE CREATE PART - MUST BE ALWAYS RUN (if table already exists we simply have an error)
	string create = "CREATE TABLE IF NOT EXISTS " + Game::table + " (";
	create += "ID INTEGER PRIMARY KEY, TITLE TEXT, DEVICE_ID INTEGER, DESCRIPTION TEXT, DEVELOPER TEXT, PUBLISHER TEXT, ESRB TEXT, PLAYERS TEXT, ";
	create += "CO_OP TEXT, RELEASE_DATE TEXT, TGDB_ID TEXT, TGDB_RATING TEXT, ";
	create += "IMAGE_FRONT TEXT, IMAGE_BACK TEXT, IMAGE_SCREEN TEXT, IMAGE_LOGO TEXT);";
	queries.push_back(create);

	//THE UPDATE PART  - MUST BE ALWAYS RUN (if colums already exist we simply have an error)
	//new colums bust be added in the update part so when the app is updated we can add those to the DB
	queries.push_back( "ALTER TABLE " +Game::table + " ADD COLUMN GAME_FILE TEXT DEFAULT '';" );
	queries.push_back( "ALTER TABLE " +Game::table + " ADD COLUMN FILE_CRC TEXT DEFAULT '';" );
	queries.push_back( "ALTER TABLE " +Game::table + " ADD COLUMN TIMES_PLAYED INTEGER DEFAULT 0;" );
	queries.push_back( "ALTER TABLE " +Game::table + " ADD COLUMN FAVORITE INTEGER DEFAULT 0;" );

	for (vector<string>::iterator it = queries.begin() ; it != queries.end(); ++it) {
		string query = *it;

		//execute query
		if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
		{
			sqlite3_step(statement);
			sqlite3_finalize(statement);
		}
		else
		{
			string message = sqlite3_errmsg(db);

			//IF COLUMN ALREADY AXISTS IS NOT AN ERROR
			if( strncmp(message.c_str(), "duplicate column", strlen("duplicate column")) != 0)
				LOG(LogLevel::Error, "Game init : " + message);
		}
	}

	//close db concction
	sqlite3_close(db);
}

// Static method for read all the items from DB
vector<Game> Game::getAllGames()
{
	vector<Game> games;

	string query = "SELECT * FROM " + Game::table;

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		while (result == SQLITE_ROW)
		{
			//create and insert the new item
			games.push_back(Game(statement));

			//go to  next record
			result = sqlite3_step(statement);
		}

		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "Game getAllGames : " + message);
	}

	sqlite3_close(db);

	return games;
}

// Static method for read an item specified by "id" from DB
Game Game::getGameById(sqlite3_int64 id)
{
	Game* game = NULL;

	string query = "SELECT * FROM " + Game::table + " WHERE id = " + to_string(id);

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		if(result == SQLITE_ROW)
		{
			game = new Game(statement);
		}
		else
		{
			game = new Game();
		}
		
		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "Game getGameById : " + message);
	}

	sqlite3_close(db);

	return *game;
}

Game* Game::getGameByTGDBId(string& TGDBId)
{
	Game* game = NULL;

	string query = "SELECT * FROM " + Game::table + " WHERE TGDB_ID = '" + TGDBId +"' LIMIT 1";

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		if(result == SQLITE_ROW)
		{
			game = new Game(statement);
		}
		
		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "Game getGameById : " + message);
	}

	sqlite3_close(db);

	return game;
}

Game* Game::getGameByFileName(string& file)
{
	Game* game = NULL;

	string query = "SELECT * FROM " + Game::table + " WHERE GAME_FILE = '" + file +"' LIMIT 1";

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		if(result == SQLITE_ROW)
		{
			game = new Game(statement);
		}
		
		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "Game getGameById : " + message);
	}

	sqlite3_close(db);

	return game;
}

string Game::getReleaseYear()
{
	if(ReleaseDate.empty())
		return "";
	else
	{
		return ReleaseDate.substr(ReleaseDate.find_last_of("/") + 1, string::npos) + ", ";
	}
}

string Game::getCleanFileName()
{
	string result = GameFile;

	boost::regex rx1("\\(.+\\)");
	boost::regex rx2("\\[.+\\]");

	result = regex_replace(result, rx1, "");
	result = regex_replace(result, rx2, "");
	result = result.substr(0, result.find_last_of('.'));
	result = trim(result);
	return result;
}

// Static method for read all the items from DB for a specified Device
vector<Game> Game::getGamesForDevice(sqlite3_int64 deviceId)
{
	vector<Game> games;

	string query = "SELECT * FROM " + Game::table + " WHERE DEVICE_ID = " + to_string(deviceId) + " ORDER BY TITLE ";

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		while (result == SQLITE_ROW)
		{
			//create and insert the new item
			games.push_back(Game(statement));

			//go to  next record
			result = sqlite3_step(statement);
		}

		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "Game getGamesForDevice : " + message);
	}

	sqlite3_close(db);

	return games;
}

void Game::remove()
{
	//remove from DB
	BaseModel::remove();

	if(DeviceId != 0 && ! GameFile.empty())
	{
		//get GameFile path
		string romPath = Device::getDeviceById(DeviceId).getRomsPath();
		//get Images path

		//delete rom file
		fs::path file = romPath / fs::path(GameFile);
		if( fs::exists(file)) fs::remove(file);
	}

	if( ! ImageFront.empty() && fs::exists(ImageFront) ) fs::remove(ImageFront);
	if( ! ImageBack.empty() && fs::exists(ImageBack) ) fs::remove(ImageBack);
	if( ! ImageLogo.empty() && fs::exists(ImageLogo) ) fs::remove(ImageLogo);
	if( ! ImageScreen.empty() && fs::exists(ImageScreen) ) fs::remove(ImageScreen);
}