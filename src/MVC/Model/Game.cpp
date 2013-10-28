#include "Game.h"

using namespace RetroEnd::Model;
using namespace std;

// The Name of the table into the DB for this Model
string Game::table = "games";

// Model Class Constructor
Game::Game() : BaseModel()
{
	deviceId = 0; //default value
}

// Model Class Constructor from DB record
Game::Game( sqlite3_stmt* record ) : BaseModel()
{
	id			= sqlite3_column_int(record, 0);
	title		= (char*)sqlite3_column_text(record,1);
	deviceId	= sqlite3_column_int(record,2);
	description = (char*)sqlite3_column_text(record,3);
	developer	= (char*)sqlite3_column_text(record,4);
	publisher	= (char*)sqlite3_column_text(record,5);
	ESRB		= (char*)sqlite3_column_text(record,6);
	players		= (char*)sqlite3_column_text(record,7);
	coOp		= (char*)sqlite3_column_text(record,8);
	releaseDate = (char*)sqlite3_column_text(record,9);
	tgbdId		= (char*)sqlite3_column_text(record,10);
	tgbdRating	= (char*)sqlite3_column_text(record,11);
	imageFront	= (char*)sqlite3_column_text(record,12);
	imageBack	= (char*)sqlite3_column_text(record,13);
	imageScreen	= (char*)sqlite3_column_text(record,14);
	imageLogo	= (char*)sqlite3_column_text(record,15);
	gameFile	= (char*)sqlite3_column_text(record,16);
	fileCRC		= (char*)sqlite3_column_text(record,17);
}

// Model Class Distructor
Game::~Game()
{

}

// Build the query for update the DB record
string Game::getUpdateQuery()
{
	string query = "UPDATE " + table + " SET";

	query += " DEVICE_ID = '"	+	to_string( deviceId )+"',";
	query += " TITLE = '"		+	sqlEscape( title )+"',";
	query += " DESCRIPTION = '"	+	sqlEscape( description )+"',";
	query += " DEVELOPER = '"	+	sqlEscape( developer )+"',";
	query += " PUBLISHER = '"	+	sqlEscape( publisher )+"',";
	query += " ESRB = '"		+	sqlEscape( ESRB )+"',";
	query += " PLAYERS = '"		+	sqlEscape( players )+"',";
	query += " CO_OP = '"		+	sqlEscape( coOp )+"',";
	query += " RELEASE_DATE = '"+	sqlEscape( releaseDate )+"',";
	query += " TGDB_ID = '"		+	sqlEscape( tgbdId )+"',";
	query += " TGDB_RATING = '"	+	sqlEscape( tgbdRating )+"',";
	query += " IMAGE_FRONT = '"	+	sqlEscape( imageFront )+"',";
	query += " IMAGE_BACK = '"	+	sqlEscape( imageBack )+"',";
	query += " IMAGE_SCREEN = '"+	sqlEscape( imageScreen )+"',";
	query += " IMAGE_LOGO = '"	+	sqlEscape( imageLogo )+"',";
	query += " GAME_FILE = '"	+	sqlEscape( gameFile )+"' ";
	query += " FILE_CRC = '"	+	sqlEscape( fileCRC )+"' ";

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
	string query = "INSERT into " + table + " (TITLE,DEVICE_ID,DESCRIPTION,DEVELOPER,PUBLISHER,ESRB,PLAYERS,CO_OP,RELEASE_DATE,TGDB_ID,TGDB_RATING,IMAGE_FRONT,IMAGE_BACK,IMAGE_SCREEN,IMAGE_LOGO, GAME_FILE, FILE_CRC)";
	query += "values ('" + sqlEscape( title ) + "','" + to_string(deviceId) + "','" + sqlEscape( description ) + "','" + sqlEscape( developer ) + "','" + sqlEscape( publisher ) + "','" + sqlEscape( ESRB ) + "','" + sqlEscape( players ) + "','" + sqlEscape( coOp ) + "','" + sqlEscape( releaseDate ) + "','" + sqlEscape( tgbdId ) + "','" + sqlEscape( tgbdRating ) + "','" + sqlEscape( imageFront ) + "','" + sqlEscape( imageBack ) + "','" + sqlEscape( imageScreen ) + "','" + sqlEscape( imageLogo ) + "','" + sqlEscape( gameFile ) + "','" + sqlEscape( fileCRC ) + "');";
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

	for (std::vector<string>::iterator it = queries.begin() ; it != queries.end(); ++it) {
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
Game Game::getGameById(int id)
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

// Static method for read all the items from DB for a specified Device
vector<Game> Game::getGamesForDevice(int deviceId)
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