#include "ConsoleView.h"

#include "../Model/Device.h"
#include "../Model/Font.h"

#include "../Controller/RenderController.h"
#include "../Controller/AudioController.h"
#include "../Controller/InputController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ConsoleView::ConsoleView() : mAnimating(false), mContainer(new BaseView())
{
	//set opacity to 0 to fadeIn
	mOpacity = 0;

	//create the moving sound
	mMoveSound = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS25.wav"));
	mSelectSound  = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS42.wav"));

	//create graphics
	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	//info container
	mInfoContainer = new BaseView();
	mInfoContainer->setSize( W / 2, H / 3);
	mInfoContainer->setPosition(W / 2, H / 3);
	mInfoContainer->setBackgroundColor(0xFF0000FF);
	this->addChild(mInfoContainer);

	//selected console logo
	mLogo = new Image();
	mLogo->setPosition(0, 0);
	mLogo->setSize(W / 2, H / 3 / 3);
	mInfoContainer->addChild(mLogo);

	//selected console name
	mName = new Label();
	mName->setColor(0xFFFFFFFF);
	mName->setSize(W / 2, H / 3 / 3);
	mName->setPosition(0, H / 3 / 3);
	mName->HorizontalTextAlign = TextAlign::Center;
	mInfoContainer->addChild(mName);

	//selected console date and manufacturer
	mDateCustomer = new Label();
	mDateCustomer->setColor(0xFFFFFFFF);
	mDateCustomer->setSize(W / 2, H / 3 / 3);
	mDateCustomer->setPosition(0, H / 3 / 3 * 2);
	mDateCustomer->HorizontalTextAlign = TextAlign::Center;
	mInfoContainer->addChild(mDateCustomer);

	//selected console info icons
	float iconSide = W / 2 / 10;

	mMediaIcon = new Image();
	mMediaIcon->setPath("data/images/icon.png");
	mMediaIcon->setSize(iconSide,iconSide);
	mMediaIcon->setPosition(iconSide*3, H/3 - iconSide);
	mInfoContainer->addChild(mMediaIcon);

	mControllerIcon = new Image();
	mControllerIcon = new Image();
	mControllerIcon->setPath("data/images/icon.png");
	mControllerIcon->setSize(iconSide,iconSide);
	mControllerIcon->setPosition(iconSide*4, H/3 - iconSide);
	mInfoContainer->addChild(mControllerIcon);

	mPlayersIcon = new Image();
	mPlayersIcon = new Image();
	mPlayersIcon->setPath("data/images/icon.png");
	mPlayersIcon->setSize(iconSide,iconSide);
	mPlayersIcon->setPosition(iconSide*5, H/3 - iconSide);
	mInfoContainer->addChild(mPlayersIcon);

	//bottom legends
	mLeftLegend = new Sprite();
	mRightLegend = new Sprite();

	//draw available consoles
	vector<Device> devices = Device::getAllDevices();

	float itemSide = (float)RenderController::getInstance().getScreenWidth() / 2;

	mContainer->setSize(itemSide, itemSide * devices.size());
	this->addChild(mContainer);

	//load all devices
	for(unsigned int i = 0; i < devices.size(); i++)
	{
		Device device = devices[i];

		if(i == 4)
		{
			mLogo->setPath(device.imageLogo);
			mName->setText(device.name);
			mDateCustomer->setText("1981, " + device.manufacturer);
		}

		if( ! device.imageConsole.empty() )
		{
			Image* image = new Image();
			image->setSize(itemSide, itemSide);
			image->setPath(device.imageConsole);
			image->setPosition(0, itemSide*i);
			mContainer->addChild(image);
		}
	}

	//fade id
	Animation* a = new Animation();
	a->millisDuration = 2000;
	a->newOpacity = new unsigned char(255);
	this->animate(a);
}

ConsoleView::~ConsoleView()
{

}

bool ConsoleView::input(Model::InputConfig* config, Model::Input input)
{
	float itemSide = (float)RenderController::getInstance().getScreenWidth() / 3;

	//TODO input from settings
	if(input.id == SDLK_DOWN && input.value != 0 && ! mContainer->isAnimating() )
	{
		Animation* a = new Animation();
		a->millisDuration = 500;
		a->moveOffset = new Eigen::Vector3f(0, -itemSide, 0);

		mContainer->animate(a);
		mMoveSound->play();
		return true;
	}

	if(input.id == SDLK_UP && input.value != 0 && ! mContainer->isAnimating())
	{
		Animation* a = new Animation();
		a->millisDuration = 500;
		a->moveOffset = new Eigen::Vector3f(0, itemSide, 0);

		mContainer->animate(a);
		mMoveSound->play();
		return true;
	}

	return false;
}
