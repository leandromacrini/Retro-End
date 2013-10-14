#include "Device.h"

using namespace RetroEnd::Model;
using namespace std;

// The Name of the table into the DB for this Model
string Device::table = "devices";

// Model Class Constructor
Device::Device() : BaseModel()
{

}

// Model Class Constructor from DB record
Device::Device( sqlite3_stmt* record ) : BaseModel()
{
	id = sqlite3_column_int(record, 0);
	name = (char*)sqlite3_column_text(record,1);
	description = (char*)sqlite3_column_text(record,2);
	developer = (char*)sqlite3_column_text(record,3);
	manufacturer = (char*)sqlite3_column_text(record,4);
	cpu = (char*)sqlite3_column_text(record,5);
	memory = (char*)sqlite3_column_text(record,6);
	graphics = (char*)sqlite3_column_text(record,7);
	sound = (char*)sqlite3_column_text(record,8);
	display = (char*)sqlite3_column_text(record,9);
	mediaType = (char*)sqlite3_column_text(record,10);
	maxControllers = (char*)sqlite3_column_text(record,11);
	releaseDate = (char*)sqlite3_column_text(record,12);
	tgbdId = (char*)sqlite3_column_text(record,13);
	tgbdRating = (char*)sqlite3_column_text(record,14);
	imageConsole = (char*)sqlite3_column_text(record,15);
	imageLogo = (char*)sqlite3_column_text(record,16);
}

// Model Class Distructor
Device::~Device()
{

}

// Build the query for update the DB record
string Device::getUpdateQuery()
{
	string query = "UPDATE " + table + " SET";
	query += " NAME = '"		+	sqlEscape( name ) +"',";
	query += " DESCRIPTION = '"	+	sqlEscape( description ) +"',";
	query += " DEVELOPER = '"	+	sqlEscape( developer ) +"',";
	query += " MANUFACTURER = '"+	sqlEscape( manufacturer ) +"',";
	query += " CPU = '"			+	sqlEscape( cpu ) +"',";
	query += " MEMORY = '"		+	sqlEscape( memory ) +"',";
	query += " GRAPHICS = '"	+	sqlEscape( graphics ) +"',";
	query += " SOUND = '"		+	sqlEscape( sound ) +"',";
	query += " DISPLAY = '"		+	sqlEscape( display ) +"',";
	query += " MEDIA_TYPE = '"	+	sqlEscape( mediaType ) +"',";
	query += " MAX_CONTROLLERS = '"	+sqlEscape( maxControllers ) +"',";
	query += " RELEASE_DATE = '"+	sqlEscape( releaseDate ) +"',";
	query += " TGDB_ID = '"		+	sqlEscape( tgbdId ) +"',";
	query += " TGDB_RATING = '"	+	sqlEscape( tgbdRating ) +"',";
	query += " IMAGE_CONSOLE = '"+	sqlEscape( imageConsole ) +"',";
	query += " IMAGE_LOGO = '"	+	sqlEscape( imageLogo ) +"'";
		
	query += " WHERE id="+to_string( id )+";";

	return query;
}

// Build the query for delete the DB record
string Device::getDeleteQuery()
{
	return "DELETE FROM " + table + " WHERE id="+to_string( id )+";";
}

// Build the query for create the DB record
string Device::getInsertQuery()
{
	string query = "INSERT into " + table + " (NAME,DESCRIPTION,DEVELOPER,MANUFACTURER,CPU,MEMORY,GRAPHICS,SOUND,DISPLAY,MEDIA_TYPE,MAX_CONTROLLERS,RELEASE_DATE,TGDB_ID,TGDB_RATING,IMAGE_CONSOLE,IMAGE_LOGO )";
	query += "values ('" + sqlEscape( name ) + "','" + sqlEscape( description ) + "','" + sqlEscape( developer ) + "','" + sqlEscape( manufacturer ) + "','" + sqlEscape( cpu ) + "','" + sqlEscape( memory ) + "','" + sqlEscape( graphics ) + "','" + sqlEscape( sound ) + "','" + sqlEscape( display ) + "','" + sqlEscape( mediaType ) + "','" + sqlEscape( maxControllers ) + "','" + sqlEscape( releaseDate ) + "','" + sqlEscape( tgbdId ) + "','" + sqlEscape( tgbdRating ) + "','" + sqlEscape( imageConsole ) + "','" + sqlEscape( imageLogo ) + "');";
	return query;
}

// Build the query for create the table into the SQLite DB
// Sould be called only on first run of the app
void Device::init()
{
	//open db connection
	sqlite3* db;
	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	vector<string> queries;

	//THE CREATE PART - MUST BE ALWAYS RUN (if table already exists we simply have an error)
	string create = "CREATE TABLE IF NOT EXISTS " + Device::table + " (";
	create += "ID INTEGER PRIMARY KEY, NAME TEXT, DESCRIPTION TEXT, DEVELOPER TEXT, MANUFACTURER TEXT, CPU TEXT, MEMORY TEXT, ";
	create += "GRAPHICS TEXT, SOUND TEXT, DISPLAY TEXT, MEDIA_TYPE TEXT, MAX_CONTROLLERS TEXT, RELEASE_DATE TEXT, TGDB_ID TEXT, TGDB_RATING TEXT, ";
	create += "IMAGE_CONSOLE TEXT, IMAGE_LOGO TEXT);";
	queries.push_back(create);

	//THE UPDATE PART  - MUST BE ALWAYS RUN (if colums already exist we simply have an error)
	//new colums bust be added in the update part so when the app is updated we can add those to the DB
	

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
			if( strncmp(message.c_str(), "duplicate column", strlen("duplicate column")) != 0)
				LOG(LogLevel::Warning, "Device init : " + message);
		}
	}

	sqlite3_close(db);
}

// Static method for read all the items from DB
vector<Device> Device::getAllDevices()
{
	vector<Device> devices;

	string query = "SELECT * FROM " + Device::table;

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		while (result == SQLITE_ROW)
		{
			devices.push_back(Device(statement));

			result = sqlite3_step(statement);
		}

		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "Device getAllDevices : " + message);
	}

	sqlite3_close(db);

	return devices;
}

// Static method for read an item specified by "id" from DB
Device Device::getDeviceById(unsigned int id)
{
	Device* device = NULL;

	string query = "SELECT * FROM " + Device::table + " WHERE id = " + to_string(id);

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		if(result == SQLITE_ROW)
		{
			device = new Device(statement);
		}
		else
		{
			device = new Device();
		}
		
		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "Device getDeviceById : " + message);
	}

	sqlite3_close(db);

	return *device;
}
