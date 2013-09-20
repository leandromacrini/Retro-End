/* Render Controller class
 *
 * Handle the rendering of the Main Window over the local SDL implementation
 */

#ifndef _RENDERCONTROLLER_H_
#define _RENDERCONTROLLER_H_

//the Makefiles define these via command line
//#define USE_OPENGL_ES
//#define USE_OPENGL_DESKTOP

#ifdef USE_OPENGL_ES
#define GLHEADER <GLES/gl.h>
#endif

#ifdef USE_OPENGL_DESKTOP
//why the hell this naming inconsistency exists is well beyond me
#ifdef WIN32
#define sleep Sleep
#endif

#define GLHEADER <SDL_opengl.h>
#endif

#ifdef _WINDOWS_
#include <Windows.h>
#endif

#include GLHEADER

#include <SDL.h>

#include "../../globals.h"

#include "BaseController.h"
#include "LogController.h"
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
			static RenderController& getInstance()
			{
				static RenderController instance;
				return instance;
			}

			void start();
			void stop();

			void update();

			bool isRunning();
		protected:

		private:
			bool mRunning;
			int mLastTime;
			SDL_Surface* mSdlScreen;
			View::MainWindow window;

			bool createSurface();
			void swapBuffers();

			//---Singleton---
			RenderController mInstance();

			RenderController() : mLastTime(0) {}; //private instance costructor for Singleton Controller
			RenderController(RenderController const&);// Don't Implement
			void operator=(RenderController const&); // Don't implement
			//---Singleton---
		};
	}
}

#endif _RENDERCONTROLLER_H_