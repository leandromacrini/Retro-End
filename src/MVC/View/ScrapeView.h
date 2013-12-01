/* Scrape View Class
 *
 * A View that search for games informations and let the user choice witch game scrape
 */

#pragma once

#include <thread>

#include "../../LIB/pugiXML/pugixml.hpp"
#include "../../globals.h"

#include "BaseView.h"
#include "MessageDialog.h"
#include "MultiChoiceDialog.h"

#include "../Model/Game.h"
#include "../Model/Device.h"
#include "../Model/Observer.h"

#include "../Controller/LogController.h"
#include "../Controller/RenderController.h"
#include "../Controller/ScraperController.h"

using namespace std;

namespace RetroEnd
{
	namespace View
	{
		class ScrapeResult
		{
		public:
			ScrapeResult()
			{
				completed = true;
				error = "";
				gamesScraped = 0;
				gamesFound = 0;
				gamesNotFound = 0;
			}
			bool completed;
			string error;
			unsigned int gamesScraped;
			unsigned int gamesFound;
			unsigned int gamesNotFound;
		};

		class ScrapeView : public BaseView
		{
		public:
			ScrapeView(vector<Model::Game>& games);
			
			Model::Observer<ScrapeResult> onScrapeComplete;

		private:
			vector<Model::Game>& mGames;
			vector<pair<Model::Game, vector<string>>>* mGameValuesFound;
			vector<pair<Model::Game, string>>* mFinalGameData;
			ScrapeResult mFinalResult;
			MessageDialog* mDialog;

			void showGamesChoices();
		};
	}
}