#include "TextComponent.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

TextComponent::TextComponent() : BaseView(), 
	mFont(NULL), mColor(0x000000FF), mAutoCalcExtent(true, true), mCentered(false)
{
}

TextComponent::TextComponent(const std::string& text, std::shared_ptr<Font> font, Eigen::Vector3f pos, Eigen::Vector2f size) : BaseView(), 
	mFont(NULL), mColor(0x000000FF), mAutoCalcExtent(true, true), mCentered(false)
{
	setText(text);
	setFont(font);
	setPosition(pos);
	setSize(size);
}

void TextComponent::onSizeChanged()
{
	mAutoCalcExtent << (getSize().x() == 0), (getSize().y() == 0);
	calculateExtent();
}

void TextComponent::setFont(std::shared_ptr<Font> font)
{
	mFont = font;

	calculateExtent();
}

void TextComponent::setColor(unsigned int color)
{
	mColor = color;
	mOpacity = mColor & 0x000000FF;
}

void TextComponent::setText(const std::string& text)
{
	mText = text;

	calculateExtent();
}

void TextComponent::setCentered(bool center)
{
	mCentered = center;
}

std::shared_ptr<Font> TextComponent::getFont() const
{
	if(mFont)
		return mFont;
	else
		return Font::get(Font::getDefaultPath(), FONT_SIZE_MEDIUM);
}

void TextComponent::draw()
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

void TextComponent::calculateExtent()
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
