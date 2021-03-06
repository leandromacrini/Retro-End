#include "RenderController.h"

#include "LogController.h"

#include "../View/Label.h"
#include "../View/Image.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

#ifdef USE_OPENGL_ES
#define glOrtho glOrthof
#endif


const string PopupMessageIconPath[] =
{
	"None",					//None
	"popup_message.png",	//Info
	"popup_warning.png",	//Warning
	"popup_error.png",		//Error
	"popup_controller_on.png",		//Controller_Added
	"popup_controller_off.png"		//Controller_Removed
};

void RenderController::pushClipRect(Eigen::Vector4i box)
{

	//glScissor starts at the bottom left of the window
	//so (0, 0, 1, 1) is the bottom left pixel
	//everything else uses y+ = down, so flip it to be consistent
	//rect.pos.y = Renderer::getScreenHeight() - rect.pos.y - rect.size.y;
	box[1] = RenderController::getScreenHeight() - box.y() - box[3];

	clipStack.push(box);
	glScissor(box[0], box[1], box[2], box[3]);
	glEnable(GL_SCISSOR_TEST);
}

void RenderController::popClipRect()
{
	if(clipStack.empty())
	{
		LOG(LogLevel::Warning, "Tried to popClipRect while the stack was empty!");
		return;
	}

	clipStack.pop();
	if(clipStack.empty())
	{
		glDisable(GL_SCISSOR_TEST);
	}else{
		Eigen::Vector4i top = clipStack.top();
		glScissor(top[0], top[1], top[2], top[3]);
	}
}

void RenderController::setColor4bArray(GLubyte* array, unsigned int color)
{
	array[0] = (color & 0xff000000) >> 24;
	array[1] = (color & 0x00ff0000) >> 16;
	array[2] = (color & 0x0000ff00) >> 8;
	array[3] = (color & 0x000000ff);
}

void RenderController::buildGLColorArray(GLubyte* ptr, unsigned int color, unsigned int vertCount)
{
	//convert color from ???? to RGBA?
	unsigned int colorRGBA;
	setColor4bArray((GLubyte *)&colorRGBA, color);
	//write color to unsigned int array
	GLuint * uiPtr = (GLuint *)ptr;
	for(unsigned int i = 0; i < vertCount; i++)
	{
		uiPtr[i] = colorRGBA;
	}
}


void RenderController::buildImageArray(float posX, float posY, GLfloat* points, GLfloat* texs, Eigen::Vector2f size, float px, float py)
{
	points[0]  = posX;				points[1]  = posY;
	points[2]  = posX;				points[3]  = posY + size.y();
	points[4]  = posX + size.x();	points[5]  = posY;

	points[6]  = posX + size.x();	points[7]  = posY;
	points[8]  = posX;				points[9]  = posY + size.y();
	points[10] = posX + size.x();	points[11] = posY + size.y();



	texs[0] = 0;		texs[1] = py;
	texs[2] = 0;		texs[3] = 0;
	texs[4] = px;		texs[5] = py;

	texs[6] = px;		texs[7] = py;
	texs[8] = 0;		texs[9] = 0;
	texs[10] = px;		texs[11] = 0;
}

void RenderController::drawImageArray(GLuint textureID, GLfloat* points, GLfloat* texs, GLubyte* colors, unsigned int numArrays)
{
	if(textureID != 0)
		glBindTexture(GL_TEXTURE_2D, textureID);
	else
		LOG(LogLevel::Error, "Tried to bind uninitialized texture!");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if(colors != NULL)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
	}

	glVertexPointer(2, GL_FLOAT, 0, points);
	glTexCoordPointer(2, GL_FLOAT, 0, texs);

	glDrawArrays(GL_TRIANGLES, 0, numArrays);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(colors != NULL)
		glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void RenderController::drawRect(int x, int y, int w, int h, unsigned int color)
{

#ifdef USE_OPENGL_ES
	GLshort points[12];
#else
	GLint points[12];
#endif

	points[0] = x; points [1] = y;
	points[2] = x; points[3] = y + h;
	points[4] = x + w; points[5] = y;

	points[6] = x + w; points[7] = y;
	points[8] = x; points[9] = y + h;
	points[10] = x + w; points[11] = y + h;

	GLubyte colors[6*4];
	buildGLColorArray(colors, color, 6);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

#ifdef USE_OPENGL_ES
	glVertexPointer(2, GL_SHORT, 0, points);
#else
	glVertexPointer(2, GL_INT, 0, points);
#endif
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableClientState(GL_BLEND);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_COLOR_ARRAY);
}


void RenderController::setMatrix(float* matrix)
{
	glLoadMatrixf(matrix);
}

void RenderController::setMatrix(const Eigen::Affine3f& matrix)
{
	setMatrix((float*)matrix.data());
}

