/* Input Controller class
*
* Handle the inputs and generates the internal events
*/

#pragma once

#include <map>
#include <SDL.h>
#include <boost/filesystem.hpp>

#include "../../globals.h"

#include "../Model/Observer.h"

#include "BaseController.h"
#include "LogController.h"
#include "RenderController.h"

#include "../Model/InputConfig.h"

using namespace std;
using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		struct InputDevice
		{
			string name;
			unsigned long vendor;
			unsigned long product;

			InputDevice(const string & deviceName, unsigned long vendorId, unsigned long productId);
			bool operator==(const InputDevice & b) const;
		};

		class InputController : public BaseController
		{
		public:


			static InputController& getInstance()
			{
				static InputController instance;
				return instance;
			}

			void start();
			void stop();
			void update();

			//EVENTS

			Model::Observer<int> onNewControllerDetected;
			Model::Observer<int> onControllerRemoved;

		protected:

		private:
			static const int DEADZONE = 23000;
			static const int SDL_USEREVENT_POLLDEVICES = SDL_USEREVENT + 100; //This event is issued when the input devices should be rescanned.

			void loadConfig();
			void writeConfig();
			static std::string getConfigPath();

			void setNumPlayers(int num);
			int getNumPlayers();

			int getNumJoysticks();
			int getButtonCountByDevice(int id);

			bool parseEvent(const SDL_Event& ev);

			Model::InputConfig* getInputConfigByPlayer(int player);

			void startPolling();
			void stopPolling();

			//non-InputManager classes shouldn't use this, as you can easily miss the keyboard
			Model::InputConfig* getInputConfigByDevice(int device);

			void loadDefaultConfig();

			int mNumJoysticks;
			int mNumPlayers;
			SDL_Joystick** mJoysticks;
			Model::InputConfig** mInputConfigs;
			Model::InputConfig* mKeyboardInputConfig;
			map<int, int>* mPrevAxisValues;

			vector<InputDevice> inputDevices;

			/*!
			Retrieve joysticks/ HID devices from system.
			\return Returns a list of InputDevices that can be compared to the current /inputDevices to check if the configuration has changed.
			\note This currently reads the content of the /dev/input on linux, searches for "js**" names and stores those. On Windows it uses GetRawInputDeviceInfo to find devices of type RIM_TYPEHID and stores those.
			*/
			vector<InputDevice> getInputDevices() const;

			static const int POLLING_INTERVAL = 5000;
			SDL_TimerID devicePollingTimer;

			/*!
			Called when devicePollingTimer runs out. Sends a SDL_UserEvent with type SDL_USEREVENT_POLLDEVICES to the event queue.
			*/
			static Uint32 devicePollingCallback(Uint32 interval, void * param);

			//---Singleton---
			InputController mInstance();

			//private instance costructor for Singleton Controller
			InputController() :mJoysticks(NULL), mInputConfigs(NULL), mKeyboardInputConfig(NULL), mPrevAxisValues(NULL), mNumJoysticks(0), mNumPlayers(0), devicePollingTimer(NULL) { };
			InputController(InputController const&);// Don't Implement
			void operator=(InputController const&); // Don't implement
			//---Singleton---
		};
	}
}
