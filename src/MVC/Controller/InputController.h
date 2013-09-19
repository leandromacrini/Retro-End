/* Input Controller class
 *
 * Handle the inputs and generates the internal events
 */

#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "../../globals.h"

using namespace std;
using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class InputController : public BaseController
		{

		public:
			void start();
			void stop();

		protected:

		private:

		};
	}
}

#endif _INPUTCONTROLLER_H_