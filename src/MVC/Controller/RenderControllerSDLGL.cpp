#include "RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

#ifdef USE_OPENGL_ES
	#define glOrtho glOrthof
#endif

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

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef USE_OPENGL_ES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
#endif   
	
#ifdef _DEBUG
	display_width = 1280;
	display_height = 720;

    sdlWindow = SDL_CreateWindow("Manta Entertainment System", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		1280, 720, 
		SDL_WINDOW_OPENGL);
#else
	SDL_DisplayMode dispMode;
	SDL_GetDesktopDisplayMode(0, &dispMode);
	display_width  = dispMode.w;
	display_height = dispMode.h;
    sdlWindow = SDL_CreateWindow("Manta Entertainment System", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		display_width, display_height, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
#endif

	if(sdlWindow == NULL)
	{
		LOG(LogLevel::Fatal, "Error creating SDL window!");
		return false;
	}

	sdlContext = SDL_GL_CreateContext(sdlWindow);

	LOG(LogLevel::Info, "Created context successfully.");

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

	glViewport(0, 0, display_width, display_height);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, display_width, display_height, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void RenderController::quitSDL()
{
	SDL_GL_DeleteContext(sdlContext);
	sdlContext = NULL;

	SDL_DestroyWindow(sdlWindow);
	sdlWindow = NULL;

	SDL_Quit();
}


void RenderController::swapBuffers()
{
	SDL_GL_SwapWindow(sdlWindow);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}