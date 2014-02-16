#include "InputConfig.h"

using namespace RetroEnd::Model;

//some util functions
string inputTypeToString(InputType type)
{
	switch(type)
	{
	case TYPE_AXIS:
		return "axis";
	case TYPE_BUTTON:
		return "button";
	case TYPE_HAT:
		return "hat";
	case TYPE_KEY:
		return "key";
	default:
		return "error";
	}
}

InputType stringToInputType(const string& type)
{
	if(type == "axis")
		return TYPE_AXIS;
	if(type == "button")
		return TYPE_BUTTON;
	if(type == "hat")
		return TYPE_HAT;
	if(type == "key")
		return TYPE_KEY;
	return TYPE_COUNT;
}


string toLower(string str)
{
	for(unsigned int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}

	return str;
}
//end util functions

InputConfig::InputConfig(int deviceId, const string& deviceName, const string& guid) : mDeviceId(deviceId), mDeviceName(deviceName), mPlayerNum(-1), mGuid(guid)
{
	
}

void InputConfig::clear()
{
	mNameMap.clear();
}

void InputConfig::mapInput(const string& name, Input input)
{
	mNameMap[toLower(name)] = input;
}

Input InputConfig::getInputByName(const string& name)
{
	return mNameMap[toLower(name)];
}

bool InputConfig::isMappedTo(const string& name, Input input)
{
	Input comp = getInputByName(name);

	if(comp.configured && comp.type == input.type && comp.id == input.id)
	{
		if(comp.type == TYPE_HAT)
		{
			return (input.value == 0 || input.value & comp.value);
		}

		if(comp.type == TYPE_AXIS)
		{
			return input.value == 0 || comp.value == input.value;
		}else{
			return true;
		}
	}
	return false;
}

vector<string> InputConfig::getMappedTo(Input input)
{
	vector<string> maps;

	typedef map<string, Input>::iterator it_type;
	for(it_type iterator = mNameMap.begin(); iterator != mNameMap.end(); iterator++)
	{
		Input chk = iterator->second;

		if(!chk.configured)
			continue;

		if(chk.device == input.device && chk.type == input.type && chk.id == input.id)
		{
			if(chk.type == TYPE_HAT)
			{
				if(input.value == 0 || input.value & chk.value)
				{
					maps.push_back(iterator->first);
				}
				continue;
			}

			if(input.type == TYPE_AXIS)
			{
				if(input.value == 0 || chk.value == input.value)
					maps.push_back(iterator->first);
			}else{
				maps.push_back(iterator->first);
			}
		}
	}

	return maps;
}

void InputConfig::loadFromXML(pugi::xml_node node, int playerNum)
{
	this->clear();

	setPlayerNum(playerNum);

	for(pugi::xml_node input = node.child("input"); input; input = input.next_sibling("input"))
	{
		string name = input.attribute("name").as_string();
		string type = input.attribute("type").as_string();
		InputType typeEnum = stringToInputType(type);

		if(typeEnum == TYPE_COUNT)
		{
			LOG(LogLevel::Error, "InputConfigModel load error - input of type \"" + type + "\" is invalid! Skipping input \"" + name + "\".\n");
			continue;
		}

		int id = input.attribute("id").as_int();
		int value = input.attribute("value").as_int();

		if(value == 0)
			LOG(LogLevel::Warning, "WARNING: InputConfigModel value is 0 for " + type + " " + to_string(id) + "!\n");

		mNameMap[toLower(name)] = Input(mDeviceId, typeEnum, id, value, true);
	}
}

void InputConfig::writeToXML(pugi::xml_node parent)
{
	pugi::xml_node cfg = parent.append_child("inputConfig");

	if(mDeviceId == DEVICE_KEYBOARD)
	{
		cfg.append_attribute("type") = "keyboard";
	}else{
		cfg.append_attribute("type") = "joystick";
		cfg.append_attribute("deviceName") = SDL_JoystickNameForIndex(mDeviceId);

	}

	typedef map<string, Input>::iterator it_type;
	for(it_type iterator = mNameMap.begin(); iterator != mNameMap.end(); iterator++)
	{
		if(!iterator->second.configured)
			continue;

		pugi::xml_node input = cfg.append_child("input");
		input.append_attribute("name") = iterator->first.c_str();
		input.append_attribute("type") = inputTypeToString(iterator->second.type).c_str();
		input.append_attribute("id").set_value(iterator->second.id);
		input.append_attribute("value").set_value(iterator->second.value);
	}
}

void InputConfig::setPlayerNum(int num) { mPlayerNum = num; }
int InputConfig::getPlayerNum() { return mPlayerNum; }
int InputConfig::getDeviceId() { return mDeviceId; }
