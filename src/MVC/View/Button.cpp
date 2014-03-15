#include "Button.h"

#include "../Controller/LogController.h"
#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Button::Button(string title) : BaseView(), 
	mTitle(title),
	mPressed(false)
{
	mTexture = TextureResource::get(IMAGES_PATH + "icon_button.png");
	mPressedTexture = TextureResource::get(IMAGES_PATH + "icon_button_pressed.png");

	setSize((float) mTexture->getSize().x(), (float) mTexture->getSize().y());
}

void Button::press()
{
	mPressed = true;
	RenderController::getInstance().setTimeout(50, [this]()
	{
		mPressed = false;
		onButtonPressed(true);
	});
}

void Button::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	//with opacity *= parent opacity (recursively)
	if(getOpacity() > 0 && Visible)
	{
		GLfloat points[12], texs[12];
		GLubyte colors[6*4];

		Eigen::Vector3f absPos = getAbsolutePosition();

		//draw image
		RenderController::buildGLColorArray(colors, 0xFFFFFF00 | getAbsoluteOpacity(), 6);
		RenderController::buildImageArray(absPos.x(), absPos.y(), points, texs, mSize);
		RenderController::drawImageArray(mPressed? mPressedTexture->getTextureID() : mTexture->getTextureID(), points, texs, colors, 6);

		//draw title
		if( ! mTitle.empty() )
		{
			Eigen::Vector2f tSize = Font::getDefaultFont()->sizeText(mTitle);

			Font::getDefaultFont()->drawText(mTitle,
				Eigen::Vector2f(
					(int)(absPos.x() + ( ( mSize.x() - tSize.x() ) / 2)),
					(int)(absPos.y() + ( ( mSize.y() - tSize.y() ) / 2))
				),
				0x000000FF >> 8 << 8  | getAbsoluteOpacity());
		}
	}
}

bool Button::input(Input input)
{
	if(input.Semantic == BUTTON_A && input.Value == SDL_PRESSED )
	{
		mPressed=true;
	}

	if(input.Semantic == BUTTON_A && input.Value == SDL_RELEASED )
	{
		mPressed=false;
		onButtonPressed(true);
	}

	return false;
}