#include "GameInfoView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

GameInfoView::GameInfoView(Game& game) : mGame(game)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	//shadow
	mSize = Eigen::Vector2f(W,H);
	mBackgroundColor = 0x1d1d1d77;

	//container
	mBack = new BaseView();
	mBack->setBackgroundColor(0xEDEDEDFF);
	mBack->setSize(W * 5/6, H * 5/6);
	mBack->setPosition(W/6 / 2, H);

	//title
	mTitle = new Label();
	mTitle->setSize( mBack->getSize().x(), 0);
	mTitle->setBackgroundColor(0x2B729533);
	mTitle->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	mTitle->HorizontalTextAlign = TextAlign::Center;
	mTitle->setText(mGame.Title);
	mBack->addChild(mTitle);

	//description
	mDescription = new Label();
	mDescription->WrapText = true;
	mDescription->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_SMALL));
	mDescription->setSize(mBack->getSize().x()/2, mBack->getSize().y() - H/4 - mTitle->getSize().y());
	mDescription->setText(game.Description);
	mDescription->setPosition(mBack->getSize().x()/2, mTitle->getSize().y());
	mBack->addChild(mDescription);

	//back cover image (if any or front or empty) 
	string image = "data/images/blank_game.png";
	if( ! mGame.ImageBack.empty() ) image = mGame.ImageBack;
	else if ( ! mGame.ImageFront.empty() ) image = mGame.ImageFront;
	else if ( ! mGame.ImageScreen.empty() ) image = mGame.ImageScreen;

	mGameBack = new Image();
	mGameBack->setPath(image);
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

	//ESRB and rating
	BaseView* rateBack = new BaseView();
	rateBack->setBackgroundColor(0x2B729533);
	rateBack->setSize(W * 5/6/2, H/4);
	rateBack->setPosition(W * 5/6/2, mBack->getSize().y() - H/4);
	mBack->addChild(rateBack);

	mESRB = new Image();
	string path = "data/images/";
	if(game.ESRB.empty())
		path += "icon_ESRB_RP.png";
	else if(game.ESRB == "E - Everyone")
		path +="icon_ESRB_E.png";
	else if(game.ESRB == "T - Teen")
		path +="icon_ESRB_T.png";
	else if(game.ESRB == "E10+ - Everyone 10+")
		path +="icon_ESRB_E10+.png";
	else if(game.ESRB == "M - Mature")
		path +="icon_ESRB_M.png";
	else if(game.ESRB == "EC - Early Childhood")
		path +="icon_ESRB_eC.png";
	else if(game.ESRB == "Ao - Adults Only 18+")
		path +="icon_ESRB_Ao.png";
	else
		path +="icon_ESRB_RP.png";

	mESRB->setPath(path);
	mESRB->resizeByHeight(H/4);
	rateBack->addChild(mESRB);

	Label* rating = new Label();
	rating->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	rating->setSize(W * 5/6/2 - mESRB->getSize().x() ,mESRB->getSize().y());
	rating->setPosition(mESRB->getSize().x(),0);
	rating->WrapText = true;
	rating->HorizontalTextAlign = TextAlign::Center;
	string rateText = "TGDB RATING\n";
	if( ! game.TGDBRating.empty())
		rateText += game.TGDBRating + "/10";
	else
		rateText += "Not rated yet";
	rating->setText(rateText);
	rateBack->addChild(rating);

	//animation
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
	if( (input.id == SDLK_SPACE && input.value != 0 ) || (input.id == 0 && input.type == TYPE_BUTTON && input.value == SDL_RELEASED))
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
