/* Input Controller class
 *
 * Handle the inputs and generates the internal events
 */

#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "../../globals.h"

#include "BaseController.h"

using namespace std;
using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class InputController : public BaseController
		{
		public:
			static InputController& getInstance()
			{
				static InputController instance;
				return instance;
			}

		protected:

		private:
			//---Singleton---
			InputController mInstance();

			InputController() {}; //private instance costructor for Singleton Controller
			InputController(InputController const&);// Don't Implement
			void operator=(InputController const&); // Don't implement
			//---Singleton---
		};
	}
}

#endif _INPUTCONTROLLER_H_