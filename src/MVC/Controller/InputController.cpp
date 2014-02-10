#include "InputController.h"
#include "SettingsController.h"
#include "LogController.h"

using namespace RetroEnd::Controller;
using namespace RetroEnd::Model;

namespace fs = boost::filesystem;

/***************/
/**InputDevice**/
/***************/
InputDevice::InputDevice(const string & deviceName, unsigned long vendorId, unsigned long productId)
	: name(deviceName), vendor(vendorId), product(productId)
{

}

bool InputDevice::operator==(const InputDevice & b) const
{
	return (name == b.name && vendor == b.vendor && product == b.product);
}

/*******************/
/**InputController**/
/*******************/
void InputController::start()
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	mNumJoysticks = SDL_NumJoysticks();

	for(int i = 0; i < mNumJoysticks; i++)
	{
		SDL_Joystick* joy = SDL_JoystickOpen(i);
		SDL_JoystickID joyId = SDL_JoystickInstanceID(joy);
		mJoysticks.push_back(joy);
		mInputConfigs[joyId] = new InputConfig(i, SDL_JoystickName(joy));

		int numAxes = SDL_JoystickNumAxes(joy);
		mPrevAxisValues[joyId] = new int[numAxes];
		std::fill(mPrevAxisValues[joyId], mPrevAxisValues[joyId] + numAxes, 0); //initialize array to 0
	}

	mKeyboardInputConfig = new InputConfig(DEVICE_KEYBOARD, "Keyboard");

	loadConfig();
}

