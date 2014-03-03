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
	LOG(LogLevel::Info, "InputController is creating and updating all the tables");
	//init all the models so the tables are always updated
	InputConfig::init();

	//keyboard is always configured and has fixed values
	loadDefaultKeyboardConfig();

	GamingController::getInstance().onGameStart += [this](Game game)
	{
		//release the joystick control before emulation
		disableJoystickHandling();
	};

	GamingController::getInstance().onGameEnd += [this](Game game)
	{
		//retake the joystick control after emulation
		enableJoystickHandling();
	};

	LOG(LogLevel::Info, "InputController started.");
}

void InputController::stop()
{
	disableJoystickHandling();
}

void InputController::disableJoystickHandling()
{
	//release all joystick
	for(auto iter = mPlayersConfig.begin(); iter != mPlayersConfig.end(); iter++)
	{
		if(SDL_JoystickGetAttached(iter->second->Joystick)) SDL_JoystickClose(iter->second->Joystick);
	}
	mPlayersConfig.clear();

	//disable joystick handling
	SDL_JoystickEventState(SDL_DISABLE);
	
	//de init SDL Joyatick
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void InputController::enableJoystickHandling()
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);
}

void InputController::addJoystick(int deviceID)
{
	SDL_Joystick* joy = SDL_JoystickOpen(deviceID);
	SDL_JoystickID joyId = SDL_JoystickInstanceID(joy);
		
	char* buffer = new char[33];

	SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), buffer, 33);
		
	string guid = string(buffer);

	InputConfig* config = new InputConfig(SDL_JoystickName(joy), guid);

	config->Joystick = joy;
	
	//TODO loadConfig(); re do this
	mPlayersConfig[joyId] = config;

	config->PlayerNumber = mPlayersConfig.size();

	//check if we have the configuration for this joystick on DB or ask the user to configure
	if(!config->TryLoadFromDB())
		onControllerNeedConfiguration(config);
	else
		RenderController::getInstance().pushPopupMessage("Controller recognized\nUsed for player "+to_string(config->PlayerNumber), PopupMessageIcon::Controller_Added);
	
	LOG(LogLevel::Info, "InputController : addJoystick " + guid);
	onControllerAdded(config->PlayerNumber);
}

void InputController::delJoystick(int joystickID)
{
	//search and remove the joystick and then shift all the players by one
	SDL_JoystickID found = -1;
	for(auto iter = mPlayersConfig.begin(); iter != mPlayersConfig.end(); iter++)
	{
		if(iter->first == joystickID)
		{
			found = iter->first;
			continue;
		}
		
		if(found != -1)
		{
			iter->second->PlayerNumber--;
		}
	}

	if(found != -1)
	{	
		
		int player = mPlayersConfig[found]->PlayerNumber;

		if(SDL_JoystickGetAttached(mPlayersConfig[found]->Joystick)) SDL_JoystickClose(mPlayersConfig[found]->Joystick);
		mPlayersConfig.erase(found);

		RenderController::getInstance().pushPopupMessage("Controller removed for player "+to_string(player), PopupMessageIcon::Controller_Removed);
		onControllerRemoved(player);
	}
}

