#include "InputConfig.h"

using namespace RetroEnd::Model;

// The Name of the table into the DB for this Model
string InputConfig::table = "inputs";

InputConfig::InputConfig(const string& deviceName, const string& guid) : BaseModel(), DeviceName(deviceName), Guid(guid) 
{
	clear();
}

InputConfig::InputConfig( sqlite3_stmt* record ) : BaseModel()
{
	id			= sqlite3_column_int(record, 0);
	Guid		= (char*)sqlite3_column_text(record,1);
	DeviceName	= sqlite3_column_int(record,2);

	clear();
	
	string xmldata = (char*)sqlite3_column_text(record,3);

	loadFromXML(xmldata);
}

// Build the query for update the DB record
string InputConfig::getUpdateQuery()
{
	string query = "UPDATE " + table + " SET";

	query += " GUID = '"		+	sqlEscape( Guid )+"',";
	query += " DEVICE_NAME = '"	+	sqlEscape( DeviceName )+"',";
	query += " XMLDATA = '"		+	sqlEscape( writeToXML() )+"'";


	query += " WHERE id="+to_string( id )+";";

	return query;
}

// Build the query for delete the DB record
string InputConfig::getDeleteQuery()
{
	return "DELETE FROM " + table + " WHERE id="+to_string( id )+";";
}

// Build the query for create the DB record
string InputConfig::getInsertQuery()
{
	string query = "INSERT into " + table + " (DEVICE_NAME,GUID,XMLDATA)";
	query += "values ('" + sqlEscape( DeviceName ) + "','" + sqlEscape(Guid) + "','" + sqlEscape( writeToXML() ) + "');";
	return query;
}

// Build the query for create the table into the SQLite DB
// Sould be called only on first run of the app
void InputConfig::init()
{
	//open db connection
	sqlite3* db;
	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;
	vector<string> queries;

	//THE CREATE PART - MUST BE ALWAYS RUN (if table already exists we simply have an error)
	string create = "CREATE TABLE IF NOT EXISTS " + InputConfig::table + " (";
	create += "ID INTEGER PRIMARY KEY, DEVICE_NAME TEXT, GUID TEXT, XMLDATA TEXT, UNIQUE (GUID) ON CONFLICT REPLACE);";
	queries.push_back(create);

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
				LOG(LogLevel::Error, "InputConfig init : " + message);
		}
	}

	//close db concction
	sqlite3_close(db);
}

void InputConfig::clear()
{
	mInputMap.clear();
}

void InputConfig::setInputMap(InputSemantic key, InputRaw value)
{
	mInputMap[key] = value;
}

InputRaw InputConfig::getInputMapValue(InputSemantic key)
{
	if ( mInputMap.find(key) == mInputMap.end() )
		return InputRaw();
	else
		return mInputMap[key];
}

InputSemantic InputConfig::getInputMapKey(InputRaw value)
{
	for (auto it = mInputMap.begin(); it != mInputMap.end(); ++it )
	{
		InputRaw input = it->second;
		if (input.Type == value.Type && input.ValueID == value.ValueID)
			return it->first;
	}
	return InputSemantic::UNMAPPED;
}

void InputConfig::loadFromXML(string xml)
{
	clear();

	pugi::xml_document doc;

	doc.load(xml.c_str());

	pugi::xml_node cfg = doc.child("inputConfig");

	DeviceName = cfg.attribute("name").as_string();
	Guid = cfg.attribute("guid").as_string();

	pugi::xml_node inputs = cfg.child("inputs");

	for(pugi::xml_node child = inputs.first_child(); child; child = child.next_sibling())
	{
		InputRaw input;

		input.Type = (InputRawType) child.attribute("type").as_int();
		input.ValueID = child.attribute("valueID").as_int();
		InputSemantic key = (InputSemantic) child.attribute("semantic").as_int();

		mInputMap[key] = input;
	}
}

string InputConfig::writeToXML()
{
	pugi::xml_document doc;

	pugi::xml_node cfg = doc.append_child("inputConfig");

	cfg.append_attribute("name").set_value(DeviceName.c_str());
	cfg.append_attribute("guid").set_value(Guid.c_str());

	pugi::xml_node inputs = cfg.append_child("inputs");

    for(auto iterator = mInputMap.begin(); iterator != mInputMap.end(); iterator++)
    {
		pugi::xml_node input =  inputs.append_child("input");
		input.append_attribute("semantic").set_value(iterator->first);
		input.append_attribute("type").set_value(iterator->second.Type);
		input.append_attribute("valueID").set_value(iterator->second.ValueID);
    }

	ostringstream oss;
	doc.save(oss);
	return oss.str();
}

bool InputConfig::TryLoadFromDB()
{
	InputConfig* config = NULL;
	//find by guid
	config = getInputConfigByGUID(this->Guid);

	//find by controller name
	if(config == NULL) config = getInputConfigByControllerName(this->DeviceName);

	if(config != NULL)
	{
		//found
		this->loadFromXML(config->writeToXML());
		delete config;
		return true;
	}
	else
	{
		//not found
		return false;
	}
}

// Static method for read an item specified by "guid" from DB
InputConfig* InputConfig::getInputConfigByGUID(string guid)
{
	InputConfig* config = NULL;

	string query = "SELECT * FROM " + InputConfig::table + " WHERE GUID = '" + guid + "';";

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		if(result == SQLITE_ROW)
		{
			config = new InputConfig(statement);
		}
		
		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "InputConfig getInputConfigByGUID : " + message);
	}

	sqlite3_close(db);

	return config;
}

// Static method for read an item specified by Controller "name"
InputConfig* InputConfig::getInputConfigByControllerName(string name)
{
	InputConfig* config = NULL;

	string query = "SELECT * FROM " + InputConfig::table + " WHERE DEVICE_NAME = '" + name + "';";

	sqlite3* db;

	sqlite3_open(RetroEnd::DB_NAME.c_str(), &db);

	sqlite3_stmt *statement;

	if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int result = sqlite3_step(statement);

		if(result == SQLITE_ROW)
		{
			config = new InputConfig(statement);
		}
		
		sqlite3_finalize(statement);
	}
	else
	{
		string message = sqlite3_errmsg(db);
		LOG(Error, "InputConfig getInputConfigByGUID : " + message);
	}

	sqlite3_close(db);

	return config;
}
