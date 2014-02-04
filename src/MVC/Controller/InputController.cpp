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
	//get current input devices from system
	inputDevices = getInputDevices();

	SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_TIMER);

	mNumJoysticks = SDL_NumJoysticks();
	mJoysticks = new SDL_Joystick*[mNumJoysticks];
	mInputConfigs = new InputConfig*[mNumJoysticks];
	mPrevAxisValues = new map<int, int>[mNumJoysticks];

	for(int i = 0; i < mNumJoysticks; i++)
	{
		mJoysticks[i] = SDL_JoystickOpen(i);
		mInputConfigs[i] = new InputConfig(i);

		for(int k = 0; k < SDL_JoystickNumAxes(mJoysticks[i]); k++)
		{
			mPrevAxisValues[i][k] = 0;
		}
	}

	mKeyboardInputConfig = new InputConfig(DEVICE_KEYBOARD);

	SDL_JoystickEventState(SDL_ENABLE);

	//start timer for input device polling
	startPolling();

	loadConfig();
}

void InputController::stop()
{
	stopPolling();

	SDL_JoystickEventState(SDL_DISABLE);

	if(!SDL_WasInit(SDL_INIT_JOYSTICK))
		return;

	if(mJoysticks != NULL)
	{
		for(int i = 0; i < mNumJoysticks; i++)
		{
			SDL_JoystickClose(mJoysticks[i]);
			delete mInputConfigs[i];
		}

		delete[] mInputConfigs;
		mInputConfigs = NULL;

		delete[] mJoysticks;
		mJoysticks = NULL;

		delete mKeyboardInputConfig;
		mKeyboardInputConfig = NULL;

		delete[] mPrevAxisValues;
		mPrevAxisValues = NULL;
	}

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_TIMER);

	inputDevices.clear();
}

void InputController::startPolling()
{
	if(devicePollingTimer != 0)
		return;

	devicePollingTimer = SDL_AddTimer(POLLING_INTERVAL, devicePollingCallback, (void *)this);
}

void InputController::stopPolling()
{
	if(devicePollingTimer == 0)
		return;

	SDL_RemoveTimer(devicePollingTimer);
	devicePollingTimer = 0;
}


int InputController::getNumJoysticks() { return mNumJoysticks; }
int InputController::getButtonCountByDevice(int id)
{
	if(id == DEVICE_KEYBOARD)
		return 120; //it's a lot, okay.
	else
		return SDL_JoystickNumButtons(mJoysticks[id]);
}

int InputController::getNumPlayers() { return mNumPlayers; }
void InputController::setNumPlayers(int num) { mNumPlayers = num; }

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

	for(int i = 0; i < mNumJoysticks; i++)
	{
		if(mInputConfigs[i]->getPlayerNum() == player)
			return mInputConfigs[i];
	}

	LOG(LogLevel::Error, "Could not find input config for player number " + to_string(player) + "!")
		return NULL;
}

bool InputController::parseEvent(const SDL_Event& ev)
{
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

	case SDL_USEREVENT:
		if (ev.user.code == SDL_USEREVENT_POLLDEVICES) {
			//poll joystick / HID again
			vector<InputDevice> currentDevices = getInputDevices();
			//compare device lists to see if devices were added/deleted
			if (currentDevices != inputDevices) {
				LOG(LogLevel::Info, "Device configuration changed!");
				//deinit and reinit InputController
				//TODO
				//deinit();
				//init();

				if(currentDevices.size() > inputDevices.size())
				{
					onNewControllerDetected(0);
					RenderController::getInstance().pushPopupMessage("New controller detected!", PopupMessageIcon::Controller_Added);
				}
				else
				{
					onControllerRemoved(0);
					RenderController::getInstance().pushPopupMessage("Controller removed!", PopupMessageIcon::Controller_Removed);
				}

				inputDevices = currentDevices;
			}
			return true;
		}
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
		string name = SDL_JoystickName(i);

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

	cfg->mapInput("sortordernext", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_F7, 1, true));
	cfg->mapInput("sortorderprevious", Input(DEVICE_KEYBOARD, TYPE_KEY, SDLK_F8, 1, true));
}

