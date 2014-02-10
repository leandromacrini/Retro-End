#include "RenderController.h"

#include "LogController.h"

#include "../View/Label.h"
#include "../View/Image.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

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
	//compute the delta time with last update
	int curTime = SDL_GetTicks();
	int deltaTime = curTime - mLastTime;
	mLastTime = curTime;

	//cap deltaTime at 1000
	if(deltaTime > 1000 || deltaTime < 0) deltaTime = 1000;

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


void RenderController::pushPopupMessage(string message, PopupMessageIcon icon)
{
	//init popup view
	if(mPopupView == NULL)
	{
		mPopupView = new View::BaseView();
		mPopupView->setSize((float)getScreenWidth(), (float)getScreenWidth());
		mShowingPopupMessage = false;
	}

	PopupMessage* popup = new PopupMessage(message, icon);

	mPopupMessages.push(popup);

	//start show messages if not already showing
	if( ! mShowingPopupMessage ) showPopupMessages();
}

void RenderController::showPopupMessages()
{
	if(mPopupMessages.empty())
	{
		mShowingPopupMessage = false;
		return;
	}

	PopupMessage* popup = mPopupMessages.front();

	mShowingPopupMessage = true;

	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	//DRAW MESSAGE
	BaseView* container = new BaseView();
	container->setSize( W, H/6 );
	container->setPosition( 0, (H - H/6) / 2);
	container->setOpacity(0);
	container->setBackgroundColor(0x2B7295FF);

	float leftMargin = 0;

	if(popup->Icon != PopupMessageIcon::None)
	{
		Image* icon = new Image();
		icon->setPath("data/images/" + PopupMessageIconPath[popup->Icon]);
		icon->setSize(H/6, H/6);
		icon->setPosition( 0, 0);
		container->addChild(icon);

		leftMargin = H/6;
	}

	Label* text = new Label();
	text->setColor(0xFFFFFFFF);
	text->setSize( W - leftMargin, 0 );
	text->setText(popup->Message);
	text->setPosition( leftMargin, (H/6 - text->getSize().y()) / 2 );
	text->WrapText = true;
	text->HorizontalTextAlign = TextAlign::Left;

	container->addChild(text);

	mPopupView->addChild(container);

#pragma region PopupAnimation

	//ANIMATE MESSAGE
	Animation* a = new Animation();
	a->millisDuration = 500;
	a->newOpacity = new unsigned char(255);
	a->endCallback = [this, container] ()
	{
		Animation* b = new Animation();
		b->millisDuration = 1500;
		b->endCallback = [this, container] ()
		{
			Animation* c = new Animation();
			c->millisDuration = 500;
			c->newOpacity = new unsigned char(0);
			c->endCallback = [this, container] ()
			{
				//remove old popup
				mPopupMessages.pop();

				mPopupView->removeChild(container);

				//go to next message
				showPopupMessages();
			};
			container->animate(c);
		};
		container->animate(b);
	};

	container->animate(a);

#pragma endregion
}