#include "Device.h"

using namespace std;
using namespace boost::filesystem;

using namespace RetroEnd::Model;


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
	Name = (char*)sqlite3_column_text(record,1);
	Description = (char*)sqlite3_column_text(record,2);
	Developer = (char*)sqlite3_column_text(record,3);
	Manufacturer = (char*)sqlite3_column_text(record,4);
	Cpu = (char*)sqlite3_column_text(record,5);
	Memory = (char*)sqlite3_column_text(record,6);
	Graphics = (char*)sqlite3_column_text(record,7);
	Sound = (char*)sqlite3_column_text(record,8);
	Display = (char*)sqlite3_column_text(record,9);
	MediaType = (char*)sqlite3_column_text(record,10);
	MaxControllers = (char*)sqlite3_column_text(record,11);
	ReleaseDate = (char*)sqlite3_column_text(record,12);
	TGDBId = (char*)sqlite3_column_text(record,13);
	TGDBRating = (char*)sqlite3_column_text(record,14);
	ImageConsole = (char*)sqlite3_column_text(record,15);
	ImageLogo = (char*)sqlite3_column_text(record,16);
	LaunchCommand = (char*)sqlite3_column_text(record,17);
}

// Model Class Distructor
Device::~Device()
{

}

// Build the query for update the DB record
string Device::getUpdateQuery()
{
	string query = "UPDATE " + table + " SET";
	query += " NAME = '"		+	sqlEscape( Name ) +"',";
	query += " DESCRIPTION = '"	+	sqlEscape( Description ) +"',";
	query += " DEVELOPER = '"	+	sqlEscape( Developer ) +"',";
	query += " MANUFACTURER = '"+	sqlEscape( Manufacturer ) +"',";
	query += " CPU = '"			+	sqlEscape( Cpu ) +"',";
	query += " MEMORY = '"		+	sqlEscape( Memory ) +"',";
	query += " GRAPHICS = '"	+	sqlEscape( Graphics ) +"',";
	query += " SOUND = '"		+	sqlEscape( Sound ) +"',";
	query += " DISPLAY = '"		+	sqlEscape( Display ) +"',";
	query += " MEDIA_TYPE = '"	+	sqlEscape( MediaType ) +"',";
	query += " MAX_CONTROLLERS = '"+sqlEscape(MaxControllers ) +"',";
	query += " RELEASE_DATE = '"+	sqlEscape( ReleaseDate ) +"',";
	query += " TGDB_ID = '"		+	sqlEscape( TGDBId ) +"',";
	query += " TGDB_RATING = '"	+	sqlEscape( TGDBRating ) +"',";
	query += " IMAGE_CONSOLE = '"+	sqlEscape( ImageConsole ) +"',";
	query += " IMAGE_LOGO = '"	+	sqlEscape( ImageLogo ) +"'";
	query += " LAUNCH_COMMAND = '"+	sqlEscape( LaunchCommand ) +"'";
		
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
	string query = "INSERT into " + table + " (NAME,DESCRIPTION,DEVELOPER,MANUFACTURER,CPU,MEMORY,GRAPHICS,SOUND,DISPLAY,MEDIA_TYPE,MAX_CONTROLLERS,RELEASE_DATE,TGDB_ID,TGDB_RATING,IMAGE_CONSOLE,IMAGE_LOGO,LAUNCH_COMMAND )";
	query += "values ('" + sqlEscape( Name ) + "','" + sqlEscape( Description ) + "','" + sqlEscape( Developer ) +
		"','" + sqlEscape( Manufacturer ) + "','" + sqlEscape( Cpu ) + "','" + sqlEscape( Memory ) + "','" + sqlEscape( Graphics ) +
		"','" + sqlEscape( Sound ) + "','" + sqlEscape( Display ) + "','" + sqlEscape( MediaType ) + "','" + sqlEscape( MaxControllers ) +
		"','" + sqlEscape( ReleaseDate ) + "','" + sqlEscape( TGDBId ) + "','" + sqlEscape( TGDBRating ) + "','" + sqlEscape( ImageConsole ) + "','" +
		sqlEscape( ImageLogo ) + "','" + sqlEscape( LaunchCommand ) + "');";
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

	//THE CREATE PART - MUST BE ALWAYS RUN
	string create = "CREATE TABLE IF NOT EXISTS " + Device::table + " (";
	create += "ID INTEGER PRIMARY KEY, NAME TEXT, DESCRIPTION TEXT, DEVELOPER TEXT, MANUFACTURER TEXT, CPU TEXT, MEMORY TEXT, ";
	create += "GRAPHICS TEXT, SOUND TEXT, DISPLAY TEXT, MEDIA_TYPE TEXT, MAX_CONTROLLERS TEXT, RELEASE_DATE TEXT, TGDB_ID TEXT, TGDB_RATING TEXT, ";
	create += "IMAGE_CONSOLE TEXT, IMAGE_LOGO TEXT);";
	queries.push_back(create);

	//THE UPDATE PART  - MUST BE ALWAYS RUN (if colums already exist we simply have an error)
	//new colums bust be added in the update part so when the app is updated we can add those to the DB without recreating the table
	queries.push_back( "ALTER TABLE " +table + " ADD COLUMN LAUNCH_COMMAND TEXT DEFAULT '';" );
	

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

	//create custom game directories if needed
	vector<Device> devices = getAllDevices();
	for (auto it = devices.begin() ; it != devices.end(); ++it)
	{
		if(! exists( it->getRomsPath() ) )	create_directory( it->getRomsPath() );
	}
}

// Static method for read all the items from DB
vector<Device> Device::getAllDevices()
{
	vector<Device> devices;

	string query = "SELECT * FROM " + Device::table + " ORDER BY RELEASE_DATE";

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
Device Device::getDeviceById(int id)
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

string Device::getRomsPath()
{
	return "data/roms/" + Name;
}