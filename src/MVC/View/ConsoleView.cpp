#include "ConsoleView.h"

#include "../Model/Font.h"

#include "../Controller/RenderController.h"
#include "../Controller/AudioController.h"
#include "../Controller/InputController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ConsoleView::ConsoleView() : mAnimating(false), mContainer(new BaseView()), mCurrentIndex(0)
{
	//set opacity to 0 to fadeIn
	mOpacity = 0;

	//create sounds
	mMoveSound = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS25.wav"));
	mEndSound = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS28.wav"));
	mSelectSound  = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS42.wav"));

	//create graphics
	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	float containerW = W / 2;
	float containerH = H / 3 * 2;

	std::shared_ptr<Font> font =  Font::get("data/fonts/pixeljosh6.ttf", FONT_SIZE_SMALL);

	//info container
	mInfoContainer = new BaseView();
	mInfoContainer->setSize( containerW, containerH);
	mInfoContainer->setPosition(containerW, H / 6);
	this->addChild(mInfoContainer);

	//selected console logo
	mLogo = new Image();
	mLogo->setPosition(0, 0);
	mLogo->setSize(containerW, H / 3);
	mInfoContainer->addChild(mLogo);

	//selected console name
	mName = new Label();
	mName->setColor(0xFFFFFFFF);
	mName->setSize((float)containerW, (float)FONT_SIZE_LARGE);
	mName->setPosition(0, H / 3);
	mName->setFont(font);
	mName->HorizontalTextAlign = TextAlign::Center;
	mName->ShadowVisible = true;
	mInfoContainer->addChild(mName);

	//selected console date and manufacturer
	mDateManufacturer = new Label();
	mDateManufacturer->setColor(0xFFFFFFFF);
	mDateManufacturer->setSize((float)containerW,  (float)FONT_SIZE_LARGE);
	mDateManufacturer->setPosition(0, H / 3 + FONT_SIZE_MEDIUM);
	mDateManufacturer->setFont(font);
	mDateManufacturer->HorizontalTextAlign = TextAlign::Center;
	mDateManufacturer->ShadowVisible = true;
	mInfoContainer->addChild(mDateManufacturer);

	//selected console info icons
	float iconSide = containerW / 9;

	mMediaIcon = new Image();
	mMediaIcon->setPath("data/images/icon.png");
	mMediaIcon->setSize(iconSide,iconSide);
	mMediaIcon->setPosition(iconSide*3, containerH - iconSide);
	mInfoContainer->addChild(mMediaIcon);

	mControllerIcon = new Image();
	mControllerIcon = new Image();
	mControllerIcon->setPath("data/images/icon.png");
	mControllerIcon->setSize(iconSide,iconSide);
	mControllerIcon->setPosition(iconSide*4, containerH - iconSide);
	mInfoContainer->addChild(mControllerIcon);

	mPlayersIcon = new Image();
	mPlayersIcon = new Image();
	mPlayersIcon->setPath("data/images/icon.png");
	mPlayersIcon->setSize(iconSide,iconSide);
	mPlayersIcon->setPosition(iconSide*5, containerH - iconSide);
	mInfoContainer->addChild(mPlayersIcon);

	//draw available consoles
	mDevices = Device::getAllDevices();

	float itemSide = W / 2;

	mContainer->setSize(itemSide, (H-itemSide)/2 + H * mDevices.size());
	this->addChild(mContainer);

	//load all devices
	for(unsigned int i = 0; i < mDevices.size(); i++)
	{
		Device device = mDevices[i];

		if( ! device.imageConsole.empty() )
		{
			Image* image = new Image();
			image->setSize(itemSide, itemSide);
			image->setPath(device.imageConsole);
			image->setPosition(0, (H-itemSide)/2 + H*i);
			mContainer->addChild(image);
		}
	}

	//draw bottom legends
	mLeftLegend = new Sprite();
	mLeftLegend->FrameHeight = 180;
	mLeftLegend->setPosition(0, H - H/6);
	mLeftLegend->setSize(W/2, H/6);
	mLeftLegend->setPath("data/images/left-legend.png");
	mLeftLegend->setLoop(true);
	mLeftLegend->setFrameDuration(500);
	this->addChild(mLeftLegend);

	mRightLegend = new Sprite();
	mRightLegend->FrameHeight = 180;
	mRightLegend->setPosition(W/2, H - H/6);
	mRightLegend->setSize(W/2, H/6);
	mRightLegend->setPath("data/images/right-legend.png");
	mRightLegend->setLoop(true);
	mRightLegend->setFrameDuration(500);
	this->addChild(mRightLegend);

	//mLeftLegend->start();
	//mRightLegend->start();

	//fade id
	Animation* a = new Animation();
	a->millisDuration = 2000;
	a->newOpacity = new unsigned char(255);
	this->animate(a);

	updateCurrentConsoleData();
}

ConsoleView::~ConsoleView()
{

}

void ConsoleView::updateCurrentConsoleData()
{
	mLogo->setPath(mDevices[mCurrentIndex].imageLogo);
	mName->setText(mDevices[mCurrentIndex].name);
	mDateManufacturer->setText(mDevices[mCurrentIndex].releaseDate + ", " + mDevices[mCurrentIndex].manufacturer);
}

void ConsoleView::move(int direction)
{
	if( mContainer->isAnimating()) return;

	if( (direction == -1 && mCurrentIndex == 0) || (direction == 1 && mCurrentIndex == mDevices.size()-1))
	{
		mEndSound->play();
		return;
	}

	mCurrentIndex+= direction;

	float H = (float)RenderController::getInstance().getScreenHeight();

	Animation* a = new Animation();
	a->millisDuration = 500;
	a->moveOffset = new Eigen::Vector3f(0, -direction*H, 0);
	mContainer->animate(a);

	Animation* b = new Animation();
	b->millisDuration = 400;
	b->newOpacity = new unsigned char(0);
	b->endCallback = [&] () {
		updateCurrentConsoleData();

		Animation* c = new Animation();
		c->millisDuration = 100;
		c->newOpacity = new unsigned char(255);
		mInfoContainer->animate(c);
	};

	mInfoContainer->animate(b);
	
	
	mMoveSound->play();
}

bool ConsoleView::input(Model::InputConfig* config, Model::Input input)
{
	//TODO input from settings
	if(input.id == SDLK_DOWN && input.value != 0 )
	{
		move(1);
		return true;
	}

	if(input.id == SDLK_UP && input.value != 0 )
	{
		move(-1);
		return true;
	}

	if(input.id == SDLK_RETURN && input.value != 0 )
	{
		mSelectSound->play();
		onOpenGamesList(mDevices.at(mCurrentIndex));
		return true;
	}

	return false;
}
