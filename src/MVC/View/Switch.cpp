#include "Switch.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Switch::Switch(bool checked) : BaseView(), 
	mChecked(checked)
{
	Focusable = true;
	mCheckedTexture = TextureResource::get(IMAGES_PATH + "icon_checked.png");
	mUncheckedTexture = TextureResource::get(IMAGES_PATH + "icon_unchecked.png");

	setSize((float) mCheckedTexture->getSize().x(), (float) mCheckedTexture->getSize().y());
}

void Switch::setChecked(bool checked)
{
	mChecked = checked;
	onCheckedChanged(mChecked);
}

bool Switch::getChecked() const
{
	return mChecked;
}

void Switch::toggle()
{
	mChecked = !mChecked;
	onCheckedChanged(mChecked);
}

void Switch::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	//with opacity *= parent opacity (recursively)
	if(getOpacity() > 0 && Visible)
	{
		GLfloat points[12], texs[12];
		GLubyte colors[6*4];

		//draw image
		RenderController::buildGLColorArray(colors, 0xFFFFFF00 | getAbsoluteOpacity(), 6);
		RenderController::buildImageArray(getAbsolutePosition().x(), getAbsolutePosition().y(), points, texs, mSize);
		RenderController::drawImageArray(mChecked? mCheckedTexture->getTextureID() : mUncheckedTexture->getTextureID(), points, texs, colors, 6);
	}
}

bool Switch::input(Input input)
{
	if(Focused && input.Semantic == InputSemantic::BUTTON_A && input.Value != SDL_RELEASED )
	{
		toggle();
	}

	return false;
}