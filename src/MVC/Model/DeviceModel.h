/* Device Model class
*
* A console is a device in Retro-End
*/
#ifndef _DEVICEMODEL_H_
#define _DEVICEMODEL_H_

#include "../../globals.h"
#include "../Controller/LogController.h"

#include "BaseModel.h"

namespace RetroEnd
{
	namespace Model
	{
		class DeviceModel : public BaseModel
		{
		public:
			DeviceModel();
			DeviceModel( sqlite3_stmt* record );

			~DeviceModel();

			//DB PROPERTIES
			string name;
			string description;
			string developer;
			string manufacturer;
			string cpu;
			string memory;
			string graphics;
			string sound;
			string display;
			string mediaType;
			string maxControllers;
			string releaseDate;
			
			string tgbdId;
			string tgbdRating;

			string imageConsole;
			string imageLogo;

			//table name
			static string table;

			//retrive all Device from DB
			static vector<DeviceModel> getAllDevices();

			//retrive the Device identified by "id" (if any)
			static DeviceModel getDeviceById(unsigned int id);

			//create the table, 
			static void init();
		protected:
			string getUpdateQuery();
			string getDeleteQuery();
			string getInsertQuery();
		};
	}

}
#endif _DEVICEMODEL_H_