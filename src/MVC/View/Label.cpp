#include "Label.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Label::Label() : BaseView(), 
	mFont(NULL), mColor(0x000000FF), mAutoCalcExtent(true, true), mCentered(false)
{
}

Label::Label(const std::string& text, std::shared_ptr<Font> font, Eigen::Vector3f pos, Eigen::Vector2f size) : BaseView(), 
	mFont(NULL), mColor(0x000000FF), mAutoCalcExtent(true, true), mCentered(false)
{
	setText(text);
	setFont(font);
	setPosition(pos);
	setSize(size);
}

void Label::onSizeChanged()
{
	mAutoCalcExtent << (getSize().x() == 0), (getSize().y() == 0);
	calculateExtent();
}

void Label::setFont(std::shared_ptr<Font> font)
{
	mFont = font;

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

void Label::setCentered(bool center)
{
	mCentered = center;
}

std::shared_ptr<Font> Label::getFont() const
{
	if(mFont)
		return mFont;
	else
		return Font::get(Font::getDefaultPath(), FONT_SIZE_MEDIUM);
}

void Label::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	if(mFont && !mText.empty())
	{
		Eigen::Vector3f pos3f = getAbsolutePosition();
		Eigen::Vector2f pos2f(pos3f.x(), pos3f.y());

		if(mCentered)
		{
			Eigen::Vector2f textSize = mFont->sizeWrappedText(mText, getSize().x());
			Eigen::Vector2f pos((getSize().x() - textSize.x()) / 2, 0);
			mFont->drawWrappedText(mText, pos2f+pos, getSize().x(), (mColor >> 8 << 8) | getOpacity());
		}else{
			mFont->drawWrappedText(mText, pos2f, getSize().x(), mColor >> 8 << 8  | getOpacity());
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
