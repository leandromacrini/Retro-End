#include "ConsoleView.h"

#include "../Model/Font.h"

#include "../Controller/RenderController.h"
#include "../Controller/AudioController.h"
#include "../Controller/InputController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ConsoleView::ConsoleView() : mAnimating(false), mContainer(new BaseView()), mCurrentIndex(0), BaseView()
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
	float containerH = H / 2;

	std::shared_ptr<Font> font =  Font::get("data/fonts/eurof35.ttf", FONT_SIZE_MEDIUM);

	//info container
	mInfoContainer = new BaseView();
	mInfoContainer->setSize( containerW, containerH );
	mInfoContainer->setPosition(containerW, (H - containerH) / 2);
	this->addChild(mInfoContainer);

	//selected console logo
	mLogo = new Image( );
	mLogo->setPosition( 0, 0 );
	mLogo->setSize( containerW, H / 6 );
	mInfoContainer->addChild( mLogo );

	//selected console date and manufacturer
	mDateManufacturer = new Label();
	mDateManufacturer->setColor( 0x000000FF );
	mDateManufacturer->setSize( containerW, 0 );
	mDateManufacturer->setFont( font );
	mDateManufacturer->setPosition( 0, ( containerH - mDateManufacturer->getFont()->getSize()) / 2 );
	mDateManufacturer->HorizontalTextAlign = TextAlign::Center;
	mInfoContainer->addChild( mDateManufacturer );

	//selected console info icons
	float iconSide = W/10;

	mMediaIcon2 = new Image();
	mMediaIcon2->setSize(iconSide,iconSide);
	mMediaIcon2->setPosition((containerW - iconSide*4)/2 , containerH - iconSide);
	mInfoContainer->addChild(mMediaIcon2);

	mMediaIcon1 = new Image();
	mMediaIcon1->setSize(iconSide,iconSide);
	mMediaIcon1->setPosition((containerW - iconSide*4)/2 + iconSide, containerH - iconSide);
	mInfoContainer->addChild(mMediaIcon1);

	mControllerIcon1 = new Image();
	mControllerIcon1->setSize(iconSide,iconSide);
	mControllerIcon1->setPosition((containerW - iconSide*4)/2 + iconSide*2, containerH - iconSide);
	mInfoContainer->addChild(mControllerIcon1);

	mControllerIcon2 = new Image();
	mControllerIcon2->setSize(iconSide,iconSide);
	mControllerIcon2->setPosition((containerW - iconSide*4)/2 + iconSide*3, containerH - iconSide);
	mInfoContainer->addChild(mControllerIcon2);

	//draw available consoles
	mDevices = Device::getAllDevices();

	float itemSide = W / 2;

	mContainer->setSize(itemSide, (H-itemSide)/2 + H * mDevices.size());
	this->addChild(mContainer);

	//load all devices
	for(unsigned int i = 0; i < mDevices.size(); i++)
	{
		Device device = mDevices[i];

		Image* image = new Image();
		image->setSize(itemSide, itemSide);
		if( boost::filesystem::exists("data/consoles/" + device.Name + "/device.png") )
			image->setPath("data/consoles/" + device.Name + "/device.png");
		else
			image->setPath("data/images/blank_device.png");
		image->setPosition(0, (H-itemSide)/2 + H*i);
		mContainer->addChild(image);
	}

	//fade id
	Animation* a = new Animation();
	a->millisDuration = 2000;
	a->newOpacity = new unsigned char(255);
	this->animate(a);

	Sprite* help = new Sprite();
	help->setPosition(W/2, H* 19/20);
	help->setSize(W/2, H/20);
	help->FrameHeight = 54;
	help->setPath("data/images/left-legend.png");
	help->setLoop(true);
	help->setFrameDuration( 1000 );
	help->start();
	this->addChild(help);

	//updateCurrentConsoleData();
}

ConsoleView::~ConsoleView()
{

}

void ConsoleView::updateCurrentConsoleData()
{
	LOG(LogLevel::Debug, "ConsoleView::updateCurrentConsoleData");

	if( boost::filesystem::exists("data/consoles/" + mDevices[mCurrentIndex].Name +"/logo.png") )
		mLogo->setPath("data/consoles/" + mDevices[mCurrentIndex].Name +"/logo.png");
	else
		mLogo->setPath("data/images/blank.png");

	if( boost::filesystem::exists("data/consoles/" + mDevices[mCurrentIndex].Name +"/mediatype1.png") )
		mMediaIcon1->setPath("data/consoles/" + mDevices[mCurrentIndex].Name +"/mediatype1.png");
	else
		mMediaIcon1->setPath("data/images/blank.png");

	if( boost::filesystem::exists("data/consoles/" + mDevices[mCurrentIndex].Name +"/mediatype2.png") )
		mMediaIcon2->setPath("data/consoles/" + mDevices[mCurrentIndex].Name +"/mediatype2.png");
	else
		mMediaIcon2->setPath("data/images/blank.png");

	if( boost::filesystem::exists("data/consoles/" + mDevices[mCurrentIndex].Name +"/controller1.png") )
		mControllerIcon1->setPath("data/consoles/" + mDevices[mCurrentIndex].Name +"/controller1.png");
	else
		mControllerIcon1->setPath("data/images/blank.png");

	if( boost::filesystem::exists("data/consoles/" + mDevices[mCurrentIndex].Name +"/controller2.png") )
		mControllerIcon2->setPath("data/consoles/" + mDevices[mCurrentIndex].Name +"/controller2.png");
	else
		mControllerIcon2->setPath("data/images/blank.png");

	mDateManufacturer->setText(mDevices[mCurrentIndex].ReleaseDate + ", " + mDevices[mCurrentIndex].Manufacturer);
}

void ConsoleView::move(int direction)
{
	LOG(LogLevel::Debug, "ConsoleView::move -> " + to_string(direction));
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
	LOG(LogLevel::Debug, "ConsoleView::input -> " + to_string(input.id));

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
		LOG(LogLevel::Debug, "ConsoleView::onOpenGamesList -> " + to_string(mCurrentIndex));
		mSelectSound->play();
		onOpenGamesList(mDevices.at(mCurrentIndex));
		return true;
	}

	return false;
}
