#pragma once

#include "../../globals.h"

#include "../Model/Sound.h"
#include "../Model/Device.h"
#include "../Model/Game.h"

#include "BaseView.h"
#include "Sprite.h"
#include "Image.h"
#include "Label.h"
#include "ListView.h"

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

			bool input(Model::InputConfig* config, Model::Input input) override;
		private:
			void updateCurrentGameData();
			void GamesView::updateImageSizeAndPosition();

			shared_ptr<Model::Sound> mMoveSound;
			shared_ptr<Model::Sound> mSelectSound;

			Model::Device mDevice;
			vector<Model::Game> mGames;

			ListView* mGamesList;
			Image* mBackgroundImage;
			Image* mDeviceLogo;
			
			Image* mGameImage;
			Image* mGameESRB;
			Image* mGameRate;
			Image* mGamePlayers;
			Image* mGameCoOp;
			
			Label* mDateManufacturer;

			Sprite* mLeftLegend;
			Sprite* mRightLegend;
		};

	}

}