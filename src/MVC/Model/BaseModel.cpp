#include "BaseModel.h"

#include <boost/algorithm/string.hpp>
#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

BaseModel::BaseModel() : id(0)
{
	
}

BaseModel::~BaseModel()
{

}

void BaseModel::save()
{
	//open db concction
	connect();

	sqlite3_stmt *statement;
	string query;

	//check if is an insert or an update
	if(id == 0)
		query = getInsertQuery();
	else
		query = getUpdateQuery();

	//execute query
	if(sqlite3_prepare_v2(mDB, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		sqlite3_step(statement);
		sqlite3_finalize(statement);

		//update the item id if the record was created now
		if(id==0)
		{
			id = sqlite3_last_insert_rowid(mDB);
		}
	}
	else
	{
		string message = sqlite3_errmsg(mDB);
		LOG(Error, "BaseModel save error : " + message);
	}

	//close db concction
	disconnect();
}

void BaseModel::remove()
{
	//check if it exists on DB
	if(id == 0 ) return;

	//open db concction
	connect();

	sqlite3_stmt *statement;
	string query = getDeleteQuery();

	//execute query
	if(sqlite3_prepare_v2(mDB, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		sqlite3_step(statement);
		sqlite3_finalize(statement);

		//erase the id of the deleted record
		id = 0;
	}
	else
	{
		string message = sqlite3_errmsg(mDB);
		LOG(Error, "BaseModel remove error : " + message);
	}

	//close db concction
	disconnect();
}

void BaseModel::connect()
{
	sqlite3_open(RetroEnd::DB_NAME.c_str(), &mDB);
}

void BaseModel::disconnect()
{
	sqlite3_close(mDB);
	mDB = NULL;
}

string BaseModel::sqlEscape(string& input)
{
	return boost::replace_all_copy(input, "'", "''");
}