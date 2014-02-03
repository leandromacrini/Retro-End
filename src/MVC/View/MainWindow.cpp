#include "MainWindow.h"

#include "ConsoleView.h"
#include "GameInfoView.h"
#include "GamesView.h"
#include "Label.h"
#include "MultiChoiceDialog.h"
#include "ScrapeView.h"
#include "Sprite.h"

#include "../Controller/AudioController.h"
#include "../Controller/GamingController.h"
#include "../Controller/InputController.h"
#include "../Controller/RenderController.h"
#include "../Controller/ScraperController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

MainWindow::MainWindow() : mScrapeComplete(false), mScrapeView(NULL)
{
	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	this->setSize( W, H );

	Image* background = new Image();
	background->setSize( W, H );
	background->setPath("data/images/bg_bn.png");
	background->setTiled(true);
	addChild(background);

	showLogo();
}

MainWindow::~MainWindow()
{

}

void MainWindow::checkForNewGames()
{
	//TODO ADD A LOADING DIALOG
	vector<Game>* newGames = new vector<Game>();
	GamingController::getInstance().checkNewGamesAvailability(*newGames);
	if(newGames->size() > 0)
	{
		MultiChoiceDialog* dialog = new MultiChoiceDialog();

		vector<string>* values = new vector<string>();
		values->push_back("Yes");
		values->push_back("No");

		dialog->showDialog(to_string(newGames->size()) + " new games was found.\n Do you want to try to automatically get Game informations from The Games DB?", "Attention", values,
		[this, newGames, dialog] (unsigned int index)
		{
			if(index == 0) //yes
			{
				try
				{
					mScrapeView = new ScrapeView(*newGames);
					((ScrapeView*)mScrapeView)->onScrapeComplete += [this] (ScrapeResult result)
					{
						//the scraper event work on a different thread so we can't remove it from here
						//we have to mark it to delete and than let the main thread remove it when need
						mScrapeComplete = true;
						ScrapeResult* pointerResult = new ScrapeResult(result);
						mScrapeResult = (void*) pointerResult;
					};
					addChild(mScrapeView);
				}
				catch (const char * ex)
				{
					RenderController::getInstance().pushPopupMessage(string(ex), PopupMessageIcon::Error);
				}
			}
			removeChild(dialog);
		});
		addChild(dialog);
	}
}

void MainWindow::checkForOldGames()
{
	//TODO ADD A LOADING DIALOG
	vector<Game> *oldGames = new vector<Game>();
	GamingController::getInstance().checkOldGamesAvailability(*oldGames);
	if(oldGames->size() > 0)
	{
		MultiChoiceDialog* dialog = new MultiChoiceDialog();

		vector<string>* values = new vector<string>();
		values->push_back("Yes");
		values->push_back("No");

		dialog->showDialog(to_string(oldGames->size()) + " games as been removed from the roms folder. Would you like to remove those games from Manta's database?", "Attention", values,
		[this, oldGames, dialog] (unsigned int index)
		{
			for(auto it = oldGames->begin(); it != oldGames->end(); ++it)
			{
				//yes?
				if(index == 0)		
					it->remove();	//remove the file
				else
				{
					it->GameFile = "";
					it->save();		//clear game file so it will not be rechecked
				}
			}
			removeChild(dialog);

			//we have ended with old games, check the new games
			checkForNewGames();
		});
		addChild(dialog);
	}
	else
	{
		//we have ended with old games, check the new games
		checkForNewGames();
	}
}

bool MainWindow::input(InputConfig* config, Input input)
{
	if(input.id == SDLK_p && input.value != 0 )
	{
		RenderController::getInstance().pushPopupMessage("Miao testo lungo lungo lungo lungo lungo lungo lungo lungo lungo!", PopupMessageIcon::Info);
		return true;
	}

	//set input only to the last view added
	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(config, input);
	}
	return true;
}

void MainWindow::showLogo()
{
	mConsoleView = new ConsoleView(); //pre load images

	((ConsoleView*)mConsoleView)->onOpenGamesList += [this] (Device device)
	{
		GamesView* games = new GamesView();
		games->setSize(getSize());
		games->setDevice(device);

		games->onOpenGameInfo += [this] (Game game)
		{
			GameInfoView* info = new GameInfoView(game);
			addChild(info);
		};

		addChild(games);
	};

	Image* logo = new Image();
	logo->setSize((float)RenderController::getInstance().getScreenWidth()/2, (float)RenderController::getInstance().getScreenHeight() / 2);
	logo->setPosition((float)RenderController::getInstance().getScreenWidth()/4, (float)RenderController::getInstance().getScreenHeight() / 4);
	logo->setPath("data/logo black.png");
	logo->setOpacity(0);
	addChild(logo);

	Animation* a = new Animation();

	a->millisDuration = 2000;
	a->endCallback = [this, logo] ()
	{
		shared_ptr<Sound> sound = AudioController::getInstance().createSound("data/sounds/splash.wav");
		sound->play();

		Animation* a = new Animation();

		a->millisDuration = 2000;
		a->newOpacity = new unsigned char(255);
		a->endCallback = [this, logo] ()
		{
			Animation* a = new Animation();
			a->millisDuration = 1000;
			a->newSize = new Eigen::Vector2f((float)RenderController::getInstance().getScreenWidth()/5, (float)RenderController::getInstance().getScreenHeight() / 5);
			a->moveOffset = new Eigen::Vector3f((float)RenderController::getInstance().getScreenWidth()/2, (float)-RenderController::getInstance().getScreenHeight() / 4, 0);
			a->endCallback =  [this] ()
			{
				//Show Consoles
				mConsoleView->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
				mConsoleView->setPosition(0,0);
				addChild(mConsoleView);

				//async check if old games are still available
				checkForOldGames();
			};

			logo->animate(a);
		};

		logo->animate(a);
	};

	logo->animate(a);
}

void MainWindow::update(unsigned int deltaTime)
{
	//we need to do this here because the scraper work on a different thread
	if(mScrapeComplete)
	{
		removeChild(mScrapeView);

		//show result
		if(((ScrapeResult*)mScrapeResult)->gamesFound > 0)
			RenderController::getInstance().pushPopupMessage( to_string( ((ScrapeResult*)mScrapeResult)->gamesFound ) + " games was founded on TGDB", PopupMessageIcon::Info);

		if(((ScrapeResult*)mScrapeResult)->gamesNotFound > 0)
			RenderController::getInstance().pushPopupMessage( to_string( ((ScrapeResult*)mScrapeResult)->gamesNotFound ) + " games was non founded on TGDB and added as filename", PopupMessageIcon::Warning);
		mScrapeResult = NULL;
		mScrapeView = NULL;
		mScrapeComplete = false;
	}

	BaseView::update(deltaTime);
}
