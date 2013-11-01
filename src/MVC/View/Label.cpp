#include "Label.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Label::Label() : BaseView(), 
	mFont(NULL), mColor(0x000000FF), mAutoCalcExtent(true, true)
{
	HorizontalTextAlign = TextAlign::Left;

	ShadowVisible = false;
	ShadowColor   = 0x000000AA;
	ShadowOffset  = Eigen::Vector2i(2,2);

	WrapText = false;
}

void Label::onSizeChanged()
{
	mAutoCalcExtent << (getSize().x() == 0), (getSize().y() == 0);
	calculateExtent();
}

void Label::setColor(unsigned int color)
{
	mColor = color;
	mOpacity = mColor & 0x000000FF;
}

void Label::setText(const std::string& text)
{
	mText = text;

	calculateExtent();
}

void Label::setFont(std::shared_ptr<Font> font)
{
	mFont = font;

	calculateExtent();
}

std::shared_ptr<Font> Label::getFont() const
{
	if(mFont)
		return mFont;
	else
		return Font::getDefaultFont();
}

void Label::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	if(getFont() && !mText.empty())
	{
		Eigen::Vector3f pos3f = getAbsolutePosition();
		Eigen::Vector2f pos2f(pos3f.x(), pos3f.y());

		//compute the text align offset
		float xTextOffset = 0;
		switch (HorizontalTextAlign)
		{
		case RetroEnd::View::Left:
			xTextOffset = getAbsolutePosition().x();
			break;
		case RetroEnd::View::Center:
			xTextOffset = pos2f.x() + ( ( mSize.x() - getFont()->sizeText(mText).x() ) / 2);
			break;
		case RetroEnd::View::Right:
			xTextOffset = pos2f.x() + mSize.x() - getFont()->sizeText(mText).x();
			break;
		default:
			break;
		}

		unsigned char opacity = getAbsoluteOpacity();

		if(opacity > 0)
		{
			//draw shadow if enabled
			if(ShadowVisible)
			{
				//TODO Shadow Global Opacity ?
				if(WrapText)
					getFont()->drawWrappedText(mText, Eigen::Vector2f(xTextOffset + ShadowOffset.x(), pos2f.y() + ShadowOffset.y()), mSize.x() , ShadowColor >> 8 << 8  | getAbsoluteOpacity());
				else
					getFont()->drawText(mText, Eigen::Vector2f(xTextOffset + ShadowOffset.x(), pos2f.y() + ShadowOffset.y()), ShadowColor >> 8 << 8  | getAbsoluteOpacity());
			}
			//draw text
			if(WrapText)
				getFont()->drawWrappedText(mText, Eigen::Vector2f(xTextOffset, pos2f.y()), mSize.x() , mColor >> 8 << 8  | getAbsoluteOpacity());
			else
				getFont()->drawText(mText, Eigen::Vector2f(xTextOffset, pos2f.y()), mColor >> 8 << 8  | getAbsoluteOpacity());
		}
	}
}

void Label::calculateExtent()
{
	std::shared_ptr<Font> font = getFont();

	if(mAutoCalcExtent.x())
	{
		mSize = font->sizeText(mText);
	}else{
		if(mAutoCalcExtent.y())
		{
			mSize[1] = font->sizeWrappedText(mText, getSize().x()).y();
		}
	}
}
