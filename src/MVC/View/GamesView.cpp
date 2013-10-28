#include "GamesView.h"

#include "../Controller/RenderController.h"
#include "../Controller/AudioController.h"
#include "../Controller/InputController.h"
#include "../Controller/ScraperController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

GamesView::GamesView()
{
	std::shared_ptr<Font> font =  Font::get("data/fonts/pixeljosh6.ttf", FONT_SIZE_SMALL);
	std::shared_ptr<Font> bigfont =  Font::get("data/fonts/pixeljosh6.ttf", FONT_SIZE_MEDIUM);

	mMoveSound = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS25.wav"));
	mSelectSound  = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS42.wav"));

	//create graphics
	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	//instantiate graphics
	mBackgroundImage = new Image();
	mBackgroundImage->setSize( W, H );
	mBackgroundImage->setPath("data/images/sfondo1.jpg");
	this->addChild( mBackgroundImage );

	mDeviceLogo = new Image( );
	mDeviceLogo->setPosition( W/2, 0 );
	mDeviceLogo->setSize( W/2, H/3 );
	this->addChild( mDeviceLogo );

	mGamesList = new ListView();
	mGamesList->setPosition(W/2, H/3);
	mGamesList->setSize(W/2, 2* H/3);
	mGamesList->SelectedRowBackgroundColor = 0xFFFFFFFF;
	mGamesList->HorizontalTextAlign = TextAlign::Left;
	mGamesList->ItemFont = font;
	this->addChild( mGamesList );

	mGameImage = new Image();
	mGameImage->setPosition(H/4, H/6);
	this->addChild(mGameImage);

	//selected game date and manufacturer
	mDateManufacturer = new Label();
	mDateManufacturer->setColor(0xFFFFFFFF);
	mDateManufacturer->setSize(W/2,  (float)FONT_SIZE_LARGE);
	mDateManufacturer->setPosition(0, H - FONT_SIZE_LARGE);
	mDateManufacturer->setFont(font);
	mDateManufacturer->ShadowVisible = true;
	mDateManufacturer->HorizontalTextAlign = TextAlign::Center;
	this->addChild(mDateManufacturer);

	//game icons
	mGameESRB = new Image();
	mGameESRB->setPath("data/images/icon_ESRB_Ao.png");
	mGameESRB->setPosition(W/2/5*1, (H/6 - H/7) / 2);
	mGameESRB->resizeByHeight(H/7);
	this->addChild(mGameESRB);

	mGamePlayers = new Image();
	mGamePlayers->setPath("data/images/icon_player_4.png");
	mGamePlayers->setPosition(W/2/5*2, (H/6 - H/7) / 2);
	mGamePlayers->resizeByHeight(H/7);
	this->addChild(mGamePlayers);

	mGameCoOp = new Image();
	mGameCoOp->setPath("data/images/icon_coop.png");
	mGameCoOp->setPosition(W/2/5*3, (H/6 - H/7) / 2);
	mGameCoOp->resizeByHeight(H/7);
	this->addChild(mGameCoOp);

	mGameRate = new Image();
}

void GamesView::setDevice(Model::Device& device)
{
	mDevice = device;

	//set logo
	mDeviceLogo->setPath(device.imageLogo);

	//set games list
	mGamesList->removeAllRows();

	mGames = Game::getGamesForDevice((int)device.id);

	if(mGames.size() == 0)
	{
		ScraperController::getInstance().ScrapeAllGamesForDevice(device, true, 20);

		//ScraperController::getInstance().ScrapeAllGamesForDevice(device);
		mGames = Game::getGamesForDevice((int)device.id);
	}

	for (auto it(mGames.begin()); it != mGames.end(); ++it)
	{
		mGamesList->addRow(it->title);
	}

	updateCurrentGameData();

}

GamesView::~GamesView()
{

}