bool RenderController::isRunning()
{
	return mRunning;
}

void RenderController::update()
{
	//if we are playing avoid any update
	if(GamingController::getInstance().IsPlaying()) return;

	//start show messages if not already showing
	if(mNeedShowPopupMessage && !mShowingPopupMessage) showPopupMessages();

	//compute the delta time with last update
	unsigned int curTime = SDL_GetTicks();
	unsigned int deltaTime = curTime - mLastTime;
	mLastTime = curTime;

	//cap deltaTime at 1000
	if(deltaTime > 1000 || deltaTime < 0) deltaTime = 1000;

	//check timeouts
	auto it = mTimeouts.begin();
	while(it != mTimeouts.end())
	{
		if((*it)->Delay > deltaTime)
		{
			(*it)->Delay -= deltaTime;
			it++;
		} else {
			(*it)->Callback();
			it = mTimeouts.erase(it);
		}
	}

	//update the main window
	mainWindow->update(deltaTime);
	if(mPopupView) mPopupView->update(deltaTime);

	//swap here so we can read the last screen state during updates (see ImageComponent::copyScreen())
	swapBuffers(); 

	//render the main window and all its child starting from Identity Matrix
	mainWindow->render(Eigen::Affine3f::Identity());
	if(mPopupView) mPopupView->render(Eigen::Affine3f::Identity());
}

View::BaseView* RenderController::getCurrentWindow()
{
	return mainWindow;
}

void RenderController::exit()
{
	mRunning = false;
}

int RenderController::getScreenWidth()
{
	return display_width;
}

int RenderController::getScreenHeight()
{
	return display_height;
}

void RenderController::setTimeout(unsigned int delay, function<void ()> callback)
{
	mTimeouts.push_back(new Timeout(delay, callback));
}

void RenderController::pushPopupMessage(string message, PopupMessageIcon icon)
{
	mMutex.lock();
	
	mPopupMessages.push(PopupMessage(message, icon));
	mNeedShowPopupMessage = true;
	
	mMutex.unlock();
}

void RenderController::showPopupMessages()
{
	//init popup view online the first time
	//is here because at constructor time we don't have the surface to get H and W
	if(mPopupView == NULL)
	{
		mPopupView = new View::BaseView();
		mPopupView->setSize((float)getScreenWidth(), (float)getScreenWidth());
	}

	if(mPopupMessages.empty())
	{
		mShowingPopupMessage = false;
		mNeedShowPopupMessage = false;
		return;
	}
	PopupMessage& popup = mPopupMessages.front();

	mShowingPopupMessage = true;

	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	//DRAW MESSAGE
	Image* container = new Image();
	container->setSize( W/2, H/6 );
	container->setPosition( W/4, H/24);
	container->setOpacity(0);
	container->setPath("data/images/message.png");

	float leftMargin = 0;

	if(popup.Icon != PopupMessageIcon::None)
	{
		leftMargin = H/6;

		Image* icon = new Image();
		icon->setPath("data/images/" + PopupMessageIconPath[popup.Icon]);
		icon->setSize(leftMargin, leftMargin);
		icon->setPosition( 0, 0);
		container->addChild(icon);
	}

	Label* text = new Label();
	text->setColor(0xFFFFFFFF);
	text->setSize( W/2 - leftMargin, 0 );
	text->setText(popup.Message);
	text->setPosition( leftMargin, (H/6 - text->getSize().y()) / 2 );
	text->WrapText = true;
	text->HorizontalTextAlign = TextAlign::Center;
	text->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_MEDIUM));

	container->addChild(text);

	mPopupView->addChild(container);

#pragma region PopupAnimation

	//ANIMATE MESSAGE
	Animation* a = new Animation();
	a->millisDuration = 500;
	a->newOpacity = 255;
	a->endCallback = [this, container] ()
	{
		Animation* b = new Animation();
		b->millisDuration = 2000;
		b->endCallback = [this, container] ()
		{
			Animation* c = new Animation();
			c->millisDuration = 500;
			c->newOpacity = 0;
			c->endCallback = [this, container] ()
			{
				mMutex.lock();

				//remove old popup
				mPopupMessages.pop();

				mPopupView->removeChild(container);

				//go to next message
				showPopupMessages();

				mMutex.unlock();
			};
			container->animate(c);
		};
		container->animate(b);
	};

	container->animate(a);

#pragma endregion
}


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

#ifndef _DEBUG
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
		SDL_WINDOW_OPENGL);
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

	LOG(LogLevel::Info, "RenderController started");
}

void RenderController::manta()
{
	//create the main window
	mainWindow = new View::MainWindow();
	mPopupView = NULL;

	mRunning = true;
}

void RenderController::stop()
{
	quitSDL();
	LOG(LogLevel::Info, "RenderController stoped");
}

void RenderController::initSDL() {

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

	swapBuffers();
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
