#include "SettingsController.h"

using namespace RetroEnd::Controller;

string readValueFromDB(string name)
{
	string result;

	//open db connection
	sqlite3* db;
	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	//create query
	string query = "SELECT VALUE FROM settings WHERE NAME ='" + name + "';";

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		//read value if exists
		if(sqlite3_step(statement) == SQLITE_ROW) result = (char*)sqlite3_column_text(statement, 0);

		//finalize statement
		sqlite3_finalize(statement);
	}
	else
	{
		//log the error
		string message = sqlite3_errmsg(db);
		LOG(Error, "SettingsController readValueFromDB : " + message);
	}

	//close db connection
	sqlite3_close(db);

	return result;
}

void writeValueToDB(string name, string value)
{
	//open db connection
	sqlite3* db;
	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	//create query
	string query = "INSERT INTO settings (NAME, VALUE) values ('"+name+"','"+value+"');";

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		//write value if exists
		sqlite3_step(statement);

		//finalize statement
		sqlite3_finalize(statement);
	}
	else
	{
		//log the error
		string message = sqlite3_errmsg(db);
		LOG(Error, "SettingsController writeValueFromDB : " + message);
	}

	//close db connection
	sqlite3_close(db);
}

void SettingsController::start()
{

	LOG(LogLevel::Info, "SettingsController is creating and updating all the tables.");
	LOG(LogLevel::Info, "Ignore any create or update error if new elements are already into the DB.");

	//open db connection
	sqlite3* db;
	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	//CREATE TABLES
	string query = "CREATE TABLE IF NOT EXISTS settings (ID INTEGER PRIMARY KEY, NAME TEXT, VALUE TEXT, UNIQUE(NAME) ON CONFLICT REPLACE);";

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		sqlite3_step(statement);
		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "SettingsController start : " + message);
	}

	//close db connection
	sqlite3_close(db);
}

void SettingsController::stop() {

}

bool SettingsController::getProperty(string name, bool defaultValue)
{
	bool result = defaultValue;

	string value = readValueFromDB(name);

	if( ! value.empty() )
	{
		result = ( value =="true" || value =="1" );
	}

	return result;
}

float SettingsController::getProperty(string name, float defaultValue)
{
	float result = defaultValue;

	string value = readValueFromDB(name);

	if( ! value.empty() )
	{
		result = std::stof( value );
	}

	return result;
}


string	SettingsController::getProperty(string name, string defaultValue)
{
	string result = defaultValue;

	string value = readValueFromDB(name);

	if( ! value.empty() )
	{
		result = value;
	}

	return result;
}

void SettingsController::setProperty(string name, bool value)
{
	writeValueToDB(name, to_string(value));
}

void SettingsController::setProperty(string name, float value)
{
	writeValueToDB(name, to_string(value));
}

void SettingsController::setProperty(string name, string value)
{
	writeValueToDB(name, value);
}