void InputController::stop()
{
	SDL_JoystickEventState(SDL_DISABLE);

	if(!SDL_WasInit(SDL_INIT_JOYSTICK)) return;

	for(auto iter = mJoysticks.begin(); iter != mJoysticks.end(); iter++)
	{
		SDL_JoystickClose(*iter);
	}

	mJoysticks.clear();

	for(auto iter = mInputConfigs.begin(); iter != mInputConfigs.end(); iter++)
	{
		delete iter->second;
	}

	mInputConfigs.clear();

	for(auto iter = mPrevAxisValues.begin(); iter != mPrevAxisValues.end(); iter++)
	{
		delete[] iter->second;
	}

	mPrevAxisValues.clear();

	if(mKeyboardInputConfig != NULL)
	{
		delete mKeyboardInputConfig;
		mKeyboardInputConfig = NULL;
	}

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

int InputController::getNumJoysticks()
{
	return mNumJoysticks;
}

int InputController::getButtonCountByDevice(int id)
{
	if(id == DEVICE_KEYBOARD)
		return 120; //it's a lot, okay.
	else
		return SDL_JoystickNumButtons(mJoysticks[id]);
}

int InputController::getNumPlayers()
{
	return mNumPlayers;
}

void InputController::setNumPlayers(int num)
{
	mNumPlayers = num;
}

InputConfig* InputController::getInputConfigByDevice(int device)
{
	if(device == DEVICE_KEYBOARD)
		return mKeyboardInputConfig;
	else
		return mInputConfigs[device];
}

InputConfig* InputController::getInputConfigByPlayer(int player)
{
	if(mKeyboardInputConfig->getPlayerNum() == player)
		return mKeyboardInputConfig;

	for(auto iter = mInputConfigs.begin(); iter != mInputConfigs.end(); iter++)
	{
		if(iter->second->getPlayerNum() == player)
		{
			return iter->second;
		}
	}

	LOG(LogLevel::Error, "Could not find input config for player number " + to_string(player) + "!")
		return NULL;
}

bool InputController::parseEvent(const SDL_Event& ev)
{
	LOG(LogLevel::Debug, "InputController::parseEvent -> " + to_string(ev.type));

	View::BaseView* mWindow = RenderController::getInstance().getCurrentWindow();

	bool causedEvent = false;
	switch(ev.type)
	{
	case SDL_JOYAXISMOTION:
		//if it switched boundaries
		if((abs(ev.jaxis.value) > DEADZONE) != (abs(mPrevAxisValues[ev.jaxis.which][ev.jaxis.axis]) > DEADZONE))
		{
			int normValue;
			if(abs(ev.jaxis.value) <= DEADZONE)
				normValue = 0;
			else
				if(ev.jaxis.value > 0)
					normValue = 1;
				else
					normValue = -1;

			mWindow->input(getInputConfigByDevice(ev.jaxis.which), Input(ev.jaxis.which, TYPE_AXIS, ev.jaxis.axis, normValue, false));
			causedEvent = true;
		}

		mPrevAxisValues[ev.jaxis.which][ev.jaxis.axis] = ev.jaxis.value;
		return causedEvent;

	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		mWindow->input(getInputConfigByDevice(ev.jbutton.which), Input(ev.jbutton.which, TYPE_BUTTON, ev.jbutton.button, ev.jbutton.state == SDL_PRESSED, false));
		return true;

	case SDL_JOYHATMOTION:
		mWindow->input(getInputConfigByDevice(ev.jhat.which), Input(ev.jhat.which, TYPE_HAT, ev.jhat.hat, ev.jhat.value, false));
		return true;

	case SDL_KEYDOWN:
		if(ev.key.keysym.sym == SDLK_F4)
		{
			SDL_Event* quit = new SDL_Event();
			quit->type = SDL_QUIT;
			SDL_PushEvent(quit);
			return false;
		}

		mWindow->input(getInputConfigByDevice(DEVICE_KEYBOARD), Input(DEVICE_KEYBOARD, TYPE_KEY, ev.key.keysym.sym, 1, false));
		return true;

	case SDL_KEYUP:
		mWindow->input(getInputConfigByDevice(DEVICE_KEYBOARD), Input(DEVICE_KEYBOARD, TYPE_KEY, ev.key.keysym.sym, 0, false));
		return true;

	case SDL_JOYDEVICEADDED:
		//stop();
		//start();
		return true;
	}

	return false;
}

void InputController::loadConfig()
{
	//if there are no joysticks we have nothing to do
	if(mNumJoysticks == 0) return;

	//in the DB we save a key-value pair for each device we have configured
	//the key is the device name
	//the value is the XML based configuration

	mNumPlayers = 0;

	for(int i = 0; i < mNumJoysticks; i++)
	{
		mInputConfigs[i]->setPlayerNum(-1);
		string name = SDL_JoystickNameForIndex(i);

		std::string config = SettingsController::getInstance().getStringProperty(name, "");

		if(config.empty())
		{
			LOG(LogLevel::Warning, "Could not find configuration for joystick named \"" + name + "\"! Skipping it.\n");
			continue;
		}
		
		pugi::xml_document doc;
		pugi::xml_parse_result res = doc.load(config.c_str());

		pugi::xml_node node = doc.child("inputConfig");

		string type = node.attribute("type").as_string();

		if(type == "keyboard")
		{
			getInputConfigByDevice(DEVICE_KEYBOARD)->loadFromXML(node, mNumPlayers);
			mNumPlayers++;
		}else if(type == "joystick")
		{
			mInputConfigs[i]->loadFromXML(node, mNumPlayers);
			mNumPlayers++;
			break;

		}else{
			LOG(LogLevel::Warning, "Device type \"" + type + "\" unknown!\n");
		}
	}

	if(mNumPlayers == 0)
	{
		LOG(LogLevel::Info, "No input configs loaded. Loading default keyboard config.");
		loadDefaultConfig();
	}

	LOG(LogLevel::Info, "Loaded InputConfig data for " + to_string(getNumPlayers()) + " devices.");
}

//used in an "emergency" where no configs could be loaded from the InputController config file
//allows the user to select to reconfigure in menus if this happens without having to delete es_input.cfg manually
void InputController::loadDefaultConfig()
{
	InputConfig* cfg = getInputConfigByDevice(DEVICE_KEYBOARD);

	mNumPlayers++;
	cfg->setPlayerNum(0);
	cfg->mapInput("up", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_UP, 1, true));
	cfg->mapInput("down", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_DOWN, 1, true));
	cfg->mapInput("left", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_LEFT, 1, true));
	cfg->mapInput("right", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_RIGHT, 1, true));

	cfg->mapInput("a", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_RETURN, 1, true));
	cfg->mapInput("b", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_ESCAPE, 1, true));
	cfg->mapInput("menu", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_F1, 1, true));
	cfg->mapInput("select", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_F2, 1, true));
	cfg->mapInput("pageup", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_RIGHTBRACKET, 1, true));
	cfg->mapInput("pagedown", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_LEFTBRACKET, 1, true));

	cfg->mapInput("mastervolup", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_PLUS, 1, true));
	cfg->mapInput("mastervoldown", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_MINUS, 1, true));
}

void InputController::writeConfig()
{

	string path = getConfigPath();

	pugi::xml_document doc;

	pugi::xml_node root = doc.append_child("inputList");

	mKeyboardInputConfig->writeToXML(root);
	for(int i = 0; i < mNumJoysticks; i++)
	{
		mInputConfigs[i]->writeToXML(root);
	}

	doc.save_file(path.c_str());
}

string InputController::getConfigPath()
{

	string path = "";//TODO getHomePath();
	path += "/.emulationstation/es_input.cfg";
	return path;
}

void InputController::update()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		LOG(LogLevel::Debug, "InputController::SDL_PollEvent -> " + to_string(event.type));

		switch(event.type)
		{
		case SDL_QUIT:
			RenderController::getInstance().exit();
			break;
		
		default:
			parseEvent(event);
			break;
		}
	}
}