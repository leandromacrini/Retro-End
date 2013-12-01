#include "ScrapeView.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ScrapeView::ScrapeView(vector<Game>& games) : BaseView(), mGames(games)
{

	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	//initilize data
	mGameValuesFound = new vector<pair<Model::Game, vector<string>>>();
	mFinalGameData   = new vector<pair<Model::Game, string>>();

	//dark background
	mSize = Eigen::Vector2f(W,H);
	mBackgroundColor = 0x1d1d1d99;

	//show loading dialog
	mDialog = new MessageDialog();
	mDialog->setMessage("Retring informations from TGDB ...");	
	addChild(mDialog);

	if(mGameValuesFound->size() == 0)
	{
		for(auto gamesIt = mGames.begin(); gamesIt != mGames.end(); ++gamesIt)
		{
			//TODO Make async
			vector<string> results = vector<string>();
			ScraperController::getInstance().SearchGameData(*gamesIt, results);

			mGameValuesFound->push_back(pair<Game, vector<string>>( *gamesIt,results ));
		}
	}
	
	removeChild(mDialog);

	showGamesChoices();
}

void ScrapeView::showGamesChoices()
{
	//here we show the choice to the user until we have cleaned the list
	if(mGameValuesFound->size() > mFinalResult.gamesScraped)
	{
		int gameIndex = mFinalResult.gamesScraped;
		Game* game = &(mGameValuesFound->at(gameIndex).first);
		vector<string> results = mGameValuesFound->at(gameIndex).second;

		Device device = Device::getDeviceById(game->DeviceId);

		//list choices
		vector<string>* choices = new vector<string>();

		//first item is Go to next game
		choices->push_back("None - Go to next game");

		for(auto it = results.begin(); it != results.end(); ++it)
		{
			pugi::xml_document doc;
			doc.load((*it).c_str());
			string title= doc.child("Game").child("GameTitle").text().get();
			choices->push_back(title);
		}

		if(choices->size() > 0)
		{
			MultiChoiceDialog* mcd = new MultiChoiceDialog();
			addChild(mcd);
			mcd->showDialog("Choose the right game for this file\n"+game->GameFile, device.Name , choices, [this, game, choices, mcd, gameIndex] (unsigned int index)
			{
				if(index == 0)
				{
					mFinalResult.gamesNotFound++;

					//we store an empyy game where the title will be the file name
					mFinalGameData->push_back(pair<Game, string>(*game, "" ));
				}
				else
				{
					//we store the game and the xml response of the selected index
					mFinalGameData->push_back(pair<Game, string>(*game, mGameValuesFound->at(gameIndex).second.at(index-1) ));

					mFinalResult.gamesFound++;
					
				}
				mFinalResult.gamesScraped++;
				removeChild(mcd);

				showGamesChoices();
			});
		}
		else
		{
			mFinalResult.gamesNotFound++;
			mFinalResult.gamesScraped++;

			//we store an empyy game where the title will be the file name
			mFinalGameData->push_back(pair<Game, string>(*game, "" ));

			//recursion
			showGamesChoices();
		}
	}
	else
	{
		//TODO Make async
		mDialog = new MessageDialog();
		mDialog->setMessage("Retring informations from TGDB ...");	
		addChild(mDialog);

		//here we download all the images for the scraped games and save them to DB
		ScraperController::getInstance().StoreGamedata(mFinalGameData);

		removeChild(mDialog);
		//finally 
		onScrapeComplete(mFinalResult);
	}
}

//TODO override update and run after scrape complete on the main thread!
//if need for making scraping call async