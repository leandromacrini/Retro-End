#include "GameInfoView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

GameInfoView::GameInfoView(Game game) : mGame(game)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	mSize = Eigen::Vector2f(W,H);
	mBackgroundColor = 0x1d1d1d77;

	mBack = new BaseView();
	mBack->setBackgroundColor(0xEDEDEDFF);
	mBack->setSize(W * 5/6, H * 5/6);
	mBack->setPosition(W/6 / 2, H);

	Animation* a = new Animation();
	a->moveOffset = new Eigen::Vector3f(0, -(H * 5/6)-(H/6 / 2) - H/6 / 4, 0);
	a->millisDuration = 250;
	a->endCallback = [H, this] () 
	{
		Animation* a = new Animation();
		a->moveOffset = new Eigen::Vector3f(0, H/6 / 4, 0);
		a->millisDuration = 100;
		mBack->animate(a);
	};
	mBack->animate(a);

	this->addChild(mBack);
}

GameInfoView::~GameInfoView()
{

}

bool GameInfoView::input(InputConfig* config, Model::Input input)
{
	if(input.id == SDLK_SPACE && input.value != 0 )
	{
		float H = (float) RenderController::getInstance().getScreenHeight();

		//EXIT ANIMATION
		Animation* a = new Animation();
		a->moveOffset = new Eigen::Vector3f(0, -H/6 / 4, 0);
		a->millisDuration = 100;
		a->endCallback = [this] () 
		{
			Animation* a = new Animation();
			a->moveOffset = new Eigen::Vector3f(0, mBack->getSize().y(), 0);
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
