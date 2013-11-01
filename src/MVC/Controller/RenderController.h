/* Render Controller class
 *
 * Handle the rendering of the Main Window over the local SDL implementation
 */

#pragma once

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

#include "../../globals.h"

#include GLHEADER

#include <SDL.h>
#include <Eigen/Dense>
#include <stack>

#include "BaseController.h"
#include "LogController.h"

#include "../View/BaseView.h"
#include "../View/MainWindow.h"
#include "../View/TestWindow.h"


using namespace std;
using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class PopupMessage
		{
		public:
			PopupMessage(string message, string icon) : Message(message), Icon(icon) { }

			string Message;
			string Icon;
		};

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
			void exit();
			void update();

			bool isRunning();
			
			int getScreenWidth();
			int getScreenHeight();

			View::BaseView* getCurrentWindow();

			void pushClipRect(Eigen::Vector4i box);
			void popClipRect();

			static void drawRect(int x, int y, int w, int h, unsigned int color);
			static void setMatrix(float* mat);
			static void setMatrix(const Eigen::Affine3f& transform);
			static void setColor4bArray(GLubyte* array, unsigned int color);
			static void buildGLColorArray(GLubyte* ptr, unsigned int color, unsigned int vertCount);
		
			void pushPopupMessage(string message, string icon = "");

		private:
			queue<PopupMessage*> mPopupMessages;
			void showPopupMessages();
			bool mShowingPopupMessage;

			bool mRunning;
			int mLastTime;
			SDL_Surface* mSdlScreen;
			View::BaseView* mainWindow;
			View::BaseView* mPopupView;
			std::stack<Eigen::Vector4i> clipStack;

			bool createSurface();
			void swapBuffers();

			//---Singleton---
			RenderController mInstance();

			RenderController() : mLastTime(0), mPopupView(NULL) { }; //private instance costructor for Singleton Controller

			RenderController(RenderController const&);// Don't Implement
			void operator=(RenderController const&); // Don't implement
			//---Singleton---
		};
	}
}
