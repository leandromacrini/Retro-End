#include "ConsoleView.h"

#include "../Model/Device.h"
#include "../Controller/RenderController.h"
#include "../Controller/AudioController.h"
#include "Image.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;


ConsoleView::ConsoleView() : mAnimating(false), mContainer(new BaseView())
{
	//set opacity to 0 to fadeIn
	mOpacity = 0;

	//create the moving sound
	mMoveSound = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS25.wav"));

	vector<Device> devices = Device::getAllDevices();

	float itemSide = (float)RenderController::getInstance().getScreenWidth() / 3;

	mContainer->setSize(itemSide, itemSide * devices.size());
	this->addChild(mContainer);

	//load all devices
	for(unsigned int i = 0; i < devices.size(); i++)
	{
		Device device = devices[i];

		if( ! device.imageConsole.empty() )
		{
			Image* image = new Image();
			image->setSize(itemSide*1.5f, itemSide);
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
