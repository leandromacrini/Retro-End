#pragma once

#include <ctime>

#include "../../globals.h"

#include "BaseView.h"
#include "Sprite.h"
#include "Image.h"
#include "Label.h"
#include "GameListView.h"
#include "NetplayView.h"

#include "../Model/Observer.h"
#include "../Model/Sound.h"
#include "../Model/Device.h"
#include "../Model/Game.h"
#include "../Model/Font.h"

namespace RetroEnd
{
	namespace View
	{
		class GamesView : public BaseView
		{
		public:
			GamesView();
			~GamesView();

			void setDevice(Model::Device& device);

			void update(unsigned int deltaTime) override;

			//EVENTS

			Model::Observer<Model::Game> onOpenGameInfo;	//open game info event
			Model::Observer<int> onOpenGameHelpScreen;		//open game help screen
			Model::Observer<NetplayRequest> onOpenNetplayScreen;			//open Netplay screen

			bool input(Model::Input input) override;
		private:
			void startGame();
			void endGame();

			void updateCurrentGameData();
			void toggleGameFavorite();
			void updateImageSizeAndPosition();

			//toggle to avoid multi click on games
			bool mIsPlaying;

			shared_ptr<Model::Sound> mCartridgeSound;

			Model::Device mDevice;
			vector<Model::Game> mGames;

			GameListView* mGamesList;
			Image* mBackgroundImage;
			Image* mDeviceLogo;
			Image* mCartridge;

			BaseView* mGameInfoContainer;

			Image* mGameImage;
			Label* mDateManufacturer;
			Label* mDeveloper;

			Sprite* mRightLegend;

			bool gameEnded;
			string gameEndedError;
		};

	}

}