void InputController::writeConfig()
{
	if(!mJoysticks)
	{
		LOG(LogLevel::Error, "ERROR - cannot write config without being initialized!");
		return;
	}

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

Uint32 InputController::devicePollingCallback(Uint32 interval, void* param)
{
	//this thing my be running in a different thread, so we're not allowed to call
	//any functions or change/allocate/delete stuff, but can send a user event
	SDL_Event event;
	event.user.type = SDL_USEREVENT;
	event.user.code = SDL_USEREVENT_POLLDEVICES;
	event.user.data1 = nullptr;
	event.user.data2 = nullptr;
	if (SDL_PushEvent(&event) != 0) {
		LOG(LogLevel::Error, "InputManager::devicePollingCallback - SDL event queue is full!");
	}

	return interval;
}

std::vector<InputDevice> InputController::getInputDevices() const
{
	std::vector<InputDevice> currentDevices;

	//retrieve all input devices from system
#if defined (__APPLE__)
#error TODO: Not implemented for MacOS yet!!!
#elif defined(__linux__)
	//open linux input devices file system
	const std::string inputPath("/dev/input");
	fs::directory_iterator dirIt(inputPath);
	while (dirIt != fs::directory_iterator()) {
		//get directory entry
		std::string deviceName = (*dirIt).path().string();
		//remove parent path
		deviceName.erase(0, inputPath.length() + 1);
		//check if it start with "js"
		if (deviceName.length() >= 3 && deviceName.find("js") == 0) {
			//looks like a joystick. add to devices.
			currentDevices.push_back(InputDevice(deviceName, 0, 0));
		}
		++dirIt;
	}
	//or dump /proc/bus/input/devices anbd search for a Handler=..."js"... entry
#elif defined(WIN32) || defined(_WIN32)
	RAWINPUTDEVICELIST * deviceList = nullptr;
	UINT nrOfDevices = 0;
	//get number of input devices
	if (GetRawInputDeviceList(deviceList, &nrOfDevices, sizeof(RAWINPUTDEVICELIST)) != -1 && nrOfDevices > 0)
	{
		//get list of input devices
		deviceList = new RAWINPUTDEVICELIST[nrOfDevices];
		if (GetRawInputDeviceList(deviceList, &nrOfDevices, sizeof(RAWINPUTDEVICELIST)) != -1)
		{
			//loop through input devices
			for (unsigned int i = 0; i < nrOfDevices; i++)
			{
				//get device name
				char * rawName = new char[2048];
				UINT rawNameSize = 2047;
				GetRawInputDeviceInfo(deviceList[i].hDevice, RIDI_DEVICENAME, (void *)rawName, &rawNameSize);
				//null-terminate string
				rawName[rawNameSize] = '\0';
				//convert to string
				std::string deviceName = rawName;
				delete [] rawName;
				//get deviceType
				RID_DEVICE_INFO deviceInfo;
				UINT deviceInfoSize = sizeof(RID_DEVICE_INFO);
				GetRawInputDeviceInfo(deviceList[i].hDevice, RIDI_DEVICEINFO, (void *)&deviceInfo, &deviceInfoSize);
				//check if it is a HID. we ignore keyboards and mice...
				if (deviceInfo.dwType == RIM_TYPEHID)
				{
					//check if the vendor/product already exists in list. yes. could be more elegant...
					std::vector<InputDevice>::const_iterator cdIt = currentDevices.cbegin();
					while (cdIt != currentDevices.cend())
					{
						if (cdIt->name == deviceName && cdIt->product == deviceInfo.hid.dwProductId && cdIt->vendor == deviceInfo.hid.dwVendorId)
						{
							//device already there
							break;
						}
						++cdIt;
					}
					//was the device found?
					if (cdIt == currentDevices.cend())
					{
						//no. add it.
						currentDevices.push_back(InputDevice(deviceName, deviceInfo.hid.dwProductId, deviceInfo.hid.dwVendorId));
					}
				}
			}
		}
		delete [] deviceList;
	}
#endif

	return currentDevices;
}

void InputController::update()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_JOYHATMOTION:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_JOYAXISMOTION:
		case SDL_USEREVENT:
			parseEvent(event);
			break;
		case SDL_QUIT:
			RenderController::getInstance().exit();
			break;
		}
	}
}