bool InputController::parseEvent(const SDL_Event& ev)
{
	LOG(LogLevel::Debug, "InputController::parseEvent -> " + to_string(ev.type));

	View::BaseView* mWindow = RenderController::getInstance().getCurrentWindow();

	switch(ev.type)
	{
	case SDL_JOYAXISMOTION:
		{

			//if it switched boundaries
			int normValue;
			if(abs(ev.jaxis.value) <= DEADZONE)
				normValue = 0;
			else
				if(ev.jaxis.value > 0)
					normValue = 1;
				else
					normValue = -1;

			InputConfig* config = mPlayersConfig[ev.jaxis.which];

			if(config == NULL) return true;

			InputRaw raw = InputRaw(normValue < 0 ? TYPE_AXIS_NEGATIVE : TYPE_AXIS_POSITIVE, ev.jaxis.axis);

			InputSemantic semantic = config->getInputMapKey(raw);

			mWindow->input(Input(abs(normValue), config->PlayerNumber, semantic, raw.Type, raw.ValueID, ev.jaxis.timestamp));
			return true;
		}
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		{
			//check if the user is pressing start + select = System button

			InputConfig* config = mPlayersConfig[ev.jaxis.which];

			InputRaw start = config->getInputMapValue(InputSemantic::START);
			InputRaw select= config->getInputMapValue(InputSemantic::SELECT);

			if( start.Type != TYPE_UNKNOWN && select.Type != TYPE_UNKNOWN )
			{
				bool startPressed  = SDL_JoystickGetButton(config->Joystick, start.ValueID ) != 0;
				bool selectPressed = SDL_JoystickGetButton(config->Joystick, select.ValueID ) != 0;

				if(startPressed && selectPressed)
				{
					SDL_Event* quit = new SDL_Event();
					quit->type = SDL_QUIT;
					SDL_PushEvent(quit);
					return false;
					//TODO exit window
					//mWindow->input(Input(SDL_PRESSED, config->PlayerNumber, InputSemantic::SYSTEM, TYPE_BUTTON, VALUE_ID_UNKNOWN, ev.jbutton.timestamp));
					//return true;
				}
			}
			
			InputRaw raw = InputRaw(TYPE_BUTTON, ev.jbutton.button);
			InputSemantic semantic = config->getInputMapKey(raw);

			mWindow->input(Input(ev.jbutton.state, config->PlayerNumber, semantic, raw.Type, raw.ValueID, ev.jbutton.timestamp));
			return true;
		}
	case SDL_JOYHATMOTION:
		//TODO understand what is this
		//mWindow->input(getInputConfigByDevice(ev.jhat.which), Input(ev.jhat.which, TYPE_HAT, ev.jhat.hat, ev.jhat.value, false));
		return true;

	case SDL_KEYUP:
	case SDL_KEYDOWN:
		{
			if(ev.key.keysym.sym == SDLK_F4)
			{
				SDL_Event* quit = new SDL_Event();
				quit->type = SDL_QUIT;
				SDL_PushEvent(quit);
				return false;
				//TODO exit window
				//mWindow->input(Input(SDL_PRESSED, config->PlayerNumber, InputSemantic::SYSTEM, TYPE_BUTTON, VALUE_ID_UNKNOWN, ev.jbutton.timestamp));
				//return true;
			}
			
			InputRaw raw = InputRaw(TYPE_KEY, ev.key.keysym.sym);
			InputSemantic semantic = mKeyboardInputConfig->getInputMapKey(raw);

			mWindow->input(Input(ev.key.state, mKeyboardInputConfig->PlayerNumber, semantic, raw.Type, raw.ValueID, ev.key.timestamp));
			return true;
		}
	
	case SDL_JOYDEVICEADDED:
		{
			addJoystick(ev.jdevice.which);
			return true;
		}
	case SDL_JOYDEVICEREMOVED:
		{
			delJoystick(ev.jdevice.which);
			return true;
		}
	}

	return false;
}


void InputController::loadDefaultKeyboardConfig()
{
	mKeyboardInputConfig = new InputConfig("Keyboard", "DEFAULT_KEYBOARD_CONFIGURATION");

	mKeyboardInputConfig->setInputMap(InputSemantic::UP, InputRaw(TYPE_KEY, SDLK_UP));
	mKeyboardInputConfig->setInputMap(InputSemantic::DOWN, InputRaw(TYPE_KEY, SDLK_DOWN));
	mKeyboardInputConfig->setInputMap(InputSemantic::LEFT, InputRaw(TYPE_KEY, SDLK_LEFT));
	mKeyboardInputConfig->setInputMap(InputSemantic::RIGHT, InputRaw(TYPE_KEY, SDLK_RIGHT));

	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_A, InputRaw(TYPE_KEY,  SDLK_x));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_B, InputRaw(TYPE_KEY,  SDLK_z));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_X, InputRaw(TYPE_KEY,  SDLK_s));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_Y, InputRaw(TYPE_KEY,  SDLK_a));

	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_L, InputRaw(TYPE_KEY,  SDLK_e));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_L2, InputRaw(TYPE_KEY,  SDLK_w));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_L3, InputRaw(TYPE_KEY,  SDLK_q));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_R, InputRaw(TYPE_KEY,  SDLK_r));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_R2, InputRaw(TYPE_KEY,  SDLK_t));
	mKeyboardInputConfig->setInputMap(InputSemantic::BUTTON_R3, InputRaw(TYPE_KEY,  SDLK_y));

	mKeyboardInputConfig->setInputMap(InputSemantic::SELECT, InputRaw(TYPE_KEY,  SDLK_SPACE));
	mKeyboardInputConfig->setInputMap(InputSemantic::START, InputRaw(TYPE_KEY,  SDLK_RETURN));
	mKeyboardInputConfig->setInputMap(InputSemantic::SYSTEM, InputRaw(TYPE_KEY,  SDLK_ESCAPE));

	mKeyboardInputConfig->PlayerNumber = 1;
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