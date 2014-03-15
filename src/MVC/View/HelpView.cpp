#include "HelpView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

HelpView::HelpView(HelpScreen screen)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	//shadow
	mSize = Eigen::Vector2f(W,H);
	BackgroundColor = 0x1d1d1d77;
	
	//back cover image (if any or front or empty)
	string image;
	switch (screen)
	{
	case RetroEnd::View::CONSOLE_HELP:
		image = "data/images/HelpConsole.png";
		break;
	case RetroEnd::View::GAME_HELP:
		image = "data/images/HelpGame.png";
		break;
	default:
		break;
	}

	//container
	mBack = new Image();
	mBack->BackgroundColor = 0xEDEDEDFF;
	mBack->setSize(W * 5/6, H * 5/6);
	mBack->setPosition(W/6 / 2, H);
	mBack->setPath(image);

	//title
	mTitle = new Label();
	mTitle->setSize( mBack->getSize().x(), 0);
	mTitle->BackgroundColor = 0x2B729533;
	mTitle->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	mTitle->HorizontalTextAlign = TextAlign::Center;
	mTitle->setText("HELP SCREEN");
	mBack->addChild(mTitle);

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

HelpView::~HelpView()
{

}

bool HelpView::input(Model::Input input)
{
	if(input.Semantic == InputSemantic::SELECT && input.Value != SDL_RELEASED )
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
