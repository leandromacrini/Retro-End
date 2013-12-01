/* Base Model class
 *
 * Create the interface for access to any Model
 * Create the SQLite DB data layer
 */

#ifndef _BASEMODEL_H_
#define _BASEMODEL_H_

#include <sqlite3.h>

#include "../../globals.h"

using namespace std;

namespace RetroEnd
{
	namespace Model
	{
		class BaseModel
		{

		public:
			BaseModel();
			~BaseModel();
			
			//Identifier
			sqlite3_int64 id;

			//Save instance into DB
			virtual void save();

			//Remove instance from DB
			virtual void remove();

		protected:

			//SQLite Tablename "table" is static into childs

			//Data Access queries (should be overridden by childs)

			virtual string getInsertQuery() = 0;
			virtual string getUpdateQuery() = 0;
			virtual string getDeleteQuery() = 0;

			//opening, closing and executing DB functions
			void connect();
			void disconnect();

			//sql escaping function
			string sqlEscape(string& input);

		private:
			sqlite3* mDB;
		};
	}
}

#endif _BASEMODEL_H_