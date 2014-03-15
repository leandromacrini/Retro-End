#include "InputConfigurationView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

InputConfigurationView::InputConfigurationView(InputConfig* config) : mConfig(config), mCurrentButton(InputSemantic::UNMAPPED), mElapsedTime(0), mLastTimeStamp(0)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	//shadow
	mSize = Eigen::Vector2f(W,H);
	BackgroundColor = 0x1d1d1d77;

	//container
	mBack = new BaseView();
	mBack->BackgroundColor = 0xEDEDEDFF;
	mBack->setSize(W * 5/6, H * 5/6);
	mBack->setPosition(W/6 / 2, H);

	//title
	mTitle = new Label();
	mTitle->setSize( mBack->getSize().x(), 0);
	mTitle->BackgroundColor = 0x2B729533;
	mTitle->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	mTitle->HorizontalTextAlign = TextAlign::Center;
	mTitle->setText("BUTTON CONFIGURATION");
	mBack->addChild(mTitle);

	//description
	mDescription = new Label();
	mDescription->setSize( mBack->getSize().x(), 0);
	mDescription->setPosition(0, mTitle->getSize().y());
	mDescription->WrapText = true;
	mDescription->setColor(0x000000FF);
	mDescription->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	mDescription->HorizontalTextAlign = TextAlign::Center;
	mDescription->setText("You will now configure the new controller\n\nPress the shown button or wait 5 second to jump to next one (for example if you don't have L2 or R3 buttons)\n\nMove and press some buttons to clear the state and press RETURN or Button A on a configurated joystick to start");
	mBack->addChild(mDescription);

	//button label
	mButton = new Label();
	mButton->setSize( mBack->getSize().x(), 0);
	mButton->setPosition(0, mBack->getSize().y()/2);
	mButton->WrapText = true;
	mButton->setColor(0xFF0055FF);
	mButton->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	mButton->HorizontalTextAlign = TextAlign::Center;
	mBack->addChild(mButton);

	//animation
	Animation* a = new Animation();
	a->moveOffset = Eigen::Vector3f(0, -(H * 5/6)-(H/6 / 2) - H/6 / 4, 0);
	a->millisDuration = 250;
	a->endCallback = [H, this] () 
	{
		Animation* a = new Animation();
		a->moveOffset = Eigen::Vector3f(0, H/6 / 4, 0);
		a->millisDuration = 100;
		mBack->animate(a);
	};
	mBack->animate(a);

	this->addChild(mBack);
}

void InputConfigurationView::configureNextButton()
{
	LOG(LogLevel::Info, "InputConfigurationView Step " + to_string(mCurrentButton));

	//start now?
	if(mCurrentButton == InputSemantic::UNMAPPED)
	{
		mCurrentButton = InputSemantic::UP;
	}
	else
	{
		mCurrentButton = InputSemantic(((int)mCurrentButton) + 1);
	}
	mElapsedTime = 0;

	switch (mCurrentButton)
	{
	case UP:
		mButton->setText("PRESS UP");
		break;
	case DOWN:
		mButton->setText("PRESS DOWN");
		break;
	case LEFT:
		mButton->setText("PRESS LEFT");
		break;
	case RIGHT:
		mButton->setText("PRESS RIGHT");
		break;
	case BUTTON_A:
		mButton->setText("PRESS BUTTON_A");
		break;
	case BUTTON_B:
		mButton->setText("PRESS BUTTON_B");
		break;
	case BUTTON_X:
		mButton->setText("PRESS BUTTON_X");
		break;
	case BUTTON_Y:
		mButton->setText("PRESS BUTTON_Y");
		break;
	case BUTTON_L:
		mButton->setText("PRESS BUTTON_L");
		break;
	case BUTTON_L2:
		mButton->setText("PRESS BUTTON_L2");
		break;
	case BUTTON_L3:
		mButton->setText("PRESS BUTTON_L3");
		break;
	case BUTTON_R:
		mButton->setText("PRESS BUTTON_R");
		break;
	case BUTTON_R2:
		mButton->setText("PRESS BUTTON_R2");
		break;
	case BUTTON_R3:
		mButton->setText("PRESS BUTTON_R3");
		break;
	case START:
		mButton->setText("PRESS START");
		break;
	case SELECT:
		mButton->setText("PRESS SELECT");
		break;
	case SYSTEM:
		//we have done
		{
			mConfig->save();

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
					RenderController::getInstance().pushPopupMessage("Controller configuration saved\nUsed for player "+to_string(mConfig->PlayerNumber), PopupMessageIcon::Controller_Added);
				};
				mBack->animate(a);
			};
			mBack->animate(a);
		}
		break;
	default:
		break;
	}
}

InputConfigurationView::~InputConfigurationView()
{

}

bool InputConfigurationView::input(Model::Input input)
{
	//check if user press RETURN or A on other controller
	if(mCurrentButton == InputSemantic::UNMAPPED)
	{
		if(( input.RawData.ValueID == SDLK_RETURN && input.Value == SDL_PRESSED ) || (input.Semantic == InputSemantic::BUTTON_A && input.PlayerNumber != mConfig->PlayerNumber))
		{
			configureNextButton();
		}
	}
	else
	{
		if(input.PlayerNumber == mConfig->PlayerNumber && input.Value == SDL_PRESSED && input.RawData.Timestamp - mLastTimeStamp > 250) //250 millis to avoid multi pressure and axis bug on some chinese controllers
		{
			LOG(LogLevel::Info, "Set " + to_string(input.RawData.ValueID) + " at " + to_string(input.RawData.Timestamp) + " last " + to_string(mLastTimeStamp));
			//store the value
			mConfig->setInputMap(mCurrentButton, InputRaw(input.RawData.Type, input.RawData.ValueID));

			configureNextButton();
			mLastTimeStamp = input.RawData.Timestamp;
		}
		
	}
	return true;
}

void InputConfigurationView::update(unsigned int deltaTime)
{
	BaseView::update(deltaTime);

	//if we have staarted check if for button timeout 
	if(mCurrentButton != InputSemantic::UNMAPPED)
	{
		if(mElapsedTime + deltaTime < BUTTON_TIMEOUT)
		{
			mDescription->setText("\nPress the button to use or wait (" + to_string((int)((BUTTON_TIMEOUT - mElapsedTime)/1000)) + ") seconds");
			mElapsedTime+=deltaTime;
		}
		else
		{
			configureNextButton();
		}
	}
}