void GamesView::updateImageSizeAndPosition()
{
	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	if( ! mGameImage->getPath().empty() )
	{
		//resize on width or height
		if(mGameImage->getTextureSize().x() > mGameImage->getTextureSize().y())
		{
			float imageMargin  = H/6;
			float maxImageSide = W/2 - imageMargin*2;

			mGameImage->resizeByWidth(maxImageSide);
			mGameImage->setPosition(imageMargin, imageMargin);
		}
		else
		{
			float imageMargin  = H/6;
			float maxImageSide = H - imageMargin*2;

			mGameImage->resizeByHeight(maxImageSide);
			mGameImage->setPosition((W/2 - mGameImage->getSize().x())/2, imageMargin);
		}
	}
}

void GamesView::updateCurrentGameData()
{
	Game game = mGames[mGamesList->getSelectedIndex()];
	
	//date and data text
	string text = game.releaseDate;

	if( ! game.developer.empty() )
		text += (text.empty()? "" : ", ") + game.developer;

	if( ! game.publisher.empty() )
		text += (text.empty()? "" : " (") + game.publisher + (text.empty()? "" : ")");

	mDateManufacturer->setText(text);

	//icons
	mGameCoOp->Visible = game.coOp == "Yes";

	if(game.ESRB == "E - Everyone")
	{
		mGameESRB->Visible = true;
		mGameESRB->setPath("data/images/icon_ESRB_E.png");
	}
	else if (game.ESRB == "E10+ - Everyone 10+")
	{
		mGameESRB->Visible = true;
		mGameESRB->setPath("data/images/icon_ESRB_E10+.png");
	}
	else if (game.ESRB == "T - Teen")
	{
		mGameESRB->Visible = true;
		mGameESRB->setPath("data/images/icon_ESRB_T.png");
	}
	else if (game.ESRB == "M - Mature")
	{
		mGameESRB->Visible = true;
		mGameESRB->setPath("data/images/icon_ESRB_M.png");
	}
	else if (game.ESRB == "EC - Early Childhood")
	{
		mGameESRB->Visible = true;
		mGameESRB->setPath("data/images/icon_ESRB_eC.png");
	}
	else if (game.ESRB == "AO - Adults Only") //TODO 
	{
		mGameESRB->Visible = true;
		mGameESRB->setPath("data/images/icon_ESRB_Ao.png");
	}
	else if (game.ESRB == "RP - Rating Pending")
	{
		mGameESRB->Visible = true;
		mGameESRB->setPath("data/images/icon_ESRB_RP.png");
	}
	else
	{
		mGameESRB->Visible = false;
		mGameESRB->setPath("");
	}

	if(game.players == "1")
	{
		mGamePlayers->Visible = true;
		mGamePlayers->setPath("data/images/icon_player_1.png");
	}
	else if(game.players == "2")
	{
		mGamePlayers->Visible = true;
		mGamePlayers->setPath("data/images/icon_player_2.png");
	}
	else if(game.players == "3")
	{
		mGamePlayers->Visible = true;
		mGamePlayers->setPath("data/images/icon_player_3.png");
	}
	else if(game.players == "4+")
	{
		mGamePlayers->Visible = true;
		mGamePlayers->setPath("data/images/icon_player_4.png");
	}
	else
	{
		mGamePlayers->Visible = false;
		mGamePlayers->setPath("");
	}

	//image size is computed on the longest side
	//image must always fit inside his box
	mGameImage->setPath("");
	if( ! game.imageFront.empty() )
	{
		mGameImage->setPath( game.imageFront );
	}
	else if ( ! game.imageBack.empty() )
	{
		mGameImage->setPath( game.imageBack );
	}
	else if ( ! game.imageScreen.empty() )
	{
		mGameImage->setPath( game.imageScreen );
	}
	updateImageSizeAndPosition();

}

void GamesView::move(int direction)
{

	mGamesList->setSelectedIndex( mGamesList->getSelectedIndex() + direction );

	float H = (float)RenderController::getInstance().getScreenHeight();

	updateCurrentGameData();
	
	mMoveSound->play();
}

bool GamesView::input(Model::InputConfig* config, Model::Input input)
{
	//TODO input from settings
	if(input.id == SDLK_DOWN && input.value != 0 )
	{
		move(1);
		return true;
	}

	if(input.id == SDLK_UP && input.value != 0)
	{
		move(-1);
		return true;
	}

	if(input.id == SDLK_ESCAPE && input.value != 0)
	{
		mParent->removeChild(this);
		return true;
	}

	return true;
}
