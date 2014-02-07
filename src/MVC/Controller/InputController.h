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

			void start();
			void stop();
			void update();

			//EVENTS

			Model::Observer<int> onNewControllerDetected;
			Model::Observer<int> onControllerRemoved;

		protected:

		private:
			static const int DEADZONE = 23000;

			void loadConfig();
			void writeConfig();
			static std::string getConfigPath();

			void setNumPlayers(int num);
			int getNumPlayers();

			int getNumJoysticks();
			int getButtonCountByDevice(int id);

			bool parseEvent(const SDL_Event& ev);

			Model::InputConfig* getInputConfigByPlayer(int player);
			Model::InputConfig* getInputConfigByDevice(int device);

			void loadDefaultConfig();

			int mNumJoysticks;
			int mNumPlayers;
			std::vector<SDL_Joystick*> mJoysticks;
			std::map<SDL_JoystickID, Model::InputConfig*> mInputConfigs;

			Model::InputConfig* mKeyboardInputConfig;

			std::map<SDL_JoystickID, int*> mPrevAxisValues;

			//---Singleton---
			InputController mInstance();

			//private instance costructor for Singleton Controller
			InputController() : mKeyboardInputConfig(NULL), mNumJoysticks(0), mNumPlayers(0) { };
			InputController(InputController const&);// Don't Implement
			void operator=(InputController const&); // Don't implement
			//---Singleton---
		};
	}
}
