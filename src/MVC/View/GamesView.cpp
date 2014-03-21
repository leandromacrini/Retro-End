#include "GamesView.h"

#include <ctime>

#include "../Controller/RenderController.h"
#include "../Controller/AudioController.h"
#include "../Controller/InputController.h"
#include "../Controller/ScraperController.h"
#include "../Controller/GamingController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

GamesView::GamesView() : mIsPlaying(false), gameEnded(false)
{
	//create sounds
	mCartridgeSound = AudioController::getInstance().createSound("data/sounds/cartridge_insert.wav");

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
	addChild( gameShadow );

	mDeviceLogo = new Image( );
	mDeviceLogo->setPosition( W/2, 0 );
	mDeviceLogo->setSize( W/2, H/6 );
	addChild( mDeviceLogo );

	mGamesList = new GameListView();
	mGamesList->setPosition(W/2, H/6);
	mGamesList->RowHeight = 64;
	mGamesList->setSize(W/2, ((-1.0f + (int)(H * 5/6 / mGamesList->RowHeight)) * mGamesList->RowHeight));
	mGamesList->HorizontalTextAlign = TextAlign::Left;
	mGamesList->TitleColor = 0x1D1D1D66;
	mGamesList->SelectedTitleColor = 0x00000000FF;
	mGamesList->SelectedRowBackgroundColor = 0x2B729533;
	mGamesList->Focused = true;
	mGamesList->onItemPressed += [this](Game game)
	{
		if( ! mIsPlaying)
		{
			startGame();
		}
	};
	mGamesList->onFastMoved += [this](int index)
	{
		cout<<"fast moved"<<endl;
		Animation* a = new Animation();
		a->millisDuration = 150;
		a->newOpacity = 0;
		mGameInfoContainer->animate(a);
	};
	mGamesList->onSelectedItemChanged += [this](int index)
	{
		mGameInfoContainer->animate(new Animation()); //abort fade animation
		cout<<"item changed"<<endl;
		updateCurrentGameData();
	};
	addChild( mGamesList );

	mGameInfoContainer = new BaseView();
	mGameInfoContainer->setSize(W/2, H);
	mGameInfoContainer->setPosition(0, 0);
	addChild(mGameInfoContainer);

	mGameImage = new Image();
	mGameImage->setPosition(H/4, H/6);
	mGameInfoContainer->addChild(mGameImage);

	//selected game date and manufacturer
	mDateManufacturer = new Label();
	mDateManufacturer->setColor(0x000000FF);
	mDateManufacturer->setSize(W/2, 0);
	mDateManufacturer->setFont(bigfont);
	mDateManufacturer->setPosition(0, H/6/2 - bigfont->getSize()/2);
	mDateManufacturer->HorizontalTextAlign = TextAlign::Center;
	mGameInfoContainer->addChild(mDateManufacturer);

	//selected game developer
	mDeveloper = new Label();
	mDeveloper->setColor(0x000000FF);
	mDeveloper->setSize(W/2, 0);
	mDeveloper->setPosition(0, H*5/6 + H/6/2 - bigfont->getSize()/2);
	mDeveloper->setFont(bigfont);
	mDeveloper->HorizontalTextAlign = TextAlign::Center;
	mGameInfoContainer->addChild(mDeveloper);

	Sprite* help = new Sprite();
	help->setPosition(W/2, H* 19/20);
	help->setSize(W/2, H/20);
	help->FrameHeight = 54;
	help->setPath("data/images/left-legend.png");
	help->setLoop(true);
	help->setFrameDuration( 1000 );
	help->start();
	addChild(help);

	//FOR LAST
	mCartridge = new Image();
	mCartridge->setPath("data/images/manta_cartridge.png");
	mCartridge->setPosition( (W - mCartridge->getSize().x()) / 2, H);
	addChild(mCartridge);

	//subscribe to game end event to make the rollback animation for the cartridge
	GamingController::getInstance().onGameEnd += [&](Game& game)
	{
		gameEnded = true;
	};

	//subscribe to game end event to make the rollback animation for the cartridge
	GamingController::getInstance().onGameError += [&](const string& error)
	{
		gameEnded = true;
		gameEndedError = error;
	};
}

