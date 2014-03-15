#include "TextInputDialog.h"

#include "../Controller/LogController.h"
#include "../Controller/InputController.h"
#include "../Controller/RenderController.h"
#include "boost/bind.hpp"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

TextInputDialog::TextInputDialog(string title, string value, function<void (string date)> callback, bool obfuscated) : BaseView(),
	mCallback(callback)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	RenderController::getInstance().getCurrentWindow()->addChild(this);
	Focused = true;

	mTextInputCallback = [this](string text)
	{
		mTextField->setText(mTextField->getText() + text);
	};

	mSize = Eigen::Vector2f(W,H);
	BackgroundColor = 0x1d1d1d99;

	mBack = new BaseView();
	mBack->BackgroundColor = 0xEDEDEDFF;
	mBack->setSize(W * 5/10, H * 6/10);
	mBack->setPosition(W* 2.5f/10, H);
	addChild(mBack);
	
	mTitle = new Label();
	mTitle->setSize(mBack->getSize().x(), 0);
	mTitle->BackgroundColor = 0x2B729533;
	mTitle->setColor(0xFFFFFFFF);
	mTitle->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	mTitle->HorizontalTextAlign = TextAlign::Center;
	mTitle->setText(title);
	mBack->addChild(mTitle);

	mTextField = new Label(obfuscated);
	mTextField->setSize(mBack->getSize().x() *2/3, (float)FONT_SIZE_LARGE);
	mTextField->setPosition(mBack->getSize().x()*1/6,mBack->getSize().y()/2);
	mTextField->BackgroundColor = 0x1d1d1d99;
	mTextField->setColor(0x000000FF);
	mTextField->HorizontalTextAlign = TextAlign::Center;
	mTextField->setText(value);
	mBack->addChild(mTextField);
	
	Animation* a = new Animation();
	a->moveOffset = Eigen::Vector3f(0, -(H *8/10) -50, 0);
	a->millisDuration = 250;
	a->endCallback = [H, this] () 
	{
		Animation* a = new Animation();
		a->moveOffset = Eigen::Vector3f(0, 50, 0);
		a->millisDuration = 100;
		a->endCallback = [this]()
		{
			//start taking text input here to avoid already pressed keys
			InputController::getInstance().onTextInputReceived += mTextInputCallback;
		};
		mBack->animate(a);
	};
	mBack->animate(a);
}

bool TextInputDialog::input(Input input)
{
	//backspace
	if(input.RawData.ValueID == SDLK_BACKSPACE && input.RawData.Type == InputRawType::TYPE_KEY && input.Value != SDL_RELEASED)
	{
		string text = mTextField->getText();
		if(text.size()) text.pop_back();
		mTextField->setText(text);
		return true;
	}

	//this dialog close on return and on JOYSTICK button A
	if( ((input.Semantic == InputSemantic::BUTTON_A && input.RawData.Type == InputRawType::TYPE_BUTTON)
		||
		(input.RawData.ValueID == SDLK_RETURN && input.RawData.Type == InputRawType::TYPE_KEY))
		&& input.Value != SDL_RELEASED )
	{
		if(mBack->isAnimating()) return false;

		float H = (float) RenderController::getInstance().getScreenHeight();

		//EXIT ANIMATION
		Animation* a = new Animation();
		a->moveOffset = Eigen::Vector3f(0, -H/6 / 4, 0);
		a->millisDuration = 100;
		a->endCallback = [this] () 
		{
			Animation* a = new Animation();
			a->moveOffset = Eigen::Vector3f(0, mBack->getSize().y(), 0);
			a->millisDuration = 250;
			a->endCallback = [this] () 
			{
				this->mParent->removeChild(this);
				if(mCallback) mCallback(mTextField->getText());
			};
			mBack->animate(a);
		};

		mBack->animate(a);

		//clean my event observer
		InputController::getInstance().onTextInputReceived -= mTextInputCallback;

		return true;
	}

	//burn any other input
	return true;
}