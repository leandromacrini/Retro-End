#include "GamesView.h"

#include "../Controller/RenderController.h"
#include "../Controller/AudioController.h"
#include "../Controller/InputController.h"
#include "../Controller/ScraperController.h"
#include "../Controller/GamingController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

GamesView::GamesView()
{
	//create sounds
	mMoveSound = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS25.wav"));
	mSelectSound  = shared_ptr<Sound>( AudioController::getInstance().createSound("data/sounds/GS42.wav"));

	//create graphics
	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	std::shared_ptr<Font> bigfont =  Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE);

	//instantiate graphics
	mBackgroundImage = new Image();
	mBackgroundImage->setSize( W, H );
	mBackgroundImage->setPath("data/images/bg_bn.png");
	mBackgroundImage->setTiled(true);
	this->addChild( mBackgroundImage );

	Image* shadow = new Image();
	shadow->setSize( 94, H * 5/6 );
	shadow->setPosition( W/2, H/6 );
	shadow->setPath("data/images/shadow.png");
	this->addChild( shadow );

	Image* gameShadow = new Image();
	gameShadow->setSize( W/2, H );
	gameShadow->setPosition( 0, 0 );
	gameShadow->setPath("data/images/radial.png");
	this->addChild( gameShadow );

	mDeviceLogo = new Image( );
	mDeviceLogo->setPosition( W/2, 0 );
	mDeviceLogo->setSize( W/2, H/6 );
	this->addChild( mDeviceLogo );

	mGamesList = new GameListView();
	mGamesList->setPosition(W/2, H/6);
	mGamesList->RowHeight = 64;
	mGamesList->setSize(W/2, ((-1.0f + (int)(H * 5/6 / mGamesList->RowHeight)) * mGamesList->RowHeight));
	mGamesList->HorizontalTextAlign = TextAlign::Left;
	mGamesList->TitleColor = 0x1D1D1D66;
	mGamesList->SelectedTitleColor = 0x00000000FF;
	mGamesList->SelectedRowBackgroundColor = 0x2B729533;
	this->addChild( mGamesList );

	mGameImage = new Image();
	mGameImage->setPosition(H/4, H/6);
	this->addChild(mGameImage);

	//selected game date and manufacturer
	mDateManufacturer = new Label();
	mDateManufacturer->setColor(0x000000FF);
	mDateManufacturer->setSize(W/2, 0);
	mDateManufacturer->setFont(bigfont);
	mDateManufacturer->setPosition(0, H/6/2 - bigfont->getSize()/2);
	mDateManufacturer->HorizontalTextAlign = TextAlign::Center;
	this->addChild(mDateManufacturer);

	//selected game developer
	mDeveloper = new Label();
	mDeveloper->setColor(0x000000FF);
	mDeveloper->setSize(W/2, 0);
	mDeveloper->setPosition(0, H*5/6 + H/6/2 - bigfont->getSize()/2);
	mDeveloper->setFont(bigfont);
	mDeveloper->HorizontalTextAlign = TextAlign::Center;
	this->addChild(mDeveloper);

	Sprite* help = new Sprite();
	help->setPosition(W/4, H* 19/20);
	help->setSize(W/2, H/20);
	help->FrameHeight = 54;
	help->setPath("data/images/left-legend.png");
	help->setLoop(true);
	help->setFrameDuration( 1000 );
	help->start();
	this->addChild(help);
}

void GamesView::setDevice(Model::Device& device)
{
	mDevice = device;

	//set logo
	mDeviceLogo->setPath("data/consoles/" + device.Name + "/logo.png");

	//set games list
	mGamesList->removeAllRows();

	mGamesList->setPointerPath("data/consoles/" + device.Name + "/pointer.png");

	mGames = Game::getGamesForDevice(device.id);

	if(mGames.size() == 0)
	{
		ScraperController::getInstance().ScrapeAllGamesForDevice(device, true, 20);

		mGames = Game::getGamesForDevice(device.id);
	}

	for (auto it(mGames.begin()); it != mGames.end(); ++it)
	{
		mGamesList->addRow(*it);
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
			float maxImageSide = H/3*2;

			mGameImage->resizeByWidth(maxImageSide);
		}
		else
		{
			float maxImageSide = H/3*2;

			mGameImage->resizeByHeight(maxImageSide);
		}
		mGameImage->setPosition((W/2 - mGameImage->getSize().x())/2, (H - mGameImage->getSize().y())/2);
	}
}

void GamesView::updateCurrentGameData()
{
	Game game = mGames[mGamesList->getSelectedIndex()];
	
	//date and data text

	mDeveloper->setText(game.Developer);
	mDateManufacturer->setText(game.getReleaseYear() + game.Publisher);

	//image size is computed on the longest side
	//image must always fit inside his box
	mGameImage->setPath("");
	if( ! game.ImageFront.empty() )
	{
		mGameImage->setPath( game.ImageFront );
	}
	else if ( ! game.ImageBack.empty() )
	{
		mGameImage->setPath( game.ImageBack );
	}
	else if ( ! game.ImageScreen.empty() )
	{
		mGameImage->setPath( game.ImageScreen );
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

	if(input.id == SDLK_LEFT && input.value != 0 )
	{
		move(- min(10,(int) mGamesList->getSelectedIndex()));
		return true;
	}

	if(input.id == SDLK_RIGHT && input.value != 0)
	{
		move(min(10u, mGames.size() -1 - mGamesList->getSelectedIndex()));
		return true;
	}

	if(input.id == SDLK_ESCAPE && input.value != 0)
	{
		mParent->removeChild(this);
		return true;
	}

	if(input.id == SDLK_SPACE && input.value != 0)
	{
		onOpenGameInfo(mGames.at(mGamesList->getSelectedIndex()));
		return true;
	}

	if(input.id == SDLK_RETURN && input.value != 0)
	{
		mSelectSound->play();
		GamingController::getInstance().launchGame(mDevice, mGames.at(mGamesList->getSelectedIndex()));
		return true;
	}

	return true;
}