void GamesView::endGame()
{
	float Y = mCartridge->getSize().y();

	//cartridge animation
	Animation* a = new Animation();
	a->millisDelay = 1500;
	a->millisDuration = 150;
	a->moveOffset = Eigen::Vector3f(0, -Y/2, 0);
	a->endCallback = [this, Y] ()
	{
		//play cartridge sound
		mCartridgeSound->play();
		Animation* a = new Animation();
		a->millisDuration = 350;
		a->moveOffset = Eigen::Vector3f(0, Y, 0);
		a->endCallback = [this] ()
		{
			//toggle game block
			mIsPlaying = false;
		};

		mCartridge->animate(a);
	};
	mCartridge->animate(a);
}

void GamesView::startGame()
{
	//toggle game block
	mIsPlaying = true;

	//cartridge animation
	Animation* a = new Animation();
	a->millisDuration = 350;
	a->moveOffset = Eigen::Vector3f(0, -mCartridge->getSize().y(), 0);
	a->endCallback = [this] ()
	{
		//play cartridge sound
		mCartridgeSound->play();

		//put down the cartridge
		Animation* a = new Animation();
		a->millisDuration = 150;
		a->moveOffset = Eigen::Vector3f(0, mCartridge->getSize().y()/2, 0);
		a->endCallback = [this] ()
		{
			//wait for sound to end and start the game
			Animation* a = new Animation();
			a->millisDuration = 500;
			a->endCallback = [this] ()
			{
				//launch game
				GamingController::getInstance().launchSingleGame(mDevice, mGames.at(mGamesList->getSelectedIndex()));
			};
		mCartridge->animate(a);
		};
		mCartridge->animate(a);
	};
	mCartridge->animate(a);
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
			float maxImageSide = H * 2/3;

			mGameImage->resizeByWidth(maxImageSide);
		}
		else
		{
			float maxImageSide = H * 2/3;

			mGameImage->resizeByHeight(maxImageSide);
		}
		mGameImage->setPosition((W/2 - mGameImage->getSize().x())/2, (H - mGameImage->getSize().y())/2);
	}
}

void GamesView::toggleGameFavorite()
{
	unsigned int index = mGamesList->getSelectedIndex();
	//update game value
	Game game = mGames[index];
	game.Favorite = ! game.Favorite;
	game.save();

	//reload list
	setDevice(mDevice);
	mGamesList->setSelectedIndex(index);
	updateCurrentGameData();
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
	mGameInfoContainer->setOpacity(255);

}

bool GamesView::input(Model::Input input)
{
	if(input.Semantic == InputSemantic::BUTTON_Y && input.Value != SDL_RELEASED )
	{
		toggleGameFavorite();
		return true;
	}

	if(input.Semantic == InputSemantic::START && input.Value != SDL_RELEASED )
	{
		onOpenNetplayScreen(NetplayRequest(mDevice, mGames.at(mGamesList->getSelectedIndex())));
		return true;
	}

	if(input.Semantic == InputSemantic::BUTTON_B && input.Value != SDL_RELEASED )
	{
		mParent->removeChild(this);
		return true;
	}

	if(input.Semantic == InputSemantic::BUTTON_X && input.Value != SDL_RELEASED )
	{
		onOpenGameInfo(mGames.at(mGamesList->getSelectedIndex()));
		return true;
	}

	if(input.Semantic == InputSemantic::SELECT && input.Value != SDL_RELEASED )
	{
		onOpenGameHelpScreen(0);
		return true;
	}

	return BaseView::input(input);
}

void GamesView::update(unsigned int deltaTime)
{
	//local state to avoid lambda problem
	if(gameEnded)
	{
		endGame();
		if( ! gameEndedError.empty() )
			RenderController::getInstance().pushPopupMessage(gameEndedError, PopupMessageIcon::Error);

		gameEnded = false;
		gameEndedError.clear();
	}

	//call father method
	BaseView::update(deltaTime);
}
