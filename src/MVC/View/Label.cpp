#include "Label.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Label::Label(bool obfuscated) : BaseView(),
	mObfuscated(obfuscated),
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
}

void Label::setText(const string& text)
{
	mText = text;

	calculateExtent();
}

string Label::getText()
{
	return mText;
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

bool Label::input(Input input)
{

	if(Focused && input.Semantic == BUTTON_A && input.Value == SDL_PRESSED )
	{
		onLabelPressed(1);
		return true;
	}

	return BaseView::input(input);
}

void Label::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	if(getFont() && !mText.empty())
	{
		Eigen::Vector3f absPos = getAbsolutePosition();

		string text = mText;

		if(mObfuscated) text = string(mText.length(), '*');

		//compute the text align offset
		int xTextOffset = 0; // this MUST be an INTEGER
	
		switch (HorizontalTextAlign)
		{
		case RetroEnd::View::Left:
			xTextOffset = (int)absPos.x();
			break;
		case RetroEnd::View::Center:
			xTextOffset = (int)(absPos.x() + ( ( mSize.x() - getFont()->sizeText(text).x() ) / 2));
			break;
		case RetroEnd::View::Right:
			xTextOffset = (int)(absPos.x() + mSize.x() - getFont()->sizeText(text).x());
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
					getFont()->drawWrappedText(text, Eigen::Vector2f(absPos.x() + ShadowOffset.x(), absPos.y() + ShadowOffset.y()), mSize.x() , ShadowColor >> 8 << 8  | getAbsoluteOpacity(), HorizontalTextAlign);
				else
					getFont()->drawText(text, Eigen::Vector2f(xTextOffset + ShadowOffset.x(), absPos.y() + ShadowOffset.y()), ShadowColor >> 8 << 8  | getAbsoluteOpacity());
			}
			//draw text
			if(WrapText)
				getFont()->drawWrappedText(text, Eigen::Vector2f(absPos.x(), absPos.y()), mSize.x() , mColor >> 8 << 8  | getAbsoluteOpacity(), HorizontalTextAlign);
			else
				getFont()->drawText(text, Eigen::Vector2f(xTextOffset, absPos.y()), mColor >> 8 << 8  | getAbsoluteOpacity());
		}
	}
}

void Label::calculateExtent()
{
	std::shared_ptr<Font> font = getFont();

	string text = mText;

	if(mObfuscated) text = string(mText.length(), '*');

	if(mAutoCalcExtent.x())
	{
		mSize = font->sizeText(text);
	}else{
		if(mAutoCalcExtent.y())
		{
			mSize[1] = font->sizeWrappedText(text, getSize().x()).y();
		}
	}
}
