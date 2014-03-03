#pragma once

#include <ctime>

#include "../../globals.h"

#include "BaseView.h"
#include "Sprite.h"
#include "Image.h"
#include "Label.h"
#include "GameListView.h"

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

			bool input(Model::Input input) override;
		private:
			void startGame();
			void endGame();
			void move(int direction); // 1 Next | -1 Prev

			int mMoving; // 1 Next | 0 Stop | -1 Prev
			clock_t mLastCheck;
			void startMoving(int direction); // 1 Next | -1 Prev
			void stopMoving();

			void updateCurrentGameData();
			void toggleGameFavorite();
			void updateImageSizeAndPosition();

			//toggle to avoid multi click on games
			bool mIsPlaying;

			shared_ptr<Model::Sound> mMoveSound;
			shared_ptr<Model::Sound> mSelectSound;
			shared_ptr<Model::Sound> mCartridgeSound;

			Model::Device mDevice;
			vector<Model::Game> mGames;

			GameListView* mGamesList;
			Image* mBackgroundImage;
			Image* mDeviceLogo;
			Image* mCartridge;
			Image* mGameImage;

			Label* mDateManufacturer;
			Label* mDeveloper;

			Sprite* mLeftLegend;
			Sprite* mRightLegend;

			bool gameEnded;
			string gameEndedError;
		};

	}

}