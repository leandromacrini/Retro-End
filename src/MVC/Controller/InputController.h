/* Input Controller class
*
* Handle the inputs and generates the internal events
*/

#pragma once

#include <map>
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

			//activation of the controller
			void start();
			void stop();
			void update();

			//starts getting joystick events
			void enableJoystickHandling();
			void disableJoystickHandling();

			//handle inputConfig creation
			void addJoystick(int deviceID);		//deviceID is the Nth joystick in the system
			void delJoystick(int joystickID);	//joystickID is the SDL ID of the opened SDL_Joystick

			//EVENTS
			Model::Observer<int> onControllerAdded;
			Model::Observer<int> onControllerRemoved;
			Model::Observer<Model::InputConfig* > onControllerNeedConfiguration;

		protected:

		private:
			//analog to digital value limit
			static const int DEADZONE = 23000;

			bool parseEvent(const SDL_Event& ev);

			void loadDefaultKeyboardConfig();

			std::map<SDL_JoystickID, Model::InputConfig*> mPlayersConfig;

			Model::InputConfig* mKeyboardInputConfig;

			//---Singleton---
			InputController mInstance();

			//private instance costructor for Singleton Controller
			InputController() : mKeyboardInputConfig(NULL) { };
			InputController(InputController const&);// Don't Implement
			void operator=(InputController const&); // Don't implement
			//---Singleton---
		};
	}
}
