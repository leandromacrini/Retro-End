/* Render Controller class
 *
 * Handle the rendering of the Main Window over the local SDL implementation
 */

#ifndef _RENDERCONTROLLER_H_
#define _RENDERCONTROLLER_H_

#include "../../globals.h"

#include "../View/MainWindow.h"

using namespace std;
using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class RenderController : public BaseController
		{

		public:
			void start();
			void stop();

		protected:

		private:
			View::MainWindow mWindow;
		};
	}
}

#endif _RENDERCONTROLLER_H_