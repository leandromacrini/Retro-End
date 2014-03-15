#pragma once

#include "../../globals.h"

#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "../../LIB/pugiXML/pugixml.hpp"

#include "BaseModel.h"

#include "../Controller/LogController.h"

#define DEVICE_UNKNOWN	-2
#define DEVICE_KEYBOARD -1

#define VALUE_ID_UNKNOWN -1

namespace RetroEnd
{
	namespace Model
	{
		//this is the source of the input
		enum InputRawType
		{
			TYPE_AXIS_POSITIVE,
			TYPE_AXIS_NEGATIVE,
			TYPE_BUTTON,
			TYPE_HAT,
			TYPE_KEY,
			TYPE_TEXT,
			TYPE_UNKNOWN
		};

		//this is the semantic of the received input if mapped to a config
		enum InputSemantic
		{
			UP,
			DOWN,
			LEFT,
			RIGHT,
			BUTTON_A,
			BUTTON_B,
			BUTTON_X,
			BUTTON_Y,
			BUTTON_L,
			BUTTON_R,
			START,
			SELECT,
			BUTTON_L2,
			BUTTON_L3,
			BUTTON_R2,
			BUTTON_R3,
			SYSTEM,
			TEXT_INPUT,
			UNMAPPED
		};

		struct InputRaw
		{
		public:
			InputRawType Type;	//the type of input
			int ValueID;		//the ID of input
			unsigned int Timestamp;

			InputRaw()
			{
				Type = TYPE_UNKNOWN;
				ValueID = VALUE_ID_UNKNOWN;
				Timestamp = 0;
			}

			InputRaw(InputRawType type, int valueID, unsigned int timestamp = 0) : Type(type), ValueID(valueID), Timestamp(timestamp)
			{

			}
		};

		struct Input
		{
			int Value;
			int PlayerNumber;
			InputSemantic Semantic;
			InputRaw RawData;

			Input() : Value(0), PlayerNumber(-1), Semantic(InputSemantic::UNMAPPED), RawData()
			{

			}

			Input(int value, int player, InputSemantic semantic, InputRawType type, int valueid, unsigned int timestamp)
			{
				Value = value;
				PlayerNumber = player;
				Semantic = semantic;
				RawData.Type = type;
				RawData.ValueID = valueid;
				RawData.Timestamp = timestamp;
			}
		};

		class InputConfig : public BaseModel
		{
		public:
			InputConfig(const string& deviceName, const string& guid);

			//constructor from sqlite record
			InputConfig( sqlite3_stmt* record );

			//table name
			static string table;

			void clear();

			//DB PROPERTIES
			string DeviceName;
			string Guid;

			//RUN TIME PROPERTIES
			SDL_Joystick* Joystick;
			int PlayerNumber;

			void setInputMap(InputSemantic key, InputRaw value);
			InputSemantic getInputMapKey(InputRaw value);
			InputRaw getInputMapValue(InputSemantic key);

			bool TryLoadFromDB();

			static InputConfig* getInputConfigByGUID(string guid);
			static InputConfig* getInputConfigByControllerName(string name);

			//create the table, 
			static void init();
		
		protected:
			string getUpdateQuery();
			string getDeleteQuery();
			string getInsertQuery();

		private:
			map<InputSemantic, InputRaw> mInputMap;

			void loadFromXML(string xml);
			string writeToXML();
		};
	}
}
