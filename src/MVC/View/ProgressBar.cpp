#include "ProgressBar.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ProgressBar::ProgressBar() : BaseView(), 
	mValue(0)
{
	mBorderTexture = TextureResource::get(IMAGES_PATH + "icon_progress_border.png");
	mValueTexture = TextureResource::get(IMAGES_PATH + "icon_progress_value.png");

	setSize((float) mBorderTexture->getSize().x(), (float) mBorderTexture->getSize().y());
}

int ProgressBar::getValue() const
{
	return mValue;
}

void ProgressBar::setValue(int value)
{
	mValue = value;

	if(mValue>100) mValue = 100;
	if(mValue<0) mValue = 0;

	onValueChanged(mValue);
}

void ProgressBar::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	//with opacity *= parent opacity (recursively)
	if(getOpacity() > 0 && Visible)
	{
		GLfloat points[12], texsValue[12], texsBorder[12];
		GLubyte colors[6*4];
		RenderController::buildGLColorArray(colors, 0xFFFFFF00 | getAbsoluteOpacity(), 6);

		//value "size"
		Eigen::Vector2f valueSize( mSize.x() * mValue/100, mSize.y());

		//draw value
		RenderController::buildImageArray(getAbsolutePosition().x(), getAbsolutePosition().y(), points, texsValue, valueSize);
		RenderController::drawImageArray(mValueTexture->getTextureID(), points, texsValue, colors, 6);

		//draw border

		RenderController::buildImageArray(getAbsolutePosition().x(), getAbsolutePosition().y(), points, texsBorder, mSize);
		RenderController::drawImageArray(mBorderTexture->getTextureID(), points, texsBorder, colors, 6);
	}
}

bool ProgressBar::input(Input input)
{
	if(input.Semantic == InputSemantic::RIGHT && input.Value != SDL_RELEASED )
	{
		setValue(mValue+10);
		return true;
	}

	if(input.Semantic == InputSemantic::LEFT && input.Value != SDL_RELEASED )
	{
		setValue(mValue-10);
		return true;
	}

	return false;
}