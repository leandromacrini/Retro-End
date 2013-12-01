/* Device Model class
*
* A console is a device in Retro-End
*/

#pragma once

#include "../../globals.h"
#include "../Controller/LogController.h"
#include <boost/filesystem.hpp>

#include "BaseModel.h"

namespace RetroEnd
{
	namespace Model
	{
		class Device : public BaseModel
		{
		public:
			Device();
			Device( sqlite3_stmt* record );

			~Device();

			//DB PROPERTIES
			string Name;
			string Description;
			string Developer;
			string Manufacturer;
			string Cpu;
			string Memory;
			string Graphics;
			string Sound;
			string Display;
			string MediaType;
			string MaxControllers;
			string ReleaseDate;
			
			string TGDBId;
			string TGDBRating;

			string ImageConsole;
			string ImageLogo;
			
			string LaunchCommand;

			//computed properties
			string getReleaseYear();
			string getRomsPath();

			//table name
			static string table;

			//retrive all Device from DB
			static vector<Device> getAllDevices();

			//retrive the Device identified by "id" (if any)
			static Device getDeviceById(sqlite3_int64 id);

			//create the table, 
			static void init();
		protected:
			string getUpdateQuery();
			string getDeleteQuery();
			string getInsertQuery();
		};
	}

}
