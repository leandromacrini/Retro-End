#include "RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

bool RenderController::createSurface() //unsigned int display_width, unsigned int display_height)
{
	LOG(LogLevel::Info, "Creating surface...");

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		string msg = "Error initializing SDL!\n	";
		msg += SDL_GetError();

		LOG(LogLevel::Error, msg);
		return false;
	}

	SDL_WM_SetCaption("Retro-End", "Retro-End");

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef USE_OPENGL_ES
#define glOrtho glOrthof
#endif    

#ifdef _DEBUG
    mSdlScreen = SDL_SetVideoMode(1280, 720, 16, SDL_OPENGL);
#else
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); //vsync
	mSdlScreen = SDL_SetVideoMode(0, 0, 16, SDL_OPENGL | SDL_FULLSCREEN);
#endif

	if(mSdlScreen == NULL)
	{
		LOG(LogLevel::Fatal, "Error creating SDL video surface!");
		return false;
	}

	LOG(LogLevel::Info, "Created surface successfully.");

	SDL_ShowCursor(0);

	return true;
}

void RenderController::start()
{
	initSDL();

	//create the main window
	mainWindow = new View::MainWindow();
	mPopupView = NULL;

	LOG(LogLevel::Info, "RenderController started");
}

void RenderController::stop()
{
	quitSDL();
	LOG(LogLevel::Info, "RenderController stoped");
}

void RenderController::initSDL() {

	mRunning = true;

	if( ! createSurface() )
	{
		LOG(LogLevel::Fatal, "Renderer failed to create surface!");
		mRunning = false;
	}

	glViewport(0, 0, mSdlScreen->w, mSdlScreen->h);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, mSdlScreen->w, mSdlScreen->h, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void RenderController::quitSDL()
{
	SDL_Quit();
}


void RenderController::swapBuffers()
{
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}