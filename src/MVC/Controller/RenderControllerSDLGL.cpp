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

	//ATM it is best to just leave the window icon alone on windows.
	//When compiled as a Windows application, ES at least has an icon in the taskbar
	//The method below looks pretty shite as alpha isn't taken into account...
#ifndef WIN32
	//try loading PNG from memory
	size_t width = 0;
	size_t height = 0;
	std::vector<unsigned char> rawData = ImageIO::loadFromMemoryRGBA32(ES_logo_32_png_data, ES_logo_32_png_size, width, height);
	if (!rawData.empty()) {
		//SDL interprets each pixel as a 32-bit number, so our masks must depend on the endianness (byte order) of the machine
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		Uint32 rmask = 0xff000000; Uint32 gmask = 0x0000ff00; Uint32 bmask = 0x00ff0000; Uint32 amask = 0x000000ff;
#else
		Uint32 rmask = 0x000000ff; Uint32 gmask = 0x00ff0000; Uint32 bmask = 0x0000ff00; Uint32 amask = 0xff000000;
#endif
		//try creating SDL surface from logo data
		SDL_Surface * logoSurface = SDL_CreateRGBSurfaceFrom((void *)rawData.data(), width, height, 32, width*4, rmask, gmask, bmask, amask);
		if (logoSurface != nullptr) {
			//change window icon. this sucks atm, but there's nothing better we can do. SDL 1.3 or 2.0 should sort this out...
			SDL_WM_SetIcon(logoSurface, nullptr);
		}
	}
#endif

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef USE_OPENGL_ES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
#endif
	
#ifndef _FULLSCREEN
	mSdlWindow = SDL_CreateWindow("Retro-End",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280,
		720,
		SDL_WINDOW_OPENGL);
#else
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); //vsync
	mSdlWindow = SDL_CreateWindow("Retro-End",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		0, 0,
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
	mSdlScreen = SDL_GetWindowSurface(mSdlWindow);

	if(mSdlScreen == NULL)
	{
		LOG(LogLevel::Fatal, "Error creating SDL video surface!");
		return false;
	}

	LOG(LogLevel::Info, "Created surface successfully.");

	sdlContext = SDL_GL_CreateContext(mSdlWindow);

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
	SDL_GL_SwapWindow(mSdlWindow);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}