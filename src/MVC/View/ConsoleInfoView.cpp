#include "ConsoleInfoView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ConsoleInfoView::ConsoleInfoView(Device& device) : mDevice(device)
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
	mTitle->setText(mDevice.Name);
	mBack->addChild(mTitle);

	//description
	mDescription = new Label();
	mDescription->WrapText = true;
	mDescription->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_SMALL));
	mDescription->setSize(mBack->getSize().x()/2, mBack->getSize().y() - H/4 - mTitle->getSize().y());
	mDescription->setText(mDevice.Description);
	mDescription->setPosition(mBack->getSize().x()/2, mTitle->getSize().y());
	mBack->addChild(mDescription);

	mGameBack = new Image();

	//back cover image (if any or front or empty) 
	if( boost::filesystem::exists("data/consoles/" + device.Name + "/device.png") )
			mGameBack->setPath("data/consoles/" + device.Name + "/device.png");

	//resize on width or height
	if(mGameBack->getTextureSize().x() > mGameBack->getTextureSize().y())
	{
		float maxImageSide = mBack->getSize().x()/2;

		if(mGameBack->getSize().x() > maxImageSide) mGameBack->resizeByWidth(maxImageSide);
	}
	else
	{
		float maxImageSide = mBack->getSize().y() - mTitle->getSize().y() * 2;
		
		if(mGameBack->getSize().y() > maxImageSide)
			mGameBack->resizeByHeight(maxImageSide);
	}
	mGameBack->setPosition( (mBack->getSize().x()/2 - mGameBack->getSize().x()) / 2.0f,  (mBack->getSize().y() - mGameBack->getSize().y()) / 2.0f );
	mBack->addChild(mGameBack);

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

ConsoleInfoView::~ConsoleInfoView()
{

}

bool ConsoleInfoView::input(Model::Input input)
{
	if(input.Semantic == InputSemantic::BUTTON_X && input.Value != SDL_RELEASED )
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
			};
			mBack->animate(a);
		};

		mBack->animate(a);

		return true;
	}

	return true;
}
