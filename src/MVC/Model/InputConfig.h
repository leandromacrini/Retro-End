#pragma once

#include "../../globals.h"

#include <map>
#include <SDL.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "../../LIB/pugiXML/pugixml.hpp"

#include "../Controller/LogController.h"

#define DEVICE_KEYBOARD -1

namespace RetroEnd
{
	namespace Model
	{
		enum InputType
		{
			TYPE_AXIS,
			TYPE_BUTTON,
			TYPE_HAT,
			TYPE_KEY,
			TYPE_COUNT
		};

		struct Input
		{
		public:
			int device;
			InputType type;
			int id;
			int value;
			bool configured;

			Input()
			{
				device = DEVICE_KEYBOARD;
				configured = false;
				id = -1;
				value = -999;
				type = TYPE_COUNT;
			}

			Input(int dev, InputType t, int i, int val, bool conf) : device(dev), type(t), id(i), value(val), configured(conf)
			{
			}

			string getHatDir(int val)
			{
				if(val & SDL_HAT_UP)
					return "up";
				else if(val & SDL_HAT_DOWN)
					return "down";
				else if(val & SDL_HAT_LEFT)
					return "left";
				else if(val & SDL_HAT_RIGHT)
					return "right";
				return "neutral?";
			}

			string string()
			{
				if(!configured)
					return "";

				stringstream stream;
				switch(type)
				{
				case TYPE_BUTTON:
					stream << "Button " << id;
					break;
				case TYPE_AXIS:
					stream << "Axis " << id << (value > 0 ? "+" : "-");
					break;
				case TYPE_HAT:
					stream << "Hat " << id << " " << getHatDir(value);
					break;
				case TYPE_KEY:
					stream << "Key " << SDL_GetKeyName((SDLKey)id);
					break;
				default:
					stream << "Input to string error";
					break;
				}

				return (std::string) stream.str();
			}
		};

		class InputConfig
		{
		public:
			InputConfig(int deviceId);

			void clear();
			void mapInput(const string& name, Input input);
			void setPlayerNum(int num);

			int getPlayerNum();
			int getDeviceId();

			//Returns the input mapped to this name.
			Input getInputByName(const string& name);

			//Returns true if Input is mapped to this name, false otherwise.
			bool isMappedTo(const string& name, Input input);

			//Returns a list of names this input is mapped to.
			vector<string> getMappedTo(Input input);

			void loadFromXML(pugi::xml_node root, int playerNum);
			void writeToXML(pugi::xml_node parent);
		private:
			map<string, Input> mNameMap;
			const int mDeviceId;
			int mPlayerNum;
		};
	}
}
