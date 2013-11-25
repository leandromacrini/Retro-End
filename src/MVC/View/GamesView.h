#pragma once

#include "../../globals.h"

#include "../Model/Observer.h"
#include "../Model/Sound.h"
#include "../Model/Device.h"
#include "../Model/Game.h"

#include "BaseView.h"
#include "Sprite.h"
#include "Image.h"
#include "Label.h"
#include "GameListView.h"

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

			void move(int direction); // 1 Next | -1 Prev

			Model::Observer<Model::Game> onOpenGameInfo; //open game info event

			bool input(Model::InputConfig* config, Model::Input input) override;
		private:
			void updateCurrentGameData();
			void GamesView::updateImageSizeAndPosition();

			shared_ptr<Model::Sound> mMoveSound;
			shared_ptr<Model::Sound> mSelectSound;

			Model::Device mDevice;
			vector<Model::Game> mGames;

			GameListView* mGamesList;
			Image* mBackgroundImage;
			Image* mDeviceLogo;
			
			Image* mGameImage;

			Label* mDateManufacturer;
			Label* mDeveloper;

			Sprite* mLeftLegend;
			Sprite* mRightLegend;
		};

	}

}